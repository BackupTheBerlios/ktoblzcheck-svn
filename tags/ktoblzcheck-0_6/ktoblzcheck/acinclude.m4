dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_namespaces.html
dnl
AC_DEFUN([AC_CXX_NAMESPACES],
[AC_CACHE_CHECK(whether the compiler implements namespaces,
ac_cv_cxx_namespaces,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([namespace Outer { namespace Inner { int i = 0; }}],
                [using namespace Outer::Inner; return i;],
 ac_cv_cxx_namespaces=yes, ac_cv_cxx_namespaces=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_namespaces" = yes; then
  AC_DEFINE(HAVE_NAMESPACES,,[define if the compiler implements namespaces])
fi
])


dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_have_ext_hash_map.html
dnl
AC_DEFUN([AC_CXX_HAVE_EXT_HASH_MAP],
[AC_CACHE_CHECK(whether the compiler has ext/hash_map,
ac_cv_cxx_have_ext_hash_map,
[AC_REQUIRE([AC_CXX_NAMESPACES])
  AC_LANG_SAVE
  AC_LANG_CPLUSPLUS
  AC_TRY_COMPILE([#include <ext/hash_map>
#ifdef HAVE_NAMESPACES
using namespace __gnu_cxx;
#endif],[hash_map<int, int> t; return 0;],
  ac_cv_cxx_have_ext_hash_map=yes, ac_cv_cxx_have_ext_hash_map=no)
  AC_LANG_RESTORE
])
if test "$ac_cv_cxx_have_ext_hash_map" = yes; then
   AC_DEFINE(HAVE_EXT_HASH_MAP,,[define if the compiler has ext/hash_map])
fi
])


# Compile warning arguments to ./configure
# by Christian Stimming <stimming@tuhh.de> 2003-11-19

dnl ACX_COMPILE_WARN()
dnl Add arguments for compile warnings and debug options to ./configure.
dnl
AC_DEFUN(ACX_COMPILE_WARN,
[

dnl For enabling of debugging flags/code
AC_ARG_ENABLE(debug,
  [  --enable-debug          compile with debugging code enabled, default=no],
  [case "${enableval}" in
     yes)   CXXFLAGS="${CXXFLAGS} -g"
	    CFLAGS="${CFLAGS} -g"
	    LDFLAGS="${LDFLAGS} -g"
	    AC_DEFINE(DEBUG,1,[Define if you want debugging code enabled.]) ;;
     no) ;;
     *) AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
   esac
  ], [
	# Default value if the argument was not given
	CXXFLAGS="${CXXFLAGS}"
	CFLAGS="${CFLAGS}"
	LDFLAGS="${LDFLAGS}"
])

dnl If this is gcc, then ...
if test ${GCC}x = yesx; then

  dnl Enable all warnings
  AC_ARG_ENABLE(warnings,
    [  --enable-warnings       enable compilation warnings, default=yes],
    [case "${enableval}" in
       yes) CXXFLAGS="${CXXFLAGS} -Wall -Wno-long-long -pedantic -ansi"
	    CFLAGS="${CFLAGS} -Wall -pedantic" ;;
       all) CXXFLAGS="${CXXFLAGS} -Wall -Wno-long-long -pedantic -ansi"
	    CFLAGS="${CFLAGS} -Wall -pedantic -ansi" ;;
       no) ;;
       *) AC_MSG_ERROR(bad value ${enableval} for --enable-warnings) ;;
     esac
  ], [ 
     # Default value if the argument was not given
     CXXFLAGS="${CXXFLAGS} -Wall -Wno-long-long" 
     CFLAGS="${CFLAGS} -Wall" 
  ])
  dnl For enabling error on warnings
  AC_ARG_ENABLE(error-on-warning,
    [  --enable-error-on-warning treat all compile warnings as errors, default=no],
    [case "${enableval}" in
       yes) CXXFLAGS="${CXXFLAGS} -Werror" 
	    CFLAGS="${CFLAGS} -Werror" ;;
       no) ;;
       *) AC_MSG_ERROR(bad value ${enableval} for --enable-error-on-warning) ;;
     esac
  ], [ 
     # Default value if the argument was not given
     CXXFLAGS="${CXXFLAGS}" 
     CFLAGS="${CFLAGS}" 
  ])
fi

])
