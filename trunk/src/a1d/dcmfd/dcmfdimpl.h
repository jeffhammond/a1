/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include <dcmf.h>
#include <dcmf_globalcollectives.h>
#include <dcmf_collectives.h>

typedef struct 
{
   int thread_level;
   int num_threads;
} 
A1D_Thread_info_t; 

extern A1D_Thread_info_t A1D_Thread_info;
