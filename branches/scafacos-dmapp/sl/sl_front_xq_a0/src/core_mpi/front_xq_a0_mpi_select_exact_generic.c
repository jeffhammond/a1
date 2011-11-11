/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/core_mpi/mpi_select_exact_generic.c
 *  timestamp: 2011-01-13 09:48:59 +0100
 *  
 */


/* sl_macro MSEG_ROOT */
/* sl_macro MSEG_REDUCEBCAST_THRESHOLD */
/* sl_macro MSEG_BORDER_UPDATE_PROPAGATION */
/* sl_macro MSEG_BORDER_UPDATE_FULL */
/* sl_macro MSEG_TRACE_IF */

  
#include "sl_common.h"


/* config */
/*#define SYNC_ON_INIT
#define SYNC_ON_EXIT*/
#define HAVENT_MPI_IN_PLACE

/*#define PRINT_SDISPLS*/
/*#define PRINT_STATS*/
/*#define PRINT_TIMINGS  0*/

/*#define VERIFY*/


#define MIN_LE  0
#define MIN_RI  1
#define MAX_LE  2
#define MAX_RI  3

typedef struct _border_info_t {
#ifdef MSEG_BORDER_UPDATE_FULL
  slint_t done;
#endif
  slint_t update;
  slint_t crange[2], cmmlr[4];
#ifdef elem_weight
  slweight_t wrange[2], wmmlr[4];
#endif

} border_info_t;


#define COUNT_LO   0
#define COUNT_HI   1
#define WEIGHT_LO  2
#define WEIGHT_HI  3


slint_t mseg_rounds = 0;  /* sl_global, sl_var mseg_rounds */

#ifdef MSEG_ROOT
int mseg_root = -1;  /* sl_global, sl_var mseg_root */
#endif

#define REDUCEBCAST_ROOT  0

#if !defined(MSEG_REDUCEBCAST_THRESHOLD) && defined(GLOBAL_REDUCEBCAST_THRESHOLD)
# define MSEG_REDUCEBCAST_THRESHOLD  GLOBAL_REDUCEBCAST_THRESHOLD
#endif

#ifdef MSEG_BORDER_UPDATE_PROPAGATION
double mseg_border_update_propagation = 0.0;  /* sl_global, sl_var mseg_border_update_propagation */
#endif

#ifdef MSEG_BORDER_UPDATE_FULL
slint_t mseg_border_update_full = 0;  /* sl_global, sl_var mseg_border_update_full */
#endif

slint_t mseg_binnings = -1;  /* sl_global, sl_var mseg_binnings */

#ifndef MSEG_TRACE_IF
# ifdef GLOBAL_TRACE_IF
#  define MSEG_TRACE_IF  GLOBAL_TRACE_IF
# else
#  define MSEG_TRACE_IF  (sl_mpi_rank == -1)
# endif
#endif


void border_update(slint_t doweights, border_info_t *bi, partcond_intern_t *pc, slint_t dir, slint_t check_inconsistence, slint_t prop) /* sl_func border_update */
{
#ifdef MSEG_BORDER_UPDATE_PROPAGATION
  slint_t cprop;
# ifdef elem_weight
  slweight_t wprop;
# endif
#endif

  /* forward */
  if (dir > 0)
  {
    /* init from range */
    bi[0].cmmlr[MIN_LE] = bi[0].crange[0];
    bi[0].cmmlr[MAX_LE] = bi[0].crange[1];

    /* init from min/max */
    if (pc[0].pcm & SLPC_COUNTS_MM)
    {
#ifdef MSEG_BORDER_UPDATE_PROPAGATION
      if (prop)
      {
        cprop = round((bi[-1].cmmlr[MAX_LE] - bi[-1].cmmlr[MIN_LE]) * 0.5 * mseg_border_update_propagation);
        SL_ASSERT(cprop >= 0);

        SL_TRACE_IF(MSEG_TRACE_IF, "forward: cprop: %" slint_fmt, cprop);

        bi[0].cmmlr[MIN_LE] = xmin(bi[-1].cmmlr[MIN_LE] + cprop + pc[0].count_min, bi[0].cmmlr[MIN_RI]);
        bi[0].cmmlr[MAX_LE] = xmax(bi[-1].cmmlr[MAX_LE] - cprop + pc[0].count_max, bi[0].cmmlr[MAX_RI]);

        SL_TRACE_IF(MSEG_TRACE_IF, "forward: count[min/max-left]: min(%" slint_fmt " + %" slint_fmt " + %" slint_fmt ", %" slint_fmt "), max(%" slint_fmt " - % "slint_fmt " + %" slint_fmt ", %" slint_fmt ")",
          bi[-1].cmmlr[MIN_LE], cprop, pc[0].count_min, bi[0].cmmlr[MIN_RI], bi[-1].cmmlr[MAX_LE], cprop, pc[0].count_max, bi[0].cmmlr[MAX_RI]);

      } else
#endif
      {
        bi[0].cmmlr[MIN_LE] = bi[-1].cmmlr[MIN_LE] + pc[0].count_min;
        bi[0].cmmlr[MAX_LE] = bi[-1].cmmlr[MAX_LE] + pc[0].count_max;

        SL_TRACE_IF(MSEG_TRACE_IF, "forward: count[min/max-left]: %" slint_fmt " + %" slint_fmt ", %" slint_fmt " + %" slint_fmt,
          bi[-1].cmmlr[MIN_LE], pc[0].count_min, bi[-1].cmmlr[MAX_LE], pc[0].count_max);
      }
    }

    /* check against low/high */
    if (pc[0].pcm & SLPC_COUNTS_LH)
    {
      if (bi[0].cmmlr[MIN_LE] < pc[1].count_low)  bi[0].cmmlr[MIN_LE] = pc[1].count_low;
      if (bi[0].cmmlr[MAX_LE] > pc[0].count_high) bi[0].cmmlr[MAX_LE] = pc[0].count_high;
    }

    /* fit to range */
    bi[0].cmmlr[MIN_LE] = xminmax(bi[0].crange[0], bi[0].cmmlr[MIN_LE], bi[0].crange[1]);
    bi[0].cmmlr[MAX_LE] = xminmax(bi[0].crange[0], bi[0].cmmlr[MAX_LE], bi[0].crange[1]);

#ifdef elem_weight
    if (doweights)
    {
      /* init from range */
      bi[0].wmmlr[MIN_LE] = bi[0].wrange[0];
      bi[0].wmmlr[MAX_LE] = bi[0].wrange[1];

      /* init from min/max */
      if (pc[0].pcm & SLPC_WEIGHTS_MM)
      {
# ifdef MSEG_BORDER_UPDATE_PROPAGATION
        if (prop)
        {
          wprop = (bi[-1].wmmlr[MAX_LE] - bi[-1].wmmlr[MIN_LE]) * 0.5 * mseg_border_update_propagation;
          SL_ASSERT(wprop >= 0.0);

          SL_TRACE_IF(MSEG_TRACE_IF, "forward: wprop: %" slweight_fmt, wprop);
  
          bi[0].wmmlr[MIN_LE] = xmin(bi[-1].wmmlr[MIN_LE] + wprop + pc[0].weight_min, bi[0].wmmlr[MIN_RI]);
          bi[0].wmmlr[MAX_LE] = xmax(bi[-1].wmmlr[MAX_LE] - wprop + pc[0].weight_max, bi[0].wmmlr[MAX_RI]);

          SL_TRACE_IF(MSEG_TRACE_IF, "wprop: weight[min/max-left]: min(%" slweight_fmt " + %" slweight_fmt " + %" slweight_fmt ", %" slweight_fmt "), max(%" slweight_fmt " - %" slweight_fmt " + %" slweight_fmt ", %" slweight_fmt ")",
            bi[-1].wmmlr[MIN_LE], wprop, pc[0].weight_min, bi[0].wmmlr[MIN_RI], bi[-1].wmmlr[MAX_LE], wprop, pc[0].weight_max, bi[0].wmmlr[MAX_RI]);

        } else
# endif
        {
          bi[0].wmmlr[MIN_LE] = bi[-1].wmmlr[MIN_LE] + pc[0].weight_min;
          bi[0].wmmlr[MAX_LE] = bi[-1].wmmlr[MAX_LE] + pc[0].weight_max;

          SL_TRACE_IF(MSEG_TRACE_IF, "forward: weight[min/max-left]: %" slweight_fmt " + %" slweight_fmt ", %" slweight_fmt " + %" slweight_fmt,
            bi[-1].wmmlr[MIN_LE], pc[0].weight_min, bi[-1].wmmlr[MAX_LE], pc[0].weight_max);
        }
      }

      /* check against low/high (on demand) */
      if (pc[0].pcm & SLPC_WEIGHTS_LH)
      {
        if (bi[0].wmmlr[MIN_LE] < pc[1].weight_low)  bi[0].wmmlr[MIN_LE] = pc[1].weight_low;
        if (bi[0].wmmlr[MAX_LE] > pc[0].weight_high) bi[0].wmmlr[MAX_LE] = pc[0].weight_high;
      }

      /* fit to range */
      bi[0].wmmlr[MIN_LE] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MIN_LE], bi[0].wrange[1]);
      bi[0].wmmlr[MAX_LE] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MAX_LE], bi[0].wrange[1]);
    }
#endif
  } else /* backward */
  {
    /* init from range */
    bi[0].cmmlr[MAX_RI] = bi[0].crange[0];
    bi[0].cmmlr[MIN_RI] = bi[0].crange[1];

    /* init from min/max */
    if (pc[0].pcm & SLPC_COUNTS_MM)
    {
#ifdef MSEG_BORDER_UPDATE_PROPAGATION
      if (prop)
      {
        cprop = round((bi[1].cmmlr[MIN_RI] - bi[1].cmmlr[MAX_RI]) * 0.5 * mseg_border_update_propagation);
        SL_ASSERT(cprop >= 0);

        SL_TRACE_IF(MSEG_TRACE_IF, "backward: cprop: %" slint_fmt, cprop);

        bi[0].cmmlr[MIN_RI] = xmax(bi[1].cmmlr[MIN_RI] - cprop - pc[1].count_min, bi[0].cmmlr[MIN_LE]);
        bi[0].cmmlr[MAX_RI] = xmin(bi[1].cmmlr[MAX_RI] + cprop - pc[1].count_max, bi[0].cmmlr[MAX_LE]);

        SL_TRACE_IF(MSEG_TRACE_IF, "backward: count[min/max-right]: max(%" slint_fmt " - %" slint_fmt " + %" slint_fmt ", %" slint_fmt "), min(%" slint_fmt " + %" slint_fmt " - %" slint_fmt ", %" slint_fmt ")",
          bi[1].cmmlr[MIN_RI], cprop, pc[1].count_min, bi[0].cmmlr[MIN_LE], bi[1].cmmlr[MAX_RI], cprop, pc[1].count_max, bi[0].cmmlr[MAX_LE]);

      } else
#endif
      {
        bi[0].cmmlr[MIN_RI] = bi[1].cmmlr[MIN_RI] - pc[1].count_min;
        bi[0].cmmlr[MAX_RI] = bi[1].cmmlr[MAX_RI] - pc[1].count_max;

        SL_TRACE_IF(MSEG_TRACE_IF, "backward: count[min/max-right]: %" slint_fmt " - %" slint_fmt ", %" slint_fmt " - %" slint_fmt "",
          bi[1].cmmlr[MIN_RI], pc[1].count_min, bi[1].cmmlr[MAX_RI], pc[1].count_max);
      }
    }

    /* check against low/high (on demand) */
    if (pc[0].pcm & SLPC_COUNTS_LH)
    {
      if (bi[0].cmmlr[MAX_RI] < pc[1].count_low)  bi[0].cmmlr[MAX_RI] = pc[1].count_low;
      if (bi[0].cmmlr[MIN_RI] > pc[0].count_high) bi[0].cmmlr[MIN_RI] = pc[0].count_high;
    }

    /* fit to range */
    bi[0].cmmlr[MIN_RI] = xminmax(bi[0].crange[0], bi[0].cmmlr[MIN_RI], bi[0].crange[1]);
    bi[0].cmmlr[MAX_RI] = xminmax(bi[0].crange[0], bi[0].cmmlr[MAX_RI], bi[0].crange[1]);

#ifdef elem_weight
    if (doweights)
    {
      /* init from range */
      bi[0].wmmlr[MAX_RI] = bi[0].wrange[0];
      bi[0].wmmlr[MIN_RI] = bi[0].wrange[1];

      /* init from min/max */
      if (pc[0].pcm & SLPC_WEIGHTS_MM)
      {
#ifdef MSEG_BORDER_UPDATE_PROPAGATION
        if (prop)
        {
          wprop = (bi[1].wmmlr[MIN_RI] - bi[1].wmmlr[MAX_RI]) * 0.5 * mseg_border_update_propagation;
          SL_ASSERT(wprop >= 0.0);

          SL_TRACE_IF(MSEG_TRACE_IF, "backward: wprop: %" slweight_fmt, wprop);

          bi[0].wmmlr[MIN_RI] = xmax(bi[1].wmmlr[MIN_RI] - wprop - pc[1].weight_min, bi[0].wmmlr[MIN_LE]);
          bi[0].wmmlr[MAX_RI] = xmin(bi[1].wmmlr[MAX_RI] + wprop - pc[1].weight_max, bi[0].wmmlr[MAX_LE]);

          SL_TRACE_IF(MSEG_TRACE_IF, "backward: weight[min/max-right]: max(%" slweight_fmt " - %" slweight_fmt " - %" slweight_fmt ", %" slweight_fmt "), min(%" slweight_fmt " + %" slweight_fmt " - %" slweight_fmt ", %" slweight_fmt ")",
            bi[1].wmmlr[MIN_RI], wprop, pc[1].weight_min, bi[0].wmmlr[MIN_LE], bi[1].wmmlr[MAX_RI], wprop, pc[1].weight_max, bi[0].wmmlr[MAX_LE]);

        } else
#endif
        {
          bi[0].wmmlr[MIN_RI] = bi[1].wmmlr[MIN_RI] - pc[1].weight_min;
          bi[0].wmmlr[MAX_RI] = bi[1].wmmlr[MAX_RI] - pc[1].weight_max;

          SL_TRACE_IF(MSEG_TRACE_IF, "backward: weight[min/max-right]: %" slweight_fmt " - %" slweight_fmt ", %" slweight_fmt " - %" slweight_fmt,
            bi[1].wmmlr[MIN_RI], pc[1].weight_min, bi[1].wmmlr[MAX_RI], pc[1].weight_max);
        }
      }

      /* check against low/high (on demand) */
      if (pc[0].pcm & SLPC_WEIGHTS_LH)
      {
        if (bi[0].wmmlr[MAX_RI] < pc[1].weight_low)  bi[0].wmmlr[MAX_RI] = pc[1].weight_low;
        if (bi[0].wmmlr[MIN_RI] > pc[0].weight_high) bi[0].wmmlr[MIN_RI] = pc[0].weight_high;
      }

      /* fit to range */
      bi[0].wmmlr[MIN_RI] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MIN_RI], bi[0].wrange[1]);
      bi[0].wmmlr[MAX_RI] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MAX_RI], bi[0].wrange[1]);
    }
#endif
  }

  SL_TRACE_IF(MSEG_TRACE_IF, "count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "",
    bi[0].cmmlr[MIN_LE], bi[0].cmmlr[MAX_LE], bi[0].cmmlr[MIN_RI], bi[0].cmmlr[MAX_RI]);

#ifdef elem_weight
  if (doweights)
    SL_TRACE_IF(MSEG_TRACE_IF, "weight[min/max-left/right]: %" slweight_fmt " / %" slweight_fmt " - %" slweight_fmt " / %" slweight_fmt,
      bi[0].wmmlr[MIN_LE], bi[0].wmmlr[MAX_LE], bi[0].wmmlr[MIN_RI], bi[0].wmmlr[MAX_RI]);
#endif

  if (check_inconsistence)
  {
    /* check against inconsistence */
    if (bi[0].cmmlr[MIN_LE] > bi[0].cmmlr[MIN_RI]) bi[0].cmmlr[MIN_LE] = bi[0].cmmlr[MIN_RI] = (bi[0].cmmlr[MIN_LE] + bi[0].cmmlr[MIN_RI]) / 2;
    if (bi[0].cmmlr[MAX_LE] < bi[0].cmmlr[MAX_RI]) bi[0].cmmlr[MAX_LE] = bi[0].cmmlr[MAX_RI] = (bi[0].cmmlr[MAX_LE] + bi[0].cmmlr[MAX_RI]) / 2;

    SL_TRACE_IF(MSEG_TRACE_IF, "consistence count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "",
      bi[0].cmmlr[MIN_LE], bi[0].cmmlr[MAX_LE], bi[0].cmmlr[MIN_RI], bi[0].cmmlr[MAX_RI]);

#ifdef elem_weight
    if (doweights)
    {
      if (bi[0].wmmlr[MIN_LE] > bi[0].wmmlr[MIN_RI]) bi[0].wmmlr[MIN_LE] = bi[0].wmmlr[MIN_RI] = (bi[0].wmmlr[MIN_LE] + bi[0].wmmlr[MIN_RI]) / 2;
      if (bi[0].wmmlr[MAX_LE] < bi[0].wmmlr[MAX_RI]) bi[0].wmmlr[MAX_LE] = bi[0].wmmlr[MAX_RI] = (bi[0].wmmlr[MAX_LE] + bi[0].wmmlr[MAX_RI]) / 2;

      SL_TRACE_IF(MSEG_TRACE_IF, "consistence weight[min/max-left/right]: %" slweight_fmt " / %" slweight_fmt " - %" slweight_fmt " / %" slweight_fmt,
        bi[0].wmmlr[MIN_LE], bi[0].wmmlr[MAX_LE], bi[0].wmmlr[MIN_RI], bi[0].wmmlr[MAX_RI]);
    }
#endif
  }
}


#ifdef MSEG_BORDER_UPDATE_FULL
void border_update_full(slint_t doweights, border_info_t *bi, partcond_intern_t *pc, slint_t check_inconsistence) /* sl_func border_update_full */
{
  slint_t lo = -1, hi = -1;

  bi[0].cmmlr[MIN_LE] = bi[0].cmmlr[MAX_RI] = bi[0].crange[0];
  bi[0].cmmlr[MAX_LE] = bi[0].cmmlr[MIN_RI] = bi[0].crange[1];

#ifdef elem_weight
  if (doweights)
  {
    bi[0].wmmlr[MIN_LE] = bi[0].wmmlr[MAX_RI] = bi[0].wrange[0];
    bi[0].wmmlr[MAX_LE] = bi[0].wmmlr[MIN_RI] = bi[0].wrange[1];
  }
#endif

  /* init from min/max */
  if (pc[0].pcm & (SLPC_COUNTS_MM|SLPC_WEIGHTS_MM))
  {
    /* seach backward */
    for (lo = 0; (!bi[lo - 1].done); --lo);

    /* seach forward */
    for (hi = 0; (!bi[hi + 1].done); ++hi);

    SL_TRACE_IF(MSEG_TRACE_IF, "found: lo: %" slint_fmt ", hi: %" slint_fmt "", lo, hi);

    if (pc[0].pcm & SLPC_COUNTS_MM)
    {
      bi[0].cmmlr[MIN_LE] = bi[lo - 1].cmmlr[MIN_LE];
      bi[0].cmmlr[MAX_LE] = bi[lo - 1].cmmlr[MAX_LE];
      SL_ASSERT(bi[0].cmmlr[MIN_LE] == bi[0].cmmlr[MAX_LE]);

      bi[0].cmmlr[MIN_RI] = bi[hi + 1].cmmlr[MIN_RI]; 
      bi[0].cmmlr[MAX_RI] = bi[hi + 1].cmmlr[MAX_RI]; 
      SL_ASSERT(bi[0].cmmlr[MIN_RI] == bi[0].cmmlr[MAX_RI]);
    }

    SL_TRACE_IF(MSEG_TRACE_IF, "start: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "",
      bi[0].cmmlr[MIN_LE], bi[0].cmmlr[MAX_LE], bi[0].cmmlr[MIN_RI], bi[0].cmmlr[MAX_RI]);

#ifdef elem_weight
    if (doweights)
    {
      if (pc[0].pcm & SLPC_WEIGHTS_MM)
      {
        bi[0].wmmlr[MIN_LE] = bi[lo - 1].wmmlr[MIN_LE];
        bi[0].wmmlr[MAX_LE] = bi[lo - 1].wmmlr[MAX_LE];
        SL_ASSERT(bi[0].wmmlr[MIN_LE] == bi[0].wmmlr[MAX_LE]);

        bi[0].wmmlr[MIN_RI] = bi[hi + 1].wmmlr[MIN_RI];
        bi[0].wmmlr[MAX_RI] = bi[hi + 1].wmmlr[MAX_RI];
        SL_ASSERT(bi[0].wmmlr[MIN_RI] == bi[0].wmmlr[MAX_RI]);
      }
    }
#endif
  
    /* go backward */
    for (; lo <= 0; ++lo)
    {
      if (pc[0].pcm & SLPC_COUNTS_MM)
      {
        SL_TRACE_IF(MSEG_TRACE_IF, "- count[min/max-left]: %" slint_fmt " + %" slint_fmt ", %" slint_fmt " + %" slint_fmt "",
          bi[0].cmmlr[MIN_LE], pc[0].count_min, bi[0].cmmlr[MAX_LE], pc[0].count_max);

        bi[0].cmmlr[MIN_LE] = bi[0].cmmlr[MIN_LE] + pc[lo].count_min;
        bi[0].cmmlr[MAX_LE] = bi[0].cmmlr[MAX_LE] + pc[lo].count_max;
      }

#ifdef elem_weight
      if (doweights && pc[0].pcm & SLPC_WEIGHTS_MM)
      {
        bi[0].wmmlr[MIN_LE] = bi[0].wmmlr[MIN_LE] + pc[lo].weight_min;
        bi[0].wmmlr[MAX_LE] = bi[0].wmmlr[MAX_LE] + pc[lo].weight_max;
      }
#endif
    }

    /* go forward */
    for (; 0 <= hi; --hi)
    {
      if (pc[0].pcm & SLPC_COUNTS_MM)
      {
        SL_TRACE_IF(MSEG_TRACE_IF, "+ count[min/max-left]: %" slint_fmt " - %" slint_fmt ", %" slint_fmt " - %" slint_fmt "",
          bi[0].cmmlr[MIN_RI], pc[hi + 1].count_min, bi[0].cmmlr[MAX_RI], pc[hi + 1].count_max);

        bi[0].cmmlr[MIN_RI] = bi[0].cmmlr[MIN_RI] - pc[hi + 1].count_min;
        bi[0].cmmlr[MAX_RI] = bi[0].cmmlr[MAX_RI] - pc[hi + 1].count_max;
      }

#ifdef elem_weight
      if (doweights && pc[0].pcm & SLPC_WEIGHTS_MM)
      {
        bi[0].wmmlr[MIN_RI] = bi[0].wmmlr[MIN_RI] + pc[hi + 1].weight_min;
        bi[0].wmmlr[MAX_RI] = bi[0].wmmlr[MAX_RI] + pc[hi + 1].weight_max;
      }
#endif
    }

  } else SL_TRACE_IF(MSEG_TRACE_IF, "no search");

  /* check against low/high */
  if (pc[0].pcm & SLPC_COUNTS_LH)
  {
    if (bi[0].cmmlr[MIN_LE] < pc[1].count_low)  bi[0].cmmlr[MIN_LE] = pc[1].count_low;
    if (bi[0].cmmlr[MAX_LE] > pc[0].count_high) bi[0].cmmlr[MAX_LE] = pc[0].count_high;
    if (bi[0].cmmlr[MAX_RI] < pc[1].count_low)  bi[0].cmmlr[MAX_RI] = pc[1].count_low;
    if (bi[0].cmmlr[MIN_RI] > pc[0].count_high) bi[0].cmmlr[MIN_RI] = pc[0].count_high;
  }

#ifdef elem_weight
  if (doweights && pc[0].pcm & SLPC_WEIGHTS_MM)
  {
    if (bi[0].wmmlr[MIN_LE] < pc[1].weight_low)  bi[0].wmmlr[MIN_LE] = pc[1].weight_low;
    if (bi[0].wmmlr[MAX_LE] > pc[0].weight_high) bi[0].wmmlr[MAX_LE] = pc[0].weight_high;
    if (bi[0].wmmlr[MAX_RI] < pc[1].weight_low)  bi[0].wmmlr[MAX_RI] = pc[1].weight_low;
    if (bi[0].wmmlr[MIN_RI] > pc[0].weight_high) bi[0].wmmlr[MIN_RI] = pc[0].weight_high;
  }
#endif

  /* fit to range */
  bi[0].cmmlr[MIN_LE] = xminmax(bi[0].crange[0], bi[0].cmmlr[MIN_LE], bi[0].crange[1]);
  bi[0].cmmlr[MAX_LE] = xminmax(bi[0].crange[0], bi[0].cmmlr[MAX_LE], bi[0].crange[1]);
  bi[0].cmmlr[MIN_RI] = xminmax(bi[0].crange[0], bi[0].cmmlr[MIN_RI], bi[0].crange[1]);
  bi[0].cmmlr[MAX_RI] = xminmax(bi[0].crange[0], bi[0].cmmlr[MAX_RI], bi[0].crange[1]);

#ifdef elem_weight
  if (doweights)
  {
    bi[0].wmmlr[MIN_LE] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MIN_LE], bi[0].wrange[1]);
    bi[0].wmmlr[MAX_LE] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MAX_LE], bi[0].wrange[1]);
    bi[0].wmmlr[MIN_RI] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MIN_RI], bi[0].wrange[1]);
    bi[0].wmmlr[MAX_RI] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MAX_RI], bi[0].wrange[1]);
  }
#endif

  SL_TRACE_IF(MSEG_TRACE_IF, "count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "",
    bi[0].cmmlr[MIN_LE], bi[0].cmmlr[MAX_LE], bi[0].cmmlr[MIN_RI], bi[0].cmmlr[MAX_RI]);

#ifdef elem_weight
  if (doweights)
    SL_TRACE_IF(MSEG_TRACE_IF, "weight[min/max-left/right]: %" slweight_fmt " / %" slweight_fmt " - %" slweight_fmt " / %" slweight_fmt,
      bi[0].wmmlr[MIN_LE], bi[0].wmmlr[MAX_LE], bi[0].wmmlr[MIN_RI], bi[0].wmmlr[MAX_RI]);
#endif

  if (check_inconsistence)
  {
    /* check against inconsistence */
    if (bi[0].cmmlr[MIN_LE] > bi[0].cmmlr[MIN_RI]) bi[0].cmmlr[MIN_LE] = bi[0].cmmlr[MIN_RI] = (bi[0].cmmlr[MIN_LE] + bi[0].cmmlr[MIN_RI]) / 2;
    if (bi[0].cmmlr[MAX_LE] < bi[0].cmmlr[MAX_RI]) bi[0].cmmlr[MAX_LE] = bi[0].cmmlr[MAX_RI] = (bi[0].cmmlr[MAX_LE] + bi[0].cmmlr[MAX_RI]) / 2;

    SL_TRACE_IF(MSEG_TRACE_IF, "consistence count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "",
      bi[0].cmmlr[MIN_LE], bi[0].cmmlr[MAX_LE], bi[0].cmmlr[MIN_RI], bi[0].cmmlr[MAX_RI]);

#ifdef elem_weight
    if (doweights)
    {
      if (bi[0].wmmlr[MIN_LE] > bi[0].wmmlr[MIN_RI]) bi[0].wmmlr[MIN_LE] = bi[0].wmmlr[MIN_RI] = (bi[0].wmmlr[MIN_LE] + bi[0].wmmlr[MIN_RI]) / 2;
      if (bi[0].wmmlr[MAX_LE] < bi[0].wmmlr[MAX_RI]) bi[0].wmmlr[MAX_LE] = bi[0].wmmlr[MAX_RI] = (bi[0].wmmlr[MAX_LE] + bi[0].wmmlr[MAX_RI]) / 2;

      SL_TRACE_IF(MSEG_TRACE_IF, "consistence weight[min/max-left/right]: %" slweight_fmt " / %" slweight_fmt " - %" slweight_fmt " / %" slweight_fmt,
        bi[0].wmmlr[MIN_LE], bi[0].wmmlr[MAX_LE], bi[0].wmmlr[MIN_RI], bi[0].wmmlr[MAX_RI]);
    }
#endif
  }
}
#endif


void border_change_old(slint_t doweights, border_info_t *bi, border_info_t *bi_old, slint_t gcs, slint_t gc, slweight_t gws, slweight_t gw) /* sl_func border_change_old */
{
  SL_TRACE_IF(MSEG_TRACE_IF, "change: gcs = %" slint_fmt ", gc = %" slint_fmt "", gcs, gc);

  bi[0].crange[0] += gcs;
  bi[0].crange[1] = bi[0].crange[0] + gc;

  SL_TRACE_IF(MSEG_TRACE_IF, "counts_range: %" slint_fmt "  %" slint_fmt "", bi[0].crange[0], bi[0].crange[1]);

  bi[0].cmmlr[MIN_LE] = xminmax(bi[0].crange[0], bi[0].cmmlr[MIN_LE], bi[0].crange[1]);
  bi[0].cmmlr[MAX_LE] = xminmax(bi[0].crange[0], bi[0].cmmlr[MAX_LE], bi[0].crange[1]);
  bi[0].cmmlr[MIN_RI] = xminmax(bi[0].crange[0], bi[0].cmmlr[MIN_RI], bi[0].crange[1]);
  bi[0].cmmlr[MAX_RI] = xminmax(bi[0].crange[0], bi[0].cmmlr[MAX_RI], bi[0].crange[1]);

  SL_TRACE_IF(MSEG_TRACE_IF, "count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "",
    bi[0].cmmlr[MIN_LE], bi[0].cmmlr[MAX_LE], bi[0].cmmlr[MIN_RI], bi[0].cmmlr[MAX_RI]);

#ifdef elem_weight
  if (doweights)
  {
    SL_TRACE_IF(MSEG_TRACE_IF, "change: gws = %" slweight_fmt ", gc = %" slweight_fmt, gws, gw);

    bi[0].wrange[0] += gws;
    bi[0].wrange[1] = bi[0].wrange[0] + gw;

    SL_TRACE_IF(MSEG_TRACE_IF, "weights_range: %" slweight_fmt "  %" slweight_fmt, bi[0].wrange[0], bi[0].wrange[1]);

    bi[0].wmmlr[MIN_LE] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MIN_LE], bi[0].wrange[1]);
    bi[0].wmmlr[MAX_LE] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MAX_LE], bi[0].wrange[1]);
    bi[0].wmmlr[MIN_RI] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MIN_RI], bi[0].wrange[1]);
    bi[0].wmmlr[MAX_RI] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MAX_RI], bi[0].wrange[1]);

    SL_TRACE_IF(MSEG_TRACE_IF, "weight[min/max-left/right]: %" slweight_fmt " / %" slweight_fmt " - %" slweight_fmt " / %" slweight_fmt,
      bi[0].wmmlr[MIN_LE], bi[0].wmmlr[MAX_LE], bi[0].wmmlr[MIN_RI], bi[0].wmmlr[MAX_RI]);
  }
#endif
      
  SL_TRACE_IF(MSEG_TRACE_IF, "range diff 0: %" slint_fmt "-%" slint_fmt " | %" slint_fmt "-%" slint_fmt,
    bi[0].crange[0] - bi[-1].crange[1], bi[0].crange[0] - bi[-1].crange[0],
    bi[1].crange[0] - bi[ 0].crange[0], bi[1].crange[1] - bi[ 0].crange[0]);
  SL_TRACE_IF(MSEG_TRACE_IF, "range diff 1: %" slint_fmt "-%" slint_fmt " | %" slint_fmt "-%" slint_fmt,
    bi[0].crange[1] - bi[-1].crange[1], bi[0].crange[1] - bi[-1].crange[0],
    bi[1].crange[0] - bi[ 0].crange[1], bi[1].crange[1] - bi[ 0].crange[1]);

#ifndef MSEG_BORDER_UPDATE_FULL
# ifdef elem_weight
  if (doweights)
  {
    if (bi[0].cmmlr[MIN_LE] != bi_old[0].cmmlr[MIN_LE] || bi[0].cmmlr[MAX_LE] != bi_old[0].cmmlr[MAX_LE]
     || bi[0].wmmlr[MIN_LE] != bi_old[0].wmmlr[MIN_LE] || bi[0].wmmlr[MAX_LE] != bi_old[0].wmmlr[MAX_LE]) bi[1].update = 1;

    if (bi[0].cmmlr[MIN_RI] != bi_old[0].cmmlr[MIN_RI] || bi[0].cmmlr[MAX_RI] != bi_old[0].cmmlr[MAX_RI]
     || bi[0].wmmlr[MIN_RI] != bi_old[0].wmmlr[MIN_RI] || bi[0].wmmlr[MAX_RI] != bi_old[0].wmmlr[MAX_RI]) bi[-1].update = 1;

  } else
# endif
  {
    if (bi[0].cmmlr[MIN_LE] != bi_old[0].cmmlr[MIN_LE] || bi[0].cmmlr[MAX_LE] != bi_old[0].cmmlr[MAX_LE]) bi[1].update = 1;

    if (bi[0].cmmlr[MIN_RI] != bi_old[0].cmmlr[MIN_RI] || bi[0].cmmlr[MAX_RI] != bi_old[0].cmmlr[MAX_RI]) bi[-1].update = 1;
  }

  bi[0].update = 0;
#endif
}


void border_change(slint_t doweights, border_info_t *bi, border_info_t *bi_old, slint_t gcs, slint_t gc, slweight_t gws, slweight_t gw, slint_t dir) /* sl_func border_change */
{
  SL_TRACE_IF(MSEG_TRACE_IF, "change: gcs = %" slint_fmt ", gc = %" slint_fmt "", gcs, gc);

  bi[0].crange[0] += gcs;
  bi[0].crange[1] = bi[0].crange[0] + gc;

  SL_TRACE_IF(MSEG_TRACE_IF, "counts_range: %" slint_fmt "  %" slint_fmt "", bi[0].crange[0], bi[0].crange[1]);

#ifdef elem_weight
  if (doweights)
  {
    SL_TRACE_IF(MSEG_TRACE_IF, "change: gws = %" slweight_fmt ", gc = %" slweight_fmt, gws, gw);

    bi[0].wrange[0] += gws;
    bi[0].wrange[1] = bi[0].wrange[0] + gw;

    SL_TRACE_IF(MSEG_TRACE_IF, "weights_range: %" slweight_fmt "  %" slweight_fmt, bi[0].wrange[0], bi[0].wrange[1]);
  }
#endif

  /* forward or hit */
  if (dir >= 0)
  {
    bi[0].cmmlr[MIN_LE] = xminmax(bi[0].crange[0], bi[0].cmmlr[MIN_LE], bi[0].crange[1]);
    bi[0].cmmlr[MAX_LE] = xminmax(bi[0].crange[0], bi[0].cmmlr[MAX_LE], bi[0].crange[1]);

    SL_TRACE_IF(MSEG_TRACE_IF, "count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "",
      bi[0].cmmlr[MIN_LE], bi[0].cmmlr[MAX_LE], bi[0].cmmlr[MIN_RI], bi[0].cmmlr[MAX_RI]);

#ifdef elem_weight
    if (doweights)
    {
      bi[0].wmmlr[MIN_LE] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MIN_LE], bi[0].wrange[1]);
      bi[0].wmmlr[MAX_LE] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MAX_LE], bi[0].wrange[1]);

      SL_TRACE_IF(MSEG_TRACE_IF, "weight[min/max-left/right]: %" slweight_fmt " / %" slweight_fmt " - %" slweight_fmt " / %" slweight_fmt,
        bi[0].wmmlr[MIN_LE], bi[0].wmmlr[MAX_LE], bi[0].wmmlr[MIN_RI], bi[0].wmmlr[MAX_RI]);
    }
#endif

#ifndef MSEG_BORDER_UPDATE_FULL
    if (bi[0].cmmlr[MIN_LE] != bi_old[0].cmmlr[MIN_LE] || bi[0].cmmlr[MAX_LE] != bi_old[0].cmmlr[MAX_LE]
# ifdef elem_weight
     || (doweights && (bi[0].wmmlr[MIN_LE] != bi_old[0].wmmlr[MIN_LE] || bi[0].wmmlr[MAX_LE] != bi_old[0].wmmlr[MAX_LE]))
# endif
     ) bi[1].update = 1;
#endif
  }
  
  /* backward or hit */
  if (dir <= 0)
  {
    bi[0].cmmlr[MIN_RI] = xminmax(bi[0].crange[0], bi[0].cmmlr[MIN_RI], bi[0].crange[1]);
    bi[0].cmmlr[MAX_RI] = xminmax(bi[0].crange[0], bi[0].cmmlr[MAX_RI], bi[0].crange[1]);

    SL_TRACE_IF(MSEG_TRACE_IF, "count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "",
      bi[0].cmmlr[MIN_LE], bi[0].cmmlr[MAX_LE], bi[0].cmmlr[MIN_RI], bi[0].cmmlr[MAX_RI]);

#ifdef elem_weight
    if (doweights)
    {
      bi[0].wmmlr[MIN_RI] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MIN_RI], bi[0].wrange[1]);
      bi[0].wmmlr[MAX_RI] = xminmax(bi[0].wrange[0], bi[0].wmmlr[MAX_RI], bi[0].wrange[1]);

      SL_TRACE_IF(MSEG_TRACE_IF, "weight[min/max-left/right]: %" slweight_fmt " / %" slweight_fmt " - %" slweight_fmt " / %" slweight_fmt,
        bi[0].wmmlr[MIN_LE], bi[0].wmmlr[MAX_LE], bi[0].wmmlr[MIN_RI], bi[0].wmmlr[MAX_RI]);
    }
#endif

#ifndef MSEG_BORDER_UPDATE_FULL
    if (bi[0].cmmlr[MIN_RI] != bi_old[0].cmmlr[MIN_RI] || bi[0].cmmlr[MAX_RI] != bi_old[0].cmmlr[MAX_RI]
# ifdef elem_weight
     || (doweights && (bi[0].wmmlr[MIN_RI] != bi_old[0].wmmlr[MIN_RI] || bi[0].wmmlr[MAX_RI] != bi_old[0].wmmlr[MAX_RI]))
# endif
     ) bi[-1].update = 1;
#endif
  }

#ifndef MSEG_BORDER_UPDATE_FULL
  bi[0].update = 0;
#endif

  SL_TRACE_IF(MSEG_TRACE_IF, "range diff 0: %" slint_fmt "-%" slint_fmt " | %" slint_fmt "-%" slint_fmt,
    bi[0].crange[0] - bi[-1].crange[1], bi[0].crange[0] - bi[-1].crange[0],
    bi[1].crange[0] - bi[ 0].crange[0], bi[1].crange[1] - bi[ 0].crange[0]);
  SL_TRACE_IF(MSEG_TRACE_IF, "range diff 1: %" slint_fmt "-%" slint_fmt " | %" slint_fmt "-%" slint_fmt,
    bi[0].crange[1] - bi[-1].crange[1], bi[0].crange[1] - bi[-1].crange[0],
    bi[1].crange[0] - bi[ 0].crange[1], bi[1].crange[1] - bi[ 0].crange[1]);
}


void border_init(slint_t doweights, border_info_t *bi, partcond_intern_t *pc, slint_t tc, slweight_t tw) /* sl_func border_init */
{
#ifdef MSEG_BORDER_UPDATE_FULL
  bi[0].done = (pc == NULL);
#endif
  bi[0].update = (pc != NULL);

  bi[0].crange[0] = 0;
  bi[0].crange[1] = tc;

  SL_TRACE_IF(MSEG_TRACE_IF, "count range: %" slint_fmt " - %" slint_fmt "",
    bi[0].crange[0], bi[0].crange[1]);

  bi[0].cmmlr[MIN_LE] = bi[0].cmmlr[MAX_LE] = bi[0].cmmlr[MIN_RI] = bi[0].cmmlr[MAX_RI] = -1;

  if (pc) SL_TRACE_IF(MSEG_TRACE_IF, "pc-count: min/max: %" slint_fmt "/%" slint_fmt " - low/high: %" slint_fmt "/%" slint_fmt "", pc->count_min, pc->count_max, pc->count_low, pc->count_high);

#ifdef elem_weight
  if (doweights)
  {
    bi[0].wrange[0] = 0.0;
    bi[0].wrange[1] = tw;

    SL_TRACE_IF(MSEG_TRACE_IF, "weight range: %" slweight_fmt " - %" slweight_fmt,
      bi[0].wrange[0], bi[0].wrange[1]);

    bi[0].wmmlr[MIN_LE] = bi[0].wmmlr[MAX_LE] = bi[0].wmmlr[MIN_RI] = bi[0].wmmlr[MAX_RI] = -1.0;
  }

  if (pc) SL_TRACE_IF(MSEG_TRACE_IF, "pc-weight: min/max: %" slweight_fmt "/%" slweight_fmt " - low/high: %" slweight_fmt "/%" slweight_fmt, pc->weight_min, pc->weight_max, pc->weight_low, pc->weight_high);
#endif

  if (!pc)
  {
    bi[0].cmmlr[MIN_LE] = bi[0].cmmlr[MAX_LE] = 0;
    bi[0].cmmlr[MIN_RI] = bi[0].cmmlr[MAX_RI] = tc;

#ifdef elem_weight
    if (doweights)
    {
      bi[0].wmmlr[MIN_LE] = bi[0].wmmlr[MAX_LE] = 0.0;
      bi[0].wmmlr[MIN_RI] = bi[0].wmmlr[MAX_RI] = tw;
    }
#endif
  }

/*  SL_TRACE_IF(MSEG_TRACE_IF, "count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "",
    bi[0].cmmlr[MIN_LE], bi[0].cmmlr[MAX_LE], bi[0].cmmlr[MIN_RI], bi[0].cmmlr[MAX_RI]);

#ifdef elem_weight
  if (doweights)
    SL_TRACE_IF(MSEG_TRACE_IF, "weight[min/max-left/right]: %" slweight_fmt " / %" slweight_fmt " - %" slweight_fmt " / %" slweight_fmt,
      bi[0].wmmlr[MIN_LE], bi[0].wmmlr[MAX_LE], bi[0].wmmlr[MIN_RI], bi[0].wmmlr[MAX_RI]);
#endif*/
}


inline void border_currents(slint_t doweights, border_info_t *bi, slweight_t *currents) /* sl_func border_currents */
{
  SL_TRACE_IF(MSEG_TRACE_IF, "count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "",
    bi[0].cmmlr[MIN_LE], bi[0].cmmlr[MAX_LE], bi[0].cmmlr[MIN_RI], bi[0].cmmlr[MAX_RI]);

  SL_TRACE_IF(MSEG_TRACE_IF, "crange: %" slint_fmt " - %" slint_fmt "",
    bi[0].crange[0], bi[0].crange[1]);

  /* select highest min and lowest max */
  currents[COUNT_LO] = xmax(bi[0].cmmlr[MIN_LE], bi[0].cmmlr[MAX_RI]) - bi[0].crange[0];
  currents[COUNT_HI] = xmin(bi[0].cmmlr[MAX_LE], bi[0].cmmlr[MIN_RI]) - bi[0].crange[0];

  SL_TRACE_IF(MSEG_TRACE_IF, "currents count: %" slweight_fmt " - %" slweight_fmt, currents[COUNT_LO], currents[COUNT_HI]);

#ifdef elem_weight
  if (doweights)
  {
    SL_TRACE_IF(MSEG_TRACE_IF, "weight[min/max-left/right]: %" slweight_fmt " / %" slweight_fmt " - %" slweight_fmt " / %" slweight_fmt,
      bi[0].wmmlr[MIN_LE], bi[0].wmmlr[MAX_LE], bi[0].wmmlr[MIN_RI], bi[0].wmmlr[MAX_RI]);

    SL_TRACE_IF(MSEG_TRACE_IF, "wrange: %" slweight_fmt " - %" slweight_fmt, bi[0].wrange[0], bi[0].wrange[1]);

    /* select highest min and lowest max */
    currents[WEIGHT_LO] = xmax(bi[0].wmmlr[MIN_LE], bi[0].wmmlr[MAX_RI]) - bi[0].wrange[0];
    currents[WEIGHT_HI] = xmin(bi[0].wmmlr[MAX_LE], bi[0].wmmlr[MIN_RI]) - bi[0].wrange[0];

    SL_TRACE_IF(MSEG_TRACE_IF, "currents weight: %" slweight_fmt " - %" slweight_fmt, currents[WEIGHT_LO], currents[WEIGHT_HI]);
  }
#endif
}


slint_t mpi_select_exact_generic_bulk(elements_t *s, slint_t nelements, slint_t nparts, partcond_t *pconds, binning_t *bm, splitter_t *sp, int size, int rank, MPI_Comm comm) /* sl_proto, sl_func mpi_select_exact_generic_bulk */
{
  const slint_t max_nborders = nparts - 1;
  slint_t border_lo, border_hi, nborders_removed;
  slint_t borders[max_nborders], border_bins[max_nborders];

  border_info_t border_infos_[1 + max_nborders + 1], *border_infos = border_infos_ + 1, border_info_old;

  slint_t total_counts;
#ifdef elem_weight
  slweight_t total_weights;
#endif

  partcond_intern_t pci[nparts];

  slweight_t currents[2 * WEIGHT_FACTOR];

  slweight_t final_locals[WEIGHT_FACTOR], final_globals[WEIGHT_FACTOR];

  slint_t round, direction, refine, finalize;

  slweight_t mcw, dcw, lcw[WEIGHT_FACTOR], gcw[WEIGHT_FACTOR];

  slint_t gc, gcs;
#ifdef elem_weight
  slweight_t gw, gws;
#endif

  slint_t i, j, k, ix;

#ifdef elem_weight
  slint_t doweights, weight_factor;
#else
# define doweights  0
#endif

#ifdef VERIFY
  slint_t v;
#endif

  global_bins_t gb;


  SL_TRACE_IF(MSEG_TRACE_IF, "starting mpi_select_exact_generic");

  /* sl_tid rti_tid_mpi_select_exact_generic rti_tid_mpi_select_exact_generic_sync_init rti_tid_mpi_select_exact_generic_sync_exit */

  rti_treset(rti_tid_mpi_select_exact_generic_while);                    /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_generic_while_check);              /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_generic_while_check_bins);         /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_generic_while_check_bins_local);   /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_generic_while_check_bins_global);  /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_generic_while_check_round1);       /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_generic_while_check_pre);          /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_generic_while_check_part);         /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_generic_while_check_part_root);    /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_generic_while_check_final);        /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_generic_while_check_final_root);   /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_generic_while_check_post);         /* sl_tid */

  rti_tstart(rti_tid_mpi_select_exact_generic);

  rti_tstart(rti_tid_mpi_select_exact_generic_sync_init);
#ifdef SYNC_ON_INIT
  MPI_Barrier(comm);
#endif
  rti_tstop(rti_tid_mpi_select_exact_generic_sync_init);

#ifdef VERIFY
  v = elements_validate_order(s, 1);
  
  SL_TRACE_IF(MSEG_TRACE_IF, "elements order: %s (%" slint_fmt ")", (v > 0)?"FAILED":"SUCCESS", v);
#endif

#ifdef elem_weight
  doweights = ((pconds->pcm & (SLPC_WEIGHTS_MM|SLPC_WEIGHTS_LH)) != 0);
#endif

#ifdef elem_weight
  weight_factor = 1 + (doweights != 0);
# define MY_WEIGHT_FACTOR  weight_factor
#else
# define MY_WEIGHT_FACTOR  1
#endif

  mpi_binning_create(&gb, max_nborders, mseg_binnings, s, nelements, doweights, bm, size, rank, comm);

  /* init parts */
  border_lo = 0;
  border_hi = max_nborders - 1;
  for (i = border_lo; i <= border_hi; ++i)
  {
    borders[i] = i;
    border_bins[i] = 0;
  }

  /* reset splitter */
  sp->n = nparts * nelements;
  splitter_reset(sp);

  rti_tstart(rti_tid_mpi_select_exact_generic_while);

  direction = 1;

  round = 0;
  while (border_lo <= border_hi)
  {
    ++round;

    SL_TRACE_IF(MSEG_TRACE_IF, "ROUND: %" slint_fmt ", %s, %" slint_fmt " border(s)", round, (direction > 0)?"forward":"backward", border_hi - border_lo + 1);

    nborders_removed = 0;

    mpi_binning_pre(&gb, size, rank, comm);

    SL_TRACE_IF(MSEG_TRACE_IF, "ROUND: %" slint_fmt ", bm_nbins: %" slint_fmt, round, gb.bm->nbins);

    finalize = (gb.bm->nbins <= 1);

    rti_tstart(rti_tid_mpi_select_exact_generic_while_check);

    i = (direction > 0)?border_lo:border_hi;
    while ((direction > 0)?(i <= border_hi):(i >= border_lo))
    {
      SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ": PART: %" slint_fmt ",%" slint_fmt ": %s", round, i, borders[i], ((direction > 0)?"forward":"backward"));

      ix = i;

      rti_tstart(rti_tid_mpi_select_exact_generic_while_check_bins);

      if (!finalize || (finalize && round == 1 && i == border_lo))
      {
        mpi_binning_exec_reset(&gb, size, rank, comm);

        rti_tstart(rti_tid_mpi_select_exact_generic_while_check_bins_local);

        while ((direction > 0)?(ix <= border_hi):(ix >= border_lo))
        {
          SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": bin %" slint_fmt, ix, borders[ix], border_bins[ix]);

          if (mpi_binning_exec_local(&gb, border_bins[ix], size, rank, comm) < 0)
          {
            SL_TRACE_IF(MSEG_TRACE_IF, "break");
            break;
          }

          ix += direction;
        }

        rti_tstop(rti_tid_mpi_select_exact_generic_while_check_bins_local);

        SL_TRACE_IF(MSEG_TRACE_IF, "global %d (i = %" slint_fmt ", ix = %" slint_fmt ")", abs(ix - i), i, ix);

        rti_tstart(rti_tid_mpi_select_exact_generic_while_check_bins_global);

        mpi_binning_exec_global(&gb,
#ifdef MSEG_ROOT
          mseg_root,
#else
          -1,
#endif
          size, rank, comm);

        rti_tstop(rti_tid_mpi_select_exact_generic_while_check_bins_global);

      } else ix += direction;

      rti_tstop(rti_tid_mpi_select_exact_generic_while_check_bins);

      if (round == 1 && i == border_lo)
      {
        /* do initialization */
        rti_tstart(rti_tid_mpi_select_exact_generic_while_check_round1);

#ifdef MSEG_ROOT
        if (mseg_root < 0 || mseg_root == rank)
#endif
        {
          total_counts = 0;
#ifdef elem_weight
          total_weights = 0.0;
#endif
          for (j = 0; j < gb.bm->nbins; ++j)
          {
            total_counts += *gb_counts(&gb, border_bins[i], j);
#ifdef elem_weight
            if (doweights)
              total_weights += *gb_weights(&gb, border_bins[i], j);
#endif
          }

          SL_TRACE_IF(MSEG_TRACE_IF, "total_counts = %" slint_fmt, total_counts);
#ifdef elem_weight
          if (doweights)
            SL_TRACE_IF(MSEG_TRACE_IF, "total_weights = %" slweight_fmt , total_weights);
#endif

          init_partconds_intern(nparts, pci, pconds, nparts, total_counts, elem_weight_ifelse(total_weights, 0));

          /* init lowest and highest part (sentinels) */
          SL_TRACE_IF(MSEG_TRACE_IF, "init lowest border:");
          border_init(doweights, &border_infos[border_lo - 1], NULL, total_counts, elem_weight_ifelse(total_weights, 0));

          SL_TRACE_IF(MSEG_TRACE_IF, "init highest border:");
          border_init(doweights, &border_infos[border_hi + 1], NULL, total_counts, elem_weight_ifelse(total_weights, 0));

#ifdef MSEG_BORDER_UPDATE_PROPAGATION
          /* init+update forwards */
          for (j = border_lo; j <= border_hi; ++j)
          {
            SL_TRACE_IF(MSEG_TRACE_IF, "init border %" slint_fmt ",%" slint_fmt ":", j, borders[j]);
            border_init(doweights, &border_infos[borders[j]], &pci[borders[j]], total_counts, elem_weight_ifelse(total_weights, 0));

            SL_TRACE_IF(MSEG_TRACE_IF, "update update %" slint_fmt ",%" slint_fmt ":", j, borders[j]);
            border_update(doweights, &border_infos[borders[j]], &pci[borders[j]], 1, 0, 0);
          }
#endif

          /* [init+]update backwards */
          for (j = border_hi; j >= border_lo; --j)
          {
#ifndef MSEG_BORDER_UPDATE_PROPAGATION
            SL_TRACE_IF(MSEG_TRACE_IF, "init border %" slint_fmt ",%" slint_fmt ":", j, borders[j]);
            border_init(doweights, &border_infos[borders[j]], &pci[borders[j]], total_counts, elem_weight_ifelse(total_weights, 0));
#endif
            SL_TRACE_IF(MSEG_TRACE_IF, "update border %" slint_fmt ",%" slint_fmt ":", j, borders[j]);
            border_update(doweights, &border_infos[borders[j]], &pci[borders[j]], -1, 0, 1);
          }
        }
        
        rti_tstop(rti_tid_mpi_select_exact_generic_while_check_round1);
      }

do_partitioning:

      rti_tstart(rti_tid_mpi_select_exact_generic_while_check_pre);

#ifdef MSEG_ROOT
      if (mseg_root < 0 || mseg_root == rank)
#endif
      {
        SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": check", i, borders[i]);

        /* save to old limits */
        border_info_old = border_infos[borders[i]];

#ifdef MSEG_BORDER_UPDATE_FULL
        if (mseg_border_update_full)
          border_update_full(doweights, &border_infos[borders[i]], &pci[borders[i]], 1);
        else
#endif
        {
          /* is an update required? */
          if (border_infos[borders[i]].update) border_update(doweights, &border_infos[borders[i]], &pci[borders[i]], direction, 1, 1);
          else { SL_TRACE_IF(MSEG_TRACE_IF, ""); SL_TRACE_IF(MSEG_TRACE_IF, ""); SL_TRACE_IF(MSEG_TRACE_IF, ""); }
        }

        /* get currents */
        border_currents(doweights, &border_infos[borders[i]], currents);

        SL_ASSERT_IF((rank == 0), currents[COUNT_LO] <= currents[COUNT_HI]);
        SL_ASSERT_IF((rank == 0), 0 <= currents[COUNT_LO]);
        
        SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": currents count: %" slweight_fmt " - %" slweight_fmt " (range: %" slweight_fmt ")",
          i, borders[i], currents[COUNT_LO], currents[COUNT_HI], currents[COUNT_HI] - currents[COUNT_LO]);

#ifdef elem_weight
        if (doweights)
        {
          SL_ASSERT_IF((rank == 0), currents[WEIGHT_LO] <= currents[WEIGHT_HI]);

          SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": currents weight: %" slweight_fmt " - %" slweight_fmt " (range: %" slweight_fmt ")",
            i, borders[i], currents[WEIGHT_LO], currents[WEIGHT_HI], currents[WEIGHT_HI] - currents[WEIGHT_LO]);
        }
#endif
      }

      rti_tstop(rti_tid_mpi_select_exact_generic_while_check_pre);

      refine = 0;

      if (!finalize)
      {
        rti_tstart(rti_tid_mpi_select_exact_generic_while_check_part);

#ifdef MSEG_ROOT
        if (mseg_root < 0 || mseg_root == rank)
#endif
        {
          gcs = 0;
#ifdef elem_weight
          gws = 0.0;
#endif

          for (k = 0; k < gb.bm->nbins; ++k)
          {
            gc = *gb_counts(&gb, border_bins[i], k);

            currents[COUNT_LO] -= gc;
            currents[COUNT_HI] -= gc;

            SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": k = %" slint_fmt ", currents count: %" slweight_fmt " - %" slweight_fmt ", gc = %" slint_fmt ", gcs = %" slint_fmt,
              i, borders[i], k, currents[COUNT_LO], currents[COUNT_HI], gc, gcs);

#ifdef elem_weight
            if (doweights)
            {
              gw = *gb_weights(&gb, border_bins[i], k);

              currents[WEIGHT_LO] -= gw;
              currents[WEIGHT_HI] -= gw;

              SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": k = %" slint_fmt ", currents weight: %" slweight_fmt " - %" slweight_fmt ", gw = %" slweight_fmt ", gws = %" slweight_fmt,
                i, borders[i], k, currents[WEIGHT_LO], currents[WEIGHT_HI], gw, gws);
            }
#endif

            /* stop and refine if max count is skipped OR min count AND max weight is skipped */
            if ((currents[COUNT_HI] < 0)
#ifdef elem_weight
              || (doweights && currents[COUNT_LO] < 0 && currents[WEIGHT_HI] < 0.0)
#endif
              )
            {
              refine = 1;
              break;
            }

            gcs += gc;
            gc = 0;

#ifdef elem_weight
            gws += gw;
            gw = 0.0;
#endif

            /* if between min/max counts */
            if (currents[COUNT_LO] <= 0 && currents[COUNT_HI] >= 0)
            {
#ifdef elem_weight
              if (doweights)
              {
                SL_TRACE_IF(MSEG_TRACE_IF, "got to next: %d && %d", (currents[COUNT_HI] > 0), (currents[WEIGHT_LO] > 0));

                /* go to next if max count not reached AND min weight not reached */
                if (currents[COUNT_HI] > 0 && currents[WEIGHT_LO] > 0) continue;
              }
#endif

              /* look ahead for a better stop */
              if (k + 1 < gb.bm->nbins && currents[COUNT_HI] - *gb_counts(&gb, border_bins[i], k + 1) >= 0)
              {
#ifdef elem_weight
                if (doweights)
                {
                  /* continue if weights will improve */
                  if (xabs(currents[WEIGHT_LO] + currents[WEIGHT_HI]) > xabs(currents[WEIGHT_LO] + currents[WEIGHT_HI] - 2 * *gb_weights(&gb, border_bins[i], k + 1))) continue;

                } else
#endif
                {
                  /* continue if counts will improve */
                  if (xabs(currents[COUNT_LO] + currents[COUNT_HI]) > xabs(currents[COUNT_LO] + currents[COUNT_HI] - 2 * *gb_counts(&gb, border_bins[i], k + 1))) continue;
                }
              }

              /* stop */
              break;
            }
          }

          SL_ASSERT_IF((rank == 0), k < gb.bm->nbins);

          SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": %s k = %" slint_fmt, i, borders[i], (refine)?"REFINE":"HIT", k);

          /* make sure k is safe (it is used as index later) */
          if (k >= gb.bm->nbins) k = gb.bm->nbins - 1;

          k = (k + 1) * ((refine)?-1:1);
        }

#ifdef MSEG_ROOT
        rti_tstart(rti_tid_mpi_select_exact_generic_while_check_part_root);
        if (mseg_root >= 0) MPI_Bcast(&k, 1, int_mpi_datatype, mseg_root, comm);
        rti_tstop(rti_tid_mpi_select_exact_generic_while_check_part_root);
#endif

        refine = (k < 0);
        if (k < 0) k = -k;
        --k;

        /* refine or hit */
        if (refine)
        {
          SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": refine bin: %" slint_fmt " @ k = %" slint_fmt, i, borders[i], border_bins[i], k);

          border_bins[i] = mpi_binning_refine(&gb, border_bins[i], k, sp, borders[i] + 1, size, rank, comm);
          
          SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": new bin: %" slint_fmt, i, borders[i], border_bins[i]);

        } else
        {
          mpi_binning_hit(&gb, border_bins[i], k, sp, borders[i] + 1, size, rank, comm);
        }

        rti_tstop(rti_tid_mpi_select_exact_generic_while_check_part);

      } else
      {
        rti_tstart(rti_tid_mpi_select_exact_generic_while_check_final);

#ifdef MSEG_ROOT
        rti_tstart(rti_tid_mpi_select_exact_generic_while_check_final_root);
        if (mseg_root >= 0) MPI_Bcast(currents, 2 * MY_WEIGHT_FACTOR, weight_mpi_datatype, mseg_root, comm);
        rti_tstop(rti_tid_mpi_select_exact_generic_while_check_final_root);
#endif

        SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": currents count: %" slweight_fmt " - %" slweight_fmt,
          i, borders[i], currents[COUNT_LO], currents[COUNT_HI]);

#ifdef elem_weight
        if (doweights)
          SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": currents weight: %" slweight_fmt " - %" slweight_fmt,
            i, borders[i], currents[WEIGHT_LO], currents[WEIGHT_HI]);
#endif

        /* do exscan */
        final_locals[0] =
#ifdef elem_weight
          final_locals[1] =
#endif
          0.0;

        for (j = 0; j < nelements; ++j)
        {
          final_locals[0] += lb_bin_count(&gb.lb, border_bins[i], j);
#ifdef elem_weight
          final_locals[1] += lb_bin_weight(&gb.lb, border_bins[i], j);
#endif
        }
        
        SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": final_locals[0]: %" slweight_fmt, i, borders[i], final_locals[0]);
#ifdef elem_weight
        if (doweights)
          SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": final_locals[1]: %" slweight_fmt, i, borders[i], final_locals[1]);
#endif

        MPI_Exscan(final_locals, final_globals, MY_WEIGHT_FACTOR, weight_mpi_datatype, MPI_SUM, comm);
        if (rank == 0) final_globals[0] =
#ifdef elem_weight
          final_globals[1] =
#endif
          0.0;

        SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": final_globals[0]: %" slweight_fmt, i, borders[i], final_globals[0]);
#ifdef elem_weight
        if (doweights)
          SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": final_globals[1]: %" slweight_fmt, i, borders[i], final_globals[1]);
#endif

        k = 0;

#ifdef elem_weight
        if (doweights)
        {
          /* middle of min/max weight */
          mcw = (currents[WEIGHT_LO] + currents[WEIGHT_HI]) / 2.0;

          /* min. part of weight to contribute */
          dcw = xmax(0, mcw - final_globals[1]);

          SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": weights: mcw = %" slweight_fmt ", dcw = %" slweight_fmt, i, borders[i], mcw, dcw);

        } else
#endif
        {
          /* middle of min/max count */
          mcw = (currents[COUNT_LO] + currents[COUNT_HI]) / 2;

          /* min. part of count to contribute */
          dcw = xmax(0, mcw - final_globals[0]);

          SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": counts: mcw = %" slweight_fmt ", dcw = %" slweight_fmt, i, borders[i], mcw, dcw);
        }

        mpi_binning_finalize(&gb, border_bins[i], dcw, currents[COUNT_LO] - final_globals[0], currents[COUNT_HI] - final_globals[0], lcw, sp, borders[i] + 1, size, rank, comm);
        
        SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": lcw[0] = %" slweight_fmt, i, borders[i], lcw[0]);
#ifdef elem_weight
        if (doweights)
          SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": lcw[1] = %" slweight_fmt, i, borders[i], lcw[1]);
#endif

        gcs = gc = 0;
#ifdef elem_weight
        gws = gw = 0.0;
#endif

        SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": next border: %" slint_fmt " <= %" slint_fmt " + %" slint_fmt " <= %" slint_fmt,
          i, borders[i], border_lo, i, direction, border_hi);

        /* if the next open border is really the _next_ border */
        if (border_lo <= i + direction && i + direction <= border_hi && borders[i + direction] == borders[i] + direction)
        {
          SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": next border: %" slint_fmt " == %" slint_fmt " + %" slint_fmt,
            i, borders[i], borders[i + direction], borders[i], direction);

#ifdef elem_weight
          if (doweights)
          {
            /* need to determine the exact global counts/weights from the local counts/weights */
# ifdef MSEG_ROOT
            if (mseg_root >= 0) MPI_Reduce(lcw, gcw, MY_WEIGHT_FACTOR, weight_mpi_datatype, MPI_SUM, mseg_root, comm);
            else
# endif
            {
# ifdef MSEG_REDUCEBCAST_THRESHOLD
              if (size >= MSEG_REDUCEBCAST_THRESHOLD)
              {
                SL_TRACE_IF(MSEG_TRACE_IF, "%d >= %d: allreduce = reduce + bcast", size, (int) MSEG_REDUCEBCAST_THRESHOLD);

                MPI_Reduce(lcw, gcw, MY_WEIGHT_FACTOR, weight_mpi_datatype, MPI_SUM, REDUCEBCAST_ROOT, comm);
                MPI_Bcast(gcw, MY_WEIGHT_FACTOR, weight_mpi_datatype, REDUCEBCAST_ROOT, comm);

              } else
# endif
                MPI_Allreduce(lcw, gcw, MY_WEIGHT_FACTOR, weight_mpi_datatype, MPI_SUM, comm);
            }

          } else
#endif
          {
            /* global counts is just what we selected above */
            gcw[0] = mcw;
          }

#ifdef MSEG_ROOT
          if (mseg_root < 0 || mseg_root == rank)
#endif
          {
            gcs = gcw[0];
#ifdef elem_weight
            gws = gcw[1];
#endif
          }


          SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": gcs = %" slint_fmt, i, borders[i], gcs);
/*          SL_ASSERT(currents[COUNT_LO] <= gcs && gcs <= currents[COUNT_HI]);*/
          SL_ASSERT_IF(MSEG_TRACE_IF, currents[COUNT_LO] <= gcs && gcs <= currents[COUNT_HI]);
#ifdef elem_weight
          SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": gws = %" slweight_fmt, i, borders[i], gws);
          SL_ASSERT(currents[WEIGHT_LO] <= gws && gws <= currents[WEIGHT_HI]);
#endif
        }

        rti_tstop(rti_tid_mpi_select_exact_generic_while_check_final);
      }

      rti_tstart(rti_tid_mpi_select_exact_generic_while_check_post);

#ifdef MSEG_ROOT
      if (mseg_root < 0 || mseg_root == rank)
#endif
      {
/*        border_change_old(doweights, &border_infos[borders[i]], &border_info_old, gcs, gc, elem_weight_ifelse(gws, 0.0), elem_weight_ifelse(gw, 0.0));*/
        border_change(doweights, &border_infos[borders[i]], &border_info_old, gcs, gc, elem_weight_ifelse(gws, 0.0), elem_weight_ifelse(gw, 0.0), (refine)?direction:0);
      }
      
      SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ",%" slint_fmt ": %s", i, borders[i], (refine)?"REFINE":"REMOVE");

      if (refine)
      {
        borders[i - nborders_removed * direction] = borders[i];
        border_bins[i - nborders_removed * direction] = border_bins[i];

      } else
      {
        ++nborders_removed;
#ifdef MSEG_BORDER_UPDATE_FULL
        border_infos[borders[i]].done = 1;
#endif
      }

      rti_tstop(rti_tid_mpi_select_exact_generic_while_check_post);

      i += direction;
      
      SL_TRACE_IF(MSEG_TRACE_IF, "do partitioning: %" slint_fmt " vs. %" slint_fmt, i, ix);
      
      if (i != ix) goto do_partitioning;
    }
    rti_tstop(rti_tid_mpi_select_exact_generic_while_check);

    mpi_binning_post(&gb, size, rank, comm);

    /* restrict the parts */
    if (direction > 0) border_hi -= nborders_removed;
    else border_lo += nborders_removed;

    SL_TRACE_IF(MSEG_TRACE_IF, "%" slint_fmt ": remove %" slint_fmt", lo: %" slint_fmt ", hi: %" slint_fmt "", round, nborders_removed, border_lo, border_hi);

    /* change direction */
    direction *= -1;
  }
  
  mseg_rounds = round;

  rti_tstop(rti_tid_mpi_select_exact_generic_while);

  mpi_binning_destroy(&gb, size, rank, comm);

  rti_tstop(rti_tid_mpi_select_exact_generic);

  rti_tstart(rti_tid_mpi_select_exact_generic_sync_exit);
#ifdef SYNC_ON_EXIT
  MPI_Barrier(comm);
#endif
  rti_tstop(rti_tid_mpi_select_exact_generic_sync_exit);

#ifdef VERIFY
  if (size == 1) v = -1;
  else v = mpi_post_check_partconds_intern(s, nelements, nparts, pci, sp->displs, size, rank, comm);
  
  SL_ASSERT_IF(rank == 0, v < 0);
  
  SL_NOTICE_IF(rank == 0, "post_check_partconds: %s (%" slint_fmt ")", (v >= 0)?"FAILED":"SUCCESS", v);
#endif

#ifdef PRINT_SDISPLS
  printf("%d: sdispls:", rank);
  for (i = 0; i < nparts; ++i) printf(" %d ", sp->displs[i]);
  printf("\n");
#endif

#ifdef PRINT_STATS
  mpi_select_stats(s, nparts, sdispls, size, rank, comm);
#endif

#if defined(PRINT_TIMINGS) && defined(SL_USE_RTI_TIM)
  if (rank == PRINT_TIMINGS)
  {
    printf("%d: mpi_select_exact_generic: %f\n", rank, rti_tlast(rti_tid_mpi_select_exact_generic));
    printf("%d: mpi_select_exact_generic: sync init: %f\n", rank, rti_tlast(rti_tid_mpi_select_exact_generic_sync_init));
    printf("%d: mpi_select_exact_generic: while: %f\n", rank, rti_tlast(rti_tid_mpi_select_exact_generic_while));
    printf("%d: mpi_select_exact_generic:  check: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_generic_while_check));
    printf("%d: mpi_select_exact_generic:   bins: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_generic_while_check_bins));
    printf("%d: mpi_select_exact_generic:    local: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_generic_while_check_bins_local));
    printf("%d: mpi_select_exact_generic:    global: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_generic_while_check_bins_global));
    printf("%d: mpi_select_exact_generic:   round1: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_generic_while_check_round1));
    printf("%d: mpi_select_exact_generic:   pre: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_generic_while_check_pre));
    printf("%d: mpi_select_exact_generic:   part: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_generic_while_check_part));
    printf("%d: mpi_select_exact_generic:    root: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_generic_while_check_part_root));
    printf("%d: mpi_select_exact_generic:   final: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_generic_while_check_final));
    printf("%d: mpi_select_exact_generic:    root: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_generic_while_check_final_root));
    printf("%d: mpi_select_exact_generic:   post: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_generic_while_check_post));
    printf("%d: mpi_select_exact_generic: sync exit: %f\n", rank, rti_tlast(rti_tid_mpi_select_exact_generic_sync_exit));
    printf("%d: mpi_select_exact_generic: rounds: %" slint_fmt "\n", rank, round);
  }
#endif

  return 0;
}


slint_t mpi_select_exact_generic_grouped(elements_t *s, slint_t nelements, partcond_t *pcond, MPI_Comm pcond_comm, MPI_Comm group_comm, binning_t *bm, splitter_t *sp, int size, int rank, MPI_Comm comm) /* sl_proto, sl_func mpi_select_exact_generic_grouped */
{
  slint_t npconds = -1;
  partcond_t *pconds;


  mpi_gather_partconds_grouped(pcond, pcond_comm, group_comm, NULL, &npconds, size, rank, comm);

  pconds = sl_alloca(npconds, sizeof(partcond_t));
  
  mpi_gather_partconds_grouped(pcond, pcond_comm, group_comm, pconds, &npconds, size, rank, comm);

  mpi_select_exact_generic_bulk(s, nelements, npconds, pconds, bm, sp, size, rank, comm);
  
  sl_freea(pconds);
  
  return 0;
}


#if 0
slint_t mpi_select_exact_generic(elements_t *s, slint_t nelements, slint_t nparts, partcond_t *pconds, binning_t *bm, splitter_t *sp, int size, int rank, MPI_Comm comm) /* sl_proto, sl_func mpi_select_exact_generic */
{
  const slint_t max_nborders = nparts - 1;
  slint_t border_lo, border_hi, nborders_removed;
/*  slint_t borders[max_nborders], border_bins[max_nborders];

  border_info_t border_infos_[1 + max_nborders + 1], *border_infos = border_infos_ + 1, border_info_old;*/

  slint_t round, direction;

  slint_t i;
/*  slint_t i, j, k, l;*/

  slint_t curr_l, curr_g, curr_p;
  slint_t next_l, next_g, next_p;
  
  slint_t binning_at_once = 1;
  

  border_lo = 0;
  border_hi = max_nborders - 1;

  direction = 1;

  round = 0;
  while (border_lo <= border_hi)
  {
    ++round;

    nborders_removed = 0;

    i = (direction > 0)?border_lo:border_hi;

    next_l = i;
    next_g = -1;
    next_p = -1;
    
    while ((direction > 0)?(i <= border_hi):(i >= border_lo))
    {
      curr_l = next_l;
      curr_g = next_g;
      curr_p = next_p;


      if (border_lo <= curr_g && curr_g <= border_hi)
      {
        /* init global binning at curr_g */
      
        next_p = curr_g;
      }

      if (border_lo <= curr_p && curr_p <= border_hi)
      {
        /* wait global binning at curr_p */
        
      
        /* partitioning at curr_p */
        
        i += binning_at_once * direction;
      }
      
      if (border_lo <= curr_l && curr_l <= border_hi)
      {
        /* local binning at curr_l */

        next_l += binning_at_once * direction;
        next_g = curr_l;
      }
    }

    /* restrict the parts */
    if (direction > 0) border_hi -= nborders_removed;
    else border_lo += nborders_removed;

    /* change direction */
    direction *= -1;
  }
  
  return 0;
}
#endif
