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

#include <time.h>
#include <expat.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "strlist.h"
#include "main.h"
#include "ruleset.h"

#define BUFFSIZE 1024
#define MAXDEPTH 20
#define MAXSTACK 20

// External Vars
extern int      debug;
extern strList  *keyStrings;
extern strList  *valStrings;
extern strList  *patternStrings;

// Pointers to work with
cfgRule     *currentRule;
cfgRule     *ruleStack[MAXSTACK];
char        **stringStack;

// Counts
int     cntCfgRule = 0;

/**
 * cfgStartElement:
 * @userdata:  Void
 * @name:  The element name
 * @attributes: The element attribs
 *
 * called when the start of an element has been detected.
 */
static void XMLCALL
cfgStartElement(void *userData, const char *name, const char **atts) {
    cfgRules *ruleset = (cfgRules *)userData;
    if (debug > 1)
        fprintf(stdout,"cfgStartElement\n");

    // Parsing Rules
    if (strcmp((char *) name, "rules") == 0) {
        // Init Ruleset
        ruleset = malloc(sizeof(cfgRules));
        ruleset->data = NULL;
        ruleset->scale=1;
        ruleset->rule = NULL;
        
        while (*atts != NULL) {
            if(strcmp((char *) *(atts), "data") == 0) {
                size_t c = strlen((char *) *(atts +1)) +1;
                ruleset->data = malloc(c);
                strncpy((char *) ruleset->data, (char *) *(atts +1), c);
            } else if(strcmp((char *) *(atts), "scale") == 0) {
                sscanf((char *) *(atts +1),"%f",&ruleset->scale);
            }
            atts+=2;
        }
    }
    // Parsing Rule
    else if (strcmp(name, "rule") == 0) {
        ruleset->cntRule++;
        ruleset->depth++;
        
        // Create Rule
        cfgRule *new;
        new = malloc(sizeof(cfgRule));
        new->next = NULL;
        new->type = 0;
        new->parent = NULL;
        new->nparent = NULL;

        char *buf, *buf2;
        stringStack = malloc(MAXSTACK*sizeof(char *));
        int c;
        
#ifdef DEBUG        
        new->d = ruleset->depth;
        
        int h;
        for(h=0;h<ruleset->depth;h++){
            fprintf(stdout,"-");
        }
        fprintf(stdout,"rule\n");
#endif        
        
        // Populate Rule
        while (*atts != NULL) {
            if(strcmp((char *) *(atts), "e") == 0) {
                if(strstr((char *) *(atts +1),"way") != NULL)
                    new->type |= WAY;
                if(strstr((char *) *(atts +1),"node") != NULL)
                    new->type |= NODE;
            } else if(strcmp((char *) *(atts), "k") == 0) {
                buf2 = strdup((char *) *(atts +1));
                c = 0;
                while((buf = strsep(&buf2, "|")) != NULL) {
                     *(stringStack+c) = malloc(strlen(buf)+1);
                    strncpy((char *) *(stringStack+c),buf,strlen(buf)+1);
                    c++;
                }
                new->key = malloc((c+1)*sizeof(char *));
                *(new->key+c) = NULL;
                while(c--) {
                    STRLIST_GET(keyStrings,*(stringStack+c),*(new->key+c));
                    *(stringStack+c) = NULL;
                } 
                free(buf2);
            } else if(strcmp((char *) *(atts), "v") == 0) {
                buf2 = strdup((char *) *(atts +1));
                c = 0;
                while((buf = strsep(&buf2, "|")) != NULL) {
                    *(stringStack+c) = malloc(strlen(buf)+1);
                    strncpy((char *) *(stringStack+c),buf,strlen(buf)+1);
                    c++;
                }
                new->value = malloc((c+1)*sizeof(char *));
                *(new->value+c) = NULL;
                while(c--) {
                    STRLIST_GET(valStrings,*(stringStack+c),*(new->value+c));
                    *(stringStack+c) = NULL;
                } 
                free(buf2);
            }
            atts+=2;
        }
        
        free(stringStack);
 
        // Insert Rule to chain
        if(ruleset->rule == NULL)
            ruleset->rule = new;
        else    
            currentRule->next = new;
        currentRule = new;
        
        // Adding to stack
        ruleStack[ruleset->depth] = currentRule;
        
    }
    // Parsing Else
    else if (strcmp(name, "else") == 0) {
        ruleset->cntElse++;
        ruleset->depth++;
        
#ifdef DEBUG        
        new->d = ruleset->depth;
        
        int h;
        for(h=0;h<ruleset->depth;h++){
            fprintf(stdout,"-");
        }
        fprintf(stdout,"else\n");
#endif        
    }
    // Parsing Polygone, etc.
    else if (
        strcmp(name, "polygone") == 0 ||
        strcmp(name, "line") == 0
    ) {
        // Create Draw
        cfgDraw *new;
        new = malloc(sizeof(cfgDraw));  
        new->pattern = NULL;    
        
        // Populate Draw
        if (strcmp(name, "polygone") == 0)
            new->type = POLYGONE;
        else if (strcmp(name, "line") == 0)
            new->type = LINE;  
        
        while (*atts != NULL) {
            if(strcmp((char *) *(atts), "color") == 0) {
                sscanf((char *) *(atts+1),"%f,%f,%f",&new->color[0],
                            &new->color[1],&new->color[2]);
            } else if(strcmp((char *) *(atts), "width") == 0) {
                sscanf((char *) *(atts+1),"%f",&new->width);
            } else if(strcmp((char *) *(atts), "pattern") == 0) {
                STRLIST_GET(patternStrings,(char *) *(atts+1),new->pattern);
            }
            atts+=2;
        }
        
        // Insert Draw
        if(currentRule->parent == 0)
            LL_APPEND(new,ruleStack[ruleset->depth]->draw);
        else
            LL_APPEND(new,ruleStack[ruleset->depth]->ndraw);
    }
}

/**
 * cfgEndElement:
 * @userData:  Void
 * @name:  The element name
 *
 * called when the end of an element has been detected.
 */
static void XMLCALL
cfgEndElement(void *userData, const char *name) {
    cfgRules *ruleset = (cfgRules *)userData;
    if (debug > 1)
        fprintf(stdout,"cfgEndElement\n");
        
    if (strcmp(name, "rule") == 0) {
        // Fetching Parrent from stack
        if(ruleset->depth > 0) {
            if (ruleStack[ruleset->depth-1]->parent == NULL) {
                ruleStack[ruleset->depth]->parent = ruleStack[ruleset->depth-1];
            } else {   // If parent allready closed we are else.
                ruleStack[ruleset->depth]->nparent = ruleStack[ruleset->depth-1];
            }
        }
        
        ruleset->depth--;
    } else if (strcmp(name, "else") == 0) {
        ruleset->depth--;
    }
}

/**
 * rulesetRead
 */
cfgRules* rulesetRead(char *filename) {
    if (debug > 1)
        fprintf(stdout,"rulesetRead\n");
    
    // Local Vars
    int         len;
    int         done;
    char        *buf;
    cfgRules    *ruleset = NULL;    
    
    // NULL rule stack
    for (len=0;len<MAXDEPTH;len++) {
        ruleStack[len] = NULL;
    }
    
    // Open file
    FILE *fd = fopen(filename,"r");
    if(fd == NULL) {
        fprintf(stderr,"Error: Can't open file \"%s\"\n",filename);
        return NULL;
    }
    
    ruleset = malloc(sizeof(cfgRules));
    ruleset->depth = -1;
    ruleset->cntRule = 0;
    ruleset->cntElse = 0;
    
    long start = (long)clock();
    
    // Create XML Parser
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetElementHandler(parser, cfgStartElement, cfgEndElement);

    XML_SetUserData(parser, ruleset);
    
    // Create Buffer
    buf = malloc(BUFFSIZE*sizeof(char));
    
    // Looping over XML
    while(!feof(fd)) {
         len = (int)fread(buf, 1, BUFFSIZE, fd);
         if (ferror(fd)) {
            fprintf(stderr, "Read error\n");
            return NULL;;
        }
        done = len < sizeof(buf);
        if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
            fprintf(stderr, "Parse error at line %iu:\n%s\n",
                (int) XML_GetCurrentLineNumber(parser),
                XML_ErrorString(XML_GetErrorCode(parser)));
            exit(-1);
        }        
    }
    
    // Cleaning Memory
    XML_ParserFree(parser);
    free(buf);
    fclose(fd);
    
    if (debug > 0)
        fprintf(stdout," Ruleset parsing done. (%i/%i) [%fs]\n",
                ruleset->cntRule, ruleset->cntElse,
                ((long)clock()-start)/(double)CLOCKS_PER_SEC);   
    
    return(ruleset);
}