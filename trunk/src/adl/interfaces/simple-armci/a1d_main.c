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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>
#include <pthread.h>

//#include <bpcore/bgp_atomic_ops.h>
//#include <spi/bgp_SPI.h>

#include <dcmf.h>
#include <dcmf_globalcollectives.h>
#include <dcmf_collectives.h>

#include <mpi.h>

#include "a1d_api.h"

MPI_Comm A1D_COMM_WORLD;

void A1D_Startup()
{
    int mpi_initialized, mpi_provided;
    int mpi_status;
    DCMF_Result dcmf_result;
    DCMF_Configure_t dcmf_config;

    /* MPI has to be initialized for this implementation to work */
    MPI_Initialized(&mpi_initialized);
    assert(mpi_initialized==1);

    /* MPI has to be thread-safe so that DCMF doesn't explode */
    MPI_Query_thread(&mpi_provided);
    assert(mpi_provided==MPI_THREAD_MULTIPLE);

    /* have to use our own communicator for collectives to be proper */
    mpi_status = MPI_Comm_dup(MPI_COMM_WORLD,A1D_COMM_WORLD);
    assert(mpi_status==0);

    /* barrier before DCMF_Messager_configure to make sure MPI is ready everywhere */
    mpi_status = MPI_Barrier(A1D_COMM_WORLD);
    assert(mpi_status==0);

    /* to be safe, but perhaps not necessary */
    dcmf_config.thread_level = DCMF_THREAD_MULTIPLE;
    /* this implementation of ARMCI for BGP is going to use interrupts exclusively */
    dcmf_config.interrupts = DCMF_INTERRUPTS_ON;

    /* reconfigure DCMF with interrupts on */
    DCMF_CriticalSection_enter(0);
    dcmf_result = DCMF_Messager_configure(&config, &config);
    assert(dcmf_result==DCMF_SUCCESS);
    DCMF_CriticalSection_exit(0);

    /* barrier after DCMF_Messager_configure to make sure everyone has the new DCMF config */
    mpi_status = MPI_Barrier(A1D_COMM_WORLD);
    assert(mpi_status==0);

    return(0);
}

void* A1D_Allocate_local(long bytes)
{
    void* ptr;
    ptr = malloc((size_t)bytes);
    /* accept null pointer if no memory requested */
    assert( (ptr!=NULL) || (bytes==0) );
    return ptr;
}

int A1D_Allocate_shared(void* ptrs[], long bytes)
{
    long rank;
    void* tmp_ptr;
    int mpi_status;

    rank = (long) DCMF_Messager_rank();

    tmp_ptr = A1D_Allocate_local(bytes);
    /* set to null just */
    if (bytes == 0) tmp_ptr = NULL;

    mpi_status = MPI_Allgather(tmp_ptr,sizeof(void*),MPI_BYTE,
                               ptrs,sizeof(void*),MPI_BYTE,
                               A1D_COMM_WORLD);
    assert(mpi_status==0);

    return;
}

void A1D_Free_local(void* ptr)
{
    free(ptr);
    return;
}

void A1D_Free_shared(void* ptr)
{
    if (ptr!=NULL) A1D_Free_local(ptr);
    return;
}

