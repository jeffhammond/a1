/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#if !defined ARMCI_H_INCLUDED
#define ARMCI_H_INCLUDED

int ARMCI_Init();

int ARMCI_Finalize();

#define ARMCI_Put(src, dst, bytes, proc) \
        A1_Put(proc, src, dst, bytes)

#define ARMCI_PutS(src_ptr, src_stride_ar, dst_ptr, dst_stride_ar, \
                   count, stride_levels, proc) \
        A1_PutS(proc, stride_levels, count, src_ptr, src_stride_ar, \
                   dst_ptr, dst_stride_ar)                           

#define ARMCI_Get(src, dst, bytes, proc) \
        A1_Get(proc, src, dst, bytes)

#define ARMCI_GetS(src_ptr, src_stride_ar, dst_ptr, dst_stride_ar, \
                   count, stride_levels, proc) \
        A1_GetS(proc, stride_levels, count, src_ptr, src_stride_ar, \
                   dst_ptr, dst_stride_ar)  

#define ARMCI_Acc(datatype, scale, src, dst, \
                  bytes, proc) \
        A1_PutAcc(proc, src, dst, bytes, datatype \
                  scale) \

#define ARMCI_AccS(datatype, scale, src_ptr, src_stride_ar, dst_ptr, dst_stride_ar, \
                   count, stride_levels, proc) \
        A1_PutAccS(proc, stride_levels, count, src_ptr, src_stride_ar, \
                   dst_ptr, dst_stride_ar, datatype, scale)  

#define ARMCI_Barrier() \
        A1_Barrier_group(A1_GROUP_WORLD)

#endif /* ARMCI_H_INCLUDED */
