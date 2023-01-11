memphis_headers = files(
	'memphis-private.h',
	'libmercator.h',
	'ruleset.h',
	'textpath.h',
	'mlib.h',
	'osm05.h',
)

memphis_public_headers = files(
	'memphis.h',
	'memphis-data-pool.h',
	'memphis-debug.h',
	'memphis-map.h',
	'memphis-renderer.h',
	'memphis-rule.h',
	'memphis-rule-set.h',
)

memphis_sources = files(
	'libmercator.c',
	'mlib.c',
	'ruleset.c',
	'textpath.c',
	'osm05.c',
	'memphis-map.c',
	'memphis-rule.c',
	'memphis-rule-set.c',
	'memphis-renderer.c',
	'memphis-data-pool.c',
	'memphis-debug.c',
)

memphis_dir = include_directories('.')

memphis_public_deps = [
	glib_dep, gobject_dep, cairo_dep
]

memphis_deps = [
	memphis_public_deps,
	expat_dep,
	m_dep
]

memphis_lib = library(
	'memphis-0.2',
	memphis_sources,
	memphis_headers,
	memphis_public_headers,
	dependencies: memphis_deps,
	install: true
)

install_headers(memphis_public_headers, subdir : 'memphis')

pkg.generate(memphis_lib,
	filebase: 'memphis-0.2',
	name: 'Memphis',
	description: 'A map renderer for OpenStreetMap data',
	requires: memphis_public_deps
)

memphis_dep = declare_dependency(
	link_with: memphis_lib,
	include_directories: root_dir,
	dependencies: memphis_public_deps
)

if get_option('gir')
	memphis_gir = gnome.generate_gir(
		memphis_lib,
		namespace: 'Memphis',
		nsversion: '0.2',
		sources: [
			memphis_public_headers,
			memphis_sources
		],
		header: 'memphis/memphis.h',
		includes: ['GObject-2.0', 'GLib-2.0', 'cairo-1.0'],
		dependencies: memphis_deps,
		install: true
	)

	if get_option('vapi')
		gnome.generate_vapi(
			'memphis-0.2',
			sources: memphis_gir[0],
			packages: ['glib-2.0', 'gobject-2.0', 'cairo'],
			install: true
		)
	endif
endif

executable('memphis',
	'main.c',
	'main.h',
	dependencies: memphis_dep
)
