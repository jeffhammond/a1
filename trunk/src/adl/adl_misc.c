/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1d.h"
#include "a1u.h"

int A1_Rank() 
{
    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

  fn_exit:
    A1U_FUNC_EXIT();
    return A1D_Rank();

  fn_fail:
    goto fn_exit;
}

int A1_Size()
{
    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

  fn_exit:
    A1U_FUNC_EXIT();
    return A1D_Size();

  fn_fail:
    goto fn_exit;
}


double A1_Time_seconds()
{
    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

  fn_exit:
    A1U_FUNC_EXIT();  
    return A1D_Time_seconds();

  fn_fail:
    goto fn_exit;
}


unsigned long long A1_Time_cycles()
{
    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

  fn_exit:
    A1U_FUNC_EXIT();
    return A1D_Time_cycles();

  fn_fail:
    goto fn_exit;
}
