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
 * int32_tellectual property rights of third parties.  The copyright holders
 * disclaim any liability to any recipient for claims brought against
 * recipient by any third party for infringement of that parties
 * int32_tellectual property rights.
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

#include "a1d_atomic.h"

DCMF_Protocol_t A1D_Inc32_protocol;

void A1DI_Inc32_cb(void * clientdata, const DCMF_Control_t * info, size_t peer)
{
    int32_t * address;
    int32_t incr;
    A1D_Inc32_t * data = (A1D_Inc32_t *) info ;

    address = data->address;
    incr    = data->incr;

    (*address) += incr;

    return;
}

void A1D_Inc32_initialize()
{
    DCMF_Result dcmf_result;
    DCMF_Control_Configuration_t conf;

    DCMF_CriticalSection_enter(0);

    conf.protocol           = DCMF_DEFAULT_CONTROL_PROTOCOL;
    conf.network            = DCMF_DEFAULT_NETWORK;
    conf.cb_recv            = A1DI_Inc32_cb;
    conf.cb_recv_clientdata = NULL;

    dcmf_result = DCMF_Control_register(&A1D_Inc32_protocol, &conf);
    assert(dcmf_result==DCMF_SUCCESS);

    DCMF_CriticalSection_exit(0);

    return;
}

int32_t A1D_Inc32(int proc, int32_t * remote, int32_t incr)
{
    DCMF_Result dcmf_result;
    A1D_Inc32_t data;
    DCMF_Control_t payload;

    DCMF_CriticalSection_enter(0);

    data.address = remote;
    data.incr    = incr;

    memcpy(&payload, &data, sizeof(A1D_Inc32_t));

    dcmf_result =  DCMF_Control(&A1D_Inc32_protocol,
                                DCMF_SEQUENTIAL_CONSISTENCY,
                                proc,
                                &payload);
    assert(dcmf_result==DCMF_SUCCESS);

    DCMF_CriticalSection_exit(0);

    return(0);
}

int32_t A1D_Inc64(int proc, int64_t * remote, int64_t incr)
{
    return(0);
}

int32_t A1D_Fetch_and_inc32(int proc, int32_t * local, int32_t * remote, int32_t incr)
{
    return(0);
}

int32_t A1D_Fetch_and_inc64(int proc, int64_t * local, int64_t * remote, int64_t incr)
{
    return(0);
}

int32_t A1D_Swap32(int proc, int32_t * local, int32_t * remote)
{
    return(0);
}

int32_t A1D_Swap64(int proc, int64_t * local, int64_t * remote)
{
    return(0);
}

