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

GType
memphis_rule_attr_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (type == 0))
    type = g_boxed_type_register_static (g_intern_static_string ("MemphisRuleAttr"),
          (GBoxedCopyFunc) memphis_rule_attr_copy,
          (GBoxedFreeFunc) memphis_rule_attr_free);

  return type;
}

MemphisRuleAttr*
memphis_rule_attr_new ()
{
  return g_slice_new (MemphisRuleAttr);
}

void
memphis_rule_attr_free (MemphisRuleAttr * attr)
{
  g_assert (attr != NULL);

  if (attr->style)
    g_free (attr->style);

  g_slice_free (MemphisRuleAttr, attr);
}

MemphisRuleAttr *
memphis_rule_attr_copy (const MemphisRuleAttr * attr)
{
  g_assert (attr != NULL);

  MemphisRuleAttr * new;
  new = g_slice_dup (MemphisRuleAttr, attr);

  if (attr->style)
    new->style = g_strdup (attr->style);

  return new;
}

GType
memphis_rule_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (type == 0))
    {
      type = g_boxed_type_register_static (
          g_intern_static_string ("MemphisRule"),
          (GBoxedCopyFunc) memphis_rule_copy,
          (GBoxedFreeFunc) memphis_rule_free);
    }

  return type;
}

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
  MemphisRule *rule = g_slice_new (MemphisRule);
  rule->keys = NULL;
  rule->values = NULL;
  rule->type = MEMPHIS_RULE_TYPE_UNKNOWN;
  rule->polygon = NULL;
  rule->line = NULL;
  rule->border = NULL;
  rule->text = NULL;
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
  if (G_UNLIKELY (rule == NULL))
    return NULL;

  MemphisRule *res;
  res = g_slice_dup (MemphisRule, rule);

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

  if (rule->keys)
    g_strfreev (rule->keys);
  if (rule->values)
    g_strfreev (rule->values);
  if (rule->polygon)
    memphis_rule_attr_free (rule->polygon);
  if (rule->line)
    memphis_rule_attr_free (rule->line);
  if (rule->border)
    memphis_rule_attr_free (rule->border);
  if (rule->text)
    memphis_rule_attr_free (rule->text);

  g_slice_free (MemphisRule, rule);
}
