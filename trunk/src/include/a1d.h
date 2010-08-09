/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"

#if !defined A1D_H_INCLUDED
#define A1D_H_INCLUDED

int A1D_Initialize(int A1_thread_level);

int A1D_Finalize(void);

int A1D_Exchange_segments(A1_group_t* group, void **ptr, long bytes);

int A1D_Release_segments(A1_group_t* group, void *ptr);

void A1D_GroupBarrier(A1_group_t* group);

void A1D_GlobalBarrier();

int A1D_Put(int target, void* src, void* dst, int bytes);

int A1D_Get(int target, void* src, void* dst, int bytes);

int A1D_PutS(int target, void* source_ptr, int *src_stride_ar, void* target_ptr,
         int *trg_stride_ar, int *count, int stride_levels);

int A1D_GetS(int target, void* source_ptr, int *src_stride_ar, void* target_ptr,
         int *trg_stride_ar, int *count, int stride_levels);

int A1D_PutAcc(int target, void* source_ptr, void* target_ptr, int bytes,
                A1_datatype a1_type, void* scaling);

int A1D_Flush(int proc);

int A1D_Flush_all();

int A1D_Rank();

int A1D_Size();

double A1D_Time_seconds();

unsigned long long A1D_Time_cycles();

#endif /* A1D_H_INCLUDED */
