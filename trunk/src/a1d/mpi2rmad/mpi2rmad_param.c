/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "mpi2rmadimpl.h"

void A1DI_Read_parameters() {

    char* value = NULL;

    A1U_FUNC_ENTER();

//    if ((value = getenv("A1_ALIGNMENT")) != NULL) {
//        a1_alignment = atoi(value);
//    }

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}
