
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

libktoblzcheck_la_LDFLAGS = -no-undefined -version-info $(LIBKTOBLZCHECK_CURRENT):$(LIBKTOBLZCHECK_REVISION):$(LIBKTOBLZCHECK_AGE)
libktoblzcheck_la_LIBADD = -lstdc++

# Add check script for this package.
check_PROGRAMS = iban_check
iban_check_SOURCES = iban_check.cc
iban_check_LDADD = $(ktoblzcheck_libs)
TESTS = iban_check
TESTS_ENVIRONMENT = BANKDATA_SRCPATH=$(top_srcdir)/src/bankdata

