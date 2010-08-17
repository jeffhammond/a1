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
#include <pthread.h>
#include <bpcore/bgp_atomic_ops.h>

/*************************************************
 *                 Constants                     *
 ************************************************/

#define A1C_ALIGNMENT 16

#define A1C_DISABLE_CHT 0
#define A1C_DISABLE_INTERRUPTS 0 
#define A1C_CHT_PAUSE_CYCLES 750

#define A1C_DIRECT_NONCONTIG_PUT_THRESHOLD 512
#define A1C_DIRECT_NONCONTIG_GET_THRESHOLD 512 
#define A1C_DIRECT_NONCONTIG_PUTACC_THRESHOLD 2048 
#define A1C_DIRECT_NONCONTIG_GETACC_THRESHOLD 2048

#define A1C_ENABLE_IMMEDIATE_FLUSH 0 
#define A1C_FLUSHALL_PENDING_LIMIT 512 

#define A1C_REQUEST_POOL_INITIAL 200
#define A1C_REQUEST_POOL_INCREMENT 100
#define A1C_REQUEST_POOL_LIMIT 500

#define A1C_HANDLE_POOL_SIZE 100

#define A1C_MAX_STRIDED_DIM 4

/*************************************************
*                  BGP Atomics                   *
*************************************************/

extern _BGP_Atomic global_atomic;

/* TODO: we should have a more general lock system which can do test-test-and-set,
 *        backoff and other lock designs. */
#define A1DI_GLOBAL_ATOMIC_ACQUIRE() while(!_bgp_test_and_set(&global_atomic, 1))

#define A1DI_GLOBAL_ATOMIC_RELEASE() do{ global_atomic.atom = 0; }while(0)

/*************************************************
*           Likely and Unlikely Ifs              *
*************************************************/

#define likely_if(x) if(__builtin_expect(x,1))
#define unlikely_if(x) if(__builtin_expect(x,0))

/*************************************************
 *          Generic  Macros                      *
 *************************************************/
#define A1DI_Wait_cycles(cycles)                   \
   do {                                            \
      unsigned long long start = DCMF_Timebase();  \
      unsigned long long stop = DCMF_Timebase();   \
      while((stop - start) < cycles)               \
           stop = DCMF_Timebase();                 \
   } while(0)                                      \

#define A1DI_Wait_seconds(seconds)       \
   do {                                  \
      double start = DCMF_Timer();       \
      double stop = DCMF_Timer();        \
      while((stop - start) < seconds)    \
           stop = DCMF_Timer();          \
   } while(0)                            \

/*************************************************
 *          Memory Allocation Macros             *
 *************************************************/

#define A1DI_Malloc_aligned(ptr, num) posix_memalign(ptr, a1_settings.alignment, num)

#define A1DI_Malloc(ptr, num)  !(ptr = malloc(num))

#define A1DI_Free(ptr) free(ptr)

#define A1DI_Memset(ptr, val, num)  memset(ptr, val, num)

#define A1DI_Memcpy(trg, src, num)  memcpy(trg, src, num)

/*************************************************
 *          Critical Section Macros              *
 *************************************************/

#define A1DI_CRITICAL_CALL(call)                                  \
    do {                                                          \
      if((A1D_Messager_info.thread_level > A1_THREAD_MATCHED)     \
            || !a1_settings.disable_cht)                          \
      {                                                           \
        DCMF_CriticalSection_enter(0);                            \
        call;                                                     \
        DCMF_CriticalSection_exit(0);                             \
      }                                                           \
      else                                                        \
      {                                                           \
        call;                                                     \
      }                                                           \
    } while (0)                                                   \

#define A1DI_CRITICAL_ENTER()                                     \
    do {                                                          \
      if((A1D_Messager_info.thread_level > A1_THREAD_MATCHED)     \
            || !a1_settings.disable_cht)                          \
      {                                                           \
        DCMF_CriticalSection_enter(0);                            \
      }                                                           \
    } while (0)                                                   \

/*
#define A1DI_CRITICAL_ENTER()                                     \
    do {                                                          \
      if((A1D_Messager_info.thread_level > A1_THREAD_MATCHED)     \
            || !a1_settings.disable_cht)                          \
      {                                                           \
        A1DI_GLOBAL_ATOMIC_ACQUIRE();                             \
      }                                                           \
    } while (0)                                                   \
*/

#define A1DI_CRITICAL_EXIT()                                      \
    do {                                                          \
      if((A1D_Messager_info.thread_level > A1_THREAD_MATCHED)     \
            || !a1_settings.disable_cht)                          \
      {                                                           \
        DCMF_CriticalSection_exit(0);                             \
      }                                                           \
    } while (0)                                                   \

/*
#define A1DI_CRITICAL_EXIT()                                      \
    do {                                                          \
      if((A1D_Messager_info.thread_level > A1_THREAD_MATCHED)     \
            || !a1_settings.disable_cht)                          \
      {                                                           \
        A1DI_GLOBAL_ATOMIC_RELEASE();                             \
      }                                                           \
    } while (0)                                                   \
*/

#define A1DI_CRITICAL_CYCLE()                                     \
    do {                                                          \
      if((A1D_Messager_info.thread_level > A1_THREAD_MATCHED)     \
            || !a1_settings.disable_cht)                          \
      {                                                           \
        DCMF_CriticalSection_cycle(0);                            \
      }                                                           \
    } while (0)

#define A1DI_Advance()                                            \
    do {                                                          \
        DCMF_Messager_advance(0);                                 \
    } while (0)                                                   \

#define A1DI_Conditional_advance(boolean)                         \
    while(boolean) {                                              \
          DCMF_Messager_advance(0);                               \
    }                                                             \

#define A1DI_ACC_EXECUTE(datatype, source, target, scaling, count)          \
   do {                                                                     \
     int i;                                                                 \
     datatype *a = (datatype *) source;                                     \
     datatype *b = (datatype *) target;                                     \
     datatype c = (datatype) scaling;                                       \
     for(i=0; i<count; i++)                                                 \
          b[i] = b[i] + a[i]*c;                                             \
   } while(0)                                                               \

/*************************************************
 *             Data Structures                   *
 *************************************************/
typedef struct
{
    volatile uint32_t disable_cht;
    volatile uint32_t cht_pause_cycles;
    volatile uint32_t disable_interrupts;
    volatile uint32_t enable_immediate_flush;
    volatile uint32_t direct_noncontig_put_threshold;
    volatile uint32_t direct_noncontig_get_threshold;
    volatile uint32_t direct_noncontig_putacc_threshold;
    volatile uint32_t direct_noncontig_getacc_threshold;
    volatile uint32_t flushall_pending_limit;
    volatile uint32_t alignment;
} A1_Settings_t;

typedef struct
{
    volatile uint32_t initial_size;
    volatile uint32_t increment_size;
    volatile uint32_t limit_size;
    volatile uint32_t total_size;
} A1_Requestpool_info_t;

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
    int stride_level;
    int block_sizes[A1C_MAX_STRIDED_DIM];
    void *target_ptr;
    int trg_stride_ar[2];
} A1D_Packed_puts_header_t;

typedef struct
{
    uint32_t target;
    int stride_level;
    int block_sizes[A1C_MAX_STRIDED_DIM];
    void* source_ptr;
    int src_stride_ar[2];
    void* target_ptr;
    int trg_stride_ar[2];
} A1D_Packed_gets_header_t;

typedef struct
{
    int stride_level;
    int block_sizes[A1C_MAX_STRIDED_DIM];
    void *target_ptr;
    int trg_stride_ar[2];
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

typedef struct A1D_Handle_t
{
    A1D_Request_t *request_list;
    volatile int active;
    uintptr_t user_handle_ptr;
    struct A1D_Handle_t *next;
} A1D_Handle_t;

typedef struct A1D_Handle_pool_t
{
    A1D_Handle_t *head;
    void *region_ptr;
} A1D_Handle_pool_t;

typedef struct A1D_Request_t
{
    DCMF_Request_t request;
    uintptr_t buffer_ptr;
    struct A1D_Request_t *next;
} A1D_Request_t;

typedef struct
{
    A1D_Request_t *head;
    void** region_ptr;
    uint32_t region_count;
} A1D_Request_pool_t;

typedef struct
{
    DCMF_Protocol_t protocol;
    volatile int rcv_active;
    void **xchange_ptr;
    size_t xchange_size;
} A1D_Control_xchange_info_t;

/*************************************************
 *             Global variables                  *
 ************************************************/

extern _BGP_Atomic global_atomic;

extern volatile uint32_t global_lock;

/* TODO: is extern rather than static the right declaration here? */
extern pthread_t A1DI_CHT_pthread;

extern A1D_Process_info_t A1D_Process_info;
extern A1D_Control_xchange_info_t A1D_Control_xchange_info;
extern A1D_Request_pool_t A1D_Request_pool;
exterm A1D_Handle_pool_t A1D_Handle_pool;

extern DCMF_Configure_t A1D_Messager_info;
extern DCMF_Protocol_t A1D_Control_flushack_protocol;
extern DCMF_Protocol_t A1D_Send_flush_protocol;
extern DCMF_Protocol_t A1D_GlobalBarrier_protocol;
extern DCMF_Protocol_t A1D_Generic_put_protocol;
extern DCMF_Protocol_t A1D_Generic_get_protocol;
extern DCMF_Protocol_t A1D_Generic_putacc_protocol;
extern DCMF_Protocol_t A1D_Packed_puts_protocol;
extern DCMF_Protocol_t A1D_Packed_gets_protocol;
extern DCMF_Protocol_t A1D_Packed_gets_response_protocol;
extern DCMF_Protocol_t A1D_Packed_putaccs_protocol;
extern DCMF_Callback_t A1D_Nocallback;
extern DCMF_Memregion_t *A1D_Memregion_global;

extern void **A1D_Membase_global;
extern void **A1D_Put_Flushcounter_ptr;
extern volatile int *A1D_Connection_send_active;
extern volatile int *A1D_Connection_put_active;
extern volatile int A1D_Control_flushack_active;
extern volatile int A1D_Put_flushack_active;
extern volatile int A1D_Expecting_getresponse;

extern A1_Settings_t a1_settings;
extern A1_Requestpool_info_t a1_requestpool_info;

/************************************************* 
 *             Function Prototypes               *
 ************************************************/

void A1DI_Global_lock_acquire();

void A1DI_Global_lock_release();

void A1DI_Generic_done(void *, DCMF_Error_t *);

void A1DI_Free_done(void *, DCMF_Error_t *);

int A1DI_Memregion_Global_initialize();

DCMF_Result A1DI_Put_initialize();

DCMF_Result A1DI_Packed_puts_initialize();

DCMF_Result A1DI_Get_initialize();

A1D_Request_t* A1DI_Get_request();

void A1DI_Release_request(A1D_Request_t *request);

DCMF_Result A1DI_Packed_gets_initialize();

DCMF_Result A1DI_Putacc_initialize();

DCMF_Result A1DI_Packed_putaccs_initialize();

DCMF_Result A1DI_GlobalBarrier_initialize();

DCMF_Result A1DI_Send_flush_initialize();

DCMF_Result A1DI_Put_flush_initialize();

DCMF_Result A1DI_Control_flushack_initialize();

int A1DI_GlobalBarrier();

int A1DI_Read_parameters();

int A1DI_Send_flush(int proc);

int A1DI_Packed_puts(int target,
                     int stride_level,
                     int *block_sizes,
                     void* source_ptr,
                     int *src_stride_ar,
                     void* target_ptr,
                     int *trg_stride_ar);

int A1DI_Pack_strided(void **packet,
                      int *size_packet,
                      int stride_level,
                      int *block_sizes,
                      void *source_ptr,
                      int *src_stride_ar,
                      void *target_ptr,
                      int *trg_stride_ar);

int A1DI_Pack_strided_putaccs(void **packet,
                              int *size_packet,
                              int stride_level,
                              int *block_sizes,
                              void *source_ptr,
                              int *src_stride_ar,
                              void *target_ptr,
                              int *trg_stride_ar,
                              A1_datatype_t a1_type,
                              void *scaling);

void* A1DI_Pack_data_strided(void *pointer,
                             int stride_level,
                             int *block_sizes,
                             void *source_ptr,
                             int *src_stride_ar);

int A1DI_Unpack_strided(void *packet);

int A1DI_Unpack_strided_putaccs(void *packet);

void* A1DI_Unpack_data_strided(void *pointer,
                               int stride_level,
                               int *block_sizes,
                               void *trg_ptr,
                               int *trg_stride_ar);

void* A1DI_Unpack_data_strided_acc(void *pointer,
                                   int stride_level,
                                   int *block_sizes,
                                   void *trg_ptr,
                                   int *trg_stride_ar,
                                   A1_datatype_t a1_type,
                                   void* scaling);

int A1D_Acc_process(void *src, int bytes, A1D_Putacc_header_t *header);
