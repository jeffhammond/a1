/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1u.h"
#include "a1d.h"
#include "dcmfdimpl.h"

int A1D_AllFence()
{
    int status = DCMF_SUCCESS;
    int dst;
    DCMF_Request request[A1D_Process_info.num_ranks];
    DCQuad msginfo;   
 
    A1U_FUNC_ENTER();

    A1D_Control_fenceack_info_t.rcv_active = A1D_Process_info.num_ranks - 1;
    for(dst = 0; dst < A1D_Process_info.num_ranks; dst++) {
      if(dst != A1D_Process_info.my_rank) {
         result = DCMF_Send(&A1D_Send_fence_info.protocol,
                       &request[dst],
                       A1D_nocallback,
                       DCMF_SEQUENTIAL_CONSISTENCY,
                       dst,
                       0,
                       NULL,
                       &msginfo,
                       1);
      }
    }
    A1U_ERR_POP(result,"Send returned with an error \n");
    while(A1D_Control_fenceack_info_t.rcv_active) A1DI_CRITICAL(DCMF_Messager_advance());  

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}
