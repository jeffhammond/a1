/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/core/sort_radix_ip.c
 *  timestamp: 2010-11-22 14:37:18 +0100
 *  
 */


/* sl_macro SR_IP_INSERTSORT */
#define SR_IP_INSERTSORT


#include "sl_common.h"


slint_t sa_ip_threshold = sort_radix_threshold_rec;  /* sl_global, sl_var sa_ip_threshold */


#ifdef key_integer


slint_t rs_rec_ip(elements_t *s, elements_t *sx, slint_t rhigh, slint_t rlow, slint_t rwidth) /* sl_func rs_rec_ip */
{
/* fixed size of local arrays import for performance */
#define max_nclasses (powof2_typed(sort_radix_width_max, slkey_pure_t))

  slkey_pure_t bit_mask, nclasses;

  slint_t i, j, k, current_width, c[max_nclasses];
  elements_t xi, end, parts[max_nclasses];

  elem_assign_at(s, s->size, &end);

  current_width = xmin(rwidth, rhigh - rlow + 1);
  rhigh -= current_width - 1;

  nclasses = powof2_typed(current_width, slkey_pure_t);
  bit_mask = nclasses - 1;


  /* zero all counter */
  for (i = 0; i < nclasses; i++) c[i] = 0;

  /* count the number of elements in every class */
  for (elem_assign(s, &xi); xi.keys < end.keys; elem_inc(&xi)) ++c[key_radix_key2class(key_purify(*xi.keys), rhigh, bit_mask)];

  /* compute the target of every class */
  elem_assign(s, &parts[0]);
  for (i = 1; i < nclasses; i++) elem_assign_at(&parts[i - 1], c[i - 1], &parts[i]);;

  /* split the elements */
  elem_assign(s, &end);
  for (i = 0; i < nclasses; i++)
  {
    elem_add(&end, c[i]);

    elem_assign(&parts[i], &xi);

    while (xi.keys < end.keys)
    {
      j = key_radix_key2class(key_purify(*xi.keys), rhigh, bit_mask);

      while (j != i)
      {
        k = key_radix_key2class(key_purify(*parts[j].keys), rhigh, bit_mask);

        if (k != j) elem_xchange(&xi, &parts[j], sx);

        elem_inc(&parts[j]);

        j = k;
      }

      elem_inc(&xi);
    }
  }

  --rhigh;

  if (rhigh >= rlow)
  {
#ifdef SR_IP_INSERTSORT
    bit_mask = 0;
    if (rhigh - rlow + 1 <= key_radix_high) bit_mask = powof2_typed(rhigh - rlow + 1, slkey_pure_t);
    bit_mask = (bit_mask - 1) << rlow;
#endif

    elem_assign(s, &xi);
    for (i = 0; i < nclasses; i++)
    {
      xi.size = c[i];

#if defined(SR_IP_INSERTSORT)
      if (xi.size > sa_ip_threshold) rs_rec_ip(&xi, sx, rhigh, rlow, rwidth);
      else
      {
        if (xi.size > 1)
# ifdef SR_IP_INSERTSORT
          sort_insert_bmask_kernel(&xi, sx, bit_mask);
# endif
      }
#else
      if (xi.size > 1) rs_rec_ip(&xi, sx, rhigh, rlow, rwidth);
#endif

      elem_add(&xi, c[i]);
    }
  }

  return 0;
}


slint_t sort_radix_ip(elements_t *s, elements_t *sx, slint_t rhigh, slint_t rlow, slint_t rwidth) /* sl_proto, sl_func sort_radix_ip */
{
  elements_t _sx;


  if (s == NULL) return -1;

  if (s->size < 2) return 0;

  rti_tstart(rti_tid_sort_radix);

  if (sx == NULL || sx->size < 1)
  {
    sx = &_sx;
    elements_alloc(sx, 1, SLCM_ALL);

  } else if (sx->size < 1) return -1;

  if (rhigh < 0) rhigh = key_radix_high;
  if (rlow < 0) rlow = key_radix_low;
  if (rwidth <= 0) rwidth = sort_radix_width_default;

  rs_rec_ip(s, sx, rhigh, rlow, xmin(rwidth, sort_radix_width_max));

  if (sx == &_sx) elements_free(sx);

  rti_tstop(rti_tid_sort_radix);

  return 0;
}


#endif /* key_integer */
