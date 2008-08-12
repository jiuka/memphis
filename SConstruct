# scons build file

envXML = Environment()
envCairo = Environment()

SOURCES = [
    'ruleset.c'
]

envXML.MergeFlags(['-Wall -g -lm -std=c99 -lexpat',
                ])
envCairo.MergeFlags(['-Wall -g -lm -std=c99 -lexpat',
                    '!pkg-config cairo --cflags --libs',
                ])
               
m = envCairo.Program('memphis', source = SOURCES)
Default(m)

envXML.Program('testRuleset', source=['ruleset.c','testRuleset.c'])