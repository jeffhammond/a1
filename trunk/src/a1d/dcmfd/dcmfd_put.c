/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1u.h"
#include "a1d.h"
#include "dcmfdimpl.h"

int A1D_Put(void* src, void* dst, int bytes, int proc)
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Request_t request;
    DCMF_Memregion_t *src_memregion, *dst_memregion;
    unsigned src_disp, dst_disp;
 
    A1U_FUNC_ENTER();

    result = A1DI_Find_memregion(&src_memregion, &src_disp, src, A1D_Process_info.my_rank);     
    A1U_ERR_POP(result,"could not find memoryregion associated with the address \n"); 
    result = A1DI_Find_memregion(&dst_memregion, &dst_disp, dst, A1D_Process_info.my_rank);
    A1U_ERR_POP(result,"could not find memoryregion associated with the address \n");    

    A1D_Put_info.done_active += 1;
    A1D_Put_info.ack_active += 1;
    result = DCMF_Put(&A1D_Put_info.protocol,
                      &request,
                      A1D_Put_info.done_callback,
                      DCMF_SEQUENTIAL_CONSISTENCY,
                      proc,  
                      bytes,
                      src_memregion,
                      dst_memregion,
                      src_disp,
                      dst_disp,
                      no_callback);
    A1U_ERR_POP(result,"Put returned with an error \n");
    while (A1D_Put_info.done_active) A1DI_CRITICAL(DCMF_Messager_advance()); 

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}
