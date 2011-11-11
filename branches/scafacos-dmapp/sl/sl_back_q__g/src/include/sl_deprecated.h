/*
 *  SL - Sorting Library, v0.1, (michael.hofmann@informatik.tu-chemnitz.de)
 *  
 *  file: src/include/sl_deprecated.h
 *  timestamp: 2010-09-27 14:04:04 +0200
 *  
 */


#ifndef __SL_DEPRECATED_H__
#define __SL_DEPRECATED_H__


/* sl_macro MPI_BINNING_REDUCEBCAST_THRESHOLD */
#ifdef MB_REDUCEBCAST_THRESHOLD
# define MPI_BINNING_REDUCEBCAST_THRESHOLD  MB_REDUCEBCAST_THRESHOLD
#endif


#endif /* __SL_DEPRECATED_H__ */
