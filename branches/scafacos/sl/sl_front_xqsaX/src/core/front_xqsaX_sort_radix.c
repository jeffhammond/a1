/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/core/sort_radix.c
 *  timestamp: 2010-08-25 17:24:49 +0200
 *  
 */


#include "sl_common.h"


#ifdef key_integer


slint_t sort_radix(elements_t *s, elements_t *sx, slint_t rhigh, slint_t rlow, slint_t rwidth) /* sl_proto, sl_func sort_radix */
{
  if (sx && sx->size >= s->size) return sort_radix_db(s, sx, rhigh, rlow, rwidth);
  else return sort_radix_ip(s, sx, rhigh, rlow, rwidth);
}


#endif /* key_integer */
