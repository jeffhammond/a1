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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include <mpi.h>

#include "armci.h"

#ifdef __CRAYXE
#include <dmapp.h>
void parse_error(dmapp_return_t rc)
{
    switch (rc)
    {
        case DMAPP_RC_SUCCESS:
            //fprintf(stderr,"DMAPP_RC_SUCCESS\n");
            break;

        case DMAPP_RC_INVALID_PARAM:
            fprintf(stderr,"DMAPP_RC_INVALID_PARAM\n");
            break;

        case DMAPP_RC_ALIGNMENT_ERROR:
            fprintf(stderr,"DMAPP_RC_ALIGNMENT_ERROR\n");
            break;

        case DMAPP_RC_TRANSACTION_ERROR:
            fprintf(stderr,"DMAPP_RC_TRANSACTION_ERROR\n");
            break;

        case DMAPP_RC_RESOURCE_ERROR:
            fprintf(stderr,"DMAPP_RC_RESOURCE_ERROR\n");
            break;

        case DMAPP_RC_PERMISSION_ERROR:
            fprintf(stderr,"DMAPP_RC_PERMISSION_ERROR\n");
            break;

        case DMAPP_RC_NO_SPACE:
            fprintf(stderr,"DMAPP_RC_NO_SPACE\n");
            break;

        case DMAPP_RC_NOT_DONE:
            fprintf(stderr,"DMAPP_RC_NOT_DONE\n");
            break;

        case DMAPP_RC_NOT_SUPPORTED:
            fprintf(stderr,"DMAPP_RC_NOT_SUPPORTED\n");
            break;

        case DMAPP_RC_NOT_FOUND:
            fprintf(stderr,"DMAPP_RC_NOT_FOUND\n");
            break;

        case DMAPP_RC_BUSY:
            fprintf(stderr,"DMAPP_RC_BUSY\n");
            break;

        case DMAPP_RC_NOT_USED:
            fprintf(stderr,"DMAPP_RC_NOT_USED\n");
            break;

        default:
            fprintf(stderr,"Unknown DMAPP error code. \n");
            break;
    }
    fflush(stderr);
    return;
}

#endif

//#define FLUSH_IMPLEMENTED

MPI_Comm A1_COMM_WORLD;

#ifdef __CRAYXE
dmapp_jobinfo_t dmapp_info;
dmapp_seg_desc_t * dmapp_sheap_ptr;
int64_t * flush_qword = NULL;
#endif

#ifdef FLUSH_IMPLEMENTED
int * flush_list;
const int DMAPP_FLUSH_COUNT_MAX = 100;
#endif

void ARMCI_Boot(void)
{
#ifdef __CRAYXE
    dmapp_return_t                      dmapp_status = DMAPP_RC_SUCCESS;
    dmapp_rma_attrs_ext_t               dmapp_config_in, dmapp_config_out;

    dmapp_config_in.max_concurrency      = 1; /* not thread-safe */
    dmapp_config_in.max_outstanding_nb   = DMAPP_DEF_OUTSTANDING_NB; /*  512 */
    dmapp_config_in.offload_threshold    = DMAPP_OFFLOAD_THRESHOLD;  /* 4096 */

#  ifdef DETERMINISTIC_ROUTING
    dmapp_config_in.put_relaxed_ordering = DMAPP_ROUTING_DETERMINISTIC;
    dmapp_config_in.get_relaxed_ordering = DMAPP_ROUTING_DETERMINISTIC;
#  else
    dmapp_config_in.put_relaxed_ordering = DMAPP_ROUTING_ADAPTIVE;
    dmapp_config_in.get_relaxed_ordering = DMAPP_ROUTING_ADAPTIVE;
#  endif

#  ifndef FLUSH_IMPLEMENTED
    dmapp_config_in.PI_ordering          = DMAPP_PI_ORDERING_STRICT;
#  else
    dmapp_config_in.PI_ordering          = DMAPP_PI_ORDERING_RELAXED;
#  endif

    dmapp_status = dmapp_init_ext( &dmapp_config_in, &dmapp_config_out );
    assert(dmapp_status==DMAPP_RC_SUCCESS);

#ifndef FLUSH_IMPLEMENTED
    /* without strict PI ordering, we have to flush remote stores with a get packet to force global visibility */
    assert( dmapp_config_out.PI_ordering == DMAPP_PI_ORDERING_STRICT);
#endif

#endif

    return;
}

int ARMCI_Init(void)
{
    int mpi_status = MPI_SUCCESS;
    int mpi_rank = -1;
    int mpi_size = -1;

    int64_t in[2], out[2];

#ifdef __CRAYXE
    dmapp_return_t                      dmapp_status = DMAPP_RC_SUCCESS;
    dmapp_rma_attrs_ext_t               dmapp_config_in, dmapp_config_out;
    dmapp_pe_t                          dmapp_rank = -1;
    int                                 dmapp_size = -1;
#endif

    /* have to use our own communicator for collectives to be proper */
    mpi_status = MPI_Comm_dup(MPI_COMM_WORLD,&A1_COMM_WORLD);
    assert(mpi_status==0);

    /* get my MPI rank */
    mpi_status = MPI_Comm_rank(A1_COMM_WORLD,&mpi_rank);
    assert(mpi_status==0);

    /* get MPI world size */
    mpi_status = MPI_Comm_size(A1_COMM_WORLD,&mpi_size);
    assert(mpi_status==0);

#ifdef __CRAYXE
//    dmapp_config_in.max_concurrency      = 1; /* not thread-safe */
//    dmapp_config_in.max_outstanding_nb   = DMAPP_DEF_OUTSTANDING_NB; /*  512 */
//    dmapp_config_in.offload_threshold    = DMAPP_OFFLOAD_THRESHOLD;  /* 4096 */
//
//#  ifdef DETERMINISTIC_ROUTING
//    dmapp_config_in.put_relaxed_ordering = DMAPP_ROUTING_DETERMINISTIC;
//    dmapp_config_in.get_relaxed_ordering = DMAPP_ROUTING_DETERMINISTIC;
//#  else
//    dmapp_config_in.put_relaxed_ordering = DMAPP_ROUTING_ADAPTIVE;
//    dmapp_config_in.get_relaxed_ordering = DMAPP_ROUTING_ADAPTIVE;
//#  endif
//
//#  ifndef FLUSH_IMPLEMENTED
//    dmapp_config_in.PI_ordering          = DMAPP_PI_ORDERING_STRICT;
//#  else
//    dmapp_config_in.PI_ordering          = DMAPP_PI_ORDERING_RELAXED;
//#  endif
//
//    dmapp_status = dmapp_init_ext( &dmapp_config_in, &dmapp_config_out );
//    assert(dmapp_status==DMAPP_RC_SUCCESS);
//
//#ifndef FLUSH_IMPLEMENTED
//    /* without strict PI ordering, we have to flush remote stores with a get packet to force global visibility */
//    assert( dmapp_config_out.PI_ordering == DMAPP_PI_ORDERING_STRICT);
//#endif

    dmapp_status = dmapp_get_jobinfo(&dmapp_info);
    assert(dmapp_status==DMAPP_RC_SUCCESS);

    dmapp_rank      = dmapp_info.pe;
    dmapp_size      = dmapp_info.npes;
    dmapp_sheap_ptr = &(dmapp_info.sheap_seg);

    /* make sure PMI and DMAPP agree */
    assert(mpi_rank==(int)dmapp_rank);
    assert(mpi_size==dmapp_size);

    flush_qword = dmapp_sheap_malloc( sizeof(int64_t) );
    assert(flush_qword!=NULL);

    in[0]  = (int64_t) flush_qword;
    in[1]  = (int64_t) flush_qword;
    in[1] *= -1;
    out[0] = 0;
    out[1] = 0;

    mpi_status = MPI_Allreduce( in, out, 2, MPI_INT64_T, MPI_MAX, A1_COMM_WORLD );
    assert(mpi_status==MPI_SUCCESS);

    if ( (out[0]==in[0]) && (out[1]==in[1]) )
    {
        fprintf(stderr, "%d: flush_qword address (%p) is symmetric, O(1) storage required for sheap. \n",    mpi_rank, flush_qword );
    }
    else
    {
        fprintf(stderr, "%d: flush_qword address (%p) is nonsymmetric, O(N) storage required for sheap. \n", mpi_rank, flush_qword );
    }
    fflush(stderr);

    (*flush_qword) = mpi_rank;
#endif

#ifdef FLUSH_IMPLEMENTED
    flush_list = malloc( mpi_size * sizeof(int) );
    assert(flush_list != NULL);
#endif

    mpi_status = MPI_Barrier(A1_COMM_WORLD);
    assert(mpi_status==MPI_SUCCESS);

    return(0);
}

void ARMCI_Finalize(void)
{
    int mpi_status = MPI_SUCCESS;
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
#endif

    mpi_status = MPI_Barrier(A1_COMM_WORLD);
    assert(mpi_status==MPI_SUCCESS);

#ifdef FLUSH_IMPLEMENTED
    free(flush_list);
#endif

#ifdef __CRAYXE
    dmapp_status = dmapp_finalize();
    assert(dmapp_status==DMAPP_RC_SUCCESS);
#endif

    return;
}

void ARMCI_Cleanup(void)
{
    ARMCI_Finalize();

    return;
}

void ARMCI_Error(char *msg, int code)
{
    fprintf(stderr,"%s",msg);
    MPI_Abort(MPI_COMM_WORLD,code);

    return;
}

int ARMCI_Malloc(void * ptr_arr[], int bytes)
{
    int mpi_status = MPI_SUCCESS;
    void * tmp_ptr = NULL;

#ifdef __CRAYXE
    tmp_ptr = dmapp_sheap_malloc( (size_t)bytes );
    assert(tmp_ptr!=NULL);
#endif

    mpi_status = MPI_Allgather( &tmp_ptr, sizeof(void*), MPI_BYTE,
                                ptr_arr,  sizeof(void*), MPI_BYTE,
                                A1_COMM_WORLD );
    assert(mpi_status==MPI_SUCCESS);

    return(0);
}

int ARMCI_Free(void * ptr)
{
    if (ptr != NULL)
    {
        dmapp_sheap_free(ptr);
    }
    else
    {
        fprintf(stderr, "You tried to free a NULL pointer.  Please check your code. \n");
        fflush(stderr);
    }

    return(0);
}

void ARMCI_Fence(int proc)
{
    int mpi_status = MPI_SUCCESS;
    int mpi_rank = -1;

#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
    dmapp_seg_desc_t * remote_sheap_ptr = NULL;
    int64_t temp = -1;
#endif

#if defined(FLUSH_IMPLEMENTED) && defined(__CRAYXE)
    dmapp_status = dmapp_get( &temp, flush_qword, dmapp_sheap_ptr, (dmapp_pe_t)proc, 1, DMAPP_QW );
    parse_error(dmapp_status);
    assert(dmapp_status==DMAPP_RC_SUCCESS);

    if (temp != proc)
    {
        mpi_status = MPI_Comm_rank(A1_COMM_WORLD, &mpi_rank);
        assert(mpi_status==0);

        fprintf(stderr, "flush_qword: expected %d, got %ld \n", mpi_rank, temp);
        fflush(stderr);
    }
#endif

    return;
}
void ARMCI_AllFence(void)
{
    int i;
    int mpi_status = MPI_SUCCESS;
    int mpi_size = -1;

#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
    dmapp_seg_desc_t * remote_sheap_ptr = NULL;

    int count = 0;
    int gsync = 0;

#ifdef FLUSH_IMPLEMENTED
    int temp[DMAPP_FLUSH_COUNT_MAX+1];
#endif
#endif

    mpi_status = MPI_Comm_size(A1_COMM_WORLD,&mpi_size);
    assert(mpi_status==0);

#ifdef FLUSH_IMPLEMENTED
    for ( i=0 ; i<mpi_size ; i++)
    {
        if ( flush_list[i] > 0 )
        {
            ARMCI_Fence(i);
#if 0
#ifdef __CRAYXE
            dmapp_status = dmapp_get_nbi( &temp[count], flush_qword, dmapp_sheap_ptr, (dmapp_pe_t)i, 1, DMAPP_QW );
            parse_error(dmapp_status);
            assert(dmapp_status==DMAPP_RC_SUCCESS);

            count++;

            if ( count > DMAPP_FLUSH_COUNT_MAX )
            {
                dmapp_status = dmapp_gsync_wait();
                parse_error(dmapp_status);
                assert(dmapp_status==DMAPP_RC_SUCCESS);

                count = 0;
                gsync++;
            }
#endif
#endif
        }
    }

#if 0
#ifdef __CRAYXE
    /* in case we never reached count > DMAPP_FLUSH_COUNT_MAX, we must call gsync at least once
     * to ensure that implicit NB get ops complete remotely, thus ensuring global visability  */
    if ( gsync == 0 )
    {
        dmapp_status = dmapp_gsync_wait();
        parse_error(dmapp_status);
        assert(dmapp_status==DMAPP_RC_SUCCESS);
    }
#endif
#endif

    for ( i=0 ; i<mpi_size ; i++) flush_list[i] = 0;
#endif

    return;
}

void ARMCI_Barrier(void)
{
    int mpi_status = MPI_SUCCESS;

    mpi_status = MPI_Barrier(A1_COMM_WORLD);
    assert(mpi_status==MPI_SUCCESS);

    ARMCI_AllFence();

    return;
}

int ARMCI_Put(void *src, void *dst, int bytes, int proc)
{
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;

#if 1
    dmapp_jobinfo_t    job;
    dmapp_seg_desc_t * seg = NULL;

    dmapp_status = dmapp_get_jobinfo(&job);
    assert(dmapp_status==DMAPP_RC_SUCCESS);

    seg = &(job.sheap_seg);
#endif

    /* empirically, DMAPP_DW delivers the best performance.
     * no benefit was observed with DMAPP_QW or DMAPP_DQW; in fact, performance was worse */
    if (bytes%4 == 0)
    {
        //dmapp_status = dmapp_put( dst, dmapp_sheap_ptr, proc, src, bytes/4, DMAPP_DW);
        dmapp_status = dmapp_put( dst, seg, proc, src, bytes/4, DMAPP_DW);
        parse_error(dmapp_status);
        assert(dmapp_status==DMAPP_RC_SUCCESS);
    }
    else
    {
        //dmapp_status = dmapp_put( dst, dmapp_sheap_ptr, proc, src, bytes, DMAPP_BYTE);
        dmapp_status = dmapp_put( dst, seg, proc, src, bytes, DMAPP_BYTE);
        parse_error(dmapp_status);
        assert(dmapp_status==DMAPP_RC_SUCCESS);
    }
#endif

#ifdef FLUSH_IMPLEMENTED
    flush_list[proc]++;
#endif

    return(0);
}

