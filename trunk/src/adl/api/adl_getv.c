/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1d.h"
#include "a1u.h"

/* This is here because the build system does not yet have the necessary
 * logic to set these options for each device. */
#define A1D_IMPLEMENTS_GETV

#if defined A1D_IMPLEMENTS_GETV

int A1_GetV(int target,
            A1_iov_t *iov_ar,
            int ar_len)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status = A1D_GetV(target,
                      iov_ar,
                      ar_len);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_GetV returned error\n");

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1_NbGetV(int target,
              A1_iov_t *iov_ar,
              int ar_len,
              A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status = A1D_NbGetV(target,
                        iov_ar,
                        ar_len,
                        a1_handle);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_NbGetV returned error\n");

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

#else

/* TODO: implement A1_GetV in terms of A1_Get for lazy port option */

#endif /* A1D_IMPLEMENTS_GETV */