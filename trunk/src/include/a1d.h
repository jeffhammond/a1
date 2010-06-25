/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"

#if !defined A1D_H_INCLUDED
#define A1D_H_INCLUDED

int A1D_Initialize(int A1_thread_level,
               int A1_num_threads,
               int A1_num_memtypes,
               a1_memtype_t A1_memtypes[]);

int A1D_Finalize(void);

#endif /* A1D_H_INCLUDED */
