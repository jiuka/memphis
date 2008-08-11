/*
 * CCairoRenderer
 *
 * Author: Marius Rieder <marius.rieder@durchmesser.ch>
 */

#ifndef OSM05_H
#define OSM05_H

typedef struct _osmBounds osmBounds;

struct _osmBounds {
        float minlat;
        float minlon;
        float maxlat;
        float maxlon;
    };

typedef struct _osmTag osmTag;

struct _osmTag {
        char *key;
        char *value;
        osmTag *next;
    };
    
typedef struct _osmNode osmNode;
    
struct _osmNode {
        int id;
        float lat;
        float lon;
        short int layer;
        osmTag *tag;
        osmNode *next;
    };
    
typedef struct _osmNd osmNd;

struct _osmNd {
        osmNode *node;
        osmNd   *next;
    };
    
typedef struct _osmWay osmWay;

struct _osmWay {
        int id;
        short int layer;
        osmTag  *tag;
        osmNd   *nd;
        osmWay  *next;
    };

typedef struct _osmMember osmMember;

struct _osmMember {
        osmNode     *node;
        osmWay      *way;
        char        *role;
        osmMember   *next;
    };

typedef struct _osmRelation omsRelation;

struct _osmRelation {
        int         id;
        osmTag      *tag;
        osmMember   *member;
    };

typedef struct _osmFile osmFile;
struct _osmFile {
        osmBounds   *bounds;
        osmNode     *nodes;
        osmWay      *ways;
    };


#endif /* OSM05_H */

/*
 * vim: expandtab shiftwidth=4:
 */