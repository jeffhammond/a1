/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_CollectiveProtocol_t A1D_GlobalAllreduce_protocol;
DCMF_Geometry_t geometry;
DCMF_CollectiveProtocol_t A1D_Barrier_protocol, A1D_Localbarrier_protocol;
DCMF_Allreduce_Configuration_t allreduce_conf;
DCMF_Barrier_Configuration_t barrier_conf;
DCMF_CollectiveProtocol_t *barrier_ptr, *localbarrier_ptr;
DCMF_CollectiveRequest_t crequest;
unsigned *allreduce_ranklist;

static DCMF_Geometry_t *getGeometry (int x)
{
    return &geometry;
}

int A1DI_GlobalAllreduce_initialize()
{
    int i,status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    barrier_conf.protocol = DCMF_GI_BARRIER_PROTOCOL;
    barrier_conf.cb_geometry = getGeometry;
    status = DCMF_Barrier_register(&A1D_Barrier_protocol,
                                   &barrier_conf);

    barrier_conf.protocol = DCMF_LOCKBOX_BARRIER_PROTOCOL;
    barrier_conf.cb_geometry = getGeometry;
    status = DCMF_Barrier_register(&A1D_Localbarrier_protocol,
                                   &barrier_conf);

    status = A1DI_Malloc((void **) &allreduce_ranklist, A1D_Process_info.num_ranks * sizeof(unsigned));
    A1U_ERR_POP(status != 0,
                "A1DI_Malloc returned with error %d \n", status);

    for(i=0; i<A1D_Process_info.num_ranks; i++)
        allreduce_ranklist[i] = i;

    barrier_ptr = &A1D_Barrier_protocol;
    localbarrier_ptr  = &A1D_Localbarrier_protocol;
    status = DCMF_Geometry_initialize(&geometry,
                                      0,
                                      allreduce_ranklist,
                                      A1D_Process_info.num_ranks,
                                      &barrier_ptr,
                                      1,
                                      &localbarrier_ptr,
                                      1,
                                      &crequest,
                                      0,
                                      1);

    allreduce_conf.protocol = DCMF_TORUS_BINOMIAL_ALLREDUCE_PROTOCOL;
    allreduce_conf.cb_geometry = getGeometry;
    allreduce_conf.reuse_storage = 1;
    status = DCMF_Allreduce_register(&A1D_GlobalAllreduce_protocol,
                                     &allreduce_conf);
    A1U_ERR_POP(status != DCMF_SUCCESS,
                "DCMF_Allreduce_register returned with error %d \n", status);

    fn_exit:
    A1U_FUNC_EXIT();
    return status;

    fn_fail:
    goto fn_exit;
}

int A1DI_GlobalAllreduce_finalize()
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    status = A1DI_Free(allreduce_ranklist);
    A1U_ERR_POP(status != 0,
                "A1DI_Free returned with error %d \n", status);

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
    DCMF_CollectiveRequest_t ar_crequest;
    DCMF_Callback_t done_callback;
    DCMF_Op reduce_op;
    DCMF_Dt datatype;
    int bytes = 0;
    void *in_abs = NULL;
    volatile unsigned ga_active = 0;

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

    if (a1_op == A1_MAXABS || a1_op == A1_MINABS)
    {
        switch (a1_type)
        {
        case A1_DOUBLE:
            datatype = DCMF_DOUBLE;
            bytes = count * sizeof(double);
            status = A1DI_Malloc(&in_abs, bytes);
            A1U_ERR_POP(status != A1_SUCCESS,
                        "A1DI_Malloc returned error in A1DI_GlobalAllreduce \n");
            A1DI_ABS(double, in, in_abs, count);
            in = in_abs;
            break;
        case A1_INT32:
            datatype = DCMF_SIGNED_INT;
            bytes = count * sizeof(int32_t);
            status = A1DI_Malloc(&in_abs, bytes);
            A1U_ERR_POP(status != A1_SUCCESS,
                        "A1DI_Malloc returned error in A1DI_GlobalAllreduce \n");
            A1DI_ABS(int32_t, in, in_abs, count);
            in = in_abs;
            break;
        case A1_INT64:
            datatype = DCMF_SIGNED_LONG_LONG;
            bytes = count * sizeof(int64_t);
            status = A1DI_Malloc(&in_abs, bytes);
            A1U_ERR_POP(status != A1_SUCCESS,
                        "A1DI_Malloc returned error in A1DI_GlobalAllreduce \n");
            A1DI_ABS(int64_t, in, in_abs, count);
            in = in_abs;
            break;
        case A1_UINT32:
            datatype = DCMF_UNSIGNED_INT;
            break;
        case A1_UINT64:
            datatype = DCMF_UNSIGNED_LONG_LONG;
            break;
        case A1_FLOAT:
            datatype = DCMF_FLOAT;
            bytes = count * sizeof(float);
            status = A1DI_Malloc(&in_abs, bytes);
            A1U_ERR_POP(status != A1_SUCCESS,
                        "A1DI_Malloc returned error in A1DI_GlobalAllreduce \n");
            A1DI_ABS(float, in, in_abs, count);
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
        case A1_DOUBLE:
            datatype = DCMF_DOUBLE;
            break;
        case A1_INT32:
            datatype = DCMF_SIGNED_INT;
            break;
        case A1_INT64:
            datatype = DCMF_SIGNED_LONG_LONG;
            break;
        case A1_UINT32:
            datatype = DCMF_UNSIGNED_INT;
            break;
        case A1_UINT64:
            datatype = DCMF_UNSIGNED_LONG_LONG;
            break;
        case A1_FLOAT:
            datatype = DCMF_FLOAT;
            break;
        default:
            A1U_ERR_ABORT(status != DCMF_SUCCESS, "Unsupported A1_datatype \n");
            break;
        }
    }

    ga_active += 1;
    done_callback.function = A1DI_Generic_done;
    done_callback.clientdata = (void *) &ga_active;

    status = DCMF_Allreduce(&A1D_GlobalAllreduce_protocol,
                            &ar_crequest,
                            done_callback,
                            DCMF_SEQUENTIAL_CONSISTENCY,
                            &geometry,
                            (char *) in,
                            (char *) out,
                            count,
                            datatype,
                            reduce_op);

    A1DI_Conditional_advance(ga_active > 0);

    fn_exit:
    if (in_abs != NULL) 
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
        status = A1DI_GlobalAllreduce(count, a1_op, a1_type, in, out);
        A1U_ERR_ABORT(status != A1_SUCCESS,
                      "A1DI_GlobalAllreduce returned with an error");
        goto fn_exit;
    }
    else
    {
        A1U_ERR_POP(1,
                    "A1D_Allreduce_group not implemented for non-world groups!");
        goto fn_fail;
    }

    fn_exit: A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;

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
        A1U_ERR_POP(1,
                    "A1DI_NbAllreduce has not been implemented \n");
    }
    else
    {
        A1U_ERR_POP(1,
                    "A1D_NbAllreduce_group not implemented for non-world groups!");
    }

    fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

    fn_fail:
    goto fn_exit;

}
