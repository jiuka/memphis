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
#include <glib.h>
#include <math.h>
#include <time.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "osm05.h"
#include "libmercator.h"
#include "list.h"
#include "renderer.h"
#include "ruleset.h"
#include "textpath.h"

// External Vars
extern memphisOpt   *opts;
extern GTree        *keyStrings;
extern GTree        *valStrings;

/*
 * Internal used return values for stringInStrings.
 */
typedef enum compare_result_e {
    TAG_CMP_NOT_EQUAL   = 0,
    TAG_CMP_EQUAL       = 1,
    TAG_CMP_ANY         = 2,
    TAG_CMP_MISSING     = 3,
} compare_result_e;

/*
 * function: drawPath
 * @cr  Array of cairo resources
 * @nd  Liked list if osmNd's
 *
 * This function is used to prepare a Path.
 */
void drawPath(renderInfo *info, GSList *nodes) {
    GSList *iter;
    osmNode *nd;
    coordinates xy;
    
    if (opts->debug > 1)
        fprintf(stdout,"drawPath\n");

    iter = nodes;
    nd = iter->data;
    xy = coord2xy(nd->lat, nd->lon, info->zoom);
    cairo_move_to(info->cr, xy.x-info->offset.x,
                            xy.y-info->offset.y);
    
    iter = g_slist_next(iter);
    while(iter) {
        nd = iter->data;
        xy = coord2xy(nd->lat, nd->lon, info->zoom);
        cairo_line_to(info->cr, xy.x-info->offset.x, xy.y-info->offset.y);
        iter = g_slist_next(iter);
    }
}

/*
 * function: strokePath
 * @cr  Array of cairo resources
 *
 * This function is stroke all current path without drawing anithing.
 */
void strokePath(renderInfo *info) {
    if (opts->debug > 1)
        fprintf(stdout,"strokePath\n");
        
    cairo_set_line_width (info->cr, 0);
    cairo_stroke(info->cr);
}

/*
 * function: drawPolygone
 *
 * This function fill the prepared paths with the configured color.
 */
void drawPolygone(renderInfo *info, cfgDraw *draw) {
    if (opts->debug > 1)
        fprintf(stdout,"drawPolygone\n");

    cairo_surface_t *image;
    cairo_pattern_t *pattern;

    if(draw->pattern) {
        char *filename;
        int w, h;

        filename = g_strdup_printf("pattern/%s.png", draw->pattern);
        image = cairo_image_surface_create_from_png(filename);
        g_free(filename);

        w = cairo_image_surface_get_width (image);
        h = cairo_image_surface_get_height (image);

        pattern = cairo_pattern_create_for_surface (image);
        cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
    }

    cairo_set_fill_rule (info->cr, CAIRO_FILL_RULE_EVEN_ODD);
    if(draw->pattern)
        cairo_set_source (info->cr, pattern);
    else
        cairo_set_source_rgb (info->cr, (double)draw->color[0]/(double)255,
                                        (double)draw->color[1]/(double)255,
                                        (double)draw->color[2]/(double)255);

    cairo_fill_preserve(info->cr);

    if(draw->pattern) {
        cairo_pattern_destroy (pattern);
        cairo_surface_destroy (image);
    }
}

/*
 * function: drawLine
 *
 * This function draw the prepared paths with the configured color.
 */
void drawLine(renderInfo *info, cfgDraw *draw) {
    if (opts->debug > 1)
        fprintf(stdout,"drawLine\n");

    cairo_set_line_cap  (info->cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_width (info->cr, draw->width*LINESIZE(info->zoom));

    cairo_set_source_rgb (info->cr, (double)draw->color[0]/(double)255,
                                    (double)draw->color[1]/(double)255,
                                    (double)draw->color[2]/(double)255);
    cairo_stroke_preserve(info->cr);
}

/*
 * function: drawText
 *
 * This function draw the given text along the current path.
 */
void drawText(renderInfo *info, char *text, cfgDraw *draw) {
    if (opts->debug > 1)
        fprintf(stdout,"drawText\n");

    cairo_select_font_face (info->cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                                              CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_source_rgb (info->cr, (double)draw->color[0]/(double)255,
                                    (double)draw->color[1]/(double)255,
                                    (double)draw->color[2]/(double)255);
    cairo_set_font_size (info->cr, draw->width*LINESIZE(info->zoom));
    textPath(info->cr, text);
}

/*
 * function: stringInStrings
 *
 * Check if string is an strings.
 */
compare_result_e stringInStrings(char *string, char **strings) {
    if (opts->debug > 1)
        fprintf(stdout,"stringInStrings\n");
    compare_result_e r = TAG_CMP_NOT_EQUAL;
    while (*strings != NULL) {
        if (string == *strings) {
            return TAG_CMP_EQUAL;
        }
        if(strcmp(*strings,"*") == 0)
            r = TAG_CMP_ANY;
        if(strcmp(*strings,"~") == 0)
            r = TAG_CMP_MISSING;

        strings++;
    }
    return (r);
}

/*
 * function: matchRule
 *
 * Check if a element matchs a rule.
 */
int matchRule(cfgRule *rule, osmTag *tag) {
    int k, v;
    
    if (opts->debug > 1)
        fprintf(stdout,"matchRule\n");
        
    while(tag) {
        k = stringInStrings(tag->key, rule->key);
        v = stringInStrings(tag->value, rule->value);

        if (k == TAG_CMP_EQUAL && v == TAG_CMP_EQUAL)
            return TRUE;
        if (k == TAG_CMP_EQUAL && v == TAG_CMP_ANY)
            return TRUE;
        if (k == TAG_CMP_NOT_EQUAL && v == TAG_CMP_MISSING)
            return TRUE;

        tag = tag->next;
    }
    return FALSE;
}

/*
 * function: checkRule
 *
 * Check if a element match to a rule and all it's parent.
 */
int checkRule(cfgRule *rule, osmTag *tag, short int type) {
    if (opts->debug > 1)
        fprintf(stdout,"checkRule\n");

    int not;
    cfgRule *iter;

    if(rule->nparent) {
        iter = rule->nparent;
        not = TRUE;
    } else {
        iter = rule->parent;
        not = FALSE;
    }

    while(iter) {

        if(matchRule(iter, tag) == not) {
            return(0);
        }

        if(iter->nparent) {
            iter = iter->nparent;
            not = TRUE;
        } else {
            iter = iter->parent;
            not = FALSE;
        }
    }

    if(matchRule(rule, tag)) {
        return(1);
    } else {
        return(-1);
    }

}

/*
 * function: renderCairoRun
 */
int renderCairoRun(renderInfo *info) {
    if (opts->debug > 1)
        fprintf(stdout,"renderCairoRun\n");
    int l;

    // Vars uder while looping throug data
    osmWay      *way;
    cfgRule     *rule;
    cfgDraw     *draw;
    int         paths;

    // Start checking osm from bottom layer.
    for(l=-5;l<=5;l++) {

        if (opts->debug > 0) {
            fprintf(stdout,"\r Cairo drawing z%i Layer % 2i", info->zoom, l);
            fflush(stdout);
        }

        // Process Rule by Rule
        LIST_FOREACH(rule, info->ruleset->rule) {

            if(rule->draw != NULL) { // Draw Match first

                paths = 0;
                // Loop through ways for
                LIST_FOREACH(way, info->osm->ways) {
                    //Only objects on current layer
                    if(way->layer != l)
                        continue;

                    if( checkRule(rule, way->tag, WAY) == 1) {
                        drawPath(info, way->nd);
                        paths++;
                    }
                }
                if(paths) {
                    draw = rule->draw;
                    while(draw) {
                        switch(draw->type) {
                            case POLYGONE:
                                drawPolygone(info,draw);
                                break;
                            case LINE:
                                drawLine(info,draw);
                                break;
                            case TEXT:
                                break;
                        }
                        draw = draw->next;
                    }
                }
                strokePath(info);
                
                paths = 0;
                // Text Rendering
                draw = rule->draw;
                while(draw) {
                    switch(draw->type) {
                        case TEXT:
                            paths++;
                            break;
                        case POLYGONE:
                        case LINE:
                            break;
                    }
                    if(paths)
                        break;
                    draw = draw->next;
                }
                if(paths) {
                    LIST_FOREACH(way, info->osm->ways) {
                        //Only objects on current layer
                        if(way->layer != l || way->name == NULL)
                            continue;

                        if( checkRule(rule, way->tag, WAY) == 1) {
                            drawPath(info, way->nd);
                            drawText(info, way->name, draw);
                        }
                    }

                }
                strokePath(info);
            }
            if (rule->ndraw != NULL) { // Draw Else after

                paths = 0;
                // Loop through ways for
                LIST_FOREACH(way, info->osm->ways) {
                    //Only objects on current layer
                    if(way->layer != l)
                        continue;

                    if( checkRule(rule, way->tag, WAY) == -1) {
                        drawPath(info, way->nd);
                        paths++;
                    }
                }
                if(paths) {
                    draw = rule->ndraw;
                    while(draw) {
                        switch(draw->type) {
                            case POLYGONE:
                                drawPolygone(info,draw);
                                break;
                            case LINE:
                                drawLine(info,draw);
                                break;
                            case TEXT:
                                break;
                        }
                        draw = draw->next;
                    }
                }
                strokePath(info);
            }
        }
    }

    if (opts->debug > 0)
        fprintf(stdout,"\r Cairo drawing done\n");

    return(0);
}

/*
 * function: renderCairo
 */
int renderCairo(cfgRules *ruleset, osmFile *osm) {
    if (opts->debug > 1)
        fprintf(stdout,"renderCairo\n");
    int z;
    renderInfo *info;
    
    // Initialize all layers
    for (z=0;z<=opts->maxlayer-opts->minlayer;z++) {
        coordinates min, max;
        
        info = g_new(renderInfo, 1);
        info->zoom = z+opts->minlayer;
        info->ruleset = ruleset;
        info->osm = osm;
                
        min = coord2xy(osm->minlat, osm->minlon, info->zoom);
        max = coord2xy(osm->maxlat, osm->maxlon, info->zoom);
        int w = (int)ceil(max.x-min.x);
        int h = (int)ceil(min.y-max.y);

        info->offset = coord2xy(osm->maxlat, osm->minlon, info->zoom);

        info->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,w,h);
        info->cr = cairo_create(info->surface);

        cairo_rectangle(info->cr, 0, 0, w, h);
        cairo_set_source_rgb(info->cr,
                                    (double)ruleset->background[0]/(double)255,
                                    (double)ruleset->background[1]/(double)255,
                                    (double)ruleset->background[2]/(double)255);
        cairo_fill(info->cr);
        
        renderCairoRun(info);
        
        // Saving Images
        char *filename;

        if (opts->debug > 0) {
            fprintf(stdout," Cairo rendering Z%i", info->zoom);
            fflush(stdout);
        }
        filename = g_strdup_printf("tiles/%02i.png", info->zoom);
        cairo_surface_write_to_png(info->surface, filename);
        g_free(filename);
        cairo_destroy(info->cr);
        cairo_surface_destroy(info->surface);
        if (opts->debug > 0)
            fprintf(stdout," done.\n");
            
        g_free(info);
    }
    
    return (0);
}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

