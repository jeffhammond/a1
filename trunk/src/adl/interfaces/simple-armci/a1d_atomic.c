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

#ifdef __bgp__

DCMF_Protocol_t A1D_Fetch32_protocol;
DCMF_Protocol_t A1D_Inc32_protocol;

/***********************************************************************/

void A1DI_Fetch32_cb(void * clientdata, const DCMF_Control_t * info, size_t peer)
{
    int32_t   value;
    int32_t * return_address;
    A1D_Fetch32_t * data = (A1D_Fetch32_t *) info ;

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1DI_Fetch32_cb \n");
#endif

    value          = data->value;
    return_address = data->return_address;

    if ( return_address == NULL )
    {
        fprintf(stderr,"A1DI_Fetch32_cb: return_address is a NULL pointer. This is bad. \n");
        //assert( return_address != NULL );
    }
    else
    {
        (*return_address) = value;
    }

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1DI_Fetch32_cb \n");
#endif

    return;
}

void A1DI_Inc32_cb(void * clientdata, const DCMF_Control_t * info, size_t peer)
{
    int32_t   incr;
    int32_t * incr_address;
    int32_t * return_address;
    A1D_Inc32_t * data = (A1D_Inc32_t *) info ;

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1DI_Inc32_cb \n");
#endif

    incr           = data->incr;
    incr_address   = data->incr_address;
    return_address = data->return_address;

    if ( incr_address == NULL )
    {
        fprintf(stderr,"A1DI_Inc32_cb: incr_address is a NULL pointer. This is bad. \n");
        //assert( incr_address != NULL );
    }
    else
    {
        if ( return_address != NULL )
        {
            DCMF_Result dcmf_result;
            A1D_Fetch32_t return_data;
            DCMF_Control_t return_payload;

            return_data.value          = (*incr_address);
            return_data.return_address = return_address;

            memcpy(&return_payload, &return_data, sizeof(A1D_Inc32_t));

            dcmf_result =  DCMF_Control(&A1D_Fetch32_protocol,
                                        DCMF_SEQUENTIAL_CONSISTENCY,
                                        peer,
                                        &return_payload);
        }

        if ( incr != 0 )
        {
            /* TODO: use actual atomic here */
            (*incr_address) += incr;
        }
    }

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1DI_Inc32_cb \n");
#endif

    return;
}

/***********************************************************************/

void A1DI_Fetch32_Initialize()
{
    DCMF_Result dcmf_result;
    DCMF_Control_Configuration_t conf;

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1DI_Fetch32_Initialize \n");
#endif

    if ( sizeof(A1D_Fetch32_t) > sizeof(DCMF_Control_t) )
    {
        fprintf(stderr,"A1D_Fetch32_t requires more storage than DCMF_Control_t! \n");
        assert(0);
    }

    conf.protocol           = DCMF_DEFAULT_CONTROL_PROTOCOL;
    conf.network            = DCMF_DEFAULT_NETWORK;
    conf.cb_recv            = A1DI_Fetch32_cb;
    conf.cb_recv_clientdata = NULL;

    dcmf_result = DCMF_Control_register(&A1D_Fetch32_protocol, &conf);
    assert(dcmf_result==DCMF_SUCCESS);

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1DI_Fetch32_Initialize \n");
#endif

    return;
}

void A1DI_Inc32_Initialize()
{
    DCMF_Result dcmf_result;
    DCMF_Control_Configuration_t conf;

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1DI_Inc32_Initialize \n");
#endif

    if ( sizeof(A1D_Inc32_t) > sizeof(DCMF_Control_t) )
    {
        fprintf(stderr,"A1D_Inc32_t requires more storage than DCMF_Control_t! \n");
        assert(0);
    }

    conf.protocol           = DCMF_DEFAULT_CONTROL_PROTOCOL;
    conf.network            = DCMF_DEFAULT_NETWORK;
    conf.cb_recv            = A1DI_Inc32_cb;
    conf.cb_recv_clientdata = NULL;

    dcmf_result = DCMF_Control_register(&A1D_Inc32_protocol, &conf);
    assert(dcmf_result==DCMF_SUCCESS);

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1DI_Inc32_Initialize \n");
#endif

    return;
}

void A1DI_Atomic_Initialize()
{
#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1DI_Atomic_Initialize \n");
#endif

    A1DI_Fetch32_Initialize();
    A1DI_Fetch64_Initialize();
    A1DI_Inc32_Initialize();
    A1DI_Inc64_Initialize();

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1DI_Atomic_Initialize \n");
#endif

    return;
}

/* ifdef __bgp__ */

#endif

/***********************************************************************/

void A1D_Fetch32(int proc, int32_t * remote, int32_t * local)
{
#ifdef __bgp__
    DCMF_Result dcmf_result;
    A1D_Inc32_t data;
    DCMF_Control_t payload;
    int32_t temp; /* TODO: this should go away */
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_Fetch32 \n");
#endif

#ifdef __bgp__
    DCMF_CriticalSection_enter(0);

    data.incr           = 0;
    data.incr_address   = remote;
    data.return_address = &temp;

    memcpy(&payload, &data, sizeof(A1D_Inc32_t));

    dcmf_result = DCMF_Control(&A1D_Inc32_protocol,
                               DCMF_SEQUENTIAL_CONSISTENCY,
                               proc,
                               &payload);
    assert(dcmf_result==DCMF_SUCCESS);

    /* TODO: this is not the preferred way to do it */
    (*local) = temp;

    DCMF_CriticalSection_exit(0);
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_Fetch32 \n");
#endif

    return;
}

void A1D_Inc32(int proc, int32_t * incr_address, int32_t incr)
{
#ifdef __bgp__
    DCMF_Result dcmf_result;
    A1D_Inc32_t data;
    DCMF_Control_t payload;
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_Inc32 \n");
#endif

#ifdef __bgp__
    DCMF_CriticalSection_enter(0);

    data.incr           = incr;
    data.incr_address   = incr_address;
    data.return_address = NULL;

    memcpy(&payload, &data, sizeof(A1D_Inc32_t));

    dcmf_result = DCMF_Control(&A1D_Inc32_protocol,
                               DCMF_SEQUENTIAL_CONSISTENCY,
                               proc,
                               &payload);
    assert(dcmf_result==DCMF_SUCCESS);

    DCMF_CriticalSection_exit(0);
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_Inc32 \n");
#endif

    return;
}

void A1D_Fetch_and_inc32(int proc, int32_t * local, int32_t * remote, int32_t incr)
{
    return;
}

void A1D_Swap32(int proc, int32_t * local, int32_t * remote)
{
    return;
}

