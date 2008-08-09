#include <cairo.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "config01.h"
#include "osm05.h"
#include "main.h"
#include "ll.h"

#define LAYERMIN     12
#define LAYERMAX     18

#define TILESIZE(z)     (int)(256*exp2(z))
#define LINESIZE(z)     (int)(exp2(z))

float offLat;
float offLon;
float scaLat[LAYERMAX-LAYERMIN];
float scaLon[LAYERMAX-LAYERMIN];

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

int drawCairo(cfgFile *config, osmFile *osmSrc) {
    if (debug > 1)
        fprintf(stdout,"drawCairo\n");
    offLat = osmSrc->bounds->minlat;
    offLon = osmSrc->bounds->minlon;
    int i;
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {
        scaLat[i] = TILESIZE(i)/(osmSrc->bounds->maxlat - osmSrc->bounds->minlat);
        scaLon[i] = TILESIZE(i)/(osmSrc->bounds->maxlon - osmSrc->bounds->minlon);
    }
    
    cairo_surface_t *surface[LAYERMAX-LAYERMIN];
    cairo_t *cr[LAYERMAX-LAYERMIN];
    
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {
        surface[i] = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                TILESIZE(i), TILESIZE(i));
        cr[i] = cairo_create(surface[i]);
        // Draw Land. No Costlign/Sea support
        cairo_rectangle (cr[i], 0,0,TILESIZE(i),TILESIZE(i));
        cairo_set_source_rgb(cr[i], 0.95, 0.95, 0.95);
        cairo_fill(cr[i]);
    }
    
    osmWay *way;
    osmTag *tag;
    cfgWay *cfgWay;
    cfgTag *cfgTag;
    int tags, matchs;

    for(i=-5;i<=5;i++) {
        LIST_FOREACH(cfgWay, config->ways) {
            LIST_FOREACH(way, osmSrc->ways) {
                tags = 0;
                matchs = 0;
                LIST_FOREACH(cfgTag, cfgWay->tag) {
                    tags++;
                    LIST_FOREACH(tag, way->tag) {
                        if (strcmp(cfgTag->key,tag->key) == 0 &&
                                strcmp(cfgTag->value,tag->value) == 0) {
                            matchs++;
                            break;
                        }
                    }
                    tag = NULL;
                }
                cfgTag = NULL;
                if (tags == matchs) {
                    if (cfgWay->type == POLYGONE) {
                        drawPolygone(cr, way->nd,cfgWay->color[0],
                                        cfgWay->color[1], cfgWay->color[2]);
                    } else if (cfgWay->type == STREET) {
                        drawWay(cr, way->nd,cfgWay->width,cfgWay->color[0],
                                        cfgWay->color[1], cfgWay->color[2]);
                    }
                }
            }
        }
    }
  /*
  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 40.0);

  cairo_move_to(cr, 10.0, 50.0);
  cairo_show_text(cr, "Disziplin ist Macht.");
  
  cairo_rectangle (cr, 12, 12, 232, 70);
  cairo_new_sub_path (cr);
  cairo_arc (cr, 64, 64, 40, 0, 2*3.1415);
  cairo_new_sub_path (cr);
  cairo_arc_negative (cr, 192, 64, 40, 0, -2*3.1415);

  cairo_set_fill_rule (cr, CAIRO_FILL_RULE_EVEN_ODD);
cairo_set_source_rgb (cr, 0, 0.7, 0); cairo_fill_preserve (cr);
cairo_set_source_rgb (cr, 0, 0, 0); cairo_stroke (cr);

cairo_set_line_cap  (cr, CAIRO_LINE_CAP_ROUND);

cairo_set_line_width (cr, 10.0);
cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
cairo_move_to (cr, 128.0, 50.0);
cairo_line_to (cr, 128.0, 200.0);
cairo_stroke (cr);

cairo_set_line_width (cr2, 8.0);
cairo_set_source_rgb(cr2, 0.8, 0.8, 0.8);
cairo_move_to (cr2, 50, 128);
cairo_line_to (cr2, 200, 128);
cairo_stroke (cr2);


*/
    char *filename;
    filename = malloc((size_t) 12);
    for (i=0;i<LAYERMAX-LAYERMIN;i++) {
        sprintf(filename,"layer%02i.png",i+LAYERMIN);
        
        cairo_surface_write_to_png(surface[i], filename);

        cairo_destroy(cr[i]);
        cairo_surface_destroy(surface[i]);
    }

}