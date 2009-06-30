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

G_BEGIN_DECLS

#define MEMPHIS_TYPE_RULESET memphis_rule_set_get_type()

#define MEMPHIS_RULESET(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MEMPHIS_TYPE_RULESET, MemphisRuleSet))

#define MEMPHIS_RULESET_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), MEMPHIS_TYPE_RULESET, MemphisRuleSetClass))

#define MEMPHIS_IS_RULESET(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MEMPHIS_TYPE_RULESET))

#define MEMPHIS_IS_RULESET_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), MEMPHIS_TYPE_RULESET))

#define MEMPHIS_RULESET_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), MEMPHIS_TYPE_RULESET, MemphisRuleSetClass))

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

G_END_DECLS

#endif /* _MEMPHIS_RULE_SET */

