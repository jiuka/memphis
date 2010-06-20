#!/usr/bin/env seed

/* An example how to use Memphis' gobject-introspection data in
 * javascript using Seed.
 *
 * If you installed Memphis in /usr/local you have to export this path:
 * export GI_TYPELIB_PATH=$GI_TYPELIB_PATH:/usr/local/lib/girepository-1.0/
 *
 * Author: Simon Wenner
 * License: Public Domain
 */

const Memphis = imports.gi.Memphis;
const Cairo = imports.cairo;

var map = new Memphis.Map ();
map.load_from_file ("map.osm");

var rules = new Memphis.RuleSet ();
rules.load_from_file ("default-rules.xml");

var r = new Memphis.Renderer.full (rules, map);
r.set_resolution (256);

var surface = new Cairo.ImageSurface (Cairo.Format.ARGB32, 256, 256);
var cr = new Cairo.Context (surface);

/* cairo test */
cr.scale(256, 256);
cr.set_source_rgb (0, 0, 0);
cr.rectangle (0.0, 0.0, 1.0, 1.0);
cr.stroke ();

Seed.print ("Map tiles at zoom level 14: " + r.get_row_count (14) + " x " + r.get_column_count (14));
var x = 8608;
var y = 5754;

Seed.print ("Draw tile (" + x + ", " + y + ")...")
// FIXME: Segfaults with Seed 2.28.1
//r.draw_tile (cr, x, y, 14);
surface.write_to_png ("javascript_test.png");

cr.destroy ();

Seed.print ("Done.")
