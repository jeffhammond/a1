/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1u.h"
#include "a1d.h"
#include "dcmfdimpl.h"

DCMF_Configure_t A1D_Messager_info;
A1D_Thread_info_t A1D_Thread_info;
A1D_Process_info_t A1D_Process_info;
A1D_GlobalBarrier_info_t A1D_GlobablBarrier_info;
A1D_Put_info_t A1D_Put_info;
A1D_Get_info_t A1D_Get_info;
A1D_Control_info_t A1D_Control_info;
A1D_Memregion_t **A1D_Memregion_lists = NULL;
uint32_t *A1D_Memregion_count;

void A1DI_Generic_callback (void *clientdata, DCMF_Error_t *error) {
    --(*((uint32_t *) clientdata));
}

void A1DI_CtrlRcv_callback (void *clientdata, const DCMF_Control_t *info, size_t peer) {
    A1D_Memregion_t *newregion;

    newregion = (A1D_Memregion_t *) malloc (sizeof(A1D_Memregion_t));
    memcpy(&(newregion->mregion), info, sizeof(DCMF_Memregion_t));
    DCMF_Memregion_query(&(newregion->mregion), &(newregion->bytes), &(newregion->vaddress));
   
    A1D_Control_info.addressarray_ptr[peer] = newregion->vaddress;     
    A1DI_Insert_memregion(newregion, peer); 
 
    --(*((uint32_t *) clientdata));
}

DCMF_Result A1DI_Control_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Control_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_CONTROL_PROTOCOL;
    conf.network = DCMF_DEFAULT_NETWORK;
    conf.cb_recv = A1DI_CtrlRcv_callback;
    conf.cb_recv_clientdata = (void *) &A1D_Control_info.rcv_active;

    result = DCMF_Control_register(&A1D_Control_info.protocol, &conf);
    A1U_ERR_POP(result,"Control registartion returned with error %d \n",result);

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

DCMF_Result A1DI_GlobalBarrier_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_GlobalBarrier_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_GLOBALBARRIER_PROTOCOL;
    result = DCMF_GlobalBarrier_register(&A1D_GlobalBarrier_info.protocol, &conf);
    A1U_ERR_POP(result,"global barrier registartion returned with error %d \n",result);

    A1D_GlobalBarrier_info.callback.function = A1DI_Generic_callback;
    A1D_GlobalBarrier_info.callback.clientdata = (void *) &A1D_GlobalBarrier_info.active;

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

DCMF_Result A1DI_Put_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Put_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_PUT_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    result = DCMF_Put_register(&A1D_Put_info.protocol, &conf);
    A1U_ERR_POP(result,"put registartion returned with error %d \n",result);

    A1D_Put_info.done_callback.function = A1DI_Generic_callback;
    A1D_Put_info.done_callback.clientdata = (void *) &A1D_Put_info.done_active;

    A1D_Put_info.ack_callback.function = A1DI_Generic_callback;
    A1D_Put_info.ack_callback.clientdata = (void *) &A1D_Put_info.ack_active;

    no_callback.function = NULL;
    no_callback.clientdata = NULL;

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

DCMF_Result A1DI_Get_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Get_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_GET_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    result = DCMF_Get_register(&A1D_Get_info.protocol, &conf);
    A1U_ERR_POP(result,"get registartion returned with error %d \n",result);

    A1D_Get_info.callback.function = A1DI_Generic_callback;
    A1D_Get_info.callback.clientdata = (void *) &A1D_Get_info.active;

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

int A1DI_Memregion_initialize() {

    int result = A1_SUCCESS;
    int rank;

    A1U_FUNC_ENTER();

    A1D_Memregion_lists = (A1D_Memregion_t **) malloc (sizeof(A1D_Memregion_t *)*A1D_Process_info.num_ranks);
    A1U_ERR_POP(result = (!A1D_Memregion_lists),"memory allocation for memregion list failed \n");    
    for(rank=0; rank<A1D_Process_info.num_ranks; rank++) {
         A1D_Memregion_lists[rank] = NULL;
    }

    A1D_Memregion_count = (uint32_t *) malloc (sizeof(uint32_t)*A1D_Process_info.num_ranks); 
    A1U_ERR_POPANDSTMT(result = (!A1D_Memregion_count), goto countalloc_fail, "allocation of memory\
                   for memregion count failed \n");     

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  countalloc_fail:
    free(A1D_Memregion_lists);
  fn_fail:
    goto fn_exit;
}

int A1D_Initialize(int thread_level, int num_threads,
         int num_memtypes, a1_memtype_t memtypes[])
{
    DCMF_Result result = DCMF_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL(DCMF_Messager_initialize());

    A1D_Thread_info.thread_level = thread_level;
    A1D_Thread_info.num_threads  = num_threads;

    A1D_Messager_info.thread_level = thread_level;
    A1D_Messager_info.interrupts = DCMF_INTERRUPTS_OFF;

    A1D_Process_info.my_rank   = DCMF_Messger_rank();
    A1D_Process_info.num_ranks = DCMF_Messager_size();

    result = DCMF_Messager_configure(&A1D_Messager_info, &A1D_Messager_info);
    A1U_ERR_POP(result,"global barrier initialize returned with error \n");      

    result = A1DI_Control_initialize();
    A1U_ERR_POP(result,"control initialize returned with error \n");

    result = A1DI_GlobalBarrier_initialize();
    A1U_ERR_POP(result,"global barrier initialize returned with error \n");

    result = A1DI_Put_initialize(); 
    A1U_ERR_POP(result,"Put initialize returned with error \n");

    result = A1DI_Get_initialize();
    A1U_ERR_POP(result,"Get initialize returned with error \n"); 

    result = A1DI_Memregion_initialize();
    A1U_ERR_POP(result,"Memregion list initialize returned with error \n");    

    /* FIXME: Need to do stuff here! */

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

