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
#include <expat.h>

#include "main.h"
#include "osm05.h"
#include "renderer.h"
#include "ruleset.h"
#include "strlist.h"

// Global Vars
strList     *keyStrings;
strList     *valStrings;
strList     *patternStrings;
memphisOpt  *opts;

void banner() {
	fprintf(stdout,"Memphis OSM Renderer\n");
}

void usage(char *prog) {
    banner();
	fprintf(stdout,"%s [-v|-q] [-m|-t <X> <Y>] <configfile> <datafile>\n", prog);
}

int main(int argc, char **argv) {    
    cfgRules *ruleset;
    osmFile *osm;
    
    opts = malloc(sizeof(memphisOpt));
    opts->debug = 1;
    opts->cfgfn = NULL;
    opts->osmfn = NULL;
    opts->mode = MODE_MAP;
    opts->minlayer = 12;
    opts->maxlayer = 17;
   
    int i;
    for (i = 1; i < argc ; i++) {
        if ((!strcmp(argv[i], "-q")) || (!strcmp(argv[i], "--quiet"))) {
            opts->debug--;
        } else if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--verbose"))) {
            opts->debug++;
        } else if ((!strcmp(argv[i], "-m")) || (!strcmp(argv[i], "--map"))) {
            opts->mode = MODE_MAP;
        } else if ((!strcmp(argv[i], "-t")) || (!strcmp(argv[i], "--tile"))) {
            if(sscanf(argv[i+1],"%hi",&opts->tile_x)!=1 ||
               sscanf(argv[i+2],"%hi",&opts->tile_y)!=1 ) {
                usage((char *) *argv);
                exit(-1);
            }
            i+=2;
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
    
    keyStrings = strlist_init();
    valStrings = strlist_init();
    patternStrings = strlist_init();
                    	
    banner();
    
    ruleset = (cfgRules *) rulesetRead(opts->cfgfn);
    if(ruleset == NULL)
        return(-1);

    osm = (osmFile *) osmRead(opts->osmfn);
    if(ruleset == NULL)
        return(-1);
    
    renderCairo(ruleset, osm);
    
    return(0);
}

