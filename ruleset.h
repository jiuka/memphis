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

#ifndef RULESET_H
#define RULESET_H

typedef enum osmTypes {
    WAY         = 1,
    NODE        = 2,
    RELATION    = 4,
} osmTypes;

typedef enum mapPrimitives {
    LINE        = 1,
    POLYGONE    = 2,
    TEXT        = 3,
    ROAD        = 4,
} mapPrimitives;

/*
 * Struct for Style
 */
typedef struct _cfgStyle cfgStyle;
struct _cfgStyle {
        short int   zoom;     /* -1 = All */
        char        *src;
    };

/*
 * LinkedList Struct for Draws
 */
typedef struct cfgDraw_ cfgDraw;
struct cfgDraw_ {
        mapPrimitives type;
        short int   minzoom;
        short int   maxzoom;
        char        **styleclass;
        cfgDraw     *next;
    };

/*
 * LinkedList Struct for Rule/Else
 */
typedef struct cfgRule_ cfgRule;
typedef struct cfgElse_ cfgElse;

struct cfgElse_ {
        cfgRule     *sub;
        cfgRule     *next;
        cfgDraw     *draw;
#ifdef DEBUG
        int         d;
#endif
    };
struct cfgRule_ {
        short int   type;
        char        **key;
        char        **value;
        cfgRule     *parent;
        cfgRule     *nparent;
        cfgRule     *next;
        cfgDraw     *draw;
        cfgDraw     *ndraw;
#ifdef DEBUG
        int         d;
#endif
    };

/*
 * LinkedList Struct for Rules
 */
typedef struct cfgRules_ cfgRules;
struct cfgRules_ {
        int         cntRule;
        int         cntElse;
        int         depth;
        short int   background[3];
        cfgRule     *rule;
        GSList      *style;
    };

/*
 * Prototypes
 */
cfgRules* rulesetRead(char *filename);
void rulesetFree(cfgRules * ruleset);

#endif /* RULESET_H */

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

