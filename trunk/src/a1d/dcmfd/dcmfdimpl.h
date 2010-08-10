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

#define A1C_ENABLE_CHT 0

#define A1C_DIRECT_NONCONTIG_THRESHOLD 512

#define A1C_ENABLE_IMMEDIATE_FLUSH 0 
#define A1C_FLUSHALL_PENDING_LIMIT 512 

#define A1C_REQUEST_POOL_INITIAL 200
#define A1C_REQUEST_POOL_INCREMENT 100
#define A1C_REQUEST_POOL_LIMIT 500

/*************************************************
 *                  Macros                       *
 *************************************************/

#define A1DI_CRITICAL_CALL(call)                                 \
    do {                                                          \
      if(A1D_Messager_info.thread_level > A1_THREAD_SERIALIZED)   \
      {                                                           \
        DCMF_CriticalSection_enter(0);                            \
        call;                                                     \
        DCMF_CriticalSection_exit(0);                             \
      }                                                           \
      else                                                       \
      {                                                           \
        call;                                                     \
      }                                                           \
    } while (0)                                                  \

#define A1DI_CRITICAL_ENTER()                                    \
    do {                                                          \
      if(A1D_Messager_info.thread_level > A1_THREAD_SERIALIZED)   \
      {                                                           \
        DCMF_CriticalSection_enter(0);                            \
      }                                                           \
    } while (0)                                                  \

#define A1DI_CRITICAL_EXIT()                                     \
    do {                                                          \
      if(A1D_Messager_info.thread_level > A1_THREAD_SERIALIZED)   \
      {                                                           \
        DCMF_CriticalSection_exit(0);                             \
      }                                                           \
    } while (0)                                                  \

#define A1DI_CRITICAL_CYCLE()                                    \
    do {                                                          \
      if(A1D_Messager_info.thread_level > A1_THREAD_SERIALIZED)   \
      {                                                           \
        DCMF_CriticalSection_cycle(0);                            \
      }                                                           \
    } while (0)

#define A1DI_Advance()                                           \
    do {                                                          \
      if(!a1_enable_cht)                                          \
      {                                                           \
        DCMF_Messager_advance(0);                                 \
      }                                                           \
    } while (0)                                                  \

#define A1DI_Conditional_advance(boolean)                        \
    do {                                                          \
      if(!a1_enable_cht)                                          \
      {                                                           \
        DCMF_Messager_advance(0);                                 \
      }                                                           \
    } while (boolean)                                            \


#define A1DI_ACC_EXECUTE(datatype, source, target, scaling, count) do {    \
    int i;                                                                 \
    datatype *a = (datatype *) source;                                     \
    datatype *b = (datatype *) target;                                     \
    datatype c = (datatype) scaling;                                       \
    for(i=0; i<count; i++)                                                 \
          b[i] = b[i] + a[i]*c;                                            \
} while(0)                                                                 \

/*************************************************
 *             Data Structures                   *
 *************************************************/

typedef struct
{
    size_t my_rank;
    size_t my_node;
    size_t num_ranks;
    size_t num_nodes;
    DCMF_Hardware_t hw;
} A1D_Process_info_t;

typedef struct
{
    DCMF_Request_t request;
    void *buffer_ptr;
    int bytes;
} A1D_Buffer_info_t;

typedef union
{
    DCQuad info[2];
    struct
    {
        void* target_ptr;
        A1_datatype_t datatype;
        union
        {
            int32_t int32_value;
            int64_t int64_value;
            uint32_t uint32_value;
            uint64_t uint64_value;
            float float_value;
            double double_value;
        } scaling;
    };
} A1D_Putacc_header_t;

/* TODO: Pack header supports only upto 3D arrays. Need to increase structure or 
 * find a better way to represent it */
typedef struct
{
    void *target_ptr;
    int trg_stride_ar[2];
    int count[3];
    int stride_levels;
    int is_getresponse;
} A1D_Packed_puts_header_t;

typedef struct
{
    uint32_t target;
    void* source_ptr;
    int src_stride_ar[2];
    void* target_ptr;
    int trg_stride_ar[2];
    int count[3];
    int stride_levels;
} A1D_Packed_gets_header_t;

typedef struct
{
    void *target_ptr;
    int trg_stride_ar[2];
    int count[3];
    int stride_levels;
    A1_datatype_t datatype;
    union
    {
        int32_t int32_value;
        int64_t int64_value;
        uint32_t uint32_value;
        uint64_t uint64_value;
        float float_value;
        double double_value;
    } scaling;
} A1D_Packed_putaccs_header_t;

typedef struct
{
    A1D_Buffer_info_t buffer_info;
    A1D_Putacc_header_t header;
} A1D_Putacc_recv_info_t;

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
    void** region_ptr;
    uint32_t region_count;
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

/* TODO: is extern rather than static the right declaration here? */
extern pthread_t A1DI_CHT_pthread;

extern A1D_Process_info_t A1D_Process_info;
extern A1D_Control_xchange_info_t A1D_Control_xchange_info;
extern A1D_Control_flushack_info_t A1D_Control_flushack_info;
extern A1D_Send_info_t A1D_Send_flush_info;
extern A1D_GlobalBarrier_info_t A1D_GlobalBarrier_info;
extern A1D_Request_pool_t A1D_Request_pool;

extern DCMF_Configure_t A1D_Messager_info;
extern DCMF_Protocol_t A1D_Generic_put_protocol;
extern DCMF_Protocol_t A1D_Generic_get_protocol;
extern DCMF_Protocol_t A1D_Generic_putacc_protocol;
extern DCMF_Protocol_t A1D_Packed_puts_protocol;
extern DCMF_Protocol_t A1D_Packed_gets_protocol;
extern DCMF_Protocol_t A1D_Packed_putaccs_protocol;
extern DCMF_Callback_t A1D_Nocallback;
extern DCMF_Memregion_t *A1D_Memregion_global;

extern void **A1D_Membase_global;
extern void **A1D_Put_Flushcounter_ptr;
extern volatile uint32_t *A1D_Connection_send_active;
extern volatile uint32_t *A1D_Connection_put_active;
extern uint32_t A1D_Expecting_getresponse;

/* TODO: Put these in some global structure ala A1D_Process_info.
 *        If these settings are going to be used for all devices,
 *        and I suspect many of them will, then this structure should
 *        be defined at a higher level and should probably be declared
 *        in the src/include directory. */
/* COMMENT: The reason to be these in a global structure is so that it is
 *          clear they are global-scope when called in a function.  It
 *          is not intuitive where these variables live otherwise.
 */
extern uint32_t a1_enable_cht;
extern uint32_t a1_enable_immediate_flush;
extern uint32_t a1_alignment;
extern uint32_t a1_direct_noncontig_threshold;
extern uint32_t a1_flushall_pending_limit;
extern uint32_t a1_request_pool_initial;
extern uint32_t a1_request_pool_increment;
extern uint32_t a1_request_pool_limit;
extern uint32_t a1_request_pool_size;

/************************************************* 
 *             Function Prototypes               *
 ************************************************/

void A1DI_Generic_done(void *, DCMF_Error_t *);

void A1DI_Free_done(void *, DCMF_Error_t *);

DCMF_Request_t* A1DI_Get_request();

void A1DI_Free_request(A1D_Request_info_t *request);

void A1DI_GlobalBarrier();

void A1DI_Read_parameters();

int A1DI_Send_flush(int proc);

int A1DI_Packed_puts(int target,
                     void* source_ptr,
                     int *src_stride_ar,
                     void* target_ptr,
                     int *trg_stride_ar,
                     int *count,
                     int stride_levels,
                     int is_getresponse);

int A1DI_Pack_strided(void **packet,
                      int *size_packet,
                      void *source_ptr,
                      int *src_stride_ar,
                      void *target_ptr,
                      int *trg_stride_ar,
                      int *count,
                      int stride_levels,
                      int is_getresponse);

int A1DI_Pack_strided_putaccs(void **packet,
                              int *size_packet,
                              void *source_ptr,
                              int *src_stride_ar,
                              void *target_ptr,
                              int *trg_stride_ar,
                              int *count,
                              int stride_levels,
                              A1_datatype_t a1_type,
                              void *scaling);

void* A1DI_Pack_data_strided(void *pointer,
                             void *source_ptr,
                             int *src_stride_ar,
                             int *count,
                             int stride_level);

int A1DI_Unpack_strided(void *packet);

int A1DI_Unpack_strided_putaccs(void *packet);

void* A1DI_Unpack_data_strided(void *pointer,
                               void *trg_ptr,
                               int *trg_stride_ar,
                               int *count,
                               int stride_level);

void* A1DI_Unpack_data_strided_acc(void *pointer,
                                   void *trg_ptr,
                                   int *trg_stride_ar,
                                   int *count,
                                   int stride_level,
                                   A1_datatype_t a1_type,
                                   void* scaling);

int A1D_Acc_process(void *src, 
                    int bytes, 
                    A1D_Putacc_header_t *header);
