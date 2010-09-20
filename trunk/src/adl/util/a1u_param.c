/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1d.h"
#include "a1u.h"

/* FIXME: move this to a more appropriate place once all
 *         this A1U/A1D settings crap is sorted out */
#define A1C_NETWORK_BYPASS 1

A1U_Settings_t a1u_settings;

int A1U_Read_parameters()
{
    int result = A1_SUCCESS;
    char* value = NULL;

    A1U_FUNC_ENTER();

    a1u_settings.network_bypass = A1C_NETWORK_BYPASS;
    if ((value = getenv("A1_NETWORK_BYPASS")) != NULL)
    {
        a1u_settings.network_bypass = atoi(value);
    }

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}
