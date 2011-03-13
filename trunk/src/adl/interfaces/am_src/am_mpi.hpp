#include <mpi.h>
#include <stdlib.h>
#include <vector>

typedef struct {
  int tag;
} AM_Handle_t;

typedef void (*fptr_t)(void* rbuf, int size);

static int g_tag=0;
static std::vector<fptr_t> g_routines;

/* registers an active message handler -- this is a collective function! */
int AM_Register(fptr_t handler_routine, AM_Handle_t *handle) {

  // store function pointer
  g_routines.push_back(handler_routine);
  
  // fix tag for this handler
  handle->tag = g_tag++;

}

/* send an AM to a peer, does not support datatypes yet */
int AM_Send(void *buf, int count, int dest, AM_Handle_t handle) {
  
  MPI_Send(buf, count, MPI_BYTE, dest, handle.tag, MPI_COMM_WORLD);

}

/* progress the AM framework, works only with COMM_WORLD so far */
int AM_Progress() {

  int flag;
  MPI_Status status;
  MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status );

  if(flag) {
    int src=status.MPI_SOURCE;
    int tag=status.MPI_TAG;

    //printf("found msg from %i with tag %i\n", src, tag);
    
    int size;
    MPI_Get_count(&status, MPI_BYTE, &size);
    
    void *tmpbuf = malloc(size);

    MPI_Recv(tmpbuf, size, MPI_BYTE, src, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    g_routines[tag](tmpbuf, size);

    free(tmpbuf);
  }
}

