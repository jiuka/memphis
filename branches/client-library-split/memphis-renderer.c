/*
 * Memphis - Cairo Rederer for OSM in C
 * Copyright (C) 2008  Marius Rieder <marius.rieder@durchmesser.ch>
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

#include <math.h>
#include <string.h>
#include <glib/gstdio.h>

#include "memphis-renderer.h"
#include "list.h"
#include "libmercator.h"
#include "ruleset.h"
#include "osm05.h"
#include "textpath.h"

G_DEFINE_TYPE (MemphisRenderer, memphis_renderer, G_TYPE_OBJECT)

#define MEMPHIS_RENDERER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), MEMPHIS_TYPE_RENDERER, MemphisRendererPrivate))

#define MEMPHIS_RENDERER_MIN_ZOOM_LEVEL 12
#define MEMPHIS_RENDERER_MAX_ZOOM_LEVEL 18

enum
{
  PROP_0,
  PROP_MAP,
  PROP_RULE_SET,
  PROP_RESOLUTION,
  PROP_DEBUG_LEVEL
};

typedef struct _MemphisRendererPrivate MemphisRendererPrivate;
struct _MemphisRendererPrivate
{
  MemphisMap *map;
  MemphisRuleSet *rules;
  guint resolution;
  gint8 debug_level;
};

/*
 * Internal used to save data of a renderer run
 */
typedef struct renderInfo_ renderInfo;
struct renderInfo_ {
  coordinates     offset;
  guint           zoom_level;
  cairo_t         *cr;
  MemphisRendererPrivate *priv;
};

/*
 * Internal used return values for stringInStrings.
 */
typedef enum compare_result_e {
  TAG_CMP_NOT_EQUAL   = 0,
  TAG_CMP_EQUAL       = 1,
  TAG_CMP_ANY         = 2,
  TAG_CMP_MISSING     = 3,
} compare_result_e;

static int renderCairo (renderInfo *info);

MemphisRenderer*
memphis_renderer_new ()
{
  return g_object_new (MEMPHIS_TYPE_RENDERER, NULL);
}

MemphisRenderer*
memphis_renderer_new_full (MemphisRuleSet *rules, MemphisMap *map)
{
  MemphisRenderer* r = g_object_new (MEMPHIS_TYPE_RENDERER, NULL);
  if (map)
    memphis_renderer_set_map (r, map);
  if (rules)
    memphis_renderer_set_rules_set (r, rules);
  return r;
}

void
memphis_renderer_free (MemphisRenderer *renderer)
{
  g_return_if_fail (MEMPHIS_IS_RENDERER (renderer));
  
  g_object_unref (G_OBJECT (renderer));
}

/* does not obey resolution settings!
 * creates a png of the whole data of unpredictable size.
 * probably not a very useful function for a generic library.
 * Should be removed! */
void
memphis_renderer_draw_png (MemphisRenderer *renderer, 
    gchar *filename, guint zoom_level)
{
  g_return_if_fail (MEMPHIS_IS_RENDERER (renderer));

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (renderer);
  renderInfo *info;
  osmFile *osm;
  cfgRules *ruleset;
  coordinates min, max;
  cairo_surface_t *surface;

  g_return_if_fail (MEMPHIS_IS_RULESET (priv->rules)
      && MEMPHIS_IS_MAP (priv->map));

  if (!(priv->rules->ruleset && priv->map->map)) {
    if (priv->debug_level > 0)
      g_fprintf (stdout, " No map and/or rules data: Draw nothing\n");
    return;
  }

  if (priv->debug_level > 1)
    g_fprintf (stdout, "renderCairo\n");

  zoom_level = CLAMP (zoom_level, MEMPHIS_RENDERER_MIN_ZOOM_LEVEL,
      MEMPHIS_RENDERER_MAX_ZOOM_LEVEL);

  ruleset = priv->rules->ruleset;
  osm = priv->map->map;
  min = coord2xy (osm->minlat, osm->minlon, zoom_level, priv->resolution);
  max = coord2xy (osm->maxlat, osm->maxlon, zoom_level, priv->resolution);
  int w = (int) ceil (max.x - min.x);
  int h = (int) ceil (min.y - max.y);

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, w, h);

  info = g_new (renderInfo, 1);
  info->offset = coord2xy (osm->maxlat, osm->minlon, zoom_level, priv->resolution);
  info->zoom_level = zoom_level;
  info->cr = cairo_create (surface);
  info->priv = priv;

  cairo_rectangle (info->cr, 0, 0, w, h);
  cairo_set_source_rgb (info->cr,
      (double)ruleset->background[0] / 255.0,
      (double)ruleset->background[1] / 255.0,
      (double)ruleset->background[2] / 255.0);
  cairo_fill (info->cr);

  renderCairo (info);

  if (priv->debug_level > 0) {
    g_fprintf (stdout, " Cairo rendering Z%i to '%s'", info->zoom_level, filename);
    fflush (stdout);
  }
  cairo_surface_write_to_png (surface, filename);
  cairo_destroy (info->cr);
  cairo_surface_destroy (surface);

  g_free (info);

  if (priv->debug_level > 0)
    g_fprintf (stdout, " done.\n");
}

void
memphis_renderer_draw_tile (MemphisRenderer *renderer,
    cairo_t *cr,
    guint x,
    guint y,
    guint zoom_level)
{
  g_return_if_fail (MEMPHIS_IS_RENDERER (renderer));

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (renderer);
  renderInfo *info;
  osmFile *osm;
  cfgRules *ruleset;
  coordinates crd;

  g_return_if_fail (MEMPHIS_IS_RULESET (priv->rules)
      && MEMPHIS_IS_MAP (priv->map));

  if (!(priv->rules->ruleset && priv->map->map)) {
    if (priv->debug_level > 0)
      g_fprintf (stdout, " No map and/or rules data: Draw nothing\n");
    return;
  }

  ruleset = priv->rules->ruleset;
  osm = priv->map->map;
  
  info = g_new (renderInfo, 1);
  info->cr = cr;
  info->zoom_level = CLAMP (zoom_level, MEMPHIS_RENDERER_MIN_ZOOM_LEVEL,
      MEMPHIS_RENDERER_MAX_ZOOM_LEVEL);
  info->priv = priv;

  crd = tile2latlon (x, y, info->zoom_level);
  info->offset = coord2xy (crd.x, crd.y, info->zoom_level, priv->resolution);

  if (priv->debug_level > 0)
    g_fprintf (stdout, " Cairo rendering tile: (%i, %i, %i)\n", x, y, info->zoom_level);

  cairo_rectangle (info->cr, 0, 0, priv->resolution, priv->resolution);
  cairo_set_source_rgb (info->cr,
      (double) ruleset->background[0] / 255.0,
      (double) ruleset->background[1] / 255.0,
      (double) ruleset->background[2] / 255.0);
  cairo_fill (info->cr);

  // TODO: Is this a good cut-off to draw background tiles only?
  if (x < memphis_renderer_get_max_x_tile (renderer, info->zoom_level) + 2 &&
      x > memphis_renderer_get_min_x_tile (renderer, info->zoom_level) - 2 &&
      y < memphis_renderer_get_max_y_tile (renderer, info->zoom_level) + 2 &&
      y > memphis_renderer_get_min_y_tile (renderer, info->zoom_level) - 2)
  {
    renderCairo (info);
  }

  g_free (info);

  if (priv->debug_level > 0)
    g_fprintf (stdout, " done.\n");
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
memphis_renderer_set_map (MemphisRenderer *self, MemphisMap *map)
{
  g_return_if_fail (MEMPHIS_IS_RENDERER (self) && MEMPHIS_IS_MAP (map));

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  if (priv->map)
    g_object_unref (priv->map);

  priv->map = g_object_ref (map);
}

MemphisMap*
memphis_renderer_get_map (MemphisRenderer *self)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), NULL);

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
  if (priv->rules)
    g_object_unref (priv->rules);

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
  if (priv->map)
    memphis_map_set_debug_level (priv->map, debug_level);
  if (priv->rules)
    memphis_rule_set_set_debug_level (priv->rules, debug_level);
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
  * MemphisRenderer:debug-level:
  *
  * The debug level of the renderer.
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
  priv->debug_level = 1;
}

gint
memphis_renderer_get_row_count (MemphisRenderer *self, guint zoom_level)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), -1);

  return (1 << zoom_level);
}

gint
memphis_renderer_get_column_count (MemphisRenderer *self, guint zoom_level)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), -1);

  return (1 << zoom_level);
}

gint
memphis_renderer_get_min_x_tile (MemphisRenderer *self, guint zoom_level)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), -1);

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  g_return_val_if_fail (MEMPHIS_IS_MAP (priv->map), -1);
  g_return_val_if_fail (priv->map->map, -1);
  return lon2tilex (priv->map->map->minlon, zoom_level);
}

gint
memphis_renderer_get_max_x_tile (MemphisRenderer *self, guint zoom_level)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), -1);
  
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  g_return_val_if_fail (MEMPHIS_IS_MAP (priv->map), -1);
  g_return_val_if_fail (priv->map->map, -1);
  return lon2tilex (priv->map->map->maxlon, zoom_level);
}

gint
memphis_renderer_get_min_y_tile (MemphisRenderer *self, guint zoom_level)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), -1);
  
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  g_return_val_if_fail (MEMPHIS_IS_MAP (priv->map), -1);
  g_return_val_if_fail (priv->map->map, -1);
  return lat2tiley (priv->map->map->maxlat, zoom_level);
}

gint
memphis_renderer_get_max_y_tile (MemphisRenderer *self, guint zoom_level)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), -1);
  
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  g_return_val_if_fail (MEMPHIS_IS_MAP (priv->map), -1);
  g_return_val_if_fail (priv->map->map, -1);
  return lat2tiley (priv->map->map->minlat, zoom_level);
}

gboolean
memphis_renderer_tile_has_data (MemphisRenderer *self, guint x, guint y,
    guint zoom_level)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), FALSE);

  gint minx, miny, maxx, maxy;
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  g_return_val_if_fail (MEMPHIS_IS_MAP (priv->map), -1);
  g_return_val_if_fail (priv->map->map, -1);

  minx = lon2tilex (priv->map->map->minlon, zoom_level);
  miny = lat2tiley (priv->map->map->minlat, zoom_level);
  maxx = lon2tilex (priv->map->map->maxlon, zoom_level);
  maxy = lat2tiley (priv->map->map->maxlat, zoom_level);

  if (x < minx || x > maxx || y < miny || y > maxy)
    return FALSE;
  
  return TRUE;
}

/*
 * function: drawPath
 * @cr  Array of cairo resources
 * @nd  Liked list if osmNd's
 *
 * This function is used to prepare a Path.
 */
static void drawPath (renderInfo *info, GSList *nodes) {
  GSList *iter;
  osmNode *nd;
  coordinates xy;
  MemphisRendererPrivate *p = info->priv;

  if (G_UNLIKELY (p->debug_level > 1))
    g_fprintf (stdout, "drawPath\n");

  iter = nodes;
  nd = iter->data;
  xy = coord2xy (nd->lat, nd->lon, info->zoom_level, p->resolution);
  cairo_move_to (info->cr, xy.x - info->offset.x,
                           xy.y - info->offset.y);

  iter = g_slist_next(iter);
  while (iter) {
    nd = iter->data;
    xy = coord2xy (nd->lat, nd->lon, info->zoom_level, p->resolution);
    cairo_line_to (info->cr, xy.x - info->offset.x,
                             xy.y - info->offset.y);
    iter = g_slist_next (iter);
  }
}

/*
 * function: strokePath
 * @cr  Array of cairo resources
 *
 * This function is stroke all current path without drawing anithing.
 */
static void strokePath (renderInfo *info) {
  if (G_UNLIKELY (info->priv->debug_level > 1))
    g_fprintf (stdout,"strokePath\n");

  cairo_set_line_width (info->cr, 0);
  cairo_stroke (info->cr);
}

/*
 * function: drawPolygone
 *
 * This function fill the prepared paths with the configured color.
 */
static void drawPolygone (renderInfo *info, cfgDraw *draw) {
  if (G_UNLIKELY (info->priv->debug_level > 1))
    g_fprintf (stdout, "drawPolygone\n");

  cairo_surface_t *image = NULL;
  cairo_pattern_t *pattern = NULL;

  if (draw->pattern) {
    char *filename;

    /* TODO ast: the pattern may be cached, e.g. using a GCache structure */

    filename = g_strdup_printf ("pattern/%s.png", draw->pattern);
    image = cairo_image_surface_create_from_png (filename);
    if (cairo_surface_status (image) != CAIRO_STATUS_SUCCESS) {
      g_warning ("pattern '%s' not found\n", filename);
      g_free (filename);
      return;
    }
    g_free (filename);

    pattern = cairo_pattern_create_for_surface (image);
    cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
    cairo_surface_destroy (image);
  }

  cairo_set_fill_rule (info->cr, CAIRO_FILL_RULE_EVEN_ODD);

  if (pattern)
    cairo_set_source (info->cr, pattern);
  else
    cairo_set_source_rgb (info->cr, (double)draw->color[0]/255.0,
                                    (double)draw->color[1]/255.0,
                                    (double)draw->color[2]/255.0);

  cairo_fill_preserve (info->cr);

  if (pattern)
    cairo_pattern_destroy (pattern);
}

/*
 * function: drawLine
 *
 * This function draw the prepared paths with the configured color.
 */
static void drawLine (renderInfo *info, cfgDraw *draw) {
  if (G_UNLIKELY (info->priv->debug_level > 1))
    g_fprintf (stdout, "drawLine\n");

  cairo_set_line_cap (info->cr, CAIRO_LINE_CAP_ROUND);
  cairo_set_line_join (info->cr, CAIRO_LINE_JOIN_ROUND);
  cairo_set_line_width (info->cr, draw->width * LINESIZE (info->zoom_level));

  cairo_set_source_rgb (info->cr, (double)draw->color[0]/255.0,
                                  (double)draw->color[1]/255.0,
                                  (double)draw->color[2]/255.0);
  cairo_stroke_preserve(info->cr);
}

/*
 * function: drawText
 *
 * This function draw the given text along the current path.
 */
static void drawText (renderInfo *info, char *text, cfgDraw *draw) {
  if (G_UNLIKELY (info->priv->debug_level > 1))
    g_fprintf(stdout, "drawText\n");

  cairo_select_font_face (info->cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                                            CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_source_rgb (info->cr, (double)draw->color[0]/255.0,
                                  (double)draw->color[1]/255.0,
                                  (double)draw->color[2]/255.0);
  cairo_set_font_size (info->cr, draw->width * LINESIZE (info->zoom_level));
  textPath (info->cr, text);
}

/*
 * function: stringInStrings
 *
 * Check if string is an strings.
 */
static compare_result_e stringInStrings(char *string, char **strings,
      gint8 debug_level) {
  if (G_UNLIKELY (debug_level > 1))
    g_fprintf(stdout, "stringInStrings\n");
  compare_result_e r = TAG_CMP_NOT_EQUAL;
  while (*strings != NULL) {
    if (string == *strings) {
      return TAG_CMP_EQUAL;
    }
    if(strcmp(*strings, "*") == 0)
      r = TAG_CMP_ANY;
    if(strcmp(*strings, "~") == 0)
      r = TAG_CMP_MISSING;

    strings++;
  }
  return r;
}

/*
 * function: matchRule
 *
 * Check if a element matchs a rule.
 */
static int matchRule (cfgRule *rule, osmTag *tag, gint8 debug_level) {
  int k, v;

  if (G_UNLIKELY (debug_level > 1))
    g_fprintf(stdout, "matchRule\n");

  while(tag) {
      k = stringInStrings (tag->key, rule->key, debug_level);
      v = stringInStrings (tag->value, rule->value, debug_level);

      if (k == TAG_CMP_EQUAL && v == TAG_CMP_EQUAL)
        return TRUE;
      if (k == TAG_CMP_EQUAL && v == TAG_CMP_ANY)
      return TRUE;
      if (k == TAG_CMP_NOT_EQUAL && v == TAG_CMP_MISSING)
        return TRUE;

      tag = tag->next;
  }
  return FALSE;
}

/*
 * function: checkRule
 *
 * Check if a element match to a rule and all it's parent.
 */
static int checkRule (cfgRule *rule, osmTag *tag, short int type,
    gint8 debug_level) {
  if (G_UNLIKELY (debug_level > 1))
    g_fprintf(stdout, "checkRule\n");

  int not;
  cfgRule *iter;

  if (rule->nparent) {
    iter = rule->nparent;
    not = TRUE;
  } else {
    iter = rule->parent;
    not = FALSE;
  }

  while (iter) {

      if (matchRule(iter, tag, debug_level) == not) {
        return 0;
      }

      if (iter->nparent) {
        iter = iter->nparent;
        not = TRUE;
      } else {
        iter = iter->parent;
        not = FALSE;
      }
  }

  if(matchRule(rule, tag, debug_level)) {
    return 1;
  } else {
    return -1;
  }
}

static void renderPaths (renderInfo *info, int layer,
        cfgRule *rule, cfgDraw *draw) {
  int paths = 0;
  osmWay *way;
  MemphisRendererPrivate *p = info->priv;
  osmFile *osm = p->map->map;

  // Loop through ways for
  LIST_FOREACH(way, osm->ways) {
    //Only objects on current layer
    if (way->layer != layer)
      continue;

    if ( checkRule(rule, way->tag, WAY, p->debug_level) == 1) {
      drawPath(info, way->nd);
      paths++;
    }
  }
  if (paths) {
      while(draw) {
          if (draw->minzoom > info->zoom_level || draw->maxzoom < info->zoom_level) {
              draw = draw->next;
              continue;
          }
          switch(draw->type) {
              case POLYGONE:
                  drawPolygone(info, draw);
                  break;
              case LINE:
                  drawLine(info, draw);
                  break;
              case TEXT: break;   /* ignore */
          }
          draw = draw->next;
      }
  }
  strokePath (info);
}

static void renderText (renderInfo *info, int layer,
    cfgRule *rule, cfgDraw *draw) {
  osmWay *way;
  MemphisRendererPrivate *p = info->priv;
  osmFile *osm = p->map->map;

  while (draw) {
      if (draw->type == TEXT) {
          if (draw->minzoom <= info->zoom_level && info->zoom_level <= draw->maxzoom) {
              LIST_FOREACH(way, osm->ways) {
                  //Only objects on current layer
                  if (way->layer != layer || way->name == NULL)
                      continue;

                  if (checkRule(rule, way->tag, WAY, p->debug_level) == 1) {
                      drawPath(info, way->nd);
                      drawText(info, way->name, draw);
                  }
              }

          }
          break;
      }
      draw = draw->next;
  }
  strokePath (info);
}

/*
 * function: renderCairo
 */
static int renderCairo (renderInfo *info) {
  int layer;
  MemphisRendererPrivate *p = info->priv;
  cfgRules *ruleset = p->rules->ruleset;

  if (p->debug_level > 1)
    g_fprintf (stdout, "renderCairoRun\n");

  // Vars used while looping through data
  cfgRule     *rule;

  // Start checking osm from bottom layer.
  for (layer = -5; layer <= 5; layer++) {

      if (p->debug_level > 0) {
          g_fprintf(stdout,"\r Cairo drawing z%i Layer % 2i", info->zoom_level, layer);
          fflush(stdout);
      }

      // Process Rule by Rule
      LIST_FOREACH(rule, ruleset->rule) {

          if(rule->draw != NULL) { // Draw Match first
              renderPaths(info, layer, rule, rule->draw);

              // Text Rendering
              renderText(info, layer, rule, rule->draw);
          }
          if (rule->ndraw != NULL) { // Draw Else after
              renderPaths(info, layer, rule, rule->ndraw);
          }
      }
  }

  if (p->debug_level > 0)
      g_fprintf (stdout, "\r Cairo drawing done\n");

  return 0;
}
