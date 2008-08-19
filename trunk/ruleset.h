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
} mapPrimitives;

/*
 * LinkedList Struct for Draws
 */
typedef struct _cfgDraw cfgDraw;
struct _cfgDraw {
        mapPrimitives type;
        short int   minlayer;
        short int   maxlayer;
        short int   color[3];
        char*       pattern;
        float       width;
        cfgDraw     *next;
    };

/*
 * LinkedList Struct for Rule/Else
 */
typedef struct _cfgRule cfgRule;
typedef struct _cfgElse cfgElse;

struct _cfgElse {
        cfgRule     *sub;
        cfgRule     *next;
        cfgDraw     *draw;
#ifdef DEBUG
        int         d;
#endif
    };
struct _cfgRule {
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
typedef struct _cfgRules cfgRules;
struct _cfgRules {
        int         cntRule;
        int         cntElse;
        int         depth;

        short int   background[3];

        char        *data;
        float       scale;
        cfgRule     *rule;
    };

/*
 * Prototypes
 */
cfgRules* rulesetRead(char *filename);

#endif /* RULESET_H */

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

