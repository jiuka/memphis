project('memphis', 'c', version: '0.2.4')

gnome = import('gnome')
pkg = import('pkgconfig')

cc = meson.get_compiler('c')

glib_dep = dependency('glib-2.0', version: '>=2.16')
gobject_dep = dependency('gobject-2.0', version: '>=2.10')
cairo_dep = dependency('cairo', version: '>=1.4')
expat_dep = dependency('expat')
m_dep = cc.find_library('m', required : false)

root_dir = include_directories('.')

subdir('memphis')
if get_option('demos')
    subdir('demos')
endif
subdir('tests')
if get_option('gtk_doc')
    subdir('docs/reference')
endif
