/* 
* The following is a notice of limited availability of the code, and disclaimer
* which must be included in the prologue of the code and in all source listings
* of the code.
* 
* Copyright (c) 2010  Argonne Leadership Computing Facility, Argonne National 
* Laboratory
* 
* Permission is hereby granted to use, reproduce, prepare derivative works, and
* to redistribute to others.
* 
* 
*                          LICENSE
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
* 
* - Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
* 
* - Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer listed
*   in this license in the documentation and/or other materials
*   provided with the distribution.
* 
* - Neither the name of the copyright holders nor the names of its
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
* 
* The copyright holders provide no reassurances that the source code
* provided does not infringe any patent, copyright, or any other
* intellectual property rights of third parties.  The copyright holders
* disclaim any liability to any recipient for claims brought against
* recipient by any third party for infringement of that parties
* intellectual property rights.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <a1.h>

#define MAX_XDIM 1024 
#define MAX_YDIM 1024
#define ITERATIONS 100
#define SKIP 10

int main() {

   int i, j, rank, nranks, msgsize, dest;
   int xdim, ydim;
   long bufsize;
   double **buffer;
   double t_start, t_stop, t_latency;
   int count[2], src_stride, trg_stride, stride_levels;
   
   A1_Initialize(A1_THREAD_SINGLE); 

   rank = A1_Process_id(A1_GROUP_WORLD); 
   nranks = A1_Process_total(A1_GROUP_WORLD);

   A1_Barrier_group(A1_GROUP_WORLD);

   bufsize = MAX_XDIM * MAX_YDIM * sizeof(double);
   buffer = (double **) malloc (sizeof(double *) * nranks); 
   A1_Exchange_segments(A1_GROUP_WORLD, (void **) buffer, bufsize);

   for(i=0; i<bufsize/sizeof(double); i++) {
     *(buffer[rank] + i) = 1.0 + rank;
   }

   if(rank == 0) {
     printf("A1_GetS Latency in usec \n");
     printf("%30s %22s \n", "Dimensions(array of doubles)", "Latency");
     fflush(stdout);

     dest = 1;

     src_stride = MAX_YDIM*sizeof(double);
     trg_stride = MAX_YDIM*sizeof(double);
     stride_levels = 1;

     for(xdim=1; xdim<=MAX_XDIM; xdim*=2) 
     {

        count[1] = xdim;

        for(ydim=1; ydim<=MAX_YDIM; ydim*=2) 
        {

          count[0] = ydim*sizeof(double); 

          for(i=0; i<ITERATIONS+SKIP; i++) 
          { 

             if(i == SKIP)
                 t_start = A1_Time_seconds();              

             A1_GetS(1, (void *) buffer[dest], &src_stride, (void *) buffer[rank], &trg_stride, count, stride_levels); 
 
          }
          t_stop = A1_Time_seconds();

          char temp[10]; 
          sprintf(temp,"%dX%d", xdim, ydim);
          printf("%30s %20.2f \n", temp, ((t_stop-t_start)*1000000)/ITERATIONS);
          fflush(stdout);

          for(i=0; i<xdim; i++) 
          {
              for(j=0; j<ydim; j++) 
              {
                   if(*(buffer[rank] + i*MAX_XDIM + j) != (1.0 + dest)) 
                   {
                      printf("Data validation failed at X: %d Y: %d Expected : %f Actual : %f \n",
                              i, j, (1.0 + dest), *(buffer[rank] + i*MAX_XDIM + j));
                      fflush(stdout);
                      return -1;
                   }
               }
           }               

      }

    }

   }

   A1_Barrier_group(A1_GROUP_WORLD);

   A1_Release_segments(A1_GROUP_WORLD, (void *) buffer[rank]); 
 
   A1_Finalize();

   return 0;
}
