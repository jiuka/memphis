#!/bin/sh
libtoolize --copy
aclocal -I m4
gtkdocize --copy
autoconf
autoheader
automake --add-missing --copy
./configure "$@"
