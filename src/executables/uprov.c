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

#include <libfdisk/libfdisk.h>

#include <udo/udo.h>

#include "cmdline.h"
#include "uprov.h"

/* Flags --file and --config */
#define UPROV_CMDLINE_FLAG_FILE_CONFIG \
	UPROV_CMDLINE_FLAG_FILE | \
	UPROV_CMDLINE_FLAG_CONFIG

/* Flags --file and --resize-last */
#define UPROV_CMDLINE_FLAG_FILE_RESIZE_LAST \
	UPROV_CMDLINE_FLAG_FILE | \
	UPROV_CMDLINE_FLAG_RESIZE_LAST

/* Flags --file, --config, and --size */
#define UPROV_CMDLINE_FLAG_FILE_CONFIG_SIZE \
	UPROV_CMDLINE_FLAG_FILE | \
	UPROV_CMDLINE_FLAG_CONFIG | \
	UPROV_CMDLINE_FLAG_SIZE

/*************************************
 * Start of main function prototypes *
 *************************************/

static int
handle_file_config (struct uprov_cmdline *cmdline);

static int
handle_file_config_size (struct uprov_cmdline *cmdline);

/***********************************
 * End of main function prototypes *
 ***********************************/

int
main (const int argc,
      const char *argv[])
{
	int ret = 1;

	uint32_t flag_type;

	struct uprov_cmdline *cmdline = NULL;

	udo_log_set_level(UDO_LOG_ALL);

	cmdline = uprov_cmdline_create(NULL, argv, argc);
	if (!cmdline)
		return 1;

	flag_type = uprov_cmdline_get_flag_type(cmdline);

	switch (flag_type) {
		case UPROV_CMDLINE_FLAG_FILE_CONFIG:
			ret = handle_file_config(cmdline);
			break;

		case UPROV_CMDLINE_FLAG_FILE_RESIZE_LAST:
			udo_log_print(UDO_LOG_INFO, "Passed --file and --resize-last flags\n");
			break;

		case UPROV_CMDLINE_FLAG_FILE_CONFIG_SIZE:
			ret = handle_file_config_size(cmdline);
			break;

		default:
			udo_log_print(UDO_LOG_ERROR, "No flag combination found.\n");
			uprov_cmdline_destroy(cmdline);
			break;
	}

	uprov_cmdline_destroy(cmdline);

	return ret;
}


/*******************************
 * Start of handle_* functions *
 *******************************/

static int
handle_file_config (struct uprov_cmdline *cmdline)
{
	int err = -1;
	const void *data = NULL;
	const char *part_file = NULL;
	struct udo_file_ops *flops = NULL;
	struct uprov_device *device = NULL;
	uint32_t ask_type = FDISK_ASKTYPE_NONE;

	struct udo_file_ops_create_info file_info;
	memset(&file_info, 0, sizeof(file_info));

	part_file = uprov_cmdline_get_part_file(cmdline);
	device = uprov_device_create(NULL, part_file, ask_type);
	if (!device)
		return 1;

	file_info.fname = uprov_cmdline_get_config_file(cmdline);
	flops = udo_file_ops_create(NULL, &file_info);
	if (!flops) {
		uprov_device_destroy(device);
		return 1;
	}

	data = udo_file_ops_get_data(flops, 0);
	err = uprov_device_repart_wholedisk(device, data, ask_type);
	if (err == -1) {
		udo_file_ops_destroy(flops, 0);
		uprov_device_destroy(device);
		return 1;
	}

	udo_file_ops_destroy(flops, 0);
	uprov_device_destroy(device);

	return 0;
}


static int
handle_file_config_size (struct uprov_cmdline *cmdline)
{
	int err = -1;
	char *size = NULL;
	const void *data = NULL;
	struct udo_file_ops *flops = NULL;
	struct uprov_device *device = NULL;
	uint32_t ask_type = FDISK_ASKTYPE_NONE;

	struct udo_file_ops_create_info file_info;
	memset(&file_info, 0, sizeof(file_info));

	size = (char*) uprov_cmdline_get_size(cmdline);
	file_info.size = uprov_device_str_to_bytes(size);
	file_info.fname = uprov_cmdline_get_part_file(cmdline);
	flops = udo_file_ops_create(NULL, &file_info);
	if (!flops)
		return 1;

	udo_file_ops_destroy(flops, file_info.size); flops = NULL;

	device = uprov_device_create(NULL, file_info.fname, ask_type);
	if (!device)
		return 1;

	memset(&file_info, 0, sizeof(file_info));
	file_info.fname = uprov_cmdline_get_config_file(cmdline);
	flops = udo_file_ops_create(NULL, &file_info);
	if (!flops) {
		uprov_device_destroy(device);
		return 1;
	}

	data = udo_file_ops_get_data(flops, 0);
	err = uprov_device_repart_wholedisk(device, data, ask_type);
	if (err == -1) {
		udo_file_ops_destroy(flops, 0);
		uprov_device_destroy(device);
		return 1;
	}

	udo_file_ops_destroy(flops, 0);
	uprov_device_destroy(device);

	return 0;
}

/*****************************
 * End of handle_* functions *
 *****************************/
