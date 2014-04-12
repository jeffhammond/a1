
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#undef TRACE_ERR
#ifndef TRACE_ERR
#define TRACE_ERR(x) /*  fprintf x*/
#endif

#define ARMCIX_Assert(x)  //assert(x)

typedef enum {
  ARMCIX_ACC_INT,
  ARMCIX_ACC_FLT,
  ARMCIX_ACC_DBL
} ARMCIX_Type_t;

typedef struct 
{
  int       count[2];
  int       sstride;
  int       dstride;
  double    scale;
  char    * dstaddr;
} ARMCIX_Info_t;

void ARMCIX_Initialize(int *mt, int *nt);
void ARMCIX_Finalize();

void ARMCIX_Acc (int               dt,
		 void            * scale,
		 void            * buffer,
		 void            * dstbuf,
		 int               bytes,
		 int               target);

void ARMCIX_AccS (int               dt,
		  void            * scale,
		  void            * buffer,
		  int               src_stride_arr[],
		  void            * dstbuf,
		  int               dst_stride_arr[],
		  int               count[],
		  int               levels,
		  int               target);

void ARMCIX_Fence (int target);

void ARMCIX_Barrier ();		 

unsigned long long ARMCIX_Wtimebase();

double ARMCIX_Wtime();
