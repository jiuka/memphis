# scons build file

env = Environment()

env.ParseConfig('pkg-config --cflags --libs cairo')

conf = Configure(env)

if not conf.CheckDeclaration('strsep',includes="#include <string.h>"):
    print 'Did not find strsep(), using local version'
else:
    conf.env.Append(CCFLAGS = '-DHAVE_STRSEP')

if not conf.CheckDeclaration('strdup',includes="#include <string.h>"):
    print 'Did not find strdup(), using local version'
else:
    conf.env.Append(CCFLAGS = '-DHAVE_STRDUP')
    
if not conf.CheckLibWithHeader('expat','expat.h','C'):
    print 'Did not find expat, exiting!'
    Exit(1)
    
if not conf.CheckLibWithHeader('cairo','cairo.h','C'):
    print 'Did not find cairo, exiting!'
    Exit(1)

env = conf.Finish()

SOURCES = [
    'libmercator.c',
    'main.c',
    'osm05.c',
    'renderer.c',
    'ruleset.c',
    'compat.c',
    'textpath.c'
]

env.MergeFlags(['-Wall -g -lm -std=c99 -lexpat'])              

Default(env.Program('memphis', source = SOURCES))

#env.Program('testRuleset', source=['ruleset.c','testRuleset.c'])
#env.Program('testStrlist', source=['testStrlist.c'])
#env.Program('testOSM', source=['osm05.c','testOSM.c'])
env.Program('testTextPath', source=['test/testTextPath.c','textpath.c'])
