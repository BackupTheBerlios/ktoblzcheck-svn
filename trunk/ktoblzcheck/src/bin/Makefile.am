# Automake variables for the various executables in this directory

# This program should be installed
bin_PROGRAMS = ktoblzcheck

# This programs should be built, but not be installed
noinst_PROGRAMS = ctest benchmark ibanchk

INCLUDES = -I$(top_srcdir)/src/include
LIBS = $(ktoblzcheck_libs)

# Specify the source files. 
ktoblzcheck_SOURCES = ktoblzcheck.cc
ibanchk_SOURCES = ibanchk.cc
benchmark_SOURCES = benchmark.cc
# ctest.c will be found automatically

# Add check script for this package.
TESTS = check_ktoblzcheck
TESTS_ENVIRONMENT = PATH=$${PATH}:$(top_builddir)/src/lib LD_LIBRARY_PATH=$(top_builddir)/src/lib BANKDATA_SRCPATH=$(top_srcdir)/src/bankdata
dist_check_SCRIPTS = check_ktoblzcheck
check_ktoblzcheck: ktoblzcheck
