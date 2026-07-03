/*
 * MIT License
 *
 * Copyright (c) 2023-2026 Underview
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#include <linux/fs.h>

#include <libfdisk/libfdisk.h>
#include <udo/udo.h>

#include "device.h"

#define IS_GPT            0x4B
#define PARTLABEL_MAX     72
#define BLK_NAME_MAX      (1<<5)
#define TABLE_TYPE_MAX    (1<<3)
#define PARTITIONS_MAX    (1<<7)
#define FSTYPE_MAX        (1<<8)
#define TYPE_CODE_STR_MAX (1<<6)
#define PART_NAME_MAX     (BLK_NAME_MAX+3)

/*
 * @brief Structure defining a single partition entry.
 *        For a caller given partitioned file.
 *
 * @member number        - Partition number.
 * @member start_sector  - The starting sector of a partition.
 * @member end_sector    - The final sector of a partition.
 * @member sector_size   - Amount of sectors a partition has.
 * @member logical       - Boolean indicating if partition
 *                         logical or not. Used for MBR.
 *                         Always false in the GPT case.
 * @member extended      - Boolean indicating if partition
 *                         extended or not. Used for MBR.
 *                         Always false in the GPT case.
 * @member fstype        - File system type of a partition.
 * @member partlabel     - GPT partition label of a partition.
 * @member fslabel       - File system label of a partition.
 * @member type          - Stores the partition type.
 *      @member code     - Stores partition type
 *                         (Linux, Linux extended, etc..)
 *                         in integer representation.
 *                         Used MBR based partition table types.
 *      @member code_str - Store partition type in string format.
 *                         GUID partition table type in string format.
 *                         Used GPT based partition table types.
 *                         https://wiki.archlinux.org/title/GPT_fdisk#Partition_type
 */
struct uprov_device_part
{
	size_t   number;
	uint64_t start_sector;
	uint64_t end_sector;
	uint64_t sector_size;
	uint8_t  logical;
	uint8_t  extended;
	char     fstype[FSTYPE_MAX];
	char     fslabel[FSLABEL_MAX];
	char     partlabel[PARTLABEL_MAX];
	union _type_code {
		uint32_t code;
		char     code_str[TYPE_CODE_STR_MAX];
	} type;
};


/*
 * @brief Structure storing everything required to partition a file.
 *
 * @member err         - Stores information about the error that occured
 *                       for the given context and may later be retrieved
 *                       by caller.
 * @member free        - If structure allocated with calloc(3) member will be
 *                       set to true so that, we know to call free(3) when
 *                       destroying the context.
 * @member parts       - Array of partitions for the given @fname.
 * @member fname       - Block device name in string format.
 * @member fname_fd    - @fname open file descriptor.
 * @member ptable_type - Partition table type contained in @fname.
 * @member sector_sz   - Byte size of each sector in @fname.
 *                       Typically 512 bytes per sector.
 * @member part_count  - Size of @parts array. Amount of partitions
 *                       associated with the context.
 * @member part_name   - Used to temporarily acquire and store name
 *                       of a partition. Or the absolute path to
 *                       the block device partition.
 */
struct uprov_device
{
	struct udo_log_error_struct err;
	uint8_t                     free;
	struct uprov_device_part    parts[PARTITIONS_MAX];
	char                        fname[UDO_FILE_PATH_MAX];
	int                         fname_fd;
	char                        ptable_type[TABLE_TYPE_MAX];
	uint16_t                    sector_sz;
	size_t                      part_count;
	char                        part_name[PART_NAME_MAX];
};

/*****************************************
 * Start of global to C source functions *
 *****************************************/

struct p_uprov_fdisk
{
	struct fdisk_context   *ctx;
	struct fdisk_table     *table;
	struct fdisk_partition *part;
	struct fdisk_parttype  *parttype;
};


static void
p_uprov_fdisk_part_destroy (struct p_uprov_fdisk *fdisk)
{
	if (fdisk->parttype) {
		fdisk_unref_parttype(fdisk->parttype);
		fdisk->parttype = NULL;
	}

	if (fdisk->part) {
		fdisk_unref_partition(fdisk->part);
		fdisk->part = NULL;
	}
}


static void
p_uprov_fdisk_destroy (struct p_uprov_fdisk *fdisk)
{
	if (fdisk->table) {
		fdisk_unref_table(fdisk->table);
		fdisk->table = NULL;
	}

	if (fdisk->ctx) {
		fdisk_deassign_device(fdisk->ctx, 1);
		fdisk_unref_context(fdisk->ctx);
		fdisk->ctx = NULL;
	}
}

/***************************************
 * End of global to C source functions *
 ***************************************/


/***************************************
 * Start uprov_device_create functions *
 ***************************************/

static int
p_device_create_with_fdisk (struct uprov_device *device)
{
	uint32_t p;
	uint8_t gpt;
	int err = -1;

	struct stat sb;

	struct p_uprov_fdisk fdisk;

	struct fdisk_label *lb = NULL;

	char *fstype = NULL, *fslabel = NULL, *partlabel = NULL;

	memset(&fdisk, 0, sizeof(struct p_uprov_fdisk));

	device->fname_fd = open(device->fname, O_RDWR);
	if (device->fname_fd == -1) {
		udo_log_error("open: %s\n", strerror(errno));
		p_uprov_fdisk_destroy(&fdisk);
		return -1;
	}

	err = fstat(device->fname_fd, &sb);
	if (err == -1) {
		udo_log_error("fstat: %s\n", strerror(errno));
		return -1;
	}

	if ((sb.st_mode & S_IFMT) != S_IFBLK)
		return 0;

	fdisk.ctx = fdisk_new_context();
	if (!(fdisk.ctx)) {
		udo_log_error("fdisk_new_context failed\n");
		return -1;
	}

	err = fdisk_assign_device_by_fd(fdisk.ctx, \
		device->fname_fd, device->fname, 0);
	if (err < 0) {
		udo_log_error("fdisk_assign_device_by_fd('%d','%s') failed\n", \
		              device->fname_fd, device->fname);
		p_uprov_fdisk_destroy(&fdisk);
		return -1;
	}

	err = fdisk_get_partitions(fdisk.ctx, &(fdisk.table));
	if (err != 0) {
		udo_log_error("fdisk_get_partitions('%d','%s') failed\n", \
		              device->fname_fd, device->fname);
		p_uprov_fdisk_destroy(&fdisk);
		return -1;
	}

	device->sector_sz = fdisk_get_sector_size(fdisk.ctx);
	device->part_count = fdisk_table_get_nents(fdisk.table);

	lb = fdisk_get_label(fdisk.ctx, NULL);
	strncpy(device->ptable_type, fdisk_label_get_name(lb), TABLE_TYPE_MAX-1);
	gpt = UDO_STRTOU(device->ptable_type);

	for (p = 0; p < device->part_count; p++) {
		fdisk.part = fdisk_table_get_partition_by_partno(fdisk.table, p);

		device->parts[p].number = fdisk_partition_get_partno(fdisk.part) + 1;
		device->parts[p].start_sector = fdisk_partition_get_start(fdisk.part);
		device->parts[p].end_sector = fdisk_partition_get_end(fdisk.part);
		device->parts[p].sector_size = fdisk_partition_get_size(fdisk.part);

		/* Acquire fslabel of a partiton */
		fdisk_partition_to_string(fdisk.part, fdisk.ctx, \
					  FDISK_FIELD_FSLABEL, \
					  &fslabel);
		if (fslabel) {
			strncpy(device->parts[p].fslabel, \
				fslabel, FSLABEL_MAX);
			free(fslabel); fslabel = NULL;
		}

		/* Acquire fstype of a partition */
		fdisk_partition_to_string(fdisk.part, fdisk.ctx, \
		                          FDISK_FIELD_FSTYPE, \
		                          &fstype);
		if (fstype) {
			strncpy(device->parts[p].fstype, \
				fstype, FSTYPE_MAX);
			free(fstype); fstype = NULL;
		}

		fdisk.parttype = fdisk_partition_get_type(fdisk.part);

		if (gpt == IS_GPT) {
			strncpy(device->parts[p].type.code_str, \
				fdisk_parttype_get_string(fdisk.parttype), \
				TYPE_CODE_STR_MAX);

			fdisk_partition_to_string(fdisk.part, fdisk.ctx, \
						  FDISK_FIELD_NAME, \
						  &partlabel);
			if (partlabel) {
				strncpy(device->parts[p].partlabel, \
					partlabel, PARTLABEL_MAX);
				free(partlabel); partlabel = NULL;
			}
		} else {
			device->parts[p].type.code = \
				fdisk_parttype_get_code(fdisk.parttype);

			if (fdisk_partition_is_nested(fdisk.part)) {
				device->parts[p].logical = true;
			} else if (fdisk_partition_is_container(fdisk.part)) {
				device->parts[p].extended = true;
			}
		}

		p_uprov_fdisk_part_destroy(&fdisk);
	}

	p_uprov_fdisk_destroy(&fdisk);

	return 0;
}


struct uprov_device *
uprov_device_create (struct uprov_device *p_device,
                     const char *fname)
{
	int ret = -1;

	struct uprov_device *device = p_device;

	if (!fname) {
		udo_log_error("Incorrect data passed\n");
		return NULL;
	}

	if (!device) {
		device = calloc(1, sizeof(struct uprov_device));
		if (!device) {
			udo_log_error("calloc: %s\n", strerror(errno));
			return NULL;
		}
	}

	strncpy(&(device->fname[0]), fname, \
		UDO_FILE_PATH_MAX - 1);

	ret = p_device_create_with_fdisk(device);
	if (ret == -1) {
		uprov_device_destroy(device);
		return NULL;
	}

	return device;
}

/*************************************
 * End uprov_device_create functions *
 *************************************/


/***************************************
 * Start uprov_device_resize functions *
 ***************************************/

struct p_uprov_disk_part
{
	int temp;
};

struct p_uprov_disk
{
	struct p_uprov_disk_part parts[PARTITIONS_MAX];
};


static int
p_parse_part_string (struct p_uprov_disk UDO_UNUSED *udisk,
                     const char *part_string)
{
	uint16_t word = 0;

	while (*part_string) {
		switch (*part_string) {
			case ' ':
			case ':':
			case '\n':
				part_string++;
				continue;
			default:
				word += *part_string;
				break;
		}

		switch (word) {
			case 321: /* mbr */
				fprintf(stdout, "mbr\n");
				word = 0;
				break;
			case 331: /* gpt */
				fprintf(stdout, "gpt\n");
				word = 0;
				break;
			case 440: /* PTABLE */
				fprintf(stdout, "PTABLE\n");
				word = 0;
				break;
			default:
				break;
		}

		part_string++;
	}

	return 0;
}


int
uprov_device_resize_wholedisk (struct uprov_device *device,
                               const char *part_string)
{
	int err = -1;

	struct p_uprov_disk disk;

	if (!device || !part_string) {
		udo_log_error("Incorrect data passed\n");
		return -1;
	}

	err = p_parse_part_string(&disk, part_string);
	if (err == -1)
		return -1;

	return 0;
}

/*************************************
 * End uprov_device_resize functions *
 *************************************/


/************************************
 * Start uprov_device_get functions *
 ************************************/

const char *
uprov_device_get_fname (struct uprov_device *device)
{
	if (!device)
		return NULL;

	if (!(*device->fname)) {
		udo_log_set_error(device, UDO_LOG_ERR_UNCOMMON, \
		                  "Block device name not found.\n");
		return NULL;
	}

	return device->fname;
}


int
uprov_device_get_fname_fd (struct uprov_device *device)
{
	if (!device)
		return -1;

	return device->fname_fd;
}


const char *
uprov_device_get_ptable_type (struct uprov_device *device)
{
	if (!device)
		return NULL;

	if (!(*device->ptable_type)) {
		udo_log_set_error(device, UDO_LOG_ERR_UNCOMMON, \
		                  "Partition table type name not found.\n");
		return NULL;
	}

	return device->ptable_type;
}


uint16_t
uprov_device_get_sector_sz (struct uprov_device *device)
{
	if (!device)
		return UINT16_MAX;

	return device->sector_sz;
}


size_t
uprov_device_get_part_count (struct uprov_device *device)
{
	if (!device)
		return (size_t)-1;

	return device->part_count;
}


size_t
uprov_device_get_part_num (struct uprov_device *device,
                           const size_t part_index)
{
	if (!device)
		return (size_t)-1;

	return device->parts[part_index].number;
}


static const char *
p_get_part_name_format (const char *fname)
{
	uint32_t var = 0;

	while (*fname) {
		var += (uint32_t) *fname;
		switch (var) {
			case 617: /* /dev/hd */
			case 628: /* /dev/sd */
			case 631: /* /dev/vd */
				return "%s%d";
			case 851: /* /dev/nvme */
			case 855: /* /dev/loop  */
			case 1043: /* /dev/mmcblk  */
				return "%sp%d";
			default:
				break;
		}

		fname++;
	}

	return "%s%d";
}


const char *
uprov_device_get_part_name (struct uprov_device *device,
                            const size_t part_index)
{
	if (!device)
		return NULL;

	snprintf(device->part_name, PART_NAME_MAX, \
		p_get_part_name_format(device->fname), \
		device->fname, device->parts[part_index].number);

	return device->part_name;
}


uint64_t
uprov_device_get_part_start_sector (struct uprov_device *device,
                                    const size_t part_index)
{
	if (!device)
		return UINT64_MAX;

	return device->parts[part_index].start_sector;
}


uint64_t
uprov_device_get_part_end_sector (struct uprov_device *device,
                                  const size_t part_index)
{
	if (!device)
		return UINT64_MAX;

	return device->parts[part_index].end_sector;
}


uint64_t
uprov_device_get_part_sector_size (struct uprov_device *device,
                                   const size_t part_index)
{
	if (!device)
		return UINT64_MAX;

	return device->parts[part_index].sector_size;
}


uint8_t
uprov_device_get_part_logical (struct uprov_device *device,
                               const size_t part_index)
{
	if (!device)
		return UINT8_MAX;

	return device->parts[part_index].logical;
}


uint8_t
uprov_device_get_part_extended (struct uprov_device *device,
                                const size_t part_index)
{
	if (!device)
		return UINT8_MAX;

	return device->parts[part_index].extended;
}


const char *
uprov_device_get_part_fstype (struct uprov_device *device,
                              const size_t part_index)
{
	if (!device)
		return NULL;

	if (!(*device->parts[part_index].fstype)) {
		udo_log_set_error(device, UDO_LOG_ERR_UNCOMMON, \
		                  "File system type not set.\n");
		return NULL;
	}

	return device->parts[part_index].fstype;
}


const char *
uprov_device_get_part_fslabel (struct uprov_device *device,
                               const size_t part_index)
{
	if (!device)
		return NULL;

	if (!(*device->parts[part_index].fslabel)) {
		udo_log_set_error(device, UDO_LOG_ERR_UNCOMMON, \
		                  "File system label not set.\n");
		return NULL;
	}

	return device->parts[part_index].fslabel;
}


const char *
uprov_device_get_part_partlabel (struct uprov_device *device,
                                 const size_t part_index)
{
	if (!device)
		return NULL;

	if (UDO_STRTOU(device->ptable_type) != IS_GPT) {
		udo_log_set_error(device, UDO_LOG_ERR_UNCOMMON, \
		                  "Not a GPT disk\n");
		return NULL;
	}

	if (!(*device->parts[part_index].partlabel)) {
		udo_log_set_error(device, UDO_LOG_ERR_UNCOMMON, \
		                  "GPT PARTLABEL not set.\n");
		return NULL;
	}

	return device->parts[part_index].partlabel;
}


uint32_t
uprov_device_get_part_type_code (struct uprov_device *device,
                                 const size_t part_index)
{
	if (!device)
		return UINT32_MAX;

	if (UDO_STRTOU(device->ptable_type) == IS_GPT) {
		udo_log_set_error(device, UDO_LOG_ERR_UNCOMMON, \
		                  "Not an MBR disk\n");
		return UINT32_MAX;
	}

	return device->parts[part_index].type.code;
}


const char *
uprov_device_get_part_type_code_string (struct uprov_device *device,
                                        const size_t part_index)
{
	if (!device || \
	    !(*device->parts[part_index].type.code_str))
		return NULL;

	if (UDO_STRTOU(device->ptable_type) != IS_GPT) {
		udo_log_set_error(device, UDO_LOG_ERR_UNCOMMON, \
		                  "Not a GPT disk\n");
		return NULL;
	}

	if (!(*device->parts[part_index].type.code_str)) {
		udo_log_set_error(device, UDO_LOG_ERR_UNCOMMON, \
		                  "GPT partition type code not set.\n");
		return NULL;
	}

	return device->parts[part_index].type.code_str;
}

/**********************************
 * End uprov_device_get functions *
 **********************************/


/****************************************
 * Start uprov_device_destroy functions *
 ****************************************/

void
uprov_device_destroy (struct uprov_device *device)
{
	if (!device)
		return;

	close(device->fname_fd);

	if (device->free) {
		free(device);
	} else {
		memset(device, 0, sizeof(struct uprov_device));
	}
}

/**************************************
 * End uprov_device_destroy functions *
 **************************************/


/****************************************************
 * Start of non struct uprov_device param functions *
 ****************************************************/

int
uprov_device_get_sizeof (void)
{
	return sizeof(struct uprov_device);
}

/**************************************************
 * End of non struct uprov_device param functions *
 **************************************************/
