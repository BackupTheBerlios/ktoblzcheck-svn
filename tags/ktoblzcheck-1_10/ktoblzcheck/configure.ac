# Input file to autoconf to produce a ./configure script

AC_INIT(README)
AC_CANONICAL_TARGET([])

#############################################################
#
# Version numbering
#
VERSION_MAJOR="1"
VERSION_MINOR="10"
rpm_release="1"
# no patchlevel here since this package is small enough to stick with
# two numbers.

AM_INIT_AUTOMAKE(ktoblzcheck, "${VERSION_MAJOR}.${VERSION_MINOR}")
AM_CONFIG_HEADER(config.h)

# shared library version numbers
LIBKTOBLZCHECK_CURRENT=4
LIBKTOBLZCHECK_AGE=3
LIBKTOBLZCHECK_REVISION=0
LIBKTOBLZCHECK_EFFECTIVE="`echo \$(($LIBKTOBLZCHECK_CURRENT-LIBKTOBLZCHECK_AGE))`"

AC_SUBST(VERSION_MAJOR)
AC_SUBST(VERSION_MINOR)
AC_SUBST(rpm_release)
# rpm_version is set by the VERSION in AM_INIT_AUTOMAKE
AC_SUBST(LIBKTOBLZCHECK_CURRENT)
AC_SUBST(LIBKTOBLZCHECK_AGE)
AC_SUBST(LIBKTOBLZCHECK_REVISION)
AC_SUBST(LIBKTOBLZCHECK_EFFECTIVE)

#############################################################
#
# Check for programs
#
AC_PREFIX_DEFAULT(/usr/local)
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP

AC_PATH_PROG([SED], [sed], [NO])
AC_PATH_PROG([LYNX], [lynx], [NO])
AC_PATH_PROG([GREP], [grep], [NO])
AC_PATH_PROG([PERL], [perl], [NO])
AC_PATH_PROG([RECODE], [recode], [NO])

# Check language features
AC_C_LONG_LONG

# Disable static libraries by default. Modern distros want the shared
# ones.
AC_DISABLE_STATIC

AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

#############################################################
#
# OS dependant settings
#
AQ_CHECK_OS

case "$OS_TYPE" in
    posix)
        ktoblzcheck_libs='-L$(top_builddir)/src/lib -lktoblzcheck'
	ktoblzcheck_includes='-I$(top_srcdir)/src/include'
    	;;
    windows)
	ACX_WINDOWS_PATHS
        ktoblzcheck_libs='-L$(top_builddir)/src/lib -lktoblzcheck -L/mingw/lib -L$(WIN_PATH_SYSTEM_MINGW)'
	ktoblzcheck_includes='-I$(top_srcdir)/src/include'
    	;;
esac
AC_SUBST(ktoblzcheck_libs)
AC_SUBST(ktoblzcheck_includes)

AM_CONDITIONAL(IS_WINDOWS, [test "$OS_TYPE" = "windows"])

#############################################################
#
# check path for installation of bank data
#
AC_MSG_CHECKING(bankdata path)
if test "x$prefix" != xNONE; then
   BANKDATA_PATH="${prefix}/share/${PACKAGE}"
else
   BANKDATA_PATH="${ac_default_prefix}/share/${PACKAGE}"
fi

AC_ARG_WITH(bankdata-path, 
  [  --with-bankdata-path=DIR where to store the bank data],
  [BANKDATA_PATH="$withval"])
AC_MSG_RESULT($BANKDATA_PATH)

# add some quotations, necessary for windows paths
BANKDATA_PATH="`echo $BANKDATA_PATH | sed 's:\\\\:\\\\\\\\:g'`"

AC_SUBST(BANKDATA_PATH)
AC_DEFINE_UNQUOTED(BANKDATA_PATH, "$BANKDATA_PATH", [Default path for bank data])

#############################################################
#
# python
AC_ARG_ENABLE(python,
  [  --enable-python         install python bindings (default=no)],
  [ case "${enableval}" in
    yes) enable_python="yes";;
    no)  enable_python="no";;
    *) AC_MSG_ERROR(bad value ${enableval} for --enable-python);;
    esac
  ],
  enable_python="yes")

# Check for python interpreter unless it is explicitly disabled
if test "x$enable_python" = "xyes"; then
  # Make sure at least version 2.3 is available (otherwise ctypes
  # won't work)
  AM_PATH_PYTHON( [2.3], [enable_python="yes"], [enable_python="no"] )
fi

AC_MSG_CHECKING(whether to install python-bindings)
if test "x$enable_python" = "xyes"; then
  AC_MSG_RESULT(yes)
  # Check for python ctypes module, otherwise terminate
  #AC_PYTHON_MODULE( [ctypes], [required] )
else
  AC_MSG_RESULT(no)
fi
AM_CONDITIONAL(PYTHON_USE, [test "$enable_python" = "yes"])


#############################################################
#
# Add compile switches
#
ACX_COMPILE_WARN

#############################################################
#
# Output
#
AC_CONFIG_FILES([
  Makefile
  Doxyfile
  ktoblzcheck.spec
  ktoblzcheck.iss
  ktoblzcheck.pc
  macros/Makefile
  src/Makefile
  src/bankdata/Makefile
  src/bankdata/online_update.pl
  src/bankdata/bundesbank.pl
  src/bin/Makefile
  src/include/Makefile
  src/include/ktoblzcheck.h
  src/lib/Makefile
  src/python/Makefile
  doc/Makefile
  doc/mainpage.doxygen
])
AC_OUTPUT
