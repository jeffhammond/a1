/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Protocol_t A1D_Packed_putaccs_protocol;

void A1DI_RecvDone_packedputaccs_callback(void *clientdata, DCMF_Error_t *error)
{
    A1D_Buffer_info_t *buffer_info = (A1D_Buffer_info_t *) clientdata;

    A1DI_Unpack_strided_putaccs(buffer_info->buffer_ptr);

    A1DI_Free(buffer_info->buffer_ptr);
    A1DI_Free((void *) buffer_info);
}

DCMF_Request_t* A1DI_RecvSend_packedputaccs_callback(void *clientdata,
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
                  "A1DI_Malloc_aligned failed in A1DI_RecvSend_packedputaccs_callback\n");

    *rcvlen = sndlen;
    result = A1DI_Malloc_aligned((void **) rcvbuf, sndlen);
    A1U_ERR_ABORT(result != 0,
                  "A1DI_Malloc_aligned failed in A1DI_RecvSend_packedputaccs_callback\n");

    buffer_info->buffer_ptr = (void *) *rcvbuf;

    cb_done->function = A1DI_RecvDone_packedputaccs_callback;
    cb_done->clientdata = (void *) buffer_info;

    return &(buffer_info->request);
}

void A1DI_RecvSendShort_packedputaccs_callback(void *clientdata,
                                               const DCQuad *msginfo,
                                               unsigned count, /* TODO: this is not used */
                                               size_t peer,
                                               const char *src,
                                               size_t bytes)
{
    A1DI_Unpack_strided_putaccs((void *) src);
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
                "DCMF_Send_register returned with error %d \n",
                result);

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}

int A1DI_Packed_putaccs(int target,
                        int stride_level,
                        int *block_sizes,
                        void* source_ptr,
                        int *src_stride_ar,
                        void* target_ptr,
                        int *trg_stride_ar,
                        A1_datatype_t a1_type,
                        void *scaling,
                        A1D_Handle_t *a1d_handle)
{

    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Callback_t done_callback;
    void *packet;
    int size_packet;

    A1U_FUNC_ENTER();

    result = A1DI_Pack_strided_putaccs(&packet,
                                       &size_packet,
                                       stride_level,
                                       block_sizes,
                                       source_ptr,
                                       src_stride_ar,
                                       target_ptr,
                                       trg_stride_ar,
                                       a1_type,
                                       scaling);
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "Pack acc function returned with an error \n");

    A1DI_Load_request(a1d_handle);
    done_callback.function = A1DI_Handle_done;
    done_callback.clientdata = (void *) a1d_handle;
    a1d_handle->active++;
    /* Assigning the packing buffer pointer in request so that it can be free when the
     * request is complete, in the callback */
    a1d_handle->request_head->request->buffer_ptr = packet;

    result = DCMF_Send(&A1D_Packed_putaccs_protocol,
                       &(a1d_handle->request_list->request),
                       done_callback,
                       DCMF_SEQUENTIAL_CONSISTENCY,
                       target,
                       size_packet,
                       packet,
                       NULL,
                       0);
    A1U_ERR_POP(result, "Send returned with an error \n");

    A1D_Connection_send_active[target]++;

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;

}

int A1DI_Direct_putaccs(int target,
                        int stride_level,
                        int *block_sizes,
                        void* source_ptr,
                        int *src_stride_ar,
                        void* target_ptr,
                        int *trg_stride_ar,
                        A1_datatype_t a1_type,
                        void *scaling,
                        A1D_Handle_t *a1d_handle)
{
    int i, result = A1_SUCCESS;
    A1D_Putacc_header_t header;
    DCMF_Callback_t done_callback;

    A1U_FUNC_ENTER();

    if (stride_level > 0)
    {

        for (i = 0; i < block_sizes[stride_level]; i++)
        {
            A1DI_Direct_putaccs(target,
                                stride_level - 1,
                                block_sizes,
                                (void *) ((size_t) source_ptr + i * src_stride_ar[stride_level - 1]),
                                src_stride_ar,
                                (void *) ((size_t) target_ptr + i * trg_stride_ar[stride_level - 1]),
                                trg_stride_ar,
                                a1_type,
                                scaling, 
                                a1d_handle);

        }

    }
    else
    {

        A1DI_Load_request(a1d_handle);
        done_callback.function = A1DI_Handle_done;
        done_callback.clientdata = (void *) a1d_handle;
        a1d_handle->active++;

        header.target_ptr = target_ptr;
        header.datatype = a1_type;
        likely_if(a1_type == A1_DOUBLE) 
        {
            (header.scaling).double_value = *((double *) scaling);
        } 
        else 
        {
            switch (a1_type)
            {
            case A1_INT32:
                (header.scaling).int32_value = *((int32_t *) scaling);
                break;
            case A1_INT64:
                (header.scaling).int64_value = *((int64_t *) scaling);
                break;
            case A1_UINT32:
                (header.scaling).uint32_value = *((uint32_t *) scaling);
                break;
            case A1_UINT64:
                (header.scaling).uint64_value = *((uint64_t *) scaling);
                break;
            case A1_FLOAT:
                (header.scaling).float_value = *((float *) scaling);
                break;
            default:
                result = A1_ERROR;
                A1U_ERR_POP((result != A1_SUCCESS),
                            "Invalid data type in putacc \n");
                break;
            }
        }

        result = DCMF_Send(&A1D_Generic_putacc_protocol,
                           &(a1d_handle->request_ptr->request),
                           done_callback,
                           DCMF_SEQUENTIAL_CONSISTENCY,
                           target,
                           block_sizes[0],
                           source_ptr,
                           (DCQuad *) &header,
                           (unsigned) 2);
        A1U_ERR_POP((result != A1_SUCCESS), "Putacc returned with an error \n");

        A1D_Connection_send_active[target]++;

    }

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}

int A1D_PutAccS(int target,
                int stride_level,
                int *block_sizes,
                void* source_ptr,
                int *src_stride_ar,
                void* target_ptr,
                int *trg_stride_ar,
                A1_datatype_t a1_type,
                void* scaling)
{
    DCMF_Result result = DCMF_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    a1d_handle = A1DI_Get_handle();

    if (block_sizes[0] >= a1_settings.direct_noncontig_putacc_threshold)
    {

        result = A1DI_Direct_putaccs(target,
                                     stride_level,
                                     block_sizes,
                                     source_ptr,
                                     src_stride_ar,
                                     target_ptr,
                                     trg_stride_ar,
                                     a1_type,
                                     scaling,
                                     a1d_handle);
        A1U_ERR_POP(result, "Direct putaccs function returned with an error \n");

    }
    else
    {

        result = A1DI_Packed_putaccs(target,
                                     stride_level,
                                     block_sizes,
                                     source_ptr,
                                     src_stride_ar,
                                     target_ptr,
                                     trg_stride_ar,
                                     a1_type,
                                     scaling,
                                     a1d_handle);
        A1U_ERR_POP(result, "Packed puts function returned with an error \n");

    }

    A1DI_Conditional_advance(a1d_handle->active > 0);

  fn_exit:
    A1DI_Release_handle(a1d_handle); 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}

int A1D_NbPutAccS(int target,
                int stride_level,
                int *block_sizes,
                void* source_ptr,
                int *src_stride_ar,
                void* target_ptr,
                int *trg_stride_ar,
                A1_datatype_t a1_type,
                void* scaling,
                A1_handle_t *a1_handle)
{
    DCMF_Result result = DCMF_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    /* Initializing handle. the handle must have been initialized using *
     * A1_Init_handle */
    if(a1_handle == NULL)
    {
      a1d_handle = A1DI_Get_handle();
      A1DI_Load_request(a1d_handle);
      A1DI_Set_user_handle(a1d_handle, a1_handle);
      *a1_handle = (A1_handle_t) a1d_handle;
    }
    else
    {
      a1d_handle = (A1D_handle_t) a1_handle;
      A1DI_Load_request(a1d_handle);
    }

    if (block_sizes[0] >= a1_settings.direct_noncontig_putacc_threshold)
    {

        result = A1DI_Direct_putaccs(target,
                                     stride_level,
                                     block_sizes,
                                     source_ptr,
                                     src_stride_ar,
                                     target_ptr,
                                     trg_stride_ar,
                                     a1_type,
                                     scaling,
                                     a1d_handle);
        A1U_ERR_POP(result, "Direct putaccs function returned with an error \n");

    }
    else
    {

        result = A1DI_Packed_putaccs(target,
                                     stride_level,
                                     block_sizes,
                                     source_ptr,
                                     src_stride_ar,
                                     target_ptr,
                                     trg_stride_ar,
                                     a1_type,
                                     scaling,
                                     a1d_handle);
        A1U_ERR_POP(result, "Packed puts function returned with an error \n");

    }

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}