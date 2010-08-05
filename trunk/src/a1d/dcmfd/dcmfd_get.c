/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

int A1D_Get(int target, void* src, void* dst, int bytes)
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t request;
    DCMF_Callback_t callback;
    int active;
    unsigned src_disp, dst_disp;
 
    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    callback.function = A1DI_Generic_done;
    callback.clientdata = (void *) &active;

    src_disp = (size_t)src - (size_t)A1D_Membase_global[target];    
    dst_disp = (size_t)dst - (size_t)A1D_Membase_global[A1D_Process_info.my_rank];    
 
    active = 1;
    result = DCMF_Get(&A1D_Generic_get_protocol,
                      &request,
                      callback,
                      DCMF_SEQUENTIAL_CONSISTENCY,
                      target,  
                      bytes,
                      &A1D_Memregion_global[target],
                      &A1D_Memregion_global[A1D_Process_info.my_rank],
                      src_disp,
                      dst_disp);
    A1U_ERR_POP(result,"Get returned with an error \n");
    while (active>0) DCMF_Messager_advance(); 

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}
