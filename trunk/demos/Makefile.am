noinst_PROGRAMS = tile-renderer

INCLUDES = -I$(top_srcdir)
AM_CPPFLAGS = $(DEPS_CFLAGS)
AM_LDFLAGS = $(DEPS_LIBS) ../memphis/libmemphis-0.2.la

tile_renderer_SOURCES = tile-renderer.c

EXTRA_DIST = \
	map.osm \
	rule.xml \
	javascript_seed_example.js
