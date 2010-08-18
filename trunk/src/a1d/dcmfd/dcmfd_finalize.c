/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

int A1D_Finalize(void)
{
    int status = A1_SUCCESS;
    int count = 0;

    A1U_FUNC_ENTER();

    /* TODO: need to unset "A1 is alive" global variable */

    A1DI_CRITICAL_ENTER();

    A1DI_Request_pool_finalize();

    if (a1_settings.enable_cht)
    {
        status = pthread_cancel(A1DI_CHT_pthread);
    }

    count = DCMF_Messager_finalize();
    A1U_WARNING(count == 0,"DCMF_Messager_finalize has been called more than once.");

  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}
