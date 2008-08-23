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
#include "../mlib.h"

memphisOpt   *opts;
GStringChunk *stringChunk;
GTree        *stringTree;

int main () {

    opts = malloc(sizeof(memphisOpt));
    opts->debug=1;

    stringChunk = g_string_chunk_new(265);
    stringTree = g_tree_new(m_tree_strcmp);

    cfgRules *ruleset = (cfgRules *) rulesetRead("test/ruleset.xml");

    g_tree_destroy(stringTree);

    rulesetFree(ruleset);

    g_string_chunk_free(stringChunk);

	return (0);
}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */
