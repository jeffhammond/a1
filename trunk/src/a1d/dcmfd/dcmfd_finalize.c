/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

void A1DI_Request_pool_finalize()
{
    int i;

    A1U_FUNC_ENTER();

    for (i = 0; i < A1D_Request_pool.region_count; i++)
    {
        free(A1D_Request_pool.region_ptr[i]);
    }
    free(A1D_Request_pool.region_ptr);

    fn_exit: A1U_FUNC_EXIT();
    return;

    fn_fail: goto fn_exit;
}

int A1D_Finalize(void)
{
    int result = A1_SUCCESS;
    int count = 0;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    A1DI_Request_pool_finalize();

    if (a1_enable_cht)
    {
        result = pthread_cancel(A1DI_CHT_pthread);
    }

    count = DCMF_Messager_finalize();
    A1U_WARNING(count == 0,"DCMF_Messager_finalize has been called more than once.");

    fn_exit: A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}
