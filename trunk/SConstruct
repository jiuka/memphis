# scons build file

env = Environment()
env.MergeFlags(['-Wall -g -lm -std=c99 -lexpat'])

env.ParseConfig('pkg-config --cflags --libs cairo')
env.ParseConfig('pkg-config --cflags --libs glib-2.0')

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
    'main.c',
    'osm05.c',
    'renderer.c',
    'ruleset.c',
    'textpath.c'
]

Default(env.Program('memphis', source = SOURCES))

env.Program('testTextPath', source=['test/testTextPath.c','textpath.c'])
env.Program('testStrlist', source=['test/testStrlist.c','strlist.c'])
env.Program('testSize', source=['test/testSize.c'])
env.Program('testOSM', source=['test/testOSM.c','osm05.c'])

#env.Program('testRuleset', source=['ruleset.c','testRuleset.c'])
#env.Program('testOSM', source=['osm05.c','testOSM.c'])
