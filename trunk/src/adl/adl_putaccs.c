/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1d.h"
#include "a1u.h"

int A1_PutAccS(int target,
               int stride_level,
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

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status = A1D_PutAccS(target,
                         stride_level,
                         block_sizes,
                         source_ptr,
                         src_stride_ar,
                         target_ptr,
                         trg_stride_ar,
                         a1_type,
                         scaling);
    A1U_ERR_POP(status, "A1D_PutAccS returned error\n");

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1_NbPutAccS(int target,
                int stride_level,
                int *block_sizes,
                void* source_ptr,
                int *src_stride_ar,
                void* target_ptr,
                int *trg_stride_ar,
                A1_datatype_t a1_type,
                void* scaling,
                A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status = A1D_NbPutAccS(target,
                          stride_level,
                          block_sizes,
                          source_ptr,
                          src_stride_ar,
                          target_ptr,
                          trg_stride_ar,
                          a1_type,
                          scaling,
                          a1_handle);
    A1U_ERR_POP(status, "NbPutAccS returned error\n");

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}
