/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

int A1D_Flush(int proc)
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t request;
    DCQuad msginfo;
  
    A1U_FUNC_ENTER();

    DCMF_CriticalSection_enter (0);

    /* FIXME: Need to do stuff here! */
    A1D_Control_fenceack_info.rcv_active = 1; 
    result = DCMF_Send(&A1D_Send_fence_info.protocol,
                       &request,
                       A1D_Nocallback,
                       DCMF_SEQUENTIAL_CONSISTENCY,
                       proc,
                       0,
                       NULL,
                       &msginfo,
                       1);
    A1U_ERR_POP(result,"Send returned with an error \n");      
    while(A1D_Control_fenceack_info.rcv_active) A1DI_CRITICAL(DCMF_Messager_advance());           

  fn_exit:
    DCMF_CriticalSection_exit (0);
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}
