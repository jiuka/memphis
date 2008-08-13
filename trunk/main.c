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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "osm05.h"
#include "renderer.h"
#include "ruleset.h"
#include "strlist.h"

// Global Vars
int         debug;
strList     *keyStrings;
strList     *valStrings;
strList     *patternStrings;

void banner() {
	fprintf(stdout,"Memphis OSM Renderer\n");
}

void usage(char *prog) {
    banner();
	fprintf(stdout,"%s [-v] <configfile> <datafile>\n", prog);
}

int main(int argc, char **argv) {
    debug = 1;
    char *cfgfn = NULL;
    char *osmfn = NULL;
   
    int i;
    for (i = 1; i < argc ; i++) {
        if ((!strcmp(argv[i], "-q")) || (!strcmp(argv[i], "--quiet"))) {
            debug--;
        } else if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--verbose"))) {
            debug++;
        } else if (cfgfn == NULL) {
            cfgfn = argv[i];
        } else if (osmfn == NULL) {
            osmfn = argv[i];
        } else {
            usage((char *) *argv);
            exit(-1);
        }
    }
    
    if (argc < 2 || cfgfn == NULL || osmfn == NULL) {
        usage((char *) *argv);
        exit(-1);
    }
                    	
	banner();
	
	cfgRules *ruleset;
	ruleset = (cfgRules *) rulesetRead(cfgfn);

	osmFile *osm;
	osm = (osmFile *) osmRead(osmfn);
	
	renderCairo(ruleset, osm);
    
    return(0);
}