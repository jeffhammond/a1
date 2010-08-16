/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Protocol_t A1D_Generic_putacc_protocol;

void A1DI_RecvDone_putacc_callback(void *clientdata, DCMF_Error_t *error)
{
    int result = A1_SUCCESS;

    A1D_Putacc_recv_info_t *putacc_recv_info =
            (A1D_Putacc_recv_info_t *) clientdata;
    A1D_Putacc_header_t *header = &(putacc_recv_info->header);
    A1D_Buffer_info_t *buffer_info = &(putacc_recv_info->buffer_info);

    result = A1D_Acc_process(buffer_info->buffer_ptr,
                             buffer_info->bytes,
                             header);
    A1U_ERR_ABORT(result,
                  "A1D_Acc_process failed in A1DI_RecvDone_putacc_callback\n");

    A1DI_Free(buffer_info->buffer_ptr);
    A1DI_Free((void *) putacc_recv_info);
}

DCMF_Request_t* A1DI_RecvSend_putacc_callback(void *clientdata,
                                              const DCQuad *msginfo,
                                              unsigned count,
                                              size_t peer,
                                              size_t sndlen,
                                              size_t *rcvlen,
                                              char **rcvbuf,
                                              DCMF_Callback_t *cb_done)
{
    int result = 0;
    A1D_Putacc_recv_info_t *putacc_recv_info;

    result = A1DI_Malloc_aligned((void **) &putacc_recv_info,
                                 sizeof(A1D_Putacc_recv_info_t));
    A1U_ERR_ABORT(result != 0,
                  "A1DI_Malloc_aligned failed in A1DI_RecvSend_packedputs_callback\n");

    memcpy((void *) &(putacc_recv_info->header), (void *) msginfo, count
            * sizeof(DCQuad));

    *rcvlen = sndlen;
    result = A1DI_Malloc_aligned((void **) rcvbuf, sndlen);
    A1U_ERR_ABORT(result != 0,
                  "A1DI_Malloc_aligned failed in A1DI_RecvSend_packedputs_callback\n");

    (putacc_recv_info->buffer_info).buffer_ptr = (void *) *rcvbuf;
    (putacc_recv_info->buffer_info).bytes = sndlen;

    cb_done->function = A1DI_RecvDone_putacc_callback;
    cb_done->clientdata = (void *) putacc_recv_info;

    return &((putacc_recv_info->buffer_info).request);
}

void A1DI_RecvSendShort_putacc_callback(void *clientdata,
                                        const DCQuad *msginfo,
                                        unsigned count,
                                        size_t peer,
                                        const char *src,
                                        size_t bytes)
{
    int result = A1_SUCCESS;
    A1D_Putacc_header_t *header;
  
    header = (A1D_Putacc_header_t *) msginfo;

    result = A1D_Acc_process((void *) src,
                             bytes,
                             header);
    A1U_ERR_ABORT(result,
                  "A1D_Acc_process failed in A1DI_RecvSendShort_putacc_callback\n");

}

DCMF_Result A1DI_Putacc_initialize()
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Send_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_EAGER_SEND_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    conf.cb_recv_short = A1DI_RecvSendShort_putacc_callback;
    conf.cb_recv_short_clientdata = NULL;
    conf.cb_recv = A1DI_RecvSend_putacc_callback;
    conf.cb_recv_clientdata = NULL;

    result = DCMF_Send_register(&A1D_Generic_putacc_protocol, &conf);
    A1U_ERR_POP(result != DCMF_SUCCESS,
                "putacc registartion returned with error %d \n",
                result);

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

int A1D_Acc_process(void *src, int bytes, A1D_Putacc_header_t *header)
{
    int result = A1_SUCCESS;

    A1U_FUNC_ENTER();

    likely_if(header->datatype == A1_DOUBLE) 
    {
       A1DI_ACC_EXECUTE(double,
                        src,
                        header->target_ptr,
                        (header->scaling).double_value,
                        bytes/sizeof(double));
    }
    else
    {
       switch (header->datatype)
       {
       case A1_INT32:
            A1DI_ACC_EXECUTE(int32_t,
                            src,
                            header->target_ptr,
                            (header->scaling).int32_value,
                            bytes / sizeof(int32_t));
           break;
       case A1_INT64:
           A1DI_ACC_EXECUTE(int64_t,
                            src,
                            header->target_ptr,
                            (header->scaling).int64_value,
                            bytes / sizeof(int64_t));
           break;
       case A1_UINT32:
           A1DI_ACC_EXECUTE(uint32_t,
                            src,
                            header->target_ptr,
                            (header->scaling).uint32_value,
                            bytes / sizeof(uint32_t));
           break;
       case A1_UINT64:
           A1DI_ACC_EXECUTE(uint64_t,
                            src,
                            header->target_ptr,
                            (header->scaling).uint64_value,
                            bytes / sizeof(uint64_t));
           break;
       case A1_FLOAT:
           A1DI_ACC_EXECUTE(float,
                            src,
                            header->target_ptr,
                            (header->scaling).float_value,
                            bytes/sizeof(float));
           break;
       default:
           A1U_ERR_POP(result, "Invalid datatype received in Putacc operation \n");
           break;
       }
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}

int A1D_PutAcc(int target,
               void* source_ptr,
               void* target_ptr,
               int bytes,
               A1_datatype_t a1_type,
               void* scaling)
{
    DCMF_Result result = A1_SUCCESS;
    DCMF_Request_t request;
    DCMF_Callback_t callback;
    volatile int active;
    A1D_Putacc_header_t header;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    callback.function = A1DI_Generic_done;
    callback.clientdata = (void *) &active;
    active = 1;

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
           A1U_ERR_POP((result != A1_SUCCESS), "Invalid data type in putacc \n");
           break;
       }
    }

    result = DCMF_Send(&A1D_Generic_putacc_protocol,
                       &request,
                       callback,
                       DCMF_SEQUENTIAL_CONSISTENCY,
                       target,
                       bytes,
                       source_ptr,
                       (DCQuad *) &header,
                       (unsigned) 2);
    A1U_ERR_POP((result != A1_SUCCESS), "Putacc returned with an error \n");

    A1D_Connection_send_active[target]++;
    A1DI_Conditional_advance(active > 0);

  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}
