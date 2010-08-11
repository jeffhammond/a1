/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

A1D_Request_pool_t A1D_Request_pool;

void A1DI_Reset_request_pool()
{

    A1U_FUNC_ENTER();

    A1D_Request_pool.current = A1D_Request_pool.head;

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}

DCMF_Request_t* A1DI_Get_request()
{

    int index;
    A1D_Request_info_t *a1_request = NULL;

    A1U_FUNC_ENTER();

    if (!A1D_Request_pool.current)
    {
        if (a1_requestpool_info.total_size < a1_requestpool_info.limit_size)
        {
            A1DI_Request_pool_increment();
        }
        else
        {
            A1DI_Flush_all();
        }
    }
    a1_request = A1D_Request_pool.current;
    A1D_Request_pool.current = A1D_Request_pool.current->next;

  fn_exit:
    A1U_FUNC_EXIT();
    return &(a1_request->request);

  fn_fail:
    goto fn_exit;
}

int A1DI_Request_pool_increment()
{

    int result = A1_SUCCESS;
    int index;
    A1D_Request_info_t *request;

    A1U_FUNC_ENTER();

    result = A1DI_Malloc_aligned((void **) &request, 
                                 sizeof(A1D_Request_info_t)
                                       * a1_requestpool_info.increment_size);
    A1U_ERR_POP(result = !request,
                "memory allocation for request pool failed \n");
    a1_requestpool_info.total_size = a1_requestpool_info.total_size + a1_requestpool_info.increment_size;

    A1D_Request_pool.region_ptr[A1D_Request_pool.region_count]
            = (void *) request;
    A1D_Request_pool.region_count++;

    A1D_Request_pool.current = request;
    A1D_Request_pool.tail->next = request;
    request->prev = A1D_Request_pool.tail;
    A1D_Request_pool.tail = &request[a1_requestpool_info.increment_size - 1];
    A1D_Request_pool.tail->next = NULL;
    for (index = 1; index < a1_requestpool_info.increment_size; index++)
    {
        request[index - 1].next = &request[index];
        request[index].prev = &request[index - 1];
    }

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
    A1D_Request_info_t *request;

    A1U_FUNC_ENTER();

    max_regions = (a1_requestpool_info.limit_size - a1_requestpool_info.initial_size)
                        / a1_requestpool_info.increment_size + 1;
    result = A1DI_Malloc_aligned((void **) &(A1D_Request_pool.region_ptr),
                                 sizeof(void *) * max_regions);
    A1U_ERR_POP(result != 0, "A1DI_Malloc_aligned failed while allocating request pool\
                       regions list in A1DI_Request_pool_initialize\n");

    result = A1DI_Malloc_aligned((void **) &request, 
                                  sizeof(A1D_Request_info_t)
                                        * a1_requestpool_info.initial_size);
    A1U_ERR_POP(result != 0, "A1DI_Malloc_aligned failed while allocating request pool\
                      in A1DI_Request_pool_initialize\n");
    a1_requestpool_info.total_size = a1_requestpool_info.initial_size;

    A1D_Request_pool.region_ptr[A1D_Request_pool.region_count]
            = (void *) request;
    A1D_Request_pool.region_count++;

    A1D_Request_pool.head = request;
    A1D_Request_pool.current = request;
    A1D_Request_pool.tail = &request[a1_requestpool_info.initial_size - 1];
    A1D_Request_pool.head->prev = NULL;
    A1D_Request_pool.tail->next = NULL;
    for (index = 1; index < a1_requestpool_info.initial_size; index++)
    {
        request[index - 1].next = &request[index];
        request[index].prev = &request[index - 1];
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}