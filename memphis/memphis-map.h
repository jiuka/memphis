/*
 * Memphis - Cairo Rederer for OSM in C
 * Copyright (C) 2009  Simon Wenner <simon@wenner.ch>
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

#ifndef _MEMPHIS_MAP
#define _MEMPHIS_MAP

#include <glib-object.h>

G_BEGIN_DECLS

#define MEMPHIS_TYPE_MAP memphis_map_get_type ()
G_DECLARE_FINAL_TYPE (MemphisMap, memphis_map, MEMPHIS, MAP, GObject)

MemphisMap* memphis_map_new (void);

gboolean memphis_map_load_from_file (MemphisMap *self,
    const gchar *filename,
    GError **error);
gboolean memphis_map_load_from_data (MemphisMap *self,
    const gchar *data,
    guint size,
    GError **error);

void memphis_map_get_bounding_box (MemphisMap *self,
    gdouble *minlat,
    gdouble *minlon,
    gdouble *maxlat,
    gdouble *maxlon);

G_END_DECLS

#endif /* _MEMPHIS_MAP */
