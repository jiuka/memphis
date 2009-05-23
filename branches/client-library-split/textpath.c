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
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ruleset.h"
#include "textpath.h"

static void point_on_path(cairo_path_t *path, double *lengths, double *x, double *y) {
    int i;
    double ratio, the_y = *y, the_x = *x, dx, dy;
    cairo_path_data_t *data, current_point;

    for (i=0; i + path->data[i].header.length < path->num_data &&
	    (the_x > lengths[i] || path->data[i].header.type == CAIRO_PATH_MOVE_TO);
        i += path->data[i].header.length) {

        the_x -= lengths[i];
        data = &path->data[i];
        switch (data->header.type) {
            case CAIRO_PATH_MOVE_TO:
                current_point = data[1];
                break;
            case CAIRO_PATH_LINE_TO:
                current_point = data[1];
                break;
            case CAIRO_PATH_CURVE_TO:
                current_point = data[3];
                break;
            case CAIRO_PATH_CLOSE_PATH:
                break;
        }
    }
    data = &path->data[i];

    switch (data->header.type) {
        case CAIRO_PATH_MOVE_TO:
            break;
        case CAIRO_PATH_LINE_TO:
            {
                ratio = the_x / lengths[i];
                /* Line polynomial */
                *x = current_point.point.x * (1 - ratio) + data[1].point.x * ratio;
                *y = current_point.point.y * (1 - ratio) + data[1].point.y * ratio;

                /* Line gradient */
                dx = -(current_point.point.x - data[1].point.x);
                dy = -(current_point.point.y - data[1].point.y);

                /*optimization for: ratio = the_y / sqrt (dx * dx + dy * dy);*/
                ratio = the_y / lengths[i];
                *x += -dy * ratio;
                *y +=  dx * ratio;
            }
            break;
        case CAIRO_PATH_CURVE_TO:
        case CAIRO_PATH_CLOSE_PATH:
            break;
    }
}


static void transform_path(cairo_path_t *path,
                           trans_point_func_t f,
                           cairo_path_t *dpath,
                           double *lengths)
{
    int i;
    cairo_path_data_t *data;

    for (i=0; i < path->num_data; i += path->data[i].header.length) {
        data = &path->data[i];
        switch (data->header.type) {
            case CAIRO_PATH_CURVE_TO:
                f (dpath, lengths, &data[3].point.x, &data[3].point.y);
                f (dpath, lengths, &data[2].point.x, &data[2].point.y);
            case CAIRO_PATH_MOVE_TO:
            case CAIRO_PATH_LINE_TO:
                f (dpath, lengths, &data[1].point.x, &data[1].point.y);
                break;
            case CAIRO_PATH_CLOSE_PATH:
                break;
        }
    }
}

static double* pathLength(cairo_path_t *path) {
    int i;
    cairo_path_data_t *data, current_point;
    double *lengths;

    lengths = g_malloc0((path->num_data+1) * sizeof (double));

    for (i=0; i < path->num_data; i += path->data[i].header.length) {
        data = &path->data[i];
                
        switch (data->header.type) {
            case CAIRO_PATH_MOVE_TO:
                current_point = data[1];
                break;
            case CAIRO_PATH_LINE_TO:
                {
                    double dx, dy;
                    dx = data[1].point.x - current_point.point.x;
                    dy = data[1].point.y - current_point.point.y;
                    lengths[i] = hypot(dx, dy);
                    current_point = data[1];
                    lengths[path->num_data] += lengths[i];
                }
                break;
            case CAIRO_PATH_CURVE_TO:
            case CAIRO_PATH_CLOSE_PATH:
                break;
        }
    }

    return lengths;
}

void textPath(cairo_t *cr, char *text) {
    int i, n;
    double *lengths, x, y;
    cairo_path_t *path;
    cairo_text_extents_t extents;

    path = cairo_copy_path_flat(cr);

    lengths = pathLength(path);

    cairo_text_extents (cr, text, &extents);

    n = (int) (lengths[path->num_data]/extents.width/6);
    if (n == 0 && lengths[path->num_data] > extents.width)
        n = 1;
    
    cairo_new_path (cr);

    y = 0-(extents.height/2 + extents.y_bearing);
    for(i=0;i<n;i++) {

        x = (lengths[path->num_data]/n/2)*((2*i)+1)-(extents.width/2 + extents.x_bearing);

        cairo_move_to (cr, x,y);
        cairo_text_path (cr, text);
    }

    // Map o path
    cairo_path_t *current_path;
    current_path = cairo_copy_path (cr);
    cairo_new_path (cr);

    transform_path (current_path,
		  (trans_point_func_t) point_on_path, path, lengths);

    cairo_append_path (cr, current_path);

    cairo_fill(cr);
    
    // Free memory
    cairo_path_destroy(path);
    cairo_path_destroy(current_path);
    g_free(lengths);

}

/*
 * function: setMatrix
 */
static void setMatrix(cairo_path_t *path, cairo_t *cr, double  *lengths,
                      double offset, double width) {
    int i;
    double ratio, dx, dy, ox, oy, sin, cos, off;
    cairo_matrix_t matrix;
    
    off = offset;
    
    // Calculating Start Point
    for(i=0; i<path->num_data; i+=2) {
        if(lengths[i]>=offset)
            break;
        offset -= lengths[i];
    }
    
    ratio = offset / lengths[i];
    dx = path->data[i-1].point.x*ratio + path->data[i+1].point.x*(1-ratio);
    dy = path->data[i-1].point.y*ratio + path->data[i+1].point.y*(1-ratio);
    
    // Calculating Middle Point
    offset = off + width/2;
    for(i=0; i<path->num_data; i+=2) {
        if(lengths[i]>=offset)
            break;
        offset -= lengths[i];
    }
    
    ratio = offset / lengths[i];
    ox = path->data[i-1].point.x*(1-ratio) + path->data[i+1].point.x*(ratio);
    oy = path->data[i-1].point.y*(1-ratio) + path->data[i+1].point.y*(ratio);

    // Calculating End Point
    offset = off + width;;
    for(i=0; i<path->num_data; i+=2) {
        if(lengths[i]>=offset)
            break;
        offset -= lengths[i];
    }
    
    ratio = offset / lengths[i];
    dx -= path->data[i-1].point.x*ratio + path->data[i+1].point.x*(1-ratio);
    dy -= path->data[i-1].point.y*ratio + path->data[i+1].point.y*(1-ratio);
    
    ratio = hypot(dx,dy);

    sin = dy/ratio;
    cos = dx/ratio;

    cairo_matrix_init(&matrix, cos, sin, -sin, cos, ox, oy);
    cairo_set_matrix(cr, &matrix);
}

/*
 * function: charPath
 */
void charPath(cairo_t *cr, char *text) {
    int n, i, charNum;
    char *str;
    cairo_path_t *path;
    cairo_matrix_t matrix;
    cairo_text_extents_t extents, extents_char, extents_str;
    double *lengths, offset, delta, x, y;

    path = cairo_copy_path_flat(cr);    // Get the current path from cairo

    lengths = pathLength(path);        // Calculate length of path.

    cairo_text_extents(cr, text, &extents);

    n = (int) (lengths[path->num_data]/extents.width/6);
    if (n == 0 && lengths[path->num_data] > extents.width)
        n = 1;
    
    delta = lengths[path->num_data]/n;
    offset = delta/2 - extents.width;
    
    cairo_new_path (cr);
    
    for(charNum=0; charNum <strlen(text); charNum++) {
        str = g_strdup(text);
        str[charNum+1] = '\0';
        cairo_text_extents (cr, str, &extents_str);
       
        strncpy(str,text+charNum,1);
        str[1] = '\0';
        cairo_text_extents (cr, str, &extents_char);

        for(i=0; i<n; i++) {
            setMatrix(path, cr, lengths,
                      (extents_str.width-extents_char.width)+offset+(delta*i),
                      extents_char.width);
            
            x = 0.0-(extents_char.width/2 + extents_char.x_bearing);
            y = 0.0-(extents.height/2 + extents.y_bearing);
            cairo_move_to (cr, x, y);
            cairo_show_text (cr, str);
       }
       g_free(str);
    }
    
    cairo_matrix_init(&matrix, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    cairo_set_matrix(cr, &matrix);

}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

