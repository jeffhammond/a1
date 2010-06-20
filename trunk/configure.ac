dnl
dnl $Id: configure.in 16 2008-02-05 02:18:30Z balaji $
dnl
dnl Copyright (C) 2010 by Argonne National Laboratory.
dnl     See COPYRIGHT in top-level directory.
dnl

AC_PREREQ(2.62)

AC_INIT([a1],[0])
AC_CONFIG_AUX_DIR(m4)
AC_CONFIG_MACRO_DIR(m4)
AM_INIT_AUTOMAKE([-Wall -Werror foreign 1.11 silent-rules])

LT_PREREQ([2.2.6])

# Bug in libtool adds -O2 and -g by default
PAC_PUSH_VAR(CFLAGS)
LT_INIT(disable-shared)
PAC_POP_VAR(CFLAGS)

if test ! -s "$srcdir/maint/version" ; then
   AC_ERROR([Version information not found])
fi

# ABI version
liba1_abi_version=`cat $srcdir/maint/version | grep ^liba1_abi_version | cut -f3 -d' '`
AC_SUBST(liba1_abi_version)

# Release version
# Produce a numeric version assuming the following format:
# Version: [MAJ].[MIN].[REV][EXT][EXT_NUMBER]
# Example: 1.0.7rc1 has
#          MAJ = 1
#          MIN = 0
#          REV = 7
#          EXT = rc
#          EXT_NUMBER = 1
#
# Converting to numeric version will convert EXT to a format number:
#          ALPHA (a) = 0
#          BETA (b)  = 1
#          RC (rc)   = 2
#          PATCH (p) = 3
# Regular releases are treated as patch 0
#
# Numeric version will have 1 digit for MAJ, 2 digits for MIN,
# 2 digits for REV, 1 digit for EXT and 2 digits for EXT_NUMBER.
A1_VERSION=`cat $srcdir/maint/version | grep ^A1_VERSION | cut -f3 -d' '`
changequote(<<,>>)
V1=`expr $A1_VERSION : '\([0-9]*\)\.[0-9]*\.*[0-9]*[a-zA-Z]*[0-9]*'`
V2=`expr $A1_VERSION : '[0-9]*\.\([0-9]*\)\.*[0-9]*[a-zA-Z]*[0-9]*'`
V3=`expr $A1_VERSION : '[0-9]*\.[0-9]*\.*\([0-9]*\)[a-zA-Z]*[0-9]*'`
V4=`expr $A1_VERSION : '[0-9]*\.[0-9]*\.*[0-9]*\([a-zA-Z]*\)[0-9]*'`
V5=`expr $A1_VERSION : '[0-9]*\.[0-9]*\.*[0-9]*[a-zA-Z]*\([0-9]*\)'`
changequote([,])

if test "$V2" -le 9 ; then V2=0$V2 ; fi
if test "$V3" = "" ; then V3=0; fi
if test "$V3" -le 9 ; then V3=0$V3 ; fi
if test "$V4" = "a" ; then
    V4=0
elif test "$V4" = "b" ; then
    V4=1
elif test "$V4" = "rc" ; then
    V4=2
elif test "$V4" = "" ; then
    V4=3
    V5=0
elif test "$V4" = "p" ; then
    V4=3
fi
if test "$V5" -le 9 ; then V5=0$V5 ; fi

A1_NUMVERSION=`expr $V1$V2$V3$V4$V5 + 0`
AC_SUBST(A1_NUMVERSION)
AC_SUBST(A1_VERSION)

AC_CONFIG_HEADER(src/include/a1conf.h)
AH_TOP([/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2010 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
#ifndef A1CONF_H_INCLUDED
#define A1CONF_H_INCLUDED
])
AH_BOTTOM([#endif])

## Some of the autoconf macros automatically edit the CFLAGS and
## friends. We save and reset them to avoid this behavior. We want to
## control what flags get set.
PAC_PUSH_ALL_FLAGS
AC_PROG_CC
PAC_POP_ALL_FLAGS

PAC_ARG_STRICT
PAC_CC_FUNCTION_NAME_SYMBOL

## Check if the necessary headers are available
AC_CHECK_HEADERS(stdio.h stdlib.h string.h strings.h unistd.h stdarg.h sys/types.h \
		sys/stat.h time.h errno.h)

## Check what device we want to use
a1_device_dcmfd=false
AC_ARG_WITH(device, AC_HELP_STRING([--with-device],[A1 device to use]),
	a1_device=$withval,a1_device=dcmfd)
got_device=no
for dev in `echo $a1_device | sed -e 's/:/ /g' -e 's/,/ /g'` ; do
    case "$dev" in
    	 dcmfd)
		PAC_SET_HEADER_LIB_PATH(dcmf)
		PAC_CHECK_HEADER_LIB(dcmf,dcmf.h,libdcmf.cnk,DCMF_Messager_initialize,
				got_dcmf=yes,got_dcmf=no)
		if test "$got_dcmf" = "yes" ; then
		   a1_device_dcmfd=true
		   got_device=yes
		fi
		;;
	 *)
		;;
    esac
done
AM_CONDITIONAL([a1_device_dcmfd],[$a1_device_dcmfd])
if test "$got_device" = "no" ; then
   AC_ERROR([no valid device found])
fi

## Error checking functionality
AC_ARG_ENABLE(error-checking,
	AC_HELP_STRING([--enable-error-checking],[Enable error checking functionality]),
	enable_error_checking=$enableval,
	enable_error_checking=yes)
if test "$enable_error_checking" = "yes" ; then
   AC_DEFINE(ERROR_CHECKING,1,[Define if error checking is enabled])
fi

## Check if __VA_ARGS__ is defined by the compiler
AC_MSG_CHECKING([whether the compiler allows __VA_ARGS__])
rm -f conftest.c
cat > conftest.c <<EOF
#include <stdio.h>
#define my_print(...) printf(__VA_ARGS__)
int main()
{
    my_print("woohoo!");
    return 0;
}
EOF
pac_tmp_compile='$CC $CFLAGS conftest.c -o conftest'
if AC_TRY_EVAL(pac_tmp_compile) && test -s conftest ; then
   AC_DEFINE(COMPILER_ACCEPTS_VA_ARGS,1,[Define if compiler supports __VA_ARGS__])
   AC_MSG_RESULT(yes)
else
   AC_MSG_RESULT(no)
fi
rm -f conftest*

## Strict compilation
PAC_ARG_STRICT

## Debugging support
AC_ARG_ENABLE(g, AC_HELP_STRING([--enable-g],[Enable Debugging]),
                 [ debug=$enableval ],
                 [ debug=no ])
AC_MSG_CHECKING(debugging support)
AC_MSG_RESULT($debug)
if test "$debug" = "yes"; then
   CFLAGS="$CFLAGS -g"
   AC_DEFINE(ENABLE_DEBUG,1,[Define if debugging support is enabled])
fi

## Documentation
AC_PATH_PROG([DOXYGEN],[doxygen],,$PATH)
AC_SUBST(DOXYGEN)

AC_SUBST(top_srcdir)

## Final output
AC_OUTPUT(Makefile
	src/include/a1.h
	a1-doxygen.cfg
	)
