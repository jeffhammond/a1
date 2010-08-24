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

     status = A1D_Initialize(A1_THREAD_SINGLE);
     A1U_ERR_POP(status != A1_SUCCESS, "A1D_Initialize returned an error\n"); 

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

     status = A1D_Finalize();
     A1U_ERR_POP(status != A1_SUCCESS, "A1D_Finalize returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;

}
