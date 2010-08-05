#
# Copyright (C) 2010 by Argonne National Laboratory.
#     See COPYRIGHT in top-level directory.
#

liba1_la_SOURCES += $(top_srcdir)/src/a1d/mpi2rmad/mpi2rmad_param.c \
        $(top_srcdir)/src/a1d/mpi2rmad/mpi2rmad_initialize.c \
	$(top_srcdir)/src/a1d/mpi2rmad/mpi2rmad_finalize.c \
	$(top_srcdir)/src/a1d/mpi2rmad/mpi2rmad_malloc.c \
	$(top_srcdir)/src/a1d/mpi2rmad/mpi2rmad_free.c \
	$(top_srcdir)/src/a1d/mpi2rmad/mpi2rmad_flush.c \
	$(top_srcdir)/src/a1d/mpi2rmad/mpi2rmad_flush_all.c \
	$(top_srcdir)/src/a1d/mpi2rmad/mpi2rmad_put.c \
	$(top_srcdir)/src/a1d/mpi2rmad/mpi2rmad_get.c \
	$(top_srcdir)/src/a1d/mpi2rmad/mpi2rmad_collectives.c \
	$(top_srcdir)/src/a1d/mpi2rmad/mpi2rmad_misc.c 
