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
#include "memphis-rule-set.h"
#include "memphis-data-pool.h"
#include "osm05.h"
#include "ruleset.h"

#ifndef _MEMPHIS_PRIVATE
#define _MEMPHIS_PRIVATE

osmFile *memphis_map_get_osmFile (MemphisMap *map);

cfgRules *memphis_rule_set_get_cfgRules (MemphisRuleSet *ruleset);

GStringChunk *memphis_data_pool_get_string_chunk (MemphisDataPool *self);

GTree *memphis_data_pool_get_string_tree (MemphisDataPool *self);

#endif /* _MEMPHIS_PRIVATE */
