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

#include "../src/memphis.h"
#include <string.h>

#define RESOLUTION 512
#define RULES_PATH "../demos/rule.xml"
#define MAP_PATH "../demos/map.osm"
#define INVALID_XML_FILE "../README"

/* MemphisMap */

static void
map_new ()
{
  MemphisMap *map = NULL;
  map = memphis_map_new ();
  g_assert (MEMPHIS_IS_MAP (map));
  memphis_map_free (map);
}

static void
map_load_data ()
{
  MemphisMap *map;

  const gchar map_data[] =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
  <osm version=\"0.6\" generator=\"OpenStreetMap server\">\
  <bounds minlat=\"47.68779\" minlon=\"8.6199\" \
      maxlat=\"47.71121\" maxlon=\"8.65136\"/>\
  <node id=\"26038696\" lat=\"47.6928564\" lon=\"8.6483791\" \
      version=\"1\" changeset=\"220413\" user=\"cdw\" uid=\"5297\" \
      visible=\"true\" \timestamp=\"2007-02-20T16:01:03Z\">\
  <tag k=\"created_by\" v=\"JOSM\"/>\
  </node>\
  </osm>";

  map = memphis_map_new ();
  memphis_map_set_debug_level (map, 0);
  memphis_map_load_from_data (map, map_data, strlen (map_data));
  memphis_map_free (map);
}

static void
map_load_file1 ()
{
  MemphisMap *map;
  map = memphis_map_new ();
  memphis_map_set_debug_level (map, 0);
  memphis_map_load_from_file (map, MAP_PATH);
  memphis_map_free (map);
}

static void
map_load_file2 ()
{
  MemphisMap *map;
  map = memphis_map_new ();
  memphis_map_set_debug_level (map, 0);
  //memphis_map_load_from_file (map, INVALID_XML_FILE);
  // FIXME: syntax erros should not exit the lib but return a GError
  g_test_message ("FIXME: XML syntax error exits libmemphis.");
  memphis_map_free (map);
}

/* MemphisRule */

static void
rule_new ()
{
  MemphisRule *rule = NULL;
  rule = memphis_rule_new ();
  g_assert (MEMPHIS_IS_RULE (rule));
  memphis_rule_free (rule);
}

/* MemphisRuleSet */

static void
rule_set_new ()
{
  MemphisRuleSet *rules = NULL;
  rules = memphis_rule_set_new ();
  g_assert (MEMPHIS_IS_RULE_SET (rules));
  memphis_rule_set_free (rules);
}

static void
rule_set_load_data ()
{
  MemphisRuleSet *rules;

  const gchar rules_data[] =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
  <rules version=\"0.1\" autor=\"MariusRieder\" background=\"#f0f0f0\">\
      <rule e=\"way\" k=\"highway\" v=\"*\">\
          <line color=\"#000000\" width=\"1.0\"/>\
      </rule>\
  </rules>";

  rules = memphis_rule_set_new ();
  memphis_rule_set_set_debug_level (rules, 0);
  memphis_rule_set_load_from_data (rules, rules_data, strlen (rules_data));
  memphis_rule_set_free (rules);
}

static void
rule_set_load_file1 ()
{
  MemphisRuleSet *rules;
  rules = memphis_rule_set_new ();
  memphis_rule_set_set_debug_level (rules, 0);
  memphis_rule_set_load_from_file (rules, RULES_PATH);
  memphis_rule_set_load_from_file (rules, RULES_PATH);
  memphis_rule_set_free (rules);
}

static void
rule_set_bg ()
{
  MemphisRuleSet *rules;
  guint16 r1, g1, b1;
  guint16 r2, g2, b2;
  r2 = g2 = b2 = 0;
  rules = memphis_rule_set_new ();
  memphis_rule_set_set_debug_level (rules, 0);
  memphis_rule_set_load_from_file (rules, RULES_PATH);
  // FIXME: should work without a file!
  g_test_message ("FIXME: should work without a file!");
  r1 = (guint16) g_test_rand_int ();
  g1 = (guint16) g_test_rand_int ();
  b1 = (guint16) g_test_rand_int ();
  memphis_rule_set_set_bg_color (rules, r1, g1, b1);
  memphis_rule_set_get_bg_color (rules, &r2, &g2, &b2);
  g_assert_cmpuint(r1, ==, r2);
  g_assert_cmpuint(g1, ==, g2);
  g_assert_cmpuint(b1, ==, b2);
  memphis_rule_set_free (rules);
}

static void
rule_set_set_get_rule ()
{
  MemphisRule *rule;
  MemphisRuleSet *rules;
  rules = memphis_rule_set_new ();
  memphis_rule_set_set_debug_level (rules, 0);
  memphis_rule_set_load_from_file (rules, RULES_PATH);

  // one
  rule = memphis_rule_set_get_rule (rules,
      "natural:water|coastline"); // not available
  g_assert (rule == NULL);

  // two
  rule = memphis_rule_set_get_rule (rules,
      "highway:motorway");

  g_assert (MEMPHIS_IS_RULE (rule));
  g_assert_cmpstr (rule->keys[0], ==, "highway");
  g_assert_cmpstr (rule->values[0], ==, "motorway");

  memphis_rule_free (rule);

  // three
  rule = memphis_rule_set_get_rule (rules,
      "landuse|natural:wood|forest");

  g_assert (MEMPHIS_IS_RULE (rule));
  g_assert_cmpstr (rule->keys[0], ==, "landuse");
  g_assert_cmpstr (rule->keys[1], ==, "natural");
  g_assert_cmpstr (rule->values[0], ==, "wood");
  g_assert_cmpstr (rule->values[1], ==, "forest");

  rule->line_size = 10.0;
  rule->line_color[0] = 255;
  rule->line_color[1] = 0;
  rule->line_color[2] = 0;
  rule->border_size = 12.0;
  rule->text_z[0] = 13;
  rule->text_size = 9.0;
  memphis_rule_set_set_rule (rules, rule);
  memphis_rule_free (rule);
  memphis_rule_set_free (rules);
}

static void
rule_set_rm_rule ()
{
  MemphisRuleSet *rules;
  rules = memphis_rule_set_new ();
  memphis_rule_set_set_debug_level (rules, 0);
  memphis_rule_set_load_from_file (rules, RULES_PATH);

  g_assert (memphis_rule_set_remove_rule (rules, "highway:motorway"));
  g_assert (memphis_rule_set_remove_rule (rules, "highway:primary"));
  g_assert (!memphis_rule_set_remove_rule (rules, "natural:water|coastline"));

  memphis_rule_set_free (rules);
}

/* MemphisRenderer */

static void
renderer_new ()
{
  MemphisRenderer *r = NULL;
  r = memphis_renderer_new_full (NULL, NULL);
  g_assert (MEMPHIS_IS_RENDERER (r));
  memphis_renderer_free (r);

  r = NULL;
  r = memphis_renderer_new ();
  g_assert (MEMPHIS_IS_RENDERER (r));
  memphis_renderer_free (r);
}

static void
renderer_resolution ()
{
  MemphisRenderer *r = NULL;
  r = memphis_renderer_new ();
  guint res = g_test_rand_int_range (0, 100000);
  memphis_renderer_set_resolution (r, res);
  g_assert_cmpint (memphis_renderer_get_resolution (r), ==, res);
  memphis_renderer_free (r);
}

static void
renderer_draw_nothing ()
{
  MemphisRenderer *r;
  cairo_surface_t *surface;
  cairo_t *cr;

  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, RESOLUTION, RESOLUTION);
  cr =  cairo_create(surface);
  g_assert (surface != NULL);
  g_assert (cr != NULL);

  r = memphis_renderer_new ();
  memphis_renderer_set_debug_level (r, 0);
  memphis_renderer_draw_tile (r, cr, 1, 1, 12);

  g_assert (memphis_renderer_tile_has_data (r, 1, 1, 12) == FALSE);

  cairo_destroy(cr);
  cairo_surface_destroy(surface);

  memphis_renderer_free (r);
}

static void
renderer_draw_empty_map_and_rules ()
{
  MemphisRenderer *r;
  MemphisRuleSet *rules;
  MemphisMap *map;
  cairo_surface_t *surface;
  cairo_t *cr;

  rules = memphis_rule_set_new ();
  map = memphis_map_new ();

  r = memphis_renderer_new_full (rules, map);
  memphis_renderer_set_debug_level (r, 0);
  g_assert (r != NULL);

  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, RESOLUTION, RESOLUTION);
  cr =  cairo_create(surface);
  g_assert (surface != NULL);
  g_assert (cr != NULL);

  memphis_renderer_draw_tile (r, cr, 1, 1, 12);

  g_assert (memphis_renderer_tile_has_data (r, 1, 1, 12) == FALSE);
  g_assert (memphis_renderer_get_min_x_tile (r, 12) == -1);
  g_assert (memphis_renderer_get_max_x_tile (r, 12) == -1);
  g_assert (memphis_renderer_get_min_y_tile (r, 12) == -1);
  g_assert (memphis_renderer_get_max_y_tile (r, 12) == -1);

  cairo_destroy(cr);
  cairo_surface_destroy(surface);

  memphis_renderer_free (r);
  memphis_map_free (map);
  memphis_rule_set_free (rules);
}

int
main (int argc, char **argv)
{
  g_type_init ();
  g_test_init (&argc, &argv, NULL);
  g_test_bug_base ("https://trac.openstreetmap.ch/trac/memphis/ticket/");

  g_test_add_func ("/map/new", map_new);
  g_test_add_func ("/map/load_data", map_load_data);
  g_test_add_func ("/map/load_file1", map_load_file1);
  g_test_add_func ("/map/load_file2", map_load_file2);

  g_test_add_func ("/rule/new", rule_new);

  g_test_add_func ("/rule_set/new", rule_set_new);
  g_test_add_func ("/rule_set/load_data", rule_set_load_data);
  g_test_add_func ("/rule_set/load_file1", rule_set_load_file1);
  //g_test_add_func ("/rule_set/load_file2", rule_set_load_file2);
  g_test_add_func ("/rule_set/bg", rule_set_bg);
  g_test_add_func ("/rule_set/set_get_rule", rule_set_set_get_rule);
  g_test_add_func ("/rule_set/rm_rule", rule_set_rm_rule);

  g_test_add_func ("/renderer/new", renderer_new);
  g_test_add_func ("/renderer/resolution", renderer_resolution);
  g_test_add_func ("/renderer/draw_nothing", renderer_draw_nothing);
  g_test_add_func ("/renderer/draw_empty_map_and_rules",
      renderer_draw_empty_map_and_rules);

  return g_test_run ();
}
