
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef SL_USE_MPI
# include <mpi.h>
#endif

#include "test_fmm.h"

#ifdef SL_USE_MPI
MPI_Comm comm;
int comm_size, comm_rank;
#else
# define comm_size  1
# define comm_rank  0
#endif

#define PRINT_PROGRESS  10


void fmm_create(fmm_data_t *fmm, fmm_info_t *nfo)
{
  pint_t total_mem = 0;

  fmm->nglobal = (nfo->nparticles_per_process >= 0)?(nfo->nparticles_per_process * comm_size):nfo->nparticles;
  fmm->nlocal = (fmm->nglobal / comm_size) + ((comm_rank < fmm->nglobal % comm_size)?1:0);

  fmm->ibox = malloc(fmm->nlocal * sizeof(INTEGER_C));
  total_mem += fmm->nlocal * sizeof(INTEGER_C);
  
  fmm->xyz = malloc(fmm->nlocal * 3 * sizeof(REAL_C));
  total_mem += fmm->nlocal * 3 * sizeof(REAL_C);
  
  fmm->q = malloc(fmm->nlocal * sizeof(REAL_C));
  total_mem += fmm->nlocal * sizeof(REAL_C);

  fmm->ibox_mask = ~((INTEGER_C) 0);
  if (nfo->addr_byte >= 0 && nfo->addr_iboxbits >= 0)
  {
    fmm->addr_desc[0] = (nfo->addr_byte > 0)?nfo->addr_byte:0;
    fmm->addr_desc[1] = (nfo->addr_iboxbits > 0)?nfo->addr_iboxbits:0;

    if (fmm->addr_desc[1] > 0) fmm->ibox_mask = ~(fmm->ibox_mask << (sizeof(INTEGER_C) * 8 - fmm->addr_desc[1]));

  } else
  {
    fmm->addr_desc[0] = sizeof(INTEGER_C);
    fmm->addr_desc[1] = 0;
  }
  if (fmm->addr_desc[0] > 0)
  {
    fmm->addr = malloc(fmm->nlocal * fmm->addr_desc[0]);
    total_mem += fmm->nlocal * fmm->addr_desc[0];

  } else fmm->addr = NULL;

  if (nfo->scratch)
  {
    fmm->scr = malloc(fmm->nlocal * sizeof(INTEGER_C));
    total_mem += fmm->nlocal * sizeof(INTEGER_C);

  } else fmm->scr = NULL;

  if (nfo->results)
  {
    fmm->pot = malloc(fmm->nlocal * sizeof(REAL_C));
    total_mem += fmm->nlocal * sizeof(REAL_C);

  } else fmm->pot = NULL;

  if (nfo->results || nfo->grad_mem)
  {
    fmm->grad = malloc(fmm->nlocal * 3 * sizeof(REAL_C));
    total_mem += fmm->nlocal * 3 * sizeof(REAL_C);

  } else fmm->grad = NULL;
  
  if (nfo->max_mem > 0 && total_mem < nfo->max_mem)
  {
    fmm->mem_size = nfo->max_mem - total_mem;
    fmm->mem = malloc(fmm->mem_size);
    
  } else
  {
    fmm->mem_size = 0;
    fmm->mem = NULL;
  }
  
  if (nfo->grad_mem)
  {
    fmm->mem0 = fmm->grad;
    fmm->mem_sizes[0] = fmm->nlocal * 3 * sizeof(REAL_C);

  } else
  {
    fmm->mem0 = NULL;
    fmm->mem_sizes[0] = 0;
  }
  
  fmm->mem1 = fmm->mem;
  fmm->mem_sizes[1] = fmm->mem_size;
}


void fmm_init_particles(fmm_data_t *fmm)
{
  pint_t i, base, i_first, i_next;
#ifdef PRINT_PROGRESS
  pint_t k;
#endif
  INTEGER_C a;
  unsigned char *addr_c;


#ifdef SL_USE_MPI
  z_srand(2501 * (comm_rank + 1));
#endif

  fmm->depth = 0;

  base = 0;
#ifdef SL_USE_MPI
  MPI_Exscan(&fmm->nlocal, &base, 1, PARAM_INTEGER_MPI, MPI_SUM, comm);
#endif

  addr_c = fmm->addr;

  i_first = 0;
  i_next = fmm->nlocal;

#ifdef PRINT_PROGRESS
  for (k = 0; k < PRINT_PROGRESS; ++k)
  {
    i_next = ((k + 1) * fmm->nlocal) / PRINT_PROGRESS;
#endif

    for (i = i_first; i < i_next; ++i)
    {
      fmm->ibox[i] = 0;

      fmm->xyz[3 * i + 0] = (REAL_C) z_rand() / (REAL_C) Z_RAND_MAX;
      fmm->xyz[3 * i + 1] = (REAL_C) z_rand() / (REAL_C) Z_RAND_MAX;
      fmm->xyz[3 * i + 2] = (REAL_C) z_rand() / (REAL_C) Z_RAND_MAX;

      fmm->q[i] = (REAL_C) z_rand() / (REAL_C) Z_RAND_MAX;

      a = base + i;

      /* copy lowest fmm->addr_desc[0] bytes to addr */
      if (fmm->addr_desc[0] == sizeof(INTEGER_C)) ((INTEGER_C *) fmm->addr)[i] = a;
      else switch (fmm->addr_desc[0])
      {
        case 8: *addr_c = a & 0xFF; ++addr_c; a >>= 8;
        case 7: *addr_c = a & 0xFF; ++addr_c; a >>= 8;
        case 6: *addr_c = a & 0xFF; ++addr_c; a >>= 8;
        case 5: *addr_c = a & 0xFF; ++addr_c; a >>= 8;
        case 4: *addr_c = a & 0xFF; ++addr_c; a >>= 8;
        case 3: *addr_c = a & 0xFF; ++addr_c; a >>= 8;
        case 2: *addr_c = a & 0xFF; ++addr_c; a >>= 8;
        case 1: *addr_c = a & 0xFF; ++addr_c; a >>= 8;
      }

      if (fmm->addr_desc[1] > 0)
      {
        /* mask out everything on the left of the address bits */
        a &= ~(~((INTEGER_C) 0) << fmm->addr_desc[1]);

        /* move remaining highest address bits to the outer left */
        a <<= sizeof(a) * 8 - fmm->addr_desc[1];

      } else a = 0;

      /* blend highest address bits (left) into the box number (right) */
      fmm->ibox[i] |= a;
    }

#ifdef PRINT_PROGRESS
    z_printf("%" PARAM_INTEGER_FMT " %%\n", (i * 100) / fmm->nlocal);

    i_first = i_next;
  }
#endif
}


void fmm_move_particles(fmm_data_t *fmm, pint_t nmoves)
{
  pint_t i, j;
  
  
  for (j = 0; j < nmoves; ++j)
  {
    i = (((j + 1) * fmm->nlocal) / nmoves) - 1;

/*    printf("%d: changing %d\n", (int) j, (int) i);*/
  
    fmm->xyz[3 * i + 0] = (REAL_C) z_rand() / (REAL_C) Z_RAND_MAX;
    fmm->xyz[3 * i + 1] = (REAL_C) z_rand() / (REAL_C) Z_RAND_MAX;
    fmm->xyz[3 * i + 2] = (REAL_C) z_rand() / (REAL_C) Z_RAND_MAX;
  }
}


void fmm_inc_depth(fmm_data_t *fmm)
{
  pint_t i, i_first, i_next;
#ifdef PRINT_PROGRESS
  pint_t k;
#endif
  REAL_C v;


  i_first = 0;
  i_next = fmm->nlocal;

#ifdef PRINT_PROGRESS
  for (k = 0; k < PRINT_PROGRESS; ++k)
  {
    i_next = ((k + 1) * fmm->nlocal) / PRINT_PROGRESS;
#endif

    for (i = i_first; i < i_next; ++i)
    {
      fmm->ibox[i] = (fmm->ibox[i] & ~fmm->ibox_mask) + ((fmm->ibox[i] & fmm->ibox_mask) << 3);
  
      v = fmm->xyz[3 * i + 0] * pow(2.0, fmm->depth);
      v -= trunc(v);
      fmm->ibox[i] += (v < 0.5)?0:1;

      v = fmm->xyz[3 * i + 1] * pow(2.0, fmm->depth);
      v -= trunc(v);
      fmm->ibox[i] += (v < 0.5)?0:2;

      v = fmm->xyz[3 * i + 2] * pow(2.0, fmm->depth);
      v -= trunc(v);
      fmm->ibox[i] += (v < 0.5)?0:4;
    }

#ifdef PRINT_PROGRESS
    z_printf("%" PARAM_INTEGER_FMT " %%\n", (i * 100) / fmm->nlocal);

    i_first = i_next;
  }
#endif

  ++fmm->depth;
}


void fmm_scalecoordinates(fmm_data_t *fmm, double scale, pint_t depth)
{
  pint_t i, j, i_first, i_next;
#ifdef PRINT_PROGRESS
  pint_t k;
#endif
  REAL_C v[3];
  INTEGER_C b;


  i_first = 0;
  i_next = fmm->nlocal;

#ifdef PRINT_PROGRESS
  for (k = 0; k < PRINT_PROGRESS; ++k)
  {
    i_next = ((k + 1) * fmm->nlocal) / PRINT_PROGRESS;
#endif

    for (i = i_first; i < i_next; ++i)
    {
      v[0] = (fmm->xyz[3 * i + 0] *= scale);
      v[1] = (fmm->xyz[3 * i + 1] *= scale);
      v[2] = (fmm->xyz[3 * i + 2] *= scale);

      b = 0;
      for (j = 0; j < depth; ++j)
      {
        b <<= 3;

        b += (v[0] < 0.5)?0:1;
        v[0] = 2.0 * v[0] - trunc(2.0 * v[0]);

        b += (v[1] < 0.5)?0:2;
        v[1] = 2.0 * v[1] - trunc(2.0 * v[1]);

        b += (v[2] < 0.5)?0:4;
        v[2] = 2.0 * v[2] - trunc(2.0 * v[2]);
      }
      fmm->ibox[i] = (fmm->ibox[i] & ~fmm->ibox_mask) + b;
    }

    if (fmm->scr)
    for (i = i_first; i < i_next; ++i) fmm->scr[i] = 1;

#ifdef PRINT_PROGRESS
    z_printf("%" PARAM_INTEGER_FMT " %%\n", (i * 100) / fmm->nlocal);

    i_first = i_next;
  }
#endif

  fmm->depth = depth;
}


void fmm_unscalecoordinates(fmm_data_t *fmm, double scale)
{
  pint_t i, i_first, i_next;
#ifdef PRINT_PROGRESS
  pint_t k;
#endif


  i_first = 0;
  i_next = fmm->nlocal;

#ifdef PRINT_PROGRESS
  for (k = 0; k < PRINT_PROGRESS; ++k)
  {
    i_next = ((k + 1) * fmm->nlocal) / PRINT_PROGRESS;
#endif

    for (i = i_first; i < i_next; ++i)
    {
      fmm->xyz[3 * i + 0] /= scale;
      fmm->xyz[3 * i + 1] /= scale;
      fmm->xyz[3 * i + 2] /= scale;
    }

#ifdef PRINT_PROGRESS
    z_printf("%" PARAM_INTEGER_FMT " %%\n", (i * 100) / fmm->nlocal);

    i_first = i_next;
  }
#endif
}


void fmm_calculate(fmm_data_t *fmm)
{
  pint_t i, i_first, i_next;
#ifdef PRINT_PROGRESS
  pint_t k;
#endif

  if (fmm->pot && fmm->grad)
  {
    i_first = 0;
    i_next = fmm->nlocal;

#ifdef PRINT_PROGRESS
    for (k = 0; k < PRINT_PROGRESS; ++k)
    {
      i_next = ((k + 1) * fmm->nlocal) / PRINT_PROGRESS;
#endif
      for (i = i_first; i < i_next; ++i)
      {
        fmm->pot[i] = i + 0.6;

        fmm->grad[3 * i + 0] = i + 0.1;
        fmm->grad[3 * i + 1] = i + 0.2;
        fmm->grad[3 * i + 2] = i + 0.3;
      }

#ifdef PRINT_PROGRESS
      z_printf("%" PARAM_INTEGER_FMT " %%\n", (i * 100) / fmm->nlocal);

      i_first = i_next;
    }
#endif
  }
}


void fmm_restore(fmm_data_t *fmm, double scale, INTEGER_C **addr)
{
  pint_t i, i_first, i_next;
#ifdef PRINT_PROGRESS
  pint_t k;
#endif
  INTEGER_C a, b;
  unsigned char *addr_c;

  addr_c = fmm->addr;
  addr_c += fmm->addr_desc[0] - 1;

  i_first = 0;
  i_next = fmm->nlocal;

#ifdef PRINT_PROGRESS
  for (k = 0; k < PRINT_PROGRESS; ++k)
  {
    i_next = ((k + 1) * fmm->nlocal) / PRINT_PROGRESS;
#endif
    for (i = i_first; i < i_next; ++i)
    {
      fmm->xyz[3 * i + 0] /= scale;
      fmm->xyz[3 * i + 1] /= scale;
      fmm->xyz[3 * i + 2] /= scale;

      a = 0;

      if (fmm->addr_desc[0] == sizeof(INTEGER_C)) a = ((INTEGER_C *) fmm->addr)[i];
      else switch (fmm->addr_desc[0])
      {
        case 8: a <<= 8; a += *addr_c; --addr_c;
        case 7: a <<= 8; a += *addr_c; --addr_c;
        case 6: a <<= 8; a += *addr_c; --addr_c;
        case 5: a <<= 8; a += *addr_c; --addr_c;
        case 4: a <<= 8; a += *addr_c; --addr_c;
        case 3: a <<= 8; a += *addr_c; --addr_c;
        case 2: a <<= 8; a += *addr_c; --addr_c;
        case 1: a <<= 8; a += *addr_c; --addr_c;
      }

      addr_c += 2 * fmm->addr_desc[0];

      if (fmm->addr_desc[1] > 0)
      {
        b = fmm->ibox[i];

        /* move highest address bits to the outer right */
        b >>= sizeof(b) * 8 - fmm->addr_desc[1];

        /* mask out everything on the left of the address bits */
        b &= ~(~((INTEGER_C) 0) << fmm->addr_desc[1]);

      } else b = 0;
    
      if (sizeof(b) > fmm->addr_desc[0]) a |= (b << fmm->addr_desc[0] * 8);

      fmm->ibox[i] = a;
    }

#ifdef PRINT_PROGRESS
    z_printf("%" PARAM_INTEGER_FMT " %%\n", (i * 100) / fmm->nlocal);

    i_first = i_next;
  }
#endif

  *addr = fmm->ibox;

  fmm->mem0 = NULL;
  fmm->mem_sizes[0] = 0;
}


void fmm_destroy(fmm_data_t *fmm)
{
  if (fmm->ibox) free(fmm->ibox);
  if (fmm->xyz) free(fmm->xyz);
  if (fmm->q) free(fmm->q);
  if (fmm->addr) free(fmm->addr);
  if (fmm->scr) free(fmm->scr);
  if (fmm->pot) free(fmm->pot);
  if (fmm->grad) free(fmm->grad);
  if (fmm->mem) free(fmm->mem);
}


void fmm_print(fmm_data_t *fmm, INTEGER_C *keys)
{
  pint_t i, j;
  INTEGER_C mask = ~0;

  if (keys == NULL)
  {
    keys = fmm->ibox;
    mask = fmm->ibox_mask;
  }

/*  for (i = 0; i < fmm->nlocal; ++i) printf("%" INTEGER_FMT ": %.16llX / %" INTEGER_FMT " [%f,%f,%f] [%f] %.16llX\n", i, keys[i] & mask, keys[i] & mask, fmm->xyz[3 * i + 0], fmm->xyz[3 * i + 1], fmm->xyz[3 * i + 2], fmm->q[i], fmm->ibox[i]);*/

  for (i = 0; i < fmm->nlocal; ++i)
  {
    printf("%" INTEGER_FMT ": %.16llX / %" INTEGER_FMT " [%f,%f,%f] [%f] %.16llX ", i, keys[i] & mask, keys[i] & mask, fmm->xyz[3 * i + 0], fmm->xyz[3 * i + 1], fmm->xyz[3 * i + 2], fmm->q[i], fmm->ibox[i]);

    for (j = 0; j < fmm->addr_desc[0]; ++j) printf(" %.2X", (unsigned int) *(((unsigned char *) fmm->addr) + (i * fmm->addr_desc[0]) + j));

    if (fmm->grad) printf(" [%f,%f,%f] ", fmm->grad[3 * i + 0], fmm->grad[3 * i + 1], fmm->grad[3 * i + 2]);

    printf("\n");
  }
}
