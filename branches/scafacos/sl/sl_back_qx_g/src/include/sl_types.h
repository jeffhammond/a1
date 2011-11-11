/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_types.h
 *  timestamp: 2011-01-12 09:59:11 +0100
 *  
 */


#ifndef __SL_TYPES_H__
#define __SL_TYPES_H__


/* sl_type slint_t slint */
typedef sl_int_type_c slint_t, slint;  /* deprecated 'slint' */

#define slint_fmt   sl_int_type_fmt    /* sl_macro */

/* sl_type slindex_t */
typedef sl_index_type_c slindex_t;

#define sindex_fmt  sl_index_type_fmt  /* sl_macro */

/* sl_type slkey_t */
typedef sl_key_type_c slkey_t;

/* sl_type slkey_pure_t slpkey_t */
typedef sl_key_pure_type_c slkey_pure_t, slpkey_t;

/* DATAX_TEMPLATE_BEGIN */
/* sl_type sldata0_t */
#ifdef sl_data0_type_c
typedef sl_data0_type_c sldata0_t;
#endif
/* sl_type sldata1_t */
#ifdef sl_data1_type_c
typedef sl_data1_type_c sldata1_t;
#endif
/* sl_type sldata2_t */
#ifdef sl_data2_type_c
typedef sl_data2_type_c sldata2_t;
#endif
/* sl_type sldata3_t */
#ifdef sl_data3_type_c
typedef sl_data3_type_c sldata3_t;
#endif
/* sl_type sldata4_t */
#ifdef sl_data4_type_c
typedef sl_data4_type_c sldata4_t;
#endif
/* sl_type sldata5_t */
#ifdef sl_data5_type_c
typedef sl_data5_type_c sldata5_t;
#endif
/* sl_type sldata6_t */
#ifdef sl_data6_type_c
typedef sl_data6_type_c sldata6_t;
#endif
/* sl_type sldata7_t */
#ifdef sl_data7_type_c
typedef sl_data7_type_c sldata7_t;
#endif
/* sl_type sldata8_t */
#ifdef sl_data8_type_c
typedef sl_data8_type_c sldata8_t;
#endif
/* sl_type sldata9_t */
#ifdef sl_data9_type_c
typedef sl_data9_type_c sldata9_t;
#endif
/* sl_type sldata10_t */
#ifdef sl_data10_type_c
typedef sl_data10_type_c sldata10_t;
#endif
/* sl_type sldata11_t */
#ifdef sl_data11_type_c
typedef sl_data11_type_c sldata11_t;
#endif
/* sl_type sldata12_t */
#ifdef sl_data12_type_c
typedef sl_data12_type_c sldata12_t;
#endif
/* sl_type sldata13_t */
#ifdef sl_data13_type_c
typedef sl_data13_type_c sldata13_t;
#endif
/* sl_type sldata14_t */
#ifdef sl_data14_type_c
typedef sl_data14_type_c sldata14_t;
#endif
/* sl_type sldata15_t */
#ifdef sl_data15_type_c
typedef sl_data15_type_c sldata15_t;
#endif
/* sl_type sldata16_t */
#ifdef sl_data16_type_c
typedef sl_data16_type_c sldata16_t;
#endif
/* sl_type sldata17_t */
#ifdef sl_data17_type_c
typedef sl_data17_type_c sldata17_t;
#endif
/* sl_type sldata18_t */
#ifdef sl_data18_type_c
typedef sl_data18_type_c sldata18_t;
#endif
/* sl_type sldata19_t */
#ifdef sl_data19_type_c
typedef sl_data19_type_c sldata19_t;
#endif
/* DATAX_TEMPLATE_END */

/* sl_type slweight_t */
typedef sl_weight_type_c slweight_t;

#define slweight_fmt  sl_weight_type_fmt  /* sl_macro */

/* sl_type _slpwkey_t slpwkey_t */
typedef struct _slpwkey_t
{
  slpkey_t pkey;
  slweight_t weight;

} slpwkey_t;


/* sl_type _elements_t elements_t */
typedef struct _elements_t
{
  slint_t size, max_size;
  slkey_t *keys;

#ifdef SL_INDEX
  slindex_t *indices;
#endif

/* DATAX_TEMPLATE_BEGIN */
#ifdef SL_DATA0
  sldata0_t *data0;
#endif
#ifdef SL_DATA1
  sldata1_t *data1;
#endif
#ifdef SL_DATA2
  sldata2_t *data2;
#endif
#ifdef SL_DATA3
  sldata3_t *data3;
#endif
#ifdef SL_DATA4
  sldata4_t *data4;
#endif
#ifdef SL_DATA5
  sldata5_t *data5;
#endif
#ifdef SL_DATA6
  sldata6_t *data6;
#endif
#ifdef SL_DATA7
  sldata7_t *data7;
#endif
#ifdef SL_DATA8
  sldata8_t *data8;
#endif
#ifdef SL_DATA9
  sldata9_t *data9;
#endif
#ifdef SL_DATA10
  sldata10_t *data10;
#endif
#ifdef SL_DATA11
  sldata11_t *data11;
#endif
#ifdef SL_DATA12
  sldata12_t *data12;
#endif
#ifdef SL_DATA13
  sldata13_t *data13;
#endif
#ifdef SL_DATA14
  sldata14_t *data14;
#endif
#ifdef SL_DATA15
  sldata15_t *data15;
#endif
#ifdef SL_DATA16
  sldata16_t *data16;
#endif
#ifdef SL_DATA17
  sldata17_t *data17;
#endif
#ifdef SL_DATA18
  sldata18_t *data18;
#endif
#ifdef SL_DATA19
  sldata19_t *data19;
#endif
/* DATAX_TEMPLATE_END */

} elements_t;


/* sl_type _packed_element_t packed_element_t */
typedef struct _packed_element_t
{
  slkey_t key;

#ifdef SL_PACKED_INDEX
  slindex_t index;
#endif

/* DATAX_TEMPLATE_BEGIN */
#ifdef SL_DATA0
# ifdef sl_data0_flex
  sldata0_t data0[];
# else
  sldata0_t data0[sl_data0_size_c];
# endif
#endif
#ifdef SL_DATA1
# ifdef sl_data1_flex
  sldata1_t data1[];
# else
  sldata1_t data1[sl_data1_size_c];
# endif
#endif
#ifdef SL_DATA2
# ifdef sl_data2_flex
  sldata2_t data2[];
# else
  sldata2_t data2[sl_data2_size_c];
# endif
#endif
#ifdef SL_DATA3
# ifdef sl_data3_flex
  sldata3_t data3[];
# else
  sldata3_t data3[sl_data3_size_c];
# endif
#endif
#ifdef SL_DATA4
# ifdef sl_data4_flex
  sldata4_t data4[];
# else
  sldata4_t data4[sl_data4_size_c];
# endif
#endif
#ifdef SL_DATA5
# ifdef sl_data5_flex
  sldata5_t data5[];
# else
  sldata5_t data5[sl_data5_size_c];
# endif
#endif
#ifdef SL_DATA6
# ifdef sl_data6_flex
  sldata6_t data6[];
# else
  sldata6_t data6[sl_data6_size_c];
# endif
#endif
#ifdef SL_DATA7
# ifdef sl_data7_flex
  sldata7_t data7[];
# else
  sldata7_t data7[sl_data7_size_c];
# endif
#endif
#ifdef SL_DATA8
# ifdef sl_data8_flex
  sldata8_t data8[];
# else
  sldata8_t data8[sl_data8_size_c];
# endif
#endif
#ifdef SL_DATA9
# ifdef sl_data9_flex
  sldata9_t data9[];
# else
  sldata9_t data9[sl_data9_size_c];
# endif
#endif
#ifdef SL_DATA10
# ifdef sl_data10_flex
  sldata10_t data10[];
# else
  sldata10_t data10[sl_data10_size_c];
# endif
#endif
#ifdef SL_DATA11
# ifdef sl_data11_flex
  sldata11_t data11[];
# else
  sldata11_t data11[sl_data11_size_c];
# endif
#endif
#ifdef SL_DATA12
# ifdef sl_data12_flex
  sldata12_t data12[];
# else
  sldata12_t data12[sl_data12_size_c];
# endif
#endif
#ifdef SL_DATA13
# ifdef sl_data13_flex
  sldata13_t data13[];
# else
  sldata13_t data13[sl_data13_size_c];
# endif
#endif
#ifdef SL_DATA14
# ifdef sl_data14_flex
  sldata14_t data14[];
# else
  sldata14_t data14[sl_data14_size_c];
# endif
#endif
#ifdef SL_DATA15
# ifdef sl_data15_flex
  sldata15_t data15[];
# else
  sldata15_t data15[sl_data15_size_c];
# endif
#endif
#ifdef SL_DATA16
# ifdef sl_data16_flex
  sldata16_t data16[];
# else
  sldata16_t data16[sl_data16_size_c];
# endif
#endif
#ifdef SL_DATA17
# ifdef sl_data17_flex
  sldata17_t data17[];
# else
  sldata17_t data17[sl_data17_size_c];
# endif
#endif
#ifdef SL_DATA18
# ifdef sl_data18_flex
  sldata18_t data18[];
# else
  sldata18_t data18[sl_data18_size_c];
# endif
#endif
#ifdef SL_DATA19
# ifdef sl_data19_flex
  sldata19_t data19[];
# else
  sldata19_t data19[sl_data19_size_c];
# endif
#endif
/* DATAX_TEMPLATE_END */

} packed_element_t;


/* sl_type _packed_elements_t packed_elements_t */
typedef struct _packed_elements_t
{
  slint_t size, max_size;
  
  packed_element_t *elements;
  
} packed_elements_t;


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


/* sl_type _classification_info_t classification_info_t classification_info */
typedef struct _classification_info_t
{
  slint_t nclasses;
  slkey_pure_t *keys;
  slint_t *counts;
  slint_t *masks;

  /* */
  slint_t *all_local_sizes;
  slint_t *local_lt_eq_counts;
  slint_t *all_local_lt_eq_counts;

} classification_info_t, classification_info;  /* deprecated 'classification_info' */


/* key2class, sl_type key2class_f */
typedef slint_t (*key2class_f)(slkey_t *, slint, void *);

/* pivot-element, sl_type pivot_f */
typedef slint_t (*pivot_f)(elements_t *);

/* sorting-network, sl_type sortnet_f sortnet_data_t */
typedef void *sortnet_data_t;
typedef slint_t (*sortnet_f)(slint_t size, slint_t rank, slint_t stage, sortnet_data_t snd, slint_t *up);

/* merge2, sl_type merge2x_f merge2X_f */
typedef slint_t (*merge2x_f)(elements_t *s0, elements_t *s1, elements_t *sx);
typedef slint_t (*merge2X_f)(elements_t *s0, elements_t *s1, elements_t *sx, elements_t *t);


/* deprecated, sl_type k2c_func pivot_func sn_func m2x_func m2X_func */
typedef key2class_f k2c_func;
typedef pivot_f pivot_func;
typedef sortnet_f sn_func;
typedef merge2x_f m2x_func;
typedef merge2X_f m2X_func;


/* sl_type _mergek_t mergek_t */
typedef struct _mergek_t
{
  sortnet_f sn;
  sortnet_data_t snd;

  merge2x_f m2x;
  elements_t *sx;

} mergek_t;


/* sl_type keys_init_type_t keys_init_data_t */
typedef slint_t keys_init_type_t;
typedef void *keys_init_data_t;

/* sl_type key_set_data_t key_set_f */
typedef void *key_set_data_t;
typedef void (*key_set_f)(slkey_pure_t *k, key_set_data_t d);


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


/* elements_keys_stats */
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


/* partition conditions, sl_type _partcond2_t partcond2_t */
typedef struct _partcond2_t
{
  int weighted;
  double min_count, max_count;
  double min_weight, max_weight;
  double min_cpart, max_cpart;
  double min_wpart, max_wpart;

} partcond2_t;


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

/* partition conditions, sl_type _partcond_t partcond_t partcond_p */
typedef struct _partcond_t
{
  slint_t pcm;
  double count_min, count_max;
  double count_low, count_high;
  double weight_min, weight_max;
  double weight_low, weight_high;

} partcond_t, *partcond_p;


/* internal partition conditions, sl_type _partcond_intern_t partcond_intern_t partcond_intern_p */
typedef struct _partcond_intern_t
{
  slint_t pcm;
  slint_t count_min, count_max;
  slint_t count_low, count_high;
#ifdef elem_weight
  double weight_min, weight_max;
  double weight_low, weight_high;
#endif

} partcond_intern_t, *partcond_intern_p;


/* sl_type _parttype_t parttype_t parttype_p */
typedef struct _parttype_t
{

} parttype_t, *parttype_p;


/* generic binning method */

/* sl_type _bin_t bin_t */
typedef struct _bin_t
{
  elements_t s;

#ifdef elem_weight
  double weight;
#endif

} bin_t;


/* sl_type _splitter_t splitter_t */
typedef struct _splitter_t
{
  slint_t n;

  int *displs;
  slkey_pure_t *s;
  slint_t *sn;

} splitter_t;


struct _binning_t;

/* sl_type binning_pre_f binning_exec_f binning_refine_f binning_hit_f binning_finalize_f binning_post_f */
typedef slint_t (*binning_pre_f)(struct _binning_t *bm);
typedef slint_t (*binning_exec_f)(struct _binning_t *bm, bin_t *bin, double *counts, double *weights);
typedef slint_t (*binning_refine_f)(struct _binning_t *bm, bin_t *bin, slint_t k, double *counts, double *weights, splitter_t *sp, slint_t s, bin_t *new_bin);
typedef slint_t (*binning_hit_f)(struct _binning_t *bm, bin_t *bin, slint_t k, double *counts, splitter_t *sp, slint_t s);
typedef slint_t (*binning_finalize_f)(struct _binning_t *bm, bin_t *bin, double dcw, slint_t lc_min, slint_t lc_max, double *lcw, splitter_t *sp, slint_t s);
typedef slint_t (*binning_post_f)(struct _binning_t *bm);

#ifdef SL_DEPRECATED
/* sl_type binning_exec_pre_old_f binning_exec_post_old_f binning_refinable_old_f binning_refine_old_f */
typedef slint_t (*binning_exec_pre_old_f)(struct _binning_t *bm);
typedef slint_t (*binning_exec_post_old_f)(struct _binning_t *bm);
typedef slint_t (*binning_refinable_old_f)(struct _binning_t *bm);
typedef slint_t (*binning_refine_old_f)(struct _binning_t *bm, bin_t *bin, slint_t k, double *counts, bin_t *new_bin);
#endif


/* sl_type _binning_data_t binning_data_t */
typedef union _binning_data_t
{
  struct
  {
    slint_t rhigh, rlow, rwidth;
    slint_t rcurrent;
    slkey_pure_t bit_mask;

    elements_t sx;

  } radix;

} binning_data_t;


/* sl_type _binning_t binning_t */
typedef struct _binning_t
{
  slint_t nbins, max_nbins;
  
  binning_pre_f pre;
  binning_exec_f exec;
  binning_refine_f refine;
  binning_hit_f hit;
  binning_finalize_f finalize;
  binning_post_f post;

  slint_t sorted;

#ifdef elem_weight
  slint_t doweights;
#endif

#ifdef SL_DEPRECATED
  binning_exec_pre_old_f exec_pre_old;
  binning_exec_post_old_f exec_post_old;
  binning_refinable_old_f refinable_old;
  binning_refine_old_f refine_old;
#endif

  binning_data_t bd;

} binning_t;


/* sl_type _local_bins_t local_bins_t */
typedef struct _local_bins_t
{
  binning_t *bm;

  slint_t nbins, max_nbins;
  slint_t nelements;

#ifdef elem_weight
  slint_t doweights, weight_factor;
#endif

  slint_t nbinnings, max_nbinnings;

  slint_t nbins_new, last_new_b, last_new_k;
  bin_t *bins, *bins_new;
  bin_t *bins0, *bins1;

  double *counts, *bin_counts;
#ifdef elem_weight
  double *weights, *bin_weights;
#endif

  slint_t *bcws;
  double *cws, *bin_cws;

  slint_t last_exec_b;

} local_bins_t;


/* sl_type _global_bins_t global_bins_t */
typedef struct _global_bins_t
{
  binning_t *bm;
  
  local_bins_t lb;
  
  double *counts;
#ifdef elem_weight
  double *weights;
#endif

  slint_t *bcws;
  double *cws;

} global_bins_t;


/* sl_macro WEIGHT_FACTOR */
#ifdef elem_weight
# define WEIGHT_FACTOR  2
#else
# define WEIGHT_FACTOR  1
#endif


/* sl_macro get1d get2d get3d get4d */
#define get1d(x0)                           (x0)
#define get2d(x1, d0, x0)                  ((x0) + (d0) *  (x1))
#define get3d(x2, d1, x1, d0, x0)          ((x0) + (d0) * ((x1) + (d1) *  (x2)))
#define get4d(x3, d2, x2, d1, x1, d0, x0)  ((x0) + (d0) * ((x1) + (d1) * ((x2) + (d2) * (x3))))


/* sl_macro lb_bin_count lb_bin_weight */
#define lb_bin_count(_lb_, _b_, _j_)    ((_lb_)->bins[(_b_) * (_lb_)->nelements + _j_].s.size)
#ifdef elem_weight
# define lb_bin_weight(_lb_, _b_, _j_)  ((_lb_)->bins[(_b_) * (_lb_)->nelements + _j_].weight)
#else
# define lb_bin_weight(_lb_, _b_, _j_)  0
#endif

/* sl_macro lb_bin_counts lb_bin_weights */
#ifdef elem_weight
# define lb_bin_counts(_lb_, _b_, _j_, _k_)   ((_lb_)->bin_cws + get4d((_lb_)->bcws[_b_], (_lb_)->weight_factor, 0, (_lb_)->nelements, _j_, (_lb_)->bm->nbins, _k_))
# define lb_bin_weights(_lb_, _b_, _j_, _k_)  ((_lb_)->bin_cws + get4d((_lb_)->bcws[_b_], (_lb_)->weight_factor, 1, (_lb_)->nelements, _j_, (_lb_)->bm->nbins, _k_))
#else
# define lb_bin_counts(_lb_, _b_, _j_, _k_)   ((_lb_)->bin_cws + get4d((_lb_)->bcws[_b_], 1, 0, (_lb_)->nelements, _j_, (_lb_)->bm->nbins, _k_))
# define lb_bin_weights(_lb_, _b_, _j_, _k_)  NULL
#endif

/* sl_macro lb_counts lb_weights */
#ifdef elem_weight
# define lb_counts(_lb_, _b_, _k_)   ((_lb_)->cws + get3d((_lb_)->bcws[_b_], (_lb_)->weight_factor, 0, (_lb_)->bm->nbins, (_k_)))
# define lb_weights(_lb_, _b_, _k_)  ((_lb_)->cws + get3d((_lb_)->bcws[_b_], (_lb_)->weight_factor, 1, (_lb_)->bm->nbins, (_k_)))
#else
# define lb_counts(_lb_, _b_, _k_)   ((_lb_)->cws + get3d((_lb_)->bcws[_b_], 1, 0, (_lb_)->bm->nbins, (_k_)))
# define lb_weights(_lb_, _b_, _k_)  NULL
#endif

/* sl_macro gb_counts gb_weights */
#ifdef elem_weight
# define gb_counts(_gb_, _b_, _k_)   ((_gb_)->cws + get3d((_gb_)->bcws[_b_], (_gb_)->lb.weight_factor, 0, (_gb_)->bm->nbins, (_k_)))
# define gb_weights(_gb_, _b_, _k_)  ((_gb_)->cws + get3d((_gb_)->bcws[_b_], (_gb_)->lb.weight_factor, 1, (_gb_)->bm->nbins, (_k_)))
#else
# define gb_counts(_gb_, _b_, _k_)   ((_gb_)->cws + get3d((_gb_)->bcws[_b_], 1, 0, (_gb_)->bm->nbins, (_k_)))
# define gb_weights(_gb_, _b_, _k_)  NULL
#endif


#endif /* __SL_TYPES_H__ */
