/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

void A1D_Barrier_group(A1_group_t* group) {

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

    if(group == A1_GROUP_WORLD) {
      A1DI_GlobalBarrier();                            
      goto fn_exit;
    }

  fn_exit:
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;

}
