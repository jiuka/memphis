SUBDIRS = src demos tests docs/reference

ACLOCAL_AMFLAGS = -I m4

DISTCHECK_CONFIGURE_FLAGS=--enable-gtk-doc

pcfiles = memphis-@API_VERSION@.pc

memphis-@API_VERSION@.pc: memphis.pc
	@cp -f $< $@

pkgconfig_DATA = $(pcfiles)
pkgconfigdir = $(libdir)/pkgconfig

CLEANFILES = $(pcfiles)
