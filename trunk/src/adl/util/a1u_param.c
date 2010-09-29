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
/* units of bytes.  Very hardware specific but need to reorganize
 * code before moving into device layer. */
#define A1C_NETWORK_BYPASS_UPPER_LIMIT_1D 32768
#define A1C_NETWORK_BYPASS_UPPER_LIMIT_ND 200000

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

    /* The threshold BELOW which we do NIC-bypass.  We do this because
     * some architectures (BG/P) have a DMA that beats CPU-based
     * intranode transfers for large buffers.
     */
    a1u_settings.network_bypass = A1C_NETWORK_BYPASS_UPPER_LIMIT_1D;
    if ((value = getenv("A1_NETWORK_BYPASS_UPPER_LIMIT_1D")) != NULL)
    {
        a1u_settings.network_bypass_upper_limit_1d = atoi(value);
    }
    /* For strided, the threshold is much higher. */
    a1u_settings.network_bypass = A1C_NETWORK_BYPASS_UPPER_LIMIT_ND;
    if ((value = getenv("A1_NETWORK_BYPASS_UPPER_LIMIT_ND")) != NULL)
    {
        a1u_settings.network_bypass_upper_limit_Nd = atoi(value);
    }
    /* If bypass is off, just set upper limit to zero so we always
     * use the NIC.  We do not query network_bypass in contiguous ops. */
    if (a1u_settings.network_bypass == 0)
    {
        a1u_settings.network_bypass_upper_limit_1d = 0;
        a1u_settings.network_bypass_upper_limit_Nd = 0;
    }

    fn_exit: A1U_FUNC_EXIT();
    return result;

    fn_fail: goto fn_exit;
}
