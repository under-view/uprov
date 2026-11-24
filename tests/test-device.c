/*
 * MIT License
 *
 * Copyright (c) 2026-2027 Underview
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

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include <udo/udo.h>

#include <libfdisk/libfdisk.h>

#include "device.h"

/*****************************************
 * Start of global test-device functions *
 *****************************************/

#define DISK_FILE "/tmp/disk-file.img"

static const char \
UDO_UNUSED \
p_device_create_gpt_disk_str[] = \
	"PTABLE: gpt\n" \
	"PART: 1:2048:2048:21686148-6449-6E6F-744E-656564454649:none:BIOS_BOOT:\n" \
	"PART: 2:4096:98304:C12A7328-F81F-11D2-BA4B-00A0C93EC93B:vfat:EFI_SYSTEM:\n" \
	"PART: 3:102400:159744:0FC63DAF-8483-4772-8E79-3D69D8477DE4:ext4:GRUB_DATA:\n" \
	"PART: 4:262144:159744:0FC63DAF-8483-4772-8E79-3D69D8477DE4:ext4:ROOTFS:\n" \
	;


static const char \
UDO_UNUSED \
p_device_repart_gpt_disk_str[] = \
	"PART: 3:102400:159744:C12A7328-F81F-11D2-BA4B-00A0C93EC93B:vfat:GRUB_DATA:" \
	;


static const char \
UDO_UNUSED \
p_device_create_mbr_disk_str[] = \
	"PTABLE: mbr\n" \
	"PART: 1:2048:98304:83:ext4:LABEL_ONE:\n" \
	;


static const char \
UDO_UNUSED \
p_device_repart_mbr_disk_str[] = \
	"PART: 1:2048:98304:83:vfat:LABEL_TWO:" \
	;


static int
p_device_create_empty_file (const char *fname)
{
	struct udo_file_ops *flops = NULL;

	const size_t file_size = (size_t) (1<<29);

	struct udo_file_ops_create_info file_info;

	memset(&file_info, 0, sizeof(file_info));

	file_info.fname = fname;
	file_info.size = file_size;
	flops = udo_file_ops_create(NULL, &file_info);
	if (!flops)
		return -1;

	udo_file_ops_destroy(flops, file_size);

	return 0;
}


static struct uprov_device *
p_device_create_mbr_disk (void)
{
	int err = -1;

	struct uprov_device *device = NULL;

	const uint32_t ask_type = FDISK_ASKTYPE_NONE;

	/*
	const uint32_t ask_type = \
		FDISK_ASKTYPE_WARN | \
		FDISK_ASKTYPE_WARNX | \
		FDISK_ASKTYPE_INFO;
	*/

	err = p_device_create_empty_file(DISK_FILE);
	if (err == -1)
		return NULL;

	device = uprov_device_create(NULL, DISK_FILE, ask_type);
	if (!device)
		return NULL;

	err = uprov_device_repart_wholedisk(device, p_device_create_mbr_disk_str, ask_type);
	if (err == -1) {
		uprov_device_destroy(device);
		return NULL;
	}

	return device;
}


static struct uprov_device *
p_device_create_gpt_disk (struct uprov_device *p_device)
{
	int err = -1;

	struct uprov_device *device = p_device;

	const uint32_t ask_type = FDISK_ASKTYPE_NONE;

	/*
	const uint32_t ask_type = \
		FDISK_ASKTYPE_WARN | \
		FDISK_ASKTYPE_WARNX | \
		FDISK_ASKTYPE_INFO;
	*/

	if (device) {
		uprov_device_destroy_parts(device);
	} else {
		err = p_device_create_empty_file(DISK_FILE);
		if (err == -1)
			return NULL;

		device = uprov_device_create(NULL, DISK_FILE, ask_type);
		if (!device)
			return NULL;
	}

	err = uprov_device_repart_wholedisk(device, p_device_create_gpt_disk_str, ask_type);
	if (err == -1) {
		uprov_device_destroy(device);
		return NULL;
	}

	return device;
}

/***************************************
 * End of global test-device functions *
 ***************************************/


/*****************************************
 * Start of test_device_create functions *
 *****************************************/

static void UDO_UNUSED
test_device_create (void UDO_UNUSED **state)
{
	int err = -1;

	struct uprov_device *device = NULL;

	err = p_device_create_empty_file(DISK_FILE);
	assert_int_equal(err, 0);

	device = uprov_device_create(NULL, DISK_FILE, FDISK_ASKTYPE_NONE);
	assert_non_null(device);

	uprov_device_destroy(device);

	remove(DISK_FILE);
}

/***************************************
 * End of test_device_create functions *
 ***************************************/


/*****************************************
 * Start of test_device_resize functions *
 *****************************************/

static void UDO_UNUSED
test_device_repart_wholedisk (void UDO_UNUSED **state)
{
	struct uprov_device *device = NULL;

	device = p_device_create_mbr_disk();
	assert_non_null(device);

	device = p_device_create_gpt_disk(device);
	assert_non_null(device);

	uprov_device_destroy(device);

	remove(DISK_FILE);
}


static void UDO_UNUSED
test_device_repart_part (void UDO_UNUSED **state)
{
	int err = -1;

	struct uprov_device *device = NULL;

	device = p_device_create_mbr_disk();
	assert_non_null(device);

	err = uprov_device_repart_part(device, p_device_repart_mbr_disk_str, FDISK_ASKTYPE_NONE);
	assert_int_equal(err, 0);

	device = p_device_create_gpt_disk(device);
	assert_non_null(device);

	err = uprov_device_repart_part(device, p_device_repart_gpt_disk_str, FDISK_ASKTYPE_NONE);
	assert_int_equal(err, 0);

	uprov_device_destroy(device);

	remove(DISK_FILE);
}

/***************************************
 * End of test_device_resize functions *
 ***************************************/


/**************************************
 * Start of test_device_get functions *
 **************************************/

static void UDO_UNUSED
test_device_get_fname (void UDO_UNUSED **state)
{
	const char *fname = NULL;
	struct uprov_device *device = NULL;

	device = p_device_create_mbr_disk();
	assert_non_null(device);

	fname = uprov_device_get_fname(device);
	assert_string_equal(fname, DISK_FILE);

	uprov_device_destroy(device);

	remove(DISK_FILE);
}


static void UDO_UNUSED
test_device_get_fname_fd (void UDO_UNUSED **state)
{
	int fd = -1;
	struct uprov_device *device = NULL;

	device = p_device_create_mbr_disk();
	assert_non_null(device);

	fd = uprov_device_get_fname_fd(device);
	assert_int_not_equal(fd, -1);

	uprov_device_destroy(device);

	remove(DISK_FILE);
}


static void UDO_UNUSED
test_device_get_ptable_type (void UDO_UNUSED **state)
{
	const char *ptable_type = NULL;
	struct uprov_device *device = NULL;

	ptable_type = uprov_device_get_ptable_type(NULL);
	assert_null(ptable_type);

	device = p_device_create_mbr_disk();
	assert_non_null(device);

	ptable_type = uprov_device_get_ptable_type(device);
	assert_string_equal(ptable_type, "mbr");

	device = p_device_create_gpt_disk(device);
	assert_non_null(device);

	ptable_type = uprov_device_get_ptable_type(device);
	assert_string_equal(ptable_type, "gpt");

	uprov_device_destroy(device);

	remove(DISK_FILE);
}


static void UDO_UNUSED
test_device_get_sector_sz (void UDO_UNUSED **state)
{
	uint16_t sector_sz = 0;
	struct uprov_device *device = NULL;

	sector_sz = uprov_device_get_sector_sz(NULL);
	assert_int_equal(sector_sz, UINT16_MAX);

	device = p_device_create_mbr_disk();
	assert_non_null(device);

	sector_sz = uprov_device_get_sector_sz(device);
	assert_int_equal(sector_sz, 512); sector_sz = 0;

	device = p_device_create_gpt_disk(device);
	assert_non_null(device);

	sector_sz = uprov_device_get_sector_sz(device);
	assert_int_equal(sector_sz, 512);

	uprov_device_destroy(device);

	remove(DISK_FILE);
}


static void UDO_UNUSED
test_device_get_part_count (void UDO_UNUSED **state)
{
	size_t part_count = 0;
	struct uprov_device *device = NULL;

	part_count = uprov_device_get_part_count(NULL);
	assert_int_equal(part_count, (size_t)-1);

	device = p_device_create_mbr_disk();
	assert_non_null(device);

	part_count = uprov_device_get_part_count(device);
	assert_int_equal(part_count, 1);

	device = p_device_create_gpt_disk(device);
	assert_non_null(device);

	part_count = uprov_device_get_part_count(device);
	assert_int_equal(part_count, 4);

	uprov_device_destroy(device);

	remove(DISK_FILE);
}


static void UDO_UNUSED
test_device_get_part_num (void UDO_UNUSED **state)
{
	size_t part_num = 0;
	struct uprov_device *device = NULL;

	part_num = uprov_device_get_part_num(NULL, 0);
	assert_int_equal(part_num, (size_t)-1);

	device = p_device_create_mbr_disk();
	assert_non_null(device);

	part_num = uprov_device_get_part_num(device, 3);
	assert_int_equal(part_num, (size_t)-1);

	part_num = uprov_device_get_part_num(device, 0);
	assert_int_equal(part_num, 1);

	device = p_device_create_gpt_disk(device);
	assert_non_null(device);

	part_num = uprov_device_get_part_num(device, 6);
	assert_int_equal(part_num, (size_t)-1);

	part_num = uprov_device_get_part_num(device, 2);
	assert_int_equal(part_num, 3);

	uprov_device_destroy(device);

	remove(DISK_FILE);
}


static void UDO_UNUSED
test_device_get_part_name (void UDO_UNUSED **state)
{
	struct uprov_device *device = NULL;
	const char *part_name = NULL, *fname = NULL;

	part_name = uprov_device_get_part_name(NULL, 0);
	assert_null(part_name);

	device = p_device_create_mbr_disk();
	assert_non_null(device);

	part_name = uprov_device_get_part_name(device, 1);
	assert_null(part_name);

	part_name = uprov_device_get_part_name(device, 0);
	assert_non_null(strstr(part_name, DISK_FILE));
	assert_non_null(strstr(part_name, "1"));

	fname = uprov_device_get_fname(device);
	assert_string_equal(fname, DISK_FILE);

	device = p_device_create_gpt_disk(device);
	assert_non_null(device);

	part_name = uprov_device_get_part_name(device, 6);
	assert_null(part_name);

	part_name = uprov_device_get_part_name(device, 3);
	assert_non_null(strstr(part_name, DISK_FILE));
	assert_non_null(strstr(part_name, "4"));

	uprov_device_destroy(device);

	remove(DISK_FILE);
}


static void UDO_UNUSED
test_device_get_part_start_sector (void UDO_UNUSED **state)
{
	uint64_t start_sector = 0;
	struct uprov_device *device = NULL;

	start_sector = uprov_device_get_part_start_sector(NULL, 0);
	assert_true(start_sector == UINT64_MAX);

	device = p_device_create_mbr_disk();
	assert_non_null(device);

	start_sector = uprov_device_get_part_start_sector(device, 2);
	assert_true(start_sector == UINT64_MAX);

	start_sector = uprov_device_get_part_start_sector(device, 0);
	assert_int_equal(start_sector, 2048);

	device = p_device_create_gpt_disk(device);
	assert_non_null(device);

	start_sector = uprov_device_get_part_start_sector(device, 8);
	assert_true(start_sector == UINT64_MAX);

	start_sector = uprov_device_get_part_start_sector(device, 2);
	assert_int_equal(start_sector, 102400);

	uprov_device_destroy(device);

	remove(DISK_FILE);
}


static void UDO_UNUSED
test_device_get_part_end_sector (void UDO_UNUSED **state)
{
	uint64_t end_sector = 0;
	struct uprov_device *device = NULL;

	end_sector = uprov_device_get_part_end_sector(NULL, 0);
	assert_true(end_sector == UINT64_MAX);

	device = p_device_create_mbr_disk();
	assert_non_null(device);

	end_sector = uprov_device_get_part_end_sector(device, 2);
	assert_true(end_sector == UINT64_MAX);

	end_sector = uprov_device_get_part_end_sector(device, 0);
	assert_int_equal(end_sector, 100351);

	device = p_device_create_gpt_disk(device);
	assert_non_null(device);

	end_sector = uprov_device_get_part_end_sector(device, 8);
	assert_true(end_sector == UINT64_MAX);

	end_sector = uprov_device_get_part_end_sector(device, 2);
	assert_int_equal(end_sector, 262143);

	uprov_device_destroy(device);

	remove(DISK_FILE);
}


static void UDO_UNUSED
test_device_get_part_size_sector (void UDO_UNUSED **state)
{
	uint64_t size_sector = 0;
	struct uprov_device *device = NULL;

	size_sector = uprov_device_get_part_size_sector(NULL, 0);
	assert_true(size_sector == UINT64_MAX);

	device = p_device_create_mbr_disk();
	assert_non_null(device);

	size_sector = uprov_device_get_part_size_sector(device, 2);
	assert_true(size_sector == UINT64_MAX);

	size_sector = uprov_device_get_part_size_sector(device, 0);
	assert_int_equal(size_sector, 98304);

	device = p_device_create_gpt_disk(device);
	assert_non_null(device);

	size_sector = uprov_device_get_part_size_sector(device, 8);
	assert_true(size_sector == UINT64_MAX);

	size_sector = uprov_device_get_part_size_sector(device, 2);
	assert_int_equal(size_sector, 159744);

	uprov_device_destroy(device);

	remove(DISK_FILE);
}


static void UDO_UNUSED
test_device_get_part_size_bytes (void UDO_UNUSED **state)
{
	uint64_t bytes = 0;
	struct uprov_device *device = NULL;

	bytes = uprov_device_get_part_size_bytes(NULL, 0);
	assert_true(bytes == UINT64_MAX);

	device = p_device_create_mbr_disk();
	assert_non_null(device);

	bytes = uprov_device_get_part_size_bytes(device, 2);
	assert_true(bytes == UINT64_MAX);

	bytes = uprov_device_get_part_size_bytes(device, 0);
	assert_int_equal(bytes, 50331648);

	device = p_device_create_gpt_disk(device);
	assert_non_null(device);

	bytes = uprov_device_get_part_size_bytes(device, 8);
	assert_true(bytes == UINT64_MAX);

	bytes = uprov_device_get_part_size_bytes(device, 2);
	assert_int_equal(bytes, 81788928);

	uprov_device_destroy(device);

	remove(DISK_FILE);
}


static void UDO_UNUSED
test_device_get_part_logical (void UDO_UNUSED **state)
{
	uint8_t logical = 0;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, DISK_FILE, FDISK_ASKTYPE_NONE);
	assert_non_null(device);

	logical = uprov_device_get_part_logical(NULL, 0);
	assert_true(logical == UINT8_MAX);

	logical = uprov_device_get_part_logical(device, 1);
	assert_true(logical == 0 || logical == 1);

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_part_extended (void UDO_UNUSED **state)
{
	uint8_t extended = 0;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, DISK_FILE, FDISK_ASKTYPE_NONE);
	assert_non_null(device);

	extended = uprov_device_get_part_extended(NULL, 0);
	assert_true(extended == UINT8_MAX);

	extended = uprov_device_get_part_extended(device, 1);
	assert_true(extended == 0 || extended == 1);

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_part_fstype (void UDO_UNUSED **state)
{
	const char *fstype = NULL;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, DISK_FILE, FDISK_ASKTYPE_NONE);
	assert_non_null(device);

	fstype = uprov_device_get_part_fstype(NULL, 0);
	assert_null(fstype);

	fstype = uprov_device_get_part_fstype(device, 1);
	assert_non_null(fstype);

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_part_fslabel (void UDO_UNUSED **state)
{
	const char *fslabel = NULL;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, DISK_FILE, FDISK_ASKTYPE_NONE);
	assert_non_null(device);

	fslabel = uprov_device_get_part_fslabel(NULL, 0);
	assert_null(fslabel);

	/*
	 * TODO: update test so that disk type
	 * and partitions are created during the test.
	 *
	 * fslabel = uprov_device_get_part_partlabel(device, 1);
	 * assert_non_null(fslabel);
	 */

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_part_partlabel (void UDO_UNUSED **state)
{
	const char *partlabel = NULL;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, DISK_FILE, FDISK_ASKTYPE_NONE);
	assert_non_null(device);

	partlabel = uprov_device_get_part_partlabel(NULL, 0);
	assert_null(partlabel);

	/*
	 * TODO: update test so that disk type
	 * and partitions are created during the test.
	 *
	 * partlabel = uprov_device_get_part_partlabel(device, 1);
	 * assert_non_null(partlabel);
	 */

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_part_type_code (void UDO_UNUSED **state)
{
	uint32_t type_code = UINT32_MAX;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, DISK_FILE, FDISK_ASKTYPE_NONE);
	assert_non_null(device);

	type_code = uprov_device_get_part_type_code(NULL, 0);
	assert_uint_equal(type_code, UINT32_MAX);

	/*
	 * TODO: update test so that disk type
	 * and partitions are created during the test.
	 *
	 * type_code = uprov_device_get_part_type_code(device, 1);
	 * assert_uint_equal(type_code, UINT32_MAX);
	 */

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_part_type_code_string (void UDO_UNUSED **state)
{
	const char *type_code_str = NULL;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, DISK_FILE, FDISK_ASKTYPE_NONE);
	assert_non_null(device);

	type_code_str = uprov_device_get_part_type_code_string(NULL, 0);
	assert_null(type_code_str);

	/*
	 * TODO: update test so that disk type
	 * and partitions are created during the test.
	 *
	 * type_code_str = uprov_device_get_part_type_code_string(NULL, 0);
	 * assert_non_null(type_code_str);
	 */

	uprov_device_destroy(device);
}

/************************************
 * End of test_device_get functions *
 ************************************/


/*********************************************
 * Start of test_device_get_sizeof functions *
 *********************************************/

static void UDO_UNUSED
test_device_get_sizeof (void UDO_UNUSED **state)
{
	int size = 0;
	size = uprov_device_get_sizeof();
	assert_int_not_equal(size, 0);
}

/*******************************************
 * End of test_device_get_sizeof functions *
 *******************************************/


/***********************************************
 * Start of test_device_str_to_bytes functions *
 ***********************************************/

static void UDO_UNUSED
test_device_str_to_bytes (void UDO_UNUSED **state)
{
	char string[64];

	size_t size = 0;

	strncpy(string, "4G", sizeof("4G")+1);
	size = uprov_device_str_to_bytes(string);
	assert_int_equal(size, 4294967296);
	memset(string, 0, sizeof(string));

	strncpy(string, "512M", sizeof("512M")+1);
	size = uprov_device_str_to_bytes(string);
	assert_int_equal(size, 536870912);
	memset(string, 0, sizeof(string));

	strncpy(string, "64K", sizeof("64K")+1);
	size = uprov_device_str_to_bytes(string);
	assert_int_equal(size, 65536);
	memset(string, 0, sizeof(string));

	strncpy(string, "862688232", sizeof("862688232")+1);
	size = uprov_device_str_to_bytes(string);
	assert_int_equal(size, 862688232);
}

/*********************************************
 * End of test_device_str_to_bytes functions *
 *********************************************/

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_device_create),
		cmocka_unit_test(test_device_repart_wholedisk),
		cmocka_unit_test(test_device_repart_part),
		cmocka_unit_test(test_device_get_fname),
		cmocka_unit_test(test_device_get_fname_fd),
		cmocka_unit_test(test_device_get_ptable_type),
		cmocka_unit_test(test_device_get_sector_sz),
		cmocka_unit_test(test_device_get_part_count),
		cmocka_unit_test(test_device_get_part_num),
		cmocka_unit_test(test_device_get_part_name),
		cmocka_unit_test(test_device_get_part_start_sector),
		cmocka_unit_test(test_device_get_part_end_sector),
		cmocka_unit_test(test_device_get_part_size_sector),
		cmocka_unit_test(test_device_get_part_size_bytes),
		cmocka_unit_test(test_device_get_part_logical),
		cmocka_unit_test(test_device_get_part_extended),
		cmocka_unit_test(test_device_get_part_fstype),
		cmocka_unit_test(test_device_get_part_fslabel),
		cmocka_unit_test(test_device_get_part_partlabel),
		cmocka_unit_test(test_device_get_part_type_code),
		cmocka_unit_test(test_device_get_part_type_code_string),
		cmocka_unit_test(test_device_get_sizeof),
		cmocka_unit_test(test_device_str_to_bytes),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
