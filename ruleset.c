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

#include <expat.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "main.h"
#include "ruleset.h"

#define BUFFSIZE 1024
#define MAXDEPTH 20
#define MAXSTACK 20

// External Vars
extern int debug;

// Pointers to work with
cfgRules    *ruleset = NULL;
cfgRule     *currentRule[MAXDEPTH];
cfgRule     *closeRule[MAXDEPTH];
char        **stringStack;

int         depth;
int         iselse;

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
        depth++;
        
        // Create Rule
        cfgRule *new, *rulebuf, *rulebuf2;
        new = malloc(sizeof(cfgRule));
        new->next = NULL;
        new->type = 0;
        new->sub = NULL;
        new->nsub = NULL;

        char *buf, *buf2;
        stringStack = malloc(MAXSTACK*sizeof(char *));
        int c;
        cfgStr  *str;
        
#ifdef DEBUG        
        new->d = depth;
        
        int h;
        for(h=0;h<depth;h++){
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
                    STRLL_GET(ruleset->keys,*(stringStack+c),str);
                    if(str == NULL) {
                        str = malloc(sizeof(cfgStr));
                        str->str = malloc(strlen(*(stringStack+c)));
                        strcpy(str->str,*(stringStack+c));
                        STRLL_ADD(ruleset->keys,str);
                    }
                    *(new->key+c) = str->str;
                    str = NULL;
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
                    STRLL_GET(ruleset->values,*(stringStack+c),str);
                    if(str == NULL) {
                        str = malloc(sizeof(cfgStr));
                        str->str = malloc(strlen(*(stringStack+c)));
                        strcpy(str->str,*(stringStack+c));
                        STRLL_ADD(ruleset->values,str);
                    }
                    *(new->value+c) = str->str;
                    str = NULL;
                    *(stringStack+c) = NULL;
                } 
                free(buf2);
            }
            atts+=2;
        }
        
        free(stringStack);
 
        // Insert Rule
        if(ruleset->rule == NULL)
            ruleset->rule = new;
        
        
        for(c=depth;c< MAXDEPTH;c++) {
            if (closeRule[c] == NULL)
                break;
                
            rulebuf = closeRule[c];
	        while(rulebuf) {
	           rulebuf2 = rulebuf;
	           rulebuf = rulebuf->next;
	           rulebuf2->next = new;
	        }
            closeRule[c] = NULL;
        }
        
        new->next = closeRule[depth];
        closeRule[depth] = new;
       
        for(c=depth+1;c< MAXDEPTH;c++) {
            currentRule[c] = NULL;
        }
        
        if (currentRule[depth] == NULL && depth > 0) {
            currentRule[depth-1]->sub = new;
        }
        
        currentRule[depth] = new;        
    }
    // Parsing Else
    else if (strcmp(name, "else") == 0) {
        ruleset->cntElse++;
        depth++;
        
        // Create Else
        cfgElse *new;
        new = malloc(sizeof(cfgElse));
        
#ifdef DEBUG        
        new->d = depth;
        
        int h;
        for(h=0;h<depth;h++){
            fprintf(stdout,"-");
        }
        fprintf(stdout,"else\n");
#endif        
        
        // Insert Else
        if (currentRule[depth] != NULL) {
            currentRule[depth]->nsub = new;
        }
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
cfgEndElement(void *userData, const char *name)
{
    if (debug > 1)
        fprintf(stdout,"cfgEndElement\n");
    if (strcmp(name, "rule") == 0) {
        depth--;
    } else if (strcmp(name, "else") == 0) {
        depth--;
    }
}

/**
 * rulesetRead
 */
cfgRules* rulesetRead(char *filename) {
    if (debug > 1)
        fprintf(stdout,"cfgRead\n");
    
    // Local Vars
    int len;
    int done;
    char *buf;
    
    // NULL rule stack
    ruleset = NULL;
    for (len=0;len<MAXDEPTH;len++) {
        currentRule[len] = NULL;
        closeRule[len] = NULL;
    }
    depth = -1;
    iselse = 0;
    
    // Open file
    FILE *fd = fopen(filename,"r");
    // TODO: Check of successfull
    
    // Create XML Parser
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetElementHandler(parser, cfgStartElement, cfgEndElement);
    
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
    
    return(ruleset);
}