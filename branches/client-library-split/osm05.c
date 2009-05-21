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

#include "list.h"
#include "mlib.h"
#include "main.h"
#include "osm05.h"

#define BUFFSIZE 1024

// External Vars
extern memphisOpt   *opts;
extern GStringChunk *stringChunk;
extern GTree        *stringTree;

// Pointers to work with
osmTag      *cTag = NULL;
osmNode     *cNode = NULL;
osmWay      *cWay = NULL;

// Counts
int     cntTag = 0;
int     cntNd = 0;

/**
 * osmStartElement:
 * @userdata:  Void
 * @name:  The element name
 * @atts: The element attribs
 *
 * called when the start of an element has been detected.
 */
static void XMLCALL
osmStartElement(void *userData, const char *name, const char **atts) {
    osmFile *osm = (osmFile *) userData;
     if (opts->debug > 1)
        fprintf(stdout,"osm05startElement\n");
    // Parsing Bounds
    if (strncmp((char *) name, "bounds", 6) == 0) {
        if (opts->debug > 1)
            fprintf(stdout,"Parsing Bounds\n");
        while (*atts != NULL) {
            if(strcmp((char *) *(atts), "minlat" ) == 0) {
                sscanf((char *) *(atts+1),"%f",&osm->minlat);
            } else if(strcmp((char *) *(atts), "minlon") == 0) {
                sscanf((char *) *(atts+1),"%f",&osm->minlon);
            } else if(strcmp((char *) *(atts), "maxlat") == 0) {
                sscanf((char *) *(atts+1),"%f",&osm->maxlat);
            } else if(strcmp((char *) *(atts), "maxlon") == 0) {
                sscanf((char *) *(atts+1),"%f",&osm->maxlon);
            }
            atts+=2;
        }
    }
    // Parsing Node
    else if (strncmp((char *) name, "node", 4) == 0) {
        if (opts->debug > 1)
            fprintf(stdout,"Parsing Node\n");
        cNode = g_new(osmNode, 1);
        while (*atts != NULL) {
            if(strcmp((char *) *(atts), "id") == 0) {
                sscanf((char *) *(atts+1),"%i",&cNode->id);
            } else if(strcmp((char *) *(atts), "lat") == 0) {
                sscanf((char *) *(atts+1),"%f",&cNode->lat);
            } else if(strcmp((char *) *(atts), "lon") == 0) {
                sscanf((char *) *(atts+1),"%f",&cNode->lon);
            }
            atts+=2;
        }

        cNode->tag = NULL;
        cNode->layer = 0;

        // Insert Node
        osm->nodecnt++;
        g_hash_table_insert(osm->nodeidx, &cNode->id, cNode);
        LL_PREPEND(cNode,osm->nodes);

        if (opts->debug > 1)
            fprintf(stdout,"NODE: %i %f %f\n", cNode->id, cNode->lat, cNode->lon);
    }
    // Parsing Tags
    else if (strncmp((char *) name, "tag", 4) == 0) {
        if (opts->debug > 1)
            fprintf(stdout,"Parsing Tag\n");

        if (!cNode && !cWay) // End if ther is nothing to add the tag to
            return;
            
        char *k=NULL, *v=NULL;

        while (*atts != NULL) {
            if(strncmp((char *) *(atts), "k", 1) == 0) {
                if(strcmp((char *) *(atts+1), "created_by") == 0) {
                    return;
                } else if(strncmp((char *) *(atts+1), "source", 6) == 0) {
                    return;
                }
                k = (char *) *(atts+1);
            } else if(strncmp((char *) *(atts), "v", 1) == 0) {
                if(strcmp(k, "layer") == 0) {
                    if (cNode)
                        sscanf((char *) *(atts+1),"%hi",& cNode->layer);
                    else if (cWay)
                        sscanf((char *) *(atts+1),"%hi",& cWay->layer);
                    return;
                } else if(strcmp(k, "name") == 0) {
                    if (cWay) {
                        cWay->name = m_string_chunk_get(stringChunk, stringTree, 
                                                        (char *) *(atts+1));
                    }
                    return;
                }
                v = (char *) *(atts+1);
            }
            atts += 2;
        }
        
        cTag = g_new(osmTag, 1);
        cTag->key = m_string_chunk_get(stringChunk, stringTree, k);
        cTag->value = m_string_chunk_get(stringChunk, stringTree, v);
        
        if (opts->debug > 1)
            fprintf(stdout,"Tag: %s => %s\n", cTag->key, cTag->value);

        cntTag++;
        if (cNode)
            LL_INSERT_KEY(cTag,cNode->tag);
        else if (cWay)
            LL_INSERT_KEY(cTag,cWay->tag);

        cTag = NULL;
    }
    // Parsing Way
    else if (strncmp((char *) name, "way", 3) == 0) {
        if (opts->debug > 1)
            fprintf(stdout,"Parsing Way\n");
        cWay = g_new(osmWay, 1);
        while (*atts != NULL) {
            if(strncmp((char *) *(atts), "id", 2) == 0) {
                sscanf((char *) *(atts+1),"%i",&cWay->id);
                break;
            }
            atts+=2;
        }

        cWay->tag = NULL;
        cWay->nd = NULL;
        cWay->name = NULL;
        cWay->layer = 0;

        // Insert Way
        osm->waycnt++;
        LL_PREPEND(cWay,osm->ways);

        if (opts->debug > 1)
            fprintf(stdout,"WAY(%i)\n", cWay->id);
    }
    // Parsing WayNode
    else if (strncmp((char *) name, "nd", 2) == 0) {
        if (opts->debug > 1)
            fprintf(stdout,"Parsing Nd\n");
        int ref = 0;
        while (*atts != NULL) {
            if(strncmp((char *) *(atts), "ref", 2) == 0) {
                sscanf((char *) *(atts+1),"%i",&ref);
                break;
            }
            atts+=2;
        }

        if (ref) {
            cntNd++;
            osmNode *n;
            
            n = g_hash_table_lookup(osm->nodeidx, &ref);
            if (!n) {
                g_warning("No node with reference %d found!\n", ref);
                return;
            }

            // Insert WayNode
            cWay->nd = g_slist_prepend(cWay->nd, n);

            if (opts->debug > 1)
                fprintf(stdout," ND( %f %f )\n", n->lat, n->lon);

            cNode=NULL;
        }
    }
}


/**
 * osmEndElement:
 * @userData:  Void
 * @name:  The element name
 *
 * called when the end of an element has been detected.
 */
static void XMLCALL
osmEndElement(void *userData, const char *name) {
    if (opts->debug > 1)
        fprintf(stdout,"osm05endElement\n");
    if (strncmp((char *) name, "node", 4) == 0) {
        cNode = NULL;
    } else if (strncmp((char *) name, "way", 3) == 0) {
        if (cWay->nd != NULL)
            cWay->nd = g_slist_reverse(cWay->nd);
        cWay = NULL;
    }
}

/**
 * rulesetRead
 */
osmFile* osmRead(char *filename) {
    if (opts->debug > 1)
        fprintf(stdout,"osmRead\n");

    // Init vars
    cntTag = 0;
    cntNd = 0;

    // Local Vars
    GTimer *tOsmRead = g_timer_new();
    unsigned int size;
    unsigned int read = 0;
    struct stat filestat;
    int len;
    int done;
    char *buf;
    osmFile *osm = NULL;
    
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

    osm = g_new(osmFile, 1);
    osm->nodes = NULL;
    osm->nodeidx = g_hash_table_new(g_int_hash, g_int_equal);
    osm->nodecnt = 0;
    osm->ways = NULL;
    osm->waycnt = 0;
    osm->minlon = -190;
    osm->minlat = -190;
    osm->maxlon = -190;
    osm->maxlat = -190;

    if (opts->debug > 0) {
        fprintf(stdout," OSM parsing   0%%");
        fflush(stdout);
    }

    // Create XML Parser
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetElementHandler(parser, osmStartElement, osmEndElement);
    XML_SetUserData(parser, osm);

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
            fprintf(stdout,"\r OSM parsing % 3i%%", (int)((read*100)/size));
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

    // No bounds set
    if(osm->minlon == -190 || osm->minlat == -190 ||
       osm->maxlon == -190 || osm->maxlat == -190) {
        
        osm->minlon = 360.0;
        osm->minlat = 180.0;
        osm->maxlon = -360.0;
        osm->maxlat = -180.0;

        osmNode *node;
        LIST_FOREACH(node, osm->nodes) {
            if(node->lon < osm->minlon)
                osm->minlon = node->lon;
            if(node->lat < osm->minlat)
                osm->minlat = node->lat;
            if(node->lon > osm->maxlon)
                osm->maxlon = node->lon;
            if(node->lat > osm->maxlat)
                osm->maxlat = node->lat;
        }
    }

    g_hash_table_destroy(osm->nodeidx);
    osm->nodeidx=NULL;
    
    if (opts->debug > 0)
        fprintf(stdout,"\r OSM parsing done. (%i/%i/%i/%i) [%fs]\n",
                osm->nodecnt, osm->waycnt, cntTag, cntNd,
                g_timer_elapsed(tOsmRead,NULL));
    
    g_timer_destroy(tOsmRead);

    return(osm);
}

void osmFree(osmFile *osm) {
    osmWay *way, *lway;
    osmNode *node, *lnode;
    osmTag *tag, *ltag;
    for(way = osm->ways, lway = NULL;
        way != NULL;
        lway = way, way = way->next)
    {
        g_slist_free(way->nd);
        for(tag = way->tag, ltag = NULL;
            tag != NULL;
            ltag = tag, tag = tag->next)
        {
            if(ltag)
                g_free(ltag);
        }
        if(ltag)
            g_free(ltag);
        if(lway)
            g_free(lway);
    }
    g_free(lway);
    
    for(node = osm->nodes, lnode = NULL;
        node != NULL;
        lnode = node, node = node->next)
    {
        for(tag = node->tag, ltag = NULL;
            tag != NULL;
            ltag = tag, tag = tag->next)
        {
            if(ltag)
                g_free(ltag);
        }
        if(ltag)
            g_free(ltag);
        if(lnode)
            g_free(lnode);
    }
    g_free(lnode);
    g_free(osm);
};

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

