/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

DCMF_Protocol_t A1D_GlobalBcast_protocol;

int A1DI_GlobalBcast_initialize()
{
    int status = A1_SUCCESS;
    DCMF_GlobalBcast_Configuration_t conf;

    A1U_FUNC_ENTER();

    conf.protocol = DCMF_DEFAULT_GLOBALBCAST_PROTOCOL;
    status = DCMF_GlobalBcast_register(&A1D_GlobalBcast_protocol,
                                       &conf);
    A1U_ERR_POP(status != DCMF_SUCCESS,"DCMF_GlobalBcast_register failed ");

    fn_exit:
    A1U_FUNC_EXIT();
    return status;

    fn_fail:
    goto fn_exit;
}

int A1DI_GlobalBcast(int root,
                     int count,
                     void *buffer)
{
    int status = A1_SUCCESS;
    DCMF_Request_t request;
    DCMF_Callback_t done_callback;
    volatile unsigned gb_active = 0;

    A1U_FUNC_ENTER();

    gb_active += 1;
    done_callback.function = A1DI_Generic_done;
    done_callback.clientdata = (void *) &gb_active;

    status = DCMF_GlobalBcast(&A1D_GlobalBcast_protocol,
                              &request,
                              done_callback,
                              DCMF_SEQUENTIAL_CONSISTENCY,
                              root,
                              (char *) buffer,
                              count);
    A1U_ERR_POP(status != DCMF_SUCCESS,"DCMF_GlobalBcast failed ");

    A1DI_Conditional_advance(gb_active > 0);

    fn_exit:
    A1U_FUNC_EXIT();
    return status;

    fn_fail:
    goto fn_exit;
}

int A1DI_NbGlobalBcast(int root,
                       int count,
                       void *buffer,
                       A1D_Handle_t *a1d_handle)
{
    int status = A1_SUCCESS;
    A1D_Request_t *a1d_request;
    DCMF_Callback_t done_callback;

    A1U_FUNC_ENTER();

    a1d_request = A1DI_Get_request(1);
    A1U_ERR_POP(status = (a1d_request == NULL),
                "A1DI_Get_request returned error \n");
    A1DI_Set_handle(a1d_request, a1d_handle);

    a1d_handle->active++;

    done_callback.function = A1DI_Request_done;
    done_callback.clientdata = (void *) a1d_request;

    status = DCMF_GlobalBcast(&A1D_GlobalBcast_protocol,
                              &(a1d_request->request).message_request, /* TODO verify */
                              done_callback,
                              DCMF_SEQUENTIAL_CONSISTENCY,
                              root,
                              (char *) buffer,
                              count);
    A1U_ERR_POP(status != DCMF_SUCCESS,"DCMF_GlobalBcast failed ");

    fn_exit:
    A1U_FUNC_EXIT();
    return status;

    fn_fail:
    goto fn_exit;
}

int A1D_Bcast_group(A1_group_t* group,
                    int root,
                    int count,
                    void* buffer)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if (group == A1_GROUP_WORLD || group == NULL)
    {
        status = A1DI_GlobalBcast(root, count, buffer);
        A1U_ERR_ABORT(status != A1_SUCCESS,
                      "A1DI_GlobalBcast failed ");
        goto fn_exit;
    }
    else
    {
        A1U_ERR_POP(1,"A1D_Bcast_group not implemented for non-world groups!");
        goto fn_fail;
    }

    fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}


int A1D_NbBcast_group(A1_group_t* group,
                      int root,
                      int count,
                      void* buffer,
                      A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if (group == A1_GROUP_WORLD || group == NULL)
    {
        a1d_handle = (A1D_Handle_t *) a1_handle;

        status = A1DI_NbGlobalBcast(root, count, buffer, a1d_handle);
        A1U_ERR_ABORT(status != A1_SUCCESS,
                      "A1DI_NbGlobalBcast failed ");
        goto fn_exit;
    }
    else
    {
        A1U_ERR_POP(1,"A1D_Bcast_group not implemented for non-world groups!");
        goto fn_fail;
    }

    fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

    fn_fail:
    goto fn_exit;

}
