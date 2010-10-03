/*
 * The following is a notice of limited availability of the code, and disclaimer
 * which must be included in the prologue of the code and in all source listings
 * of the code.
 *
 * Copyright (c) 2010  Argonne Leadership Computing Facility, Argonne National
 * Laboratory
 *
 * Permission is hereby granted to use, reproduce, prepare derivative works, and
 * to redistribute to others.
 *
 *
 *                          LICENSE
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer listed
 *   in this license in the documentation and/or other materials
 *   provided with the distribution.
 *
 * - Neither the name of the copyright holders nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
    assert(provided==MPI_THREAD_SINGLE);

    int me;
    int nproc;
    MPI_Comm_rank(MPI_COMM_WORLD,&me);
    MPI_Comm_size(MPI_COMM_WORLD,&nproc);

    int status;
    double t0,t1,t2,t3,t4,t5;
    double tt0,tt1,tt2,tt3,tt4;

    int bufSize = ( argc>1 ? atoi(argv[1]) : 1000000 );
    if (me==0) printf("%d: bufSize = %d doubles\n",me,bufSize);

    /* allocate RMA buffers for windows */
    double* m1;
    double* m2;
    status = MPI_Alloc_mem(bufSize * sizeof(double), MPI_INFO_NULL, &m1);
    status = MPI_Alloc_mem(bufSize * sizeof(double), MPI_INFO_NULL, &m2);

    /* register remote pointers */
    MPI_Win w1;
    MPI_Win w2;
    status = MPI_Win_create(m1, bufSize * sizeof(double), sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &w1);
    status = MPI_Win_create(m2, bufSize * sizeof(double), sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &w2);
    MPI_Barrier(MPI_COMM_WORLD);

    /* allocate RMA buffers */
    double* b1;
    double* b2;
    status = MPI_Alloc_mem(bufSize * sizeof(double), MPI_INFO_NULL, &b1);
    status = MPI_Alloc_mem(bufSize * sizeof(double), MPI_INFO_NULL, &b2);

    /* initialize buffers */
    int i;
    for (i=0;i<bufSize;i++) b1[i]=1.0*me;
    for (i=0;i<bufSize;i++) b2[i]=-1.0;

    status = MPI_Win_fence( MPI_MODE_NOPRECEDE | MPI_MODE_NOSTORE , w1 );
    status = MPI_Win_fence( MPI_MODE_NOPRECEDE | MPI_MODE_NOSTORE , w2);
    status = MPI_Put(b1, bufSize, MPI_DOUBLE, me, 0, bufSize, MPI_DOUBLE, w1);
    status = MPI_Put(b2, bufSize, MPI_DOUBLE, me, 0, bufSize, MPI_DOUBLE, w2);
    status = MPI_Win_fence( MPI_MODE_NOSTORE , w1);
    status = MPI_Win_fence( MPI_MODE_NOSTORE , w2);

    int target;
    int j;
    double dt,bw;
    MPI_Barrier(MPI_COMM_WORLD);
    if (me==0){
        printf("MPI_Get performance test for buffer size = %d doubles\n",bufSize);
        printf("  jump    host   target       get (s)       BW (MB/s)\n");
        printf("===========================================================\n");
        fflush(stdout);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for (j=0;j<nproc;j++){
        target = (me+j) % nproc;
        MPI_Barrier(MPI_COMM_WORLD);
        t0 = MPI_Wtime();
        status = MPI_Win_lock(MPI_LOCK_EXCLUSIVE, target, MPI_MODE_NOCHECK, w1);
        t1 = MPI_Wtime();
        status = MPI_Get(b2, bufSize, MPI_DOUBLE, target, 0, bufSize, MPI_DOUBLE, w1);
        t2 = MPI_Wtime();
        status = MPI_Win_unlock(target, w1);
        t3 = MPI_Wtime();
        for (i=0;i<bufSize;i++) assert( b2[i]==(1.0*target) );
        dt = t3 - t0;
        bw = (double)bufSize*sizeof(double)*(1e-6)/dt;
        printf("%4d     %4d     %4d       %9.6f     %9.3f\n",j,me,target,dt,bw);
        fflush(stdout);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    status = MPI_Win_free(&w2);
    status = MPI_Win_free(&w1);

    status = MPI_Free_mem(b2);
    status = MPI_Free_mem(b1);

    status = MPI_Free_mem(m2);
    status = MPI_Free_mem(m1);

    MPI_Barrier(MPI_COMM_WORLD);

    if (me==0) printf("%d: MPI_Finalize\n",me);
    MPI_Finalize();

    return(0);
}



