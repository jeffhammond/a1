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
A1D_Control_xchange_info_t A1D_Control_xchange_info;
A1D_Control_fenceack_info_t A1D_Control_fenceack_info;
A1D_Send_info_t A1D_Send_noncontigput_info;
A1D_Send_info_t A1D_Send_fence_info;
A1D_GlobalBarrier_info_t A1D_GlobablBarrier_info;
A1D_Request_pool_t A1D_Request_pool;

DCMF_Protocol_t A1D_Generic_put_protocol;
DCMF_Protocol_t A1D_Generic_get_protocol;
DCMF_Callback_t A1D_Nocallback;
DCMF_Memregion_t *A1D_Memregion_global;

void **A1D_Membase_global;

char* A1DI_Unpack_data(char *pointer, void *trg_ptr, int *trg_stride_ar,\
        int *count, int stride_level)
{
     int i, size;

     A1U_FUNC_ENTER();

     if(stride_level > 0) {
         for(i=0; i<count[stride_level]; i++)
         {
            pointer = A1DI_Unpack_data(pointer, trg_ptr + i*trg_stride_ar[stride_level], trg_stride_ar,\
                                count, stride_level-1);
         }
     } else {
         memcpy(trg_ptr, (void *)pointer, count[0]);
         pointer += count[0];
     }

  fn_exit:
     A1U_FUNC_EXIT();
     return pointer;

  fn_fail:
     goto fn_exit;
}

void A1DI_Unpack(char *packet)
{
     int i, size_data;
     char *data, *temp;
     A1D_Pack_header_t *header;

     A1U_FUNC_ENTER();

     header = (A1D_Pack_header_t *) packet;
     data = packet + sizeof(A1D_Pack_header_t);

     /*Unpacking and Copying data*/
     temp = data;
     A1D_Unpack_data(temp, header->vaddress, header->trg_stride_ar, header->count,\
                 header->stride_levels);  

     A1U_FUNC_EXIT();
     return;
}

void A1DI_Generic_callback (void *clientdata, DCMF_Error_t *error) {
     --(*((uint32_t *) clientdata));
}

void A1DI_Control_fenceack_callback (void *clientdata, const DCMF_Control_t *info, size_t peer) {
     --(*((uint32_t *) clientdata));
}


void A1DI_Control_xchange_callback (void *clientdata, const DCMF_Control_t *info, size_t peer) {
     memcpy(A1D_Control_xchange_info.xchange_ptr[peer], info, A1D_Control_xchange_info.xchange_size); 
     --(*((uint32_t *) clientdata));
}

void A1DI_RecvDone_noncontigput_callback (void *clientdata, DCMF_Error_t *error) {
     A1DI_Unpack((char *) clientdata);
     free(clientdata);
}

void A1DI_RecvSendShort_noncontigput_callback (void *clientdata, const DCQuad *msginfo, unsigned count, size_t peer,
                             const char *src, size_t bytes) {
     A1DI_Unpack((char *) src); 
}

DCMF_Request_t* A1DI_RecvSend_noncontigput_callback (void *clientdata, const DCQuad *msginfo, unsigned count, size_t peer,\
                             size_t sndlen, size_t *rcvlen, char **rcvbuf, DCMF_Callback_t *cb_done) {
     /*TODO: Need to handle memory allocation failure here*/   
     *rcvlen = sndlen;
     posix_memalign((void **) &rcvbuf, 64, sndlen);
    
     cb_done->function = A1DI_RecvDone_noncontigput_callback;
     cb_done->clientdata = (void *) rcvbuf;

     return ((DCMF_Request_t *) malloc (sizeof(DCMF_Request_t)));
}

void A1DI_RecvSendShort_fence_callback (void *clientdata, const DCQuad *msginfo, unsigned count, size_t peer,
                             const char *src, size_t bytes) {
     DCMF_Control(&A1D_Control_fenceack_info.protocol, 
                 DCMF_SEQUENTIAL_CONSISTENCY,
                 peer,
                 &A1D_Control_fenceack_info.info);    

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
    A1U_ERR_POP(result,"Control xchange registartion returned with error %d \n",result);


  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

DCMF_Result A1DI_Control_fenceack_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Control_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_CONTROL_PROTOCOL;
    conf.network = DCMF_DEFAULT_NETWORK;
    conf.cb_recv = A1DI_Control_fenceack_callback;
    conf.cb_recv_clientdata = (void *) &A1D_Control_fenceack_info.rcv_active;

    result = DCMF_Control_register(&A1D_Control_fenceack_info.protocol, &conf);
    A1U_ERR_POP(result,"Control fenceack registartion returned with error %d \n",result);


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
    result = DCMF_Put_register(&A1D_Generic_put_protocol, &conf);
    A1U_ERR_POP(result,"put registartion returned with error %d \n",result);

    A1D_Nocallback.function = NULL;
    A1D_Nocallback.clientdata = NULL;

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
    result = DCMF_Get_register(&A1D_Generic_get_protocol, &conf);
    A1U_ERR_POP(result,"get registartion returned with error %d \n",result);

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

DCMF_Result A1DI_Send_noncontigput_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Send_Configuration_t conf;

    A1U_FUNC_ENTER();

    /* FIXME: The recv callback should be implemented when Send might be used *
     * with large messages */

    conf.protocol = DCMF_DEFAULT_SEND_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    conf.cb_recv_short = A1DI_RecvSendShort_noncontigput_callback;
    conf.cb_recv_short_clientdata = NULL;
    conf.cb_recv = A1DI_RecvSend_noncontigput_callback;
    conf.cb_recv_clientdata = NULL;

    result = DCMF_Send_register(&A1D_Send_noncontigput_info.protocol, &conf);
    A1U_ERR_POP(result,"send noncontigput registartion returned with error %d \n",result);

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

DCMF_Result A1DI_Send_fence_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Send_Configuration_t conf;

    A1U_FUNC_ENTER();

    /* FIXME: The recv callback should be implemented when Send might be used *
     * with large messages */

    conf.protocol = DCMF_DEFAULT_SEND_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    conf.cb_recv_short = A1DI_RecvSendShort_fence_callback;
    conf.cb_recv_short_clientdata = (void *) &A1D_Send_fence_info.rcv_active;
    conf.cb_recv = NULL; 
    conf.cb_recv_clientdata = NULL;
 
    result = DCMF_Send_register(&A1D_Send_fence_info.protocol, &conf);
    A1U_ERR_POP(result,"send fence registartion returned with error %d \n",result);

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

int A1DI_Memregion_Global_initialize() {

    int result = A1_SUCCESS;
    int out, i;

    A1U_FUNC_ENTER();

    A1D_Memregion_global = (DCMF_Memregion_t *) malloc (sizeof(DCMF_Memregion_t)\
                 *A1D_Process_info.num_ranks);
    A1U_ERR_POP(result = (!A1D_Memregion_global), "Memregion allocation Failed \n"); 

    result = DCMF_Memregion_create (&A1D_Memregion_global[A1D_Process_info.my_rank], &out,
                 -1, NULL, 0);
    A1U_ERR_POP(result,"Global Memory Registration Failed \n");

    result = A1DI_Memregion_Global_xchange();
    A1U_ERR_POP(result,"Memory Region Xchange Failed \n");

    A1D_Membase_global = (void **) malloc (sizeof(void *) * A1D_Process_info.num_ranks);
    A1U_ERR_POP(result = (!A1D_Membase_global), "Membase allocation Failed \n");
    for (i=0; i<A1D_Process_info.num_ranks; i++) 
        DCMF_Memregion_query(&A1D_Memregion_global[i], &out, (void *) &A1D_Membase_global[i]);

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

DCMF_Result A1DI_Memregion_Global_xchange() {

    DCMF_Result result = DCMF_SUCCESS;
    int rank, bytes;

    A1U_FUNC_ENTER();

    A1D_GlobalBarrier();

    A1D_Control_xchange_info.xchange_ptr = &A1D_Memregion_global;
    A1D_Control_xchange_info.xchange_size = (uint32_t) sizeof(DCMF_Memregion_t);
    A1D_Control_xchange_info.rcv_active = A1D_Process_info.num_ranks-1; 
    for(rank=0; rank<A1D_Process_info.num_ranks; rank++) {
        if(rank != A1D_Process_info.my_rank) {
            DCMF_Control(&A1D_Control_xchange_info.protocol, DCMF_SEQUENTIAL_CONSISTENCY,
                     rank, (DCMF_Control_t *) &A1D_Memregion_global[A1D_Process_info.my_rank]);
        }
    }
    while(A1D_Control_xchange_info.rcv_active) A1DI_CRITICAL(DCMF_Messager_advance());

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;

}


int A1DI_Request_pool_initialize() {

    int result = A1_SUCCESS;
    int index;
    A1D_Request_info_t *request;

    A1U_FUNC_ENTER();

    request = (A1D_Request_info_t *) malloc (sizeof(A1D_Request_info_t)*A1C_REQUEST_POOL_INITIAL);   
    A1U_ERR_POP(result = !request,"memory allocation for request pool failed \n");

    A1D_Request_pool.head = request;
    A1D_Request_pool.current = request;
    A1D_Request_pool.tail = &request[A1C_REQUEST_POOL_INITIAL-1];
    A1D_Request_pool.head->prev = NULL;
    A1D_Request_pool.tail->next = NULL;
    for(index=1; index<A1C_REQUEST_POOL_INITIAL; index++) {
       request[index-1].next = &request[index];
       request[index].prev = &request[index-1];  
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

int A1DI_Request_pool_increment() {

    int result = A1_SUCCESS;
    int index;
    A1D_Request_info_t *request;

    A1U_FUNC_ENTER();

    request = (A1D_Request_info_t *) malloc (sizeof(A1D_Request_info_t)*A1C_REQUEST_POOL_INCREMENT);
    A1U_ERR_POP(result = !request,"memory allocation for request pool failed \n");

    A1D_Request_pool.current= request;
    A1D_Request_pool.tail->next = request;
    request->prev = A1D_Request_pool.tail;
    A1D_Request_pool.tail = &request[A1C_REQUEST_POOL_INCREMENT-1];
    A1D_Request_pool.tail->next = NULL;
    for(index=1; index<A1C_REQUEST_POOL_INITIAL; index++) {
       request[index-1].next = &request[index];
       request[index].prev = &request[index-1];
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

A1D_Request_info_t* A1DI_Get_request() {

    int index;
    A1D_Request_info_t *request = NULL;

    A1U_FUNC_ENTER();

    if(!A1D_Request_pool.current) 
        A1DI_Request_pool_increment();
    request = A1D_Request_pool.current;
    A1D_Request_pool.current = A1D_Request_pool.current->next;

    request->active = 1;
    return request; 

  fn_exit:
    A1U_FUNC_EXIT();
    return request;

  fn_fail:
    goto fn_exit;
}

void A1DI_Free_request(A1D_Request_info_t *request) {

    A1U_FUNC_ENTER();
 
    if(request->next == NULL) { 
        A1D_Request_pool.current = request; 
    } else if (request->prev == NULL) {
        request->next->prev = NULL;
        request->next = NULL;
        request->prev = A1D_Request_pool.tail;
        A1D_Request_pool.tail = request;
    } else {
        request->prev->next = request->next;
        request->next->prev = request->prev;
        request->next = NULL;
        request->prev = A1D_Request_pool.tail;
        A1D_Request_pool.tail = request;
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}

DCMF_Result A1D_Barrier() { 

    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t request; 

    A1U_FUNC_ENTER();

    A1D_GlobalBarrier_info.active = 1;

    result = DCMF_GlobalBarrier(&A1D_GlobalBarrier_info.protocol, &request, A1D_GlobalBarrier_info.callback);  
    A1U_ERR_POP(result,"dcmf global barrier returned with error \n");

    while(A1D_GlobalBarrier_info.active) A1DI_CRITICAL(DCMF_Messager_advance()); 

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;    

}

int A1D_Initialize(int thread_level, int num_threads,
         int num_memtypes, a1_memtype_t memtypes[]) {

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

    result = A1DI_Control_xchange_initialize();
    A1U_ERR_POP(result,"control xchange initialize returned with error \n");

    result = A1DI_Control_fenceack_initialize();
    A1U_ERR_POP(result,"control fenceack initialize returned with error \n");

    result = A1DI_GlobalBarrier_initialize();
    A1U_ERR_POP(result,"global barrier initialize returned with error \n");

    result = A1DI_Put_initialize(); 
    A1U_ERR_POP(result,"Put initialize returned with error \n");

    result = A1DI_Get_initialize();
    A1U_ERR_POP(result,"Get initialize returned with error \n"); 

    result = A1DI_Send_noncontigput_initialize();
    A1U_ERR_POP(result,"Send noncontigput initialize returned with error \n");

    result = A1DI_Send_fence_initialize();
    A1U_ERR_POP(result,"Send fence initialize returned with error \n");

    result = A1DI_Memregion_Global_initialize();
    A1U_ERR_POP(result,"Memregion list initialize returned with error \n");    

    result = A1DI_Request_pool_initialize();
    A1U_ERR_POP(result,"Request initialization failed \n");

    /* FIXME: Need to do stuff here! */

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

