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

#ifndef _MEMPHIS_RULE
#define _MEMPHIS_RULE

#include <glib-object.h>

G_BEGIN_DECLS

#define MEMPHIS_TYPE_RULE memphis_rule_get_type()

#define MEMPHIS_RULE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MEMPHIS_TYPE_RULE, MemphisRule))

#define MEMPHIS_RULE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), MEMPHIS_TYPE_RULE, MemphisRuleClass))

#define MEMPHIS_IS_RULE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MEMPHIS_TYPE_RULE))

#define MEMPHIS_IS_RULE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), MEMPHIS_TYPE_RULE))

#define MEMPHIS_RULE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), MEMPHIS_TYPE_RULE, MemphisRuleClass))

typedef struct {
  GObject parent;

  gchar **keys;
  gchar **values;
  gint16 type; /* Node, way or relation */

  gint16 polygon_color[3];
  gint16 polygon_z[2];

  gint16 line_color[3];
  gdouble line_size;
  gint16 line_z[2];

  gint16 border_color[3];
  gdouble border_size;
  gint16 border_z[2];

  gint16 text_color[3];
  gdouble text_size;
  gint16 text_z[2];
} MemphisRule;

typedef struct {
  GObjectClass parent_class;
} MemphisRuleClass;

GType memphis_rule_get_type (void);

MemphisRule* memphis_rule_new (void);
void memphis_rule_free (MemphisRule* rule);

G_END_DECLS

#endif /* _MEMPHIS_RULE */


