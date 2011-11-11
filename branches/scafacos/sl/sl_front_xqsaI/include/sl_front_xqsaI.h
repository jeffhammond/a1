
#ifndef __SL_FRONT_XQSAI_H__
#define __SL_FRONT_XQSAI_H__

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
#define front_xqsaI_sl_int_type_c             SL_INTEGER_C
#define front_xqsaI_sl_int_type_mpi           SL_INTEGER_MPI
#define front_xqsaI_sl_int_size_mpi           1
#define front_xqsaI_sl_int_type_fmt           SL_INTEGER_FMT


/* key section */
#define front_xqsaI_sl_key_type_c             INTEGER_C
#define front_xqsaI_sl_key_type_mpi           INTEGER_MPI
#define front_xqsaI_sl_key_size_mpi           1

#define front_xqsaI_sl_key_integer
#define front_xqsaI_sl_key_type_fmt           INTEGER_FMT


/* data section */
#define front_xqsaI_SL_DATA0                  /* xyz */
#define front_xqsaI_sl_data0_type_c           REAL_C
#define front_xqsaI_sl_data0_size_c           3
#define front_xqsaI_sl_data0_type_mpi         REAL_MPI
#define front_xqsaI_sl_data0_size_mpi         3

#define front_xqsaI_SL_DATA1                  /* q */
#define front_xqsaI_sl_data1_type_c           REAL_C
#define front_xqsaI_sl_data1_size_c           1
#define front_xqsaI_sl_data1_type_mpi         REAL_MPI
#define front_xqsaI_sl_data1_size_mpi         1

#define front_xqsaI_SL_DATA2                  /* scr */
#define front_xqsaI_sl_data2_type_c           INTEGER_C
#define front_xqsaI_sl_data2_size_c           1
#define front_xqsaI_sl_data2_type_mpi         INTEGER_MPI
#define front_xqsaI_sl_data2_size_mpi         1

#define front_xqsaI_SL_DATA3                  /* addr */
#define front_xqsaI_sl_data3_type_c           INTEGER_C
#define front_xqsaI_sl_data3_size_c           1
#define front_xqsaI_sl_data3_type_mpi         INTEGER_MPI
#define front_xqsaI_sl_data3_size_mpi         1
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


#ifndef front_xqsaI_SL_INDEX
# undef front_xqsaI_SL_PACKED_INDEX
#endif


/* if no special, given, primary and heavy used integer-type ... */
#ifndef front_xqsaI_sl_int_type_c
  /* ... use a default one */
# define front_xqsaI_sl_int_type_c               long      /* sl_macro */
# undef front_xqsaI_sl_int_type_mpi
# define front_xqsaI_sl_int_type_mpi             MPI_LONG  /* sl_macro */
# undef front_xqsaI_sl_int_size_mpi
# define front_xqsaI_sl_int_size_mpi             1         /* sl_macro */
# undef front_xqsaI_sl_int_type_fmt
# define front_xqsaI_sl_int_type_fmt             "ld"      /* sl_macro */
#else
  /* ... use the given one and check whether an mpi-type is present and required */
# ifdef SL_USE_MPI
#  if !defined(front_xqsaI_sl_int_type_mpi) || !defined(front_xqsaI_sl_int_size_mpi)
#   error "front_xqsaI_sl_int_type_mpi and/or front_xqsaI_sl_int_size_mpi missing"
#  endif
# endif
# ifndef front_xqsaI_sl_int_type_fmt
#  error "front_xqsaI_sl_int_type_fmt macro is missing, using d as default"
#  define front_xqsaI_sl_int_type_fmt  "d"
# endif
#endif


/* if no special datatype for (intern) weight ... */
#ifndef front_xqsaI_sl_weight_type_c
 /* ... use the double */
# define front_xqsaI_sl_weight_type_c             double      /* sl_macro */
# undef front_xqsaI_sl_weight_type_mpi
# define front_xqsaI_sl_weight_type_mpi           MPI_DOUBLE  /* sl_macro */
# undef front_xqsaI_sl_weight_size_mpi
# define front_xqsaI_sl_weight_size_mpi           1           /* sl_macro */
# undef front_xqsaI_sl_weight_type_fmt
# define front_xqsaI_sl_weight_type_fmt           "f"         /* sl_macro */
#else
  /* ... use the given one and check whether an mpi-type is present and required */
# ifdef SL_USE_MPI
#  if !defined(front_xqsaI_sl_weight_type_mpi) || !defined(front_xqsaI_sl_weight_size_mpi)
#   error "front_xqsaI_sl_weight_type_mpi and/or front_xqsaI_sl_weight_size_mpi missing"
#  endif
# endif
# ifndef front_xqsaI_sl_weight_type_fmt
#  error "front_xqsaI_sl_weight_type_fmt macro is missing, using f as default"
#  define front_xqsaI_sl_weight_type_fmt  "f"
# endif
#endif


/* if no special datatype for indexes ... */
#ifndef front_xqsaI_sl_index_type_c
 /* ... use the primary integer type */
# define front_xqsaI_sl_index_type_c             front_xqsaI_sl_int_type_c
# undef front_xqsaI_sl_index_type_mpi
# define front_xqsaI_sl_index_type_mpi           front_xqsaI_sl_int_type_mpi
# undef front_xqsaI_sl_index_size_mpi
# define front_xqsaI_sl_index_size_mpi           front_xqsaI_sl_int_size_mpi
# undef front_xqsaI_sl_index_type_fmt
# define front_xqsaI_sl_index_type_fmt           front_xqsaI_sl_int_type_fmt
#else
  /* ... use the given one and check whether an mpi-type is present and required */
# ifdef SL_USE_MPI
#  if !defined(front_xqsaI_sl_index_type_mpi) || !defined(front_xqsaI_sl_index_size_mpi)
#   error "front_xqsaI_sl_index_type_mpi and/or front_xqsaI_sl_index_size_mpi missing"
#  endif
# endif
# ifndef front_xqsaI_sl_index_type_fmt
#  error "front_xqsaI_sl_index_type_fmt macro is missing, using d as default"
#  define front_xqsaI_sl_index_type_fmt  "d"
# endif
#endif


/* default pure keys */
#ifndef front_xqsaI_sl_key_pure_type_c
# define front_xqsaI_sl_key_pure_type_c          front_xqsaI_sl_key_type_c  /* sl_macro */
#endif
#ifndef front_xqsaI_sl_key_pure_type_mpi
# define front_xqsaI_sl_key_pure_type_mpi        front_xqsaI_sl_key_type_mpi  /* sl_macro */
#endif
#ifndef front_xqsaI_sl_key_pure_size_mpi
# define front_xqsaI_sl_key_pure_size_mpi        front_xqsaI_sl_key_size_mpi  /* sl_macro */
#endif
#ifndef front_xqsaI_sl_key_pure_type_fmt
# ifdef front_xqsaI_sl_key_type_fmt
#  define front_xqsaI_sl_key_pure_type_fmt       front_xqsaI_sl_key_type_fmt  /* sl_macro */
# endif
#endif

#ifndef front_xqsaI_sl_key_purify
 /* key val -> key val */
 #define front_xqsaI_sl_key_purify(k)            (k)  /* sl_macro */
#endif
#ifndef front_xqsaI_sl_key_get_pure
 /* key component pointer -> key val pointer */
 #define front_xqsaI_sl_key_get_pure(k)          (k)  /* sl_macro */
#endif
#ifndef front_xqsaI_sl_key_set_pure
 /* key component pointer and key val */
 #define front_xqsaI_sl_key_set_pure(k, p)       (*(k) = p)  /* sl_macro */
#endif


/* default pure key comparisons */
#ifndef front_xqsaI_sl_key_pure_cmp_eq
 #define front_xqsaI_sl_key_pure_cmp_eq(k0, k1)  ((k0) == (k1))  /* sl_macro */
#endif
#ifndef front_xqsaI_sl_key_pure_cmp_ne
 #define front_xqsaI_sl_key_pure_cmp_ne(k0, k1)  ((k0) != (k1))  /* sl_macro */
#endif
#ifndef front_xqsaI_sl_key_pure_cmp_lt
 #define front_xqsaI_sl_key_pure_cmp_lt(k0, k1)  ((k0) < (k1))  /* sl_macro */
#endif
#ifndef front_xqsaI_sl_key_pure_cmp_le
 #define front_xqsaI_sl_key_pure_cmp_le(k0, k1)  ((k0) <= (k1))  /* sl_macro */
#endif
#ifndef front_xqsaI_sl_key_pure_cmp_gt
 #define front_xqsaI_sl_key_pure_cmp_gt(k0, k1)  ((k0) > (k1))  /* sl_macro */
#endif
#ifndef front_xqsaI_sl_key_pure_cmp_ge
 #define front_xqsaI_sl_key_pure_cmp_ge(k0, k1)  ((k0) >= (k1))  /* sl_macro */
#endif


/* default key comparisons */
#ifndef front_xqsaI_sl_key_cmp_eq
 #define front_xqsaI_sl_key_cmp_eq(k0, k1)       (front_xqsaI_sl_key_pure_cmp_eq(front_xqsaI_sl_key_purify(k0), front_xqsaI_sl_key_purify(k1)))  /* sl_macro */
#endif
#ifndef front_xqsaI_sl_key_cmp_ne
 #define front_xqsaI_sl_key_cmp_ne(k0, k1)       (front_xqsaI_sl_key_pure_cmp_ne(front_xqsaI_sl_key_purify(k0), front_xqsaI_sl_key_purify(k1)))  /* sl_macro */
#endif
#ifndef front_xqsaI_sl_key_cmp_lt
 #define front_xqsaI_sl_key_cmp_lt(k0, k1)       (front_xqsaI_sl_key_pure_cmp_lt(front_xqsaI_sl_key_purify(k0), front_xqsaI_sl_key_purify(k1)))  /* sl_macro */
#endif
#ifndef front_xqsaI_sl_key_cmp_le
 #define front_xqsaI_sl_key_cmp_le(k0, k1)       (front_xqsaI_sl_key_pure_cmp_le(front_xqsaI_sl_key_purify(k0), front_xqsaI_sl_key_purify(k1)))  /* sl_macro */
#endif
#ifndef front_xqsaI_sl_key_cmp_gt
 #define front_xqsaI_sl_key_cmp_gt(k0, k1)       (front_xqsaI_sl_key_pure_cmp_gt(front_xqsaI_sl_key_purify(k0), front_xqsaI_sl_key_purify(k1)))  /* sl_macro */
#endif
#ifndef front_xqsaI_sl_key_cmp_ge
 #define front_xqsaI_sl_key_cmp_ge(k0, k1)       (front_xqsaI_sl_key_pure_cmp_ge(front_xqsaI_sl_key_purify(k0), front_xqsaI_sl_key_purify(k1)))  /* sl_macro */
#endif


/* default random key */
#ifdef front_xqsaI_sl_key_integer
# if !defined(front_xqsaI_sl_key_val_srand) || !defined(front_xqsaI_sl_key_val_rand) || !defined(front_xqsaI_sl_key_val_rand_minmax)
#  undef front_xqsaI_sl_key_val_srand
#  undef front_xqsaI_sl_key_val_rand
#  undef front_xqsaI_sl_key_val_rand_minmax
#  define front_xqsaI_sl_key_val_srand(_s_)                 sl_srand(_s_)                                        /* sl_macro */
#  define front_xqsaI_sl_key_val_rand()                     ((front_xqsaI_sl_key_pure_type_c) sl_rand())                     /* sl_macro */
#  define front_xqsaI_sl_key_val_rand_minmax(_min_, _max_)  ((front_xqsaI_sl_key_pure_type_c) sl_rand_minmax(_min_, _max_))  /* sl_macro */
# endif
#endif


/* disable data components on request */
/* DATAX_TEMPLATE_BEGIN */
#ifdef front_xqsaI_SL_DATA0_IGNORE
# undef front_xqsaI_SL_DATA0
#endif
#ifdef front_xqsaI_SL_DATA1_IGNORE
# undef front_xqsaI_SL_DATA1
#endif
#ifdef front_xqsaI_SL_DATA2_IGNORE
# undef front_xqsaI_SL_DATA2
#endif
#ifdef front_xqsaI_SL_DATA3_IGNORE
# undef front_xqsaI_SL_DATA3
#endif
#ifdef front_xqsaI_SL_DATA4_IGNORE
# undef front_xqsaI_SL_DATA4
#endif
#ifdef front_xqsaI_SL_DATA5_IGNORE
# undef front_xqsaI_SL_DATA5
#endif
#ifdef front_xqsaI_SL_DATA6_IGNORE
# undef front_xqsaI_SL_DATA6
#endif
#ifdef front_xqsaI_SL_DATA7_IGNORE
# undef front_xqsaI_SL_DATA7
#endif
#ifdef front_xqsaI_SL_DATA8_IGNORE
# undef front_xqsaI_SL_DATA8
#endif
#ifdef front_xqsaI_SL_DATA9_IGNORE
# undef front_xqsaI_SL_DATA9
#endif
#ifdef front_xqsaI_SL_DATA10_IGNORE
# undef front_xqsaI_SL_DATA10
#endif
#ifdef front_xqsaI_SL_DATA11_IGNORE
# undef front_xqsaI_SL_DATA11
#endif
#ifdef front_xqsaI_SL_DATA12_IGNORE
# undef front_xqsaI_SL_DATA12
#endif
#ifdef front_xqsaI_SL_DATA13_IGNORE
# undef front_xqsaI_SL_DATA13
#endif
#ifdef front_xqsaI_SL_DATA14_IGNORE
# undef front_xqsaI_SL_DATA14
#endif
#ifdef front_xqsaI_SL_DATA15_IGNORE
# undef front_xqsaI_SL_DATA15
#endif
#ifdef front_xqsaI_SL_DATA16_IGNORE
# undef front_xqsaI_SL_DATA16
#endif
#ifdef front_xqsaI_SL_DATA17_IGNORE
# undef front_xqsaI_SL_DATA17
#endif
#ifdef front_xqsaI_SL_DATA18_IGNORE
# undef front_xqsaI_SL_DATA18
#endif
#ifdef front_xqsaI_SL_DATA19_IGNORE
# undef front_xqsaI_SL_DATA19
#endif
/* DATAX_TEMPLATE_END */


/* sl_macro front_xqsaI_sl_elem_weight */


/* disable sl_dataX_weight if there is not weight */
#ifndef front_xqsaI_sl_elem_weight
/* DATAX_TEMPLATE_BEGIN */
# undef front_xqsaI_sl_data0_weight
# undef front_xqsaI_sl_data1_weight
# undef front_xqsaI_sl_data2_weight
# undef front_xqsaI_sl_data3_weight
# undef front_xqsaI_sl_data4_weight
# undef front_xqsaI_sl_data5_weight
# undef front_xqsaI_sl_data6_weight
# undef front_xqsaI_sl_data7_weight
# undef front_xqsaI_sl_data8_weight
# undef front_xqsaI_sl_data9_weight
# undef front_xqsaI_sl_data10_weight
# undef front_xqsaI_sl_data11_weight
# undef front_xqsaI_sl_data12_weight
# undef front_xqsaI_sl_data13_weight
# undef front_xqsaI_sl_data14_weight
# undef front_xqsaI_sl_data15_weight
# undef front_xqsaI_sl_data16_weight
# undef front_xqsaI_sl_data17_weight
# undef front_xqsaI_sl_data18_weight
# undef front_xqsaI_sl_data19_weight
/* DATAX_TEMPLATE_END */
#endif


/* disable front_xqsaI_sl_elem_weight if the weight component is missing */
/* DATAX_TEMPLATE_BEGIN */
#if defined(front_xqsaI_sl_data0_weight) && !defined(front_xqsaI_SL_DATA0)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data1_weight) && !defined(front_xqsaI_SL_DATA1)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data2_weight) && !defined(front_xqsaI_SL_DATA2)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data3_weight) && !defined(front_xqsaI_SL_DATA3)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data4_weight) && !defined(front_xqsaI_SL_DATA4)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data5_weight) && !defined(front_xqsaI_SL_DATA5)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data6_weight) && !defined(front_xqsaI_SL_DATA6)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data7_weight) && !defined(front_xqsaI_SL_DATA7)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data8_weight) && !defined(front_xqsaI_SL_DATA8)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data9_weight) && !defined(front_xqsaI_SL_DATA9)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data10_weight) && !defined(front_xqsaI_SL_DATA10)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data11_weight) && !defined(front_xqsaI_SL_DATA11)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data12_weight) && !defined(front_xqsaI_SL_DATA12)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data13_weight) && !defined(front_xqsaI_SL_DATA13)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data14_weight) && !defined(front_xqsaI_SL_DATA14)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data15_weight) && !defined(front_xqsaI_SL_DATA15)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data16_weight) && !defined(front_xqsaI_SL_DATA16)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data17_weight) && !defined(front_xqsaI_SL_DATA17)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data18_weight) && !defined(front_xqsaI_SL_DATA18)
# undef front_xqsaI_sl_elem_weight
#endif
#if defined(front_xqsaI_sl_data19_weight) && !defined(front_xqsaI_SL_DATA19)
# undef front_xqsaI_sl_elem_weight
#endif
/* DATAX_TEMPLATE_END */


/* verify that the flex component is the last (FIXME: only if packed is on?) */
/* DATAX_TEMPLATE_BEGIN */
#ifdef front_xqsaI_SL_DATA0
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data0_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA1
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data1_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA2
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data2_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA3
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data3_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA4
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data4_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA5
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data5_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA6
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data6_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA7
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data7_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA8
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data8_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA9
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data9_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA10
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data10_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA11
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data11_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA12
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data12_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA13
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data13_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA14
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data14_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA15
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data15_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA16
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data16_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA17
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data17_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA18
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data18_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
#  endif
# endif
#endif
#ifdef front_xqsaI_SL_DATA19
# ifdef front_xqsaI_FLECKS_GUARD
#  error "flexible data component is not the last data component!"
# else
#  ifdef front_xqsaI_sl_data19_flex
#   define front_xqsaI_FLECKS_GUARD  /* sl_macro */
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




/* sl_type front_xqsaI_slint_t front_xqsaI_slint */
typedef front_xqsaI_sl_int_type_c front_xqsaI_slint_t, front_xqsaI_slint;  /* deprecated 'front_xqsaI_slint' */

#define front_xqsaI_slint_fmt   front_xqsaI_sl_int_type_fmt    /* sl_macro */

/* sl_type front_xqsaI_slindex_t */
typedef front_xqsaI_sl_index_type_c front_xqsaI_slindex_t;

#define front_xqsaI_sindex_fmt  front_xqsaI_sl_index_type_fmt  /* sl_macro */

/* sl_type front_xqsaI_slkey_t */
typedef front_xqsaI_sl_key_type_c front_xqsaI_slkey_t;

/* sl_type front_xqsaI_slkey_pure_t front_xqsaI_slpkey_t */
typedef front_xqsaI_sl_key_pure_type_c front_xqsaI_slkey_pure_t, front_xqsaI_slpkey_t;

/* DATAX_TEMPLATE_BEGIN */
/* sl_type front_xqsaI_sldata0_t */
#ifdef front_xqsaI_sl_data0_type_c
typedef front_xqsaI_sl_data0_type_c front_xqsaI_sldata0_t;
#endif
/* sl_type front_xqsaI_sldata1_t */
#ifdef front_xqsaI_sl_data1_type_c
typedef front_xqsaI_sl_data1_type_c front_xqsaI_sldata1_t;
#endif
/* sl_type front_xqsaI_sldata2_t */
#ifdef front_xqsaI_sl_data2_type_c
typedef front_xqsaI_sl_data2_type_c front_xqsaI_sldata2_t;
#endif
/* sl_type front_xqsaI_sldata3_t */
#ifdef front_xqsaI_sl_data3_type_c
typedef front_xqsaI_sl_data3_type_c front_xqsaI_sldata3_t;
#endif
/* sl_type front_xqsaI_sldata4_t */
#ifdef front_xqsaI_sl_data4_type_c
typedef front_xqsaI_sl_data4_type_c front_xqsaI_sldata4_t;
#endif
/* sl_type front_xqsaI_sldata5_t */
#ifdef front_xqsaI_sl_data5_type_c
typedef front_xqsaI_sl_data5_type_c front_xqsaI_sldata5_t;
#endif
/* sl_type front_xqsaI_sldata6_t */
#ifdef front_xqsaI_sl_data6_type_c
typedef front_xqsaI_sl_data6_type_c front_xqsaI_sldata6_t;
#endif
/* sl_type front_xqsaI_sldata7_t */
#ifdef front_xqsaI_sl_data7_type_c
typedef front_xqsaI_sl_data7_type_c front_xqsaI_sldata7_t;
#endif
/* sl_type front_xqsaI_sldata8_t */
#ifdef front_xqsaI_sl_data8_type_c
typedef front_xqsaI_sl_data8_type_c front_xqsaI_sldata8_t;
#endif
/* sl_type front_xqsaI_sldata9_t */
#ifdef front_xqsaI_sl_data9_type_c
typedef front_xqsaI_sl_data9_type_c front_xqsaI_sldata9_t;
#endif
/* sl_type front_xqsaI_sldata10_t */
#ifdef front_xqsaI_sl_data10_type_c
typedef front_xqsaI_sl_data10_type_c front_xqsaI_sldata10_t;
#endif
/* sl_type front_xqsaI_sldata11_t */
#ifdef front_xqsaI_sl_data11_type_c
typedef front_xqsaI_sl_data11_type_c front_xqsaI_sldata11_t;
#endif
/* sl_type front_xqsaI_sldata12_t */
#ifdef front_xqsaI_sl_data12_type_c
typedef front_xqsaI_sl_data12_type_c front_xqsaI_sldata12_t;
#endif
/* sl_type front_xqsaI_sldata13_t */
#ifdef front_xqsaI_sl_data13_type_c
typedef front_xqsaI_sl_data13_type_c front_xqsaI_sldata13_t;
#endif
/* sl_type front_xqsaI_sldata14_t */
#ifdef front_xqsaI_sl_data14_type_c
typedef front_xqsaI_sl_data14_type_c front_xqsaI_sldata14_t;
#endif
/* sl_type front_xqsaI_sldata15_t */
#ifdef front_xqsaI_sl_data15_type_c
typedef front_xqsaI_sl_data15_type_c front_xqsaI_sldata15_t;
#endif
/* sl_type front_xqsaI_sldata16_t */
#ifdef front_xqsaI_sl_data16_type_c
typedef front_xqsaI_sl_data16_type_c front_xqsaI_sldata16_t;
#endif
/* sl_type front_xqsaI_sldata17_t */
#ifdef front_xqsaI_sl_data17_type_c
typedef front_xqsaI_sl_data17_type_c front_xqsaI_sldata17_t;
#endif
/* sl_type front_xqsaI_sldata18_t */
#ifdef front_xqsaI_sl_data18_type_c
typedef front_xqsaI_sl_data18_type_c front_xqsaI_sldata18_t;
#endif
/* sl_type front_xqsaI_sldata19_t */
#ifdef front_xqsaI_sl_data19_type_c
typedef front_xqsaI_sl_data19_type_c front_xqsaI_sldata19_t;
#endif
/* DATAX_TEMPLATE_END */

/* sl_type front_xqsaI_slweight_t */
typedef front_xqsaI_sl_weight_type_c front_xqsaI_slweight_t;

#define front_xqsaI_slweight_fmt  front_xqsaI_sl_weight_type_fmt  /* sl_macro */

/* sl_type front_xqsaI__slpwkey_t front_xqsaI_slpwkey_t */
typedef struct front_xqsaI__slpwkey_t
{
  front_xqsaI_slpkey_t pkey;
  front_xqsaI_slweight_t weight;

} front_xqsaI_slpwkey_t;


/* sl_type front_xqsaI__elements_t front_xqsaI_elements_t */
typedef struct front_xqsaI__elements_t
{
  front_xqsaI_slint_t size, max_size;
  front_xqsaI_slkey_t *keys;

#ifdef front_xqsaI_SL_INDEX
  front_xqsaI_slindex_t *indices;
#endif

/* DATAX_TEMPLATE_BEGIN */
#ifdef front_xqsaI_SL_DATA0
  front_xqsaI_sldata0_t *data0;
#endif
#ifdef front_xqsaI_SL_DATA1
  front_xqsaI_sldata1_t *data1;
#endif
#ifdef front_xqsaI_SL_DATA2
  front_xqsaI_sldata2_t *data2;
#endif
#ifdef front_xqsaI_SL_DATA3
  front_xqsaI_sldata3_t *data3;
#endif
#ifdef front_xqsaI_SL_DATA4
  front_xqsaI_sldata4_t *data4;
#endif
#ifdef front_xqsaI_SL_DATA5
  front_xqsaI_sldata5_t *data5;
#endif
#ifdef front_xqsaI_SL_DATA6
  front_xqsaI_sldata6_t *data6;
#endif
#ifdef front_xqsaI_SL_DATA7
  front_xqsaI_sldata7_t *data7;
#endif
#ifdef front_xqsaI_SL_DATA8
  front_xqsaI_sldata8_t *data8;
#endif
#ifdef front_xqsaI_SL_DATA9
  front_xqsaI_sldata9_t *data9;
#endif
#ifdef front_xqsaI_SL_DATA10
  front_xqsaI_sldata10_t *data10;
#endif
#ifdef front_xqsaI_SL_DATA11
  front_xqsaI_sldata11_t *data11;
#endif
#ifdef front_xqsaI_SL_DATA12
  front_xqsaI_sldata12_t *data12;
#endif
#ifdef front_xqsaI_SL_DATA13
  front_xqsaI_sldata13_t *data13;
#endif
#ifdef front_xqsaI_SL_DATA14
  front_xqsaI_sldata14_t *data14;
#endif
#ifdef front_xqsaI_SL_DATA15
  front_xqsaI_sldata15_t *data15;
#endif
#ifdef front_xqsaI_SL_DATA16
  front_xqsaI_sldata16_t *data16;
#endif
#ifdef front_xqsaI_SL_DATA17
  front_xqsaI_sldata17_t *data17;
#endif
#ifdef front_xqsaI_SL_DATA18
  front_xqsaI_sldata18_t *data18;
#endif
#ifdef front_xqsaI_SL_DATA19
  front_xqsaI_sldata19_t *data19;
#endif
/* DATAX_TEMPLATE_END */

} front_xqsaI_elements_t;


/* sl_type front_xqsaI__packed_element_t front_xqsaI_packed_element_t */
typedef struct front_xqsaI__packed_element_t
{
  front_xqsaI_slkey_t key;

#ifdef front_xqsaI_SL_PACKED_INDEX
  front_xqsaI_slindex_t index;
#endif

/* DATAX_TEMPLATE_BEGIN */
#ifdef front_xqsaI_SL_DATA0
# ifdef front_xqsaI_sl_data0_flex
  front_xqsaI_sldata0_t data0[];
# else
  front_xqsaI_sldata0_t data0[front_xqsaI_sl_data0_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA1
# ifdef front_xqsaI_sl_data1_flex
  front_xqsaI_sldata1_t data1[];
# else
  front_xqsaI_sldata1_t data1[front_xqsaI_sl_data1_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA2
# ifdef front_xqsaI_sl_data2_flex
  front_xqsaI_sldata2_t data2[];
# else
  front_xqsaI_sldata2_t data2[front_xqsaI_sl_data2_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA3
# ifdef front_xqsaI_sl_data3_flex
  front_xqsaI_sldata3_t data3[];
# else
  front_xqsaI_sldata3_t data3[front_xqsaI_sl_data3_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA4
# ifdef front_xqsaI_sl_data4_flex
  front_xqsaI_sldata4_t data4[];
# else
  front_xqsaI_sldata4_t data4[front_xqsaI_sl_data4_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA5
# ifdef front_xqsaI_sl_data5_flex
  front_xqsaI_sldata5_t data5[];
# else
  front_xqsaI_sldata5_t data5[front_xqsaI_sl_data5_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA6
# ifdef front_xqsaI_sl_data6_flex
  front_xqsaI_sldata6_t data6[];
# else
  front_xqsaI_sldata6_t data6[front_xqsaI_sl_data6_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA7
# ifdef front_xqsaI_sl_data7_flex
  front_xqsaI_sldata7_t data7[];
# else
  front_xqsaI_sldata7_t data7[front_xqsaI_sl_data7_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA8
# ifdef front_xqsaI_sl_data8_flex
  front_xqsaI_sldata8_t data8[];
# else
  front_xqsaI_sldata8_t data8[front_xqsaI_sl_data8_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA9
# ifdef front_xqsaI_sl_data9_flex
  front_xqsaI_sldata9_t data9[];
# else
  front_xqsaI_sldata9_t data9[front_xqsaI_sl_data9_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA10
# ifdef front_xqsaI_sl_data10_flex
  front_xqsaI_sldata10_t data10[];
# else
  front_xqsaI_sldata10_t data10[front_xqsaI_sl_data10_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA11
# ifdef front_xqsaI_sl_data11_flex
  front_xqsaI_sldata11_t data11[];
# else
  front_xqsaI_sldata11_t data11[front_xqsaI_sl_data11_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA12
# ifdef front_xqsaI_sl_data12_flex
  front_xqsaI_sldata12_t data12[];
# else
  front_xqsaI_sldata12_t data12[front_xqsaI_sl_data12_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA13
# ifdef front_xqsaI_sl_data13_flex
  front_xqsaI_sldata13_t data13[];
# else
  front_xqsaI_sldata13_t data13[front_xqsaI_sl_data13_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA14
# ifdef front_xqsaI_sl_data14_flex
  front_xqsaI_sldata14_t data14[];
# else
  front_xqsaI_sldata14_t data14[front_xqsaI_sl_data14_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA15
# ifdef front_xqsaI_sl_data15_flex
  front_xqsaI_sldata15_t data15[];
# else
  front_xqsaI_sldata15_t data15[front_xqsaI_sl_data15_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA16
# ifdef front_xqsaI_sl_data16_flex
  front_xqsaI_sldata16_t data16[];
# else
  front_xqsaI_sldata16_t data16[front_xqsaI_sl_data16_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA17
# ifdef front_xqsaI_sl_data17_flex
  front_xqsaI_sldata17_t data17[];
# else
  front_xqsaI_sldata17_t data17[front_xqsaI_sl_data17_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA18
# ifdef front_xqsaI_sl_data18_flex
  front_xqsaI_sldata18_t data18[];
# else
  front_xqsaI_sldata18_t data18[front_xqsaI_sl_data18_size_c];
# endif
#endif
#ifdef front_xqsaI_SL_DATA19
# ifdef front_xqsaI_sl_data19_flex
  front_xqsaI_sldata19_t data19[];
# else
  front_xqsaI_sldata19_t data19[front_xqsaI_sl_data19_size_c];
# endif
#endif
/* DATAX_TEMPLATE_END */

} front_xqsaI_packed_element_t;


/* sl_type front_xqsaI__packed_elements_t front_xqsaI_packed_elements_t */
typedef struct front_xqsaI__packed_elements_t
{
  front_xqsaI_slint_t size, max_size;
  
  front_xqsaI_packed_element_t *elements;
  
} front_xqsaI_packed_elements_t;


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


/* sl_type front_xqsaI__classification_info_t front_xqsaI_classification_info_t front_xqsaI_classification_info */
typedef struct front_xqsaI__classification_info_t
{
  front_xqsaI_slint_t nclasses;
  front_xqsaI_slkey_pure_t *keys;
  front_xqsaI_slint_t *counts;
  front_xqsaI_slint_t *masks;

  /* */
  front_xqsaI_slint_t *all_local_sizes;
  front_xqsaI_slint_t *local_lt_eq_counts;
  front_xqsaI_slint_t *all_local_lt_eq_counts;

} front_xqsaI_classification_info_t, front_xqsaI_classification_info;  /* deprecated 'front_xqsaI_classification_info' */


/* key2class, sl_type front_xqsaI_key2class_f */
typedef front_xqsaI_slint_t (*front_xqsaI_key2class_f)(front_xqsaI_slkey_t *, front_xqsaI_slint, void *);

/* pivot-element, sl_type front_xqsaI_pivot_f */
typedef front_xqsaI_slint_t (*front_xqsaI_pivot_f)(front_xqsaI_elements_t *);

/* sorting-network, sl_type front_xqsaI_sortnet_f front_xqsaI_sortnet_data_t */
typedef void *front_xqsaI_sortnet_data_t;
typedef front_xqsaI_slint_t (*front_xqsaI_sortnet_f)(front_xqsaI_slint_t size, front_xqsaI_slint_t rank, front_xqsaI_slint_t stage, front_xqsaI_sortnet_data_t snd, front_xqsaI_slint_t *up);

/* merge2, sl_type front_xqsaI_merge2x_f front_xqsaI_merge2X_f */
typedef front_xqsaI_slint_t (*front_xqsaI_merge2x_f)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);
typedef front_xqsaI_slint_t (*front_xqsaI_merge2X_f)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx, front_xqsaI_elements_t *t);


/* deprecated, sl_type front_xqsaI_k2c_func front_xqsaI_pivot_func front_xqsaI_sn_func front_xqsaI_m2x_func front_xqsaI_m2X_func */
typedef front_xqsaI_key2class_f front_xqsaI_k2c_func;
typedef front_xqsaI_pivot_f front_xqsaI_pivot_func;
typedef front_xqsaI_sortnet_f front_xqsaI_sn_func;
typedef front_xqsaI_merge2x_f front_xqsaI_m2x_func;
typedef front_xqsaI_merge2X_f front_xqsaI_m2X_func;


/* sl_type front_xqsaI__mergek_t front_xqsaI_mergek_t */
typedef struct front_xqsaI__mergek_t
{
  front_xqsaI_sortnet_f sn;
  front_xqsaI_sortnet_data_t snd;

  front_xqsaI_merge2x_f m2x;
  front_xqsaI_elements_t *sx;

} front_xqsaI_mergek_t;


/* sl_type front_xqsaI_keys_init_type_t front_xqsaI_keys_init_data_t */
typedef front_xqsaI_slint_t front_xqsaI_keys_init_type_t;
typedef void *front_xqsaI_keys_init_data_t;

/* sl_type front_xqsaI_key_set_data_t front_xqsaI_key_set_f */
typedef void *front_xqsaI_key_set_data_t;
typedef void (*front_xqsaI_key_set_f)(front_xqsaI_slkey_pure_t *k, front_xqsaI_key_set_data_t d);


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


/* front_xqsaI_elements_keys_stats */
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


/* partition conditions, sl_type front_xqsaI__partcond2_t front_xqsaI_partcond2_t */
typedef struct front_xqsaI__partcond2_t
{
  int weighted;
  double min_count, max_count;
  double min_weight, max_weight;
  double min_cpart, max_cpart;
  double min_wpart, max_wpart;

} front_xqsaI_partcond2_t;


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

/* partition conditions, sl_type front_xqsaI__partcond_t front_xqsaI_partcond_t front_xqsaI_partcond_p */
typedef struct front_xqsaI__partcond_t
{
  front_xqsaI_slint_t pcm;
  double count_min, count_max;
  double count_low, count_high;
  double weight_min, weight_max;
  double weight_low, weight_high;

} front_xqsaI_partcond_t, *front_xqsaI_partcond_p;


/* internal partition conditions, sl_type front_xqsaI__partcond_intern_t front_xqsaI_partcond_intern_t front_xqsaI_partcond_intern_p */
typedef struct front_xqsaI__partcond_intern_t
{
  front_xqsaI_slint_t pcm;
  front_xqsaI_slint_t count_min, count_max;
  front_xqsaI_slint_t count_low, count_high;
#ifdef elem_weight
  double weight_min, weight_max;
  double weight_low, weight_high;
#endif

} front_xqsaI_partcond_intern_t, *front_xqsaI_partcond_intern_p;


/* sl_type front_xqsaI__parttype_t front_xqsaI_parttype_t front_xqsaI_parttype_p */
typedef struct front_xqsaI__parttype_t
{

} front_xqsaI_parttype_t, *front_xqsaI_parttype_p;


/* generic binning method */

/* sl_type front_xqsaI__bin_t front_xqsaI_bin_t */
typedef struct front_xqsaI__bin_t
{
  front_xqsaI_elements_t s;

#ifdef elem_weight
  double weight;
#endif

} front_xqsaI_bin_t;


/* sl_type front_xqsaI__splitter_t front_xqsaI_splitter_t */
typedef struct front_xqsaI__splitter_t
{
  front_xqsaI_slint_t n;

  int *displs;
  front_xqsaI_slkey_pure_t *s;
  front_xqsaI_slint_t *sn;

} front_xqsaI_splitter_t;


struct front_xqsaI__binning_t;

/* sl_type front_xqsaI_binning_pre_f front_xqsaI_binning_exec_f front_xqsaI_binning_refine_f front_xqsaI_binning_hit_f front_xqsaI_binning_finalize_f front_xqsaI_binning_post_f */
typedef front_xqsaI_slint_t (*front_xqsaI_binning_pre_f)(struct front_xqsaI__binning_t *bm);
typedef front_xqsaI_slint_t (*front_xqsaI_binning_exec_f)(struct front_xqsaI__binning_t *bm, front_xqsaI_bin_t *bin, double *counts, double *weights);
typedef front_xqsaI_slint_t (*front_xqsaI_binning_refine_f)(struct front_xqsaI__binning_t *bm, front_xqsaI_bin_t *bin, front_xqsaI_slint_t k, double *counts, double *weights, front_xqsaI_splitter_t *sp, front_xqsaI_slint_t s, front_xqsaI_bin_t *new_bin);
typedef front_xqsaI_slint_t (*front_xqsaI_binning_hit_f)(struct front_xqsaI__binning_t *bm, front_xqsaI_bin_t *bin, front_xqsaI_slint_t k, double *counts, front_xqsaI_splitter_t *sp, front_xqsaI_slint_t s);
typedef front_xqsaI_slint_t (*front_xqsaI_binning_finalize_f)(struct front_xqsaI__binning_t *bm, front_xqsaI_bin_t *bin, double dcw, front_xqsaI_slint_t lc_min, front_xqsaI_slint_t lc_max, double *lcw, front_xqsaI_splitter_t *sp, front_xqsaI_slint_t s);
typedef front_xqsaI_slint_t (*front_xqsaI_binning_post_f)(struct front_xqsaI__binning_t *bm);

#ifdef SL_DEPRECATED
/* sl_type front_xqsaI_binning_exec_pre_old_f front_xqsaI_binning_exec_post_old_f front_xqsaI_binning_refinable_old_f front_xqsaI_binning_refine_old_f */
typedef front_xqsaI_slint_t (*front_xqsaI_binning_exec_pre_old_f)(struct front_xqsaI__binning_t *bm);
typedef front_xqsaI_slint_t (*front_xqsaI_binning_exec_post_old_f)(struct front_xqsaI__binning_t *bm);
typedef front_xqsaI_slint_t (*front_xqsaI_binning_refinable_old_f)(struct front_xqsaI__binning_t *bm);
typedef front_xqsaI_slint_t (*front_xqsaI_binning_refine_old_f)(struct front_xqsaI__binning_t *bm, front_xqsaI_bin_t *bin, front_xqsaI_slint_t k, double *counts, front_xqsaI_bin_t *new_bin);
#endif


/* sl_type front_xqsaI__binning_data_t front_xqsaI_binning_data_t */
typedef union front_xqsaI__binning_data_t
{
  struct
  {
    front_xqsaI_slint_t rhigh, rlow, rwidth;
    front_xqsaI_slint_t rcurrent;
    front_xqsaI_slkey_pure_t bit_mask;

    front_xqsaI_elements_t sx;

  } radix;

} front_xqsaI_binning_data_t;


/* sl_type front_xqsaI__binning_t front_xqsaI_binning_t */
typedef struct front_xqsaI__binning_t
{
  front_xqsaI_slint_t nbins, max_nbins;
  
  front_xqsaI_binning_pre_f pre;
  front_xqsaI_binning_exec_f exec;
  front_xqsaI_binning_refine_f refine;
  front_xqsaI_binning_hit_f hit;
  front_xqsaI_binning_finalize_f finalize;
  front_xqsaI_binning_post_f post;

  front_xqsaI_slint_t sorted;

#ifdef elem_weight
  front_xqsaI_slint_t doweights;
#endif

#ifdef SL_DEPRECATED
  front_xqsaI_binning_exec_pre_old_f exec_pre_old;
  front_xqsaI_binning_exec_post_old_f exec_post_old;
  front_xqsaI_binning_refinable_old_f refinable_old;
  front_xqsaI_binning_refine_old_f refine_old;
#endif

  front_xqsaI_binning_data_t bd;

} front_xqsaI_binning_t;


/* sl_type front_xqsaI__local_bins_t front_xqsaI_local_bins_t */
typedef struct front_xqsaI__local_bins_t
{
  front_xqsaI_binning_t *bm;

  front_xqsaI_slint_t nbins, max_nbins;
  front_xqsaI_slint_t nelements;

#ifdef elem_weight
  front_xqsaI_slint_t doweights, weight_factor;
#endif

  front_xqsaI_slint_t nbinnings, max_nbinnings;

  front_xqsaI_slint_t nbins_new, last_new_b, last_new_k;
  front_xqsaI_bin_t *bins, *bins_new;
  front_xqsaI_bin_t *bins0, *bins1;

  double *counts, *bin_counts;
#ifdef elem_weight
  double *weights, *bin_weights;
#endif

  front_xqsaI_slint_t *bcws;
  double *cws, *bin_cws;

  front_xqsaI_slint_t last_exec_b;

} front_xqsaI_local_bins_t;


/* sl_type front_xqsaI__global_bins_t front_xqsaI_global_bins_t */
typedef struct front_xqsaI__global_bins_t
{
  front_xqsaI_binning_t *bm;
  
  front_xqsaI_local_bins_t lb;
  
  double *counts;
#ifdef elem_weight
  double *weights;
#endif

  front_xqsaI_slint_t *bcws;
  double *cws;

} front_xqsaI_global_bins_t;


/* sl_macro front_xqsaI_WEIGHT_FACTOR */
#ifdef elem_weight
# define front_xqsaI_WEIGHT_FACTOR  2
#else
# define front_xqsaI_WEIGHT_FACTOR  1
#endif


/* sl_macro front_xqsaI_get1d front_xqsaI_get2d front_xqsaI_get3d front_xqsaI_get4d */
#define front_xqsaI_get1d(x0)                           (x0)
#define front_xqsaI_get2d(x1, d0, x0)                  ((x0) + (d0) *  (x1))
#define front_xqsaI_get3d(x2, d1, x1, d0, x0)          ((x0) + (d0) * ((x1) + (d1) *  (x2)))
#define front_xqsaI_get4d(x3, d2, x2, d1, x1, d0, x0)  ((x0) + (d0) * ((x1) + (d1) * ((x2) + (d2) * (x3))))


/* sl_macro front_xqsaI_lb_bin_count front_xqsaI_lb_bin_weight */
#define front_xqsaI_lb_bin_count(_lb_, _b_, _j_)    ((_lb_)->bins[(_b_) * (_lb_)->nelements + _j_].s.size)
#ifdef elem_weight
# define front_xqsaI_lb_bin_weight(_lb_, _b_, _j_)  ((_lb_)->bins[(_b_) * (_lb_)->nelements + _j_].weight)
#else
# define front_xqsaI_lb_bin_weight(_lb_, _b_, _j_)  0
#endif

/* sl_macro front_xqsaI_lb_bin_counts front_xqsaI_lb_bin_weights */
#ifdef elem_weight
# define front_xqsaI_lb_bin_counts(_lb_, _b_, _j_, _k_)   ((_lb_)->bin_cws + front_xqsaI_get4d((_lb_)->bcws[_b_], (_lb_)->weight_factor, 0, (_lb_)->nelements, _j_, (_lb_)->bm->nbins, _k_))
# define front_xqsaI_lb_bin_weights(_lb_, _b_, _j_, _k_)  ((_lb_)->bin_cws + front_xqsaI_get4d((_lb_)->bcws[_b_], (_lb_)->weight_factor, 1, (_lb_)->nelements, _j_, (_lb_)->bm->nbins, _k_))
#else
# define front_xqsaI_lb_bin_counts(_lb_, _b_, _j_, _k_)   ((_lb_)->bin_cws + front_xqsaI_get4d((_lb_)->bcws[_b_], 1, 0, (_lb_)->nelements, _j_, (_lb_)->bm->nbins, _k_))
# define front_xqsaI_lb_bin_weights(_lb_, _b_, _j_, _k_)  NULL
#endif

/* sl_macro front_xqsaI_lb_counts front_xqsaI_lb_weights */
#ifdef elem_weight
# define front_xqsaI_lb_counts(_lb_, _b_, _k_)   ((_lb_)->cws + front_xqsaI_get3d((_lb_)->bcws[_b_], (_lb_)->weight_factor, 0, (_lb_)->bm->nbins, (_k_)))
# define front_xqsaI_lb_weights(_lb_, _b_, _k_)  ((_lb_)->cws + front_xqsaI_get3d((_lb_)->bcws[_b_], (_lb_)->weight_factor, 1, (_lb_)->bm->nbins, (_k_)))
#else
# define front_xqsaI_lb_counts(_lb_, _b_, _k_)   ((_lb_)->cws + front_xqsaI_get3d((_lb_)->bcws[_b_], 1, 0, (_lb_)->bm->nbins, (_k_)))
# define front_xqsaI_lb_weights(_lb_, _b_, _k_)  NULL
#endif

/* sl_macro front_xqsaI_gb_counts front_xqsaI_gb_weights */
#ifdef elem_weight
# define front_xqsaI_gb_counts(_gb_, _b_, _k_)   ((_gb_)->cws + front_xqsaI_get3d((_gb_)->bcws[_b_], (_gb_)->lb.weight_factor, 0, (_gb_)->bm->nbins, (_k_)))
# define front_xqsaI_gb_weights(_gb_, _b_, _k_)  ((_gb_)->cws + front_xqsaI_get3d((_gb_)->bcws[_b_], (_gb_)->lb.weight_factor, 1, (_gb_)->bm->nbins, (_k_)))
#else
# define front_xqsaI_gb_counts(_gb_, _b_, _k_)   ((_gb_)->cws + front_xqsaI_get3d((_gb_)->bcws[_b_], 1, 0, (_gb_)->bm->nbins, (_k_)))
# define front_xqsaI_gb_weights(_gb_, _b_, _k_)  NULL
#endif


/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_adds.h
 *  timestamp: 2010-08-12 10:08:32 +0200
 *  
 */




/* sl_macro front_xqsaI_elem_set_size front_xqsaI_elem_set_max_size front_xqsaI_elem_set_keys front_xqsaI_elem_set_indices */
#define front_xqsaI_elem_set_size(_e_, _s_)      ((_e_)->size = (_s_))
#define front_xqsaI_elem_set_max_size(_e_, _s_)  ((_e_)->max_size = (_s_))
#define front_xqsaI_elem_set_keys(_e_, _k_)      ((_e_)->keys = (_k_))
#define front_xqsaI_elem_set_indices(_e_, _i_)   ((_e_)->indices = (_i_))

/* sl_macro front_xqsaI_pelem_set_size front_xqsaI_pelem_set_max_size front_xqsaI_pelem_set_elements */
#define front_xqsaI_pelem_set_size(_e_, _s_)      ((_e_)->size = (_s_))
#define front_xqsaI_pelem_set_max_size(_e_, _s_)  ((_e_)->max_size = (_s_))
#define front_xqsaI_pelem_set_elements(_e_, _l_)  ((_e_)->elements = (_l_))


#ifndef SL_FRACRES
# define SL_FRACRES       1000000.0
# define SL_FRAC2INT(p)   ((front_xqsaI_slint_t) ((p) * -SL_FRACRES))
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
extern FILE *front_xqsaI_sl_notice_fstream;
extern FILE *front_xqsaI_sl_error_fstream;
extern FILE *front_xqsaI_sl_debug_fstream;

/* src/include/sl_rti_intern.h */
extern rti front_xqsaI_rti_env;

/* src/core/front_xqsaI_sort_radix_ip.c */
extern front_xqsaI_slint_t front_xqsaI_sa_ip_threshold;

/* src/core_mpi/mpi_common.c */
#ifdef SL_USE_MPI
extern MPI_Datatype front_xqsaI_int_mpi_datatype;
extern MPI_Datatype front_xqsaI_key_mpi_datatype;
extern MPI_Datatype front_xqsaI_pkey_mpi_datatype;
extern MPI_Datatype front_xqsaI_pwkey_mpi_datatype;
extern MPI_Datatype front_xqsaI_index_mpi_datatype;
extern MPI_Datatype front_xqsaI_weight_mpi_datatype;
extern MPI_Datatype front_xqsaI_data_mpi_datatype[];
#endif
#ifdef SL_USE_MPI
extern int front_xqsaI_sl_mpi_rank;
#endif

/* src/core_mpi/mpi_elements.c */
extern void *front_xqsaI_me_sendrecv_replace_mem;
extern front_xqsaI_slint_t front_xqsaI_me_sendrecv_replace_memsize;
extern front_xqsaI_slint_t front_xqsaI_me_sendrecv_replace_mpi_maxsize;

/* src/core_mpi/front_xqsaI_mpi_select_exact_generic.c */
extern front_xqsaI_slint_t front_xqsaI_mseg_rounds;
extern int front_xqsaI_mseg_root;
extern double front_xqsaI_mseg_border_update_propagation;
extern front_xqsaI_slint_t front_xqsaI_mseg_border_update_full;
extern front_xqsaI_slint_t front_xqsaI_mseg_binnings;

/* src/core_mpi/front_xqsaI_mpi_sort_partition.c */
extern double front_xqsaI_msp_t[];
extern front_xqsaI_slint_t front_xqsaI_msp_sync;


/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_protos.h
 *  timestamp: 2011-01-13 10:37:23 +0100
 *  
 */




/* src/core/binning.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_create)(front_xqsaI_local_bins_t *lb, front_xqsaI_slint_t max_nbins, front_xqsaI_slint_t max_nbinnings, front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, front_xqsaI_slint_t doweights, front_xqsaI_binning_t *bm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_destroy)(front_xqsaI_local_bins_t *lb);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_pre)(front_xqsaI_local_bins_t *lb);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_exec_reset)(front_xqsaI_local_bins_t *lb);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_exec)(front_xqsaI_local_bins_t *lb, front_xqsaI_slint_t b);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_refine)(front_xqsaI_local_bins_t *lb, front_xqsaI_slint_t b, front_xqsaI_slint_t k, front_xqsaI_splitter_t *sp, front_xqsaI_slint_t s);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_hit)(front_xqsaI_local_bins_t *lb, front_xqsaI_slint_t b, front_xqsaI_slint_t k, front_xqsaI_splitter_t *sp, front_xqsaI_slint_t s);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_finalize)(front_xqsaI_local_bins_t *lb, front_xqsaI_slint_t b, front_xqsaI_slweight_t dcw, front_xqsaI_slint_t lc_min, front_xqsaI_slint_t lc_max, front_xqsaI_slweight_t *lcw, front_xqsaI_splitter_t *sp, front_xqsaI_slint_t s);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_post)(front_xqsaI_local_bins_t *lb);

/* src/core/binning_radix.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_radix_create)(front_xqsaI_binning_t *bm, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth, front_xqsaI_slint_t sorted);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_radix_destroy)(front_xqsaI_binning_t *bm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_radix_pre)(front_xqsaI_binning_t *bm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_radix_exec)(front_xqsaI_binning_t *bm, front_xqsaI_bin_t *bin, front_xqsaI_slweight_t *counts, front_xqsaI_slweight_t *weights);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_radix_refine)(front_xqsaI_binning_t *bm, front_xqsaI_bin_t *bin, front_xqsaI_slint_t k, front_xqsaI_slweight_t *counts, front_xqsaI_slweight_t *weights, front_xqsaI_splitter_t *sp, front_xqsaI_slint_t s, front_xqsaI_bin_t *new_bin);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_radix_hit)(front_xqsaI_binning_t *bm, front_xqsaI_bin_t *bin, front_xqsaI_slint_t k, front_xqsaI_slweight_t *counts, front_xqsaI_splitter_t *sp, front_xqsaI_slint_t s);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_radix_finalize)(front_xqsaI_binning_t *bm, front_xqsaI_bin_t *bin, front_xqsaI_slweight_t dcw, front_xqsaI_slint_t lc_min, front_xqsaI_slint_t lc_max, front_xqsaI_slweight_t *lcw, front_xqsaI_splitter_t *sp, front_xqsaI_slint_t s);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_radix_post)(front_xqsaI_binning_t *bm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_radix_exec_pre_old)(front_xqsaI_binning_t *bm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_radix_exec_post_old)(front_xqsaI_binning_t *bm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_radix_refinable_old)(front_xqsaI_binning_t *bm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_binning_radix_refine_old)(front_xqsaI_binning_t *bm, front_xqsaI_bin_t *bin, front_xqsaI_slint_t k, front_xqsaI_slweight_t *counts, front_xqsaI_bin_t *new_bin);

/* src/core/checksum_crc.c */
unsigned short SL_PROTO(front_xqsaI_elements_crc16)(front_xqsaI_elements_t *s, front_xqsaI_slint n, front_xqsaI_slint keys, front_xqsaI_slint data);
unsigned int SL_PROTO(front_xqsaI_elements_crc32)(front_xqsaI_elements_t *s, front_xqsaI_slint n, front_xqsaI_slint keys, front_xqsaI_slint data);

/* src/core/elements.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_alloc)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, slcint_t components);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_alloc2)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, front_xqsaI_slint_t keys, front_xqsaI_slint_t indices, front_xqsaI_slint_t data, front_xqsaI_slint_t weights);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_alloc_old)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, front_xqsaI_slint_t keys, front_xqsaI_slint_t data);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_free)(front_xqsaI_elements_t *s);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_alloc_from_blocks)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nblocks, void **blocks, front_xqsaI_slint_t *blocksizes, front_xqsaI_slint_t alignment, front_xqsaI_slint_t nmax, slcint_t components);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_alloc_from_block2)(front_xqsaI_elements_t *s, void *block, front_xqsaI_slint_t blocksize, front_xqsaI_slint_t alignment, front_xqsaI_slint_t nmax, front_xqsaI_slint_t keys, front_xqsaI_slint_t indices, front_xqsaI_slint_t data, front_xqsaI_slint_t weights);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_alloc_from_block)(front_xqsaI_elements_t *s, void *block, front_xqsaI_slint_t blocksize, front_xqsaI_slint_t alignment, front_xqsaI_slint_t nmax);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_copy)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *d);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_copy_at)(front_xqsaI_elements_t *s, front_xqsaI_slint_t sat, front_xqsaI_elements_t *d, front_xqsaI_slint_t dat);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_ncopy)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *d, front_xqsaI_slint_t n);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_nmove)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *d, front_xqsaI_slint_t n);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_printf)(front_xqsaI_elements_t *s, const char *prefix);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_extract)(front_xqsaI_elements_t *src, front_xqsaI_slint_t nelements, front_xqsaI_elements_t *dst0, front_xqsaI_elements_t *dst1);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_touch)(front_xqsaI_elements_t *s);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_random_exchange)(front_xqsaI_elements_t *s, front_xqsaI_slint_t rounds, front_xqsaI_elements_t *xs);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_init_keys)(front_xqsaI_elements_t *s, front_xqsaI_slint_t dtype, front_xqsaI_slint_t _min, front_xqsaI_slint_t _max);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_init_keys2)(front_xqsaI_elements_t *s, front_xqsaI_slint_t dtype, front_xqsaI_slkey_pure_t key_min, front_xqsaI_slkey_pure_t key_max);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_keys_init)(front_xqsaI_elements_t *s, front_xqsaI_keys_init_type_t t, front_xqsaI_keys_init_data_t d);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_init_keys_from_file)(front_xqsaI_elements_t *s, front_xqsaI_slint_t data, char *filename, front_xqsaI_slint_t from, front_xqsaI_slint_t to, front_xqsaI_slint_t const_bytes_per_line);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_save_keys_to_file)(front_xqsaI_elements_t *s, char *filename);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_validate_order)(front_xqsaI_elements_t *s, front_xqsaI_slint_t n);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_validate_order_bmask)(front_xqsaI_elements_t *s, front_xqsaI_slint_t n, front_xqsaI_slkey_pure_t bmask);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_validate_order_weight)(front_xqsaI_elements_t *s, front_xqsaI_slint_t n, front_xqsaI_slkey_pure_t weight);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_keys_stats)(front_xqsaI_elements_t *s, front_xqsaI_slkey_pure_t *stats);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_print_keys)(front_xqsaI_elements_t *s);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_print_all)(front_xqsaI_elements_t *s);
front_xqsaI_slweight_t SL_PROTO(front_xqsaI_elements_get_weight)(front_xqsaI_elements_t *s);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_get_minmax_keys)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, front_xqsaI_slkey_pure_t *minmaxkeys);

/* src/core/elements_packed.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_alloc_packed)(front_xqsaI_packed_elements_t *s, front_xqsaI_slint_t nelements);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_free_packed)(front_xqsaI_packed_elements_t *s);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_pack_indexed)(front_xqsaI_elements_t *s, front_xqsaI_packed_elements_t *d, front_xqsaI_slindex_t *rindx, front_xqsaI_slindex_t *windx);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_pack)(front_xqsaI_elements_t *s, front_xqsaI_packed_elements_t *d);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_unpack_indexed)(front_xqsaI_packed_elements_t *s, front_xqsaI_elements_t *d, front_xqsaI_slindex_t *rindx, front_xqsaI_slindex_t *windx);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_unpack)(front_xqsaI_packed_elements_t *s, front_xqsaI_elements_t *d);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elements_unpack_keys)(front_xqsaI_packed_elements_t *s, front_xqsaI_slkey_t *k);

/* src/core/key2class.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_key2class_equal)(front_xqsaI_slkey_t *k, front_xqsaI_slint i, void *ci);
front_xqsaI_slint SL_PROTO(front_xqsaI_key2class_split)(front_xqsaI_slkey_t *k, front_xqsaI_slint i, void *ci);
front_xqsaI_slint SL_PROTO(front_xqsaI_key2class_split_keys)(front_xqsaI_slkey_t *k, front_xqsaI_slint i, void *ci);
front_xqsaI_slint SL_PROTO(front_xqsaI_key2class_random)(front_xqsaI_slkey_t *k, front_xqsaI_slint i, void *ci);
front_xqsaI_slint SL_PROTO(front_xqsaI_key2class_ci_nocounts)(front_xqsaI_slkey_t *k, front_xqsaI_slint i, void *ci);
front_xqsaI_slint SL_PROTO(front_xqsaI_key2class_ci)(front_xqsaI_slkey_t *k, front_xqsaI_slint i, void *ci);

/* src/core/merge2_basic.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_01_x)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx, front_xqsaI_m2x_func _x0_1, front_xqsaI_m2x_func _0x_1);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_01_X)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx, front_xqsaI_elements_t *t, front_xqsaI_m2X_func _X0_1, front_xqsaI_m2X_func _0X_1);

/* src/core/merge2_basic_auto.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_auto_01_x)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);

/* src/core/merge2_basic_sbin.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_sbin_x0_1)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_sbin_0x_1)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_sbin_01_x)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_sbin_01)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *t);

/* src/core/merge2_basic_shyb.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_shyb_x0_1)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_shyb_0x_1)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_shyb_01_x)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_shyb_01)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *t);

/* src/core/merge2_basic_sseq.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_sseq_x0_1)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_sseq_0x_1)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_sseq_01_x)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_sseq_01)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *t);

/* src/core/merge2_basic_straight.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_straight_x0_1)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_straight_0x_1)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_straight_01_x)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_straight_x_0_1)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_straight_X0_1)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx, front_xqsaI_elements_t *t);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_straight_0X_1)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx, front_xqsaI_elements_t *t);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_straight_01_X)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx, front_xqsaI_elements_t *t);
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_basic_straight_X0_1u)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx, front_xqsaI_elements_t *t);

/* src/core/front_xqsaI_merge2_compo_hula.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_compo_hula)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *xs);

/* src/core/front_xqsaI_merge2_compo_tridgell.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_compo_tridgell)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);

/* src/core/front_xqsaI_merge2_memory_adaptive.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_memory_adaptive)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx);

/* src/core/merge2_simplify.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_merge2_simplify_s1)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *sx, front_xqsaI_slint s1elements);

/* src/core/mergep_2way.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mergep_2way_ip_int)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slint_t p, int *displs, front_xqsaI_merge2x_f m2x);

/* src/core/mergep_heap.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mergep_heap_int)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *d, front_xqsaI_slint_t p, int *displs, int *counts);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mergep_heap_int_idx)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *d, front_xqsaI_slint_t p, int *displs, int *counts);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mergep_heap_idx)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *d, front_xqsaI_slint_t p, front_xqsaI_slindex_t *displs, front_xqsaI_slindex_t *counts);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mergep_heap_unpack_idx)(front_xqsaI_packed_elements_t *s, front_xqsaI_elements_t *d, front_xqsaI_slint_t p, front_xqsaI_slindex_t *displs, front_xqsaI_slindex_t *counts);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mergep_heap_unpack_idxonly)(front_xqsaI_packed_elements_t *s, front_xqsaI_elements_t *d, front_xqsaI_slint_t p, front_xqsaI_slindex_t *displs, front_xqsaI_slindex_t *counts);

/* src/core/search.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_sequential_lt)(front_xqsaI_elements_t *s, front_xqsaI_slkey_t *k);
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_sequential_le)(front_xqsaI_elements_t *s, front_xqsaI_slkey_t *k);
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_sequential_gt)(front_xqsaI_elements_t *s, front_xqsaI_slkey_t *k);
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_sequential_ge)(front_xqsaI_elements_t *s, front_xqsaI_slkey_t *k);
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_binary_lt)(front_xqsaI_elements_t *s, front_xqsaI_slkey_t *k);
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_binary_le)(front_xqsaI_elements_t *s, front_xqsaI_slkey_t *k);
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_binary_gt)(front_xqsaI_elements_t *s, front_xqsaI_slkey_t *k);
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_binary_ge)(front_xqsaI_elements_t *s, front_xqsaI_slkey_t *k);
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_binary_lt2)(front_xqsaI_elements_t *s, front_xqsaI_slkey_pure_t k);
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_binary_le2)(front_xqsaI_elements_t *s, front_xqsaI_slkey_pure_t k);
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_binary_gt2)(front_xqsaI_elements_t *s, front_xqsaI_slkey_pure_t k);
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_binary_ge2)(front_xqsaI_elements_t *s, front_xqsaI_slkey_pure_t k);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_sl_search_binary_lt_bmask)(front_xqsaI_elements_t *s, front_xqsaI_slkey_pure_t k, front_xqsaI_slkey_pure_t bmask);
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_hybrid_lt)(front_xqsaI_elements_t *s, front_xqsaI_slkey_t *k, front_xqsaI_slint t);
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_hybrid_le)(front_xqsaI_elements_t *s, front_xqsaI_slkey_t *k, front_xqsaI_slint t);
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_hybrid_gt)(front_xqsaI_elements_t *s, front_xqsaI_slkey_t *k, front_xqsaI_slint t);
front_xqsaI_slint SL_PROTO(front_xqsaI_sl_search_hybrid_ge)(front_xqsaI_elements_t *s, front_xqsaI_slkey_t *k, front_xqsaI_slint t);

/* src/core/sl_common.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_ilog2c)(front_xqsaI_slint x);
front_xqsaI_slint SL_PROTO(front_xqsaI_ilog2f)(front_xqsaI_slint x);
long long SL_PROTO(front_xqsaI_sl_random64)();
double SL_PROTO(front_xqsaI_sl_nrandom)();
double SL_PROTO(front_xqsaI_sl_urandom)();
front_xqsaI_slint SL_PROTO(front_xqsaI_print_bits)(front_xqsaI_slint v);
front_xqsaI_slint SL_PROTO(front_xqsaI_pivot_random)(front_xqsaI_elements_t *s);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_counts2displs)(front_xqsaI_slint_t n, int *counts, int *displs);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_displs2counts)(front_xqsaI_slint_t n, int *displs, int *counts, front_xqsaI_slint_t total_counts);

/* src/core/sl_elem.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elem_set_data)(front_xqsaI_elements_t *e, ...);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elem_reverse)(front_xqsaI_elements_t *e, front_xqsaI_elements_t *t);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elem_nxchange_at)(front_xqsaI_elements_t *e0, front_xqsaI_slint_t at0, front_xqsaI_elements_t *e1, front_xqsaI_slint_t at1, front_xqsaI_slint_t n, front_xqsaI_elements_t *t);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elem_nxchange)(front_xqsaI_elements_t *e0, front_xqsaI_elements_t *e1, front_xqsaI_slint_t n, front_xqsaI_elements_t *t);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elem_nxchange_ro0)(front_xqsaI_elements_t *e0, front_xqsaI_elements_t *e1, front_xqsaI_slint_t n, front_xqsaI_elements_t *t);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elem_rotate)(front_xqsaI_elements_t *e, front_xqsaI_slint_t m, front_xqsaI_slint_t n, front_xqsaI_elements_t *t);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elem_rotate_ro0)(front_xqsaI_elements_t *e, front_xqsaI_slint_t m, front_xqsaI_slint_t n, front_xqsaI_elements_t *t);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_elem_rotate_ro1)(front_xqsaI_elements_t *e, front_xqsaI_slint_t m, front_xqsaI_slint_t n, front_xqsaI_elements_t *t);

/* src/core/front_xqsaI_sort_counting.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_sort_counting_use_displs)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *d, front_xqsaI_slint_t ndispls, front_xqsaI_slint_t *displs);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_sort_counting_use_counts)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *d, front_xqsaI_slint_t ncounts, front_xqsaI_slint_t *counts);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_sort_counting_get_counts)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *d, front_xqsaI_slint_t ncounts, front_xqsaI_slint_t *counts);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_sort_counting)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *d, front_xqsaI_slint_t ncounts);

/* src/core/front_xqsaI_sort_heap.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_sort_heap)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *xs);

/* src/core/front_xqsaI_sort_insert.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_sort_insert_bmask_kernel)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slkey_pure_t bmask);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_sort_insert)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx);

/* src/core/sort_permute.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_sort_permute_forward)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slint *perm, front_xqsaI_slint offset, front_xqsaI_slint mask_bit);
front_xqsaI_slint SL_PROTO(front_xqsaI_sort_permute_backward)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slint *perm, front_xqsaI_slint offset, front_xqsaI_slint mask_bit);

/* src/core/front_xqsaI_sort_quick.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_sort_quick)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *xs);

/* src/core/front_xqsaI_sort_radix.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_sort_radix)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth);

/* src/core/front_xqsaI_sort_radix_1bit.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_sort_radix_1bit_kernel)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow);
front_xqsaI_slint SL_PROTO(front_xqsaI_sort_radix_1bit)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow);

/* src/core/front_xqsaI_sort_radix_af.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_sort_radix_af)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slint rhigh, front_xqsaI_slint rlow, front_xqsaI_slint rwidth);

/* src/core/front_xqsaI_sort_radix_db.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_sort_radix_db)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth);

/* src/core/front_xqsaI_sort_radix_ip.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_sort_radix_ip)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth);

/* src/core/front_xqsaI_sort_radix_iter.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_sort_radix_iter)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slint presorted, front_xqsaI_slint rhigh, front_xqsaI_slint rlow, front_xqsaI_slint rwidth);

/* src/core/front_xqsaI_sort_radix_ma.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_sort_radix_ma)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth);

/* src/core/sortnet.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_sn_hypercube_lh)(front_xqsaI_slint size, front_xqsaI_slint rank, front_xqsaI_slint stage, void *snp, front_xqsaI_slint *up);
front_xqsaI_slint SL_PROTO(front_xqsaI_sn_hypercube_hl)(front_xqsaI_slint size, front_xqsaI_slint rank, front_xqsaI_slint stage, void *snp, front_xqsaI_slint *up);
front_xqsaI_slint SL_PROTO(front_xqsaI_sn_odd_even_trans)(front_xqsaI_slint size, front_xqsaI_slint rank, front_xqsaI_slint stage, void *snp, front_xqsaI_slint *up);
front_xqsaI_slint SL_PROTO(front_xqsaI_sn_odd)(front_xqsaI_slint size, front_xqsaI_slint rank, front_xqsaI_slint stage, void *snp, front_xqsaI_slint *up);
front_xqsaI_slint SL_PROTO(front_xqsaI_sn_even)(front_xqsaI_slint size, front_xqsaI_slint rank, front_xqsaI_slint stage, void *snp, front_xqsaI_slint *up);
front_xqsaI_slint SL_PROTO(front_xqsaI_sn_batcher)(front_xqsaI_slint size, front_xqsaI_slint rank, front_xqsaI_slint stage, void *snp, front_xqsaI_slint *up);
front_xqsaI_slint SL_PROTO(front_xqsaI_sn_bitonic)(front_xqsaI_slint size, front_xqsaI_slint rank, front_xqsaI_slint stage, void *snp, front_xqsaI_slint *up);
front_xqsaI_slint SL_PROTO(front_xqsaI_sn_connected)(front_xqsaI_slint size, front_xqsaI_slint rank, front_xqsaI_slint stage, void *snp, front_xqsaI_slint *up);

/* src/core/splitter.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_splitter_reset)(front_xqsaI_splitter_t *sp);

/* src/core/splitx.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_splitx_radix)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slint_t nclasses, front_xqsaI_slint_t shl, front_xqsaI_slint_t *counts);
front_xqsaI_slint SL_PROTO(front_xqsaI_split2_lt_ge)(front_xqsaI_elements_t *s, front_xqsaI_slkey_pure_t *k, front_xqsaI_elements_t *t);
front_xqsaI_slint SL_PROTO(front_xqsaI_split2_le_gt)(front_xqsaI_elements_t *s, front_xqsaI_slkey_pure_t *k, front_xqsaI_elements_t *t);
front_xqsaI_slint SL_PROTO(front_xqsaI_split3_lt_eq_gt)(front_xqsaI_elements_t *s, front_xqsaI_slkey_pure_t *k, front_xqsaI_elements_t *t, front_xqsaI_slint *nlt, front_xqsaI_slint *nle);
front_xqsaI_slint SL_PROTO(front_xqsaI_split3_lt_eq_gt_old)(front_xqsaI_elements_t *s, front_xqsaI_slkey_pure_t *k, front_xqsaI_elements_t *t, front_xqsaI_slint *nlt, front_xqsaI_slint *nle);
front_xqsaI_slint SL_PROTO(front_xqsaI_split2_b)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slkey_pure_t bmask);
front_xqsaI_slint SL_PROTO(front_xqsaI_splitk_k2c_af)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slint k, front_xqsaI_slint *c, front_xqsaI_k2c_func k2c, void *k2c_data);
front_xqsaI_slint SL_PROTO(front_xqsaI_splitk_k2c)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_slint k, front_xqsaI_slint *c, front_xqsaI_k2c_func k2c, void *k2c_data);
front_xqsaI_slint SL_PROTO(front_xqsaI_splitk_k2c_count)(front_xqsaI_elements_t *s, front_xqsaI_slint k, front_xqsaI_slint *c, front_xqsaI_k2c_func k2c, void *k2c_data);


#ifdef SL_USE_MPI

/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_protos_mpi.h
 *  timestamp: 2011-01-13 10:37:23 +0100
 *  
 */




/* src/core_mpi/mpi_binning.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_binning_create)(front_xqsaI_global_bins_t *gb, front_xqsaI_slint_t max_nbins, front_xqsaI_slint_t max_nbinnings, front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, front_xqsaI_slint_t doweights, front_xqsaI_binning_t *bm, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_binning_destroy)(front_xqsaI_global_bins_t *gb, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_binning_pre)(front_xqsaI_global_bins_t *gb, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_binning_exec_reset)(front_xqsaI_global_bins_t *gb, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_binning_exec_local)(front_xqsaI_global_bins_t *gb, front_xqsaI_slint_t b, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_binning_exec_global)(front_xqsaI_global_bins_t *gb, front_xqsaI_slint_t root, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_binning_refine)(front_xqsaI_global_bins_t *gb, front_xqsaI_slint_t b, front_xqsaI_slint_t k, front_xqsaI_splitter_t *sp, front_xqsaI_slint_t s, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_binning_hit)(front_xqsaI_global_bins_t *gb, front_xqsaI_slint_t b, front_xqsaI_slint_t k, front_xqsaI_splitter_t *sp, front_xqsaI_slint_t s, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_binning_finalize)(front_xqsaI_global_bins_t *gb, front_xqsaI_slint_t b, front_xqsaI_slweight_t dcw, front_xqsaI_slint_t lc_min, front_xqsaI_slint_t lc_max, front_xqsaI_slweight_t *lcw, front_xqsaI_splitter_t *sp, front_xqsaI_slint_t s, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_binning_post)(front_xqsaI_global_bins_t *gb, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_common.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_datatypes_init)();
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_datatypes_release)();
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_get_grid)(front_xqsaI_slint_t ndims, front_xqsaI_slint_t *dims, front_xqsaI_slint_t *pos, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_subgroups_create)(front_xqsaI_slint_t nsubgroups, MPI_Comm *sub_comms, int *sub_sizes, int *sub_ranks, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_subgroups_delete)(front_xqsaI_slint_t nsubgroups, MPI_Comm *sub_comms, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_elements.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_mpi_elements_init_keys_from_file)(front_xqsaI_elements_t *s, char *filename, front_xqsaI_slint from, front_xqsaI_slint to, front_xqsaI_slint const_bytes_per_line, front_xqsaI_slint root, int size, int rank, MPI_Comm comm);
front_xqsaI_slint SL_PROTO(front_xqsaI_mpi_elements_validate_order)(front_xqsaI_elements_t *s, front_xqsaI_slint n, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_linear_exchange_pure_keys)(front_xqsaI_slkey_pure_t *in, front_xqsaI_slkey_pure_t *out, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_elements_check_order)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, front_xqsaI_slint_t *orders, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_check_global_order)(front_xqsaI_slkey_pure_t local_min, front_xqsaI_slkey_pure_t local_max, int root, int size, int rank, MPI_Comm comm);
unsigned short SL_PROTO(front_xqsaI_mpi_cs16)(front_xqsaI_elements_t *s, front_xqsaI_slint n, front_xqsaI_slint keys, front_xqsaI_slint data, int size, int rank, MPI_Comm comm);
unsigned int SL_PROTO(front_xqsaI_mpi_cs32)(front_xqsaI_elements_t *s, front_xqsaI_slint n, front_xqsaI_slint keys, front_xqsaI_slint data, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_elements_get_counts)(front_xqsaI_elements_t *s, front_xqsaI_slint_t *clocal, front_xqsaI_slint_t *cglobal, int root, int size, int rank, MPI_Comm comm);
front_xqsaI_slweight_t SL_PROTO(front_xqsaI_mpi_elements_get_weights)(front_xqsaI_elements_t *s, front_xqsaI_slweight_t *wlocal, front_xqsaI_slweight_t *wglobal, int root, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_elements_get_counts_and_weights)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, front_xqsaI_slint_t *counts, front_xqsaI_slweight_t *weights, int root, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_elements_sendrecv_replace)(front_xqsaI_elements_t *s, int count, int dest, int sendtag, int source, int recvtag, int size, int rank, MPI_Comm comm);
unsigned int SL_PROTO(front_xqsaI_mpi_elements_crc32)(front_xqsaI_elements_t *s, front_xqsaI_slint_t n, front_xqsaI_slint_t keys, front_xqsaI_slint_t data, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_elements_alltoallv.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_elements_alltoallv_db)(front_xqsaI_elements_t *sbuf, int *scounts, int *sdispls, front_xqsaI_elements_t *rbuf, int *rcounts, int *rdispls, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_elements_alltoallv_ip)(front_xqsaI_elements_t *sbuf, front_xqsaI_elements_t *sx, int *scounts, int *sdispls, int *rcounts, int *rdispls, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_elements_packed.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_elements_packed_datatype_create)(MPI_Datatype *pdt, front_xqsaI_slint_t structured);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_elements_packed_datatype_destroy)(MPI_Datatype *pdt);

/* src/core_mpi/front_xqsaI_mpi_find_exact.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_find_exact_equal)(front_xqsaI_elements_t *s, front_xqsaI_slint_t other_rank, front_xqsaI_slint_t high_rank, front_xqsaI_slint_t *ex_start, front_xqsaI_slint_t *ex_size, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_find_exact)(front_xqsaI_elements_t *s, front_xqsaI_slint_t other_rank, front_xqsaI_slint_t high_rank, front_xqsaI_slint_t *dst_size, front_xqsaI_slint_t *ex_start, front_xqsaI_slint_t *ex_sizes, front_xqsaI_slint_t *nx_move, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_linsplit.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_linsplit)(MPI_Comm comm_in, front_xqsaI_slkey_pure_t *keys_in, MPI_Comm *comms_out, front_xqsaI_slint_t *parity, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_linsplit2)(MPI_Comm comm_in, front_xqsaI_slkey_pure_t *keys_in, MPI_Comm *comms_out, front_xqsaI_slint_t *parity, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_merge2.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_merge2)(front_xqsaI_elements_t *s, front_xqsaI_slint_t other_rank, front_xqsaI_slint_t high_rank, front_xqsaI_slint_t *dst_size, front_xqsaI_merge2x_f m2, front_xqsaI_elements_t *xs, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_mergek.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_mergek_equal)(front_xqsaI_elements_t *s, front_xqsaI_sortnet_f sn, front_xqsaI_sortnet_data_t snd, front_xqsaI_merge2x_f m2x, front_xqsaI_elements_t *xs, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_mergek)(front_xqsaI_elements_t *s, front_xqsaI_sortnet_f sn, front_xqsaI_sortnet_data_t snd, front_xqsaI_merge2x_f m2x, front_xqsaI_elements_t *xs, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_mergek_equal2)(front_xqsaI_elements_t *s, front_xqsaI_sortnet_f sn, front_xqsaI_sortnet_data_t snd, front_xqsaI_merge2x_f m2x, front_xqsaI_elements_t *xs, int *sizes, int *ranks, MPI_Comm *comms);

/* src/core_mpi/mpi_mergek_presort.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_mergek_presort_radix)(front_xqsaI_mergek_t *mk, front_xqsaI_slint_t rmerge, front_xqsaI_elements_t *s_in, MPI_Comm *comms_in, front_xqsaI_slint_t parity_in, front_xqsaI_elements_t *s_out, MPI_Comm *comms_out, front_xqsaI_slint_t *parity_out, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_partition_exact_generic.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_partition_exact_generic)(front_xqsaI_elements_t *s, front_xqsaI_partcond_t *pcond, front_xqsaI_binning_t *bm, int *scounts, int *rcounts, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_partition_exact_generic2)(front_xqsaI_elements_t *s, front_xqsaI_partcond_t *pcond, front_xqsaI_binning_t *bm, int *scounts, int *rcounts, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_partition_exact_radix.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_partition_exact_radix)(front_xqsaI_elements_t *s, front_xqsaI_partcond_t *pcond, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth, front_xqsaI_slint_t sorted, int *scounts, int *rcounts, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_partition_exact_radix_grouped.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_partition_exact_radix_ngroups)(front_xqsaI_elements_t *s, front_xqsaI_partcond_t *pcond, front_xqsaI_slint_t ngroups, MPI_Comm *group_comms, front_xqsaI_elements_t *sx, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth, int *scounts, int *rcounts, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_partition_exact_radix_2groups)(front_xqsaI_elements_t *s, front_xqsaI_partcond_t *pcond, MPI_Comm group_comm, front_xqsaI_elements_t *sx, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth, int *scounts, int *rcounts, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_partition_joink.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_mpi_partition_joink)(front_xqsaI_elements_t *s, front_xqsaI_slint *sizes, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_partition_radix2.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_partition_radix2)(front_xqsaI_elements_t *s, front_xqsaI_partcond2_t *pc, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth, int *scounts, int *sdispls, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_partition_radix_old.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_partition_radix_old)(front_xqsaI_elements_t *s, front_xqsaI_partcond2_t *pc, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth, int *scounts, int *sdispls, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_partition_sample.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_partition_sample_regular)(front_xqsaI_elements_t *s, front_xqsaI_partcond_t *pcond, int *scounts, int *rcounts, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_rebalance.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_rebalance)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_slint_t stable, front_xqsaI_slint_t *dst_size, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_rebalance_alltoallv)(front_xqsaI_elements_t *sbuf, int *scounts, int *sdispls, front_xqsaI_elements_t *rbuf, int *rcounts, int *rdispls, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_redistribute_exact_generic.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_redistribute_exact_generic)(front_xqsaI_elements_t *sin, front_xqsaI_elements_t *sout, front_xqsaI_partcond_t *pcond, front_xqsaI_parttype_t *ptype, front_xqsaI_binning_t *bm, int *rcounts, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_select_common.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_init_partconds)(front_xqsaI_slint_t npconds, front_xqsaI_partcond_t *pconds, front_xqsaI_slint_t nparts, front_xqsaI_slint_t total_count, front_xqsaI_slweight_t total_weight);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_init_partconds_intern)(front_xqsaI_slint_t npconds, front_xqsaI_partcond_intern_t *pci, front_xqsaI_partcond_t *pc, front_xqsaI_slint_t nparts, front_xqsaI_slint_t total_count, front_xqsaI_slweight_t total_weight);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_merge_partconds)(front_xqsaI_partcond_t *pconds_in, front_xqsaI_slint_t npconds_in, front_xqsaI_partcond_t *pcond_out);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_gather_partconds_grouped)(front_xqsaI_partcond_t *pcond_in, MPI_Comm pcond_in_comm, MPI_Comm pconds_out_comm, front_xqsaI_partcond_t *pconds_out, front_xqsaI_slint_t *npconds_out, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_gather_partconds)(front_xqsaI_partcond_t *pcond_in, front_xqsaI_partcond_t *pconds_out, int root, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_allgather_partconds)(front_xqsaI_partcond_t *pcond_in, front_xqsaI_partcond_t *pconds_out, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_bcast_partconds)(front_xqsaI_slint_t npconds, front_xqsaI_partcond_t *pconds, int root, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_post_check_partconds)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, front_xqsaI_slint_t nparts, front_xqsaI_partcond_t *pconds, int *sdispls, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_post_check_partconds_intern)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, front_xqsaI_slint_t nparts, front_xqsaI_partcond_intern_t *pci, int *sdispls, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_select_stats)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nparts, int *sdispls, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_select_exact_generic.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_select_exact_generic_bulk)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, front_xqsaI_slint_t nparts, front_xqsaI_partcond_t *pconds, front_xqsaI_binning_t *bm, front_xqsaI_splitter_t *sp, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_select_exact_generic_grouped)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, front_xqsaI_partcond_t *pcond, MPI_Comm pcond_comm, MPI_Comm group_comm, front_xqsaI_binning_t *bm, front_xqsaI_splitter_t *sp, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_select_exact_generic)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, front_xqsaI_slint_t nparts, front_xqsaI_partcond_t *pconds, front_xqsaI_binning_t *bm, front_xqsaI_splitter_t *sp, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_select_exact_radix.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_select_exact_radix)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, front_xqsaI_slint_t nparts, front_xqsaI_partcond_t *pconds, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth, front_xqsaI_slint_t sorted, int *sdispls, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_select_exact_radix_fixed)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, front_xqsaI_slint_t nparts, front_xqsaI_partcond_t *pconds, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth, int *sdispls, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_select_exact_radix_grouped)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nelements, front_xqsaI_partcond_t *pcond, MPI_Comm pcond_comm, MPI_Comm group_comm, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth, front_xqsaI_slint_t sorted, int *sdispls, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_select_qs.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_mpi_select_qs)(front_xqsaI_elements_t *s, front_xqsaI_slint n, front_xqsaI_slint *iths, front_xqsaI_pivot_func pi, front_xqsaI_slint threshold, front_xqsaI_slkey_pure_t *e, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_select_radix2.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_select_radix2)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nparts, front_xqsaI_partcond_t *pconds, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth, int *sdispls, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_select_stats2)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nparts, int *sdispls, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_select_radix_pg2)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nparts, front_xqsaI_partcond_t *pcond, MPI_Comm pcomm, MPI_Comm mcomm, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth, int *sdispls, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_select_sample.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_select_sample_regular)(front_xqsaI_elements_t *s, front_xqsaI_slint_t nparts, front_xqsaI_partcond_t *pconds, front_xqsaI_slint_t nsamples, front_xqsaI_splitter_t *sp, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_sort_merge.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_sort_merge)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *xs, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_sort_merge2)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *xs, front_xqsaI_slint_t merge_type, front_xqsaI_slint_t sort_type, double *times, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_sort_partition.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_sort_partition)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *xs, front_xqsaI_slint_t part_type, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_sort_partition_radix)(front_xqsaI_elements_t *s0, front_xqsaI_elements_t *s1, front_xqsaI_elements_t *xs, front_xqsaI_slint_t part_type, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_sort_partition_exact_radix)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_partcond_t *pcond, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_sort_partition_exact_radix_ngroups)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_partcond_t *pcond, front_xqsaI_slint_t ngroups, MPI_Comm *group_comms, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_sort_partition_exact_radix_2groups)(front_xqsaI_elements_t *s, front_xqsaI_elements_t *sx, front_xqsaI_partcond_t *pcond, MPI_Comm group_comm, front_xqsaI_slint_t rhigh, front_xqsaI_slint_t rlow, front_xqsaI_slint_t rwidth, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_splitk.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_mpi_splitk)(front_xqsaI_elements_t *s, front_xqsaI_k2c_func k2c, void *ci, front_xqsaI_elements_t *sx, front_xqsaI_elements_t *sa, front_xqsaI_slint *nne, front_xqsaI_slint *nue, int size, int rank, MPI_Comm comm);

/* src/core_mpi/front_xqsaI_mpi_splitk_dummy.c */
front_xqsaI_slint SL_PROTO(front_xqsaI_mpi_splitk_dummy)(front_xqsaI_elements_t *s, front_xqsaI_k2c_func k2c, void *ci, front_xqsaI_elements_t *sx, front_xqsaI_slint *send_stats, int size, int rank, MPI_Comm comm);

/* src/core_mpi/mpi_xcounts2ycounts.c */
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_xcounts2ycounts_all2all)(int *xcounts, int *ycounts, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_xcounts2ycounts_sparse)(int *xcounts, int *ycounts, front_xqsaI_slint_t ytotal, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_xcounts2ycounts_grouped)(int *xcounts, front_xqsaI_slint_t nxcounts, int *ycounts, MPI_Comm group_comm, MPI_Comm master_comm, int size, int rank, MPI_Comm comm);
front_xqsaI_slint_t SL_PROTO(front_xqsaI_mpi_subxdispls2ycounts)(front_xqsaI_slint_t nsubs, int *sub_xdispls, front_xqsaI_slint_t *sub_sources, front_xqsaI_slint_t *sub_sizes, MPI_Comm sub_comm, int sub_size, int *ycounts, int size, int rank, MPI_Comm comm);


#endif /* SL_USE_MPI */


#undef SL_PROTO
#endif /* __SL_FRONT_XQSAI_H__ */
