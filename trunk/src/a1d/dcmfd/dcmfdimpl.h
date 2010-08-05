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

#define A1C_ENABLE_CHT 1

#define A1C_PACKING_LIMIT 512

#define A1C_ENABLE_SCALEFREE_FLUSH 0 
#define A1C_FLUSHALL_PENDING_LIMIT 512 

#define A1C_REQUEST_POOL_INITIAL 200
#define A1C_REQUEST_POOL_INCREMENT 50
#define A1C_REQUEST_POOL_LIMIT 500

/*************************************************
 *                  Macros                       *
 *************************************************/

#define A1DI_CRITICAL_CALL(call) do {                             \
      if(A1D_Messager_info.thread_level > A1_THREAD_SERIALIZED) { \
        DCMF_CriticalSection_enter(0);                            \
        call;                                                     \
        DCMF_CriticalSection_exit(0);                             \
      } else {                                                    \
        call;                                                     \
      }                                                           \
    } while (0)

#define A1DI_CRITICAL_ENTER() do {                                \
      if(A1D_Messager_info.thread_level > A1_THREAD_SERIALIZED) { \
        DCMF_CriticalSection_enter(0);                            \
      }                                                           \
    } while (0)                                                   \

#define A1DI_CRITICAL_EXIT() do {                                 \
      if(A1D_Messager_info.thread_level > A1_THREAD_SERIALIZED) { \
        DCMF_CriticalSection_exit(0);                             \
      }                                                           \
    } while (0)                                                   \

#define A1D_Advance() do {                                        \
      if(!a1_enable_cht) {                                        \
        DCMF_Messager_advance(0);                                 \
      }                                                           \
    } while (0)                                                   \



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
   struct A1D_Request_info_t *next;
   struct A1D_Request_info_t *prev;
} A1D_Request_info_t;

typedef struct 
{
   A1D_Request_info_t *head;
   A1D_Request_info_t *current;
   A1D_Request_info_t *tail;
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
} A1D_Control_flushack_info_t;

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
extern A1D_Control_flushack_info_t A1D_Control_flushack_info;
extern A1D_Send_info_t A1D_Send_noncontigput_info;
extern A1D_Send_info_t A1D_Send_flush_info;
extern A1D_GlobalBarrier_info_t A1D_GlobalBarrier_info;
extern A1D_Request_pool_t A1D_Request_pool;

extern DCMF_Configure_t A1D_Messager_info;
extern DCMF_Protocol_t A1D_Generic_put_protocol;
extern DCMF_Protocol_t A1D_Generic_get_protocol;
extern DCMF_Callback_t A1D_Nocallback;
extern DCMF_Memregion_t *A1D_Memregion_global;

extern void **A1D_Membase_global;
extern void **A1D_Put_Flushcounter_ptr;
extern uint32_t *A1D_Connection_send_active;
extern uint32_t *A1D_Connection_put_active;

extern uint32_t a1_enable_cht;
extern uint32_t a1_enable_scalefree_flush; 
extern uint32_t a1_alignment;
extern uint32_t a1_packing_limit;
extern uint32_t a1_flushall_pending_limit;
extern uint32_t a1_request_pool_initial; 
extern uint32_t a1_request_pool_increment; 
extern uint32_t a1_request_pool_limit; 
extern uint32_t a1_request_pool_size; 

/************************************************* 
 *             Function Prototypes               *
 ************************************************/

void A1DI_Generic_done(void *, DCMF_Error_t *);

DCMF_Request_t* A1DI_Get_request();

void A1DI_Free_request(A1D_Request_info_t *request);

void A1DI_GlobalBarrier();

void A1DI_Read_parameters();

int A1DI_Send_flush(int proc);
