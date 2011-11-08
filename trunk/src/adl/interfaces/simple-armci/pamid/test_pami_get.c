#ifdef __bgq__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <mpi.h>
#include <pami.h>

static size_t world_size, world_rank = -1;

#define PRINT_SUCCESS 0

#define TEST_ASSERT(c,m) \
        do { \
        if (!(c)) { \
                    printf(m" FAILED on rank %ld\n", world_rank); \
                    fflush(stdout); \
                  } \
        else if (PRINT_SUCCESS) { \
                    printf(m" SUCCEEDED on rank %ld\n", world_rank); \
                    fflush(stdout); \
                  } \
        sleep(1); \
        /*assert(c);*/ \
        } \
        while(0);

void cb_done(pami_context_t context, void * cookie, pami_result_t result)
{
  int * active = (int *) cookie;

  (*active)--;

  return;
}

int main(int argc, char *argv[])
{
    int provided;
    int mpi_status;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    assert( size > 1 );

    /**********************************************************************/

    pami_result_t pami_result = PAMI_SUCCESS;

    /* initialize the client */
    char * clientname = "A1D";
    pami_client_t a1d_client;
    pami_result = PAMI_Client_create(clientname, &a1d_client, NULL, 0);
    TEST_ASSERT(pami_result == PAMI_SUCCESS,"PAMI_Client_create");

    /* query properties of the client */
    pami_configuration_t config;
    size_t num_contexts;

    config.name = PAMI_CLIENT_NUM_TASKS;
    pami_result = PAMI_Client_query( &a1d_client, &config,1);
    TEST_ASSERT(pami_result == PAMI_SUCCESS,"PAMI_Client_query");
    world_size = config.value.intval;

    config.name = PAMI_CLIENT_TASK_ID;
    pami_result = PAMI_Client_query( &a1d_client, &config,1);
    TEST_ASSERT(pami_result == PAMI_SUCCESS,"PAMI_Client_query");
    world_rank = config.value.intval;
    printf("hello world from rank %ld of %ld \n", world_rank, world_size );
    fflush(stdout);
    //sleep(1);

    config.name = PAMI_CLIENT_NUM_CONTEXTS;
    pami_result = PAMI_Client_query( &a1d_client, &config, 1);
    TEST_ASSERT(pami_result == PAMI_SUCCESS,"PAMI_Client_query");
    num_contexts = config.value.intval;

    /* initialize the contexts */
    pami_context_t * contexts;
    contexts = (pami_context_t *) malloc( num_contexts * sizeof(pami_context_t) );
    assert( contexts!=NULL );

    result = PAMI_Context_createv( client, &a1d_client, 0, contexts, num_contexts );
    TEST_ASSERT(result == PAMI_SUCCESS,"PAMI_Context_createv");

    printf("%ld contexts were created by rank %ld \n", num_contexts, world_rank );
    fflush(stdout);
    //sleep(1);

    /**********************************************************************/

    size_t min_count   = (size_t) ( argc > 1 ? atoi(argv[1]) : 1    );
    size_t max_count   = (size_t) ( argc > 2 ? atoi(argv[2]) : 1024 );
    size_t repetitions = (size_t) ( argc > 3 ? atoi(argv[3]) : 10   );

    if ( rank == 0 ) printf( "size = %ld max_count = %ld bytes \n", world_size, max_count );

    mpi_status = MPI_Barrier(MPI_COMM_WORLD);
    assert(mpi_status==0);

    //for ( int count = min_count ; count < max_count ; count*=2 )
    for ( size_t count = min_count ; count < max_count ; count++ )
    {
        size_t bytes = count * sizeof(char);
        size_t bytes_out;

        char * shared_buffer = malloc( bytes );
        assert( shared_buffer!=NULL );

        char correct = (char)( ((int)'0') + (rank%10) );
        memset( shared_buffer[i], correct, count );

        char ** shared_buffer_list = malloc( world_size * sizeof(char *) );
        assert( shared_buffer_list!=NULL );

        mpi_status = MPI_Allgather(&shared_buffer,     sizeof(char *), MPI_BYTE,
                                   shared_buffer_list, sizeof(char *), MPI_BYTE,
                                   MPI_COMM_WORLD);
        assert(mpi_status==0);

        mpi_status = MPI_Barrier(MPI_COMM_WORLD);
        assert(mpi_status==0);

        if (rank == 0)
        {
            char * local_buffer = malloc( bytes );
            assert( local_buffer!=NULL );

            for ( size_t target_task = 1 ; target_task < world_size ; target_task++ )
            {
                double t0, t1, dt, bw;

                for ( size_t i = 0 ; i < count ; i++ ) local_buffer[i] = 'x';

                volatile int active = 0;

                size_t context_offset = 0;
                pami_get_simple_t parameters;

                pami_result_t = PAMI_Context_lock(contexts[0]);
                TEST_ASSERT(pami_result == PAMI_SUCCESS,"PAMI_Context_lock");

                pami_result = PAMI_Endpoint_create( a1d_client, target_task, context_offset, &parameters.rma.dest );
                TEST_ASSERT(pami_result == PAMI_SUCCESS,"PAMI_Endpoint_create");

                parameters.rma.hints   = null_send_hint;
                parameters.rma.bytes   = count;
                parameters.rma.cookie  = (void *) &active;
                parameters.rma.done_fn = cb_done;
                parameters.addr.local  = local_buffer;
                parameters.addr.remote = shared_buffer_list[target_task];

                t0 = PAMI_Wtime(a1d_client);
                for ( size_t r = 0 ; r < repetitions ; r++ )
                {
                    active++;

                    pami_result = PAMI_Get(contexts[context_offset], &parameters);
                    TEST_ASSERT(pami_result == PAMI_SUCCESS,"PAMI_Get");

                    while (active > 0) pami_result = PAMI_Context_advance(contexts[0], 1000);
                    TEST_ASSERT(pami_result == PAMI_SUCCESS,"PAMI_Context_advance");
                }
                t1 = PAMI_Wtime(a1d_client);

                pami_result_t = PAMI_Context_unlock(contexts[0]);
                TEST_ASSERT(pami_result == PAMI_SUCCESS,"PAMI_Context_unlock");

                char correct = (char)( ((int)'0') + (target%10) );

                int errors = 0;
                for ( size_t i = 0 ; i < count ; i++ )
                    if ( local_buffer[i] != correct ) errors++;

                if ( errors > 0 )
                    for ( size_t i = 0 ; i < count ; i++ )
                        printf("%d: target %d local_buffer[%d] = %c (expected = %c) \n", rank, target, i, local_buffer[i], correct );

                fflush(stdout);

                dt =  ( t1 - t0 ) / repetitions;
                bw = 1e-6 * bytes / dt;
                printf("%ld: PAMI_Get of from rank %d to rank %ld of %ld bytes took %lf seconds (%lf MB/s)\n",
                       world_rank, target, 0, bytes, dt, bw);
                fflush(stdout);
            }
            free(local_buffer);
        }
        mpi_status = MPI_Barrier(MPI_COMM_WORLD);
        assert(mpi_status==0);

        free(shared_buffer_list);
        free(shared_buffer);
    }

    mpi_status = MPI_Barrier(MPI_COMM_WORLD);
    assert(mpi_status==0);

    MPI_Finalize();

    return(0);
}
#else

int main()
{
    return(1);
}

#endif
