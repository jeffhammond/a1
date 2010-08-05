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
    DCMF_Callback_t done_callback, ack_callback;
    int done_active, ack_active;
    size_t src_disp, dst_disp;
 
    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if(a1_enable_scalefree_flush) {
        done_callback  = A1D_Nocallback;
        done_active = 0; 
        ack_callback.function = A1DI_Generic_done; 
        ack_callback.clientdata = (void *) &ack_active;
        ack_active = 1;
    } else {
        done_callback.function = A1DI_Generic_done;
        done_callback.clientdata = (void *) &done_active;
        done_active = 1;
        ack_callback = A1D_Nocallback;
        ack_active = 0;
        A1D_Connection_put_active[target]++;
    }

    src_disp = (size_t)src - (size_t)A1D_Membase_global[A1D_Process_info.my_rank];    
    dst_disp = (size_t)dst - (size_t)A1D_Membase_global[target];  

    result = DCMF_Put(&A1D_Generic_put_protocol,
                      &request,
                      done_callback,
                      DCMF_SEQUENTIAL_CONSISTENCY,
                      target,  
                      bytes,
                      &A1D_Memregion_global[A1D_Process_info.my_rank],
                      &A1D_Memregion_global[target],
                      src_disp,
                      dst_disp,
                      ack_callback);
    A1U_ERR_POP(result,"Put returned with an error \n");
    while (done_active>0 || ack_active>0) A1D_Advance(); 

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}
