#ifndef __SL_TUNE_H__
#define __SL_TUNE_H__

#if defined(JUMP)

 #define sort_radix_width_default    10
 #define sort_radix_width_max        10

 #define sort_radix_threshold_rec    256
 #define sort_radix_threshold_iter   8

#endif


#if defined(JUGENE)
#endif


#if defined(JUROPA)
#endif


#if defined(TETHYS)

 #define sort_radix_width_default    8
 #define sort_radix_width_max        8

 #define sort_radix_threshold_rec    100
 #define sort_radix_threshold_iter   8

#endif
#endif /* __SL_TUNE_H__ */
