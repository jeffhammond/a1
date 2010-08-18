/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

A1D_Handle_pool_t A1D_Handle_pool;

void A1DI_Release_handle(A1D_Handle_t *a1d_handle)
{
     
    A1U_FUNC_ENTER();

    A1DI_Release_request_list(a1d_handle->request_list);

    a1d_handle->request_list = NULL;
    *(a1d_handle->user_handle_ptr) = NULL;

    a1d_handle->next = A1D_Handle_pool.head;
    A1D_Handle_pool.head = a1d_handle;
    
  fn_exit:
    A1U_FUNC_EXIT();
    return;
    
  fn_fail:
    goto fn_exit;
}  

void A1DI_Load_request(A1D_Handle_t *a1d_handle)
{
    A1D_Request_t *a1d_request;

    A1U_FUNC_ENTER();

    a1d_request = A1DI_Get_request();
 
    a1d_request->next = a1d_handle->request_list;
    a1d_handle->request_list = a1d_request;

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}

A1D_Handle_t* A1DI_Get_handle()
{
    A1D_Handle_t *a1d_handle = NULL;

    A1U_FUNC_ENTER();

    A1DI_Conditional_advance(A1D_Handle_pool.head == NULL);         

    a1d_handle = A1D_Handle_pool.head;
    A1D_Handle_pool.head = A1D_Handle_pool.head->next;

    a1d_handle->request_list = NULL;
    a1d_handle->user_handle_ptr = NULL;
    a1d_handle->active = 0;

  fn_exit:
    A1U_FUNC_EXIT();
    return a1d_handle;

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
                                 sizeof(A1D_Handle_t) * A1C_HANDLE_POOL_SIZE);
    A1U_ERR_POP(status != 0,
                "A1DI_Malloc_aligned failed while allocating handle pool\
                      in A1DI_Handle_pool_initialize\n");
    a1_requestpool_info.total_size = a1_requestpool_info.initial_size;

    A1D_Handle_pool.region_ptr = (void *) a1d_handle;
    A1D_Handle_pool.head = a1d_handle;
    for (index = 0; index < a1_requestpool_info.initial_size-1; index++)
    {
        a1d_handle[index].next = &a1d_handle[index+1];
    } 
    a1d_handle[index].next = NULL; 

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1D_Wait_handle(A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    a1d_handle = (A1D_Handle_t *) a1_handle;
    A1DI_Conditional_advance(a1d_handle->active > 0);

    A1DI_Release_handle(a1_handle);

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int A1D_Test_handle(A1_handle_t a1_handle, A1_bool_t* completed)
{
    int status = A1_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    a1d_handle = (A1D_Request_t *) a1_handle;
    A1DI_Advance();
    *completed = (a1d_handle->active > 0) ? A1_FALSE : A1_TRUE;

    if(*completed == A1_TRUE)
         A1DI_Release_handle(a1d_handle);

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}
