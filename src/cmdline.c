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
#include <errno.h>
#include <getopt.h>

#include <udo/log.h>

#include "cmdline.h"

/*
 * @brief Structure defining storing information about the
 *        flag passed to the executable and the arguments
 *        of said flag.
 *
 * @member err         - Stores information about the error that occured
 *                       for the given context and may later be retrieved
 *                       by caller.
 * @member free        - If structure allocated with calloc(3) member will be
 *                       set to true so that, we know to call free(3) when
 *                       destroying the context.
 * @member empty_flags - Stores integer holding command line
 *                       flags that contain no arguments.
 * @member part_num    - Stores partition number to update.
 */
struct uprov_cmdline
{
	struct udo_log_error_struct err;
	uint8_t                     free;
	uint8_t                     empty_flags;
	size_t                      part_num;
};

/****************************************
 * Start uprov_cmdline_create functions *
 ****************************************/

UDO_STATIC_INLINE
struct option *
p_cmdline_get_opts (void)
{
	static struct option
	cmdline_opts[] = \
	{
		{"resize", required_argument, 0, 'r' },
		{"help", no_argument, 0, 'h' },
		{0, 0, 0, 0 }
	};

	return (struct option*)&(cmdline_opts);
}


UDO_STATIC_INLINE
char *
p_cmdline_get_opts_str (void)
{
	return "r:h";
}


static void
p_cmdline_help_msg (const char *bin_name)
{
	udo_log_print(UDO_LOG_WARNING, "Usage:\n");
	udo_log_print(UDO_LOG_INFO, "\t%s <options>\n\n", bin_name);
	udo_log_print(UDO_LOG_WARNING, "Example:\n");
	udo_log_print(UDO_LOG_INFO, "\t%s --resize 1\n\n", bin_name);
	udo_log_print(UDO_LOG_WARNING,    "Options:\n");
	udo_log_print(UDO_LOG_SUCCESS, "\t-r,--resize <partition number> "); udo_log_print(UDO_LOG_INFO, "Resize a drive partition to extend\n");
	udo_log_print(UDO_LOG_INFO,    "\t                               "); udo_log_print(UDO_LOG_INFO, "it out to the end of drive.\n");
	udo_log_print(UDO_LOG_SUCCESS, "\t-h,--help                      "); udo_log_print(UDO_LOG_INFO, "See this message.\n");
}


struct uprov_cmdline *
uprov_cmdline_create (struct uprov_cmdline *p_cmdline,
                      char *argv[],
                      int argc)
{
	int c, opt_index = 0;

	struct uprov_cmdline *cmdline = p_cmdline;

	/*
	 * If no argument passed. No point
	 * and executing rest of function.
	 */
	if (argc == 1) {
		p_cmdline_help_msg(argv[0]);
		return NULL;
	}

	if (!cmdline) {
		cmdline = calloc(1, sizeof(struct uprov_cmdline));
		if (!cmdline) {
			udo_log_error("calloc: %s\n", strerror(errno));
			return NULL;
		}
	}

	/*
	 * Allows us to control what's
	 * displayed upon invalid option.
	 */
	opterr = 0;

	while (1) {
		c = getopt_long(argc, argv, \
		                p_cmdline_get_opts_str(),  \
		                p_cmdline_get_opts(), \
		                &opt_index);
		if (c == -1)
			break;

		switch (c) {
			case 'r':
				cmdline->part_num = atoi(optarg);
				udo_log_print(UDO_LOG_WARNING, \
				              "Resizing partition %d\n", \
				              cmdline->part_num);
				break;

			case 'h':
			case '?':
			default:
				p_cmdline_help_msg(argv[0]);
				uprov_cmdline_destroy(cmdline);
				return NULL;
		}
	}

	return cmdline;
}

/**************************************
 * End uprov_cmdline_create functions *
 **************************************/


/*****************************************
 * Start uprov_cmdline_destroy functions *
 *****************************************/

void
uprov_cmdline_destroy (struct uprov_cmdline *cmdline)
{
	if (!cmdline)
		return;

	if (cmdline->free) {
		free(cmdline);
	} else {
		memset(cmdline, 0, sizeof(struct uprov_cmdline));
	}
}

/***************************************
 * End uprov_cmdline_destroy functions *
 ***************************************/


/*****************************************************
 * Start of non struct uprov_cmdline param functions *
 *****************************************************/

int
uprov_cmdline_get_sizeof (void)
{
	return sizeof(struct uprov_cmdline);
}

/***************************************************
 * End of non struct uprov_cmdline param functions *
 ***************************************************/
