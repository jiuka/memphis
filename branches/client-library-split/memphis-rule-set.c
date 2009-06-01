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

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), MEMPHIS_TYPE_RULESET, MemphisRuleSetPrivate))

typedef struct _MemphisRuleSetPrivate MemphisRuleSetPrivate;

struct _MemphisRuleSetPrivate {
    int dummy;
};

static void
memphis_rule_set_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
memphis_rule_set_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
memphis_rule_set_dispose (GObject *object)
{
  MemphisRuleSet *self = MEMPHIS_RULESET (object);

  rulesetFree(self->ruleset);
  G_OBJECT_CLASS (memphis_rule_set_parent_class)->dispose (object);
}

static void
memphis_rule_set_class_init (MemphisRuleSetClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (MemphisRuleSetPrivate));

  object_class->get_property = memphis_rule_set_get_property;
  object_class->set_property = memphis_rule_set_set_property;
  object_class->dispose = memphis_rule_set_dispose;
}

static void
memphis_rule_set_init (MemphisRuleSet *self)
{
  self->ruleset = NULL;
}

MemphisRuleSet*
memphis_rule_set_new_from_file (gchar* filename)
{
  MemphisRuleSet* mruleset = g_object_new (MEMPHIS_TYPE_RULESET, NULL);
  mruleset->ruleset = (cfgRules *) rulesetRead (filename);
  return mruleset;
}

MemphisRuleSet*
memphis_rule_set_new_from_data (gchar* data)
{
  // TODO
  return g_object_new (MEMPHIS_TYPE_RULESET, NULL);
}

void
memphis_rule_set_free (MemphisRuleSet* rules)
{
  g_object_unref (G_OBJECT (rules));
}
