/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "armciu.h"
#include "armcid.h"

int ARMCID_Init(void)
{
    int status = ARMCI_SUCCESS;

    ARMCIU_FUNC_ENTER();

    /* FIXME: Need to do stuff here! */

fn_exit:
    ARMCIU_FUNC_EXIT();
    return status;

fn_fail:
    goto fn_exit;
}
