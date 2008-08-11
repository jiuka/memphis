/*
 *
 */

#ifndef CONFIG02_H
#define CONFIG02_H

#define POLYGONE    1
#define LINE        2

#define NODE        1
#define WAY         2


#define LAYERMIN     12
#define LAYERMAX     18

/*
 * LinkedList Struct for Tags
 */
typedef struct _cfgTag cfgTag;
struct _cfgTag {
        char *key;
        char *value;
        cfgTag *next;
    };

/*
 * LinkedList Struct for DrawRules
 */
typedef struct _cfgDraw cfgDraw;
struct _cfgDraw {
        int     type;
        float   color[3];
        float   width;
        cfgDraw *next;
    };

/*
 * LinkedList Struct for Rules
 */
typedef struct _cfgRule cfgRule;
struct _cfgRule {
        int     minlayer;
        int     maxlayer;
        int     type;
        cfgTag  *tag;
        cfgDraw *draw;
        cfgRule *next;
    };

/*
 * LinkedList Struct for Layers
 */
typedef struct _cfgLayer cfgLayer;
struct _cfgLayer {
        char        *name;
        cfgRule     *rule;
        cfgTag  *tag;
        cfgLayer    *next;
    };

#endif /* CONFIG02_H */

/*
 * vim: expandtab shiftwidth=4:
 */