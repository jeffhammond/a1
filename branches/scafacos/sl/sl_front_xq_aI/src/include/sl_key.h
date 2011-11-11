/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_key.h
 *  timestamp: 2010-10-25 10:07:22 +0200
 *  
 */


#ifndef __SL_KEY_H__
#define __SL_KEY_H__


/* sl_macro sl_key_type_c sl_key_type_mpi sl_key_size_mpi sl_key_type_fmt sl_key_integer sl_key_memcpy */


#define sl_key_byte                          ((slint_t) sizeof(sl_key_type_c))  /* sl_macro */

#ifndef sl_key_copy
 #ifndef sl_key_memcpy
  #define sl_key_copy(src, dst)              ARRAY1_COPY(src, dst)
 #else
  #define sl_key_copy(src, dst)              memcpy(dst, src, sl_key_byte)  /* sl_macro */
 #endif
#endif
#ifndef sl_key_ncopy
 #define sl_key_ncopy(src, dst, n)           memcpy(dst, src, (n) * sl_key_byte)  /* sl_macro */
#endif
#ifndef sl_key_nmove
 #define sl_key_nmove(src, dst, n)           memmove(dst, src, (n) * sl_key_byte)  /* sl_macro */
#endif


#define key_type_c                           sl_key_type_c
#define key_type_mpi                         (sl_key_type_mpi)
#define key_size_mpi                         (sl_key_size_mpi)
#ifdef sl_key_type_fmt
# define key_type_fmt                        sl_key_type_fmt
#endif
#define key_integer                          sl_key_integer

#define key_pure_type_c                      sl_key_pure_type_c
#define key_pure_type_mpi                    (sl_key_pure_type_mpi)
#define key_pure_size_mpi                    (sl_key_pure_size_mpi)
#ifdef sl_key_pure_type_fmt
# define key_pure_type_fmt                   sl_key_pure_type_fmt
#endif

#define key_purify(k)                        (sl_key_purify(k))
#define key_get_pure(k)                      (sl_key_get_pure(k))
#define key_set_pure(k, p)                   (sl_key_set_pure(k, p))


#define key_n                                1
#define key_byte                             (sl_key_byte)

#define key_cmp_eq(k0, k1)                   (cc_rti_cadd_cmp(1) sl_key_cmp_eq((k0), (k1)))
#define key_cmp_ne(k0, k1)                   (cc_rti_cadd_cmp(1) sl_key_cmp_ne((k0), (k1)))
#define key_cmp_lt(k0, k1)                   (cc_rti_cadd_cmp(1) sl_key_cmp_lt((k0), (k1)))
#define key_cmp_le(k0, k1)                   (cc_rti_cadd_cmp(1) sl_key_cmp_le((k0), (k1)))
#define key_cmp_gt(k0, k1)                   (cc_rti_cadd_cmp(1) sl_key_cmp_gt((k0), (k1)))
#define key_cmp_ge(k0, k1)                   (cc_rti_cadd_cmp(1) sl_key_cmp_ge((k0), (k1)))

#define key_pure_cmp_eq(k0, k1)              (cc_rti_cadd_cmp(1) sl_key_pure_cmp_eq((k0), (k1)))
#define key_pure_cmp_ne(k0, k1)              (cc_rti_cadd_cmp(1) sl_key_pure_cmp_ne((k0), (k1)))
#define key_pure_cmp_lt(k0, k1)              (cc_rti_cadd_cmp(1) sl_key_pure_cmp_lt((k0), (k1)))
#define key_pure_cmp_le(k0, k1)              (cc_rti_cadd_cmp(1) sl_key_pure_cmp_le((k0), (k1)))
#define key_pure_cmp_gt(k0, k1)              (cc_rti_cadd_cmp(1) sl_key_pure_cmp_gt((k0), (k1)))
#define key_pure_cmp_ge(k0, k1)              (cc_rti_cadd_cmp(1) sl_key_pure_cmp_ge((k0), (k1)))

#ifdef sl_key_val_srand
# define key_val_srand(_s_)                  sl_key_val_srand(_s_)
#endif
#ifdef sl_key_val_rand
# define key_val_rand()                      sl_key_val_rand()
#endif
#ifdef sl_key_val_rand_minmax
# define key_val_rand_minmax(_min_, _max_)   sl_key_val_rand_minmax(_min_, _max_)
#endif


#define key_at(_s_, _sat_)                   ((_s_) + (_sat_))

#define key_assign(src, dst)                 (dst = src)
#define key_assign_at(src, sat, dst)         (dst = &src[sat])
#define key_null(k)                          (k = NULL)
#define key_inc(k)                           (++k)
#define key_dec(k)                           (--k)
#define key_add(k, n)                        (k += n)
#define key_sub(k, n)                        (k -= n)

#define key_copy(src, dst)                   (cc_rti_cadd_movek(1) sl_key_copy(src, dst))
#define key_ncopy(src, dst, n)               (cc_rti_cadd_movek(n) sl_key_ncopy(src, dst, n))
#define key_nmove(src, dst, n)               (cc_rti_cadd_movek(n) sl_key_nmove(src, dst, n))

#define key_copy_at(src, sat, dst, dat)      key_copy(&(src)[sat], &(dst)[dat])
#define key_ncopy_at(src, sat, dst, dat, n)  key_ncopy(&(src)[sat], &(dst)[dat], n)
#define key_nmove_at(src, sat, dst, dat, n)  key_nmove(&(src)[sat], &(dst)[dat], n)

#define key_xchange(k0, k1, t)               (key_copy(k0, t), key_copy(k1, k0), key_copy(t, k1))
#define key_xchange_at(k0, at0, k1, at1, t)  (key_copy_at(k0, at0, t, 0), key_copy_at(k1, at1, k0, at0), key_copy_at(t, 0, k1, at1))

#define key_cm                               SLCM_KEYS

#ifdef key_integer
# define key_radix_low                       0
# define key_radix_high                      (sizeof(key_pure_type_c) * 8 - 1)
# define key_radix_key2class(_k_, _x_, _y_)  (((_k_) >> (_x_)) & (_y_))
#endif



#endif /* __SL_KEY_H__ */
