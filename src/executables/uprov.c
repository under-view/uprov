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

int
main (const int argc,
      const char *argv[])
{
	uint32_t flag_type;

	struct uprov_cmdline *cmdline = NULL;

	udo_log_set_level(UDO_LOG_ALL);

	cmdline = uprov_cmdline_create(NULL, argv, argc);
	if (!cmdline)
		return 1;

	flag_type = uprov_cmdline_get_flag_type(cmdline);
	switch (flag_type) {
		case UPROV_CMDLINE_FLAG_FILE_CONFIG:
			udo_log_print(UDO_LOG_INFO, "Passed --file and --config flags\n");
			break;

		case UPROV_CMDLINE_FLAG_FILE_RESIZE_LAST:
			udo_log_print(UDO_LOG_INFO, "Passed --file and --resize-last flags\n");
			break;

		case UPROV_CMDLINE_FLAG_FILE_CONFIG_SIZE:
			udo_log_print(UDO_LOG_INFO, "Passed --file, --config, and --size flags\n");
			break;

		default:
			udo_log_print(UDO_LOG_ERROR, "No flag combination found.\n");
			uprov_cmdline_destroy(cmdline);
			return 1;
	}

	uprov_cmdline_destroy(cmdline);

	return 0;
}
