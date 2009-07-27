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
    'libmercator.c',
    'osm05.c',
    'ruleset.c',
    'textpath.c',
    'mlib.c',
    'memphis-data-pool.c',
    'memphis-map.c',
    'memphis-rule.c',
    'memphis-rule-set.c',
    'memphis-renderer.c'
]

Default(env.Program('memphis', source = ['main.c', SOURCES]))

env.Program('testTextPath', source=['test/testTextPath.c','textpath.c'])
env.Program('testSize', source=['test/testSize.c'])
env.Program('testOSM', source=['test/testOSM.c','osm05.c','mlib.c'])
env.Program('testRuleset', source=['test/testRuleset.c','ruleset.c','mlib.c'])

#env.Program('testRuleset', source=['ruleset.c','testRuleset.c'])

env.Program('tile-renderer', source = ['demos/tile-renderer.c', SOURCES])
env.Program('crash', source = ['demos/do_not_crash.c', SOURCES])
