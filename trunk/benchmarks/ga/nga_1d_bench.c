/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  Copyright (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 *
 * nga_1d_bench.c
 *
 *  Created on: Sep 20, 2010
 *      Author: jeff
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "macdecls.h"
#include "sndrcv.h"
#include "ga.h"
#include "mpi.h"

#ifdef USE_ARMCI
#include "../armci/src/armci.h"
#else
#include "$(A1_INSTALL)/include/armci.h"
#endif

int main(int argc, char **argv)
{
    int desired = MPI_THREAD_MULTIPLE;
    int provided;
    MPI_Init_thread(&argc, &argv, desired, &provided);

    int mpi_nproc, mpi_me;

    MPI_Comm_size(MPI_COMM_WORLD,&mpi_nproc);
    MPI_Comm_rank(MPI_COMM_WORLD,&mpi_me);

    GA_Initialize();
    // This is unnecessary for basic GA functionality
    //MA_init(MT_DBL, 128*1024*1024, 32*1024*1024);

    int ga_nproc = GA_Nnodes();
    int ga_me    = GA_Nodeid();

    dims[0] = pow(2,rank);
    chunk[0] = -1;

    int g_a = GA_Create_handle();
    GA_Set_pgroup(g_a,GA_Pgroup_get_world());

    int ndim = 2;
    int dims[ndim];
    int chunk[ndim];

    GA_Set_array_name(g_a,"A");
    GA_Set_data(g_a,ndim,dims,MT_DBL);
    GA_Set_chunk(g_a,chunk);

    int status = GA_Allocate(g_a);
    if(0 != status){ GA_Error("GA_Allocate failed",0); };

    GA_Zero(g_a);
    GA_Sync();
    double val = -1.0;
    GA_Fill(g_a,&val);
    GA_Sync();

    GA_Print_distribution(g_a);

    GA_Destroy(g_a);

    GA_Terminate();
    MPI_Finalize();

    return(0);
}
