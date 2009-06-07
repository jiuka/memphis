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

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), MEMPHIS_TYPE_MAP, MemphisMapPrivate))

typedef struct _MemphisMapPrivate MemphisMapPrivate;

struct _MemphisMapPrivate {
    int dummy;
};

static void
memphis_map_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
memphis_map_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
memphis_map_dispose (GObject *object)
{
  G_OBJECT_CLASS (memphis_map_parent_class)->dispose (object);
}

static void
memphis_map_finalize (GObject *object)
{
  MemphisMap *self = MEMPHIS_MAP (object);

  osmFree(self->map);
  G_OBJECT_CLASS (memphis_map_parent_class)->finalize (object);
}

static void
memphis_map_class_init (MemphisMapClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (MemphisMapPrivate));

  object_class->get_property = memphis_map_get_property;
  object_class->set_property = memphis_map_set_property;
  object_class->dispose = memphis_map_dispose;
  object_class->finalize = memphis_map_finalize;
}

static void
memphis_map_init (MemphisMap *self)
{
  self->map = NULL;
  self->debug_level = 1;
}

MemphisMap*
memphis_map_new_from_file (gchar* filename)
{
  MemphisMap* mmap = g_object_new (MEMPHIS_TYPE_MAP, NULL);
  mmap->map = osmRead(filename, mmap->debug_level);
  return mmap;
}

MemphisMap*
memphis_map_new_from_data (gchar* data)
{
  // TODO
  return g_object_new (MEMPHIS_TYPE_MAP, NULL);
}

void
memphis_map_free (MemphisMap* map)
{
  g_object_unref (G_OBJECT (map));
}
