
#ifndef __TEST_FMM_H__
#define __TEST_FMM_H__


#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef SL_USE_MPI
# include <mpi.h>
#endif


#include "fmm_sort.h"
#ifdef SL_USE_MPI
# include "mpi_fmm_sort.h"
#endif


#define z_max(_a_,_b_)            (((_a_)>(_b_))?(_a_):(_b_))
#define z_min(_a_,_b_)            (((_a_)<(_b_))?(_a_):(_b_))

#if defined(JUMP) || defined(TETHYS)
# include <stdlib.h>
# define Z_RAND_MIN     0
# define Z_RAND_MAX     0x7FFFFFFFL
# define z_rand()       random()
# define z_srand(_s_)   srandom(_s_)
#else
# include <stdlib.h>
# define Z_RAND_MIN     0
# define Z_RAND_MAX     RAND_MAX
# define z_rand()       random()
# define z_srand(_s_)   srandom(_s_)
#endif


time_t timer;

#ifdef SL_USE_MPI

# define z_printf(_s_, _x_...)  do { if (comm_rank == 0) { timer = time(NULL); printf("%d: %.*s: " _s_, comm_rank, (int) strlen(ctime(&timer))-1, ctime(&timer), ##_x_); } } while (0)

# define z_wtime()              MPI_Wtime()

# define MODSTR                 "mpi"
# define SELECTSORT(_s_)        mpi_##_s_
# define NTOTALN(_nt_, _n_)     _nt_, _n_

#else

# define z_printf(_s_, _x_...)  do { timer = time(NULL); printf("%.*s: " _s_, (int) strlen(ctime(&timer))-1, ctime(&timer), ##_x_); } while (0)

#include <sys/time.h>

struct timeval tv;
# define z_wtime()              (gettimeofday(&tv, NULL), (double) (tv.tv_sec + tv.tv_usec / 1000000.0))

# define MODSTR                 "seq"
# define SELECTSORT(_s_)        _s_
# define NTOTALN(_nt_, _n_)     _n_

#endif


typedef struct _fmm_data_t
{
  pint_t nlocal, nglobal;
  pint_t depth;

  INTEGER_C *ibox;
  INTEGER_C ibox_mask;
  REAL_C *xyz;
  REAL_C *q;
  INTEGER_C addr_desc[2];
  void *addr;
  INTEGER_C *scr;
  REAL_C *pot;
  REAL_C *grad;

  void *mem;
  pint_t mem_size;

  void *mem0, *mem1;
  pint_t mem_sizes[2];

} fmm_data_t;


typedef struct _fmm_info_t
{
  pint_t nparticles;
  pint_t nparticles_per_process;
  pint_t max_depth;
  pint_t depth;
  double frac;
  pint_t addr_byte;
  pint_t addr_iboxbits;
  pint_t max_mem;
  pint_t grad_mem;
  
  pint_t scratch;
  pint_t results;

  pint_t sort_front;
  pint_t sort_back;

  double move_rate;
  pint_t rounds;
 
} fmm_info_t;


#ifdef SL_USE_MPI
extern MPI_Comm comm;
extern int comm_size, comm_rank;
#else
#define comm_size  1
#define comm_rank  0
#endif


void fmm_create(fmm_data_t *fmm, fmm_info_t *nfo);
void fmm_init_particles(fmm_data_t *fmm);
void fmm_move_particles(fmm_data_t *fmm, pint_t nmoves);
void fmm_inc_depth(fmm_data_t *fmm);
void fmm_scalecoordinates(fmm_data_t *fmm, double scale, pint_t depth);
void fmm_unscalecoordinates(fmm_data_t *fmm, double scale);
void fmm_calculate(fmm_data_t *fmm);
void fmm_restore(fmm_data_t *fmm, double scale, INTEGER_C **addr);
void fmm_destroy(fmm_data_t *fmm);
void fmm_print(fmm_data_t *fmm, INTEGER_C *keys);


void test_seq_fmm(int argc, char *argv[]);

#ifdef SL_USE_MPI
void test_mpi_fmm(int argc, char *argv[]);
#endif


#endif /* __TEST_FMM_H__ */
