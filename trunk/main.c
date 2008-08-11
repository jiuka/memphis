#include <stdio.h>
#include "ll.h"
#include "main.h"
#include "osm05.h"
#include "config02.h"
#include <unistd.h>

int main(int argc, char **argv) {
    debug = 0;
    
    int i;
    for (i = 1; i < argc ; i++) {
	   if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--verbose"))) {
	       debug++;
	   }
	}
	
	fprintf(stdout,"Memphis OSM Renderer\n");
	
	fprintf(stdout," Read Config\n");
	cfgLayer *config;
	config = cfgRead(argv[argc-2]);

	fprintf(stdout," Read Source\n");
	osmFile *osmSrc;
	osmSrc = osmRead(argv[argc-1]);

	fprintf(stdout," Cairo Draw Tiles\n");
    drawCairo2(config, osmSrc);
    
    cfgFree(config);
    
    return(0);
}