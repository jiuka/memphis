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

#include "memphis-rule.h"

G_DEFINE_TYPE (MemphisRule, memphis_rule, G_TYPE_OBJECT)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), MEMPHIS_TYPE_RULE, MemphisRulePrivate))

typedef struct _MemphisRulePrivate MemphisRulePrivate;

/*
struct _MemphisRulePrivate {
    int dummy;
};
*/

static void
memphis_rule_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
memphis_rule_set_property (GObject *object, guint property_id,
                              const GValue *value, GParamSpec *pspec)
{
  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
memphis_rule_dispose (GObject *object)
{
  G_OBJECT_CLASS (memphis_rule_parent_class)->dispose (object);
}

static void
memphis_rule_finalize (GObject *object)
{
  MemphisRule *self = MEMPHIS_RULE (object);

  if (self->keys != NULL)
    g_strfreev (self->keys);
  if (self->values != NULL)
    g_strfreev (self->values);
  G_OBJECT_CLASS (memphis_rule_parent_class)->finalize (object);
}

static void
memphis_rule_class_init (MemphisRuleClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  //g_type_class_add_private (klass, sizeof (MemphisRulePrivate));

  object_class->get_property = memphis_rule_get_property;
  object_class->set_property = memphis_rule_set_property;
  object_class->dispose = memphis_rule_dispose;
  object_class->finalize = memphis_rule_finalize;
}

static void
memphis_rule_init (MemphisRule *self)
{
  self->keys = NULL;
  self->values = NULL;
  self->type = 0;

  self->polygon_color[0] = -1;
  self->polygon_color[1] = -1;
  self->polygon_color[2] = -1;
  self->polygon_z[0] = -1;
  self->polygon_z[1] = -1;

  self->line_color[0] = -1;
  self->line_color[1] = -1;
  self->line_color[2] = -1;
  self->line_size = -1.0;
  self->line_z[0] = -1;
  self->line_z[1] = -1;

  self->border_color[0] = -1;
  self->border_color[1] = -1;
  self->border_color[2] = -1;
  self->border_size = -1.0;
  self->border_z[0] = -1;
  self->border_z[1] = -1;

  self->text_color[0] = -1;
  self->text_color[1] = -1;
  self->text_color[2] = -1;
  self->text_size = -1.0;
  self->text_z[0] = -1;
  self->text_z[1] = -1;
}

MemphisRule*
memphis_rule_new (void)
{
  return g_object_new (MEMPHIS_TYPE_RULE, NULL);
}

void
memphis_rule_free (MemphisRule* rule)
{
  g_object_unref (rule);
}
