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

#include <glib.h>
#include <math.h>
#include <time.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libmercator.h"
#include "list.h"
#include "renderer.h"
#include "ruleset.h"
#include "textpath.h"

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
static void drawPath(renderInfo *info, GSList *nodes, gint8 debug_level) {
    GSList *iter;
    osmNode *nd;
    coordinates xy;
    
    if (G_UNLIKELY(debug_level > 1))
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
static void strokePath(renderInfo *info, gint8 debug_level) {
    if (G_UNLIKELY(debug_level > 1))
        fprintf(stdout,"strokePath\n");
        
    cairo_set_line_width (info->cr, 0);
    cairo_stroke(info->cr);
}

/*
 * function: drawPolygone
 *
 * This function fill the prepared paths with the configured color.
 */
static void drawPolygone(renderInfo *info, cfgDraw *draw, gint8 debug_level) {
    if (G_UNLIKELY(debug_level > 1))
        fprintf(stdout,"drawPolygone\n");

    cairo_surface_t *image = NULL;
    cairo_pattern_t *pattern = NULL;

    if(draw->pattern) {
        char *filename;

        /* TODO ast: the pattern may be cached, e.g. using a GCache structure */

        filename = g_strdup_printf("pattern/%s.png", draw->pattern);
        image = cairo_image_surface_create_from_png(filename);
        if (cairo_surface_status(image) != CAIRO_STATUS_SUCCESS) {
            g_warning("pattern '%s' not found\n", filename);
            g_free(filename);
            return;
        }
        g_free(filename);

        pattern = cairo_pattern_create_for_surface (image);
        cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
        cairo_surface_destroy (image);
    }

    cairo_set_fill_rule (info->cr, CAIRO_FILL_RULE_EVEN_ODD);

    if(pattern)
        cairo_set_source (info->cr, pattern);
    else
        cairo_set_source_rgb (info->cr, (double)draw->color[0]/(double)255,
                                        (double)draw->color[1]/(double)255,
                                        (double)draw->color[2]/(double)255);

    cairo_fill_preserve(info->cr);

    if(pattern)
        cairo_pattern_destroy (pattern);
}

/*
 * function: drawLine
 *
 * This function draw the prepared paths with the configured color.
 */
static void drawLine(renderInfo *info, cfgDraw *draw, gint8 debug_level) {
    if (G_UNLIKELY(debug_level > 1))
        fprintf(stdout,"drawLine\n");

    cairo_set_line_cap  (info->cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_join (info->cr, CAIRO_LINE_JOIN_ROUND);
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
static void drawText(renderInfo *info, char *text, cfgDraw *draw, gint8 debug_level) {
    if (G_UNLIKELY(debug_level > 1))
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
static compare_result_e stringInStrings(char *string, char **strings, gint8 debug_level) {
    if (G_UNLIKELY(debug_level > 1))
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
static int matchRule(cfgRule *rule, osmTag *tag, gint8 debug_level) {
    int k, v;
    
    if (G_UNLIKELY(debug_level > 1))
        fprintf(stdout,"matchRule\n");
        
    while(tag) {
        k = stringInStrings(tag->key, rule->key, debug_level);
        v = stringInStrings(tag->value, rule->value, debug_level);

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
static int checkRule(cfgRule *rule, osmTag *tag, short int type,
        gint8 debug_level) {
    if (G_UNLIKELY(debug_level > 1))
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

        if(matchRule(iter, tag, debug_level) == not) {
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

    if(matchRule(rule, tag, debug_level)) {
        return(1);
    } else {
        return(-1);
    }

}

static void renderPaths(renderInfo *info, int layer, cfgRule *rule,
        cfgDraw *draw, gint8 debug_level) {
    int paths = 0;
    osmWay *way;

    // Loop through ways for
    LIST_FOREACH(way, info->osm->ways) {
        //Only objects on current layer
        if(way->layer != layer)
            continue;

        if( checkRule(rule, way->tag, WAY, debug_level) == 1) {
            drawPath(info, way->nd, debug_level);
            paths++;
        }
    }
    if(paths) {
        while(draw) {
            if(draw->minzoom > info->zoom || draw->maxzoom < info->zoom) {
                draw = draw->next;
                continue;
            }
            switch(draw->type) {
                case POLYGONE:
                    drawPolygone(info, draw, debug_level);
                    break;
                case LINE:
                    drawLine(info, draw, debug_level);
                    break;
                case TEXT: break;   /* ignore */
            }
            draw = draw->next;
        }
    }
    strokePath(info, debug_level);
} 

static void renderText(renderInfo *info, int layer, cfgRule *rule, cfgDraw *draw, gint8 debug_level) {
    osmWay      *way;
    while(draw) {
        if (draw->type == TEXT) {
            if(draw->minzoom <= info->zoom && info->zoom <= draw->maxzoom) {
                LIST_FOREACH(way, info->osm->ways) {
                    //Only objects on current layer
                    if(way->layer != layer || way->name == NULL)
                        continue;

                    if( checkRule(rule, way->tag, WAY, debug_level) == 1) {
                        drawPath(info, way->nd, debug_level);
                        drawText(info, way->name, draw, debug_level);
                    }
                }

            }

            break;
        }
        draw = draw->next;
    }
    strokePath(info, debug_level);
}

/*
 * function: renderCairoRun
 */
int renderCairoRun(renderInfo *info, gint8 debug_level) {
    if (debug_level > 1)
        fprintf(stdout,"renderCairoRun\n");
    int layer;

    // Vars used while looping through data
    cfgRule     *rule;

    // Start checking osm from bottom layer.
    for(layer = -5; layer <= 5; layer++) {

        if (debug_level > 0) {
            fprintf(stdout,"\r Cairo drawing z%i Layer % 2i", info->zoom, layer);
            fflush(stdout);
        }

        // Process Rule by Rule
        LIST_FOREACH(rule, info->ruleset->rule) {

            if(rule->draw != NULL) { // Draw Match first
                renderPaths(info, layer, rule, rule->draw, debug_level);

                // Text Rendering
                renderText(info, layer, rule, rule->draw, debug_level);
            }
            if (rule->ndraw != NULL) { // Draw Else after
                renderPaths(info, layer, rule, rule->ndraw, debug_level);
            }
        }
    }

    if (debug_level > 0)
        fprintf(stdout,"\r Cairo drawing done\n");

    return(0);
}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

