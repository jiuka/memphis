#!/bin/sh
libtoolize --copy
aclocal
autoconf
autoheader
automake --add-missing --copy
./configure
