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

#define MEMPHIS_TYPE_RENDERER                  (memphis_renderer_get_type ())
#define MEMPHIS_RENDERER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MEMPHIS_TYPE_RENDERER, MemphisRenderer))
#define MEMPHIS_IS_RENDERER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MEMPHIS_TYPE_RENDERER))
#define MEMPHIS_RENDERER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MEMPHIS_TYPE_RENDERER, MemphisRendererClass))
#define MEMPHIS_IS_RENDERER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MEMPHIS_TYPE_RENDERER))
#define MEMPHIS_RENDERER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MEMPHIS_TYPE_RENDERER, MemphisRendererClass))

typedef struct _MemphisRenderer        MemphisRenderer;
typedef struct _MemphisRendererClass   MemphisRendererClass;

struct _MemphisRenderer
{
  GObject parent_instance;
};

struct _MemphisRendererClass
{
  GObjectClass parent_class;
};

GType memphis_renderer_get_type (void);

MemphisRenderer* memphis_renderer_new ();
MemphisRenderer* memphis_renderer_new_full (MemphisRuleSet *rules,
    MemphisMap *map);
void memphis_renderer_free (MemphisRenderer *renderer);

void memphis_renderer_set_resolution (MemphisRenderer *renderer,
    guint resolution);
void memphis_renderer_set_debug_level (MemphisRenderer *renderer,
    gint8 debug_level);
void memphis_renderer_set_map (MemphisRenderer *renderer,
    MemphisMap* map);
void memphis_renderer_set_rules_set (MemphisRenderer *renderer,
    MemphisRuleSet* rules);

guint memphis_renderer_get_resolution (MemphisRenderer *renderer);
gint8 memphis_renderer_get_debug_level (MemphisRenderer *renderer);
MemphisMap* memphis_renderer_get_map (MemphisRenderer *renderer);
MemphisRuleSet* memphis_renderer_get_rule_set (MemphisRenderer *renderer);

void memphis_renderer_draw_png (MemphisRenderer *renderer,
    gchar *filename,
    guint zoom_level);

void memphis_renderer_draw_tile (MemphisRenderer *renderer,
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

#endif /* __MEMPHIS_RENDERER_H__ */
