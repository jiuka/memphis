#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "ll.h"
#include "main.h"
#include "osm05.h"
#include "libosm.h"
#include "config02.h"

int main(int argc, char **argv) {
    debug = 0;
    tiled = 0;
    char *cfgfn = NULL;
    char *osmfn = NULL;
    
    int i;
    for (i = 1; i < argc ; i++) {
	   if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--verbose"))) {
	       debug++;
	   } else if ((!strcmp(argv[i], "-t")) || (!strcmp(argv[i], "--tile"))) {
	       tiled ++;
	   } else if (cfgfn == NULL) {
	       cfgfn = malloc(strlen(argv[i])+1);
	       strncpy(cfgfn,argv[i], strlen((argv[i]))+1);
	   } else if (osmfn == NULL) {
	       osmfn = malloc(strlen(argv[i])+1);
	       strncpy(osmfn,argv[i], strlen(argv[i])+1);
	   }
	}
                    	
	fprintf(stdout,"Memphis OSM Renderer\n");
	
	fprintf(stdout," Read Config\n");
	cfgLayer *config;
	config = (cfgLayer *) cfgRead(cfgfn);

	fprintf(stdout," Read Source\n");
	osmFile *osmSrc;
	osmSrc = (osmFile *) osmRead(osmfn);

	fprintf(stdout," Cairo Draw Tiles\n");
    drawCairo(config, osmSrc);
    
    cfgFree(config);
    
    return(0);
}