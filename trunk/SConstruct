# scons build file

env = Environment()
env.MergeFlags(['-Wall -Werror -lm -std=c99 -lexpat'])

if ARGUMENTS.get('optimize') in ('y', 'yes'):
    env.Append(CCFLAGS = ['-O2', '-fomit-frame-pointer'])
else:
    env.Append(CCFLAGS = ['-g'])

if ARGUMENTS.get('profile') in ('y', 'yes'):
    env.Append(CCFLAGS = ['-pg'])
    env.Append(LINKFLAGS = ['-pg'])

env.ParseConfig('pkg-config --cflags --libs cairo')
env.ParseConfig('pkg-config --cflags --libs glib-2.0')
env.ParseConfig('pkg-config --cflags --libs gobject-2.0')

conf = Configure(env)

if not conf.CheckLibWithHeader('expat','expat.h','C'):
    print 'Did not find expat, exiting!'
    Exit(1)

if not conf.CheckLibWithHeader('cairo','cairo.h','C'):
    print 'Did not find cairo, exiting!'
    Exit(1)

if not conf.CheckLibWithHeader('glib-2.0','glib.h','C'):
    print 'Did not find cairo, exiting!'
    Exit(1)

env = conf.Finish()

SOURCES = [
    'src/libmercator.c',
    'src/osm05.c',
    'src/ruleset.c',
    'src/textpath.c',
    'src/mlib.c',
    'src/memphis-data-pool.c',
    'src/memphis-debug.c',
    'src/memphis-map.c',
    'src/memphis-rule.c',
    'src/memphis-rule-set.c',
    'src/memphis-renderer.c'
]

Default(env.Program('memphis', source = ['src/main.c', SOURCES]))

env.Program('testTextPath', source=['test/testTextPath.c','src/textpath.c'])
env.Program('testSize', source=['test/testSize.c'])
env.Program('testOSM', source=['test/testOSM.c','osm05.c','src/mlib.c'])
env.Program('testRuleset', source=['test/testRuleset.c','src/ruleset.c','src/mlib.c'])

env.Program('tile-renderer', source = ['demos/tile-renderer.c', SOURCES])
