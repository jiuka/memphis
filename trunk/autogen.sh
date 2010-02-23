#!/bin/sh
libtoolize --copy
aclocal
gtkdocize --copy
autoconf
autoheader
automake --add-missing --copy
./configure "$@"
