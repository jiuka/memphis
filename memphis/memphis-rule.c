/*
 * Copyright (C) 2009 Simon Wenner <simon@wenner.ch>
 *
 * This file is inspired by clutter-color.c which is
 * Copyright (C) 2006 OpenedHand, and has the same license.
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

/**
 * SECTION:memphis-rule
 * @short_description: Describes a drawing rule.
 *
 * A boxed struct to describe a drawing rule for the #MemphisRuleSet.
 */

#include "memphis-rule.h"

static const GEnumValue _memphis_rule_type_values[] = {
  { MEMPHIS_RULE_TYPE_UNKNOWN, "MEMPHIS_RULE_TYPE_UNKNOWN", "unknown" },
  { MEMPHIS_RULE_TYPE_NODE, "MEMPHIS_RULE_TYPE_NODE", "node" },
  { MEMPHIS_RULE_TYPE_WAY, "MEMPHIS_RULE_TYPE_WAY", "way" },
  { MEMPHIS_RULE_TYPE_RELATION, "MEMPHIS_RULE_TYPE_RELATION", "relation" },
  { 0, NULL, NULL }
};

GType
memphis_rule_type_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (type == 0))
    type = g_enum_register_static ("MemphisRuleType", _memphis_rule_type_values);

  return type;
}

G_DEFINE_BOXED_TYPE(MemphisRuleAttr, memphis_rule_attr, memphis_rule_attr_copy, memphis_rule_attr_free)

/**
 * memphis_rule_attr_new:
 *
 * Returns: a new #MemphisRuleAttr.
 *
 * Since: 0.2
 */
MemphisRuleAttr*
memphis_rule_attr_new ()
{
  return g_new0 (MemphisRuleAttr, 1);
}

/**
 * memphis_rule_attr_free:
 * @attr: a #MemphisRuleAttr
 *
 * Frees the memory of a #MemphisRuleAttr.
 *
 * Since: 0.2
 */
void
memphis_rule_attr_free (MemphisRuleAttr * attr)
{
  g_assert (attr != NULL);

  g_clear_pointer (&attr->style, g_free);
  g_free (attr);
}

/**
 * memphis_rule_attr_copy:
 * @attr: a #MemphisRuleAttr
 *
 * Returns: a copy of the rule attribute.
 *
 * Since: 0.2
 */
MemphisRuleAttr *
memphis_rule_attr_copy (const MemphisRuleAttr * attr)
{
  g_assert (attr != NULL);

  MemphisRuleAttr * new;
  new = g_memdup2 (attr, sizeof(MemphisRuleAttr));

  if (attr->style)
    new->style = g_strdup (attr->style);

  return new;
}

G_DEFINE_BOXED_TYPE(MemphisRule, memphis_rule, memphis_rule_copy, memphis_rule_free)

/**
 * memphis_rule_new:
 *
 * Creates a newly allocated #MemphisRule to be freed
 * with memphis_rule_free().
 *
 * Returns: a #MemphisRule.
 *
 * Since: 0.1
 */
MemphisRule *
memphis_rule_new (void)
{
  MemphisRule *rule = g_new0 (MemphisRule, 1);
  rule->type = MEMPHIS_RULE_TYPE_UNKNOWN;
  return rule;
}

/**
 * memphis_rule_copy:
 * @rule: a #MemphisRule
 *
 * Makes a copy of the memphis rule structure. The result must be
 * freed using memphis_rule_free().
 *
 * Returns: an allocated copy of @rule.
 *
 * Since: 0.1
 */
MemphisRule *
memphis_rule_copy (const MemphisRule *rule)
{
  MemphisRule *res;

  if (G_UNLIKELY (rule == NULL))
    return NULL;

  res = g_memdup2 (rule, sizeof(MemphisRule));
  if (rule->keys)
    res->keys = g_strdupv (rule->keys);
  if (rule->values)
    res->values = g_strdupv (rule->values);
  if (rule->polygon)
    res->polygon = memphis_rule_attr_copy (rule->polygon);
  if (rule->line)
    res->line = memphis_rule_attr_copy (rule->line);
  if (rule->border)
    res->border = memphis_rule_attr_copy (rule->border);
  if (rule->text)
    res->text = memphis_rule_attr_copy (rule->text);

  return res;
}

/**
 * memphis_rule_free:
 * @rule: a #MemphisRule
 *
 * Frees a memphis rule structure created with memphis_rule_new() or
 * memphis_rule_copy().
 *
 * Since: 0.1
 */
void
memphis_rule_free (MemphisRule *rule)
{
  if (G_UNLIKELY (rule == NULL))
    return;

  g_clear_pointer (&rule->keys, g_strfreev);
  g_clear_pointer (&rule->values, g_strfreev);
  g_clear_pointer (&rule->polygon, memphis_rule_attr_free);
  g_clear_pointer (&rule->line, memphis_rule_attr_free);
  g_clear_pointer (&rule->border, memphis_rule_attr_free);
  g_clear_pointer (&rule->text, memphis_rule_attr_free);

  g_free (rule);
}
