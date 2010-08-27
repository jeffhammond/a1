/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1d.h"
#include "a1u.h"

/*
int A1_Rmw(int target, void* local_ptr, void* remote_ptr, A1_atomic_op_t op, int value)                    
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status = A1_Rmw(target, local_ptr, remote_ptr, op, value);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_Rmw returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}
*/
