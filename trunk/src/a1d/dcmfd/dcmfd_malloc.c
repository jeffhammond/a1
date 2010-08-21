/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Memregion_t *A1D_Memregion_global;
void **A1D_Membase_global;

int A1DI_Memregion_Global_xchange()
{

    int status = A1_SUCCESS;
    DCMF_Control_t info;
    int rank;

    A1U_FUNC_ENTER();

    /*TODO: Use DCMF_Send operations instead to exploit TORUS network */

    A1D_Control_xchange_info.xchange_ptr = (void *) A1D_Memregion_global;
    A1D_Control_xchange_info.xchange_size = sizeof(DCMF_Memregion_t);
    A1D_Control_xchange_info.rcv_active += A1D_Process_info.num_ranks - 1;

    A1DI_GlobalBarrier();

    memcpy((void *) &info,
           (void *) &A1D_Memregion_global[A1D_Process_info.my_rank],
           sizeof(DCMF_Memregion_t));
    for (rank = 0; rank < A1D_Process_info.num_ranks; rank++)
    {
        if (rank != A1D_Process_info.my_rank)
        {
            status = DCMF_Control(&A1D_Control_xchange_info.protocol,
                                  DCMF_SEQUENTIAL_CONSISTENCY,
                                  rank,
                                  &info);
            A1U_ERR_POP(status != DCMF_SUCCESS,
                        "DCMF_Control failed in A1DI_Memregion_Global_xchange\n");
        }
    }
    A1DI_Conditional_advance(A1D_Control_xchange_info.rcv_active > 0);

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;

}

int A1DI_Memregion_Global_initialize()
{

    int status = A1_SUCCESS;
    unsigned int out, i;

    A1U_FUNC_ENTER();

    status = A1DI_Malloc_aligned((void **) &A1D_Memregion_global,
                            sizeof(DCMF_Memregion_t) * A1D_Process_info.num_ranks);
    A1U_ERR_POP(status != 0, "Memregion allocation Failed \n");

    status
            = DCMF_Memregion_create(&A1D_Memregion_global[A1D_Process_info.my_rank],
                                    &out,
                                    (size_t) - 1,
                                    NULL,
                                    0);
    A1U_ERR_POP(status != DCMF_SUCCESS, "Global Memory Registration Failed \n");

    status = A1DI_Memregion_Global_xchange();
    A1U_ERR_POP(status != A1_SUCCESS, "Memory Region Xchange Failed \n");

    status = A1DI_Malloc_aligned((void **) &A1D_Membase_global, 
                                 sizeof(void *)
                                      * A1D_Process_info.num_ranks);
    A1U_ERR_POP(status != 0, "Membase allocation Failed \n");

    for (i = 0; i < A1D_Process_info.num_ranks; i++)
    {
        status = DCMF_Memregion_query(&A1D_Memregion_global[i],
                                      &out,
                                      (void **) &A1D_Membase_global[i]);
        A1U_ERR_POP(status != DCMF_SUCCESS, "Memregion query failed \n");
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int A1DI_Memaddress_xchange(void **ptr)
{

    int status = A1_SUCCESS;
    DCMF_Control_t cmsg;
    int rank, bytes;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    /* TODO: Send can be used instead of control messages to take advantage of the TORUS network */
    A1D_Control_xchange_info.xchange_ptr = (void *) ptr;
    A1D_Control_xchange_info.xchange_size = sizeof(void *);
    A1D_Control_xchange_info.rcv_active += A1D_Process_info.num_ranks - 1;

    A1DI_GlobalBarrier();

    memcpy((void *) &cmsg,
           (void *) &ptr[A1D_Process_info.my_rank],
           sizeof(void *));

    for (rank = 0; rank < A1D_Process_info.num_ranks; rank++)
    {
        if (rank != A1D_Process_info.my_rank)
        {
            DCMF_Control(&A1D_Control_xchange_info.protocol,
                         DCMF_SEQUENTIAL_CONSISTENCY,
                         rank,
                         &cmsg);
        }
    }
    A1DI_Conditional_advance(A1D_Control_xchange_info.rcv_active > 0);

  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;

}

int A1D_Exchange_segments(A1_group_t* group, void **ptr, long bytes)
{
    int status = A1_SUCCESS;
    DCMF_Memregion_t *memregion;

    A1U_FUNC_ENTER();

    status = A1DI_Malloc_aligned((void **) &ptr[A1D_Process_info.my_rank], bytes);
    A1U_ERR_POP(status != 0,
                "memregion allocation failed \n");

    status = A1DI_Memaddress_xchange(ptr);
    A1U_ERR_POP(status, "memaddress exchange returned with error \n");

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}
