#ifdef __bgp__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <mpi.h>
#include <dcmf.h>

DCMF_Protocol_t default_proto;
DCMF_Protocol_t eager_proto;
DCMF_Protocol_t rzv_proto;

static int rank = -1, size = -1;

void cb_done(void * clientdata, DCMF_Error_t * error)
{
    printf("%d: cb_done \n", rank);

    --(*((uint32_t *) clientdata));
}

int main(int argc, char *argv[])
{
    int provided;
    int mpi_status;
    DCMF_Result dcmf_result;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    assert( size > 1 );

    DCMF_Send_Configuration_t default_conf;
    DCMF_Send_Configuration_t eager_conf;
    DCMF_Send_Configuration_t rzv_conf;

    default_conf.protocol = DCMF_DEFAULT_SEND_PROTOCOL;
    default_conf.network = DCMF_DEFAULT_NETWORK;

    dcmf_result = DCMF_Get_register(&get_protocol, &conf);
    assert(dcmf_result==DCMF_SUCCESS);

    mpi_status = MPI_Barrier(MPI_COMM_WORLD);
    assert(mpi_status==0);

    DCMF_CriticalSection_enter(0);
    DCMF_CriticalSection_exit(0);
    assert(dcmf_result==DCMF_SUCCESS);

    mpi_status = MPI_Barrier(MPI_COMM_WORLD);
    assert(mpi_status==0);

    MPI_Finalize();

    return 0;
}
#else
int main()
{
    return(1);
}
#endif
