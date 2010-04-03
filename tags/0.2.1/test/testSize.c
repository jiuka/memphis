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

#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>

#include "../osm05.h"
#include "../ruleset.h"

int main () {
    
    fprintf(stdout,"gsize: %i\n",(int) sizeof(gsize));
    
    fprintf(stdout,"G_MINFLOAT: %f\n", G_MINFLOAT);
    fprintf(stdout,"G_MAXFLOAT: %f\n", G_MAXFLOAT);
    fprintf(stdout,"G_MINDOUBLE: %f\n", G_MINDOUBLE);
    fprintf(stdout,"G_MAXDOUBLE: %f\n", G_MAXDOUBLE);
    
    fprintf(stdout,"cairo_line_cap_t: %i\n",(int) sizeof(cairo_line_cap_t));
    
    fprintf(stdout,"short int: %i\n",(int) sizeof(short int));
    fprintf(stdout,"guint8: %i\n",(int) sizeof(guint8));
    
    
    fprintf(stdout,"double: %i\n",(int) sizeof(double));
    fprintf(stdout,"float: %i\n",(int) sizeof(float));
    fprintf(stdout,"void*: %i\n",(int) sizeof(void*));
    
    
    fprintf(stdout,"osmTag: %i\n",(int) sizeof(osmTag));
    fprintf(stdout,"osmNode: %i\n",(int) sizeof(osmNode));
    fprintf(stdout,"osmWay: %i\n",(int) sizeof(osmWay));
    
    fprintf(stdout,"cfgRules: %i\n",(int) sizeof(cfgRules));
    fprintf(stdout,"cfgRule: %i\n",(int) sizeof(cfgRule));
    fprintf(stdout,"cfgDraw: %i\n",(int) sizeof(cfgDraw));
	
		
	return (0);
}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */
