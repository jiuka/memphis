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

#ifndef RENDER_H
#define RENDER_H

#include <cairo.h>

#include "libmercator.h"
#include "ruleset.h"
#include "osm05.h"

typedef struct _renderInfo renderInfo;
struct _renderInfo {
        coordinates     offset;
        cairo_surface_t *surface;
        cairo_t         *cr;
        short int       zoom;
        cfgRules        *ruleset;
        osmFile         *osm;
   };

/*
 * Function Prototype
 */
int renderCairo(cfgRules *ruleset, osmFile *osm);

#endif /* RENDER_H */

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

