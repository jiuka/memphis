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
    int z;
    
    for (z=0;z<=LAYERMAX-LAYERMIN;z++) {
       cairo_new_path(info->cr[z]);
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
        
    cairo_surface_t *image;
    cairo_pattern_t *pattern;
        
    if(draw->pattern) {
        char * filename = malloc(50*sizeof(char));
        int w, h;
        
        sprintf(filename,"pattern/%s.png",draw->pattern);
        image = cairo_image_surface_create_from_png(filename);
        free(filename);
        
        w = cairo_image_surface_get_width (image);
        h = cairo_image_surface_get_height (image);


                
        pattern = cairo_pattern_create_for_surface (image);
        cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
    }
    
    int z;
    for (z=0;z<=LAYERMAX-LAYERMIN;z++) {
        if (LAYERMIN+z<draw->minlayer || LAYERMIN+z>draw->maxlayer)
            continue;
        cairo_set_fill_rule (info->cr[z], CAIRO_FILL_RULE_EVEN_ODD);
        if(draw->pattern)
            cairo_set_source (info->cr[z], pattern);
        else
            cairo_set_source_rgb (info->cr[z],
                                    (double)draw->color[0]/(double)255,
                                    (double)draw->color[1]/(double)255,
                                    (double)draw->color[2]/(double)255);
        
        cairo_fill_preserve(info->cr[z]);
    }
        
    if(draw->pattern) {
        //cairo_pattern_destroy (pattern);
        //cairo_surface_destroy (image);
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
    
    int z;
    for (z=0;z<=LAYERMAX-LAYERMIN;z++) {
        if (z < draw->minlayer-LAYERMIN || z > draw->maxlayer-LAYERMIN)
            continue;
        
        cairo_set_line_cap  (info->cr[z], CAIRO_LINE_CAP_ROUND);
        cairo_set_line_width (info->cr[z], draw->width*LINESIZE(z));

        cairo_set_source_rgb (info->cr[z],
                                    (double)draw->color[0]/(double)255,
                                    (double)draw->color[1]/(double)255,
                                    (double)draw->color[2]/(double)255);
        cairo_stroke_preserve(info->cr[z]);
    }
}

/**
 *
 */
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

int checkRule(cfgRule *rule, osmTag *tag, short int type) {
    if (debug > 1)
        fprintf(stdout,"checkRule\n");

    int not;
    cfgRule *iter;
    
    if(rule->nparent) {
        iter = rule->nparent;
        not = 1;
    } else {
        iter = rule->parent;
        not = 0;
    }
    
    while(iter) {
        
        if(matchRule(iter, tag) == not) {
            return(0);
        }
        
        if(iter->nparent) {
            iter = iter->nparent;
            not = 1;
        } else {
            iter = iter->parent;
            not = 0;
        }
    }
    
    if(matchRule(rule, tag)) {
        return(1);
    } else {
        return(-1);
    }
}

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
        
        cairo_rectangle (info->cr[z], 0, 0, TILESIZE* numTiles(z), TILESIZE* numTiles(z));
        cairo_set_source_rgb (info->cr[z], 0.95,0.95,0.95);
        cairo_fill(info->cr[z]);
   }
    
    // Vars uder while looping throug data
    osmWay      *way;
    //osmTag      *tag;
    cfgRule     *rule;
    cfgDraw     *draw;
    int         paths;
    
    long start = (long)clock();
    
    
    // Start checking osm from bottom layer.
    for(l=-5;l<=5;l++) {
            
        // Process Rule by Rule
        LIST_FOREACH(rule, ruleset->rule) {
            
            if(rule->draw != NULL) { // Draw Match first
            
                paths = 0;
                // Loop through ways for
                LIST_FOREACH(way, osm->ways) {
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
                        }
                        draw = draw->next;
                    }
                }
                strokePath(info);
            }
            if (rule->ndraw != NULL) { // Draw Else after
                     
                paths = 0;
                // Loop through ways for
                LIST_FOREACH(way, osm->ways) {
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
                        }
                        draw = draw->next;
                    }
                }
                strokePath(info);
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