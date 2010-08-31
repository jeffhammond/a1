/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

A1D_Buffer_pool_t A1D_Buffer_pool;

A1D_Buffer_t* A1DI_Get_buffer(int size, int wait_and_advance)
{
    A1D_Buffer_t *a1d_buffer = NULL;
    int index;

    A1U_FUNC_ENTER();

    do { 
        for(index=0; index<A1C_NUM_BUF_SIZES; index++) 
        {
           if((size <= A1D_Buffer_pool.sizes[index]) && A1D_Buffer_pool.pool_heads[index] != NULL)
           {
              a1d_buffer = A1D_Buffer_pool.pool_heads[index];
              A1D_Buffer_pool.pool_heads[index] = A1D_Buffer_pool.pool_heads[index]->next; 
              a1d_buffer->pool_index = index;
              return a1d_buffer;
            } 
         }
 
         if(wait_and_advance) 
         {
             A1DI_Advance();
         }
         else
         {
              void* new_buffer;
              A1DI_Malloc((void **) &new_buffer, sizeof(A1D_Buffer_t) + size);
              a1d_buffer = (A1D_Buffer_t *) new_buffer;
              a1d_buffer->buffer_ptr = (void *) ((size_t) new_buffer + sizeof(A1D_Buffer_t));
              a1d_buffer->pool_index = -1;
              return a1d_buffer;
         }
     } while(a1d_buffer == NULL);	

  fn_exit: 
    A1U_FUNC_EXIT();
    return a1d_buffer;

  fn_fail: 
    goto fn_exit;
}

void A1DI_Release_buffer(A1D_Buffer_t *a1d_buffer)
{
    A1U_FUNC_ENTER();

    if(a1d_buffer->pool_index == -1) 
    {
       A1DI_Free(a1d_buffer);
    }
    else
    {
       a1d_buffer->next = A1D_Buffer_pool.pool_heads[a1d_buffer->pool_index];
       A1D_Buffer_pool.pool_heads[a1d_buffer->pool_index] = a1d_buffer;
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}

int A1DI_Buffer_pool_initialize()
{
    int status = A1_SUCCESS;
    int i, j;
    A1D_Buffer_t *a1d_buffer;

    A1U_FUNC_ENTER();

    /*TODO: We should make these assignments dynamic*/
    A1D_Buffer_pool.sizes[0] = a1_settings.put_packetsize_limit;
    A1D_Buffer_pool.sizes[1] = a1_settings.putacc_packetsize_limit;

    A1D_Buffer_pool.limits[0] = A1C_PUTGET_BUFFERPOOL_LIMIT;
    A1D_Buffer_pool.limits[1] = A1C_PUTACC_BUFFERPOOL_LIMIT;    

    for(i=0; i<A1C_NUM_BUF_SIZES; i++)
    {
        /* Initializing Put and Get buffer pool */
        status = A1DI_Malloc((void **) &(A1D_Buffer_pool.pool_region_ptrs[i]), 
                                     sizeof(A1D_Buffer_t) * A1D_Buffer_pool.limits[i]);
        A1U_ERR_POP(status != A1_SUCCESS,
                    "A1DI_Malloc failed while allocating request pool\
                          in A1DI_Buffer_pool_initialize\n");
  
        status = A1DI_Malloc((void **) &(A1D_Buffer_pool.mem_region_ptrs[i]),
                                     A1D_Buffer_pool.sizes[i]  * A1D_Buffer_pool.limits[i]);
        A1U_ERR_POP(status != A1_SUCCESS,
                    "A1DI_Malloc failed while allocating request pool\
                          in A1DI_Buffer_pool_initialize\n");
  
        a1d_buffer = A1D_Buffer_pool.pool_region_ptrs[i];
        A1D_Buffer_pool.pool_heads[i] = a1d_buffer;
        for (j=0; j<A1D_Buffer_pool.limits[i]-1; j++)
        {
            a1d_buffer[j].next = &a1d_buffer[j+1];
            a1d_buffer[j].buffer_ptr = (void *) ((size_t) A1D_Buffer_pool.mem_region_ptrs[i] + 
                                          j * A1D_Buffer_pool.sizes[i]);
            a1d_buffer[j].pool_index == i;
        }
        a1d_buffer[j].next = NULL;
        a1d_buffer[j].buffer_ptr = (void *) ((size_t) A1D_Buffer_pool.mem_region_ptrs[i] +
                                      (A1D_Buffer_pool.limits[i]-1) * A1D_Buffer_pool.sizes[i]);
        a1d_buffer[j].pool_index == i;
     }

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

void A1DI_Buffer_pool_finalize()
{
    int i;

    A1U_FUNC_ENTER();

    for(i=0; i<A1C_NUM_BUF_SIZES; i++)
    {
       A1DI_Free((void *) (A1D_Buffer_pool.pool_region_ptrs[i]));
       A1DI_Free((void *) (A1D_Buffer_pool.mem_region_ptrs[i]));
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}
