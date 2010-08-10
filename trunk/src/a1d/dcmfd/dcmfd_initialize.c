/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Configure_t A1D_Messager_info;
A1D_Process_info_t A1D_Process_info;

A1D_Control_xchange_info_t A1D_Control_xchange_info;

A1D_Request_pool_t A1D_Request_pool;
uint32_t a1_request_pool_size;

DCMF_Callback_t A1D_Nocallback;

void A1DI_CHT_advance_function(void * dummy)
{
    DCMF_CriticalSection_enter(0);
    while (1)
    {
        DCMF_Messager_advance(0);
        DCMF_CriticalSection_cycle(0);
    }
    DCMF_CriticalSection_exit(0);
}

void A1DI_Generic_done(void *clientdata, DCMF_Error_t *error)
{
    --(*((uint32_t *) clientdata));
}

void A1DI_Free_done(void *clientdata, DCMF_Error_t *error)
{
    A1D_Buffer_info_t *buffer_info = (A1D_Buffer_info_t *) clientdata;
    free(buffer_info->buffer_ptr);
    free((void *) buffer_info);
}

void A1DI_Control_xchange_callback(void *clientdata,
                                   const DCMF_Control_t *info,
                                   size_t peer)
{
    memcpy((void *) ((size_t) A1D_Control_xchange_info.xchange_ptr
                   + (size_t)(peer * A1D_Control_xchange_info.xchange_size)),
           (void *) info,
           A1D_Control_xchange_info.xchange_size);

    --(*((uint32_t *) clientdata));
}

DCMF_Result A1DI_Control_xchange_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Control_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_CONTROL_PROTOCOL;
    conf.network = DCMF_DEFAULT_NETWORK;
    conf.cb_recv = A1DI_Control_xchange_callback;
    conf.cb_recv_clientdata = (void *) &A1D_Control_xchange_info.rcv_active;

    result = DCMF_Control_register(&A1D_Control_xchange_info.protocol, &conf);
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "Control xchange registartion returned with error %d \n",
                result);

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}

int A1DI_Request_pool_initialize()
{

    int result = A1_SUCCESS;
    int index, max_regions;
    A1D_Request_info_t *request;

    A1U_FUNC_ENTER();

    max_regions = (a1_request_pool_limit - a1_request_pool_initial)
            / a1_request_pool_increment + 1;
    result = posix_memalign((void **) &(A1D_Request_pool.region_ptr),
                            16,
                            sizeof(void *) * max_regions);
    A1U_ERR_POP(result != 0, "memory region list allocation failed \n");

    result = posix_memalign((void **) &request, 16, sizeof(A1D_Request_info_t)
            * a1_request_pool_initial);
    A1U_ERR_POP(result != 0, "memory allocation for request pool failed \n");
    a1_request_pool_size = a1_request_pool_initial;

    A1D_Request_pool.region_ptr[A1D_Request_pool.region_count]
            = (void *) request;
    A1D_Request_pool.region_count++;

    A1D_Request_pool.head = request;
    A1D_Request_pool.current = request;
    A1D_Request_pool.tail = &request[a1_request_pool_initial - 1];
    A1D_Request_pool.head->prev = NULL;
    A1D_Request_pool.tail->next = NULL;
    for (index = 1; index < a1_request_pool_initial; index++)
    {
        request[index - 1].next = &request[index];
        request[index].prev = &request[index - 1];
    }

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}

int A1DI_Request_pool_increment()
{

    int result = A1_SUCCESS;
    int index;
    A1D_Request_info_t *request;

    A1U_FUNC_ENTER();

    posix_memalign((void **) &request, 16, sizeof(A1D_Request_info_t)
            * a1_request_pool_increment);
    A1U_ERR_POP(result = !request,
                "memory allocation for request pool failed \n");
    a1_request_pool_size = a1_request_pool_size + a1_request_pool_increment;

    A1D_Request_pool.region_ptr[A1D_Request_pool.region_count]
            = (void *) request;
    A1D_Request_pool.region_count++;

    A1D_Request_pool.current = request;
    A1D_Request_pool.tail->next = request;
    request->prev = A1D_Request_pool.tail;
    A1D_Request_pool.tail = &request[a1_request_pool_increment - 1];
    A1D_Request_pool.tail->next = NULL;
    for (index = 1; index < a1_request_pool_increment; index++)
    {
        request[index - 1].next = &request[index];
        request[index].prev = &request[index - 1];
    }

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}

DCMF_Request_t* A1DI_Get_request()
{

    int index;
    A1D_Request_info_t *a1_request = NULL;

    A1U_FUNC_ENTER();

    if (!A1D_Request_pool.current)
    {
        if (a1_request_pool_size < a1_request_pool_limit)
        {
            A1DI_Request_pool_increment();
        }
        else
        {
            A1DI_Flush_all();
        }
    }
    a1_request = A1D_Request_pool.current;
    A1D_Request_pool.current = A1D_Request_pool.current->next;

  fn_exit: 
    A1U_FUNC_EXIT();
    return &(a1_request->request);

  fn_fail: 
    goto fn_exit;
}

void A1DI_Reset_request_pool()
{

    A1U_FUNC_ENTER();

    A1D_Request_pool.current = A1D_Request_pool.head;

  fn_exit: 
    A1U_FUNC_EXIT();
    return;

  fn_fail: 
    goto fn_exit;
}

int A1D_Initialize(int thread_level)
{

    DCMF_Result result = DCMF_SUCCESS;
    int count = 0;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    count = DCMF_Messager_initialize();
    A1U_WARNING(count == 0,"DCMF_Messager_initialize has been called more than once.");

    A1D_Messager_info.thread_level = thread_level;
    A1D_Messager_info.interrupts = DCMF_INTERRUPTS_OFF;

    A1D_Process_info.my_rank = DCMF_Messager_rank();
    A1D_Process_info.num_ranks = DCMF_Messager_size();
    /* TODO: initialize node rank/size properly on BGP */
    A1D_Process_info.my_node = DCMF_Messager_rank();
    A1D_Process_info.num_nodes = DCMF_Messager_size();

    result = DCMF_Messager_configure(&A1D_Messager_info, &A1D_Messager_info);
    A1U_ERR_POP(result != DCMF_SUCCESS, "DCMF_Messager_configure returned with error \n");

    A1DI_Read_parameters();

    if (a1_enable_cht)
    {
        result = pthread_create(&A1DI_CHT_pthread, NULL, &A1DI_CHT_advance_function, NULL);
        A1U_ERR_POP(result != 0, "pthread_create returned with error \n");
    }

    result = A1DI_Control_xchange_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,"A1DI_Control_xchange_initialize returned with error \n");

    result = A1DI_Control_flushack_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,"A1DI_Control_flushack_initialize returned with error \n");

    result = A1DI_GlobalBarrier_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,"A1DI_GlobalBarrier_initialize returned with error \n");

    result = A1DI_Put_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,"A1DI_Put_initialize returned with error \n");

    result = A1DI_Get_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,"A1DI_Get_initialize returned with error \n");

    result = A1DI_Putacc_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,"A1DI_Putacc_initialize returned with error \n");

    result = A1DI_Packed_puts_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,"A1DI_Packed_puts_initialize returned with error \n");

    result = A1DI_Packed_gets_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,"A1DI_Packed_gets_initialize returned with error \n");

    result = A1DI_Packed_putaccs_initialize();
    A1U_ERR_POP(result!=A1_SUCCESS,"A1DI_Packed_putaccs_initialize returned with error \n");

    result = A1DI_Send_flush_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,"A1DI_Send_flush_initialize returned with error \n");

    result = A1DI_Put_flush_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,"Put flush initialize returned with error \n");

    result = A1DI_Request_pool_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,"A1DI_Request_pool_initialize failed \n");

    result = A1DI_Memregion_Global_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,"A1DI_Memregion_Global_initialize returned with error \n");

    /* FIXME: Need to do stuff here! */

  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}

