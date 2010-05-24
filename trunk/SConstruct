# scons build file

env = Environment()
env.MergeFlags(['-Wall -Werror -lm -std=c99 -lexpat'])
env.Append(CCFLAGS = ['-I.'])

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
    'memphis/libmercator.c',
    'memphis/osm05.c',
    'memphis/ruleset.c',
    'memphis/textpath.c',
    'memphis/mlib.c',
    'memphis/memphis-data-pool.c',
    'memphis/memphis-debug.c',
    'memphis/memphis-map.c',
    'memphis/memphis-rule.c',
    'memphis/memphis-rule-set.c',
    'memphis/memphis-renderer.c',
]

Default(env.Program('memphis/memphis', source = ['memphis/main.c', SOURCES]))

env.Program('test/testTextPath', source=['test/testTextPath.c','memphis/textpath.c'])
env.Program('test/testSize', source=['test/testSize.c'])
env.Program('test/testOSM', source=['test/testOSM.c','memphis/osm05.c','memphis/mlib.c'])
env.Program('test/testRuleset', source=['test/testRuleset.c','memphis/ruleset.c','memphis/mlib.c'])

env.Program('demos/tile-renderer', source = ['demos/tile-renderer.c', SOURCES])
