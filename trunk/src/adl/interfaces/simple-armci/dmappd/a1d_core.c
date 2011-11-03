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

int mpi_rank;
int mpi_size;

#ifdef DMAPPD_USES_MPI
MPI_Comm A1D_COMM_WORLD;
#else
# ifdef __CRAYXE
   dmapp_c_pset_handle_t A1D_Pset_world;
# endif
  int A1D_Pset_world_exported = 0;
#endif

#ifdef __CRAYXE
  dmapp_seg_desc_t      A1D_Sheap_desc;
#endif

int64_t * A1D_Acc_lock;

int A1D_Rank()
{
    return mpi_rank;
}

int A1D_Size()
{
    return mpi_size;
}

int A1D_Initialize()
{

#ifdef DMAPPD_USES_MPI
    int mpi_initialized, mpi_provided;
    int mpi_status;
#else
    int pmi_spawned = 0;
    int pmi_status  = PMI_SUCCESS;
#endif

#ifdef __CRAYXE
    dmapp_return_t                      dmapp_status = DMAPP_RC_SUCCESS;

    dmapp_rma_attrs_ext_t               dmapp_config_in, dmapp_config_out;

    dmapp_jobinfo_t                     dmapp_info;
    dmapp_pe_t                          dmapp_rank = -1;
    int                                 dmapp_size = -1;

#ifndef DMAPPD_USES_MPI
    uint64_t                            world_pset_concat_buf_size = -1;
    void *                              world_pset_concat_buf = NULL;
    dmapp_c_pset_delimiter_strided_t    world_pset_strided;
    dmapp_c_pset_desc_t                 dmapp_world_desc;
    uint64_t                            dmapp_world_id = 1000;
    uint64_t                            dmapp_world_modes = DMAPP_C_PSET_MODE_CONCAT; /* TODO: do I need this bit set? */

    uint32_t                            dmapp_reduce_max_int32t = 0;
    uint32_t                            dmapp_reduce_max_int64t = 0;
#endif

#endif
    int                                 sheapflag = 0;

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_Initialize() \n");
#endif

#ifdef DMAPPD_USES_MPI

    /***************************************************
     *
     * configure MPI
     *
     ***************************************************/

    /* MPI has to be Initialized for this implementation to work */
    MPI_Initialized(&mpi_initialized);
    assert(mpi_initialized==1);

    /* MPI has to tolerate threads because A1 supports them */
    MPI_Query_thread(&mpi_provided);
    assert(mpi_provided>MPI_THREAD_SINGLE);

    /* have to use our own communicator for collectives to be proper */
    mpi_status = MPI_Comm_dup(MPI_COMM_WORLD,&A1D_COMM_WORLD);
    assert(mpi_status==0);

    /* get my MPI rank */
    mpi_status = MPI_Comm_rank(A1D_COMM_WORLD,&mpi_rank);
    assert(mpi_status==0);

    /* get MPI world size */
    mpi_status = MPI_Comm_size(A1D_COMM_WORLD,&mpi_size);
    assert(mpi_status==0);

    /* barrier to make sure MPI is ready everywhere */
    mpi_status = MPI_Barrier(A1D_COMM_WORLD);
    assert(mpi_status==0);

#else
# ifdef __CRAYXE

    /***************************************************
     *
     * configure PMI
     *
     ***************************************************/

    /* initialize PMI (may not be necessary */
    pmi_status = PMI_Init(&pmi_spawned);
    assert(pmi_status==PMI_SUCCESS);

    if (pmi_spawned==PMI_TRUE)
        fprintf(stderr,"PMI says this process is spawned.  This is bad. \n");
    assert(pmi_spawned==PMI_FALSE);

    /* get my PMI rank */
    pmi_status = PMI_Get_rank(&mpi_rank);
    assert(pmi_status==PMI_SUCCESS);

    /* get PMI world size */
    pmi_status = PMI_Get_size(&mpi_size);
    assert(pmi_status==PMI_SUCCESS);

# endif
#endif

#ifdef __CRAYXE

    /***************************************************
     *
     * configure DMAPP
     *
     ***************************************************/

    dmapp_config_in.max_outstanding_nb   = DMAPP_DEF_OUTSTANDING_NB;
    dmapp_config_in.offload_threshold    = DMAPP_OFFLOAD_THRESHOLD;
    dmapp_config_in.put_relaxed_ordering = DMAPP_ROUTING_DETERMINISTIC;
    dmapp_config_in.get_relaxed_ordering = DMAPP_ROUTING_DETERMINISTIC;
    dmapp_config_in.max_concurrency      = 1; /* not thread-safe */
#ifdef FLUSH_IMPLEMENTED
    dmapp_config_in.PI_ordering          = DMAPP_PI_ORDERING_RELAXED;
#else
    dmapp_config_in.PI_ordering          = DMAPP_PI_ORDERING_STRICT;
#endif

    dmapp_status = dmapp_init_ext( &dmapp_config_in, &dmapp_config_out );
    assert(dmapp_status==DMAPP_RC_SUCCESS);

#ifndef FLUSH_IMPLEMENTED
    /* without strict PI ordering, we have to flush remote stores with a get packet to force global visibility */
    assert( dmapp_config_out.PI_ordering == DMAPP_PI_ORDERING_STRICT);
#endif

    dmapp_status = dmapp_get_jobinfo(&dmapp_info);
    assert(dmapp_status==DMAPP_RC_SUCCESS);

    dmapp_rank = dmapp_info.pe;
    dmapp_size = dmapp_info.npes;
    memcpy( &A1D_Sheap_desc, &(dmapp_info.sheap_seg), sizeof(dmapp_seg_desc_t) ); /* TODO: better to keep pointer instead? */

    /* make sure PMI and DMAPP agree */
    assert(mpi_rank==dmapp_rank);
    assert(mpi_size==dmapp_size);

#ifndef DMAPPD_USES_MPI

    /***************************************************
     *
     * setup DMAPP world pset
     *
     ***************************************************/

    dmapp_status = dmapp_c_greduce_nelems_max(DMAPP_C_INT32, &dmapp_reduce_max_int32t);
    fprintf(stderr, "dmapp_reduce_max_int32t = %d \n", dmapp_reduce_max_int32t );
    dmapp_status = dmapp_c_greduce_nelems_max(DMAPP_C_INT64, &dmapp_reduce_max_int64t);
    fprintf(stderr, "dmapp_reduce_max_int64t = %d \n", dmapp_reduce_max_int64t );
    //assert(dmapp_reduce_max_int32t>2);
    //assert(dmapp_reduce_max_int64t>2);

    /* allocate proportional to job size, since this is important for performance of concatenation */
    world_pset_concat_buf_size       = 8 * mpi_size;
    world_pset_concat_buf            = dmapp_sheap_malloc( world_pset_concat_buf_size );

    world_pset_strided.n_pes         = mpi_size;
    world_pset_strided.base_pe       = 0;
    world_pset_strided.stride_pe     = 1;

    dmapp_world_desc.concat_buf      = world_pset_concat_buf;
    dmapp_world_desc.concat_buf_size = world_pset_concat_buf_size;
    dmapp_world_desc.type            = DMAPP_C_PSET_DELIMITER_STRIDED; /* FYI: this is only documented in dmapp.h */
    dmapp_world_desc.u.stride_type   = world_pset_strided;

    dmapp_status = dmapp_c_pset_create( &dmapp_world_desc, dmapp_world_id, dmapp_world_modes, NULL, &A1D_Pset_world );
    assert(dmapp_status==DMAPP_RC_SUCCESS);

    /* out-of-band sync required between pset create and pset export */
    /* not using A1D_Barrier because that might switch to using dmapp_barrier */
    pmi_status = PMI_Barrier();
    assert(pmi_status==PMI_SUCCESS);

    /* export pset after out-of-band sync */
    dmapp_status = dmapp_c_pset_export( A1D_Pset_world );
    assert(dmapp_status==DMAPP_RC_SUCCESS);

    A1D_Pset_world_exported = 1;
#endif

#endif

    /***************************************************
     *
     * setup protocols
     *
     ***************************************************/

#ifdef FLUSH_IMPLEMENTED
    /* allocate Put list */
    A1D_Put_flush_list = malloc( pmi_size * sizeof(uint32_t) );
    assert(A1D_Put_flush_list != NULL);
#endif

#ifdef __CRAYXE
    A1D_Acc_lock = dmapp_sheap_malloc( sizeof(int64_t) );
#endif

    A1D_Allreduce_issame64((size_t)A1D_Acc_lock, &sheapflag);
    assert(sheapflag==1);

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_Initialize() \n");
#endif

    return(0);
}

int A1D_Finalize()
{
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_Finalize() \n");
#endif

    A1D_Print_stats();

#ifdef FLUSH_IMPLEMENTED
    free(A1D_Put_flush_list);
#endif

    /* barrier so that no one is able to access remote memregions after they are destroyed */
    A1D_Barrier();

#ifdef __CRAYXE
    /* shut down DMAPP */
    dmapp_status = dmapp_finalize();
    assert(dmapp_status==DMAPP_RC_SUCCESS);
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_Finalize() \n");
#endif

    return(0);
}

/***************************************************
 *
 * global shared memory allocation
 *
 ***************************************************/

int A1D_Allocate_shared(void * ptrs[], int bytes)
{
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
#endif
    void *  tmp_ptr       = NULL;
    int     max_bytes     = 0;

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_Allocate_shared(void* ptrs[], int bytes) \n");
#endif

    A1D_Barrier();

#ifdef __CRAYXE
    A1D_Allreduce_max32( bytes, &max_bytes );

    /* allocate memory from symmetric heap */
    tmp_ptr = dmapp_sheap_malloc( (size_t)max_bytes );
    assert(dmapp_status==DMAPP_RC_SUCCESS);
#endif

    /* allgather addresses into pointer vector */
    A1D_Allgather( &tmp_ptr, ptrs, sizeof(void*) );

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_Allocate_shared(void* ptrs[], int bytes) \n");
#endif

    return(0);
}

void A1D_Free_shared(void * ptr)
{
#ifdef __CRAYXE
    dmapp_return_t dmapp_status = DMAPP_RC_SUCCESS;
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_Free_shared(void* ptr) \n");
#endif

    /* barrier so that no one tries to access memory which is no longer allocated
     * and to ensure that the user calls this function collectively */
    A1D_Barrier();

#ifdef __CRAYXE
    if (ptr != NULL)
    {
        dmapp_sheap_free(ptr);
        assert(dmapp_status==DMAPP_RC_SUCCESS);
    }
    else
    {
        fprintf(stderr, "You tried to free a NULL pointer.  Please check your code. \n");
        fflush(stderr);
    }
#endif

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_Free_shared(void* ptr) \n");
#endif

    return;
}

/***************************************************
 *
 * local memory allocation
 *
 ***************************************************/

void * A1D_Allocate_local(int bytes)
{
    void * tmp;

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_Allocate_local(void ** ptr, int bytes) \n");
#endif

    if (bytes>0)
    {
        tmp = calloc(bytes,1);
        assert( tmp != NULL );
    }
    else
    {
        if (bytes<0)
        {
            fprintf(stderr, "You requested %d bytes.  What kind of computer do you think I am? \n",bytes);
            fflush(stderr);
        }
        tmp = NULL;
    }

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_Allocate_local(void ** ptr, int bytes) \n");
#endif

    return tmp;
}

void A1D_Free_local(void * ptr)
{
#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"entering A1D_Free_local(void* ptr) \n");
#endif

    if (ptr != NULL)
    {
        free(ptr);
    }
    else
    {
        fprintf(stderr, "You tried to free a NULL pointer.  Please check your code. \n");
        fflush(stderr);
    }

#ifdef DEBUG_FUNCTION_ENTER_EXIT
    fprintf(stderr,"exiting A1D_Free_local(void* ptr) \n");
#endif

    return;
}
