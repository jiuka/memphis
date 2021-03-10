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

#ifndef _MEMPHIS_DATA_POOL
#define _MEMPHIS_DATA_POOL

#include <glib-object.h>

G_BEGIN_DECLS

#define MEMPHIS_TYPE_DATA_POOL memphis_data_pool_get_type ()
G_DECLARE_FINAL_TYPE (MemphisDataPool, memphis_data_pool, MEMPHIS, DATA_POOL, GObject)

MemphisDataPool* memphis_data_pool_new (void);

G_END_DECLS

#endif /* _MEMPHIS_DATA_POOL */
