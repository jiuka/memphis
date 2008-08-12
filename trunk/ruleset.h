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

#define WAY         1
#define NODE        2
#define RELATION    4

/*
 * LinkedList Struct for Draws
 */
typedef struct _cfgDraw cfgDraw;
struct _cfgDraw {
        short int   type;
    };

/*
 * LinkedList Struct for Strings
 */
typedef struct _cfgStr cfgStr;
struct _cfgStr {
        char    *str;
        cfgStr  *next;
    };

/*
 * LinkedList Struct for Rule/Else
 */
typedef struct _cfgRule cfgRule;
typedef struct _cfgElse cfgElse;

struct _cfgElse {
        cfgRule     *sub;
        cfgRule     *next;
#ifdef DEBUG
        int         d;
#endif
    };
struct _cfgRule {
        short int   type;
        char        **key;
        char        **value;
        cfgRule     *sub;
        cfgElse     *nsub;
        cfgRule     *next;
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
        char        *data;
        float       scale;
        cfgRule     *rule;
        
        cfgStr      *keys;
        cfgStr      *values;
    };

/*
 * Prototypes
 */
cfgRules* rulesetRead(char *filename);

#endif /* RULESET_H */

/*
 * vim: expandtab shiftwidth=4:
 */