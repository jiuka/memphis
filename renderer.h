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

#ifndef RENDER_H
#define RENDER_H

#include <cairo.h>

#include "libmercator.h"
#include "ruleset.h"
#include "osm05.h"

typedef struct renderInfo_ renderInfo;
struct renderInfo_ {
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

