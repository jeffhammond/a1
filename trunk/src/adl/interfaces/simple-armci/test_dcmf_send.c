#ifdef __bgp__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <mpi.h>
#include <dcmf.h>

int posix_memalign(void **memptr, size_t alignment, size_t size); 

/***************************************************************/

static int rank = -1, size = -1;

/***************************************************************/

static int              recv_peer;
static size_t           recv_bytes;
static DCMF_Request_t * recv_request;
static void *           recv_buffer;
static volatile int    local_active;

/***************************************************************/

void local_completion_cb(void * clientdata, DCMF_Error_t * error)
{
    printf("%d: local_completion_cb \n", rank );
    fflush(stdout);

    local_active = 0;

    return;
}

/***************************************************************/

DCMF_Protocol_t control_proto;

void remote_completion_cb(void * clientdata, const DCMF_Control_t * info, size_t peer)
{
    printf("%d: remote_completion_cb peer=%d \n", rank, peer );
    fflush(stdout);

    return;
}

/***************************************************************/

void default_long_cleanup_cb(void * clientdata, DCMF_Error_t * error)
{
    size_t i;
    int peer = (int) clientdata;
    DCMF_Result dcmf_result;
    DCMF_Control_t info;
    char * char_buffer = (char*) recv_buffer;

    printf("%d: default_long_cleanup_cb peer=%d \n", rank, peer);
    fflush(stdout);

    for (i=0;i<recv_bytes;i++)
        printf("%d: recv_buffer[%d] = %c \n", rank, i, char_buffer[i]);

    fflush(stdout);

    free(recv_request);
    free(recv_buffer);

    dcmf_result =  DCMF_Control(&control_proto,
                                DCMF_SEQUENTIAL_CONSISTENCY,
                                peer,
                                &info);
    assert(dcmf_result==DCMF_SUCCESS);

    return;
}

void default_short_cb(void *clientdata,
                      const DCQuad *msginfo,
                      unsigned count,
                      size_t peer,
                      const char *src,
                      size_t bytes)
{
    size_t i;

    printf("%d: default_short_cb peer=%d count=%u \n", rank, peer, count);
    fflush(stdout);
    for (i=0;i<bytes;i++)
        printf("%d: src[%d] = %c \n", rank, i, src[i]);

    fflush(stdout);

    return;
}

DCMF_Request_t * default_long_cb(void *clientdata,
                                 const DCQuad *msginfo,
                                 unsigned count,
                                 size_t peer,
                                 size_t sndlen,
                                 size_t *rcvlen,
                                 char **rcvbuf,
                                 DCMF_Callback_t *cb_done)
{
    int rc = 0;
    printf("%d: default_long_cb peer=%d count=%u \n", rank, peer, count);
    fflush(stdout);

    rc = posix_memalign( (void**) &recv_request, 128, sizeof(DCMF_Request_t) );
    assert( (rc == 0) && (recv_request != NULL) );

    rc = posix_memalign( (void**) &recv_buffer, 128, sndlen );
    assert( (rc == 0) && (recv_buffer != NULL) );

    (*rcvlen) = sndlen;
    (*rcvbuf) = (char*) recv_buffer;

    recv_bytes = (*rcvlen);

    cb_done->function   = default_long_cleanup_cb;
    cb_done->clientdata = (void *) peer;

    return recv_request;
}

int main(int argc, char *argv[])
{
    int rc;
    int mpi_status;
    DCMF_Result dcmf_result;

    /***************************************************************/

    int provided;
    MPI_Init_thread( &argc, &argv, MPI_THREAD_MULTIPLE, &provided );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    assert( size > 1 );

    /***************************************************************/

    DCMF_Control_Configuration_t control_conf;

    control_conf.protocol           = DCMF_DEFAULT_CONTROL_PROTOCOL;
    control_conf.network            = DCMF_DEFAULT_NETWORK;
    control_conf.cb_recv            = remote_completion_cb;
    control_conf.cb_recv_clientdata = NULL;

    DCMF_CriticalSection_enter(0);
    dcmf_result = DCMF_Control_register(&control_proto, &control_conf);
    DCMF_CriticalSection_exit(0);
    assert(dcmf_result==DCMF_SUCCESS);

    /***************************************************************/

    DCMF_Protocol_t default_proto;
    DCMF_Send_Configuration_t default_conf;

    default_conf.protocol                 = DCMF_DEFAULT_SEND_PROTOCOL;
    default_conf.network                  = DCMF_DEFAULT_NETWORK;
    default_conf.cb_recv_short            = default_short_cb;
    default_conf.cb_recv_short_clientdata = NULL;
    default_conf.cb_recv                  = default_long_cb;
    default_conf.cb_recv_clientdata       = NULL;

    DCMF_CriticalSection_enter(0);
    dcmf_result = DCMF_Send_register(&default_proto, &default_conf);
    DCMF_CriticalSection_exit(0);
    assert(dcmf_result==DCMF_SUCCESS);

    /***************************************************************/

    mpi_status = MPI_Barrier(MPI_COMM_WORLD);
    assert(mpi_status==0);

    /***************************************************************/

    if ( rank == 0 )
    {
        int i;
        DCMF_Request_t request;
        DCMF_Callback_t callback;
        size_t target = 1;
        size_t send_bytes = ( argc > 1 ? atoi(argv[1]) : 1 );
        char * send_buffer = NULL;

        printf("%d: sending %d bytes \n", rank, send_bytes );
        fflush(stdout);

        rc = posix_memalign( (void**) &send_buffer, 128, send_bytes );
        assert( (rc == 0) && (send_buffer != NULL) );

        for (i=0;i<send_bytes;i++) send_buffer[i] = 'X';

        callback.function   = local_completion_cb;
        callback.clientdata = NULL;

        local_active  = 1;

        printf("%d: before DCMF_Send \n", rank );
        fflush(stdout);

        DCMF_CriticalSection_enter(0);
        dcmf_result = DCMF_Send(&default_proto,
                                &request,
                                callback,
                                DCMF_SEQUENTIAL_CONSISTENCY,
                                target,
                                send_bytes,
                                send_buffer,
                                NULL,
                                0);
        assert(dcmf_result==DCMF_SUCCESS);
        printf("%d: after DCMF_Send \n", rank );
        fflush(stdout);

        while ( local_active ) DCMF_Messager_advance(0);
        printf("%d: after local completion \n", rank );
        fflush(stdout);

        DCMF_CriticalSection_exit(0);
    }

    /***************************************************************/

    mpi_status = MPI_Barrier(MPI_COMM_WORLD);
    assert(mpi_status==0);

    /***************************************************************/

    MPI_Finalize();

    return 0;
}
#else
int main()
{
    return(1);
}
#endif
