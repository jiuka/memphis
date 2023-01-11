
glib_prefix  = glib_dep.get_pkgconfig_variable('prefix')
glib_docpath = join_paths(glib_prefix, 'share', 'gtk-doc', 'html')
docpath = join_paths(get_option('datadir'), 'gtk-doc', 'html')


gnome.gtkdoc(
    'memphis',
    src_dir: memphis_dir,
    install: true,
    main_sgml: 'libmemphis-docs.sgml',
    dependencies: memphis_deps,
    include_directories: root_dir,
    fixxref_args: [
        '--html-dir=@0@'.format(docpath),
        '--extra-dir=@0@'.format(join_paths(glib_docpath, 'glib')),
        '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gobject')),
        '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gio')),
    ],
    ignore_headers: [
        'libmercator.h',
        'main.h',
        'memphis-private.h',
        'mlib.h',
        'osm05.h',
        'ruleset.h',
        'textpath.h'
    ]
)
