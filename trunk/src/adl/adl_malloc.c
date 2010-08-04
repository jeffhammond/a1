/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1d.h"
#include "a1u.h"

int A1_Exchange_segments(A1_group_t* group, void* ptr[], int bytes)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status = A1D_Exchange_segments(group, ptr, bytes);
    A1U_ERR_POP(status, "Exchange segments returned error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}
