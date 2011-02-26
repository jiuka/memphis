noinst_PROGRAMS = tile-renderer tile-server

INCLUDES = -I$(top_srcdir)
AM_CPPFLAGS = $(DEPS_CFLAGS)
AM_LDFLAGS = $(DEPS_LIBS) ../memphis/libmemphis-0.2.la

tile_renderer_SOURCES = tile-renderer.c

tile_server_SOURCES = tile-server.c

memphisrulesdir = $(datadir)/memphis
memphisrules_DATA = \
	default-rules.xml

EXTRA_DIST = \
	map.osm \
	default-rules.xml \
	javascript_seed_example.js
