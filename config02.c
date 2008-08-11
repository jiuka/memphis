#include <libxml/parser.h>
#include <stdio.h>
#include <string.h>

#include "config02.h"
#include "main.h"
#include "ll.h"

// Pointers to work with
cfgLayer    *cCfgFile = NULL;
cfgLayer    *cCfgLayer = NULL;
cfgTag      *cCfgTag = NULL;
cfgRule     *cCfgRule = NULL;
cfgDraw     *cCfgDraw = NULL;

// Counts
int     cntCfgLayer = 0;
int     cntCfgRule = 0;
int     cntCfgTag = 0;
int     cntCfgDraw = 0;


/**
 * cfg02startDocument:
 * @ctxt:  An XML parser context
 *
 * called when the document start has been detected.
 */
static void cfg02startDocument(void *ctx) {
    if (debug > 1)
        fprintf(stdout,"cfg02startDocument\n");
}

/**
 * cfg02endDocument:
 * @ctxt:  An XML parser context
 *
 * called when the document end has been detected.
 */
static void cfg02endDocument(void *ctx) {
    if (debug > 1)
        fprintf(stdout,"cfg02endDocument\n");
    if (debug > 0)
        fprintf(stdout,"  Layers: % 8i\n  Rules:  % 8i\n  Tags:   % 8i\n  Draw:   % 8i\n", cntCfgLayer, cntCfgRule, cntCfgTag, cntCfgDraw);
}

/**
 * cfg02startElement:
 * @ctxt:  An XML parser context
 * @name:  The element name
 * @attributes: The element attribs
 *
 * called when the start of an element has been detected.
 */
static void cfg02startElement(void *ctx, const xmlChar *name,
		                      const xmlChar **attributes) {
    if (debug > 1)
        fprintf(stdout,"cfg02startElement\n");

    // Parsing Layer
    if (strncmp((char *) name, "layer", 5) == 0) {
        cntCfgLayer++;
        cCfgLayer = malloc(sizeof(cfgLayer));
        while (*attributes != NULL) {
            if(strncmp((char *) *(attributes), "name", 4) == 0) {
                size_t c = strlen((char *) *(attributes+1)) +1;
                cCfgLayer->name = malloc(c);
                strncpy((char *) cCfgLayer->name,(char *) *(attributes+1), c);
            }
            attributes+=2;
	   }
	   
	   cCfgLayer->rule = NULL;
	   cCfgLayer->next = NULL;
	   
	   // Insert Node
	   LL_APPEND(cCfgLayer,cCfgFile);
    }
    // Parsing Rule
    else if (strncmp((char *) name, "rule", 4) == 0) {
        cntCfgRule++;
        cCfgRule = malloc(sizeof(cfgRule));
        cCfgRule->minlayer = LAYERMIN;
        cCfgRule->maxlayer = LAYERMAX;
        while (*attributes != NULL) {
            if(strncmp((char *) *(attributes), "type", 4) == 0) {
                if(strncmp((char *) *(attributes+1), "way", 8) == 0) {
                    cCfgRule->type = WAY;
                } else if(strncmp((char *) *(attributes+1), "node", 6) == 0) {
                    cCfgRule->type = NODE;
                }
            } else if(strncmp((char *) *(attributes), "layer", 4) == 0) {
                sscanf((char *) *(attributes+1),"%i:%i",&cCfgRule->minlayer,
                                                        &cCfgRule->maxlayer);
            }
            attributes+=2;
	   }
	   
	   cCfgRule->tag = NULL;
	   cCfgRule->draw = NULL;
	   cCfgRule->next = NULL;
	   
	   // Insert Rule  
       LL_APPEND(cCfgRule, cCfgLayer->rule);

    }
    // Parsing Tag
    else if (strncmp((char *) name, "tag", 3) == 0) {
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
                strncpy((char *) cCfgTag->value,(char *) *(attributes+1), c);
            }  
            attributes+=2;
	   }
	   cCfgTag->next = NULL;
	   
	   //Insert Tag to Rule
	   LL_APPEND(cCfgTag,cCfgRule->tag);
    }
    // Parsing Draw
    else if (strncmp((char *) name, "draw", 4) == 0) {
        cntCfgDraw++;
        cCfgDraw = malloc(sizeof(cfgDraw));
        while (*attributes != NULL) {
            if(strncmp((char *) *(attributes), "type", 1) == 0) {
                if(strncmp((char *) *(attributes+1), "polygone", 8) == 0) {
                    cCfgDraw->type = POLYGONE;
                } else if(strncmp((char *) *(attributes+1), "line", 4) == 0) {
                    cCfgDraw->type = LINE;
                }
            } else if(strncmp((char *) *(attributes), "color", 5) == 0) {
                sscanf((char *) *(attributes+1),"%f,%f,%f",&cCfgDraw->color[0],
                                                           &cCfgDraw->color[1],
                                                           &cCfgDraw->color[2]);
            } else if(strncmp((char *) *(attributes), "width", 5) == 0) {
                sscanf((char *) *(attributes+1),"%f",&cCfgDraw->width);
            }  
            attributes+=2;
	   }
	   
	   cCfgDraw->next = NULL;

	   //Insert Draw to Rule
	   LL_INSERT(cCfgDraw,cCfgRule->draw);

	   
	   cCfgDraw = NULL;
    }
}

/**
 * cfg02endElement:
 * @ctxt:  An XML parser context
 * @name:  The element name
 *
 * called when the end of an element has been detected.
 */
static void cfg02endElement(void *ctx, const xmlChar *name) {
    if (strncmp((char *) name, "layer", 5) == 0) {
       cCfgLayer = NULL;
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
    cfg02startDocument,
    cfg02endDocument,
    cfg02startElement,
    cfg02endElement,
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

xmlSAXHandlerPtr cfgSAX2Handler = &cfgSAX2HandlerStruct;

cfgLayer* cfgRead(char *filename) {
    if (debug > 1)
        fprintf(stdout,"cfgRead\n");
    
    xmlInitParser();
    
    xmlParserCtxtPtr ctxt;
    int res;
    
    cCfgFile = NULL;
    
    res = xmlSAXUserParseFile(cfgSAX2Handler, NULL, filename);
    
    xmlCleanupParser();
    xmlMemoryDump();

    return(cCfgFile);
}

void cfgFree(cfgLayer* cfg) {
    cfgLayer *lFree;
    cfgRule *rFree;
    for (cCfgLayer = cfg, lFree=NULL; cCfgLayer; lFree=cCfgLayer,cCfgLayer = cCfgLayer->next) {
        for(cCfgRule=cCfgLayer->rule,rFree=NULL;cCfgRule; rFree=cCfgRule,cCfgRule=cCfgRule->next) {
            if(rFree)
                free(rFree);
        }
        if(rFree)
            free(rFree);
        if(lFree)
            free(lFree);
    }
    if(lFree)
        free(lFree);
}
