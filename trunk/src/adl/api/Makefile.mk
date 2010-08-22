#
# Copyright (C) 2010 by Argonne National Laboratory.
#     See COPYRIGHT in top-level directory.
#

liba1_la_SOURCES += $(top_srcdir)/src/adl/util/adl_initialize.c \
	$(top_srcdir)/src/adl/util/adl_finalize.c \
	$(top_srcdir)/src/adl/util/adl_malloc.c \
	$(top_srcdir)/src/adl/util/adl_free.c \
	$(top_srcdir)/src/adl/util/adl_flush.c \
	$(top_srcdir)/src/adl/util/adl_flush_group.c \
	$(top_srcdir)/src/adl/util/adl_put.c \
	$(top_srcdir)/src/adl/util/adl_puts.c \
	$(top_srcdir)/src/adl/util/adl_putv.c \
	$(top_srcdir)/src/adl/util/adl_get.c \
	$(top_srcdir)/src/adl/util/adl_gets.c \
	$(top_srcdir)/src/adl/util/adl_getv.c \
    $(top_srcdir)/src/adl/util/adl_putacc.c \
    $(top_srcdir)/src/adl/util/adl_putaccs.c \
    $(top_srcdir)/src/adl/util/adl_putaccv.c \
    $(top_srcdir)/src/adl/util/adl_wait.c \
	$(top_srcdir)/src/adl/util/adl_collectives.c \
	$(top_srcdir)/src/adl/util/adl_misc.c \
	$(top_srcdir)/src/adl/util/adl_handle.c
