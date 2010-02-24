/*
 * Copyright (C) 2009 Simon Wenner <simon@wenner.ch>
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

#ifndef MEMPHIS_RULE_H
#define MEMPHIS_RULE_H

#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _MemphisRule MemphisRule;
typedef struct _MemphisRuleAttr MemphisRuleAttr;

#define MEMPHIS_RULE(obj)     ((MemphisRule *) (obj))

/**
 * MemphisRuleAttr:
 * @z_min: minimum visible zoom level
 * @z_max: maximum visible zoom level
 * @color_red: red component, between 0 and 255
 * @color_green: green component, between 0 and 255
 * @color_blue: blue component, between 0 and 255
 * @color_alpha: transparency component, between 0 and 255
 * @style: field for future use (line style, polygon pattern...)
 * @size: the size
 *
 * Defines the drawing attributes for a #MemphisRule.
 *
 * Since: 0.1
 */
struct _MemphisRuleAttr {
  guint8 z_min;
  guint8 z_max;
  guint8 color_red;
  guint8 color_green;
  guint8 color_blue;
  guint8 color_alpha;
  gchar *style;
  gdouble size;
};

/**
 * MemphisRuleType:
 * @MEMPHIS_RULE_TYPE_UNKNOWN: unknown
 * @MEMPHIS_RULE_TYPE_NODE: a node
 * @MEMPHIS_RULE_TYPE_WAY: a way
 * @MEMPHIS_RULE_TYPE_RELATION: a realtion
 *
 * Defines a the data type of the rule. Only ways are supported in
 * Memphis 0.1.x.
 *
 * Since: 0.1
 */
typedef enum {
  MEMPHIS_RULE_TYPE_UNKNOWN,
  MEMPHIS_RULE_TYPE_NODE,
  MEMPHIS_RULE_TYPE_WAY,
  MEMPHIS_RULE_TYPE_RELATION
} MemphisRuleType;

/**
 * MemphisRule:
 * @keys: an array of key strings
 * @values: an array of value strings
 * @type: the type of the rule
 * @polygon: a pointer to the polygon or NULL
 * @line: a pointer to the line or NULL
 * @border: a pointer to the border or NULL
 * @text: a pointer to the text or NULL
 *
 * Defines a drawing rule for the #MemphisRuleSet.
 *
 * Since: 0.1
 */
struct _MemphisRule {
  gchar **keys;
  gchar **values;
  MemphisRuleType type;
  MemphisRuleAttr *polygon;
  MemphisRuleAttr *line;
  MemphisRuleAttr *border;
  MemphisRuleAttr *text;
};

GType memphis_rule_get_type (void) G_GNUC_CONST;
#define MEMPHIS_TYPE_RULE (memphis_rule_get_type ())

/**
 * memphis_rule_new:
 *
 * Returns: a new #MemphisRule.
 *
 * Since: 0.1
 */
MemphisRule * memphis_rule_new (void);

/**
 * memphis_rule_copy:
 * @rule: a #MemphisRule
 *
 * Returns: a copy of the rule.
 *
 * Since: 0.1
 */
MemphisRule * memphis_rule_copy (const MemphisRule *rule);

/**
 * memphis_rule_free:
 * @rule: a #MemphisRule
 *
 * Frees the memory of a #MemphisRule.
 *
 * Since: 0.1
 */
void memphis_rule_free (MemphisRule *rule);

G_END_DECLS

#endif
