/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Protocol_t A1D_GlobalBarrier_protocol;

DCMF_CollectiveProtocol_t A1D_Barrier_protocol, A1D_Localbarrier_protocol;
DCMF_CollectiveProtocol_t *barrier_ptr, *localbarrier_ptr;
DCMF_CollectiveProtocol_t *barrier_ptr, *localbarrier_ptr;
DCMF_Geometry_t geometry;
DCMF_Barrier_Configuration_t barrier_conf;
DCMF_CollectiveRequest_t crequest;

DCMF_Geometry_t *getGeometry (int x)
{
    return &geometry;
}

int A1DI_GlobalBarrier_initialize()
{
    int status = A1_SUCCESS;
    DCMF_GlobalBarrier_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_GLOBALBARRIER_PROTOCOL;
    status = DCMF_GlobalBarrier_register(&A1D_GlobalBarrier_protocol, &conf);
    A1U_ERR_POP(status != DCMF_SUCCESS,
                "DCMF_GlobalBarrier_register returned with error %d \n",
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
    A1U_ERR_ABORT(status != DCMF_SUCCESS,
                  "DCMF_GlobalBarrier returned with an error");

    A1DI_Conditional_advance(active > 0);

    fn_exit: A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;

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
    A1U_ERR_ABORT(status != DCMF_SUCCESS,
                  "DCMF_GlobalBarrier returned with an error");

    fn_exit:
    A1U_FUNC_EXIT();
    return status;

    fn_fail:
    goto fn_exit;

}

/********************************************************/

int A1D_Barrier_group(A1_group_t* group)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if (group == A1_GROUP_WORLD || group == NULL)
    {

        status = A1DI_GlobalBarrier();
        A1U_ERR_ABORT(status != A1_SUCCESS,
                      "DCMF_GlobalBarrier returned with an error");
        goto fn_exit;
    }
    else
    {
        A1U_ERR_POP(1,
                    "A1D_Barrier_group not implemented for non-world groups!");
        goto fn_fail;
    }

    fn_exit: A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;

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
        A1U_ERR_ABORT(status != A1_SUCCESS,
                      "DCMF_NbGlobalBarrier returned with an error");

        goto fn_exit;
    }
    else
    {
        A1U_ERR_POP(1,
                    "A1D_NbBarrier_group not implemented for non-world groups!");
        goto fn_fail;
    }

    fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

    fn_fail:
    goto fn_exit;

}

/*******************************************************************/

int A1D_Sync_group(A1_group_t* group)
{

    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if (group == A1_GROUP_WORLD || group == NULL)
    {
        status = A1DI_Flush_all();
        A1U_ERR_ABORT(status != A1_SUCCESS,
                      "A1DI_Flush_all returned with an error");
        status = A1DI_GlobalBarrier();
        A1U_ERR_ABORT(status != A1_SUCCESS,
                      "A1DI_GlobalBarrier returned with an error");
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
        A1U_ERR_ABORT(status != A1_SUCCESS,
                      "A1DI_Flush_all returned with an error");

        status = A1DI_NbGlobalBarrier(a1d_handle);
        A1U_ERR_ABORT(status != A1_SUCCESS,
                      "A1DI_NbGlobalBarrier returned with an error");

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
