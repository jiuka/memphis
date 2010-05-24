/*
 * An example how to use Memphis in the Vala programming language.
 *
 * Compile command:
 * valac --pkg cairo --pkg memphis-0.2 --vapidir=../ example.vala
 *
 * Author: Simon Wenner
 * License: Public Domain
 */

using GLib;
using Memphis;
using Cairo;

int main ()
{
	/* load map data */
	var map = new Memphis.Map ();
	try {
		map.load_from_file ("../../../demos/map.osm");
	} catch (GLib.Error e) {
		GLib.warning (e.message);
	}

	/* load a drawing rules file */
	var rules = new Memphis.RuleSet ();
	try {
		rules.load_from_file ("../../../demos/rule.xml");
	} catch (GLib.Error e) {
		GLib.warning (e.message);
	}

	/* setup the rendering */
	var renderer = new Memphis.Renderer.full (rules, map);
	var surface = new Cairo.ImageSurface (Cairo.Format.ARGB32, 256, 256);
	var cr = new Cairo.Context (surface);

	/* draw one tile as a PNG image */
	renderer.draw_tile (cr, 8608, 5754, 14);
	surface.write_to_png ("vala_test.png");

	return 0;
}
