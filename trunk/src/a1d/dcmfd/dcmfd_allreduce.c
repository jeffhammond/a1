/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Geometry_t geometry;
DCMF_CollectiveProtocol_t A1D_Barrier_protocol, A1D_Localbarrier_protocol;
DCMF_CollectiveProtocol_t *barrier_ptr, *localbarrier_ptr;
DCMF_Barrier_Configuration_t barrier_conf;

DCMF_CollectiveRequest_t crequest;

DCMF_CollectiveProtocol_t A1D_GlobalAllreduce_tree_protocol;
DCMF_CollectiveProtocol_t A1D_GlobalAllreduce_torus_protocol;
DCMF_Allreduce_Configuration_t allreduce_tree_conf;
DCMF_Allreduce_Configuration_t allreduce_torus_conf;

unsigned *allreduce_ranklist;

static DCMF_Geometry_t *getGeometry(int x)
{
    return &geometry;
}

/* NOTES
 *
 * DCMF_Allreduce cannot do DCMF_PROD using the tree.
 * DCMF_Allreduce cannot do in-place for DCMF_PROD using either the tree or the torus.
 * DCMF_Allreduce cannot use tree for DCMF_SUM with DCMF_FLOAT.
 *
 */

int A1DI_GlobalAllreduce_initialize()
{
    int i, status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    barrier_conf.protocol = DCMF_GI_BARRIER_PROTOCOL;
    barrier_conf.cb_geometry = getGeometry;
    status = DCMF_Barrier_register(&A1D_Barrier_protocol, &barrier_conf);

    barrier_conf.protocol = DCMF_LOCKBOX_BARRIER_PROTOCOL;
    barrier_conf.cb_geometry = getGeometry;
    status = DCMF_Barrier_register(&A1D_Localbarrier_protocol, &barrier_conf);

    status
    = A1DI_Malloc((void **) &allreduce_ranklist, A1D_Process_info.num_ranks * sizeof(unsigned));
    A1U_ERR_POP(status != 0,
                "A1DI_Malloc returned with error %d \n", status);

    for (i = 0; i < A1D_Process_info.num_ranks; i++) allreduce_ranklist[i] = i;

    barrier_ptr = &A1D_Barrier_protocol;
    localbarrier_ptr = &A1D_Localbarrier_protocol;
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

    /* register tree allreduce for operations/types which support it */
    allreduce_tree_conf.protocol = DCMF_TREE_ALLREDUCE_PROTOCOL;
    allreduce_tree_conf.cb_geometry = getGeometry;
    allreduce_tree_conf.reuse_storage = 1;
    status = DCMF_Allreduce_register(&A1D_GlobalAllreduce_tree_protocol,
                                     &allreduce_tree_conf);
    A1U_ERR_POP(status != DCMF_SUCCESS,
                "DCMF_Allreduce_register (tree) returned with error %d \n", status);

    /* register torus allreduce for operations/types not supported by the tree */
    allreduce_torus_conf.protocol = DCMF_TORUS_BINOMIAL_ALLREDUCE_PROTOCOL;
    allreduce_torus_conf.cb_geometry = getGeometry;
    allreduce_torus_conf.reuse_storage = 1;
    status = DCMF_Allreduce_register(&A1D_GlobalAllreduce_torus_protocol,
                                     &allreduce_torus_conf);
    A1U_ERR_POP(status != DCMF_SUCCESS,
                "DCMF_Allreduce_register (torus) returned with error %d \n", status);

    /* check if geometry is valid for protocols */
    A1U_ERR_POP(DCMF_Geometry_analyze(&geometry, &A1D_GlobalAllreduce_tree_protocol),
                "DCMF_Geometry_analyze (tree) returned with error %d \n", status);
    A1U_ERR_POP(DCMF_Geometry_analyze(&geometry, &A1D_GlobalAllreduce_torus_protocol),
                "DCMF_Geometry_analyze (torus) returned with error %d \n", status);

    fn_exit: A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

int A1DI_GlobalAllreduce_finalize()
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_Free(allreduce_ranklist);

    fn_exit: A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

int A1DI_GlobalAllreduce_tree(int count,
                              DCMF_Op dcmf_op,
                              DCMF_Dt dcmf_type,
                              void *in,
                              void *out)
{
    int status = A1_SUCCESS;
    DCMF_CollectiveRequest_t request;
    DCMF_Callback_t done_callback;
    int bytes = 0;
    volatile unsigned active = 0;

    A1U_FUNC_ENTER();

    active += 1;
    done_callback.function = A1DI_Generic_done;
    done_callback.clientdata = (void *) &active;

    status = DCMF_Allreduce(&A1D_GlobalAllreduce_tree_protocol,
                            &request,
                            done_callback,
                            DCMF_SEQUENTIAL_CONSISTENCY,
                            &geometry,
                            (char *) in,
                            (char *) out,
                            count,
                            dcmf_type,
                            dcmf_op);

    A1DI_Conditional_advance(active > 0);

    fn_exit:
    A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

int A1DI_GlobalAllreduce_torus(int count,
                               DCMF_Op dcmf_op,
                               DCMF_Dt dcmf_type,
                               void *in,
                               void *out)
{
    int status = A1_SUCCESS;
    DCMF_CollectiveRequest_t request;
    DCMF_Callback_t done_callback;
    int bytes = 0;
    volatile unsigned active = 0;

    A1U_FUNC_ENTER();

    active += 1;
    done_callback.function = A1DI_Generic_done;
    done_callback.clientdata = (void *) &active;

    status = DCMF_Allreduce(&A1D_GlobalAllreduce_torus_protocol,
                            &request,
                            done_callback,
                            DCMF_SEQUENTIAL_CONSISTENCY,
                            &geometry,
                            (char *) in,
                            (char *) out,
                            count,
                            dcmf_type,
                            dcmf_op);

    A1DI_Conditional_advance(active > 0);

    fn_exit:
    A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

int A1D_Allreduce_group(A1_group_t* group,
                        int count,
                        A1_reduce_op_t a1_op,
                        A1_datatype_t a1_type,
                        void* in,
                        void* out)
{
    int status = A1_SUCCESS;
    DCMF_Op reduce_op;
    DCMF_Dt datatype;
    int bytes;
    void* in_abs;
    int use_tree = 0;
    int outofplc = 0;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    /* set the DCMF type */

    switch (a1_type)
    {
        case A1_DOUBLE:
            datatype = DCMF_DOUBLE;
            break;
        case A1_FLOAT:
            datatype = DCMF_FLOAT;
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
        default:
            A1U_ERR_ABORT(1, "A1D_Allreduce_group unsupported datatype ");
            break;
    }

    /* set the DCMF reduce op */

    switch (a1_op)
    {
        case A1_SUM:
            reduce_op = DCMF_SUM;
            break;
        case A1_MAX:
            reduce_op = DCMF_MAX;
            break;
        case A1_MIN:
            reduce_op = DCMF_MIN;
            break;
        case A1_MAXABS:
            reduce_op = DCMF_MAX;
            break;
        case A1_MINABS:
            reduce_op = DCMF_MIN;
            break;
        case A1_PROD:
            reduce_op = DCMF_PROD;
            break;
        case A1_OR:
            reduce_op = DCMF_LOR;
            break;
        default:
            A1U_ERR_POP(1, "A1D_Allreduce_group unsupported reduce op ");
            break;
    }

    /* if necessary, take the absolute value or copy the buffer */

    if ( ( a1_op == A1_MAXABS) || ( a1_op == A1_MINABS) )
    {
        switch (a1_type)
        {
            case A1_DOUBLE:
                bytes = count * sizeof(double);
                status = A1DI_Malloc(&in_abs, bytes);
                A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Malloc failed ");
                A1DI_ABS(double, in, in_abs, count);
                break;
            case A1_FLOAT:
                bytes = count * sizeof(float);
                status = A1DI_Malloc(&in_abs, bytes);
                A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Malloc failed ");
                A1DI_ABS(float, in, in_abs, count);
                break;
            case A1_INT32:
                bytes = count * sizeof(int32_t);
                status = A1DI_Malloc(&in_abs, bytes);
                A1U_ERR_POP(status != A1_SUCCESS, "A1DI_Malloc failed ");
                A1DI_ABS(int32_t, in, in_abs, count);
                break;
            case A1_INT64:
                bytes = count * sizeof(int64_t);
                status = A1DI_Malloc(&in_abs, bytes);
                A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Malloc failed ");
                A1DI_ABS(int64_t, in, in_abs, count);
                break;
            case A1_UINT32: /* no need to do ABS in this case */
                bytes = count * sizeof(uint32_t);
                status = A1DI_Malloc(&in_abs, bytes);
                A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Malloc failed ");
                A1DI_COPY(uint32_t, in, in_abs, count);
                break;
            case A1_UINT64: /* no need to do ABS in this case */
                bytes = count * sizeof(uint64_t);
                status = A1DI_Malloc(&in_abs, bytes);
                A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Malloc failed ");
                A1DI_COPY(uint64_t, in, in_abs, count);
                break;
            default:
                A1U_ERR_POP(1,"unreachable - this error will be thrown above ");
                break;
        }
        outofplc = 1;
    }
    else if ( ( a1_op == A1_PROD) || ( a1_op == A1_OR) )
    {
        switch (a1_type)
        {
            case A1_DOUBLE:
                bytes = count * sizeof(double);
                status = A1DI_Malloc(&in_abs, bytes);
                A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Malloc failed ");
                A1DI_COPY(double, in, in_abs, count);
                break;
            case A1_FLOAT:
                bytes = count * sizeof(float);
                status = A1DI_Malloc(&in_abs, bytes);
                A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Malloc failed ");
                A1DI_COPY(float, in, in_abs, count);
                break;
            case A1_INT32:
                bytes = count * sizeof(int32_t);
                status = A1DI_Malloc(&in_abs, bytes);
                A1U_ERR_POP(status != A1_SUCCESS, "A1DI_Malloc failed ");
                A1DI_COPY(int32_t, in, in_abs, count);
                break;
            case A1_INT64:
                bytes = count * sizeof(int64_t);
                status = A1DI_Malloc(&in_abs, bytes);
                A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Malloc failed ");
                A1DI_COPY(int64_t, in, in_abs, count);
                break;
            case A1_UINT32:
                bytes = count * sizeof(uint32_t);
                status = A1DI_Malloc(&in_abs, bytes);
                A1U_ERR_POP(status != A1_SUCCESS, "A1DI_Malloc failed ");
                A1DI_COPY(uint32_t, in, in_abs, count);
                break;
            case A1_UINT64:
                bytes = count * sizeof(uint64_t);
                status = A1DI_Malloc(&in_abs, bytes);
                A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Malloc failed ");
                A1DI_COPY(uint64_t, in, in_abs, count);
                break;
            default:
                A1U_ERR_POP(1,"unreachable - this error will be thrown above ");
                break;
        }
        outofplc = 1;
    }
    else
    {
        outofplc = 0;
    }

    /* determine if we can use the tree network for this op/type combo */

    switch (a1_op)
    {
        /* DCMF can use tree for MAX/MIN and FLOAT/DOUBLE/INT32/INT64 types */
        case A1_MAX:
        case A1_MIN:
        case A1_MAXABS:
        case A1_MINABS:
            use_tree = 1;
            break;
            /* FLOAT on tree only for MIN/MAX */
        case A1_SUM:
            use_tree = (a1_type != A1_FLOAT);
            break;
            /* these two can't be done on the tree network */
        case A1_PROD:
        case A1_OR:
            use_tree = 0;
            break;
        default:
            A1U_ERR_POP(1, "A1D_Allreduce_group unsupported reduce op ");
            break;
    }

    /* finally, we do the collective */

    if (group == A1_GROUP_WORLD || group == NULL)
    {
        if (use_tree == 1)
        {
            if ( outofplc == 1)
            {
                status = A1DI_GlobalAllreduce_tree(count, reduce_op, datatype, in_abs, out);
                A1U_ERR_ABORT(status != A1_SUCCESS,"A1DI_GlobalAllreduce_tree failed ");
                A1DI_Free(in_abs);
                goto fn_exit;
            }
            else
            {
                status = A1DI_GlobalAllreduce_tree(count, reduce_op, datatype, in, out);
                A1U_ERR_ABORT(status != A1_SUCCESS,"A1DI_GlobalAllreduce_tree failed ");
                goto fn_exit;
            }
        }
        else /* use torus */
        {
            if ( outofplc == 1)
            {
                status = A1DI_GlobalAllreduce_torus(count, reduce_op, datatype, in_abs, out);
                A1U_ERR_ABORT(status != A1_SUCCESS,"A1DI_GlobalAllreduce_torus failed ");
                A1DI_Free(in_abs);
                goto fn_exit;
            }
            else
            {
                status = A1DI_GlobalAllreduce_torus(count, reduce_op, datatype, in, out);
                A1U_ERR_ABORT(status != A1_SUCCESS,"A1DI_GlobalAllreduce_torus failed ");
                goto fn_exit;
            }
        }
    }
    else
    {
        A1U_ERR_POP(1,"A1D_Allreduce_group not implemented for non-world groups!");
        goto fn_fail;
    }

    fn_exit:
    A1DI_CRITICAL_EXIT();
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

    A1U_ERR_POP(1,"A1DI_NbAllreduce has not been implemented ");

    fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}


