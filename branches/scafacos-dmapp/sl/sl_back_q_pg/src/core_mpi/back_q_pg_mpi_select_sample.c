/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/core_mpi/mpi_select_sample.c
 *  timestamp: 2011-01-13 09:48:59 +0100
 *  
 */


/* sl_macro MSS_TRACE_IF */


#include "sl_common.h"


/*#define sl_pivot_equal(_n_, _i_, _r_)  ((slint_t) (((((double) (_i_) + 1) * (_n_)) / ((_r_) + 1)) + 0.5))*/
#define sl_pivot_equal(_n_, _i_, _r_)  (((_i_) * (_n_)) / ((_r_) + 1))


#define MSS_ROOT  0

#ifndef MSS_TRACE_IF
# ifdef GLOBAL_TRACE_IF
#  define MSS_TRACE_IF  GLOBAL_TRACE_IF
# else
#  define MSS_TRACE_IF  (sl_mpi_rank == 0)
# endif
#endif


slint_t mpi_select_sample_regular(elements_t *s, slint_t nparts, partcond_t *pconds, slint_t nsamples, splitter_t *sp, int size, int rank, MPI_Comm comm) /* sl_proto, sl_func mpi_select_sample_regular */
{
  slint_t i, j;
  slweight_t w, wi, wold;

  const slint_t nslocal = nsamples;
  const slint_t nsglobal = nslocal * size;

  const slint_t nsplitter = nparts - 1;

#ifdef elem_weight
  slpwkey_t lskeys[nslocal];
  slpwkey_t gskeys[nsglobal];
#else
  slkey_pure_t lskeys[nslocal];
  slkey_pure_t gskeys[nsglobal];
#endif
  slkey_pure_t skeys[nsplitter];

  slint_t lgcounts[2];
#ifdef elem_weight
  slweight_t lgweights[2];
#endif

  partcond_intern_t pci[nparts];

  elements_t gs, e;

#ifdef elem_weight
  slint_t doweights;
#else
# define doweights  0
#endif


#ifdef elem_weight
  doweights = ((pconds->pcm & (SLPC_WEIGHTS_MM|SLPC_WEIGHTS_LH)) != 0);
#endif

#ifdef elem_weight
  if (doweights) mpi_elements_get_counts_and_weights(s, 1, lgcounts, lgweights, -1, size, rank, comm);
  else
#endif
    mpi_elements_get_counts(s, &lgcounts[0], &lgcounts[1], -1, size, rank, comm);

  init_partconds_intern(nparts, pci, pconds, nparts, lgcounts[1], elem_weight_ifelse(doweights?lgweights[1]:0, 0));

  SL_TRACE_IF(MSS_TRACE_IF, "counts: %" slint_fmt " / %" slint_fmt, lgcounts[0], lgcounts[1]);
#ifdef elem_weight
  if (doweights)
    SL_TRACE_IF(MSS_TRACE_IF, "weights: %" slweight_fmt " / %" slweight_fmt "", lgweights[0], lgweights[1]);
#endif

#ifdef elem_weight
  j = 0;
  w = 0;
  wold = 0;
  for (i = 0; i < nslocal; ++i)
  {
    wi = (i + 1) * lgweights[0] / (nslocal + 1);

    while (w < wi && j < lgcounts[0])
    {
      w += elem_weight(s, j);
      ++j;
    }
    
    if (j < lgcounts[0]) lskeys[i].pkey = *key_get_pure(elem_key_at(s, j));
    else lskeys[i].pkey = *key_get_pure(elem_key_at(s, j - 1)) + 1;

    lskeys[i].weight = w - wold;
    
    wold = w;

/*    printf("%" slint_fmt ": key: %" slint_fmt " / weight: %" slweight_fmt "\n", i, lskeys[i].pkey, lskeys[i].weight);*/
  }
#else
  for (i = 0; i < nslocal; ++i) lskeys[i] = *key_get_pure(elem_key_at(s, sl_pivot_equal(s->size, i + 1, nslocal)));
#endif

#ifdef elem_weight
  MPI_Gather(lskeys, nslocal, pwkey_mpi_datatype, gskeys, nslocal, pwkey_mpi_datatype, MSS_ROOT, comm);
#else
  MPI_Gather(lskeys, nslocal, pkey_mpi_datatype, gskeys, nslocal, pkey_mpi_datatype, MSS_ROOT, comm);
#endif

  if (rank == MSS_ROOT)
  {
    elements_alloc(&gs, nsglobal, SLCM_ALL);

    gs.size = nsglobal;
#ifdef elem_weight
    for (i = 0; i < nsglobal; ++i)
    {
      key_set_pure(elem_key_at(&gs, i), gskeys[i].pkey);
      elem_weight(&gs, i) = gskeys[i].weight;
    }
#else
    for (i = 0; i < nsglobal; ++i) key_set_pure(elem_key_at(&gs, i), gskeys[i]);
#endif

    sort_radix(&gs, NULL, -1, -1, -1);

/*    printf("samples:\n");
    elements_print_all(&gs);*/

#ifdef elem_weight
    j = 0;
    w = 0;
    for (i = 0; i < nsplitter; ++i)
    {
      wi = (i + 1) * lgweights[1] / (nsplitter + 1);

      while (w < wi && j < nsglobal)
      {
        w += elem_weight(&gs, j);
        ++j;
      }

      if (j > 0 && (wi - w + elem_weight(&gs, j - 1)) <= (w - wi))
      {
        w -= elem_weight(&gs, j - 1);
        --j;
      }
      skeys[i] = *key_get_pure(elem_key_at(&gs, j - 1));

/*      printf("%" slint_fmt ": key: %" slint_fmt " / weight: %" slweight_fmt "\n", i, skeys[i], w);*/
    }
    
    skeys[i] = *key_get_pure(elem_key_at(&gs, sl_pivot_equal(gs.size, i + 1, nsplitter)));
#else
    for (i = 0; i < nsplitter; ++i) skeys[i] = *key_get_pure(elem_key_at(&gs, sl_pivot_equal(gs.size, i + 1, nsplitter)));
#endif

    elements_free(&gs);
  }

#ifdef elem_weight
  MPI_Bcast(&skeys, nsplitter, pwkey_mpi_datatype, MSS_ROOT, comm);
#else
  MPI_Bcast(&skeys, nsplitter, pkey_mpi_datatype, MSS_ROOT, comm);
#endif

  sp->displs[0] = 0;
  for (i = 0; i < nsplitter; ++i)
  {
    elem_assign_at(s, sp->displs[i], &e);
    e.size = s->size - sp->displs[i];

    sp->displs[i + 1] = sp->displs[i] + sl_search_binary_lt(&e, &skeys[i]);

    printf("%d: %" slint_fmt ": %" sl_key_pure_type_fmt " -> displs[%" slint_fmt "] = %d\n", rank, i, skeys[i], i + 1, sp->displs[i + 1]);
  }
  
  return 0;
}
