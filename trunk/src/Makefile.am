AM_CPPFLAGS =		\
	$(DEPS_CFLAGS)		\
	-DDATADIR=\""$(datadir)"\"	\
	-I$(top_srcdir) \
	-std=c99

# memphis cli tool:
# switch to bin_PROGRAMS when mature
noinst_PROGRAMS = memphis

memphis_SOURCES = main.c main.h
memphis_LDADD = $(DEPS_LIBS) ./libmemphis-0.1.la

# libmemphis:
lib_LTLIBRARIES = libmemphis-0.1.la

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
	memphis-data-pool.c \
	memphis-debug.c
	
noinst_HEADERS = \
	memphis-private.h \
	libmercator.h \
	list.h \
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

libmemphis_0_1_la_LIBADD = $(DEPS_LIBS)

libmemphis_includedir = $(includedir)/libmemphis-0.1/memphis

libmemphis_0_1_la_LDFLAGS = -version-info $(LIBRARY_VERSION) \
	-export-symbols-regex		\
	^memphis_.*
