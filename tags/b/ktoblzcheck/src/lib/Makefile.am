AUTOMAKE_OPTIONS = 1.4

INCLUDES=-I../include

iheaderdir=${includedir}/ktoblzcheck

lib_LTLIBRARIES=libktoblzcheck.la

libktoblzcheck_la_SOURCES= \
	ktoblzcheck.cc

libktoblzcheck_la_LDFLAGS= -version-info 1:0:0
libktoblzcheck_la_LIBADD= @LIBS@ -lstdc++
include_HEADERS = 

clean-local:
	rm -f *.def
	rm -f *.dll
