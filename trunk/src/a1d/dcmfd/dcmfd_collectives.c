/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Protocol_t A1D_GlobalBarrier_protocol;

DCMF_Result A1DI_GlobalBarrier_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_GlobalBarrier_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_GLOBALBARRIER_PROTOCOL;
    result = DCMF_GlobalBarrier_register(&A1D_GlobalBarrier_protocol,
                                         &conf);
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "global barrier registartion returned with error %d \n",
                result);

  fn_exit:

    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

int A1DI_GlobalBarrier()
{

    int result = DCMF_SUCCESS;
    DCMF_Request_t request;
    DCMF_Callback_t callback;
    volatile int active;

    A1U_FUNC_ENTER();

    active = 1;
    callback.function = A1DI_Generic_done;
    callback.clientdata = (void *) &active;

    result = DCMF_GlobalBarrier(&A1D_GlobalBarrier_protocol,
                                &request,
                                callback);
    A1U_ERR_ABORT(result, "DCMF_GlobalBarrier returned with an error");

    A1DI_Conditional_advance(active > 0);

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;

}

int A1D_Barrier_group(A1_group_t* group)
{
    int result = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if (group == A1_GROUP_WORLD || group == NULL)
    {
        result = A1DI_GlobalBarrier();
        A1U_ERR_ABORT(result != A1_SUCCESS, "DCMF_GlobalBarrier returned with an error");
        goto fn_exit;
    }
    else
    {
        A1U_ERR_POP(1, "A1D_Barrier_group not implemented for non-world groups!");
        goto fn_fail;
    }


  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;

}


int A1D_Sync_group(A1_group_t* group)
{

    int result = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if (group == A1_GROUP_WORLD || group == NULL)
    {
        result = A1DI_Flush_all();
        A1U_ERR_ABORT(result != A1_SUCCESS, "A1DI_Flush_all returned with an error");
        result = A1DI_GlobalBarrier();
        A1U_ERR_ABORT(result != A1_SUCCESS, "A1DI_GlobalBarrier returned with an error");
        goto fn_exit;
    }
    else
    {
        A1U_ERR_POP(1, "A1D_Sync_group not implemented for non-world groups!");
        goto fn_fail;
    }

  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;

}
