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
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "../ruleset.h"
#include "../main.h"

memphisOpt  *opts;
GTree       *keyStrings;
GTree       *valStrings;
GTree       *patternStrings;

gint g_strcmp(gconstpointer  a, gconstpointer  b) {
    return strcmp((char *)a,(char *)b);
}

gboolean g_freeTree (gpointer key, gpointer value, gpointer data) {
    g_free(key);
    return FALSE;
}

int main () {
    
    opts = malloc(sizeof(memphisOpt));
    opts->debug=1;
    
    
    keyStrings = g_tree_new(g_strcmp);
    valStrings = g_tree_new(g_strcmp);
    patternStrings = g_tree_new(g_strcmp);
    
    cfgRules *ruleset = (cfgRules *) rulesetRead("test/ruleset.xml");
        
    g_tree_destroy(keyStrings);
    g_tree_destroy(valStrings);
    g_tree_destroy(patternStrings);
    
    // Free
    keyStrings = g_tree_new(g_strcmp);
    valStrings = g_tree_new(g_strcmp);
    patternStrings = g_tree_new(g_strcmp);
    
    rulesetFree(ruleset);
    
    g_tree_foreach(keyStrings, g_freeTree,NULL);
    g_tree_foreach(valStrings, g_freeTree,NULL);
    g_tree_foreach(patternStrings, g_freeTree,NULL);
    
    g_tree_destroy(keyStrings);
    g_tree_destroy(valStrings);
    g_tree_destroy(patternStrings);
		
	return (0);
}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */
