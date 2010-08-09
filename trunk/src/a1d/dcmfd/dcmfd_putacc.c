/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

#define A1DI_ACC_EXECUTE(datatype, source, target, scaling, count) do {    \
    int i;                                                                 \
    datatype *a = (datatype *) source;                                     \
    datatype *b = (datatype *) target;                                     \
    datatype c = (datatype *) scaling;                                     \
    for(i=0; i<count; i++)                                                 \
          a[i] = a[i] + b[i]*c;                                            \
} while(0)                                                                 \

int A1D_Acc_process(void *src, int bytes, A1D_Putacc_header_t *header) 
{
    int result = A1_SUCCESS;

    A1U_FUNC_ENTER();

    switch(header->datatype)
    {
        case A1_INT32:
               A1DI_ACC_EXECUTE(int32_t, src, header->target_ptr, header->int32_value,
                    bytes/sizeof(int32_t));
               break;
        case A1_INT64:
               A1DI_ACC_EXECUTE(int64_t, src, header->target_ptr, header->int64_value,
                    bytes/sizeof(int64_t));
               break;
        case A1_UINT32:
               A1DI_ACC_EXECUTE(uint32_t, src, header->target_ptr, header->uint32_value,
                    bytes/sizeof(uint32_t));
               break;
        case A1_UINT64:
               A1DI_ACC_EXECUTE(uint64_t, src, header->target_ptr, header->uint64_value,
                    bytes/sizeof(uint64_t));
               break;
        case A1_FLOAT:
               A1DI_ACC_EXECUTE(float, src, header->target_ptr, header->float_value,
                    bytes/sizeof(float));
               break;
        case A1_DOUBLE:
               A1DI_ACC_EXECUTE(double, src, header->target_ptr, header->double_value,
                    bytes/sizeof(double));
               break;
        default:
               A1U_ERR_POP(result,
                    "Invalid datatype received in Putacc operation \n");
               break;
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

int A1D_PutAcc(int target, void* source_ptr, void* target_ptr, int bytes,
                A1_datatype a1_type, void* scaling)
{
    DCMF_Result result = A1_SUCCESS;
    DCMF_Request_t request;
    DCMF_Callback_t done_callback;
    int done_active;
    A1D_Putacc_header_t header;
 
    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    done_callback.function = A1DI_Generic_done;
    done_callback.clientdata = (void *) &done_active;
    done_active = 1;

    header.target_ptr = target_ptr;
    header.datatype = a1_type;
    switch(a1_type) 
    {
       case A1_INT32: 
             header.int32_value = *((int32_t *) scaling);
             break;
       case A1_INT64: 
             header.int64_value = *((int64_t *) scaling);
             break;
       case A1_UINT32:
             header.uint32_value = *((uint32_t *) scaling);
             break;
       case A1_UINT64:
             header.uint64_value = *((uint64_t *) scaling);
             break;
       case A1_FLOAT:
             header.float_value = *((float *) scaling);
             break;
       case A1_DOUBLE: 
             header.double_value = *((double *) scaling); 
             break;
       default:
             result = A1_ERROR;
             A1_ERR_POP("Invalid data type in putacc \n");  
             break;
    }

    result = DCMF_Send(&A1D_Generic_putacc_protocol,
                      &request,
                      callback,
                      DCMF_SEQUENTIAL_CONSISTENCY,
                      target,
                      bytes,
                      source_ptr,
                      &header,
                      2);
    A1U_ERR_POP((result != A1_SUCCESS),"Putacc returned with an error \n");

    while (done_active>0) A1DI_Advance(); 

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}
