/********************************************************************
 * The following is a notice of limited availability of the code, and disclaimer
 * which must be included in the prologue of the code and in all source listings
 * of the code.
 *
 * Copyright (c) 2010 Argonne Leadership Computing Facility, Argonne National Laboratory
 *
 * Permission is hereby granted to use, reproduce, prepare derivative works, and
 * to redistribute to others.
 *
 *                 LICENSE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer listed
 *    in this license in the documentation and/or other materials
 *    provided with the distribution.
 *
 *  - Neither the name of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * The copyright holders provide no reassurances that the source code
 * provided does not infringe any patent, copyright, or any other
 * intellectual property rights of third parties.  The copyright holders
 * disclaim any liability to any recipient for claims brought against
 * recipient by any third party for infringement of that parties
 * intellectual property rights.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************/

#include "a1d_comm.h"

DCMF_Protocol_t A1D_PutC_protocol;
DCMF_Protocol_t A1D_GetC_protocol;
#ifdef ACCUMULATE_IMPLEMENTED
DCMF_Protocol_t A1D_AccC_protocol;
#endif

void A1D_Done_cb(void * clientdata, DCMF_Error_t * error)
{
    --(*((uint32_t *) clientdata));
}

int A1DI_PutC_initialize()
{
    DCMF_Result dcmf_result;
    DCMF_Put_Configuration_t conf;

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1DI_PutC_initialize() \n");
#endif

    DCMF_CriticalSection_enter(0);

    conf.protocol = DCMF_DEFAULT_PUT_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;

    dcmf_result = DCMF_Put_register(&A1D_Put_protocol, &conf);
    assert(dcmf_result==DCMF_SUCCESS);

    DCMF_CriticalSection_exit(0);

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1DI_PutC_initialize() \n");
#endif

    return(0);
}

int A1DI_GetC_initialize()
{
    DCMF_Result dcmf_result;
    DCMF_Get_Configuration_t conf;

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1DI_GetC_initialize() \n");
#endif

    conf.protocol = DCMF_DEFAULT_GET_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;

    dcmf_result = DCMF_Get_register(&A1D_Get_protocol, &conf);
    assert(dcmf_result==DCMF_SUCCESS);

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1DI_GetC_initialize() \n");
#endif

    return(0);
}

#ifdef ACCUMULATE_IMPLEMENTED
int A1DI_AccC_Initialize()
{
    DCMF_Result dcmf_result;
    DCMF_Send_Configuration_t conf;

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1DI_AccC_Initialize() \n");
#endif

    conf.protocol = DCMF_DEFAULT_SEND_PROTOCOL;
    conf.network = DCMF_TORUS_NETWORK;
    conf.cb_recv_short = A1D_Acc_short_cb;
    conf.cb_recv_short_clientdata = NULL;
    conf.cb_recv = A1D_Acc_cb;
    conf.cb_recv_clientdata = NULL;

    dcmf_result = DCMF_Send_register(&A1D_Acc_protocol, &conf);
    assert(dcmf_result==DCMF_SUCCESS);

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1DI_AccC_Initialize() \n");
#endif

    return(0);
}
#endif


int A1D_GetC(int target, int bytes, void* src, void* dst)
{
    DCMF_Result dcmf_result;
    DCMF_Request_t request;
    DCMF_Callback_t done_callback;
    volatile int done_active;
    size_t src_disp, dst_disp;

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_GetC(int target, int bytes, void* src, void* dst) \n");
#endif

    DCMF_CriticalSection_enter(0);

    done_callback.function = A1D_Done_cb;
    done_callback.clientdata = (void *) &done_active;

    done_active = 1;

    src_disp = (size_t) src - (size_t) A1D_Baseptr_list[mpi_rank];
    dst_disp = (size_t) dst - (size_t) A1D_Baseptr_list[target];

    dcmf_result = DCMF_Get(&A1D_Get_protocol,
                           &request,
                           done_callback,
                           DCMF_RELAXED_CONSISTENCY,
                           target,
                           bytes,
                           &A1D_Memregion_list[target],
                           &A1D_Memregion_list[mpi_rank],
                           src_disp,
                           dst_disp);
    assert(dcmf_result==DCMF_SUCCESS);

    A1DI_Conditional_advance(done_active > 0);

    DCMF_CriticalSection_exit(0);

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_GetC(int target, int bytes, void* src, void* dst) \n");
#endif


    return(0);
}

int A1D_PutC(int target, int bytes, void* src, void* dst)
{
    DCMF_Result dcmf_result;
    DCMF_Request_t request;
    DCMF_Callback_t done_callback;
    volatile int done_active;
    size_t src_disp, dst_disp;

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_PutC(int target, int bytes, void* src, void* dst) \n");
#endif

    DCMF_CriticalSection_enter(0);

    src_disp = (size_t) src - (size_t) A1D_Baseptr_list[mpi_rank];
    dst_disp = (size_t) dst - (size_t) A1D_Baseptr_list[target];

    done_callback.function = A1D_Done_cb;
    done_callback.clientdata = (void *) &done_active;

    done_active = 1;

#ifdef FLUSH_IMPLEMENTED

    A1D_Put_flush_list[target]++;

    /* local completion only - must flush later */
    dcmf_result = DCMF_Put(&A1D_Put_protocol,
                           &request,
                           done_callback, /* local completion */
                           DCMF_SEQUENTIAL_CONSISTENCY,
                           target,
                           bytes,
                           &A1D_Memregion_list[mpi_rank],
                           &A1D_Memregion_list[target],
                           src_disp,
                           dst_disp,
                           A1D_Nocallback); /* remote completion */
    assert(dcmf_result==DCMF_SUCCESS);

    A1DI_Conditional_advance(done_active > 0);

    DCMF_CriticalSection_exit(0);

#else

    DCMF_CriticalSection_enter(0);

    /* end-to-end completion - no flush required */
    dcmf_result = DCMF_Put(&A1D_Put_protocol,
                           &request,
                           A1D_Nocallback, /* local completion */
                           DCMF_RELAXED_CONSISTENCY,
                           target,
                           bytes,
                           &A1D_Memregion_list[mpi_rank],
                           &A1D_Memregion_list[target],
                           src_disp,
                           dst_disp,
                           done_callback); /* remote completion */
    assert(dcmf_result==DCMF_SUCCESS);

#endif

    A1DI_Conditional_advance(done_active > 0);

    DCMF_CriticalSection_exit(0);

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_PutC(int target, int bytes, void* src, void* dst) \n");
#endif

    return(0);
}

int A1D_AccC(int proc, int bytes, void* src, void* dst, int type, void* scale)
{
    return 0;
}

#ifdef STRIDED_IMPLEMENTED

int A1D_GetS(int proc, stride_levels, block_sizes,
             src_ptr, src_stride_arr,
             dst_ptr, dst_stride_arr)
{
    return 0;
}

int A1D_PutS(int proc, stride_levels, block_sizes,
             src_ptr, src_stride_arr,
             dst_ptr, dst_stride_arr)
{
    return 0;
}

#ifdef ACCUMULATE_IMPLEMENTED
int A1D_AccS(int proc, stride_levels, block_sizes,
             src_ptr, src_stride_arr,
             dst_ptr, dst_stride_arr,
             int type, void* scale)
{
    return 0;
}
#endif

#endif
