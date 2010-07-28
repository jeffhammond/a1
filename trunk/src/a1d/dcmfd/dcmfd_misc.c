/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "dcmfdimpl.h"

void A1D_Rank(int* rank)
{
    A1U_FUNC_ENTER();

    *rank = A1D_Process_info.my_rank;

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}

void A1D_Size(int* size)
{
    A1U_FUNC_ENTER();

    *size = A1D_Process_info.num_ranks;

  fn_exit:
    A1U_FUNC_EXIT();
    return;

  fn_fail:
    goto fn_exit;
}

unsigned long long A1D_Time()
{
    A1U_FUNC_ENTER();

  fn_exit:
    A1U_FUNC_EXIT();
    return DCMF_Timebase();

  fn_fail:
    goto fn_exit;
} 
