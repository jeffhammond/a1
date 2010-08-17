/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

A1D_Request_pool_t A1D_Request_pool;

A1D_Request_t* A1DI_Get_request()
{
    A1D_Request_t *a1d_request = NULL;

    A1U_FUNC_ENTER();

    if (!A1D_Request_pool.head)
    {
        if (a1_requestpool_info.total_size < a1_requestpool_info.limit_size)
        {
            A1DI_Request_pool_increment();
        }
        else
        {
            A1DI_Conditional_advance(!A1D_Request_pool.head);
        }
    }

    a1d_request = A1D_Request_pool.head;
    a1d_request->next = NULL;
    a1d_request->buffer_ptr = NULL;
    A1D_Request_pool.head = A1D_Request_pool.head->next;

  fn_exit: 
    A1U_FUNC_EXIT();
    return a1d_request;

  fn_fail: 
    goto fn_exit;
}

void A1DI_Release_request(A1D_Request_t *a1d_request)
{
    A1U_FUNC_ENTER();

    a1d_request->next = A1D_Request_pool.head;
    A1D_Request_pool.head = a1d_request;

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}

void A1DI_Release_request_list(A1D_Request_t *a1d_request_list)
{
    A1D_Request_t *tail_request;

    A1U_FUNC_ENTER();

    tail_request = a1d_request_list;
    while(tail_request->next != NULL) 
    { 
        if(tail_request->buffer_ptr != NULL)
        {
            A1DI_Free(tail_request->buffer_ptr);
        }
        tail_request = temp_request->next;
    }
    if(tail_request->buffer_ptr != NULL)
    {
        A1DI_Free(tail_request->buffer_ptr);
    }
    
    tail_request->next = A1D_Request_pool.head;
    A1D_Request_pool.head = a1d_request_list;   

  fn_exit:
    A1U_FUNC_EXIT(); 
    return;

  fn_fail:
    goto fn_exit;
}

int A1DI_Request_pool_increment()
{

    int result = A1_SUCCESS;
    int index;
    A1D_Request_t *a1d_request;

    A1U_FUNC_ENTER();

    result = A1DI_Malloc_aligned((void **) &a1d_request, 
                                 sizeof(A1D_Request_t) * a1_requestpool_info.increment_size);
    A1U_ERR_POP(result = !a1d_request,
                "memory allocation for request pool failed \n");
    a1_requestpool_info.total_size = a1_requestpool_info.total_size
            + a1_requestpool_info.increment_size;

    A1D_Request_pool.region_ptr[A1D_Request_pool.region_count] = (void *) a1d_request;
    A1D_Request_pool.region_count++;

    A1D_Request_pool.head = a1d_request;
    for (index = 0; index < a1_requestpool_info.increment_size-1; index++)
    {
        a1d_request[index - 1].next = &a1d_request[index];
    } 
    a1d_request[index] = NULL;

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}

int A1DI_Request_pool_initialize()
{

    int result = A1_SUCCESS;
    int index, max_regions;
    A1D_Request_t *a1d_request;

    A1U_FUNC_ENTER();

    max_regions = (a1_requestpool_info.limit_size
            - a1_requestpool_info.initial_size)
            / a1_requestpool_info.increment_size + 1;
    result = A1DI_Malloc_aligned((void **) &(A1D_Request_pool.region_ptr),
                                 sizeof(void *) * max_regions);
    A1U_ERR_POP(result != 0,
                "A1DI_Malloc_aligned failed while allocating request pool\
                       regions list in A1DI_Request_pool_initialize\n");

    result = A1DI_Malloc_aligned((void **) &a1d_request, 
                                 sizeof(A1D_Request_t) * a1_requestpool_info.initial_size);
    A1U_ERR_POP(result != 0,
                "A1DI_Malloc_aligned failed while allocating request pool\
                      in A1DI_Request_pool_initialize\n");
    a1_requestpool_info.total_size = a1_requestpool_info.initial_size;

    A1D_Request_pool.region_ptr[A1D_Request_pool.region_count] = (void *) a1d_request;
    A1D_Request_pool.region_count++;

    A1D_Request_pool.head = a1d_request;
    for (index = 0; index < a1_requestpool_info.initial_size-1; index++)
    {
        a1d_request[index].next = &a1d_request[index+1];
    }
    a1d_request[index]->next = NULL;

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}


void A1DI_Request_pool_finalize()
{
    int i;

    A1U_FUNC_ENTER();

    for (i = 0; i < A1D_Request_pool.region_count; i++)
    {
        A1DI_Free(A1D_Request_pool.region_ptr[i]);
    }
    A1DI_Free(A1D_Request_pool.region_ptr);

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}
