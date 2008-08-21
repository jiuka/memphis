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
#include <glib.h>
#include <glib/gstdio.h>
#include <time.h>
#include <expat.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "list.h"
#include "ruleset.h"

#define BUFFSIZE 1024
#define MAXDEPTH 20
#define MAXSTACK 20

// External Vars
extern memphisOpt   *opts;
extern GTree        *keyStrings;
extern GTree        *valStrings;
extern GTree        *patternStrings;

// Pointers to work with
cfgRule     *currentRule;
cfgRule     *ruleStack[MAXSTACK];
char        **stringStack;

/**
 * cfgStartElement:
 * @userdata:   cfgRules struct to work with
 * @name:       The element name
 * @atts:       The element attributes
 *
 * Called when the start of an element has been detected.
 */
static void XMLCALL
cfgStartElement(void *userData, const char *name, const char **atts) {
    cfgRules *ruleset = (cfgRules *)userData;
    if (opts->debug > 1)
        fprintf(stdout,"cfgStartElement\n");

    // Parsing Rules
    if (strcmp((char *) name, "rules") == 0) {
        // Init Ruleset
        ruleset->rule = NULL;

        while (*atts != NULL) {
            if(strcmp((char *) *(atts), "background") == 0) {
                sscanf((char *) *(atts+1),"#%2x%2x%2x",
                                    (unsigned int *)&ruleset->background[0],
                                    (unsigned int *)&ruleset->background[1],
                                    (unsigned int *)&ruleset->background[2]);
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
        new = g_new(cfgRule, 1);
        new->next = NULL;
        new->type = 0;
        new->parent = NULL;
        new->nparent = NULL;
        new->draw = NULL;
        new->ndraw = NULL;

        int c;

        // Populate Rule
        while (*atts != NULL) {
            if(strcmp((char *) *(atts), "e") == 0) {
                if(strstr((char *) *(atts +1),"way") != NULL)
                    new->type |= WAY;
                if(strstr((char *) *(atts +1),"node") != NULL)
                    new->type |= NODE;
            } else if(strcmp((char *) *(atts), "k") == 0) {
                new->key = g_strsplit((char *) *(atts +1), "|", 0);
                for(c=0;c<g_strv_length(new->key);c++) {
                    char *tmp;
                    tmp = *(new->key+c);
                    *(new->key+c) = g_tree_lookup(keyStrings, tmp);
                    if(*(new->key+c) == NULL) {
                        g_tree_insert(keyStrings, tmp, tmp);
                        *(new->key+c) = tmp;
                    } else  {
		              g_free(tmp);
                    }
                }
            } else if(strcmp((char *) *(atts), "v") == 0) {
                new->value = g_strsplit((char *) *(atts +1), "|", 0);
                for(c=0;c<g_strv_length(new->value);c++) {
                    char *tmp;
                    tmp = *(new->value+c);
                    *(new->value+c) = g_tree_lookup(valStrings, tmp);
                    if(*(new->value+c) == NULL) {
                        g_tree_insert(valStrings, tmp, tmp);
                        *(new->value+c) = tmp;
                    } else  {
		              g_free(tmp);
                    }
                }
            }
            atts+=2;
        }

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
    }
    // Parsing Polygone, etc.
    else if (
        strcmp(name, "polygone") == 0 ||
        strcmp(name, "line") == 0 ||
        strcmp(name, "text") == 0
    ) {
        // Create Draw
        cfgDraw *new;
        new = g_new(cfgDraw, 1);
        new->pattern = NULL;
        new->minlayer = 0;
        new->maxlayer = 99;

        // Populate Draw
        if (strcmp(name, "polygone") == 0)
            new->type = POLYGONE;
        else if (strcmp(name, "line") == 0)
            new->type = LINE;
        else if (strcmp(name, "text") == 0)
            new->type = TEXT;

        while (*atts != NULL) {
            if(strcmp((char *) *(atts), "color") == 0) {
                sscanf((char *) *(atts+1),"#%2x%2x%2x",
                                            (unsigned int *)&new->color[0],
                                            (unsigned int *)&new->color[1],
                                            (unsigned int *)&new->color[2]);
            } else if(strcmp((char *) *(atts), "width") == 0) {
                sscanf((char *) *(atts+1),"%f",&new->width);
            } else if(strcmp((char *) *(atts), "pattern") == 0) {
                new->pattern = g_tree_lookup(patternStrings, (char *) *(atts+1));
                if(new->pattern == NULL) {
                    new->pattern = g_strdup((char *) *(atts+1));
                    g_tree_insert(patternStrings, (char *) *(atts+1), new->pattern);
                }
            } else if(strcmp((char *) *(atts), "layer") == 0) {
                sscanf((char *) *(atts+1),"%hi:%hi",
                                            &new->minlayer,
                                            &new->maxlayer);
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
 * @userdata:   cfgRules struct to work with
 * @name:       The element name
 *
 * Called when the end of an element has been detected.
 */
static void XMLCALL
cfgEndElement(void *userData, const char *name) {
    cfgRules *ruleset = (cfgRules *)userData;
    if (opts->debug > 1)
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
 * rulesetRead:
 * @filename:   Filename of the rule file
 *
 * Called to parse rules in the rulefile. Returns a cfgRules struct.
 */
cfgRules* rulesetRead(char *filename) {
    if (opts->debug > 1)
        fprintf(stdout,"rulesetRead\n");

    // Local Vars
    GTimer *tRulesetRead = g_timer_new();
    unsigned int size;
    unsigned int read = 0;
    struct stat filestat;
    int         len;
    int         done;
    char        *buf;
    cfgRules    *ruleset = NULL;

    // NULL rule stack
    for (len=0;len<MAXDEPTH;len++) {
        ruleStack[len] = NULL;
    }
    
    // Test file
    if (!g_file_test (filename, G_FILE_TEST_IS_REGULAR)) {
        fprintf(stderr,"Error: \"%s\" is not a file.\n",filename);
        return NULL;
    }
    
    g_stat(filename, &filestat);
    size = (int) filestat.st_size;

    // Open file
    FILE *fd = fopen(filename,"r");
    if(fd == NULL) {
        fprintf(stderr,"Error: Can't open file \"%s\"\n",filename);
        return NULL;
    }

    ruleset = g_new(cfgRules, 1);
    ruleset->depth = -1;
    ruleset->cntRule = 0;
    ruleset->cntElse = 0;

    if (opts->debug > 0) {
        fprintf(stdout," Ruleset parsing   0%%");
        fflush(stdout);
    }

    // Create XML Parser
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetElementHandler(parser, cfgStartElement, cfgEndElement);

    XML_SetUserData(parser, ruleset);

    // Create Buffer
    buf = g_malloc(BUFFSIZE*sizeof(char));

    // Looping over XML
    while(!feof(fd)) {
         len = (int)fread(buf, 1, BUFFSIZE, fd);
         if (ferror(fd)) {
            fprintf(stderr, "Read error\n");
            return NULL;;
        }
        read += len;
        if (opts->debug > 0) {
            fprintf(stdout,"\r Ruleset parsing % 3i%%", (int)((read*100)/size));
            fflush(stdout);
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
    g_free(buf);
    fclose(fd);

    if (opts->debug > 0)
        fprintf(stdout,"\r Ruleset parsing done. (%i/%i) [%fs]\n",
                ruleset->cntRule,  ruleset->cntElse,
                g_timer_elapsed(tRulesetRead,NULL));
    
    g_timer_destroy(tRulesetRead);

    return(ruleset);
}

void rulesetFree(cfgRules * ruleset) {
    cfgRule *rule, *lrule;
    cfgDraw *draw, *ldraw;
    
    for(rule=ruleset->rule,lrule=NULL;rule;lrule=rule,rule=rule->next) {
        for(draw=rule->draw,ldraw=NULL;draw;ldraw=draw,draw=draw->next) {
            if(draw->pattern)
                g_tree_replace(patternStrings, draw->pattern, draw->pattern);
            if(ldraw)
                g_free(ldraw);
        }
        if(ldraw)
            g_free(ldraw);
        for(draw=rule->ndraw,ldraw=NULL;draw;ldraw=draw,draw=draw->next) {
            if(draw->pattern)
                g_tree_replace(patternStrings, draw->pattern, draw->pattern);
            if(ldraw)
                g_free(ldraw);
        }
        if(ldraw)
            g_free(ldraw);

        char **tmp;
        tmp = rule->key;
        while (*tmp != NULL) {
            g_tree_replace(keyStrings, *tmp, *tmp);
            tmp++;
        }
        g_free(rule->key);

        tmp = rule->value;
        while (*tmp != NULL) {
            g_tree_replace(keyStrings, *tmp, *tmp);
            tmp++;
        }
        g_free(rule->value);

        if(lrule)
            g_free(lrule);
    }
    g_free(lrule);
    g_free(ruleset);
};

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

