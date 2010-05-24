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

G_DEFINE_TYPE (MemphisMap, memphis_map, G_TYPE_OBJECT)

#define MEMPHIS_MAP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), MEMPHIS_TYPE_MAP, MemphisMapPrivate))

enum
{
  PROP_0,
};

typedef struct _MemphisMapPrivate MemphisMapPrivate;

struct _MemphisMapPrivate {
  osmFile *map;
};

static void
memphis_map_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  //MemphisMap *self = MEMPHIS_MAP (object);
  //MemphisMapPrivate *priv = MEMPHIS_MAP_GET_PRIVATE (self);
  switch (property_id)
  {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
memphis_map_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
  //MemphisMap *self = MEMPHIS_MAP (object);
  switch (property_id)
  {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);;
  }
}

static void
memphis_map_finalize (GObject *object)
{
  MemphisMap *self = MEMPHIS_MAP (object);
  MemphisMapPrivate *priv = MEMPHIS_MAP_GET_PRIVATE (self);

  if (priv->map != NULL)
    osmFree (priv->map);
  G_OBJECT_CLASS (memphis_map_parent_class)->finalize (object);
}

static void
memphis_map_class_init (MemphisMapClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (MemphisMapPrivate));

  object_class->get_property = memphis_map_get_property;
  object_class->set_property = memphis_map_set_property;
  object_class->finalize = memphis_map_finalize;
}

static void
memphis_map_init (MemphisMap *self)
{
  MemphisMapPrivate *priv = MEMPHIS_MAP_GET_PRIVATE (self);
  priv->map = NULL;
}

/**
 * memphis_map_new:
 *
 * Returns: a new #MemphisMap without any data.
 *
 * Since: 0.1
 */
MemphisMap*
memphis_map_new ()
{
  return g_object_new (MEMPHIS_TYPE_MAP, NULL);
}

/**
 * memphis_map_load_from_file:
 * @map: a #MemphisMap
 * @filename: a path to a OSM map file
 * @error: a pointer to a GError or NULL
 *
 * Load map data from an OSM XML file.
 *
 * Since: 0.2
 */
void
memphis_map_load_from_file (MemphisMap *map, const gchar *filename,
    GError **error)
{
  g_return_if_fail (MEMPHIS_IS_MAP (map) && filename != NULL);

  MemphisMapPrivate *priv = MEMPHIS_MAP_GET_PRIVATE (map);
  if (priv->map != NULL)
    osmFree (priv->map);

  priv->map = osmRead (filename, error);
}

/**
 * memphis_map_load_from_data:
 * @map: a #MemphisMap
 * @data: a character array with OSM data
 * @size: the size of the array
 * @error: a pointer to a GError or NULL
 *
 * Load map data from an OSM XML file.
 *
 * Since: 0.2
 */
void
memphis_map_load_from_data (MemphisMap *map, const gchar *data,
    guint size, GError **error)
{
  g_return_if_fail (MEMPHIS_IS_MAP (map) && data != NULL);

  MemphisMapPrivate *priv = MEMPHIS_MAP_GET_PRIVATE (map);
  if (priv->map != NULL)
    osmFree (priv->map);

  priv->map = osmRead_from_buffer (data, size, error);
}

/**
 * memphis_map_free:
 * @map: a #MemphisMap
 *
 * Frees the memory of a #MemphisMap.
 *
 * Since: 0.1
 */
void
memphis_map_free (MemphisMap *map)
{
  g_object_unref (G_OBJECT (map));
}

/**
 * memphis_map_get_bounding_box:
 * @map: a #MemphisMap
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
memphis_map_get_bounding_box (MemphisMap *map,
    gdouble *minlat,
    gdouble *minlon,
    gdouble *maxlat,
    gdouble *maxlon)
{
  MemphisMapPrivate *priv = MEMPHIS_MAP_GET_PRIVATE (map);
  if (priv->map == NULL)
    {
      *minlat = *minlon = *maxlat = *maxlon = 0.0;
      return;
    }

  *minlat = priv->map->minlat;
  *minlon = priv->map->minlon;
  *maxlat = priv->map->maxlat;
  *maxlon = priv->map->maxlon;
}

/* private shared functions */

osmFile *
memphis_map_get_osmFile (MemphisMap *map)
{
  g_assert (MEMPHIS_IS_MAP (map));

  MemphisMapPrivate *priv = MEMPHIS_MAP_GET_PRIVATE (map);
  return priv->map;
}
