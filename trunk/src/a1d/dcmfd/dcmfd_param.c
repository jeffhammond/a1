/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

uint32_t a1_enable_cht = A1C_ENABLE_CHT;
uint32_t a1_enable_scalefree_flush = A1C_ENABLE_SCALEFREE_FLUSH;
uint32_t a1_alignment = A1C_ALIGNMENT;
uint32_t a1_direct_noncontig_threshold = A1C_DIRECT_NONCONTIG_THRESHOLD;
uint32_t a1_packed_noncontig_limit = A1C_PACKED_NONCONTIG_LIMIT;
uint32_t a1_request_pool_initial = A1C_REQUEST_POOL_INITIAL; 
uint32_t a1_request_pool_increment = A1C_REQUEST_POOL_INCREMENT;
uint32_t a1_request_pool_limit = A1C_REQUEST_POOL_LIMIT;   
uint32_t a1_flushall_pending_limit = A1C_FLUSHALL_PENDING_LIMIT;

void A1DI_Read_parameters() {

    char* value = NULL;

    A1U_FUNC_ENTER();

    if ((value = getenv("A1_FLUSHALL_PENDING_LIMIT")) != NULL) {
        a1_flushall_pending_limit = atoi(value);
    }

    if ((value = getenv("A1_DIRECT_NONCONTIG_THRESHOLD")) != NULL) {
        a1_direct_noncontig_threshold = atoi(value);
    }

    if ((value = getenv("A1_PACKED_NONCONTIG_LIMIT")) != NULL) {
        a1_packed_noncontig_limit = atoi(value);
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
