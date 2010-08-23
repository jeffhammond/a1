/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1d.h"
#include "a1u.h"

/* This is here because the build system does not yet have the necessary
 * logic to set these options for each device. */
#define A1D_IMPLEMENTS_PUTACCV

#if defined A1D_IMPLEMENTS_PUTACCV

int A1_PutAccV(int target, 
               A1_iov_t *iov_ar,
               int ar_len,
               A1_datatype_t a1_type,
               void* scaling)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status = A1D_PutAccV(target, 
                         iov_ar,
                         ar_len,
                         a1_type,
                         scaling); 
    A1U_ERR_POP(status, "A1D_PutAccV returned error\n");

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1_NbPutAccV(int target,
               A1_iov_t *iov_ar,
               int ar_len,
               A1_datatype_t a1_type,
               void* scaling,
               A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status = A1D_NbPutAccV(target,    
                           iov_ar,
                           ar_len,
                           a1_type,
                           scaling,
                           a1_handle);      
    A1U_ERR_POP(status, "A1D_NbPutAccV returned error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

#else

int A1_PutAccV(int target,
               A1_iov_t *iov_ar,
               int ar_len,
               A1_datatype_t a1_type,
               void* scaling);
{
    int i, j, status = A1_SUCCESS;
    A1_handle_t a1_handle;

    A1U_FUNC_ENTER();

    status = A1_Allocate_handle(&a1_handle);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1_Allocate_handle returned error\n");

    for (i=0; i<ar_len; i++)
    {
        for(j=0; j<iov_ar[i].ptr_ar_len; j++)
        {

             status = A1D_NbPutAcc(target,
                                   iov_ar[i].source_ptr_ar[j],
                                   iov_ar[i].target_ptr_ar[j],
                                   iov_ar[i].size,
                                   a1_type,
                                   scaling,
 	   			  a1_handle);
             A1U_ERR_POP(status != A1_SUCCESS, "A1D_NbPutAcc returned with an error \n");

        }
    }

    status = A1_Wait_handle(a1_handle);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1_Wait_handle returned error\n");

  fn_exit:
    A1_Release_handle(a1_handle);
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int A1_NbPutAccV(int target,
                 A1_iov_t *iov_ar,
                 int ar_len,
                 A1_datatype_t a1_type,
                 void* scaling,
                 A1_handle_t a1_handle);
{
    int i, j, status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    for (i=0; i<ar_len; i++)
    {
        for(j=0; j<iov_ar[i].ptr_ar_len; j++)
        {

             status = A1D_NbPutAcc(target,
                                   iov_ar[i].source_ptr_ar[j],
                                   iov_ar[i].target_ptr_ar[j],
                                   iov_ar[i].size,
                                   a1_type,
                                   scaling,
 	   			  a1_handle);
             A1U_ERR_POP(status != A1_SUCCESS, "A1D_NbPutAcc returned with an error \n");

        }
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

#endif /* A1D_IMPLEMENTS_PUTACCV */
