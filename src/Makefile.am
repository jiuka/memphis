
lib_LTLIBRARIES = libmemphis-0.1.la

libmemphis_headers = \
	memphis.h \
	memphis-data-pool.h \
	memphis-rule.h \
	memphis-rule-set.h \
	memphis-map.h \
	memphis-renderer.h \
	libmercator.h \
	ruleset.h \
	list.h \
	textpath.h \
	mlib.h \
	osm05.h

libmemphis_0_1_la_SOURCES = \
	libmercator.c \
	mlib.c \
	ruleset.c \
	textpath.c \
	osm05.c \
	memphis-map.c \
	memphis-rule.c \
	memphis-rule-set.c \
	memphis-renderer.c \
	memphis-data-pool.c
	
noinst_HEADERS = \
	libmercator.h \
	list.h \
	textpath.h \
	mlib.h

libmemphis_include_HEADERS = \
	memphis.h \
	memphis-data-pool.h \
	memphis-rule.h \
	memphis-rule-set.h \
	memphis-map.h \
	memphis-renderer.h \
	osm05.h \
	ruleset.h

libmemphis_0_1_la_LIBADD = $(DEPS_LIBS)

libmemphis_includedir = $(includedir)/libmemphis-0.1/memphis

libmemphis_0_1_la_LDFLAGS = -version-info $(LIBRARY_VERSION) \
	-export-symbols-regex		\
	^memphis_.*

AM_CPPFLAGS = 				\
	$(DEPS_CFLAGS)			\
	-DDATADIR=\""$(datadir)"\"	\
	-I$(top_srcdir) \
	-std=c99
