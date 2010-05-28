/*
 * Memphis - Cairo Rederer for OSM in C
 * Copyright (C) 2008  Marius Rieder <marius.rieder@durchmesser.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <glib.h>
#include <glib/gstdio.h>
#include <expat.h>
#include <string.h>

#include "ruleset.h"
#include "mlib.h"
#include "memphis-data-pool.h"
#include "memphis-debug.h"

#define BUFFSIZE 1024
#define MAXDEPTH 20
#define MAXSTACK 20

typedef struct rulesUserData_ rulesUserData;
struct rulesUserData_ {
    // Pointers to work with
    cfgRule *currentRule;
    cfgRule *ruleStack[MAXSTACK];
    char **stringStack;
    MemphisDataPool *pool;
    // Collected Data
    cfgRules *ruleset;
};

static void
cfgDraw_list_append(cfgDraw *draw, cfgDraw **list)
{
    g_assert (draw != NULL);

    cfgDraw *curr = *list;
    cfgDraw *prev = NULL;

    while (curr != NULL) {
        prev = curr;
        curr = curr->next;
    }
    if (prev != NULL)
        prev->next = draw;
    else
        *list = draw;

    draw->next = NULL;
}

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
    rulesUserData *data = (rulesUserData *)userData;
    cfgRules *ruleset = data->ruleset;
    GStringChunk *stringChunk = data->pool->stringChunk;
    GTree *stringTree = data->pool->stringTree;

    memphis_debug ("cfgStartElement");

    // Parsing Rules
    if (strcmp((char *) name, "rules") == 0) {
        // Init Ruleset
        ruleset->rule = NULL;

        while (*atts != NULL) {
            if(strcmp((char *) *(atts), "background") == 0) {
                unsigned int r, g, b, a;
                if(sscanf((char *) *(atts+1),"#%2x%2x%2x%2x",
                        &r, &g, &b, &a)
                        < 4)
                {
                    // Default background alpha value
                    a = 255;
                }
                ruleset->background[0] = r;
                ruleset->background[1] = g;
                ruleset->background[2] = b;
                ruleset->background[3] = a;
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
                new->key = g_strsplit((char *) *(atts + 1), "|", 0);
                for(c = 0; c < g_strv_length(new->key); c++) {
                    char *tmp = *(new->key + c);
                    *(new->key + c) = m_string_chunk_get(stringChunk,
                                                          stringTree, (char *) tmp);
		            g_free(tmp);
                }
            } else if(strcmp((char *) *(atts), "v") == 0) {
                new->value = g_strsplit((char *) *(atts + 1), "|", 0);
                for(c = 0; c < g_strv_length(new->value); c++) {
                    char *tmp = *(new->value + c);
                    *(new->value + c) = m_string_chunk_get(stringChunk,
                                                           stringTree, tmp);
		            g_free(tmp);
                }
            }
            atts += 2;
        }

        // Insert Rule to chain
        if(ruleset->rule == NULL)
            ruleset->rule = new;
        else
            data->currentRule->next = new;
        data->currentRule = new;

        // Adding to stack
        data->ruleStack[ruleset->depth] = data->currentRule;

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
        new->minzoom = 0;
        new->maxzoom = 99;

        // Populate Draw
        if (strcmp(name, "polygone") == 0)
            new->type = POLYGONE;
        else if (strcmp(name, "line") == 0)
            new->type = LINE;
        else if (strcmp(name, "text") == 0)
            new->type = TEXT;

        while (*atts != NULL) {
            if(strcmp((char *) *(atts), "color") == 0) {
                unsigned int r, g, b;
                sscanf((char *) *(atts+1),"#%2x%2x%2x", &r, &g, &b);
                new->color[0] = r;
                new->color[1] = g;
                new->color[2] = b;
            } else if(strcmp((char *) *(atts), "width") == 0) {
                sscanf((char *) *(atts+1),"%f",&new->width);
            } else if(strcmp((char *) *(atts), "pattern") == 0) {
                new->pattern = m_string_chunk_get(stringChunk, stringTree,
                                                  (char *) *(atts+1));
            } else if(strcmp((char *) *(atts), "zoom") == 0) {
                sscanf((char *) *(atts+1),"%hi:%hi",
                        &new->minzoom,
                        &new->maxzoom);
            }
            atts+=2;
        }

        // Insert Draw
        if(data->currentRule->parent == 0)
            cfgDraw_list_append(new, &(data->ruleStack[ruleset->depth]->draw));
        else
            cfgDraw_list_append(new, &(data->ruleStack[ruleset->depth]->ndraw));
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
    rulesUserData *data = (rulesUserData *)userData;
    cfgRules *ruleset = data->ruleset;
    
    memphis_debug ("cfgEndElement");

    if (strcmp(name, "rule") == 0) {
        // Fetching Parent from stack
        if(ruleset->depth > 0) {
            if (data->ruleStack[ruleset->depth-1]->parent == NULL) {
                data->ruleStack[ruleset->depth]->parent = data->ruleStack[ruleset->depth-1];
            } else {   // If parent already closed we are else.
                data->ruleStack[ruleset->depth]->nparent = data->ruleStack[ruleset->depth-1];
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
cfgRules* rulesetRead(const char *filename, GError **error)
{
    memphis_debug ("rulesetRead");

    // Local Vars
    GTimer *tRulesetRead = g_timer_new();
    unsigned int size;
    unsigned int read = 0;
    struct stat filestat;
    int         len;
    int         done;
    char        *buf;
    rulesUserData *data = g_new(rulesUserData, 1);
    cfgRules    *ruleset = NULL;

    // NULL rule stack
    for (len=0;len<MAXDEPTH;len++) {
        data->ruleStack[len] = NULL;
    }
    
    // Test file
    if (!g_file_test (filename, G_FILE_TEST_IS_REGULAR)) {
        g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_NOENT,
                "Failed to open file: %s", filename);
        g_warning ("Warning: \"%s\" is not a file.", filename);
        g_free (data);
        return NULL;
    }
    
    g_stat(filename, &filestat);
    size = (int) filestat.st_size;

    // Open file
    FILE *fd = fopen (filename, "r");
    if(fd == NULL) {
        g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_ACCES,
                "Failed to open file: %s", filename);
        g_warning ("Warning: Can't open file \"%s\"", filename);
        g_free (data);
        return NULL;
    }

    ruleset = rulesetNew ();
    data->ruleset = ruleset;
    data->pool = memphis_data_pool_new ();

    if (G_UNLIKELY (memphis_debug_get_print_progress ())) {
        g_fprintf(stdout, " Ruleset parsing   0%%");
        fflush(stdout);
    }
    int progress = 0;

    // Create XML Parser
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetElementHandler(parser, cfgStartElement, cfgEndElement);

    XML_SetUserData(parser, data);

    // Create Buffer
    buf = g_malloc(BUFFSIZE*sizeof(char));

    // Looping over XML
    while(!feof(fd)) {
        len = (int)fread(buf, 1, BUFFSIZE, fd);
        if (ferror(fd)) {
            g_warning ("Ruleset read error");
            g_set_error (error, G_MARKUP_ERROR, G_MARKUP_ERROR_PARSE,
                    "Failed to parse file: %s", filename);
            // cleanup
            XML_ParserFree(parser);
            g_free(buf);
            fclose(fd);
            g_free(data);
            g_free(ruleset);
            return NULL;
        }
        read += len;
        if (G_UNLIKELY (memphis_debug_get_print_progress ())) {
            int new_progress = (int)((read * 100.0) / size);
            if (new_progress > progress) {
                g_fprintf (stdout, "\r Ruleset parsing % 3i%%", new_progress);
                fflush(stdout);
                progress = new_progress;
            }
        }
        done = len < sizeof(buf);
        if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
            g_warning ("Parse error at line %i: %s",
                    (int) XML_GetCurrentLineNumber(parser),
                    XML_ErrorString(XML_GetErrorCode(parser)));
            g_set_error (error, G_MARKUP_ERROR, G_MARKUP_ERROR_PARSE,
                    "Rules parse error at line %i: %s",
                    (int) XML_GetCurrentLineNumber(parser), filename);
            // cleanup
            XML_ParserFree(parser);
            g_free(buf);
            fclose(fd);
            g_free(data);
            g_free(ruleset);
            return NULL;
        }
    }

    // Cleaning Memory
    XML_ParserFree(parser);
    g_free(buf);
    fclose(fd);
    g_free(data);

    if (G_UNLIKELY (memphis_debug_get_print_progress ()))
        g_fprintf (stdout, "\r Ruleset parsing done. (%i/%i) [%fs]\n",
                ruleset->cntRule,  ruleset->cntElse,
                g_timer_elapsed(tRulesetRead,NULL));
    
    g_timer_destroy(tRulesetRead);

    return ruleset;
}

/**
 * rulesetRead_from_buffer:
 * @buffer: Buffer containing a rule-set
 * @size: Size of the buffer
 *
 * Called to parse rules in a buffer. Returns a cfgRules struct.
 */
cfgRules* rulesetRead_from_buffer (const char *buffer, guint size,
        GError **error)
{
    memphis_debug ("rulesetRead");

    g_assert (buffer != NULL && size > 0);

    // Local Vars
    GTimer *tRulesetRead = g_timer_new();
    int         len;
    int         isDone = 0;
    rulesUserData *data = g_new(rulesUserData, 1);
    cfgRules    *ruleset = NULL;

    // NULL rule stack
    for (len=0; len<MAXDEPTH; len++) {
        data->ruleStack[len] = NULL;
    }

    ruleset = rulesetNew ();
    data->ruleset = ruleset;
    data->pool = memphis_data_pool_new ();

    if (G_UNLIKELY (memphis_debug_get_print_progress ())) {
        g_fprintf(stdout, " Ruleset parsing ...");
        fflush(stdout);
    }

    // Create XML Parser
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetElementHandler(parser, cfgStartElement, cfgEndElement);
    XML_SetUserData(parser, data);

    // Parse the buffer
    if (XML_Parse (parser, buffer, size, isDone) == XML_STATUS_ERROR) {
        g_warning ("Parse error at line %i: %s",
                (int) XML_GetCurrentLineNumber(parser),
                XML_ErrorString(XML_GetErrorCode(parser)));
        g_set_error (error, G_MARKUP_ERROR, G_MARKUP_ERROR_PARSE,
                "Rules parse error at line %i",
                (int) XML_GetCurrentLineNumber(parser));
        // cleanup
        XML_ParserFree(parser);
        g_free(data);
        g_free(ruleset);
        return NULL;
    }

    // Cleaning Memory
    XML_ParserFree(parser);
    g_free(data);

    if (G_UNLIKELY (memphis_debug_get_print_progress ()))
        g_fprintf (stdout, "\r Ruleset parsing done. (%i/%i) [%fs]\n",
                ruleset->cntRule,  ruleset->cntElse,
                g_timer_elapsed(tRulesetRead,NULL));

    g_timer_destroy(tRulesetRead);

    return ruleset;
}

void rulesetFree(cfgRules * ruleset) {
    cfgRule *rule, *lrule;
    cfgDraw *draw, *ldraw;

    for(rule = ruleset->rule, lrule = NULL;
        rule != NULL;
        lrule = rule, rule = rule->next)
    {
        for(draw = rule->draw, ldraw = NULL;
            draw != NULL;
            ldraw = draw, draw = draw->next)
        {
            if(ldraw)
                g_free(ldraw);
        }
        if(ldraw)
            g_free(ldraw);
        for(draw = rule->ndraw, ldraw = NULL;
            draw != NULL;
            ldraw = draw, draw = draw->next)
        {
            if(ldraw)
                g_free(ldraw);
        }
        if(ldraw)
            g_free(ldraw);

        g_free(rule->key);
        g_free(rule->value);

        if(lrule)
            g_free(lrule);
    }
    g_free(lrule);
    g_free(ruleset);
}

cfgRules* rulesetNew ()
{
    cfgRules* ruleset = g_new (cfgRules, 1);
    ruleset->depth = -1;
    ruleset->cntRule = 0;
    ruleset->cntElse = 0;
    // default background color: white
    ruleset->background[0] = 255;
    ruleset->background[1] = 255;
    ruleset->background[2] = 255;
    ruleset->background[3] = 255;
    ruleset->rule = NULL;

    return ruleset;
}

void cfgRuleFree (cfgRule *rule)
{
    g_free (rule->key);
    g_free (rule->value);

    cfgDraw *tmp;
    cfgDraw *drw = rule->draw;
    while (drw != NULL)
    {
        tmp = drw;
        drw = drw->next;
        g_free (tmp);
    }

    g_free (rule);
}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

