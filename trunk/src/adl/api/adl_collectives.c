/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1d.h"
#include "a1u.h"

int A1_Barrier_group(A1_group_t* group)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    /* barrier is meaningless with 1 process */
    if ( 1==A1_Process_size(A1_GROUP_WORLD) ) goto fn_exit;

    status = A1D_Barrier_group(group);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_Barrier_group returned an error\n");

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1_NbBarrier_group(A1_group_t* group, A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    /* barrier is meaningless with 1 process */
    if ( 1==A1_Process_size(A1_GROUP_WORLD) ) goto fn_exit;

    status = A1D_NbBarrier_group(group, a1_handle);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_NbBarrier_group returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int A1_Sync_group(A1_group_t* group)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    /* no collective bypass for 1 proc here because we will use DCMF for some operations
     * which need to be completed by flush */

    status = A1D_Sync_group(group);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_Sync_group returned an error\n");

  fn_exit: 
    A1U_FUNC_EXIT();
    return status;

  fn_fail: 
    goto fn_exit;
}

int A1_NbSync_group(A1_group_t* group, A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    /* no collective bypass for 1 proc here because we will use DCMF for some operations
     * which need to be completed by flush */

    status = A1D_NbSync_group(group, a1_handle);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_NbSync_group returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int A1_Allreduce_group(A1_group_t* group,
                       int count,
                       A1_reduce_op_t a1_op,
                       A1_datatype_t a1_type,
                       void* in,
                       void* out)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    if (count <= 0) goto fn_exit;

    /* bypass any sort of network API or communication altogether */
    if ( 1==A1_Process_size(A1_GROUP_WORLD) )
    {
        switch (a1_type)
        {
            case A1_DOUBLE:
                memcpy(in,out,count*size(double));
                break;
            case A1_INT32:
                memcpy(in,out,count*size(int32_t));
                break;
            case A1_INT64:
                memcpy(in,out,count*size(int64_t));
                break;
            case A1_UINT32:
                memcpy(in,out,count*size(uint32_t));
                break;
            case A1_UINT64:
                memcpy(in,out,count*size(uint64_t));
                break;
            case A1_FLOAT:
                memcpy(in,out,count*size(float));
                break;
            default:
                A1U_ERR_ABORT(status != A1_SUCCESS, "Unsupported A1_datatype \n");
                break;
        }
        goto fn_exit;
    }

    status = A1D_Allreduce_group(group,
                                 count,
                                 a1_op,
                                 a1_type,
                                 in,
                                 out);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_Allreduce_group returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int A1_NbAllreduce_group(A1_group_t* group,
                         int count,
                         A1_reduce_op_t a1_op,
                         A1_datatype_t a1_type,
                         void* in,
                         void* out,
                         A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    if (count <= 0) goto fn_exit;

    /* bypass any sort of network API or communication altogether */
    if ( 1==A1_Process_size(A1_GROUP_WORLD) )
    {
        switch (a1_type)
        {
            case A1_DOUBLE:
                memcpy(in,out,count*size(double));
                break;
            case A1_INT32:
                memcpy(in,out,count*size(int32_t));
                break;
            case A1_INT64:
                memcpy(in,out,count*size(int64_t));
                break;
            case A1_UINT32:
                memcpy(in,out,count*size(uint32_t));
                break;
            case A1_UINT64:
                memcpy(in,out,count*size(uint64_t));
                break;
            case A1_FLOAT:
                memcpy(in,out,count*size(float));
                break;
            default:
                A1U_ERR_ABORT(status != A1_SUCCESS, "Unsupported A1_datatype \n");
                break;
        }
        goto fn_exit;
    }

    status = A1D_NbAllreduce_group(group,
                                   count,
                                   a1_op,
                                   a1_type,
                                   in,
                                   out,
                                   a1_handle);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_NbAllreduce_group returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int A1_Bcast_group(A1_group_t* group,
                   int root,
                   int count,
                   void* buffer)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    if (count <= 0) goto fn_exit;

    /* bypass any sort of network API or communication altogether */
    if ( 1==A1_Process_size(A1_GROUP_WORLD) ) goto fn_exit;

    status = A1D_Bcast_group(group,
                             root,
                             count,
                             buffer);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_Bcast_group returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}

int A1_NbBcast_group(A1_group_t* group,
                     int root,
                     int count,
                     void* buffer,
                     A1_handle_t a1_handle)
{
    int status = A1_SUCCESS;

    A1U_FUNC_ENTER();

    /* FIXME: The profiling interface needs to go here */

    /* FIXME: Locking functionality needs to go here */

#   ifdef HAVE_ERROR_CHECKING
#   endif

    if (count <= 0) goto fn_exit;

    /* bypass any sort of network API or communication altogether */
    if ( 1==A1_Process_size(A1_GROUP_WORLD) ) goto fn_exit;

    status = A1D_NbBcast_group(group,
                               root,
                               count,
                               buffer,
                               a1_handle);
    A1U_ERR_POP(status!=A1_SUCCESS, "A1D_NbBcast_group returned an error\n");

  fn_exit:
    A1U_FUNC_EXIT();
    return status;

  fn_fail:
    goto fn_exit;
}
