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
#include "ruleset.h"
#include "memphis-rule.h"

G_BEGIN_DECLS

#define MEMPHIS_TYPE_RULE_SET memphis_rule_set_get_type()

#define MEMPHIS_RULE_SET(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MEMPHIS_TYPE_RULE_SET, MemphisRuleSet))

#define MEMPHIS_RULE_SET_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), MEMPHIS_TYPE_RULE_SET, MemphisRuleSetClass))

#define MEMPHIS_IS_RULE_SET(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MEMPHIS_TYPE_RULE_SET))

#define MEMPHIS_IS_RULE_SET_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), MEMPHIS_TYPE_RULE_SET))

#define MEMPHIS_RULE_SET_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), MEMPHIS_TYPE_RULE_SET, MemphisRuleSetClass))

typedef struct {
  GObject parent;
  cfgRules *ruleset;
} MemphisRuleSet;

typedef struct {
  GObjectClass parent_class;
} MemphisRuleSetClass;

GType memphis_rule_set_get_type (void);

MemphisRuleSet* memphis_rule_set_new ();
void memphis_rule_set_free (MemphisRuleSet *rules);

void memphis_rule_set_load_from_file (MemphisRuleSet *rules,
    const gchar *filename);
void memphis_rule_set_load_from_data (MemphisRuleSet *rules,
    const gchar *data,
    guint size);

void memphis_rule_set_set_debug_level (MemphisRuleSet *rules,
    gint8 debug_level);

gint8 memphis_rule_set_get_debug_level (MemphisRuleSet *rules);

void memphis_rule_set_set_bg_color (MemphisRuleSet *rules,
    guint16 r, guint16 g, guint16 b);
void memphis_rule_set_get_bg_color (MemphisRuleSet *self,
    guint16 *r, guint16 *g, guint16 *b);

GList* memphis_rule_set_get_rule_ids (MemphisRuleSet *rules);

void memphis_rule_set_set_rule (MemphisRuleSet *rules, MemphisRule *rule);
MemphisRule* memphis_rule_set_get_rule (MemphisRuleSet *rules, const gchar *id);

gboolean memphis_rule_set_remove_rule (MemphisRuleSet *rules, const gchar *id);

G_END_DECLS

#endif /* _MEMPHIS_RULE_SET */

