SUBDIRS = memphis demos tests docs/reference bindings

ACLOCAL_AMFLAGS = -I m4

DISTCHECK_CONFIGURE_FLAGS=--enable-gtk-doc --enable-introspection

pcfiles = memphis-@API_VERSION@.pc

memphis-@API_VERSION@.pc: memphis.pc
	@cp -f $< $@

pkgconfig_DATA = $(pcfiles)
pkgconfigdir = $(libdir)/pkgconfig

CLEANFILES = $(pcfiles)

EXTRA_DIST = \
	m4/introspection.m4 \
	m4/gtk-doc.m4 \
	MAINTAINERS
