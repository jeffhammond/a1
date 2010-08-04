/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "a1.h"
#include "a1u.h"
#include "a1d.h"
#include <dcmf.h>
#include <dcmf_globalcollectives.h>
#include <dcmf_collectives.h>
#include <assert.h>

/*************************************************
 *                 Constants                     *
 ************************************************/

#define A1C_ALIGNMENT 16

#define A1C_ENABLE_SCALEFREE_FLUSH 0 
#define A1C_FLUSHALL_BATCH_SIZE 512

#define A1C_REQUEST_POOL_INITIAL 1000
#define A1C_REQUEST_POOL_INCREMENT 100

/*************************************************
 *                  Macros                       *
 *************************************************/

#define A1DI_CRITICAL_CALL(call) do {                             \
      if(A1D_Messager_info.thread_level > A1_THREAD_SERIALIZED) {   \
        DCMF_CriticalSection_enter(0);                            \
        call;                                                     \
        DCMF_CriticalSection_exit(0);                             \
      } else {                                                    \
        call;                                                     \
      }                                                           \
    } while (0)

#define A1DI_CRITICAL_ENTER() do {                                  \
      if(A1D_Messager_info.thread_level > A1_THREAD_SERIALIZED) {   \
        DCMF_CriticalSection_enter(0);                            \
      }                                                           \
    } while (0)      \

#define A1DI_CRITICAL_EXIT() do {                                  \
      if(A1D_Messager_info.thread_level > A1_THREAD_SERIALIZED) {   \                                                  \
        DCMF_CriticalSection_exit(0);                            \
      }                                                           \
    } while (0)      \

#define A1D_Advance() do {                                  \
      if(!A1D_Messager_info.cht_enabled) {   \
        DCMF_Messager_advance(0);                            \
      }                                                           \
    } while (0)      \



/*************************************************
 *             Data Structures                   *
 *************************************************/

typedef struct
{
   size_t my_rank;
   size_t num_ranks;
   DCMF_Hardware_t hw;
} A1D_Process_info_t;

/* TODO: Pack header supports only upto 3D arrays. Need to increase structure or 
 * find a better way to represent it */
typedef struct
{
   void *vaddress;
   uint32_t stride_levels;
   uint32_t src_stride_ar[2];
   uint32_t trg_stride_ar[2];
   uint32_t count[3];
} A1D_Pack_header_t;

typedef struct A1D_Request_info_t
{
   DCMF_Request_t request;
   uint32_t active;
   struct A1D_Request_info_t *next;
   struct A1D_Request_info_t *prev;
} A1D_Request_info_t;

typedef struct 
{
   A1D_Request_info_t *head;
   A1D_Request_info_t *tail;
   A1D_Request_info_t *current;
} A1D_Request_pool_t;

typedef struct
{
   DCMF_Protocol_t protocol;
   size_t rcv_active;
   void **xchange_ptr;
   size_t xchange_size;
} A1D_Control_xchange_info_t;

typedef struct
{
   DCMF_Protocol_t protocol;
   uint32_t rcv_active;
   DCMF_Control_t info;
} A1D_Control_fenceack_info_t;

typedef struct
{
   DCMF_Protocol_t protocol;
   uint32_t rcv_active;
} A1D_Send_info_t;

typedef struct
{
   DCMF_Protocol_t protocol;
   DCMF_Callback_t callback;
   uint32_t active;
} A1D_GlobalBarrier_info_t;

/*************************************************
 *             Global variables                  *
 ************************************************/

extern A1D_Process_info_t A1D_Process_info;
extern A1D_Control_xchange_info_t A1D_Control_xchange_info;
extern A1D_Control_fenceack_info_t A1D_Control_fenceack_info;
extern A1D_Send_info_t A1D_Send_noncontigput_info;
extern A1D_Send_info_t A1D_Send_fence_info;
extern A1D_GlobalBarrier_info_t A1D_GlobalBarrier_info;
extern A1D_Request_pool_t A1D_Request_pool;

extern DCMF_Configure_t A1D_Messager_info;
extern DCMF_Protocol_t A1D_Generic_put_protocol;
extern DCMF_Protocol_t A1D_Generic_get_protocol;
extern DCMF_Callback_t A1D_Nocallback;
extern DCMF_Memregion_t *A1D_Memregion_global;

extern void **A1D_Membase_global;
extern uint32_t *A1D_Connection_active;
extern uint32_t enable_scalefree_flush; 

/************************************************* 
 *             Function Prototypes               *
 ************************************************/

void A1DI_Generic_callback(void *, DCMF_Error_t *);

A1D_Request_info_t* A1DI_Get_request();

void A1DI_Free_request(A1D_Request_info_t *request);

void A1DI_GlobalBarrier();

void A1DI_Read_parameters();
