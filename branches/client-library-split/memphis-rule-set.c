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
#include <string.h>

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

  MemphisRuleSetPrivate *priv = MEMPHIS_RULESET_GET_PRIVATE (rules);
  if (rules->ruleset != NULL)
    rulesetFree (rules->ruleset);

  rules->ruleset = rulesetRead_from_buffer (data, size, priv->debug_level);
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

void
memphis_rule_set_set_bg_color (MemphisRuleSet *self,
    gint16 r, gint16 g, gint16 b)
{
  self->ruleset->background[0] = r;
  self->ruleset->background[1] = g;
  self->ruleset->background[2] = b;
}

void
memphis_rule_set_get_bg_color (MemphisRuleSet *self,
    gint16 *r, gint16 *g, gint16 *b)
{
  *r = self->ruleset->background[0];
  *g = self->ruleset->background[1];
  *b = self->ruleset->background[2];
}

GList *
memphis_rule_set_get_rule_ids (MemphisRuleSet *self)
{
  g_return_val_if_fail (MEMPHIS_IS_RULESET (self), NULL);

  GList *list = NULL;
  cfgRule *curr = self->ruleset->rule;
  while (curr != NULL)
   {
     if (curr->draw != NULL)
      {
        gchar *keys = g_strjoinv ("|", curr->key);
        gchar *values = g_strjoinv ("|", curr->value);
        gchar *id = g_strconcat (keys, ":", values, NULL);
        list = g_list_append (list, id);
        g_free (keys);
        g_free (values);
      }            
      curr = curr->next;
   }
   return list;
}

static MemphisRule *
rule_new_from_cfgRule (cfgRule *curr)
{
  MemphisRule *rule;
  rule = memphis_rule_new ();
  rule->keys = g_strdupv (curr->key);
  rule->values = g_strdupv (curr->value);
  rule->type = curr->type;

  cfgDraw *drw = curr->draw;
  gboolean line_seen = FALSE;
  while (drw != NULL)
    {
      if (drw->type == POLYGONE)
        {
          rule->polygon_color[0] = drw->color[0];
          rule->polygon_color[1] = drw->color[1];
          rule->polygon_color[2] = drw->color[2];
          rule->polygon_z[0] = drw->minzoom;
          rule->polygon_z[1] = drw->maxzoom;
          // TODO support pattern
        }
      else if (drw->type == LINE)
        {
          if (line_seen) {
            rule->line_color[0] = drw->color[0];
            rule->line_color[1] = drw->color[1];
            rule->line_color[2] = drw->color[2];
            rule->line_size = drw->width;
            rule->line_z[0] = drw->minzoom;
            rule->line_z[1] = drw->maxzoom;
          } else {
            rule->border_color[0] = drw->color[0];
            rule->border_color[1] = drw->color[1];
            rule->border_color[2] = drw->color[2];
            rule->border_size = drw->width;
            rule->border_z[0] = drw->minzoom;
            rule->border_z[1] = drw->maxzoom;
            line_seen = TRUE;
          }
        }
      else if (drw->type == TEXT)
        {
          rule->text_color[0] = drw->color[0];
          rule->text_color[1] = drw->color[1];
          rule->text_color[2] = drw->color[2];
          rule->text_size = drw->width;
          rule->text_z[0] = drw->minzoom;
          rule->text_z[1] = drw->maxzoom;
        }
      drw = drw->next;
    }
  /*
  cfgDraw *ndrw = curr->ndraw;
  while (ndrw != NULL)
    {
      g_print ("NDRAW: %d\n", ndrw->type);
      ndrw = ndrw->next;
    }
  */

  return rule;
}

static cfgRule *
search_rule (cfgRule *rules, gchar **keys, gchar **values)
{
  int i;
  gint len;
  gboolean found = FALSE;
  gboolean miss = FALSE;
  gint num_keys = g_strv_length (keys);
  gint num_values = g_strv_length (values);
  cfgRule *curr = rules;

  while (curr != NULL && !found)
   {
      miss = FALSE;
      if (curr->draw != NULL)
      {
        len = g_strv_length (curr->key);
        if (len != num_keys)
          {
            curr = curr->next;
            continue;
          }
        for (i = 0; i < len; i++)
          {
            if (strcmp (curr->key[i], keys[i]) != 0)
              miss = TRUE;
          }

        len = g_strv_length (curr->value);
        if (len != num_values || miss)
          {
            curr = curr->next;
            continue;
          }
        for (i = 0; i < len; i++)
          {
            if (strcmp (curr->value[i], values[i]) != 0)
              miss = TRUE;
          }
        if (miss)
          {
            curr = curr->next;
            continue;
          }

        found = TRUE;
      }
    else
      {
        curr = curr->next;
      }
   }

  return curr;
}

MemphisRule *
memphis_rule_set_get_rule (MemphisRuleSet *self, const gchar *id)
{
  g_return_val_if_fail (MEMPHIS_IS_RULESET (self) && id != NULL, NULL);

  gchar **tmp = g_strsplit (id, ":", -1);
  gchar **keys = g_strsplit (tmp[0], "|", -1);
  gchar **values = g_strsplit (tmp[1], "|", -1);
  g_strfreev (tmp);

  cfgRule *res = search_rule (self->ruleset->rule, keys, values);

  g_strfreev (keys);
  g_strfreev (values);

  if (res != NULL)
      return rule_new_from_cfgRule (res);

  return NULL;
}

void
memphis_rule_set_set_rule (MemphisRuleSet *self, MemphisRule *rule)
{
  g_return_if_fail (MEMPHIS_IS_RULESET (self) && MEMPHIS_IS_RULE (rule));

  cfgRule *res = search_rule (self->ruleset->rule, rule->keys, rule->values);
  cfgDraw *drw, *tmp;

  if (res != NULL)
    {
      drw = res->draw;
      /* delete old cfgDraw's */
      while (drw != NULL)
        {
          tmp = drw;
          drw = drw->next;
          g_free (tmp);
        }
      drw = NULL;
      /* Add new cfgDraw's */
      if (rule->polygon_color[0] != -1)
        {
          tmp = g_new (cfgDraw, 1);
          tmp->next = drw;
          tmp->type = POLYGONE;
          tmp->minzoom = rule->polygon_z[0];
          tmp->maxzoom = rule->polygon_z[1];
          tmp->color[0] = rule->polygon_color[0];
          tmp->color[1] = rule->polygon_color[1];
          tmp->color[2] = rule->polygon_color[2];
          drw = tmp;
        }
      if (rule->line_color[0] != -1)
        {
          tmp = g_new (cfgDraw, 1);
          tmp->next = drw;
          tmp->type = LINE;
          tmp->minzoom = rule->line_z[0];
          tmp->maxzoom = rule->line_z[1];
          tmp->color[0] = rule->line_color[0];
          tmp->color[1] = rule->line_color[1];
          tmp->color[2] = rule->line_color[2];
          tmp->width = rule->line_size;
          drw = tmp;
        }
      if (rule->border_color[0] != -1)
        {
          tmp = g_new (cfgDraw, 1);
          tmp->next = drw;
          tmp->type = LINE;
          tmp->minzoom = rule->border_z[0];
          tmp->maxzoom = rule->border_z[1];
          tmp->color[0] = rule->border_color[0];
          tmp->color[1] = rule->border_color[1];
          tmp->color[2] = rule->border_color[2];
          tmp->width = rule->border_size;
          drw = tmp;
        }
      if (rule->text_color[0] != -1)
        {
          tmp = g_new (cfgDraw, 1);
          tmp->next = drw;
          tmp->type = TEXT;
          tmp->minzoom = rule->text_z[0];
          tmp->maxzoom = rule->text_z[1];
          tmp->color[0] = rule->text_color[0];
          tmp->color[1] = rule->text_color[1];
          tmp->color[2] = rule->text_color[2];
          tmp->width = rule->text_size;
          drw = tmp;
        }
        res->draw = drw;
      /* Update cfgDraw's */
      /*while (drw != NULL)
        {
          
          if (drw->type == LINE)
            {
              if (line_seen) {
                drw->minzoom = rule->line_z[0];
                drw->maxzoom = rule->line_z[1];
                drw->color[0] = rule->line_color[0];
                drw->color[1] = rule->line_color[1];
                drw->color[2] = rule->line_color[2];
                g_print ("COLOR2: %d %d %d\n", drw->color[0], drw->color[1], drw->color[2]);
                drw->width = rule->line_size;
              } else {
                drw->minzoom = rule->border_z[0];
                drw->maxzoom = rule->border_z[1];
                drw->color[0] = rule->border_color[0];
                drw->color[1] = rule->border_color[1];
                drw->color[2] = rule->border_color[2];
                g_print ("COLOR2b: %d %d %d\n", drw->color[0], drw->color[1], drw->color[2]);
                drw->width = rule->border_size;
                line_seen = TRUE;
              }
            }
          else if (drw->type == TEXT)
            {
              drw->minzoom = rule->text_z[0];
              drw->maxzoom = rule->text_z[1];
              drw->color[0] = rule->text_color[0];
              drw->color[1] = rule->text_color[1];
              drw->color[2] = rule->text_color[2];
              drw->width = rule->text_size;
            }
          else if (drw->type == POLYGONE)
            {
              drw->minzoom = rule->polygon_z[0];
              drw->maxzoom = rule->polygon_z[1];
              drw->color[0] = rule->polygon_color[0];
              drw->color[1] = rule->polygon_color[1];
              drw->color[2] = rule->polygon_color[2];
            }
           drw = drw->next;
        }*/
    }
  else
    {
      /* Append cfgRule */

      /* Add new cfgDraw's */

      // TODO
    }
}
