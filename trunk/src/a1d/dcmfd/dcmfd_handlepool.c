/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

A1D_Handle_pool_t A1D_Handle_pool;
A1D_Handle_t **A1D_Active_handle_list;

int A1D_Allocate_handle(A1_handle_t *a1_handle)
{
    int status = A1_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    a1d_handle = A1DI_Get_handle();
    A1U_ERR_POP(a1d_handle == NULL,
                "A1DI_Get_handle returned NULL in A1D_Allocate_handle. Handles exhausted \n");
    *a1_handle = (A1_handle_t *) a1d_handle;

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
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

  fn_fail:
    goto fn_exit;
}

int A1DI_Load_request(A1D_Handle_t *a1d_handle)
{
    int status = A1_SUCCESS;
    A1D_Request_t *a1d_request;

    A1U_FUNC_ENTER();

    a1d_request = A1DI_Get_request();
    A1U_ERR_POP(status = (a1d_request == NULL),
                "A1DI_Get_request returned NULL in A1DI_Load_request. Rquests exhausted \n");
 
    a1d_request->next = a1d_handle->request_list;
    a1d_handle->request_list = a1d_request;

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}


A1D_Handle_t* A1DI_Get_handle()
{
    A1D_Handle_t *a1d_handle = NULL;
    int index;

    A1U_FUNC_ENTER();

    if(A1D_Handle_pool.head == NULL)
    {
        return NULL;
    }              

    a1d_handle = A1D_Handle_pool.head;
    A1D_Handle_pool.head = A1D_Handle_pool.head->next;

    a1d_handle->request_list = NULL;
    a1d_handle->active = 0;

    /* The size of active handle list is equal to handle pool size,
     * So we should find a free index if we had got a handle above.*/
    index = 0;
    while(A1D_Active_handle_list[index] != NULL) index++;
    A1D_Active_handle_list[index] = a1d_handle;
    a1d_handle->active_list_index = index;

  fn_exit:
    A1U_FUNC_EXIT();
    return a1d_handle;

  fn_fail:
    goto fn_exit;
}

void A1DI_Release_handle(A1D_Handle_t *a1d_handle)
{
    A1U_FUNC_ENTER();

    if(a1d_handle->request_list != NULL)
    {
        A1DI_Release_request_list(a1d_handle->request_list);
        a1d_handle->request_list = NULL;
    }

    A1D_Active_handle_list[a1d_handle->active_list_index] = NULL;
    a1d_handle->active_list_index = -1;

    a1d_handle->next = A1D_Handle_pool.head;
    A1D_Handle_pool.head = a1d_handle;

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}

int A1DI_Handle_pool_initialize()
{

    int status = A1_SUCCESS;
    int index;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    status = A1DI_Malloc_aligned((void **) &a1d_handle,
                                 sizeof(A1D_Handle_t) * a1_settings.handlepool_size);
    A1U_ERR_POP(status != 0,
                "A1DI_Malloc_aligned failed while allocating handle pool\
                      in A1DI_Handle_pool_initialize\n");

    A1D_Handle_pool.region_ptr = (void *) a1d_handle;
    A1D_Handle_pool.head = a1d_handle;
    for (index = 0; index < a1_settings.handlepool_size; index++)
    {
        a1d_handle[index].next = &a1d_handle[index+1];
    } 
    a1d_handle[index].next = NULL; 

    status = A1DI_Malloc_aligned((void **) &A1D_Active_handle_list, 
                                 sizeof(A1D_Handle_t *) * a1_settings.handlepool_size);
    A1U_ERR_POP(status != 0,
                "A1DI_Malloc_aligned failed in A1DI_Handle_pool_initialize\n");

    for (index = 0; index < a1_settings.handlepool_size; index++)
    {
        A1D_Active_handle_list[index] = NULL;
    }    

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

void A1DI_Handle_pool_finalize()
{
    int i;

    A1U_FUNC_ENTER();

    A1DI_Free(A1D_Handle_pool.region_ptr);
  
  fn_exit:
    A1U_FUNC_EXIT();
    return;
  
  fn_fail:
    goto fn_exit;
}
