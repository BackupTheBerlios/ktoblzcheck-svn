# Installation of the data file

# Have this installed in $prefix/ktoblzcheck
idatadir = $(BANKDATA_PATH)
dist_idata_DATA = bankdata.txt ibandata.txt

dist_pkgdata_SCRIPTS = \
  bundesbank.sed \
  online_update.pl

EXTRA_DIST = online_update.pl.in bundesbank.sed.in

CLEANFILES = online_update.pl bundesbank.sed
