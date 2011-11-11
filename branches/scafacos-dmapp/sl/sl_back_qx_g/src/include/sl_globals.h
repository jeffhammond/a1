/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_globals.h
 *  timestamp: 2011-01-13 10:37:24 +0100
 *  
 */


#ifndef __SL_GLOBALS_H__
#define __SL_GLOBALS_H__


/* src/include/sl_debug.h */
extern FILE *sl_notice_fstream;
extern FILE *sl_error_fstream;
extern FILE *sl_debug_fstream;

/* src/include/sl_rti_intern.h */
extern rti rti_env;

/* src/core/sort_radix_ip.c */
extern slint_t sa_ip_threshold;

/* src/core_mpi/mpi_common.c */
#ifdef SL_USE_MPI
extern MPI_Datatype int_mpi_datatype;
extern MPI_Datatype key_mpi_datatype;
extern MPI_Datatype pkey_mpi_datatype;
extern MPI_Datatype pwkey_mpi_datatype;
extern MPI_Datatype index_mpi_datatype;
extern MPI_Datatype weight_mpi_datatype;
extern MPI_Datatype data_mpi_datatype[];
#endif
#ifdef SL_USE_MPI
extern int sl_mpi_rank;
#endif

/* src/core_mpi/mpi_elements.c */
extern void *me_sendrecv_replace_mem;
extern slint_t me_sendrecv_replace_memsize;
extern slint_t me_sendrecv_replace_mpi_maxsize;

/* src/core_mpi/mpi_select_exact_generic.c */
extern slint_t mseg_rounds;
extern int mseg_root;
extern double mseg_border_update_propagation;
extern slint_t mseg_border_update_full;
extern slint_t mseg_binnings;

/* src/core_mpi/mpi_sort_partition.c */
extern double msp_t[];
extern slint_t msp_sync;


#endif /* __SL_GLOBALS_H__ */
