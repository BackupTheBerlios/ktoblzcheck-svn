#!/bin/sh

echo libtoolize -f --automake
libtoolize -f --automake
echo aclocal -I macros...
aclocal -I macros
echo autoheader...
autoheader
echo automake -a...
automake -a
echo autoconf...
autoconf
echo
echo
echo configure $@
./configure $@
