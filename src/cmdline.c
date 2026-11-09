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

#include <stdlib.h>
#include <string.h>
#include <stddef.h> /* For ptrdiff_t */
#include <errno.h>

#include <udo/log.h>

#include "cmdline.h"

#define DEV_FILE_PATH_MAX (1<<6)

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
 * @member flag_type   - Stores integer holding command line
 *                       flags that contain no arguments.
 * @member config_file - File path to configuration file.
 * @member part_file   - File path to file to partition.
 * @member size        - Size of a newly created file.
 */
struct uprov_cmdline
{
	struct udo_log_error_struct err;
	uint8_t                     free;
	uint32_t                    flag_type;
	char                        config_file[UDO_FILE_PATH_MAX];
	char                        part_file[UDO_FILE_PATH_MAX];
	char                        size[32];
};

/****************************************
 * Start uprov_cmdline_create functions *
 ****************************************/

static void
p_cmdline_help_msg (const char *bin_name)
{
	udo_log_print(UDO_LOG_WARNING, "Usage:\n");
	udo_log_print(UDO_LOG_INFO,    "\t%s <options>\n\n", bin_name);
	udo_log_print(UDO_LOG_WARNING, "Example:\n");
	udo_log_print(UDO_LOG_INFO,    "\t%s --file /dev/sda -c part-config.txt\n", bin_name);
	udo_log_print(UDO_LOG_INFO,    "\t%s --file some-part.img -c part-config.txt\n\n", bin_name);
	udo_log_print(UDO_LOG_WARNING, "Options:\n");
	udo_log_print(UDO_LOG_SUCCESS, "\t-c,--config <required>"); udo_log_print(UDO_LOG_INFO, "\tPartition configuration file.\n");
	udo_log_print(UDO_LOG_SUCCESS, "\t-f,--file <required>  "); udo_log_print(UDO_LOG_INFO, "\tFile to repartition.\n");
	udo_log_print(UDO_LOG_SUCCESS, "\t-r,--resize-last      "); udo_log_print(UDO_LOG_INFO, "\tResize a drive partition to extend\n");
	udo_log_print(UDO_LOG_SUCCESS, "\t                      "); udo_log_print(UDO_LOG_INFO, "\tit out to the end of drive.\n");
	udo_log_print(UDO_LOG_SUCCESS, "\t-s,--size <required>  "); udo_log_print(UDO_LOG_INFO, "\tSize of new file being created.\n");
	udo_log_print(UDO_LOG_SUCCESS, "\t                      "); udo_log_print(UDO_LOG_INFO, "\tFlag only used if new file being created.\n");
	udo_log_print(UDO_LOG_SUCCESS, "\t                      "); udo_log_print(UDO_LOG_INFO, "\t<number>G - for gigabytes\n");
	udo_log_print(UDO_LOG_SUCCESS, "\t                      "); udo_log_print(UDO_LOG_INFO, "\t<number>M - for megabytes\n");
	udo_log_print(UDO_LOG_SUCCESS, "\t                      "); udo_log_print(UDO_LOG_INFO, "\t<number>K - for kilobytes\n");
	udo_log_print(UDO_LOG_SUCCESS, "\t                      "); udo_log_print(UDO_LOG_INFO, "\t<number>  - for bytes\n");
	udo_log_print(UDO_LOG_SUCCESS, "\t-h,--help             "); udo_log_print(UDO_LOG_INFO, "\tSee this message.\n");
}


struct uprov_cmdline *
uprov_cmdline_create (struct uprov_cmdline *p_cmdline,
                      const char *argv[],
                      const int argc)
{
	int a;

	char *arg = NULL;

	ptrdiff_t ret = 0;

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

	for (a = 1; a < argc; a++) {
		switch (UDO_STRTOU_UTOC(argv[a], '=', &ret)) {
			case 144: /* -c */
			case 720: /* --config */
			{
				arg = (argv[a][ret] == '=') ? \
					((char *)&(argv[a][ret+1])) : \
					((char *)argv[++a]);

				strncpy(cmdline->config_file, arg, UDO_FILE_PATH_MAX-1);
				cmdline->flag_type |= UPROV_CMDLINE_FLAG_CONFIG;
			}
				break;

			case 147: /* -f */
			case 506: /* --file */
			{
				arg = (argv[a][ret] == '=') ? \
					((char *)&(argv[a][ret+1])) : \
					((char *)argv[++a]);

				strncpy(cmdline->part_file, arg, UDO_FILE_PATH_MAX-1);
				cmdline->flag_type |= UPROV_CMDLINE_FLAG_FILE;
			}
				break;

			case 159: /* -r */
			case 1229: /* --resize-last */
				cmdline->flag_type |= UPROV_CMDLINE_FLAG_RESIZE_LAST;
				break;

			case 160: /* -s */
			case 533: /* --size */
			{
				arg = (argv[a][ret] == '=') ? \
					((char *)&(argv[a][ret+1])) : \
					((char *)argv[++a]);

				strncpy(cmdline->size, arg, sizeof(cmdline->size)-1);
				cmdline->flag_type |= UPROV_CMDLINE_FLAG_SIZE;
			}
				break;

			case 149: /* -h */
			case 515: /* --help */
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


/*************************************
 * Start uprov_cmdline_get functions *
 *************************************/

uint32_t
uprov_cmdline_get_flag_type (struct uprov_cmdline *cmdline)
{
	if (!cmdline)
		return 0;

	return cmdline->flag_type;
}


const char *
uprov_cmdline_get_config_file (struct uprov_cmdline *cmdline)
{
	if (!cmdline || !(*cmdline->config_file))
		return NULL;

	return cmdline->config_file;
}


const char *
uprov_cmdline_get_part_file (struct uprov_cmdline *cmdline)
{
	if (!cmdline || !(*cmdline->part_file))
		return NULL;

	return cmdline->part_file;
}


const char *
uprov_cmdline_get_size (struct uprov_cmdline *cmdline)
{
	if (!cmdline || !(*cmdline->size))
		return NULL;

	return cmdline->size;
}

/***********************************
 * End uprov_cmdline_get functions *
 ***********************************/


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
