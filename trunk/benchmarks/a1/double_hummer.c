#include <stdio.h>
#include <mpi.h>

#define SIZE 100*1024*1024

int main(int argc, char **argv)
{
 
   int i;
   double source[SIZE] __attribute__((__aligned__(16)));
   double target[SIZE] __attribute__((__aligned__(16)));
   double scaling[2] __attribute__((__aligned__(16))); 
   double _Complex csource __attribute__((__aligned__(16))); 
   double _Complex cscaling __attribute__((__aligned__(16)));
   double _Complex ctarget __attribute__((__aligned__(16)));
   double t_start, t_stop;

   MPI_Init(&argc, &argv);

   scaling[0] = 2.0;
   scaling[1] = 2.0;
   for(i=0; i<SIZE; i++) 
   {
      source[i] = i+1;
      target[i] = i+1;
   }

   t_start = MPI_Wtime();
   cscaling = __lfpd(scaling);
   for(i=0; i<SIZE; i=i+2)
   {
      csource = __lfpd(&(source[i]));
      ctarget = __lfpd(&(target[i]));
      ctarget = __fpmadd(ctarget, cscaling, csource);
      __stfpd (&(target[i]), ctarget);
   }
   t_stop = MPI_Wtime();

   printf("Time (in msec) with double hummmer: %d  target: %f\n", (t_stop-t_start)*1000, 
                target[0]);   
   fflush(stdout);

   for(i=0; i<SIZE; i++)
   {
      target[i] = i+1;
   }

   t_start = MPI_Wtime();
   for(i=0; i<SIZE; i++)
   {
      target[i] = target[i] + source[i]*scaling[0];
   }
   t_stop = MPI_Wtime();

   printf("Time (in msec) with regular arithmetic: %d target: %f\n", (t_stop-t_start)*1000,
                 target[0]); 
   fflush(stdout);

   MPI_Finalize();

   return 0;
}
