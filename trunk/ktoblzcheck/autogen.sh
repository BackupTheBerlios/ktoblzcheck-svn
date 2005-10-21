#!/bin/sh -rx

# First cache the command names in variables. If you want to
# override the names, simply set the variables before calling this
# script.

#: ${INTLTOOLIZE=intltoolize}
#: ${GETTEXTIZE=gettextize}
: ${LIBTOOLIZE=libtoolize}
: ${ACLOCAL=aclocal}
: ${AUTOMAKE=automake}
: ${AUTOCONF=autoconf}
: ${AUTOHEADER=autoheader}

#${INTLTOOLIZE}                          || exit 1
#${GETTEXTIZE}                           || exit 1
${LIBTOOLIZE} -f --automake             || exit 1
${ACLOCAL} -I macros ${ACLOCAL_FLAGS}   || exit 1
${AUTOMAKE} --add-missing               || exit 1
${AUTOCONF}                             || exit 1
${AUTOHEADER}                           || exit 1

echo "Now you can run ./configure"
