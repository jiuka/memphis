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

/**
 * SECTION:memphis-rule-set
 * @short_description: Defines drawing rules for the renderer.
 *
 * This Object defines drawing rules for a #MemphisRenderer. The rules
 * can be loaded from an XML file or can they be defined during
 * execution time.
 */

#include <string.h>
#include "mlib.h"
#include "ruleset.h"
#include "memphis-private.h"
#include "memphis-rule-set.h"
#include "memphis-data-pool.h"

struct _MemphisRuleSet
{
  GObject parent_instance;

  cfgRules *ruleset;
};

G_DEFINE_TYPE (MemphisRuleSet, memphis_rule_set, G_TYPE_OBJECT)

static void
memphis_rule_set_finalize (GObject *object)
{
  MemphisRuleSet *self = MEMPHIS_RULE_SET (object);

  g_clear_pointer (&self->ruleset, rulesetFree);

  G_OBJECT_CLASS (memphis_rule_set_parent_class)->finalize (object);
}

static void
memphis_rule_set_class_init (MemphisRuleSetClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = memphis_rule_set_finalize;
}

static void
memphis_rule_set_init (MemphisRuleSet *self)
{
  self->ruleset = rulesetNew ();
}

/**
 * memphis_rule_set_new:
 *
 * Returns: a new #MemphisRuleSet whithout any rules.
 *
 * Since: 0.1
 */
MemphisRuleSet*
memphis_rule_set_new (void)
{
  return g_object_new (MEMPHIS_TYPE_RULE_SET, NULL);
}

/**
 * memphis_rule_set_load_from_file:
 * @rules: a #MemphisRuleSet
 * @filename: a path to a rules file
 * @error: a pointer to a #GError or %NULL
 *
 * Load rules from an XML file.
 *
 * Since: 0.2
 */
void
memphis_rule_set_load_from_file (MemphisRuleSet *rules,
    const gchar *filename,
    GError **error)
{
  g_return_if_fail (MEMPHIS_IS_RULE_SET (rules) && filename != NULL);

  g_clear_pointer (&rules->ruleset, rulesetFree);

  rules->ruleset = rulesetRead (filename, error);
}

/**
 * memphis_rule_set_load_from_data:
 * @rules: a #MemphisRuleSet
 * @data: a character array with rules XML data
 * @size: the size of the array
 * @error: a pointer to a #GError or %NULL
 *
 * Load rules data from an XML file.
 *
 * Since: 0.2
 */
void
memphis_rule_set_load_from_data (MemphisRuleSet *rules,
    const gchar *data,
    guint size,
    GError **error)
{
  g_return_if_fail (MEMPHIS_IS_RULE_SET (rules) && data != NULL);

  g_clear_pointer (&rules->ruleset, rulesetFree);

  rules->ruleset = rulesetRead_from_buffer (data, size, error);
}

/**
 * memphis_rule_set_set_bg_color:
 * @self: a #MemphisRuleSet
 * @r: red color component
 * @g: green color component
 * @b: blue color component
 * @a: transparency
 *
 * Sets the background color and transparency rule.
 *
 * Since: 0.1
 */
void
memphis_rule_set_set_bg_color (MemphisRuleSet *self,
    guint8 r,
    guint8 g,
    guint8 b,
    guint8 a)
{
  self->ruleset->background[0] = r;
  self->ruleset->background[1] = g;
  self->ruleset->background[2] = b;
  self->ruleset->background[3] = a;
}

/**
 * memphis_rule_set_get_bg_color:
 * @self: a #MemphisRuleSet
 * @r: (out): red color component
 * @g: (out): green color component
 * @b: (out): blue color component
 * @a: (out): transparency
 *
 * Assigns the background color and the transparency of the background
 * to r, g, b and a.
 *
 * Since: 0.1
 */
void
memphis_rule_set_get_bg_color (MemphisRuleSet *self,
    guint8 *r,
    guint8 *g,
    guint8 *b,
    guint8 *a)
{
  *r = self->ruleset->background[0];
  *g = self->ruleset->background[1];
  *b = self->ruleset->background[2];
  *a = self->ruleset->background[3];
}

/**
 * memphis_rule_set_get_rule_ids:
 * @rules: a #MemphisRuleSet
 *
 * Returns: (transfer full) (element-type utf8): a list of rule id strings.
 * Free the list with g_list_free_full() when done.
 *
 * These strings have the following form:
 * key1|key2|...|keyN:value1|value2|...|valueM
 * 
 * Example: "waterway:river|stream|canal"
 * 
 * Since: 0.1
 */
GList *
memphis_rule_set_get_rule_ids (MemphisRuleSet *self)
{
  g_return_val_if_fail (MEMPHIS_IS_RULE_SET (self), NULL);

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
          rule->polygon = memphis_rule_attr_new ();
          rule->polygon->color_red = drw->color[0];
          rule->polygon->color_green = drw->color[1];
          rule->polygon->color_blue = drw->color[2];
          rule->polygon->color_alpha = 255;
          rule->polygon->z_min = drw->minzoom;
          rule->polygon->z_max = drw->maxzoom;
          rule->polygon->style = NULL;
          // TODO support pattern
        }
      else if (drw->type == LINE)
        {
          if (line_seen) {
            /* line with border */
            rule->border = memphis_rule_attr_new ();
            rule->border->color_red = rule->line->color_red;
            rule->border->color_green = rule->line->color_green;
            rule->border->color_blue = rule->line->color_blue;
            rule->border->color_alpha = rule->line->color_alpha;
            rule->border->size = (rule->line->size - drw->width) * 0.5;
            rule->border->z_min = rule->line->z_min;
            rule->border->z_max = rule->line->z_max;
            rule->border->style = NULL;

            rule->line->color_red = drw->color[0];
            rule->line->color_green = drw->color[1];
            rule->line->color_blue = drw->color[2];
            rule->line->color_alpha = 255;
            rule->line->size = drw->width;
            rule->line->z_min = drw->minzoom;
            rule->line->z_max = drw->maxzoom;
            rule->line->style = NULL;
            
          } else {
            /* only a single line */
            rule->line = memphis_rule_attr_new ();
            rule->line->color_red = drw->color[0];
            rule->line->color_green = drw->color[1];
            rule->line->color_blue = drw->color[2];
            rule->line->color_alpha = 255;
            rule->line->size = drw->width;
            rule->line->z_min = drw->minzoom;
            rule->line->z_max = drw->maxzoom;
            rule->line->style = NULL;
            line_seen = TRUE;
          }
        }
      else if (drw->type == TEXT)
        {
          rule->text = memphis_rule_attr_new ();
          rule->text->color_red = drw->color[0];
          rule->text->color_green = drw->color[1];
          rule->text->color_blue = drw->color[2];
          rule->text->color_alpha = 255;
          rule->text->size = drw->width;
          rule->text->z_min = drw->minzoom;
          rule->text->z_max = drw->maxzoom;
          rule->text->style = NULL;
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
  GStringChunk *string_chunk = memphis_data_pool_get_string_chunk (pool);
  GTree *string_tree = memphis_data_pool_get_string_tree (pool);
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
      new->value[c] = m_string_chunk_get(string_chunk,
                                         string_tree, tmp);
      g_free(tmp);
    }
  new->key = g_strdupv (rule->keys);
  len = g_strv_length(new->key);
  for(c = 0; c < len; c++)
    {
      char *tmp = new->key[c];
      new->key[c] = m_string_chunk_get(string_chunk,
                                       string_tree, tmp);
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

/**
 * memphis_rule_set_get_rule:
 * @rules: a #MemphisRuleSet
 * @id: an id string
 *
 * Returns: (nullable): a #MemphisRule that has the given id string or %NULL otherwise.
 * 
 * Since: 0.1
 */
MemphisRule *
memphis_rule_set_get_rule (MemphisRuleSet *self, const gchar *id)
{
  g_return_val_if_fail (MEMPHIS_IS_RULE_SET (self) && id != NULL, NULL);

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

/**
 * memphis_rule_set_set_rule:
 * @rules: a #MemphisRuleSet
 * @rule: a #MemphisRule
 *
 * Adds the given rule to the rule set.
 * 
 * Since: 0.1
 */
void
memphis_rule_set_set_rule (MemphisRuleSet *self, MemphisRule *rule)
{
  g_return_if_fail (MEMPHIS_IS_RULE_SET (self) && MEMPHIS_RULE (rule));

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

      /* Add new cfgDraw's */
      add_new_cfgDraws (res, rule);
    }
  else
    {
      /* Append cfgRule at last position */
      cfgRule *curr = self->ruleset->rule;
      while (curr->next != NULL)
        curr = curr->next;

      cfgRule *new = cfgRule_new_from_rule (rule);

      curr->next = new;
      self->ruleset->cntRule++;
    }
}

/**
 * memphis_rule_set_remove_rule:
 * @rules: a #MemphisRuleSet
 * @id: an id string
 *
 * Removes the rule with the given id from the rules set.
 *
 * Returns: true if the rule could be found and removed.
 * 
 * Since: 0.1
 */
gboolean
memphis_rule_set_remove_rule (MemphisRuleSet *self, const gchar *id)
{
  g_return_val_if_fail (MEMPHIS_IS_RULE_SET (self) && id != NULL, FALSE);

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
  cfgRule *curr = self->ruleset->rule;
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
  self->ruleset->cntRule--;
  return TRUE;
}

/* private shared functions */

cfgRules *
memphis_rule_set_get_cfgRules (MemphisRuleSet *self)
{
  g_assert (self != NULL);

  return self->ruleset;
}
