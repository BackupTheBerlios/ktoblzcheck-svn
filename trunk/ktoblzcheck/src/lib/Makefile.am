AUTOMAKE_OPTIONS = 1.4

INCLUDES=-I../include

iheaderdir=${includedir}
include_HEADERS = 

lib_LTLIBRARIES=libktoblzcheck.la

libktoblzcheck_la_SOURCES= \
	ktoblzcheck.cc \
	accnum.cc 
libktoblzcheck_la_LDFLAGS= -version-info 1:0:0
libktoblzcheck_la_LIBADD= @LIBS@ -lstdc++

#ktoblzcheck.cc: data.cc
#data.cc: ../bankdata/bankdata.txt
#	( cd ../bankdata ; sh transform.sh >../lib/data.cc )
# - disable because data.cc isn't used anyway after compile-into-library was disabled