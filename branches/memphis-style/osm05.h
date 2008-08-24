/*
 * Memphis - Cairo Rederer for OSM in C
 * Copyright (C) 2008  <marius.rieder@durchmesser.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef OSM05_H
#define OSM05_H

#include <glib.h>

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

typedef struct _osmWay osmWay;
struct _osmWay {
        int id;
        short int layer;
        char    *name;
        osmTag  *tag;
        GSList  *nd;
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
osmFile* osmRead(char *filename);
void osmFree(osmFile *osm);

#endif /* OSM05_H */

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

