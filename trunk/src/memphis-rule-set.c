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

#include <string.h>
#include "mlib.h"
#include "ruleset.h"
#include "memphis-private.h"
#include "memphis-rule-set.h"
#include "memphis-data-pool.h"

G_DEFINE_TYPE (MemphisRuleSet, memphis_rule_set, G_TYPE_OBJECT)

#define MEMPHIS_RULE_SET_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), MEMPHIS_TYPE_RULE_SET, MemphisRuleSetPrivate))

enum
{
  PROP_0,
  PROP_DEBUG_LEVEL
};

typedef struct _MemphisRuleSetPrivate MemphisRuleSetPrivate;

struct _MemphisRuleSetPrivate {
  gint8 debug_level;
  cfgRules *ruleset;
};

static void
memphis_rule_set_get_property (GObject *object, guint property_id,
                              GValue *value, GParamSpec *pspec)
{
  MemphisRuleSet *self = MEMPHIS_RULE_SET (object);
  MemphisRuleSetPrivate *priv = MEMPHIS_RULE_SET_GET_PRIVATE (self);
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
  MemphisRuleSet *self = MEMPHIS_RULE_SET (object);
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
  MemphisRuleSet *self = MEMPHIS_RULE_SET (object);
  MemphisRuleSetPrivate *priv = MEMPHIS_RULE_SET_GET_PRIVATE (self);

  if (priv->ruleset != NULL)
    rulesetFree(priv->ruleset);
  
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
  MemphisRuleSetPrivate *priv = MEMPHIS_RULE_SET_GET_PRIVATE (self);
  priv->ruleset = NULL;
  priv->debug_level = 1;
}

MemphisRuleSet*
memphis_rule_set_new ()
{
  return g_object_new (MEMPHIS_TYPE_RULE_SET, NULL);
}

void
memphis_rule_set_load_from_file (MemphisRuleSet *rules, const gchar *filename)
{
  g_return_if_fail (MEMPHIS_IS_RULE_SET (rules) && filename != NULL);

  MemphisRuleSetPrivate *priv = MEMPHIS_RULE_SET_GET_PRIVATE (rules);
  if (priv->ruleset != NULL)
    rulesetFree (priv->ruleset);

  priv->ruleset = rulesetRead (filename, priv->debug_level);
}

void
memphis_rule_set_load_from_data (MemphisRuleSet *rules,
    const gchar *data,
    guint size)
{
  g_return_if_fail (MEMPHIS_IS_RULE_SET (rules) && data != NULL);

  MemphisRuleSetPrivate *priv = MEMPHIS_RULE_SET_GET_PRIVATE (rules);
  if (priv->ruleset != NULL)
    rulesetFree (priv->ruleset);

  priv->ruleset = rulesetRead_from_buffer (data, size, priv->debug_level);
}

void
memphis_rule_set_free (MemphisRuleSet *rules)
{
  g_return_if_fail (MEMPHIS_IS_RULE_SET (rules));

  g_object_unref (G_OBJECT (rules));
}

void
memphis_rule_set_set_debug_level (MemphisRuleSet *rules,
    gint8 debug_level)
{
  g_return_if_fail (MEMPHIS_IS_RULE_SET (rules));

  MemphisRuleSetPrivate *priv = MEMPHIS_RULE_SET_GET_PRIVATE (rules);
  priv->debug_level = debug_level;
}

gint8
memphis_rule_set_get_debug_level (MemphisRuleSet *rules)
{
  g_return_val_if_fail (MEMPHIS_IS_RULE_SET (rules), -1);

  MemphisRuleSetPrivate *priv = MEMPHIS_RULE_SET_GET_PRIVATE (rules);
  return priv->debug_level;
}

void
memphis_rule_set_set_bg_color (MemphisRuleSet *self,
    guint8 r,
    guint8 g,
    guint8 b,
    guint8 a)
{
  MemphisRuleSetPrivate *priv = MEMPHIS_RULE_SET_GET_PRIVATE (self);

  priv->ruleset->background[0] = (gint16) r;
  priv->ruleset->background[1] = (gint16) g;
  priv->ruleset->background[2] = (gint16) b;
  // TODO: alpha support
}

void
memphis_rule_set_get_bg_color (MemphisRuleSet *self,
    guint8 *r,
    guint8 *g,
    guint8 *b,
    guint8 *a)
{
  MemphisRuleSetPrivate *priv = MEMPHIS_RULE_SET_GET_PRIVATE (self);

  *r = (guint16) priv->ruleset->background[0];
  *g = (guint16) priv->ruleset->background[1];
  *b = (guint16) priv->ruleset->background[2];
  // TODO: alpha support
}

GList *
memphis_rule_set_get_rule_ids (MemphisRuleSet *self)
{
  g_return_val_if_fail (MEMPHIS_IS_RULE_SET (self), NULL);

  MemphisRuleSetPrivate *priv = MEMPHIS_RULE_SET_GET_PRIVATE (self);

  GList *list = NULL;
  cfgRule *curr = priv->ruleset->rule;
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
  switch (curr->type)
    {
      case (WAY):
        rule->type = MEMPHIS_RULE_TYPE_WAY;
        break;
      case (NODE):
        rule->type = MEMPHIS_RULE_TYPE_NODE;
        break;
      case (RELATION):
        rule->type = MEMPHIS_RULE_TYPE_RELATION;
        break;
      default:
        rule->type = MEMPHIS_RULE_TYPE_UNKNOWN;
    }

  cfgDraw *drw = curr->draw;
  gboolean line_seen = FALSE;
  while (drw != NULL)
    {
      if (drw->type == POLYGONE)
        {
          rule->polygon = g_slice_new0 (MemphisRuleAttr);
          rule->polygon->color_red = drw->color[0];
          rule->polygon->color_green = drw->color[1];
          rule->polygon->color_blue = drw->color[2];
          rule->polygon->color_alpha = 255;
          rule->polygon->z_min = drw->minzoom;
          rule->polygon->z_max = drw->maxzoom;
          // TODO support pattern
        }
      else if (drw->type == LINE)
        {
          if (line_seen) {
            /* line with border */
            rule->border = g_slice_new0 (MemphisRuleAttr);
            rule->border->color_red = rule->line->color_red;
            rule->border->color_green = rule->line->color_green;
            rule->border->color_blue = rule->line->color_blue;
            rule->border->color_alpha = rule->line->color_alpha;
            rule->border->size = (rule->line->size - drw->width) * 0.5;
            rule->border->z_min = rule->line->z_min;
            rule->border->z_max = rule->line->z_max;

            rule->line->color_red = drw->color[0];
            rule->line->color_green = drw->color[1];
            rule->line->color_blue = drw->color[2];
            rule->line->color_alpha = 255;
            rule->line->size = drw->width;
            rule->line->z_min = drw->minzoom;
            rule->line->z_max = drw->maxzoom;
          } else {
            /* only a single line */
            rule->line = g_slice_new0 (MemphisRuleAttr);
            rule->line->color_red = drw->color[0];
            rule->line->color_green = drw->color[1];
            rule->line->color_blue = drw->color[2];
            rule->line->color_alpha = 255;
            rule->line->size = drw->width;
            rule->line->z_min = drw->minzoom;
            rule->line->z_max = drw->maxzoom;
            line_seen = TRUE;
          }
        }
      else if (drw->type == TEXT)
        {
          rule->text = g_slice_new0 (MemphisRuleAttr);
          rule->text->color_red = drw->color[0];
          rule->text->color_green = drw->color[1];
          rule->text->color_blue = drw->color[2];
          rule->text->color_alpha = 255;
          rule->text->size = drw->width;
          rule->text->z_min = drw->minzoom;
          rule->text->z_max = drw->maxzoom;
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

static void
add_new_cfgDraws (cfgRule *out, MemphisRule *rule)
{
  cfgDraw *tmp;
  cfgDraw *drw = NULL;

  if (rule->polygon != NULL)
    {
      tmp = g_new (cfgDraw, 1);
      tmp->next = drw;
      tmp->type = POLYGONE;
      tmp->minzoom = rule->polygon->z_min;
      tmp->maxzoom = rule->polygon->z_max;
      tmp->color[0] = rule->polygon->color_red;
      tmp->color[1] = rule->polygon->color_green;
      tmp->color[2] = rule->polygon->color_blue;
      drw = tmp;
    }
  if (rule->line != NULL)
    {
      tmp = g_new (cfgDraw, 1);
      tmp->next = drw;
      tmp->type = LINE;
      tmp->minzoom = rule->line->z_min;
      tmp->maxzoom = rule->line->z_max;
      tmp->color[0] = rule->line->color_red;
      tmp->color[1] = rule->line->color_green;
      tmp->color[2] = rule->line->color_blue;
      tmp->width = rule->line->size;
      drw = tmp;
    }
  if (rule->border != NULL)
    {
      tmp = g_new (cfgDraw, 1);
      tmp->next = drw;
      tmp->type = LINE;
      tmp->minzoom = rule->border->z_min;
      tmp->maxzoom = rule->border->z_max;
      tmp->color[0] = rule->border->color_red;
      tmp->color[1] = rule->border->color_green;
      tmp->color[2] = rule->border->color_blue;
      if (rule->line != NULL)
        tmp->width = 2.0 * rule->border->size + rule->line->size;
      else
        tmp->width = rule->border->size; /* polygon border */
      drw = tmp;
    }
  if (rule->text != NULL)
    {
      tmp = g_new (cfgDraw, 1);
      tmp->next = drw;
      tmp->type = TEXT;
      tmp->minzoom = rule->text->z_min;
      tmp->maxzoom = rule->text->z_max;
      tmp->color[0] = rule->text->color_red;
      tmp->color[1] = rule->text->color_green;
      tmp->color[2] = rule->text->color_blue;
      tmp->width = rule->text->size;
      drw = tmp;
    }
    out->draw = drw;
}

static cfgRule *
cfgRule_new_from_rule (MemphisRule *rule)
{
  int c, len;
  MemphisDataPool *pool = memphis_data_pool_new ();
  cfgRule *new = g_new (cfgRule, 1);

  switch (rule->type)
    {
      case (MEMPHIS_RULE_TYPE_WAY):
        new->type = WAY;
        break;
      case (MEMPHIS_RULE_TYPE_NODE):
        new->type = NODE;
        break;
      case (MEMPHIS_RULE_TYPE_RELATION):
        new->type = RELATION;
        break;
      default:
        new->type = 0;
    }

  new->value = g_strdupv (rule->values);
  len = g_strv_length (new->value);
  for(c = 0; c < len; c++)
    {
      char *tmp = new->value[c];
      new->value[c] = m_string_chunk_get(pool->stringChunk,
                                         pool->stringTree, tmp);
      g_free(tmp);
    }
  new->key = g_strdupv (rule->keys);
  len = g_strv_length(new->key);
  for(c = 0; c < len; c++)
    {
      char *tmp = new->key[c];
      new->key[c] = m_string_chunk_get(pool->stringChunk,
                                       pool->stringTree, tmp);
      g_free(tmp);
    }

  g_object_unref (pool);

  new->parent = NULL;
  new->nparent = NULL;
  new->next = NULL;
  new->draw = NULL;
  new->ndraw = NULL;

  add_new_cfgDraws (new, rule);

  return new;
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
  g_return_val_if_fail (MEMPHIS_IS_RULE_SET (self) && id != NULL, NULL);

  MemphisRuleSetPrivate *priv = MEMPHIS_RULE_SET_GET_PRIVATE (self);

  gchar **tmp = g_strsplit (id, ":", -1);
  gchar **keys = g_strsplit (tmp[0], "|", -1);
  gchar **values = g_strsplit (tmp[1], "|", -1);
  g_strfreev (tmp);

  cfgRule *res = search_rule (priv->ruleset->rule, keys, values);

  g_strfreev (keys);
  g_strfreev (values);

  if (res != NULL)
    return rule_new_from_cfgRule (res);

  return NULL;
}

void
memphis_rule_set_set_rule (MemphisRuleSet *self, MemphisRule *rule)
{
  g_return_if_fail (MEMPHIS_IS_RULE_SET (self) && MEMPHIS_RULE (rule));

  MemphisRuleSetPrivate *priv = MEMPHIS_RULE_SET_GET_PRIVATE (self);
  cfgRule *res = search_rule (priv->ruleset->rule, rule->keys, rule->values);
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

      /* Add new cfgDraw's */
      add_new_cfgDraws (res, rule);
    }
  else
    {
      /* Append cfgRule at last position */
      cfgRule *curr = priv->ruleset->rule;
      while (curr->next != NULL)
        curr = curr->next;

      cfgRule *new = cfgRule_new_from_rule (rule);

      curr->next = new;
      priv->ruleset->cntRule++;
    }
}

gboolean
memphis_rule_set_remove_rule (MemphisRuleSet *self, const gchar *id)
{
  g_return_val_if_fail (MEMPHIS_IS_RULE_SET (self) && id != NULL, FALSE);

  MemphisRuleSetPrivate *priv = MEMPHIS_RULE_SET_GET_PRIVATE (self);

  gchar **tmp = g_strsplit (id, ":", -1);
  gchar **keys = g_strsplit (tmp[0], "|", -1);
  gchar **values = g_strsplit (tmp[1], "|", -1);
  g_strfreev (tmp);

  int i;
  gint len;
  gboolean found = FALSE;
  gboolean miss = FALSE;
  gint num_keys = g_strv_length (keys);
  gint num_values = g_strv_length (values);
  cfgRule *curr = priv->ruleset->rule;
  cfgRule *prev = NULL;

  while (curr != NULL && !found)
   {
      miss = FALSE;
      len = g_strv_length (curr->key);
      if (len != num_keys)
        {
          prev = curr;
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
          prev = curr;
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
          prev = curr;
          curr = curr->next;
          continue;
        }

      found = TRUE;
   }

  g_strfreev (keys);
  g_strfreev (values);

  if (curr == NULL)
    return FALSE;

  prev->next = curr->next;
  cfgRuleFree (curr);
  priv->ruleset->cntRule--;
  return TRUE;
}

/* private shared functions */

cfgRules *
memphis_rule_set_get_cfgRules (MemphisRuleSet *self)
{
  g_assert (self != NULL);

  MemphisRuleSetPrivate *priv = MEMPHIS_RULE_SET_GET_PRIVATE (self);

  return priv->ruleset;
}
