/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "armci.h"
#include "armcid.h"
#include "armciu.h"

int ARMCI_Init(void)
{
    int status;

    ARMCIU_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

    status = ARMCID_Init();
    ARMCIU_ERR_POP(status, "device init returned error\n");

fn_exit:
    ARMCIU_FUNC_EXIT();
    return status;

fn_fail:
    goto fn_exit;
}
