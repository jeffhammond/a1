##
## This code has been borrowed from MPICH2
## (http://www.mcs.anl.gov/research/projects/mpich2) which is released
## under the BSD license
##

dnl/*D
dnl PAC_C_CHECK_COMPILER_OPTION - Check that a compiler option is accepted
dnl without warning messages
dnl
dnl Synopsis:
dnl PAC_C_CHECK_COMPILER_OPTION(optionname,action-if-ok,action-if-fail)
dnl
dnl Output Effects:
dnl
dnl If no actions are specified, a working value is added to 'COPTIONS'
dnl
dnl Notes:
dnl This is now careful to check that the output is different, since 
dnl some compilers are noisy.
dnl 
dnl We are extra careful to prototype the functions in case compiler options
dnl that complain about poor code are in effect.
dnl
dnl Because this is a long script, we have ensured that you can pass a 
dnl variable containing the option name as the first argument.
dnl
dnl gcc 4.2.4 on 32-bit does not complain about the -Wno-type-limits option 
dnl even though it doesn't support it.  However, when another warning is 
dnl triggered, it gives an error that the option is not recognized.  So we 
dnl need to test with a conftest file that will generate warnings
dnl D*/
AC_DEFUN([PAC_C_CHECK_COMPILER_OPTION],[
AC_MSG_CHECKING([whether C compiler accepts option $1])
pccco_save_CFLAGS="$CFLAGS"
CFLAGS="$1 $CFLAGS"
rm -f conftest.out
pac_success=no
# conftest3.c has an invalid prototype to ensure we generate warnings
echo 'int main(){}' > conftest3.c
echo 'int foo(void);int foo(void){return 0;}' > conftest2.c
echo 'int main(void);int main(void){return 0;}' > conftest.c
if ${CC-cc} $CFLAGS $CPPFLAGS -o conftest conftest3.c $LDFLAGS >/dev/null 2>&1 &&
   ${CC-cc} $pccco_save_CFLAGS $CPPFLAGS -o conftest conftest.c $LDFLAGS >conftest.bas 2>&1 ; then
   if ${CC-cc} $CFLAGS $CPPFLAGS -o conftest conftest.c $LDFLAGS >conftest.out 2>&1 ; then
      if diff -b conftest.out conftest.bas >/dev/null 2>&1 ; then
         AC_MSG_RESULT(yes)
         AC_MSG_CHECKING([whether routines compiled with $1 can be linked with ones compiled without $1])       
         rm -f conftest.out
         rm -f conftest.bas
         if ${CC-cc} -c $pccco_save_CFLAGS $CPPFLAGS conftest2.c >conftest2.out 2>&1 ; then
            if ${CC-cc} $CFLAGS $CPPFLAGS -o conftest conftest2.o conftest.c $LDFLAGS >conftest.bas 2>&1 ; then
               if ${CC-cc} $CFLAGS $CPPFLAGS -o conftest conftest2.o conftest.c $LDFLAGS >conftest.out 2>&1 ; then
                  if diff -b conftest.out conftest.bas >/dev/null 2>&1 ; then
		     pac_success=yes
                  else
		     :
                  fi  
               else
                  :
               fi
	    else
               # Could not link with the option!
	       :
            fi
         else
            if test -s conftest2.out ; then
               cat conftest2.out >&AC_FD_CC
            fi
         fi
      else
         :
      fi
   else
       :
   fi
else
    # Could not compile without the option!
    :
fi
CFLAGS="$pccco_save_CFLAGS"
if test "$pac_success" = yes ; then
   AC_MSG_RESULT(yes)	  
   ifelse($2,,COPTIONS="$COPTIONS $1",$2)
else
   AC_MSG_RESULT(no)
   if test -s conftest.out ; then cat conftest.out >&AC_FD_CC ; fi    
   $3
fi
# This is needed for Mac OSX 10.5
rm -rf conftest.dSYM
rm -f conftest*
])


dnl Use the value of enable-strict to update CFLAGS
dnl pac_cc_strict_flags contains the strict flags.
dnl
dnl -std=c89 is used to select the C89 version of the ANSI/ISO C standard.
dnl As of this writing, many C compilers still accepted only this version,
dnl not the later C99 version. When all compilers accept C99, this 
dnl should be changed to the appropriate standard level.  Note that we've
dnl had trouble with gcc 2.95.3 accepting -std=c89 but then trying to 
dnl compile program with a invalid set of options 
dnl (-D __STRICT_ANSI__-trigraphs)
AC_DEFUN([PAC_CC_STRICT],[
export enable_strict_done
if test "$enable_strict_done" != "yes" ; then

    # Some comments on strict warning options.
    # These were added to reduce warnings:
    #   -Wno-missing-field-initializers  -- We want to allow a struct to be 
    #       initialized to zero using "struct x y = {0};" and not require 
    #       each field to be initialized individually.
    #   -Wno-unused-parameter -- For portability, some parameters go unused
    #	    when we have different implementations of functions for 
    #	    different platforms
    #   -Wno-unused-label -- We add fn_exit: and fn_fail: on all functions, 
    #	    but fn_fail may not be used if the function doesn't return an 
    #	    error.
    #   -Wno-sign-compare -- read() and write() return bytes read/written
    #       as a signed value, but we often compare this to size_t (or
    #	    msg_sz_t) variables.
    #   -Wno-format-zero-length -- this warning is irritating and useless, since
    #                              a zero-length format string is very well defined
    #   -Wno-type-limits -- There are places where we compare an unsigned to 
    #	    a constant that happens to be zero e.g., if x is unsigned and 
    #	    MIN_VAL is zero, we'd like to do "MPIU_Assert(x >= MIN_VAL);".
    #       Note this option is not supported by gcc 4.2.  This needs to be added 
    #	    after most other warning flags, so that we catch a gcc bug on 32-bit 
    #	    that doesn't give a warning that this is unsupported, unless another
    #	    warning is triggered, and then if gives an error.
    # These were removed to reduce warnings:
    #   -Wcast-qual -- Sometimes we need to cast "volatile char*" to 
    #	    "char*", e.g., for memcpy.
    #   -Wpadded -- We catch struct padding with asserts when we need to
    #   -Wredundant-decls -- Having redundant declarations is benign and the 
    #	    code already has some.
    #   -Waggregate-return -- This seems to be a performance-related warning
    #       aggregate return values are legal in ANSI C, but they may be returned
    #	    in memory rather than through a register.  We do use aggregate return
    #	    values, but they are structs of a single basic type (used to enforce
    #	    type checking for relative vs. absolute ptrs), and with optimization
    #	    the aggregate value is converted to a scalar.
    # the embedded newlines in this string are safe because we evaluate each
    # argument in the for-loop below and append them to the CFLAGS with a space
    # as the separator instead
    pac_common_strict_flags="
        -Wall
        -Wextra
        -Wno-missing-field-initializers
        -Wstrict-prototypes
        -Wmissing-prototypes
        -DGCC_WALL
        -Wno-unused-parameter
        -Wno-unused-label
        -Wshadow
        -Wmissing-declarations
        -Wno-long-long
        -Wfloat-equal
        -Wdeclaration-after-statement
        -Wundef
        -Wno-endif-labels
        -Wpointer-arith
        -Wbad-function-cast
        -Wcast-align
        -Wwrite-strings
        -Wno-sign-compare
        -Wold-style-definition
        -Wno-multichar
        -Wno-deprecated-declarations
        -Wpacked
        -Wnested-externs
        -Winvalid-pch
        -Wno-pointer-sign
        -Wvariadic-macros
        -std=c89
        -Wno-format-zero-length
	-Wno-type-limits
    "
    pac_cc_strict_flags=""
    case "$1" in 
        yes|all|posix)
		enable_strict_done="yes"
		pac_cc_strict_flags="-O2 $pac_common_strict_flags -D_POSIX_C_SOURCE=199506L"
        ;;

        noposix)
		enable_strict_done="yes"
		pac_cc_strict_flags="-O2 $pac_common_strict_flags"
        ;;

	noopt)
		enable_strict_done="yes"
		pac_cc_strict_flags="$pac_common_strict_flags -D_POSIX_C_SOURCE=199506L"
	;;
        
        no)
		# Accept and ignore this value
		:
        ;;

        *)
		if test -n "$1" ; then
		   AC_MSG_WARN([Unrecognized value for enable-strict:$1])
		fi
        ;;

    esac

    # See if the above options work with the compiler
    accepted_flags=""
    for flag in $pac_cc_strict_flags ; do
        # the save_CFLAGS variable must be namespaced, otherwise they
        # may not actually be saved if an invoked macro also uses
        # save_CFLAGS
        pcs_save_CFLAGS=$CFLAGS
	CFLAGS="$CFLAGS $accepted_flags"
	PAC_C_CHECK_COMPILER_OPTION($flag,accepted_flags="$accepted_flags $flag",)
        CFLAGS=$pcs_save_CFLAGS
    done
    pac_cc_strict_flags=$accepted_flags
fi
])

dnl/*D
dnl PAC_ARG_STRICT - Add --enable-strict to configure.  
dnl
dnl Synopsis:
dnl PAC_ARG_STRICT
dnl 
dnl Output effects:
dnl Adds '--enable-strict' to the command line.
dnl
dnl D*/
AC_DEFUN([PAC_ARG_STRICT],[
	AC_ARG_ENABLE(strict,
	[--enable-strict  - Turn on strict compilation testing])
	PAC_CC_STRICT($enable_strict)
	CFLAGS="$CFLAGS $pac_cc_strict_flags"
	export CFLAGS
])

# determine if the compiler defines a symbol containing the function name
# Inspired by checks within the src/mpid/globus/configure.in file in MPICH2
#
# These tests check not only that the compiler defines some symbol, such
# as __FUNCTION__, but that the symbol correctly names the function.
#
# Defines 
#   HAVE__FUNC__      (if __func__ defined)
#   HAVE_CAP__FUNC__  (if __FUNC__ defined)
#   HAVE__FUNCTION__  (if __FUNCTION__ defined)
#
AC_DEFUN([PAC_CC_FUNCTION_NAME_SYMBOL],[
AC_CACHE_CHECK([whether the compiler defines __func__],
pac_cv_have__func__,[
tmp_am_cross=no
AC_RUN_IFELSE([
#include <string.h>
int foo(void);
int foo(void)
{
    return (strcmp(__func__, "foo") == 0);
}
int main(int argc, char ** argv)
{
    return (foo() ? 0 : 1);
}
], pac_cv_have__func__=yes, pac_cv_have__func__=no,tmp_am_cross=yes)
if test "$tmp_am_cross" = yes ; then
    AC_LINK_IFELSE([
#include <string.h>
int foo(void);
int foo(void)
{
    return (strcmp(__func__, "foo") == 0);
}
int main(int argc, char ** argv)
{
    return (foo() ? 0 : 1);
}
], pac_cv_have__func__=yes, pac_cv_have__func__=no)
fi
])

if test "$pac_cv_have__func__" = "yes" ; then
    AC_DEFINE(HAVE__FUNC__,,[define if the compiler defines __func__])
fi

AC_CACHE_CHECK([whether the compiler defines __FUNC__],
pac_cv_have_cap__func__,[
tmp_am_cross=no
AC_RUN_IFELSE([
#include <string.h>
int foo(void);
int foo(void)
{
    return (strcmp(__FUNC__, "foo") == 0);
}
int main(int argc, char ** argv)
{
    return (foo() ? 0 : 1);
}
], pac_cv_have_cap__func__=yes, pac_cv_have_cap__func__=no,tmp_am_cross=yes)
if test "$tmp_am_cross" = yes ; then
    AC_LINK_IFELSE([
#include <string.h>
int foo(void);
int foo(void)
{
    return (strcmp(__FUNC__, "foo") == 0);
}
int main(int argc, char ** argv)
{
    return (foo() ? 0 : 1);
}
], pac_cv_have__func__=yes, pac_cv_have__func__=no)
fi
])

if test "$pac_cv_have_cap__func__" = "yes" ; then
    AC_DEFINE(HAVE_CAP__FUNC__,,[define if the compiler defines __FUNC__])
fi

AC_CACHE_CHECK([whether the compiler sets __FUNCTION__],
pac_cv_have__function__,[
tmp_am_cross=no
AC_RUN_IFELSE([
#include <string.h>
int foo(void);
int foo(void)
{
    return (strcmp(__FUNCTION__, "foo") == 0);
}
int main(int argc, char ** argv)
{
    return (foo() ? 0 : 1);
}
], pac_cv_have__function__=yes, pac_cv_have__function__=no,tmp_am_cross=yes)
if test "$tmp_am_cross" = yes ; then
    AC_LINK_IFELSE([
#include <string.h>
int foo(void);
int foo(void)
{
    return (strcmp(__FUNCTION__, "foo") == 0);
}
int main(int argc, char ** argv)
{
    return (foo() ? 0 : 1);
}
], pac_cv_have__func__=yes, pac_cv_have__func__=no)
fi
])

if test "$pac_cv_have__function__" = "yes" ; then
    AC_DEFINE(HAVE__FUNCTION__,,[define if the compiler defines __FUNCTION__])
fi

])
