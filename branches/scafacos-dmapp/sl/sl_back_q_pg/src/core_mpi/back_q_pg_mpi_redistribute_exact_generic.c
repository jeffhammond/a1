/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/core_mpi/mpi_redistribute_exact_generic.c
 *  timestamp: 2011-01-03 17:58:14 +0100
 *  
 */


/* sl_macro MREG_TRACE_IF */


#include "sl_common.h"


/* config */
/*#define PRINT_TIMINGS  0*/


#ifndef MREG_TRACE_IF
# ifdef GLOBAL_TRACE_IF
#  define MREG_TRACE_IF  GLOBAL_TRACE_IF
# else
#  define MREG_TRACE_IF  (sl_mpi_rank == -1)
# endif
#endif


slint_t mpi_redistribute_exact_generic(elements_t *sin, elements_t *sout, partcond_t *pcond, parttype_t *ptype, binning_t *bm, int *rcounts, int size, int rank, MPI_Comm comm) /* sl_proto, sl_func mpi_redistribute_exact_generic */
{
#if defined(PRINT_TIMINGS) && defined(SL_USE_RTI_TIM)
  if (rank == PRINT_TIMINGS)
  {
  }
#endif

  return 0;
}
