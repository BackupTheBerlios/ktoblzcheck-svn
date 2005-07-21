#!/bin/sh -x

libtoolize -f --automake
aclocal -I macros ${ACLOCAL_FLAGS}
autoheader
automake --add-missing
autoconf

echo "Now you can run ./configure"
