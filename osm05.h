/*
 * Memphis - Cairo Rederer for OSM in C
 * Copyright (C) 2008  Marius Rieder <marius.rieder@durchmesser.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef OSM05_H
#define OSM05_H

#include <glib.h>
#include "memphis-map.h"

typedef struct osmTag_ osmTag;
struct osmTag_ {
        char *key;
        char *value;
        osmTag *next;
    };

typedef struct osmNode_ osmNode;
struct osmNode_ {
        int id;
        float lat;
        float lon;
        short int layer;
        osmTag *tag;
        osmNode *next;
    };

typedef struct osmWay_ osmWay;
struct osmWay_ {
        int id;
        short int layer;
        char    *name;
        osmTag  *tag;
        GSList  *nd;
        osmWay  *next;
    };

typedef struct osmMember_ osmMember;
struct osmMember_ {
        osmNode     *node;
        osmWay      *way;
        char        *role;
        osmMember   *next;
    };

typedef struct osmRelation_ omsRelation;
struct osmRelation_ {
        int         id;
        osmTag      *tag;
        osmMember   *member;
    };

typedef struct osmFile_ osmFile;
struct osmFile_ {
        osmNode         *nodes;     // Node List
        GHashTable      *nodeidx;   // Node Hash
        unsigned int    nodecnt;    // Node Count
        osmWay          *ways;
        unsigned int    waycnt;
        float           minlat;
        float           minlon;
        float           maxlat;
        float           maxlon;
    };

/*
 * Prototypes
 */
osmFile* osmRead (char *filename, gint8 debug_level);
void osmFree(osmFile *osm);

#endif /* OSM05_H */

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

