/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Protocol_t A1D_Generic_put_protocol;

DCMF_Result A1DI_Put_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Put_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_PUT_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    result = DCMF_Put_register(&A1D_Generic_put_protocol, &conf);
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "put registartion returned with error %d \n",
                result);

    A1D_Nocallback.function = NULL;
    A1D_Nocallback.clientdata = NULL;

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

int A1D_Put(int target, void* src, void* dst, int bytes)
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t request;
    DCMF_Callback_t done_callback, ack_callback;
    volatile int done_active, ack_active;
    size_t src_disp, dst_disp;

    A1U_FUNC_ENTER();

    /* TODO: don't we need logic to decide when to do immediate completion??? */
    if (a1_settings.enable_immediate_flush)
    {
        done_callback = A1D_Nocallback;
        done_active = 0;
        ack_callback.function = A1DI_Generic_done;
        ack_callback.clientdata = (void *) &ack_active;
        ack_active = 1;
    }
    else
    {
        done_callback.function = A1DI_Generic_done;
        done_callback.clientdata = (void *) &done_active;
        done_active = 1;
        ack_callback = A1D_Nocallback;
        ack_active = 0;
        A1D_Connection_put_active[target]++;
    }

    src_disp = (size_t) src - (size_t) A1D_Membase_global[A1D_Process_info.my_rank];
    dst_disp = (size_t) dst - (size_t) A1D_Membase_global[target];

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
    A1U_ERR_POP(result, "Put returned with an error \n");

    A1DI_Conditional_advance(done_active > 0 || ack_active > 0);

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}
