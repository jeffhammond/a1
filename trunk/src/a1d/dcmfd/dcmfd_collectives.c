/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Protocol_t A1D_GlobalBarrier_protocol;
DCMF_Protocol_t A1D_GlobalAllreduce_protocol;

int A1DI_GlobalBarrier_initialize()
{
    int status = A1_SUCCESS;
    DCMF_GlobalBarrier_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_GLOBALBARRIER_PROTOCOL;
    status = DCMF_GlobalBarrier_register(&A1D_GlobalBarrier_protocol,
                                         &conf);
    A1U_ERR_POP(status != DCMF_SUCCESS,
                "global barrier registartion returned with error %d \n",
                status);

  fn_exit:

    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int A1DI_GlobalBarrier()
{

    int status = A1_SUCCESS;
    DCMF_Request_t request;
    DCMF_Callback_t done_callback;
    volatile int active;

    A1U_FUNC_ENTER();

    active = 1;
    done_callback.function = A1DI_Generic_done;
    done_callback.clientdata = (void *) &active;

    status = DCMF_GlobalBarrier(&A1D_GlobalBarrier_protocol,
                                &request,
                                done_callback);
    A1U_ERR_ABORT(status != DCMF_SUCCESS, "DCMF_GlobalBarrier returned with an error");

    A1DI_Conditional_advance(active > 0);

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;

}

int A1D_Barrier_group(A1_group_t* group)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if (group == A1_GROUP_WORLD || group == NULL)
    {

        status = A1DI_GlobalBarrier();
        A1U_ERR_ABORT(status != A1_SUCCESS, "DCMF_GlobalBarrier returned with an error");
        goto fn_exit;
    }
    else
    {
        A1U_ERR_POP(1, "A1D_Barrier_group not implemented for non-world groups!");
        goto fn_fail;
    }

  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;

}

int A1D_Sync_group(A1_group_t* group)
{

    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if (group == A1_GROUP_WORLD || group == NULL)
    {
        status = A1DI_Flush_all();
        A1U_ERR_ABORT(status != A1_SUCCESS, "A1DI_Flush_all returned with an error");
        status = A1DI_GlobalBarrier();
        A1U_ERR_ABORT(status != A1_SUCCESS, "A1DI_GlobalBarrier returned with an error");
        goto fn_exit;
    }
    else
    {
        A1U_ERR_POP(1, "A1D_Sync_group not implemented for non-world groups!");
        goto fn_fail;
    }

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;

}

int A1DI_NbGlobalBarrier(A1D_Handle_t *a1d_handle)
{

    int status = A1_SUCCESS;
    A1D_Request_t *a1d_request;
    DCMF_Callback_t done_callback;
    volatile int active;

    A1U_FUNC_ENTER();

    a1d_request = A1DI_Get_request(1);
    A1U_ERR_POP(status = (a1d_request == NULL),
                "A1DI_Get_request returned error \n");
    A1DI_Set_handle(a1d_request, a1d_handle);

    a1d_handle->active++;

    done_callback.function = A1DI_Request_done;
    done_callback.clientdata = (void *) a1d_request;

    status = DCMF_GlobalBarrier(&A1D_GlobalBarrier_protocol,
                                &(a1d_request->request),
                                done_callback);
    A1U_ERR_ABORT(status != DCMF_SUCCESS, "DCMF_GlobalBarrier returned with an error");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;

}

int A1D_NbBarrier_group(A1_group_t* group, A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if (group == A1_GROUP_WORLD || group == NULL)
    {
        a1d_handle = (A1D_Handle_t *) a1_handle;

        status = A1DI_NbGlobalBarrier(a1d_handle);
        A1U_ERR_ABORT(status != A1_SUCCESS, "DCMF_NbGlobalBarrier returned with an error");

        goto fn_exit;
    }
    else
    {
        A1U_ERR_POP(1, "A1D_NbBarrier_group not implemented for non-world groups!");
        goto fn_fail;
    }

  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;

}


int A1D_NbSync_group(A1_group_t* group, A1_handle_t a1_handle)
{
    int status = A1_SUCCESS; 
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if (group == A1_GROUP_WORLD || group == NULL)
    {
        a1d_handle = (A1D_Handle_t *) a1_handle;

        /*This has to be replace with a non-blocking flushall to make it truly non blocking*/
        status = A1DI_Flush_all();
        A1U_ERR_ABORT(status != A1_SUCCESS, "A1DI_Flush_all returned with an error");

        status = A1DI_NbGlobalBarrier(a1d_handle);
        A1U_ERR_ABORT(status != A1_SUCCESS, "A1DI_NbGlobalBarrier returned with an error");

        goto fn_exit;
    }
    else
    {
        A1U_ERR_POP(1, "A1D_NbSync_group not implemented for non-world groups!");
        goto fn_fail;
    }

  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;

}

int A1DI_GlobalAllreduce_initialize()
{
    int status = A1_SUCCESS;
    DCMF_GlobalAllreduce_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_GLOBALALLREDUCE_PROTOCOL;
    status = DCMF_GlobalAllreduce_register(&A1D_GlobalAllreduce_protocol,
                                           &conf);
    A1U_ERR_POP(status != DCMF_SUCCESS,
                "global allreduce registartion returned with error %d \n",
                status);

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int A1DI_GlobalAllreduce(int count,
                         A1_reduce_op_t a1_op,
                         A1_datatype_t a1_type, 
                         void *in, 
                         void *out)
{
    int status = A1_SUCCESS;
    DCMF_Request_t request;
    DCMF_Callback_t done_callback;
    DCMF_Op reduce_op;
    DCMF_Dt datatype;
    int bytes = 0;
    void *in_abs = NULL;
    volatile int active;

    A1U_FUNC_ENTER();

    switch (a1_op)
    {
    case A1_SUM:
        reduce_op = DCMF_SUM;
        break;
    case A1_PROD:
        reduce_op = DCMF_PROD;
        break;
    case A1_MAX:
        reduce_op = DCMF_MAX;
        break;
    case A1_MAXABS:
        /* FIXME I don't think any DCMF built-in ops will give us the maximum absolute
         *        value, so we might have to do something more complicated here.        */
        A1U_ERR_POP(status != DCMF_SUCCESS, "A1_MAXABS not implemented yet \n");
        break;
    case A1_MIN:
        reduce_op = DCMF_MIN;
        break;
    case A1_MINABS:
        /* FIXME I don't think any DCMF built-in ops will give us the minimum absolute
         *        value, so we might have to do something more complicated here.        */
        A1U_ERR_POP(status != DCMF_SUCCESS, "A1_MINABS not implemented yet \n");
        break;
    case A1_OR:
        reduce_op = DCMF_LOR;
        break;
    default:
        A1U_ERR_POP(status != DCMF_SUCCESS, "Unsupported A1_reduce_op \n");
        break;
    }

    if(a1_op == A1_MAXABS || a1_op == A1_MINABS)
    {
       switch (a1_type)
       {
         case A1_INT32:
               datatype = DCMF_SIGNED_SHORT;
               bytes = count*32;
               status = A1DI_Malloc(&in_abs, bytes);
               A1U_ERR_POP(status != A1_SUCCESS, "A1DI_Malloc returned error\
                              in A1DI_GlobalAllreduce \n");
               A1DI_ABS(int32_t, in, in_abs, count);
               in = in_abs;
               break;
         case A1_INT64:
               datatype = DCMF_SIGNED_LONG_LONG;
               bytes = count*64;
               status = A1DI_Malloc(&in_abs, bytes);
               A1U_ERR_POP(status != A1_SUCCESS, "A1DI_Malloc returned error\
                              in A1DI_GlobalAllreduce \n");
               A1DI_ABS(int64_t, in, in_abs, count);
               in = in_abs;
               break;
         case A1_UINT32:
               datatype = DCMF_UNSIGNED_SHORT;
               break;        
         case A1_UINT64:
               datatype = DCMF_UNSIGNED_LONG_LONG;
               break;
         case A1_FLOAT:
               datatype = DCMF_FLOAT;
               bytes = count*sizeof(float);
               status = A1DI_Malloc(&in_abs, bytes);
               A1U_ERR_POP(status != A1_SUCCESS, "A1DI_Malloc returned error\
                              in A1DI_GlobalAllreduce \n");
               A1DI_ABS(float, in, in_abs, count);
               in = in_abs;
               break;
         case A1_DOUBLE:
               datatype = DCMF_DOUBLE;
               bytes = count*sizeof(double);
               status = A1DI_Malloc(&in_abs, bytes);
               A1U_ERR_POP(status != A1_SUCCESS, "A1DI_Malloc returned error\
                              in A1DI_GlobalAllreduce \n");
               A1DI_ABS(double, in, in_abs, count);
               in = in_abs;
               break;
         default:
               A1U_ERR_POP(status != DCMF_SUCCESS, "Unsupported A1_datatype \n");
               break;
       }
    }
    else
    {
       switch (a1_type)
       {
         case A1_INT32:
               datatype = DCMF_SIGNED_SHORT;
               break;
         case A1_INT64:
               datatype = DCMF_SIGNED_LONG_LONG;
               break;
         case A1_UINT32:
               datatype = DCMF_UNSIGNED_SHORT;
               break;        
         case A1_UINT64:
               datatype = DCMF_UNSIGNED_LONG_LONG;
               break;
         case A1_FLOAT:
               datatype = DCMF_FLOAT;
               break;
         case A1_DOUBLE:
               datatype = DCMF_DOUBLE;
               break;
         default:
               A1U_ERR_POP(status != DCMF_SUCCESS, "Unsupported A1_datatype \n");
               break;
       }

    }

    active = 1;
    done_callback.function = A1DI_Generic_done;
    done_callback.clientdata = (void *) &active;

    status = DCMF_GlobalAllreduce(&A1D_GlobalAllreduce_protocol,
                                  &request,
                                  done_callback,
                                  DCMF_SEQUENTIAL_CONSISTENCY,
                                  -1,
                                  (char *) in,
                                  (char *) out,
                                  count,
                                  datatype,
                                  reduce_op);
    A1U_ERR_POP(status != DCMF_SUCCESS, "DCMF_GlobalAllreduce returned with an error");

    A1DI_Conditional_advance(active > 0);
 
  fn_exit:
    if(in_abs != NULL)
         A1DI_Free(in_abs);
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;

}

int A1D_Allreduce_group(A1_group_t* group,
                        int count,
                        A1_reduce_op_t a1_op,
                        A1_datatype_t a1_type,
                        void* in,
                        void* out)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if (group == A1_GROUP_WORLD || group == NULL)
    {
        status = A1DI_GlobalAllreduce(count,
                                      a1_op,
                                      a1_type,
                                      in,
                                      out);
        A1U_ERR_ABORT(status != A1_SUCCESS, "A1DI_GlobalAllreduce returned with an error");
        goto fn_exit;
    }
    else
    {
        A1U_ERR_POP(1, "A1D_Allreduce_group not implemented for non-world groups!");
        goto fn_fail;
    }


  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;

}

int A1DI_NbGlobalAllreduce(int count,
                           A1_reduce_op_t a1_op,
                           A1_datatype_t a1_type, 
                           void *in, 
                           void *out,
                           A1D_Handle_t *a1d_handle)
{
    int status = A1_SUCCESS;
    A1D_Request_t *a1d_request;
    DCMF_Callback_t done_callback;
    DCMF_Op reduce_op;
    DCMF_Dt datatype;
    int bytes;
    void *in_abs = NULL;

    A1U_FUNC_ENTER();

    switch (a1_op)
    {
      case A1_SUM:
            reduce_op = DCMF_SUM;
            break;
      case A1_PROD:
            reduce_op = DCMF_PROD;
            break;
      case A1_MAX:
      case A1_MAXABS:
            reduce_op = DCMF_MAX;
            break;
      case A1_MIN:
      case A1_MINABS:
            reduce_op = DCMF_MIN;
            break;
      case A1_OR:
            reduce_op = DCMF_LOR;
            break;
      default:
            A1U_ERR_POP(status != DCMF_SUCCESS, "Unsupported A1_reduce_op \n");
            break;
    }

    if(a1_op == A1_MAXABS || a1_op == A1_MINABS)
    {
       switch (a1_type)
       {
         case A1_INT32:
               datatype = DCMF_SIGNED_SHORT;
               bytes = count*32;
               status = A1DI_Malloc(&in_abs, bytes);
               A1U_ERR_POP(status != A1_SUCCESS, "A1DI_Malloc returned error\
                              in A1DI_GlobalAllreduce \n");
               A1DI_ABS(int32_t, in, in_abs, count);
               in = in_abs;
               break;
         case A1_INT64:
               datatype = DCMF_SIGNED_LONG_LONG;
               bytes = count*64;
               status = A1DI_Malloc(&in_abs, bytes);
               A1U_ERR_POP(status != A1_SUCCESS, "A1DI_Malloc returned error\
                              in A1DI_GlobalAllreduce \n");
               A1DI_ABS(int64_t, in, in_abs, count);
               in = in_abs;
               break;
         case A1_UINT32:
               datatype = DCMF_UNSIGNED_SHORT;
               break;        
         case A1_UINT64:
               datatype = DCMF_UNSIGNED_LONG_LONG;
               break;
         case A1_FLOAT:
               datatype = DCMF_FLOAT;
               bytes = count*sizeof(float);
               status = A1DI_Malloc(&in_abs, bytes);
               A1U_ERR_POP(status != A1_SUCCESS, "A1DI_Malloc returned error\
                              in A1DI_GlobalAllreduce \n");
               A1DI_ABS(float, in, in_abs, count);
               in = in_abs;
               break;
         case A1_DOUBLE:
               datatype = DCMF_DOUBLE;
               bytes = count*sizeof(double);
               status = A1DI_Malloc(&in_abs, bytes);
               A1U_ERR_POP(status != A1_SUCCESS, "A1DI_Malloc returned error\
                              in A1DI_GlobalAllreduce \n");
               A1DI_ABS(double, in, in_abs, count);
               in = in_abs;
               break;
         default:
               A1U_ERR_POP(status != DCMF_SUCCESS, "Unsupported A1_datatype \n");
               break;
       }
    }
    else
    {
       switch (a1_type)
       {
         case A1_INT32:
               datatype = DCMF_SIGNED_SHORT;
               break;
         case A1_INT64:
               datatype = DCMF_SIGNED_LONG_LONG;
               break;
         case A1_UINT32:
               datatype = DCMF_UNSIGNED_SHORT;
               break;        
         case A1_UINT64:
               datatype = DCMF_UNSIGNED_LONG_LONG;
               break;
         case A1_FLOAT:
               datatype = DCMF_FLOAT;
               break;
         case A1_DOUBLE:
               datatype = DCMF_DOUBLE;
               break;
         default:
               A1U_ERR_POP(status != DCMF_SUCCESS, "Unsupported A1_datatype \n");
               break;
       }

    }

    a1d_request = A1DI_Get_request(1);
    A1U_ERR_POP(status = (a1d_request == NULL),
                "A1DI_Get_request returned error \n");
    A1DI_Set_handle(a1d_request, a1d_handle);

    a1d_request->buffer_ptr = in_abs;

    a1d_handle->active++;

    done_callback.function = A1DI_Request_done;
    done_callback.clientdata = (void *) a1d_request;

    status = DCMF_GlobalAllreduce(&A1D_GlobalAllreduce_protocol,
                                  &(a1d_request->request),
                                  done_callback,
                                  DCMF_SEQUENTIAL_CONSISTENCY,
                                  -1,
                                  (char *) in,
                                  (char *) out,
                                  count,
                                  datatype,
                                  reduce_op);
    A1U_ERR_POP(status != DCMF_SUCCESS, "DCMF_GlobalAllreduce returned with an error");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;

}

int A1D_NbAllreduce_group(A1_group_t* group,
                          int count,
                          A1_reduce_op_t a1_op,
                          A1_datatype_t a1_type,
                          void* in,
                          void* out,
                          A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if (group == A1_GROUP_WORLD || group == NULL)
    {
        a1d_handle = (A1D_Handle_t *) a1_handle;

        status = A1DI_NbGlobalAllreduce(count,
                                        a1_op,
                                        a1_type,
                                        in,
                                        out,
                                        a1d_handle);
        A1U_ERR_ABORT(status != A1_SUCCESS, "A1DI_GlobalAllreduce returned with an error");
        goto fn_exit;
    }
    else
    {
        A1U_ERR_POP(1, "A1D_Allreduce_group not implemented for non-world groups!");
        goto fn_fail;
    }


  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;

}
