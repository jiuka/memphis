#
# Process this file with autoconf to produce a configure script.
#
m4_define([memphis_major_version], [0])
m4_define([memphis_minor_version], [2])
m4_define([memphis_micro_version], [0])
m4_define([memphis_version],
          [memphis_major_version.memphis_minor_version.memphis_micro_version])
m4_define([api_version],
          [memphis_major_version.memphis_minor_version])

AC_PREREQ(2.62)
AC_INIT([memphis],
        [memphis_version],
        [http://trac.openstreetmap.ch/trac/memphis/])
AC_CONFIG_SRCDIR([src/memphis.h])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wno-portability])
AC_LIBTOOL_DLOPEN

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_LIBTOOL

CPPFLAGS+="-Wall -Werror"

# API_VERSION is used to generate pc file only
API_VERSION=api_version
AC_SUBST(API_VERSION)

# used for -version-info current[:revision[:age]]
# see http://www.gnu.org/software/libtool/manual/html_node/Link-mode.html
LIBRARY_VERSION=0:2:0
AC_SUBST(LIBRARY_VERSION)

AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

# check for gnome libraries
PKG_CHECK_MODULES(DEPS,
  [   glib-2.0 >= 2.16,
      gobject-2.0 >= 2.10,
      cairo >= 1.4
  ]
)

# link against libmath
DEPS_LIBS+="-lm"

# check for expat
AC_CHECK_LIB([expat], [XML_ParserCreate], [DEPS_LIBS="$DEPS_LIBS -lexpat"],
  [AC_MSG_ERROR([cannot find expat library])])
AC_CHECK_HEADER([expat.h], [],
  [AC_MSG_ERROR([cannot find expat.h header])])

# check for gtk-doc
GTK_DOC_CHECK(1.12)

# check for gobject-introspection
GOBJECT_INTROSPECTION_CHECK([0.6.7])

# -----------------------------------------------------------
# Enable debug
# -----------------------------------------------------------

AC_ARG_ENABLE(debug,
  AC_HELP_STRING([--enable-debug],
    [turn on debugging [default=no]]),[],[enable_debug="no"])
if test x$enable_debug = xyes; then
  AC_DEFINE(ENABLE_DEBUG, [], [Enable debug code])
fi

# -----------------------------------------------------------
AC_CONFIG_FILES([Makefile
                 src/Makefile
                 demos/Makefile
                 tests/Makefile
                 docs/reference/Makefile
                 memphis.pc
                ])
AC_OUTPUT

echo ""
echo " memphis $VERSION"
echo ""
echo "          Prefix: ${prefix}"
echo "  Compiler flags: ${CPPFLAGS}"
echo "           Debug: ${enable_debug}"
echo "   Documentation: ${enable_gtk_doc}"
echo "   Introspection: ${enable_introspection}"
echo ""

