/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Protocol_t A1D_Rmw_protocol;

volatile int rmw_active;

volatile int rmw_value_response;

void A1DI_Rmw_callback(void *clientdata,
                       const DCMF_Control_t *info,
                       size_t peer)
{
    A1D_Rmw_pkt_t *rmw_pkt;
    A1D_Rmw_pkt_t rmw_response_pkt;
    int old_value;
    DCMF_Control_t cmsg;

    rmw_pkt = (A1D_Rmw_pkt_t *) info;

    /* If this is a response packet for 
     * an RMW operation */
    if(rmw_pkt->counter_ptr == NULL) 
    { 
       rmw_value_response = rmw_pkt->value;
       rmw_active--;
    } 
    else 
    {
       old_value = *((int *) (rmw_pkt->counter_ptr));

       if(rmw_pkt->op == A1_FETCH_AND_ADD) 
       {
           *((int *) (rmw_pkt->counter_ptr)) += rmw_pkt->value;
       } 
       else if(rmw_pkt->op == A1_SWAP)
       {
           *((int *) (rmw_pkt->counter_ptr)) = rmw_pkt->value; 
       }
       else
       {
           A1U_ERR_ABORT(A1_ERROR,
                  "Invalid optype received in A1DI_Rmw_callback \n");
       }

       rmw_response_pkt.counter_ptr = NULL;
       rmw_response_pkt.value = old_value;

       memcpy(&cmsg, &rmw_response_pkt, sizeof(A1D_Rmw_pkt_t));

       DCMF_Control(&A1D_Rmw_protocol,
                    DCMF_SEQUENTIAL_CONSISTENCY,
                    peer,
                    &cmsg);
    }

}

int A1DI_Rmw_initialize()
{
    int status = A1_SUCCESS;
    DCMF_Control_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_CONTROL_PROTOCOL;
    conf.network = DCMF_DEFAULT_NETWORK;
    conf.cb_recv = A1DI_Rmw_callback;
    conf.cb_recv_clientdata = NULL;

    status = DCMF_Control_register(&A1D_Rmw_protocol, &conf);
    A1U_ERR_POP(status != DCMF_SUCCESS,
                "RMW registartion returned with error %d \n",
                status);

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

/*********************************
        NEEDS REVISION
**********************************
int A1D_Rmw(int target,
            void* source_ptr_in,
            void* source_ptr_out,
            void* target_ptr,
            int bytes,
            A1_atomic_op_t op,
            A1_datatype_t a1_type)
{
    int status = A1_SUCCESS;
    int old_value;
    A1D_Rmw_pkt_t rmw_pkt;
    DCMF_Control_t cmsg;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();
 
    rmw_active++;

    rmw_pkt.counter_ptr = remote_ptr;
    rmw_pkt.value = value;
    rmw_pkt.op = op;

    memcpy(&cmsg, &rmw_pkt, sizeof(A1D_Rmw_pkt_t));

    status = DCMF_Control(&A1D_Rmw_protocol,
                          DCMF_SEQUENTIAL_CONSISTENCY,
                          target,
                          &cmsg);
    A1U_ERR_POP(status != DCMF_SUCCESS,
               "DCMF_Control failed in A1D_Rmw_counter\n");

    A1DI_Conditional_advance(rmw_active > 0);

    *((int *) local_ptr) = rmw_value_response;

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

*********************************/
