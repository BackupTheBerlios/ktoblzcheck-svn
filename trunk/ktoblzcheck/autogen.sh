#!/bin/sh

echo libtoolize
libtoolize -f --automake
echo aclocal...
aclocal -I macros
echo autoheader...
autoheader
echo automake...
automake -a
echo autoconf...
autoconf
echo
echo
echo configure $@
./configure $@
