/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1u.h"
#include "a1d.h"
#include "dcmfdimpl.h"

int A1D_GlobalBarrier() 
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t request;

    A1U_FUNC_ENTER();

    A1D_GlobalBarrier_info.active=1;
    result = DCMF_GlobalBarrier(&A1D_GlobalBarrier_info.protocol,
                       &request,
                       A1D_GlobalBarrier_info.callback);
    A1U_ERR_POP(result,"Global Barrier failed \n"); 
    while(A1D_GlobalBarrier_info.active) A1DI_CRITICAL(DCMF_Messager_advance());   

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}
