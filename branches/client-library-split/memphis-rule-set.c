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

#include "memphis-rule-set.h"

G_DEFINE_TYPE (MemphisRuleSet, memphis_rule_set, G_TYPE_OBJECT)

#define MEMPHIS_RULESET_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), MEMPHIS_TYPE_RULESET, MemphisRuleSetPrivate))

enum
{
  PROP_0,
  PROP_DEBUG_LEVEL
};

typedef struct _MemphisRuleSetPrivate MemphisRuleSetPrivate;

struct _MemphisRuleSetPrivate {
  gint8 debug_level;
};

static void
memphis_rule_set_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  MemphisRuleSet *self = MEMPHIS_RULESET (object);
  MemphisRuleSetPrivate *priv = MEMPHIS_RULESET_GET_PRIVATE (self);
  switch (property_id)
  {
    case PROP_DEBUG_LEVEL:
      g_value_set_int (value, priv->debug_level);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
memphis_rule_set_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
  MemphisRuleSet *self = MEMPHIS_RULESET (object);
  switch (property_id)
  {
    case PROP_DEBUG_LEVEL:
        memphis_rule_set_set_debug_level (self, g_value_get_int (value));
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
memphis_rule_set_finalize (GObject *object)
{
  MemphisRuleSet *self = MEMPHIS_RULESET (object);

  if (self->ruleset != NULL)
    rulesetFree(self->ruleset);
  
  G_OBJECT_CLASS (memphis_rule_set_parent_class)->finalize (object);
}

static void
memphis_rule_set_class_init (MemphisRuleSetClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (MemphisRuleSetPrivate));

  object_class->get_property = memphis_rule_set_get_property;
  object_class->set_property = memphis_rule_set_set_property;
  object_class->finalize = memphis_rule_set_finalize;

  /**
  * MemphisRuleSet:debug-level:
  *
  * The debug level of the rule parser.
  *
  * Since: 0.1
  */
  g_object_class_install_property (object_class,
      PROP_DEBUG_LEVEL,
      g_param_spec_int ("debug-level",
        "Debug level",
        "The parser debug level",
        0,
        2,
        1,
        G_PARAM_READWRITE));
}

static void
memphis_rule_set_init (MemphisRuleSet *self)
{
  MemphisRuleSetPrivate *priv = MEMPHIS_RULESET_GET_PRIVATE (self);
  self->ruleset = NULL;
  priv->debug_level = 1;
}

MemphisRuleSet*
memphis_rule_set_new ()
{
  return g_object_new (MEMPHIS_TYPE_RULESET, NULL);
}

void
memphis_rule_set_load_from_file (MemphisRuleSet *rules, const gchar *filename)
{
  g_return_if_fail (MEMPHIS_IS_RULESET (rules) && filename != NULL);

  MemphisRuleSetPrivate *priv = MEMPHIS_RULESET_GET_PRIVATE (rules);
  if (rules->ruleset != NULL)
    rulesetFree (rules->ruleset);

  rules->ruleset = rulesetRead (filename, priv->debug_level);
}

void
memphis_rule_set_load_from_data (MemphisRuleSet *rules,
    const gchar *data,
    guint size)
{
  g_return_if_fail (MEMPHIS_IS_RULESET (rules) && data != NULL);
  
  if (rules->ruleset != NULL)
    rulesetFree (rules->ruleset);
  
  // TODO
  rules->ruleset = NULL;
}

void
memphis_rule_set_free (MemphisRuleSet *rules)
{
  g_return_if_fail (MEMPHIS_IS_RULESET (rules));

  g_object_unref (G_OBJECT (rules));
}

void
memphis_rule_set_set_debug_level (MemphisRuleSet *rules,
    gint8 debug_level)
{
  g_return_if_fail (MEMPHIS_IS_RULESET (rules));

  MemphisRuleSetPrivate *priv = MEMPHIS_RULESET_GET_PRIVATE (rules);
  priv->debug_level = debug_level;
}

gint8
memphis_rule_set_get_debug_level (MemphisRuleSet *rules)
{
  g_return_val_if_fail (MEMPHIS_IS_RULESET (rules), -1);

  MemphisRuleSetPrivate *priv = MEMPHIS_RULESET_GET_PRIVATE (rules);
  return priv->debug_level;
}
