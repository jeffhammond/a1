
#include <stdio.h>
#include <mpi.h>


#include "mpi_fmm_sort.h"


/*#define VERBOSE
#define TIMING
#define VALIDATE
#define CHECKSUM*/

/*#define FMM_SORT_RADIX_1BIT*/

#define SORT_BACK_ONE2NCHARGES

/*#define SL_OUTPUT_TO_FILE*/

#define DO_MPI_INIT

#define MPI_SENDRECV_REPLACE_MAX_SIZE  10000000


#ifdef FMM_SORT_RADIX_1BIT
# define fmm_sort_radix(_prefix_, _s_, _sx_, _h_, _l_, _w_)  _prefix_##sort_radix_1bit(_s_, _sx_, _h_, _l_)
#else
# define fmm_sort_radix(_prefix_, _s_, _sx_, _h_, _l_, _w_)  _prefix_##sort_radix(_s_, _sx_, _h_, _l_, _w_)
#endif


INTEGER_C mpi_log2_floor(INTEGER_C v)
{
  INTEGER_C x = 0;

  v >>= 1;

  while (v)
  {
    x++;
    v >>= 1;
  }

  return x;
}


#ifndef NO_SL_FRONT

void mpi_fmm_sort_front_body(void *mem0, void *mem1, pint_t *mem_sizes, pint_t *depth, pint_t *subx, pint_t *n, front_(slkey_t) *ibox, front_(sldata0_t) *xyz, front_(sldata1_t) *q, pint_t *addr_desc, void *addr, front_(sldata2_t) *scr, pint_t *type)
{
  typedef front_(slint_t) front_slint_t;

  front_slint_t front_type, i, highest;

#ifndef NOT_sl_front_xqsa0
  front_xqsa0_elements_t s0, *sx0, smem0;
  front_xqsa0_merge2x_f m20 = front_xqsa0_merge2_compo_hula;  /* front_xqsa0_merge2_basic_straight_01_x front_xqsa0_merge2_compo_tridgell front_xqsa0_merge2_compo_hula */
#endif
#ifndef NOT_sl_front_xqsaI
  front_xqsaI_elements_t s1, *sx1, smem1;
  front_xqsaI_merge2x_f m21 = front_xqsaI_merge2_compo_hula;  /* front_xqsaI_merge2_basic_straight_01_x front_xqsaI_merge2_compo_tridgell front_xqsaI_merge2_compo_hula */
#endif
#ifndef NOT_sl_front_xqsaX
  front_xqsaX_elements_t s2, *sx2, smem2;
  front_xqsaX_merge2x_f m22 = front_xqsaX_merge2_compo_hula;  /* front_xqsaX_merge2_basic_straight_01_x front_xqsaX_merge2_compo_tridgell front_xqsaX_merge2_compo_hula */
#endif
#ifndef NOT_sl_front_xq_a0
  front_xq_a0_elements_t s3, *sx3, smem3;
  front_xq_a0_merge2x_f m23 = front_xq_a0_merge2_compo_hula;  /* front_xq_a0_merge2_basic_straight_01_x front_xq_a0_merge2_compo_tridgell front_xq_a0_merge2_compo_hula */
#endif
#ifndef NOT_sl_front_xq_aI
  front_xq_aI_elements_t s4, *sx4, smem4;
  front_xq_aI_merge2x_f m24 = front_xq_aI_merge2_compo_hula;  /* front_xq_aI_merge2_basic_straight_01_x front_xq_aI_merge2_compo_tridgell front_xq_aI_merge2_compo_hula */
#endif
#ifndef NOT_sl_front_xq_aX
  front_xq_aX_elements_t s5, *sx5, smem5;
  front_xq_aX_merge2x_f m25 = front_xq_aX_merge2_compo_hula;  /* front_xq_aX_merge2_basic_straight_01_x front_xq_aX_merge2_compo_tridgell front_xq_aX_merge2_compo_hula */
#endif

  void *mems[] = { mem0, mem1 };
  void *xmem;
  pint_t xmem_size;

#ifdef TIMING
  double ta, tr, tm;
#endif

#ifdef VALIDATE
  front_slint_t o, l;
# ifdef CHECKSUM
  unsigned int crc32_in, crc32_out;
# endif
#endif

  MPI_Comm comm;
  int size, rank;
#ifdef DO_MPI_INIT
  int flag;
#endif

#ifdef SL_OUTPUT_TO_FILE
  char output_file_str[32];
  FILE *sl_debug_fstream;
#endif


#ifdef TIMING
  ta = MPI_Wtime();
#endif

#ifdef DO_MPI_INIT
  MPI_Initialized(&flag);
  if (!flag) MPI_Init(NULL, NULL);
#endif

  comm = MPI_COMM_WORLD;

  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);

  if (type && *type >= 0)
  {
    switch (*type)
    {
      case 0: front_type = 1; break;
      case 1: front_type = 0; break;
    }

  } else
  {
    if (scr != NULL) front_type = 0;
    else front_type = 1;
  }
  
  front_type *= 3;

  if (addr_desc == NULL)
  {
#ifdef VERBOSE
    if (rank == 0) printf("front: addr_desc = %p\n", addr_desc);
#endif
    front_type += 1;

  } else
  {
#ifdef VERBOSE
    if (rank == 0) printf("front: addr_desc = %" PARAM_INTEGER_FMT ", %" PARAM_INTEGER_FMT "\n", addr_desc[0], addr_desc[1]);
#endif
    if (addr_desc[0] == 0) front_type += 0;
    else if (addr_desc[0] == sizeof(INTEGER_C) && addr_desc[1] <= 0) front_type += 1;
    else front_type += 2;
  }

#ifdef VERBOSE
  if (rank == 0) printf("front: type = %" front_slint_fmt "\n", front_type);
#endif

  if (rank == 0)
  {
    switch (front_type)
    {
#ifdef NOT_sl_front_xqsa0
      case 0:
        fprintf(stderr, "error: sl_front_xqsa0 required!!!\n");
        break;
#endif
#ifdef NOT_sl_front_xqsaI
      case 1:
        fprintf(stderr, "error: sl_front_xqsaI required!!!\n");
        break;
#endif
#ifdef NOT_sl_front_xqsaX
      case 2:
        fprintf(stderr, "error: sl_front_xqsaX required!!!\n");
        break;
#endif
#ifdef NOT_sl_front_xq_a0
      case 3:
        fprintf(stderr, "error: sl_front_xq_a0 required!!!\n");
        break;
#endif
#ifdef NOT_sl_front_xq_aI
      case 4:
        fprintf(stderr, "error: sl_front_xq_aI required!!!\n");
        break;
#endif
#ifdef NOT_sl_front_xq_aX
      case 5:
        fprintf(stderr, "error: sl_front_xq_aX required!!!\n");
        break;
#endif
    }
  }

#ifdef SL_OUTPUT_TO_FILE
  sprintf(output_file_str, "sort_front.debug.%d", rank);
  front_xqsa0(front_xqsa0_sl_debug_fstream = )
  front_xqsaI(front_xqsaI_sl_debug_fstream = )
  front_xqsaX(front_xqsaX_sl_debug_fstream = )
  front_xq_a0(front_xq_a0_sl_debug_fstream = )
  front_xq_aI(front_xq_aI_sl_debug_fstream = )
  front_xq_aX(front_xq_aX_sl_debug_fstream = ) sl_debug_fstream = fopen(output_file_str, "w");
#endif

  front_xqsa0(front_xqsa0_sl_mpi_rank = )
  front_xqsaI(front_xqsaI_sl_mpi_rank = )
  front_xqsaX(front_xqsaX_sl_mpi_rank = )
  front_xq_a0(front_xq_a0_sl_mpi_rank = )
  front_xq_aI(front_xq_aI_sl_mpi_rank = )
  front_xq_aX(front_xq_aX_sl_mpi_rank = ) rank;

  front_aX = addr_desc[0];

  if (mem_sizes && ((mems[0] && mem_sizes[0] > 0) || (mems[1] && mem_sizes[1])))
  {
    switch (front_type)
    {
#ifndef NOT_sl_front_xqsa0
      case 0:
        front_xqsa0_elements_alloc_from_blocks(&smem0, 2, mems, mem_sizes, 8, *n, SLCM_KEYS|SLCM_DATA);
        sx0 = &smem0;
        break;
#endif
#ifndef NOT_sl_front_xqsaI
      case 1:
        front_xqsaI_elements_alloc_from_blocks(&smem1, 2, mems, mem_sizes, 8, *n, SLCM_KEYS|SLCM_DATA);
        sx1 = &smem1;
        break;
#endif
#ifndef NOT_sl_front_xqsaX
      case 2:
        front_xqsaX_elements_alloc_from_blocks(&smem2, 2, mems, mem_sizes, 8, *n, SLCM_KEYS|SLCM_DATA);
        sx2 = &smem2;
        break;
#endif
#ifndef NOT_sl_front_xq_a0
      case 3:
        front_xq_a0_elements_alloc_from_blocks(&smem3, 2, mems, mem_sizes, 8, *n, SLCM_KEYS|SLCM_DATA);
        sx3 = &smem3;
        break;
#endif
#ifndef NOT_sl_front_xq_aI
      case 4:
        front_xq_aI_elements_alloc_from_blocks(&smem4, 2, mems, mem_sizes, 8, *n, SLCM_KEYS|SLCM_DATA);
        sx4 = &smem4;
        break;
#endif
#ifndef NOT_sl_front_xq_aX
      case 5:
        front_xq_aX_elements_alloc_from_blocks(&smem5, 2, mems, mem_sizes, 8, *n, SLCM_KEYS|SLCM_DATA);
        sx5 = &smem5;
        break;
#endif
    }

  } else
  {
    front_xqsa0(sx0 = NULL;)
    front_xqsaI(sx1 = NULL;)
    front_xqsaX(sx2 = NULL;)
    front_xq_a0(sx3 = NULL;)
    front_xq_aI(sx4 = NULL;)
    front_xq_aX(sx5 = NULL;)
  }
  
#ifdef VERBOSE
  if (rank == 0)
  {
    printf("front_xqsa: type = %" front_slint_fmt "\n", front_type);
    printf("front_xqsa: sizeof(front_slint_t) = %d byte - #elements: %" PARAM_INTEGER_FMT " - mem0: %p / %" PARAM_INTEGER_FMT " bytes, mem1: %p / %" PARAM_INTEGER_FMT " bytes\n", (int) sizeof(front_(slint_t)), *n, mem0, (mem_sizes)?mem_sizes[0]:0, mem1, (mem_sizes)?mem_sizes[1]:0);
    printf("front_xqsa: ibox:      sizeof(front_slkey_t)   = %d byte\n", (int) sizeof(front_(slkey_t)));
    printf("front_xqsa:  xyz: %d * sizeof(front_sldata0_t) = %d * %d byte\n", (int) front_(sl_data0_size_c), (int) front_(sl_data0_size_c), (int) sizeof(front_(sldata0_t)));
    printf("front_xqsa:    q: %d * sizeof(front_sldata1_t) = %d * %d byte\n", (int) front_(sl_data1_size_c), (int) front_(sl_data1_size_c), (int) sizeof(front_(sldata1_t)));
    printf("front_xqsa:  scr: %d * sizeof(front_sldata2_t) = %d * %d byte\n", (int) front_(sl_data2_size_c), (int) front_(sl_data2_size_c), (int) sizeof(front_(sldata2_t)));
/*    switch (front_type)
    {
      case 0:
      case 3:
        printf("front_xqsa: addr: %d * sizeof(front_sldata3_t) = %d * %d byte\n", (int) front_xqsa0_sl_data3_size_c, (int) front_xqsa0_sl_data3_size_c, (int) sizeof(front_xqsa0_sldata3_t));
        break;
      case 1:
      case 4:
        printf("front_xqsa: addr: %d * sizeof(front_sldata3_t) = %d * %d byte\n", (int) front_xqsaI_sl_data3_size_c, (int) front_xqsaI_sl_data3_size_c, (int) sizeof(front_xqsaI_sldata3_t));
        break;
      case 2:
      case 5:
        printf("front_xqsa: addr: %d * sizeof(front_sldata3_t) = %d * %d byte\n", (int) front_xqsaX_sl_data3_size_c, (int) front_xqsaX_sl_data3_size_c, (int) sizeof(front_xqsaX_sldata3_t));
       break;
    }*/
  }
#endif

  if (*subx != 0) for (i = 0; i < *n; i++) ibox[i] -= *subx;

  switch (front_type)
  {
#ifndef NOT_sl_front_xqsa0
    case 0:
      s0.size = s0.max_size= *n;
      s0.keys = ibox;
      s0.data0 = xyz;
      s0.data1 = q;
      s0.data2 = scr;
/*      s0.data3 = addr;*/
      break;
#endif
#ifndef NOT_sl_front_xqsaI
    case 1:
      s1.size = s1.max_size= *n;
      s1.keys = ibox;
      s1.data0 = xyz;
      s1.data1 = q;
      s1.data2 = scr;
      s1.data3 = addr;
      break;
#endif
#ifndef NOT_sl_front_xqsaX
    case 2:
      s2.size = s2.max_size= *n;
      s2.keys = ibox;
      s2.data0 = xyz;
      s2.data1 = q;
      s2.data2 = scr;
      s2.data3 = addr;
      break;
#endif
#ifndef NOT_sl_front_xq_a0
    case 3:
      s3.size = s3.max_size= *n;
      s3.keys = ibox;
      s3.data0 = xyz;
      s3.data1 = q;
/*      s3.data2 = scr;*/
/*      s3.data3 = addr;*/
      break;
#endif
#ifndef NOT_sl_front_xq_aI
    case 4:
      s4.size = s4.max_size= *n;
      s4.keys = ibox;
      s4.data0 = xyz;
      s4.data1 = q;
/*      s4.data2 = scr;*/
      s4.data3 = addr;
      break;
#endif
#ifndef NOT_sl_front_xq_aX
    case 5:
      s5.size = s5.max_size= *n;
      s5.keys = ibox;
      s5.data0 = xyz;
      s5.data1 = q;
/*      s5.data2 = scr;*/
      s5.data3 = addr;
      break;
#endif
  }

#if defined(VALIDATE) && defined(CHECKSUM)
  switch (front_type)
  {
#ifndef NOT_sl_front_xqsa0
    case 0: crc32_in = front_xqsa0_mpi_elements_crc32(&s0, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_front_xqsaI
    case 1: crc32_in = front_xqsaI_mpi_elements_crc32(&s1, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_front_xqsaX
    case 2: crc32_in = front_xqsaX_mpi_elements_crc32(&s2, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_front_xq_a0
    case 3: crc32_in = front_xq_a0_mpi_elements_crc32(&s3, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_front_xq_aI
    case 4: crc32_in = front_xq_aI_mpi_elements_crc32(&s4, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_front_xq_aX
    case 5: crc32_in = front_xq_aX_mpi_elements_crc32(&s5, 1, 1, 1, size, rank, comm); break;
#endif
  }
#endif

  if (depth != NULL)
  {
    if (*depth > 0) highest = 3 * *depth - 1; else highest = -1;
  }

  key_mask = ~((front_(slkey_t)) 0);
  if (addr_desc != NULL && addr_desc[1] > 0) key_mask = ~(key_mask << (sizeof(front_(slkey_t)) * 8 - addr_desc[1]));

#ifdef VERBOSE
  if (rank == 0)
  {
    if (depth == NULL) printf("front: starting local radix-sort (lowest 3 bit)\n");
    else printf("front: starting local radix-sort (depth = %" PARAM_INTEGER_FMT " -> sorting bits [0..%" front_slint_fmt "])\n", *depth, highest);
  }
#endif

#ifdef TIMING
  tr = MPI_Wtime();
#endif

  switch (front_type)
  {
#ifndef NOT_sl_front_xqsa0
    case 0:
      if (depth == NULL) front_xqsa0_sort_radix_iter(&s0, sx0, 1, 2, 0, -1);
      else fmm_sort_radix(front_xqsa0_, &s0, sx0, highest, -1, -1);
      break;
#endif
#ifndef NOT_sl_front_xqsaI
    case 1:
      if (depth == NULL) front_xqsaI_sort_radix_iter(&s1, sx1, 1, 2, 0, -1);
      else fmm_sort_radix(front_xqsaI_, &s1, sx1, highest, -1, -1);
      break;
#endif
#ifndef NOT_sl_front_xqsaX
    case 2:
      if (depth == NULL) front_xqsaX_sort_radix_iter(&s2, sx2, 1, 2, 0, -1);
      else fmm_sort_radix(front_xqsaX_, &s2, sx2, highest, -1, -1);
      break;
#endif
#ifndef NOT_sl_front_xq_a0
    case 3:
      if (depth == NULL) front_xq_a0_sort_radix_iter(&s3, sx3, 1, 2, 0, -1);
      else fmm_sort_radix(front_xq_a0_, &s3, sx3, highest, -1, -1);
      break;
#endif
#ifndef NOT_sl_front_xq_aI
    case 4:
      if (depth == NULL) front_xq_aI_sort_radix_iter(&s4, sx4, 1, 2, 0, -1);
      else fmm_sort_radix(front_xq_aI_, &s4, sx4, highest, -1, -1);
      break;
#endif
#ifndef NOT_sl_front_xq_aX
    case 5:
      if (depth == NULL) front_xq_aX_sort_radix_iter(&s5, sx5, 1, 2, 0, -1);
      else fmm_sort_radix(front_xq_aX_, &s5, sx5, highest, -1, -1);
      break;
#endif
  }

#ifdef TIMING
  tr = MPI_Wtime() - tr;
#endif

#ifdef VERBOSE
  if (rank == 0) printf("front: local radix-sort done\n");
#endif

#ifdef VALIDATE
  switch (front_type)
  {
#ifndef NOT_sl_front_xqsa0
    case 0: l = front_xqsa0_elements_validate_order(&s0, 1); break;
#endif
#ifndef NOT_sl_front_xqsaI
    case 1: l = front_xqsaI_elements_validate_order(&s1, 1); break;
#endif
#ifndef NOT_sl_front_xqsaX
    case 2: l = front_xqsaX_elements_validate_order(&s2, 1); break;
#endif
#ifndef NOT_sl_front_xq_a0
    case 3: l = front_xq_a0_elements_validate_order(&s3, 1); break;
#endif
#ifndef NOT_sl_front_xq_aI
    case 4: l = front_xq_aI_elements_validate_order(&s4, 1); break;
#endif
#ifndef NOT_sl_front_xq_aX
    case 5: l = front_xq_aX_elements_validate_order(&s5, 1); break;
#endif
    default: l = 1;
  }
#endif

  switch (front_type)
  {
#ifndef NOT_sl_front_xqsa0
    case 0: front_xqsa0_mpi_datatypes_init(); break;
#endif
#ifndef NOT_sl_front_xqsaI
    case 1: front_xqsaI_mpi_datatypes_init(); break;
#endif
#ifndef NOT_sl_front_xqsaX
    case 2: front_xqsaX_mpi_datatypes_init(); break;
#endif
#ifndef NOT_sl_front_xq_a0
    case 3: front_xq_a0_mpi_datatypes_init(); break;
#endif
#ifndef NOT_sl_front_xq_aI
    case 4: front_xq_aI_mpi_datatypes_init(); break;
#endif
#ifndef NOT_sl_front_xq_aX
    case 5: front_xq_aX_mpi_datatypes_init(); break;
#endif
  }

#ifdef VERBOSE
  if (rank == 0) printf("front: %d: starting parallel sort (num. proc. = %d)\n", rank, size);
#endif

  xmem = NULL;
  xmem_size = 0;

  if (mem_sizes)
  {
    if (mem0 && mem_sizes[0] > xmem_size)
    {
      xmem = mem0;
      xmem_size = mem_sizes[0];
    }
    if (mem1 && mem_sizes[1] > xmem_size)
    {
      xmem = mem1;
      xmem_size = mem_sizes[1];
    }
  }

#ifdef MPI_SENDRECV_REPLACE_MAX_SIZE
  front_xqsa0(front_xqsa0_me_sendrecv_replace_mpi_maxsize = )
  front_xqsaI(front_xqsaI_me_sendrecv_replace_mpi_maxsize = )
  front_xqsaX(front_xqsaX_me_sendrecv_replace_mpi_maxsize = )
  front_xq_a0(front_xq_a0_me_sendrecv_replace_mpi_maxsize = )
  front_xq_aI(front_xq_aI_me_sendrecv_replace_mpi_maxsize = )
  front_xq_aX(front_xq_aX_me_sendrecv_replace_mpi_maxsize = ) MPI_SENDRECV_REPLACE_MAX_SIZE;
#endif

#ifdef TIMING
  tm = MPI_Wtime();
#endif

  switch (front_type)
  {
#ifndef NOT_sl_front_xqsa0
    case 0:
      if (xmem && xmem_size)
      {
        front_xqsa0_me_sendrecv_replace_mem = xmem;
        front_xqsa0_me_sendrecv_replace_memsize = xmem_size;

        m20 = front_xqsa0_merge2_memory_adaptive;
      }
      front_xqsa0_mpi_mergek(&s0, front_xqsa0_sn_batcher, NULL, m20, sx0, size, rank, comm);
      break;
#endif
#ifndef NOT_sl_front_xqsaI
    case 1:
      if (xmem && xmem_size)
      {
        front_xqsaI_me_sendrecv_replace_mem = xmem;
        front_xqsaI_me_sendrecv_replace_memsize = xmem_size;

        m21 = front_xqsaI_merge2_memory_adaptive;
      }
      front_xqsaI_mpi_mergek(&s1, front_xqsaI_sn_batcher, NULL, m21, sx1, size, rank, comm);
      break;
#endif
#ifndef NOT_sl_front_xqsaX
    case 2:
      if (xmem && xmem_size)
      {
        front_xqsaX_me_sendrecv_replace_mem = xmem;
        front_xqsaX_me_sendrecv_replace_memsize = xmem_size;

        m22 = front_xqsaX_merge2_memory_adaptive;
      }
      front_xqsaX_mpi_mergek(&s2, front_xqsaX_sn_batcher, NULL, m22, sx2, size, rank, comm);
      break;
#endif
#ifndef NOT_sl_front_xq_a0
    case 3:
      if (xmem && xmem_size)
      {
        front_xq_a0_me_sendrecv_replace_mem = xmem;
        front_xq_a0_me_sendrecv_replace_memsize = xmem_size;

        m23 = front_xq_a0_merge2_memory_adaptive;
      }
      front_xq_a0_mpi_mergek(&s3, front_xq_a0_sn_batcher, NULL, m23, sx3, size, rank, comm);
      break;
#endif
#ifndef NOT_sl_front_xq_aI
    case 4:
      if (xmem && xmem_size)
      {
        front_xq_aI_me_sendrecv_replace_mem = xmem;
        front_xq_aI_me_sendrecv_replace_memsize = xmem_size;

        m24 = front_xq_aI_merge2_memory_adaptive;
      }
      front_xq_aI_mpi_mergek(&s4, front_xq_aI_sn_batcher, NULL, m24, sx4, size, rank, comm);
      break;
#endif
#ifndef NOT_sl_front_xq_aX
    case 5:
      if (xmem && xmem_size)
      {
        front_xq_aX_me_sendrecv_replace_mem = xmem;
        front_xq_aX_me_sendrecv_replace_memsize = xmem_size;

        m25 = front_xq_aX_merge2_memory_adaptive;
      }
      front_xq_aX_mpi_mergek(&s5, front_xq_aX_sn_batcher, NULL, m25, sx5, size, rank, comm);
      break;
#endif
  }

#ifdef TIMING
  tm = MPI_Wtime() - tm;
#endif

#ifdef VERBOSE
  if (rank == 0) printf("front: %d: parallel sort done\n", rank);
#endif

  switch (front_type)
  {
#ifndef NOT_sl_front_xqsa0
    case 0: front_xqsa0_mpi_datatypes_release(); break;
#endif
#ifndef NOT_sl_front_xqsaI
    case 1: front_xqsaI_mpi_datatypes_release(); break;
#endif
#ifndef NOT_sl_front_xqsaX
    case 2: front_xqsaX_mpi_datatypes_release(); break;
#endif
#ifndef NOT_sl_front_xq_a0
    case 3: front_xq_a0_mpi_datatypes_release(); break;
#endif
#ifndef NOT_sl_front_xq_aI
    case 4: front_xq_aI_mpi_datatypes_release(); break;
#endif
#ifndef NOT_sl_front_xq_aX
    case 5: front_xq_aX_mpi_datatypes_release(); break;
#endif
  }

  if (*subx != 0) for (i = 0; i < *n; i++) ibox[i] += *subx;

#ifdef VALIDATE
  switch (front_type)
  {
#ifndef NOT_sl_front_xqsa0
    case 0: o = front_xqsa0_mpi_elements_validate_order(&s0, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_front_xqsaI
    case 1: o = front_xqsaI_mpi_elements_validate_order(&s1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_front_xqsaX
    case 2: o = front_xqsaX_mpi_elements_validate_order(&s2, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_front_xq_a0
    case 3: o = front_xq_a0_mpi_elements_validate_order(&s3, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_front_xq_aI
    case 4: o = front_xq_aI_mpi_elements_validate_order(&s4, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_front_xq_aX
    case 5: o = front_xq_aX_mpi_elements_validate_order(&s5, 1, size, rank, comm); break;
#endif
    default: o = 1;
  }
# ifndef VERBOSE
  if (o)
# endif
    if (rank == 0) printf("front: %d: sorting_order: %s - local_order: %s\n", rank, (!o)?"success":"FAILED", (!l)?"success":"failed");

# ifdef CHECKSUM
  switch (front_type)
  {
#ifndef NOT_sl_front_xqsa0
    case 0: crc32_out = front_xqsa0_mpi_elements_crc32(&s0, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_front_xqsaI
    case 1: crc32_out = front_xqsaI_mpi_elements_crc32(&s1, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_front_xqsaX
    case 2: crc32_out = front_xqsaX_mpi_elements_crc32(&s2, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_front_xq_a0
    case 3: crc32_out = front_xq_a0_mpi_elements_crc32(&s3, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_front_xq_aI
    case 4: crc32_out = front_xq_aI_mpi_elements_crc32(&s4, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_front_xq_aX
    case 5: crc32_out = front_xq_aX_mpi_elements_crc32(&s5, 1, 1, 1, size, rank, comm); break;
#endif
  }
#  ifndef VERBOSE
  if (crc32_in == crc32_out)
#  endif
    if (rank == 0) printf("front: %d: checksum: %s (%X vs. %X)\n", rank, (crc32_in == crc32_out)?"success":"FAILED", crc32_in, crc32_out);
# endif
#endif

#ifdef SL_OUTPUT_TO_FILE
  fclose(sl_debug_fstream);
  front_xqsa0(front_xqsa0_sl_debug_fstream = )
  front_xqsaI(front_xqsaI_sl_debug_fstream = )
  front_xqsaX(front_xqsaX_sl_debug_fstream = )
  front_xq_a0(front_xq_a0_sl_debug_fstream = )
  front_xq_aI(front_xq_aI_sl_debug_fstream = )
  front_xq_aX(front_xq_aX_sl_debug_fstream = ) sl_debug_fstream = NULL;
#endif

#ifdef TIMING
  ta = MPI_Wtime() - ta;

  printf("front: %d: tr = %f, tm = %f, ta = %f\n", rank, tr, tm, ta);
#endif
}


void mpi_fmm_sort_front_mem_(void *mem0, void *mem1, pint_t *mem_sizes, pint_t *depth, pint_t *subx, pint_t *n, front_(slkey_t) *ibox, front_(sldata0_t) *xyz, front_(sldata1_t) *q, pint_t *addr_desc, void *addr, front_(sldata2_t) *scr, pint_t *type);

#pragma weak mpi_fmm_sort_front_mem_=mpi_fmm_sort_front_mem
void mpi_fmm_sort_front_mem(void *mem0, void *mem1, pint_t *mem_sizes, pint_t *depth, pint_t *subx, pint_t *n, front_(slkey_t) *ibox, front_(sldata0_t) *xyz, front_(sldata1_t) *q, pint_t *addr_desc, void *addr, front_(sldata2_t) *scr, pint_t *type)
{
  mpi_fmm_sort_front_body(mem0, mem1, mem_sizes, depth, subx, n, ibox, xyz, q, addr_desc, addr, scr, type);
}


void mpi_fmm_sort_front_(pint_t *depth, pint_t *subx, pint_t *n, front_(slkey_t) *ibox, front_(sldata0_t) *xyz, front_(sldata1_t) *q, pint_t *addr_desc, void *addr, front_(sldata2_t) *scr, pint_t *type);

#pragma weak mpi_fmm_sort_front_=mpi_fmm_sort_front
void mpi_fmm_sort_front(pint_t *depth, pint_t *subx, pint_t *n, front_(slkey_t) *ibox, front_(sldata0_t) *xyz, front_(sldata1_t) *q, pint_t *addr_desc, void *addr, front_(sldata2_t) *scr, pint_t *type)
{
  mpi_fmm_sort_front_body(NULL, NULL, NULL, depth, subx, n, ibox, xyz, q, addr_desc, addr, scr, type);
}


void mpi_fmm_sort_front_3bit_mem_(void *mem0, void *mem1, pint_t *mem_sizes, pint_t *subx, pint_t *n, front_(slkey_t) *ibox, front_(sldata0_t) *xyz, front_(sldata1_t) *q, pint_t *addr_desc, void *addr, front_(sldata2_t) *scr, pint_t *type);

#pragma weak mpi_fmm_sort_front_3bit_mem_=mpi_fmm_sort_front_3bit_mem
void mpi_fmm_sort_front_3bit_mem(void *mem0, void *mem1, pint_t *mem_sizes, pint_t *subx, pint_t *n, front_(slkey_t) *ibox, front_(sldata0_t) *xyz, front_(sldata1_t) *q, pint_t *addr_desc, void *addr, front_(sldata2_t) *scr, pint_t *type)
{
  mpi_fmm_sort_front_body(mem0, mem1, mem_sizes, NULL, subx, n, ibox, xyz, q, addr_desc, addr, scr, type);
}


void mpi_fmm_sort_front_3bit_(pint_t *subx, pint_t *n, front_(slkey_t) *ibox, front_(sldata0_t) *xyz, front_(sldata1_t) *q, pint_t *addr_desc, void *addr, front_(sldata2_t) *scr, pint_t *type);

#pragma weak mpi_fmm_sort_front_3bit_=mpi_fmm_sort_front_3bit
void mpi_fmm_sort_front_3bit(pint_t *subx, pint_t *n, front_(slkey_t) *ibox, front_(sldata0_t) *xyz, front_(sldata1_t) *q, pint_t *addr_desc, void *addr, front_(sldata2_t) *scr, pint_t *type)
{
  mpi_fmm_sort_front_body(NULL, NULL, NULL, NULL, subx, n, ibox, xyz, q, addr_desc, addr, scr, type);
}

#endif /* NO_SL_FRONT */


#ifndef NO_SL_BACK

void mpi_fmm_sort_back_mem_(void *mem0, void *mem1, pint_t *mem_sizes, pint_t *ntotal, pint_t *n, back_(slkey_t) *addr, back_(sldata0_t) *q, back_(sldata1_t) *xyz, back_(sldata2_t) *pot, back_(sldata3_t) *grad, pint_t *type);

#pragma weak mpi_fmm_sort_back_mem_=mpi_fmm_sort_back_mem
void mpi_fmm_sort_back_mem(void *mem0, void *mem1, pint_t *mem_sizes, pint_t *ntotal, pint_t *n, back_(slkey_t) *addr, back_(sldata0_t) *q, back_(sldata1_t) *xyz, back_(sldata2_t) *pot, back_(sldata3_t) *grad, pint_t *type)
{
  typedef back_(slint_t) back_slint_t;

  back_slint_t back_type, highest;

#ifndef NOT_sl_back_qxpg
  back_qxpg_elements_t s0, *sx0, smem0;
  back_qxpg_merge2x_f m20 = back_qxpg_merge2_compo_hula;  /* back_qxpg_merge2_basic_straight_01_x back_qxpg_merge2_compo_tridgell back_qxpg_merge2_compo_hula */
#endif
#ifndef NOT_sl_back_qx_g
  back_qx_g_elements_t s1, *sx1, smem1;
  back_qx_g_merge2x_f m21 = back_qx_g_merge2_compo_hula;  /* back_qx_g_merge2_basic_straight_01_x back_qx_g_merge2_compo_tridgell back_qx_g_merge2_compo_hula */
#endif
#ifndef NOT_sl_back_q_pg
  back_q_pg_elements_t s2, *sx2, smem2;
  back_q_pg_merge2x_f m22 = back_q_pg_merge2_compo_hula;  /* back_q_pg_merge2_basic_straight_01_x back_q_pg_merge2_compo_tridgell back_q_pg_merge2_compo_hula */
#endif
#ifndef NOT_sl_back_q__g
  back_q__g_elements_t s3, *sx3, smem3;
  back_q__g_merge2x_f m23 = back_q__g_merge2_compo_hula;  /* back_q__g_merge2_basic_straight_01_x back_q__g_merge2_compo_tridgell back_q__g_merge2_compo_hula */
#endif

  void *mems[] = { mem0, mem1 };
  void *xmem;
  pint_t xmem_size;

#ifdef TIMING
  double tr, tm, ta;
#endif

#ifdef VALIDATE
  back_slint_t o, l;
# ifdef CHECKSUM
  unsigned int crc32_in, crc32_out;
# endif
#endif

  MPI_Comm comm;
  int size, rank;
#ifdef DO_MPI_INIT
  int flag;
#endif

#ifdef SL_OUTPUT_TO_FILE
  char output_file_str[32];
  FILE *sl_debug_fstream;
#endif


#ifdef TIMING
  ta = MPI_Wtime();
#endif

#ifdef DO_MPI_INIT
  MPI_Initialized(&flag);
  if (!flag) MPI_Init(NULL, NULL);
#endif

  comm = MPI_COMM_WORLD;

  MPI_Comm_size(comm, &size);
  MPI_Comm_rank(comm, &rank);

  if (type && *type >= 0) back_type = *type;
  else
  {
    if (xyz != NULL && pot != NULL) back_type = 0;
    else if (xyz != NULL && pot == NULL) back_type = 1;
    else if (xyz == NULL && pot != NULL) back_type = 2;
    else if (xyz == NULL && pot == NULL) back_type = 3;
    else back_type = -1;
  }

#ifdef VERBOSE
  if (rank == 0) printf("back: type = %" back_slint_fmt "\n", back_type);
#endif

#ifdef SL_OUTPUT_TO_FILE
  sprintf(output_file_str, "sort_back.debug.%d", rank);
  back_qxpg(back_qxpg_sl_debug_fstream = )
  back_qx_g(back_qx_g_sl_debug_fstream = )
  back_q_pg(back_q_pg_sl_debug_fstream = )
  back_q__g(back_q__g_sl_debug_fstream = ) sl_debug_fstream = fopen(output_file_str, "w");
#endif

  back_qxpg(back_qxpg_sl_mpi_rank = )
  back_qx_g(back_qx_g_sl_mpi_rank = )
  back_q_pg(back_q_pg_sl_mpi_rank = )
  back_q__g(back_q__g_sl_mpi_rank = ) rank;
  
  if (mem_sizes && ((mems[0] && mem_sizes[0] > 0) || (mems[1] && mem_sizes[1])))
  {
    switch (back_type)
    {
#ifndef NOT_sl_back_qxpg
      case 0:
        back_qxpg_elements_alloc_from_blocks(&smem0, 2, mems, mem_sizes, 8, *n, SLCM_KEYS|SLCM_DATA);
        sx0 = &smem0;
        break;
#endif
#ifndef NOT_sl_back_qx_g
      case 1:
        back_qx_g_elements_alloc_from_blocks(&smem1, 2, mems, mem_sizes, 8, *n, SLCM_KEYS|SLCM_DATA);
        sx1 = &smem1;
        break;
#endif
#ifndef NOT_sl_back_q_pg
      case 2:
        back_q_pg_elements_alloc_from_blocks(&smem2, 2, mems, mem_sizes, 8, *n, SLCM_KEYS|SLCM_DATA);
        sx2 = &smem2;
        break;
#endif
#ifndef NOT_sl_back_q__g
      case 3:
        back_q__g_elements_alloc_from_blocks(&smem3, 2, mems, mem_sizes, 8, *n, SLCM_KEYS|SLCM_DATA);
        sx3 = &smem3;
        break;
#endif
    }

  } else
  {
    back_qxpg(sx0 = NULL;)
    back_qx_g(sx1 = NULL;)
    back_q_pg(sx2 = NULL;)
    back_q__g(sx3 = NULL;)
  }

#ifdef VERBOSE
  if (rank == 0)
  {
    printf("back: sizeof(back_slint_t) = %d bytes - #elements: %" PARAM_INTEGER_FMT " - mem0: %p / %" PARAM_INTEGER_FMT " bytes, mem1: %p / %" PARAM_INTEGER_FMT " bytes\n", (int) sizeof(back_(slint_t)), *n, mem0, (mem_sizes)?mem_sizes[0]:0, mem1, (mem_sizes)?mem_sizes[1]:0);
    printf("back: addr:      sizeof(back_slkey_t)   = %d byte\n", (int) sizeof(back_(slkey_t)));
    printf("back:    q: %d * sizeof(back_sldata0_t) = %d * %d byte\n", (int) back_(sl_data0_size_c), (int) back_(sl_data0_size_c), (int) sizeof(back_(sldata0_t)));
    printf("back:  xyz: %d * sizeof(back_sldata1_t) = %d * %d byte\n", (int) back_(sl_data1_size_c), (int) back_(sl_data1_size_c), (int) sizeof(back_(sldata1_t)));
    printf("back:  pot: %d * sizeof(back_sldata2_t) = %d * %d byte\n", (int) back_(sl_data2_size_c), (int) back_(sl_data2_size_c), (int) sizeof(back_(sldata2_t)));
    printf("back: grad: %d * sizeof(back_sldata3_t) = %d * %d byte\n", (int) back_(sl_data3_size_c), (int) back_(sl_data3_size_c), (int) sizeof(back_(sldata3_t)));
  }
#endif

  switch (back_type)
  {
#ifndef NOT_sl_back_qxpg
    case 0:
      s0.size = s0.max_size = *n;
      s0.keys = addr;
      s0.data0 = q;
      s0.data1 = xyz;
      s0.data2 = pot;
      s0.data3 = grad;
      break;
#endif
#ifndef NOT_sl_back_qx_g
    case 1:
      s1.size = s1.max_size = *n;
      s1.keys = addr;
      s1.data0 = q;
      s1.data1 = xyz;
/*      s1.data2 = pot;*/
      s1.data3 = grad;
      break;
#endif
#ifndef NOT_sl_back_q_pg
    case 2:
      s2.size = s2.max_size = *n;
      s2.keys = addr;
      s2.data0 = q;
/*      s2.data1 = xyz;*/
      s2.data2 = pot;
      s2.data3 = grad;
      break;
#endif
#ifndef NOT_sl_back_q__g
    case 3:
      s3.size = s3.max_size = *n;
      s3.keys = addr;
      s3.data0 = q;
/*      s3.data1 = xyz;*/
/*      s3.data2 = pot;*/
      s3.data3 = grad;
      break;
#endif
  }

#if defined(VALIDATE) && defined(CHECKSUM)
  switch (back_type)
  {
#ifndef NOT_sl_back_qxpg
    case 0: crc32_in = back_qxpg_mpi_elements_crc32(&s0, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_back_qx_g
    case 1: crc32_in = back_qx_g_mpi_elements_crc32(&s1, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_back_q_pg
    case 2: crc32_in = back_q_pg_mpi_elements_crc32(&s2, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_back_q__g
    case 3: crc32_in = back_q__g_mpi_elements_crc32(&s3, 1, 1, 1, size, rank, comm); break;
#endif
  }
#endif

#ifdef SORT_BACK_ONE2NCHARGES
  highest = mpi_log2_floor(*ntotal);
#else
  highest = mpi_log2_floor(*ntotal - 1);
#endif

#ifdef VERBOSE
  if (rank == 0) printf("back: starting local radix-sort (total ncharges = %" PARAM_INTEGER_FMT " -> sorting bits [0..%" back_slint_fmt "])\n", *ntotal, highest);
#endif

#ifdef TIMING
  tr = MPI_Wtime();
#endif

  switch (back_type)
  {
#ifndef NOT_sl_back_qxpg
    case 0: fmm_sort_radix(back_qxpg_, &s0, sx0, highest, -1, -1); break;
#endif
#ifndef NOT_sl_back_qx_g
    case 1: fmm_sort_radix(back_qx_g_, &s1, sx1, highest, -1, -1); break;
#endif
#ifndef NOT_sl_back_q_pg
    case 2: fmm_sort_radix(back_q_pg_, &s2, sx2, highest, -1, -1); break;
#endif
#ifndef NOT_sl_back_q__g
    case 3: fmm_sort_radix(back_q__g_, &s3, sx3, highest, -1, -1); break;
#endif
  }

#ifdef TIMING
  tr = MPI_Wtime() - tr;
#endif

#ifdef VERBOSE
  if (rank == 0) printf("back: local radix-sort done\n");
#endif

#ifdef VALIDATE
  switch (back_type)
  {
#ifndef NOT_sl_back_qxpg
    case 0: l = back_qxpg_elements_validate_order(&s0, 1); break;
#endif
#ifndef NOT_sl_back_qx_g
    case 1: l = back_qx_g_elements_validate_order(&s1, 1); break;
#endif
#ifndef NOT_sl_back_q_pg
    case 2: l = back_q_pg_elements_validate_order(&s2, 1); break;
#endif
#ifndef NOT_sl_back_q__g
    case 3: l = back_q__g_elements_validate_order(&s3, 1); break;
#endif
    default: l = 1;
  }
#endif

  switch (back_type)
  {
#ifndef NOT_sl_back_qxpg
    case 0: back_qxpg_mpi_datatypes_init(); break;
#endif
#ifndef NOT_sl_back_qx_g
    case 1: back_qx_g_mpi_datatypes_init(); break;
#endif
#ifndef NOT_sl_back_q_pg
    case 2: back_q_pg_mpi_datatypes_init(); break;
#endif
#ifndef NOT_sl_back_q__g
    case 3: back_q__g_mpi_datatypes_init(); break;
#endif
  }

#ifdef VERBOSE
  if (rank == 0) printf("back: %d: starting parallel sort (num. proc. = %d)\n", rank, size);
#endif

  xmem = NULL;
  xmem_size = 0;

  if (mem_sizes)
  {
    if (mem0 && mem_sizes[0] > xmem_size)
    {
      xmem = mem0;
      xmem_size = mem_sizes[0];
    }
    if (mem1 && mem_sizes[1] > xmem_size)
    {
      xmem = mem1;
      xmem_size = mem_sizes[1];
    }
  }

#ifdef MPI_SENDRECV_REPLACE_MAX_SIZE
  back_qxpg(back_qxpg_me_sendrecv_replace_mpi_maxsize = )
  back_qx_g(back_qx_g_me_sendrecv_replace_mpi_maxsize = )
  back_q_pg(back_q_pg_me_sendrecv_replace_mpi_maxsize = )
  back_q__g(back_q__g_me_sendrecv_replace_mpi_maxsize = ) MPI_SENDRECV_REPLACE_MAX_SIZE;
#endif

#ifdef TIMING
  tm = MPI_Wtime();
#endif

  switch (back_type)
  {
#ifndef NOT_sl_back_qxpg
    case 0:
      if (xmem && xmem_size)
      {
        back_qxpg_me_sendrecv_replace_mem = xmem;
        back_qxpg_me_sendrecv_replace_memsize = xmem_size;

        m20 = back_qxpg_merge2_memory_adaptive;
      }
      back_qxpg_mpi_mergek(&s0, back_qxpg_sn_batcher, NULL, m20, sx0, size, rank, comm);
      break;
#endif
#ifndef NOT_sl_back_qx_g
    case 1:
      if (xmem && xmem_size)
      {
        back_qx_g_me_sendrecv_replace_mem = xmem;
        back_qx_g_me_sendrecv_replace_memsize = xmem_size;

        m21 = back_qx_g_merge2_memory_adaptive;
      }
      back_qx_g_mpi_mergek(&s1, back_qx_g_sn_batcher, NULL, m21, sx1, size, rank, comm);
      break;
#endif
#ifndef NOT_sl_back_q_pg
    case 2:
      if (xmem && xmem_size)
      {
        back_q_pg_me_sendrecv_replace_mem = xmem;
        back_q_pg_me_sendrecv_replace_memsize = xmem_size;

        m22 = back_q_pg_merge2_memory_adaptive;
      }
      back_q_pg_mpi_mergek(&s2, back_q_pg_sn_batcher, NULL, m22, sx2, size, rank, comm);
      break;
#endif
#ifndef NOT_sl_back_q__g
    case 3:
      if (xmem && xmem_size)
      {
        back_q__g_me_sendrecv_replace_mem = xmem;
        back_q__g_me_sendrecv_replace_memsize = xmem_size;

        m23 = back_q__g_merge2_memory_adaptive;
      }
      back_q__g_mpi_mergek(&s3, back_q__g_sn_batcher, NULL, m23, sx3, size, rank, comm);
      break;
#endif
  }

#ifdef TIMING
  tm = MPI_Wtime() - tm;
#endif

#ifdef VERBOSE
  if (rank == 0) printf("back: %d: parallel sort done\n", rank);
#endif

  switch (back_type)
  {
#ifndef NOT_sl_back_qxpg
    case 0: back_qxpg_mpi_datatypes_release(); break;
#endif
#ifndef NOT_sl_back_qx_g
    case 1: back_qx_g_mpi_datatypes_release(); break;
#endif
#ifndef NOT_sl_back_q_pg
    case 2: back_q_pg_mpi_datatypes_release(); break;
#endif
#ifndef NOT_sl_back_q__g
    case 3: back_q__g_mpi_datatypes_release(); break;
#endif
  }

#ifdef VALIDATE
  switch (back_type)
  {
#ifndef NOT_sl_back_qxpg
    case 0: o = back_qxpg_mpi_elements_validate_order(&s0, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_back_qx_g
    case 1: o = back_qx_g_mpi_elements_validate_order(&s1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_back_q_pg
    case 2: o = back_q_pg_mpi_elements_validate_order(&s2, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_back_q__g
    case 3: o = back_q__g_mpi_elements_validate_order(&s3, 1, size, rank, comm); break;
#endif
    default: o = 1;
  }
# ifndef VERBOSE
  if (o)
# endif
    if (rank == 0) printf("back: %d: sorting_order: %s - local_order: %s\n", rank, (!o)?"success":"FAILED", (!l)?"success":"failed");

# ifdef CHECKSUM
  switch (back_type)
  {
#ifndef NOT_sl_back_qxpg
    case 0: crc32_out = back_qxpg_mpi_elements_crc32(&s0, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_back_qx_g
    case 1: crc32_out = back_qx_g_mpi_elements_crc32(&s1, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_back_q_pg
    case 2: crc32_out = back_q_pg_mpi_elements_crc32(&s2, 1, 1, 1, size, rank, comm); break;
#endif
#ifndef NOT_sl_back_q__g
    case 3: crc32_out = back_q__g_mpi_elements_crc32(&s3, 1, 1, 1, size, rank, comm); break;
#endif
  }
#  ifndef VERBOSE
  if (crc32_in == crc32_out)
#  endif
    if (rank == 0) printf("back: %d: checksum: %s (%X vs. %X)\n", rank, (crc32_in == crc32_out)?"success":"FAILED", crc32_in, crc32_out);
# endif
#endif

#ifdef SL_OUTPUT_TO_FILE
  fclose(sl_debug_fstream);
  back_qxpg(back_qxpg_sl_debug_fstream = )
  back_qx_g(back_qx_g_sl_debug_fstream = )
  back_q_pg(back_q_pg_sl_debug_fstream = )
  back_q__g(back_q__g_sl_debug_fstream = ) sl_debug_fstream = NULL;
#endif

#ifdef TIMING
  ta = MPI_Wtime() - ta;

  printf("back: %d: tr = %f, tm = %f, ta = %f\n", rank, tr, tm, ta);
#endif
}


void mpi_fmm_sort_back_(pint_t *ntotal, pint_t *n, back_(slkey_t) *addr, back_(sldata0_t) *q, back_(sldata1_t) *xyz, back_(sldata2_t) *pot, back_(sldata3_t) *grad, pint_t *type);

#pragma weak mpi_fmm_sort_back_=mpi_fmm_sort_back
void mpi_fmm_sort_back(pint_t *ntotal, pint_t *n, back_(slkey_t) *addr, back_(sldata0_t) *q, back_(sldata1_t) *xyz, back_(sldata2_t) *pot, back_(sldata3_t) *grad, pint_t *type)
{
  mpi_fmm_sort_back_mem(NULL, NULL, NULL, ntotal, n, addr, q, xyz, pot, grad, type);
}

#endif /* NO_SL_BACK */


/* deprecated */
#ifdef DEPRECATED

void mpi_fmm_sort_blue_mem_(void *mem, pint_t *mem_size, pint_t *depth, pint_t *subx, pint_t *n, front_xq_aI_slkey_t *ibox, front_xq_aI_sldata0_t *xyz, front_xq_aX_sldata1_t *q, front_xq_aX_sldata3_t *addr);

#pragma weak mpi_fmm_sort_blue_mem_=mpi_fmm_sort_blue_mem
void mpi_fmm_sort_blue_mem(void *mem, pint_t *mem_size, pint_t *depth, pint_t *subx, pint_t *n, front_xq_aI_slkey_t *ibox, front_xq_aI_sldata0_t *xyz, front_xq_aX_sldata1_t *q, front_xq_aX_sldata3_t *addr)
{
  pint_t mem_sizes[] = { (mem_size)?*mem_size:0, 0 };
  mpi_fmm_sort_front_body(mem, NULL, (mem_size)?mem_sizes:NULL, depth, subx, n, ibox, xyz, q, NULL, addr, NULL, NULL);
}


void mpi_fmm_sort_blue_(pint_t *depth, pint_t *subx, pint_t *n, front_xq_aI_slkey_t *ibox, front_xq_aI_sldata0_t *xyz, front_xq_aX_sldata1_t *q, front_xq_aX_sldata3_t *addr);

#pragma weak mpi_fmm_sort_blue_=mpi_fmm_sort_blue
void mpi_fmm_sort_blue(pint_t *depth, pint_t *subx, pint_t *n, front_xq_aI_slkey_t *ibox, front_xq_aI_sldata0_t *xyz, front_xq_aX_sldata1_t *q, front_xq_aX_sldata3_t *addr)
{
  mpi_fmm_sort_front_body(NULL, NULL, NULL, depth, subx, n, ibox, xyz, q, NULL, addr, NULL, NULL);
}


void mpi_fmm_sort_blue_3bit_mem_(void *mem, pint_t *mem_size, pint_t *subx, pint_t *n, front_xq_aI_slkey_t *ibox, front_xq_aI_sldata0_t *xyz, front_xq_aX_sldata1_t *q, front_xq_aX_sldata3_t *addr);

#pragma weak mpi_fmm_sort_blue_3bit_mem_=mpi_fmm_sort_blue_3bit_mem
void mpi_fmm_sort_blue_3bit_mem(void *mem, pint_t *mem_size, pint_t *subx, pint_t *n, front_xq_aI_slkey_t *ibox, front_xq_aI_sldata0_t *xyz, front_xq_aX_sldata1_t *q, front_xq_aX_sldata3_t *addr)
{
  pint_t mem_sizes[] = { (mem_size)?*mem_size:0, 0 };
  mpi_fmm_sort_front_body(mem, NULL, (mem_size)?mem_sizes:NULL, NULL, subx, n, ibox, xyz, q, NULL, addr, NULL, NULL);
}


void mpi_fmm_sort_blue_3bit_(pint_t *subx, pint_t *n, front_xq_aI_slkey_t *ibox, front_xq_aI_sldata0_t *xyz, front_xq_aX_sldata1_t *q, front_xq_aX_sldata3_t *addr);

#pragma weak mpi_fmm_sort_blue_3bit_=mpi_fmm_sort_blue_3bit
void mpi_fmm_sort_blue_3bit(pint_t *subx, pint_t *n, front_xq_aI_slkey_t *ibox, front_xq_aI_sldata0_t *xyz, front_xq_aX_sldata1_t *q, front_xq_aX_sldata3_t *addr)
{
  mpi_fmm_sort_front_body(NULL, NULL, NULL, NULL, subx, n, ibox, xyz, q, NULL, addr, NULL, NULL);
}


void mpi_fmm_sort_green_mem_(void *mem, pint_t *mem_size, pint_t *depth, pint_t *subx, pint_t *n, front_xqsaX_slkey_t *ibox, front_xqsaX_sldata0_t *xyz, front_xqsaX_sldata1_t *q, front_xqsaX_sldata3_t *addr, front_xqsaX_sldata2_t *scr);

#pragma weak mpi_fmm_sort_green_mem_=mpi_fmm_sort_green_mem
void mpi_fmm_sort_green_mem(void *mem, pint_t *mem_size, pint_t *depth, pint_t *subx, pint_t *n, front_xqsaX_slkey_t *ibox, front_xqsaX_sldata0_t *xyz, front_xqsaX_sldata1_t *q, front_xqsaX_sldata3_t *addr, front_xqsaX_sldata2_t *scr)
{
  pint_t mem_sizes[] = { (mem_size)?*mem_size:0, 0 };
  mpi_fmm_sort_front_body(mem, NULL, (mem_size)?mem_sizes:NULL, depth, subx, n, ibox, xyz, q, NULL, addr, scr, NULL);
}


void mpi_fmm_sort_green_(pint_t *depth, pint_t *subx, pint_t *n, front_xqsaX_slkey_t *ibox, front_xqsaX_sldata0_t *xyz, front_xqsaX_sldata1_t *q, front_xqsaX_sldata3_t *addr, front_xqsaX_sldata2_t *scr);

#pragma weak mpi_fmm_sort_green_=mpi_fmm_sort_green
void mpi_fmm_sort_green(pint_t *depth, pint_t *subx, pint_t *n, front_xqsaX_slkey_t *ibox, front_xqsaX_sldata0_t *xyz, front_xqsaX_sldata1_t *q, front_xqsaX_sldata3_t *addr, front_xqsaX_sldata2_t *scr)
{
  mpi_fmm_sort_front_body(NULL, NULL, NULL, depth, subx, n, ibox, xyz, q, NULL, addr, scr, NULL);
}


void mpi_fmm_sort_green_3bit_mem_(void *mem, pint_t *mem_size, pint_t *subx, pint_t *n, front_xqsaX_slkey_t *ibox, front_xqsaX_sldata0_t *xyz, front_xqsaX_sldata1_t *q, front_xqsaX_sldata3_t *addr, front_xqsaX_sldata2_t *scr);

#pragma weak mpi_fmm_sort_green_3bit_mem_=mpi_fmm_sort_green_3bit_mem
void mpi_fmm_sort_green_3bit_mem(void *mem, pint_t *mem_size, pint_t *subx, pint_t *n, front_xqsaX_slkey_t *ibox, front_xqsaX_sldata0_t *xyz, front_xqsaX_sldata1_t *q, front_xqsaX_sldata3_t *addr, front_xqsaX_sldata2_t *scr)
{
  pint_t mem_sizes[] = { (mem_size)?*mem_size:0, 0 };
  mpi_fmm_sort_front_body(mem, NULL, (mem_size)?mem_sizes:NULL, NULL, subx, n, ibox, xyz, q, NULL, addr, scr, NULL);
}


void mpi_fmm_sort_green_3bit_(pint_t *subx, pint_t *n, front_xqsaX_slkey_t *ibox, front_xqsaX_sldata0_t *xyz, front_xqsaX_sldata1_t *q, front_xqsaX_sldata3_t *addr, front_xqsaX_sldata2_t *scr);

#pragma weak mpi_fmm_sort_green_3bit_=mpi_fmm_sort_green_3bit
void mpi_fmm_sort_green_3bit(pint_t *subx, pint_t *n, front_xqsaX_slkey_t *ibox, front_xqsaX_sldata0_t *xyz, front_xqsaX_sldata1_t *q, front_xqsaX_sldata3_t *addr, front_xqsaX_sldata2_t *scr)
{
  mpi_fmm_sort_front_body(NULL, NULL, NULL, NULL, subx, n, ibox, xyz, q, NULL, addr, scr, NULL);
}


void mpi_fmm_sort_red_mem_(void *mem, pint_t *mem_size, pint_t *ntotal, pint_t *n, back_qxpg_slkey_t *addr, back_qxpg_sldata3_t *grad, back_qxpg_sldata2_t *pot, back_qxpg_sldata0_t *q);

#pragma weak mpi_fmm_sort_red_mem_=mpi_fmm_sort_red_mem
void mpi_fmm_sort_red_mem(void *mem, pint_t *mem_size, pint_t *ntotal, pint_t *n, back_qxpg_slkey_t *addr, back_qxpg_sldata3_t *grad, back_qxpg_sldata2_t *pot, back_qxpg_sldata0_t *q)
{
  pint_t mem_sizes[] = { (mem_size)?*mem_size:0, 0 };
  mpi_fmm_sort_back_mem(mem, NULL, (mem_size)?mem_sizes:NULL, ntotal, n, addr, q, NULL, pot, grad, NULL);
}


void mpi_fmm_sort_red_(pint_t *ntotal, pint_t *n, back_qxpg_slkey_t *addr, back_qxpg_sldata3_t *grad, back_qxpg_sldata2_t *pot, back_qxpg_sldata0_t *q);

#pragma weak mpi_fmm_sort_red_=mpi_fmm_sort_red
void mpi_fmm_sort_red(pint_t *ntotal, pint_t *n, back_qxpg_slkey_t *addr, back_qxpg_sldata3_t *grad, back_qxpg_sldata2_t *pot, back_qxpg_sldata0_t *q)
{
  mpi_fmm_sort_back_mem(NULL, NULL, NULL, ntotal, n, addr, q, NULL, pot, grad, NULL);
}


void mpi_fmm_sort_red_without_pot_mem_(void *mem, pint_t *mem_size, pint_t *ntotal, pint_t *n, back_qxpg_slkey_t *addr, back_qxpg_sldata3_t *grad, back_qxpg_sldata0_t *q);

#pragma weak mpi_fmm_sort_red_without_pot_mem_=mpi_fmm_sort_red_without_pot_mem
void mpi_fmm_sort_red_without_pot_mem(void *mem, pint_t *mem_size, pint_t *ntotal, pint_t *n, back_qxpg_slkey_t *addr, back_qxpg_sldata3_t *grad, back_qxpg_sldata0_t *q)
{
  pint_t mem_sizes[] = { (mem_size)?*mem_size:0, 0 };
  mpi_fmm_sort_back_mem(mem, NULL, (mem_size)?mem_sizes:NULL, ntotal, n, addr, q, NULL, NULL, grad, NULL);
}


void mpi_fmm_sort_red_without_pot_(pint_t *ntotal, pint_t *n, back_qxpg_slkey_t *addr, back_qxpg_sldata3_t *grad, back_qxpg_sldata0_t *q);

#pragma weak mpi_fmm_sort_red_without_pot_=mpi_fmm_sort_red_without_pot
void mpi_fmm_sort_red_without_pot(pint_t *ntotal, pint_t *n, back_qxpg_slkey_t *addr, back_qxpg_sldata3_t *grad, back_qxpg_sldata0_t *q)
{
  mpi_fmm_sort_back_mem(NULL, NULL, NULL, ntotal, n, addr, q, NULL, NULL, grad, NULL);
}

#endif /* DEPRECATED */

