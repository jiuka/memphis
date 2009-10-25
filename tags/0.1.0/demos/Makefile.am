noinst_PROGRAMS = tile-renderer

INCLUDES = -I$(top_srcdir)
AM_CPPFLAGS = $(DEPS_CFLAGS)
AM_LDFLAGS = $(DEPS_LIBS) ../src/libmemphis-0.1.la

tile_renderer_SOURCES = tile-renderer.c

EXTRA_DIST = \
	map.osm \
	rule.xml
