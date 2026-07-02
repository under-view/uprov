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

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include <udo/udo.h>

#include "device.h"

/*****************************************
 * Start of global test-device functions *
 *****************************************/

static const char \
create_gpt_disk[] = \
	"PTABLE: gpt\n" \
	;


static const char \
create_mbr_disk[] = \
	"PTABLE: mbr\n" \
	;


static struct udo_file_ops * UDO_UNUSED
p_device_create_empty_file (const char *fname,
                            const size_t size)
{
	struct udo_file_ops *flops = NULL;

	struct udo_file_ops_create_info file_info;

	memset(&file_info, 0, sizeof(file_info));

	file_info.size = size;
	file_info.fname = fname;
	flops = udo_file_ops_create(NULL, &file_info);
	if (!flops)
		return NULL;

	return flops;
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
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, BLOCK_DEVICE);
	assert_non_null(device);

	uprov_device_destroy(device);
}

/***************************************
 * End of test_device_create functions *
 ***************************************/


/*****************************************
 * Start of test_device_resize functions *
 *****************************************/

static void UDO_UNUSED
test_device_resize_wholedisk (void UDO_UNUSED **state)
{
	int err = -1;

	struct udo_file_ops *flops = NULL;
	struct uprov_device *device = NULL;

	const char *fname = "/tmp/gpt-file.wic";

	flops = p_device_create_empty_file(fname, UDO_PAGE_SIZE*3);
	assert_non_null(flops);

	udo_file_ops_destroy(flops, UDO_PAGE_SIZE*3);

	device = uprov_device_create(NULL, fname);
	assert_non_null(device);

	err = uprov_device_resize_wholedisk(device, create_mbr_disk);
	assert_int_equal(err, 0);

	err = uprov_device_resize_wholedisk(device, create_gpt_disk);
	assert_int_equal(err, 0);

	uprov_device_destroy(device);
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

	device = uprov_device_create(NULL, BLOCK_DEVICE);
	assert_non_null(device);

	fname = uprov_device_get_fname(device);
	assert_string_equal(fname, BLOCK_DEVICE);

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_fname_fd (void UDO_UNUSED **state)
{
	int fd = -1;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, BLOCK_DEVICE);
	assert_non_null(device);

	fd = uprov_device_get_fname_fd(device);
	assert_int_not_equal(fd, -1);

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_ptable_type (void UDO_UNUSED **state)
{
	const char *ptable_type = NULL;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, BLOCK_DEVICE);
	assert_non_null(device);

	ptable_type = uprov_device_get_fname(device);
	assert_non_null(ptable_type);

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_sector_sz (void UDO_UNUSED **state)
{
	uint16_t sector_sz = 0;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, BLOCK_DEVICE);
	assert_non_null(device);

	sector_sz = uprov_device_get_sector_sz(device);
	assert_int_not_equal(sector_sz, UINT16_MAX);

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_part_count (void UDO_UNUSED **state)
{
	size_t part_count = 0;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, BLOCK_DEVICE);
	assert_non_null(device);

	part_count = uprov_device_get_part_count(device);
	assert_int_not_equal(part_count, (size_t)-1);

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_part_num (void UDO_UNUSED **state)
{
	size_t part_num = 0;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, BLOCK_DEVICE);
	assert_non_null(device);

	part_num = uprov_device_get_part_num(NULL, 0);
	assert_int_equal(part_num, (size_t)-1);

	part_num = uprov_device_get_part_num(device, 0);
	assert_int_equal(part_num, 1);

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_part_name (void UDO_UNUSED **state)
{
	const char *part_name = NULL;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, BLOCK_DEVICE);
	assert_non_null(device);

	part_name = uprov_device_get_part_name(NULL, 0);
	assert_null(part_name);

	part_name = uprov_device_get_part_name(device, 2);
	assert_non_null(strstr(part_name, BLOCK_DEVICE));
	assert_non_null(strstr(part_name, "3"));

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_part_start_sector (void UDO_UNUSED **state)
{
	uint64_t start_sector = 0;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, BLOCK_DEVICE);
	assert_non_null(device);

	start_sector = uprov_device_get_part_start_sector(NULL, 0);
	assert_true(start_sector == UINT64_MAX);

	start_sector = uprov_device_get_part_start_sector(device, 0);
	assert_false(start_sector == UINT64_MAX);

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_part_end_sector (void UDO_UNUSED **state)
{
	uint64_t end_sector = 0;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, BLOCK_DEVICE);
	assert_non_null(device);

	end_sector = uprov_device_get_part_end_sector(NULL, 0);
	assert_true(end_sector == UINT64_MAX);

	end_sector = uprov_device_get_part_end_sector(device, 0);
	assert_false(end_sector == UINT64_MAX);

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_part_sector_size (void UDO_UNUSED **state)
{
	uint64_t sector_size = 0;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, BLOCK_DEVICE);
	assert_non_null(device);

	sector_size = uprov_device_get_part_sector_size(NULL, 0);
	assert_true(sector_size == UINT64_MAX);

	sector_size = uprov_device_get_part_sector_size(device, 0);
	assert_false(sector_size == UINT64_MAX);

	uprov_device_destroy(device);
}


static void UDO_UNUSED
test_device_get_part_logical (void UDO_UNUSED **state)
{
	uint8_t logical = 0;
	struct uprov_device *device = NULL;

	device = uprov_device_create(NULL, BLOCK_DEVICE);
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

	device = uprov_device_create(NULL, BLOCK_DEVICE);
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

	device = uprov_device_create(NULL, BLOCK_DEVICE);
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

	device = uprov_device_create(NULL, BLOCK_DEVICE);
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

	device = uprov_device_create(NULL, BLOCK_DEVICE);
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

	device = uprov_device_create(NULL, BLOCK_DEVICE);
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

	device = uprov_device_create(NULL, BLOCK_DEVICE);
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

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_device_create),
		cmocka_unit_test(test_device_resize_wholedisk),
		cmocka_unit_test(test_device_get_fname),
		cmocka_unit_test(test_device_get_fname_fd),
		cmocka_unit_test(test_device_get_ptable_type),
		cmocka_unit_test(test_device_get_sector_sz),
		cmocka_unit_test(test_device_get_part_count),
		cmocka_unit_test(test_device_get_part_num),
		cmocka_unit_test(test_device_get_part_name),
		cmocka_unit_test(test_device_get_part_start_sector),
		cmocka_unit_test(test_device_get_part_end_sector),
		cmocka_unit_test(test_device_get_part_sector_size),
		cmocka_unit_test(test_device_get_part_logical),
		cmocka_unit_test(test_device_get_part_extended),
		cmocka_unit_test(test_device_get_part_fstype),
		cmocka_unit_test(test_device_get_part_fslabel),
		cmocka_unit_test(test_device_get_part_partlabel),
		cmocka_unit_test(test_device_get_part_type_code),
		cmocka_unit_test(test_device_get_part_type_code_string),
		cmocka_unit_test(test_device_get_sizeof),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
