/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

_BGP_Atomic global_atomic;

DCMF_Configure_t A1D_Messager_info;
A1D_Process_info_t A1D_Process_info;

DCMF_Callback_t A1D_Nocallback;

pthread_t A1DI_CHT_pthread;

void *A1DI_CHT_advance_function(void * dummy)
{
    DCMF_CriticalSection_enter(0);
    while (1)
    {
        DCMF_Messager_advance(0);
        DCMF_CriticalSection_exit(0);
        A1DI_Wait_cycles(a1_settings.cht_pause_cycles);
        DCMF_CriticalSection_enter(0);        
    }
    DCMF_CriticalSection_exit(0);
}

int A1D_Initialize(int thread_level)
{

    int status = A1_SUCCESS;
    int count = 0;

    A1U_FUNC_ENTER();

    /* TODO: need a non-DCMF lock here to make this function thread-safe */
    /* TODO: need to set "A1 is alive" global variable */

    count = DCMF_Messager_initialize();
    A1U_WARNING(count == 0,"DCMF_Messager_initialize has been called more than once.");

    A1D_Nocallback.function = NULL;
    A1D_Nocallback.clientdata = NULL;

    status = A1DI_Read_parameters();
    A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Read_parameters returned with error \n");

    if(a1_settings.enable_cht) {
        A1D_Messager_info.thread_level = DCMF_THREAD_MULTIPLE;
        A1D_Messager_info.interrupts = DCMF_INTERRUPTS_OFF;
    } else if(!a1_settings.disable_interrupts) {
        A1D_Messager_info.thread_level = DCMF_THREAD_MULTIPLE;
        A1D_Messager_info.interrupts = DCMF_INTERRUPTS_ON;
    } else {
        switch(thread_level) 
        {
           case A1_THREAD_SINGLE:
                 thread_level = DCMF_THREAD_SINGLE; 
                 break;
           case A1_THREAD_FUNNELED:
                 thread_level = DCMF_THREAD_FUNNELED;
                 break;           
           case A1_THREAD_SERIALIZED:
                 thread_level = DCMF_THREAD_SERIALIZED;
                 break;
           case A1_THREAD_GENERAL:
                 thread_level = DCMF_THREAD_MULTIPLE;
                 break;      
           default: 
                 A1U_ERR_POP(A1_ERROR, 
                             "Unsupported thread level provided in Initialize \n");
                 break;
        }
        A1D_Messager_info.interrupts = DCMF_INTERRUPTS_OFF;
    }

    status = DCMF_Messager_configure(&A1D_Messager_info, &A1D_Messager_info);
    A1U_ERR_POP(status != DCMF_SUCCESS, "DCMF_Messager_configure returned with error \n");

    A1D_Process_info.my_rank = DCMF_Messager_rank();
    A1D_Process_info.num_ranks = DCMF_Messager_size();
    /* TODO: initialize node rank/size properly on BGP */
    A1D_Process_info.my_node = DCMF_Messager_rank();
    A1D_Process_info.num_nodes = DCMF_Messager_size();

    if (a1_settings.enable_cht)
    {
        status = pthread_create(&A1DI_CHT_pthread, NULL, &A1DI_CHT_advance_function, NULL);
        A1U_ERR_POP(status != 0, "pthread_create returned with error \n");
    }

    A1DI_CRITICAL_ENTER();

    status = A1DI_Control_xchange_initialize();
    A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Control_xchange_initialize returned with error \n");

    status = A1DI_Control_flushack_initialize();
    A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Control_flushack_initialize returned with error \n");

    status = A1DI_GlobalBarrier_initialize();
    A1U_ERR_POP(status != A1_SUCCESS,"A1DI_GlobalBarrier_initialize returned with error \n");

    status = A1DI_Put_initialize();
    A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Put_initialize returned with error \n");

    status = A1DI_Get_initialize();
    A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Get_initialize returned with error \n");

    status = A1DI_Putacc_initialize();
    A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Putacc_initialize returned with error \n");

    status = A1DI_Packed_puts_initialize();
    A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Packed_puts_initialize returned with error \n");

    status = A1DI_Packed_gets_initialize();
    A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Packed_gets_initialize returned with error \n");

    status = A1DI_Packed_gets_response_initialize();
    A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Packed_gets_initialize returned with error \n");

    status = A1DI_Packed_putaccs_initialize();
    A1U_ERR_POP(status!=A1_SUCCESS,"A1DI_Packed_putaccs_initialize returned with error \n");

    status = A1DI_Send_flush_initialize();
    A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Send_flush_initialize returned with error \n");

    status = A1DI_Put_flush_initialize();
    A1U_ERR_POP(status != A1_SUCCESS,"Put flush initialize returned with error \n");

    status = A1DI_Request_pool_initialize();
    A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Request_pool_initialize failed \n");

    status = A1DI_Handle_pool_initialize();
    A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Handle_pool_initialize failed \n");

    status = A1DI_Memregion_Global_initialize();
    A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Memregion_Global_initialize returned with error \n");

    /* FIXME: Need to do stuff here! */

  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

