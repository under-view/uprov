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

/*
 * Required by cmocka
 */
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include <udo/log.h>

#include "cmdline.h"

/**********************************************
 * Start of test_cmdline_flags_zero functions *
 **********************************************/

static void UDO_UNUSED
test_cmdline_flags_zero (void UDO_UNUSED **state)
{
	struct uprov_cmdline *cmdline = NULL;

	const char *test_flags[] = { "uprov" };

	cmdline = uprov_cmdline_create(NULL, \
		test_flags, UDO_ARRAY_SIZE(test_flags));
	assert_null(cmdline);
}

/********************************************
 * End of test_cmdline_flags_zero functions *
 ********************************************/


/**********************************************
 * Start of test_cmdline_flags_help functions *
 **********************************************/

static void UDO_UNUSED
test_cmdline_flags_help (void UDO_UNUSED **state)
{
	struct uprov_cmdline *cmdline = NULL;

	const char *test_flags[] = { "./uprov", "--help" };

	cmdline = uprov_cmdline_create(NULL, \
		test_flags, UDO_ARRAY_SIZE(test_flags));
	assert_null(cmdline);

	test_flags[1] = "-h";
	cmdline = uprov_cmdline_create(NULL, \
		test_flags, UDO_ARRAY_SIZE(test_flags));
	assert_null(cmdline);
}

/********************************************
 * End of test_cmdline_flags_help functions *
 ********************************************/


/************************************************
 * Start of test_cmdline_flags_config functions *
 ************************************************/

static void UDO_UNUSED
test_cmdline_flags_config (void UDO_UNUSED **state)
{
	struct uprov_cmdline *cmdline = NULL;

	const char *config_file = NULL, *config_file_eq = NULL;

	const char *test_flags[] = { "./uprov", "--config", "some-file.txt" };
	const char *test_flags_eq[] = { "./uprov", "--config=some-file.txt" };

	config_file_eq = (const char *) &(test_flags_eq[1][9]);

	udo_log_set_level(UDO_LOG_ALL);

	/*************** test_flags ***************/

	cmdline = uprov_cmdline_create(NULL, test_flags, UDO_ARRAY_SIZE(test_flags));
	assert_non_null(cmdline);
	config_file = uprov_cmdline_get_config_file(cmdline);
	assert_string_equal(config_file, test_flags[2]);
	uprov_cmdline_destroy(cmdline); cmdline = NULL;

	test_flags[1] = "-c";
	cmdline = uprov_cmdline_create(NULL, test_flags, UDO_ARRAY_SIZE(test_flags));
	assert_non_null(cmdline);
	config_file = uprov_cmdline_get_config_file(cmdline);
	assert_string_equal(config_file, test_flags[2]);
	uprov_cmdline_destroy(cmdline); cmdline = NULL;

	/*************** test_flags_eq ***************/

	cmdline = uprov_cmdline_create(NULL, test_flags_eq, UDO_ARRAY_SIZE(test_flags_eq));
	assert_non_null(cmdline);
	config_file = uprov_cmdline_get_config_file(cmdline);
	assert_string_equal(config_file, config_file_eq);
	uprov_cmdline_destroy(cmdline); cmdline = NULL;

	test_flags[1] = "-c=some-file.txt";
	cmdline = uprov_cmdline_create(NULL, test_flags_eq, UDO_ARRAY_SIZE(test_flags_eq));
	assert_non_null(cmdline);
	config_file = uprov_cmdline_get_config_file(cmdline);
	assert_string_equal(config_file, config_file_eq);
	uprov_cmdline_destroy(cmdline); cmdline = NULL;
}

/**********************************************
 * End of test_cmdline_flags_config functions *
 **********************************************/


/**********************************************
 * Start of test_cmdline_get_sizeof functions *
 **********************************************/

static void UDO_UNUSED
test_cmdline_get_sizeof (void UDO_UNUSED **state)
{
	int size = 0;
	size = uprov_cmdline_get_sizeof();
	assert_int_not_equal(size, 0);
}

/********************************************
 * End of test_cmdline_get_sizeof functions *
 ********************************************/

int
main (void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_cmdline_flags_zero),
		cmocka_unit_test(test_cmdline_flags_help),
		cmocka_unit_test(test_cmdline_flags_config),
		cmocka_unit_test(test_cmdline_get_sizeof),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
