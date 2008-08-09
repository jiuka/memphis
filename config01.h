/*
 *
 */


#ifndef CONFIG01_H
#define CONFIG01_H

#define POLYGONE    1
#define STREET      2

typedef struct _cfgTag cfgTag;

struct _cfgTag {
        char *key;
        char *value;
        cfgTag *next;
    };
    
typedef struct _cfgNode cfgNode;
    
struct _cfgNode {
        float color[3];
        cfgTag *tag;
        cfgNode *next;
    };
    
typedef struct _cfgWay cfgWay;

struct _cfgWay {
        short int   type;
        float   color[3];
        int     width;
        cfgTag  *tag;
        cfgWay  *next;
    };



typedef struct _cfgFile cfgFile;
struct _cfgFile {
        cfgNode     *nodes;
        cfgWay      *ways;
    };


#endif /* CONFIG01_H */

/*
 * vim: expandtab shiftwidth=4:
 */