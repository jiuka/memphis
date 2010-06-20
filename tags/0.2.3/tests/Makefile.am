check_PROGRAMS = unit-tests

TESTS = $(check_PROGRAMS)

INCLUDES = -I$(top_srcdir)
AM_CPPFLAGS = $(DEPS_CFLAGS)
AM_LDFLAGS = $(DEPS_LIBS) ../memphis/libmemphis-0.2.la

unit_tests_SOURCES = unit-tests.c
