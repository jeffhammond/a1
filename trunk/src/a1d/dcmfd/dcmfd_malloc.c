/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Memregion_t *A1D_Memregion_global;
void **A1D_Membase_global;

DCMF_Protocol_t A1D_Counter_setup_protocol;
volatile int counter_setup_active;
volatile void* counter_ptr_response;

void A1DI_Counter_setup_callback(void *clientdata,
                                 const DCMF_Control_t *info,
                                 size_t peer)
{
    memcpy((void *) &counter_ptr_response, (void *) info, sizeof(void *));
    counter_setup_active--;
}    

int A1DI_Counter_setup_initialize()
{
    int status = A1_SUCCESS;
    DCMF_Control_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_CONTROL_PROTOCOL;
    conf.network = DCMF_DEFAULT_NETWORK;
    conf.cb_recv = A1DI_Counter_setup_callback;
    conf.cb_recv_clientdata = NULL;

    status = DCMF_Control_register(&A1D_Counter_setup_protocol, &conf);
    A1U_ERR_POP(status != DCMF_SUCCESS,
                "Counter setup registartion returned with error %d \n",
                status);

  fn_exit:
    A1U_FUNC_EXIT();
     return status;

  fn_fail:
    goto fn_exit;
}

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
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;

}

int A1D_Exchange_segments(A1_group_t* group, void **ptr, int bytes)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    status = A1DI_Malloc_aligned((void **) &ptr[A1D_Process_info.my_rank], bytes);
    A1U_ERR_POP(status != 0,
                "memregion allocation failed \n");

    status = A1DI_Memaddress_xchange(ptr);
    A1U_ERR_POP(status, "memaddress exchange returned with error \n");

  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1D_Alloc_segment(void** ptr, int bytes)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    status = A1DI_Malloc_aligned(ptr, bytes);
    A1U_ERR_POP(status != 0,
                "A1DI_Malloc_aligned returned error in A1D_Alloc_segment\n");

  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1D_Alloc_counter(A1_counter_t *counter)
{
    int index, status = A1_SUCCESS;
    DCMF_Control_t cmsg;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    /* TODO: Important. Currently we allocate shared counter at rank 0. 
     * We have to explore ways to make this hierarchical and remove the 
     * bottleneck   */
    if(A1D_Process_info.my_rank == 0) 
    {
        status = A1DI_Malloc_aligned(counter, sizeof(int));
        A1U_ERR_POP(status != 0,
                "A1DI_Malloc_aligned returned error in A1D_Alloc_counter\n");

        memcpy(&cmsg, counter, sizeof(void *));

        for(index=1; index<A1D_Process_info.num_ranks; index++)
        {
             status = DCMF_Control(&A1D_Counter_setup_protocol,
                                   DCMF_SEQUENTIAL_CONSISTENCY,
                                   index,
                                   &cmsg);
             A1U_ERR_POP(status != DCMF_SUCCESS,
                   "DCMF_Control failed in A1D_Alloc_counter\n");
        }
    }
    else
    {
        counter_setup_active++; 
 
        A1DI_Conditional_advance(counter_setup_active > 0); 
 
        *counter = (void *) counter_ptr_response; 
    }

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}
