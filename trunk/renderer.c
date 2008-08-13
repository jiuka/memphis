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

#include <math.h>
#include <time.h>
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "osm05.h"
#include "libmercator.h"
#include "list.h"
#include "renderer.h"
#include "ruleset.h"
#include "strlist.h"

// External Vars
extern int      debug;
extern strList  *keyStrings;
extern strList  *valStrings;

/*
 * function: drawPath
 * @cr  Array of cairo resources
 * @nd  Liked list if osmNd's
 *
 * This function is used to prepare a Path.
 */
void drawPath(renderInfo *info, osmNd *nd) {
    coordinates xy;
    int i;
    
    for (i=0;i<=LAYERMAX-LAYERMIN;i++) {
        xy = coord2xy(nd->node->lat,nd->node->lon,i+LAYERMIN);
        cairo_move_to(info->cr[i],
                        xy.x-info->offset[i].x,
                        xy.y-info->offset[i].y);
    }
    nd = nd->next;
    while(nd) {
        for (i=0;i<=LAYERMAX-LAYERMIN;i++) {
            xy = coord2xy(nd->node->lat,nd->node->lon,i+LAYERMIN);
            cairo_line_to(info->cr[i],
                            xy.x-info->offset[i].x,
                            xy.y-info->offset[i].y);
        }
        nd = nd->next;
    }
}

/*
 * function: strokePath
 * @cr  Array of cairo resources
 *
 * This function is stroke all current path without drawing anithing.
 */
void strokePath(renderInfo *info) {
    int i;
    
    for (i=0;i<=LAYERMAX-LAYERMIN;i++) {
        cairo_set_line_width (info->cr[i], 0);
       cairo_stroke(info->cr[i]);
    }
}

/*
 * function: drawPolygone
 *
 * This function fill the prepared paths with the configured color.
 */
void drawPolygone(renderInfo *info, cfgDraw *draw) {
    if (debug > 1)
        fprintf(stdout,"drawPolygone\n");
    
    int i;
    for (i=0;i<=LAYERMAX-LAYERMIN;i++) {
        cairo_set_fill_rule (info->cr[i], CAIRO_FILL_RULE_EVEN_ODD);
        cairo_set_source_rgb (info->cr[i], draw->color[0],draw->color[1],
                                        draw->color[2]);
        cairo_fill_preserve(info->cr[i]);
    }
}

/*
 * function: drawLine
 *
 * This function draw the prepared paths with the configured color.
 */
void drawLine(renderInfo *info, cfgDraw *draw) {
    if (debug > 1)
        fprintf(stdout,"drawLine\n");
    
    int i;
    for (i=0;i<=LAYERMAX-LAYERMIN;i++) {
        cairo_set_line_cap  (info->cr[i], CAIRO_LINE_CAP_ROUND);
        cairo_set_line_width (info->cr[i], draw->width*LINESIZE(i));

        cairo_set_source_rgb (info->cr[i], draw->color[0],draw->color[1],
                                        draw->color[2]);
        cairo_stroke_preserve(info->cr[i]);
    }
}

int stringInStrings(char *string, char **strings) {
    if (debug > 1)
        fprintf(stdout,"stringInStrings\n");
    int r=0;
    while (*strings != NULL) {
        if (string == *strings)
            return(1);
        if(strcmp(*strings,"*") == 0)
            r = 2;
        if(strcmp(*strings,"~") == 0)
            r = 3;
        
        *strings++;
    }
    return(r);
}

int matchRule(cfgRule *rule, osmTag *tag) {
    if (debug > 1)
        fprintf(stdout,"matchRule\n");
    int k, v;
    while(tag) {
        k = 0;
        k = stringInStrings(tag->key, rule->key);
        v = 0;
        v = stringInStrings(tag->value, rule->value);
                
        if (k==1 && v==1)
            return(1);
        if (k==1 && v==2)
            return(1);
        if (k==0 && v==3)
            return(1);
        
        tag = tag->next;
    }
    return(0);
}

cfgDraw* applyWayRule(cfgRule *rule, osmWay *way) {
    if (debug > 1)
        fprintf(stdout,"applyWayRule\n");
    cfgRule *end = rule->next;
    while(rule) {
        if(rule == end)
            break;
        if(matchRule(rule, way->tag)) {
            
            if (rule->sub != NULL)
                rule = rule->sub;
            else
                return rule->draw;
        } else {
            if (rule->nsub != NULL) {
                if (rule->nsub->sub != NULL)
                    rule = rule->nsub->sub;
                else
                    return rule->nsub->draw;
            } else {
                rule = rule->next;
            }
        }
        
	}
	return(NULL);
};

int renderCairo(cfgRules *ruleset, osmFile *osm) {
    if (debug > 1)
        fprintf(stdout,"renderCairo\n");
    int z, l;
    renderInfo *info;
    
    info = malloc(sizeof(renderInfo));
    
    // Calculate X/Y Draw offsets
    for (z=0;z<=LAYERMAX-LAYERMIN;z++) {
        info->offset[z] = coord2xy(osm->bounds->maxlat,
                                    osm->bounds->minlon,z+LAYERMIN);
                                    
        info->surface[z] = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                    TILESIZE*numTiles(z), TILESIZE* numTiles(z));
        info->cr[z] = cairo_create(info->surface[z]);
   }
    
    // Vars uder while looping throug data
    osmWay      *way;
    //osmTag      *tag;
    cfgRule     *rule;
    cfgDraw     *draw;
    
    long start = (long)clock();
    
    // Process Rule by Rule
    LIST_FOREACH(rule, ruleset->rule) {
        if (debug > 1)
            fprintf(stdout,"rule\n");
        
        // Start checking osm from bottom layer.
        for(l=-5;l<=5;l++) {
            // Loop throug osmdata
            LIST_FOREACH(way, osm->ways) {
                //Only objects on current layer
                if(way->layer != l)
                    continue;
                draw = applyWayRule(rule,way);
                if(draw) {
                    drawPath(info, way->nd);
                    while(draw) {
                        switch(draw->type) {
                            case POLYGONE:
                                drawPolygone(info,draw);
                                break;
                            case LINE:
                                drawLine(info,draw);
                                break;
                        }
                        draw = draw->next;
                    }
                    strokePath(info);
                }
            }
        }
    }
    
    
    if (debug > 0)
        fprintf(stdout," Cairo draw done.  [%fs]\n",
                ((long)clock()-start)/(double)CLOCKS_PER_SEC); 
    
    // Saving Images
    char *filename;
    filename = malloc(sizeof(char)*50);
    
    for (z=0;z<=LAYERMAX-LAYERMIN;z++) {
        start = (long)clock();
        sprintf(filename,"tiles/%02i.png",z+LAYERMIN);
        cairo_surface_write_to_png(info->surface[z], filename);
        cairo_destroy(info->cr[z]);
        cairo_surface_destroy(info->surface[z]);
        if (debug > 0)
            fprintf(stdout,"  Cairo rendering Z%i done.  [%fs]\n", z+LAYERMIN,
                    ((long)clock()-start)/(double)CLOCKS_PER_SEC); 
   }

    

    return(0);
}