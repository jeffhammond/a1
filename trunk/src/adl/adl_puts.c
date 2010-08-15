/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1d.h"
#include "a1u.h"

/* This is here because the build system does not yet have the necessary
 * logic to set these options for each device. */
#define A1D_IMPLEMENTS_PUTS

#if defined A1D_IMPLEMENTS_PUTS

int A1_PutS(int target,
            int stride_levels,
            int *block_sizes,
            void* source_ptr,
            int *src_stride_ar,
            void* target_ptr,
            int *trg_stride_ar)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status = A1D_PutS(target,
                      stride_levels,
                      block_sizes,
                      source_ptr,
                      src_stride_ar,
                      target_ptr,
                      trg_stride_ar);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_PutS returned error\n");

    fn_exit: A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

#else

/* TODO: implement A1_PutS in terms of A1_Put for lazy port option */

#endif /* A1D_IMPLEMENTS_PUTS */
