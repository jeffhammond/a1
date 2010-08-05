/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Configure_t A1D_Messager_info;
A1D_Process_info_t A1D_Process_info;
A1D_Control_xchange_info_t A1D_Control_xchange_info;
A1D_Control_flushack_info_t A1D_Control_flushack_info;
A1D_Send_info_t A1D_Send_noncontigput_info;
A1D_Send_info_t A1D_Send_flush_info;
A1D_GlobalBarrier_info_t A1D_GlobalBarrier_info;
A1D_Request_pool_t A1D_Request_pool;

DCMF_Protocol_t A1D_Generic_put_protocol;
DCMF_Protocol_t A1D_Generic_get_protocol;
DCMF_Callback_t A1D_Nocallback;
DCMF_Memregion_t *A1D_Memregion_global;

void **A1D_Membase_global;
void **A1D_Put_Flushcounter_ptr;
uint32_t *A1D_Connection_send_active;
uint32_t *A1D_Connection_put_active;

char* A1DI_Unpack_data(void *pointer, void *trg_ptr, int *trg_stride_ar,\
        int *count, int stride_level)
{
     int i;

     A1U_FUNC_ENTER();

     if(stride_level > 0) {
         for(i=0; i<count[stride_level]; i++)
         {
            pointer = A1DI_Unpack_data(pointer, (void *) ((size_t)trg_ptr + i*trg_stride_ar[stride_level-1]), trg_stride_ar,\
                                count, stride_level-1);
         }
     } else {
         memcpy(trg_ptr, pointer, count[0]);
         pointer = (void *)((size_t)pointer + count[0]);
     }

  fn_exit:
     A1U_FUNC_EXIT();
     return pointer;

  fn_fail:
     goto fn_exit;
}

void A1DI_Unpack(void *packet)
{
     void *temp;
     A1D_Pack_header_t *header;

     A1U_FUNC_ENTER();

     header = (A1D_Pack_header_t *) packet;

     /*Unpacking and Copying data*/
     temp = (void *)((size_t)packet + sizeof(A1D_Pack_header_t));
     A1DI_Unpack_data(temp, header->vaddress, header->trg_stride_ar, header->count,\
                 header->stride_levels);  

  fn_exit:
     A1U_FUNC_EXIT();
     return;

  fn_fail:
     goto fn_exit;
}

void A1DI_Generic_done (void *clientdata, DCMF_Error_t *error) {
     --(*((uint32_t *) clientdata));
}

void A1DI_Control_flushack_callback (void *clientdata, const DCMF_Control_t *info, size_t peer) {
     --(*((uint32_t *) clientdata));
}


void A1DI_Control_xchange_callback (void *clientdata, const DCMF_Control_t *info, size_t peer) {
     memcpy((void *) ((size_t) A1D_Control_xchange_info.xchange_ptr + (size_t) (peer*A1D_Control_xchange_info.xchange_size)),\
         (void *) info, A1D_Control_xchange_info.xchange_size); 
     --(*((uint32_t *) clientdata));
}

void A1DI_RecvDone_noncontigput_callback (void *clientdata, DCMF_Error_t *error) {
     A1DI_Unpack((void *) clientdata);
     free(clientdata);
}

void A1DI_RecvSendShort_noncontigput_callback (void *clientdata, const DCQuad *msginfo, unsigned count, size_t peer,
                             const char *src, size_t bytes) {
     A1DI_Unpack((void *) src); 
}

DCMF_Request_t* A1DI_RecvSend_noncontigput_callback (void *clientdata, const DCQuad *msginfo, unsigned count, size_t peer,\
                             size_t sndlen, size_t *rcvlen, char **rcvbuf, DCMF_Callback_t *cb_done) {
     /*TODO: Need to handle memory allocation failure here*/   
     *rcvlen = sndlen;
     posix_memalign((void **) rcvbuf, 16, sndlen);
    
     cb_done->function = A1DI_RecvDone_noncontigput_callback;
     cb_done->clientdata = (void *) *rcvbuf;

     return ((DCMF_Request_t *) malloc (sizeof(DCMF_Request_t)));
}

void A1DI_RecvSendShort_flush_callback (void *clientdata, const DCQuad *msginfo, unsigned count, size_t peer,
                             const char *src, size_t bytes) {
     DCMF_Control(&A1D_Control_flushack_info.protocol, 
                 DCMF_SEQUENTIAL_CONSISTENCY,
                 peer,
                 &A1D_Control_flushack_info.info);    

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

DCMF_Result A1DI_Control_flushack_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Control_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_CONTROL_PROTOCOL;
    conf.network = DCMF_DEFAULT_NETWORK;
    conf.cb_recv = A1DI_Control_flushack_callback;
    conf.cb_recv_clientdata = (void *) &A1D_Control_flushack_info.rcv_active;

    result = DCMF_Control_register(&A1D_Control_flushack_info.protocol, &conf);
    A1U_ERR_POP(result,"Control flushack registartion returned with error %d \n",result);


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

    A1D_GlobalBarrier_info.callback.function = A1DI_Generic_done;
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

DCMF_Result A1DI_Send_flush_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Send_Configuration_t conf;

    A1U_FUNC_ENTER();

    /* FIXME: The recv callback should be implemented when Send might be used *
     * with large messages */

    conf.protocol = DCMF_DEFAULT_SEND_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    conf.cb_recv_short = A1DI_RecvSendShort_flush_callback;
    conf.cb_recv_short_clientdata = (void *) &A1D_Send_flush_info.rcv_active;
    conf.cb_recv = NULL; 
    conf.cb_recv_clientdata = NULL;
 
    result = DCMF_Send_register(&A1D_Send_flush_info.protocol, &conf);
    A1U_ERR_POP(result,"send flush registartion returned with error %d \n",result);

    /* Allocating memory for vector that tracks connections with active sends */
    posix_memalign((void **) &A1D_Connection_send_active, 16, sizeof(uint32_t) * A1D_Process_info.num_ranks);
    A1U_ERR_POP(result = (!A1D_Connection_send_active), "Connection send active buffer allocation Failed \n");
    memset(A1D_Connection_send_active, 0, sizeof(uint32_t)*A1D_Process_info.num_ranks);

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

DCMF_Result A1DI_Put_flush_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Control_t cmsg;
    int rank;

    A1U_FUNC_ENTER();

    posix_memalign((void **) &A1D_Put_Flushcounter_ptr, 16, sizeof(void *) * A1D_Process_info.num_ranks);
    A1U_ERR_POP(result = (!A1D_Put_Flushcounter_ptr), "put flush ptr buffer allocation Failed \n");
    posix_memalign((void **) &(A1D_Put_Flushcounter_ptr[A1D_Process_info.my_rank]), 16, 2);
    A1U_ERR_POP(result = (!(A1D_Put_Flushcounter_ptr[A1D_Process_info.my_rank])), "put flush buffer allocation Failed \n");

    /*TODO: Use DCMF_Send operations instead to exploit TORUS network */
    A1D_Control_xchange_info.xchange_ptr = (void *) A1D_Put_Flushcounter_ptr;
    A1D_Control_xchange_info.xchange_size = sizeof(void *);
    A1D_Control_xchange_info.rcv_active += A1D_Process_info.num_ranks-1;

    A1DI_GlobalBarrier();

    memcpy((void *) &cmsg, (void *) &(A1D_Put_Flushcounter_ptr[A1D_Process_info.my_rank]), sizeof(void *));
    for(rank=0; rank<A1D_Process_info.num_ranks; rank++) {
        if(rank != A1D_Process_info.my_rank) {
            DCMF_Control(&A1D_Control_xchange_info.protocol, DCMF_SEQUENTIAL_CONSISTENCY,
                     rank, &cmsg);
        }
    }
    while(A1D_Control_xchange_info.rcv_active > 0) DCMF_Messager_advance();   

    /* Allocating memory for vector thats tracks connections with active puts */
    posix_memalign((void **) &A1D_Connection_put_active, 16, sizeof(uint32_t) * A1D_Process_info.num_ranks);
    A1U_ERR_POP(result = (!A1D_Connection_put_active), "Connection put active buffer allocation Failed \n");
    memset(A1D_Connection_put_active, 0, sizeof(uint32_t)*A1D_Process_info.num_ranks);

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

DCMF_Result A1DI_Memregion_Global_xchange() {

    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Control_t cmsg;
    int rank;

    A1U_FUNC_ENTER();

    /*TODO: Use DCMF_Send operations instead to exploit TORUS network */

    A1D_Control_xchange_info.xchange_ptr = (void *) A1D_Memregion_global;
    A1D_Control_xchange_info.xchange_size = sizeof(DCMF_Memregion_t);
    A1D_Control_xchange_info.rcv_active += A1D_Process_info.num_ranks-1;

    A1DI_GlobalBarrier();

    memcpy((void *) &cmsg, (void *) &A1D_Memregion_global[A1D_Process_info.my_rank], sizeof(DCMF_Memregion_t)); 
    for(rank=0; rank<A1D_Process_info.num_ranks; rank++) {
        if(rank != A1D_Process_info.my_rank) {
            DCMF_Control(&A1D_Control_xchange_info.protocol, DCMF_SEQUENTIAL_CONSISTENCY,
                     rank, &cmsg);
        }
    }
    while(A1D_Control_xchange_info.rcv_active > 0) DCMF_Messager_advance();

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

    posix_memalign((void **) &A1D_Memregion_global, 16, sizeof(DCMF_Memregion_t)*A1D_Process_info.num_ranks);
    A1U_ERR_POP(result = (!A1D_Memregion_global), "Memregion allocation Failed \n"); 

    result = DCMF_Memregion_create (&A1D_Memregion_global[A1D_Process_info.my_rank], &out,
                 (size_t) -1, NULL, 0);
    A1U_ERR_POP(result,"Global Memory Registration Failed \n");

    result = A1DI_Memregion_Global_xchange();
    A1U_ERR_POP(result,"Memory Region Xchange Failed \n");

    posix_memalign((void **) &A1D_Membase_global, 16, sizeof(void *) * A1D_Process_info.num_ranks);
    A1U_ERR_POP(result = (!A1D_Membase_global), "Membase allocation Failed \n");

    for (i=0; i<A1D_Process_info.num_ranks; i++) { 
        result =  DCMF_Memregion_query(&A1D_Memregion_global[i], &out, (void **) &A1D_Membase_global[i]);
        A1U_ERR_POP(result, "Memregion query failed \n");
    }

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

    posix_memalign((void **) &request, 16, sizeof(A1D_Request_info_t)*A1C_REQUEST_POOL_INITIAL); 
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

    posix_memalign((void **) &request, 16, sizeof(A1D_Request_info_t)*A1C_REQUEST_POOL_INCREMENT);
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

int A1D_Initialize(int thread_level) {

    DCMF_Result result = DCMF_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    DCMF_Messager_initialize();

    A1D_Messager_info.thread_level = thread_level;
    A1D_Messager_info.interrupts = DCMF_INTERRUPTS_OFF;

    A1D_Process_info.my_rank   = DCMF_Messager_rank();
    A1D_Process_info.num_ranks = DCMF_Messager_size();
    DCMF_Hardware(&(A1D_Process_info.hw));

    result = DCMF_Messager_configure(&A1D_Messager_info, &A1D_Messager_info);
    A1U_ERR_POP(result,"global barrier initialize returned with error \n");     

    A1DI_Read_parameters();

    result = A1DI_Control_xchange_initialize();
    A1U_ERR_POP(result,"control xchange initialize returned with error \n");

    result = A1DI_Control_flushack_initialize();
    A1U_ERR_POP(result,"control flushack initialize returned with error \n");

    result = A1DI_GlobalBarrier_initialize();
    A1U_ERR_POP(result,"global barrier initialize returned with error \n");

    result = A1DI_Put_initialize(); 
    A1U_ERR_POP(result,"Put initialize returned with error \n");

    result = A1DI_Get_initialize();
    A1U_ERR_POP(result,"Get initialize returned with error \n"); 

    result = A1DI_Send_noncontigput_initialize();
    A1U_ERR_POP(result,"Send noncontigput initialize returned with error \n");

    result = A1DI_Send_flush_initialize();
    A1U_ERR_POP(result,"Send flush initialize returned with error \n");

    result = A1DI_Put_flush_initialize();
    A1U_ERR_POP(result,"Put flush initialize returned with error \n");

    result = A1DI_Request_pool_initialize();
    A1U_ERR_POP(result,"Request initialization failed \n");

    result = A1DI_Memregion_Global_initialize();
    A1U_ERR_POP(result,"Memregion list initialize returned with error \n");    

    /* FIXME: Need to do stuff here! */

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

