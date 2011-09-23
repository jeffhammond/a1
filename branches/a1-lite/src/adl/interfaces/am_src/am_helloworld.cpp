#include "am_mpi.hpp"

/* END OF AM HANDLING */

void fn_hello(void *rbuf, int size) {
  printf("hello from %i\n", *(int*)rbuf);
}

static int g_leave=0;
void fn_leave(void *rbuf, int size) {
  g_leave = 1;
}

int main() {
  MPI_Init(NULL, NULL);

  int r,p;
  MPI_Comm comm=MPI_COMM_WORLD;
  MPI_Comm_rank(comm, &r);
  MPI_Comm_size(comm, &p);

  if(p < 2) {
    printf("need at least two processes!\n");
    MPI_Abort(comm, 2);
  }

  AM_Handle_t handle_hello, handle_leave;
  AM_Register(&fn_leave, &handle_leave);
  AM_Register(&fn_hello, &handle_hello);
  
  if(r ==0) {
    AM_Send(&r, sizeof(r), 1, handle_hello);
    AM_Send(&r, sizeof(r), 1, handle_leave);
  } else while(!g_leave) AM_Progress();

  MPI_Finalize();
}
