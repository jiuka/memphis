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

#ifndef __MEMPHIS_RENDERER_H__
#define __MEMPHIS_RENDERER_H__

#include <glib-object.h>
#include <cairo.h>
#include "memphis.h"

G_BEGIN_DECLS

#define MEMPHIS_TYPE_RENDERER memphis_renderer_get_type ()
G_DECLARE_FINAL_TYPE (MemphisRenderer, memphis_renderer, MEMPHIS, RENDERER, GObject)

MemphisRenderer* memphis_renderer_new (void);
MemphisRenderer* memphis_renderer_new_full (MemphisRuleSet *rules,
    MemphisMap *map);

void memphis_renderer_set_resolution (MemphisRenderer *renderer,
    guint resolution);
void memphis_renderer_set_map (MemphisRenderer *renderer,
    MemphisMap* map);
void memphis_renderer_set_rule_set (MemphisRenderer *renderer,
    MemphisRuleSet* rules);

guint memphis_renderer_get_resolution (MemphisRenderer *renderer);
MemphisMap* memphis_renderer_get_map (MemphisRenderer *renderer);
MemphisRuleSet* memphis_renderer_get_rule_set (MemphisRenderer *renderer);

void memphis_renderer_draw_png (MemphisRenderer *self,
    gchar *filename,
    guint zoom_level);

void memphis_renderer_draw_tile (MemphisRenderer *self,
    cairo_t *cr,
    guint x,
    guint y,
    guint zoom_level);

gint memphis_renderer_get_row_count (MemphisRenderer *renderer,
    guint zoom_level);
gint memphis_renderer_get_column_count (MemphisRenderer *renderer,
    guint zoom_level);
gint memphis_renderer_get_min_x_tile (MemphisRenderer *renderer,
    guint zoom_level);
gint memphis_renderer_get_max_x_tile (MemphisRenderer *renderer,
    guint zoom_level);
gint memphis_renderer_get_min_y_tile (MemphisRenderer *renderer,
    guint zoom_level);
gint memphis_renderer_get_max_y_tile (MemphisRenderer *renderer,
    guint zoom_level);
gboolean memphis_renderer_tile_has_data (MemphisRenderer *renderer,
    guint x,
    guint y,
    guint zoom_level);

G_END_DECLS

#endif /* __MEMPHIS_RENDERER_H__ */
