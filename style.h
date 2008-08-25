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

#ifndef STYLE_H
#define STYLE_H

#include <glib.h>
#include <cairo.h>

typedef struct _cssStyle cssStyle;
struct _cssStyle {
    cssStyle            *next;
    guint8              backgroundcolor[3];
    guint8              width;
    guint8              bordercolor[3];
    guint8              borderwidth;
    cairo_line_cap_t    linecap:4;
    cairo_line_join_t   linejoin:4;
    char                *dash;
};

/*
 * Prototypes
 */

cssStyle* getStyle(cssStyle *styles, char **class);
cssStyle* styleRead(GSList *styles, int zoom);
void styleFree(cssStyle *style);

#endif /* STYLE_H */

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */
