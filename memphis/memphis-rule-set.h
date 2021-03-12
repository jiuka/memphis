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

#ifndef _MEMPHIS_RULE_SET
#define _MEMPHIS_RULE_SET

#include <glib-object.h>
#include "memphis-rule.h"

G_BEGIN_DECLS

#define MEMPHIS_TYPE_RULE_SET memphis_rule_set_get_type ()
G_DECLARE_FINAL_TYPE (MemphisRuleSet, memphis_rule_set, MEMPHIS, RULE_SET, GObject)

MemphisRuleSet* memphis_rule_set_new (void);

gboolean memphis_rule_set_load_from_file (MemphisRuleSet *rules,
    const gchar *filename,
    GError **error);
gboolean memphis_rule_set_load_from_data (MemphisRuleSet *rules,
    const gchar *data,
    guint size,
    GError **error);

void memphis_rule_set_set_bg_color (MemphisRuleSet *self,
    guint8 r, guint8 g, guint8 b, guint8 a);
void memphis_rule_set_get_bg_color (MemphisRuleSet *self,
    guint8 *r, guint8 *g, guint8 *b, guint8 *a);

GList* memphis_rule_set_get_rule_ids (MemphisRuleSet *rules);

void memphis_rule_set_set_rule (MemphisRuleSet *rules, MemphisRule *rule);
MemphisRule* memphis_rule_set_get_rule (MemphisRuleSet *rules, const gchar *id);

gboolean memphis_rule_set_remove_rule (MemphisRuleSet *rules, const gchar *id);

G_END_DECLS

#endif /* _MEMPHIS_RULE_SET */

