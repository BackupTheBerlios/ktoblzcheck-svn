AUTOMAKE_OPTIONS = 1.4

bin_PROGRAMS = ktoblzcheck ctest

INCLUDES=-I../include
EXTRA_DIST =


ktoblzcheck_SOURCES = \
	ktoblzcheck.cc

ktoblzcheck_LDADD = -L../lib -lktoblzcheck @LIBS@

ctest_LDADD = -L../lib -lktoblzcheck @LIBS@

install:
	cp .libs/ktoblzcheck /usr/bin
