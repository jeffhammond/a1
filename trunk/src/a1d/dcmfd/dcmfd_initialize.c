/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Configure_t A1D_Messager_info;
A1D_Process_info_t A1D_Process_info;

DCMF_Callback_t A1D_Nocallback;

pthread_t A1DI_CHT_pthread;

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

int A1D_Initialize(int thread_level)
{

    DCMF_Result result = DCMF_SUCCESS;
    int count = 0;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    count = DCMF_Messager_initialize();
    A1U_WARNING(count == 0,"DCMF_Messager_initialize has been called more than once.");

    A1DI_Read_parameters();

    A1D_Messager_info.thread_level = DCMF_THREAD_MULTIPLE;
    if(a1_settings.enable_cht) {
        A1D_Messager_info.interrupts = DCMF_INTERRUPTS_OFF;
    } else if (!a1_settings.disable_interrupts) {
        A1D_Messager_info.interrupts = DCMF_INTERRUPTS_ON;
    }

    result = DCMF_Messager_configure(&A1D_Messager_info, &A1D_Messager_info);
    A1U_ERR_POP(result != DCMF_SUCCESS, "DCMF_Messager_configure returned with error \n");

    A1D_Process_info.my_rank = DCMF_Messager_rank();
    A1D_Process_info.num_ranks = DCMF_Messager_size();
    /* TODO: initialize node rank/size properly on BGP */
    A1D_Process_info.my_node = DCMF_Messager_rank();
    A1D_Process_info.num_nodes = DCMF_Messager_size();

    if (a1_settings.enable_cht)
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

