
#include "armcix.h"

#define ITERATIONS 100

#ifndef BUFSIZE
#define BUFSIZE 4*1024*1024
#endif

#define WARMUP

double sndbuf[BUFSIZE/sizeof(double)] __attribute__ ((__aligned__(64)));
double rcvbuf[BUFSIZE/sizeof(double)] __attribute__ ((__aligned__(64)));

unsigned long long accumulate (size_t            sndlen, 
			       int               mytask, 
			       int               origin,
			       int               target)
{
  TRACE_ERR((stderr, "(%u) Do test ... sndlen = %zu\n", mytask, sndlen));
  unsigned i;
  unsigned long long t1 = ARMCIX_Wtimebase();
  double scale = 1.0;

  if (mytask == origin)
  {
    for (i = 0; i < ITERATIONS; i++)
    {
      //fprintf(stderr, "(%u) Starting Iteration %d of size %zu dstaddr %p\n", mytask, i, sndlen, rcvbuf);

      ARMCIX_Acc ( ARMCIX_ACC_FLT,
		   &scale,
		   sndbuf,
		   rcvbuf,
		   sndlen * sizeof(double),
		   target );      
      ARMCIX_Fence (target);
    }
  }
  ARMCIX_Barrier();
  
  unsigned long long t2 = ARMCIX_Wtimebase();
  return ((t2 - t1) / ITERATIONS);
}

int main (int argc, char ** argv)
{
  int my_task, origin_task, target_task, num_tasks;
  ARMCIX_Initialize(&my_task, &num_tasks);
  origin_task = 0;
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
    index[2] += sprintf (&str[2][index[2]], "  cycles     usec    cycles     usec    cycles     usec  ");
    
    fprintf (stdout, "#\n");
    fprintf (stdout, "%s\n", str[0]);
    fprintf (stdout, "%s\n", str[1]);
    fprintf (stdout, "%s\n", str[2]);
    fflush (stdout);
  }

  unsigned long long cycles;
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
    cycles = accumulate (sndlen, my_task, origin_task, target_task);
    usec   = (double)cycles / 1600.0;
    index += sprintf (&str[index], "%8lld %8.4f \n", cycles, usec);
  }
  
  if (my_task == origin_task)
    fprintf (stdout, "%s\n", str);

  ARMCIX_Finalize();
  
  return 0;
}


#undef TRACE_ERR
