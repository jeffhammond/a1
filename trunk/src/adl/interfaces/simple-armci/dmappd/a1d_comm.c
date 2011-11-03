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

#include "a1d_headers.h"
#include "a1d_globals.h"

#include "a1d_api.h"

/*********************************************************************/

#define DMAPP_FLUSH_COUNT_MAX 100

/*********************************************************************/

int A1D_GetC(int target, int bytes, void * src, void * dst)
{
    uint64_t nelems = 0;
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_GetC(int target, int bytes, void* src, void* dst) \n");
#endif

#ifdef __CRAYXE
    if (bytes%16 == 0)
    {
        nelems = bytes/16;
        dmapp_status = dmapp_get( dst, src, &A1D_Sheap_desc, (dmapp_pe_t)target, nelems, DMAPP_DQW);
        assert(dmapp_status==DMAPP_RC_SUCCESS);
    }
    else if (bytes%8 == 0)
    {
        nelems = bytes/8;
        dmapp_status = dmapp_get( dst, src, &A1D_Sheap_desc, (dmapp_pe_t)target, nelems, DMAPP_QW);
        assert(dmapp_status==DMAPP_RC_SUCCESS);
    }
    else if (bytes%4 == 0)
    {
        nelems = bytes/4;
        dmapp_status = dmapp_get( dst, src, &A1D_Sheap_desc, (dmapp_pe_t)target, nelems, DMAPP_DW);
        assert(dmapp_status==DMAPP_RC_SUCCESS);
    }
    else
    {
        nelems = bytes;
        dmapp_status = dmapp_get( dst, src, &A1D_Sheap_desc, (dmapp_pe_t)target, nelems, DMAPP_BYTE);
        assert(dmapp_status==DMAPP_RC_SUCCESS);
    }
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_GetC(int target, int bytes, void* src, void* dst) \n");
#endif

    return(0);
}

int A1D_PutC(int target, int bytes, void * src, void * dst)
{
    uint64_t nelems = 0;
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_PutC(int target, int bytes, void* src, void* dst) \n");
#endif

#ifdef __CRAYXE
    if (bytes%16 == 0)
    {
        nelems = bytes/16;
        dmapp_status = dmapp_put( dst, &A1D_Sheap_desc, (dmapp_pe_t)target, src, nelems, DMAPP_DQW);
        assert(dmapp_status==DMAPP_RC_SUCCESS);
    }
    else if (bytes%8 == 0)
    {
        nelems = bytes/8;
        dmapp_status = dmapp_put( dst, &A1D_Sheap_desc, (dmapp_pe_t)target, src, nelems, DMAPP_QW);
        assert(dmapp_status==DMAPP_RC_SUCCESS);
    }
    else if (bytes%4 == 0)
    {
        nelems = bytes/4;
        dmapp_status = dmapp_put( dst, &A1D_Sheap_desc, (dmapp_pe_t)target, src, nelems, DMAPP_DW);
        assert(dmapp_status==DMAPP_RC_SUCCESS);
    }
    else
    {
        nelems = bytes;
        dmapp_status = dmapp_put( dst, &A1D_Sheap_desc, (dmapp_pe_t)target, src, nelems, DMAPP_BYTE);
        assert(dmapp_status==DMAPP_RC_SUCCESS);
    }
#endif

#ifdef FLUSH_IMPLEMENTED
    A1D_Put_flush_list[target]++;
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_PutC(int target, int bytes, void* src, void* dst) \n");
#endif

    return(0);
}

int A1D_AccC(int proc, int bytes, void * src, void * dst, int type, void * scale)
{
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_AccC(int target, int bytes, void* src, void* dst, void * scale) \n");
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_AccC(int target, int bytes, void* src, void* dst, void * scale) \n");
#endif

    return 0;
}

/*********************************************************************/

#ifdef STRIDED_IMPLEMENTED

int A1D_GetS(int proc, stride_levels, block_sizes,
             src_ptr, src_stride_arr,
             dst_ptr, dst_stride_arr)
{
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
#endif


    return 0;
}

int A1D_PutS(int proc, stride_levels, block_sizes,
             src_ptr, src_stride_arr,
             dst_ptr, dst_stride_arr)
{
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
#endif


    return 0;
}

int A1D_AccS(int proc, stride_levels, block_sizes,
             src_ptr, src_stride_arr,
             dst_ptr, dst_stride_arr,
             int type, void * scale)
{
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
#endif


    return 0;
}

#endif

/*********************************************************************/

int A1D_Flush(int target)
{
    int64_t temp = -1;
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_Flush(int target) \n");
#endif

#ifdef __CRAYXE
    dmapp_status = dmapp_get( &temp, A1D_Acc_lock, &A1D_Sheap_desc, (dmapp_pe_t)target, 1, DMAPP_QW );
    assert(dmapp_status==DMAPP_RC_SUCCESS);
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_Flush(int target) \n");
#endif

    return(0);
}

int A1D_Flush_all(void)
{
    int     count = 0;
    int     gsync = 0;
    int64_t temp[DMAPP_FLUSH_COUNT_MAX+1];
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_Flush(int target) \n");
#endif

#ifdef __CRAYXE
    /* this is not necessary until NB ops are implemented */
    dmapp_status = dmapp_gsync_wait();
    assert(dmapp_status==DMAPP_RC_SUCCESS);

    for ( int i=0 ; i<mpi_size ; i++)
    {
        if ( A1D_Put_flush_list[i] > 0 )
        {
            dmapp_status = dmapp_get_nbi( &temp[count], A1D_Acc_lock, &A1D_Sheap_desc, (dmapp_pe_t)i, 1, DMAPP_QW );
            assert(dmapp_status==DMAPP_RC_SUCCESS);

            count++;

            if ( count > DMAPP_FLUSH_COUNT_MAX )
            {
                dmapp_status = dmapp_gsync_wait();
                assert(dmapp_status==DMAPP_RC_SUCCESS);

                count = 0;
                gsync++;
            }
        }
    }

    /* in case we never reached count > DMAPP_FLUSH_COUNT_MAX, we must call gsync at least once 
     * to ensure that implicit NB get ops complete remotely, thus ensuring global visability  */
    if ( gsync == 0 )
    {
        dmapp_status = dmapp_gsync_wait();
        assert(dmapp_status==DMAPP_RC_SUCCESS);
    }
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_Flush(int target) \n");
#endif

    return(0);
}
