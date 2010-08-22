/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

int A1DI_Direct_getv(int target,
                     A1_iov_t *iov_ar,
                     int ar_len,
                     A1D_Handle_t *a1d_handle)
{
    int i, j, status = A1_SUCCESS;
    size_t src_disp, dst_disp, size;
    DCMF_Callback_t done_callback;

    A1U_FUNC_ENTER();

    for (i=0; i<ar_len; i++)
    {
        for(j=0; j<iov_ar[i].ptr_ar_len; j++)
        {

              src_disp = (size_t) iov_ar[i].source_ptr_ar[j]
                      - (size_t) A1D_Membase_global[A1D_Process_info.my_rank];
              dst_disp = (size_t) iov_ar[i].target_ptr_ar[j]
                      - (size_t) A1D_Membase_global[target];
              size = iov_ar[i].size;

              status = A1DI_Load_request(a1d_handle);
              A1U_ERR_POP(status != A1_SUCCESS,
                      "A1DI_Load_request returned error in A1DI_Direct_getv. Rquests exhausted \n");

              done_callback.function = A1DI_Handle_done;
              done_callback.clientdata = (void *) a1d_handle;
              a1d_handle->active++;

              status = DCMF_Get(&A1D_Generic_get_protocol,
                                &(a1d_handle->request_list->request),
                                done_callback,
                                DCMF_RELAXED_CONSISTENCY,
                                target,
                                size,
                                &A1D_Memregion_global[target],
                                &A1D_Memregion_global[A1D_Process_info.my_rank],
                                src_disp,
                                dst_disp);
              A1U_ERR_POP(status != DCMF_SUCCESS, "DCMF_Get returned with an error \n");

        }
    } 

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1D_GetV(int target,
             A1_iov_t *iov_ar,
             int ar_len)
{
    int status = A1_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    a1d_handle = A1DI_Get_handle();
    A1U_ERR_POP(status = (a1d_handle == NULL),
                "A1DI_Get_handle returned NULL in A1D_GetS. Handles exhausted \n");

    status = A1DI_Direct_getv(target,
                              iov_ar,
                              ar_len,
                              a1d_handle);
    A1U_ERR_POP(status, "A1DI_Direct_getv returned with an error \n");

    A1DI_Conditional_advance(a1d_handle->active > 0);

  fn_exit:
    A1DI_Release_handle(a1d_handle);
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1D_NbGetV(int target,
               A1_iov_t *iov_ar,
               int ar_len,
               A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;
    A1D_Handle_t *a1d_handle;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    a1d_handle = (A1D_Handle_t *) a1_handle;

    status = A1DI_Direct_getv(target,
                              iov_ar,
                              ar_len,
                              a1d_handle);
    A1U_ERR_POP(status, "A1DI_Direct_getv returned with an error \n");

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}
