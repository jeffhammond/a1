/*
 * Copyright (C) 2010. See COPYRIGHT in top-level directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "armcix.h"
#define MAX_XDIM        1024
#define MAX_YDIM        1024

#define MAX_DATA_SIZE   (MAX_XDIM*MAX_YDIM*sizeof(double))
#define NUM_ITERATIONS  ((xdim*ydim <= 1024) ? 64 : 16)
#define NUM_WARMUP_ITER 1 

int main(int argc, char ** argv) {
  int    rank, nproc;
  int    target_rank, xdim, ydim, test_iter;
  int    stride[1], count[2], levels;
  double scale;
  double   *buf, *rbuf;

  ARMCIX_Initialize(&rank, &nproc);

  if (rank == 0) printf("Starting one-sided strided performance test with %d processes\n", nproc);

  buf  = (double*)malloc(MAX_DATA_SIZE);
  rbuf = (double*)malloc(MAX_DATA_SIZE);
  memset(buf, 0, MAX_DATA_SIZE);

  if (rank == 0)
    printf("%12s %12s %12s %12s\n", "Trg. Rank", "Xdim Ydim",
	   "Acc (usec)", "Acc (MiB/s)");

  stride[0] = MAX_XDIM*sizeof(double);
  levels    = 1;
  scale     = 1.0;

  for (target_rank = nproc-1;rank == 0 && target_rank < nproc; target_rank++){

    for (xdim = 1; xdim <= MAX_XDIM; xdim *= 2) {
      count[0] = xdim*sizeof(double);

      for (ydim = 1; ydim <= MAX_YDIM; ydim *= 2) {
        const int data_size = xdim*ydim*sizeof(double);
        double    t_get, t_put, t_acc;

        count[1] = ydim;

        for (test_iter = 0; test_iter < NUM_ITERATIONS + NUM_WARMUP_ITER; test_iter++) {
          if (test_iter == NUM_WARMUP_ITER)
            t_acc = ARMCIX_Wtime();

          ARMCIX_AccS(ARMCIX_ACC_DBL, (void*) &scale, buf, stride, rbuf, stride, count, levels, target_rank);
        }
        ARMCIX_Fence(target_rank);
        t_acc = (ARMCIX_Wtime() - t_acc)/NUM_ITERATIONS;

        printf("%12d %6d%6d %12.3f %12.3f\n", target_rank, xdim, ydim,
            t_acc*1.0e6, data_size/(1024.0*1024.0)/t_acc);
      }
    }
  }

  ARMCIX_Barrier();
  
  free(rbuf);
  free(buf);

  ARMCIX_Finalize();

  return 0;
}
