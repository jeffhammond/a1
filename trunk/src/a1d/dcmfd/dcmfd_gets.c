/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

uint32_t A1D_Expecting_getresponse;
volatile uint32_t get_active;
DCMF_Callback_t get_callback;

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
    int active;

    A1U_FUNC_ENTER();

    /*Copying header information*/
    packet.target = A1D_Process_info.my_rank;
    packet.source_ptr = source_ptr;
    packet.target_ptr = target_ptr;
    packet.stride_levels = stride_levels;
    memcpy(packet.src_stride_ar, src_stride_ar, stride_levels * sizeof(uint32_t));
    memcpy(packet.trg_stride_ar, trg_stride_ar, stride_levels * sizeof(uint32_t));
    memcpy(packet.count, count, (stride_levels + 1) * sizeof(uint32_t));

    /* TODO: will this be thread-safe in A1_THREAD_MULTIPLE mode??? */
    request = A1DI_Get_request();

    A1DI_CRITICAL_ENTER();
    result = DCMF_Send(&A1D_Packed_gets_protocol,
                       request,
                       A1D_Nocallback,
                       DCMF_SEQUENTIAL_CONSISTENCY,
                       target,
                       sizeof(A1D_Packed_gets_header_t),
                       (void *) &packet,
                       NULL,
                       0);
    A1DI_CRITICAL_EXIT();
    A1U_ERR_POP(result, "Send returned with an error \n");

    /* TODO: will this be thread-safe in A1_THREAD_MULTIPLE mode??? */
    A1D_Connection_send_active[target]++;

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;

}

int A1DI_Direct_gets(int target,
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
            A1DI_Direct_gets(target,
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

        /* TODO: will this be thread-safe in A1_THREAD_MULTIPLE mode??? */
        request = A1DI_Get_request();

        src_disp = (size_t) source_ptr - (size_t) A1D_Membase_global[target];
        dst_disp = (size_t) target_ptr - (size_t) A1D_Membase_global[A1D_Process_info.my_rank];

        get_active = get_active + 1;
        A1DI_CRITICAL_ENTER();
        result = DCMF_Get(&A1D_Generic_get_protocol,
                          request,
                          get_callback,
                          DCMF_SEQUENTIAL_CONSISTENCY,
                          target,
                          count[0],
                          &A1D_Memregion_global[target],
                          &A1D_Memregion_global[A1D_Process_info.my_rank],
                          src_disp,
                          dst_disp);
        A1DI_CRITICAL_EXIT();
        A1U_ERR_POP(result, "DCMF_Get returned with an error \n");

        /* TODO: will this be thread-safe in A1_THREAD_MULTIPLE mode??? */
        A1D_Connection_send_active[target]++;
    }

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
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

    if (count[0] >= a1_direct_noncontig_threshold)
    {

        get_active = 0;
        get_callback.function = A1DI_Generic_done;
        get_callback.clientdata = (void *) &get_active;

        result = A1DI_Direct_gets(target,
                                  source_ptr,
                                  src_stride_ar,
                                  target_ptr,
                                  trg_stride_ar,
                                  count,
                                  stride_levels);
        A1U_ERR_POP(result, "A1DI_Direct_gets returned with an error \n");
        while (get_active > 0) A1DI_Advance();

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
        while (A1D_Expecting_getresponse > 0) A1DI_Advance();

    }

    fn_exit: A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}
