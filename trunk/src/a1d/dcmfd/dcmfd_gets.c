/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Protocol_t A1D_Packed_gets_protocol;
volatile int A1D_Expecting_getresponse;

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

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

int A1DI_Packed_gets(int target,
                     void* source_ptr,
                     int *src_stride_ar,
                     void* target_ptr,
                     int *trg_stride_ar,
                     int *count,
                     int stride_levels)
{

    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t *request;
    A1D_Packed_gets_header_t packet;
    volatile int active;

    A1U_FUNC_ENTER();

    /*Copying header information*/
    packet.target = A1D_Process_info.my_rank;
    packet.source_ptr = source_ptr;
    packet.target_ptr = target_ptr;
    packet.stride_levels = stride_levels;
    memcpy(packet.src_stride_ar, src_stride_ar, stride_levels * sizeof(uint32_t));
    memcpy(packet.trg_stride_ar, trg_stride_ar, stride_levels * sizeof(uint32_t));
    memcpy(packet.count, count, (stride_levels + 1) * sizeof(uint32_t));

    request = A1DI_Get_request();

    result = DCMF_Send(&A1D_Packed_gets_protocol,
                       request,
                       A1D_Nocallback,
                       DCMF_SEQUENTIAL_CONSISTENCY,
                       target,
                       sizeof(A1D_Packed_gets_header_t),
                       (void *) &packet,
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

int A1DI_Direct_gets(int target,
                     void* source_ptr,
                     int *src_stride_ar,
                     void* target_ptr,
                     int *trg_stride_ar,
                     int* count,
                     int stride_level,
                     volatile int *get_active)
{
    int result = A1_SUCCESS;
    DCMF_Request_t *request;
    DCMF_Callback_t callback;
    int i, size;
    size_t src_disp, dst_disp;

    A1U_FUNC_ENTER();

    if (stride_level > 0)
    {

        for (i = 0; i < count[stride_level]; i++)
        {
            A1DI_Direct_gets(target,
                             (void *) ((size_t) source_ptr + i
                                     * src_stride_ar[stride_level - 1]),
                             src_stride_ar,
                             (void *) ((size_t) target_ptr + i
                                     * trg_stride_ar[stride_level - 1]),
                             trg_stride_ar,
                             count,
                             stride_level - 1,
                             get_active);
        }

    }
    else
    {

        request = A1DI_Get_request();

        src_disp = (size_t) source_ptr - (size_t) A1D_Membase_global[target];
        dst_disp = (size_t) target_ptr - (size_t) A1D_Membase_global[A1D_Process_info.my_rank];

        *get_active = *get_active + 1;
        callback.function = A1DI_Generic_done;
        callback.clientdata = (void *) get_active; 

        result = DCMF_Get(&A1D_Generic_get_protocol,
                          request,
                          callback,
                          DCMF_SEQUENTIAL_CONSISTENCY,
                          target,
                          count[0],
                          &A1D_Memregion_global[target],
                          &A1D_Memregion_global[A1D_Process_info.my_rank],
                          src_disp,
                          dst_disp);
        A1U_ERR_POP(result, "DCMF_Get returned with an error \n");

        A1D_Connection_send_active[target]++;
    }

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}

int A1D_GetS(int target,
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

        volatile int get_active = 0;

        get_active = 0;

        result = A1DI_Direct_gets(target,
                                  source_ptr,
                                  src_stride_ar,
                                  target_ptr,
                                  trg_stride_ar,
                                  count,
                                  stride_levels,
                                  &get_active);
        A1U_ERR_POP(result, "A1DI_Direct_gets returned with an error \n");

        A1DI_Conditional_advance(get_active > 0);

    }
    else
    {

        A1D_Expecting_getresponse = 1;

        result = A1DI_Packed_gets(target,
                                  source_ptr,
                                  src_stride_ar,
                                  target_ptr,
                                  trg_stride_ar,
                                  count,
                                  stride_levels);
        A1U_ERR_POP(result, "A1DI_Packed_gets returned with an error \n");

        A1DI_Conditional_advance(A1D_Expecting_getresponse > 0);

    }

  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}
