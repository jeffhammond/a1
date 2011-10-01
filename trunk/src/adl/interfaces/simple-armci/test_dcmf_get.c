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

int main(int argc, char *argv[])
{
    int rank, size;
    int provided;
    int mpi_status;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    assert( size > 1 );

    {
        DCMF_Result dcmf_result;
        DCMF_Get_Configuration_t conf;

        conf.protocol = DCMF_DEFAULT_GET_PROTOCOL;
        conf.network = DCMF_TORUS_NETWORK;

        dcmf_result = DCMF_Get_register(&get_protocol, &conf);
        assert(dcmf_result==DCMF_SUCCESS);
    }

    int max_count   = ( argc > 1 ? atoi(argv[1]) : 20 );
    int repetitions = ( argc > 2 ? atoi(argv[2]) : 10 );

    if ( rank == 0 ) printf( "size = %d max_count = %d bytes \n", size, max_count );

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

//        void ** baseptr_list = (void *) malloc( size * sizeof(void *) );
//        assert( baseptr_list!=NULL );
//        DCMF_CriticalSection_enter(0);
//        for (int i=0; i<size; i++)
//        {
//            dcmf_result = DCMF_Memregion_query( &memregion_list[i], &bytes_out, &baseptr_list[i] );
//            assert( dcmf_result==DCMF_SUCCESS );
//        }
//        DCMF_CriticalSection_exit(0);

        mpi_status = MPI_Barrier(MPI_COMM_WORLD);
        assert(mpi_status==0);

        if (rank == 0)
        {
            int * local_buffer = malloc( bytes );
            assert( local_buffer!=NULL );

            DCMF_Memregion_t local_memregion;
            DCMF_CriticalSection_enter(0);
            dcmf_result = DCMF_Memregion_create( &local_memregion, &bytes_out, bytes, local_buffer, 0 );
            DCMF_CriticalSection_exit(0);
            assert( dcmf_result==DCMF_SUCCESS && bytes_out==bytes );

            for (int remote=1; remote<size; remote++)
            {
                double t0, t1, dt, bw;

                for ( int i = 0 ; i < count ; i++ ) local_buffer[i] = -1;

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
                                           //DCMF_RELAXED_CONSISTENCY,
                                           DCMF_SEQUENTIAL_CONSISTENCY,
                                           remote,
                                           bytes,
                                           &memregion_list[remote],
                                           &local_memregion,
                                           0,
                                           0);

                    while (active > 0) DCMF_Messager_advance();

                    DCMF_CriticalSection_exit(0);

                    assert(dcmf_result==DCMF_SUCCESS);
                }
                t1 = DCMF_Timer();

                //for ( int i = 0 ; i < count ; i++ ) assert( local_buffer[i] == remote );
                for (int i = 0; i < count; i++)
                    if ( local_buffer[i] != remote ) printf("remote %d local_buffer[%d] = %d \n", remote, i, local_buffer[i] );
                fflush(stdout);

                dt =  ( t1 - t0 ) / repetitions;
                bw = (double) bytes / dt / 1000000;
                printf("DCMF_Get of from rank %d to rank %d of %d bytes took %lf seconds (%lf MB/s)\n",
                       remote, 0, bytes, dt, bw);
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

    mpi_status = MPI_Barrier(MPI_COMM_WORLD);
    assert(mpi_status==0);

    MPI_Finalize();

    return 0;
}
