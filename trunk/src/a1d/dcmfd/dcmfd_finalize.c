/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1u.h"
#include "a1d.h"
#include "dcmfdimpl.h"

int A1D_Finalize(void)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    DCMF_CriticalSection_enter (0);

    DCMF_Messager_finalize();     

  fn_exit:
    DCMF_CriticalSection_exit (0);
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}
