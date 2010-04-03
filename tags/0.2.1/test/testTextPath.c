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
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>

#include "../textpath.h"
#include "../ruleset.h"

int main () {
	cairo_surface_t *surf;
	cairo_t *cr;
	cfgDraw *draw;
	
	draw = malloc(sizeof(cfgDraw));
	
	draw->color[0] = 0;
	draw->color[1] = 0;
	draw->color[2] = 0;

	double width=400, height=400;

	surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	cr = cairo_create (surf);

	cairo_rectangle (cr, 0, 0, width, height);
	cairo_set_source_rgb (cr, 1,1,1);
	cairo_fill (cr);
	
	cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                            CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_set_font_size (cr, 20.0);

    GTimer *t1 = g_timer_new();
	
	//Test1
    cairo_move_to(cr,  50,  50);
    cairo_line_to(cr, 350,  50);
    
    cairo_set_source_rgb (cr, 0,0,1);
    cairo_set_line_width (cr, 1.0);
    cairo_stroke_preserve(cr);
    
    textPath(cr, "Test1");
	
	//Test2
    cairo_move_to(cr,  50, 100);
    cairo_line_to(cr, 350, 100);
    cairo_line_to(cr, 350, 350);
    
    cairo_set_source_rgb (cr, 0,0,1);
    cairo_set_line_width (cr, 1.0);
    cairo_stroke_preserve(cr);
    
    textPath(cr, "Test2");
	
	//Test3
    cairo_move_to(cr,  50, 150);
    cairo_line_to(cr, 200, 150);
    cairo_curve_to(cr,250, 150, 300, 200, 300, 250);
    cairo_line_to(cr, 300, 350);
    
    cairo_set_source_rgb (cr, 0,0,1);
    cairo_set_line_width (cr, 1.0);
    cairo_stroke_preserve(cr);
    
    textPath(cr, "Test3");
	
	//Test4
    cairo_move_to(cr,  50, 200);
    cairo_line_to(cr, 200, 200);
    cairo_curve_to(cr,250, 200, 250, 200, 250, 250);
    cairo_line_to(cr, 250, 300);
    cairo_curve_to(cr, 250, 350, 250, 350, 200, 350);
    cairo_line_to(cr, 150, 350);
    cairo_curve_to(cr, 50, 350, 50, 250, 150, 250);
    
    cairo_curve_to(cr, 175, 250, 175, 300, 200, 300);
    
    cairo_set_source_rgb (cr, 0,0,1);
    cairo_set_line_width (cr, 1.0);
    cairo_stroke_preserve(cr);
    
    textPath(cr, "Test4");
    
    g_timer_stop(t1);

    
	cairo_surface_write_to_png (surf, "testTextPath.png");
	cairo_destroy (cr);
	cairo_surface_destroy (surf);
	
	//////////////////////////////////////////////////////

	surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	cr = cairo_create (surf);

	cairo_rectangle (cr, 0, 0, width, height);
	cairo_set_source_rgb (cr, 1,1,1);
	cairo_fill (cr);
	
	cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                            CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_set_font_size (cr, 20.0);

    GTimer *t2 = g_timer_new();
	
	//Test1
    cairo_move_to(cr,  50,  50);
    cairo_line_to(cr, 350,  50);
    
    cairo_set_source_rgb (cr, 0,0,1);
    cairo_set_line_width (cr, 1.0);
    cairo_stroke_preserve(cr);
    
    charPath(cr, "Test1");

	//Test2
    cairo_move_to(cr,  50, 100);
    cairo_line_to(cr, 350, 100);
    cairo_line_to(cr, 350, 350);
    
    cairo_set_source_rgb (cr, 0,0,1);
    cairo_set_line_width (cr, 1.0);
    cairo_stroke_preserve(cr);
    
    charPath(cr, "Test2");
	
	//Test3
    cairo_move_to(cr,  50, 150);
    cairo_line_to(cr, 200, 150);
    cairo_curve_to(cr,250, 150, 300, 200, 300, 250);
    cairo_line_to(cr, 300, 350);
    
    cairo_set_source_rgb (cr, 0,0,1);
    cairo_set_line_width (cr, 1.0);
    cairo_stroke_preserve(cr);
    
    charPath(cr, "Test3");
	
	//Test4
    cairo_move_to(cr,  50, 200);
    cairo_line_to(cr, 200, 200);
    cairo_curve_to(cr,250, 200, 250, 200, 250, 250);
    cairo_line_to(cr, 250, 300);
    cairo_curve_to(cr, 250, 350, 250, 350, 200, 350);
    cairo_line_to(cr, 150, 350);
    cairo_curve_to(cr, 50, 350, 50, 250, 150, 250);
    
    cairo_curve_to(cr, 175, 250, 175, 300, 200, 300);
    
    cairo_set_source_rgb (cr, 0,0,1);
    cairo_set_line_width (cr, 1.0);
    cairo_stroke_preserve(cr);
    
    charPath(cr, "Test4");
    
    g_timer_stop(t2);

	cairo_surface_write_to_png (surf, "testCharPath.png");
	cairo_destroy (cr);
	cairo_surface_destroy (surf);
	
	fprintf(stdout,"T1 %f\nT2 %f\n", g_timer_elapsed(t1,NULL), g_timer_elapsed(t2,NULL));
	
	return (0);
}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */
