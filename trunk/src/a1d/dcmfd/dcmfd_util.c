#include "dcmfdimpl.h"

A1D_Control_xchange_info_t A1D_Control_xchange_info;

void A1DI_Generic_done(void *clientdata, DCMF_Error_t *error)
{
    --(*((uint32_t *) clientdata));
}

void A1DI_Free_done(void *clientdata, DCMF_Error_t *error)
{
    A1D_Buffer_info_t *buffer_info = (A1D_Buffer_info_t *) clientdata;
    A1DI_Free(buffer_info->buffer_ptr);
    A1DI_Free((void *) buffer_info);
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

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

void* A1DI_Unpack_data_strided(void *pointer,
                               void *trg_ptr,
                               int *trg_stride_ar,
                               int *count,
                               int stride_level)
{
    int i;

    A1U_FUNC_ENTER();

    if (stride_level > 0)
    {
        for (i = 0; i < count[stride_level]; i++)
        {
            pointer = A1DI_Unpack_data_strided(pointer,
                                               (void *) ((size_t) trg_ptr + i * trg_stride_ar[stride_level - 1]),
                                               trg_stride_ar,
                                               count,
                                               stride_level - 1);
        }
    }
    else
    {
        memcpy(trg_ptr, pointer, count[0]);
        pointer = (void *) ((size_t) pointer + count[0]);
    }

    fn_exit: A1U_FUNC_EXIT();
    return pointer;

    fn_fail: goto fn_exit;
}

int A1DI_Unpack_strided(void *packet)
{
    int result = A1_SUCCESS;
    void *temp;
    A1D_Packed_puts_header_t *header;

    A1U_FUNC_ENTER();

    header = (A1D_Packed_puts_header_t *) packet;

    /*Unpacking and Copying data*/
    temp = (void *) ((size_t) packet + sizeof(A1D_Packed_puts_header_t));
    A1DI_Unpack_data_strided(temp,
                             header->target_ptr,
                             header->trg_stride_ar,
                             header->count,
                             header->stride_levels);

    if (header->is_getresponse)
    {
        A1D_Expecting_getresponse = 0;
    }

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}

void* A1DI_Pack_data_strided(void *pointer,
                             void *source_ptr,
                             int *src_stride_ar,
                             int *count,
                             int stride_level)
{
    int i, size;

    A1U_FUNC_ENTER();

    if (stride_level > 0)
    {
        for (i = 0; i < count[stride_level]; i++)
        {
            pointer = A1DI_Pack_data_strided(pointer,
                                             (void *) ((size_t) source_ptr + i * src_stride_ar[stride_level - 1]),
                                             src_stride_ar,
                                             count,
                                             stride_level - 1);
        }
    }
    else
    {
        memcpy(pointer, source_ptr, count[0]);
        pointer = (void *) ((size_t) pointer + count[0]);
    }

    fn_exit: A1U_FUNC_EXIT();
    return pointer;

    fn_fail: goto fn_exit;
}

int A1DI_Pack_strided(void **packet,
                      int *size_packet,
                      void *source_ptr,
                      int *src_stride_ar,
                      void *target_ptr,
                      int *trg_stride_ar,
                      int *count,
                      int stride_levels,
                      int is_getresponse)
{
    int result = A1_SUCCESS;
    int i, size_data;
    void *temp;
    A1D_Packed_puts_header_t header;

    A1U_FUNC_ENTER();

    size_data = count[0];
    for (i = 1; i <= stride_levels; i++)
        size_data *= count[i];
    *size_packet = sizeof(A1D_Packed_puts_header_t) + size_data;

    result = A1DI_Malloc_aligned(packet, *size_packet);
    A1U_ERR_POP(result != A1_SUCCESS, "A1DI_Malloc_aligned failed while allocating packet\
               in A1DI_Pack_strided\n");

    /*Copying header information*/
    header.target_ptr = target_ptr;
    header.stride_levels = stride_levels;
    memcpy(header.trg_stride_ar, trg_stride_ar, stride_levels * sizeof(uint32_t));
    memcpy(header.count, count, (stride_levels + 1) * sizeof(uint32_t));
    header.is_getresponse = is_getresponse;
    memcpy(*packet, &header, sizeof(A1D_Packed_puts_header_t));

    /*Copying data*/
    temp = (void *) ((size_t)(*packet) + sizeof(A1D_Packed_puts_header_t));
    A1DI_Pack_data_strided(temp,
                           source_ptr,
                           src_stride_ar,
                           count,
                           stride_levels);

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}

void* A1DI_Unpack_data_strided_acc(void *pointer,
                                   void *trg_ptr,
                                   int *trg_stride_ar,
                                   int *count,
                                   int stride_level,
                                   A1_datatype_t a1_type,
                                   void* scaling)
{
    int i;

    A1U_FUNC_ENTER();

    if (stride_level > 0)
    {
        for (i = 0; i < count[stride_level]; i++)
        {
            pointer = A1DI_Unpack_data_strided_acc(pointer,
                                                   (void *) ((size_t) trg_ptr + i * trg_stride_ar[stride_level - 1]),
                                                   trg_stride_ar,
                                                   count,
                                                   stride_level - 1,
                                                   a1_type,
                                                   scaling);
        }
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
                             count[0] / sizeof(int32_t));
            break;
        case A1_INT64:
            A1DI_ACC_EXECUTE(int64_t,
                             pointer,
                             trg_ptr,
                             *((int64_t *) scaling),
                             count[0] / sizeof(int64_t));
            break;
        case A1_UINT32:
            A1DI_ACC_EXECUTE(uint32_t,
                             pointer,
                             trg_ptr,
                             *((uint32_t *) scaling),
                             count[0] / sizeof(uint32_t));
            break;
        case A1_UINT64:
            A1DI_ACC_EXECUTE(uint64_t,
                             pointer,
                             trg_ptr,
                             *((uint64_t *) scaling),
                             count[0] / sizeof(uint64_t));
            break;
        case A1_FLOAT:
            A1DI_ACC_EXECUTE(float, 
                             pointer, 
                             trg_ptr, 
                             *((float *) scaling),
                             count[0]/sizeof(float));
            break;
        case A1_DOUBLE:
            A1DI_ACC_EXECUTE(double, 
                             pointer, 
                             trg_ptr, 
                             *((double *) scaling),
                             count[0]/sizeof(double));
            break;
        default:
            A1U_ERR_ABORT(A1_ERROR,
                          "Invalid datatype received in Putacc operation \n");
            break;
        }
        pointer = (void *) ((size_t) pointer + count[0]);
    }

    fn_exit: A1U_FUNC_EXIT();
    return pointer;

    fn_fail: goto fn_exit;
}

int A1DI_Unpack_strided_putaccs(void *packet)
{
    int result = A1_SUCCESS;
    void *temp;
    A1D_Packed_putaccs_header_t *header;

    A1U_FUNC_ENTER();

    header = (A1D_Packed_putaccs_header_t *) packet;

    /*Unpacking and Copying data*/
    temp = (void *) ((size_t) packet + sizeof(A1D_Packed_putaccs_header_t));
    A1DI_Unpack_data_strided_acc(temp,
                                 header->target_ptr,
                                 header->trg_stride_ar,
                                 header->count,
                                 header->stride_levels,
                                 header->datatype,
                                 (void *) &(header->scaling));

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}

int A1DI_Pack_strided_putaccs(void **packet,
                              int *size_packet,
                              void *source_ptr,
                              int *src_stride_ar,
                              void *target_ptr,
                              int *trg_stride_ar,
                              int *count,
                              int stride_levels,
                              A1_datatype_t a1_type,
                              void *scaling)
{
    int result = A1_SUCCESS;
    int i, size_data;
    void *temp;
    A1D_Packed_putaccs_header_t header;

    A1U_FUNC_ENTER();

    size_data = count[0];
    for (i = 1; i <= stride_levels; i++) size_data *= count[i];
    *size_packet = sizeof(A1D_Packed_putaccs_header_t) + size_data;

    result = A1DI_Malloc_aligned(packet, *size_packet);
    A1U_ERR_POP(result != A1_SUCCESS, "A1DI_Malloc_aligned while allocating packet in A1DI_Pack_strided_putaccs\n");

    /*Copying header information*/
    header.target_ptr = target_ptr;
    header.stride_levels = stride_levels;
    memcpy(header.trg_stride_ar, trg_stride_ar, stride_levels * sizeof(uint32_t));
    memcpy(header.count, count, (stride_levels + 1) * sizeof(uint32_t));
    header.datatype = a1_type;
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
    case A1_DOUBLE:
        memcpy((void *) &(header.scaling), scaling, sizeof(double));
        break;
    default:
        A1U_ERR_POP(result, "Invalid a1_type received in Putacc operation \n");
        break;
    }
    memcpy(*packet, &header, sizeof(A1D_Packed_putaccs_header_t));

    /*Copying data*/
    temp = (void *) ((size_t)(*packet) + sizeof(A1D_Packed_putaccs_header_t));
    A1DI_Pack_data_strided(temp,
                           source_ptr,
                           src_stride_ar,
                           count,
                           stride_levels);

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}
