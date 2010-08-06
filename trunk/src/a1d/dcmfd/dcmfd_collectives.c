/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

void A1DI_Barrier_global() {

    int result = DCMF_SUCCESS;
    DCMF_Request_t request;

    A1U_FUNC_ENTER();

    A1D_GlobalBarrier_info.active += 1;

    result = DCMF_GlobalBarrier(&A1D_GlobalBarrier_info.protocol, 
                  &request,
                  A1D_GlobalBarrier_info.callback);
    A1U_ERR_ABORT(result, "DCMF_GlobalBarrier returned with an error");

    while(A1D_GlobalBarrier_info.active > 0) A1DI_Advance(); 
    
  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;

}

void A1D_Barrier_group(A1_group_t* group) {

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if(group == A1_GROUP_WORLD) {
      A1DI_Barrier_global();
      goto fn_exit;
    }

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;

}
