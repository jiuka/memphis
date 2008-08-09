#include <stdio.h>
#include "main.h"
#include "osm05.h"
#include "config01.h"
#include <unistd.h>

int main(int argc, char **argv) {
    debug = 0;
    
    int i;
    for (i = 1; i < argc ; i++) {
	   if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--verbose"))) {
	       debug++;
	   }
	}
	
	fprintf(stdout,"CCairoRenderer\n");
	
	fprintf(stdout," Read Config");
	fflush(stdout);
	cfgFile *config;
	config = cfgRead(argv[argc-2]);
	fprintf(stdout," Done\n");
	
	/*
	cfgWay *iw;
	iw = config->ways;
	while(iw != NULL) {
	   fprintf(stdout, "NODE %i (%i, %i, %i)\n",iw->type,iw->color[0],iw->color[1],iw->color[2]);
	   
	   cfgTag *it;
	   it = iw->tag;
	   while(it != NULL) {
	       fprintf(stdout, " %s => %s\n",it->key,it->value);
	       it = it->next;
	   }
	   
	   iw = iw->next;
	}*/
	
	fprintf(stdout," Read Source");
	fflush(stdout);
	osmFile *osmSrc;
	osmSrc = osmRead(argv[argc-1]);
	fprintf(stdout," Done\n");
	
	fprintf(stdout," Cairo Draw Images");
	fflush(stdout);
    drawCairo(config, osmSrc);
	fprintf(stdout," Done\n");
    	
	/*
	osmNode *in;
	in = nodes;
	while(in != NULL) {
	   fprintf(stdout, "NODE(%i)\n",in->id);
	   
	   osmTag *it;
	   it = in->tag;
	   while(it != NULL) {
	       fprintf(stdout, " %s => %s\n",it->key,it->value);
	       it = it->next;
	   }
	   
	   in = in->next;
	}
	osmWay *iw;
	iw = osmSrc->ways;
	while(iw != NULL) {
	   fprintf(stdout, "WAY(%i)\n",iw->id);
	   
	   osmTag *it;
	   it = iw->tag;
	   while(it != NULL) {
	       fprintf(stdout, " %s ==> %s\n",it->key,it->value);
	       it = it->next;
	   }
	   
	   osmNd *in;
	   in = iw->nd;
	   while(in != NULL) {
	       fprintf(stdout, " %f/%f\n",in->node->lat,in->node->lon);
	       in = in->next;
	   }
	   
	   iw = iw->next;
	}
	*/

    //return(0);
}