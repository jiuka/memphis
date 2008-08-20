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
memphisOpt  *opts;

gint g_strcmp(gconstpointer  a, gconstpointer  b) {
    return strcmp((char *)a,(char *)b);
}

gboolean g_freeTree (gpointer key, gpointer value, gpointer data) {
    g_free(key);
    return FALSE;
}

void banner() {
    fprintf(stdout,"Memphis OSM Renderer\n");
}

void usage(char *prog) {
    banner();
    fprintf(stdout,"%s [-v|-q] [-m|-t] <configfile> <datafile>\n", prog);
}

int main(int argc, char **argv) {
    
    cfgRules *ruleset;
    osmFile *osm;

    opts = g_new(memphisOpt, 1);
    opts->debug = 1;
    opts->cfgfn = NULL;
    opts->osmfn = NULL;
    opts->mode = MODE_MAP;
    opts->minlayer = MEMPHIS_MIN_LAYER;
    opts->maxlayer = MEMPHIS_MAX_LAYER;

    int i;
    for (i = 1; i < argc ; i++) {
        if ((!strcmp(argv[i], "-q")) || (!strcmp(argv[i], "--quiet"))) {
            opts->debug--;
        } else if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--verbose"))) {
            opts->debug++;
        } else if ((!strcmp(argv[i], "-m")) || (!strcmp(argv[i], "--map"))) {
            opts->mode = MODE_MAP;
        } else if ((!strcmp(argv[i], "-t")) || (!strcmp(argv[i], "--tile"))) {
            opts->mode = MODE_TILE;
        } else if (!strcmp(argv[i], "--minlayer") && ((i + 1) < argc)) {
            opts->minlayer = atoi(argv[i + 1]);
            opts->minlayer = CLAMP(opts->minlayer,
                                   MEMPHIS_MIN_LAYER,
                                   MEMPHIS_MAX_LAYER);
            ++i;
        } else if (!strcmp(argv[i], "--maxlayer") && ((i + 1) < argc)) {
            opts->maxlayer = atoi(argv[i + 1]);
            opts->maxlayer = CLAMP(opts->maxlayer,
                                   MEMPHIS_MIN_LAYER,
                                   MEMPHIS_MAX_LAYER);
            ++i;
        } else if (opts->cfgfn == NULL) {
            opts->cfgfn = argv[i];
        } else if (opts->osmfn == NULL) {
            opts->osmfn = argv[i];
        } else {
            usage((char *) *argv);
            exit(-1);
        }
    }

    if (argc < 2 || opts->cfgfn == NULL || opts->osmfn == NULL) {
        usage((char *) *argv);
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

