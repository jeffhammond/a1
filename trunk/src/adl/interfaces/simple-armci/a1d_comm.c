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

#include "a1d_api.h"
#include "a1d_util.h"

DCMF_Protocol_t A1D_Get_prot;
DCMF_Protocol_t A1D_Put_prot;
//DCMF_Protocol_t A1D_Acc_prot;

void A1D_Decrement(void *clientdata, DCMF_Error_t *error)
{
    --(*((uint32_t *) clientdata));
}

void A1D_Register_comm()
{
    DCMF_Result dcmf_result;
    DCMF_Get_Configuration_t get_conf;
    DCMF_Put_Configuration_t put_conf;
//    DCMF_Send_Configuration_t acc_conf;

    DCMF_CriticalSection_enter(0);

    get_conf.protocol = DCMF_DEFAULT_GET_PROTOCOL;
    get_conf.network = DCMF_TORUS_NETWORK;
    dcmf_result = DCMF_Get_register(&A1D_Get_protocol, &get_conf);
    assert(dcmf_result==DCMF_SUCCESS);

    put_conf.protocol = DCMF_DEFAULT_PUT_PROTOCOL;
    put_conf.network = DCMF_TORUS_NETWORK;
    dcmf_result = DCMF_Put_register(&A1D_Put_protocol, &put_conf);
    assert(dcmf_result==DCMF_SUCCESS);

//    acc_conf.protocol = DCMF_DEFAULT_SEND_PROTOCOL;
//    acc_conf.network = DCMF_TORUS_NETWORK;
//    acc_conf.cb_recv_short = A1DI_RecvSendShort_Acc_cb;
//    acc_conf.cb_recv_short_clientdata = NULL;
//    acc_conf.cb_recv = A1DI_RecvSend_Acc_cb;
//    acc_conf.cb_recv_clientdata = NULL;
//    dcmf_result = DCMF_Send_register(&A1D_Acc_protocol, &conf);
//    assert(dcmf_result==DCMF_SUCCESS);

    DCMF_CriticalSection_exit(0);

    return(0);
}

int A1D_Get(int target, int bytes, void* src, void* dst)
{
    DCMF_Result dcmf_result;
    DCMF_Request_t request;
    DCMF_Callback_t callback;
    size_t src_disp, dst_disp;
    volatile int active;

    DCMF_CriticalSection_enter(0);

    callback.function = A1D_Decrement;
    callback.clientdata = (void *) &active;

    src_disp = (size_t) src - (size_t) A1D_Memregion_base[target];
    dst_disp = (size_t) dst - (size_t) A1D_Memregion_base[dcmf_rank];

    active = 1;

    dcmf_result = DCMF_Get(&A1D_Get_prot,
                           &request,
                           callback,
                           DCMF_RELAXED_CONSISTENCY,
                           (size_t)target,
                           bytes,
                           &A1D_Memregions[target],
                           &A1D_Memregions[dcmf_rank],
                           src_disp,
                           dst_disp);
    assert(dcmf_result==DCMF_SUCCESS);

    while(active > 0) DCMF_Messager_advance(0);

    DCMF_CriticalSection_exit(0);

    return(0);
}

int A1D_PutC(int proc, int bytes, src, dst)
{
    DCMF_Result dcmf_result;
    DCMF_Request_t request;
    DCMF_Callback_t callback;
    size_t src_disp, dst_disp;
    volatile int active;

    DCMF_CriticalSection_enter(0);

    callback.function = A1D_Decrement;
    callback.clientdata = (void *) &active;

    src_disp = (size_t) src - (size_t) A1D_Memregion_base[dcmf_rank];
    dst_disp = (size_t) dst - (size_t) A1D_Memregion_base[target];

    /* wait for both local and remote completion */
    active = 2;

    /* remote completion requested, hence ordering not required */
    dcmf_result = DCMF_Put(&A1D_Put_prot,
                           &request,
                           callback,
                           DCMF_RELAXED_CONSISTENCY,
                           target,
                           bytes,
                           &A1D_Memregion_global[dcmf_rank],
                           &A1D_Memregion_global[target],
                           src_disp,
                           dst_disp,
                           callback);
    assert(dcmf_result==DCMF_SUCCESS);

    while(active > 0) DCMF_Messager_advance(0);

    DCMF_CriticalSection_exit(0);
}

#define A1DI_ACC(datatype, source, target, scaling, count)                  \
   do {                                                                     \
     int i;                                                                 \
     datatype *s = (datatype *) source;                                     \
     datatype *t = (datatype *) target;                                     \
     datatype c = (datatype) scaling;                                       \
     for(i=0; i<count; i++)                                                 \
          t[i] += s[i]*c;                                                   \
   } while(0)

int A1D_AccC(int proc, int bytes, src, dst, int type, void* scale)
{
    return 0;
}

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

int A1D_AccS(int proc, stride_levels, block_sizes,
                          src_ptr, src_stride_arr,
                          dst_ptr, dst_stride_arr,
                          int type, void* scale)
{
    return 0;
}



