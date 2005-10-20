#!/bin/sh -rx

# Make sure this script exits if a command was not found
trap exit ERR

libtoolize -f --automake
aclocal -I macros ${ACLOCAL_FLAGS}
autoheader
automake --add-missing
autoconf

echo "Now you can run ./configure"
