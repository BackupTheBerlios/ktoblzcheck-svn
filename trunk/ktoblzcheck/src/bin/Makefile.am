#AUTOMAKE_OPTIONS = 1.4

bin_PROGRAMS = ktoblzcheck 

INCLUDES = -I$(top_srcdir)/src/include
#EXTRA_DIST=

ktoblzcheck_SOURCES = \
  ktoblzcheck.cc

LIBS = -L$(top_builddir)/src/lib -lktoblzcheck

#ktoblzcheck_LDADD = 

noinst_PROGRAMS = ctest benchmark
#ctest_LDADD =
#benchmark_LDADD =
benchmark_SOURCES = benchmark.cc

# Add check script for 
TESTS = check_ktoblzcheck
TESTS_ENVIRONMENT = BANKDATA_SRCPATH=$(top_srcdir)/src/bankdata
dist_check_SCRIPTS = check_ktoblzcheck
check_ktoblzcheck: ktoblzcheck
