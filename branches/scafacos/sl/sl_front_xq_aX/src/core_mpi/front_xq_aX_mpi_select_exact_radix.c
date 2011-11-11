/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/core_mpi/mpi_select_exact_radix.c
 *  timestamp: 2010-10-04 09:02:08 +0200
 *  
 */


#include "sl_common.h"


/*#define radix_low                 0
#define radix_high                (sizeof(slkey_pure_t) * 8 - 1)
#define radix_key2class(k, x, y)  (((k) >> (x)) & y)*/

#define myabs(x)  (((x) >= 0)?(x):-(x))

#ifdef elem_weight
# define NCONDS  2
#else
# define NCONDS  1
#endif

/* config */
/*#define SYNC_ON_INIT*/
#define HAVENT_MPI_IN_PLACE

/* sl_macro MPI_SELECT_EXACT_RADIX_REDUCEBCAST_THRESHOLD */
#define REDUCEBCAST_ROOT  0

/*#define PRINT_SDISPLS*/
/*#define PRINT_STATS*/
/*#define PRINT_TIMINGS  0*/

/*#define VERIFY*/

#define DEBUG_OR_NOT  (sl_mpi_rank == -1)


slint_t mpi_select_exact_radix(elements_t *s, slint_t nelements, slint_t nparts, partcond_t *pconds, slint_t rhigh, slint_t rlow, slint_t rwidth, slint_t sorted, int *sdispls, int size, int rank, MPI_Comm comm) /* sl_proto, sl_func mpi_select_exact_radix */
{
  binning_t bm;

  binning_radix_create(&bm, rhigh, rlow, rwidth, sorted|SL_SORTED_IN);

#if 1
  splitter_t sp;
  sp.displs = sdispls;

  mpi_select_exact_generic_bulk(s, nelements, nparts, pconds, &bm, &sp, size, rank, comm);
#else
  mpi_select_exact_generic_bulk_old(s, nelements, nparts, pconds, &bm, sdispls, size, rank, comm);
#endif

  binning_radix_destroy(&bm);
  
  return 0;
}


slint_t mpi_select_exact_radix_fixed(elements_t *s, slint_t nelements, slint_t nparts, partcond_t *pconds, slint_t rhigh, slint_t rlow, slint_t rwidth, int *sdispls, int size, int rank, MPI_Comm comm) /* sl_proto, sl_func mpi_select_exact_radix_fixed */
{
  slkey_pure_t max_nclasses, nclasses, bit_mask;
  slkey_pure_t k, l;

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
  elements_t areas0[max_nareas * nelements], areas1[max_nareas * nelements], *areas, *areas_new;

  slint_t *area_counts, *current_counts;
  double *local_counts, *global_counts;
#ifdef elem_weight
  double *local_weights, *global_weights, *current_weights;
#endif

  slint_t current_cmm[2];
#ifdef elem_weight
  double current_wmm[2];
#endif

  slint_t final_areas[max_nborders * nelements];
  double final_locals[NCONDS * max_nborders], *final_globals;

  slint_t current_width;
  slint_t round, direction, refine, finalize;
  slint_t last_new_area, last_new_class;

  slint_t lc, lcs, gc, gcs, lcv[nelements], lcsv[nelements];
#ifdef elem_weight
  double lw, gw, lws, gws;
  double mw, dw;
  double mcw[4];
#else
  slint_t mc, dc;
#endif

  slint_t i, j;

  elements_t xi, end;

#ifdef VERIFY
  slint_t v;
#endif


  SL_TRACE_IF(DEBUG_OR_NOT, "starting mpi_select_exact_radix");

  /* sl_tid rti_tid_mpi_select_exact_radix rti_tid_mpi_select_exact_radix_sync */

  rti_treset(rti_tid_mpi_select_exact_radix_while);                   /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_radix_while_count);             /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_radix_while_allreduce);         /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_radix_while_round1);            /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_radix_while_round1_allgather);  /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_radix_while_exscan);            /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_radix_while_check);             /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_radix_while_check_pre);         /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_radix_while_check_classes);     /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_radix_while_check_final);       /* sl_tid */
  rti_treset(rti_tid_mpi_select_exact_radix_while_check_post);        /* sl_tid */

  rti_tstart(rti_tid_mpi_select_exact_radix_sync);
#ifdef SYNC_ON_INIT
  MPI_Barrier(comm);
#endif
  rti_tstop(rti_tid_mpi_select_exact_radix_sync);

#ifdef VERIFY
  v = elements_validate_order(s, 1);
  
  SL_TRACE_IF(DEBUG_OR_NOT, "elements order: %s (%" slint_fmt ")", (v > 0)?"FAILED":"SUCCESS", v);
#endif

  rti_tstart(rti_tid_mpi_select_exact_radix);

  if (rhigh < 0) rhigh = key_radix_high;
  if (rlow < 0) rlow = key_radix_low;
  if (rwidth < 0) rwidth = sort_radix_width_default;
  
  max_nclasses = powof2_typed(rwidth, slkey_pure_t);

/*  SL_TRACE_IF(DEBUG_OR_NOT, "alloc area_counts: %" slint_fmt " * %d", max_nareas * nelements * max_nclasses, sizeof(slint_t));
  SL_TRACE_IF(DEBUG_OR_NOT, "alloc local_counts: %" slint_fmt " * %d", NCONDS * (max_nareas * max_nclasses + max_nareas), sizeof(slint_t));
  SL_TRACE_IF(DEBUG_OR_NOT, "alloc global_counts: %" slint_fmt " * %d", NCONDS * (max_nareas * max_nclasses + max_nareas), sizeof(slint_t));*/

  area_counts = sl_alloc(max_nareas * nelements * max_nclasses, sizeof(slint_t));
  local_counts = sl_alloc(NCONDS * (max_nareas * max_nclasses + max_nareas), sizeof(double));
  global_counts = sl_alloc(NCONDS * (max_nareas * max_nclasses + max_nareas), sizeof(double));

  /* init areas (first area = all elements) */
  areas = areas0;
  areas_new = areas1;

  nareas = 1;
  for (j = 0; j < nelements; ++j) elem_assign(&s[j], &areas[0 * nelements + j]);

  /* init parts */
  border_lo = 0;
  border_hi = max_nborders - 1;
  for (i = border_lo; i <= border_hi; ++i)
  {
    borders[i] = i;
    border_areas[i] = 0;
  }

  /* init sdispls */
  for (i = 0; i < nparts; ++i)
  for (j = 0; j < nelements; ++j) sdispls[i * nelements + j] = 0;

  rti_tstart(rti_tid_mpi_select_exact_radix_while);

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

      rti_tstart(rti_tid_mpi_select_exact_radix_while_count);

      /* for every area */
      for (i = 0; i < nareas; ++i)
      {
        local_counts[nareas * nclasses + i] = 0;
#ifdef elem_weight
        local_weights[nareas * nclasses + i] = 0.0;
#endif

        /* for every list of elements */
        for (j = 0; j < nelements; ++j)
        {
          SL_TRACE_IF(DEBUG_OR_NOT, "area %" slint_fmt ",%" slint_fmt ": size = %" slint_fmt, i, j, areas[i * nelements + j].size);

          elem_assign_at(&areas[i * nelements + j], areas[i * nelements + j].size, &end);
          
          current_counts = area_counts + ((i * nelements + j) * nclasses);
#ifdef elem_weight
          current_weights = local_weights + (i * nclasses);
#endif

          for (k = 0; k < nclasses; ++k) current_counts[k] = 0;

          if (nclasses > 1)
          {
            /* counts and weights in every class */
            for (elem_assign(&areas[i * nelements + j], &xi); xi.keys < end.keys; elem_inc(&xi))
            {
              k = key_radix_key2class(key_purify(*xi.keys), rhigh, bit_mask);
              current_counts[k] += 1;
/*              SL_TRACE_IF(DEBUG_OR_NOT, "key %" sl_key_pure_type_fmt " goes to bin %"  sl_key_pure_type_fmt, key_purify(*xi.keys), k);*/
#ifdef elem_weight
              current_weights[k] += elem_weight(&xi, 0);
#endif
            }

          } else
          {
            /* total counts and weights */
            current_counts[0] = areas[i * nelements + j].size;

#ifdef elem_weight
            for (elem_assign(&areas[i * nelements + j], &xi); xi.keys < end.keys; elem_inc(&xi)) current_weights[0] += elem_weight(&xi, 0);
#endif
          }
          
          for (k = 0; k < nclasses; ++k) local_counts[i * nclasses + k] += current_counts[k];

          /* total counts and weights in this area */
          local_counts[nareas * nclasses + i] += areas[i * nelements + j].size;
#ifdef elem_weight
          for (k = 0; k < nclasses; ++k) local_weights[nareas * nclasses + i] += current_weights[k];
#endif
        }

        SL_TRACE_ARRAY_IF(DEBUG_OR_NOT, "%" slint_fmt ": counts =", " %f", k, nclasses, (&local_counts[i * nclasses]), i);
      }

      rti_tstop(rti_tid_mpi_select_exact_radix_while_count);

      --rhigh;

      SL_TRACE_IF(DEBUG_OR_NOT, "all-reducing %" slint_fmt " doubles", (slint_t) (NCONDS * (nareas * nclasses + nareas)));

      rti_tstart(rti_tid_mpi_select_exact_radix_while_allreduce);

      /* create global counts and weights */
#ifdef MPI_SELECT_EXACT_RADIX_REDUCEBCAST_THRESHOLD
      if (size >= MPI_SELECT_EXACT_RADIX_REDUCEBCAST_THRESHOLD)
      {
        MPI_Reduce(local_counts, global_counts, NCONDS * (nareas * nclasses + nareas), MPI_DOUBLE, MPI_SUM, REDUCEBCAST_ROOT, comm);
        MPI_Bcast(global_counts, NCONDS * (nareas * nclasses + nareas), MPI_DOUBLE, REDUCEBCAST_ROOT, comm);

      } else
#endif
        MPI_Allreduce(local_counts, global_counts, NCONDS * (nareas * nclasses + nareas), MPI_DOUBLE, MPI_SUM, comm);

      rti_tstop(rti_tid_mpi_select_exact_radix_while_allreduce);
    }

    /* do initializations */
    if (round == 1)
    {
      rti_tstart(rti_tid_mpi_select_exact_radix_while_round1);

      for (i = 0; i < nparts; ++i)
      {
        /* truncate counts, set default values and determine local (count/weight) limits */
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
      border_infos[border_lo - 1].crange[1] = 0;
      border_infos[border_lo - 1].cmmlr[MIN_LE] = border_infos[border_lo - 1].cmmlr[MAX_LE] = 0;
      border_infos[border_lo - 1].cmmlr[MIN_RI] = border_infos[border_lo - 1].cmmlr[MAX_RI] = 0;

      SL_TRACE_IF(DEBUG_OR_NOT, "lowest: %" slint_fmt ": init count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "", border_lo - 1,
        border_infos[border_lo - 1].cmmlr[MIN_LE], border_infos[border_lo - 1].cmmlr[MAX_LE], border_infos[border_lo - 1].cmmlr[MIN_RI], border_infos[border_lo - 1].cmmlr[MAX_RI]);

#ifdef elem_weight
      border_infos[border_lo - 1].wrange[0] = 0.0;
      border_infos[border_lo - 1].wrange[1] = 0.0;
      border_infos[border_lo - 1].wmmlr[MIN_LE] = border_infos[border_lo - 1].wmmlr[MAX_LE] = 0.0;
      border_infos[border_lo - 1].wmmlr[MIN_RI] = border_infos[border_lo - 1].wmmlr[MAX_RI] = 0.0;

      SL_TRACE_IF(DEBUG_OR_NOT, "lowest: %" slint_fmt ": init weight[min/max-left/right]: %f / %f - %f / %f", border_lo - 1,
        border_infos[border_lo - 1].wmmlr[MIN_LE], border_infos[border_lo - 1].wmmlr[MAX_LE], border_infos[border_lo - 1].wmmlr[MIN_RI], border_infos[border_lo - 1].wmmlr[MAX_RI]);
#endif

      /* init highest part (sentinel) */
      border_infos[border_hi + 1].update = 0;
      border_infos[border_hi + 1].crange[0] = global_counts[nareas * nclasses + 0];
      border_infos[border_hi + 1].crange[1] = global_counts[nareas * nclasses + 0];
      border_infos[border_hi + 1].cmmlr[MIN_LE] = border_infos[border_hi + 1].cmmlr[MAX_LE] = 0;
      border_infos[border_hi + 1].cmmlr[MIN_RI] = border_infos[border_hi + 1].cmmlr[MAX_RI] = global_counts[nareas * nclasses + 0];

      SL_TRACE_IF(DEBUG_OR_NOT, "highest: %" slint_fmt ": init count[min/max-left/right]: %" slint_fmt " / %" slint_fmt " - %" slint_fmt " / %" slint_fmt "", border_hi + 1,
        border_infos[border_hi + 1].cmmlr[MIN_LE], border_infos[border_hi + 1].cmmlr[MAX_LE], border_infos[border_hi + 1].cmmlr[MIN_RI], border_infos[border_hi + 1].cmmlr[MAX_RI]);

#ifdef elem_weight
      border_infos[border_hi + 1].wrange[0] = global_weights[nareas * nclasses + 0];
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
          for (j = 0; j < nelements; ++j) final_areas[i * nelements + j] = area_counts[(0 * nelements + j) * nclasses + 0];

          final_locals[NCONDS * i + 0] = local_counts[nareas * nclasses + 0];
#ifdef elem_weight
          final_locals[NCONDS * i + 1] = local_weights[nareas * nclasses + 0];
#endif
        }
      }
      
      /* first direction: forward */
      direction = 1;

      rti_tstop(rti_tid_mpi_select_exact_radix_while_round1);
    }

    /* compute prefixes for finalization */
    if (finalize)
    {
      /* determine number of parts to finalize */
      j = border_hi - border_lo + 1;
    
      SL_TRACE_IF(DEBUG_OR_NOT, "Exscan: finalizing %" slint_fmt " parts", j);

      rti_tstart(rti_tid_mpi_select_exact_radix_while_exscan);

      /* use local_counts to store the global prefix sums */      
      final_globals = local_counts;

      /* create global prefix sums (set rank 0 to zero) */
      MPI_Exscan(&final_locals[NCONDS * border_lo], &final_globals[NCONDS * border_lo], NCONDS * j, MPI_DOUBLE, MPI_SUM, comm);
      if (rank == 0) for (i = border_lo; i <= border_hi; ++i) final_globals[NCONDS * i + 0] = 
#ifdef elem_weight
        final_globals[NCONDS * i + 1] = 
#endif
        0.0;

      rti_tstop(rti_tid_mpi_select_exact_radix_while_exscan);
    }

    /* check all remaining parts */
    SL_TRACE_IF(DEBUG_OR_NOT, "ROUND: %" slint_fmt ", %s", round, (direction > 0)?"forward":"backward");

    nareas_new = 0;
    last_new_area = last_new_class = -1;
    nborders_removed = 0;

    rti_tstart(rti_tid_mpi_select_exact_radix_while_check);

    i = (direction > 0)?border_lo:border_hi;
    while ((direction > 0)?(i <= border_hi):(i >= border_lo))
    {
      /* check partition borders[i] */
      SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ": PART: %" slint_fmt ",%" slint_fmt, round, i, borders[i]);

      rti_tstart(rti_tid_mpi_select_exact_radix_while_check_pre);

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

      SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": crange: %" slint_fmt " - %" slint_fmt "", i, borders[i], border_infos[borders[i]].crange[0], border_infos[borders[i]].crange[1]);

      /* select highest min and lowest max */
      current_cmm[0] = xmax(border_infos[borders[i]].cmmlr[MIN_LE], border_infos[borders[i]].cmmlr[MAX_RI]) - border_infos[borders[i]].crange[0];
      current_cmm[1] = xmin(border_infos[borders[i]].cmmlr[MAX_LE], border_infos[borders[i]].cmmlr[MIN_RI]) - border_infos[borders[i]].crange[0];

      if (rank == 0) SL_ASSERT(current_cmm[0] <= current_cmm[1]);
      
      if (rank == 0) SL_ASSERT(0 <= current_cmm[0]);

      SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": current_count: %" slint_fmt " - %" slint_fmt "", i, borders[i], current_cmm[0], current_cmm[1]);

#ifdef elem_weight
      SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": weight[min/max-left/right]: %f / %f - %f / %f", i, borders[i],
        border_infos[borders[i]].wmmlr[MIN_LE], border_infos[borders[i]].wmmlr[MAX_LE], border_infos[borders[i]].wmmlr[MIN_RI], border_infos[borders[i]].wmmlr[MAX_RI]);

      SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": wrange: %f - %f", i, borders[i], border_infos[borders[i]].wrange[0], border_infos[borders[i]].wrange[1]);

      /* select highest min and lowest max */
      current_wmm[0] = xmax(border_infos[borders[i]].wmmlr[MIN_LE], border_infos[borders[i]].wmmlr[MAX_RI]) - border_infos[borders[i]].wrange[0];
      current_wmm[1] = xmin(border_infos[borders[i]].wmmlr[MAX_LE], border_infos[borders[i]].wmmlr[MIN_RI]) - border_infos[borders[i]].wrange[0];

      if (rank == 0) SL_ASSERT(current_wmm[0] <= current_wmm[1]);

      SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": current_weight: %f - %f", i, borders[i], current_wmm[0], current_wmm[1]);
#endif

      rti_tstop(rti_tid_mpi_select_exact_radix_while_check_pre);

      /* HIT is the default */
      refine = 0;

      if (!finalize)
      {
        rti_tstart(rti_tid_mpi_select_exact_radix_while_check_classes);

        lcs = gcs = 0;
#ifdef elem_weight
        lws = gws = 0.0;
#endif

        for (k = 0; k < nclasses; ++k)
        {
          lc = local_counts[border_areas[i] * nclasses + k];
          gc = global_counts[border_areas[i] * nclasses + k];

          current_cmm[0] -= gc;
          current_cmm[1] -= gc;

          SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": k = %" sl_key_pure_type_fmt ", current_count: %" slint_fmt " - %" slint_fmt ", lc = %" slint_fmt ", lcs = %" slint_fmt ", gc = %" slint_fmt ", gcs = %" slint_fmt,
            i, borders[i], k, current_cmm[0], current_cmm[1], lc, lcs, gc, gcs);

#ifdef elem_weight
          lw = local_weights[border_areas[i] * nclasses + k];
          gw = global_weights[border_areas[i] * nclasses + k];

          current_wmm[0] -= gw;
          current_wmm[1] -= gw;

          SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": k = %" sl_key_pure_type_fmt ", current_weight: %e - %e", i, borders[i], k, current_wmm[0], current_wmm[1]);
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
            SL_TRACE_IF(DEBUG_OR_NOT, "got to next: %d && %d", (current_cmm[1] > 0), (current_wmm[0] > 0));

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

        SL_ASSERT_IF((rank == 0), k < nclasses);

        SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": %s k = %" sl_key_pure_type_fmt ", lcs = %" slint_fmt, i, borders[i], (refine)?"REFINE":"HIT", k, lcs);

        /* make sure k is safe (it is used as index later) */
        if (k >= nclasses) k = nclasses - 1;

        /* break the local contribution into contributions for the lists of elements */
        for (j = 0; j < nelements; ++j)
        {
          lcsv[j] = 0;
          for (l = 0; l < k; ++l) lcsv[j] += area_counts[((border_areas[i] * nelements + j) * nclasses) + l];

          if (refine) lcv[j] = area_counts[((border_areas[i] * nelements + j) * nclasses) + k];
          else
          {
            lcv[j] = 0;
            lcsv[j] += area_counts[((border_areas[i] * nelements + j) * nclasses) + k];
          }

          lcs -= lcsv[j];
        }

        rti_tstop(rti_tid_mpi_select_exact_radix_while_check_classes);

      } else
      {
        rti_tstart(rti_tid_mpi_select_exact_radix_while_check_final);
        
        k = 0;

#ifdef elem_weight
        /* middle of min/max weight */
        mw = (current_wmm[0] + current_wmm[1]) / 2.0;

        /* min. part of weight to contribute */
        dw = xmax(0, mw - final_globals[NCONDS * i + 1]);

        SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": mw = %e, dw = %e", i, borders[i], mw, dw);
#else
        /* middle of min/max count */
        mc = (current_cmm[0] + current_cmm[1]) / 2;

        /* min. part of count to contribute */
        dc = xmax(0, mc - final_globals[NCONDS * i + 0]);

        SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": mc = %" slint_fmt ", dc = %" slint_fmt, i, borders[i], mc, dc);
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
#ifdef elem_weight
          lc = 0;

          for (j = 0; j < nelements; ++j)
          {
            elem_assign_at(&areas[border_areas[i] * nelements + j], areas[border_areas[i] * nelements + j].size, &end);

            for (elem_assign(&areas[border_areas[i] * nelements + j], &xi); xi.keys < end.keys; elem_inc(&xi))
            {
              dw -= elem_weight(&xi, 0);
              ++lc;

              if (dw < 0.0 || lc >= final_locals[NCONDS * i + 0])
              {
                dw += elem_weight(&xi, 0);
                --lc;
                break;
              }
            }
          }

          lw = dw;
#else
          lc = dc;
#endif
        }

        /* check mc against min/max count borders */
        lc = xminmax(current_cmm[0] - final_globals[NCONDS * i + 0], lc, current_cmm[1] - final_globals[NCONDS * i + 0]);

        /* check agains 0 (don't step back!) and the local contribution */
        lc = xminmax(0, lc, final_locals[NCONDS * i + 0]);

        lcs = lc;
#ifdef elem_weight
        lws = lw;
#endif

#ifdef elem_weight
        SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": next border: %" slint_fmt " <= %" slint_fmt " + %" slint_fmt " <= %" slint_fmt,
          i, borders[i], border_lo, i, direction, border_hi);
        if (border_lo <= i + direction && i + direction <= border_hi)
          SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": next border: %" slint_fmt " == %" slint_fmt " + %" slint_fmt,
            i, borders[i], borders[i + direction], borders[i], direction);

        /* FIXME: finalize geht auch rückwärts!!! */

        /* if the next open border is really the _next_ border */
        if (border_lo <= i + direction && i + direction <= border_hi && borders[i + direction] == borders[i] + direction)
        {
          /* determine the exact global counts/weights (damn, this is expensive) */
          mcw[0] = lcs;
          mcw[1] = lws;
          MPI_Allreduce(&mcw[0], &mcw[2], 2, MPI_DOUBLE, MPI_SUM, comm);

        } else
        {
          /* the exact global counts/weights are not required */
          mcw[2] = 0.0;
          mcw[3] = 0.0;
        }

        gc = 0;
        gcs = mcw[2];
        gw = 0.0;
        gws = mcw[3];
        
        SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": gcs = %" slint_fmt ", gws = %f", i, borders[i], gcs, gws);
#else
        /* the global count is simply mc */
        gc = 0;
        gcs = mc;

        SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": gcs = %" slint_fmt, i, borders[i], gcs);
#endif

        SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": lcs = %" slint_fmt, i, borders[i], lcs);

        /* break the local contribution into contributions for the lists of elements */
        for (j = 0; j < nelements; ++j)
        {
          lcv[j] = 0;
          lcsv[j] = xmin(lcs, final_areas[i * nelements + j]);
          
          lcs -= lcsv[j];
        }

        SL_TRACE_ARRAY_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": lcsv = ", "%" slint_fmt, j, nelements, lcsv, i, borders[i]);

        rti_tstop(rti_tid_mpi_select_exact_radix_while_check_final);
      }

      SL_ASSERT(lcs == 0);
      
      /* accept local contributions */
      for (j = 0; j < nelements; ++j) sdispls[(borders[i] + 1) * nelements + j] += lcsv[j];

      rti_tstart(rti_tid_mpi_select_exact_radix_while_check_post);

      /* this is wrong, e.g., even if gc == 0 and gcs == 0 then crange[1] is set to crange[0]! */
/*      if (gc > 0 || gcs > 0
#ifdef elem_weight
       || gw != 0.0 || gws != 0.0
#endif
       )*/
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
      
      SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": range diff 0: %" slint_fmt "-%" slint_fmt " | %" slint_fmt "-%" slint_fmt, i, borders[i],
        border_infos[borders[i]].crange[0] - border_infos[borders[i] - 1].crange[1], border_infos[borders[i]].crange[0] - border_infos[borders[i] - 1].crange[0],
        border_infos[borders[i] + 1].crange[0] - border_infos[borders[i]].crange[0], border_infos[borders[i] + 1].crange[1] - border_infos[borders[i]].crange[0]);
      SL_TRACE_IF(DEBUG_OR_NOT, "%" slint_fmt ",%" slint_fmt ": range diff 1: %" slint_fmt "-%" slint_fmt " | %" slint_fmt "-%" slint_fmt, i, borders[i],
        border_infos[borders[i]].crange[1] - border_infos[borders[i] - 1].crange[1], border_infos[borders[i]].crange[1] - border_infos[borders[i] - 1].crange[0],
        border_infos[borders[i] + 1].crange[0] - border_infos[borders[i]].crange[1], border_infos[borders[i] + 1].crange[1] - border_infos[borders[i]].crange[1]);

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
            for (j = 0; j < nelements; ++j)
            {
              elem_assign_at(&areas[border_areas[i] * nelements + j], lcsv[j], &areas_new[nareas_new * nelements + j]);
              areas_new[nareas_new * nelements + j].size = lcv[j];
            }
            border_areas[i] = nareas_new;
            ++nareas_new;
          }

        } else
        {
          for (j = 0; j < nelements; ++j) final_areas[(i - nborders_removed * direction) * nelements + j] = lcv[j];

          /* save local count/weight for the later prefix calculations */
          final_locals[NCONDS * (i - nborders_removed * direction) + 0] = lc;
#ifdef elem_weight
          final_locals[NCONDS * (i - nborders_removed * direction) + 1] = lw;
#endif
        }

        borders[i - nborders_removed * direction] = borders[i];
        border_areas[i - nborders_removed * direction] = border_areas[i];

      } else ++nborders_removed;

      rti_tstop(rti_tid_mpi_select_exact_radix_while_check_post);

      i += direction;
    }

    /* restrict the parts */
    if (direction > 0) border_hi -= nborders_removed;
    else border_lo += nborders_removed;

    /* change direction */
    direction *= -1;

    rti_tstop(rti_tid_mpi_select_exact_radix_while_check);
    
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

  rti_tstop(rti_tid_mpi_select_exact_radix_while);

  sl_free(area_counts);
  sl_free(local_counts);
  sl_free(global_counts);

  rti_tstop(rti_tid_mpi_select_exact_radix);

#ifdef VERIFY
  v = mpi_post_check_partconds(s, nelements, nparts, pconds, sdispls, size, rank, comm);
  
  SL_ASSERT_IF(rank == 0, v < 0);
  
  SL_NOTICE_IF(rank == 0, "post_check_partconds: %s (%" slint_fmt ")", (v >= 0)?"FAILED":"SUCCESS", v);
#endif

#ifdef PRINT_SDISPLS
  printf("%d: sdispls:", rank);
  for (i = 0; i < nparts; ++i) printf(" %d ", sdispls[i]);
  printf("\n");
#endif

#ifdef PRINT_STATS
  mpi_select_stats(s, nparts, sdispls, size, rank, comm);
#endif

#if defined(PRINT_TIMINGS) && defined(SL_USE_RTI_TIM)
  if (rank == PRINT_TIMINGS)
  {
    printf("%d: mpi_select_exact_radix: %f\n", rank, rti_tlast(rti_tid_mpi_select_exact_radix));
    printf("%d: mpi_select_exact_radix: sync: %f\n", rank, rti_tlast(rti_tid_mpi_select_exact_radix_sync));
    printf("%d: mpi_select_exact_radix: while: %f\n", rank, rti_tlast(rti_tid_mpi_select_exact_radix_while));
    printf("%d: mpi_select_exact_radix:  count: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_radix_while_count));
    printf("%d: mpi_select_exact_radix:  allreduce: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_radix_while_allreduce));
    printf("%d: mpi_select_exact_radix:  round1: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_radix_while_round1));
    printf("%d: mpi_select_exact_radix:   allgather: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_radix_while_round1_allgather));
    printf("%d: mpi_select_exact_radix:  exscan: %f\n", rank, rti_tlast(rti_tid_mpi_select_exact_radix_while_exscan));
    printf("%d: mpi_select_exact_radix:  check: %f\n", rank, rti_tcumu(rti_tid_mpi_select_exact_radix_while_check));
    printf("%d: mpi_select_exact_radix:   pre: %f\n", rank, rti_tlast(rti_tid_mpi_select_exact_radix_while_check_pre));
    printf("%d: mpi_select_exact_radix:   classes: %f\n", rank, rti_tlast(rti_tid_mpi_select_exact_radix_while_check_classes));
    printf("%d: mpi_select_exact_radix:   final: %f\n", rank, rti_tlast(rti_tid_mpi_select_exact_radix_while_check_final));
    printf("%d: mpi_select_exact_radix:   post: %f\n", rank, rti_tlast(rti_tid_mpi_select_exact_radix_while_check_post));
    printf("%d: mpi_select_exact_radix: rounds: %" slint_fmt "\n", rank, round);
  }
#endif

  return 0;
}


slint_t mpi_select_exact_radix_grouped(elements_t *s, slint_t nelements, partcond_t *pcond, MPI_Comm pcond_comm, MPI_Comm group_comm, slint_t rhigh, slint_t rlow, slint_t rwidth, slint_t sorted, int *sdispls, int size, int rank, MPI_Comm comm) /* sl_proto, sl_func mpi_select_exact_radix_grouped */
{
  slint_t npconds = -1;
  partcond_t *pconds;


  mpi_gather_partconds_grouped(pcond, pcond_comm, group_comm, NULL, &npconds, size, rank, comm);

  pconds = sl_alloca(npconds, sizeof(partcond_t));
  
  mpi_gather_partconds_grouped(pcond, pcond_comm, group_comm, pconds, &npconds, size, rank, comm);

  mpi_select_exact_radix(s, nelements, npconds, pconds, rhigh, rlow, rwidth, sorted, sdispls, size, rank, comm);
/*  mpi_select_exact_radix_fixed(s, nelements, npconds, pconds, rhigh, rlow, rwidth, sdispls, size, rank, comm);*/
  
  sl_freea(pconds);
  
  return 0;
}
