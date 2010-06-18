/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#if !defined ARMCIU_H_INCLUDED
#define ARMCIU_H_INCLUDED

#include "armciconf.h"

/* FIXME: FUNC_ENTER/EXIT can be used for profiling in the future */

#define ARMCIU_FUNC_ENTER(...)
#define ARMCIU_FUNC_EXIT(...)

#if defined HAVE__FUNC__
#define HYDU_FUNC __func__
#elif defined HAVE_CAP__FUNC__
#define HYDU_FUNC __FUNC__
#elif defined HAVE__FUNCTION__
#define HYDU_FUNC __FUNCTION__
#endif

#if defined __FILE__ && defined HYDU_FUNC
#define HYDU_error_printf(...)                                          \
    {                                                                   \
        fprintf(stderr, "%s (%s:%d): ", HYDU_FUNC, __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                                   \
    }
#elif defined __FILE__
#define HYDU_error_printf(...)                               \
    {                                                        \
        fprintf(stderr, "%s (%d): ", __FILE__, __LINE__);    \
        fprintf(stderr, __VA_ARGS__);                        \
    }
#else
#define HYDU_error_printf(...)                                          \
    {                                                                   \
        fprintf(stderr, __VA_ARGS__);                                   \
    }
#endif

#define HYDU_ASSERT(x, status)                                          \
    {                                                                   \
        if (!(x)) {                                                     \
            HYDU_ERR_SETANDJUMP(status, HYD_INTERNAL_ERROR,             \
                                 "assert (%s) failed\n", #x);           \
        }                                                               \
    }

#define HYDU_ERR_POP(status, ...)                                       \
    {                                                                   \
        if (status && !HYD_SILENT_ERROR(status)) {                      \
            HYDU_error_printf(__VA_ARGS__);                             \
            goto fn_fail;                                               \
        }                                                               \
        else if (HYD_SILENT_ERROR(status)) {                            \
            goto fn_exit;                                               \
        }                                                               \
    }

#define HYDU_ERR_SETANDJUMP(status, error, ...)                         \
    {                                                                   \
        status = error;                                                 \
        HYDU_ERR_POP(status, __VA_ARGS__);                              \
    }

#define HYDU_ERR_CHKANDJUMP(status, chk, error, ...)                    \
    {                                                                   \
        if ((chk))                                                      \
            HYDU_ERR_SETANDJUMP(status, error, __VA_ARGS__);            \
    }

#endif /* ARMCIU_H_INCLUDED */
