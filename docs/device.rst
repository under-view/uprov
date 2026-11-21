.. default-domain:: C

device (Device)
===============

Header: uprov/device.h

Table of contents (click to go)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

======
Macros
======

=====
Enums
=====

======
Unions
======

=======
Structs
=======

1. :c:struct:`uprov_device_part`
#. :c:struct:`uprov_device`

=========
Functions
=========

1. :c:func:`uprov_device_create`
#. :c:func:`uprov_device_repart_wholedisk`
#. :c:func:`uprov_device_repart_part`
#. :c:func:`uprov_device_get_fname`

API Documentation
~~~~~~~~~~~~~~~~~

===========================
uprov_device_part (private)
===========================

| Structure defining a single partition entry.
| For a caller given partitioned file.

.. c:struct:: uprov_device_part

	.. c:member::
		size_t number;
		uint64_t start_sector;
		uint64_t end_sector;
		uint64_t sector_size;
		uint8_t logical;
		uint8_t extended;
		char fstype[FSTYPE_MAX];
		char fslabel[FSLABEL_MAX];
		char partlabel[PARTLABEL_MAX];

	.. c:union:: type

		.. c:member::
			uint32_t code;
			char code_str[TYPE_CODE_STR_MAX];

	:c:member:`number`
		| Partition number.

	:c:member:`start_sector`
		| The starting sector of a partition.

	:c:member:`end_sector`
		| The final sector of a partition.

	:c:member:`sector_size`
		| Amount of sectors a partition has.

	:c:member:`logical`
		| Boolean indicating if a partition
		| is logical or not. Used for MBR.
		| Always false in the GPT case.

	:c:member:`extended`
		| Boolean indicating if a partition
		| is extended or not. Used for MBR.
		| Always false in the GPT case.

	:c:member:`fstype`
		| File system type of a partition.

	:c:member:`partlabel`
		| GPT partition label of a partition.

	:c:member:`fslabel`
		| File system label of a partition.

	:c:member:`type`
		| Stores the partition type.

		:c:member:`code`
			| Stores partition type
			| (Linux, Linux extended, etc..)
			| in integer representation.
			| Used MBR based partition table types.

		:c:member:`code_str`
			| Store partition type in string format.
			| GUID partition table type in string format.
			| Used GPT based partition table types.
			| `GPT_fdisk#Partition_type`_

======================
uprov_device (private)
======================

| Structure storing everything required to partition a file.

.. c:struct:: uprov_device

	.. c:member::
		struct udo_log_error_struct err;
		uint8_t free;
		struct uprov_device_part parts[PARTITIONS_MAX];
		char fname[UDO_FILE_PATH_MAX];
		int fname_fd;
		char ptable_type[TABLE_TYPE_MAX];
		uint16_t sector_sz;
		size_t part_count;
		char part_name[PART_NAME_MAX];

	:c:member:`err`
		| Stores information about the error that occured
		| for the given context and may later be retrieved
		| by caller.

	:c:member:`free`
		| If structure allocated with `calloc(3)`_ member will be
		| set to true. So that the api knows to call `free(3)`_ when
		| destroying the context.

	:c:member:`parts`
		| Array of partitions for the given ``fname``.

	:c:member:`fname`
		| File name of file to repartition in string format.

	:c:member:`fname_fd`
		| ``fname`` open file descriptor.

	:c:member:`ptable_type`
		| Partition table type contained in ``fname``.

	:c:member:`sector_sz`
		| Byte size of each sector in ``fname``.
		| Typically ``512`` bytes per sector.

	:c:member:`part_count`
		| Size of ``parts`` array. Amount of partitions
		| associated with the context.

	:c:member:`part_name`
		| Used to temporarily acquire and store name
		| of a partition. Or the absolute path to
		| the block device partition.

=========================================================================================================================================

===================
uprov_device_create
===================

.. c:function:: struct uprov_device *uprov_device_create(struct uprov_device *device, const char *fname, const uint32_t ask_type);

| Given a file parse with libfdisk and populate
| it's partitions in a ``struct`` :c:struct:`uprov_device` context.
|
| If empty file given function returns pointer to an
| unpopulate ``struct`` :c:struct:`uprov_device`. Caller will later
| need to make a call to :c:func:`uprov_device_repart_wholedisk`
| to populate the ``struct`` :c:struct:`uprov_device` and create the disk.

	.. list-table::
		:header-rows: 1

		* - Param
		  - Decription
		* - device
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`uprov_device`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`uprov_device`
		    | context.
		* - fname
		  - | Pointer to string storing absolute path to file
		    | to associate with a ``struct`` :c:struct:`uprov_device` context.
		* - ask_type
		  - | Must pass an `enum fdisk_ask_type`_ enumerator.
		    | Used for libfdisk debug output.

	Returns:
		| **on success:** Pointer to a ``struct`` :c:struct:`uprov_device`
		| **on failure:** ``NULL``

=========================================================================================================================================

=============================
uprov_device_repart_wholedisk
=============================

.. c:function:: int uprov_device_repart_wholedisk(struct uprov_device *device, const char *part_string, const uint32_t ask_type);

| Repartition file with caller defined partition information.

	1. PARTITION_NUMBER
	#. START_SECTOR
	#. SIZE_SECTORS
	#. PARTITION_TYPE_CODE
	#. FILE_SYSTEM_TYPE
	#. FILE_SYSTEM_LABEL (Space character unsupported)

	| Acceptable format:

	.. code-block::

		PTABLE: mbr
		PART: 1:2:3:4:5:6:
		PART: 1:2:3:4:5:6:
		PART: 1:2:3:4:5:6:

		PTABLE: gpt
		PART: 1:2:3:4:5:6:
		PART: 1:2:3:4:5:6:
		PART: 1:2:3:4:5:6:

	.. list-table::
		:header-rows: 1

		* - Param
		  - Decription
		* - device
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`uprov_device`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`uprov_device`
		    | context.
		* - part_string
		  - | String used to determine how to repartition disk.
		* - ask_type
		  - | Must pass an `enum fdisk_ask_type`_ enumerator.
		    | Used for libfdisk debug output.

	Returns:
		| **on success:** 0
		| **on failure:** -1

=========================================================================================================================================

========================
uprov_device_repart_part
========================

.. c:function:: int uprov_device_repart_part(struct uprov_device *device, const char *part_string, const uint32_t ask_type);

| Repartition file with caller defined partition information.

	1. PARTITION_NUMBER
	#. START_SECTOR
	#. SIZE_SECTORS
	#. PARTITION_TYPE_CODE
	#. FILE_SYSTEM_TYPE
	#. FILE_SYSTEM_LABEL (Space character unsupported)

	| Acceptable format:

	.. code-block::

		PART: 1:2:3:4:5:6:

	.. list-table::
		:header-rows: 1

		* - Param
		  - Decription
		* - device
		  - | May be ``NULL`` or a pointer to a ``struct`` :c:struct:`uprov_device`.
		    | If ``NULL`` memory will be allocated and return to
		    | caller. If not ``NULL`` address passed will be used
		    | to store the newly created ``struct`` :c:struct:`uprov_device`
		    | context.
		* - part_string
		  - | String used to determine how to repartition disk.
		* - ask_type
		  - | Must pass an `enum fdisk_ask_type`_ enumerator.
		    | Used for libfdisk debug output.

	Returns:
		| **on success:** 0
		| **on failure:** -1

=========================================================================================================================================

======================
uprov_device_get_fname
======================

.. c:function:: const char *uprov_device_get_fname(struct uprov_device *device);

| Returns absolute path to file in string format.

	.. list-table::
		:header-rows: 1

		* - Param
		  - Decription
		* - device
		  - | Pointer to a valid struct uprov_device.

	Returns:
		| **on success:** Absolute path to partitioned file
		| **on failure:** ``NULL``

=========================================================================================================================================

.. _calloc(3): https://www.man7.org/linux/man-pages/man3/malloc.3.html
.. _free(3): https://www.man7.org/linux/man-pages/man3/free.3.html
.. _GPT_fdisk#Partition_type: https://wiki.archlinux.org/title/GPT_fdisk#Partition_type
.. _UDO_FILE_PATH_MAX: https://libudo.readthedocs.io/en/latest/macros.html#c.UDO_FILE_PATH_MAX
.. _enum fdisk_ask_type: https://github.com/util-linux/util-linux/blob/master/libfdisk/src/libfdisk.h.in
