
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>

#include <mpi.h>
#include "a1.h"
#include "hwi/include/bqc/A2_inlines.h"

#define ITERATIONS 100

#ifndef BUFSIZE
#define BUFSIZE 4*1024*1024
#endif

#define WARMUP

#define TRACE_ERR(X) 

void test_rmw (int               mytask, 
	       int               origin,
	       int               ntasks)
{
  TRACE_ERR((stderr, "(%u) Do test ... \n", mytask));

  int i = 0;
  int *outbuf = (int *) malloc (2*sizeof(int));
  outbuf[0] = mytask;
  outbuf[1] = origin;
  int target = origin+1;    
  if (target >= ntasks)
    target = 0;

  A1_AllFence();  
  if (mytask == origin)
  {
    for (i = 0; i < ntasks-1; i++)
    {
      A1_Rmw ( target, 
	       &outbuf[0],
	       &outbuf[1],
	       &outbuf[0],
	       sizeof(int),
	       A1_SWAP,
	       A1_INT32 );
      A1_Flush (target);
      target ++;
      outbuf[0] = outbuf[1];
      //printf ("%d: current swap %d\n", i, outbuf[0]);
    }
  }
  A1_AllFence();  
  MPI_Barrier(MPI_COMM_WORLD);

  printf ("%d: My new task id %d\n", mytask, outbuf[0]);
  assert ( outbuf[0] == ((mytask+ntasks-1)%ntasks) );
}

int my_task, origin_task, num_tasks;
int main (int argc, char ** argv)
{
  MPI_Init (&argc, &argv);
  A1_Initialize(0);
  origin_task = 0;

  MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_task);

  test_rmw (my_task, origin_task, num_tasks);
  
  A1_Finalize();
  MPI_Finalize();

  return 0;
}

#undef TRACE_ERR
