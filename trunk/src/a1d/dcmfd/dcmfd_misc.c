/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

int A1D_Rank()
{
    A1U_FUNC_ENTER();

  fn_exit:
    A1U_FUNC_EXIT();
    return A1D_Process_info.my_rank;

  fn_fail:
    goto fn_exit;
}

int A1D_Size()
{
    A1U_FUNC_ENTER();

  fn_exit:
    A1U_FUNC_EXIT();
    return A1D_Process_info.num_ranks;

  fn_fail:
    goto fn_exit;
}

double A1D_Time_seconds()
{
    double time;

    A1U_FUNC_ENTER();

    time = (double) DCMF_Timebase() / ((double) (A1D_Process_info.hw.clockMHz * 1000000));

  fn_exit:
    A1U_FUNC_EXIT();
    return time;

  fn_fail:
    goto fn_exit;
} 


unsigned long long A1D_Time_cycles()
{
    A1U_FUNC_ENTER();

  fn_exit:
    A1U_FUNC_EXIT();
    return DCMF_Timebase(); 

  fn_fail:
    goto fn_exit;
}
