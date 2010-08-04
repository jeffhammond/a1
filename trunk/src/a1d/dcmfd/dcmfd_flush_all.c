/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

int A1D_Flush_all()
{
    int result = DCMF_SUCCESS;
    int b,i,dst;
    DCMF_Request_t request[A1C_FLUSHALL_BATCH_SIZE];
    DCQuad msginfo;   
 
    A1U_FUNC_ENTER();

    if(enable_scalefree_flush) {
       return result;
    }

    int batches = A1D_Process_info.num_ranks / A1C_FLUSHALL_BATCH_SIZE;
    int batched = batches * A1C_FLUSHALL_BATCH_SIZE;
    int remainder = A1D_Process_info.num_ranks - batched;

    A1DI_CRITICAL_ENTER();

    for(b = 0; b < batches; b++) {
        A1D_Control_fenceack_info.rcv_active = A1C_FLUSHALL_BATCH_SIZE - 1;
        for(i = 0; i < A1C_FLUSHALL_BATCH_SIZE; i++) {
          dst = i + b * A1C_FLUSHALL_BATCH_SIZE;
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
        while(A1D_Control_fenceack_info.rcv_active>0) A1D_Advance();
    }
    if( remainder > 0 ) {
        A1D_Control_fenceack_info.rcv_active = remainder - 1;
        for(i = 0; i < remainder; i++) {
          dst = i + batched;
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
        while(A1D_Control_fenceack_info.rcv_active>0) A1D_Advance();
    }
    memset(A1D_Connection_active, 0, sizeof(uint32_t)*A1D_Process_info.num_ranks);

  fn_exit:
  A1DI_CRITICAL_EXIT();;
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}
