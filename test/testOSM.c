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
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../memphis/osm05.h"
#include "../memphis/main.h"
#include "../memphis/mlib.h"


memphisOpt  *opts;
GStringChunk *stringChunk;
GTree        *stringTree;

int main () {

    opts = malloc(sizeof(memphisOpt));
    opts->debug=1;

    stringChunk = g_string_chunk_new(256);
    stringTree = g_tree_new((GCompareFunc) g_strcmp0);

    osmFile *osm = (osmFile *) osmRead("test/map.osm", 1);

    g_tree_unref(stringTree);

    osmFree(osm);

    g_string_chunk_free(stringChunk);

    g_free(opts);

	return (0);
}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */
