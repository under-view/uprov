.. default-domain:: C

version (Uprov Version)
=======================

Header: uprov/version.h

Table of contents (click to go)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

======
Macros
======

1. :c:macro:`uprov_version_get`
#. :c:macro:`uprov_version_get_major`
#. :c:macro:`uprov_version_get_minor`
#. :c:macro:`uprov_version_get_patch`

=====
Enums
=====

======
Unions
======

=======
Structs
=======

=========
Functions
=========

API Documentation
~~~~~~~~~~~~~~~~~

=================
uprov_version_get
=================

.. c:macro:: uprov_version_get

| Returns version of library in C string format.

	.. code-block::

		#define uprov_version_get() \
			UDO_STRINGIFY(UPROV_VERSION_MAJOR) "." \
			UDO_STRINGIFY(UPROV_VERSION_MINOR) "." \
			UDO_STRINGIFY(UPROV_VERSION_PATCH)

	Returns:
		| Library version

=======================
uprov_version_get_major
=======================

.. c:macro:: uprov_version_get_major

| Returns library version major in integer format.

	.. code-block::

		#define uprov_version_get_major() UPROV_VERSION_MAJOR

	Returns:
		| Library version major

=======================
uprov_version_get_minor
=======================

.. c:macro:: uprov_version_get_minor

| Returns library version minor in integer format.

	.. code-block::

		#define uprov_version_get_minor() UPROV_VERSION_MINOR

	Returns:
		| Library version minor

=======================
uprov_version_get_patch
=======================

.. c:macro:: uprov_version_get_patch

| Returns library version patch in integer format.

	.. code-block::

		#define uprov_version_get_patch() UPROV_VERSION_MINOR

	Returns:
		| Library version patch
