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
 * SECTION:memphis-data-pool
 * @short_description: The Memphis data pool.
 *
 * A singleton that provides a shared pool of efficient memory.
 * (i.e. a #GStringChunk and a #GTree for strings).
 */

#include "memphis-data-pool.h"
#include "mlib.h"


struct _MemphisDataPool
{
  GObject parent;
  GStringChunk *stringChunk;
  GTree *stringTree;
};

G_DEFINE_TYPE (MemphisDataPool, memphis_data_pool, G_TYPE_OBJECT)

static MemphisDataPool *instance = NULL;

static void
memphis_data_pool_finalize (GObject *object)
{
  MemphisDataPool *self = (MemphisDataPool *) object;

  g_clear_pointer (&self->stringTree, g_tree_unref);
  g_clear_pointer (&self->stringChunk, g_string_chunk_free);

  G_OBJECT_CLASS (memphis_data_pool_parent_class)->finalize (object);
}

static void
memphis_data_pool_class_init (MemphisDataPoolClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = memphis_data_pool_finalize;
}

static void
memphis_data_pool_init (MemphisDataPool *self)
{
  self->stringChunk = g_string_chunk_new (265);
  self->stringTree = g_tree_new ((GCompareFunc) g_strcmp0);
}

/**
* memphis_data_pool_new:
*
* Returns: a reference to the #MemphisDataPool.
*
* Since: 0.1
*/
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

GStringChunk *
memphis_data_pool_get_string_chunk (MemphisDataPool *self)
{
  g_return_val_if_fail (MEMPHIS_IS_DATA_POOL (self), NULL);

  return self->stringChunk;
}

GTree *
memphis_data_pool_get_string_tree (MemphisDataPool *self)
{
  g_return_val_if_fail (MEMPHIS_IS_DATA_POOL (self), NULL);

  return self->stringTree;
}
