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

    A1DI_Release_request_list(a1d_handle->request_head, a1d_handle->request_tail);

    *(a1d_handle->a1_handle_ptr) = NULL;

    a1d_handle->next = A1D_Handle_pool.head;
    A1D_Handle_pool.head = a1d_handle;
    
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

    while(A1D_Handle_pool.head != NULL) 
        A!DI_Advance();         

    a1d_handle = A1D_Handle_pool.head;
    A1D_Handle_pool.head = A1D_Handle_pool.head->next;

    a1d_handle->request_head = NULL;
    a1d_handle->request_tail = NULL;
    a1d_handle->a1_handle_ptr = NULL;
    a1d_handle->done_active = 0;

  fn_exit:
    A1U_FUNC_EXIT();
    return a1_request;

  fn_fail:
    goto fn_exit;
}

int A1DI_Handle_pool_initialize()
{

    int result = A1_SUCCESS;
    int index;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    result = A1DI_Malloc_aligned((void **) &a1d_handle,
                                 sizeof(A1D_Handle_t) * A1C_HANDLE_POOL_SIZE);
    A1U_ERR_POP(result != 0,
                "A1DI_Malloc_aligned failed while allocating handle pool\
                      in A1DI_Handle_pool_initialize\n");
    a1_requestpool_info.total_size = a1_requestpool_info.initial_size;

    A1D_Handle_pool.region_ptr = (void *) a1d_handle;
    A1D_Handle_pool.head = a1d_handle;
    for (index = 1; index < a1_requestpool_info.initial_size-1; index++)
    {
        a1d_handle[index - 1].next = &request[index];
    } 
    a1d_handle[index] = NULL; 

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}

int A1D_Wait_handle(A1_handle_t a1_handle)
{
    DCMF_Result result = DCMF_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    a1d_handle = (A1D_Handle_t *) a1_handle;
    A1DI_Conditional_advance(a1_request->done_active > 0);

    A1DI_Release_handle(a1_handle);

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}

int A1D_Test_handle(A1_handle_t a1_handle, A1_bool_t* completed)
{
    DCMF_Result result = DCMF_SUCCESS;
    A1D_handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    a1d_handle = (A1D_Request_t *) a1_handle;
    A1DI_Advance();
    *completed = (a1d_handle->done_active > 0) ? A1_FALSE : A1_TRUE;

    if(*completed == A1_TRUE)
         A1DI_Release_handle(a1_request);

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}
