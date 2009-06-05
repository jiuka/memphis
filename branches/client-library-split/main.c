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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <sys/resource.h>

#include "main.h"
#include "renderer.h"
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

static int renderCairo(cfgRules *ruleset, osmFile *osm, gint8 debug_level) {
    if (debug_level > 1)
        fprintf(stdout,"renderCairo\n");
    int z;
    renderInfo *info;
    
    // Initialize all layers
    for (z = 0; z <= (opts->maxlayer - opts->minlayer); z++) {
        coordinates min, max;
        
        info = g_new(renderInfo, 1);
        info->zoom = z + opts->minlayer;
        info->ruleset = ruleset;
        info->osm = osm;
                
        min = coord2xy(osm->minlat, osm->minlon, info->zoom);
        max = coord2xy(osm->maxlat, osm->maxlon, info->zoom);
        int w = (int)ceil(max.x-min.x);
        int h = (int)ceil(min.y-max.y);

        info->offset = coord2xy(osm->maxlat, osm->minlon, info->zoom);

        info->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,w,h);
        info->cr = cairo_create(info->surface);

        cairo_rectangle(info->cr, 0, 0, w, h);
        cairo_set_source_rgb(info->cr,
                                    (double)ruleset->background[0]/(double)255,
                                    (double)ruleset->background[1]/(double)255,
                                    (double)ruleset->background[2]/(double)255);
        cairo_fill(info->cr);
        
        renderCairoRun(info, debug_level);
        
        // Saving Images
        char *filename;

        filename = g_strdup_printf("%s/%02i.png", opts->outdir, info->zoom);
        if (debug_level > 0) {
            fprintf(stdout," Cairo rendering Z%i to '%s'", info->zoom, filename);
            fflush(stdout);
        }
        cairo_surface_write_to_png(info->surface, filename);
        g_free(filename);
        cairo_destroy(info->cr);
        cairo_surface_destroy(info->surface);
        if (debug_level > 0)
            fprintf(stdout," done.\n");
            
        g_free(info);
    }
    
    return (0);
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
    fprintf(stdout,"Memphis OSM Renderer " MEMPHIS_VERSION "\n");
}

int main(int argc, char **argv) {

    g_type_init ();

    MemphisRuleSet *ruleset;
    MemphisMap *osm;

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
        exit(-1);
    }

    g_option_context_free(optctx);

    banner();

    ruleset = memphis_rule_set_new_from_file (opts->cfgfn);
    if(ruleset == NULL)
        return(-1);

    osm = memphis_map_new_from_file (opts->osmfn);
    if(osm == NULL)
        return(-1);

    renderCairo(ruleset->ruleset, osm->map, opts->debug);

    memphis_map_free (osm);
    memphis_rule_set_free (ruleset);

    return(0);
}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

