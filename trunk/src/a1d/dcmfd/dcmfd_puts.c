/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

char* A1DI_Pack_data(void *pointer, void *source_ptr, int *src_stride_ar,\
        int *count, int stride_level)
{
    int result = A1_SUCCESS;
    int i, size;

    A1U_FUNC_ENTER();

    if(stride_level > 0) {
         for(i=0; i<count[stride_level]; i++)
         {
            pointer = A1DI_Pack_data(pointer, (void *) ((size_t)source_ptr + i*src_stride_ar[stride_level]), src_stride_ar,\
                                count, stride_level-1);
         }
    } else {
         memcpy( pointer, source_ptr, count[0]);
         pointer = (void *)((size_t)pointer + count[0]);
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return pointer;

  fn_fail:
    goto fn_exit;
}

int A1DI_Pack(void **packet, int *size_packet, void *source_ptr, int *src_stride_ar, void *target_ptr,\
        int *trg_stride_ar, int *count, int stride_levels)
{
    int result = A1_SUCCESS;
    int i, size_data;
    void *temp; 
    A1D_Pack_header_t header; 
 
    A1U_FUNC_ENTER();       

    size_data = count[0];
    for(i=1; i<=stride_levels; i++)  size_data *= count[i];
    *size_packet = sizeof(A1D_Pack_header_t) + size_data; 

    result = posix_memalign(packet, 64, *size_packet);  
    A1U_ERR_POP(result,"packet allocation failed \n"); 

    /*Copying header information*/
    header.vaddress = target_ptr;
    header.stride_levels = stride_levels;
    memcpy(header.src_stride_ar, src_stride_ar, stride_levels*sizeof(uint32_t));
    memcpy(header.trg_stride_ar, trg_stride_ar, stride_levels*sizeof(uint32_t));
    memcpy(header.count, count, (stride_levels+1)*sizeof(uint32_t));
    memcpy(*packet, &header, sizeof(A1D_Pack_header_t));

    A1D_Pack_header_t *hdr_ptr;
    hdr_ptr = (A1D_Pack_header_t *) *packet;
 
    /*Copying data*/
    temp = (void *) ((size_t)(*packet) + sizeof(A1D_Pack_header_t));
    A1DI_Pack_data(temp, source_ptr, src_stride_ar, count, stride_levels);

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
} 

int A1D_PutS(int target, void* source_ptr, int *src_stride_ar, void* target_ptr,\
         int *trg_stride_ar, int *count, int stride_levels) 
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t request;
    DCMF_Callback_t callback;
    DCQuad msginfo;
    int active, size_packet;
    void *packet;
    unsigned src_disp, dst_disp;
 
    A1U_FUNC_ENTER();

    DCMF_CriticalSection_enter (0);

    callback.function = A1DI_Generic_callback;
    callback.clientdata = (void *) &active;

    result = A1DI_Pack(&packet, &size_packet, source_ptr, src_stride_ar, target_ptr, trg_stride_ar, count,\
          stride_levels); 
    A1U_ERR_POP(result,"Pack function returned with an error \n");    

    active = 1;
    result = DCMF_Send(&A1D_Send_noncontigput_info.protocol,
                      &request,
                      callback,
                      DCMF_SEQUENTIAL_CONSISTENCY,
                      target,  
                      size_packet,
                      packet,
                      &msginfo,
                      1);
    A1U_ERR_POP(result,"Send returned with an error \n");
    while (active > 0) DCMF_Messager_advance(); 

  fn_exit:
    DCMF_CriticalSection_exit (0);
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}
