/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"


volatile A1_Settings_t a1_settings;
volatile A1_Requestpool_info_t a1_requestpool_info;

void A1DI_Read_parameters()
{

    char* value = NULL;

    A1U_FUNC_ENTER();
   
    a1_settings.alignment = A1C_ALIGNMENT;

    a1_settings.disable_cht = A1C_DISABLE_CHT;
    if ((value = getenv("A1_DISABLE_CHT")) != NULL)
    {
        a1_settings.disable_cht = atoi(value);
    }

    a1_settings.cht_pause_cycles = A1C_CHT_PAUSE_CYCLES;
    if ((value = getenv("A1_CHT_PAUSE_CYCLES")) != NULL)
    {
        a1_settings.cht_pause_cycles = atoi(value);
    }

    a1_settings.disable_interrupts = A1C_DISABLE_INTERRUPTS; 
    if ((value = getenv("A1_DISABLE_INTERRUPTS")) != NULL)
    {
        a1_settings.disable_interrupts = atoi(value);
    }

    a1_settings.flushall_pending_limit = A1C_FLUSHALL_PENDING_LIMIT; 
    if ((value = getenv("A1_FLUSHALL_PENDING_LIMIT")) != NULL)
    {
        a1_settings.flushall_pending_limit = atoi(value);
    }

    a1_settings.direct_noncontig_threshold = A1C_DIRECT_NONCONTIG_THRESHOLD;
    if ((value = getenv("A1_DIRECT_NONCONTIG_THRESHOLD")) != NULL)
    {
        a1_settings.direct_noncontig_threshold = atoi(value); 
        if(a1_settings.disable_cht == 1 && a1_settings.disable_interrupts == 1) {
            a1_settings.direct_noncontig_threshold = 0;
        }
    }
  
    a1_requestpool_info.initial_size = A1C_REQUEST_POOL_INITIAL;
    if ((value = getenv("A1_REQUEST_POOL_INITIAL")) != NULL)
    {
        a1_requestpool_info.initial_size = atoi(value);
    }

    a1_requestpool_info.increment_size = A1C_REQUEST_POOL_INCREMENT;
    if ((value = getenv("A1_REQUEST_POOL_INCREMENT")) != NULL)
    {
        a1_requestpool_info.increment_size = atoi(value);
    }

    a1_requestpool_info.limit_size = A1C_REQUEST_POOL_LIMIT;
    if ((value = getenv("A1_REQUEST_POOL_LIMIT")) != NULL)
    {
        a1_requestpool_info.limit_size = atoi(value);
    }

    a1_requestpool_info.total_size = 0;

  fn_exit: 
    A1U_FUNC_EXIT();
    return;

  fn_fail: 
    goto fn_exit;
}
