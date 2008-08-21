/*
 * Memphis - Cairo Rederer for OSM in C
 * Copyright (C) 2008  <marius.rieder@durchmesser.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <expat.h>

#include <sys/resource.h>

#include "main.h"
#include "osm05.h"
#include "renderer.h"
#include "ruleset.h"
#include "list.h"

// Global Vars
GTree       *keyStrings;
GTree       *valStrings;
GTree       *patternStrings;

/* Renderer Options */
static memphisOpt opts_storage = {
    .debug = 1,
    .cfgfn = NULL,
    .osmfn = NULL,
    .mode = MODE_MAP,
    .minlayer = MEMPHIS_MIN_LAYER,
    .maxlayer = MEMPHIS_MAX_LAYER,
};
memphisOpt  *opts = &opts_storage;

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
};



gint g_strcmp(gconstpointer  a, gconstpointer  b) {
    return strcmp((char *)a,(char *)b);
}

gboolean g_freeTree (gpointer key, gpointer value, gpointer data) {
    g_free(key);
    return FALSE;
}

void banner() {
    fprintf(stdout,"Memphis OSM Renderer " MEMPHIS_VERSION "\n");
}

void usage(char *prog) {
    banner();
    fprintf(stdout,"%s [-v|-q] [-m|-t] <configfile> <datafile>\n", prog);
}

int main(int argc, char **argv) {
    
    cfgRules *ruleset;
    osmFile *osm;

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
            "memphis [-qvmt] [--minlayer] [--maxlayer] <RULESFILE> <OSMFILE>");
    g_option_context_add_group(optctx, grp);
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

    keyStrings = g_tree_new(g_strcmp);
    valStrings = g_tree_new(g_strcmp);
    patternStrings = g_tree_new(g_strcmp);

    banner();

    ruleset = (cfgRules *) rulesetRead(opts->cfgfn);
    if(ruleset == NULL)
        return(-1);

    osm = (osmFile *) osmRead(opts->osmfn);
    if(ruleset == NULL)
        return(-1);
        
    g_tree_destroy(keyStrings);
    g_tree_destroy(valStrings);
    g_tree_destroy(patternStrings);

    renderCairo(ruleset, osm);
    
    // Free
    keyStrings = g_tree_new(g_strcmp);
    valStrings = g_tree_new(g_strcmp);
    patternStrings = g_tree_new(g_strcmp);
    
    osmFree(osm);
    rulesetFree(ruleset);
    
    g_tree_foreach(keyStrings, g_freeTree,NULL);
    g_tree_foreach(valStrings, g_freeTree,NULL);
    g_tree_foreach(patternStrings, g_freeTree,NULL);
    
    g_tree_destroy(keyStrings);
    g_tree_destroy(valStrings);
    g_tree_destroy(patternStrings);

    return(0);
}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

