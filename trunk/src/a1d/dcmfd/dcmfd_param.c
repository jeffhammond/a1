/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

uint32_t a1_enable_cht = A1C_ENABLE_CHT;
uint32_t a1_enable_scalefree_flush = A1C_ENABLE_SCALEFREE_FLUSH;
uint32_t a1_alignment = A1C_ALIGNMENT;
uint32_t a1_packing_limit = A1C_PACKING_LIMIT;
uint32_t a1_request_pool_initial = A1C_REQUEST_POOL_INITIAL; 
uint32_t a1_request_pool_increment = A1C_REQUEST_POOL_INCREMENT;
uint32_t a1_request_pool_limit = A1C_REQUEST_POOL_LIMIT;   

void A1DI_Read_parameters() {

    char* value = NULL;

    A1U_FUNC_ENTER();

    if ((value = getenv("A1_ENABLE_SCALEFREE_FLUSH")) != NULL) {
        a1_enable_scalefree_flush = atoi(value);
    }

    if ((value = getenv("A1_ALIGNMENT")) != NULL) {
        a1_alignment = atoi(value);
    }

    if ((value = getenv("A1_ENABLE_SCALEFREE_FLUSH")) != NULL) {
        a1_enable_scalefree_flush = atoi(value);
    }

    if ((value = getenv("A1_PACKING_LIMIT")) != NULL) {
        a1_packing_limit = atoi(value);
    }

    if ((value = getenv("A1_ENABLE_CHT")) != NULL) {
        a1_enable_cht = atoi(value);
    }

    if ((value = getenv("A1_REQUEST_POOL_INITIAL")) != NULL) {
        a1_request_pool_initial = atoi(value);
    }

    if ((value = getenv("A1_REQUEST_POOL_INCREMENT")) != NULL) {
        a1_request_pool_increment = atoi(value);
    }

    if ((value = getenv("A1_REQUEST_POOL_LIMIT")) != NULL) {
        a1_request_pool_limit = atoi(value);
    }

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}
