/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

A1_Settings_t a1_settings;
A1_Requestpool_info_t a1_requestpool_info;

int A1DI_Read_parameters()
{
    int result = A1_SUCCESS;
    char* value = NULL;

    A1U_FUNC_ENTER();

    a1_settings.alignment = A1C_ALIGNMENT;

    a1_settings.enable_cht = A1C_ENABLE_CHT;
    if ((value = getenv("A1_ENABLE_CHT")) != NULL)
    {
        a1_settings.enable_cht = atoi(value);
    }

    a1_settings.cht_pause_cycles = A1C_CHT_PAUSE_CYCLES;
    if ((value = getenv("A1_CHT_PAUSE_CYCLES")) != NULL)
    {
        a1_settings.cht_pause_cycles = atoi(value);
    }

    a1_settings.enable_interrupts = A1C_ENABLE_INTERRUPTS;
    if ((value = getenv("A1_ENABLE_INTERRUPTS")) != NULL)
    {
        a1_settings.enable_interrupts = atoi(value);
    }

    a1_settings.flushall_pending_limit = A1C_FLUSHALL_PENDING_LIMIT;
    if ((value = getenv("A1_FLUSHALL_PENDING_LIMIT")) != NULL)
    {
        a1_settings.flushall_pending_limit = atoi(value);
    }

    a1_settings.direct_noncontig_put_threshold
            = A1C_DIRECT_NONCONTIG_PUT_THRESHOLD;
    if ((value = getenv("A1_DIRECT_NONCONTIG_PUT_THRESHOLD")) != NULL)
    {
        a1_settings.direct_noncontig_put_threshold = atoi(value);
        if (a1_settings.enable_cht == 0 && a1_settings.enable_interrupts == 0)
        {
            a1_settings.direct_noncontig_put_threshold = 0;
        }
    }

    a1_settings.direct_noncontig_get_threshold
            = A1C_DIRECT_NONCONTIG_GET_THRESHOLD;
    if ((value = getenv("A1_DIRECT_NONCONTIG_GET_THRESHOLD")) != NULL)
    {
        a1_settings.direct_noncontig_get_threshold = atoi(value);
        if (a1_settings.enable_cht == 0 && a1_settings.enable_interrupts == 0)
        {
            a1_settings.direct_noncontig_get_threshold = 0;
        }
    }

    a1_settings.direct_noncontig_putacc_threshold
            = A1C_DIRECT_NONCONTIG_PUTACC_THRESHOLD;
    if ((value = getenv("A1_DIRECT_NONCONTIG_PUTACC_THRESHOLD")) != NULL)
    {
        a1_settings.direct_noncontig_putacc_threshold = atoi(value);
        if (a1_settings.enable_cht == 0 && a1_settings.enable_interrupts == 0)
        {
            a1_settings.direct_noncontig_putacc_threshold = 0;
        }
    }

    a1_settings.direct_noncontig_getacc_threshold
            = A1C_DIRECT_NONCONTIG_GETACC_THRESHOLD;
    if ((value = getenv("A1_DIRECT_NONCONTIG_GETACC_THRESHOLD")) != NULL)
    {
        a1_settings.direct_noncontig_getacc_threshold = atoi(value);
        if (a1_settings.enable_cht == 0 && a1_settings.enable_interrupts == 0)
        {
            a1_settings.direct_noncontig_getacc_threshold = 0;
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

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}

int A1DI_Print_parameters(void)
{
    int result = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1U_output_printf("=============== A1 Parameters ================\n");
    A1U_output_printf("A1 is using the DCMF (Blue Gene/P) device\n");

    if (a1_settings.enable_cht)
    {
        A1U_output_printf("passive-target progress enabled via CHT\n");
    }
    else if (a1_settings.enable_interrupts)
    {
        A1U_output_printf("passive-target progress enabled via DCMF interrupts\n");
    }
    else
    {
        A1U_output_printf("passive-target progress disabled (THIS IS BAD)\n");
    }

    A1U_output_printf("cht_pause_cycles = %u\n",
                      a1_settings.cht_pause_cycles);
    A1U_output_printf("direct_noncontig_put_threshold = %u\n",
                      a1_settings.direct_noncontig_put_threshold);
    A1U_output_printf("direct_noncontig_get_threshold = %u\n",
                      a1_settings.direct_noncontig_get_threshold);
    A1U_output_printf("direct_noncontig_putacc_threshold = %u\n",
                      a1_settings.direct_noncontig_putacc_threshold);
    A1U_output_printf("direct_noncontig_getacc_threshold = %u\n",
                      a1_settings.direct_noncontig_getacc_threshold);
    A1U_output_printf("flushall_pending_limit = %u\n",
                      a1_settings.flushall_pending_limit);

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}
