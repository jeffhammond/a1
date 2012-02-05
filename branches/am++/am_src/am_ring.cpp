#include "am_mpi.hpp"

/* END OF AM HANDLING */

static int g_iter=0;
static int p,r;
static AM_Handle_t handle_ring;

#define ITERS 1000

void fn_ring(void *rbuf, int size) {
  g_iter++;
  int next = ((*(int*)rbuf) + 2) % p;
  //printf("[%i] received from %i, sending to %i\n", r, *(int*)rbuf, next);
  AM_Send(&r, sizeof(r), next, handle_ring);
}

int main() {
  MPI_Init(NULL, NULL);

  MPI_Comm comm=MPI_COMM_WORLD;
  MPI_Comm_rank(comm, &r);
  MPI_Comm_size(comm, &p);

  if(p < 2) {
    printf("need at least two processes!\n");
    MPI_Abort(comm, 2);
  }

  AM_Register(&fn_ring, &handle_ring);
  
  double t = -MPI_Wtime();

  if(r ==0) AM_Send(&r, sizeof(r), 1, handle_ring);
  
  while(g_iter < ITERS) AM_Progress();

  t += MPI_Wtime();

  if(!r) printf("time per hop: %f us\n", t*1e6/ITERS/p);

  MPI_Finalize();
}
