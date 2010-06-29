/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1u.h"
#include "a1d.h"
#include "dcmfdimpl.h"

int A1D_Free(void *ptr)
{
    DCMF_Result result = DCMF_SUCCESS;
    int disp;
    DCMF_Memregion_t *memregion;

    A1U_FUNC_ENTER();

    result = A1DI_Find_memregion(&memregion, &disp, ptr, A1D_Process_info.my_rank);
    A1U_ERR_POP(result,"could not find memoryregion associated with the address \n");

    /*FIXME: We need to free the corresponding memory region structures of all the 
      processes */ 

    result = DCMF_Memregion_destroy(memregion);
    A1U_ERR_POP(result,"memory region destroy returned with error \n");

    free(ptr);

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}
