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

#include "memphis-data-pool.h"
#include "mlib.h"

G_DEFINE_TYPE (MemphisDataPool, memphis_data_pool, G_TYPE_OBJECT)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), MEMPHIS_TYPE_DATA_POOL, MemphisDataPoolPrivate))

typedef struct _MemphisDataPoolPrivate MemphisDataPoolPrivate;

struct _MemphisDataPoolPrivate {
    int dummy;
};

static MemphisDataPool *instance = NULL;

static void
memphis_data_pool_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
memphis_data_pool_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
memphis_data_pool_dispose (GObject *object)
{
  G_OBJECT_CLASS (memphis_data_pool_parent_class)->dispose (object);
}

static void
memphis_data_pool_finalize (GObject *object)
{
  MemphisDataPool *self = (MemphisDataPool *) object;
  g_tree_destroy (self->stringTree);
  g_string_chunk_free (self->stringChunk);
  
  G_OBJECT_CLASS (memphis_data_pool_parent_class)->finalize (object);
}

static void
memphis_data_pool_class_init (MemphisDataPoolClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (MemphisDataPoolPrivate));

  object_class->get_property = memphis_data_pool_get_property;
  object_class->set_property = memphis_data_pool_set_property;
  object_class->dispose = memphis_data_pool_dispose;
  object_class->finalize = memphis_data_pool_finalize;
}

static void
memphis_data_pool_init (MemphisDataPool *self)
{
  self->stringChunk = g_string_chunk_new (265);
  self->stringTree = g_tree_new (m_tree_strcmp);
}

MemphisDataPool*
memphis_data_pool_new (void)
{
  MemphisDataPool *pool;
  
  if (instance == NULL)
    {
      pool = g_object_new (MEMPHIS_TYPE_DATA_POOL, NULL);
      instance = pool;
    }
  else
    {
      pool = g_object_ref (instance);
    }

  return pool;
}
