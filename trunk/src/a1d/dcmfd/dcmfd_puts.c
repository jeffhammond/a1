/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Protocol_t A1D_Packed_puts_protocol;

void A1DI_RecvDone_packedputs_callback(void *clientdata, DCMF_Error_t *error)
{
    A1D_Buffer_info_t *buffer_info = (A1D_Buffer_info_t *) clientdata;

    A1DI_Unpack_strided(buffer_info->buffer_ptr);

    free(buffer_info->buffer_ptr);
    free((void *) buffer_info);
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

void A1DI_RecvSendShort_packedputs_callback(void *clientdata,
                                            const DCQuad *msginfo,
                                            unsigned count,
                                            size_t peer,
                                            const char *src,
                                            size_t bytes)
{
    A1DI_Unpack_strided((void *) src);
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

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

int A1DI_Packed_puts(int target,
                     void* source_ptr,
                     int *src_stride_ar,
                     void* target_ptr,
                     int *trg_stride_ar,
                     int *count,
                     int stride_levels,
                     int is_getresponse)
{

    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t *request;
    DCMF_Callback_t callback;
    void *packet;
    unsigned size_packet;
    volatile int active;

    A1U_FUNC_ENTER();

    result = A1DI_Pack_strided(&packet,
                               &size_packet,
                               source_ptr,
                               src_stride_ar,
                               target_ptr,
                               trg_stride_ar,
                               count,
                               stride_levels,
                               is_getresponse);
    A1U_ERR_POP(result != DCMF_SUCCESS, "A1DI_Pack_strided returned with an error\n");

    /* TODO: This seems a bit scary to me.  Can't we have different functions:
     *        one for inside callback and one for normal usage, just to be safe?
     */
    /* If this put is in response to a get, we are in a callback, so we should not 
     hit advance. We set buffer and request pointers in the callback which frees it
     when invoked */
    if (is_getresponse)
    {
        A1D_Buffer_info_t *response_info;
        posix_memalign((void **) &response_info, 16, sizeof(A1D_Buffer_info_t));
        response_info->buffer_ptr = packet;
        request = &(response_info->request);
        callback.function = A1DI_Free_done;
        callback.clientdata = (void *) response_info;
    }
    else
    {
        request = A1DI_Get_request();
        callback.function = A1DI_Generic_done;
        callback.clientdata = (void *) &active;
        active = 1;
    }

    result = DCMF_Send(&A1D_Packed_puts_protocol,
                       request,
                       callback,
                       DCMF_SEQUENTIAL_CONSISTENCY,
                       target,
                       size_packet,
                       packet,
                       NULL,
                       0);
    A1U_ERR_POP(result, "Send returned with an error \n");

    if (!is_getresponse)
    {
        A1D_Connection_send_active[target]++;
        A1DI_Conditional_advance(active > 0);
        free(packet);
    }

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;

}

int A1DI_Direct_puts(int target,
                     void* source_ptr,
                     int *src_stride_ar,
                     void* target_ptr,
                     int *trg_stride_ar,
                     int* count,
                     int stride_level)
{
    int result = A1_SUCCESS;
    DCMF_Request_t *request;
    int i, size;
    size_t src_disp, dst_disp;

    A1U_FUNC_ENTER();

    if (stride_level > 0)
    {

        for (i = 0; i < count[stride_level]; i++)
        {
            A1DI_Direct_puts(target,
                             (void *) ((size_t) source_ptr + i
                                     * src_stride_ar[stride_level - 1]),
                             src_stride_ar,
                             (void *) ((size_t) target_ptr + i
                                     * trg_stride_ar[stride_level - 1]),
                             trg_stride_ar,
                             count,
                             stride_level - 1);
        }

    }
    else
    {

        request = A1DI_Get_request();

        src_disp = (size_t) source_ptr
                - (size_t) A1D_Membase_global[A1D_Process_info.my_rank];
        dst_disp = (size_t) target_ptr - (size_t) A1D_Membase_global[target];

        result = DCMF_Put(&A1D_Generic_put_protocol,
                          request,
                          A1D_Nocallback,
                          DCMF_SEQUENTIAL_CONSISTENCY,
                          target,
                          count[0],
                          &A1D_Memregion_global[A1D_Process_info.my_rank],
                          &A1D_Memregion_global[target],
                          src_disp,
                          dst_disp,
                          A1D_Nocallback);
        A1U_ERR_POP(result, "Put returned with an error \n");

        A1D_Connection_put_active[target]++;

    }

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}

int A1D_PutS(int target,
             void* source_ptr,
             int *src_stride_ar,
             void* target_ptr,
             int *trg_stride_ar,
             int *count,
             int stride_levels)
{
    DCMF_Result result = DCMF_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if (count[0] >= a1_settings.direct_noncontig_threshold)
    {

        result = A1DI_Direct_puts(target,
                                  source_ptr,
                                  src_stride_ar,
                                  target_ptr,
                                  trg_stride_ar,
                                  count,
                                  stride_levels);
        A1U_ERR_POP(result, "Direct puts function returned with an error \n");

        if (a1_settings.enable_immediate_flush)
        {
            A1DI_Put_flush(target);
        }
        else
        {
            A1DI_Put_flush_local(target);
        }

    }
    else
    {

        result = A1DI_Packed_puts(target,
                                  source_ptr,
                                  src_stride_ar,
                                  target_ptr,
                                  trg_stride_ar,
                                  count,
                                  stride_levels,
                                  0);
        A1U_ERR_POP(result, "Packed puts function returned with an error \n");

        if (a1_settings.enable_immediate_flush)
        {
            A1DI_Send_flush(target);
        }

    }

  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}
