#!/bin/sh
libtoolize --copy
aclocal -I m4
gtkdocize --copy || { echo "gtkdocize missing. Please install gtk-doc."; exit 1; }
autoconf
autoheader
automake --add-missing --copy
./configure "$@"
