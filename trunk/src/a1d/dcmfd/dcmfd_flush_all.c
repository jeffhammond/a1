/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

int A1D_Flush_all()
{
    int result = DCMF_SUCCESS;
    int dst;
    DCMF_Request_t request[A1D_Process_info.num_ranks];
    DCQuad msginfo;   
 
    A1U_FUNC_ENTER();

    if(enable_scalefree_flush) {
       return result;
    }

    DCMF_CriticalSection_enter (0);

    A1D_Control_fenceack_info.rcv_active = A1D_Process_info.num_ranks - 1;
    for(dst = 0; dst < A1D_Process_info.num_ranks; dst++) {
      if(dst != A1D_Process_info.my_rank && A1D_Connection_active[dst] > 0) {
         result = DCMF_Send(&A1D_Send_fence_info.protocol,
                       &request[dst],
                       A1D_Nocallback,
                       DCMF_SEQUENTIAL_CONSISTENCY,
                       dst,
                       0,
                       NULL,
                       &msginfo,
                       1);
      }
    }
    A1U_ERR_POP(result,"Send returned with an error \n");
    while(A1D_Control_fenceack_info.rcv_active>0) DCMF_Messager_advance(); 

    memset(A1D_Connection_active, 0, sizeof(uint32_t)*A1D_Process_info.num_ranks); 

  fn_exit:
    DCMF_CriticalSection_exit (0);
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}
