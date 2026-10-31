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

#ifndef UPROV_CMDLINE_H
#define UPROV_CMDLINE_H

#include <udo/macros.h>

/*
 * Stores information about the uprov_cmdline context.
 */
struct uprov_cmdline;


/*
 * @brief Given flags or arguments from executable. Parse
 *        and create a struct uprov_cmdline context storing
 *        flags passed to executable.
 *
 * @param cmdline - May be NULL or a pointer to a struct uprov_cmdline.
 *                  If NULL memory will be allocated and return to
 *                  caller. If not NULL address passed will be used
 *                  to store the newly created struct uprov_cmdline
 *                  context.
 * @param argv    - Executable flags or arguments strings.
 * @param argc    - Amount of arguments passed to executable.
 *                  Array size of @argv.
 *
 * @returns
 *	on success: Pointer to a struct uprov_cmdline
 *	on failure: NULL
 */
UDO_API
struct uprov_cmdline *
uprov_cmdline_create (struct uprov_cmdline *cmdline,
                      char *argv[],
                      int argc);


/*
 * @brief Frees any allocated memory and closes FD's (if open) create after
 *        uprov_cmdline_create() call.
 *
 * @param device - Pointer to a valid struct uprov_cmdline.
 */
UDO_API
void
uprov_cmdline_destroy (struct uprov_cmdline *cmdline);


/*
 * @brief Returns size of the internal structure. So,
 *        if caller decides to allocate memory outside
 *        of API interface they know the exact amount
 *        of bytes.
 *
 * @returns
 *	on success: sizeof(struct uprov_cmdline)
 *	on failure: sizeof(struct uprov_cmdline)
 */
int
uprov_cmdline_get_sizeof (void);

#endif /* UPROV_CMDLINE_H */
