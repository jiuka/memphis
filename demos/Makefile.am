noinst_PROGRAMS = tile-renderer

INCLUDES = -I$(top_srcdir)

AM_CPPFLAGS = $(DEPS_CFLAGS)
AM_LDFLAGS = $(DEPS_LIBS)

tile_renderer_SOURCES = tile-renderer.c
tile_renderer_LDADD = $(DEPS_LIBS) ../src/libmemphis-0.1.la

EXTRA_DIST = \
	map.osm \
	rule.xml
