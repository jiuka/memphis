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
#include "osm05.h"

G_BEGIN_DECLS

#define MEMPHIS_TYPE_MAP memphis_map_get_type()

#define MEMPHIS_MAP(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MEMPHIS_TYPE_MAP, MemphisMap))

#define MEMPHIS_MAP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), MEMPHIS_TYPE_MAP, MemphisMapClass))

#define MEMPHIS_IS_MAP(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MEMPHIS_TYPE_MAP))

#define MEMPHIS_IS_MAP_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), MEMPHIS_TYPE_MAP))

#define MEMPHIS_MAP_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), MEMPHIS_TYPE_MAP, MemphisMapClass))

typedef struct {
  GObject parent;
  osmFile *map;
} MemphisMap;

typedef struct {
  GObjectClass parent_class;
} MemphisMapClass;

GType memphis_map_get_type (void);

MemphisMap* memphis_map_new ();
void memphis_map_free (MemphisMap *map);

void memphis_map_load_from_file (MemphisMap *map, const gchar *filename);
void memphis_map_load_from_data (MemphisMap *map, const gchar *data, guint size);

void memphis_map_set_debug_level (MemphisMap *map, gint8 debug_level);
gint8 memphis_map_get_debug_level (MemphisMap *map);

void memhis_map_get_bounding_box (MemphisMap *map,
    gdouble *minlat,
    gdouble *minlon,
    gdouble *maxlat,
    gdouble *maxlon);

G_END_DECLS

#endif /* _MEMPHIS_MAP */
