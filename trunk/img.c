#include <cairo.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>


#include "config02.h"
#include "libosm.h"
#include "osm05.h"
#include "main.h"
#include "ll.h"

coordinates offXY[LAYERMAX-LAYERMIN];


#include <sys/stat.h>
int isDir(const char *dname) {
  struct stat sbuf;

  if (lstat(dname, &sbuf) == -1) {
    return 0;
  }

  if(S_ISDIR(sbuf.st_mode)) {
    return 1;
  }

return 0;
}


int tagMatch(cfgTag *cfg, osmTag *osm) {
    int tags = 0;
    int matchs = 0;
    osmTag *oTag;
    cfgTag *cTag;
    LIST_FOREACH(cTag, cfg) {
        tags++;
        LIST_FOREACH(oTag, osm) {
            if (strcmp(cTag->key,oTag->key) == 0 &&
                strcmp(cTag->value,oTag->value) == 0) {
                matchs++;
                break;
            }
        }
        oTag = NULL;
    }
    cTag = NULL;
    return (tags == matchs);
}


/*
 * function: drawPath
 * @cr  Array of cairo resources
 * @nd  Liked list if osmNd's
 *
 * This function is used to prepare a Path.
 */
void drawPath(cairo_t *cr[], osmNd *nd) {
    coordinates xy;
    int i;
    
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {
        xy = coord2xy(nd->node->lat,nd->node->lon,i+LAYERMIN);
        cairo_move_to(cr[i],xy.x-offXY[i].x,xy.y-offXY[i].y);
    }
    nd = nd->next;
    while(nd) {
        for (i=0;i<LAYERMAX-LAYERMIN;i++) {
            xy = coord2xy(nd->node->lat,nd->node->lon,i+LAYERMIN);
            cairo_line_to(cr[i],xy.x-offXY[i].x,xy.y-offXY[i].y);
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
void strokePath(cairo_t *cr[]) {
    coordinates xy;
    int i;
    
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {
        cairo_set_line_width (cr[i], 0);
       cairo_stroke(cr[i]);
    }
}

/*
 * function: drawPolygone
 * @cr      Array of cairo resources
 * @draw    A cfgDraw stucture
 *
 * This function fill the prepared paths with the configured color.
 */
void drawPolygone(cairo_t *cr[], cfgDraw *draw) {
    if (debug > 1)
        fprintf(stdout,"drawPolygone\n");
    
    int i;
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {
        cairo_set_fill_rule (cr[i], CAIRO_FILL_RULE_EVEN_ODD);
        cairo_set_source_rgb (cr[i], draw->color[0],draw->color[1],
                                        draw->color[2]);
        cairo_fill_preserve(cr[i]);
    }
}

/*
 * function: drawWay
 * @cr      Array of cairo resources
 * @draw    A cfgDraw stucture
 *
 * This function draw the prepared paths with the configured color.
 */
void drawWay(cairo_t *cr[], cfgDraw *draw) {
    if (debug > 1)
        fprintf(stdout,"drawWay\n");
    
    int i;
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {
        cairo_set_line_cap  (cr[i], CAIRO_LINE_CAP_ROUND);
        cairo_set_line_width (cr[i], draw->width*LINESIZE(i));
        cairo_set_source_rgb (cr[i], draw->color[0],draw->color[1],
                                        draw->color[2]);
        cairo_stroke_preserve(cr[i]);
    }
}

/*
 * function: drawRoad
 * @cr      Array of cairo resources
 * @draw    A cfgDraw stucture
 *
 * This function draw the prepared paths to roads.
 */
void drawRoad(cairo_t *cr[], cfgDraw *draw) {
    if (debug > 1)
        fprintf(stdout,"drawRoad\n");
    
    int i;
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {
        if(i+LAYERMIN<draw->minlayer || i+LAYERMIN>draw->maxlayer) {
                continue;}
        //cairo_set_line_cap  (cr[i], CAIRO_LINE_CAP_BUTT);
        cairo_set_line_cap  (cr[i], CAIRO_LINE_CAP_ROUND);
        cairo_set_line_width (cr[i], draw->width*LINESIZE(i));
        cairo_set_source_rgb (cr[i], draw->border[0], draw->border[1],
                                        draw->border[2]);
        cairo_stroke_preserve(cr[i]);
        
        
        cairo_set_line_cap  (cr[i], CAIRO_LINE_CAP_ROUND);
        cairo_set_line_width (cr[i], draw->width*LINESIZE(i)-4);
        cairo_set_source_rgb (cr[i], draw->color[0],draw->color[1],
                                        draw->color[2]);
        cairo_stroke_preserve(cr[i]);
    }
}

/*
 * function: drawCairo
 * @config  A Linkedlist of cfgLayer
 * @osmSrc  A osmFile struct.
 *
 * This function fill the prepared paths with the configured color.
 */
int drawCairo(cfgLayer *config, osmFile *osmSrc) {
    
    int i;
    
    // Calculate X/Y Draw offset for all layers
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {
        offXY[i] = coord2xy(osmSrc->bounds->maxlat,osmSrc->bounds->minlon,i+LAYERMIN);
    }
    
    cairo_surface_t *surface[LAYERMAX-LAYERMIN];
    cairo_t *cr[LAYERMAX-LAYERMIN];
    
    // Create all Layers cairo environment
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {
        surface[i] = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                TILESIZE*numTiles(i), TILESIZE* numTiles(i));
        cr[i] = cairo_create(surface[i]);
    }
    
    // Vars uder while looping throug data
    osmWay      *way;
    osmTag      *tag;
    cfgLayer    *layer;
    cfgRule     *rule;
    cfgDraw     *draw;
    
    // Process cfgLayer in order.
    LIST_FOREACH(layer, config) {
        fprintf(stdout,"  Layer: %s\n",layer->name);
        int used; 
        
        // Start checking osm from bottom layer.
        for(i=-5;i<=5;i++) {
            
            //Process each rule in the layer in oder
            LIST_FOREACH(rule, layer->rule) {
                
                used = 0;
                strokePath(cr);
                
                // Loop throug osmdata
                LIST_FOREACH(way, osmSrc->ways) {
                    //Only objects on current layer
                    if(way->layer != i)
                        continue;
                    
                    // Check if matching rule and prepare path
                    if (tagMatch(rule->tag, way->tag)) {
                        drawPath(cr, way->nd);
                        used = 1;
                    }
                }
                
                // If a path was prepared draw it
                if (used) {
                    LIST_FOREACH(draw, rule->draw) {
                        if (draw->type == LINE) {
                            drawWay(cr, draw);
                        } else if (draw->type == ROAD) {
                            drawRoad(cr, draw);
                        } else if (draw->type == POLYGONE) {
                            drawPolygone(cr, draw);
                        }
                    }
                }
            }
        }
    }
    
    char *filename;
    filename = malloc(sizeof(char)*50);
    
    if (!isDir("tiles")){
        mkdir("tiles",0777);
    }
    
    if(tiled) {
    
        cairo_surface_t *surfaceOut;
        cairo_t *crOut;
    
        surfaceOut = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                TILESIZE, TILESIZE);
        crOut = cairo_create(surfaceOut);
    
    
        for (i=0;i<LAYERMAX-LAYERMIN;i++) {
            int x, y;
            sprintf(filename,"tiles/%i",i+LAYERMIN);
            if (!isDir(filename)){
                mkdir(filename,0777);
            }
            for (x=0;x<exp2(i);x++) {
                sprintf(filename,"tiles/%i/%i",i+LAYERMIN,x);
                if (!isDir(filename)){
                    mkdir(filename,0777);
                }
                for (y=0;y<exp2(i);y++) {
                    sprintf(filename,"tiles/%i/%i/%02i.png",i+LAYERMIN,x,y);
        
                    // Draw Land. No Costlign/Sea support
                    cairo_rectangle (crOut, 0,0,TILESIZE,TILESIZE);
                    cairo_set_source_rgb(crOut, 0.9, 0.9, 0.9);
                    cairo_fill_preserve(crOut);
        
                    cairo_set_source_surface(crOut,surface[i],256*-x,256*-y);
                    cairo_fill(crOut);
        
                    cairo_surface_write_to_png(surfaceOut, filename);
                }
            }

            cairo_destroy(cr[i]);
            cairo_surface_destroy(surface[i]);
        }
    } else {
            for (i=0;i<LAYERMAX-LAYERMIN;i++) {
                sprintf(filename,"tiles/%02i.png",i+LAYERMIN);
                cairo_surface_write_to_png(surface[i], filename);
            }
    }
}