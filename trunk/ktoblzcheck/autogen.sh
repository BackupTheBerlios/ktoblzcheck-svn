#!/bin/sh -x

# First cache the command names in variables. If you want to
# override the names, simply set the variables before calling this
# script.
set -e

#: ${INTLTOOLIZE=intltoolize}
#: ${GETTEXTIZE=gettextize}
: ${LIBTOOLIZE=libtoolize}
: ${ACLOCAL=aclocal}
: ${AUTOMAKE=automake}
: ${AUTOCONF=autoconf}
: ${AUTOHEADER=autoheader}

#${INTLTOOLIZE}
#${GETTEXTIZE}
${LIBTOOLIZE} -f --automake
${ACLOCAL} -I macros ${ACLOCAL_FLAGS}
${AUTOMAKE} --add-missing
${AUTOCONF}
${AUTOHEADER}

echo "Now you can run ./configure"
