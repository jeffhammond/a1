/* -*- Mode: C; c-basic-offset:4 ; -*- */
/D*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Configure_t A1D_Messager_info;
A1D_Process_info_t A1D_Process_info;
A1D_Control_xchange_info_t A1D_Control_xchange_info;
A1D_Control_flushack_info_t A1D_Control_flushack_info;
A1D_Send_info_t A1D_Send_flush_info;
A1D_GlobalBarrier_info_t A1D_GlobalBarrier_info;
A1D_Request_pool_t A1D_Request_pool;

DCMF_Protocol_t A1D_Generic_put_protocol;
DCMF_Protocol_t A1D_Generic_get_protocol;
DCMF_Protocol_t A1D_Generic_putacc_protocol;
DCMF_Protocol_t A1D_Packed_puts_protocol;
DCMF_Protocol_t A1D_Packed_gets_protocol;
DCMF_Protocol_t A1D_Packed_putaccs_protocol;
DCMF_Callback_t A1D_Nocallback;
DCMF_Memregion_t *A1D_Memregion_global;

void **A1D_Membase_global;
void **A1D_Put_Flushcounter_ptr;
uint32_t *A1D_Connection_send_active;
uint32_t *A1D_Connection_put_active;

uint32_t a1_request_pool_size;

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

void A1DI_Control_flushack_callback(void *clientdata,
                                    const DCMF_Control_t *info,
                                    size_t peer)
{
    --(*((uint32_t *) clientdata));
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

void A1DI_RecvDone_packedputs_callback(void *clientdata, DCMF_Error_t *error)
{
    A1D_Buffer_info_t *buffer_info = (A1D_Buffer_info_t *) clientdata;

    A1DI_Unpack_strided(buffer_info->buffer_ptr);

    free(buffer_info->buffer_ptr);
    free((void *) buffer_info);
}

void A1DI_RecvDone_packedputaccs_callback(void *clientdata, DCMF_Error_t *error)
{
    A1D_Buffer_info_t *buffer_info = (A1D_Buffer_info_t *) clientdata;

    A1DI_Unpack_strided_putaccs(buffer_info->buffer_ptr);

    free(buffer_info->buffer_ptr);
    free((void *) buffer_info);
}

void A1DI_RecvDone_putacc_callback(void *clientdata, DCMF_Error_t *error)
{
    int result = A1_SUCCESS;

    A1D_Putacc_recv_info_t *putacc_recv_info =
            (A1D_Putacc_recv_info_t *) clientdata;
    A1D_Putacc_header_t *header = &(putacc_recv_info->header);
    A1D_Buffer_info_t *buffer_info = &(putacc_recv_info->buffer_info);

    result = A1D_Acc_process(buffer_info->buffer_ptr,
                             buffer_info->bytes,
                             header);
    A1U_ERR_ABORT(result,
                  "A1D_Acc_process failed in A1DI_RecvDone_putacc_callback\n");

    free(buffer_info->buffer_ptr);
    free((void *) putacc_recv_info);
}

void A1DI_RecvSendShort_packedputs_callback(void *clientdata,
                                            const DCQuad *msginfo,
                                            unsigned count,
                                            size_t peer,
                                            const char *src,
                                            size_t bytes)
{
    A1DI_Unpack_strided((void *) src);
}

void A1DI_RecvSendShort_packedgets_callback(void *clientdata,
                                            const DCQuad *msginfo,
                                            unsigned count,
                                            size_t peer,
                                            const char *src,
                                            size_t bytes)
{

    A1D_Packed_gets_header_t *header = (A1D_Packed_gets_header_t *) src;
    int is_getresponse = 1;

    A1DI_Packed_puts(header->target,
                     header->source_ptr,
                     header->src_stride_ar,
                     header->target_ptr,
                     header->trg_stride_ar,
                     header->count,
                     header->stride_levels,
                     is_getresponse);

}

void A1DI_RecvSendShort_packedputaccs_callback(void *clientdata,
                                               const DCQuad *msginfo,
                                               unsigned count,
                                               size_t peer,
                                               const char *src,
                                               size_t bytes)
{
    A1DI_Unpack_strided_putaccs((void *) src);
}

void A1DI_RecvSendShort_putacc_callback(void *clientdata,
                                        const DCQuad *msginfo,
                                        unsigned count,
                                        size_t peer,
                                        const char *src,
                                        size_t bytes)
{
    int result = A1_SUCCESS;

    result = A1D_Acc_process((void *) src,
                             bytes,
                             (A1D_Putacc_header_t *) msginfo);
    A1U_ERR_ABORT(result,
                  "A1D_Acc_process failed in A1DI_RecvSendShort_putacc_callback\n");
}

DCMF_Request_t* A1DI_RecvSend_packedputs_callback(void *clientdata,
                                                  const DCQuad *msginfo,
                                                  unsigned count,
                                                  size_t peer,
                                                  size_t sndlen,
                                                  size_t *rcvlen,
                                                  char **rcvbuf,
                                                  DCMF_Callback_t *cb_done)
{
    int result = 0;
    A1D_Buffer_info_t *buffer_info;

    result = posix_memalign((void **) &buffer_info,
                            16,
                            sizeof(A1D_Buffer_info_t));
    A1U_ERR_ABORT(result != 0,
                  "posix_memalign failed in A1DI_RecvSend_packedputs_callback\n");

    *rcvlen = sndlen;
    result = posix_memalign((void **) rcvbuf, 16, sndlen);
    A1U_ERR_ABORT(result != 0,
                  "posix_memalign failed in A1DI_RecvSend_packedputs_callback\n");

    buffer_info->buffer_ptr = (void *) *rcvbuf;

    cb_done->function = A1DI_RecvDone_packedputs_callback;
    cb_done->clientdata = (void *) buffer_info;

    return &(buffer_info->request);
}

DCMF_Request_t* A1DI_RecvSend_putacc_callback(void *clientdata,
                                              const DCQuad *msginfo,
                                              unsigned count,
                                              size_t peer,
                                              size_t sndlen,
                                              size_t *rcvlen,
                                              char **rcvbuf,
                                              DCMF_Callback_t *cb_done)
{
    int result = 0;
    A1D_Putacc_recv_info_t *putacc_recv_info;

    result = posix_memalign((void **) &putacc_recv_info,
                            16,
                            sizeof(A1D_Putacc_recv_info_t));
    A1U_ERR_ABORT(result != 0,
                  "posix_memalign failed in A1DI_RecvSend_packedputs_callback\n");

    memcpy((void *) &(putacc_recv_info->header), (void *) msginfo, count
            * sizeof(DCQuad));

    *rcvlen = sndlen;
    result = posix_memalign((void **) rcvbuf, 16, sndlen);
    A1U_ERR_ABORT(result != 0,
                  "posix_memalign failed in A1DI_RecvSend_packedputs_callback\n");

    (putacc_recv_info->buffer_info).buffer_ptr = (void *) *rcvbuf;
    (putacc_recv_info->buffer_info).bytes = sndlen;

    cb_done->function = A1DI_RecvDone_putacc_callback;
    cb_done->clientdata = (void *) putacc_recv_info;

    return &((putacc_recv_info->buffer_info).request);
}

DCMF_Request_t* A1DI_RecvSend_packedputaccs_callback(void *clientdata,
                                                     const DCQuad *msginfo,
                                                     unsigned count,
                                                     size_t peer,
                                                     size_t sndlen,
                                                     size_t *rcvlen,
                                                     char **rcvbuf,
                                                     DCMF_Callback_t *cb_done)
{
    int result = 0;
    A1D_Buffer_info_t *buffer_info;

    result = posix_memalign((void **) &buffer_info,
                            16,
                            sizeof(A1D_Buffer_info_t));
    A1U_ERR_ABORT(result != 0,
                  "posix_memalign failed in A1DI_RecvSend_packedputaccs_callback\n");

    *rcvlen = sndlen;
    result = posix_memalign((void **) rcvbuf, 16, sndlen);
    A1U_ERR_ABORT(result != 0,
                  "posix_memalign failed in A1DI_RecvSend_packedputaccs_callback\n");

    buffer_info->buffer_ptr = (void *) *rcvbuf;

    cb_done->function = A1DI_RecvDone_packedputaccs_callback;
    cb_done->clientdata = (void *) buffer_info;

    return &(buffer_info->request);
}

void A1DI_RecvSendShort_flush_callback(void *clientdata,
                                       const DCQuad *msginfo,
                                       unsigned count,
                                       size_t peer,
                                       const char *src,
                                       size_t bytes)
{
    int result = A1_SUCCESS;

    result = DCMF_Control(&A1D_Control_flushack_info.protocol,
                          DCMF_SEQUENTIAL_CONSISTENCY,
                          peer,
                          &A1D_Control_flushack_info.info);
    A1U_ERR_ABORT(result != DCMF_SUCCESS,
                  "DCMF_Control failed in A1DI_RecvSendShort_flush_callback\n");
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

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
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
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "Control flushack registartion returned with error %d \n",
                result);

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}

DCMF_Result A1DI_GlobalBarrier_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_GlobalBarrier_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_GLOBALBARRIER_PROTOCOL;
    result = DCMF_GlobalBarrier_register(&A1D_GlobalBarrier_info.protocol,
                                         &conf);
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "global barrier registartion returned with error %d \n",
                result);

    A1D_GlobalBarrier_info.callback.function = A1DI_Generic_done;
    A1D_GlobalBarrier_info.callback.clientdata
            = (void *) &A1D_GlobalBarrier_info.active;

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}

DCMF_Result A1DI_Put_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Put_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_PUT_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    result = DCMF_Put_register(&A1D_Generic_put_protocol, &conf);
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "put registartion returned with error %d \n",
                result);

    A1D_Nocallback.function = NULL;
    A1D_Nocallback.clientdata = NULL;

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}

DCMF_Result A1DI_Get_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Get_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_GET_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    result = DCMF_Get_register(&A1D_Generic_get_protocol, &conf);
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "get registartion returned with error %d \n",
                result);

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}

DCMF_Result A1DI_Putacc_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Send_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_SEND_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    conf.cb_recv_short = A1DI_RecvSendShort_putacc_callback;
    conf.cb_recv_short_clientdata = NULL;
    conf.cb_recv = A1DI_RecvSend_putacc_callback;
    conf.cb_recv_clientdata = NULL;

    result = DCMF_Send_register(&A1D_Generic_putacc_protocol, &conf);
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "putacc registartion returned with error %d \n",
                result);

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}

DCMF_Result A1DI_Packed_puts_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Send_Configuration_t conf;

    A1U_FUNC_ENTER();

    /* FIXME: The recv callback should be implemented when Send might be used *
     * with large messages */

    conf.protocol = DCMF_DEFAULT_SEND_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    conf.cb_recv_short = A1DI_RecvSendShort_packedputs_callback;
    conf.cb_recv_short_clientdata = NULL;
    conf.cb_recv = A1DI_RecvSend_packedputs_callback;
    conf.cb_recv_clientdata = NULL;

    result = DCMF_Send_register(&A1D_Packed_puts_protocol, &conf);
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "packed puts registartion returned with error %d \n",
                result);

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}

DCMF_Result A1DI_Packed_gets_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Send_Configuration_t conf;

    A1U_FUNC_ENTER();

    /* FIXME: The recv callback should be implemented when Send might be used *
     * with large messages */

    conf.protocol = DCMF_DEFAULT_SEND_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    conf.cb_recv_short = A1DI_RecvSendShort_packedgets_callback;
    conf.cb_recv_short_clientdata = NULL;
    conf.cb_recv = NULL;
    conf.cb_recv_clientdata = NULL;

    result = DCMF_Send_register(&A1D_Packed_gets_protocol, &conf);
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "packed gets registartion returned with error %d \n",
                result);

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}

DCMF_Result A1DI_Packed_putaccs_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Send_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_SEND_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    conf.cb_recv_short = A1DI_RecvSendShort_packedputaccs_callback;
    conf.cb_recv_short_clientdata = NULL;
    conf.cb_recv = A1DI_RecvSend_packedputaccs_callback;
    conf.cb_recv_clientdata = NULL;

    result = DCMF_Send_register(&A1D_Packed_putaccs_protocol, &conf);
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "packed putaccs registartion returned with error %d \n",
                result);

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
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
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "send flush registartion returned with error %d \n",
                result);

    /* Allocating memory for vector that tracks connections with active sends */
    result = posix_memalign((void **) &A1D_Connection_send_active,
                            16,
                            sizeof(uint32_t) * A1D_Process_info.num_ranks);
    A1U_ERR_POP(result != 0,
                "Connection send active buffer allocation Failed \n");
    memset(A1D_Connection_send_active, 0, sizeof(uint32_t)
            * A1D_Process_info.num_ranks);

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}

DCMF_Result A1DI_Put_flush_initialize()
{
    DCMF_Result result = A1_SUCCESS;
    DCMF_Control_t cmsg;
    int rank;

    A1U_FUNC_ENTER();

    result = posix_memalign((void **) &A1D_Put_Flushcounter_ptr,
                            16,
                            sizeof(void *) * A1D_Process_info.num_ranks);
    A1U_ERR_POP(result != 0, "put flush ptr buffer allocation Failed \n");
    result
            = posix_memalign((void **) &(A1D_Put_Flushcounter_ptr[A1D_Process_info.my_rank]),
                             16,
                             2);
    A1U_ERR_POP(result != 0, "put flush buffer allocation Failed \n");

    /*TODO: Use DCMF_Send operations instead to exploit TORUS network */
    A1D_Control_xchange_info.xchange_ptr = (void *) A1D_Put_Flushcounter_ptr;
    A1D_Control_xchange_info.xchange_size = sizeof(void *);
    A1D_Control_xchange_info.rcv_active += A1D_Process_info.num_ranks - 1;

    A1DI_GlobalBarrier();

    memcpy((void *) &cmsg,
           (void *) &(A1D_Put_Flushcounter_ptr[A1D_Process_info.my_rank]),
           sizeof(void *));
    for (rank = 0; rank < A1D_Process_info.num_ranks; rank++)
    {
        if (rank != A1D_Process_info.my_rank)
        {
            result = DCMF_Control(&A1D_Control_xchange_info.protocol,
                                  DCMF_SEQUENTIAL_CONSISTENCY,
                                  rank,
                                  &cmsg);
            A1U_ERR_POP(result != DCMF_SUCCESS,
                        "DCMF_Control failed in A1DI_Put_flush_initialize\n");
        }
    }
    while (A1D_Control_xchange_info.rcv_active > 0)
        A1DI_Advance();

    /* Allocating memory for vector thats tracks connections with active puts */
    result = posix_memalign((void **) &A1D_Connection_put_active,
                            16,
                            sizeof(uint32_t) * A1D_Process_info.num_ranks);
    A1U_ERR_POP(result != 0,
                "Connection put active buffer allocation Failed \n");
    memset(A1D_Connection_put_active, 0, sizeof(uint32_t)
            * A1D_Process_info.num_ranks);

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}

DCMF_Result A1DI_Memregion_Global_xchange()
{

    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Control_t cmsg;
    int rank;

    A1U_FUNC_ENTER();

    /*TODO: Use DCMF_Send operations instead to exploit TORUS network */

    A1D_Control_xchange_info.xchange_ptr = (void *) A1D_Memregion_global;
    A1D_Control_xchange_info.xchange_size = sizeof(DCMF_Memregion_t);
    A1D_Control_xchange_info.rcv_active += A1D_Process_info.num_ranks - 1;

    A1DI_GlobalBarrier();

    memcpy((void *) &cmsg,
           (void *) &A1D_Memregion_global[A1D_Process_info.my_rank],
           sizeof(DCMF_Memregion_t));
    for (rank = 0; rank < A1D_Process_info.num_ranks; rank++)
    {
        if (rank != A1D_Process_info.my_rank)
        {
            result = DCMF_Control(&A1D_Control_xchange_info.protocol,
                                  DCMF_SEQUENTIAL_CONSISTENCY,
                                  rank,
                                  &cmsg);
            A1U_ERR_POP(result != DCMF_SUCCESS,
                        "DCMF_Control failed in A1DI_Memregion_Global_xchange\n");
        }
    }
    while (A1D_Control_xchange_info.rcv_active > 0)
        A1DI_Advance();

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;

}

int A1DI_Memregion_Global_initialize()
{

    int result = A1_SUCCESS;
    int out, i;

    A1U_FUNC_ENTER();

    result = posix_memalign((void **) &A1D_Memregion_global,
                            16,
                            sizeof(DCMF_Memregion_t)
                                    * A1D_Process_info.num_ranks);
    A1U_ERR_POP(result != 0, "Memregion allocation Failed \n");

    result
            = DCMF_Memregion_create(&A1D_Memregion_global[A1D_Process_info.my_rank],
                                    &out,
                                    (size_t) - 1,
                                    NULL,
                                    0);
    A1U_ERR_POP(result != DCMF_SUCCESS, "Global Memory Registration Failed \n");

    result = A1DI_Memregion_Global_xchange();
    A1U_ERR_POP(result != A1_SUCCESS, "Memory Region Xchange Failed \n");

    result = posix_memalign((void **) &A1D_Membase_global, 16, sizeof(void *)
            * A1D_Process_info.num_ranks);
    A1U_ERR_POP(result != 0, "Membase allocation Failed \n");

    for (i = 0; i < A1D_Process_info.num_ranks; i++)
    {
        result = DCMF_Memregion_query(&A1D_Memregion_global[i],
                                      &out,
                                      (void **) &A1D_Membase_global[i]);
        A1U_ERR_POP(result != DCMF_SUCCESS, "Memregion query failed \n");
    }

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
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

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
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

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
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

    fn_exit: A1U_FUNC_EXIT();
    return &(a1_request->request);

    fn_fail: goto fn_exit;
}

void A1DI_Reset_request_pool()
{

    A1U_FUNC_ENTER();

    A1D_Request_pool.current = A1D_Request_pool.head;

    fn_exit: A1U_FUNC_EXIT();
    return;

    fn_fail: goto fn_exit;
}

int A1D_Initialize(int thread_level)
{

    DCMF_Result result = DCMF_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    DCMF_Messager_initialize();

    A1D_Messager_info.thread_level = thread_level;
    A1D_Messager_info.interrupts = DCMF_INTERRUPTS_OFF;

    A1D_Process_info.my_rank = DCMF_Messager_rank();
    A1D_Process_info.num_ranks = DCMF_Messager_size();
    DCMF_Hardware(&(A1D_Process_info.hw));

    result = DCMF_Messager_configure(&A1D_Messager_info, &A1D_Messager_info);
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "global barrier initialize returned with error \n");

    A1DI_Read_parameters();

    result = A1DI_Control_xchange_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,
                "control xchange initialize returned with error \n");

    result = A1DI_Control_flushack_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,
                "control flushack initialize returned with error \n");

    result = A1DI_GlobalBarrier_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,
                "global barrier initialize returned with error \n");

    result = A1DI_Put_initialize();
    A1U_ERR_POP(result != A1_SUCCESS, "Put initialize returned with error \n");

    result = A1DI_Get_initialize();
    A1U_ERR_POP(result != A1_SUCCESS, "Get initialize returned with error \n");

    result = A1DI_Putacc_initialize();
    A1U_ERR_POP(result != A1_SUCCESS, "Get initialize returned with error \n");

    result = A1DI_Packed_puts_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,
                "Packed puts initialize returned with error \n");

    result = A1DI_Packed_gets_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,
                "Packed puts initialize returned with error \n");

    result = A1DI_Packed_putaccs_initialize();
    A1U_ERR_POP(result!=A1_SUCCESS,"Get initialize returned with error \n");

    result = A1DI_Send_flush_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,
                "Send flush initialize returned with error \n");

    result = A1DI_Put_flush_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,
                "Put flush initialize returned with error \n");

    result = A1DI_Request_pool_initialize();
    A1U_ERR_POP(result != A1_SUCCESS, "Request initialization failed \n");

    result = A1DI_Memregion_Global_initialize();
    A1U_ERR_POP(result != A1_SUCCESS,
                "Memregion list initialize returned with error \n");

    /* FIXME: Need to do stuff here! */

    fn_exit: A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}

