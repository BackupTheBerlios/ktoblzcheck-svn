#!/bin/sh -rx

# Make sure this script exits if a command was not found
trap exit ERR

: ${LIBTOOLIZE=libtoolize}
: ${ACLOCAL=aclocal}
: ${AUTOMAKE=automake}
: ${AUTOHEADER=autoheader}
: ${AUTOCONF=autoconf}

${LIBTOOLIZE} -f --automake
${ACLOCAL} -I macros ${ACLOCAL_FLAGS}
${AUTOMAKE} --add-missing
${AUTOHEADER}
${AUTOCONF}

echo "Now you can run ./configure"
