/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

int A1DI_Packed_puts(int target, void* source_ptr, int *src_stride_ar, void* target_ptr,\
         int *trg_stride_ar, int *count, int stride_levels, int is_getresponse) {

    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t *request;
    DCMF_Callback_t callback;
    DCQuad msginfo;
    void *packet;
    unsigned size_packet, active;
 
    A1U_FUNC_ENTER();

    result = A1DI_Pack_strided(&packet, &size_packet, source_ptr, src_stride_ar, target_ptr, trg_stride_ar, count,\
            stride_levels, is_getresponse); 
    A1U_ERR_POP(result!=DCMF_SUCCESS,"Pack function returned with an error \n");

    request = A1DI_Get_request();
    
    /* If this put is in response to a get, we are in a callback, so we should not 
      hit advance. We set buffer address in the callback which frees it when 
      invoked */ 
    if(is_getresponse) {
      callback.function = A1DI_Free_done;
      callback.clientdata = (void *) packet;
      active = 0; 
    } else {
      callback.function = A1DI_Generic_done;
      callback.clientdata = (void *) &active;
      active = 1;
    }

    result = DCMF_Send(&A1D_Packed_puts_info.protocol,
                      request,
                      callback,
                      DCMF_SEQUENTIAL_CONSISTENCY,
                      target,  
                      size_packet,
                      packet,
                      &msginfo,
                      1);
    A1U_ERR_POP(result,"Send returned with an error \n");
   
    if(!is_getresponse) { 
      while(active > 0) A1DI_Advance();
      free(packet);
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;

}

int A1DI_Direct_puts(int target, void* source_ptr, int *src_stride_ar, void* target_ptr,\
        int *trg_stride_ar, int* count, int stride_level)  
{
    int result = A1_SUCCESS;
    DCMF_Request_t *request;
    int i, size;
    size_t src_disp, dst_disp;

    A1U_FUNC_ENTER();

    if(stride_level > 0) {

         for(i=0; i<count[stride_level]; i++)
         {
            A1DI_Direct_puts(target, (void *) ((size_t)source_ptr + i*src_stride_ar[stride_level-1]), src_stride_ar,\
                  (void *) ((size_t)target_ptr + i*trg_stride_ar[stride_level-1]), trg_stride_ar, count, stride_level-1);
         }

    } else {

         request = A1DI_Get_request();

         src_disp = (size_t)source_ptr - (size_t)A1D_Membase_global[A1D_Process_info.my_rank];
         dst_disp = (size_t)target_ptr - (size_t)A1D_Membase_global[target];

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
         A1U_ERR_POP(result,"Put returned with an error \n");
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

int A1D_PutS(int target, void* source_ptr, int *src_stride_ar, void* target_ptr,
         int *trg_stride_ar, int *count, int stride_levels) 
{
    DCMF_Result result = DCMF_SUCCESS;
 
    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();
 
    if(count[0] >= a1_direct_noncontig_threshold) {

      result = A1DI_Direct_puts(target, source_ptr, src_stride_ar, target_ptr,
          trg_stride_ar, count, stride_levels);
      A1U_ERR_POP(result,"Direct puts function returned with an error \n");

      if(a1_enable_scalefree_flush) { 
          A1DI_Put_flush(target); 
      } else {
          A1DI_Put_flush_local(target);
      }

    } else {

      result = A1DI_Packed_puts(target, source_ptr, src_stride_ar, target_ptr,
          trg_stride_ar, count, stride_levels, 0);
      A1U_ERR_POP(result,"Packed puts function returned with an error \n");   

      if(a1_enable_scalefree_flush) {
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
