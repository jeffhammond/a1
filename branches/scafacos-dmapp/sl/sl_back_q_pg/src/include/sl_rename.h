/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_rename.h
 *  timestamp: 2011-01-13 10:37:27 +0100
 *  
 */


#ifndef __SL_RENAME_H__
#define __SL_RENAME_H__


#define SL_CONCAT(_a_, _b_)           SL_CONCAT_(_a_, _b_)
#define SL_CONCAT_(_a_, _b_)          _a_##_b_

#define SL_CONCONCAT(_a_, _b_, _c_)   SL_CONCONCAT_(_a_, _b_, _c_)
#define SL_CONCONCAT_(_a_, _b_, _c_)  _a_##_b_##_c_

#ifdef SL_PREFIX
# define SL_VAR(_v_)   SL_CONCAT(SL_PREFIX, _v_)
# define SL_FUNC(_f_)  SL_CONCAT(SL_PREFIX, _f_)
#else
# define SL_VAR(_v_)   _v_
# define SL_FUNC(_f_)  _f_
#endif


/* src/include/sl_debug.h */
#define sl_notice_fstream  SL_VAR(sl_notice_fstream)
#define sl_error_fstream  SL_VAR(sl_error_fstream)
#define sl_debug_fstream  SL_VAR(sl_debug_fstream)

/* src/include/sl_environment_intern.h */
#define sl_alloc_tmp_p  SL_VAR(sl_alloc_tmp_p)
#define ts_temp  SL_VAR(ts_temp)

/* src/include/sl_rti_intern.h */
#define rti_env  SL_VAR(rti_env)

/* src/core/binning.c */
#define binning_create  SL_FUNC(binning_create)
#define binning_destroy  SL_FUNC(binning_destroy)
#define binning_pre  SL_FUNC(binning_pre)
#define binning_exec_reset  SL_FUNC(binning_exec_reset)
#define binning_exec  SL_FUNC(binning_exec)
#define binning_refine  SL_FUNC(binning_refine)
#define binning_hit  SL_FUNC(binning_hit)
#define binning_finalize  SL_FUNC(binning_finalize)
#define binning_post  SL_FUNC(binning_post)

/* src/core/binning_radix.c */
#define binning_radix_create  SL_FUNC(binning_radix_create)
#define binning_radix_destroy  SL_FUNC(binning_radix_destroy)
#define binning_radix_pre  SL_FUNC(binning_radix_pre)
#define binning_radix_exec  SL_FUNC(binning_radix_exec)
#define binning_radix_refine  SL_FUNC(binning_radix_refine)
#define binning_radix_hit  SL_FUNC(binning_radix_hit)
#define binning_radix_finalize  SL_FUNC(binning_radix_finalize)
#define binning_radix_post  SL_FUNC(binning_radix_post)
#define binning_radix_exec_pre_old  SL_FUNC(binning_radix_exec_pre_old)
#define binning_radix_exec_post_old  SL_FUNC(binning_radix_exec_post_old)
#define binning_radix_refinable_old  SL_FUNC(binning_radix_refinable_old)
#define binning_radix_refine_old  SL_FUNC(binning_radix_refine_old)

/* src/core/checksum_crc.c */
#define icrc1  SL_FUNC(icrc1)
#define icrc  SL_FUNC(icrc)
#define reflect  SL_FUNC(reflect)
#define crcInit  SL_FUNC(crcInit)
#define crcFast  SL_FUNC(crcFast)
#define elements_crc16  SL_FUNC(elements_crc16)
#define elements_crc32  SL_FUNC(elements_crc32)

/* src/core/elements.c */
#define elements_alloc  SL_FUNC(elements_alloc)
#define elements_alloc2  SL_FUNC(elements_alloc2)
#define elements_alloc_old  SL_FUNC(elements_alloc_old)
#define elements_free  SL_FUNC(elements_free)
#define elements_alloc_from_blocks  SL_FUNC(elements_alloc_from_blocks)
#define elements_alloc_from_block2  SL_FUNC(elements_alloc_from_block2)
#define elements_alloc_from_block  SL_FUNC(elements_alloc_from_block)
#define elements_copy  SL_FUNC(elements_copy)
#define elements_copy_at  SL_FUNC(elements_copy_at)
#define elements_ncopy  SL_FUNC(elements_ncopy)
#define elements_nmove  SL_FUNC(elements_nmove)
#define elements_printf  SL_FUNC(elements_printf)
#define elements_extract  SL_FUNC(elements_extract)
#define elements_touch  SL_FUNC(elements_touch)
#define elements_random_exchange  SL_FUNC(elements_random_exchange)
#define elements_init_keys  SL_FUNC(elements_init_keys)
#define elements_init_keys2  SL_FUNC(elements_init_keys2)
#define elements_keys_init  SL_FUNC(elements_keys_init)
#define elements_init_keys_from_file  SL_FUNC(elements_init_keys_from_file)
#define elements_save_keys_to_file  SL_FUNC(elements_save_keys_to_file)
#define elements_validate_order  SL_FUNC(elements_validate_order)
#define elements_validate_order_bmask  SL_FUNC(elements_validate_order_bmask)
#define elements_validate_order_weight  SL_FUNC(elements_validate_order_weight)
#define elements_keys_stats  SL_FUNC(elements_keys_stats)
#define elements_print_keys  SL_FUNC(elements_print_keys)
#define elements_print_all  SL_FUNC(elements_print_all)
#define elements_get_weight  SL_FUNC(elements_get_weight)
#define elements_get_minmax_keys  SL_FUNC(elements_get_minmax_keys)

/* src/core/elements_packed.c */
#define elements_alloc_packed  SL_FUNC(elements_alloc_packed)
#define elements_free_packed  SL_FUNC(elements_free_packed)
#define elements_pack_indexed  SL_FUNC(elements_pack_indexed)
#define elements_pack  SL_FUNC(elements_pack)
#define elements_unpack_indexed  SL_FUNC(elements_unpack_indexed)
#define elements_unpack  SL_FUNC(elements_unpack)
#define elements_unpack_keys  SL_FUNC(elements_unpack_keys)

/* src/core/key2class.c */
#define key2class_equal  SL_FUNC(key2class_equal)
#define key2class_split  SL_FUNC(key2class_split)
#define key2class_split_keys  SL_FUNC(key2class_split_keys)
#define key2class_random  SL_FUNC(key2class_random)
#define key2class_ci_nocounts  SL_FUNC(key2class_ci_nocounts)
#define key2class_ci  SL_FUNC(key2class_ci)

/* src/core/merge2_basic.c */
#define merge2_basic_01_x  SL_FUNC(merge2_basic_01_x)
#define merge2_basic_01_X  SL_FUNC(merge2_basic_01_X)

/* src/core/merge2_basic_auto.c */
#define merge2_basic_auto_01_x  SL_FUNC(merge2_basic_auto_01_x)

/* src/core/merge2_basic_sbin.c */
#define merge2_basic_sbin_x0_1  SL_FUNC(merge2_basic_sbin_x0_1)
#define merge2_basic_sbin_0x_1  SL_FUNC(merge2_basic_sbin_0x_1)
#define merge2_basic_sbin_01_x  SL_FUNC(merge2_basic_sbin_01_x)
#define merge2_basic_sbin_01  SL_FUNC(merge2_basic_sbin_01)

/* src/core/merge2_basic_shyb.c */
#define merge2_basic_shyb_x0_1  SL_FUNC(merge2_basic_shyb_x0_1)
#define merge2_basic_shyb_0x_1  SL_FUNC(merge2_basic_shyb_0x_1)
#define merge2_basic_shyb_01_x  SL_FUNC(merge2_basic_shyb_01_x)
#define merge2_basic_shyb_01  SL_FUNC(merge2_basic_shyb_01)

/* src/core/merge2_basic_sseq.c */
#define merge2_basic_sseq_x0_1  SL_FUNC(merge2_basic_sseq_x0_1)
#define merge2_basic_sseq_0x_1  SL_FUNC(merge2_basic_sseq_0x_1)
#define merge2_basic_sseq_01_x  SL_FUNC(merge2_basic_sseq_01_x)
#define merge2_basic_sseq_01  SL_FUNC(merge2_basic_sseq_01)

/* src/core/merge2_basic_straight.c */
#define merge2_basic_straight_x0_1  SL_FUNC(merge2_basic_straight_x0_1)
#define merge2_basic_straight_0x_1  SL_FUNC(merge2_basic_straight_0x_1)
#define merge2_basic_straight_01_x  SL_FUNC(merge2_basic_straight_01_x)
#define merge2_basic_straight_x_0_1  SL_FUNC(merge2_basic_straight_x_0_1)
#define merge2_basic_straight_X0_1  SL_FUNC(merge2_basic_straight_X0_1)
#define merge2_basic_straight_0X_1  SL_FUNC(merge2_basic_straight_0X_1)
#define merge2_basic_straight_01_X  SL_FUNC(merge2_basic_straight_01_X)
#define merge2_basic_straight_X0_1u  SL_FUNC(merge2_basic_straight_X0_1u)

/* src/core/merge2_compo_hula.c */
#define merge2_compo_hula_  SL_FUNC(merge2_compo_hula_)
#define merge2_compo_hula  SL_FUNC(merge2_compo_hula)

/* src/core/merge2_compo_tridgell.c */
#define merge2_compo_tridgell  SL_FUNC(merge2_compo_tridgell)

/* src/core/merge2_memory_adaptive.c */
#define merge2_memory_adaptive  SL_FUNC(merge2_memory_adaptive)

/* src/core/merge2_simplify.c */
#define merge2_simplify_s1  SL_FUNC(merge2_simplify_s1)

/* src/core/mergep_2way.c */
#define mergep_2way_ip_int  SL_FUNC(mergep_2way_ip_int)

/* src/core/mergep_heap.c */
#define mergep_heap_int  SL_FUNC(mergep_heap_int)
#define mergep_heap_int_idx  SL_FUNC(mergep_heap_int_idx)
#define mergep_heap_idx  SL_FUNC(mergep_heap_idx)
#define mergep_heap_unpack_idx  SL_FUNC(mergep_heap_unpack_idx)
#define mergep_heap_unpack_idxonly  SL_FUNC(mergep_heap_unpack_idxonly)

/* src/core/search.c */
#define sl_search_sequential_lt  SL_FUNC(sl_search_sequential_lt)
#define sl_search_sequential_le  SL_FUNC(sl_search_sequential_le)
#define sl_search_sequential_gt  SL_FUNC(sl_search_sequential_gt)
#define sl_search_sequential_ge  SL_FUNC(sl_search_sequential_ge)
#define sl_search_binary_lt  SL_FUNC(sl_search_binary_lt)
#define sl_search_binary_le  SL_FUNC(sl_search_binary_le)
#define sl_search_binary_gt  SL_FUNC(sl_search_binary_gt)
#define sl_search_binary_ge  SL_FUNC(sl_search_binary_ge)
#define sl_search_binary_lt2  SL_FUNC(sl_search_binary_lt2)
#define sl_search_binary_le2  SL_FUNC(sl_search_binary_le2)
#define sl_search_binary_gt2  SL_FUNC(sl_search_binary_gt2)
#define sl_search_binary_ge2  SL_FUNC(sl_search_binary_ge2)
#define sl_search_binary_lt_bmask  SL_FUNC(sl_search_binary_lt_bmask)
#define sl_search_hybrid_lt  SL_FUNC(sl_search_hybrid_lt)
#define sl_search_hybrid_le  SL_FUNC(sl_search_hybrid_le)
#define sl_search_hybrid_gt  SL_FUNC(sl_search_hybrid_gt)
#define sl_search_hybrid_ge  SL_FUNC(sl_search_hybrid_ge)

/* src/core/sl_common.c */
#define ilog2c  SL_FUNC(ilog2c)
#define ilog2f  SL_FUNC(ilog2f)
#define sl_random64  SL_FUNC(sl_random64)
#define sl_nrandom_seed  SL_VAR(sl_nrandom_seed)
#define sl_nrandom  SL_FUNC(sl_nrandom)
#define sl_urandom_seed  SL_VAR(sl_urandom_seed)
#define sl_urandom  SL_FUNC(sl_urandom)
#define sl_proto  SL_FUNC(sl_proto)
#define print_bits  SL_FUNC(print_bits)
#define pivot_random  SL_FUNC(pivot_random)
#define counts2displs  SL_FUNC(counts2displs)
#define displs2counts  SL_FUNC(displs2counts)

/* src/core/sl_elem.c */
#define elem_set_data  SL_FUNC(elem_set_data)
#define elem_reverse_aio  SL_FUNC(elem_reverse_aio)
#define elem_reverse_obo  SL_FUNC(elem_reverse_obo)
#define elem_reverse  SL_FUNC(elem_reverse)
#define elem_nxchange_at  SL_FUNC(elem_nxchange_at)
#define elem_nxchange  SL_FUNC(elem_nxchange)
#define elem_nxchange_ro0  SL_FUNC(elem_nxchange_ro0)
#define elem_rotate_3rev_aio  SL_FUNC(elem_rotate_3rev_aio)
#define elem_rotate_3rev_obo  SL_FUNC(elem_rotate_3rev_obo)
#define elem_rotate_cycles_aio  SL_FUNC(elem_rotate_cycles_aio)
#define elem_rotate_cycles_obo  SL_FUNC(elem_rotate_cycles_obo)
#define elem_rotate  SL_FUNC(elem_rotate)
#define elem_rotate_ro0  SL_FUNC(elem_rotate_ro0)
#define elem_rotate_ro1  SL_FUNC(elem_rotate_ro1)

/* src/core/sort_counting.c */
#define make_counts  SL_FUNC(make_counts)
#define make_displs  SL_FUNC(make_displs)
#define make_counts2displs  SL_FUNC(make_counts2displs)
#define sort_counting_use_displs  SL_FUNC(sort_counting_use_displs)
#define sort_counting_use_counts  SL_FUNC(sort_counting_use_counts)
#define sort_counting_get_counts  SL_FUNC(sort_counting_get_counts)
#define sort_counting  SL_FUNC(sort_counting)

/* src/core/sort_heap.c */
#define hs_heapify_i  SL_FUNC(hs_heapify_i)
#define sort_heap_i  SL_FUNC(sort_heap_i)
#define sort_heap_p  SL_FUNC(sort_heap_p)
#define sort_heap  SL_FUNC(sort_heap)

/* src/core/sort_insert.c */
#define sort_insert_bmask_kernel  SL_FUNC(sort_insert_bmask_kernel)
#define sort_insert_kernel  SL_FUNC(sort_insert_kernel)
#define sort_insert  SL_FUNC(sort_insert)

/* src/core/sort_permute.c */
#define sort_permute_forward_  SL_FUNC(sort_permute_forward_)
#define sort_permute_forward_masked  SL_FUNC(sort_permute_forward_masked)
#define sort_permute_forward  SL_FUNC(sort_permute_forward)
#define sort_permute_backward_  SL_FUNC(sort_permute_backward_)
#define sort_permute_backward_masked  SL_FUNC(sort_permute_backward_masked)
#define sort_permute_backward  SL_FUNC(sort_permute_backward)

/* src/core/sort_quick.c */
#define qs_halfrec_i  SL_FUNC(qs_halfrec_i)
#define sort_quick_i  SL_FUNC(sort_quick_i)
#define qs_halfrec_p  SL_FUNC(qs_halfrec_p)
#define sort_quick_p  SL_FUNC(sort_quick_p)
#define qs_hybrid  SL_FUNC(qs_hybrid)
#define sort_quick_h  SL_FUNC(sort_quick_h)
#define sort_quick  SL_FUNC(sort_quick)

/* src/core/sort_radix.c */
#define sort_radix  SL_FUNC(sort_radix)

/* src/core/sort_radix_1bit.c */
#define split2_b_1brs  SL_FUNC(split2_b_1brs)
#define sort_radix_1bit_kernel  SL_FUNC(sort_radix_1bit_kernel)
#define sort_radix_1bit  SL_FUNC(sort_radix_1bit)

/* src/core/sort_radix_af.c */
#define rs_rec_insertsort_af  SL_FUNC(rs_rec_insertsort_af)
#define rs_rec_af  SL_FUNC(rs_rec_af)
#define sort_radix_af  SL_FUNC(sort_radix_af)

/* src/core/sort_radix_db.c */
#define tuneable_sort_radix_threshold_rec  SL_VAR(tuneable_sort_radix_threshold_rec)
#define rs_rec_db  SL_FUNC(rs_rec_db)
#define sort_radix_db  SL_FUNC(sort_radix_db)

/* src/core/sort_radix_ip.c */
#define sa_ip_threshold  SL_VAR(sa_ip_threshold)
#define rs_rec_ip  SL_FUNC(rs_rec_ip)
#define sort_radix_ip  SL_FUNC(sort_radix_ip)

/* src/core/sort_radix_iter.c */
#define tuneable_sort_radix_threshold_iter  SL_VAR(tuneable_sort_radix_threshold_iter)
#define rs_iter_insertsort  SL_FUNC(rs_iter_insertsort)
#define rs_iter  SL_FUNC(rs_iter)
#define sort_radix_iter  SL_FUNC(sort_radix_iter)

/* src/core/sort_radix_ma.c */
#define tuneable_sort_radix_threshold_rec  SL_VAR(tuneable_sort_radix_threshold_rec)
#define rs_rec_ma_db  SL_FUNC(rs_rec_ma_db)
#define rs_rec_ma  SL_FUNC(rs_rec_ma)
#define sort_radix_ma  SL_FUNC(sort_radix_ma)

/* src/core/sortnet.c */
#define sn_hypercube_lh  SL_FUNC(sn_hypercube_lh)
#define sn_hypercube_hl  SL_FUNC(sn_hypercube_hl)
#define sn_odd_even_trans  SL_FUNC(sn_odd_even_trans)
#define sn_odd  SL_FUNC(sn_odd)
#define sn_even  SL_FUNC(sn_even)
#define sn_batcher  SL_FUNC(sn_batcher)
#define sn_bitonic  SL_FUNC(sn_bitonic)
#define sn_connected  SL_FUNC(sn_connected)

/* src/core/splitter.c */
#define splitter_reset  SL_FUNC(splitter_reset)

/* src/core/splitx.c */
#define splitx_radix  SL_FUNC(splitx_radix)
#define split2_lt_ge  SL_FUNC(split2_lt_ge)
#define split2_le_gt  SL_FUNC(split2_le_gt)
#define split3_lt_eq_gt  SL_FUNC(split3_lt_eq_gt)
#define split3_lt_eq_gt_old  SL_FUNC(split3_lt_eq_gt_old)
#define split2_b  SL_FUNC(split2_b)
#define splitk_k2c_af  SL_FUNC(splitk_k2c_af)
#define splitk_k2c  SL_FUNC(splitk_k2c)
#define splitk_k2c_count  SL_FUNC(splitk_k2c_count)

/* src/core_mpi/mpi_binning.c */
#define mpi_binning_create  SL_FUNC(mpi_binning_create)
#define mpi_binning_destroy  SL_FUNC(mpi_binning_destroy)
#define mpi_binning_pre  SL_FUNC(mpi_binning_pre)
#define mpi_binning_exec_reset  SL_FUNC(mpi_binning_exec_reset)
#define mpi_binning_exec_local  SL_FUNC(mpi_binning_exec_local)
#define mpi_binning_exec_global  SL_FUNC(mpi_binning_exec_global)
#define mpi_binning_refine  SL_FUNC(mpi_binning_refine)
#define mpi_binning_hit  SL_FUNC(mpi_binning_hit)
#define mpi_binning_finalize  SL_FUNC(mpi_binning_finalize)
#define mpi_binning_post  SL_FUNC(mpi_binning_post)

/* src/core_mpi/mpi_common.c */
#define int_mpi_datatype  SL_VAR(int_mpi_datatype)
#define key_mpi_datatype  SL_VAR(key_mpi_datatype)
#define pkey_mpi_datatype  SL_VAR(pkey_mpi_datatype)
#define pwkey_mpi_datatype  SL_VAR(pwkey_mpi_datatype)
#define index_mpi_datatype  SL_VAR(index_mpi_datatype)
#define weight_mpi_datatype  SL_VAR(weight_mpi_datatype)
#define data_mpi_datatype  SL_VAR(data_mpi_datatype)
#define sl_mpi_rank  SL_VAR(sl_mpi_rank)
#define mpi_datatypes_init  SL_FUNC(mpi_datatypes_init)
#define mpi_datatypes_release  SL_FUNC(mpi_datatypes_release)
#define mpi_get_grid  SL_FUNC(mpi_get_grid)
#define mpi_subgroups_create  SL_FUNC(mpi_subgroups_create)
#define mpi_subgroups_delete  SL_FUNC(mpi_subgroups_delete)

/* src/core_mpi/mpi_elements.c */
#define me_sendrecv_replace_mem  SL_VAR(me_sendrecv_replace_mem)
#define me_sendrecv_replace_memsize  SL_VAR(me_sendrecv_replace_memsize)
#define me_sendrecv_replace_mpi_maxsize  SL_VAR(me_sendrecv_replace_mpi_maxsize)
#define mpi_elements_init_keys_from_file  SL_FUNC(mpi_elements_init_keys_from_file)
#define mpi_elements_validate_order  SL_FUNC(mpi_elements_validate_order)
#define mpi_linear_exchange_pure_keys  SL_FUNC(mpi_linear_exchange_pure_keys)
#define mpi_elements_check_order  SL_FUNC(mpi_elements_check_order)
#define mpi_check_global_order  SL_FUNC(mpi_check_global_order)
#define mpi_cs16  SL_FUNC(mpi_cs16)
#define mpi_cs32  SL_FUNC(mpi_cs32)
#define mpi_elements_get_counts  SL_FUNC(mpi_elements_get_counts)
#define mpi_elements_get_weights  SL_FUNC(mpi_elements_get_weights)
#define mpi_elements_get_counts_and_weights  SL_FUNC(mpi_elements_get_counts_and_weights)
#define mpi_elements_sendrecv_replace  SL_FUNC(mpi_elements_sendrecv_replace)
#define mpi_elements_crc32  SL_FUNC(mpi_elements_crc32)

/* src/core_mpi/mpi_elements_alltoallv.c */
#define mpi_elements_alltoallv_db  SL_FUNC(mpi_elements_alltoallv_db)
#define mpi_elements_alltoallv_ip  SL_FUNC(mpi_elements_alltoallv_ip)

/* src/core_mpi/mpi_elements_packed.c */
#define mpi_elements_packed_datatype_create  SL_FUNC(mpi_elements_packed_datatype_create)
#define mpi_elements_packed_datatype_destroy  SL_FUNC(mpi_elements_packed_datatype_destroy)

/* src/core_mpi/mpi_find_exact.c */
#define mpi_find_exact_equal  SL_FUNC(mpi_find_exact_equal)
#define mpi_find_exact  SL_FUNC(mpi_find_exact)

/* src/core_mpi/mpi_linsplit.c */
#define mpi_linsplit  SL_FUNC(mpi_linsplit)
#define mpi_linsplit2  SL_FUNC(mpi_linsplit2)

/* src/core_mpi/mpi_merge2.c */
#define mpi_merge2  SL_FUNC(mpi_merge2)

/* src/core_mpi/mpi_mergek.c */
#define mpi_mergek_equal  SL_FUNC(mpi_mergek_equal)
#define mpi_mergek  SL_FUNC(mpi_mergek)
#define mpi_mergek_equal2  SL_FUNC(mpi_mergek_equal2)

/* src/core_mpi/mpi_mergek_presort.c */
#define mpi_mergek_presort_radix  SL_FUNC(mpi_mergek_presort_radix)

/* src/core_mpi/mpi_partition_exact_generic.c */
#define mpi_partition_exact_generic  SL_FUNC(mpi_partition_exact_generic)
#define mpi_partition_exact_generic2  SL_FUNC(mpi_partition_exact_generic2)

/* src/core_mpi/mpi_partition_exact_radix.c */
#define mpi_partition_exact_radix  SL_FUNC(mpi_partition_exact_radix)

/* src/core_mpi/mpi_partition_exact_radix_grouped.c */
#define mpi_partition_exact_radix_ngroups  SL_FUNC(mpi_partition_exact_radix_ngroups)
#define mpi_partition_exact_radix_2groups  SL_FUNC(mpi_partition_exact_radix_2groups)

/* src/core_mpi/mpi_partition_joink.c */
#define dfs_visit_dcrbe  SL_FUNC(dfs_visit_dcrbe)
#define disjoint_cycle_reduce_back_edges  SL_FUNC(disjoint_cycle_reduce_back_edges)
#define mpi_partition_joink  SL_FUNC(mpi_partition_joink)

/* src/core_mpi/mpi_partition_radix2.c */
#define mpi_partition_radix2  SL_FUNC(mpi_partition_radix2)

/* src/core_mpi/mpi_partition_radix_old.c */
#define mpi_partition_radix_old  SL_FUNC(mpi_partition_radix_old)

/* src/core_mpi/mpi_partition_sample.c */
#define mpi_partition_sample_regular  SL_FUNC(mpi_partition_sample_regular)

/* src/core_mpi/mpi_rebalance.c */
#define mpi_rebalance  SL_FUNC(mpi_rebalance)
#define mpi_rebalance_alltoallv  SL_FUNC(mpi_rebalance_alltoallv)

/* src/core_mpi/mpi_redistribute_exact_generic.c */
#define mpi_redistribute_exact_generic  SL_FUNC(mpi_redistribute_exact_generic)

/* src/core_mpi/mpi_select_common.c */
#define init_partconds  SL_FUNC(init_partconds)
#define init_partconds_intern  SL_FUNC(init_partconds_intern)
#define merge_partconds  SL_FUNC(merge_partconds)
#define mpi_gather_partconds_grouped  SL_FUNC(mpi_gather_partconds_grouped)
#define mpi_gather_partconds  SL_FUNC(mpi_gather_partconds)
#define mpi_allgather_partconds  SL_FUNC(mpi_allgather_partconds)
#define mpi_bcast_partconds  SL_FUNC(mpi_bcast_partconds)
#define mpi_post_check_partconds  SL_FUNC(mpi_post_check_partconds)
#define mpi_post_check_partconds_intern  SL_FUNC(mpi_post_check_partconds_intern)
#define mpi_select_stats  SL_FUNC(mpi_select_stats)

/* src/core_mpi/mpi_select_exact_generic.c */
#define mseg_rounds  SL_VAR(mseg_rounds)
#define mseg_root  SL_VAR(mseg_root)
#define mseg_border_update_propagation  SL_VAR(mseg_border_update_propagation)
#define mseg_border_update_full  SL_VAR(mseg_border_update_full)
#define mseg_binnings  SL_VAR(mseg_binnings)
#define border_update  SL_FUNC(border_update)
#define border_update_full  SL_FUNC(border_update_full)
#define border_change_old  SL_FUNC(border_change_old)
#define border_change  SL_FUNC(border_change)
#define border_init  SL_FUNC(border_init)
#define border_currents  SL_FUNC(border_currents)
#define mpi_select_exact_generic_bulk  SL_FUNC(mpi_select_exact_generic_bulk)
#define mpi_select_exact_generic_grouped  SL_FUNC(mpi_select_exact_generic_grouped)
#define mpi_select_exact_generic  SL_FUNC(mpi_select_exact_generic)

/* src/core_mpi/mpi_select_exact_radix.c */
#define mpi_select_exact_radix  SL_FUNC(mpi_select_exact_radix)
#define mpi_select_exact_radix_fixed  SL_FUNC(mpi_select_exact_radix_fixed)
#define mpi_select_exact_radix_grouped  SL_FUNC(mpi_select_exact_radix_grouped)

/* src/core_mpi/mpi_select_qs.c */
#define mpi_select_qs  SL_FUNC(mpi_select_qs)

/* src/core_mpi/mpi_select_radix2.c */
#define mpi_select_radix2  SL_FUNC(mpi_select_radix2)
#define mpi_select_stats2  SL_FUNC(mpi_select_stats2)
#define mpi_select_radix_pg2  SL_FUNC(mpi_select_radix_pg2)

/* src/core_mpi/mpi_select_sample.c */
#define mpi_select_sample_regular  SL_FUNC(mpi_select_sample_regular)

/* src/core_mpi/mpi_sort_merge.c */
#define mpi_sort_merge  SL_FUNC(mpi_sort_merge)
#define mpi_sort_merge2  SL_FUNC(mpi_sort_merge2)

/* src/core_mpi/mpi_sort_partition.c */
#define msp_t  SL_VAR(msp_t)
#define msp_sync  SL_VAR(msp_sync)
#define mpi_sort_partition  SL_FUNC(mpi_sort_partition)
#define mpi_sort_partition_radix  SL_FUNC(mpi_sort_partition_radix)
#define mpi_sort_partition_exact_radix  SL_FUNC(mpi_sort_partition_exact_radix)
#define mpi_sort_partition_exact_radix_ngroups  SL_FUNC(mpi_sort_partition_exact_radix_ngroups)
#define mpi_sort_partition_exact_radix_2groups  SL_FUNC(mpi_sort_partition_exact_radix_2groups)

/* src/core_mpi/mpi_splitk.c */
#define flow_create  SL_FUNC(flow_create)
#define dfs_visit_rbe  SL_FUNC(dfs_visit_rbe)
#define cycle_reduce_back_edges  SL_FUNC(cycle_reduce_back_edges)
#define flow_reduce_unbalanced  SL_FUNC(flow_reduce_unbalanced)
#define mpi_splitk  SL_FUNC(mpi_splitk)

/* src/core_mpi/mpi_splitk_dummy.c */
#define mpi_splitk_dummy  SL_FUNC(mpi_splitk_dummy)

/* src/core_mpi/mpi_xcounts2ycounts.c */
#define mpi_xcounts2ycounts_all2all  SL_FUNC(mpi_xcounts2ycounts_all2all)
#define mpi_xcounts2ycounts_sparse  SL_FUNC(mpi_xcounts2ycounts_sparse)
#define mpi_xcounts2ycounts_grouped  SL_FUNC(mpi_xcounts2ycounts_grouped)
#define mpi_subxdispls2ycounts  SL_FUNC(mpi_subxdispls2ycounts)


#endif /* __SL_RENAME_H__ */
