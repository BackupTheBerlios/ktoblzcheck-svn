AUTOMAKE_OPTIONS = 1.4

bin_PROGRAMS = ktoblzcheck 

INCLUDES=-I../include
EXTRA_DIST=

ktoblzcheck_SOURCES = \
	ktoblzcheck.cc

ktoblzcheck_LDADD = -L../lib -lktoblzcheck @LIBS@

noinst_PROGRAMS = ctest
ctest_LDADD = -L../lib -lktoblzcheck @LIBS@
