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
#include <mpi.h>
#include <a1.h>

int main(int argc, char* argv[])
{
    int provided;
    int i, rank, size, msgsize, target;
    long bufsize;
    int **counter;
    int *complete;
    int increment;
    int counter_fetch;
    int counters_received;
    int t_start, t_stop, t_latency;
    int expected;
    int count;
    double t0,t1,tt,dt;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    count = ( argc > 1 ? atoi(argv[1]) : 1000000 ); 

    A1_Initialize(A1_THREAD_SINGLE);

    rank = A1_Process_id(A1_GROUP_WORLD);
    size = A1_Process_total(A1_GROUP_WORLD);

    counter = (int **) malloc(sizeof(int*) * size); 
    A1_Alloc_segment((void **) &(counter[rank]), sizeof(int)); 
    A1_Exchange_segments(A1_GROUP_WORLD, (void **) counter);

    complete = (int *) malloc(sizeof(int) * count);

    if (rank == 0)
    {
        printf("A1_RMW Test - in usec \n");
        fflush(stdout);
    }

    target = 0; 

    for(i=0; i<count; i++)
    {
       complete[i] = 0;
    } 
    if(rank == target) 
    { 
       *(counter[rank]) = 0;
    }
    increment = 1;
    counter_fetch = 0;
    counters_received = 0;

    MPI_Barrier(MPI_COMM_WORLD);
 
    if (rank>0)
    {
        while(counter_fetch < count)
        {  
            t0 = MPI_Wtime();
            A1_Rmw(target,
                   (void *) &increment,
                   (void *) &counter_fetch,
                   (void *) counter[target],
                   sizeof(int),
                   A1_FETCH_AND_ADD,
                   A1_INT32);
            t1 = MPI_Wtime();
            if (counter_fetch < count) 
            {
                complete[counter_fetch] = rank;
                counters_received++;
                tt += (t1-t0);
            }
        }
    }
    MPI_Allreduce(MPI_IN_PLACE,
                  complete,
                  count,
                  MPI_INT,
                  MPI_SUM,
                  MPI_COMM_WORLD);

    dt = (double)tt/counters_received;
    printf("process %d received %d counters in %f seconds (%f per call)\n", 
           rank, counters_received, tt, dt);
    fflush(stdout);

    if (0==rank)
    {
        printf("Checking for fairness...\n", rank);
        fflush(stdout);
        for(i=0; i<count; i++)
        {
            if (0==complete[i]) printf("counter value %d was not received by anyone!!! %d\n", i);
            else                printf("counter value %d was received by process %d\n", i, complete[i]);
        }
        fflush(stdout);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    A1_Release_segments(A1_GROUP_WORLD, counter[rank]);
    A1_Free_segment(counter[rank]);

    A1_Finalize();

    return 0;
}
