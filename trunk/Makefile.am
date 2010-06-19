#
# Copyright (c) 2009    Pavan Balaji, Argonne National Laboratory.
#
# See COPYRIGHT in top-level directory.
#

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -I$(top_srcdir)/src/include -I$(top_builddir)/src/include

lib_LIBRARIES = liba1.a
liba1_a_SOURCES =

if a1_device_dcmf
include src/a1d/dcmf/Makefile.mk
endif
