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

#include "../memphis.h"

#define RESOLUTION 256

int main () {
  g_type_init ();
  
  MemphisRenderer *r;
  MemphisRuleSet *rules;
  MemphisMap *map;
  cairo_surface_t *surface;
  cairo_t *cr;
  gint i, j, maxx, maxy;
  gchar *path;

  rules = memphis_rule_set_new ();
  memphis_rule_set_load_from_file (rules, "rule.xml");
  
  map = memphis_map_new_from_file ("map.osm");

  r = memphis_renderer_new_full (rules, map);
  memphis_renderer_set_resolution (r, RESOLUTION);
  memphis_renderer_set_zoom_level (r, 14);
  memphis_renderer_set_debug_level (r, 1);
  g_print ("Tile resolution: %u\n", memphis_renderer_get_resolution (r));

  maxx = memphis_renderer_get_max_x_tile (r);
  maxy = memphis_renderer_get_max_y_tile (r);
  i = memphis_renderer_get_min_x_tile (r);
  
  for (; i <= maxx; i++)
    {
      j = memphis_renderer_get_min_y_tile (r);
      for (; j <= maxy; j++)
        {
          surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
              RESOLUTION, RESOLUTION);
          cr =  cairo_create(surface);
          path = g_strdup_printf ("tiles/%i_%i.png", i, j);

          g_print ("Drawing tile: %i, %i\n", i, j);
          memphis_renderer_draw_tile (r, cr, i, j);
          cairo_surface_write_to_png(surface, path);
          
          g_free (path);
          cairo_destroy(cr);
          cairo_surface_destroy(surface);
        }
    }
  
  memphis_renderer_free (r);
  memphis_map_free (map);
  memphis_rule_set_free (rules);

  return 0;
}
