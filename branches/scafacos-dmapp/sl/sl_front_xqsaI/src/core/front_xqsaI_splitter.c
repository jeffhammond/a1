/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/core/splitter.c
 *  timestamp: 2010-05-17 10:31:50 +0200
 *  
 */


#include "sl_common.h"


slint_t splitter_reset(splitter_t *sp) /* sl_proto, sl_func splitter_reset */
{
  slint_t i;


  for (i = 0; i < sp->n; ++i) sp->displs[i] = 0;

  return 0;
}
