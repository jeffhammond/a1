/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

A1D_Handle_pool_t A1D_Handle_pool;
A1D_Handle_t **A1D_Active_handle_list;

int A1DI_Handle_pool_initialize()
{

    int i, status = A1_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    status = A1DI_Malloc((void **) &a1d_handle,
                         sizeof(A1D_Handle_t) * a1d_settings.handlepool_size);
    A1U_ERR_POP(status != A1_SUCCESS,"A1DI_Malloc failed ");

    A1D_Handle_pool.region_ptr = (void *) a1d_handle;
    A1D_Handle_pool.head = a1d_handle;
    for (i = 0; i < a1d_settings.handlepool_size; i++)
        a1d_handle[i].next = &a1d_handle[i + 1];
    a1d_handle[i].next = NULL;

    status = A1DI_Malloc((void **) &A1D_Active_handle_list,
                         sizeof(A1D_Handle_t *) * a1d_settings.handlepool_size);
    A1U_ERR_POP(status != A1_SUCCESS, "A1DI_Malloc failed ");

    for (i = 0; i < a1d_settings.handlepool_size; i++)
        A1D_Active_handle_list[i] = NULL;

    fn_exit: A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

void A1DI_Handle_pool_finalize()
{
    int i;

    A1U_FUNC_ENTER();

    for (i = 0; i < a1d_settings.handlepool_size; i++)
    {
        if (A1D_Active_handle_list[i] != NULL)
            A1DI_Conditional_advance((A1D_Active_handle_list[i])->active > 0);
    }

    A1DI_Free(A1D_Active_handle_list);

    A1DI_Free(A1D_Handle_pool.region_ptr);

    fn_exit: A1U_FUNC_EXIT();
    return;

    fn_fail: goto fn_exit;
}

void A1DI_Set_handle(A1D_Request_t* request, A1_handle_t handle)
{
    A1U_FUNC_ENTER();

    request->handle_ptr = handle;

    fn_exit: A1U_FUNC_EXIT();
    return;

    fn_fail: goto fn_exit;
}

A1D_Handle_t* A1DI_Get_handle()
{
    A1D_Handle_t *a1d_handle = NULL;
    int i;

    A1U_FUNC_ENTER();

    if (A1D_Handle_pool.head == NULL) return NULL;

    a1d_handle = A1D_Handle_pool.head;
    A1D_Handle_pool.head = A1D_Handle_pool.head->next;

    a1d_handle->active = 0;

    /* The size of active handle list is equal to handle pool size,
     * So we should find a free index if we had got a handle above.*/
    i = 0;
    while (A1D_Active_handle_list[i] != NULL) i++;
    A1D_Active_handle_list[i] = a1d_handle;
    a1d_handle->active_list_index = i;

  fn_exit: 
    A1U_FUNC_EXIT();
    return a1d_handle;

  fn_fail: 
    goto fn_exit;
}

void A1DI_Release_handle(A1D_Handle_t *a1d_handle)
{
    A1U_FUNC_ENTER();

    A1DI_Conditional_advance(a1d_handle->active > 0);

    A1D_Active_handle_list[a1d_handle->active_list_index] = NULL;
    a1d_handle->active_list_index = -1;

    a1d_handle->next = A1D_Handle_pool.head;
    A1D_Handle_pool.head = a1d_handle;

    fn_exit: A1U_FUNC_EXIT();
    return;

    fn_fail: goto fn_exit;
}

int A1D_Allocate_handle(A1_handle_t *a1_handle)
{
    int status = A1_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    a1d_handle = A1DI_Get_handle();
    A1U_ERR_POP(status = (a1d_handle == NULL),
                "A1DI_Get_handle returned NULL in A1D_Allocate_handle.\n");
    *a1_handle = (A1_handle_t) a1d_handle;

    fn_exit: A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

int A1D_Release_handle(A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    A1U_ASSERT(a1_handle != NULL, status)

    a1d_handle = (A1D_Handle_t *) a1_handle;
    A1DI_Release_handle(a1d_handle);

    fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}
