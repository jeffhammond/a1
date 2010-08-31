/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1u.h"
#include "a1d.h"

int A1U_Put_memcpy(int target,
                   void* src,
                   void* dst,
                   int bytes)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    fn_exit: A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

int A1U_NbPut_memcpy(int target,
                     void* src,
                     void* dst,
                     int bytes,
                     A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    fn_exit: A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

int A1U_PutS_memcpy(int target,
                    int stride_level,
                    int *block_sizes,
                    void* source_ptr,
                    int *src_stride_ar,
                    void* target_ptr,
                    int *trg_stride_ar)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    fn_exit: A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

int A1U_NbPutS_memcpy(int target,
                      int stride_level,
                      int *block_sizes,
                      void* source_ptr,
                      int *src_stride_ar,
                      void* target_ptr,
                      int *trg_stride_ar,
                      A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    fn_exit: A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

int A1U_PutV_memcpy(int target,
                    A1_iov_t *iov_ar,
                    int ar_len)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    fn_exit: A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

int A1U_NbPutV_memcpy(int target,
                      A1_iov_t *iov_ar,
                      int ar_len,
                      A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    fn_exit: A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

