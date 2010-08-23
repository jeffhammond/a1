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
#define A1D_IMPLEMENTS_GETS

#if defined A1D_IMPLEMENTS_GETS

int A1_GetS(int target,
            int stride_level,
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

    status = A1D_GetS(target,
                      stride_level,
                      block_sizes,
                      source_ptr,
                      src_stride_ar,
                      target_ptr,
                      trg_stride_ar);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_GetS returned error\n");

    fn_exit: A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

int A1_NbGetS(int target,
             int stride_level,
             int *block_sizes,
             void* source_ptr,
             int *src_stride_ar,
             void* target_ptr,
             int *trg_stride_ar,
             A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status = A1D_NbGetS(target,
                       stride_level,
                       block_sizes,
                       source_ptr,
                       src_stride_ar,
                       target_ptr,
                       trg_stride_ar,
                       a1_handle);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_NbGetS returned error\n");

    fn_exit: A1U_FUNC_EXIT();
    return status;

    fn_fail: goto fn_exit;
}

#else

int A1I_Recursive_Get(int target,
                      int stride_level,
                      int *block_sizes,
                      void *source_ptr,
                      int *src_stride_ar,
                      void *target_ptr,
                      int *trg_stride_ar,
                      A1_handle_t a1_handle)
{
    int i, status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    if (stride_level > 0)
    {

        for (i = 0; i < block_sizes[stride_level]; i++)
        {
            status = A1I_Recursive_Get(target,
                                       stride_level - 1,
                                       block_sizes,
                                       (void *) ((size_t) source_ptr + i * src_stride_ar[stride_level - 1]),
                                       src_stride_ar,
                                       (void *) ((size_t) target_ptr + i * trg_stride_ar[stride_level - 1]),
                                       trg_stride_ar,
				       a1_handle);
            A1U_ERR_POP(status != A1_SUCCESS,
                  "A1I_Recursive_Get returned error in A1I_Recursive_Get.\n");
        }

    }
    else
    {

        status = A1_NbGet(target,
                          source_ptr,
                          target_ptr,
                          src_disp,
                          block_sizes[0],
                          a1_handle);
        A1U_ERR_POP(status != A1_SUCCESS, "A1_NbGet returned with an error \n");

    }

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int A1_GetS(int target,
            int stride_level,
            int *block_sizes,
            void* source_ptr,
            int *src_stride_ar,
            void* target_ptr,
            int *trg_stride_ar)
{
    int status = A1_SUCCESS;
    A1_handle_t a1_handle;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status = A1_Allocate_handle(&a1_handle);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1_Allocate_handle returned error\n");

    status = A1I_Recursive_Get(target,
                               stride_level,
                               block_sizes,
                               source_ptr,
                               src_stride_ar,
                               target_ptr,
                               trg_stride_ar,
                               a1_handle);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_Recursive_Get returned error\n");

    status = A1_Wait_handle(a1_handle);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1_Wait_handle returned error\n");

  fn_exit:
    A1_Release_handle(a1_handle);
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int A1_NbGetS(int target,
              int stride_level,
              int *block_sizes,
              void* source_ptr,
              int *src_stride_ar,
              void* target_ptr,
              int *trg_stride_ar,
              A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    status = A1I_Recursive_Get(target,
                               stride_level,
                               block_sizes,
                               source_ptr,
                               src_stride_ar,
                               target_ptr,
                               trg_stride_ar,
                               a1_handle);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_Recursive_Get returned error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

#endif /* A1D_IMPLEMENTS_GETS */
