AUTOMAKE_OPTIONS = 1.4

bin_PROGRAMS = ktoblzcheck

INCLUDES=-I../include
EXTRA_DIST =
CPPFLAGS=@CPPFLAGS@
CFLAGS=@CFLAGS@
CXXFLAGS=@CXXFLAGS@


ktoblzcheck_SOURCES = \
	ktoblzcheck.cc

ktoblzcheck_LDADD = -L../lib -lktoblzcheck @LIBS@

install:
	cp .libs/ktoblzcheck /usr/bin
