/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

A1D_Control_xchange_info_t A1D_Control_xchange_info;

void A1DI_Generic_done(void *clientdata, DCMF_Error_t *error)
{
    --(*((uint32_t *) clientdata));
}

void A1DI_Request_done(void *clientdata, DCMF_Error_t *error)
{
    A1D_Handle_t *a1d_handle;
    A1D_Request_t *a1d_request = (A1D_Request_t *) clientdata;

    if(a1d_request->handle_ptr != NULL) 
    {
       a1d_handle = a1d_request->handle_ptr;
       --(a1d_handle->active);
    }
 
    A1DI_Release_request(a1d_request);
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

int A1DI_Control_xchange_initialize()
{
    int status = A1_SUCCESS;
    DCMF_Control_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_CONTROL_PROTOCOL;
    conf.network = DCMF_DEFAULT_NETWORK;
    conf.cb_recv = A1DI_Control_xchange_callback;
    conf.cb_recv_clientdata = (void *) &A1D_Control_xchange_info.rcv_active;

    status = DCMF_Control_register(&A1D_Control_xchange_info.protocol, &conf);
    A1U_ERR_POP(status != DCMF_SUCCESS,
                "Control xchange registartion returned with error %d \n",
                status);

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

void* A1DI_Unpack_data_strided(void *pointer,
                               int stride_level,
                               int *block_sizes,
                               void *trg_ptr,
                               int *trg_stride_ar)
{
    int i;

    A1U_FUNC_ENTER();

    if (stride_level > 0)
    {
        for (i = 0; i < block_sizes[stride_level]; i++)
        {
            pointer = A1DI_Unpack_data_strided(pointer,
                                               stride_level - 1,
                                               block_sizes,
                                               (void *) ((size_t) trg_ptr + i * trg_stride_ar[stride_level - 1]),
                                               trg_stride_ar);
        }
    }
    else
    {
        memcpy(trg_ptr, pointer, block_sizes[0]);
        pointer = (void *) ((size_t) pointer + block_sizes[0]);
    }

  fn_exit: 
    A1U_FUNC_EXIT();
    return pointer;

  fn_fail: 
    goto fn_exit;
}

int A1DI_Unpack_strided(void *packet)
{
    int status = A1_SUCCESS;
    void *temp;
    A1D_Packed_puts_header_t *header;

    A1U_FUNC_ENTER();

    header = (A1D_Packed_puts_header_t *) packet;

    /*Unpacking and Copying data*/
    temp = (void *) ((size_t) packet + sizeof(A1D_Packed_puts_header_t));
    A1DI_Unpack_data_strided(temp,
                             header->stride_level,
                             header->block_sizes,
                             header->target_ptr,
                             header->trg_stride_ar);

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

void* A1DI_Pack_data_strided(void *pointer,
                             int stride_level,
                             int *block_sizes,
                             void *source_ptr,
                             int *src_stride_ar)
{
    int i, size;

    A1U_FUNC_ENTER();

    if (stride_level > 0)
    {
        for (i = 0; i < block_sizes[stride_level]; i++)
        {
            pointer = A1DI_Pack_data_strided(pointer,
                                             stride_level - 1,
                                             block_sizes,
                                             (void *) ((size_t) source_ptr + i * src_stride_ar[stride_level - 1]),
                                             src_stride_ar);
        }
    }
    else
    {
        memcpy(pointer, source_ptr, block_sizes[0]);
        pointer = (void *) ((size_t) pointer + block_sizes[0]);
    }

  fn_exit: 
    A1U_FUNC_EXIT();
    return pointer;

  fn_fail: 
    goto fn_exit;
}

int A1DI_Pack_strided(void **packet,
                      int *size_packet,
                      int stride_level,
                      int *block_sizes,
                      void *source_ptr,
                      int *src_stride_ar,
                      void *target_ptr,
                      int *trg_stride_ar)
{
    int status = A1_SUCCESS;
    int i, size_data;
    void *temp;
    A1D_Packed_puts_header_t header;

    A1U_FUNC_ENTER();

    size_data = block_sizes[0];
    for (i = 1; i <= stride_level; i++)
        size_data *= block_sizes[i];
    *size_packet = sizeof(A1D_Packed_puts_header_t) + size_data;

    status = A1DI_Malloc_aligned(packet, *size_packet);
    A1U_ERR_POP(status != A1_SUCCESS,
                "A1DI_Malloc_aligned failed while allocating packet\
               in A1DI_Pack_strided\n");

    /*Copying header information*/
    header.target_ptr = target_ptr;
    header.stride_level = stride_level;
    memcpy(header.trg_stride_ar, trg_stride_ar, stride_level * sizeof(uint32_t));
    memcpy(header.block_sizes, block_sizes, (stride_level + 1) * sizeof(uint32_t));
    memcpy(*packet, &header, sizeof(A1D_Packed_puts_header_t));

    /*Copying data*/
    temp = (void *) ((size_t)(*packet) + sizeof(A1D_Packed_puts_header_t));
    A1DI_Pack_data_strided(temp,
                           stride_level,
                           block_sizes,
                           source_ptr,
                           src_stride_ar);

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

void* A1DI_Unpack_data_strided_acc(void *pointer,
                                   int stride_level,
                                   int *block_sizes,
                                   void *trg_ptr,
                                   int *trg_stride_ar,
                                   A1_datatype_t a1_type,
                                   void* scaling)
{
    int i;

    A1U_FUNC_ENTER();

    if (stride_level > 0)
    {
        for (i = 0; i < block_sizes[stride_level]; i++)
        {
            pointer = A1DI_Unpack_data_strided_acc(pointer,
                                                   stride_level - 1,
                                                   block_sizes,
                                                   (void *) ((size_t) trg_ptr + i * trg_stride_ar[stride_level - 1]),
                                                   trg_stride_ar,
                                                   a1_type,
                                                   scaling);
        }
    }
    else
    {
        likely_if(a1_type == A1_DOUBLE) 
        {
            A1DI_ACC_EXECUTE(double,
                             pointer,
                             trg_ptr,
                             *((double *) scaling),
                             block_sizes[0]/sizeof(double));
        }
        else
        {
            switch (a1_type)
            {
            case A1_INT32:
                A1DI_ACC_EXECUTE(int32_t,
                                 pointer,
                                 trg_ptr,
                                 *((int32_t *) scaling),
                                 block_sizes[0] / sizeof(int32_t));
                break;
            case A1_INT64:
                A1DI_ACC_EXECUTE(int64_t,
                                 pointer,
                                 trg_ptr,
                                 *((int64_t *) scaling),
                                 block_sizes[0] / sizeof(int64_t));
                break;
            case A1_UINT32:
                A1DI_ACC_EXECUTE(uint32_t,
                                 pointer,
                                 trg_ptr,
                                 *((uint32_t *) scaling),
                                 block_sizes[0] / sizeof(uint32_t));
                break;
            case A1_UINT64:
                A1DI_ACC_EXECUTE(uint64_t,
                                 pointer,
                                 trg_ptr,
                                 *((uint64_t *) scaling),
                                 block_sizes[0] / sizeof(uint64_t));
                break;
            case A1_FLOAT:
                A1DI_ACC_EXECUTE(float,
                        pointer,
                        trg_ptr,
                        *((float *) scaling),
                        block_sizes[0]/sizeof(float));
                break;
            default:
                A1U_ERR_ABORT(A1_ERROR,
                              "Invalid datatype received in Putacc operation \n");
                break;
            }
        }
        pointer = (void *) ((size_t) pointer + block_sizes[0]);
    }

  fn_exit: 
    A1U_FUNC_EXIT();
    return pointer;

  fn_fail: 
    goto fn_exit;
}

int A1DI_Unpack_strided_putaccs(void *packet)
{
    int status = A1_SUCCESS;
    void *temp;
    A1D_Packed_putaccs_header_t *header;

    A1U_FUNC_ENTER();

    header = (A1D_Packed_putaccs_header_t *) packet;

    /*Unpacking and Copying data*/
    temp = (void *) ((size_t) packet + sizeof(A1D_Packed_putaccs_header_t));
    A1DI_Unpack_data_strided_acc(temp,
                                 header->stride_level,
                                 header->block_sizes,
                                 header->target_ptr,
                                 header->trg_stride_ar,
                                 header->datatype,
                                 (void *) &(header->scaling));

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1DI_Pack_strided_putaccs(void **packet,
                              int *size_packet,
                              int stride_level,
                              int *block_sizes,
                              void *source_ptr,
                              int *src_stride_ar,
                              void *target_ptr,
                              int *trg_stride_ar,
                              A1_datatype_t a1_type,
                              void *scaling)
{
    int status = A1_SUCCESS;
    int i, size_data;
    void *temp;
    A1D_Packed_putaccs_header_t header;

    A1U_FUNC_ENTER();

    size_data = block_sizes[0];
    for (i = 1; i <= stride_level; i++) size_data *= block_sizes[i];
    *size_packet = sizeof(A1D_Packed_putaccs_header_t) + size_data;

    status = A1DI_Malloc_aligned(packet, *size_packet);
    A1U_ERR_POP(status != A1_SUCCESS,
                "A1DI_Malloc_aligned while allocating packet in A1DI_Pack_strided_putaccs\n");

    /*Copying header information*/
    header.target_ptr = target_ptr;
    header.stride_level = stride_level;
    memcpy(header.trg_stride_ar, trg_stride_ar, stride_level * sizeof(uint32_t));
    memcpy(header.block_sizes, block_sizes, (stride_level + 1) * sizeof(uint32_t));
    header.datatype = a1_type;
    likely_if(a1_type == A1_DOUBLE) 
    {
        memcpy((void *) &(header.scaling), scaling, sizeof(double));
    }
    else
    {
        switch (a1_type)
        {
        case A1_INT32:
            memcpy((void *) &(header.scaling), scaling, sizeof(int32_t));
            break;
        case A1_INT64:
            memcpy((void *) &(header.scaling), scaling, sizeof(int64_t));
            break;
        case A1_UINT32:
            memcpy((void *) &(header.scaling), scaling, sizeof(uint32_t));
            break;
        case A1_UINT64:
            memcpy((void *) &(header.scaling), scaling, sizeof(uint64_t));
            break;
        case A1_FLOAT:
            memcpy((void *) &(header.scaling), scaling, sizeof(float));
            break;
        default:
            A1U_ERR_POP(status, "Invalid a1_type received in Putacc operation \n");
            break;
        }
    }
    memcpy(*packet, &header, sizeof(A1D_Packed_putaccs_header_t));

    /*Copying data*/
    temp = (void *) ((size_t)(*packet) + sizeof(A1D_Packed_putaccs_header_t));
    A1DI_Pack_data_strided(temp,
                           stride_level,
                           block_sizes,
                           source_ptr,
                           src_stride_ar);

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}
