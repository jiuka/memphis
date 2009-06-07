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

#include "memphis-renderer.h"
#include "renderer.h"
#include <math.h>

G_DEFINE_TYPE (MemphisRenderer, memphis_renderer, G_TYPE_OBJECT);

#define MEMPHIS_RENDERER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), MEMPHIS_TYPE_RENDERER, MemphisRendererPrivate))

enum
{
  PROP_0,
  PROP_MAP,
  PROP_RULE_SET,
  PROP_RESOLUTION,
  PROP_ZOOM_LEVEL,
  PROP_DEBUG_LEVEL
};

struct _MemphisRendererPrivate
{
  MemphisMap *map;
  MemphisRuleSet *rules;
  guint resolution;
  guint zoom_level;
  gint8 debug_level;
};

MemphisRenderer*
memphis_renderer_new ()
{
  return g_object_new (MEMPHIS_TYPE_RENDERER, NULL);
}

MemphisRenderer*
memphis_renderer_new_full (MemphisRuleSet *rules, MemphisMap *map)
{
  MemphisRenderer* r = g_object_new (MEMPHIS_TYPE_RENDERER, NULL);
  memphis_renderer_set_map (r, map);
  memphis_renderer_set_rules_set (r, rules);
  return r;
}

void
memphis_renderer_free (MemphisRenderer *renderer)
{
  g_object_unref (G_OBJECT (renderer));
}

/* does not obey resolution settings!
 * creates a png of the whole data of unpredictable size.
 * probably not a very useful function for a generic library. */
void
memphis_renderer_draw_png (MemphisRenderer *renderer, 
    gchar *filename)
{
  g_return_if_fail (MEMPHIS_IS_RENDERER (renderer));

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (renderer);
  renderInfo *info;
  osmFile *osm;
  cfgRules *ruleset;
  coordinates min, max;

  g_return_if_fail (MEMPHIS_IS_RULESET (priv->rules)
      && MEMPHIS_IS_MAP (priv->map));

  if (priv->debug_level > 1)
    fprintf (stdout, "renderCairo\n");

  info = g_new (renderInfo, 1);
  info->zoom = priv->zoom_level;
  info->ruleset = ruleset = priv->rules->ruleset;
  info->osm = osm = priv->map->map;

  info->offset = coord2xy (osm->maxlat, osm->minlon, info->zoom);

  min = coord2xy (osm->minlat, osm->minlon, info->zoom);
  max = coord2xy (osm->maxlat, osm->maxlon, info->zoom);
  int w = (int) ceil (max.x - min.x);
  int h = (int) ceil (min.y - max.y);

  info->surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, w, h);
  info->cr = cairo_create (info->surface);

  cairo_rectangle (info->cr, 0, 0, w, h);
  cairo_set_source_rgb (info->cr,
      (double)ruleset->background[0] / 255.0,
      (double)ruleset->background[1] / 255.0,
      (double)ruleset->background[2] / 255.0);
  cairo_fill (info->cr);

  renderCairoRun (info, priv->debug_level);

  if (priv->debug_level > 0) {
    fprintf (stdout, " Cairo rendering Z%i to '%s'", info->zoom, filename);
    fflush (stdout);
  }
  cairo_surface_write_to_png (info->surface, filename);
  cairo_destroy (info->cr);
  cairo_surface_destroy (info->surface);

  if (priv->debug_level > 0)
    fprintf (stdout, " done.\n");

  g_free (info);
}

void
memphis_renderer_draw_tile (MemphisRenderer *renderer,
    cairo_t *cr,
    guint x,
    guint y)
{
  g_return_if_fail (MEMPHIS_IS_RENDERER (renderer));

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (renderer);
  renderInfo *info;
  osmFile *osm;
  cfgRules *ruleset;
  coordinates crd;

  g_return_if_fail (MEMPHIS_IS_RULESET (priv->rules)
      && MEMPHIS_IS_MAP (priv->map));

  info = g_new (renderInfo, 1);
  info->zoom = priv->zoom_level;
  info->ruleset = ruleset = priv->rules->ruleset;
  info->osm = osm = priv->map->map;
  info->surface = NULL;
  info->cr = cr;

  crd = tile2latlon (x, y, info->zoom);
  
  if (priv->debug_level > 0)
    fprintf (stdout, " Cairo rendering tile: (%i, %i)\n", x, y);
  
  info->offset = coord2xy (crd.x, crd.y, info->zoom);

  cairo_rectangle (info->cr, 0, 0, priv->resolution, priv->resolution);
  cairo_set_source_rgb (info->cr,
      (double) ruleset->background[0] / 255.0,
      (double) ruleset->background[1] / 255.0,
      (double) ruleset->background[2] / 255.0);
  cairo_fill (info->cr);

  // TODO: don't draw if it is empty anyway?
  renderCairoRun (info, priv->debug_level);
  
  g_free (info);
}

void
memphis_renderer_set_resolution (MemphisRenderer *self, guint resolution)
{
  g_return_if_fail (MEMPHIS_IS_RENDERER (self));

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  priv->resolution = resolution;
}

guint
memphis_renderer_get_resolution (MemphisRenderer *self)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), 0);

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  return priv->resolution;
}

void
memphis_renderer_set_zoom_level (MemphisRenderer *self, guint zoom_level)
{
  g_return_if_fail (MEMPHIS_IS_RENDERER (self));

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  priv->zoom_level = zoom_level;
}

guint
memphis_renderer_get_zoom_level (MemphisRenderer *self)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), 0);

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  return priv->zoom_level;
}

void
memphis_renderer_set_map (MemphisRenderer *self, MemphisMap *map)
{
  g_return_if_fail (MEMPHIS_IS_RENDERER (self) && MEMPHIS_IS_MAP (map));

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  priv->map = g_object_ref (map);
}

MemphisMap*
memphis_renderer_get_map (MemphisRenderer *self)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), 0);

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  return priv->map;
}

void
memphis_renderer_set_rules_set (MemphisRenderer *self,
    MemphisRuleSet *rules)
{
  g_return_if_fail (MEMPHIS_IS_RENDERER (self) &&
      MEMPHIS_IS_RULESET (rules));

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  priv->rules = g_object_ref (rules);
}

MemphisRuleSet*
memphis_renderer_get_rule_set (MemphisRenderer *self)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), 0);

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  return priv->rules;
}

void
memphis_renderer_set_debug_level (MemphisRenderer *self, gint8 debug_level)
{
  g_return_if_fail (MEMPHIS_IS_RENDERER (self));

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  priv->debug_level = debug_level;
  if (priv->map != NULL)
    priv->map->debug_level = debug_level;
  if (priv->rules != NULL)
    priv->rules->debug_level = debug_level;
}

gint8
memphis_renderer_get_debug_level (MemphisRenderer *self)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), 0);

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  return priv->debug_level;
}

static void
memphis_renderer_dispose (GObject *object)
{
  MemphisRenderer *self = MEMPHIS_RENDERER (object);
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);

  g_object_unref (G_OBJECT (priv->map));
  g_object_unref (G_OBJECT (priv->rules));

  G_OBJECT_CLASS (memphis_renderer_parent_class)->dispose (object);
}

static void
memphis_renderer_finalize (GObject *object)
{
  G_OBJECT_CLASS (memphis_renderer_parent_class)->finalize (object);
}

static void
memphis_renderer_get_property (GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
  MemphisRenderer *self = MEMPHIS_RENDERER (object);
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  switch (property_id)
    {
      case PROP_RESOLUTION:
        g_value_set_uint (value, priv->resolution);
        break;
      case PROP_ZOOM_LEVEL:
        g_value_set_uint (value, priv->zoom_level);
        break;
      case PROP_MAP:
        g_value_set_object (value, priv->map);
        break;
      case PROP_RULE_SET:
        g_value_set_object (value, priv->rules);
        break;
      case PROP_DEBUG_LEVEL:
        g_value_set_int (value, priv->debug_level);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
memphis_renderer_set_property (GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec)
{
  MemphisRenderer *self = MEMPHIS_RENDERER (object);
  switch (property_id)
    {
      case PROP_RESOLUTION:
        memphis_renderer_set_resolution (self, g_value_get_uint (value));
        break;
      case PROP_ZOOM_LEVEL:
        memphis_renderer_set_zoom_level (self, g_value_get_uint (value));
        break;
      case PROP_MAP:
        memphis_renderer_set_map (self, g_value_get_object (value));
        break;
      case PROP_RULE_SET:
        memphis_renderer_set_rules_set (self, g_value_get_object (value));
        break;
      case PROP_DEBUG_LEVEL:
        memphis_renderer_set_debug_level (self, g_value_get_int (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
memphis_renderer_class_init (MemphisRendererClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (MemphisRendererPrivate));

  object_class->get_property = memphis_renderer_get_property;
  object_class->set_property = memphis_renderer_set_property;
  object_class->dispose = memphis_renderer_dispose;
  object_class->finalize = memphis_renderer_finalize;

  /**
  * MemphisRenderer:resolution:
  *
  * The tile resolution in pixel.
  *
  * Since: 0.1
  */
  g_object_class_install_property (object_class,
      PROP_RESOLUTION,
      g_param_spec_uint ("resolution",
        "Tile resolution",
        "The tile resolution in pixel",
        8,
        2048,
        256,
        G_PARAM_READWRITE));

  /**
  * MemphisRenderer:zoom-level:
  *
  * The zoom level.
  *
  * Since: 0.1
  */
  g_object_class_install_property (object_class,
      PROP_ZOOM_LEVEL,
      g_param_spec_uint ("zoom-level",
        "Map zoom level",
        "The zoom level",
        12,
        17,
        12,
        G_PARAM_READWRITE));

  /**
  * MemphisRenderer:map:
  *
  * A MemphisMap.
  *
  * Since: 0.1
  */
  g_object_class_install_property (object_class,
      PROP_MAP,
      g_param_spec_object ("map",
        "A MemphisMap",
        "OSM map data",
        MEMPHIS_TYPE_MAP,
        G_PARAM_READWRITE));

  /**
  * MemphisRenderer:rule-set:
  *
  * A MemphisRuleSet.
  *
  * Since: 0.1
  */
  g_object_class_install_property (object_class,
      PROP_RULE_SET,
      g_param_spec_object ("rule-set",
        "A MemphisRuleSet",
        "Memphis rendering rules",
        MEMPHIS_TYPE_RULESET,
        G_PARAM_READWRITE));

  /**
  * MemphisRenderer:rule-set:
  *
  * A MemphisRuleSet.
  *
  * Since: 0.1
  */
  g_object_class_install_property (object_class,
      PROP_DEBUG_LEVEL,
      g_param_spec_int ("debug-level",
        "Debug level",
        "The renderer debug level",
        0,
        2,
        1,
        G_PARAM_READWRITE));

}

static void
memphis_renderer_init (MemphisRenderer *self)
{
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  priv->map = NULL;
  priv->rules = NULL;
  priv->resolution = 256;
  priv->zoom_level = 12;
  priv->debug_level = 1;
  
}

gint
memphis_renderer_get_row_count (MemphisRenderer *self)
{
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  return (1 << priv->zoom_level);
}

gint
memphis_renderer_get_column_count (MemphisRenderer *self)
{
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  return (1 << priv->zoom_level);
}

gint
memphis_renderer_get_min_x_tile (MemphisRenderer *self)
{
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  
  return lon2tilex (priv->map->map->minlon, priv->zoom_level);
}

gint
memphis_renderer_get_max_x_tile (MemphisRenderer *self)
{
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  
  return lon2tilex (priv->map->map->maxlon, priv->zoom_level);
}

gint
memphis_renderer_get_min_y_tile (MemphisRenderer *self)
{
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  
  return lat2tiley (priv->map->map->maxlat, priv->zoom_level);
}

gint
memphis_renderer_get_max_y_tile (MemphisRenderer *self)
{
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  
  return lat2tiley (priv->map->map->minlat, priv->zoom_level);
}

gboolean
memphis_renderer_tile_has_data (MemphisRenderer *self, gint x, gint y)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), FALSE);

  gint minx, miny, maxx, maxy;
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);

  minx = lon2tilex (priv->map->map->minlon, priv->zoom_level);
  miny = lat2tiley (priv->map->map->minlat, priv->zoom_level);
  maxx = lon2tilex (priv->map->map->maxlon, priv->zoom_level);
  maxy = lat2tiley (priv->map->map->maxlat, priv->zoom_level);

  if (x < minx || x > maxx || y < miny || y > maxy)
    return FALSE;
  
  return TRUE;
}
