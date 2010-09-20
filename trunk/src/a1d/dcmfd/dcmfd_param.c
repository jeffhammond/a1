/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

A1D_Settings_t a1d_settings;

int A1DI_Read_parameters()
{
    int result = A1_SUCCESS;
    char* value = NULL;

    A1U_FUNC_ENTER();

    a1d_settings.alignment = A1C_ALIGNMENT;

    a1d_settings.enable_cht = A1C_ENABLE_CHT;
    if ((value = getenv("A1D_ENABLE_CHT")) != NULL)
    {
        a1d_settings.enable_cht = atoi(value);
    }

    a1d_settings.mpi_active = A1C_MPI_ACTIVE;
    if ((value = getenv("A1D_MPI_ACTIVE")) != NULL)
    {
        a1d_settings.mpi_active = atoi(value);
    }

    a1d_settings.cht_pause_cycles = A1C_CHT_PAUSE_CYCLES;
    if ((value = getenv("A1D_CHT_PAUSE_CYCLES")) != NULL)
    {
        a1d_settings.cht_pause_cycles = atoi(value);
    }

    a1d_settings.enable_interrupts = A1C_ENABLE_INTERRUPTS;
    if ((value = getenv("A1D_ENABLE_INTERRUPTS")) != NULL)
    {
        a1d_settings.enable_interrupts = atoi(value);
    }

    a1d_settings.flushall_pending_limit = A1C_FLUSHALL_PENDING_LIMIT;
    if ((value = getenv("A1D_FLUSHALL_PENDING_LIMIT")) != NULL)
    {
        a1d_settings.flushall_pending_limit = atoi(value);
    }

    a1d_settings.put_packing_limit = A1C_PUT_PACKING_LIMIT;
    if ((value = getenv("A1D_PUT_PACKING_LIMIT")) != NULL)
    {
        a1d_settings.put_packing_limit = atoi(value);
        if (a1d_settings.enable_cht == 0 && a1d_settings.enable_interrupts == 0)
        {
            a1d_settings.put_packing_limit = 0;
        }
    }

    a1d_settings.put_packetsize = A1C_PUT_PACKETSIZE;
    if ((value = getenv("A1D_PUT_PACKETSIZE")) != NULL)
    {
        a1d_settings.put_packetsize = atoi(value);

        /* Having packet size less than the packing limit will not make sense and
         can lead to deadlocks in the packing code*/
        if (a1d_settings.put_packetsize < a1d_settings.put_packing_limit)
        {
            a1d_settings.put_packetsize = a1d_settings.put_packing_limit;
        }
    }

    a1d_settings.get_packing_limit = A1C_GET_PACKING_LIMIT;
    if ((value = getenv("A1D_GET_PACKING_LIMIT")) != NULL)
    {
        a1d_settings.get_packing_limit = atoi(value);
        if (a1d_settings.enable_cht == 0 && a1d_settings.enable_interrupts == 0)
        {
            a1d_settings.get_packing_limit = 0;
        }
    }

    a1d_settings.get_packetsize = A1C_GET_PACKETSIZE;
    if ((value = getenv("A1D_GET_PACKETSIZE")) != NULL)
    {
        a1d_settings.get_packetsize = atoi(value);

        /* Having packet size less than the packing limit will not make sense and 
         can lead to deadlocks in the packing code*/
        if (a1d_settings.get_packetsize < a1d_settings.get_packing_limit)
        {
            a1d_settings.get_packetsize = a1d_settings.get_packing_limit;
        }
    }

    a1d_settings.putacc_packing_limit = A1C_PUTACC_PACKING_LIMIT;
    if ((value = getenv("A1D_PUTACC_PACKING_LIMIT")) != NULL)
    {
        a1d_settings.putacc_packing_limit = atoi(value);
        if (a1d_settings.enable_cht == 0 && a1d_settings.enable_interrupts == 0)
        {
            a1d_settings.putacc_packing_limit = 0;
        }
    }

    a1d_settings.putacc_packetsize = A1C_PUTACC_PACKETSIZE;
    if ((value = getenv("A1D_PUTACC_PACKETSIZE")) != NULL)
    {
        a1d_settings.putacc_packetsize = atoi(value);

        /* Having packet size less than the packing limit will not make sense and
         can lead to deadlocks in the packing code*/
        if (a1d_settings.putacc_packetsize < a1d_settings.putacc_packing_limit)
        {
            a1d_settings.putacc_packetsize = a1d_settings.putacc_packing_limit;
        }
    }

    a1d_settings.handlepool_size = A1C_HANDLE_POOL_SIZE;
    if ((value = getenv("A1D_HANDLE_POOL_SIZE")) != NULL)
    {
        a1d_settings.handlepool_size = atoi(value);
    }

    a1d_settings.requestpool_size = A1C_REQUEST_POOL_SIZE;
    if ((value = getenv("A1D_REQUEST_POOL_SIZE")) != NULL)
    {
        a1d_settings.requestpool_size = atoi(value);
    }

    a1d_settings.use_handoff = A1C_USE_HANDOFF;
    if ((value = getenv("A1D_USE_HANDOFF")) != NULL)
    {
        a1d_settings.use_handoff = atoi(value);
    }
    if (!a1d_settings.enable_cht)
    {
        a1d_settings.use_handoff = 0;
    }

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

    if (a1d_settings.enable_cht)
    {
        A1U_output_printf("passive-target progress enabled via CHT\n");
    }
    else if (a1d_settings.enable_interrupts)
    {
        A1U_output_printf("passive-target progress enabled via DCMF interrupts\n");
    }
    else
    {
        A1U_output_printf("passive-target progress disabled (THIS IS BAD)\n");
    }

    A1U_output_printf("cht_pause_cycles = %u\n", a1d_settings.cht_pause_cycles);
    A1U_output_printf("put_packing_limit = %u\n", a1d_settings.put_packing_limit);
    A1U_output_printf("get_packing_limit = %u\n", a1d_settings.get_packing_limit);
    A1U_output_printf("putacc_packing_limit = %u\n", a1d_settings.putacc_packing_limit);
    A1U_output_printf("flushall_pending_limit = %u\n", a1d_settings.flushall_pending_limit);

    /* TODO: finish output for the rest of the device-dependent settings */

  fn_exit: 
    A1U_FUNC_EXIT();
    return result;

  fn_fail: 
    goto fn_exit;
}
