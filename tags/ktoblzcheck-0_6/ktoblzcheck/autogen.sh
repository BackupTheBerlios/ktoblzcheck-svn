#!/bin/sh

echo aclocal...
aclocal
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
