/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/core_mpi/mpi_mergek_presort.c
 *  timestamp: 2010-06-17 12:20:35 +0200
 *  
 */


#include "sl_common.h"


#define DEBUG_OR_NOT  (sl_mpi_rank == -1)


slint_t mpi_mergek_presort_radix(mergek_t *mk, slint_t rmerge, elements_t *s_in, MPI_Comm *comms_in, slint_t parity_in, elements_t *s_out, MPI_Comm *comms_out, slint_t *parity_out, int size, int rank, MPI_Comm comm) /* sl_proto, sl_func mpi_mergek_presort_radix */
{
  slint_t i, j, k, l;

  int sizes_out[2], ranks_out[2];

  slkey_pure_t keys[2];
  MPI_Comm comms[2];
  int sizes[2], ranks[2];
  
  mergek_t _mk = { sn_batcher, NULL, merge2_basic_straight_01_x, NULL };
  

  if (mk == NULL) mk = &_mk;

  comms_out[0] = comms_out[1] = MPI_COMM_NULL;
  *parity_out = 0;
  elem_null(&s_out[0]);
  elem_null(&s_out[1]);
  sizes_out[0] = sizes_out[1] = 0;
  ranks_out[0] = ranks_out[1] = -1;

  SL_TRACE_IF(DEBUG_OR_NOT, "comms_in: (%" comm_fmt ",%" comm_fmt "), parity_in: %" slint_fmt ", keys: (%" slint_fmt ",%" slint_fmt "\n", comm_str(comms_in[0]), comm_str(comms_in[1]), parity_in, s_in[0].size, s_in[1].size);
  
  SL_TRACE_IF(DEBUG_OR_NOT, "%d: keys in #0 %" slint_fmt "\n", rank, s_in[0].size);
  if (DEBUG_OR_NOT) elements_print_keys(&s_in[0]);

  SL_TRACE_IF(DEBUG_OR_NOT, "%d: keys in #1 %" slint_fmt "\n", rank, s_in[1].size);
  if (DEBUG_OR_NOT) elements_print_keys(&s_in[1]);

  for (i = 0; i < 2; ++i)
  {
    j = (i + parity_in) % 2;
    
    SL_TRACE_IF(DEBUG_OR_NOT, "i = %" slint_fmt ", j = %" slint_fmt "", i, j);

    if (comms_in[j] == MPI_COMM_NULL) continue;

    keys[0] = s_in[j].keys[0] >> rmerge;
    keys[1] = s_in[j].keys[s_in[j].size - 1] >> rmerge;
    
    SL_TRACE_IF(DEBUG_OR_NOT, "keys = %" sl_key_pure_type_fmt ",%" sl_key_pure_type_fmt "", keys[0], keys[1]);

    mpi_linsplit(comms_in[j], keys, comms, parity_out, size, rank, comm);

    if (comms[0] != MPI_COMM_NULL)
    {
      MPI_Comm_size(comms[0], &sizes[0]);
      MPI_Comm_rank(comms[0], &ranks[0]);

      if (sizes[0] <= 1) MPI_Comm_free(&comms[0]);

    } else { sizes[0] = 0; ranks[0] = -1; }

    if (comms[1] != MPI_COMM_NULL)
    {
      MPI_Comm_size(comms[1], &sizes[1]);
      MPI_Comm_rank(comms[1], &ranks[1]);
    
      if (sizes[1] <= 1) MPI_Comm_free(&comms[1]);

    } else { sizes[1] = 0; ranks[1] = -1; }

    SL_TRACE_IF(DEBUG_OR_NOT, "comm#0: %d of %d - comm#1: %d of %d\n", ranks[0], sizes[0], ranks[1], sizes[1]);
    
    if (sizes[0] > 1 && sizes[1] > 1)
    {
      comms_out[0] = comms[0];
      comms_out[1] = comms[1];
      
      SL_TRACE_IF(DEBUG_OR_NOT, "#0 searching lt %" sl_key_pure_type_fmt " in:", (keys[0] + 1) << rmerge);
      if (DEBUG_OR_NOT) elements_print_keys(&s_in[j]);

      k = sl_search_binary_lt2(&s_in[j], (keys[0] + 1) << rmerge);
      elem_assign(&s_in[j], &s_out[0]);
      s_out[0].size = k;

      SL_TRACE_IF(DEBUG_OR_NOT, "#1 searching lt %" sl_key_pure_type_fmt " in:", keys[1] << rmerge);
      if (DEBUG_OR_NOT) elements_print_keys(&s_in[j]);

      k = sl_search_binary_lt2(&s_in[j], keys[1] << rmerge);
      elem_assign_at(&s_in[j], k, &s_out[1]);
      s_out[1].size = s_in[j].size - k;

      sizes_out[0] = sizes[0];
      sizes_out[1] = sizes[1];

      ranks_out[0] = ranks[0];
      ranks_out[1] = ranks[1];

    } else
    {
/*      l = *parity_out;*/
      l = j;

      if (comms[0] != MPI_COMM_NULL)
      {
        comms_out[l] = comms[0];
        sizes_out[l] = sizes[0];
        ranks_out[l] = ranks[0];
      }

      if (comms[1] != MPI_COMM_NULL)
      {
        comms_out[l] = comms[1];
        sizes_out[l] = sizes[1];
        ranks_out[l] = ranks[1];
      }

      elem_assign(&s_in[j], &s_out[l]);
    }
  }

  SL_TRACE_IF(DEBUG_OR_NOT, "%d: keys out #0 %" slint_fmt "\n", rank, s_out[0].size);
  if (DEBUG_OR_NOT) elements_print_keys(&s_out[0]);

  SL_TRACE_IF(DEBUG_OR_NOT, "%d: keys out #1 %" slint_fmt "\n", rank, s_out[1].size);
  if (DEBUG_OR_NOT) elements_print_keys(&s_out[1]);

  SL_TRACE_IF(DEBUG_OR_NOT, "mergek_2: (%" slint_fmt ",%d,%d,%" comm_fmt ") and (%" slint_fmt ",%d,%d,%" comm_fmt ")", s_out[0].size, sizes_out[0], ranks_out[0], comm_str(comms_out[0]), s_out[1].size, sizes_out[1], ranks_out[1], comm_str(comms_out[1]));

  mpi_mergek_equal2(s_out, mk->sn, mk->snd, mk->m2x, mk->sx, sizes_out, ranks_out, comms_out);
  
  return 0;
}
