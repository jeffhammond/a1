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

#ifdef DMAPPD_USES_MPI



#else

int A1D_Barrier(void)
{
#ifdef __CRAYXE
    int pmi_status = PMI_SUCCESS;
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_Barrier() \n");
#endif

#ifdef __CRAYXE
    pmi_status = PMI_Barrier();
    assert(pmi_status==PMI_SUCCESS);
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_Barrier() \n");
#endif

    return(0);
}

int A1D_Allgather(void * local, void * gout, int local_bytes )
{
#ifdef __CRAYXE
    int pmi_status = PMI_SUCCESS;
#endif
    int rank = -1, size = -1;
    int * order = NULL;
    int * temp = NULL;

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_Allgather(void * local, void * gout, int local_size ) \n");
#endif

#ifdef __CRAYXE
    /* get my PMI rank - this is redundant*/
    pmi_status = PMI_Get_rank(&rank);
    assert(pmi_status==PMI_SUCCESS);

    /* get PMI world size - this is redundant*/
    pmi_status = PMI_Get_size(&size);
    assert(pmi_status==PMI_SUCCESS);
#endif

    /* buffer for ranks in their PMI_Allgather order */
    order = (int *) malloc( size * sizeof(int) );
    assert(order!=NULL);

    /* buffer for local in their PMI_Allgather order */
    temp = (int *) malloc( size * local_bytes );
    assert(temp!=NULL);

#ifdef __CRAYXE
    pmi_status = PMI_Allgather( &rank, order, sizeof(int) );
    assert(pmi_status==PMI_SUCCESS);

    /* finally allgather the actual data */
    pmi_status = PMI_Allgather( local, temp, local_bytes);
    assert(pmi_status==PMI_SUCCESS);
#endif

    /* reorder the data properly */
    for(int i=0 ; i<size ; i++)
        memcpy( &gout[ order[i] * local_bytes ], 
                &temp[    i     * local_bytes ],
                local_bytes );

    free(temp);
    free(order);

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_Allgather(void * local, void * gout, int local_size ) \n");
#endif

    return(0);
}

int A1D_Allreduce_max32(int32_t in, int32_t * out)
{
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_Allreduce_max32(int32_t in, int32_t * out) \n");
#endif

#ifdef __CRAYXE
    assert(A1D_Pset_world_exported);

    dmapp_status = dmapp_c_greduce_start( A1D_Pset_world, &in, out, 1, DMAPP_C_INT32, DMAPP_C_MAX );
    assert(dmapp_status==DMAPP_RC_SUCCESS);

    /* wait for greduce to finish */
    dmapp_status = dmapp_c_pset_wait( A1D_Pset_world );
    assert(dmapp_status==DMAPP_RC_SUCCESS);
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_Allreduce_max32(int32_t in, int32_t * out) \n");
#endif

    return(0);
}

int A1D_Allreduce_issame32(int32_t value, int * flag)
{
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
#endif
    int32_t in[2], out[2];

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_Allreduce_issame32(int32_t value, int * flag) \n");
#endif

    *flag = 0;

    in[0]  = value;
    in[1]  = -value;
    out[0] = 0;
    out[1] = 0;
#ifdef __CRAYXE
    assert(A1D_Pset_world_exported);

    dmapp_status = dmapp_c_greduce_start( A1D_Pset_world, &in, &out, 2, DMAPP_C_INT32, DMAPP_C_MAX );
    assert(dmapp_status==DMAPP_RC_SUCCESS);

    /* wait for greduce to finish */
    dmapp_status = dmapp_c_pset_wait( A1D_Pset_world );
    assert(dmapp_status==DMAPP_RC_SUCCESS);
#endif
    if ( (out[0] == value) && (out[1] = -value) )
        (*flag) = 1;

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_Allreduce_issame32(int32_t value, int * flag) \n");
#endif

    return(0);
}

int A1D_Allreduce_issame64(int64_t value, int * flag)
{
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
#endif
    int64_t in[2], out[2];

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_Allreduce_issame64(int64_t value, int * flag) \n");
#endif

    *flag = 0;

    in[0]  = value;
    in[1]  = -value;
    out[0] = 0;
    out[1] = 0;
#ifdef __CRAYXE
    assert(A1D_Pset_world_exported);

    dmapp_status = dmapp_c_greduce_start( A1D_Pset_world, &in, &out, 2, DMAPP_C_INT64, DMAPP_C_MAX );
    assert(dmapp_status==DMAPP_RC_SUCCESS);

    /* wait for greduce to finish */
    dmapp_status = dmapp_c_pset_wait( A1D_Pset_world );
    assert(dmapp_status==DMAPP_RC_SUCCESS);
#endif
    if ( (out[0] == value) && (out[1] = -value) )
        (*flag) = 1;

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_Allreduce_issame64(int64_t value, int * flag) \n");
#endif

    return(0);
}

#endif
