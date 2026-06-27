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

#ifndef UPROV_VERSION_H
#define UPROV_VERSION_H

#include <udo/macros.h>

/*
 * @brief Returns version of library in C string format.
 *
 * @returns
 * 	Library version
 */
#define uprov_version_get() \
	UDO_STRINGIFY(UPROV_VERSION_MAJOR) "." \
	UDO_STRINGIFY(UPROV_VERSION_MINOR) "." \
	UDO_STRINGIFY(UPROV_VERSION_PATCH)


/*
 * @brief Returns library version major in integer format.
 *
 * @returns
 * 	Library version major
 */
#define uprov_version_get_major() UPROV_VERSION_MAJOR


/*
 * @brief Returns library version minor in integer format.
 *
 * @returns
 * 	Library version minor
 */
#define uprov_version_get_minor() UPROV_VERSION_MINOR


/*
 * @brief Returns library version patch in integer format.
 *
 * @returns
 * 	Library version patch
 */
#define uprov_version_get_patch() UPROV_VERSION_PATCH

#endif /* UPROV_VERSION_H */
