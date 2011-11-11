/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_debug.h
 *  timestamp: 2010-12-09 14:42:05 +0100
 *  
 */


#ifndef __SL_DEBUG_H__
#define __SL_DEBUG_H__


#include <stdarg.h>
#include <stdio.h>


#ifdef DECLARE_DEBUG
/* sl_var sl_notice_fstream sl_error_fstream sl_debug_fstream */
FILE *sl_notice_fstream = NULL;  /* sl_global */
FILE *sl_error_fstream = NULL;   /* sl_global */
FILE *sl_debug_fstream = NULL;   /* sl_global */
#endif
extern FILE *sl_notice_fstream, *sl_error_fstream, *sl_debug_fstream;


#define SL_NOTICE_FSTREAM  (sl_notice_fstream?sl_notice_fstream:stdout)
#define SL_ERROR_FSTREAM   (sl_error_fstream?sl_error_fstream:stderr)
#ifdef DEBUG_STDERR
# define SL_DEBUG_FSTREAM  (sl_debug_fstream?sl_debug_fstream:stderr)
# else
# define SL_DEBUG_FSTREAM  (sl_debug_fstream?sl_debug_fstream:stdout)
#endif


#ifdef SL_USE_MPI
 extern int sl_mpi_rank;
 #define MPI_STR      "%d: "
 #define MPI_PARAM    sl_mpi_rank
#else
 #ifdef sl_mpi_rank
 # undef sl_mpi_rank
 #endif
 #define sl_mpi_rank  -2
 #define MPI_STR      "%s"
 #define MPI_PARAM    ""
#endif


#if 1
# define comm_fmt       "s"
# define comm_str(_c_)  (((_c_) != MPI_COMM_NULL)?"valid":"null")
#else
# define comm_fmt       "p"
# define comm_str(_c_)  (_c_)
#endif


#define SL_NOTICE(_format_, _args_...)           fprintf(SL_NOTICE_FSTREAM, MPI_STR _format_ "\n", MPI_PARAM, ##_args_)
#define SL_NOTICE_IF(_if_, _format_, _args_...)  SL_MOP(if (_if_) SL_NOTICE(_format_, ##_args_); )

#define SL_ERROR(_format_, _args_...)            fprintf(SL_ERROR_FSTREAM, MPI_STR "%s:%i:%s: " _format_ "\n", MPI_PARAM, __FILE__, __LINE__, __func__, ##_args_)
#define SL_ERROR_IF(_if_, _format_, _args_...)   SL_MOP(if (_if_) SL_ERROR(_format_, ##_args_); )

#ifdef SLDEBUG_OUTPUT
# define SL_DEBUG_INTRO(_level_) \
    SL_MOP(if (_level_ <= SLDEBUG_OUTPUT) fprintf(SL_DEBUG_FSTREAM, MPI_STR "%s:%i:%s: ", MPI_PARAM, __FILE__, __LINE__, __func__); )
# define SL_DEBUG_CORE(_level_, _format_, _args_...) \
    SL_MOP(if (_level_ <= SLDEBUG_OUTPUT) fprintf(SL_DEBUG_FSTREAM, _format_, ##_args_); )
# define SL_DEBUG_OUTRO(_level_) \
    SL_MOP(if (_level_ <= SLDEBUG_OUTPUT) fprintf(SL_DEBUG_FSTREAM, "\n"); fflush(stdout); )
/*# define SL_DEBUG(_level_, _format_, _args_... ) \
    SL_MOP(if (_level_ <= SLDEBUG_OUTPUT) fprintf(SL_DEBUG_FSTREAM, MPI_STR "%s:%i:%s: " _format_ "\n", MPI_PARAM, __FILE__, __LINE__, __func__, ##_args_); fflush(stdout); )*/
# define SL_DEBUG(_level_, _format_, _args_... ) \
    ((_level_ <= SLDEBUG_OUTPUT)?(fprintf(SL_DEBUG_FSTREAM, MPI_STR "%s:%i:%s: " _format_ "\n", MPI_PARAM, __FILE__, __LINE__, __func__, ##_args_), fflush(SL_DEBUG_FSTREAM)):fflush(SL_DEBUG_FSTREAM))
# define SL_DEBUG_IF(_if_, _level_, _format_, _args_... ) \
    (((_if_) && (_level_ <= SLDEBUG_OUTPUT))?(fprintf(SL_DEBUG_FSTREAM, MPI_STR "%s:%i:%s: " _format_ "\n", MPI_PARAM, __FILE__, __LINE__, __func__, ##_args_), fflush(SL_DEBUG_FSTREAM)):fflush(SL_DEBUG_FSTREAM))
#else
# define SL_DEBUG_INTRO(_x_...)                  SL_NOP()
# define SL_DEBUG_CORE(_x_...)                   SL_NOP()
# define SL_DEBUG_OUTRO(_x_...)                  SL_NOP()
# define SL_DEBUG(_x_...)                        0
# define SL_DEBUG_IF(_x_...)                     0
#endif

#define SL_ASSERT(_x_)                           SL_MOP(if (_x_); else SL_DEBUG(0, "ASSERT: '%s' failed.", #_x_); )
#define SL_ASSERT_IF(_if_, _x_)                  SL_MOP(if (_if_) SL_ASSERT(_x_); )

#define SL_TRACE(_format_, _args_...)            SL_DEBUG(3, _format_, ##_args_)
#define SL_TRACE_IF(_if_, _format_, _args_...)   SL_DEBUG_IF(_if_, 3, _format_, ##_args_)

#define SL_TRACE_ARRAY(_f_, _e_, _i_, _n_, _a_, _args_...) \
   SL_MOP(SL_DEBUG_INTRO(3); \
          SL_DEBUG_CORE(3, _f_, ##_args_); \
          for (_i_ = 0; _i_ < _n_; ++_i_) SL_DEBUG_CORE(3, _e_, _a_[_i_]); \
          SL_DEBUG_OUTRO(3); )
#define SL_TRACE_ARRAY_IF(_if_, _f_, _e_, _i_, _n_, _a_, _args_...) SL_MOP(if (_if_) SL_TRACE_ARRAY(_f_, _e_, _i_, _n_, _a_, ##_args_); )


#endif /* __SL_DEBUG_H__ */
