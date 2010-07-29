/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

uint32_t enable_scalefree_flush = A1C_ENABLE_SCALEFREE_FLUSH;

void A1DI_Read_parameters() {

    char* value = NULL;

    A1U_FUNC_ENTER();

    if ((value = getenv("A1_ENABLE_SCALEFREE_FLUSH")) != NULL) {
        enable_scalefree_flush = atoi(value);
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}
