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

    for(i=0; i<A1D_Request_pool.region_count; i++) {
         free(A1D_Request_pool.region_ptr[i]);
    }
    free(A1D_Request_pool.region_ptr);

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}

int A1D_Finalize(void)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    A1DI_Request_pool_finalize();

    DCMF_Messager_finalize();     

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}
