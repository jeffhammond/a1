/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Result A1DI_Memaddress_xchange(void **ptr) {

    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Control_t cmsg;
    int rank, bytes;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    /* TODO: Send can be used instead of control messages to take advantage of the TORUS network */

    A1D_Control_xchange_info.xchange_ptr = (void *) ptr;
    A1D_Control_xchange_info.xchange_size = sizeof(void *);
    A1D_Control_xchange_info.rcv_active += A1D_Process_info.num_ranks-1;

    A1DI_GlobalBarrier();

    memcpy((void *) &cmsg, (void *) &ptr[A1D_Process_info.my_rank], sizeof(void *)); 
    for(rank=0; rank<A1D_Process_info.num_ranks; rank++) {
        if(rank != A1D_Process_info.my_rank) {
            DCMF_Control(&A1D_Control_xchange_info.protocol, DCMF_SEQUENTIAL_CONSISTENCY,
                     rank, &cmsg);
        }
    }
    while(A1D_Control_xchange_info.rcv_active > 0) A1D_Advance();

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;

}

int A1D_Exchange_segments(A1_group_t* group, void **ptr, long bytes)
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Memregion_t *memregion;

    A1U_FUNC_ENTER();

    ptr[A1D_Process_info.my_rank] = malloc (bytes);
    A1U_ERR_POP(result = !ptr[A1D_Process_info.my_rank],"memregion allocation failed \n");    

    result = A1DI_Memaddress_xchange(ptr);
    A1U_ERR_POP(result,"memaddress exchange returned with error \n");       

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}
