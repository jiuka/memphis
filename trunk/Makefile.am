SUBDIRS = src demos

ACLOCAL_AMFLAGS = -I m4

pcfiles = memphis-@API_VERSION@.pc

memphis-@API_VERSION@.pc: memphis.pc
	@cp -f $< $@

pkgconfig_DATA = $(pcfiles)
pkgconfigdir = $(libdir)/pkgconfig

CLEANFILES = $(pcfiles)
