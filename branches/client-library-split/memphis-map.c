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

#include "memphis-map.h"
#include "osm05.h"

G_DEFINE_TYPE (MemphisMap, memphis_map, G_TYPE_OBJECT)

#define MEMPHIS_MAP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), MEMPHIS_TYPE_MAP, MemphisMapPrivate))

enum
{
  PROP_0,
  PROP_DEBUG_LEVEL
};

typedef struct _MemphisMapPrivate MemphisMapPrivate;

struct _MemphisMapPrivate {
  gint8 debug_level;
};

static void
memphis_map_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  MemphisMap *self = MEMPHIS_MAP (object);
  MemphisMapPrivate *priv = MEMPHIS_MAP_GET_PRIVATE (self);
  switch (property_id)
  {
    case PROP_DEBUG_LEVEL:
      g_value_set_int (value, priv->debug_level);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
memphis_map_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
  MemphisMap *self = MEMPHIS_MAP (object);
  switch (property_id)
  {
    case PROP_DEBUG_LEVEL:
        memphis_map_set_debug_level (self, g_value_get_int (value));
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);;
  }
}

static void
memphis_map_finalize (GObject *object)
{
  MemphisMap *self = MEMPHIS_MAP (object);

  if (self->map != NULL)
    osmFree (self->map);
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

  /**
  * MemphisMap:debug-level:
  *
  * The debug level of the map parser.
  *
  * Since: 0.1
  */
  g_object_class_install_property (object_class,
      PROP_DEBUG_LEVEL,
      g_param_spec_int ("debug-level",
        "Debug level",
        "The parser debug level",
        0,
        2,
        1,
        G_PARAM_READWRITE));
}

static void
memphis_map_init (MemphisMap *self)
{
  MemphisMapPrivate *priv = MEMPHIS_MAP_GET_PRIVATE (self);
  self->map = NULL;
  priv->debug_level = 1;
}

MemphisMap*
memphis_map_new ()
{
  return g_object_new (MEMPHIS_TYPE_MAP, NULL);
}

void
memphis_map_load_from_file (MemphisMap *map, const gchar *filename)
{
  g_return_if_fail (MEMPHIS_IS_MAP (map) && filename != NULL);

  MemphisMapPrivate *priv = MEMPHIS_MAP_GET_PRIVATE (map);
  if (map->map != NULL)
    osmFree (map->map);

  map->map = osmRead (filename, priv->debug_level);
}

void
memphis_map_load_from_data (MemphisMap *map, const gchar *data, guint size)
{
  g_return_if_fail (MEMPHIS_IS_MAP (map) && data != NULL);

  MemphisMapPrivate *priv = MEMPHIS_MAP_GET_PRIVATE (map);
  if (map->map != NULL)
    osmFree (map->map);

  map->map = osmRead_from_buffer (data, size, priv->debug_level);
}

void
memphis_map_free (MemphisMap *map)
{
  g_object_unref (G_OBJECT (map));
}

void
memphis_map_set_debug_level (MemphisMap *map,
    gint8 debug_level)
{
  g_return_if_fail (MEMPHIS_IS_MAP (map));

  MemphisMapPrivate *priv = MEMPHIS_MAP_GET_PRIVATE (map);
  priv->debug_level = debug_level;
}

gint8
memphis_map_get_debug_level (MemphisMap *map)
{
  g_return_val_if_fail (MEMPHIS_IS_MAP (map), -1);

  MemphisMapPrivate *priv = MEMPHIS_MAP_GET_PRIVATE (map);
  return priv->debug_level;
}

void
memhis_map_get_bounding_box (MemphisMap *map,
    gdouble *minlat,
    gdouble *minlon,
    gdouble *maxlat,
    gdouble *maxlon)
{
  *minlat = map->map->minlat;
  *minlon = map->map->minlon;
  *maxlat = map->map->maxlat;
  *maxlon = map->map->maxlon;
}
