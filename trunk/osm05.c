#include "libxml/parser.h"
#include "stdio.h"
#include <string.h>

#include "osm05.h"
#include "main.h"
#include "ll.h"

char string1[]="user";
char string2[]="timestamp";

// Pointers to work with
osmFile     *cFile = NULL;
osmTag      *cTag = NULL;
osmNode     *cNode = NULL;
osmNd       *cNd = NULL;
osmWay      *cWay = NULL;
osmBounds   *cBounds = NULL;

// Counts
int     cntTag = 0;
int     cntNode = 0;
int     cntWay = 0;
int     cntNd = 0;


/**
 * osm05startDocument:
 * @ctxt:  An XML parser context
 *
 * called when the document start has been detected.
 */
static void osm05startDocument(void *ctx) {
    if (debug > 1)
        fprintf(stdout,"osm05startDocument\n");
}

/**
 * osm05endDocument:
 * @ctxt:  An XML parser context
 *
 * called when the document end has been detected.
 */
static void osm05endDocument(void *ctx) {
    if (debug > 1)
        fprintf(stdout,"osm05endDocument\n");
    if (debug > 0)
        fprintf(stdout,"Finished Parsing\n Bounds: %f,%f,%f,%f\n Nodes: % 8i\n Tags:  % 8i\n Ways:  % 8i\n Nds:   % 8i\n", cBounds->minlat, cBounds->minlon, cBounds->maxlat, cBounds->maxlon, cntNode, cntTag, cntWay, cntNd);
}

/**
 * osm05startElement:
 * @ctxt:  An XML parser context
 * @name:  The element name
 * @attributes: The element attribs
 *
 * called when the start of an element has been detected.
 */
static void osm05startElement(void *ctx, const xmlChar *name,
		                      const xmlChar **attributes) {
    if (debug > 1)
        fprintf(stdout,"osm05startElement\n");
    // Parsing Bounds
    if (strncmp((char *) name, "bounds", 6) == 0) {
        if (debug > 1)
            fprintf(stdout,"Parsing Bounds\n");
        while (*attributes != NULL) {
            if(strncmp((char *) *(attributes), "minlat", 6) == 0) {
                sscanf((char *) *(attributes+1),"%f",&cFile->bounds->minlat);
            } else if(strncmp((char *) *(attributes), "minlon", 6) == 0) {
                sscanf((char *) *(attributes+1),"%f",&cFile->bounds->minlon);
            } else if(strncmp((char *) *(attributes), "maxlat", 6) == 0) {
                sscanf((char *) *(attributes+1),"%f",&cFile->bounds->maxlat);
            } else if(strncmp((char *) *(attributes), "maxlon", 6) == 0) {
                sscanf((char *) *(attributes+1),"%f",&cFile->bounds->maxlon);
            }
            attributes+=2;
        }
    }
    // Parsing Nodes
    else if (strncmp((char *) name, "node", 4) == 0) {
        cntNode++;
        cNode = malloc(sizeof(osmNode));
        while (*attributes != NULL) {
            if(strncmp((char *) *(attributes), "id", 2) == 0) {
                sscanf((char *) *(attributes+1),"%i",& cNode->id);
            } else if(strncmp((char *) *(attributes), "lat", 3) == 0) {
                sscanf((char *) *(attributes+1),"%f",& cNode->lat);
            } else if(strncmp((char *) *(attributes), "lon", 3) == 0) {
                sscanf((char *) *(attributes+1),"%f",& cNode->lon);
            }
            attributes+=2;
	   }
	   
	   cNode->tag = NULL;
	   cNode->layer = 0;
	   
	   // Insert Node
	   LL_INSERT_ID(cNode,cFile->nodes);
	   
	   if (debug > 1)
	       fprintf(stdout,"NODE: %i %f %f\n", cNode->id, cNode->lat, cNode->lon);
    }
    // Parsing Tags
    else if (strncmp((char *) name, "tag", 4) == 0) {
        cntTag++;
        cTag = malloc(sizeof(osmTag));
        while (*attributes != NULL) {
            if(strncmp((char *) *(attributes), "k", 1) == 0) {
                if(strncmp((char *) *(attributes+1), "created_by", 10) == 0) {
                    free(cTag);
	                cTag = NULL;
                    return;
                }
                size_t c = strlen((char *) *(attributes+1)) +1;
                cTag->key = malloc(c);
                strncpy((char *) cTag->key,(char *) *(attributes+1), c);
            } else 
            if(strncmp((char *) *(attributes), "v", 1) == 0) {
                if(strncmp(cTag->key, "layer", 5) == 0) {
                    free(cTag);
                    cTag = NULL;
                    if (cNode)
                        sscanf((char *) *(attributes+1),"%i",& cNode->layer);
                    if (cWay)
                        sscanf((char *) *(attributes+1),"%i",& cWay->layer);
                    return;
                }
                size_t c = strlen((char *) *(attributes+1)) +1;
                cTag->value = malloc(c);
                strncpy((char *) cTag-> value,(char *) *(attributes+1), c);
            }  
            attributes+=2;
	   }
	   
	   if (debug > 1)
	       fprintf(stdout,"Tag: %s => %s\n", cTag->key, cTag->value);
	   
	   if (cNode)
	       LL_INSERT_KEY(cTag,cNode->tag);
	   if (cWay)
	       LL_INSERT_KEY(cTag,cWay->tag);
	   
	   cTag = NULL;
    }
    // Parsing Way
    else if (strncmp((char *) name, "way", 3) == 0) {
        cntWay++;
        cWay = malloc(sizeof(osmWay));
        while (*attributes != NULL) {
            if(strncmp((char *) *(attributes), "id", 2) == 0) {
                sscanf((char *) *(attributes+1),"%i",&cWay->id);
                break;
            }
            attributes+=2;
	   }
	   
	   cWay->tag = NULL;
	   cWay->nd = NULL;
	   cWay->layer = 0;
	   
	   // Insert Way
	   LL_INSERT_ID(cWay,cFile->ways);
	   
	   if (debug > 1)
	       fprintf(stdout,"WAY(%i)\n", cWay->id);
    }
    // Parsing WayNode
    else if (strncmp((char *) name, "nd", 2) == 0) {
        cntNd++;
        int ref = 0;
        while (*attributes != NULL) {
            if(strncmp((char *) *(attributes), "ref", 2) == 0) {
                sscanf((char *) *(attributes+1),"%i",&ref);
                break;
            }
            attributes+=2;
        }
        
        if (ref) {
            cNd = malloc(sizeof(osmNd));
            
            LL_SEARCH_ID(cFile->nodes,ref,cNd->node);

            // Insert WayNode
            LL_APPEND(cNd,cWay->nd);
       
            if (debug > 1)
                fprintf(stdout," ND( %f %f )\n", cNd->node->lat, cNd->node->lon);
                
            cNode=NULL;
            cNd = NULL;
        }
    }
}

/**
 * osm05endElement:
 * @ctxt:  An XML parser context
 * @name:  The element name
 *
 * called when the end of an element has been detected.
 */
static void osm05endElement(void *ctx, const xmlChar *name) {
    if (debug > 1)
        fprintf(stdout,"osm05endElement\n");
    if (strncmp((char *) name, "node", 4) == 0) {
        cNode = NULL;
    } else if (strncmp((char *) name, "way", 3) == 0) {
        cWay = NULL;
    }
}


xmlSAXHandler osmSAX2HandlerStruct = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    osm05endDocument,
    osm05startElement,
    osm05endElement,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    NULL,
    NULL,
    NULL,
    NULL
};

xmlSAXHandlerPtr osmSAX2Handler = &osmSAX2HandlerStruct;

osmFile* osmRead(char *filename) {
    if (debug > 1)
        fprintf(stdout,"osmRead\n");
    
    xmlInitParser();
    
    xmlParserCtxtPtr ctxt;
    int res;
    
    cFile = NULL;
    cFile = malloc(sizeof(cFile));
    cFile->bounds = malloc(sizeof(osmBounds));
    cFile->ways = NULL;
    cFile->nodes = NULL;
    
    res = xmlSAXUserParseFile(osmSAX2Handler, NULL, filename);
    
    xmlCleanupParser();
    xmlMemoryDump();

    return(cFile);
}
