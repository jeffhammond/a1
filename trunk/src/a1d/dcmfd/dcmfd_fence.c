/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1u.h"
#include "a1d.h"
#include "dcmfdimpl.h"

int A1D_Fence(int proc)
{
    DCMF_Result status = DCMF_SUCCESS;
    DCMF_Request request;
    DCQuad msginfo;
  
    A1U_FUNC_ENTER();

    /* FIXME: Need to do stuff here! */
    A1D_Control_fenceack_info_t.rcv_active = 1; 
    result = DCMF_Send(&A1D_Send_fence_info.protocol,
                       &request,
                       A1D_nocallback,
                       DCMF_SEQUENTIAL_CONSISTENCY,
                       proc,
                       0,
                       NULL,
                       &msginfo,
                       1);
    A1U_ERR_POP(result,"Send returned with an error \n");      
    while(A1D_Control_fenceack_info_t.rcv_active) A1DI_CRITICAL(DCMF_Messager_advance());           

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}
