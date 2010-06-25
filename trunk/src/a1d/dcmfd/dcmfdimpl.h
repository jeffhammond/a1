/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include <dcmf.h>
#include <dcmf_globalcollectives.h>
#include <dcmf_collectives.h>

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
   DCMF_Callback_t callback;
   uint32_t active;
   uint32_t rcv_active;
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
   DCMF_Callback_t callback;
   uint32_t active;
}
A1D_Put_info_t;

typedef struct
{
   DCMF_Protocol_t protocol;
   DCMF_Callback_t callback;
   uint32_t active;
}
A1D_Get_info_t;

extern DCMF_Configure_t A1D_Messager_info;
extern A1D_Thread_info_t A1D_Thread_info;
extern A1D_Process_info_t A1D_Process_info;
extern A1D_Control_info_t A1D_Control_info;
extern A1D_GlobalBarrier_info_t A1D_GlobalBarrier_info;
extern A1D_Put_info_t A1D_Put_info; 
extern A1D_Get_info_t A1D_Get_info; 

void A1DI_Generic_Callback(void *, DCMF_Error_t *error);
