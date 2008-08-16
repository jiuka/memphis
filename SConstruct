# scons build file

env = Environment()

SOURCES = [
    'libmercator.c',
    'main.c',
    'osm05.c',
    'renderer.c',
    'ruleset.c',
    'compat.c',
    'textpath.c'
]

env.MergeFlags(['-Wall -g -lm -std=c99 -lexpat',
                    '!pkg-config cairo --cflags --libs',
                ])

Default(env.Program('memphis', source = SOURCES))

#env.Program('testRuleset', source=['ruleset.c','testRuleset.c'])
#env.Program('testStrlist', source=['testStrlist.c'])
#env.Program('testOSM', source=['osm05.c','testOSM.c'])
env.Program('testTextPath', source=['test/testTextPath.c','textpath.c'])
