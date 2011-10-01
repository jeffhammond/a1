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
#include <assert.h>
#include <mpi.h>
#include <dcmf.h>

DCMF_Protocol_t get_protocol;

void cb_done(void * clientdata, DCMF_Error_t * error)
{
    --(*((uint32_t *) clientdata));
}

void blocking_get(int remote, int bytes,
                 DCMF_Memregion_t origin_memregion, size_t origin_offset,
                 DCMF_Memregion_t remote_memregion, size_t remote_offset)
{
    DCMF_Result dcmf_result;
    DCMF_Request_t request;
    DCMF_Callback_t done_callback;
    volatile int active;

    DCMF_CriticalSection_enter(0);

    done_callback.function = cb_done;
    done_callback.clientdata = (void *) &active;

    active = 1;

    dcmf_result = DCMF_Get(&get_protocol,
                           &request,
                           done_callback,
                           //DCMF_RELAXED_CONSISTENCY,
                           DCMF_SEQUENTIAL_CONSISTENCY,
                           remote,
                           bytes,
                           origin_memregion,
                           remote_memregion,
                           origin_offset,
                           remote_offset);
    assert(dcmf_result==DCMF_SUCCESS);

    A1DI_Conditional_advance(active > 0);

    DCMF_CriticalSection_exit(0);

    return;
}

int main(int argc, char *argv[])
{
    int rank, size;
    int provided;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    {
        DCMF_Result dcmf_result;
        DCMF_Get_Configuration_t conf;

        conf.protocol = DCMF_DEFAULT_GET_PROTOCOL;
        conf.network = DCMF_TORUS_NETWORK;

        dcmf_result = DCMF_Get_register(&get_protocol, &conf);
        assert(dcmf_result==DCMF_SUCCESS);
    }

    int max_count   = ( argc > 1 ? atoi(argv[1]) : 1000000 );
    int repetitions = ( argc > 2 ? atoi(argv[2]) : 10 );

    if ( rank == 0 ) printf( "size = %d max_count = %d bytes\n", size, max_count );

    for ( int count=1 ; count < max_count ; count*=2 )
    {
        DCMF_Result dcmf_result;

        int bytes = count * sizeof(int);
        int bytes_out;

        int * shared_buffer = malloc( bytes );
        assert( shared_buffer!=NULL );
        for ( int i = 0 ; i < count ; i++ ) shared_buffer[i] = rank;

        DCMF_Memregion_t shared_memregion;
        DCMF_CriticalSection_enter(0);
        dcmf_result = DCMF_Memregion_create( &shared_memregion, &bytes_out, bytes, shared_buffer, 0 );
        DCMF_CriticalSection_exit(0);
        assert( dcmf_result==DCMF_SUCCESS && bytes_out==bytes );

        DCMF_Memregion_t * memregion_list = (DCMF_Memregion_t *) malloc( size * sizeof(DCMF_Memregion_t) );
        assert( memregion_list!=NULL );

        mpi_status = MPI_Barrier(MPI_COMM_WORLD);
        assert(mpi_status==0);

        mpi_status = MPI_Allgather(&shared_memregion, sizeof(DCMF_Memregion_t), MPI_BYTE,
                                   memregion_list,    sizeof(DCMF_Memregion_t), MPI_BYTE,
                                   MPI_COMM_WORLD);
        assert(mpi_status==0);

        mpi_status = MPI_Barrier(MPI_COMM_WORLD);
        assert(mpi_status==0);

        if (rank == 0)
        {
            int * local_buffer = malloc( bytes );
            assert( local_buffer!=NULL );
            for ( int i = 0 ; i < count ; i++ ) local_buffer[i] = -1;

            DCMF_Memregion_t local_memregion;
            DCMF_CriticalSection_enter(0);
            dcmf_result = DCMF_Memregion_create( &local_memregion, &bytes_out, bytes, local_buffer, 0 );
            DCMF_CriticalSection_exit(0);
            assert( dcmf_result==DCMF_SUCCESS && bytes_out==bytes );

            for (int target=1; target<size; target++)
            {
                double t0, t1, dt, bw;

                t0 = DCMF_Timer();
                for (int r=0; r<repetitions; r++)
                {
                    DCMF_Result dcmf_result;
                    DCMF_Request_t request;
                    DCMF_Callback_t done_callback;
                    volatile int active;

                    DCMF_CriticalSection_enter(0);

                    done_callback.function = cb_done;
                    done_callback.clientdata = (void *) &active;

                    active = 1;

                    dcmf_result = DCMF_Get(&get_protocol,
                                           &request,
                                           done_callback,
                                           DCMF_RELAXED_CONSISTENCY,
                                           //DCMF_SEQUENTIAL_CONSISTENCY,
                                           target,
                                           bytes,
                                           local_memregion,
                                           memregion_list[target],
                                           0,
                                           0);

                    while (active > 0) DCMF_Messager_advance();

                    DCMF_CriticalSection_exit(0);

                    assert(dcmf_result==DCMF_SUCCESS);
                }
                t1 = DCMF_Timer();

                for ( int i = 0 ; i < count ; i++ ) assert( local_buffer[i] == target );
                //for (int i = 0; i < w; i++)
                //    if ( shared_buffer[i] != i ) printf("rank %d shared_buffer[%d] = %lf \n", target, i, shared_buffer[i] );

                dt =  ( t1 - t0 ) / repetitions;
                bw = (double) bytes / dt / 1000000;
                printf("DCMF_Get of from rank %d to rank %d of %d bytes took %lf seconds (%lf MB/s)\n",
                       t, 0, bytes, dt, bw);
                fflush(stdout);
            }

            DCMF_CriticalSection_enter(0);
            dcmf_result = DCMF_Memregion_destroy(&local_memregion);
            DCMF_CriticalSection_exit(0);
            assert(dcmf_result==DCMF_SUCCESS);

            free(local_buffer);
        }

        free(memregion_list);

        DCMF_CriticalSection_enter(0);
        dcmf_result = DCMF_Memregion_destroy(&shared_memregion);
        DCMF_CriticalSection_exit(0);
        assert(dcmf_result==DCMF_SUCCESS);

        free(shared_buffer);
    }

    MPI_Finalize();

    return 0;
}
