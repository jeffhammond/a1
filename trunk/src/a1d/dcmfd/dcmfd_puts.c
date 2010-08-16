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

    A1DI_Free(buffer_info->buffer_ptr);
    A1DI_Free((void *) buffer_info);
}

DCMF_Request_t* A1DI_RecvSend_packedputs_callback(void *clientdata,
                                                  const DCQuad *msginfo,
                                                  unsigned count, /* TODO: this is not used */
                                                  size_t peer,
                                                  size_t sndlen,
                                                  size_t *rcvlen,
                                                  char **rcvbuf,
                                                  DCMF_Callback_t *cb_done)
{
    int result = 0;
    A1D_Buffer_info_t *buffer_info;

    result = A1DI_Malloc_aligned((void **) &buffer_info,
                                 sizeof(A1D_Buffer_info_t));
    A1U_ERR_ABORT(result != 0,
                  "A1DI_Malloc_aligned failed in A1DI_RecvSend_packedputs_callback\n");

    *rcvlen = sndlen;
    result = A1DI_Malloc_aligned((void **) rcvbuf, sndlen);
    A1U_ERR_ABORT(result != 0,
                  "A1DI_Malloc_aligned failed in A1DI_RecvSend_packedputs_callback\n");

    buffer_info->buffer_ptr = (void *) *rcvbuf;

    cb_done->function = A1DI_RecvDone_packedputs_callback;
    cb_done->clientdata = (void *) buffer_info;

    return &(buffer_info->request);
}

void A1DI_RecvSendShort_packedputs_callback(void *clientdata,
                                            const DCQuad *msginfo,
                                            unsigned count, /* TODO: this is not used */
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

    conf.protocol = DCMF_DEFAULT_SEND_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    conf.cb_recv_short = A1DI_RecvSendShort_packedputs_callback;
    conf.cb_recv_short_clientdata = NULL;
    conf.cb_recv = A1DI_RecvSend_packedputs_callback;
    conf.cb_recv_clientdata = NULL;

    result = DCMF_Send_register(&A1D_Packed_puts_protocol, &conf);
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "DCMF_Send_register returned with error %d \n",
                result);

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail:  
    goto fn_exit;
}

int A1DI_Packed_puts(int target,
                     int stride_level,
                     int *block_sizes,
                     void* source_ptr,
                     int *src_stride_ar,
                     void* target_ptr,
                     int *trg_stride_ar)
{

    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t request;
    DCMF_Callback_t callback;
    void *packet;
    int size_packet;
    volatile int active;

    A1U_FUNC_ENTER();

    result = A1DI_Pack_strided(&packet,
                               &size_packet,
                               stride_level,
                               block_sizes,
                               source_ptr,
                               src_stride_ar,
                               target_ptr,
                               trg_stride_ar);
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "A1DI_Pack_strided returned with an error\n");

    callback.function = A1DI_Generic_done;
    callback.clientdata = (void *) &active;
    active = 1;

    result = DCMF_Send(&A1D_Packed_puts_protocol,
                       &request,
                       callback,
                       DCMF_SEQUENTIAL_CONSISTENCY,
                       target,
                       size_packet,
                       packet,
                       NULL,
                       0);
    A1U_ERR_POP(result != DCMF_SUCCESS, "DCMF_Send returned with an error \n");

    A1D_Connection_send_active[target]++;
    A1DI_Conditional_advance(active > 0);

  fn_exit:
    A1DI_Free(packet); 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;

}

int A1DI_Direct_puts(int target,
                     int stride_level,
                     int *block_sizes,
                     void* source_ptr,
                     int *src_stride_ar,
                     void* target_ptr,
                     int *trg_stride_ar)
{
    int result = A1_SUCCESS;
    A1D_Request_t *a1_request;
    int i, size;
    size_t src_disp, dst_disp;

    A1U_FUNC_ENTER();

    if (stride_level > 0)
    {

        for (i = 0; i < block_sizes[stride_level]; i++)
        {
            A1DI_Direct_puts(target,
                             stride_level - 1,
                             block_sizes,
                             (void *) ((size_t) source_ptr + i * src_stride_ar[stride_level - 1]),
                             src_stride_ar,
                             (void *) ((size_t) target_ptr + i * trg_stride_ar[stride_level - 1]),
                             trg_stride_ar);
        }

    }
    else
    {

        a1_request = A1DI_Get_request();

        src_disp = (size_t) source_ptr
                 - (size_t) A1D_Membase_global[A1D_Process_info.my_rank];
        dst_disp = (size_t) target_ptr
                 - (size_t) A1D_Membase_global[target];

        result = DCMF_Put(&A1D_Generic_put_protocol,
                          &(a1_request->request),
                          A1D_Nocallback,
                          DCMF_SEQUENTIAL_CONSISTENCY,
                          target,
                          block_sizes[0],
                          &A1D_Memregion_global[A1D_Process_info.my_rank],
                          &A1D_Memregion_global[target],
                          src_disp,
                          dst_disp,
                          A1D_Nocallback);
        A1U_ERR_POP(result, "DCMF_Put returned with an error \n");

        A1D_Connection_put_active[target]++;

    }

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}

int A1D_PutS(int target,
             int stride_level,
             int *block_sizes,
             void* source_ptr,
             int *src_stride_ar,
             void* target_ptr,
             int *trg_stride_ar)
{
    DCMF_Result result = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if (block_sizes[0] >= a1_settings.direct_noncontig_put_threshold)
    {

        result = A1DI_Direct_puts(target,
                                  stride_level,
                                  block_sizes,
                                  source_ptr,
                                  src_stride_ar,
                                  target_ptr,
                                  trg_stride_ar);
        A1U_ERR_POP(result, "A1DI_Direct_puts returned with an error \n");

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
                                  stride_level,
                                  block_sizes,
                                  source_ptr,
                                  src_stride_ar,
                                  target_ptr,
                                  trg_stride_ar);
        A1U_ERR_POP(result, "A1DI_Packed_puts returned with an error \n");

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
