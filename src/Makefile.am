INCLUDES = -I$(top_srcdir)

AM_CPPFLAGS = \
	$(DEPS_CFLAGS) \
	-DDATADIR=\""$(datadir)"\" \
	-std=c99

AM_LDFLAGS = $(DEPS_LIBS)

# memphis cli tool:
# switch to bin_PROGRAMS when mature
noinst_PROGRAMS = memphis

memphis_SOURCES = main.c main.h
memphis_LDADD = $(DEPS_LIBS) ./libmemphis-0.2.la

# libmemphis:
lib_LTLIBRARIES = libmemphis-0.2.la

libmemphis_headers = \
	memphis.h \
	memphis-data-pool.h \
	memphis-rule.h \
	memphis-rule-set.h \
	memphis-map.h \
	memphis-renderer.h \
	memphis-private.h \
	memphis-debug.h \
	libmercator.h \
	ruleset.h \
	textpath.h \
	mlib.h \
	osm05.h

libmemphis_0_2_la_SOURCES = \
	libmercator.c \
	mlib.c \
	ruleset.c \
	textpath.c \
	osm05.c \
	memphis-map.c \
	memphis-rule.c \
	memphis-rule-set.c \
	memphis-renderer.c \
	memphis-data-pool.c \
	memphis-debug.c
	
noinst_HEADERS = \
	memphis-private.h \
	libmercator.h \
	textpath.h \
	osm05.h \
	ruleset.h \
	mlib.h

libmemphis_include_HEADERS = \
	memphis.h \
	memphis-data-pool.h \
	memphis-debug.h \
	memphis-rule.h \
	memphis-rule-set.h \
	memphis-map.h \
	memphis-renderer.h

libmemphis_0_2_la_LIBADD = $(DEPS_LIBS)

libmemphis_includedir = $(includedir)/libmemphis-0.2/memphis

libmemphis_0_2_la_LDFLAGS = \
	-version-info $(LIBRARY_VERSION) \
	-export-symbols-regex \
	^memphis_.*

CLEANFILES =


# gobject-introspection support:
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
introspection_sources = \
	$(libmemphis_include_HEADERS) \
	memphis-map.c \
	memphis-rule.c \
	memphis-rule-set.c \
	memphis-renderer.c \
	memphis-data-pool.c \
	memphis-debug.c

Memphis-0.2.gir: libmemphis-0.2.la
Memphis_0_2_gir_INCLUDES = GObject-2.0 GLib-2.0 cairo-1.0
Memphis_0_2_gir_CFLAGS = $(DEPS_CFLAGS)
Memphis_0_2_gir_LIBS = libmemphis-0.2.la
Memphis_0_2_gir_FILES = $(addprefix $(srcdir)/,$(introspection_sources))
INTROSPECTION_GIRS += Memphis-0.2.gir

girdir = $(datadir)/gir-1.0
dist_gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(dist_gir_DATA) $(typelib_DATA)
endif
