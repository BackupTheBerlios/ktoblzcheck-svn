AUTOMAKE_OPTIONS = 1.4

bin_PROGRAMS = ktoblzcheck 

INCLUDES=-I../include
EXTRA_DIST=

ktoblzcheck_SOURCES = \
	ktoblzcheck.cc

ktoblzcheck_LDADD = -L../lib -lktoblzcheck @LIBS@

noinst_PROGRAMS = ctest benchmark
ctest_LDADD = -L../lib -lktoblzcheck @LIBS@
benchmark_LDADD = -L../lib -lktoblzcheck @LIBS@
benchmark_SOURCES = benchmark.cc
