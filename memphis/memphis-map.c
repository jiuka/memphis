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

/**
 * SECTION:memphis-map
 * @short_description: OpenStreetMap map data object.
 *
 * Stores an area with OSM map data. The data can be loaded from an OSM
 * XML file or a character array with XML data.
 *
 * Reference: http://wiki.openstreetmap.org/wiki/.osm
 */

#include "memphis-map.h"
#include "osm05.h"

struct _MemphisMap
{
  GObject parent_instance;

  osmFile *map;
};

G_DEFINE_TYPE (MemphisMap, memphis_map, G_TYPE_OBJECT)

static void
memphis_map_finalize (GObject *object)
{
  MemphisMap *self = MEMPHIS_MAP (object);

  g_clear_pointer (&self->map, osmFree);

  G_OBJECT_CLASS (memphis_map_parent_class)->finalize (object);
}

static void
memphis_map_class_init (MemphisMapClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = memphis_map_finalize;
}

static void
memphis_map_init (MemphisMap *self)
{
}

/**
 * memphis_map_new:
 *
 * Returns: a new #MemphisMap without any data.
 *
 * Since: 0.1
 */
MemphisMap*
memphis_map_new (void)
{
  return g_object_new (MEMPHIS_TYPE_MAP, NULL);
}

/**
 * memphis_map_load_from_file:
 * @self: a #MemphisMap
 * @filename: a path to a OSM map file
 * @error: a pointer to a GError or NULL
 *
 * Load map data from an OSM XML file.
 *
 * Since: 0.2
 */
void
memphis_map_load_from_file (MemphisMap *self, const gchar *filename,
    GError **error)
{
  g_return_if_fail (MEMPHIS_IS_MAP (self) && filename != NULL);

  g_clear_pointer (&self->map, osmFree);
  self->map = osmRead (filename, error);
}

/**
 * memphis_map_load_from_data:
 * @self: a #MemphisMap
 * @data: a character array with OSM data
 * @size: the size of the array
 * @error: a pointer to a GError or NULL
 *
 * Load map data from an OSM XML file.
 *
 * Since: 0.2
 */
void
memphis_map_load_from_data (MemphisMap *self, const gchar *data,
    guint size, GError **error)
{
  g_return_if_fail (MEMPHIS_IS_MAP (self) && data != NULL);

  g_clear_pointer (&self->map, osmFree);
  self->map = osmRead_from_buffer (data, size, error);
}

/**
 * memphis_map_get_bounding_box:
 * @self: a #MemphisMap
 * @minlat: (out): the minimum latitude
 * @minlon: (out): the minimum longitude
 * @maxlat: (out): the maximum latitude
 * @maxlon: (out): the maximum longitude
 *
 * Get the exent of the bounding box that contains all map data.
 *
 * Since: 0.1
 */
void
memphis_map_get_bounding_box (MemphisMap *self,
    gdouble *minlat,
    gdouble *minlon,
    gdouble *maxlat,
    gdouble *maxlon)
{
  if (self->map == NULL)
    {
      *minlat = *minlon = *maxlat = *maxlon = 0.0;
      return;
    }

  *minlat = self->map->minlat;
  *minlon = self->map->minlon;
  *maxlat = self->map->maxlat;
  *maxlon = self->map->maxlon;
}

/* private shared functions */

osmFile *
memphis_map_get_osmFile (MemphisMap *self)
{
  g_assert (MEMPHIS_IS_MAP (self));

  return self->map;
}
