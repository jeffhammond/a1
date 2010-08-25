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


    a1_settings.put_packing_chunkcount_threshold
            = A1C_PUT_PACKING_CHUNKCOUNT_THRESHOLD;
    if ((value = getenv("A1_PUT_PACKING_CHUNKCOUNT_THRESHOLD")) != NULL)
    {
        a1_settings.put_packing_chunkcount_threshold = atoi(value);
        if (a1_settings.enable_cht == 0 && a1_settings.enable_interrupts == 0)
        {
            a1_settings.put_packing_chunkcount_threshold = 99999;
        }
    }

    a1_settings.put_packing_chunksize_limit
            = A1C_PUT_PACKING_CHUNKSIZE_LIMIT;
    if ((value = getenv("A1_PUT_PACKING_CHUNKSIZE_LIMIT")) != NULL)
    {
        a1_settings.put_packing_chunksize_limit = atoi(value);
        if (a1_settings.enable_cht == 0 && a1_settings.enable_interrupts == 0)
        {
            a1_settings.put_packing_chunksize_limit = 0;
        }
    }

    a1_settings.put_packetsize_limit
            = A1C_PUT_PACKETSIZE_LIMIT;
    if ((value = getenv("A1_PUT_PACKETSIZE_LIMIT")) != NULL)
    {
        a1_settings.put_packetsize_limit = atoi(value);
    }

    a1_settings.get_packing_chunkcount_threshold
            = A1C_GET_PACKING_CHUNKCOUNT_THRESHOLD;
    if ((value = getenv("A1_GET_PACKING_CHUNKCOUNT_THRESHOLD")) != NULL)
    {
        a1_settings.get_packing_chunkcount_threshold = atoi(value);
        if (a1_settings.enable_cht == 0 && a1_settings.enable_interrupts == 0)
        {
            a1_settings.get_packing_chunkcount_threshold = 99999;
        }
    }

    a1_settings.get_packing_chunksize_limit
            = A1C_GET_PACKING_CHUNKSIZE_LIMIT;
    if ((value = getenv("A1_GET_PACKING_CHUNKSIZE_LIMIT")) != NULL)
    {
        a1_settings.get_packing_chunksize_limit = atoi(value);
        if (a1_settings.enable_cht == 0 && a1_settings.enable_interrupts == 0)
        {
            a1_settings.get_packing_chunksize_limit = 0;
        }
    }

    a1_settings.get_packetsize_limit
            = A1C_GET_PACKETSIZE_LIMIT;
    if ((value = getenv("A1_GET_PACKETSIZE_LIMIT")) != NULL)
    {
        a1_settings.get_packetsize_limit = atoi(value);
    }

    a1_settings.putacc_packing_chunkcount_threshold
            = A1C_PUTACC_PACKING_CHUNKCOUNT_THRESHOLD;
    if ((value = getenv("A1_PUTACC_PACKING_CHUNKCOUNT_THRESHOLD")) != NULL)
    {
        a1_settings.putacc_packing_chunkcount_threshold = atoi(value);
        if (a1_settings.enable_cht == 0 && a1_settings.enable_interrupts == 0)
        {
            a1_settings.putacc_packing_chunkcount_threshold = 99999;
        }
    }

    a1_settings.putacc_packing_chunksize_limit
            = A1C_PUTACC_PACKING_CHUNKSIZE_LIMIT;
    if ((value = getenv("A1_PUTACC_PACKING_CHUNKSIZE_LIMIT")) != NULL)
    {
        a1_settings.putacc_packing_chunksize_limit = atoi(value);
        if (a1_settings.enable_cht == 0 && a1_settings.enable_interrupts == 0)
        {
            a1_settings.putacc_packing_chunksize_limit = 0;
        }
    }

    a1_settings.putacc_packetsize_limit
            = A1C_PUTACC_PACKETSIZE_LIMIT;
    if ((value = getenv("A1_PUTACC_PACKETSIZE_LIMIT")) != NULL)
    {
        a1_settings.putacc_packetsize_limit = atoi(value);
    }

    a1_settings.handlepool_size
            = A1C_HANDLE_POOL_SIZE;
    if ((value = getenv("A1_HANDLE_POOL_SIZE")) != NULL)
    {
        a1_settings.handlepool_size = atoi(value);
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

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}

int A1D_Print_parameters(void)
{
    int result = A1_SUCCESS;

    A1U_FUNC_ENTER();

    A1DI_CRITICAL_ENTER();

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
    A1U_output_printf("put_packing_chunksize_limit = %u\n",
                      a1_settings.put_packing_chunksize_limit);
    A1U_output_printf("get_packing_chunksize_limit = %u\n",
                      a1_settings.put_packing_chunksize_limit);
    A1U_output_printf("putacc_packing_chunksize_limit = %u\n",
                      a1_settings.put_packing_chunksize_limit);
    A1U_output_printf("flushall_pending_limit = %u\n",
                      a1_settings.flushall_pending_limit);

  fn_exit: 
    A1DI_CRITICAL_EXIT();
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}
