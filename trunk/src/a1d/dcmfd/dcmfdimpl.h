/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include <dcmf.h>
#include <dcmf_globalcollectives.h>
#include <dcmf_collectives.h>

DCMF_Callback_t no_callback;

#define A1DI_CRITICAL(call) do {                                  \
      if(A1D_Thread_info.thread_level > A1_THREAD_SERIALIZED) {   \
        DCMF_CriticalSection_enter(0);                            \
        call;                                                     \
        DCMF_CriticalSection_exit(0);                             \
      } else {                                                    \
        call;                                                     \
      }                                                           \
    } while (0)                                                   \

typedef struct 
{
   int thread_level;
   int num_threads;
} 
A1D_Thread_info_t; 

typedef struct
{
   uint32_t my_rank;
   uint32_t num_ranks;
}
A1D_Process_info_t;

typedef struct
{
   DCMF_Protocol_t protocol;
   uint32_t rcv_active;
   void **addressarray_ptr;
}
A1D_Control_info_t;

typedef struct
{
   DCMF_Protocol_t protocol; 
   DCMF_Callback_t callback;
   uint32_t active;
}
A1D_GlobalBarrier_info_t;

typedef struct
{
   DCMF_Protocol_t protocol;
   DCMF_Callback_t done_callback, ack_callback;
   uint32_t done_active, ack_active;
}
A1D_Put_info_t;

typedef struct
{
   DCMF_Protocol_t protocol;
   DCMF_Callback_t callback;
   uint32_t active;
}
A1D_Get_info_t;

struct A1D_Memregion
{
   DCMF_Memregion_t mregion;
   void * vaddress;
   size_t bytes;
   struct A1D_Memregion *next;
   struct A1D_Memregion *prev;
};

typedef struct A1D_Memregion A1D_Memregion_t;

extern DCMF_Configure_t A1D_Messager_info;
extern A1D_Thread_info_t A1D_Thread_info;
extern A1D_Process_info_t A1D_Process_info;
extern A1D_Control_info_t A1D_Control_info;
extern A1D_GlobalBarrier_info_t A1D_GlobalBarrier_info;
extern A1D_Put_info_t A1D_Put_info; 
extern A1D_Get_info_t A1D_Get_info;
extern A1D_Memregion_t **A1D_Memregion_lists; 
extern uint32_t *A1D_Memregion_count;

void A1DI_Generic_Callback(void *, DCMF_Error_t *);
void A1DI_Insert_memregion(A1D_Memregion_t *, uint32_t);
int A1DI_Find_memregion(A1D_Memregion_t **, unsigned *, void *, uint32_t);
