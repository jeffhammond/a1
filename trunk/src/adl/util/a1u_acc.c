/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1u.h"
#include "a1d.h"

int A1U_Acc_memcpy(void* source_ptr,
                   void* target_ptr,
                   int bytes,
                   A1_datatype_t a1_type,
                   void* scaling)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1D_Global_lock_acquire();

    A1D_Global_lock_release();

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1U_AccS_memcpy(int stride_level,
                    int *block_sizes,
                    void* source_ptr,
                    int *src_stride_ar,
                    void* target_ptr,
                    int *trg_stride_ar,
                    A1_datatype_t a1_type,
                    void* scaling)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1D_Global_lock_acquire();

    A1D_Global_lock_release();

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1U_AccV_memcpy(A1_iov_t *iov_ar,
                    int ar_len,
                    A1_datatype_t a1_type,
                    void* scaling)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1D_Global_lock_acquire();

    A1D_Global_lock_release();

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}
