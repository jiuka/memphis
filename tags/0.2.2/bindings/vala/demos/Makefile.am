
bin_PROGRAMS = example

example_SOURCES = \
	example.vala

VALAFLAGS = \
	--vapidir=../ \
	@VALA_EXAMPLE_PACKAGES@

INCLUDES = \
	$(VALA_EXAMPLE_CFLAGS) \
	-I$(top_srcdir)

example_LDADD = \
	$(VALA_EXAMPLE_LIBS) \
	$(top_srcdir)/memphis/libmemphis-0.2.la

CLEANFILES = \
	example.c \
	example_vala.stamp
