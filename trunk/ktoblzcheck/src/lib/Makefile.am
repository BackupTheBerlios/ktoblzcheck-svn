
INCLUDES = -I$(top_srcdir)/src/include

#include_HEADERS = 

noinst_HEADERS = \
  algorithms.h

lib_LTLIBRARIES = libktoblzcheck.la

libktoblzcheck_la_SOURCES= \
  ktoblzcheck.cc \
  algorithms.cc \
  accnum.cc \
  iban.cc

libktoblzcheck_la_LDFLAGS = -version-info 1:3:0
libktoblzcheck_la_LIBADD = -lstdc++

#
# dll stuff
#

DLLTOOL = dlltool
DLLWRAP = dllwrap
WINDRES = windres

DLL_NAME = ktoblzcheck32.dll
DLL_EXP_LIB = ktoblzcheck32.lib
DLL_EXP_DEF = ktoblzcheck32.def
DLL_OBJS = \
  .libs/ktoblzcheck.o \
  .libs/algorithms.o \
  .libs/accnum.o 


DLL_LDLIBS = -L/mingw/lib -lstdc++
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
