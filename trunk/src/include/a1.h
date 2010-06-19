/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#if !defined A1_H_INCLUDED
#define A1_H_INCLUDED

/* Keep C++ compilers from getting confused */
#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#define A1_SUCCESS 0
#define A1_ERROR   1

/* FIXME: Add doxygen comments here! */

int A1_Init(void);

int A1_Finalize(void);

/* FIXME: More API functions to come */

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* A1_H_INCLUDED */
