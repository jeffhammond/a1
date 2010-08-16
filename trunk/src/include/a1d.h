/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"

#if !defined A1D_H_INCLUDED
#define A1D_H_INCLUDED

/**
 * \brief Device level implementation of A1_Initialize.
 *
 * \param[out] rc               The error code from initalizing A1
 * \param[in]  A1_thread_level  The type of thread support for A1
 *
 * \ingroup MANAGEMENT
 */
int A1D_Initialize(int A1_thread_level);

/**
 * \brief Device level implementation of A1_Finalize.
 *
 * \param[out] rc  The error code from terminating A1.  
 *
 * \ingroup MANAGEMENT
 */
int A1D_Finalize(void);

/**
 * \brief Device level implementation of A1_Exchange_segments.
 *
 *  A collective operation to allocate memory to be used in context of A1 copy operations.
 *
 * \param[out] rc         The error code.
 * \param[in]  group      Group of processes within which the pointer list is exchanged.
 * \param[in]  ptr        Pointer array. Each one points to memory allocated at one process, 
 *                        in order of ranks.
 * \param[in]  bytes      The size of memory allocated at each process.
 *
 * \ingroup MEMORY 
 */
int A1D_Exchange_segments(A1_group_t* group, void **ptr, long bytes);

/**
 * \brief Device level implementation of A1_Release_segments.
 *
 * A collective operation to invalidate and de-register memory segments
 * associated with an A1D_Exchange_segments call. 
 *
 * \param[out] rc          The error code.
 * \param[in]  group       Group of processes within which the pointer list was exchanged.
 * \param[in]  ptr         Pointer to the allocated memory.
 *
 * \ingroup MEMORY 
 */
int A1D_Release_segments(A1_group_t* group, void *ptr);

/**
 * \brief Device level implementation of A1_Barrier_group.
 *
 * On return, this call ensures that all processes within the entire group
 * have reached this point in the program.
 *
 * \param[in] group          Group of processes to synchronize.
 *
 * \ingroup  SYNCHRONIZATION
 */
int A1D_Barrier_group(A1_group_t* group);

/**
 * \brief Device level implementation of A1_Sync_group.
 *
 * On return, this call ensures that all processes within the entire group
 * have reached this point in the program and that all messages have completed remotely.
 *
 * \param[in] group          Group of processes to synchronize.
 *
 * \ingroup  SYNCHRONIZATION
 */
int A1D_Sync_group(A1_group_t* group);

/**
 * \brief Device level implementation of A1_Put.
 *
 * Blocking copy of contiguous data from local memory to remote memory.
 *
 * \param[out] rc            The error code.
 * \param[in]  target        Rank of the remote process.
 * \param[in]  source_ptr    Starting address in the (local) source memory.
 * \param[in]  target_ptr    Starting address in the (remote) target memory.
 * \param[in]  bytes         Amount of data to transfer, in bytes. 
 *
 * \ingroup  COPY OPERATIONS
 */
int A1D_Put(int target, void* src, void* dst, int bytes);

/**
 * \brief Device level implementation of A1_NbPut.
 *
 * Non-Blocking copy of contiguous data from local memory to remote memory.
 *
 * \param[out] rc            The error code.
 * \param[in]  target        Rank of the remote process.
 * \param[in]  source_ptr    Starting address in the (local) source memory.
 * \param[in]  target_ptr    Starting address in the (remote) target memory.
 * \param[in]  bytes         Amount of data to transfer, in bytes.
 * \param[in]  handle        Opaque A1 handle for request
 *
 * \ingroup  COPY OPERATIONS
 */
int A1D_NbPut(int target, void* src, void* dst, int bytes, A1_handle_t* handle);

/**
 * \brief Device level implementation of A1_PutS.
 *
 * Blocking copy of non-contiguous (strided) data from local memory to remote memory.
 *
 * \param[out] rc              The error code.
 * \param[in]  target          Rank of the remote process.
 * \param[in]  stride_level   The number of levels of stride.
 * \param[in]  block_sizes     Block size in each dimension, in bytes.
 * \param[in]  source_ptr      Starting address in the (local) source memory.
 * \param[in]  src_stride_ar   Array of stride distances at source, in bytes.
 * \param[in]  target_ptr      Starting address in the (remote) target memory.
 * \param[in]  trg_stride_ar   Array of stride distances at target, in bytes.
 *
 * \ingroup COPY OPERATIONS
 */
int A1D_PutS(int target,
             int stride_level,
             int block_sizes[],
             void* source_ptr,
             int src_stride_ar[],
             void* target_ptr,
             int trg_stride_ar[]);

/**
 * \brief Device level implementation of A1_Get.
 *
 * Blocking copy of contiguous data from remote memory to local memory.
 *
 * \param[out] rc            The error code.
 * \param[in]  target        Rank of the remote process.
 * \param[in]  source_ptr    Starting address in the (remote) source memory.
 * \param[in]  target_ptr    Starting address in the (local) target memory.
 * \param[in]  bytes         Amount of data to transfer, in bytes.
 *
 * \ingroup  COPY OPERATIONS
 */
int A1D_Get(int target, void* src, void* dst, int bytes);

/**
 * \brief Device level implementation of A1_Get.
 *
 * Non-Blocking copy of contiguous data from remote memory to local memory.
 *
 * \param[out] rc            The error code.
 * \param[in]  target        Rank of the remote process.
 * \param[in]  source_ptr    Starting address in the (remote) source memory.
 * \param[in]  target_ptr    Starting address in the (local) target memory.
 * \param[in]  bytes         Amount of data to transfer, in bytes.
 * \param[in]  handle        Opaque A1 handle for request
 *
 * \ingroup  COPY OPERATIONS
 */
int A1D_NbGet(int target, void* src, void* dst, int bytes, A1_handle_t* handle);

/**
 * \brief Device level implementation of A1_PutS.
 *
 * Blocking copy of non-contiguous (strided) data from remote memory to local memory.
 *
 * \param[out] rc              The error code.
 * \param[in]  target          Rank of the remote process.
 * \param[in]  stride_level   The number of levels of stride.
 * \param[in]  block_sizes     Block size in each dimension, in bytes.
 * \param[in]  source_ptr      Starting address in the (remote) source memory.
 * \param[in]  src_stride_ar   Array of stride distances at source, in bytes.
 * \param[in]  target_ptr      Starting address in the (local) target memory.
 * \param[in]  trg_stride_ar   Array of stride distances at target, in bytes.
 *
 * \ingroup COPY OPERATIONS
 */
int A1D_GetS(int target,
             int stride_level,
             int block_sizes[],
             void* source_ptr,
             int src_stride_ar[],
             void* target_ptr,
             int trg_stride_ar[]);

/**
 * \brief Device level implementation of A1_PutAcc
 *
 * Blocking accumulate of contiguous data from local memory onto remote memory.
 *
 * \param[out] rc            The error code.
 * \param[in]  target        Rank of the remote process.
 * \param[in]  source_ptr    Starting address in the (local) source memory.
 * \param[in]  target_ptr    Starting address in the (remote) target memory.
 * \param[in]  bytes         Amount of data to transfer, in bytes.
 * \param[in]  a1_type       Amount of data to transfer, in bytes.
 * \param[in]  scaling       Factor for scaling source
 *
 * \ingroup COPY OPERATIONS
 */
int A1D_PutAcc(int target,
               void* source_ptr,
               void* target_ptr,
               int bytes,
               A1_datatype_t a1_type,
               void* scaling);

/**
 * \brief Device level implementation of A1_NbPutAcc
 * 
 * Non-Blocking accumulate of contiguous data from local memory onto remote memory.
 * 
 * \param[out] rc            The error code.
 * \param[in]  target        Rank of the remote process.
 * \param[in]  source_ptr    Starting address in the (local) source memory.
 * \param[in]  target_ptr    Starting address in the (remote) target memory.
 * \param[in]  bytes         Amount of data to transfer, in bytes.
 * \param[in]  a1_type       Amount of data to transfer, in bytes.
 * \param[in]  scaling       Factor for scaling source
 * \param[in]  handle        Opaque A1 handle
 *
 * \ingroup COPY OPERATIONS
 */
int A1D_NbPutAcc(int target,
                 void* source_ptr,
                 void* target_ptr,
                 int bytes,
                 A1_datatype_t a1_type,
                 void* scaling,
                 A1_handle_t* handle);

/**
 * \brief Device level implementation of A1_PutAccS 
 *
 * Blocking accumulate of non-contiguous (strided) data from local memory to remote memory.
 *
 * \param[out] rc              The error code.
 * \param[in]  target          Rank of the remote process.
 * \param[in]  stride_level   The number of levels of stride.
 * \param[in]  block_sizes     Block size in each dimension, in bytes.
 * \param[in]  source_ptr      Starting address in the (local) source memory.
 * \param[in]  src_stride_ar   Array of stride distances at source, in bytes.
 * \param[in]  target_ptr      Starting address in the (remote) target memory.
 * \param[in]  trg_stride_ar   Array of stride distances at target, in bytes.
 * \param[in]  a1_type         Amount of data to transfer, in bytes.
 * \param[in]  scaling         Factor for scaling source
 *
 * \ingroup COPY OPERATIONS
 */
int A1D_PutAccS(int target,
                int stride_level,
                int block_sizes[],
                void* source_ptr,
                int *src_stride_ar,
                void* target_ptr,
                int *trg_stride_ar,
                A1_datatype_t a1_type,
                void* scaling);

/**
 * \brief Device level implementation of A1_Flush 
 * 
 *  On return, this call ensure that all blocking put or accumulate operations
 *  issued to a particular process are complete remotely.
 *
 * \param[in]  proc          Rank of the remote process.
 *
 * \ingroup COMPLETION
 */
int A1D_Flush(int proc);

/**
 * \brief Device level implementation of A1_Flush_group
 *
 *  On return, this call ensure that all blocking put or accumulate operations
 *  issued to the group of processes are complete remotely.
 *
 * \param[in]  group          Group of the remote processs.
 *
 * \ingroup COMPLETION
 */
int A1D_Flush_group(A1_group_t *group);

/**
 * \brief Device level implementation of A1_Process_id 
 *
 * Returns process rank relative to the group base specified.
 *
 * \param[out] rc          Process id in the process group.
 * \param[in]  group       Process group.
 *
 * \ingroup INFORMATION
 */
int A1D_Process_id(A1_group_t* group);

/**
 * \brief Device level implementation of A1_Process_total 
 *
 * Returns the total number of processes in  the group base specified.
 * 
 * \param[out] rc          Total number of processes in the process group.
 * \param[in]  group       Process group.
 *
 * \ingroup INFORMATION
 */
int A1D_Process_total(A1_group_t* group);

/**
 * \brief Device level implementation of A1_Node_id
 *
 * Returns node rank relative to the group base specified.
 *
 * \param[out] rc          Node id in the process group.
 * \param[in]  group       Process group.
 *
 * \ingroup INFORMATION
 */
int A1D_Node_id(A1_group_t* group);

/**
 * \brief Device level implementation of A1_Node_total
 *
 * Returns total number of nodes in the group base specified.
 * 
 * \param[out] rc          Total number of nodes in the process group.
 * \param[in]  group       Process group.
 *
 * \ingroup INFORMATION
 */
int A1D_Node_total(A1_group_t* group);

/**
 * \brief Device level implementation of A1_Time_seconds 
 * 
 * Timer in units of seconds.
 *
 * \param[out] rc          Number of secs from an arbitrary time in the past.
 *
 * \ingroup INFORMATION
 */
double A1D_Time_seconds();

/**
 * \brief Device level implementation of A1_Time_cycles 
 *
 * Timer in units of cycles.
 *
 * \param[out] rc          Number of cycles from an arbitrary time in the past.
 *
 * \ingroup INFORMATION
 */
unsigned long long A1D_Time_cycles();

#endif /* A1D_H_INCLUDED */
