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

1. :c:struct:`uprov_device`

=========
Functions
=========

API Documentation
~~~~~~~~~~~~~~~~~

======================
uprov_device (private)
======================

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

=========================================================================================================================================

.. _UDO_FILE_PATH_MAX: https://libudo.readthedocs.io/en/latest/macros.html#c.UDO_FILE_PATH_MAX
