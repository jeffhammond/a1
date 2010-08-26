/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Protocol_t A1D_Packed_puts_protocol;

void A1DI_RecvDone_packedputs_callback(void *clientdata, DCMF_Error_t *error)
{
    A1D_Request_t *a1d_request = (A1D_Request_t *) clientdata;

    A1DI_Unpack_strided(a1d_request->buffer_ptr);

    A1DI_Release_request(a1d_request);
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
    int status = 0;
    A1D_Request_t *a1d_request;

    a1d_request = A1DI_Get_request();
    A1U_ERR_ABORT(status = (a1d_request == NULL),
                "A1DI_Get_request returned NULL in A1DI_RecvSend_packedputaccs_callback.\n");

    *rcvlen = sndlen;
    status = A1DI_Malloc_aligned((void **) &(a1d_request->buffer_ptr), sndlen);
    A1U_ERR_ABORT(status != 0,
                  "A1DI_Malloc_aligned failed in A1DI_RecvSend_packedputs_callback\n");

    *rcvbuf = (char *) a1d_request->buffer_ptr;

    cb_done->function = A1DI_RecvDone_packedputs_callback;
    cb_done->clientdata = (void *) a1d_request;

    return &(a1d_request->request);
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

int A1DI_Packed_puts_initialize()
{
    int status = A1_SUCCESS;
    DCMF_Send_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_SEND_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    conf.cb_recv_short = A1DI_RecvSendShort_packedputs_callback;
    conf.cb_recv_short_clientdata = NULL;
    conf.cb_recv = A1DI_RecvSend_packedputs_callback;
    conf.cb_recv_clientdata = NULL;

    status = DCMF_Send_register(&A1D_Packed_puts_protocol, &conf);
    A1U_ERR_POP(status != DCMF_SUCCESS,
                "DCMF_Send_register returned with error %d \n",
                status);

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail:  
    goto fn_exit;
}

int A1DI_Packed_puts(int target, 
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
    A1D_Request_t *a1d_request;
    void *packet;
    int size_packet;

    A1U_FUNC_ENTER();

    status = A1DI_Pack_strided(&packet,
                               &size_packet,
                               stride_level,
                               block_sizes,
                               source_ptr,
                               src_stride_ar,
                               target_ptr,
                               trg_stride_ar);
    A1U_ERR_POP(status,
                "A1DI_Pack_strided returned with an error\n");

    a1d_request = A1DI_Get_request();
    A1U_ERR_POP(status = (a1d_request == NULL),
            "A1DI_Get_request returned error.\n");
    A1DI_Set_handle(a1d_request, a1d_handle);

    done_callback.function = A1DI_Request_done;
    done_callback.clientdata = (void *) a1d_request;

    a1d_handle->active++; 

    /* Assigning the packing buffer pointer in request so that it can be free when the 
     * request is complete, in the callback */
    a1d_request->buffer_ptr = packet;

    status = DCMF_Send(&A1D_Packed_puts_protocol,
                       &(a1d_request->request),
                       done_callback,
                       DCMF_SEQUENTIAL_CONSISTENCY,
                       target,
                       size_packet,
                       packet,
                       NULL,
                       0);
    A1U_ERR_POP(status != DCMF_SUCCESS, "DCMF_Send returned with an error \n");

    A1D_Connection_send_active[target]++;

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;

}

int A1DI_Direct_puts(int target,
                     int stride_level,
                     int *block_sizes,
                     void *source_ptr,
                     int *src_stride_ar,
                     void *target_ptr,
                     int *trg_stride_ar,
                     A1D_Handle_t *a1d_handle)
{
    int i, status = A1_SUCCESS;
    size_t src_disp, dst_disp;
    DCMF_Callback_t done_callback;
    A1D_Request_t *a1d_request;
    int chunk_count=1;
    int *block_sizes_w; 
    int y=0;

    A1U_FUNC_ENTER();

    status = A1DI_Malloc_aligned((void **) &block_sizes_w, sizeof(int)*(stride_level+1));
    A1U_ERR_POP(status != A1_SUCCESS,
             "A1DI_Malloc_aligned returned error in A1DI_Direct_puts");

    A1DI_Memcpy(block_sizes_w, block_sizes, sizeof(int)*(stride_level+1));

    for(i=1; i<=stride_level; i++) 
        chunk_count = block_sizes[i]*chunk_count;

    for(i=0; i<chunk_count; i++) 
    {

        src_disp = (size_t) source_ptr
                 - (size_t) A1D_Membase_global[A1D_Process_info.my_rank];
        dst_disp = (size_t) target_ptr
                 - (size_t) A1D_Membase_global[target];

        a1d_request = A1DI_Get_request();
        A1U_ERR_POP(status = (a1d_request == NULL),
          "A1DI_Get_request returned error.\n");
        A1DI_Set_handle(a1d_request, a1d_handle);

        done_callback.function = A1DI_Request_done;
        done_callback.clientdata = (void *) a1d_request;

        a1d_handle->active++;

        status = DCMF_Put(&A1D_Generic_put_protocol,
                          &(a1d_request->request),
                          done_callback,
                          DCMF_SEQUENTIAL_CONSISTENCY,
                          target,
                          block_sizes[0],
                          &A1D_Memregion_global[A1D_Process_info.my_rank],
                          &A1D_Memregion_global[target],
                          src_disp,
                          dst_disp,
                          A1D_Nocallback);
        A1U_ERR_POP(status != DCMF_SUCCESS, "DCMF_Put returned with an error \n");

        A1D_Connection_put_active[target]++;

        block_sizes_w[1]--;
        if(block_sizes_w[1]==0) 
        {
               y=1;
               while(block_sizes_w[y] == 0) 
               {
                  if(y == stride_level)
                  {
                     A1U_ASSERT(i == chunk_count-1, status); 
                     return status;     
                  }
                  y++;
               }
               block_sizes_w[y]--;

               source_ptr = (void *) ((size_t) source_ptr + src_stride_ar[y-1]);
               target_ptr = (void *) ((size_t) target_ptr + trg_stride_ar[y-1]);

               y--;
               while(y >= 1) 
               {
                  block_sizes_w[y] = block_sizes[y];
                  y--;
               }
        } 
        else
        {
               source_ptr = (void *) ((size_t) source_ptr + src_stride_ar[0]);
               target_ptr = (void *) ((size_t) target_ptr + trg_stride_ar[0]); 
        }

    }

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1DI_Recursive_puts(int target,
                        int stride_level,
                        int *block_sizes,
                        void *source_ptr,
                        int *src_stride_ar,
                        void *target_ptr,
                        int *trg_stride_ar,
                        A1D_Handle_t *a1d_handle)
{
    int i, status = A1_SUCCESS;
    size_t src_disp, dst_disp;
    DCMF_Callback_t done_callback;
    A1D_Request_t *a1d_request;

    A1U_FUNC_ENTER();

    if (stride_level > 0)
    {

        for (i = 0; i < block_sizes[stride_level]; i++)
        {
            status = A1DI_Recursive_puts(target,
                                         stride_level - 1,
                                         block_sizes,
                                         (void *) ((size_t) source_ptr + i * src_stride_ar[stride_level - 1]),
                                         src_stride_ar,
                                         (void *) ((size_t) target_ptr + i * trg_stride_ar[stride_level - 1]),
                                         trg_stride_ar, 
                                         a1d_handle);
             A1U_ERR_POP(status != A1_SUCCESS,
               "A1DI_Recursive_puts returned error in A1DI_Recursive_puts.\n");
        }

    }
    else
    {

        src_disp = (size_t) source_ptr
                 - (size_t) A1D_Membase_global[A1D_Process_info.my_rank];
        dst_disp = (size_t) target_ptr
                 - (size_t) A1D_Membase_global[target];

        a1d_request = A1DI_Get_request();
        A1U_ERR_POP(status = (a1d_request == NULL),
                 "A1DI_Get_request returned error.\n");
        A1DI_Set_handle(a1d_request, a1d_handle);

        done_callback.function = A1DI_Request_done;
        done_callback.clientdata = (void *) a1d_request;

        a1d_handle->active++;

        status = DCMF_Put(&A1D_Generic_put_protocol,
                          &(a1d_request->request),
                          done_callback,
                          DCMF_SEQUENTIAL_CONSISTENCY,
                          target,
                          block_sizes[0],
                          &A1D_Memregion_global[A1D_Process_info.my_rank],
                          &A1D_Memregion_global[target],
                          src_disp,
                          dst_disp,
                          A1D_Nocallback);
        A1U_ERR_POP(status != DCMF_SUCCESS, "DCMF_Put returned with an error \n");

        A1D_Connection_put_active[target]++;

    }

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

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
    int status = A1_SUCCESS;
    A1D_Handle_t *a1d_handle;
    int i, chunk_count = 1;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    a1d_handle = A1DI_Get_handle();
    A1U_ERR_POP(status = (a1d_handle == NULL),
                "A1DI_Get_handle returned NULL in A1D_PutS\n");

    for(i=1; i<=stride_level; i++)
        chunk_count = block_sizes[i]*chunk_count;

    if (chunk_count <= a1_settings.put_packing_chunkcount_threshold || 
             block_sizes[0] >= a1_settings.put_packing_chunksize_limit)
    {

        status = A1DI_Direct_puts(target,
                                  stride_level,
                                  block_sizes,
                                  source_ptr,
                                  src_stride_ar,
                                  target_ptr,
                                  trg_stride_ar,
                                  a1d_handle); 
        A1U_ERR_POP(status, "A1DI_Direct_puts returned with an error \n");

    }
    else
    {

        status = A1DI_Packed_puts(target,
                                  stride_level,
                                  block_sizes,
                                  source_ptr,
                                  src_stride_ar,
                                  target_ptr,
                                  trg_stride_ar,
                                  a1d_handle);
        A1U_ERR_POP(status, "A1DI_Packed_puts returned with an error \n");

    }

    A1DI_Conditional_advance(a1d_handle->active > 0);

  fn_exit:
    A1DI_Release_handle(a1d_handle);
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1D_NbPutS(int target,
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
    int i, chunk_count = 1;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    a1d_handle = (A1D_Handle_t *) a1_handle;

    for(i=1; i<=stride_level; i++)
        chunk_count = block_sizes[i]*chunk_count;

    if (chunk_count <= a1_settings.put_packing_chunkcount_threshold || 
             block_sizes[0] >= a1_settings.put_packing_chunksize_limit)
    {

        status = A1DI_Direct_puts(target,
                                  stride_level,
                                  block_sizes,
                                  source_ptr,
                                  src_stride_ar,
                                  target_ptr,
                                  trg_stride_ar,
                                  a1d_handle); 
        A1U_ERR_POP(status, "A1DI_Direct_puts returned with an error \n");

    }
    else
    {

        status = A1DI_Packed_puts(target,
                                  stride_level,
                                  block_sizes,
                                  source_ptr,
                                  src_stride_ar,
                                  target_ptr,
                                  trg_stride_ar,
                                  a1d_handle);
        A1U_ERR_POP(status, "A1DI_Packed_puts returned with an error \n");

    }

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}
