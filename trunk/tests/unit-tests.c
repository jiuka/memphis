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
#include <math.h>

#define RESOLUTION 512
#define RULES_PATH "../demos/rule.xml"
#define MAP_PATH "../demos/map.osm"
#define EPS 0.0001

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

  //const gchar map_data[] ="sdsd"; // FIXME: no parse error?

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
  memphis_map_load_from_data (map, map_data, strlen (map_data));
  memphis_map_free (map);
}

static void
map_load_file ()
{
  MemphisMap *map;
  map = memphis_map_new ();
  memphis_map_load_from_file (map, MAP_PATH);
  memphis_map_free (map);
}

static void
map_free ()
{
  MemphisMap *map;
  map = memphis_map_new ();
  memphis_map_free (map);
}

/* MemphisRule */

static void
rule_new ()
{
  MemphisRule *rule = NULL;
  rule = memphis_rule_new ();
  g_assert (MEMPHIS_RULE (rule));
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
  memphis_rule_set_load_from_data (rules, rules_data, strlen (rules_data));
  memphis_rule_set_free (rules);

  const gchar rules_data2[] =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
  <rules version=\"0.1\" autor=\"MariusRieder\" background=\"#f0f0f000\">\
        <!-- no rules and an invisible background -->\
  </rules>";

  rules = memphis_rule_set_new ();
  memphis_rule_set_load_from_data (rules, rules_data2, strlen (rules_data2));
  memphis_rule_set_free (rules);
}

static void
rule_set_load_file ()
{
  MemphisRuleSet *rules;
  rules = memphis_rule_set_new ();
  memphis_rule_set_load_from_file (rules, RULES_PATH);
  memphis_rule_set_load_from_file (rules, RULES_PATH);
  memphis_rule_set_free (rules);
}

static void
rule_set_free ()
{
  MemphisRuleSet *rules;
  rules = memphis_rule_set_new ();
  memphis_rule_set_free (rules);
}

static void
rule_set_background ()
{
  MemphisRuleSet *rules;
  guint8 r1, g1, b1, a1;
  guint8 r2, g2, b2, a2;
  r2 = g2 = b2 = a2 = 0;
  rules = memphis_rule_set_new ();
  r1 = (guint16) g_test_rand_int ();
  g1 = (guint16) g_test_rand_int ();
  b1 = (guint16) g_test_rand_int ();
  a1 = (guint16) g_test_rand_int ();
  memphis_rule_set_set_bg_color (rules, r1, g1, b1, a1);
  memphis_rule_set_get_bg_color (rules, &r2, &g2, &b2, &a2);
  g_assert_cmpuint(r1, ==, r2);
  g_assert_cmpuint(g1, ==, g2);
  g_assert_cmpuint(b1, ==, b2);
  g_assert_cmpuint(a1, ==, a2);
  memphis_rule_set_free (rules);
}

static void
rule_set_get_rule ()
{
  MemphisRule *rule;
  MemphisRuleSet *rules;
  rules = memphis_rule_set_new ();
  memphis_rule_set_load_from_file (rules, RULES_PATH);

  // one
  rule = memphis_rule_set_get_rule (rules,
      "natural:water|coastline"); // not available
  g_assert (rule == NULL);

  // two
  rule = memphis_rule_set_get_rule (rules,
      "highway:motorway");

  g_assert (MEMPHIS_RULE (rule));
  g_assert_cmpstr (rule->keys[0], ==, "highway");
  g_assert_cmpstr (rule->values[0], ==, "motorway");

  memphis_rule_free (rule);

  // three
  rule = memphis_rule_set_get_rule (rules,
      "landuse|natural:wood|forest");

  g_assert (MEMPHIS_RULE (rule));
  g_assert_cmpstr (rule->keys[0], ==, "landuse");
  g_assert_cmpstr (rule->keys[1], ==, "natural");
  g_assert_cmpstr (rule->values[0], ==, "wood");
  g_assert_cmpstr (rule->values[1], ==, "forest");

  memphis_rule_free (rule);
  memphis_rule_set_free (rules);
}

static void
rule_set_set_and_get_line_w_border ()
{
  MemphisRule *rule;
  MemphisRuleSet *rules;
  rules = memphis_rule_set_new ();
  memphis_rule_set_load_from_file (rules, RULES_PATH);
  //FIXME: we need a real handy rules creation function, this is horrible

  guint8 r, g, b, a, zmin, zmax;
  gdouble size;
  r = g_test_rand_int_range (0, 255);
  g = g_test_rand_int_range (0, 255);
  b = g_test_rand_int_range (0, 255);
  a = g_test_rand_int_range (0, 255);
  zmin = g_test_rand_int_range (0, 255);
  zmax = g_test_rand_int_range (0, 255);
  size = g_test_rand_double_range (0.0, 1000.0);

  guint8 br, bg, bb, ba, bzmin, bzmax;
  gdouble bsize;
  br = g_test_rand_int_range (0, 255);
  bg = g_test_rand_int_range (0, 255);
  bb = g_test_rand_int_range (0, 255);
  ba = g_test_rand_int_range (0, 255);
  bzmin = g_test_rand_int_range (0, 255);
  bzmax = g_test_rand_int_range (0, 255);
  bsize = g_test_rand_double_range (0.0, 1000.0);

  rule = memphis_rule_set_get_rule (rules,
      "railway:rail");
  g_assert (MEMPHIS_RULE (rule));
  g_assert (rule->line != NULL);
  rule->line->color_red = r;
  rule->line->color_green = g;
  rule->line->color_blue = b;
  rule->line->color_alpha = a;
  rule->line->size = size;
  rule->line->z_min = zmin;
  rule->line->z_max = zmax;

  g_assert (rule->border != NULL);
  rule->border->color_red = br;
  rule->border->color_green = bg;
  rule->border->color_blue = bb;
  rule->border->color_alpha = ba;
  rule->border->size = bsize;
  rule->border->z_min = bzmin;
  rule->border->z_max = bzmax;

  memphis_rule_set_set_rule (rules, rule);
  memphis_rule_free (rule);

  rule = memphis_rule_set_get_rule (rules,
      "railway:rail");

  g_assert_cmpint (rule->line->color_red, ==, r);
  g_assert_cmpint (rule->line->color_green, ==, g);
  g_assert_cmpint (rule->line->color_blue, ==, b);
  //g_assert_cmpint (rule->line->color_alpha, ==, a); // TODO: support alpha
  g_assert_cmpint (rule->line->z_min, ==, zmin);
  g_assert_cmpint (rule->line->z_max, ==, zmax);
  g_assert (fabs (rule->line->size - size)  < EPS);

  g_assert_cmpint (rule->border->color_red, ==, br);
  g_assert_cmpint (rule->border->color_green, ==, bg);
  g_assert_cmpint (rule->border->color_blue, ==, bb);
  //g_assert_cmpint (rule->border->color_alpha, ==, ba);
  g_assert_cmpint (rule->border->z_min, ==, bzmin);
  g_assert_cmpint (rule->border->z_max, ==, bzmax);
  g_assert (fabs (rule->border->size - bsize)  < EPS);

  memphis_rule_free (rule);
  memphis_rule_set_free (rules);
}
/*
static void
rule_set_set_and_get_polygon_w_border ()
{
  MemphisRule *rule;
  MemphisRuleSet *rules;
  rules = memphis_rule_set_new ();
  memphis_rule_set_load_from_file (rules, RULES_PATH); //FIXME

  // TODO

  memphis_rule_free (rule);
  memphis_rule_set_free (rules);
}
*/
static void
rule_set_rm_rule ()
{
  MemphisRuleSet *rules;
  rules = memphis_rule_set_new ();
  memphis_rule_set_load_from_file (rules, RULES_PATH);

  g_assert (memphis_rule_set_remove_rule (rules, "highway:motorway"));
  g_assert (memphis_rule_set_remove_rule (rules, "highway:primary"));
  g_assert (!memphis_rule_set_remove_rule (rules, "natural:water|coastline"));

  memphis_rule_set_free (rules);
}

static void
rule_set_add_rule ()
{
  MemphisRuleSet *rules;
  MemphisRule *a_rule;
  MemphisRule *b_rule;

  rules = memphis_rule_set_new ();
  memphis_rule_set_load_from_file (rules, RULES_PATH);

  a_rule = memphis_rule_new ();
  a_rule->keys = g_strsplit ("highway", "|", -1);
  a_rule->values = g_strsplit ("test|test2", "|", -1);

  g_assert (a_rule->line == NULL);
  a_rule->line = g_new (MemphisRuleAttr, 1);
  a_rule->line->size = 5.0;
  a_rule->line->color_red = 255;
  a_rule->line->color_green = 0;
  a_rule->line->color_blue = 0;
  a_rule->line->style = NULL; // FIXME
  memphis_rule_set_set_rule (rules, a_rule);

  b_rule = memphis_rule_set_get_rule (rules, "highway:test|test2");
  g_assert (b_rule != NULL);

  // TODO: compare keys and values

  memphis_rule_free (a_rule);
  memphis_rule_free (b_rule);
  memphis_rule_set_free (rules);
}

// TODO: incomplete rules? conflicting rules? empty rule-set?

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
renderer_set_map ()
{
  MemphisMap *map, *map2;
  map = memphis_map_new ();
  g_assert (MEMPHIS_IS_MAP(map));
    
  MemphisRenderer *r = NULL;
  r = memphis_renderer_new ();
  memphis_renderer_set_map (r, map);
  map2 = memphis_renderer_get_map (r);
  g_assert (MEMPHIS_IS_MAP(map2));
  g_assert (map == map2);

  memphis_renderer_free (r);

  // full constructor
  r = memphis_renderer_new_full (NULL, map);
  g_assert (MEMPHIS_IS_RENDERER(r));
  map2 = memphis_renderer_get_map (r);
  g_assert (MEMPHIS_IS_MAP(map2));
  g_assert (map == map2);

  memphis_map_free (map);
  memphis_renderer_free (r);
}

static void
renderer_tile_numbers ()
{
  const gchar map_data[] =
  "<?xml version='1.0' encoding='UTF-8'?>\
  <osm version='0.5' generator='JOSM'>\
    <bounds minlat=\"51.4540069101\" minlon=\"-0.17578125\" \
            maxlat=\"51.5634123287\" maxlon=\"-0.0\"/>\
  </osm>";

  MemphisMap *map;
  map = memphis_map_new ();
  memphis_map_load_from_data (map, map_data, strlen (map_data));
  g_assert (MEMPHIS_IS_MAP(map));

  MemphisRenderer *r = NULL;
  r = memphis_renderer_new_full (NULL, map);
  g_assert (MEMPHIS_IS_RENDERER(r));

  gint x1, x2, y1, y2;
  x1 = memphis_renderer_get_min_x_tile (r, 14);
  x2 = memphis_renderer_get_max_x_tile (r, 14);
  y1 = memphis_renderer_get_min_y_tile (r, 14);
  y2 = memphis_renderer_get_max_y_tile (r, 14);

  g_assert (x1 == 8184);
  g_assert (x2 == 8192);
  g_assert (y1 == 5444);
  g_assert (y2 == 5452);

  memphis_renderer_free (r);
  memphis_map_free (map);
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
  g_test_add_func ("/map/free", map_free);
  g_test_add_func ("/map/load_data", map_load_data);
  g_test_add_func ("/map/load_file", map_load_file);

  g_test_add_func ("/rule/new", rule_new);

  g_test_add_func ("/rule_set/new", rule_set_new);
  g_test_add_func ("/rule_set/free", rule_set_free);
  g_test_add_func ("/rule_set/load_data", rule_set_load_data);
  g_test_add_func ("/rule_set/load_file", rule_set_load_file);
  g_test_add_func ("/rule_set/background", rule_set_background);
  g_test_add_func ("/rule_set/get_rule", rule_set_get_rule);
  g_test_add_func ("/rule_set/set_and_get_line_w_border",
      rule_set_set_and_get_line_w_border);
  //g_test_add_func ("/rule_set/set_and_get_polygon_w_border",
  //    rule_set_set_and_get_polygon_w_border);
  g_test_add_func ("/rule_set/rm_rule", rule_set_rm_rule);
  g_test_add_func ("/rule_set/add_rule", rule_set_add_rule);

  g_test_add_func ("/renderer/new", renderer_new);
  g_test_add_func ("/renderer/set_map", renderer_set_map);
  g_test_add_func ("/renderer/resolution", renderer_resolution);
  g_test_add_func ("/renderer/tile_numbers", renderer_tile_numbers);
  g_test_add_func ("/renderer/draw_nothing", renderer_draw_nothing);
  g_test_add_func ("/renderer/draw_empty_map_and_rules",
      renderer_draw_empty_map_and_rules);
  // TODO: huge drawing test, performance tests

  return g_test_run ();
}
