#include <cairo.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "config02.h"
#include "osm05.h"
#include "main.h"
#include "ll.h"

#define LAYERMIN     12
#define LAYERMAX     18

#define TILESIZE(z)     (int)(256*exp2(z))
#define LINESIZE(z)     (int)(exp2(z)/(z+1))

float offLat;
float offLon;
float scaLat[LAYERMAX-LAYERMIN];
float scaLon[LAYERMAX-LAYERMIN];


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

void drawPolygone(cairo_t *cr[], osmNd *nd, float r,float g,float b) {
    if (debug > 1)
        fprintf(stdout,"drawPolygone\n");
    
    int i;
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {
        cairo_stroke(cr[i]);
        cairo_move_to(cr[i],
                    (nd->node->lon - offLon)*scaLon[i],
                    TILESIZE(i)-(nd->node->lat - offLat)*scaLat[i]);
    }
    nd = nd->next;
    while(nd) {
        for (i=0;i<LAYERMAX-LAYERMIN;i++) {
            cairo_line_to (cr[i],
                    (nd->node->lon - offLon)*scaLon[i],
                    TILESIZE(i)-(nd->node->lat - offLat)*scaLat[i]);
        }
        nd = nd->next;
    }              
    
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {
        cairo_set_fill_rule (cr[i], CAIRO_FILL_RULE_EVEN_ODD);
        cairo_set_source_rgb (cr[i], r,g,b); cairo_fill(cr[i]);
    }
}

void drawWay(cairo_t *cr[], osmNd *nd, int width, float r,float g,float b) {
    if (debug > 1)
        fprintf(stdout,"drawWay\n");
    
    int i;
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {
        cairo_stroke(cr[i]);
        cairo_set_line_cap  (cr[i], CAIRO_LINE_CAP_ROUND);
        cairo_set_line_width (cr[i], width*LINESIZE(i));
        cairo_move_to(cr[i],
                    (nd->node->lon - offLon)*scaLon[i],
                    TILESIZE(i)-(nd->node->lat - offLat)*scaLat[i]);
    }
    nd = nd->next;
    while(nd) {
        for (i=0;i<LAYERMAX-LAYERMIN;i++) {
            cairo_line_to (cr[i],
                    (nd->node->lon - offLon)*scaLon[i],
                    TILESIZE(i)-(nd->node->lat - offLat)*scaLat[i]);
        }
        nd = nd->next;
    }
    
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {  
        cairo_set_source_rgb (cr[i], r,g,b); cairo_stroke(cr[i]);
    }
}

int drawCairo2(cfgLayer *config, osmFile *osmSrc) {
    
    offLat = osmSrc->bounds->minlat;
    offLon = osmSrc->bounds->minlon;
    int i;
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {
        scaLat[i] = TILESIZE(i)/(osmSrc->bounds->maxlat - osmSrc->bounds->minlat);
        scaLon[i] = TILESIZE(i)/(osmSrc->bounds->maxlon - osmSrc->bounds->minlon);
    }
    
    cairo_surface_t *surface[LAYERMAX-LAYERMIN];
    cairo_t *cr[LAYERMAX-LAYERMIN];
    
    // Create Layers
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {
        surface[i] = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                TILESIZE(i), TILESIZE(i));
        cr[i] = cairo_create(surface[i]);
    }
    
    
    osmWay *way;
    osmTag *tag;
    cfgLayer    *layer;
    cfgRule     *rule;
    cfgDraw     *draw;
    
    LIST_FOREACH(layer, config) {
        fprintf(stdout,"  Layer: %s\n",layer->name);
        
        for(i=-5;i<=5;i++) {
            LIST_FOREACH(rule, layer->rule) {
                LIST_FOREACH(way, osmSrc->ways) {
                    if(way->layer != i)
                        continue;
                    if (tagMatch(rule->tag, way->tag)) {
                        LIST_FOREACH(draw, rule->draw) {
                            if (draw->type == LINE) {
                                drawWay(cr, way->nd,draw->width,
                                        draw->color[0], draw->color[1],
                                        draw->color[2]);
                            } else if (draw->type == POLYGONE) {
                                drawPolygone(cr, way->nd, draw->color[0],
                                        draw->color[1], draw->color[2]);
                            }
                        }
                    }
                }
            }
        }
    }
    
    char *filename;
    filename = malloc((size_t) 50);
    
    cairo_surface_t *surfaceOut;
    cairo_t *crOut;
    
    surfaceOut = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                TILESIZE(0), TILESIZE(0));
    crOut = cairo_create(surfaceOut);
    
    if (!isDir("tiles")){
        mkdir("tiles",0777);
    }
    
    
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
                cairo_rectangle (crOut, 0,0,TILESIZE(0),TILESIZE(0));
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
}