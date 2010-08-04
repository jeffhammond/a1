/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

void A1DI_GlobalBarrier()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t request;

    A1U_FUNC_ENTER();

    DCMF_CriticalSection_enter (0);

    A1D_GlobalBarrier_info.active=1;
    result = DCMF_GlobalBarrier(&A1D_GlobalBarrier_info.protocol,
                       &request,
                       A1D_GlobalBarrier_info.callback);
    A1U_ERR_ABORT(result,"Global Barrier failed \n");
    while(A1D_GlobalBarrier_info.active) A1DI_CRITICAL(DCMF_Messager_advance());

  fn_exit:
    DCMF_CriticalSection_exit (0);
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}

void A1D_Barrier(A1_group_t* group) { 

    A1U_FUNC_ENTER();

    DCMF_CriticalSection_enter (0);

    if(group == A1_GROUP_WORLD) {
      A1DI_GlobalBarrier();                            
      goto fn_exit;
    }

    DCMF_CriticalSection_exit (0);                   

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;

}
