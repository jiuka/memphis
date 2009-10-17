/*
 * Memphis - Cairo Rederer for OSM in C
 * Copyright (C) 2008  Marius Rieder <marius.rieder@durchmesser.ch>
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

#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "main.h"
#include "memphis.h"

/* Renderer Options */
static memphisOpt opts_storage = {
    .debug = 1,
    .mode = MODE_MAP,
    .minlayer = MEMPHIS_MIN_LAYER,
    .maxlayer = MEMPHIS_MAX_LAYER,
    .cfgfn = NULL,
    .osmfn = NULL,
    .outdir = MEMPHIS_DEFAULT_OUTPUT_DIRECTORY,
};
memphisOpt  *opts = &opts_storage;

static int draw (MemphisRenderer *renderer) {
  gint z, zoom;
  gchar *filename;

  // Initialize all layers 
  for (z = 0; z <= (opts->maxlayer - opts->minlayer); z++) {
    zoom = z + opts->minlayer;
    // Save Images
    filename = g_strdup_printf ("%s/%02i.png", opts->outdir, zoom);
    memphis_renderer_draw_png (renderer, filename, zoom);
    g_free (filename);
  }

  return 0;
}

static gboolean set_verbosity_level(const gchar *option_name,
                                    const gchar *value,
                                    gpointer data,
                                    GError **error)
{
    memphisOpt *o = data;
    if ((strcmp(option_name, "-q") == 0) ||
        (strcmp(option_name, "--quiet") == 0))
    {
        --o->debug;
    }
    else if ((strcmp(option_name, "-v") == 0) ||
             (strcmp(option_name, "--verbose") == 0))
    {
        ++o->debug;
    }
    return TRUE;
}

static gboolean set_layer_option_cb(const gchar *option_name,
                                    const gchar *value,
                                    gpointer data,
                                    GError **error)
{
    memphisOpt *o = data;
    int v = atoi(value);
    v = CLAMP(v, MEMPHIS_MIN_LAYER, MEMPHIS_MAX_LAYER);
    if (strcmp(option_name, "--minlayer") == 0)
        o->minlayer = v;
    else if (strcmp(option_name, "--maxlayer") == 0)
        o->maxlayer = v;
    else
        g_error("Unhandled option name: %s", option_name);
    return TRUE;
}

static gboolean set_map_mode(const gchar *option_name,
                             const gchar *value,
                             gpointer data,
                             GError **error)
{
    memphisOpt *o = data;
    o->mode = MODE_MAP;
    return TRUE;
}

/* Option parser */
static GOptionEntry memphis_option_entries[] = {
    { "quiet", 'q',     G_OPTION_FLAG_NO_ARG,
                        G_OPTION_ARG_CALLBACK, set_verbosity_level,
                        "Be quiet", NULL },
    { "verbose", 'v',   G_OPTION_FLAG_NO_ARG,
                        G_OPTION_ARG_CALLBACK, set_verbosity_level,
                        "Be verbose", NULL },
    { "map", 'm',       G_OPTION_FLAG_NO_ARG, 
                        G_OPTION_ARG_CALLBACK, set_map_mode,
                        "Set map mode", NULL },
    { "minlayer", 0,    0,
                        G_OPTION_ARG_CALLBACK, set_layer_option_cb,
                        "minimum layer to render", "LAYER" },
    { "maxlayer", 0,    0,
                        G_OPTION_ARG_CALLBACK, set_layer_option_cb,
                        "maximum layer to render", "LAYER" },
    { "out", 'o',       0,
                        G_OPTION_ARG_FILENAME, &opts_storage.outdir,
                        "tiles output directory (default: tiles)", "DIR" },
};

static void banner() {
    g_print("Memphis OSM Renderer " MEMPHIS_VERSION "\n");
}

int main(int argc, char **argv) {

    g_type_init ();

    MemphisRuleSet *ruleset;
    MemphisMap *osm;
    MemphisRenderer *renderer;
    GError *error = NULL;
    GOptionContext *optctx;
    GOptionGroup *grp;

    grp = g_option_group_new("memphis",
                             "memphis renderer options",
                             "memphis renderer options",
                             opts, NULL);
    g_option_group_add_entries(grp, memphis_option_entries);
    optctx = g_option_context_new("Memphis OSM Renderer " MEMPHIS_VERSION);
    g_option_context_set_summary(optctx,
            "memphis [-qvmt] [--minlayer] [--maxlayer] [-o DIR] <RULESFILE> <OSMFILE>");
    g_option_context_set_main_group(optctx, grp);
    if (!g_option_context_parse(optctx, &argc, &argv, &error)) {
        g_print("option parsing failed: %s\n", error->message);
        return 1;
    }

    if (argc > 1)
        opts->cfgfn = argv[1];
    if (argc > 2)
        opts->osmfn = argv[2];

    if (opts->cfgfn == NULL || opts->osmfn == NULL) {
        g_print("error: rules file or osm map file missing:\n\n%s\n",
                g_option_context_get_summary(optctx));
        return(-1);
    }

    g_option_context_free(optctx);

    banner();

    ruleset = memphis_rule_set_new ();
    memphis_rule_set_load_from_file (ruleset, opts->cfgfn);
    //if (ruleset->ruleset == NULL)
    //    return(-1);

    osm = memphis_map_new ();
    memphis_map_load_from_file (osm, opts->osmfn);

    //if (osm->map == NULL)
    //    return(-1);

    renderer = memphis_renderer_new_full (ruleset, osm);
    memphis_renderer_set_debug_level (renderer, opts->debug);
    
    draw (renderer);

    memphis_map_free (osm);
    memphis_rule_set_free (ruleset);
    memphis_renderer_free (renderer);

    return(0);
}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

