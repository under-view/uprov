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

#ifndef UPROV_DEVICE_H
#define UPROV_DEVICE_H

#include <udo/macros.h>

/*
 * Stores information about the uprov_device context.
 */
struct uprov_device;


/*
 * @brief Given a file parse with libfdisk and populate
 *        it's partitions in a struct uprov_device context.
 *
 *        If empty file given function returns pointer to an
 *        unpopulate struct uprov_device. Caller will later
 *        need to make a call to uprov_device_resize_wholedisk(3)
 *        to create the disk.
 *
 * @param device - May be NULL or a pointer to a struct uprov_device.
 *                 If NULL memory will be allocated and return to
 *                 caller. If not NULL address passed will be used
 *                 to store the newly created struct uprov_device
 *                 context.
 * @param fname  - Pointer to string storing absolute path to file
 *                 to associate with a struct uprov_device context.
 *
 * @returns
 *	on success: Pointer to a struct uprov_device
 *	on failure: NULL
 */
UDO_API
struct uprov_device *
uprov_device_create (struct uprov_device *device,
                     const char *fname);


/*
 * @brief Repartition file with caller defined partition information.
 *
 *        1 - START(SECTOR)
 *        2 - SIZE(SECTORS)
 *        3 - PARTITION_TYPE_CODE
 *        4 - FILE_SYSTEM_TYPE
 *        5 - FILE_SYSTEM_LABEL (Space character unsupported)
 *
 *        Acceptable format:
 *
 *            PTABLE: mbr
 *            PART: 1:2:3:4:5:
 *            PART: 1:2:3:4:5:
 *            PART: 1:2:3:4:5:
 *
 *            PTABLE: gpt
 *            PART: 1:2:3:4:5:
 *            PART: 1:2:3:4:5:
 *            PART: 1:2:3:4:5:
 *
 * @param device      - May be NULL or a pointer to a struct uprov_device.
 *                      If NULL memory will be allocated and return to
 *                      caller. If not NULL address passed will be used
 *                      to store the newly created struct uprov_device
 *                      context.
 * @param part_string - String used to determine how to repartition disk.
 *
 * @returns
 *	on success: 0
 *	on failure: -1
 */
UDO_API
int
uprov_device_resize_wholedisk (struct uprov_device *device,
                               const char *part_string);


/*
 * @brief Returns absolute path to file in string format.
 *
 * @param device - Pointer to a valid struct uprov_device.
 *
 * @returns
 *	on success: Absolute path to file
 *	on failure: NULL
 */
UDO_API
const char *
uprov_device_get_fname (struct uprov_device *device);


/*
 * @brief Returns file descriptor associated
 *        with open file passed to create
 *        function.
 *
 * @param device - Pointer to a valid struct uprov_device.
 *
 * @returns
 *	on success: Open file descriptor
 *	on failure: -1
 */
UDO_API
int
uprov_device_get_fname_fd (struct uprov_device *device);


/*
 * @brief Returns partition table type in string format.
 *
 * @param device - Pointer to a valid struct uprov_device.
 *
 * @returns
 *	on success: Partition table type
 *	on failure: NULL
 */
UDO_API
const char *
uprov_device_get_ptable_type (struct uprov_device *device);


/*
 * @brief Returns byte size of each sector
 *        in caller defined file.
 *
 *        Typically 512 bytes per sector.
 *
 * @param device - Pointer to a valid struct uprov_device.
 *
 * @returns
 *	on success: Byte size of each sector
 *	on failure: (uint16_t)-1 or UINT16_MAX
 */
UDO_API
uint16_t
uprov_device_get_sector_sz (struct uprov_device *device);


/*
 * @brief Returns number of partitions associated with context.
 *
 * @param device - Pointer to a valid struct uprov_device.
 *
 * @returns
 *	on success: Amount of partitions in context
 *	on failure: (size_t)-1
 */
UDO_API
size_t
uprov_device_get_part_count (struct uprov_device *device);


/*
 * @brief Returns partition number.
 *        It's generally @part_index + 1.
 *
 * @param device     - Pointer to a valid struct uprov_device.
 * @param part_index - Must pass valid partition index value.
 *
 * @returns
 * 	on success: Partition number
 * 	on failure: (size_t)-1
 */
UDO_API
size_t
uprov_device_get_part_num (struct uprov_device *device,
                           const size_t part_index);


/*
 * @brief Returns a block devices logical partition name.
 *        If file not a block device NULL is return with
 *        error properly set.
 *
 * @param device     - Pointer to a valid struct uprov_device.
 * @param part_index - Must pass valid partition index value.
 *
 * @returns
 * 	on success: Logical partition name
 * 	on failure: NULL
 */
UDO_API
const char *
uprov_device_get_part_name (struct uprov_device *device,
                            const size_t part_index);


/*
 * @brief Returns a given partitions start sector.
 *
 * @param device     - Pointer to a valid struct uprov_device.
 * @param part_index - Must pass valid partition index value.
 *
 * @returns
 * 	on success: Partition start sector
 * 	on failure: (uint64_t)-1 or UINT64_MAX
 */
UDO_API
uint64_t
uprov_device_get_part_start_sector (struct uprov_device *device,
                                    const size_t part_index);


/*
 * @brief Returns a given partitions end sector.
 *
 * @param device     - Pointer to a valid struct uprov_device.
 * @param part_index - Must pass valid partition index value.
 *
 * @returns
 * 	on success: Partition end sector
 * 	on failure: (uint64_t)-1 or UINT64_MAX
 */
UDO_API
uint64_t
uprov_device_get_part_end_sector (struct uprov_device *device,
                                  const size_t part_index);


/*
 * @brief Returns amount of sectors a partition has available.
 *
 * @param device     - Pointer to a valid struct uprov_device.
 * @param part_index - Must pass valid partition index value.
 *
 * @returns
 * 	on success: Partition sector count
 * 	on failure: (uint64_t)-1 or UINT64_MAX
 */
UDO_API
uint64_t
uprov_device_get_part_sector_size (struct uprov_device *device,
                                   const size_t part_index);


/*
 * @brief Returns a boolean value determining
 *        if the partition is logical or not.
 *
 *        1 if logical
 *        0 if not logical
 *
 * @param device     - Pointer to a valid struct uprov_device.
 * @param part_index - Must pass valid partition index value.
 *
 * @returns
 * 	on success: Boolean value 1 if logical partition 0 if not
 * 	on failure: (uint8_t)-1 or UINT8_MAX
 */
UDO_API
uint8_t
uprov_device_get_part_logical (struct uprov_device *device,
                               const size_t part_index);


/*
 * @brief Returns a boolean value determining
 *        if the partition is extended or not.
 *
 *        1 if extended
 *        0 if not extended
 *
 * @param device     - Pointer to a valid struct uprov_device.
 * @param part_index - Must pass valid partition index value.
 *
 * @returns
 * 	on success: Boolean value 1 if extended partition 0 if not
 * 	on failure: (uint8_t)-1 or UINT8_MAX
 */
UDO_API
uint8_t
uprov_device_get_part_extended (struct uprov_device *device,
                               const size_t part_index);


/*
 * @brief Returns the partitions file
 *        system type in string format.
 *
 * @param device     - Pointer to a valid struct uprov_device.
 * @param part_index - Must pass valid partition index value.
 *
 * @returns
 * 	on success: File system type in string format
 * 	on failure: NULL
 */
UDO_API
const char *
uprov_device_get_part_fstype (struct uprov_device *device,
                              const size_t part_index);


/*
 * @brief Returns the partitions file
 *        system label in string format.
 *
 * @param device     - Pointer to a valid struct uprov_device.
 * @param part_index - Must pass valid partition index value.
 *
 * @returns
 * 	on success: File system type in string format
 * 	on failure: NULL
 */
UDO_API
const char *
uprov_device_get_part_fslabel (struct uprov_device *device,
                               const size_t part_index);


/*
 * @brief Returns the partitions PARTLABEL
 *        in string format. Only used if
 *        the disk is GPT.
 *
 * @param device     - Pointer to a valid struct uprov_device.
 * @param part_index - Must pass valid partition index value.
 *
 * @returns
 * 	on success: GPT PARTLABEL in string format
 * 	on failure: NULL
 */
UDO_API
const char *
uprov_device_get_part_partlabel (struct uprov_device *device,
                                 const size_t part_index);


/*
 * @brief Returns type code given to a partition
 *        in unsigned integer format. Used for both
 *        MBR and GPT disk.
 *
 * @param device     - Pointer to a valid struct uprov_device.
 * @param part_index - Must pass valid partition index value.
 *
 * @returns
 * 	on success: Partition type code in integer format
 * 	on failure: (uint32_t)-1 or UINT32_MAX
 */
UDO_API
uint32_t
uprov_device_get_part_type_code (struct uprov_device *device,
                                 const size_t part_index);


/*
 * @brief Returns the partitions type code
 *        in string format. Only for both
 *        MBR and GPT disk.
 *
 * @param device     - Pointer to a valid struct uprov_device.
 * @param part_index - Must pass valid partition index value.
 *
 * @returns
 * 	on success: Partition type code in string format
 * 	on failure: NULL
 */
UDO_API
const char *
uprov_device_get_part_type_code_string (struct uprov_device *device,
                                        const size_t part_index);


/*
 * @brief Frees any allocated memory and closes FD's (if open) create after
 *        uprov_device_create() call.
 *
 * @param device - Pointer to a valid struct uprov_device.
 */
UDO_API
void
uprov_device_destroy (struct uprov_device *device);


/*
 * @brief Zero's out memory storing data related
 *        to the current partition table.
 *
 * @param device - Pointer to a valid struct uprov_device.
 */
UDO_API
void
uprov_device_destroy_parts (struct uprov_device *device);


/*
 * @brief Returns size of the internal structure. So,
 *        if caller decides to allocate memory outside
 *        of API interface they know the exact amount
 *        of bytes.
 *
 * @returns
 *	on success: sizeof(struct uprov_device)
 *	on failure: sizeof(struct uprov_device)
 */
int
uprov_device_get_sizeof (void);

#endif /* UPROV_DEVICE_H */
