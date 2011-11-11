
#ifndef __SL_BACK_Q_PG_H__
#define __SL_BACK_Q_PG_H__

#ifdef SL_USE_MPI
 #include <mpi.h>
#endif /* SL_USE_MPI */

#include "sl_rti.h"

#define SL_PROTO(_f_)  _f_

#include "fortran2c_types.h"
#include "config_types.h"


/* enable/disable MPI */
/*#define SL_USE_MPI*/


/* standard (SL) integer data type */
#define back_q_pg_sl_int_type_c             SL_INTEGER_C
#define back_q_pg_sl_int_type_mpi           SL_INTEGER_MPI
#define back_q_pg_sl_int_size_mpi           1
#define back_q_pg_sl_int_type_fmt           SL_INTEGER_FMT


/* key section */
#define back_q_pg_sl_key_type_c             INTEGER_C
#define back_q_pg_sl_key_type_mpi           INTEGER_MPI
#define back_q_pg_sl_key_size_mpi           1

#define back_q_pg_sl_key_integer
#define back_q_pg_sl_key_type_fmt           INTEGER_FMT


/* data section */
#define back_q_pg_SL_DATA0                  /* q */
#define back_q_pg_sl_data0_type_c           REAL_C
#define back_q_pg_sl_data0_size_c           1
#define back_q_pg_sl_data0_type_mpi         REAL_MPI
#define back_q_pg_sl_data0_size_mpi         1

/*#define back_q_pg_SL_DATA1*/                  /* xyz */
#define back_q_pg_sl_data1_type_c           REAL_C
#define back_q_pg_sl_data1_size_c           3
#define back_q_pg_sl_data1_type_mpi         REAL_MPI
#define back_q_pg_sl_data1_size_mpi         3

#define back_q_pg_SL_DATA2                  /* pot */
#define back_q_pg_sl_data2_type_c           REAL_C
#define back_q_pg_sl_data2_size_c           1
#define back_q_pg_sl_data2_type_mpi         REAL_MPI
#define back_q_pg_sl_data2_size_mpi         1

#define back_q_pg_SL_DATA3                  /* grad */
#define back_q_pg_sl_data3_type_c           REAL_C
#define back_q_pg_sl_data3_size_c           3
#define back_q_pg_sl_data3_type_mpi         REAL_MPI
#define back_q_pg_sl_data3_size_mpi         3
/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_config_intern.h
 *  timestamp: 2011-01-12 09:55:18 +0100
 *  
 */




/* override SL_USE_MPI from sl_config.h */
#ifdef SL_USE_MPI_IGNORE
# undef SL_USE_MPI
#endif

#ifdef SL_USE_MPI_FORCE
# ifndef SL_USE_MPI
#  define SL_USE_MPI
# endif
#endif


/* override inlining */
#ifdef NO_INLINE
# ifndef inline
#  define inline
# endif
#endif


#ifndef back_q_pg_SL_INDEX
# undef back_q_pg_SL_PACKED_INDEX
#endif


/* if no special, given, primary and heavy used integer-type ... */
#ifndef back_q_pg_sl_int_type_c
  /* ... use a default one */
# define back_q_pg_sl_int_type_c               long      /* sl_macro */
# undef back_q_pg_sl_int_type_mpi
# define back_q_pg_sl_int_type_mpi             MPI_LONG  /* sl_macro */
# undef back_q_pg_sl_int_size_mpi
# define back_q_pg_sl_int_size_mpi             1         /* sl_macro */
# undef back_q_pg_sl_int_type_fmt
# define back_q_pg_sl_int_type_fmt             "ld"      /* sl_macro */
#else
  /* ... use the given one and check whether an mpi-type is present and required */
# ifdef SL_USE_MPI
#  if !defined(back_q_pg_sl_int_type_mpi) || !defined(back_q_pg_sl_int_size_mpi)
#   error "back_q_pg_sl_int_type_mpi and/or back_q_pg_sl_int_size_mpi missing"
#  endif
# endif
# ifndef back_q_pg_sl_int_type_fmt
#  error "back_q_pg_sl_int_type_fmt macro is missing, using d as default"
#  define back_q_pg_sl_int_type_fmt  "d"
# endif
#endif


/* if no special datatype for (intern) weight ... */
#ifndef back_q_pg_sl_weight_type_c
 /* ... use the double */
# define back_q_pg_sl_weight_type_c             double      /* sl_macro */
# undef back_q_pg_sl_weight_type_mpi
# define back_q_pg_sl_weight_type_mpi           MPI_DOUBLE  /* sl_macro */
# undef back_q_pg_sl_weight_size_mpi
# define back_q_pg_sl_weight_size_mpi           1           /* sl_macro */
# undef back_q_pg_sl_weight_type_fmt
# define back_q_pg_sl_weight_type_fmt           "f"         /* sl_macro */
#else
  /* ... use the given one and check whether an mpi-type is present and required */
# ifdef SL_USE_MPI
#  if !defined(back_q_pg_sl_weight_type_mpi) || !defined(back_q_pg_sl_weight_size_mpi)
#   error "back_q_pg_sl_weight_type_mpi and/or back_q_pg_sl_weight_size_mpi missing"
#  endif
# endif
# ifndef back_q_pg_sl_weight_type_fmt
#  error "back_q_pg_sl_weight_type_fmt macro is missing, using f as default"
#  define back_q_pg_sl_weight_type_fmt  "f"
# endif
#endif


/* if no special datatype for indexes ... */
#ifndef back_q_pg_sl_index_type_c
 /* ... use the primary integer type */
# define back_q_pg_sl_index_type_c             back_q_pg_sl_int_type_c
# undef back_q_pg_sl_index_type_mpi
# define back_q_pg_sl_index_type_mpi           back_q_pg_sl_int_type_mpi
# undef back_q_pg_sl_index_size_mpi
# define back_q_pg_sl_index_size_mpi           back_q_pg_sl_int_size_mpi
# undef back_q_pg_sl_index_type_fmt
# define back_q_pg_sl_index_type_fmt           back_q_pg_sl_int_type_fmt
#else
  /* ... use the given one and check whether an mpi-type is present and required */
# ifdef SL_USE_MPI
#  if !defined(back_q_pg_sl_index_type_mpi) || !defined(back_q_pg_sl_index_size_mpi)
#   error "back_q_pg_sl_index_type_mpi and/or back_q_pg_sl_index_size_mpi missing"
#  endif
# endif
# ifndef back_q_pg_sl_index_type_fmt
#  error "back_q_pg_sl_index_type_fmt macro is missing, using d as default"
#  define back_q_pg_sl_index_type_fmt  "d"
# endif
#endif


/* default pure keys */
#ifndef back_q_pg_sl_key_pure_type_c
# define back_q_pg_sl_key_pure_type_c          back_q_pg_sl_key_type_c  /* sl_macro */
#endif
#ifndef back_q_pg_sl_key_pure_type_mpi
# define back_q_pg_sl_key_pure_type_mpi        back_q_pg_sl_key_type_mpi  /* sl_macro */
#endif
#ifndef back_q_pg_sl_key_pure_size_mpi
# define back_q_pg_sl_key_pure_size_mpi        back_q_pg_sl_key_size_mpi  /* sl_macro */
#endif
#ifndef back_q_pg_sl_key_pure_type_fmt
# ifdef back_q_pg_sl_key_type_fmt
#  define back_q_pg_sl_key_pure_type_fmt       back_q_pg_sl_key_type_fmt  /* sl_macro */
# endif
#endif

#ifndef back_q_pg_sl_key_purify
 /* key val -> key val */
 #define back_q_pg_sl_key_purify(k)            (k)  /* sl_macro */
#endif
#ifndef back_q_pg_sl_key_get_pure
 /* key component pointer -> key val pointer */
 #define back_q_pg_sl_key_get_pure(k)          (k)  /* sl_macro */
#endif
#ifndef back_q_pg_sl_key_set_pure
 /* key component pointer and key val */
 #define back_q_pg_sl_key_set_pure(k, p)       (*(k) = p)  /* sl_macro */
#endif


/* default pure key comparisons */
#ifndef back_q_pg_sl_key_pure_cmp_eq
 #define back_q_pg_sl_key_pure_cmp_eq(k0, k1)  ((k0) == (k1))  /* sl_macro */
#endif
#ifndef back_q_pg_sl_key_pure_cmp_ne
 #define back_q_pg_sl_key_pure_cmp_ne(k0, k1)  ((k0) != (k1))  /* sl_macro */
#endif
#ifndef back_q_pg_sl_key_pure_cmp_lt
 #define back_q_pg_sl_key_pure_cmp_lt(k0, k1)  ((k0) < (k1))  /* sl_macro */
#endif
#ifndef back_q_pg_sl_key_pure_cmp_le
 #define back_q_pg_sl_key_pure_cmp_le(k0, k1)  ((k0) <= (k1))  /* sl_macro */
#endif
#ifndef back_q_pg_sl_key_pure_cmp_gt
 #define back_q_pg_sl_key_pure_cmp_gt(k0, k1)  ((k0) > (k1))  /* sl_macro */
#endif
#ifndef back_q_pg_sl_key_pure_cmp_ge
 #define back_q_pg_sl_key_pure_cmp_ge(k0, k1)  ((k0) >= (k1))  /* sl_macro */
#endif


/* default key comparisons */
#ifndef back_q_pg_sl_key_cmp_eq
 #define back_q_pg_sl_key_cmp_eq(k0, k1)       (back_q_pg_sl_key_pure_cmp_eq(back_q_pg_sl_key_purify(k0), back_q_pg_sl_key_purify(k1)))  /* sl_macro */
#endif
#ifndef back_q_pg_sl_key_cmp_ne
 #define back_q_pg_sl_key_cmp_ne(k0, k1)       (back_q_pg_sl_key_pure_cmp_ne(back_q_pg_sl_key_purify(k0), back_q_pg_sl_key_purify(k1)))  /* sl_macro */
#endif
#ifndef back_q_pg_sl_key_cmp_lt
 #define back_q_pg_sl_key_cmp_lt(k0, k1)       (back_q_pg_sl_key_pure_cmp_lt(back_q_pg_sl_key_purify(k0), back_q_pg_sl_key_purify(k1)))  /* sl_macro */
#endif
#ifndef back_q_pg_sl_key_cmp_le
 #define back_q_pg_sl_key_cmp_le(k0, k1)       (back_q_pg_sl_key_pure_cmp_le(back_q_pg_sl_key_purify(k0), back_q_pg_sl_key_purify(k1)))  /* sl_macro */
#endif
#ifndef back_q_pg_sl_key_cmp_gt
 #define back_q_pg_sl_key_cmp_gt(k0, k1)       (back_q_pg_sl_key_pure_cmp_gt(back_q_pg_sl_key_purify(k0), back_q_pg_sl_key_purify(k1)))  /* sl_macro */
#endif
#ifndef back_q_pg_sl_key_cmp_ge
 #define back_q_pg_sl_key_cmp_ge(k0, k1)       (back_q_pg_sl_key_pure_cmp_ge(back_q_pg_sl_key_purify(k0), back_q_pg_sl_key_purify(k1)))  /* sl_macro */
#endif


/* default random key */
#ifdef back_q_pg_sl_key_integer
# if !defined(back_q_pg_sl_key_val_srand) || !defined(back_q_pg_sl_key_val_rand) || !defined(back_q_pg_sl_key_val_rand_minmax)
#  undef back_q_pg_sl_key_val_srand
#  undef back_q_pg_sl_key_val_rand
#  undef back_q_pg_sl_key_val_rand_minmax
#  define back_q_pg_sl_key_val_srand(_s_)                 sl_srand(_s_)                                        /* sl_macro */
#  define back_q_pg_sl_key_val_rand()                     ((back_q_pg_sl_key_pure_type_c) sl_rand())                     /* sl_macro */
#  define back_q_pg_sl_key_val_rand_minmax(_min_, _max_)  ((back_q_pg_sl_key_pure_type_c) sl_rand_minmax(_min_, _max_))  /* sl_macro */
# endif
#endif


/* disable data components on request */
/* DATAX_TEMPLATE_BEGIN */
#ifdef back_q_pg_SL_DATA0_IGNORE
# undef back_q_pg_SL_DATA0
#endif
#ifdef back_q_pg_SL_DATA1_IGNORE
# undef back_q_pg_SL_DATA1
#endif
#ifdef back_q_pg_SL_DATA2_IGNORE
# undef back_q_pg_SL_DATA2
#endif
#ifdef back_q_pg_SL_DATA3_IGNORE
# undef back_q_pg_SL_DATA3
#endif
#ifdef back_q_pg_SL_DATA4_IGNORE
# undef back_q_pg_SL_DATA4
#endif
#ifdef back_q_pg_SL_DATA5_IGNORE
# undef back_q_pg_SL_DATA5
#endif
#ifdef back_q_pg_SL_DATA6_IGNORE
# undef back_q_pg_SL_DATA6
#endif
#ifdef back_q_pg_SL_DATA7_IGNORE
# undef back_q_pg_SL_DATA7
#endif
#ifdef back_q_pg_SL_DATA8_IGNORE
# undef back_q_pg_SL_DATA8
#endif
#ifdef back_q_pg_SL_DATA9_IGNORE
# undef back_q_pg_SL_DATA9
#endif
#ifdef back_q_pg_SL_DATA10_IGNORE
# undef back_q_pg_SL_DATA10
#endif
#ifdef back_q_pg_SL_DATA11_IGNORE
# undef back_q_pg_SL_DATA11
#endif
#ifdef back_q_pg_SL_DATA12_IGNORE
# undef back_q_pg_SL_DATA12
#endif
#ifdef back_q_pg_SL_DATA13_IGNORE
# undef back_q_pg_SL_DATA13
#endif
#ifdef back_q_pg_SL_DATA14_IGNORE
# undef back_q_pg_SL_DATA14
#endif
#ifdef back_q_pg_SL_DATA15_IGNORE
# undef back_q_pg_SL_DATA15
#endif
#ifdef back_q_pg_SL_DATA16_IGNORE
# undef back_q_pg_SL_DATA16
#endif
#ifdef back_q_pg_SL_DATA17_IGNORE
# undef back_q_pg_SL_DATA17
#endif
#ifdef back_q_pg_SL_DATA18_IGNORE
# undef back_q_pg_SL_DATA18
#endif
#ifdef back_q_pg_SL_DATA19_IGNORE
# undef back_q_pg_SL_DATA19
#endif
/* DATAX_TEMPLATE_END */


/* sl_macro back_q_pg_sl_elem_weight */


/* disable sl_dataX_weight if there is not weight */
#ifndef back_q_pg_sl_elem_weight
/* DATAX_TEMPLATE_BEGIN */
# undef back_q_pg_sl_data0_weight
# undef back_q_pg_sl_data1_weight
# undef back_q_pg_sl_data2_weight
# undef back_q_pg_sl_data3_weight
# undef back_q_pg_sl_data4_weight
# undef back_q_pg_sl_data5_weight
# undef back_q_pg_sl_data6_weight
# undef back_q_pg_sl_data7_weight
# undef back_q_pg_sl_data8_weight
# undef back_q_pg_sl_data9_weight
# undef back_q_pg_sl_data10_weight
# undef back_q_pg_sl_data11_weight
# undef back_q_pg_sl_data12_weight
# undef back_q_pg_sl_data13_weight
# undef back_q_pg_sl_data14_weight
# undef back_q_pg_sl_data15_weight
# undef back_q_pg_sl_data16_weight
# undef back_q_pg_sl_data17_weight
# undef back_q_pg_sl_data18_weight
# undef back_q_pg_sl_data19_weight
/* DATAX_TEMPLATE_END */
#endif


/* disable back_q_pg_sl_elem_weight if the weight component is missing */
/* DATAX_TEMPLATE_BEGIN */
#if defined(back_q_pg_sl_data0_weight) && !defined(back_q_pg_SL_DATA0)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data1_weight) && !defined(back_q_pg_SL_DATA1)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data2_weight) && !defined(back_q_pg_SL_DATA2)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data3_weight) && !defined(back_q_pg_SL_DATA3)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data4_weight) && !defined(back_q_pg_SL_DATA4)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data5_weight) && !defined(back_q_pg_SL_DATA5)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data6_weight) && !defined(back_q_pg_SL_DATA6)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data7_weight) && !defined(back_q_pg_SL_DATA7)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data8_weight) && !defined(back_q_pg_SL_DATA8)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data9_weight) && !defined(back_q_pg_SL_DATA9)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data10_weight) && !defined(back_q_pg_SL_DATA10)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data11_weight) && !defined(back_q_pg_SL_DATA11)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data12_weight) && !defined(back_q_pg_SL_DATA12)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data13_weight) && !defined(back_q_pg_SL_DATA13)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data14_weight) && !defined(back_q_pg_SL_DATA14)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data15_weight) && !defined(back_q_pg_SL_DATA15)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data16_weight) && !defined(back_q_pg_SL_DATA16)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data17_weight) && !defined(back_q_pg_SL_DATA17)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data18_weight) && !defined(back_q_pg_SL_DATA18)
# undef back_q_pg_sl_elem_weight
#endif
#if defined(back_q_pg_sl_data19_weight) && !defined(back_q_pg_SL_DATA19)
# undef back_q_pg_sl_elem_weight
#endif
/* DATAX_TEMPLATE_END */


/* verify that the flex component is the last (FIXME: only if packed is on?) */
/* DATAX_TEMPLATE_BEGIN */
#ifdef back_q_pg_SL_DATA0
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data0_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA1
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data1_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA2
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data2_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA3
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data3_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA4
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data4_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA5
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data5_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA6
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data6_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA7
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data7_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA8
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data8_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA9
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data9_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA10
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data10_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA11
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data11_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA12
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data12_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA13
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data13_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA14
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data14_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA15
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data15_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA16
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data16_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA17
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data17_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA18
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data18_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef back_q_pg_SL_DATA19
# ifdef back_q_pg_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef back_q_pg_sl_data19_flex
#   define back_q_pg_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
/* DATAX_TEMPLATE_END */


/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_types.h
 *  timestamp: 2011-01-12 09:59:11 +0100
 *  
 */




/* sl_type back_q_pg_slint_t back_q_pg_slint */
typedef back_q_pg_sl_int_type_c back_q_pg_slint_t, back_q_pg_slint;  /* deprecated 'back_q_pg_slint' */

#define back_q_pg_slint_fmt   back_q_pg_sl_int_type_fmt    /* sl_macro */

/* sl_type back_q_pg_slindex_t */
typedef back_q_pg_sl_index_type_c back_q_pg_slindex_t;

#define back_q_pg_sindex_fmt  back_q_pg_sl_index_type_fmt  /* sl_macro */

/* sl_type back_q_pg_slkey_t */
typedef back_q_pg_sl_key_type_c back_q_pg_slkey_t;

/* sl_type back_q_pg_slkey_pure_t back_q_pg_slpkey_t */
typedef back_q_pg_sl_key_pure_type_c back_q_pg_slkey_pure_t, back_q_pg_slpkey_t;

/* DATAX_TEMPLATE_BEGIN */
/* sl_type back_q_pg_sldata0_t */
#ifdef back_q_pg_sl_data0_type_c
typedef back_q_pg_sl_data0_type_c back_q_pg_sldata0_t;
#endif
/* sl_type back_q_pg_sldata1_t */
#ifdef back_q_pg_sl_data1_type_c
typedef back_q_pg_sl_data1_type_c back_q_pg_sldata1_t;
#endif
/* sl_type back_q_pg_sldata2_t */
#ifdef back_q_pg_sl_data2_type_c
typedef back_q_pg_sl_data2_type_c back_q_pg_sldata2_t;
#endif
/* sl_type back_q_pg_sldata3_t */
#ifdef back_q_pg_sl_data3_type_c
typedef back_q_pg_sl_data3_type_c back_q_pg_sldata3_t;
#endif
/* sl_type back_q_pg_sldata4_t */
#ifdef back_q_pg_sl_data4_type_c
typedef back_q_pg_sl_data4_type_c back_q_pg_sldata4_t;
#endif
/* sl_type back_q_pg_sldata5_t */
#ifdef back_q_pg_sl_data5_type_c
typedef back_q_pg_sl_data5_type_c back_q_pg_sldata5_t;
#endif
/* sl_type back_q_pg_sldata6_t */
#ifdef back_q_pg_sl_data6_type_c
typedef back_q_pg_sl_data6_type_c back_q_pg_sldata6_t;
#endif
/* sl_type back_q_pg_sldata7_t */
#ifdef back_q_pg_sl_data7_type_c
typedef back_q_pg_sl_data7_type_c back_q_pg_sldata7_t;
#endif
/* sl_type back_q_pg_sldata8_t */
#ifdef back_q_pg_sl_data8_type_c
typedef back_q_pg_sl_data8_type_c back_q_pg_sldata8_t;
#endif
/* sl_type back_q_pg_sldata9_t */
#ifdef back_q_pg_sl_data9_type_c
typedef back_q_pg_sl_data9_type_c back_q_pg_sldata9_t;
#endif
/* sl_type back_q_pg_sldata10_t */
#ifdef back_q_pg_sl_data10_type_c
typedef back_q_pg_sl_data10_type_c back_q_pg_sldata10_t;
#endif
/* sl_type back_q_pg_sldata11_t */
#ifdef back_q_pg_sl_data11_type_c
typedef back_q_pg_sl_data11_type_c back_q_pg_sldata11_t;
#endif
/* sl_type back_q_pg_sldata12_t */
#ifdef back_q_pg_sl_data12_type_c
typedef back_q_pg_sl_data12_type_c back_q_pg_sldata12_t;
#endif
/* sl_type back_q_pg_sldata13_t */
#ifdef back_q_pg_sl_data13_type_c
typedef back_q_pg_sl_data13_type_c back_q_pg_sldata13_t;
#endif
/* sl_type back_q_pg_sldata14_t */
#ifdef back_q_pg_sl_data14_type_c
typedef back_q_pg_sl_data14_type_c back_q_pg_sldata14_t;
#endif
/* sl_type back_q_pg_sldata15_t */
#ifdef back_q_pg_sl_data15_type_c
typedef back_q_pg_sl_data15_type_c back_q_pg_sldata15_t;
#endif
/* sl_type back_q_pg_sldata16_t */
#ifdef back_q_pg_sl_data16_type_c
typedef back_q_pg_sl_data16_type_c back_q_pg_sldata16_t;
#endif
/* sl_type back_q_pg_sldata17_t */
#ifdef back_q_pg_sl_data17_type_c
typedef back_q_pg_sl_data17_type_c back_q_pg_sldata17_t;
#endif
/* sl_type back_q_pg_sldata18_t */
#ifdef back_q_pg_sl_data18_type_c
typedef back_q_pg_sl_data18_type_c back_q_pg_sldata18_t;
#endif
/* sl_type back_q_pg_sldata19_t */
#ifdef back_q_pg_sl_data19_type_c
typedef back_q_pg_sl_data19_type_c back_q_pg_sldata19_t;
#endif
/* DATAX_TEMPLATE_END */

/* sl_type back_q_pg_slweight_t */
typedef back_q_pg_sl_weight_type_c back_q_pg_slweight_t;

#define back_q_pg_slweight_fmt  back_q_pg_sl_weight_type_fmt  /* sl_macro */

/* sl_type back_q_pg__slpwkey_t back_q_pg_slpwkey_t */
typedef struct back_q_pg__slpwkey_t
{
  back_q_pg_slpkey_t pkey;
  back_q_pg_slweight_t weight;

} back_q_pg_slpwkey_t;


/* sl_type back_q_pg__elements_t back_q_pg_elements_t */
typedef struct back_q_pg__elements_t
{
  back_q_pg_slint_t size, max_size;
  back_q_pg_slkey_t *keys;

#ifdef back_q_pg_SL_INDEX
  back_q_pg_slindex_t *indices;
#endif

/* DATAX_TEMPLATE_BEGIN */
#ifdef back_q_pg_SL_DATA0
  back_q_pg_sldata0_t *data0;
#endif
#ifdef back_q_pg_SL_DATA1
  back_q_pg_sldata1_t *data1;
#endif
#ifdef back_q_pg_SL_DATA2
  back_q_pg_sldata2_t *data2;
#endif
#ifdef back_q_pg_SL_DATA3
  back_q_pg_sldata3_t *data3;
#endif
#ifdef back_q_pg_SL_DATA4
  back_q_pg_sldata4_t *data4;
#endif
#ifdef back_q_pg_SL_DATA5
  back_q_pg_sldata5_t *data5;
#endif
#ifdef back_q_pg_SL_DATA6
  back_q_pg_sldata6_t *data6;
#endif
#ifdef back_q_pg_SL_DATA7
  back_q_pg_sldata7_t *data7;
#endif
#ifdef back_q_pg_SL_DATA8
  back_q_pg_sldata8_t *data8;
#endif
#ifdef back_q_pg_SL_DATA9
  back_q_pg_sldata9_t *data9;
#endif
#ifdef back_q_pg_SL_DATA10
  back_q_pg_sldata10_t *data10;
#endif
#ifdef back_q_pg_SL_DATA11
  back_q_pg_sldata11_t *data11;
#endif
#ifdef back_q_pg_SL_DATA12
  back_q_pg_sldata12_t *data12;
#endif
#ifdef back_q_pg_SL_DATA13
  back_q_pg_sldata13_t *data13;
#endif
#ifdef back_q_pg_SL_DATA14
  back_q_pg_sldata14_t *data14;
#endif
#ifdef back_q_pg_SL_DATA15
  back_q_pg_sldata15_t *data15;
#endif
#ifdef back_q_pg_SL_DATA16
  back_q_pg_sldata16_t *data16;
#endif
#ifdef back_q_pg_SL_DATA17
  back_q_pg_sldata17_t *data17;
#endif
#ifdef back_q_pg_SL_DATA18
  back_q_pg_sldata18_t *data18;
#endif
#ifdef back_q_pg_SL_DATA19
  back_q_pg_sldata19_t *data19;
#endif
/* DATAX_TEMPLATE_END */

} back_q_pg_elements_t;


/* sl_type back_q_pg__packed_element_t back_q_pg_packed_element_t */
typedef struct back_q_pg__packed_element_t
{
  back_q_pg_slkey_t key;

#ifdef back_q_pg_SL_PACKED_INDEX
  back_q_pg_slindex_t index;
#endif

/* DATAX_TEMPLATE_BEGIN */
#ifdef back_q_pg_SL_DATA0
# ifdef back_q_pg_sl_data0_flex
  back_q_pg_sldata0_t data0[];
# else
  back_q_pg_sldata0_t data0[back_q_pg_sl_data0_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA1
# ifdef back_q_pg_sl_data1_flex
  back_q_pg_sldata1_t data1[];
# else
  back_q_pg_sldata1_t data1[back_q_pg_sl_data1_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA2
# ifdef back_q_pg_sl_data2_flex
  back_q_pg_sldata2_t data2[];
# else
  back_q_pg_sldata2_t data2[back_q_pg_sl_data2_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA3
# ifdef back_q_pg_sl_data3_flex
  back_q_pg_sldata3_t data3[];
# else
  back_q_pg_sldata3_t data3[back_q_pg_sl_data3_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA4
# ifdef back_q_pg_sl_data4_flex
  back_q_pg_sldata4_t data4[];
# else
  back_q_pg_sldata4_t data4[back_q_pg_sl_data4_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA5
# ifdef back_q_pg_sl_data5_flex
  back_q_pg_sldata5_t data5[];
# else
  back_q_pg_sldata5_t data5[back_q_pg_sl_data5_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA6
# ifdef back_q_pg_sl_data6_flex
  back_q_pg_sldata6_t data6[];
# else
  back_q_pg_sldata6_t data6[back_q_pg_sl_data6_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA7
# ifdef back_q_pg_sl_data7_flex
  back_q_pg_sldata7_t data7[];
# else
  back_q_pg_sldata7_t data7[back_q_pg_sl_data7_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA8
# ifdef back_q_pg_sl_data8_flex
  back_q_pg_sldata8_t data8[];
# else
  back_q_pg_sldata8_t data8[back_q_pg_sl_data8_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA9
# ifdef back_q_pg_sl_data9_flex
  back_q_pg_sldata9_t data9[];
# else
  back_q_pg_sldata9_t data9[back_q_pg_sl_data9_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA10
# ifdef back_q_pg_sl_data10_flex
  back_q_pg_sldata10_t data10[];
# else
  back_q_pg_sldata10_t data10[back_q_pg_sl_data10_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA11
# ifdef back_q_pg_sl_data11_flex
  back_q_pg_sldata11_t data11[];
# else
  back_q_pg_sldata11_t data11[back_q_pg_sl_data11_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA12
# ifdef back_q_pg_sl_data12_flex
  back_q_pg_sldata12_t data12[];
# else
  back_q_pg_sldata12_t data12[back_q_pg_sl_data12_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA13
# ifdef back_q_pg_sl_data13_flex
  back_q_pg_sldata13_t data13[];
# else
  back_q_pg_sldata13_t data13[back_q_pg_sl_data13_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA14
# ifdef back_q_pg_sl_data14_flex
  back_q_pg_sldata14_t data14[];
# else
  back_q_pg_sldata14_t data14[back_q_pg_sl_data14_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA15
# ifdef back_q_pg_sl_data15_flex
  back_q_pg_sldata15_t data15[];
# else
  back_q_pg_sldata15_t data15[back_q_pg_sl_data15_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA16
# ifdef back_q_pg_sl_data16_flex
  back_q_pg_sldata16_t data16[];
# else
  back_q_pg_sldata16_t data16[back_q_pg_sl_data16_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA17
# ifdef back_q_pg_sl_data17_flex
  back_q_pg_sldata17_t data17[];
# else
  back_q_pg_sldata17_t data17[back_q_pg_sl_data17_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA18
# ifdef back_q_pg_sl_data18_flex
  back_q_pg_sldata18_t data18[];
# else
  back_q_pg_sldata18_t data18[back_q_pg_sl_data18_size_c];
# endif
#endif
#ifdef back_q_pg_SL_DATA19
# ifdef back_q_pg_sl_data19_flex
  back_q_pg_sldata19_t data19[];
# else
  back_q_pg_sldata19_t data19[back_q_pg_sl_data19_size_c];
# endif
#endif
/* DATAX_TEMPLATE_END */

} back_q_pg_packed_element_t;


/* sl_type back_q_pg__packed_elements_t back_q_pg_packed_elements_t */
typedef struct back_q_pg__packed_elements_t
{
  back_q_pg_slint_t size, max_size;
  
  back_q_pg_packed_element_t *elements;
  
} back_q_pg_packed_elements_t;


#ifndef SLCINT_T
#define SLCINT_T
typedef long long int slcint_t;
#define slcint_fmt  "ll"
/*#define slcint_sfx  LL*/
#endif


#define SLCM_KEYS     (((slcint_t) 1) << 0)
#define SLCM_INDICES  (((slcint_t) 1) << 1)

/* DATAX_TEMPLATE_BEGIN */
#define SLCM_DATA0    (((slcint_t) 1) << (2+0))
#define SLCM_DATA1    (((slcint_t) 1) << (2+1))
#define SLCM_DATA2    (((slcint_t) 1) << (2+2))
#define SLCM_DATA3    (((slcint_t) 1) << (2+3))
#define SLCM_DATA4    (((slcint_t) 1) << (2+4))
#define SLCM_DATA5    (((slcint_t) 1) << (2+5))
#define SLCM_DATA6    (((slcint_t) 1) << (2+6))
#define SLCM_DATA7    (((slcint_t) 1) << (2+7))
#define SLCM_DATA8    (((slcint_t) 1) << (2+8))
#define SLCM_DATA9    (((slcint_t) 1) << (2+9))
#define SLCM_DATA10    (((slcint_t) 1) << (2+10))
#define SLCM_DATA11    (((slcint_t) 1) << (2+11))
#define SLCM_DATA12    (((slcint_t) 1) << (2+12))
#define SLCM_DATA13    (((slcint_t) 1) << (2+13))
#define SLCM_DATA14    (((slcint_t) 1) << (2+14))
#define SLCM_DATA15    (((slcint_t) 1) << (2+15))
#define SLCM_DATA16    (((slcint_t) 1) << (2+16))
#define SLCM_DATA17    (((slcint_t) 1) << (2+17))
#define SLCM_DATA18    (((slcint_t) 1) << (2+18))
#define SLCM_DATA19    (((slcint_t) 1) << (2+19))
/* DATAX_TEMPLATE_END */

#define SLCM_DATA     (((slcint_t) 0) \
  |SLCM_DATA0 \
  |SLCM_DATA1 \
  |SLCM_DATA2 \
  |SLCM_DATA3 \
  |SLCM_DATA4 \
  |SLCM_DATA5 \
  |SLCM_DATA6 \
  |SLCM_DATA7 \
  |SLCM_DATA8 \
  |SLCM_DATA9 \
  |SLCM_DATA10 \
  |SLCM_DATA11 \
  |SLCM_DATA12 \
  |SLCM_DATA13 \
  |SLCM_DATA14 \
  |SLCM_DATA15 \
  |SLCM_DATA16 \
  |SLCM_DATA17 \
  |SLCM_DATA18 \
  |SLCM_DATA19 \
  )

#define SLCM_WEIGHTS  (((slcint_t) 1) << (sizeof(slcint_t)*8-1))

#define SLCM_ALL      (~((slcint_t) 0))


/* sl_type back_q_pg__classification_info_t back_q_pg_classification_info_t back_q_pg_classification_info */
typedef struct back_q_pg__classification_info_t
{
  back_q_pg_slint_t nclasses;
  back_q_pg_slkey_pure_t *keys;
  back_q_pg_slint_t *counts;
  back_q_pg_slint_t *masks;

  /* */
  back_q_pg_slint_t *all_local_sizes;
  back_q_pg_slint_t *local_lt_eq_counts;
  back_q_pg_slint_t *all_local_lt_eq_counts;

} back_q_pg_classification_info_t, back_q_pg_classification_info;  /* deprecated 'back_q_pg_classification_info' */


/* key2class, sl_type back_q_pg_key2class_f */
typedef back_q_pg_slint_t (*back_q_pg_key2class_f)(back_q_pg_slkey_t *, back_q_pg_slint, void *);

/* pivot-element, sl_type back_q_pg_pivot_f */
typedef back_q_pg_slint_t (*back_q_pg_pivot_f)(back_q_pg_elements_t *);

/* sorting-network, sl_type back_q_pg_sortnet_f back_q_pg_sortnet_data_t */
typedef void *back_q_pg_sortnet_data_t;
typedef back_q_pg_slint_t (*back_q_pg_sortnet_f)(back_q_pg_slint_t size, back_q_pg_slint_t rank, back_q_pg_slint_t stage, back_q_pg_sortnet_data_t snd, back_q_pg_slint_t *up);

/* merge2, sl_type back_q_pg_merge2x_f back_q_pg_merge2X_f */
typedef back_q_pg_slint_t (*back_q_pg_merge2x_f)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);
typedef back_q_pg_slint_t (*back_q_pg_merge2X_f)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx, back_q_pg_elements_t *t);


/* deprecated, sl_type back_q_pg_k2c_func back_q_pg_pivot_func back_q_pg_sn_func back_q_pg_m2x_func back_q_pg_m2X_func */
typedef back_q_pg_key2class_f back_q_pg_k2c_func;
typedef back_q_pg_pivot_f back_q_pg_pivot_func;
typedef back_q_pg_sortnet_f back_q_pg_sn_func;
typedef back_q_pg_merge2x_f back_q_pg_m2x_func;
typedef back_q_pg_merge2X_f back_q_pg_m2X_func;


/* sl_type back_q_pg__mergek_t back_q_pg_mergek_t */
typedef struct back_q_pg__mergek_t
{
  back_q_pg_sortnet_f sn;
  back_q_pg_sortnet_data_t snd;

  back_q_pg_merge2x_f m2x;
  back_q_pg_elements_t *sx;

} back_q_pg_mergek_t;


/* sl_type back_q_pg_keys_init_type_t back_q_pg_keys_init_data_t */
typedef back_q_pg_slint_t back_q_pg_keys_init_type_t;
typedef void *back_q_pg_keys_init_data_t;

/* sl_type back_q_pg_key_set_data_t back_q_pg_key_set_f */
typedef void *back_q_pg_key_set_data_t;
typedef void (*back_q_pg_key_set_f)(back_q_pg_slkey_pure_t *k, back_q_pg_key_set_data_t d);


#undef SL_EKIT_SET
#define SL_EKIT_SET         1
#undef SL_EKIT_SET_FUNC
#define SL_EKIT_SET_FUNC    2
#undef SL_EKIT_RAND
#define SL_EKIT_RAND        3
#undef SL_EKIT_RAND_QUAD
#define SL_EKIT_RAND_QUAD   4
#undef SL_EKIT_RAND_AND
#define SL_EKIT_RAND_AND    5
#undef SL_EKIT_NRAND
#define SL_EKIT_NRAND       6


#ifndef SL_EIK_OFFSET
# define SL_EIK_OFFSET     65536LL
#endif

#ifndef SL_EIK_SET
# define SL_EIK_SET        SL_EIK_OFFSET*1
#endif

#ifndef SL_EIK_RAND
# define SL_EIK_RAND       SL_EIK_OFFSET*2
#endif

#ifndef SL_EIK_RAND_QUAD
# define SL_EIK_RAND_QUAD  SL_EIK_OFFSET*3
#endif

#ifndef SL_EIK_RAND_AND
# define SL_EIK_RAND_AND   SL_EIK_OFFSET*4
#endif

#ifndef SL_EIK_RAND_NORM
# define SL_EIK_RAND_NORM  SL_EIK_OFFSET*5
#endif


/* back_q_pg_elements_keys_stats */
#ifndef SL_EKS_MIN
# define SL_EKS_MIN   0
#endif

#ifndef SL_EKS_MAX
# define SL_EKS_MAX   1
#endif

#ifndef SL_EKS_SUM
# define SL_EKS_SUM   2
#endif

#ifndef SL_EKS_AVG
# define SL_EKS_AVG   3
#endif

#ifndef SL_EKS_STD
# define SL_EKS_STD   4
#endif

#ifndef SL_EKS_SIZE
# define SL_EKS_SIZE  5
#endif


#ifndef SL_SORTED_IN
# define SL_SORTED_IN   0x1LL
#endif

#ifndef SL_SORTED_OUT
# define SL_SORTED_OUT  0x2LL
#endif


/* partition conditions, sl_type back_q_pg__partcond2_t back_q_pg_partcond2_t */
typedef struct back_q_pg__partcond2_t
{
  int weighted;
  double min_count, max_count;
  double min_weight, max_weight;
  double min_cpart, max_cpart;
  double min_wpart, max_wpart;

} back_q_pg_partcond2_t;


#ifndef SLPC_COUNTS_MM
# define SLPC_COUNTS_MM   0x1
#endif
#ifndef SLPC_COUNTS_LH
# define SLPC_COUNTS_LH   0x2
#endif
#ifndef SLPC_WEIGHTS_MM
# define SLPC_WEIGHTS_MM  0x4
#endif
#ifndef SLPC_WEIGHTS_LH
# define SLPC_WEIGHTS_LH  0x8
#endif

/* partition conditions, sl_type back_q_pg__partcond_t back_q_pg_partcond_t back_q_pg_partcond_p */
typedef struct back_q_pg__partcond_t
{
  back_q_pg_slint_t pcm;
  double count_min, count_max;
  double count_low, count_high;
  double weight_min, weight_max;
  double weight_low, weight_high;

} back_q_pg_partcond_t, *back_q_pg_partcond_p;


/* internal partition conditions, sl_type back_q_pg__partcond_intern_t back_q_pg_partcond_intern_t back_q_pg_partcond_intern_p */
typedef struct back_q_pg__partcond_intern_t
{
  back_q_pg_slint_t pcm;
  back_q_pg_slint_t count_min, count_max;
  back_q_pg_slint_t count_low, count_high;
#ifdef elem_weight
  double weight_min, weight_max;
  double weight_low, weight_high;
#endif

} back_q_pg_partcond_intern_t, *back_q_pg_partcond_intern_p;


/* sl_type back_q_pg__parttype_t back_q_pg_parttype_t back_q_pg_parttype_p */
typedef struct back_q_pg__parttype_t
{

} back_q_pg_parttype_t, *back_q_pg_parttype_p;


/* generic binning method */

/* sl_type back_q_pg__bin_t back_q_pg_bin_t */
typedef struct back_q_pg__bin_t
{
  back_q_pg_elements_t s;

#ifdef elem_weight
  double weight;
#endif

} back_q_pg_bin_t;


/* sl_type back_q_pg__splitter_t back_q_pg_splitter_t */
typedef struct back_q_pg__splitter_t
{
  back_q_pg_slint_t n;

  int *displs;
  back_q_pg_slkey_pure_t *s;
  back_q_pg_slint_t *sn;

} back_q_pg_splitter_t;


struct back_q_pg__binning_t;

/* sl_type back_q_pg_binning_pre_f back_q_pg_binning_exec_f back_q_pg_binning_refine_f back_q_pg_binning_hit_f back_q_pg_binning_finalize_f back_q_pg_binning_post_f */
typedef back_q_pg_slint_t (*back_q_pg_binning_pre_f)(struct back_q_pg__binning_t *bm);
typedef back_q_pg_slint_t (*back_q_pg_binning_exec_f)(struct back_q_pg__binning_t *bm, back_q_pg_bin_t *bin, double *counts, double *weights);
typedef back_q_pg_slint_t (*back_q_pg_binning_refine_f)(struct back_q_pg__binning_t *bm, back_q_pg_bin_t *bin, back_q_pg_slint_t k, double *counts, double *weights, back_q_pg_splitter_t *sp, back_q_pg_slint_t s, back_q_pg_bin_t *new_bin);
typedef back_q_pg_slint_t (*back_q_pg_binning_hit_f)(struct back_q_pg__binning_t *bm, back_q_pg_bin_t *bin, back_q_pg_slint_t k, double *counts, back_q_pg_splitter_t *sp, back_q_pg_slint_t s);
typedef back_q_pg_slint_t (*back_q_pg_binning_finalize_f)(struct back_q_pg__binning_t *bm, back_q_pg_bin_t *bin, double dcw, back_q_pg_slint_t lc_min, back_q_pg_slint_t lc_max, double *lcw, back_q_pg_splitter_t *sp, back_q_pg_slint_t s);
typedef back_q_pg_slint_t (*back_q_pg_binning_post_f)(struct back_q_pg__binning_t *bm);

#ifdef SL_DEPRECATED
/* sl_type back_q_pg_binning_exec_pre_old_f back_q_pg_binning_exec_post_old_f back_q_pg_binning_refinable_old_f back_q_pg_binning_refine_old_f */
typedef back_q_pg_slint_t (*back_q_pg_binning_exec_pre_old_f)(struct back_q_pg__binning_t *bm);
typedef back_q_pg_slint_t (*back_q_pg_binning_exec_post_old_f)(struct back_q_pg__binning_t *bm);
typedef back_q_pg_slint_t (*back_q_pg_binning_refinable_old_f)(struct back_q_pg__binning_t *bm);
typedef back_q_pg_slint_t (*back_q_pg_binning_refine_old_f)(struct back_q_pg__binning_t *bm, back_q_pg_bin_t *bin, back_q_pg_slint_t k, double *counts, back_q_pg_bin_t *new_bin);
#endif


/* sl_type back_q_pg__binning_data_t back_q_pg_binning_data_t */
typedef union back_q_pg__binning_data_t
{
  struct
  {
    back_q_pg_slint_t rhigh, rlow, rwidth;
    back_q_pg_slint_t rcurrent;
    back_q_pg_slkey_pure_t bit_mask;

    back_q_pg_elements_t sx;

  } radix;

} back_q_pg_binning_data_t;


/* sl_type back_q_pg__binning_t back_q_pg_binning_t */
typedef struct back_q_pg__binning_t
{
  back_q_pg_slint_t nbins, max_nbins;
  
  back_q_pg_binning_pre_f pre;
  back_q_pg_binning_exec_f exec;
  back_q_pg_binning_refine_f refine;
  back_q_pg_binning_hit_f hit;
  back_q_pg_binning_finalize_f finalize;
  back_q_pg_binning_post_f post;

  back_q_pg_slint_t sorted;

#ifdef elem_weight
  back_q_pg_slint_t doweights;
#endif

#ifdef SL_DEPRECATED
  back_q_pg_binning_exec_pre_old_f exec_pre_old;
  back_q_pg_binning_exec_post_old_f exec_post_old;
  back_q_pg_binning_refinable_old_f refinable_old;
  back_q_pg_binning_refine_old_f refine_old;
#endif

  back_q_pg_binning_data_t bd;

} back_q_pg_binning_t;


/* sl_type back_q_pg__local_bins_t back_q_pg_local_bins_t */
typedef struct back_q_pg__local_bins_t
{
  back_q_pg_binning_t *bm;

  back_q_pg_slint_t nbins, max_nbins;
  back_q_pg_slint_t nelements;

#ifdef elem_weight
  back_q_pg_slint_t doweights, weight_factor;
#endif

  back_q_pg_slint_t nbinnings, max_nbinnings;

  back_q_pg_slint_t nbins_new, last_new_b, last_new_k;
  back_q_pg_bin_t *bins, *bins_new;
  back_q_pg_bin_t *bins0, *bins1;

  double *counts, *bin_counts;
#ifdef elem_weight
  double *weights, *bin_weights;
#endif

  back_q_pg_slint_t *bcws;
  double *cws, *bin_cws;

  back_q_pg_slint_t last_exec_b;

} back_q_pg_local_bins_t;


/* sl_type back_q_pg__global_bins_t back_q_pg_global_bins_t */
typedef struct back_q_pg__global_bins_t
{
  back_q_pg_binning_t *bm;
  
  back_q_pg_local_bins_t lb;
  
  double *counts;
#ifdef elem_weight
  double *weights;
#endif

  back_q_pg_slint_t *bcws;
  double *cws;

} back_q_pg_global_bins_t;


/* sl_macro back_q_pg_WEIGHT_FACTOR */
#ifdef elem_weight
# define back_q_pg_WEIGHT_FACTOR  2
#else
# define back_q_pg_WEIGHT_FACTOR  1
#endif


/* sl_macro back_q_pg_get1d back_q_pg_get2d back_q_pg_get3d back_q_pg_get4d */
#define back_q_pg_get1d(x0)                           (x0)
#define back_q_pg_get2d(x1, d0, x0)                  ((x0) + (d0) *  (x1))
#define back_q_pg_get3d(x2, d1, x1, d0, x0)          ((x0) + (d0) * ((x1) + (d1) *  (x2)))
#define back_q_pg_get4d(x3, d2, x2, d1, x1, d0, x0)  ((x0) + (d0) * ((x1) + (d1) * ((x2) + (d2) * (x3))))


/* sl_macro back_q_pg_lb_bin_count back_q_pg_lb_bin_weight */
#define back_q_pg_lb_bin_count(_lb_, _b_, _j_)    ((_lb_)->bins[(_b_) * (_lb_)->nelements + _j_].s.size)
#ifdef elem_weight
# define back_q_pg_lb_bin_weight(_lb_, _b_, _j_)  ((_lb_)->bins[(_b_) * (_lb_)->nelements + _j_].weight)
#else
# define back_q_pg_lb_bin_weight(_lb_, _b_, _j_)  0
#endif

/* sl_macro back_q_pg_lb_bin_counts back_q_pg_lb_bin_weights */
#ifdef elem_weight
# define back_q_pg_lb_bin_counts(_lb_, _b_, _j_, _k_)   ((_lb_)->bin_cws + back_q_pg_get4d((_lb_)->bcws[_b_], (_lb_)->weight_factor, 0, (_lb_)->nelements, _j_, (_lb_)->bm->nbins, _k_))
# define back_q_pg_lb_bin_weights(_lb_, _b_, _j_, _k_)  ((_lb_)->bin_cws + back_q_pg_get4d((_lb_)->bcws[_b_], (_lb_)->weight_factor, 1, (_lb_)->nelements, _j_, (_lb_)->bm->nbins, _k_))
#else
# define back_q_pg_lb_bin_counts(_lb_, _b_, _j_, _k_)   ((_lb_)->bin_cws + back_q_pg_get4d((_lb_)->bcws[_b_], 1, 0, (_lb_)->nelements, _j_, (_lb_)->bm->nbins, _k_))
# define back_q_pg_lb_bin_weights(_lb_, _b_, _j_, _k_)  NULL
#endif

/* sl_macro back_q_pg_lb_counts back_q_pg_lb_weights */
#ifdef elem_weight
# define back_q_pg_lb_counts(_lb_, _b_, _k_)   ((_lb_)->cws + back_q_pg_get3d((_lb_)->bcws[_b_], (_lb_)->weight_factor, 0, (_lb_)->bm->nbins, (_k_)))
# define back_q_pg_lb_weights(_lb_, _b_, _k_)  ((_lb_)->cws + back_q_pg_get3d((_lb_)->bcws[_b_], (_lb_)->weight_factor, 1, (_lb_)->bm->nbins, (_k_)))
#else
# define back_q_pg_lb_counts(_lb_, _b_, _k_)   ((_lb_)->cws + back_q_pg_get3d((_lb_)->bcws[_b_], 1, 0, (_lb_)->bm->nbins, (_k_)))
# define back_q_pg_lb_weights(_lb_, _b_, _k_)  NULL
#endif

/* sl_macro back_q_pg_gb_counts back_q_pg_gb_weights */
#ifdef elem_weight
# define back_q_pg_gb_counts(_gb_, _b_, _k_)   ((_gb_)->cws + back_q_pg_get3d((_gb_)->bcws[_b_], (_gb_)->lb.weight_factor, 0, (_gb_)->bm->nbins, (_k_)))
# define back_q_pg_gb_weights(_gb_, _b_, _k_)  ((_gb_)->cws + back_q_pg_get3d((_gb_)->bcws[_b_], (_gb_)->lb.weight_factor, 1, (_gb_)->bm->nbins, (_k_)))
#else
# define back_q_pg_gb_counts(_gb_, _b_, _k_)   ((_gb_)->cws + back_q_pg_get3d((_gb_)->bcws[_b_], 1, 0, (_gb_)->bm->nbins, (_k_)))
# define back_q_pg_gb_weights(_gb_, _b_, _k_)  NULL
#endif


/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_adds.h
 *  timestamp: 2010-08-12 10:08:32 +0200
 *  
 */




/* sl_macro back_q_pg_elem_set_size back_q_pg_elem_set_max_size back_q_pg_elem_set_keys back_q_pg_elem_set_indices */
#define back_q_pg_elem_set_size(_e_, _s_)      ((_e_)->size = (_s_))
#define back_q_pg_elem_set_max_size(_e_, _s_)  ((_e_)->max_size = (_s_))
#define back_q_pg_elem_set_keys(_e_, _k_)      ((_e_)->keys = (_k_))
#define back_q_pg_elem_set_indices(_e_, _i_)   ((_e_)->indices = (_i_))

/* sl_macro back_q_pg_pelem_set_size back_q_pg_pelem_set_max_size back_q_pg_pelem_set_elements */
#define back_q_pg_pelem_set_size(_e_, _s_)      ((_e_)->size = (_s_))
#define back_q_pg_pelem_set_max_size(_e_, _s_)  ((_e_)->max_size = (_s_))
#define back_q_pg_pelem_set_elements(_e_, _l_)  ((_e_)->elements = (_l_))


#ifndef SL_FRACRES
# define SL_FRACRES       1000000.0
# define SL_FRAC2INT(p)   ((back_q_pg_slint_t) ((p) * -SL_FRACRES))
# define SL_INT2FRAC(i)   (((double) (i)) / -SL_FRACRES)
# define SL_ISFRAC(i)     ((i) < 0)
#endif


/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_globals.h
 *  timestamp: 2011-01-13 10:37:24 +0100
 *  
 */




/* src/include/sl_debug.h */
extern FILE *back_q_pg_sl_notice_fstream;
extern FILE *back_q_pg_sl_error_fstream;
extern FILE *back_q_pg_sl_debug_fstream;

/* src/include/sl_rti_intern.h */
extern rti back_q_pg_rti_env;

/* src/core/back_q_pg_sort_radix_ip.c */
extern back_q_pg_slint_t back_q_pg_sa_ip_threshold;

/* src/core_mpi/mpi_common.c */
#ifdef SL_USE_MPI
extern MPI_Datatype back_q_pg_int_mpi_datatype;
extern MPI_Datatype back_q_pg_key_mpi_datatype;
extern MPI_Datatype back_q_pg_pkey_mpi_datatype;
extern MPI_Datatype back_q_pg_pwkey_mpi_datatype;
extern MPI_Datatype back_q_pg_index_mpi_datatype;
extern MPI_Datatype back_q_pg_weight_mpi_datatype;
extern MPI_Datatype back_q_pg_data_mpi_datatype[];
#endif
#ifdef SL_USE_MPI
extern int back_q_pg_sl_mpi_rank;
#endif

/* src/core_mpi/mpi_elements.c */
extern void *back_q_pg_me_sendrecv_replace_mem;
extern back_q_pg_slint_t back_q_pg_me_sendrecv_replace_memsize;
extern back_q_pg_slint_t back_q_pg_me_sendrecv_replace_mpi_maxsize;

/* src/core_mpi/back_q_pg_mpi_select_exact_generic.c */
extern back_q_pg_slint_t back_q_pg_mseg_rounds;
extern int back_q_pg_mseg_root;
extern double back_q_pg_mseg_border_update_propagation;
extern back_q_pg_slint_t back_q_pg_mseg_border_update_full;
extern back_q_pg_slint_t back_q_pg_mseg_binnings;

/* src/core_mpi/back_q_pg_mpi_sort_partition.c */
extern double back_q_pg_msp_t[];
extern back_q_pg_slint_t back_q_pg_msp_sync;


/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_protos.h
 *  timestamp: 2011-01-13 10:37:23 +0100
 *  
 */




/* src/core/binning.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_create)(back_q_pg_local_bins_t *lb, back_q_pg_slint_t max_nbins, back_q_pg_slint_t max_nbinnings, back_q_pg_elements_t *s, back_q_pg_slint_t nelements, back_q_pg_slint_t doweights, back_q_pg_binning_t *bm);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_destroy)(back_q_pg_local_bins_t *lb);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_pre)(back_q_pg_local_bins_t *lb);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_exec_reset)(back_q_pg_local_bins_t *lb);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_exec)(back_q_pg_local_bins_t *lb, back_q_pg_slint_t b);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_refine)(back_q_pg_local_bins_t *lb, back_q_pg_slint_t b, back_q_pg_slint_t k, back_q_pg_splitter_t *sp, back_q_pg_slint_t s);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_hit)(back_q_pg_local_bins_t *lb, back_q_pg_slint_t b, back_q_pg_slint_t k, back_q_pg_splitter_t *sp, back_q_pg_slint_t s);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_finalize)(back_q_pg_local_bins_t *lb, back_q_pg_slint_t b, back_q_pg_slweight_t dcw, back_q_pg_slint_t lc_min, back_q_pg_slint_t lc_max, back_q_pg_slweight_t *lcw, back_q_pg_splitter_t *sp, back_q_pg_slint_t s);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_post)(back_q_pg_local_bins_t *lb);

/* src/core/binning_radix.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_radix_create)(back_q_pg_binning_t *bm, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth, back_q_pg_slint_t sorted);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_radix_destroy)(back_q_pg_binning_t *bm);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_radix_pre)(back_q_pg_binning_t *bm);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_radix_exec)(back_q_pg_binning_t *bm, back_q_pg_bin_t *bin, back_q_pg_slweight_t *counts, back_q_pg_slweight_t *weights);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_radix_refine)(back_q_pg_binning_t *bm, back_q_pg_bin_t *bin, back_q_pg_slint_t k, back_q_pg_slweight_t *counts, back_q_pg_slweight_t *weights, back_q_pg_splitter_t *sp, back_q_pg_slint_t s, back_q_pg_bin_t *new_bin);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_radix_hit)(back_q_pg_binning_t *bm, back_q_pg_bin_t *bin, back_q_pg_slint_t k, back_q_pg_slweight_t *counts, back_q_pg_splitter_t *sp, back_q_pg_slint_t s);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_radix_finalize)(back_q_pg_binning_t *bm, back_q_pg_bin_t *bin, back_q_pg_slweight_t dcw, back_q_pg_slint_t lc_min, back_q_pg_slint_t lc_max, back_q_pg_slweight_t *lcw, back_q_pg_splitter_t *sp, back_q_pg_slint_t s);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_radix_post)(back_q_pg_binning_t *bm);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_radix_exec_pre_old)(back_q_pg_binning_t *bm);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_radix_exec_post_old)(back_q_pg_binning_t *bm);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_radix_refinable_old)(back_q_pg_binning_t *bm);
back_q_pg_slint_t SL_PROTO(back_q_pg_binning_radix_refine_old)(back_q_pg_binning_t *bm, back_q_pg_bin_t *bin, back_q_pg_slint_t k, back_q_pg_slweight_t *counts, back_q_pg_bin_t *new_bin);

/* src/core/checksum_crc.c */
unsigned short SL_PROTO(back_q_pg_elements_crc16)(back_q_pg_elements_t *s, back_q_pg_slint n, back_q_pg_slint keys, back_q_pg_slint data);
unsigned int SL_PROTO(back_q_pg_elements_crc32)(back_q_pg_elements_t *s, back_q_pg_slint n, back_q_pg_slint keys, back_q_pg_slint data);

/* src/core/elements.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_alloc)(back_q_pg_elements_t *s, back_q_pg_slint_t nelements, slcint_t components);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_alloc2)(back_q_pg_elements_t *s, back_q_pg_slint_t nelements, back_q_pg_slint_t keys, back_q_pg_slint_t indices, back_q_pg_slint_t data, back_q_pg_slint_t weights);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_alloc_old)(back_q_pg_elements_t *s, back_q_pg_slint_t nelements, back_q_pg_slint_t keys, back_q_pg_slint_t data);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_free)(back_q_pg_elements_t *s);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_alloc_from_blocks)(back_q_pg_elements_t *s, back_q_pg_slint_t nblocks, void **blocks, back_q_pg_slint_t *blocksizes, back_q_pg_slint_t alignment, back_q_pg_slint_t nmax, slcint_t components);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_alloc_from_block2)(back_q_pg_elements_t *s, void *block, back_q_pg_slint_t blocksize, back_q_pg_slint_t alignment, back_q_pg_slint_t nmax, back_q_pg_slint_t keys, back_q_pg_slint_t indices, back_q_pg_slint_t data, back_q_pg_slint_t weights);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_alloc_from_block)(back_q_pg_elements_t *s, void *block, back_q_pg_slint_t blocksize, back_q_pg_slint_t alignment, back_q_pg_slint_t nmax);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_copy)(back_q_pg_elements_t *s, back_q_pg_elements_t *d);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_copy_at)(back_q_pg_elements_t *s, back_q_pg_slint_t sat, back_q_pg_elements_t *d, back_q_pg_slint_t dat);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_ncopy)(back_q_pg_elements_t *s, back_q_pg_elements_t *d, back_q_pg_slint_t n);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_nmove)(back_q_pg_elements_t *s, back_q_pg_elements_t *d, back_q_pg_slint_t n);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_printf)(back_q_pg_elements_t *s, const char *prefix);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_extract)(back_q_pg_elements_t *src, back_q_pg_slint_t nelements, back_q_pg_elements_t *dst0, back_q_pg_elements_t *dst1);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_touch)(back_q_pg_elements_t *s);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_random_exchange)(back_q_pg_elements_t *s, back_q_pg_slint_t rounds, back_q_pg_elements_t *xs);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_init_keys)(back_q_pg_elements_t *s, back_q_pg_slint_t dtype, back_q_pg_slint_t _min, back_q_pg_slint_t _max);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_init_keys2)(back_q_pg_elements_t *s, back_q_pg_slint_t dtype, back_q_pg_slkey_pure_t key_min, back_q_pg_slkey_pure_t key_max);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_keys_init)(back_q_pg_elements_t *s, back_q_pg_keys_init_type_t t, back_q_pg_keys_init_data_t d);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_init_keys_from_file)(back_q_pg_elements_t *s, back_q_pg_slint_t data, char *filename, back_q_pg_slint_t from, back_q_pg_slint_t to, back_q_pg_slint_t const_bytes_per_line);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_save_keys_to_file)(back_q_pg_elements_t *s, char *filename);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_validate_order)(back_q_pg_elements_t *s, back_q_pg_slint_t n);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_validate_order_bmask)(back_q_pg_elements_t *s, back_q_pg_slint_t n, back_q_pg_slkey_pure_t bmask);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_validate_order_weight)(back_q_pg_elements_t *s, back_q_pg_slint_t n, back_q_pg_slkey_pure_t weight);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_keys_stats)(back_q_pg_elements_t *s, back_q_pg_slkey_pure_t *stats);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_print_keys)(back_q_pg_elements_t *s);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_print_all)(back_q_pg_elements_t *s);
back_q_pg_slweight_t SL_PROTO(back_q_pg_elements_get_weight)(back_q_pg_elements_t *s);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_get_minmax_keys)(back_q_pg_elements_t *s, back_q_pg_slint_t nelements, back_q_pg_slkey_pure_t *minmaxkeys);

/* src/core/elements_packed.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_alloc_packed)(back_q_pg_packed_elements_t *s, back_q_pg_slint_t nelements);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_free_packed)(back_q_pg_packed_elements_t *s);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_pack_indexed)(back_q_pg_elements_t *s, back_q_pg_packed_elements_t *d, back_q_pg_slindex_t *rindx, back_q_pg_slindex_t *windx);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_pack)(back_q_pg_elements_t *s, back_q_pg_packed_elements_t *d);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_unpack_indexed)(back_q_pg_packed_elements_t *s, back_q_pg_elements_t *d, back_q_pg_slindex_t *rindx, back_q_pg_slindex_t *windx);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_unpack)(back_q_pg_packed_elements_t *s, back_q_pg_elements_t *d);
back_q_pg_slint_t SL_PROTO(back_q_pg_elements_unpack_keys)(back_q_pg_packed_elements_t *s, back_q_pg_slkey_t *k);

/* src/core/key2class.c */
back_q_pg_slint SL_PROTO(back_q_pg_key2class_equal)(back_q_pg_slkey_t *k, back_q_pg_slint i, void *ci);
back_q_pg_slint SL_PROTO(back_q_pg_key2class_split)(back_q_pg_slkey_t *k, back_q_pg_slint i, void *ci);
back_q_pg_slint SL_PROTO(back_q_pg_key2class_split_keys)(back_q_pg_slkey_t *k, back_q_pg_slint i, void *ci);
back_q_pg_slint SL_PROTO(back_q_pg_key2class_random)(back_q_pg_slkey_t *k, back_q_pg_slint i, void *ci);
back_q_pg_slint SL_PROTO(back_q_pg_key2class_ci_nocounts)(back_q_pg_slkey_t *k, back_q_pg_slint i, void *ci);
back_q_pg_slint SL_PROTO(back_q_pg_key2class_ci)(back_q_pg_slkey_t *k, back_q_pg_slint i, void *ci);

/* src/core/merge2_basic.c */
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_01_x)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx, back_q_pg_m2x_func _x0_1, back_q_pg_m2x_func _0x_1);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_01_X)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx, back_q_pg_elements_t *t, back_q_pg_m2X_func _X0_1, back_q_pg_m2X_func _0X_1);

/* src/core/merge2_basic_auto.c */
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_auto_01_x)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);

/* src/core/merge2_basic_sbin.c */
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_sbin_x0_1)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_sbin_0x_1)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_sbin_01_x)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_sbin_01)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *t);

/* src/core/merge2_basic_shyb.c */
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_shyb_x0_1)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_shyb_0x_1)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_shyb_01_x)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_shyb_01)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *t);

/* src/core/merge2_basic_sseq.c */
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_sseq_x0_1)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_sseq_0x_1)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_sseq_01_x)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_sseq_01)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *t);

/* src/core/merge2_basic_straight.c */
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_straight_x0_1)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_straight_0x_1)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_straight_01_x)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_straight_x_0_1)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_straight_X0_1)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx, back_q_pg_elements_t *t);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_straight_0X_1)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx, back_q_pg_elements_t *t);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_straight_01_X)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx, back_q_pg_elements_t *t);
back_q_pg_slint SL_PROTO(back_q_pg_merge2_basic_straight_X0_1u)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx, back_q_pg_elements_t *t);

/* src/core/back_q_pg_merge2_compo_hula.c */
back_q_pg_slint SL_PROTO(back_q_pg_merge2_compo_hula)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *xs);

/* src/core/back_q_pg_merge2_compo_tridgell.c */
back_q_pg_slint SL_PROTO(back_q_pg_merge2_compo_tridgell)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);

/* src/core/back_q_pg_merge2_memory_adaptive.c */
back_q_pg_slint SL_PROTO(back_q_pg_merge2_memory_adaptive)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx);

/* src/core/merge2_simplify.c */
back_q_pg_slint SL_PROTO(back_q_pg_merge2_simplify_s1)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *sx, back_q_pg_slint s1elements);

/* src/core/mergep_2way.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mergep_2way_ip_int)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slint_t p, int *displs, back_q_pg_merge2x_f m2x);

/* src/core/mergep_heap.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mergep_heap_int)(back_q_pg_elements_t *s, back_q_pg_elements_t *d, back_q_pg_slint_t p, int *displs, int *counts);
back_q_pg_slint_t SL_PROTO(back_q_pg_mergep_heap_int_idx)(back_q_pg_elements_t *s, back_q_pg_elements_t *d, back_q_pg_slint_t p, int *displs, int *counts);
back_q_pg_slint_t SL_PROTO(back_q_pg_mergep_heap_idx)(back_q_pg_elements_t *s, back_q_pg_elements_t *d, back_q_pg_slint_t p, back_q_pg_slindex_t *displs, back_q_pg_slindex_t *counts);
back_q_pg_slint_t SL_PROTO(back_q_pg_mergep_heap_unpack_idx)(back_q_pg_packed_elements_t *s, back_q_pg_elements_t *d, back_q_pg_slint_t p, back_q_pg_slindex_t *displs, back_q_pg_slindex_t *counts);
back_q_pg_slint_t SL_PROTO(back_q_pg_mergep_heap_unpack_idxonly)(back_q_pg_packed_elements_t *s, back_q_pg_elements_t *d, back_q_pg_slint_t p, back_q_pg_slindex_t *displs, back_q_pg_slindex_t *counts);

/* src/core/search.c */
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_sequential_lt)(back_q_pg_elements_t *s, back_q_pg_slkey_t *k);
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_sequential_le)(back_q_pg_elements_t *s, back_q_pg_slkey_t *k);
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_sequential_gt)(back_q_pg_elements_t *s, back_q_pg_slkey_t *k);
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_sequential_ge)(back_q_pg_elements_t *s, back_q_pg_slkey_t *k);
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_binary_lt)(back_q_pg_elements_t *s, back_q_pg_slkey_t *k);
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_binary_le)(back_q_pg_elements_t *s, back_q_pg_slkey_t *k);
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_binary_gt)(back_q_pg_elements_t *s, back_q_pg_slkey_t *k);
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_binary_ge)(back_q_pg_elements_t *s, back_q_pg_slkey_t *k);
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_binary_lt2)(back_q_pg_elements_t *s, back_q_pg_slkey_pure_t k);
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_binary_le2)(back_q_pg_elements_t *s, back_q_pg_slkey_pure_t k);
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_binary_gt2)(back_q_pg_elements_t *s, back_q_pg_slkey_pure_t k);
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_binary_ge2)(back_q_pg_elements_t *s, back_q_pg_slkey_pure_t k);
back_q_pg_slint_t SL_PROTO(back_q_pg_sl_search_binary_lt_bmask)(back_q_pg_elements_t *s, back_q_pg_slkey_pure_t k, back_q_pg_slkey_pure_t bmask);
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_hybrid_lt)(back_q_pg_elements_t *s, back_q_pg_slkey_t *k, back_q_pg_slint t);
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_hybrid_le)(back_q_pg_elements_t *s, back_q_pg_slkey_t *k, back_q_pg_slint t);
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_hybrid_gt)(back_q_pg_elements_t *s, back_q_pg_slkey_t *k, back_q_pg_slint t);
back_q_pg_slint SL_PROTO(back_q_pg_sl_search_hybrid_ge)(back_q_pg_elements_t *s, back_q_pg_slkey_t *k, back_q_pg_slint t);

/* src/core/sl_common.c */
back_q_pg_slint SL_PROTO(back_q_pg_ilog2c)(back_q_pg_slint x);
back_q_pg_slint SL_PROTO(back_q_pg_ilog2f)(back_q_pg_slint x);
long long SL_PROTO(back_q_pg_sl_random64)();
double SL_PROTO(back_q_pg_sl_nrandom)();
double SL_PROTO(back_q_pg_sl_urandom)();
back_q_pg_slint SL_PROTO(back_q_pg_print_bits)(back_q_pg_slint v);
back_q_pg_slint SL_PROTO(back_q_pg_pivot_random)(back_q_pg_elements_t *s);
back_q_pg_slint_t SL_PROTO(back_q_pg_counts2displs)(back_q_pg_slint_t n, int *counts, int *displs);
back_q_pg_slint_t SL_PROTO(back_q_pg_displs2counts)(back_q_pg_slint_t n, int *displs, int *counts, back_q_pg_slint_t total_counts);

/* src/core/sl_elem.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_elem_set_data)(back_q_pg_elements_t *e, ...);
back_q_pg_slint_t SL_PROTO(back_q_pg_elem_reverse)(back_q_pg_elements_t *e, back_q_pg_elements_t *t);
back_q_pg_slint_t SL_PROTO(back_q_pg_elem_nxchange_at)(back_q_pg_elements_t *e0, back_q_pg_slint_t at0, back_q_pg_elements_t *e1, back_q_pg_slint_t at1, back_q_pg_slint_t n, back_q_pg_elements_t *t);
back_q_pg_slint_t SL_PROTO(back_q_pg_elem_nxchange)(back_q_pg_elements_t *e0, back_q_pg_elements_t *e1, back_q_pg_slint_t n, back_q_pg_elements_t *t);
back_q_pg_slint_t SL_PROTO(back_q_pg_elem_nxchange_ro0)(back_q_pg_elements_t *e0, back_q_pg_elements_t *e1, back_q_pg_slint_t n, back_q_pg_elements_t *t);
back_q_pg_slint_t SL_PROTO(back_q_pg_elem_rotate)(back_q_pg_elements_t *e, back_q_pg_slint_t m, back_q_pg_slint_t n, back_q_pg_elements_t *t);
back_q_pg_slint_t SL_PROTO(back_q_pg_elem_rotate_ro0)(back_q_pg_elements_t *e, back_q_pg_slint_t m, back_q_pg_slint_t n, back_q_pg_elements_t *t);
back_q_pg_slint_t SL_PROTO(back_q_pg_elem_rotate_ro1)(back_q_pg_elements_t *e, back_q_pg_slint_t m, back_q_pg_slint_t n, back_q_pg_elements_t *t);

/* src/core/back_q_pg_sort_counting.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_sort_counting_use_displs)(back_q_pg_elements_t *s, back_q_pg_elements_t *d, back_q_pg_slint_t ndispls, back_q_pg_slint_t *displs);
back_q_pg_slint_t SL_PROTO(back_q_pg_sort_counting_use_counts)(back_q_pg_elements_t *s, back_q_pg_elements_t *d, back_q_pg_slint_t ncounts, back_q_pg_slint_t *counts);
back_q_pg_slint_t SL_PROTO(back_q_pg_sort_counting_get_counts)(back_q_pg_elements_t *s, back_q_pg_elements_t *d, back_q_pg_slint_t ncounts, back_q_pg_slint_t *counts);
back_q_pg_slint_t SL_PROTO(back_q_pg_sort_counting)(back_q_pg_elements_t *s, back_q_pg_elements_t *d, back_q_pg_slint_t ncounts);

/* src/core/back_q_pg_sort_heap.c */
back_q_pg_slint SL_PROTO(back_q_pg_sort_heap)(back_q_pg_elements_t *s, back_q_pg_elements_t *xs);

/* src/core/back_q_pg_sort_insert.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_sort_insert_bmask_kernel)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slkey_pure_t bmask);
back_q_pg_slint_t SL_PROTO(back_q_pg_sort_insert)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx);

/* src/core/sort_permute.c */
back_q_pg_slint SL_PROTO(back_q_pg_sort_permute_forward)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slint *perm, back_q_pg_slint offset, back_q_pg_slint mask_bit);
back_q_pg_slint SL_PROTO(back_q_pg_sort_permute_backward)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slint *perm, back_q_pg_slint offset, back_q_pg_slint mask_bit);

/* src/core/back_q_pg_sort_quick.c */
back_q_pg_slint SL_PROTO(back_q_pg_sort_quick)(back_q_pg_elements_t *s, back_q_pg_elements_t *xs);

/* src/core/back_q_pg_sort_radix.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_sort_radix)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth);

/* src/core/back_q_pg_sort_radix_1bit.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_sort_radix_1bit_kernel)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow);
back_q_pg_slint SL_PROTO(back_q_pg_sort_radix_1bit)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow);

/* src/core/back_q_pg_sort_radix_af.c */
back_q_pg_slint SL_PROTO(back_q_pg_sort_radix_af)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slint rhigh, back_q_pg_slint rlow, back_q_pg_slint rwidth);

/* src/core/back_q_pg_sort_radix_db.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_sort_radix_db)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth);

/* src/core/back_q_pg_sort_radix_ip.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_sort_radix_ip)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth);

/* src/core/back_q_pg_sort_radix_iter.c */
back_q_pg_slint SL_PROTO(back_q_pg_sort_radix_iter)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slint presorted, back_q_pg_slint rhigh, back_q_pg_slint rlow, back_q_pg_slint rwidth);

/* src/core/back_q_pg_sort_radix_ma.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_sort_radix_ma)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth);

/* src/core/sortnet.c */
back_q_pg_slint SL_PROTO(back_q_pg_sn_hypercube_lh)(back_q_pg_slint size, back_q_pg_slint rank, back_q_pg_slint stage, void *snp, back_q_pg_slint *up);
back_q_pg_slint SL_PROTO(back_q_pg_sn_hypercube_hl)(back_q_pg_slint size, back_q_pg_slint rank, back_q_pg_slint stage, void *snp, back_q_pg_slint *up);
back_q_pg_slint SL_PROTO(back_q_pg_sn_odd_even_trans)(back_q_pg_slint size, back_q_pg_slint rank, back_q_pg_slint stage, void *snp, back_q_pg_slint *up);
back_q_pg_slint SL_PROTO(back_q_pg_sn_odd)(back_q_pg_slint size, back_q_pg_slint rank, back_q_pg_slint stage, void *snp, back_q_pg_slint *up);
back_q_pg_slint SL_PROTO(back_q_pg_sn_even)(back_q_pg_slint size, back_q_pg_slint rank, back_q_pg_slint stage, void *snp, back_q_pg_slint *up);
back_q_pg_slint SL_PROTO(back_q_pg_sn_batcher)(back_q_pg_slint size, back_q_pg_slint rank, back_q_pg_slint stage, void *snp, back_q_pg_slint *up);
back_q_pg_slint SL_PROTO(back_q_pg_sn_bitonic)(back_q_pg_slint size, back_q_pg_slint rank, back_q_pg_slint stage, void *snp, back_q_pg_slint *up);
back_q_pg_slint SL_PROTO(back_q_pg_sn_connected)(back_q_pg_slint size, back_q_pg_slint rank, back_q_pg_slint stage, void *snp, back_q_pg_slint *up);

/* src/core/splitter.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_splitter_reset)(back_q_pg_splitter_t *sp);

/* src/core/splitx.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_splitx_radix)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slint_t nclasses, back_q_pg_slint_t shl, back_q_pg_slint_t *counts);
back_q_pg_slint SL_PROTO(back_q_pg_split2_lt_ge)(back_q_pg_elements_t *s, back_q_pg_slkey_pure_t *k, back_q_pg_elements_t *t);
back_q_pg_slint SL_PROTO(back_q_pg_split2_le_gt)(back_q_pg_elements_t *s, back_q_pg_slkey_pure_t *k, back_q_pg_elements_t *t);
back_q_pg_slint SL_PROTO(back_q_pg_split3_lt_eq_gt)(back_q_pg_elements_t *s, back_q_pg_slkey_pure_t *k, back_q_pg_elements_t *t, back_q_pg_slint *nlt, back_q_pg_slint *nle);
back_q_pg_slint SL_PROTO(back_q_pg_split3_lt_eq_gt_old)(back_q_pg_elements_t *s, back_q_pg_slkey_pure_t *k, back_q_pg_elements_t *t, back_q_pg_slint *nlt, back_q_pg_slint *nle);
back_q_pg_slint SL_PROTO(back_q_pg_split2_b)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slkey_pure_t bmask);
back_q_pg_slint SL_PROTO(back_q_pg_splitk_k2c_af)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slint k, back_q_pg_slint *c, back_q_pg_k2c_func k2c, void *k2c_data);
back_q_pg_slint SL_PROTO(back_q_pg_splitk_k2c)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_slint k, back_q_pg_slint *c, back_q_pg_k2c_func k2c, void *k2c_data);
back_q_pg_slint SL_PROTO(back_q_pg_splitk_k2c_count)(back_q_pg_elements_t *s, back_q_pg_slint k, back_q_pg_slint *c, back_q_pg_k2c_func k2c, void *k2c_data);


#ifdef SL_USE_MPI

/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_protos_mpi.h
 *  timestamp: 2011-01-13 10:37:23 +0100
 *  
 */




/* src/core_mpi/mpi_binning.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_binning_create)(back_q_pg_global_bins_t *gb, back_q_pg_slint_t max_nbins, back_q_pg_slint_t max_nbinnings, back_q_pg_elements_t *s, back_q_pg_slint_t nelements, back_q_pg_slint_t doweights, back_q_pg_binning_t *bm, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_binning_destroy)(back_q_pg_global_bins_t *gb, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_binning_pre)(back_q_pg_global_bins_t *gb, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_binning_exec_reset)(back_q_pg_global_bins_t *gb, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_binning_exec_local)(back_q_pg_global_bins_t *gb, back_q_pg_slint_t b, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_binning_exec_global)(back_q_pg_global_bins_t *gb, back_q_pg_slint_t root, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_binning_refine)(back_q_pg_global_bins_t *gb, back_q_pg_slint_t b, back_q_pg_slint_t k, back_q_pg_splitter_t *sp, back_q_pg_slint_t s, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_binning_hit)(back_q_pg_global_bins_t *gb, back_q_pg_slint_t b, back_q_pg_slint_t k, back_q_pg_splitter_t *sp, back_q_pg_slint_t s, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_binning_finalize)(back_q_pg_global_bins_t *gb, back_q_pg_slint_t b, back_q_pg_slweight_t dcw, back_q_pg_slint_t lc_min, back_q_pg_slint_t lc_max, back_q_pg_slweight_t *lcw, back_q_pg_splitter_t *sp, back_q_pg_slint_t s, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_binning_post)(back_q_pg_global_bins_t *gb, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_common.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_datatypes_init)();
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_datatypes_release)();
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_get_grid)(back_q_pg_slint_t ndims, back_q_pg_slint_t *dims, back_q_pg_slint_t *pos, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_subgroups_create)(back_q_pg_slint_t nsubgroups, MPI_Comm *sub_comms, int *sub_sizes, int *sub_ranks, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_subgroups_delete)(back_q_pg_slint_t nsubgroups, MPI_Comm *sub_comms, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_elements.c */
back_q_pg_slint SL_PROTO(back_q_pg_mpi_elements_init_keys_from_file)(back_q_pg_elements_t *s, char *filename, back_q_pg_slint from, back_q_pg_slint to, back_q_pg_slint const_bytes_per_line, back_q_pg_slint root, int size, int rank, MPI_Comm comm);
back_q_pg_slint SL_PROTO(back_q_pg_mpi_elements_validate_order)(back_q_pg_elements_t *s, back_q_pg_slint n, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_linear_exchange_pure_keys)(back_q_pg_slkey_pure_t *in, back_q_pg_slkey_pure_t *out, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_elements_check_order)(back_q_pg_elements_t *s, back_q_pg_slint_t nelements, back_q_pg_slint_t *orders, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_check_global_order)(back_q_pg_slkey_pure_t local_min, back_q_pg_slkey_pure_t local_max, int root, int size, int rank, MPI_Comm comm);
unsigned short SL_PROTO(back_q_pg_mpi_cs16)(back_q_pg_elements_t *s, back_q_pg_slint n, back_q_pg_slint keys, back_q_pg_slint data, int size, int rank, MPI_Comm comm);
unsigned int SL_PROTO(back_q_pg_mpi_cs32)(back_q_pg_elements_t *s, back_q_pg_slint n, back_q_pg_slint keys, back_q_pg_slint data, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_elements_get_counts)(back_q_pg_elements_t *s, back_q_pg_slint_t *clocal, back_q_pg_slint_t *cglobal, int root, int size, int rank, MPI_Comm comm);
back_q_pg_slweight_t SL_PROTO(back_q_pg_mpi_elements_get_weights)(back_q_pg_elements_t *s, back_q_pg_slweight_t *wlocal, back_q_pg_slweight_t *wglobal, int root, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_elements_get_counts_and_weights)(back_q_pg_elements_t *s, back_q_pg_slint_t nelements, back_q_pg_slint_t *counts, back_q_pg_slweight_t *weights, int root, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_elements_sendrecv_replace)(back_q_pg_elements_t *s, int count, int dest, int sendtag, int source, int recvtag, int size, int rank, MPI_Comm comm);
unsigned int SL_PROTO(back_q_pg_mpi_elements_crc32)(back_q_pg_elements_t *s, back_q_pg_slint_t n, back_q_pg_slint_t keys, back_q_pg_slint_t data, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_elements_alltoallv.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_elements_alltoallv_db)(back_q_pg_elements_t *sbuf, int *scounts, int *sdispls, back_q_pg_elements_t *rbuf, int *rcounts, int *rdispls, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_elements_alltoallv_ip)(back_q_pg_elements_t *sbuf, back_q_pg_elements_t *sx, int *scounts, int *sdispls, int *rcounts, int *rdispls, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_elements_packed.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_elements_packed_datatype_create)(MPI_Datatype *pdt, back_q_pg_slint_t structured);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_elements_packed_datatype_destroy)(MPI_Datatype *pdt);

/* src/core_mpi/back_q_pg_mpi_find_exact.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_find_exact_equal)(back_q_pg_elements_t *s, back_q_pg_slint_t other_rank, back_q_pg_slint_t high_rank, back_q_pg_slint_t *ex_start, back_q_pg_slint_t *ex_size, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_find_exact)(back_q_pg_elements_t *s, back_q_pg_slint_t other_rank, back_q_pg_slint_t high_rank, back_q_pg_slint_t *dst_size, back_q_pg_slint_t *ex_start, back_q_pg_slint_t *ex_sizes, back_q_pg_slint_t *nx_move, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_linsplit.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_linsplit)(MPI_Comm comm_in, back_q_pg_slkey_pure_t *keys_in, MPI_Comm *comms_out, back_q_pg_slint_t *parity, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_linsplit2)(MPI_Comm comm_in, back_q_pg_slkey_pure_t *keys_in, MPI_Comm *comms_out, back_q_pg_slint_t *parity, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_merge2.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_merge2)(back_q_pg_elements_t *s, back_q_pg_slint_t other_rank, back_q_pg_slint_t high_rank, back_q_pg_slint_t *dst_size, back_q_pg_merge2x_f m2, back_q_pg_elements_t *xs, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_mergek.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_mergek_equal)(back_q_pg_elements_t *s, back_q_pg_sortnet_f sn, back_q_pg_sortnet_data_t snd, back_q_pg_merge2x_f m2x, back_q_pg_elements_t *xs, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_mergek)(back_q_pg_elements_t *s, back_q_pg_sortnet_f sn, back_q_pg_sortnet_data_t snd, back_q_pg_merge2x_f m2x, back_q_pg_elements_t *xs, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_mergek_equal2)(back_q_pg_elements_t *s, back_q_pg_sortnet_f sn, back_q_pg_sortnet_data_t snd, back_q_pg_merge2x_f m2x, back_q_pg_elements_t *xs, int *sizes, int *ranks, MPI_Comm *comms);

/* src/core_mpi/mpi_mergek_presort.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_mergek_presort_radix)(back_q_pg_mergek_t *mk, back_q_pg_slint_t rmerge, back_q_pg_elements_t *s_in, MPI_Comm *comms_in, back_q_pg_slint_t parity_in, back_q_pg_elements_t *s_out, MPI_Comm *comms_out, back_q_pg_slint_t *parity_out, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_partition_exact_generic.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_partition_exact_generic)(back_q_pg_elements_t *s, back_q_pg_partcond_t *pcond, back_q_pg_binning_t *bm, int *scounts, int *rcounts, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_partition_exact_generic2)(back_q_pg_elements_t *s, back_q_pg_partcond_t *pcond, back_q_pg_binning_t *bm, int *scounts, int *rcounts, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_partition_exact_radix.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_partition_exact_radix)(back_q_pg_elements_t *s, back_q_pg_partcond_t *pcond, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth, back_q_pg_slint_t sorted, int *scounts, int *rcounts, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_partition_exact_radix_grouped.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_partition_exact_radix_ngroups)(back_q_pg_elements_t *s, back_q_pg_partcond_t *pcond, back_q_pg_slint_t ngroups, MPI_Comm *group_comms, back_q_pg_elements_t *sx, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth, int *scounts, int *rcounts, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_partition_exact_radix_2groups)(back_q_pg_elements_t *s, back_q_pg_partcond_t *pcond, MPI_Comm group_comm, back_q_pg_elements_t *sx, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth, int *scounts, int *rcounts, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_partition_joink.c */
back_q_pg_slint SL_PROTO(back_q_pg_mpi_partition_joink)(back_q_pg_elements_t *s, back_q_pg_slint *sizes, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_partition_radix2.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_partition_radix2)(back_q_pg_elements_t *s, back_q_pg_partcond2_t *pc, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth, int *scounts, int *sdispls, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_partition_radix_old.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_partition_radix_old)(back_q_pg_elements_t *s, back_q_pg_partcond2_t *pc, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth, int *scounts, int *sdispls, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_partition_sample.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_partition_sample_regular)(back_q_pg_elements_t *s, back_q_pg_partcond_t *pcond, int *scounts, int *rcounts, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_rebalance.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_rebalance)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_slint_t stable, back_q_pg_slint_t *dst_size, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_rebalance_alltoallv)(back_q_pg_elements_t *sbuf, int *scounts, int *sdispls, back_q_pg_elements_t *rbuf, int *rcounts, int *rdispls, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_redistribute_exact_generic.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_redistribute_exact_generic)(back_q_pg_elements_t *sin, back_q_pg_elements_t *sout, back_q_pg_partcond_t *pcond, back_q_pg_parttype_t *ptype, back_q_pg_binning_t *bm, int *rcounts, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_select_common.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_init_partconds)(back_q_pg_slint_t npconds, back_q_pg_partcond_t *pconds, back_q_pg_slint_t nparts, back_q_pg_slint_t total_count, back_q_pg_slweight_t total_weight);
back_q_pg_slint_t SL_PROTO(back_q_pg_init_partconds_intern)(back_q_pg_slint_t npconds, back_q_pg_partcond_intern_t *pci, back_q_pg_partcond_t *pc, back_q_pg_slint_t nparts, back_q_pg_slint_t total_count, back_q_pg_slweight_t total_weight);
back_q_pg_slint_t SL_PROTO(back_q_pg_merge_partconds)(back_q_pg_partcond_t *pconds_in, back_q_pg_slint_t npconds_in, back_q_pg_partcond_t *pcond_out);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_gather_partconds_grouped)(back_q_pg_partcond_t *pcond_in, MPI_Comm pcond_in_comm, MPI_Comm pconds_out_comm, back_q_pg_partcond_t *pconds_out, back_q_pg_slint_t *npconds_out, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_gather_partconds)(back_q_pg_partcond_t *pcond_in, back_q_pg_partcond_t *pconds_out, int root, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_allgather_partconds)(back_q_pg_partcond_t *pcond_in, back_q_pg_partcond_t *pconds_out, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_bcast_partconds)(back_q_pg_slint_t npconds, back_q_pg_partcond_t *pconds, int root, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_post_check_partconds)(back_q_pg_elements_t *s, back_q_pg_slint_t nelements, back_q_pg_slint_t nparts, back_q_pg_partcond_t *pconds, int *sdispls, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_post_check_partconds_intern)(back_q_pg_elements_t *s, back_q_pg_slint_t nelements, back_q_pg_slint_t nparts, back_q_pg_partcond_intern_t *pci, int *sdispls, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_select_stats)(back_q_pg_elements_t *s, back_q_pg_slint_t nparts, int *sdispls, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_select_exact_generic.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_select_exact_generic_bulk)(back_q_pg_elements_t *s, back_q_pg_slint_t nelements, back_q_pg_slint_t nparts, back_q_pg_partcond_t *pconds, back_q_pg_binning_t *bm, back_q_pg_splitter_t *sp, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_select_exact_generic_grouped)(back_q_pg_elements_t *s, back_q_pg_slint_t nelements, back_q_pg_partcond_t *pcond, MPI_Comm pcond_comm, MPI_Comm group_comm, back_q_pg_binning_t *bm, back_q_pg_splitter_t *sp, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_select_exact_generic)(back_q_pg_elements_t *s, back_q_pg_slint_t nelements, back_q_pg_slint_t nparts, back_q_pg_partcond_t *pconds, back_q_pg_binning_t *bm, back_q_pg_splitter_t *sp, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_select_exact_radix.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_select_exact_radix)(back_q_pg_elements_t *s, back_q_pg_slint_t nelements, back_q_pg_slint_t nparts, back_q_pg_partcond_t *pconds, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth, back_q_pg_slint_t sorted, int *sdispls, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_select_exact_radix_fixed)(back_q_pg_elements_t *s, back_q_pg_slint_t nelements, back_q_pg_slint_t nparts, back_q_pg_partcond_t *pconds, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth, int *sdispls, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_select_exact_radix_grouped)(back_q_pg_elements_t *s, back_q_pg_slint_t nelements, back_q_pg_partcond_t *pcond, MPI_Comm pcond_comm, MPI_Comm group_comm, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth, back_q_pg_slint_t sorted, int *sdispls, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_select_qs.c */
back_q_pg_slint SL_PROTO(back_q_pg_mpi_select_qs)(back_q_pg_elements_t *s, back_q_pg_slint n, back_q_pg_slint *iths, back_q_pg_pivot_func pi, back_q_pg_slint threshold, back_q_pg_slkey_pure_t *e, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_select_radix2.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_select_radix2)(back_q_pg_elements_t *s, back_q_pg_slint_t nparts, back_q_pg_partcond_t *pconds, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth, int *sdispls, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_select_stats2)(back_q_pg_elements_t *s, back_q_pg_slint_t nparts, int *sdispls, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_select_radix_pg2)(back_q_pg_elements_t *s, back_q_pg_slint_t nparts, back_q_pg_partcond_t *pcond, MPI_Comm pcomm, MPI_Comm mcomm, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth, int *sdispls, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_select_sample.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_select_sample_regular)(back_q_pg_elements_t *s, back_q_pg_slint_t nparts, back_q_pg_partcond_t *pconds, back_q_pg_slint_t nsamples, back_q_pg_splitter_t *sp, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_sort_merge.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_sort_merge)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *xs, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_sort_merge2)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *xs, back_q_pg_slint_t merge_type, back_q_pg_slint_t sort_type, double *times, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_sort_partition.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_sort_partition)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *xs, back_q_pg_slint_t part_type, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_sort_partition_radix)(back_q_pg_elements_t *s0, back_q_pg_elements_t *s1, back_q_pg_elements_t *xs, back_q_pg_slint_t part_type, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_sort_partition_exact_radix)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_partcond_t *pcond, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_sort_partition_exact_radix_ngroups)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_partcond_t *pcond, back_q_pg_slint_t ngroups, MPI_Comm *group_comms, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_sort_partition_exact_radix_2groups)(back_q_pg_elements_t *s, back_q_pg_elements_t *sx, back_q_pg_partcond_t *pcond, MPI_Comm group_comm, back_q_pg_slint_t rhigh, back_q_pg_slint_t rlow, back_q_pg_slint_t rwidth, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_splitk.c */
back_q_pg_slint SL_PROTO(back_q_pg_mpi_splitk)(back_q_pg_elements_t *s, back_q_pg_k2c_func k2c, void *ci, back_q_pg_elements_t *sx, back_q_pg_elements_t *sa, back_q_pg_slint *nne, back_q_pg_slint *nue, int size, int rank, MPI_Comm comm);

/* src/core_mpi/back_q_pg_mpi_splitk_dummy.c */
back_q_pg_slint SL_PROTO(back_q_pg_mpi_splitk_dummy)(back_q_pg_elements_t *s, back_q_pg_k2c_func k2c, void *ci, back_q_pg_elements_t *sx, back_q_pg_slint *send_stats, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_xcounts2ycounts.c */
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_xcounts2ycounts_all2all)(int *xcounts, int *ycounts, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_xcounts2ycounts_sparse)(int *xcounts, int *ycounts, back_q_pg_slint_t ytotal, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_xcounts2ycounts_grouped)(int *xcounts, back_q_pg_slint_t nxcounts, int *ycounts, MPI_Comm group_comm, MPI_Comm master_comm, int size, int rank, MPI_Comm comm);
back_q_pg_slint_t SL_PROTO(back_q_pg_mpi_subxdispls2ycounts)(back_q_pg_slint_t nsubs, int *sub_xdispls, back_q_pg_slint_t *sub_sources, back_q_pg_slint_t *sub_sizes, MPI_Comm sub_comm, int sub_size, int *ycounts, int size, int rank, MPI_Comm comm);


#endif /* SL_USE_MPI */


#undef SL_PROTO
#endif /* __SL_BACK_Q_PG_H__ */
