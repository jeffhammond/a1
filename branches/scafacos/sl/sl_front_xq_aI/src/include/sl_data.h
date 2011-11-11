/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_data.h
 *  timestamp: 2010-10-05 09:11:40 +0200
 *  
 */


#ifndef __SL_DATA_H__
#define __SL_DATA_H__


#define data_nmax (0 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
 + 1 \
)

#define data_n (0 \
 + (data0_n) \
 + (data1_n) \
 + (data2_n) \
 + (data3_n) \
 + (data4_n) \
 + (data5_n) \
 + (data6_n) \
 + (data7_n) \
 + (data8_n) \
 + (data9_n) \
 + (data10_n) \
 + (data11_n) \
 + (data12_n) \
 + (data13_n) \
 + (data14_n) \
 + (data15_n) \
 + (data16_n) \
 + (data17_n) \
 + (data18_n) \
 + (data19_n) \
)

#define data_byte (0 \
 + (data0_byte) \
 + (data1_byte) \
 + (data2_byte) \
 + (data3_byte) \
 + (data4_byte) \
 + (data5_byte) \
 + (data6_byte) \
 + (data7_byte) \
 + (data8_byte) \
 + (data9_byte) \
 + (data10_byte) \
 + (data11_byte) \
 + (data12_byte) \
 + (data13_byte) \
 + (data14_byte) \
 + (data15_byte) \
 + (data16_byte) \
 + (data17_byte) \
 + (data18_byte) \
 + (data19_byte) \
)

#define data_byte_flex (0 \
 + (data0_byte_flex) \
 + (data1_byte_flex) \
 + (data2_byte_flex) \
 + (data3_byte_flex) \
 + (data4_byte_flex) \
 + (data5_byte_flex) \
 + (data6_byte_flex) \
 + (data7_byte_flex) \
 + (data8_byte_flex) \
 + (data9_byte_flex) \
 + (data10_byte_flex) \
 + (data11_byte_flex) \
 + (data12_byte_flex) \
 + (data13_byte_flex) \
 + (data14_byte_flex) \
 + (data15_byte_flex) \
 + (data16_byte_flex) \
 + (data17_byte_flex) \
 + (data18_byte_flex) \
 + (data19_byte_flex) \
)

/* regular use */
#define data_assign(_s_, _d_) \
 cc_data0_assign(_s_, _d_) \
 cc_data1_assign(_s_, _d_) \
 cc_data2_assign(_s_, _d_) \
 cc_data3_assign(_s_, _d_) \
 cc_data4_assign(_s_, _d_) \
 cc_data5_assign(_s_, _d_) \
 cc_data6_assign(_s_, _d_) \
 cc_data7_assign(_s_, _d_) \
 cc_data8_assign(_s_, _d_) \
 cc_data9_assign(_s_, _d_) \
 cc_data10_assign(_s_, _d_) \
 cc_data11_assign(_s_, _d_) \
 cc_data12_assign(_s_, _d_) \
 cc_data13_assign(_s_, _d_) \
 cc_data14_assign(_s_, _d_) \
 cc_data15_assign(_s_, _d_) \
 cc_data16_assign(_s_, _d_) \
 cc_data17_assign(_s_, _d_) \
 cc_data18_assign(_s_, _d_) \
 cc_data19_assign(_s_, _d_) \

#define data_assign_at(_s_, _sat_, _d_) \
 cc_data0_assign_at(_s_, _sat_, _d_) \
 cc_data1_assign_at(_s_, _sat_, _d_) \
 cc_data2_assign_at(_s_, _sat_, _d_) \
 cc_data3_assign_at(_s_, _sat_, _d_) \
 cc_data4_assign_at(_s_, _sat_, _d_) \
 cc_data5_assign_at(_s_, _sat_, _d_) \
 cc_data6_assign_at(_s_, _sat_, _d_) \
 cc_data7_assign_at(_s_, _sat_, _d_) \
 cc_data8_assign_at(_s_, _sat_, _d_) \
 cc_data9_assign_at(_s_, _sat_, _d_) \
 cc_data10_assign_at(_s_, _sat_, _d_) \
 cc_data11_assign_at(_s_, _sat_, _d_) \
 cc_data12_assign_at(_s_, _sat_, _d_) \
 cc_data13_assign_at(_s_, _sat_, _d_) \
 cc_data14_assign_at(_s_, _sat_, _d_) \
 cc_data15_assign_at(_s_, _sat_, _d_) \
 cc_data16_assign_at(_s_, _sat_, _d_) \
 cc_data17_assign_at(_s_, _sat_, _d_) \
 cc_data18_assign_at(_s_, _sat_, _d_) \
 cc_data19_assign_at(_s_, _sat_, _d_) \

#define data_null(_e_) \
 cc_data0_null(_e_) \
 cc_data1_null(_e_) \
 cc_data2_null(_e_) \
 cc_data3_null(_e_) \
 cc_data4_null(_e_) \
 cc_data5_null(_e_) \
 cc_data6_null(_e_) \
 cc_data7_null(_e_) \
 cc_data8_null(_e_) \
 cc_data9_null(_e_) \
 cc_data10_null(_e_) \
 cc_data11_null(_e_) \
 cc_data12_null(_e_) \
 cc_data13_null(_e_) \
 cc_data14_null(_e_) \
 cc_data15_null(_e_) \
 cc_data16_null(_e_) \
 cc_data17_null(_e_) \
 cc_data18_null(_e_) \
 cc_data19_null(_e_) \

#define data_inc(_e_) \
 cc_data0_inc(_e_) \
 cc_data1_inc(_e_) \
 cc_data2_inc(_e_) \
 cc_data3_inc(_e_) \
 cc_data4_inc(_e_) \
 cc_data5_inc(_e_) \
 cc_data6_inc(_e_) \
 cc_data7_inc(_e_) \
 cc_data8_inc(_e_) \
 cc_data9_inc(_e_) \
 cc_data10_inc(_e_) \
 cc_data11_inc(_e_) \
 cc_data12_inc(_e_) \
 cc_data13_inc(_e_) \
 cc_data14_inc(_e_) \
 cc_data15_inc(_e_) \
 cc_data16_inc(_e_) \
 cc_data17_inc(_e_) \
 cc_data18_inc(_e_) \
 cc_data19_inc(_e_) \

#define data_dec(_e_) \
 cc_data0_dec(_e_) \
 cc_data1_dec(_e_) \
 cc_data2_dec(_e_) \
 cc_data3_dec(_e_) \
 cc_data4_dec(_e_) \
 cc_data5_dec(_e_) \
 cc_data6_dec(_e_) \
 cc_data7_dec(_e_) \
 cc_data8_dec(_e_) \
 cc_data9_dec(_e_) \
 cc_data10_dec(_e_) \
 cc_data11_dec(_e_) \
 cc_data12_dec(_e_) \
 cc_data13_dec(_e_) \
 cc_data14_dec(_e_) \
 cc_data15_dec(_e_) \
 cc_data16_dec(_e_) \
 cc_data17_dec(_e_) \
 cc_data18_dec(_e_) \
 cc_data19_dec(_e_) \

#define data_add(_e_, _n_) \
 cc_data0_add(_e_, _n_) \
 cc_data1_add(_e_, _n_) \
 cc_data2_add(_e_, _n_) \
 cc_data3_add(_e_, _n_) \
 cc_data4_add(_e_, _n_) \
 cc_data5_add(_e_, _n_) \
 cc_data6_add(_e_, _n_) \
 cc_data7_add(_e_, _n_) \
 cc_data8_add(_e_, _n_) \
 cc_data9_add(_e_, _n_) \
 cc_data10_add(_e_, _n_) \
 cc_data11_add(_e_, _n_) \
 cc_data12_add(_e_, _n_) \
 cc_data13_add(_e_, _n_) \
 cc_data14_add(_e_, _n_) \
 cc_data15_add(_e_, _n_) \
 cc_data16_add(_e_, _n_) \
 cc_data17_add(_e_, _n_) \
 cc_data18_add(_e_, _n_) \
 cc_data19_add(_e_, _n_) \

#define data_sub(_e_, _n_) \
 cc_data0_sub(_e_, _n_) \
 cc_data1_sub(_e_, _n_) \
 cc_data2_sub(_e_, _n_) \
 cc_data3_sub(_e_, _n_) \
 cc_data4_sub(_e_, _n_) \
 cc_data5_sub(_e_, _n_) \
 cc_data6_sub(_e_, _n_) \
 cc_data7_sub(_e_, _n_) \
 cc_data8_sub(_e_, _n_) \
 cc_data9_sub(_e_, _n_) \
 cc_data10_sub(_e_, _n_) \
 cc_data11_sub(_e_, _n_) \
 cc_data12_sub(_e_, _n_) \
 cc_data13_sub(_e_, _n_) \
 cc_data14_sub(_e_, _n_) \
 cc_data15_sub(_e_, _n_) \
 cc_data16_sub(_e_, _n_) \
 cc_data17_sub(_e_, _n_) \
 cc_data18_sub(_e_, _n_) \
 cc_data19_sub(_e_, _n_) \

/* FIXME: add rti_cadd_moved(_n_,cmd) -> only ifdef SL_DATA else empty (like dataX) */

#define data_copy(_s_, _d_) \
 cc_data0_copy(_s_, _d_) \
 cc_data1_copy(_s_, _d_) \
 cc_data2_copy(_s_, _d_) \
 cc_data3_copy(_s_, _d_) \
 cc_data4_copy(_s_, _d_) \
 cc_data5_copy(_s_, _d_) \
 cc_data6_copy(_s_, _d_) \
 cc_data7_copy(_s_, _d_) \
 cc_data8_copy(_s_, _d_) \
 cc_data9_copy(_s_, _d_) \
 cc_data10_copy(_s_, _d_) \
 cc_data11_copy(_s_, _d_) \
 cc_data12_copy(_s_, _d_) \
 cc_data13_copy(_s_, _d_) \
 cc_data14_copy(_s_, _d_) \
 cc_data15_copy(_s_, _d_) \
 cc_data16_copy(_s_, _d_) \
 cc_data17_copy(_s_, _d_) \
 cc_data18_copy(_s_, _d_) \
 cc_data19_copy(_s_, _d_) \

#define data_ncopy(_s_, _d_, _n_) \
 cc_data0_ncopy(_s_, _d_, _n_) \
 cc_data1_ncopy(_s_, _d_, _n_) \
 cc_data2_ncopy(_s_, _d_, _n_) \
 cc_data3_ncopy(_s_, _d_, _n_) \
 cc_data4_ncopy(_s_, _d_, _n_) \
 cc_data5_ncopy(_s_, _d_, _n_) \
 cc_data6_ncopy(_s_, _d_, _n_) \
 cc_data7_ncopy(_s_, _d_, _n_) \
 cc_data8_ncopy(_s_, _d_, _n_) \
 cc_data9_ncopy(_s_, _d_, _n_) \
 cc_data10_ncopy(_s_, _d_, _n_) \
 cc_data11_ncopy(_s_, _d_, _n_) \
 cc_data12_ncopy(_s_, _d_, _n_) \
 cc_data13_ncopy(_s_, _d_, _n_) \
 cc_data14_ncopy(_s_, _d_, _n_) \
 cc_data15_ncopy(_s_, _d_, _n_) \
 cc_data16_ncopy(_s_, _d_, _n_) \
 cc_data17_ncopy(_s_, _d_, _n_) \
 cc_data18_ncopy(_s_, _d_, _n_) \
 cc_data19_ncopy(_s_, _d_, _n_) \

#define data_nmove(_s_, _d_, _n_) \
 cc_data0_nmove(_s_, _d_, _n_) \
 cc_data1_nmove(_s_, _d_, _n_) \
 cc_data2_nmove(_s_, _d_, _n_) \
 cc_data3_nmove(_s_, _d_, _n_) \
 cc_data4_nmove(_s_, _d_, _n_) \
 cc_data5_nmove(_s_, _d_, _n_) \
 cc_data6_nmove(_s_, _d_, _n_) \
 cc_data7_nmove(_s_, _d_, _n_) \
 cc_data8_nmove(_s_, _d_, _n_) \
 cc_data9_nmove(_s_, _d_, _n_) \
 cc_data10_nmove(_s_, _d_, _n_) \
 cc_data11_nmove(_s_, _d_, _n_) \
 cc_data12_nmove(_s_, _d_, _n_) \
 cc_data13_nmove(_s_, _d_, _n_) \
 cc_data14_nmove(_s_, _d_, _n_) \
 cc_data15_nmove(_s_, _d_, _n_) \
 cc_data16_nmove(_s_, _d_, _n_) \
 cc_data17_nmove(_s_, _d_, _n_) \
 cc_data18_nmove(_s_, _d_, _n_) \
 cc_data19_nmove(_s_, _d_, _n_) \

#define data_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data0_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data1_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data2_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data3_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data4_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data5_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data6_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data7_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data8_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data9_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data10_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data11_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data12_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data13_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data14_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data15_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data16_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data17_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data18_copy_at(_s_, _sat_, _d_, _dat_) \
 cc_data19_copy_at(_s_, _sat_, _d_, _dat_) \

#define data_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data0_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data1_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data2_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data3_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data4_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data5_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data6_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data7_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data8_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data9_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data10_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data11_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data12_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data13_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data14_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data15_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data16_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data17_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data18_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data19_ncopy_at(_s_, _sat_, _d_, _dat_, _n_) \

#define data_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data0_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data1_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data2_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data3_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data4_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data5_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data6_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data7_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data8_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data9_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data10_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data11_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data12_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data13_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data14_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data15_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data16_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data17_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data18_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \
 cc_data19_nmove_at(_s_, _sat_, _d_, _dat_, _n_) \

#define data_xchange(_e0_, _e1_, _t_) \
 cc_data0_xchange(_e0_, _e1_, _t_) \
 cc_data1_xchange(_e0_, _e1_, _t_) \
 cc_data2_xchange(_e0_, _e1_, _t_) \
 cc_data3_xchange(_e0_, _e1_, _t_) \
 cc_data4_xchange(_e0_, _e1_, _t_) \
 cc_data5_xchange(_e0_, _e1_, _t_) \
 cc_data6_xchange(_e0_, _e1_, _t_) \
 cc_data7_xchange(_e0_, _e1_, _t_) \
 cc_data8_xchange(_e0_, _e1_, _t_) \
 cc_data9_xchange(_e0_, _e1_, _t_) \
 cc_data10_xchange(_e0_, _e1_, _t_) \
 cc_data11_xchange(_e0_, _e1_, _t_) \
 cc_data12_xchange(_e0_, _e1_, _t_) \
 cc_data13_xchange(_e0_, _e1_, _t_) \
 cc_data14_xchange(_e0_, _e1_, _t_) \
 cc_data15_xchange(_e0_, _e1_, _t_) \
 cc_data16_xchange(_e0_, _e1_, _t_) \
 cc_data17_xchange(_e0_, _e1_, _t_) \
 cc_data18_xchange(_e0_, _e1_, _t_) \
 cc_data19_xchange(_e0_, _e1_, _t_) \

#define data_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data0_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data1_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data2_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data3_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data4_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data5_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data6_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data7_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data8_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data9_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data10_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data11_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data12_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data13_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data14_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data15_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data16_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data17_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data18_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \
 cc_data19_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_) \

/* chained versions */
#ifdef SL_DATA

 #define cc_data_assign(_s_, _d_)                           , data_assign(_s_, _d_)
 #define cc_data_assign_at(_s_, _sat_, _d_)                 , data_assign_at(_s_, _sat_, _d_)
 #define cc_data_null(_e_)                                  , data_null(_e_)
 #define cc_data_inc(_e_)                                   , data_inc(_e_)
 #define cc_data_dec(_e_)                                   , data_dec(_e_)
 #define cc_data_add(_e_, _n_)                              , data_add(_e_, _n_)
 #define cc_data_sub(_e_, _n_)                              , data_sub(_e_, _n_)
 #define cc_data_copy(_s_, _d_)                             , data_copy(_s_, _d_)
 #define cc_data_ncopy(_s_, _d_, _n_)                       , data_ncopy(_s_, _d_, _n_)
 #define cc_data_nmove(_s_, _d_, _n_)                       , data_nmove(_s_, _d_, _n_)
 #define cc_data_copy_at(_s_, _sat_, _d_, _dat_)            , data_copy_at(_s_, _sat_, _d_, _dat_)
 #define cc_data_ncopy_at(_s_, _sat_, _d_, _dat_, _n_)      , data_ncopy_at(_s_, _sat_, _d_, _dat_, _n_)
 #define cc_data_nmove_at(_s_, _sat_, _d_, _dat_, _n_)      , data_nmove_at(_s_, _sat_, _d_, _dat_, _n_)
 #define cc_data_xchange(_e0_, _e1_, _t_)                   , data_xchange(_e0_, _e1_, _t_)
 #define cc_data_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_)  , data_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_)

#else /* SL_DATA */

/* #define SL_DATA*/
 #define cc_data_assign(_s_, _d_)
 #define cc_data_assign_at(_s_, _sat_, _d_)
 #define cc_data_null(_e_)
 #define cc_data_inc(_e_)
 #define cc_data_dec(_e_)
 #define cc_data_add(_e_, _n_)
 #define cc_data_sub(_e_, _n_)
 #define cc_data_copy(_s_, _d_)
 #define cc_data_ncopy(_s_, _d_, _n_)
 #define cc_data_nmove(_s_, _d_, _n_)
 #define cc_data_copy_at(_s_, _sat_, _d_, _dat_)
 #define cc_data_ncopy_at(_s_, _sat_, _d_, _dat_, _n_)
 #define cc_data_nmove_at(_s_, _sat_, _d_, _dat_, _n_)
 #define cc_data_xchange(_e0_, _e1_, _t_)
 #define cc_data_xchange_at(_e0_, _at0_, _e1_, _at1_, _t_)

#endif /* SL_DATA */


#endif /* __SL_DATA_H__ */
