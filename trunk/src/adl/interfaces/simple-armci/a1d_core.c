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

#include "a1d_core.h"
#include "a1d_stats.h"

int mpi_rank;
int mpi_size;

MPI_Comm A1D_COMM_WORLD;
DCMF_Memregion_t* A1D_Memregion_list;
void** A1D_Baseptr_list;

int A1D_Initialize()
{
    int mpi_initialized, mpi_provided;
    int mpi_status;
    int i;
    size_t bytes_in, bytes_out;
    DCMF_Result dcmf_result;
    DCMF_Configure_t dcmf_config;
    DCMF_Memregion_t local_memregion;

    /***************************************************
     *
     * configure MPI
     *
     ***************************************************/

    /* MPI has to be initialized for this implementation to work */
    MPI_Initialized(&mpi_initialized);
    assert(mpi_initialized==1);

    /* MPI has to be thread-safe so that DCMF doesn't explode */
    MPI_Query_thread(&mpi_provided);
    assert(mpi_provided==MPI_THREAD_MULTIPLE);

    /* have to use our own communicator for collectives to be proper */
    mpi_status = MPI_Comm_dup(MPI_COMM_WORLD,&A1D_COMM_WORLD);
    assert(mpi_status==0);

    /*  */
    mpi_status = MPI_Comm_size(A1D_COMM_WORLD,&mpi_size);
    assert(mpi_status==0);

    /*  */
    mpi_status = MPI_Comm_rank(A1D_COMM_WORLD,&mpi_rank);
    assert(mpi_status==0);

    /* barrier before DCMF_Messager_configure to make sure MPI is ready everywhere */
    mpi_status = MPI_Barrier(A1D_COMM_WORLD);
    assert(mpi_status==0);

    /***************************************************
     *
     * configure DCMF
     *
     ***************************************************/

    /* to be safe, but perhaps not necessary */
    dcmf_config.thread_level = DCMF_THREAD_MULTIPLE;
    /* this implementation of ARMCI for BGP is going to use interrupts exclusively */
    dcmf_config.interrupts = DCMF_INTERRUPTS_ON;

    /* reconfigure DCMF with interrupts on */
    DCMF_CriticalSection_enter(0);
    dcmf_result = DCMF_Messager_configure(&dcmf_config, &dcmf_config);
    assert(dcmf_result==DCMF_SUCCESS);
    DCMF_CriticalSection_exit(0);

    /* barrier after DCMF_Messager_configure to make sure everyone has the new DCMF config */
    mpi_status = MPI_Barrier(A1D_COMM_WORLD);
    assert(mpi_status==0);

    /***************************************************
     *
     * setup DCMF memregions
     *
     ***************************************************/

    /* allocate memregion list */
    A1D_Memregion_list = malloc( mpi_size * sizeof(DCMF_Memregion_t) );
    assert(A1D_Memregion_list != NULL);

    /* allocate base pointer list */
    A1D_Baseptr_list = malloc( mpi_size * sizeof(void*) );
    assert(A1D_Memregion_list != NULL);

    /* create memregions */
    bytes_in = -1;
    DCMF_CriticalSection_enter(0);
    dcmf_result = DCMF_Memregion_create(&local_memregion,&bytes_out,bytes_in,NULL,0);
    DCMF_CriticalSection_exit(0);
    assert(dcmf_result==DCMF_SUCCESS);

    /* exchange memregions */
    mpi_status = MPI_Allgather(&local_memregion,sizeof(DCMF_Memregion_t),MPI_BYTE,
                               A1D_Memregion_list,sizeof(DCMF_Memregion_t),MPI_BYTE,
                               A1D_COMM_WORLD);
    assert(mpi_status==0);

    /* destroy temporary local memregion */
    DCMF_CriticalSection_enter(0);
    dcmf_result = DCMF_Memregion_destroy(&local_memregion);
    DCMF_CriticalSection_exit(0);
    assert(dcmf_result==DCMF_SUCCESS);

    /* check for valid memregions */
    DCMF_CriticalSection_enter(0);
    for (i = 0; i < mpi_size; i++)
    {
        dcmf_result = DCMF_Memregion_query(&A1D_Memregion_list[i],
                                      &bytes_out,
                                      (void **) &A1D_Baseptr_list[i]);
        assert(dcmf_result==DCMF_SUCCESS);
    }
    DCMF_CriticalSection_exit(0);

    /***************************************************
     *
     *
     *
     ***************************************************/

    return(0);
}

int A1D_Finalize()
{
    int mpi_status;
    int i;
    DCMF_Result dcmf_result;

    A1D_Print_stats();

    /* barrier so that no one is able to access remote memregions after they are destroyed */
    mpi_status = MPI_Barrier(A1D_COMM_WORLD);
    assert(mpi_status==0);

    /* destroy all memregions - probably unnecessary */
    DCMF_CriticalSection_enter(0);
    for (i = 0; i < mpi_size; i++)
    {
        dcmf_result = DCMF_Memregion_destroy(&A1D_Memregion_list[i]);
        assert(dcmf_result==DCMF_SUCCESS);
    }
    DCMF_CriticalSection_exit(0);

    /* free memregion list */
    free(A1D_Memregion_list);

    /* free base pointer list */
    free(A1D_Baseptr_list);

    mpi_status = MPI_Comm_free(&A1D_COMM_WORLD);
    assert(mpi_status==0);

    return(0);
}

/***************************************************
 *
 * local memory allocation
 *
 ***************************************************/

int A1D_Allocate_local(void** ptr, int bytes)
{
    void* tmp;
    tmp = malloc((size_t)bytes);

    /* just to be safe */
    if (bytes == 0) tmp = NULL;

    /* accept null pointer if no memory requested */
    assert( (tmp!=NULL) || (bytes==0) );

    ptr = &tmp;

    return(0);
}

void A1D_Free_local(void* ptr)
{
    if (ptr != NULL) free(ptr);
    return;
}

/***************************************************
 *
 * global shared memory allocation
 *
 ***************************************************/

int A1D_Allocate_shared(void* ptrs[], int bytes)
{
    int mpi_status;
    void* tmp_ptr;

    A1D_Allocate_local(&tmp_ptr, bytes);

    mpi_status = MPI_Allgather(&tmp_ptr,sizeof(void*),MPI_BYTE,
                               ptrs,sizeof(void*),MPI_BYTE,
                               A1D_COMM_WORLD);
    assert(mpi_status==0);

    return(0);
}


void A1D_Free_shared(void* ptr)
{
    A1D_Free_local(ptr);
    return;
}

/***************************************************
 *
 * communicator-based shared memory allocation
 *
 ***************************************************/

int A1D_Create_window(const MPI_Comm comm, int bytes, A1D_Window_t* window)
{
    int mpi_status;
    int mpi_size;
    int mpi_rank;
    void* tmp_ptr;
    MPI_Comm newcomm;

    /* save (dup) the communicator into the window object */
    mpi_status = MPI_Comm_dup(comm,&newcomm);
    assert(mpi_status==0);

    window->comm = newcomm;

    /* need array sizeof(comm) for now */
    mpi_status = MPI_Comm_size(window->comm,&mpi_size);
    assert(mpi_status==0);

    /* my rank in this communicator */
    mpi_status = MPI_Comm_rank(window->comm,&mpi_rank);
    assert(mpi_status==0);

    /* allocate list of base pointers for this window */
    window->addr_list = malloc( mpi_size * sizeof(void*) );
    assert(window->addr_list != NULL);

    /* allocate local memory for this window */
    A1D_Allocate_local(&tmp_ptr, bytes);

    /* exchange base pointers */
    mpi_status = MPI_Allgather(&tmp_ptr,sizeof(void*),MPI_BYTE,
                               window->addr_list,sizeof(void*),MPI_BYTE,
                               window->comm);
    assert(mpi_status==0);

#ifndef NO_WINDOW_BOUNDS_CHECKING
    /* allocate list of sizes */
    window->addr_list = malloc(mpi_size*sizeof(int));
    assert(window->size_list != NULL);

    /* exchange sizes pointers */
    mpi_status = MPI_Allgather(&bytes,sizeof(int),MPI_BYTE,
                               window->size_list,sizeof(int),MPI_BYTE,
                               window->comm);
    assert(mpi_status==0);

#endif

    return(0);
}

int A1D_Destroy_window(A1D_Window_t* window)
{
    int mpi_status;
    int mpi_rank;

    /* barrier so that no one is able to access remote window memory after it is free */
    mpi_status = MPI_Barrier(window->comm);
    assert(mpi_status==0);

    /* my rank in this communicator */
    mpi_status = MPI_Comm_rank(window->comm,&mpi_rank);
    assert(mpi_status==0);

    /* free the local memory */
    A1D_Free_local(window->addr_list[mpi_rank]);

    /* free the list of base pointers */
    free(window->addr_list);

#ifndef NO_WINDOW_BOUNDS_CHECKING
    /* free list of sizes */
    free(window->size_list);
#endif

    /* free the communicator */
    mpi_status = MPI_Comm_free(&(window->comm));
    assert(mpi_status==0);

    return(0);
}

