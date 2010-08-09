/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

int A1DI_Send_flush(int proc) 
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t request;
    DCQuad msginfo;

    A1U_FUNC_ENTER();   

    /* FIXME: Need to do stuff here!  What stuff? */
    A1D_Control_flushack_info.rcv_active = 1;
    result = DCMF_Send(&A1D_Send_flush_info.protocol,
                       &request,
                       A1D_Nocallback,
                       DCMF_SEQUENTIAL_CONSISTENCY,
                       proc,
                       0,
                       NULL,
                       &msginfo,
                       1);
    A1U_ERR_POP(result,"Send returned with an error \n");
    while(A1D_Control_flushack_info.rcv_active > 0) A1DI_Advance();

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

int A1DI_Send_flush_local(int proc) 
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t request;
    DCMF_Callback_t callback;
    int active;
    DCQuad msginfo;

    A1U_FUNC_ENTER();   

    callback.function = A1DI_Generic_done;
    callback.clientdata = (void *) &active;

    /* As this is local flush, you just wait for local completion and return. *
       rcv_active can be decremented asynchronously in the callback */
    active = 1;
    A1D_Control_flushack_info.rcv_active += 1;
    result = DCMF_Send(&A1D_Send_flush_info.protocol,
                       &request,
                       callback,
                       DCMF_SEQUENTIAL_CONSISTENCY,
                       proc,
                       0,
                       NULL,
                       &msginfo,
                       1);
    A1U_ERR_POP(result,"Send returned with an error \n");
    while(active > 0) A1DI_Advance();

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

int A1DI_Put_flush(int proc)
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t request;
    DCMF_Callback_t callback;
    int active, src_disp, dst_disp;

    A1U_FUNC_ENTER();

    callback.function = A1DI_Generic_done;
    callback.clientdata = (void *) &active;

    src_disp = (size_t)(A1D_Put_Flushcounter_ptr[A1D_Process_info.my_rank]) - (size_t)A1D_Membase_global[A1D_Process_info.my_rank];
    dst_disp = (size_t)(A1D_Put_Flushcounter_ptr[proc]) - (size_t)A1D_Membase_global[proc] + 1;

    active = 1;
    result = DCMF_Put(&A1D_Generic_put_protocol,
                      &request,
                      A1D_Nocallback,
                      DCMF_SEQUENTIAL_CONSISTENCY,
                      proc,
                      1,
                      &A1D_Memregion_global[A1D_Process_info.my_rank],
                      &A1D_Memregion_global[proc],
                      src_disp,
                      dst_disp,
                      callback);                       
    A1U_ERR_POP(result,"Send returned with an error \n");
    while(active > 0) A1DI_Advance();

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

int A1DI_Put_flush_local(int proc)
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t request;
    DCMF_Callback_t callback;
    int active, src_disp, dst_disp;

    A1U_FUNC_ENTER();

    callback.function = A1DI_Generic_done;
    callback.clientdata = (void *) &active;

    src_disp = (size_t)(A1D_Put_Flushcounter_ptr[A1D_Process_info.my_rank]) - (size_t)A1D_Membase_global[A1D_Process_info.my_rank];
    dst_disp = (size_t)(A1D_Put_Flushcounter_ptr[proc]) - (size_t)A1D_Membase_global[proc] + 1;

    active = 1;
    result = DCMF_Put(&A1D_Generic_put_protocol,
                      &request,
                      callback,
                      DCMF_SEQUENTIAL_CONSISTENCY,
                      proc,
                      1,
                      &A1D_Memregion_global[A1D_Process_info.my_rank],
                      &A1D_Memregion_global[proc],
                      src_disp,
                      dst_disp,
                      A1D_Nocallback);                       
    A1U_ERR_POP(result,"Send returned with an error \n");
    while(active > 0) A1DI_Advance();

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

int A1D_Flush(int proc)
{
    DCMF_Result result = DCMF_SUCCESS;
  
    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if(a1_enable_immediate_flush) {
       return result;  
    }

    if(!A1D_Connection_send_active[proc] && !A1D_Connection_put_active[proc]) {
       return result;
    }

    if(A1D_Connection_send_active[proc]) {  
       result = A1DI_Send_flush(proc);
       A1U_ERR_POP(result,"Send returned with an error \n");      
    } else {
       result = A1DI_Put_flush(proc);
       A1U_ERR_POP(result,"Put returned with an error \n");
    }

    A1D_Connection_send_active[proc] = 0; 
    A1D_Connection_put_active[proc] = 0; 

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}
