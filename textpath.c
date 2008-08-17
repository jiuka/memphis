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
#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>

#include "ruleset.h"
#include "textpath.h"

void point_on_path(cairo_path_t *path, double *lengths, double *x, double *y) {
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


void transform_path(cairo_path_t *path, trans_point_func_t f,
                    cairo_path_t *dpath, double *lengths) {
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

double* pathLength(cairo_path_t *path) {
    int i;
    cairo_path_data_t *data, current_point;
    double *lengths;

    lengths = malloc((path->num_data+1) * sizeof (double));

    for (i=0; i < path->num_data; i += path->data[i].header.length) {
        data = &path->data[i];
        lengths[i] = 0.0;
        switch (data->header.type) {
            case CAIRO_PATH_MOVE_TO:
                current_point = data[1];
                break;
            case CAIRO_PATH_LINE_TO:
                {
                    double dx, dy;
                    dx = data[1].point.x - current_point.point.x;
                    dy = data[1].point.y - current_point.point.y;
                    lengths[i] = sqrt (dx * dx + dy * dy);
                    current_point = data[1];
                }
                break;
            case CAIRO_PATH_CURVE_TO:
            case CAIRO_PATH_CLOSE_PATH:
                break;
        }
        lengths[path->num_data] += lengths[i];
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

}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

