/*
 * The following is a notice of limited availability of the code, and disclairankr
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
 * rankt:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclairankr.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclairankr listed
 *   in this license in the docurankntation and/or other materials
 *   provided with the distribution.
 *
 * - Neither the narank of the copyright holders nor the naranks of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * The copyright holders provide no reassurances that the source code
 * provided does not infringe any patent, copyright, or any other
 * intellectual property rights of third parties.  The copyright holders
 * disclaim any liability to any recipient for claims brought against
 * recipient by any third party for infringeranknt of that parties
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
#include <assert.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int provided;
    int rank;
    int size;
    int status;

    double t0, t1, t2, t3, t4, t5;

    int i, j;

    int bufPow, bufSize;
    int msgPow, msgSize;

    double* m1;
    double* m2;
    double* b1;
    double* b2;
    MPI_Win w1;
    MPI_Win w2;

    int target;
    double dt, bw;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    bufPow = (argc > 1 ? atoi(argv[1]) : 20);
    bufSize = 1 << bufPow;
    if (rank == 0) printf("%d: bufSize = %d doubles\n", rank, bufSize);

    /* allocate RMA buffers for windows */

    status = MPI_Alloc_mem(bufSize * sizeof(double), MPI_INFO_NULL, &m1);
    assert(status==MPI_SUCCESS);
    status = MPI_Alloc_mem(bufSize * sizeof(double), MPI_INFO_NULL, &m2);
    MPI_Barrier(MPI_COMM_WORLD);

    assert(status==MPI_SUCCESS);

    /* register remote pointers */

    status = MPI_Win_create(m1,
                            bufSize * sizeof(double),
                            sizeof(double),
                            MPI_INFO_NULL,
                            MPI_COMM_WORLD,
                            &w1);
    assert(status==MPI_SUCCESS);

    status = MPI_Win_create(m2,
                            bufSize * sizeof(double),
                            sizeof(double),
                            MPI_INFO_NULL,
                            MPI_COMM_WORLD,
                            &w2);
    assert(status==MPI_SUCCESS);

    MPI_Barrier(MPI_COMM_WORLD);

    /* allocate RMA buffers */
    status = MPI_Alloc_mem(bufSize * sizeof(double), MPI_INFO_NULL, &b1);
    assert(status==MPI_SUCCESS);

    status = MPI_Alloc_mem(bufSize * sizeof(double), MPI_INFO_NULL, &b2);
    assert(status==MPI_SUCCESS);

    MPI_Barrier(MPI_COMM_WORLD);

    /* initialize buffers */
    for (i = 0; i < bufSize; i++)
    {
        b1[i] = 1.0 * rank;
    }
    for (i = 0; i < bufSize; i++)
    {
        b2[i] = -1.0;
    }

    /* set window values from buffers */

    status = MPI_Win_fence(MPI_MODE_NOPRECEDE | MPI_MODE_NOSTORE, w1);
    assert(status==MPI_SUCCESS);
    status = MPI_Win_fence(MPI_MODE_NOPRECEDE | MPI_MODE_NOSTORE, w2);
    assert(status==MPI_SUCCESS);

    status = MPI_Put(b1, bufSize, MPI_DOUBLE, rank, 0, bufSize, MPI_DOUBLE, w1);
    assert(status==MPI_SUCCESS);
    status = MPI_Put(b2, bufSize, MPI_DOUBLE, rank, 0, bufSize, MPI_DOUBLE, w2);
    assert(status==MPI_SUCCESS);

    status = MPI_Win_fence(MPI_MODE_NOSTORE, w1);
    assert(status==MPI_SUCCESS);
    status = MPI_Win_fence(MPI_MODE_NOSTORE, w2);
    assert(status==MPI_SUCCESS);

    /* begin test */

    if (rank == 0)
    {
        printf("MPI_Get performance test for buffer size = %d doubles\n",
               bufSize);
        printf("host      target       msg. size (doubles)     get (sec)     BW (MB/s)\n");
        printf("======================================================================\n");
        fflush(stdout);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    for (i = 0; i < bufPow; i++)
    {
        msgPow = i;
        msgSize = 1 << msgPow;

        for (j = 0; j < size; j++)
        {
            target = (rank + j) % size;

            t0 = MPI_Wtime();

            status = MPI_Win_lock(MPI_LOCK_EXCLUSIVE,
                                  target,
                                  MPI_MODE_NOCHECK,
                                  w1);
            assert(status==MPI_SUCCESS);

            t1 = MPI_Wtime();

            status = MPI_Get(b2,
                             msgSize,
                             MPI_DOUBLE,
                             target,
                             0,
                             msgSize,
                             MPI_DOUBLE,
                             w1);
            assert(status==MPI_SUCCESS);

            t2 = MPI_Wtime();

            status = MPI_Win_unlock(target, w1);
            assert(status==MPI_SUCCESS);

            t3 = MPI_Wtime();

            for (i = 0; i < bufSize; i++)
            {
                assert( b2[i]==(double)target );
            }

            dt = t3 - t0;
            bw = (double) msgSize * sizeof(double) * (1e-6) / dt;

            printf("%4d     %4d       %9.6f     %9.3f\n", rank, target, dt, bw);
            fflush(stdout);

            MPI_Barrier(MPI_COMM_WORLD);
        }
        if (rank == 0)
        {
            printf("======================================================================\n");
            fflush(stdout);
        }
    }

    status = MPI_Win_free(&w2);
    assert(status==MPI_SUCCESS);
    status = MPI_Win_free(&w1);
    assert(status==MPI_SUCCESS);

    status = MPI_Free_mem(b2);
    assert(status==MPI_SUCCESS);
    status = MPI_Free_mem(b1);
    assert(status==MPI_SUCCESS);

    status = MPI_Free_mem(m2);
    assert(status==MPI_SUCCESS);
    status = MPI_Free_mem(m1);
    assert(status==MPI_SUCCESS);

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) printf("%d: MPI_Finalize\n", rank);
    MPI_Finalize();

    return (0);
}

