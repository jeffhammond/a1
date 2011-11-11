#define FMM_PARALLEL
#define FMM_INFO
#define FMM_CPUTIME
!c#define FMM_WALLTIME
#define FMM_STATISTICS
!c#define FMM_DAMPING
!c#define FMM_DAMPING_PP_DIPOLEMOMENTS
!c#define FMM_DAMPING_SHQEN
!c#define FMM_NOPOT
!c#define FMM_NOGRAD
!c#define FMM_IBOXSCR
#define FMM_SORTMEMORY
!c#define FMM_SORTHD
!c#define FMM_SORTHDM
!c#define FMM_MULTIPOLEMOMENTS
!c#define FMM_ALLOCATION
!c#define FMM_ALLOCATIONALL
!c#define FMM_NOFUNCTIONPOINTER
!c#define FMM_TREETOGRAD
!c#define FMM_EXTREMETREETOGRAD
!c#define FMM_COMPRESSION
!c#define FMM_EXTREMECOMPRESSION
!c#define FMM_EXTREMEEXTREMECOMPRESSION
!c#define FMM_SIGNEXPONENT
#define FMM_ISO_C_BINDING
!c#define FMM_CORRECTION_OF_FORCES
!c#define FMM_UNIFORMGRID
!c#define FMM_LOADSORT
!c#define FMM_DEBUG
!c#define FMM_PASS3IJKB
!c#define FMM_IBOXUPD3
!c#define FMM_GETDIST
!c#define FMM_NOTIFY
!c#define FMM_FRONTTOBACK
!c
#define FMM_REAL 4
#define FMM_REAL_EXTENDED 8
#define FMM_REAL_ITOR 4
#define FMM_INTEGER 8
#define FMM_LOGICAL 8
#define FMM_TESTALLOC_INTEGER 8
#if FMM_INTEGER == 8
#define FMM_C_INTEGER c_long_long
#elif FMM_INTEGER == 4
#define FMM_C_INTEGER c_int
#else
#error "fmm integer type not supported."
#endif
#ifdef FMM_PARALLEL
#define FMM_MP_REAL_MAX 8
#define FMM_MP_INTEGER_PROCESSES 4
!c maximum number of processes = 2**(8*FMM_MP_INTEGER_PROCESSES-1)
!c ======================================================================
!c | FMM_MP_INTEGER_PROCESSES |       maximum number of processes       |
!c ======================================================================
!c |             1            |                   128                   |
!c |             2            |                  32768                  |
!c |             4            |                2147483648               |
!c |             8            |           9223372036854775808           |
!c |            16            | 170141183460469231731687303715884105728 |
!c ======================================================================
#endif
!c
!c ========================================================
!c | ilinearpotential | neg.&neg. | pos.&pos. | neg.&pos. |
!c ========================================================
!c |      0 (000)     |    off    |    off    |    off    |
!c |      1 (001)     |    off    |    off    |     on    |
!c |      2 (010)     |    off    |     on    |    off    |
!c |      3 (011)     |    off    |     on    |     on    |
!c |      4 (100)     |     on    |    off    |    off    |
!c |      5 (101)     |     on    |    off    |     on    |
!c |      6 (110)     |     on    |     on    |    off    |
!c |      7 (111)     |     on    |     on    |     on    |
!c ========================================================
!c
