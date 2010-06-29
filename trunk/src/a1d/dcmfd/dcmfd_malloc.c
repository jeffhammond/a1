/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1u.h"
#include "a1d.h"
#include "dcmfdimpl.h"

void A1DI_Insert_memregion(A1D_Memregion_t *newregion, uint32_t rank) {

    A1U_FUNC_ENTER();

    A1D_Memregion_count[rank]++;

    if(!A1D_Memregion_lists[rank]) { 
         A1D_Memregion_lists[rank] = newregion;
         newregion->next = NULL;
    } else {
        A1D_Memregion_t *head;
        head = A1D_Memregion_lists[rank];
        while (newregion->vaddress > head->vaddress && head->next != NULL) {
           head = head->next;
        } 

        if((unsigned) newregion->vaddress > (unsigned) head->vaddress) {
           head->next = newregion; 
           newregion->prev = head;
           newregion->next = NULL;
        } else {
           newregion->prev = head->prev;
           head->prev->next = newregion;
           newregion->next = head;
           head->prev = newregion;
        }
    } 

  fn_exit:
    A1U_FUNC_EXIT();

  fn_fail:
    goto fn_exit;
}

int A1DI_Find_memregion(A1D_Memregion_t **memregion, unsigned *disp, void *vaddress, uint32_t rank) {

    int result = A1_SUCCESS;
    size_t bytes; 
    void *base;
    A1D_Memregion_t *head; 

    A1U_FUNC_ENTER();

    /*FIXME: Maintaining memregion list as an array will allow 
      binary searches making this faster*/
    head = A1D_Memregion_lists[rank];
    while(head->next != NULL) {
       if((unsigned) vaddress < (unsigned) head->vaddress) {
           head = head->prev;
           break;
       }   
       head = head->next;
    }

    *memregion = &(head->mregion);
    *disp = (unsigned) vaddress - (unsigned) head->vaddress;
    A1U_ERR_POP(result = (disp > head->bytes),"address out of range\n");     

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;    
}

DCMF_Result A1DI_Memregion_create(DCMF_Memregion_t **memregion, void **ptr, int bytes) {

    DCMF_Result result = DCMF_SUCCESS;
    int rank, i, out;
    A1D_Memregion_t *newregion;

    A1U_FUNC_ENTER();

    newregion = (A1D_Memregion_t *) malloc (sizeof(A1D_Memregion_t));
    A1U_ERR_POP(result = !memregion,"memory allocation for memregion failed \n");

    newregion->vaddress = malloc (bytes);
    A1U_ERR_POPANDSTMT(result = !(newregion->vaddress), goto memalloc_fail, "allocation of memory\
                   for region failed \n");
    newregion->bytes = bytes;

    result = DCMF_Memregion_create (&(newregion->mregion), &out,
                 bytes, newregion->vaddress, 0);
    A1U_ERR_POPANDSTMT(result = (result || (out < bytes)), goto memcreate_fail,\
                   "memregion create returned with error %d, bytes requested\
                    %d, bytes allocated %d\n", result, bytes, out);

    A1DI_Insert_memregion(newregion, A1D_Process_info.my_rank);
    *memregion = &(newregion->mregion);
    *ptr = newregion->vaddress;

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  memcreate_fail:
    free(newregion->vaddress);
  memalloc_fail:
    free(newregion);
  fn_fail:
    goto fn_exit;

}

DCMF_Result A1DI_Memregion_xchange(void **ptr, DCMF_Memregion_t *memregion) {

    DCMF_Result result = DCMF_SUCCESS;
    int rank, bytes;

    A1U_FUNC_ENTER();

    A1D_Control_info.addressarray_ptr = ptr;
    A1D_Control_info.rcv_active = A1D_Process_info.num_ranks-1;
    for(rank=0; rank<A1D_Process_info.num_ranks; rank++) {
        if(rank != A1D_Process_info.my_rank) {
            DCMF_Control(&A1D_Control_info.protocol, DCMF_SEQUENTIAL_CONSISTENCY,
                     rank, (DCMF_Control_t *) memregion);
        }
    }
    while(A1D_Control_info.rcv_active) A1DI_CRITICAL(DCMF_Messager_advance());

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;

}

int A1D_Malloc(void **ptr, int bytes)
{
    DCMF_Result result = DCMF_SUCCESS;
    DCMF_Memregion_t *memregion;

    A1U_FUNC_ENTER();

    result = A1DI_Memregion_create(&memregion, &ptr[A1D_Process_info.my_rank], bytes);
    A1U_ERR_POP(result,"memregion init returned with error \n");    

    result = A1DI_Memregion_xchange(ptr, memregion);
    A1U_ERR_POP(result,"memregion exchange returned with error \n");       

  fn_exit:
    A1U_FUNC_EXIT();
    return result;

  fn_fail:
    goto fn_exit;
}
