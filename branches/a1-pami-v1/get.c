
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

double *srcbuf;
double *dstbuf;

#define TRACE_ERR(X) 

uint64_t accumulate (size_t            sndlen, 
		     int               mytask, 
		     int               origin,
		     int               target)
{
  TRACE_ERR((stderr, "(%u) Do test ... sndlen = %zu\n", mytask, sndlen));
  unsigned i;
  uint64_t t1 = GetTimeBase();
  double scale = 1.0;

  if (mytask == origin)
  {
    for (i = 0; i < ITERATIONS; i++)
    {
      //fprintf(stderr, "(%u) Starting Iteration %d of size %zu dstaddr %p\n", mytask, i, sndlen, rcvbuf);
      A1_Get ( target, 
	       srcbuf,
	       dstbuf,
	       sndlen * sizeof(double) );      
      A1_Flush (target);
    }
  }
  A1_AllFence();
  MPI_Barrier(MPI_COMM_WORLD);
  
  uint64_t t2 = GetTimeBase();
  return ((t2 - t1) / ITERATIONS);
}

int main (int argc, char ** argv)
{
  int my_task, origin_task, target_task, num_tasks;
  MPI_Init (&argc, &argv);
  A1_Initialize(0);
  origin_task = 0;

  srcbuf = (double *) memalign (64, BUFSIZE);  
  dstbuf = (double *) memalign (64, BUFSIZE);

  MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_task);

  target_task = num_tasks-1;
    
  /* Display some test header information */
  if (my_task == origin_task)
  {
    char str[3][1024];
    int index[3];
    index[0] = 0;
    index[1] = 0;
    index[2] = 0;
    
    index[0] += sprintf (&str[0][index[0]], "#          ");
    index[1] += sprintf (&str[1][index[1]], "#          ");
    index[2] += sprintf (&str[2][index[2]], "#    bytes ");
    index[2] += sprintf (&str[2][index[2]], "  usec");
    
    fprintf (stdout, "#\n");
    fprintf (stdout, "%s\n", str[0]);
    fprintf (stdout, "%s\n", str[1]);
    fprintf (stdout, "%s\n", str[2]);
    fflush (stdout);
  }

  double usec;
  char str[10240];
  size_t sndlen = 1;
  int index = 0;
  for (; sndlen <= BUFSIZE/sizeof(double); sndlen = sndlen * 2)
  {
    index += sprintf (&str[index], "%10zd ", sndlen*sizeof(double));
    
#ifdef WARMUP
    accumulate (sndlen, my_task, origin_task, target_task);
#endif
    usec = ((double)accumulate (sndlen, my_task, origin_task, target_task)) / 1600.0;
    index += sprintf (&str[index], "%6.3f \n", usec);
  }
  
  if (my_task == origin_task)
    fprintf (stdout, "%s\n", str);

  A1_Finalize();
  MPI_Finalize();
  
  return 0;
}


#undef TRACE_ERR
