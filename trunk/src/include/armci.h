/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#if !defined ARMCI_H_INCLUDED
#define ARMCI_H_INCLUDED

typedef enum 
{
   ARMCI_ACC_INT,
   ARMCI_ACC_FLT,
   ARMCI_ACC_DBL,
   ARMCI_ACC_CPL,
   ARMCI_ACC_DCPL
} armci_datatype_t;

typedef enum
{
   ARMCI_FETCH_AND_ADD,
   ARMCI_FETCH_AND_ADD_LONG,
   ARMCI_SWAP,
   ARMCI_SWAP_LONG
} armci_rmw_op_t;

typedef struct {
   void **src_ptr_ar; 
   void **dst_ptr_ar; 
   int bytes;         
   int ptr_ar_len;    
} armci_giov_t;

typedef void* armci_giov_t;

typedef int armci_size_t;

int ARMCI_SET_AGGREGATE_HANDLE (armci_hdl_t* handle);

int ARMCI_UNSET_AGGREGATE_HANDLE (armci_hdl_t* handle);

int ARMCI_Init();

int ARMCI_Finalize();

void ARMCI_Error(char *message, 
                 int code);

int ARMCI_Malloc(void* ptr[], 
                 armci_size_t bytes);

void* ARMCI_Malloc_local(armci_size_t bytes);

int ARMCI_Free(void *address);

int ARMCI_Free_local(void *address);

int ARMCI_Put(void* src,  
              void* dst, 
              int bytes, 
              int proc);

int ARMCI_PutS(void* src_ptr, 
               int src_stride_ar[], 
               void* dst_ptr, 
               int dst_stride_ar[], 
               int count[], 
               int stride_levels, 
               int proc);

int ARMCI_NbPutS(void* src_ptr, 
                 int src_stride_ar[], 
                 void* dst_ptr, 
                 int dst_stride_ar[], 
                 int count[], 
                 int stride_levels, 
                 int proc,
                 armci_hdl_t* handle);

int ARMCI_PutV(armci_giov_t *dsrc_arr, 
               int arr_len, 
               int proc);

int ARMCI_Get(src, 
              dst, 
              bytes,
              proc);

int ARMCI_GetS(void *src_ptr c_ptr, 
               int src_stride_ar[], 
               void* dst_ptr,
               int dst_stride_ar[], 
               int count[], 
               int stride_levels, 
               int proc);

int ARMCI_NbGetS(void *src_ptr, 
                 int src_stride_ar[], 
                 void* dst_ptr, 
                 int dst_stride_ar[], 
                 int count[], 
                 int stride_levels, 
                 int proc, 
                 armci_hdl_t* handle);

int ARMCI_GetV(armci_giov_t *dsrc_arr,
               int arr_len,
               int proc);

int ARMCI_Acc(int datatype, 
              void *scale, 
              void* src, 
              void* dst, 
              int bytes, 
              int proc);

int ARMCI_AccS(int datatype, 
               void *scale, 
               void* src_ptr,
               int src_stride_ar[], 
               void* dst_ptr, 
               int dst_stride_ar[], 
               int count[],  
               int stride_levels, 
               int proc);

int ARMCI_NbAccS(int datatype,  
                 void *scale,
                 void* src_ptr,
                 int src_stride_ar[],
                 void* dst_ptr,
                 int dst_stride_ar[],
                 int count[],
                 int stride_levels,
                 int proc,
                 armci_hdl_t* handle);

int ARMCI_AccV(int datatype, 
               void *scale, 
               armci_giov_t *dsrc_arr, 
               int arr_len, 
               int proc);

int ARMCI_Rmw(int op, 
              void *ploc, 
              void *prem, 
              int value, 
              int proc);

int ARMCI_Wait(armci_hdl_t* handle);

int ARMCI_WaitAll();

int ARMCI_Test(armci_hdl_t* handle);

void ARMCI_Fence(int proc);

void ARMCI_AllFence();

int ARMCI_Barrier();
