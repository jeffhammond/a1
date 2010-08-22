/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1d.h"
#include "a1u.h"

int A1_PutAccV(int target, 
               A1_iov_t *iov_ar,
               int ar_len,
               A1_datatype_t a1_type,
               void* scaling)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status = A1D_PutAccV(target, 
                         iov_ar,
                         ar_len,
                         a1_type,
                         scaling); 
    A1U_ERR_POP(status, "A1D_PutAccV returned error\n");

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1_NbPutAccV(int target,
               A1_iov_t *iov_ar,
               int ar_len,
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

    status = A1D_NbPutAccV(target,    
                           iov_ar,
                           ar_len,
                           a1_type,
                           scaling,
                           a1_handle);      
    A1U_ERR_POP(status, "A1D_NbPutAccV returned error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}
