#
# Copyright (C) 2010 by Argonne National Laboratory.
#     See COPYRIGHT in top-level directory.
#

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/src/include -I$(top_builddir)/src/include

lib_LTLIBRARIES = liba1.la
liba1_la_SOURCES =
liba1_la_LDFLAGS = -version-info $(liba1_abi_version)

if a1_device_dcmf
include src/a1d/dcmf/Makefile.mk
endif
