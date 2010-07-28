/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

int A1D_Put(int target, void* src, void* dst, int bytes)
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t request;
    DCMF_Callback_t callback;
    int active;
    unsigned src_disp, dst_disp;
 
    A1U_FUNC_ENTER();

    DCMF_CriticalSection_enter (0);

    callback.function = A1DI_Generic_callback;
    callback.clientdata = (void *) &active;

    src_disp = (size_t)src - (size_t)A1D_Membase_global[A1D_Process_info.my_rank];    
    dst_disp = (size_t)dst - (size_t)A1D_Membase_global[target];    
 
    active = 1;
    result = DCMF_Put(&A1D_Generic_put_protocol,
                      &request,
                      callback,
                      DCMF_SEQUENTIAL_CONSISTENCY,
                      target,  
                      bytes,
                      &A1D_Memregion_global[A1D_Process_info.my_rank],
                      &A1D_Memregion_global[target],
                      src_disp,
                      dst_disp,
                      A1D_Nocallback);
    A1U_ERR_POP(result,"Put returned with an error \n");
    while (active) DCMF_Messager_advance(); 

  fn_exit:
    DCMF_CriticalSection_exit (0);
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}
