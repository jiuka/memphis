#include <libxml/parser.h>
#include <stdio.h>
#include <string.h>

#include "config01.h"
#include "main.h"
#include "ll.h"

// Pointers to work with
cfgFile     *cCfgFile = NULL;
cfgTag      *cCfgTag = NULL;
cfgNode     *cCfgNode = NULL;
cfgWay      *cCfgWay = NULL;

// Counts
int     cntCfgTag = 0;
int     cntCfgNode = 0;
int     cntCfgWay = 0;


/**
 * cfg01startDocument:
 * @ctxt:  An XML parser context
 *
 * called when the document start has been detected.
 */
static void cfg01startDocument(void *ctx) {
    if (debug > 1)
        fprintf(stdout,"cfg01startDocument\n");
}

/**
 * cfg01endDocument:
 * @ctxt:  An XML parser context
 *
 * called when the document end has been detected.
 */
static void cfg01endDocument(void *ctx) {
    if (debug > 1)
        fprintf(stdout,"cfg01endDocument\n");
    if (debug > 0)
        fprintf(stdout,"Finished CFG Parsing\n Nodes: % 8i\n Tags:  % 8i\n Ways:  % 8i\n", cntCfgNode, cntCfgTag, cntCfgWay);
}

/**
 * cfg01startElement:
 * @ctxt:  An XML parser context
 * @name:  The element name
 * @attributes: The element attribs
 *
 * called when the start of an element has been detected.
 */
static void cfg01startElement(void *ctx, const xmlChar *name,
		                      const xmlChar **attributes) {
    if (debug > 1)
        fprintf(stdout,"cfg01startElement\n");
    // Parsing Nodes
    /*
    else if (strncmp((char *) name, "node", 4) == 0) {
        cntCfgNode++;
        cCfgNode = malloc(sizeof(osmNode));
        while (*attributes != NULL) {
            if(strncmp((char *) *(attributes), "id", 2) == 0) {
                sscanf((char *) *(attributes+1),"%i",& cCfgNode->id);
            } else if(strncmp((char *) *(attributes), "lat", 3) == 0) {
                sscanf((char *) *(attributes+1),"%f",& cCfgNode->lat);
            } else if(strncmp((char *) *(attributes), "lon", 3) == 0) {
                sscanf((char *) *(attributes+1),"%f",& cCfgNode->lon);
            }
            attributes+=2;
	   }
	   
	   cCfgNode->tag = NULL;
	   cCfgNode->layer = 0;
	   
	   // Insert Node
	   LL_INSERT_ID(cCfgNode,cCfgFile->nodes);
	   
	   if (debug > 1)
	       fprintf(stdout,"NODE: %i %f %f\n", cCfgNode->id, cCfgNode->lat, cCfgNode->lon);
    }*/
    // Parsing Tags
    else if (strncmp((char *) name, "tag", 4) == 0) {
        cntCfgTag++;
        cCfgTag = malloc(sizeof(cfgTag));
        while (*attributes != NULL) {
            if(strncmp((char *) *(attributes), "k", 1) == 0) {
                size_t c = strlen((char *) *(attributes+1)) +1;
                cCfgTag->key = malloc(c);
                strncpy((char *) cCfgTag->key,(char *) *(attributes+1), c);
            } else 
            if(strncmp((char *) *(attributes), "v", 1) == 0) {
                size_t c = strlen((char *) *(attributes+1)) +1;
                cCfgTag->value = malloc(c);
                strncpy((char *) cCfgTag-> value,(char *) *(attributes+1), c);
            }  
            attributes+=2;
	   }
	   
	   if (debug > 1)
	       fprintf(stdout,"Tag: %s => %s\n", cCfgTag->key, cCfgTag->value);
	   
	   if (cCfgNode)
	       LL_INSERT_KEY(cCfgTag,cCfgNode->tag);
	   if (cCfgWay)
	       LL_INSERT_KEY(cCfgTag,cCfgWay->tag);
	   
	   cCfgTag = NULL;
    }
    // Parsing Way
    else if (strncmp((char *) name, "way", 3) == 0) {
        cntCfgWay++;
        cCfgWay = malloc(sizeof(cfgWay));
        cCfgWay->width = 0;
        while (*attributes != NULL) {
            if(strncmp((char *) *(attributes), "type", 4) == 0) {
                if(strncmp((char *) *(attributes+1), "polygone", 8) == 0) {
                    cCfgWay->type = POLYGONE;
                } else if(strncmp((char *) *(attributes+1), "street", 6) == 0) {
                    cCfgWay->type = STREET;
                }
            } else if(strncmp((char *) *(attributes), "width", 4) == 0) {
                sscanf((char *) *(attributes+1),"%i",&cCfgWay->width);
            } else if(strncmp((char *) *(attributes), "color", 4) == 0) {
                sscanf((char *) *(attributes+1),"%f,%f,%f",&cCfgWay->color[0],
                                                            &cCfgWay->color[1],
                                                            &cCfgWay->color[2]);
            }
            attributes+=2;
	   }
	   
	   cCfgWay->tag = NULL;
	   
	   // Insert Way
	   LL_APPEND(cCfgWay,cCfgFile->ways);
	   
	   if (debug > 1)
	       fprintf(stdout,"WAY\n");
    }
}

/**
 * cfg01endElement:
 * @ctxt:  An XML parser context
 * @name:  The element name
 *
 * called when the end of an element has been detected.
 */
static void cfg01endElement(void *ctx, const xmlChar *name) {
    if (debug > 1)
        fprintf(stdout,"cfg01endElement\n");
    if (strncmp((char *) name, "node", 4) == 0) {
        cCfgNode = NULL;
    } else if (strncmp((char *) name, "way", 3) == 0) {
        cCfgWay = NULL;
    }
}


xmlSAXHandler cfgSAX2HandlerStruct = {
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
    cfg01endDocument,
    cfg01startElement,
    cfg01endElement,
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
    NULL,
    NULL,
    NULL
};

xmlSAXHandlerPtr cfgSAX2Handler = &cfgSAX2HandlerStruct;

cfgFile* cfgRead(char *filename) {
    if (debug > 1)
        fprintf(stdout,"osmRead\n");
    
    xmlInitParser();
    
    xmlParserCtxtPtr ctxt;
    int res;
    
    cCfgFile = NULL;
    cCfgFile = malloc(sizeof(cCfgFile));
    cCfgFile->ways = NULL;
    cCfgFile->nodes = NULL;
    
    res = xmlSAXUserParseFile(cfgSAX2Handler, NULL, filename);
    
    xmlCleanupParser();
    xmlMemoryDump();

    return(cCfgFile);
}
