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

#include <udo/udo.h>

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
 */
struct uprov_cmdline
{
	struct udo_log_error_struct err;
	uint8_t                     free;
	uint8_t                     empty_flags;
};

/****************************************
 * Start uprov_cmdline_create functions *
 ****************************************/

struct uprov_cmdline *
uprov_cmdline_create (struct uprov_cmdline *p_cmdline,
                      char UDO_UNUSED *argv[],
                      int UDO_UNUSED argc)
{
	struct uprov_cmdline *cmdline = p_cmdline;

	if (!cmdline) {
		cmdline = calloc(1, sizeof(struct uprov_cmdline));
		if (!cmdline) {
			udo_log_error("calloc: %s\n", strerror(errno));
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
