/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1d.h"
#include "a1u.h"

int A1_Put(int proc, void* src, void* dst, int bytes)
{
    int status = A1_SUCCESS;
    int my_rank = A1D_Process_id(A1_GROUP_WORLD);

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    if(proc == my_rank && a1_settings.network_bypass)
    {
       status = A1U_Put_memcpy(src, dst, bytes);
       A1U_ERR_POP(status != A1_SUCCESS, "A1U_Put_memcpy returned an error\n");
    }
    else
    {
        status = A1D_Put(proc, src, dst, bytes);
        A1U_ERR_POP(status != A1_SUCCESS, "A1D_Put returned an error\n");
    }

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1_NbPut(int proc, void* src, void* dst, int bytes, A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;
    int my_rank = A1D_Process_id(A1_GROUP_WORLD);

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    if(proc == my_rank && a1_settings.network_bypass)
    {
       status = A1U_Put_memcpy(src, dst, bytes);
       A1U_ERR_POP(status != A1_SUCCESS, "A1U_Put_memcpy returned an error\n");
    }
    else
    {
        status = A1D_NbPut(proc, src, dst, bytes, a1_handle);
        A1U_ERR_POP(status != A1_SUCCESS, "A1D_NbPut returned an error\n");
    }

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}
