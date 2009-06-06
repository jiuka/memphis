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

/* inclusion guard */
#ifndef __MEMPHIS_RENDERER_H__
#define __MEMPHIS_RENDERER_H__

#include <glib-object.h>
#include <cairo.h>
#include <stdio.h>
#include "memphis.h"

/*
 * Type macros.
 */
#define MEMPHIS_TYPE_RENDERER                  (memphis_renderer_get_type ())
#define MEMPHIS_RENDERER(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MEMPHIS_TYPE_RENDERER, MemphisRenderer))
#define MEMPHIS_IS_RENDERER(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MEMPHIS_TYPE_RENDERER))
#define MEMPHIS_RENDERER_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), MEMPHIS_TYPE_RENDERER, MemphisRendererClass))
#define MEMPHIS_IS_RENDERER_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), MEMPHIS_TYPE_RENDERER))
#define MEMPHIS_RENDERER_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), MEMPHIS_TYPE_RENDERER, MemphisRendererClass))

typedef struct _MemphisRenderer        MemphisRenderer;
typedef struct _MemphisRendererClass   MemphisRendererClass;
typedef struct _MemphisRendererPrivate MemphisRendererPrivate;

struct _MemphisRenderer
{
  GObject parent_instance;

  /* instance members */
};

struct _MemphisRendererClass
{
  GObjectClass parent_class;

  /* class members */
};

GType memphis_renderer_get_type (void);

/*
 * Method definitions.
 */
MemphisRenderer* memphis_renderer_new ();
void memphis_renderer_free (MemphisRenderer *renderer);

void memphis_renderer_set_resolution (MemphisRenderer *renderer,
    guint resolution);
void memphis_renderer_set_zoom_level (MemphisRenderer *renderer,
    guint zoom_level);
void memphis_renderer_set_map (MemphisRenderer *renderer,
    MemphisMap* map);
void memphis_renderer_set_rules_set (MemphisRenderer *renderer,
    MemphisRuleSet* rules);

guint memphis_renderer_get_resolution (MemphisRenderer *renderer);
guint memphis_renderer_get_zoom_level (MemphisRenderer *renderer);
MemphisMap* memphis_renderer_get_map (MemphisRenderer *renderer);
MemphisRuleSet* memphis_renderer_get_rule_set (MemphisRenderer *renderer);

void memphis_renderer_draw_png (MemphisRenderer *renderer,
    gchar *filename);

void memphis_renderer_draw_tile (MemphisRenderer *renderer,
    cairo_t *cr,
    guint x,
    guint y);

#endif /* __MEMPHIS_RENDERER_H__ */
