/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_environment_intern.h
 *  timestamp: 2010-11-28 21:53:57 +0100
 *  
 */


#ifndef __SL_ENVIRONMENT_INTERN_H__
#define __SL_ENVIRONMENT_INTERN_H__


#ifndef ENV_TRACE_IF
# ifdef GLOBAL_TRACE_IF
#  define ENV_TRACE_IF  GLOBAL_TRACE_IF
# else
#  define ENV_TRACE_IF  (sl_mpi_rank == -1)
# endif
#endif


#define cc_sl_alloc_pre_hook(_n_, _s_) \
  SL_TRACE_IF(ENV_TRACE_IF, "sl_alloc: %" slint_fmt " * %" slint_fmt, (slint_t) (_n_), (slint_t) (_s_)), \
  cc_rti_minc_alloc() \
  cc_rti_malloc((_n_) * (_s_))

#define cc_sl_alloc_post_hook(_n_, _s_, _p_) \
  SL_TRACE_IF(ENV_TRACE_IF, "sl_alloc: %" slint_fmt " * %" slint_fmt " = %p", (slint_t) (_n_), (slint_t) (_s_), (_p_)),

#define sl_free_hook(_p_) \
  SL_TRACE_IF(ENV_TRACE_IF, "sl_free: %p", (_p_)); \
  rti_minc_free()

#define cc_sl_alloca_pre_hook(_n_, _s_) \
  SL_TRACE_IF(ENV_TRACE_IF, "sl_alloc: %" slint_fmt " * %" slint_fmt, (slint_t) (_n_), (slint_t) (_s_)),

#define cc_sl_alloca_post_hook(_n_, _s_, _p_) \
  SL_TRACE_IF(ENV_TRACE_IF, "sl_alloc: %" slint_fmt " * %" slint_fmt " = %p", (slint_t) (_n_), (slint_t) (_s_), (_p_)),

#define sl_freea_hook(_p_) \
  SL_TRACE_IF(ENV_TRACE_IF, "sl_free: %p", (_p_))


#ifndef sl_alloc
# undef sl_free
# ifdef SLDEBUG_ALLOC
#  define sl_alloc(_n_, _s_)  (cc_sl_alloc_pre_hook(_n_, _s_) sl_alloc_tmp_p = calloc((_n_), (_s_)), cc_sl_alloc_post_hook(_n_, _s_, sl_alloc_tmp_p) sl_alloc_tmp_p)
#  define sl_free(_p_)        SL_MOP(sl_free_hook(_p_); _p_ = NULL;)
# else
#  define sl_alloc(_n_, _s_)  (cc_sl_alloc_pre_hook(_n_, _s_) sl_alloc_tmp_p = malloc((_n_) * (_s_)), cc_sl_alloc_post_hook(_n_, _s_, sl_alloc_tmp_p) sl_alloc_tmp_p)
#  define sl_free(_p_)        SL_MOP(sl_free_hook(_p_); free(_p_);)
# endif
#else
# ifndef sl_free
#  error "sl_alloc defined, but sl_free is missing!"
# endif
#endif


#include <alloca.h>

#ifndef sl_alloca
# undef sl_freea
# define sl_alloca(_n_, _s_)  (cc_sl_alloca_pre_hook(_n_, _s_) sl_alloc_tmp_p = alloca((_n_) * (_s_)), cc_sl_alloca_post_hook(_n_, _s_, sl_alloc_tmp_p) sl_alloc_tmp_p)
# ifdef SLDEBUG_ALLOC
#  define sl_freea(_p_)       SL_MOP(sl_freea_hook(_p_); _p_ = NULL;)
# else
#  define sl_freea(_p_)       SL_MOP(sl_freea_hook(_p_);)
# endif
#else
# ifndef sl_freea
#  error "sl_alloca defined, but sl_freea is missing!"
# endif
#endif


#ifdef DECLARE_ENV
void *sl_alloc_tmp_p;  /* sl_var sl_alloc_tmp_p */
#endif
extern void *sl_alloc_tmp_p;


#ifndef sl_rand

 #undef sl_srand
 #undef SL_RAND_MIN
 #undef SL_RAND_MAX

# include <stdlib.h>

# define SL_RAND_MIN         0
# define SL_RAND_MAX         RAND_MAX
# define sl_rand()           rand()
# define sl_srand(s)         srand(s)

#endif

#ifndef sl_rand01
 #define sl_rand01()         ((double) sl_rand() / (double) SL_RAND_MAX)
#endif

#ifndef sl_rand11
 #define sl_rand11()         ((sl_rand01() * 2.0) - 1.0)
#endif

#ifndef sl_rand_minmax
 #define sl_rand_minmax(_min, _max)  (_min + ((double) (_max - _min) * (sl_rand() - SL_RAND_MIN) / (SL_RAND_MAX - SL_RAND_MIN)))
#endif


#ifndef sl_ts_type

 #undef sl_ts_save
 #undef sl_ts_diff_ms
 #undef sl_ts_get_ms
 #undef sl_ts_diff_s
 #undef sl_ts_get_s

 #ifdef SL_USE_MPI

  #define sl_ts_type                 double
  #define sl_ts_save(t)              (t = MPI_Wtime())
  #define sl_ts_diff_ms(from, to)    (((to) - (from)) * 1000)
  #define sl_ts_get_ms()             (MPI_Wtime() * 1000)
  #define sl_ts_diff_s(from, to)     (((to) - (from)))
  #define sl_ts_get_s()              (MPI_Wtime())

#  ifdef DECLARE_TS_MPI
#  endif

 #else

  #include <sys/time.h>

  #define sl_ts_type                 struct timeval
  #define sl_ts_save(t)              (gettimeofday(&(t), NULL))
  #define sl_ts_diff_ms(from, to)    ((double) (((to).tv_sec - (from).tv_sec) * 1000.0 + ((to).tv_usec - (from).tv_usec) / 1000.0))
  #define sl_ts_get_ms()             (sl_ts_save(ts_temp), (double) (ts_temp.tv_sec * 1000.0 + ts_temp.tv_usec / 1000.0))
  #define sl_ts_diff_s(from, to)     ((double) (((to).tv_sec - (from).tv_sec) + ((to).tv_usec - (from).tv_usec) / 1000000.0))
  #define sl_ts_get_s()              (sl_ts_save(ts_temp), (double) (ts_temp.tv_sec + ts_temp.tv_usec / 1000000.0))

#  ifdef DECLARE_TS
sl_ts_type ts_temp;  /* sl_var ts_temp */
#  endif
extern sl_ts_type ts_temp;

 #endif

#endif


#ifdef SL_DATA_IGNORE
 #undef declare_ts_temp
 #undef declare_ts_temp_mpi
#endif

#ifndef declare_ts_temp
 #define declare_ts_temp
#endif

#ifndef declare_ts_temp_mpi
 #define declare_ts_temp_mpi
#endif


#endif /* __SL_ENVIRONMENT_INTERN_H__ */
