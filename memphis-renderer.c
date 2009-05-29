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

#include "memphis.h"
#include "mlib.h"

G_DEFINE_TYPE (MemphisRenderer, memphis_renderer, G_TYPE_OBJECT);

#define MEMPHIS_RENDERER_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), MEMPHIS_TYPE_RENDERER, MemphisRendererPrivate))

enum
{
  PROP_0,
  PROP_MAP,
  PROP_RULE_SET,
  PROP_RESOLUTION,
  PROP_ZOOM_LEVEL
};

struct _MemphisRendererPrivate
{
  GStringChunk *stringChunk;
  GTree *stringTree;

  MemphisMap *map;
  MemphisRuleSet *rules;
  guint resolution;
  guint zoom_level;
};

MemphisRenderer*
memphis_renderer_new ()
{
  return g_object_new (MEMPHIS_TYPE_RENDERER, NULL);
}

void
memphis_renderer_free (MemphisRenderer *renderer)
{
  g_object_unref (G_OBJECT (renderer));
}

void
memphis_renderer_draw (MemphisRenderer *renderer, 
    cairo_t *cr)
{
  g_return_if_fail (MEMPHIS_IS_RENDERER (renderer));
  
  //cfgRules *ruleset;
  //osmFile *osm;
  //stringChunk = g_string_chunk_new (265);
  //stringTree = g_tree_new (m_tree_strcmp);

  //ruleset = (cfgRules *) rulesetRead ("rule.xml");

  //osm = (osmFile *) osmRead("zh.osm");

  //g_tree_destroy(stringTree);

  //renderCairo(ruleset, osm);

  //osmFree(osm);
  //rulesetFree(ruleset);

  //g_string_chunk_free(stringChunk);*/
}

void
memphis_renderer_draw_tile (MemphisRenderer *renderer,
    cairo_t *cr,
    guint x,
    guint y)
{
  g_return_if_fail (MEMPHIS_IS_RENDERER (renderer));

  // TODO
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
  g_return_if_fail (MEMPHIS_IS_RENDERER (self));

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  priv->map = map;
}

MemphisMap*
memphis_renderer_get_map (MemphisRenderer *self)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), 0);

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  return priv->map;
}

void
memphis_renderer_set_rules_set (MemphisRenderer *self, MemphisRuleSet *rules)
{
  g_return_if_fail (MEMPHIS_IS_RENDERER (self));

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  priv->rules = rules;
}

MemphisRuleSet*
memphis_renderer_get_rule_set (MemphisRenderer *self)
{
  g_return_val_if_fail (MEMPHIS_IS_RENDERER (self), 0);

  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  return priv->rules;
}


static void
memphis_renderer_dispose (GObject *object)
{
  MemphisRenderer *self = MEMPHIS_RENDERER (object);
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);

  g_tree_destroy(priv->stringTree);
  g_string_chunk_free(priv->stringChunk);

  g_object_unref (G_OBJECT (priv->map));
  g_object_unref (G_OBJECT (priv->rules));

  G_OBJECT_CLASS (memphis_renderer_parent_class)->dispose (object);
}

/*
static void
memphis_renderer_finalize (GObject *object)
{
  G_OBJECT_CLASS (memphis_renderer_parent_class)->finalize (object);
}
*/

static void
memphis_renderer_get_property (GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
  MemphisRenderer *self = MEMPHIS_RENDERER (object);
  switch (property_id)
    {
      case PROP_RESOLUTION:
        g_value_set_uint (value, memphis_renderer_get_resolution (self));
        break;
      case PROP_ZOOM_LEVEL:
        g_value_set_uint (value, memphis_renderer_get_zoom_level (self));
        break;
      case PROP_MAP:
        g_value_set_object (value, memphis_renderer_get_map (self));
        break;
      case PROP_RULE_SET:
        g_value_set_object (value, memphis_renderer_get_rule_set (self));
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
  //object_class->finalize = memphis_renderer_finalize;

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
        0,
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
      PROP_ZOOM_LEVEL,
      g_param_spec_object ("rule-set",
        "A MemphisRuleSet",
        "Memphis rendering rules",
        MEMPHIS_TYPE_RULESET,
        G_PARAM_READWRITE));

}

static void
memphis_renderer_init (MemphisRenderer *self)
{
  MemphisRendererPrivate *priv = MEMPHIS_RENDERER_GET_PRIVATE (self);
  priv->stringChunk = g_string_chunk_new (265);
  priv->stringTree = g_tree_new (m_tree_strcmp);
  priv->resolution = 256;
  priv->zoom_level = 12;

  /* initialize all public and private members to reasonable default values. */

}

