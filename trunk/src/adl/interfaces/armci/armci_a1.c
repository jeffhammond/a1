/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "armci.h"
#include "a1.h"
#include "a1d.h"
#include "a1u.h"

int ARMCI_Init()
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

     status = A1_Initialize(A1_THREAD_SINGLE);
     A1U_ERR_POP(status != A1_SUCCESS, "A1_Initialize returned an error\n"); 

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;

} 

int ARMCI_Finalize()
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

     status = A1_Finalize();
     A1U_ERR_POP(status != A1_SUCCESS, "A1D_Finalize returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;

}

int ARMCI_Malloc(void* ptr[],
                 armci_size_t bytes)
{
    int status = A1_SUCCESS;
    int my_rank;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status = A1_Alloc_segment(&ptr[my_rank], bytes);
    A1U_ERR_POP(status != A1_SUCCESS, "A1_Alloc_segment returned an error\n");

    status = A1_Exchange_segments(A1_GROUP_WORLD, ptr);
    A1U_ERR_POP(status != A1_SUCCESS, "A1_Exchange_segments returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;

}

void* ARMCI_Malloc_local(armci_size_t bytes)
{
    int status = A1_SUCCESS;
    void *segment_ptr;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

     status = A1_Alloc_segment(&segment_ptr, bytes);
     A1U_ERR_ABORT(status != A1_SUCCESS, "A1_Alloc_segement returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return segment_ptr;

  fn_fail:
    goto fn_exit;
}

int ARMCI_Free(void *ptr)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

     status = A1_Release_segments(A1_GROUP_WORLD, ptr);
     A1U_ERR_POP(status != A1_SUCCESS, "A1_Release_segments returned an error\n");

     status = A1_Free_segment(ptr);
     A1U_ERR_POP(status != A1_SUCCESS, "A1_Free_segment returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int ARMCI_Free_local(void *ptr)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

     status = A1_Free_segment(ptr);
     A1U_ERR_POP(status != A1_SUCCESS, "A1_Free_segment returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

void ARMCI_INIT_HANDLE(armci_hdl_t* handle)
{
    int status = A1_SUCCESS;
    A1_handle_t *a1_handle;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status = A1_Allocate_handle(a1_handle);
    A1U_ERR_ABORT(status != A1_SUCCESS, "A1_Allocate_handle returned an error\n");

    *handle = *a1_handle;

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}

int ARMCI_Put(void* src,
              void* dst,
              int bytes,
              int proc)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

     status = A1_Put(proc, src, dst, bytes);
     A1U_ERR_POP(status != A1_SUCCESS, "A1_Put returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int ARMCI_PutS(void* src_ptr,
               int src_stride_ar[],
               void* dst_ptr,
               int dst_stride_ar[],
               int count[],
               int stride_levels,
               int proc)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

     status = A1_PutS(proc,
                      stride_levels,
                      count,
                      src_ptr,
                      src_stride_ar,
                      dst_ptr,
                      dst_stride_ar);
     A1U_ERR_POP(status != A1_SUCCESS, "A1_PutS returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int ARMCI_NbPutS(void* src_ptr,
                 int src_stride_ar[],
                 void* dst_ptr,
                 int dst_stride_ar[],
                 int count[],
                 int stride_levels,
                 int proc,
                 armci_hdl_t* handle)
{
    int status = A1_SUCCESS;
    A1_handle_t a1_handle;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    a1_handle = (A1_handle_t) *handle;

    status = A1_NbPutS(proc,
                      stride_levels,
                      count,
                      src_ptr,
                      src_stride_ar,
                      dst_ptr,
                      dst_stride_ar,
                      a1_handle);
     A1U_ERR_POP(status != A1_SUCCESS, "A1_NbPutS returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int ARMCI_PutV(armci_giov_t *dsrc_arr,
               int arr_len,
               int proc)
{
    int status = A1_SUCCESS;
    A1_iov_t *a1_iov_ar;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    /* ARMCI iov and A1 iov are similar structures but follow 
     * different naming conventions. So we make a copy.*/
    posix_memalign((void **) &a1_iov_ar, 16, sizeof(a1_iov_ar)*arr_len);
    memcpy((void *) a1_iov_ar, (void *) dsrc_arr, sizeof(a1_iov_ar)*arr_len); 

    status =  A1_PutV(proc,
                      a1_iov_ar,
                      arr_len);
    A1U_ERR_POP(status != A1_SUCCESS, "A1_PutV returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int ARMCI_Get(void* src,
              void* dst,
              int bytes,
              int proc)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

     status = A1_Get(proc, src, dst, bytes);
     A1U_ERR_POP(status != A1_SUCCESS, "A1_Get returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int ARMCI_GetS(void* src_ptr,
               int src_stride_ar[],
               void* dst_ptr,
               int dst_stride_ar[],
               int count[],
               int stride_levels,
               int proc)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

     status = A1_GetS(proc,
                      stride_levels,
                      count,
                      src_ptr,
                      src_stride_ar,
                      dst_ptr,
                      dst_stride_ar);
     A1U_ERR_POP(status != A1_SUCCESS, "A1_GetS returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int ARMCI_NbGetS(void* src_ptr,
                 int src_stride_ar[],
                 void* dst_ptr,
                 int dst_stride_ar[],
                 int count[],
                 int stride_levels,
                 int proc,
                 armci_hdl_t* handle)
{
    int status = A1_SUCCESS;
    A1_handle_t a1_handle;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    a1_handle = (A1_handle_t) *handle;

    status = A1_NbGetS(proc,
                      stride_levels,
                      count,
                      src_ptr,
                      src_stride_ar,
                      dst_ptr,
                      dst_stride_ar,
                      a1_handle);
     A1U_ERR_POP(status != A1_SUCCESS, "A1_NbPutS returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int ARMCI_GetV(armci_giov_t *dsrc_arr,
               int arr_len,
               int proc)
{
    int status = A1_SUCCESS;
    A1_iov_t *a1_iov_ar;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    /* ARMCI iov and A1 iov are similar structures but follow
     * different naming conventions. So we make a copy.*/
    posix_memalign((void **) &a1_iov_ar, 16, sizeof(a1_iov_ar)*arr_len);
    memcpy((void *) a1_iov_ar, (void *) dsrc_arr, sizeof(a1_iov_ar)*arr_len);            

    status =  A1_GetV(proc,
                      a1_iov_ar,
                      arr_len);
    A1U_ERR_POP(status != A1_SUCCESS, "A1_GetV returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}



int ARMCI_Acc(int datatype,
              void *scale,
              void* src,
              void* dst,
              int bytes,
              int proc)
{
    int status = A1_SUCCESS;
    A1_datatype_t a1_type;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif
  
    if(datatype == ARMCI_ACC_INT)
    {
        a1_type = A1_INT32; 
    }
    else if(datatype == ARMCI_ACC_FLT)
    {
        a1_type = A1_FLOAT;
    } 
    else if(datatype == ARMCI_ACC_DBL)
    {
        a1_type = A1_DOUBLE;
    }
    else 
    {
        A1U_ERR_POP(status != A1_ERROR, 
               "Unsupported datatype\n"); 
    }
     
    status = A1_PutAcc(proc,
                        src,
                        dst,
                        bytes,
                        a1_type,
                        scale);
    A1U_ERR_POP(status != A1_SUCCESS, "A1_PutAcc returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int ARMCI_AccS(int datatype,
               void *scale,
               void* src_ptr,
               int src_stride_ar[],
               void* dst_ptr, 
               int dst_stride_ar[],
               int count[],
               int stride_levels,
               int proc)
{
    int status = A1_SUCCESS;
    A1_datatype_t a1_type;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif
  
    if(datatype == ARMCI_ACC_INT)
    {
        a1_type = A1_INT32; 
    }
    else if(datatype == ARMCI_ACC_FLT)
    {
        a1_type = A1_FLOAT;
    } 
    else if(datatype == ARMCI_ACC_DBL)
    {
        a1_type = A1_DOUBLE;
    }
    else 
    {
        A1U_ERR_POP(status != A1_ERROR, 
               "Unsupported datatype\n"); 
    }
     
    status = A1_PutAccS(proc,
                        stride_levels,
                        count,
                        src_ptr,
                        src_stride_ar,
                        dst_ptr,
                        dst_stride_ar,
                        a1_type,
                        scale);
    A1U_ERR_POP(status != A1_SUCCESS, "A1_PutAccS returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int ARMCI_NbAccS(int datatype,
                 void *scale,
                 void* src_ptr,
                 int src_stride_ar[],
                 void* dst_ptr, 
                 int dst_stride_ar[],
                 int count[],
                 int stride_levels,
                 int proc,
                 armci_hdl_t* handle)
{
    int status = A1_SUCCESS;
    A1_datatype_t a1_type;
    A1_handle_t a1_handle;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    a1_handle = (A1_handle_t) *handle;
  
    if(datatype == ARMCI_ACC_INT)
    {
        a1_type = A1_INT32; 
    }
    else if(datatype == ARMCI_ACC_FLT)
    {
        a1_type = A1_FLOAT;
    } 
    else if(datatype == ARMCI_ACC_DBL)
    {
        a1_type = A1_DOUBLE;
    }
    else 
    {
        A1U_ERR_POP(status != A1_ERROR, 
               "Unsupported datatype\n"); 
    }
     
    status = A1_NbPutAccS(proc,
                          stride_levels,
                          count,
                          src_ptr,
                          src_stride_ar,
                          dst_ptr,
                          dst_stride_ar,
                          a1_type,
                          scale,
                          a1_handle);
    A1U_ERR_POP(status != A1_SUCCESS, "NbA1_PutAccS returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int ARMCI_AccV(int datatype,
               void *scale,
               armci_giov_t *dsrc_arr,
               int arr_len,
               int proc)
{
    int status = A1_SUCCESS;
    A1_iov_t *a1_iov_ar;
    A1_datatype_t a1_type;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    if(datatype == ARMCI_ACC_INT)
    {
        a1_type = A1_INT32;
    }
    else if(datatype == ARMCI_ACC_FLT)
    {
        a1_type = A1_FLOAT;
    }
    else if(datatype == ARMCI_ACC_DBL)
    {
        a1_type = A1_DOUBLE;
    }
    else
    {
        A1U_ERR_POP(status != A1_ERROR,
               "Unsupported datatype\n");
    }

    /* ARMCI iov and A1 iov are similar structures but follow
     * different naming conventions. So we make a copy.*/
    posix_memalign((void **) &a1_iov_ar, 16, sizeof(a1_iov_ar)*arr_len);
    memcpy((void *) a1_iov_ar, (void *) dsrc_arr, sizeof(a1_iov_ar)*arr_len);

    status =  A1_PutAccV(proc,
                         a1_iov_ar,
                         arr_len,
                         a1_type,
                         scale);
    A1U_ERR_POP(status != A1_SUCCESS, "A1_PutAccV returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}


int ARMCI_Rmw(int op,
              void *ploc,
              void *prem,
              int value,
              int proc)
{

    int status = A1_SUCCESS;
    A1_atomic_op_t a1_op;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    if(op == ARMCI_FETCH_AND_ADD)
    {
        a1_op = A1_FETCHANDADD;
    }
    else if(op == ARMCI_SWAP)
    { 
        a1_op = ARMCI_SWAP;    
    } 
    else
    {
        A1U_ERR_POP(status != A1_ERROR,
               "Unsupported rmw operations\n");
    }

    status =  A1_Rmw(proc,
                     ploc,
                     prem,
                     a1_op,
                     value);
    A1U_ERR_POP(status != A1_SUCCESS, "A1_Rmw returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int ARMCI_Wait(armci_hdl_t* handle)
{

    int status = A1_SUCCESS;
    A1_handle_t a1_handle;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    a1_handle = (A1_handle_t) *handle;

    status =  A1_Wait_handle(a1_handle);
    A1U_ERR_POP(status != A1_SUCCESS, "A1_Wait_handle returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}


int ARMCI_Test(armci_hdl_t* handle)
{

    int status = A1_SUCCESS;
    A1_handle_t a1_handle;
    int complete;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    a1_handle = (A1_handle_t) *handle;

    status = A1_Test_handle(a1_handle, &complete);
    A1U_ERR_POP(status != A1_SUCCESS, "A1_Test_handle returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return !complete;

  fn_fail:
    goto fn_exit;
}

int ARMCI_WaitAll()
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status =  A1_Wait_handle_all();
    A1U_ERR_POP(status != A1_SUCCESS, "A1_Wait_handle_all returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

void ARMCI_Fence(int proc)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status =  A1_Flush(proc);
    A1U_ERR_ABORT(status != A1_SUCCESS, "A1_Flush returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}

void ARMCI_AllFence()
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status =  A1_Flush_group(A1_GROUP_WORLD);
    A1U_ERR_ABORT(status != A1_SUCCESS, "A1_Flush_group returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}

int ARMCI_Barrier()
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status =  A1_Barrier_group(A1_GROUP_WORLD);
    A1U_ERR_POP(status != A1_SUCCESS, "A1_Barrier_group returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}
