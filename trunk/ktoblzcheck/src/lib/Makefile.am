
INCLUDES = -I$(top_srcdir)/src/include

#include_HEADERS = 

noinst_HEADERS = \
  algorithms.h

lib_LTLIBRARIES = libktoblzcheck.la

libktoblzcheck_la_SOURCES = \
  ktoblzcheck.cc \
  methods.cc \
  algorithms.cc \
  accnum.cc \
  iban.cc

libktoblzcheck_la_LDFLAGS = -version-info 3:0:2
libktoblzcheck_la_LIBADD = -lstdc++

# Add check script for this package.
check_PROGRAMS = iban_check
iban_check_SOURCES = iban_check.cc
iban_check_LDADD = $(ktoblzcheck_libs)
TESTS = iban_check
TESTS_ENVIRONMENT = BANKDATA_SRCPATH=$(top_srcdir)/src/bankdata

############################################################
# DLL settings: 
# Only these need to be modified

DLL_BASENAME = ktoblzcheck32

DLL_OBJS = $(libktoblzcheck_la_SOURCES:%.cc=%.o)
DLL_LDLIBS = -L/mingw/lib -lstdc++

# No changes should be necessary below
DLLTOOL=dlltool
DLLWRAP=dllwrap
WINDRES=windres

DLL_NAME = $(DLL_BASENAME).dll
DLL_EXP_LIB = $(DLL_BASENAME).lib
DLL_EXP_DEF = $(DLL_BASENAME).def

DLL_WRAP_FLAGS=--export-all --output-def $(DLL_EXP_DEF) \
	--implib $(DLL_EXP_LIB) \
	--driver-name $(CXX)

$(DLL_NAME) $(DLL_EXP_LIB) $(DLL_EXP_DEF): $(lib_LTLIBRARIES) $(DLL_OBJS) ressource.o exports.o
	rm -f $(DLL_NAME) $(DLL_EXP_LIB) $(DLL_EXP_DEF) >/dev/null
	$(DLLWRAP) $(DLL_WRAP_FLAGS) -o $(DLL_NAME) \
	    $(DLL_OBJS) ressource.o exports.o $(DLL_LDFLAGS) $(DLL_LDLIBS)

dll: $(DLL_NAME)

ressource.o: ressource.rc
	$(WINDRES) -i $< -o $@

exports.o: $(DLL_OBJS)
	$(DLLTOOL) -e exports.o $(DLL_OBJS)

if IS_WINDOWS
all-local: dll
endif

CLEANFILES = $(DLL_NAME) $(DLL_EXP_LIB) $(DLL_EXP_DEF)
