/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

void A1DI_GlobalBarrier()
{

    int result = DCMF_SUCCESS;
    DCMF_Request_t request;
    DCMF_Callback_t callback;
    volatile int active;

    A1U_FUNC_ENTER();

    /* TODO: modifying global A1 state is not thread-safe, but here it is easier to use
     *        the DCMF lock instead of a separate call for an A1 lock */
    A1DI_CRITICAL_ENTER();

    active = 1;
    callback.function = A1DI_Generic_done;
    callback.clientdata = (void *) &active;

    result = DCMF_GlobalBarrier(&A1D_GlobalBarrier_protocol,
                                &request,
                                callback);
    A1DI_CRITICAL_EXIT();
    A1U_ERR_ABORT(result, "DCMF_GlobalBarrier returned with an error");

    while (active > 0) A1DI_Advance();

  fn_exit: 
    A1U_FUNC_EXIT();
    return;

  fn_fail: 
    goto fn_exit;

}

void A1D_Barrier_group(A1_group_t* group)
{
    A1U_FUNC_ENTER();

    if (group == A1_GROUP_WORLD || group == NULL)
    {
        A1DI_GlobalBarrier();
        goto fn_exit;
    }
    else
    {
        A1U_ERR_POP(1, "A1D_Barrier_group not implemented for non-world groups!");
        goto fn_fail;
    }


    fn_exit: A1U_FUNC_EXIT();
    return;

    fn_fail: goto fn_exit;

}


void A1D_Sync_group(A1_group_t* group)
{

    A1U_FUNC_ENTER();

    if (group == A1_GROUP_WORLD || group == NULL)
    {
        A1DI_Flush_all();
        A1DI_GlobalBarrier();
        goto fn_exit;
    }
    else
    {
        A1U_ERR_POP(1, "A1D_Sync_group not implemented for non-world groups!");
        goto fn_fail;
    }


    fn_exit: A1U_FUNC_EXIT();
    return;

    fn_fail: goto fn_exit;

}
