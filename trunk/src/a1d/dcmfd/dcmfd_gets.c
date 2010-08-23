/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Protocol_t A1D_Packed_gets_protocol;
DCMF_Protocol_t A1D_Packed_gets_response_protocol;
volatile int A1D_Expecting_getresponse;

int A1DI_Packed_gets_response(int target,
                              int stride_level,
                              int *block_sizes,
                              void* source_ptr,
                              int *src_stride_ar,
                              void* target_ptr,
                              int *trg_stride_ar)
{
    int status = A1_SUCCESS;
    DCMF_Request_t *request;
    DCMF_Callback_t callback;
    A1D_Request_t *a1d_request;
    void *packet;
    int size_packet;

    A1U_FUNC_ENTER();

    a1d_request = A1DI_Get_request();
    A1U_ERR_POP(status = (a1d_request == NULL),
                "A1DI_Get_request returned NULL in A1DI_Packed_gets_response. Requests exhausted \n");

    status = A1DI_Pack_strided(&packet,
                               &size_packet,
                               stride_level,
                               block_sizes,
                               source_ptr,
                               src_stride_ar,
                               target_ptr,
                               trg_stride_ar);
    A1U_ERR_POP(status != DCMF_SUCCESS,
                "A1DI_Pack_strided returned with an error\n");

    a1d_request->buffer_ptr = packet;
    request = &(a1d_request->request);
    callback.function = A1DI_Free_done;
    callback.clientdata = (void *) a1d_request;

    status = DCMF_Send(&A1D_Packed_gets_response_protocol,
                       request,
                       callback,
                       DCMF_SEQUENTIAL_CONSISTENCY,
                       target,
                       size_packet,
                       packet,
                       NULL,
                       0);
    A1U_ERR_POP(status != DCMF_SUCCESS, "Send returned with an error \n");

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;

}

void A1DI_RecvDone_packedgets_response_callback(void *clientdata,
                                                DCMF_Error_t *error)
{
    A1D_Request_t *a1d_request = (A1D_Request_t *) clientdata;

    A1DI_Unpack_strided(a1d_request->buffer_ptr);

    A1D_Expecting_getresponse--;

    A1DI_Release_request(a1d_request);
}

DCMF_Request_t* A1DI_RecvSend_packedgets_response_callback(void *clientdata,
                                                           const DCQuad *msginfo,
                                                           unsigned count, /* TODO: this is not used */
                                                           size_t peer,
                                                           size_t sndlen,
                                                           size_t *rcvlen,
                                                           char **rcvbuf,
                                                           DCMF_Callback_t *cb_done)
{
    int status = 0;
    A1D_Request_t *a1d_request;
 
    a1d_request = A1DI_Get_request();
    A1U_ERR_ABORT(status = (a1d_request == NULL),
                "A1DI_Get_request returned NULL in A1DI_RecvSend_packedgets_response_callback. Requests exhausted \n");

    *rcvlen = sndlen;
    status = A1DI_Malloc_aligned((void **) &(a1d_request->buffer_ptr), sndlen);
    A1U_ERR_ABORT(status != 0,
                  "A1DI_Malloc_aligned failed in A1DI_RecvSend_packedputs_callback\n");

    *rcvbuf = (char *) a1d_request->buffer_ptr;

    cb_done->function = A1DI_RecvDone_packedgets_response_callback;
    cb_done->clientdata = (void *) a1d_request;

    return &(a1d_request->request);
}

void A1DI_RecvSendShort_packedgets_response_callback(void *clientdata,
                                                     const DCQuad *msginfo,
                                                     unsigned count, /* TODO: this is not used */
                                                     size_t peer,
                                                     const char *src,
                                                     size_t bytes)
{
    A1DI_Unpack_strided((void *) src);

    A1D_Expecting_getresponse--;
}

int A1DI_Packed_gets_response_initialize()
{
    int status = A1_SUCCESS;
    DCMF_Send_Configuration_t conf;

    A1U_FUNC_ENTER();

    /* FIXME: The recv callback should be implemented when Send might be used *
     * with large messages */

    conf.protocol = DCMF_DEFAULT_SEND_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    conf.cb_recv_short = A1DI_RecvSendShort_packedgets_response_callback;
    conf.cb_recv_short_clientdata = NULL;
    conf.cb_recv = A1DI_RecvSend_packedgets_response_callback;
    conf.cb_recv_clientdata = NULL;

    status = DCMF_Send_register(&A1D_Packed_gets_response_protocol, &conf);
    A1U_ERR_POP(status != DCMF_SUCCESS,
                "packed puts registartion returned with error %d \n",
                status);

    fn_exit: A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

void A1DI_RecvSendShort_packedgets_callback(void *clientdata,
                                            const DCQuad *msginfo,
                                            unsigned count, /* TODO: this is not used */
                                            size_t peer,
                                            const char *src,
                                            size_t bytes)
{

    A1D_Packed_gets_header_t *header = (A1D_Packed_gets_header_t *) src;

    A1DI_Packed_gets_response(header->target,
                              header->stride_level,
                              header->block_sizes,
                              header->source_ptr,
                              header->src_stride_ar,
                              header->target_ptr,
                              header->trg_stride_ar);

}

int A1DI_Packed_gets_initialize()
{
    int status = A1_SUCCESS;
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

    status = DCMF_Send_register(&A1D_Packed_gets_protocol, &conf);
    A1U_ERR_POP(status != DCMF_SUCCESS,
                "packed gets registartion returned with error %d \n",
                status);

    fn_exit: A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

int A1DI_Packed_gets(int target,
                     int stride_level,
                     int *block_sizes,
                     void* source_ptr,
                     int *src_stride_ar,
                     void* target_ptr,
                     int *trg_stride_ar,
                     A1D_Handle_t *a1d_handle)
{

    int status = A1_SUCCESS;
    DCMF_Callback_t done_callback;
    A1D_Packed_gets_header_t *packet;

    A1U_FUNC_ENTER();

    status = A1DI_Malloc_aligned((void **) &packet, sizeof(A1D_Packed_gets_header_t));
    A1U_ERR_POP(status,"Malloc failed in A1DI_Packed_gets \n");

    /*Copying header information*/
    packet->target = A1D_Process_info.my_rank;
    packet->source_ptr = source_ptr;
    packet->target_ptr = target_ptr;
    packet->stride_level = stride_level;
    memcpy(packet->src_stride_ar, src_stride_ar, stride_level
            * sizeof(uint32_t));
    memcpy(packet->trg_stride_ar, trg_stride_ar, stride_level
            * sizeof(uint32_t));
    memcpy(packet->block_sizes, block_sizes, (stride_level + 1) * sizeof(uint32_t));

    status = A1DI_Load_request(a1d_handle);
    A1U_ERR_POP(status != A1_SUCCESS,
                "A1DI_Load_request returned error in A1DI_Packed_gets. Rquests exhausted \n");

    /* Assigning the packing buffer pointer in request so that it can be free when the
     * request is complete, in the callback */
    a1d_handle->request_list->buffer_ptr = (void *) packet;

    done_callback.function = A1DI_Handle_done;
    done_callback.clientdata = (void *) a1d_handle;
    a1d_handle->active++;

    status = DCMF_Send(&A1D_Packed_gets_protocol,
                       &(a1d_handle->request_list->request),
                       done_callback,
                       DCMF_RELAXED_CONSISTENCY,
                       target,
                       sizeof(A1D_Packed_gets_header_t),
                       (void *) packet,
                       NULL,
                       0);
    A1U_ERR_POP(status, "Send returned with an error \n");

    A1D_Connection_send_active[target]++;

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;

}

int A1DI_Recursive_gets(int target,
                     int stride_level,
                     int *block_sizes,
                     void* source_ptr,
                     int *src_stride_ar,
                     void* target_ptr,
                     int *trg_stride_ar,
                     A1D_Handle_t *a1d_handle)
{
    int i, status = A1_SUCCESS;
    DCMF_Callback_t done_callback;
    size_t src_disp, dst_disp;

    A1U_FUNC_ENTER();

    if (stride_level > 0)
    {

        for (i = 0; i < block_sizes[stride_level]; i++)
        {
            status = A1DI_Recursive_gets(target,
                                      stride_level -1,
                                      block_sizes,
                                      (void *) ((size_t) source_ptr + i * src_stride_ar[stride_level - 1]),
                                      src_stride_ar,
                                      (void *) ((size_t) target_ptr + i * trg_stride_ar[stride_level - 1]),
                                      trg_stride_ar,
                                      a1d_handle);
             A1U_ERR_POP(status != A1_SUCCESS,
                     "A1DI_Recursive_gets returned error in A1DI_Direct_gets. \n");
        }

    }
    else
    {

        src_disp = (size_t) source_ptr
                 - (size_t) A1D_Membase_global[target];
        dst_disp = (size_t) target_ptr
                 - (size_t) A1D_Membase_global[A1D_Process_info.my_rank];

        status = A1DI_Load_request(a1d_handle);
        A1U_ERR_POP(status != A1_SUCCESS,
                   "A1DI_Load_request returned error in A1DI_Recursive_gets. Rquests exhausted \n");

        done_callback.function = A1DI_Handle_done;
        done_callback.clientdata = (void *) a1d_handle;
        a1d_handle->active++;

        status = DCMF_Get(&A1D_Generic_get_protocol,
                          &(a1d_handle->request_list->request),
                          done_callback,
                          DCMF_RELAXED_CONSISTENCY,
                          target,
                          block_sizes[0],
                          &A1D_Memregion_global[target],
                          &A1D_Memregion_global[A1D_Process_info.my_rank],
                          src_disp,
                          dst_disp);
        A1U_ERR_POP(status != DCMF_SUCCESS, "DCMF_Get returned with an error \n");

    }

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1D_GetS(int target,
             int stride_level,
             int *block_sizes,
             void* source_ptr,
             int *src_stride_ar,
             void* target_ptr,
             int *trg_stride_ar)
{
    int status = A1_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    a1d_handle = A1DI_Get_handle();
    A1U_ERR_POP(status = (a1d_handle == NULL),
                "A1DI_Get_handle returned NULL in A1D_GetS. Handles exhausted \n");

    if (block_sizes[0] >= a1_settings.direct_noncontig_get_threshold)
    {

        status = A1DI_Recursive_gets(target,
                                  stride_level,
                                  block_sizes,
                                  source_ptr,
                                  src_stride_ar,
                                  target_ptr,
                                  trg_stride_ar,
                                  a1d_handle);
        A1U_ERR_POP(status, "A1DI_Recursive_gets returned with an error \n");

        A1DI_Conditional_advance(a1d_handle->active > 0);

    }
    else
    {

        A1D_Expecting_getresponse = 1;

        status = A1DI_Packed_gets(target,
                                  stride_level,
                                  block_sizes,
                                  source_ptr,
                                  src_stride_ar,
                                  target_ptr,
                                  trg_stride_ar, 
                                  a1d_handle);
        A1U_ERR_POP(status, "A1DI_Packed_gets returned with an error \n");

        A1DI_Conditional_advance(a1d_handle->active > 0 || A1D_Expecting_getresponse > 0);
        A1D_Connection_send_active[target]--;

    }

  fn_exit:
    A1DI_Release_handle(a1d_handle);
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1D_NbGetS(int target,
             int stride_level,
             int *block_sizes,
             void* source_ptr,
             int *src_stride_ar,
             void* target_ptr,
             int *trg_stride_ar,
             A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    a1d_handle = (A1D_Handle_t *) a1_handle;

    if (block_sizes[0] >= a1_settings.direct_noncontig_get_threshold)
    {

        status = A1DI_Recursive_gets(target,
                                  stride_level,
                                  block_sizes,
                                  source_ptr,
                                  src_stride_ar,
                                  target_ptr,
                                  trg_stride_ar,
                                  a1d_handle);
        A1U_ERR_POP(status, "A1DI_Recursive_gets returned with an error \n");

    }
    else
    {

        A1D_Expecting_getresponse = 1;

        status = A1DI_Packed_gets(target,
                                  stride_level,
                                  block_sizes,
                                  source_ptr,
                                  src_stride_ar,
                                  target_ptr,
                                  trg_stride_ar,
                                  a1d_handle);
        A1U_ERR_POP(status, "A1DI_Packed_gets returned with an error \n");

    }

  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}
