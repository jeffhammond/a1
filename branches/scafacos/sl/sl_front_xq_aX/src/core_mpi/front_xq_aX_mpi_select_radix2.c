/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/core_mpi/mpi_select_radix2.c
 *  timestamp: 2011-01-13 09:48:59 +0100
 *  
 */


#include "sl_common.h"


#define radix_low                 0
#define radix_high                (sizeof(slkey_pure_t) * 8 - 1)
#define radix_key2class(k, x, y)  (((k) >> (x)) & y)

#define myabs(x)  (((x) >= 0)?(x):-(x))

#define slint_fmt  sl_int_type_fmt

#ifdef elem_weight
# define NCONDS  2
#else
# define NCONDS  1
#endif

/* config */
#define SYNC_ON_INIT
#define HAVENT_MPI_IN_PLACE

/* sl_macro MPI_SELECT_RADIX_REDUCEBCAST_THRESHOLD */
#define REDUCEBCAST_ROOT  0

/*#define PRINT_STATS
#define PRINT_TIMINGS*/

#define DEBUG_OR_NOT  (sl_mpi_rank == -1)


slint_t mpi_select_radix2(elements_t *s, slint_t nparts, partcond_t *pconds, slint_t rhigh, slint_t rlow, slint_t rwidth, int *sdispls, int size, int rank, MPI_Comm comm) /* sl_proto, sl_func mpi_select_radix2 */
{
  slkey_pure_t max_nclasses, nclasses, bit_mask;
  slkey_pure_t k;

  typedef struct {
    slint_t count_min, count_max;
    slint_t count_low, count_hig;
#ifdef elem_weight
    double weight_min, weight_max;
    double weight_low, weight_hig;
#endif
  } mmlh_t;

  mmlh_t mmlh[nparts];

  const slint_t max_nborders = nparts - 1;
  slint_t border_lo, border_hi, nborders_removed;
  slint_t borders[max_nborders], border_areas[max_nborders];

#define MIN_LE  0
#define MIN_RI  1
#define MAX_LE  2
#define MAX_RI  3

  struct {
    slint_t update;
    slint_t crange[2], cmmlr[4];
#ifdef elem_weight
    double wrange[2], wmmlr[4];
#endif
  } border_infos_[1 + max_nborders + 1], *border_infos = border_infos_ + 1, border_info_old;

  const slint_t max_nareas = max_nborders;
  slint_t nareas, nareas_new;
  elements_t areas0[max_nareas], areas1[max_nareas], *areas, *areas_new;

  double *local_counts, *global_counts;
#ifdef elem_weight
  double *local_weights, *global_weights;
#endif

  slint_t current_cmm[2];
#ifdef elem_weight
  double current_wmm[2];
#endif

  double final_locals[NCONDS * max_nborders], *final_globals;

  slint_t current_width;
  slint_t round, direction, refine, finalize;
  slint_t last_new_area, last_new_class;

  slint_t lc, lcs, gc, gcs;
#ifdef elem_weight
  double lw, gw, lws, gws;
  double mw, dw;
#else
  slint_t mc, dc;
#endif

  slint_t i, j;

  elements_t xi, end;


  rti_treset(rti_tid_mpi_select_radix_while);                   /* sl_tid */
  rti_treset(rti_tid_mpi_select_radix_while_count);             /* sl_tid */
  rti_treset(rti_tid_mpi_select_radix_while_allreduce);         /* sl_tid */
  rti_treset(rti_tid_mpi_select_radix_while_round1);            /* sl_tid */
  rti_treset(rti_tid_mpi_select_radix_while_round1_allgather);  /* sl_tid */
  rti_treset(rti_tid_mpi_select_radix_while_exscan);            /* sl_tid */
  rti_treset(rti_tid_mpi_select_radix_while_check);             /* sl_tid */
  rti_treset(rti_tid_mpi_select_radix_while_check_pre);         /* sl_tid */
  rti_treset(rti_tid_mpi_select_radix_while_check_classes);     /* sl_tid */
  rti_treset(rti_tid_mpi_select_radix_while_check_final);       /* sl_tid */
  rti_treset(rti_tid_mpi_select_radix_while_check_post);        /* sl_tid */

  rti_tstart(rti_tid_mpi_select_radix);  /* sl_tid */

  rti_tstart(rti_tid_mpi_select_radix_sync);  /* sl_tid */
#ifdef SYNC_ON_INIT
  MPI_Barrier(comm);
#endif
  rti_tstop(rti_tid_mpi_select_radix_sync);

  if (rhigh < 0) rhigh = radix_high;
  if (rlow < 0) rlow = radix_low;
  if (rwidth < 0) rwidth = sort_radix_width_default;
  
  max_nclasses = powof2_typed(rwidth, slkey_pure_t);

  local_counts = sl_alloc(NCONDS * (max_nareas * max_nclasses + max_nareas), sizeof(double));
  global_counts = sl_alloc(NCONDS * (max_nareas * max_nclasses + max_nareas), sizeof(double));

  /* init areas (first area = all elements) */
  areas = areas0;
  areas_new = areas1;

  nareas = 1;
  elem_assign(s, &areas[0]);

  /* init parts */
  border_lo = 0;
  border_hi = max_nborders - 1;
  for (i = border_lo; i <= border_hi; ++i)
  {
    borders[i] = i;
    border_areas[i] = 0;
  }

  /* init sdispls */
  for (i = 0; i < nparts; ++i) sdispls[i] = 0;

  rti_tstart(rti_tid_mpi_select_radix_while);

  round = 0;
  while (border_lo <= border_hi)
  {
    ++round;

    /* setup bitmask */
    current_width = xmin(rwidth, rhigh - rlow + 1);
    rhigh -= (current_width > 0)?current_width - 1:rhigh;

    nclasses = (current_width > 0)?powof2_typed(current_width, slkey_pure_t):1;
    bit_mask = nclasses - 1;

    SL_TRACE_IF(DEBUG_OR_NOT, "ROUND: %" slint_fmt ", rhigh: %" slint_fmt ", current_width: %" slint_fmt ", nclasses: %" sl_key_pure_type_fmt, round, rhigh, current_width, nclasses);

    finalize = (current_width <= 0);

    if (!finalize || round == 1)
    {
#ifdef elem_weight
      /* init weight counters */
      local_weights = local_counts + (nareas * nclasses) + nareas;
      global_weights = global_counts + (nareas * nclasses) + nareas;
#endif

      /* zero all counter */
      for (i = 0; i < nareas; ++i)
      for (k = 0; k < nclasses; ++k) local_counts[i * nclasses + k] = 
#ifdef elem_weight
  local_weights[i * nclasses + k] = 
#endif
  0.0;

      rti_tstart(rti_tid_mpi_select_radix_while_count);

      /* for every area */
      for (i = 0; i < nareas; ++i)
      {
        elem_assign_at(&areas[i], areas[i].size, &end);

        if (nclasses > 1)
        {
          /* counts and weights in every class */
          for (elem_assign(&areas[i], &xi); xi.keys < end.keys; elem_inc(&xi))
          {
            k = radix_key2class(key_purify(*xi.keys), rhigh, bit_mask);
            local_counts[i * nclasses + k] += 1;
#ifdef elem_weight
            local_weights[i * nclasses + k] += elem_weight(&xi, 0);
#endif
          }

        } else
        {
          /* total counts and weights */
          local_counts[i * nclasses + 0] = areas[i].size;

#ifdef elem_weight
          for (elem_assign(&areas[i], &xi); xi.keys < end.keys; elem_inc(&xi)) local_weights[i * nclasses + 0] += elem_weight(&xi, 0);
#endif
        }

        /* total counts and weights in this area */
        local_counts[nareas * nclasses + i] = areas[i].size;

#ifdef elem_weight
        local_weights[nareas * nclasses + i] = 0.0;
        for (k = 0; k < nclasses; ++k) local_weights[nareas * nclasses + i] += local_weights[i * nclasses + k];
#endif
      }

      rti_tstop(rti_tid_mpi_select_radix_while_count);

      --rhigh;

      rti_tstart(rti_tid_mpi_select_radix_while_allreduce);

      /* create global counts and weights */
#ifdef MPI_SELECT_RADIX_REDUCEBCAST_THRESHOLD
      if (size >= MPI_SELECT_RADIX_REDUCEBCAST_THRESHOLD)
      {
        MPI_Reduce(local_counts, global_counts, NCONDS * (nareas * nclasses + nareas), MPI_DOUBLE, MPI_SUM, REDUCEBCAST_ROOT, comm);
        MPI_Bcast(global_counts, NCONDS * (nareas * nclasses + nareas), MPI_DOUBLE, REDUCEBCAST_ROOT, comm);

      } else
#endif
        MPI_Allreduce(local_counts, global_counts, NCONDS * (nareas * nclasses + nareas), MPI_DOUBLE, MPI_SUM, comm);

      rti_tstop(rti_tid_mpi_select_radix_while_allreduce);
    }

    /* do initializations */
    if (round == 1)
    {
      rti_tstart(rti_tid_mpi_select_radix_while_round1);

      for (i = 0; i < nparts; ++i)
      {
        /* set default values and determine local (count/weight) limits */
        init_partconds(1, &pconds[i], nparts, global_counts[nareas * nclasses + 0],
#ifdef elem_weight
          global_weights[nareas * nclasses + 0]
#else
          0
#endif
);

        mmlh[i].count_min = pconds[i].count_min;
        mmlh[i].count_max = pconds[i].count_max;
        mmlh[i].count_low = pconds[i].count_low;
        mmlh[i].count_hig = pconds[i].count_high;

#ifdef elem_weight
        mmlh[i].weight_min = pconds[i].weight_min;
        mmlh[i].weight_max = pconds[i].weight_max;
        mmlh[i].weight_low = pconds[i].weight_low;
        mmlh[i].weight_hig = pconds[i].weight_high;
#endif
      }

      /* init lowest and highest part (sentinels) */
      border_infos[border_lo - 1].update = 0;
      border_infos[border_lo - 1].crange[0] = 0;
      border_infos[border_lo - 1].crange[1] = global_counts[nareas * nclasses + 0];
      border_infos[border_lo - 1].cmmlr[MIN_LE] = border_infos[border_lo - 1].cmmlr[MAX_LE] = 0;
      border_infos[border_lo - 1].cmmlr[MIN_RI] = border_infos[border_lo - 1].cmmlr[MAX_RI] = 0;

      SL_TRACE_IF(DEBUG_OR_NOT, "lowest: %" slint_fmt ": init count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "", border_lo - 1,
        border_infos[border_lo - 1].cmmlr[MIN_LE], border_infos[border_lo - 1].cmmlr[MAX_LE], border_infos[border_lo - 1].cmmlr[MIN_RI], border_infos[border_lo - 1].cmmlr[MAX_RI]);

#ifdef elem_weight
      border_infos[border_lo - 1].wrange[0] = 0.0;
      border_infos[border_lo - 1].wrange[1] = global_weights[nareas * nclasses + 0];
      border_infos[border_lo - 1].wmmlr[MIN_LE] = border_infos[border_lo - 1].wmmlr[MAX_LE] = 0.0;
      border_infos[border_lo - 1].wmmlr[MIN_RI] = border_infos[border_lo - 1].wmmlr[MAX_RI] = 0.0;

      SL_TRACE_IF(DEBUG_OR_NOT, "lowest: %" slint_fmt ": init weight[min/max-left/right]: %f / %f - %f / %f", border_lo - 1,
        border_infos[border_lo - 1].wmmlr[MIN_LE], border_infos[border_lo - 1].wmmlr[MAX_LE], border_infos[border_lo - 1].wmmlr[MIN_RI], border_infos[border_lo - 1].wmmlr[MAX_RI]);
#endif

      /* init highest part (sentinel) */
      border_infos[border_hi + 1].update = 0;
      border_infos[border_hi + 1].crange[0] = 0;
      border_infos[border_hi + 1].crange[1] = global_counts[nareas * nclasses + 0];
      border_infos[border_hi + 1].cmmlr[MIN_LE] = border_infos[border_hi + 1].cmmlr[MAX_LE] = 0;
      border_infos[border_hi + 1].cmmlr[MIN_RI] = border_infos[border_hi + 1].cmmlr[MAX_RI] = global_counts[nareas * nclasses + 0];

      SL_TRACE_IF(DEBUG_OR_NOT, "highest: %" slint_fmt ": init count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "", border_hi + 1,
        border_infos[border_hi + 1].cmmlr[MIN_LE], border_infos[border_hi + 1].cmmlr[MAX_LE], border_infos[border_hi + 1].cmmlr[MIN_RI], border_infos[border_hi + 1].cmmlr[MAX_RI]);

#ifdef elem_weight
      border_infos[border_hi + 1].wrange[0] = 0.0;
      border_infos[border_hi + 1].wrange[1] = global_weights[nareas * nclasses + 0];
      border_infos[border_hi + 1].wmmlr[MIN_LE] = border_infos[border_hi + 1].wmmlr[MAX_LE] = 0.0;
      border_infos[border_hi + 1].wmmlr[MIN_RI] = border_infos[border_hi + 1].wmmlr[MAX_RI] = global_weights[nareas * nclasses + 0];

      SL_TRACE_IF(DEBUG_OR_NOT, "highest: %" slint_fmt ": init weight[min/max-left/right]: %f / %f - %f / %f", border_hi + 1,
        border_infos[border_hi + 1].wmmlr[MIN_LE], border_infos[border_hi + 1].wmmlr[MAX_LE], border_infos[border_hi + 1].wmmlr[MIN_RI], border_infos[border_hi + 1].wmmlr[MAX_RI]);
#endif

      /* init regular parts (backwards) */
      for (i = border_hi; i >= border_lo; --i)
      {
        border_infos[borders[i]].update = 1;
        border_infos[borders[i]].crange[0] = 0;
        border_infos[borders[i]].crange[1] = global_counts[nareas * nclasses + 0];
        border_infos[borders[i]].cmmlr[MIN_LE] = -1;
        border_infos[borders[i]].cmmlr[MIN_RI] = border_infos[borders[i] + 1].cmmlr[MIN_RI] - mmlh[borders[i] + 1].count_min;
        border_infos[borders[i]].cmmlr[MAX_LE] = -1;
        border_infos[borders[i]].cmmlr[MAX_RI] = border_infos[borders[i] + 1].cmmlr[MAX_RI] - mmlh[borders[i] + 1].count_max;

        SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": init count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "", i, borders[i],
          border_infos[borders[i]].cmmlr[MIN_LE], border_infos[borders[i]].cmmlr[MAX_LE], border_infos[borders[i]].cmmlr[MIN_RI], border_infos[borders[i]].cmmlr[MAX_RI]);

#ifdef elem_weight
        border_infos[borders[i]].wrange[0] = 0.0;
        border_infos[borders[i]].wrange[1] = global_weights[nareas * nclasses + 0];
        border_infos[borders[i]].wmmlr[MIN_LE] = -1.0;
        border_infos[borders[i]].wmmlr[MIN_RI] = border_infos[borders[i] + 1].wmmlr[MIN_RI] - mmlh[borders[i] + 1].weight_min;
        border_infos[borders[i]].wmmlr[MAX_LE] = -1.0;
        border_infos[borders[i]].wmmlr[MAX_RI] = border_infos[borders[i] + 1].wmmlr[MAX_RI] - mmlh[borders[i] + 1].weight_max;

        SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": init weight[min/max-left/right]: %f / %f - %f / %f", i, borders[i],
          border_infos[borders[i]].wmmlr[MIN_LE], border_infos[borders[i]].wmmlr[MAX_LE], border_infos[borders[i]].wmmlr[MIN_RI], border_infos[borders[i]].wmmlr[MAX_RI]);
#endif

        /* prepare for finalization in the 1st round */
        if (finalize)
        {
          final_locals[NCONDS * i + 0] = local_counts[nareas * nclasses + 0];
#ifdef elem_weight
          final_locals[NCONDS * i + 1] = local_weights[nareas * nclasses + 0];
#endif
        }
      }
      
      /* first direction: forward */
      direction = 1;

      rti_tstop(rti_tid_mpi_select_radix_while_round1);
    }

    /* compute prefixes for finalization */
    if (finalize)
    {
      /* determine number of parts to finalize */
      j = border_hi - border_lo + 1;
    
      SL_TRACE_IF(DEBUG_OR_NOT, "Exscan: finalizing %" slint_fmt " parts", j);

      rti_tstart(rti_tid_mpi_select_radix_while_exscan);

      /* use local_counts to store the global prefix sums */      
      final_globals = local_counts;

      /* create global prefix sums (set rank 0 to zero) */
      MPI_Exscan(&final_locals[NCONDS * border_lo], &final_globals[NCONDS * border_lo], NCONDS * j, MPI_DOUBLE, MPI_SUM, comm);
      if (rank == 0) for (i = border_lo; i <= border_hi; ++i) final_globals[NCONDS * i + 0] = 
#ifdef elem_weight
        final_globals[NCONDS * i + 1] = 
#endif
        0.0;

      rti_tstop(rti_tid_mpi_select_radix_while_exscan);
    }

    /* check all remaining parts */
    SL_TRACE_IF(DEBUG_OR_NOT, "ROUND: %" slint_fmt ", %s", round, (direction > 0)?"forward":"backward");

    nareas_new = 0;
    last_new_area = last_new_class = -1;
    nborders_removed = 0;

    rti_tstart(rti_tid_mpi_select_radix_while_check);

    i = (direction > 0)?border_lo:border_hi;
    while ((direction > 0)?(i <= border_hi):(i >= border_lo))
    {
      /* check partition borders[i] */
      SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ": PART: %" slint_fmt ",%" slint_fmt, round, i, borders[i]);

      rti_tstart(rti_tid_mpi_select_radix_while_check_pre);

      /* save to old limits */
      border_info_old = border_infos[borders[i]];

      /* is an update required? */
      if (border_infos[borders[i]].update)
      {
        /* forward */
        if (direction > 0)
        {
          /* init from min/max (always) */
          border_infos[borders[i]].cmmlr[MIN_LE] = border_infos[borders[i] - 1].cmmlr[MIN_LE] + mmlh[borders[i]].count_min;
          border_infos[borders[i]].cmmlr[MAX_LE] = border_infos[borders[i] - 1].cmmlr[MAX_LE] + mmlh[borders[i]].count_max;

          SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": count[min/max-left]: %" slint_fmt " + %" slint_fmt ", %" slint_fmt " + %" slint_fmt "", i, borders[i],
            border_infos[borders[i] - 1].cmmlr[MIN_LE], mmlh[borders[i]].count_min,
            border_infos[borders[i] - 1].cmmlr[MAX_LE], mmlh[borders[i]].count_max);

          /* check against low/high (on demand) */
          if (pconds->pcm & SLPC_COUNTS_LH)
          {
            if (border_infos[borders[i]].cmmlr[MIN_LE] < mmlh[borders[i] + 1].count_low) border_infos[borders[i]].cmmlr[MIN_LE] = mmlh[borders[i] + 1].count_low;
            if (border_infos[borders[i]].cmmlr[MAX_LE] > mmlh[borders[i]    ].count_hig) border_infos[borders[i]].cmmlr[MAX_LE] = mmlh[borders[i]    ].count_hig;
          }

#ifdef elem_weight
          /* init from min/max (always) */
          border_infos[borders[i]].wmmlr[MIN_LE] = border_infos[borders[i] - 1].wmmlr[MIN_LE] + mmlh[borders[i]].weight_min;
          border_infos[borders[i]].wmmlr[MAX_LE] = border_infos[borders[i] - 1].wmmlr[MAX_LE] + mmlh[borders[i]].weight_max;

          SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": weight[min/max-left]: %f + %f, %f + %f", i, borders[i],
            border_infos[borders[i] - 1].wmmlr[MIN_LE], mmlh[borders[i]].weight_min,
            border_infos[borders[i] - 1].wmmlr[MAX_LE], mmlh[borders[i]].weight_max);

          /* check against low/high (on demand) */
          if (pconds->pcm & SLPC_WEIGHTS_LH)
          {
            if (border_infos[borders[i]].wmmlr[MIN_LE] < mmlh[borders[i] + 1].weight_low) border_infos[borders[i]].wmmlr[MIN_LE] = mmlh[borders[i] + 1].weight_low;
            if (border_infos[borders[i]].wmmlr[MAX_LE] > mmlh[borders[i]    ].weight_hig) border_infos[borders[i]].wmmlr[MAX_LE] = mmlh[borders[i]    ].weight_hig;
          }
#endif
        } else /* backward */
        {
          /* init from min/max (always) */
          border_infos[borders[i]].cmmlr[MIN_RI] = border_infos[borders[i] + 1].cmmlr[MIN_RI] - mmlh[borders[i] + 1].count_min;
          border_infos[borders[i]].cmmlr[MAX_RI] = border_infos[borders[i] + 1].cmmlr[MAX_RI] - mmlh[borders[i] + 1].count_max;

          SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": count[min/max-right]: %" slint_fmt " - %" slint_fmt ", %" slint_fmt " - %" slint_fmt "", i, borders[i],
            border_infos[borders[i] + 1].cmmlr[MIN_RI], mmlh[borders[i] + 1].count_min,
            border_infos[borders[i] + 1].cmmlr[MAX_RI], mmlh[borders[i] + 1].count_max);

          /* check against low/high (on demand) */
          if (pconds->pcm & SLPC_COUNTS_LH)
          {
            if (border_infos[borders[i]].cmmlr[MAX_RI] < mmlh[borders[i] + 1].count_low) border_infos[borders[i]].cmmlr[MAX_RI] = mmlh[borders[i] + 1].count_low;
            if (border_infos[borders[i]].cmmlr[MIN_RI] > mmlh[borders[i]    ].count_hig) border_infos[borders[i]].cmmlr[MIN_RI] = mmlh[borders[i]    ].count_hig;
          }

#ifdef elem_weight
          /* init from min/max (always) */
          border_infos[borders[i]].wmmlr[MIN_RI] = border_infos[borders[i] + 1].wmmlr[MIN_RI] - mmlh[borders[i] + 1].weight_min;
          border_infos[borders[i]].wmmlr[MAX_RI] = border_infos[borders[i] + 1].wmmlr[MAX_RI] - mmlh[borders[i] + 1].weight_max;

          SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": weight[min/max-right]: %f - %f, %f - %f", i, borders[i],
            border_infos[borders[i] + 1].wmmlr[MIN_RI], mmlh[borders[i] + 1].weight_min,
            border_infos[borders[i] + 1].wmmlr[MAX_RI], mmlh[borders[i] + 1].weight_max);

          /* check against low/high (on demand) */
          if (pconds->pcm & SLPC_WEIGHTS_LH)
          {
            if (border_infos[borders[i]].wmmlr[MAX_RI] < mmlh[borders[i] + 1].weight_low) border_infos[borders[i]].wmmlr[MAX_RI] = mmlh[borders[i] + 1].weight_low;
            if (border_infos[borders[i]].wmmlr[MIN_RI] > mmlh[borders[i]    ].weight_hig) border_infos[borders[i]].wmmlr[MIN_RI] = mmlh[borders[i]    ].weight_hig;
          }
#endif
        }

        SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "", i, borders[i],
          border_infos[borders[i]].cmmlr[MIN_LE], border_infos[borders[i]].cmmlr[MAX_LE], border_infos[borders[i]].cmmlr[MIN_RI], border_infos[borders[i]].cmmlr[MAX_RI]);

        /* check against inconsistence */
        if (border_infos[borders[i]].cmmlr[MIN_LE] > border_infos[borders[i]].cmmlr[MIN_RI]) border_infos[borders[i]].cmmlr[MIN_LE] = border_infos[borders[i]].cmmlr[MIN_RI] = (border_infos[borders[i]].cmmlr[MIN_LE] + border_infos[borders[i]].cmmlr[MIN_RI]) / 2;
        if (border_infos[borders[i]].cmmlr[MAX_LE] < border_infos[borders[i]].cmmlr[MAX_RI]) border_infos[borders[i]].cmmlr[MAX_LE] = border_infos[borders[i]].cmmlr[MAX_RI] = (border_infos[borders[i]].cmmlr[MAX_LE] + border_infos[borders[i]].cmmlr[MAX_RI]) / 2;

#ifdef elem_weight
        SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": weight[min/max-left/right]: %f / %f - %f / %f", i, borders[i],
          border_infos[borders[i]].wmmlr[MIN_LE], border_infos[borders[i]].wmmlr[MAX_LE], border_infos[borders[i]].wmmlr[MIN_RI], border_infos[borders[i]].wmmlr[MAX_RI]);

        /* check against inconsistence */
        if (border_infos[borders[i]].wmmlr[MIN_LE] > border_infos[borders[i]].wmmlr[MIN_RI]) border_infos[borders[i]].wmmlr[MIN_LE] = border_infos[borders[i]].wmmlr[MIN_RI] = (border_infos[borders[i]].wmmlr[MIN_LE] + border_infos[borders[i]].wmmlr[MIN_RI]) / 2;
        if (border_infos[borders[i]].wmmlr[MAX_LE] < border_infos[borders[i]].wmmlr[MAX_RI]) border_infos[borders[i]].wmmlr[MAX_LE] = border_infos[borders[i]].wmmlr[MAX_RI] = (border_infos[borders[i]].wmmlr[MAX_LE] + border_infos[borders[i]].wmmlr[MAX_RI]) / 2;
#endif
      }

      SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "", i, borders[i],
        border_infos[borders[i]].cmmlr[MIN_LE], border_infos[borders[i]].cmmlr[MAX_LE], border_infos[borders[i]].cmmlr[MIN_RI], border_infos[borders[i]].cmmlr[MAX_RI]);

      /* select highest min and lowest max */
      current_cmm[0] = xmax(border_infos[borders[i]].cmmlr[MIN_LE], border_infos[borders[i]].cmmlr[MAX_RI]) - border_infos[borders[i]].crange[0];
      current_cmm[1] = xmin(border_infos[borders[i]].cmmlr[MAX_LE], border_infos[borders[i]].cmmlr[MIN_RI]) - border_infos[borders[i]].crange[0];

      SL_ASSERT(current_cmm[0] <= current_cmm[1]);

      SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": current_count: %" slint_fmt " - %" slint_fmt "", i, borders[i], current_cmm[0], current_cmm[1]);

#ifdef elem_weight
      SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": weight[min/max-left/right]: %f / %f - %f / %f", i, borders[i],
        border_infos[borders[i]].wmmlr[MIN_LE], border_infos[borders[i]].wmmlr[MAX_LE], border_infos[borders[i]].wmmlr[MIN_RI], border_infos[borders[i]].wmmlr[MAX_RI]);

      /* select highest min and lowest max */
      current_wmm[0] = xmax(border_infos[borders[i]].wmmlr[MIN_LE], border_infos[borders[i]].wmmlr[MAX_RI]) - border_infos[borders[i]].wrange[0];
      current_wmm[1] = xmin(border_infos[borders[i]].wmmlr[MAX_LE], border_infos[borders[i]].wmmlr[MIN_RI]) - border_infos[borders[i]].wrange[0];

      SL_ASSERT(current_wmm[0] <= current_wmm[1]);

      SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": current_weight: %f - %f", i, borders[i], current_wmm[0], current_wmm[1]);
#endif

      rti_tstop(rti_tid_mpi_select_radix_while_check_pre);

      lcs = gcs = 0;
#ifdef elem_weight
      lws = gws = 0.0;
#endif

      /* HIT is the default */
      refine = 0;

      if (!finalize)
      {
        rti_tstart(rti_tid_mpi_select_radix_while_check_classes);
      
        for (k = 0; k < nclasses; ++k)
        {
          lc = local_counts[border_areas[i] * nclasses + k];
          gc = global_counts[border_areas[i] * nclasses + k];

          current_cmm[0] -= gc;
          current_cmm[1] -= gc;

          SL_TRACE_IF(DEBUG_OR_NOT, "k = %" sl_key_pure_type_fmt ", current_count: %" slint_fmt " - %" slint_fmt "", k, current_cmm[0], current_cmm[1]);

#ifdef elem_weight
          lw = local_weights[border_areas[i] * nclasses + k];
          gw = global_weights[border_areas[i] * nclasses + k];

          current_wmm[0] -= gw;
          current_wmm[1] -= gw;

          SL_TRACE_IF(DEBUG_OR_NOT, "k = %" sl_key_pure_type_fmt ", current_weight: %f - %f", k, current_wmm[0], current_wmm[1]);
#endif

          /* stop and refine if max count is skipped OR min count AND max weight is skipped */
          if ((current_cmm[1] < 0)
#ifdef elem_weight
            || (current_cmm[0] < 0 && current_wmm[1] < 0.0)
#endif
            )
          {
            refine = 1;
            break;
          }

          lcs += lc;
          gcs += gc;
          gc = 0;

#ifdef elem_weight
          lws += lw;
          gws += gw;
          gw = 0.0;
#endif

          /* if between min/max counts */
          if (current_cmm[0] <= 0 && current_cmm[1] >= 0)
          {
#ifdef elem_weight
            /* go to next if max count not reached AND min weight not reached */
            if (current_cmm[1] > 0 && current_wmm[0] > 0) continue;
#endif

            /* look ahead for a better stop */
            if (k + 1 < nclasses && current_cmm[1] - global_counts[border_areas[i] * nclasses + k + 1] >= 0)
            {
#ifdef elem_weight
              /* continue if weights will improve */
              if (myabs(current_wmm[0] + current_wmm[1]) > myabs(current_wmm[0] + current_wmm[1] - 2 * global_weights[border_areas[i] * nclasses + k + 1])) continue;
#else
              /* continue if counts will improve */
              if (myabs(current_cmm[0] + current_cmm[1]) > myabs(current_cmm[0] + current_cmm[1] - 2 * global_counts[border_areas[i] * nclasses + k + 1])) continue;
#endif
            }

            /* stop */
            break;
          }
        }

        SL_ASSERT(k < nclasses);

        SL_TRACE_IF(DEBUG_OR_NOT, "%s k = %" sl_key_pure_type_fmt, (refine)?"REFINE":"HIT", k);
      
        rti_tstop(rti_tid_mpi_select_radix_while_check_classes);

      } else
      {
        rti_tstart(rti_tid_mpi_select_radix_while_check_final);

#ifdef elem_weight
        /* middle of min/max weight */
        mw = (current_wmm[0] + current_wmm[1]) / 2.0;

        /* min. part of weight to contribute */
        dw = xmax(0, mw - final_globals[NCONDS * i + 1]);
#else
        /* middle of min/max count */
        mc = (current_cmm[0] + current_cmm[1]) / 2;

        /* min. part of count to contribute */
        dc = xmax(0, mc - final_globals[NCONDS * i + 0]);
#endif

        /* contribute all? */
        if (
#ifdef elem_weight
          dw >= final_locals[NCONDS * i + 1]
#else
          dc >= final_locals[NCONDS * i + 0]
#endif
        )
        {
          lc = final_locals[NCONDS * i + 0];
#ifdef elem_weight
          lw = final_locals[NCONDS * i + 1];
#endif

        } else
        {
          /* contribute only a part */
          lc = 0;

#ifdef elem_weight
          lw = 0; /* not required */

          do
          {
            dw -= elem_weight(s, sdispls[borders[i] + 1] + lc);
            ++lc;

          } while (dw >= 0 && lc < final_locals[NCONDS * i + 0]);

          --lc;
#else
          lc = dc;
#endif
        }

        /* check mc against min/max count borders */
        lc = xminmax(current_cmm[0] - final_globals[NCONDS * i + 0], lc, current_cmm[1] - final_globals[NCONDS * i + 0]);

        /* check agains 0 (don't step back!) and the local contribution */
        lc = xminmax(0, lc, final_locals[NCONDS * i + 0]);

        /* the exact global counts/weights are unknown (set gc/gw so that parts_range is not changed) */
        lcs += lc;
        gc = border_infos[borders[i]].crange[1] - border_infos[borders[i]].crange[0];

#ifdef elem_weight
        lws += lw;
        gw = border_infos[borders[i]].wrange[1] - border_infos[borders[i]].wrange[0];
#endif

        rti_tstop(rti_tid_mpi_select_radix_while_check_final);
      }

      SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": sdispls[%" slint_fmt " + 1] = %d, lcs = %" slint_fmt, i, borders[i], borders[i], sdispls[borders[i] + 1], lcs);

      /* accept local contribution */
      sdispls[borders[i] + 1] += lcs;

      rti_tstart(rti_tid_mpi_select_radix_while_check_post);
      
      if (gcs > 0
#ifdef elem_weight
       || gws > 0
#endif
       )
      {
        border_infos[borders[i]].crange[0] += gcs;
        border_infos[borders[i]].crange[1] = border_infos[borders[i]].crange[0] + gc;

        SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": counts_range: %" slint_fmt "  %" slint_fmt "", i, borders[i], border_infos[borders[i]].crange[0], border_infos[borders[i]].crange[1]);

        border_infos[borders[i]].cmmlr[MIN_LE] = xminmax(border_infos[borders[i]].crange[0], border_infos[borders[i]].cmmlr[MIN_LE], border_infos[borders[i]].crange[1]);
        border_infos[borders[i]].cmmlr[MAX_LE] = xminmax(border_infos[borders[i]].crange[0], border_infos[borders[i]].cmmlr[MAX_LE], border_infos[borders[i]].crange[1]);
        border_infos[borders[i]].cmmlr[MIN_RI] = xminmax(border_infos[borders[i]].crange[0], border_infos[borders[i]].cmmlr[MIN_RI], border_infos[borders[i]].crange[1]);
        border_infos[borders[i]].cmmlr[MAX_RI] = xminmax(border_infos[borders[i]].crange[0], border_infos[borders[i]].cmmlr[MAX_RI], border_infos[borders[i]].crange[1]);

        SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "", i, borders[i],
          border_infos[borders[i]].cmmlr[MIN_LE], border_infos[borders[i]].cmmlr[MAX_LE], border_infos[borders[i]].cmmlr[MIN_RI], border_infos[borders[i]].cmmlr[MAX_RI]);

#ifdef elem_weight
        border_infos[borders[i]].wrange[0] += gws;
        border_infos[borders[i]].wrange[1] = border_infos[borders[i]].wrange[0] + gw;

        SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": weights_range: %f  %f", i, borders[i], border_infos[borders[i]].wrange[0], border_infos[borders[i]].wrange[1]);

        border_infos[borders[i]].wmmlr[MIN_LE] = xminmax(border_infos[borders[i]].wrange[0], border_infos[borders[i]].wmmlr[MIN_LE], border_infos[borders[i]].wrange[1]);
        border_infos[borders[i]].wmmlr[MAX_LE] = xminmax(border_infos[borders[i]].wrange[0], border_infos[borders[i]].wmmlr[MAX_LE], border_infos[borders[i]].wrange[1]);
        border_infos[borders[i]].wmmlr[MIN_RI] = xminmax(border_infos[borders[i]].wrange[0], border_infos[borders[i]].wmmlr[MIN_RI], border_infos[borders[i]].wrange[1]);
        border_infos[borders[i]].wmmlr[MAX_RI] = xminmax(border_infos[borders[i]].wrange[0], border_infos[borders[i]].wmmlr[MAX_RI], border_infos[borders[i]].wrange[1]);

        SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": weight[min/max-left/right]: %f / %f - %f / %f", i, borders[i],
          border_infos[borders[i]].wmmlr[MIN_LE], border_infos[borders[i]].wmmlr[MAX_LE], border_infos[borders[i]].wmmlr[MIN_RI], border_infos[borders[i]].wmmlr[MAX_RI]);
#endif
      }

      if (border_infos[borders[i]].cmmlr[MIN_LE] != border_info_old.cmmlr[MIN_LE]
       || border_infos[borders[i]].cmmlr[MAX_LE] != border_info_old.cmmlr[MAX_LE]
#ifdef elem_weight
       || border_infos[borders[i]].wmmlr[MIN_LE] != border_info_old.wmmlr[MIN_LE]
       || border_infos[borders[i]].wmmlr[MAX_LE] != border_info_old.wmmlr[MAX_LE]
#endif
       ) border_infos[borders[i] + 1].update = 1;

      if (border_infos[borders[i]].cmmlr[MIN_RI] != border_info_old.cmmlr[MIN_RI]
       || border_infos[borders[i]].cmmlr[MAX_RI] != border_info_old.cmmlr[MAX_RI]
#ifdef elem_weight
       || border_infos[borders[i]].wmmlr[MIN_RI] != border_info_old.wmmlr[MIN_RI]
       || border_infos[borders[i]].wmmlr[MAX_RI] != border_info_old.wmmlr[MAX_RI]
#endif
       ) border_infos[borders[i] - 1].update = 1;

      border_infos[borders[i]].update = 0;

      /* refine or remove */
      if (refine)
      {
        /* bits left for partitioning? */
        if (rhigh >= rlow)
        {
          if (last_new_area == border_areas[i] && last_new_class == k) border_areas[i] = nareas_new - 1;
          else
          {
            /* update last_new_... */
            last_new_area = border_areas[i];
            last_new_class = k;

            /* create new area */
            elem_assign_at(&areas[border_areas[i]], lcs, &areas_new[nareas_new]);
            areas_new[nareas_new].size = local_counts[border_areas[i] * nclasses + k];
            border_areas[i] = nareas_new;
            ++nareas_new;
          }

        } else
        {
          /* save local count/weight for the later prefix calculations */
          final_locals[NCONDS * (i - nborders_removed * direction) + 0] = lc;
#ifdef elem_weight
          final_locals[NCONDS * (i - nborders_removed * direction) + 1] = lw;
#endif
        }

        borders[i - nborders_removed * direction] = borders[i];
        border_areas[i - nborders_removed * direction] = border_areas[i];

      } else ++nborders_removed;

      rti_tstop(rti_tid_mpi_select_radix_while_check_post);

      i += direction;
    }

    /* restrict the parts */
    if (direction > 0) border_hi -= nborders_removed;
    else border_lo += nborders_removed;

    /* change direction */
    direction *= -1;

    rti_tstop(rti_tid_mpi_select_radix_while_check);
    
    /* switch areas */
    nareas = nareas_new;
    if (areas == areas0)
    {
      areas = areas1;
      areas_new = areas0;
    } else
    {
      areas = areas0;
      areas_new = areas1;
    }
  }

  rti_tstop(rti_tid_mpi_select_radix_while);

  sl_free(local_counts);
  sl_free(global_counts);

  rti_tstop(rti_tid_mpi_select_radix);

#ifdef PRINT_STATS
  mpi_select_stats(s, nparts, sdispls, size, rank, comm);
#endif

#if defined(PRINT_TIMINGS) && defined(SL_USE_RTI_TIM)
  if (rank == 0)
  {
    printf("%d: mpi_select_radix: %f\n", rank, rti_tlast(rti_tid_mpi_select_radix));
    printf("%d: mpi_select_radix: sync: %f\n", rank, rti_tlast(rti_tid_mpi_select_radix_sync));
    printf("%d: mpi_select_radix: while: %f\n", rank, rti_tlast(rti_tid_mpi_select_radix_while));
    printf("%d: mpi_select_radix:   count: %f\n", rank, rti_tcumu(rti_tid_mpi_select_radix_while_count));
    printf("%d: mpi_select_radix:   allreduce: %f\n", rank, rti_tcumu(rti_tid_mpi_select_radix_while_allreduce));
    printf("%d: mpi_select_radix:   round1: %f\n", rank, rti_tcumu(rti_tid_mpi_select_radix_while_round1));
    printf("%d: mpi_select_radix:     allgather: %f\n", rank, rti_tcumu(rti_tid_mpi_select_radix_while_round1_allgather));
    printf("%d: mpi_select_radix:   exscan: %f\n", rank, rti_tlast(rti_tid_mpi_select_radix_while_exscan));
    printf("%d: mpi_select_radix:   check: %f\n", rank, rti_tcumu(rti_tid_mpi_select_radix_while_check));
    printf("%d: mpi_select_radix:     pre: %f\n", rank, rti_tlast(rti_tid_mpi_select_radix_while_check_pre));
    printf("%d: mpi_select_radix:     classes: %f\n", rank, rti_tlast(rti_tid_mpi_select_radix_while_check_classes));
    printf("%d: mpi_select_radix:     final: %f\n", rank, rti_tlast(rti_tid_mpi_select_radix_while_check_final));
    printf("%d: mpi_select_radix:     post: %f\n", rank, rti_tlast(rti_tid_mpi_select_radix_while_check_post));
  }
#endif

  return 0;
}


slint_t mpi_select_stats2(elements_t *s, slint_t nparts, int *sdispls, int size, int rank, MPI_Comm comm) /* sl_proto, sl_func mpi_select_stats2 */
{
  slint_t i;
#ifdef elem_weight
  slint_t j;
#endif

  double v, vmin, vmax;

  slint_t partial_counts[nparts + 1];
#ifdef HAVENT_MPI_IN_PLACE
  slint_t partial_counts2[size + 1];
#endif

#ifdef elem_weight
  double partial_weights[nparts + 1];
# ifdef HAVENT_MPI_IN_PLACE
  double partial_weights2[size + 1];
# endif
#endif


  partial_counts[nparts] = 0;
#ifdef elem_weight
  partial_weights[nparts] = 0.0;
#endif

  for (i = 0; i < nparts; ++i)
  {
    partial_counts[i] = ((i < nparts - 1)?sdispls[i + 1]:s->size) - sdispls[i];
    partial_counts[nparts] += partial_counts[i];

#ifdef elem_weight
    partial_weights[i] = 0.0;
    for (j = sdispls[i]; j < ((i < nparts - 1)?sdispls[i + 1]:s->size); ++j) partial_weights[i] += elem_weight(s, j);
    partial_weights[nparts] += partial_weights[i];
#endif
  }

#ifdef HAVENT_MPI_IN_PLACE
  MPI_Reduce(partial_counts, partial_counts2, nparts + 1, int_mpi_datatype, MPI_SUM, 0, comm);
# define partial_counts   partial_counts2
# ifdef elem_weight
  MPI_Reduce(partial_weights, partial_weights2, nparts + 1, MPI_DOUBLE, MPI_SUM, 0, comm);
# define partial_weights  partial_weights2
# endif
#else
  /* recvbuf requires workaround for an in-place/aliased-buffer-check-bug in mpich2 (fixed with rev 5518) */
  MPI_Reduce((rank == 0)?MPI_IN_PLACE:partial_counts, (rank == 0)?partial_counts:NULL, nparts + 1, int_mpi_datatype, MPI_SUM, 0, comm);
# ifdef elem_weight
  MPI_Reduce((rank == 0)?MPI_IN_PLACE:partial_weights, (rank == 0)?partial_weights:NULL, nparts + 1, MPI_DOUBLE, MPI_SUM, 0, comm);
# endif
#endif

  if (rank == 0)
  {
    printf("%d: count total: %" sl_int_type_fmt "\n", rank, partial_counts[nparts]);
    v = 0.0;
    vmin = 1.0;
    vmax = 0.0;
    for (i = 0; i < nparts; ++i)
    {
/*      printf("%d: %" sl_int_type_fmt " %" sl_int_type_fmt " / %f - %" sl_int_type_fmt " / %f\n", rank, i, partial_counts[i], (double) partial_counts[i] / partial_counts[nparts], (partial_counts[nparts] / nparts) - partial_counts[i], fabs(1.0 - ((double) partial_counts[i] * nparts / partial_counts[nparts])));*/
      v += fabs((partial_counts[nparts] / nparts) - partial_counts[i]);
      if (fabs(1.0 - ((double) partial_counts[i] * nparts / partial_counts[nparts])) < vmin) vmin = fabs(1.0 - ((double) partial_counts[i] * nparts / partial_counts[nparts]));
      if (fabs(1.0 - ((double) partial_counts[i] * nparts / partial_counts[nparts])) > vmax) vmax = fabs(1.0 - ((double) partial_counts[i] * nparts / partial_counts[nparts]));
    }
    printf("%d: count min/max: %f / %f\n", rank, vmin, vmax);
    printf("%d: count average: %f - %f / %f\n", rank, (double) partial_counts[nparts] / nparts, v / nparts, v / partial_counts[nparts]);

#ifdef elem_weight
    printf("%d: weight total: %f\n", rank, partial_weights[nparts]);
    v = 0.0;
    vmin = 1.0;
    vmax = 0.0;
    for (i = 0; i < nparts; ++i)
    {
/*      printf("%d: %" sl_int_type_fmt " %f / %f - %f / %f\n", rank, i, partial_weights[i], partial_weights[i] / partial_weights[nparts], (partial_weights[nparts] / nparts) - partial_weights[i], fabs(1.0 - (partial_weights[i] * nparts / partial_weights[nparts])));*/
      v += fabs((partial_weights[nparts] / nparts) - partial_weights[i]);
      if (fabs(1.0 - (partial_weights[i] * nparts / partial_weights[nparts])) < vmin) vmin = fabs(1.0 - (partial_weights[i] * nparts / partial_weights[nparts]));
      if (fabs(1.0 - (partial_weights[i] * nparts / partial_weights[nparts])) > vmax) vmax = fabs(1.0 - (partial_weights[i] * nparts / partial_weights[nparts]));
    }
    printf("%d: weight min/max: %f / %f\n", rank, vmin, vmax);
    printf("%d: weight average: %f - %f / %f\n", rank, partial_weights[nparts] / nparts, v / nparts, v / partial_weights[nparts]);
#endif
  }
  
  return 0;
}


slint_t mpi_select_radix_pg2(elements_t *s, slint_t nparts, partcond_t *pcond, MPI_Comm pcomm, MPI_Comm mcomm, slint_t rhigh, slint_t rlow, slint_t rwidth, int *sdispls, int size, int rank, MPI_Comm comm) /* sl_proto, sl_func mpi_select_radix_pg2 */
{
  partcond_t pconds[nparts];

  if (mcomm != MPI_COMM_NULL)
  {
    /* gather local part.-conds at the part.-masters */
#ifdef HAVENT_MPI_IN_PLACE
    MPI_Allgather(pcond, sizeof(partcond_t), MPI_BYTE, pconds, sizeof(partcond_t), MPI_BYTE, mcomm);
#else
    pconds[rank] = *pcond;
    MPI_Allgather(MPI_IN_PLACE, sizeof(partcond_t), MPI_BYTE, pconds, sizeof(partcond_t), MPI_BYTE, mcomm);
#endif
  }

  /* distribute local part.-conds to the part.-members */
  if (pcomm != MPI_COMM_NULL) MPI_Bcast(pconds, nparts * sizeof(partcond_t), MPI_BYTE, 0, pcomm);

  mpi_select_radix2(s, nparts, pconds, rhigh, rlow, rwidth, sdispls, size, rank, comm);
  
  return 0;
}
