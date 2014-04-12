
#ifndef   __A1PAMI_H__
#define   __A1PAMI_H__

#include <mpi.h>
#include <pami.h>
#include <assert.h>

typedef struct 
{
  int       count[2];
  int       sstride;
  int       dstride;
  double    scale;
  char    * dstaddr;
} A1PAMI_Info_t;

#define PRINT_SUCCESS      0
#define ENABLE_PROGRESS    1
#define ENABLE_MPI         1

#define TEST_ASSERT(c,m)			\
        do { \
        if (!(c)) { \
                    printf(m" FAILED on rank %ld\n", world_rank); \
                    fflush(stdout); \
                  } \
        else if (PRINT_SUCCESS) { \
                    printf(m" SUCCEEDED on rank %ld\n", world_rank); \
                    fflush(stdout); \
                  } \
        sleep(1); \
        /*assert(c);*/ \
        } \
        while(0); 

#define A1PAMI_Assert(x)  assert(x)

extern volatile unsigned  _recv_active;

void _internal_A1_Barrier ();

#endif
