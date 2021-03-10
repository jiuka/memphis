/*
 * Memphis - Cairo Rederer for OSM in C
 * Copyright (C) 2008  Marius Rieder <marius.rieder@durchmesser.ch>
 * Copyright (C) 2009  Simon Wenner <simon@wenner.ch>
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
#include <time.h>
#include <expat.h>
#include <string.h>
#include <locale.h>

#include "mlib.h"
#include "osm05.h"
#include "memphis-data-pool.h"
#include "memphis-debug.h"
#include "memphis-private.h"

#define BUFFSIZE 1024

typedef struct mapUserData_ mapUserData;
struct mapUserData_ {
    // Pointers to work with
    osmTag *cTag;
    osmNode *cNode;
    osmWay *cWay;
    MemphisDataPool *pool;
    // Collected Data
    osmFile *osm;
    // Counts (used for debugging only!)
    int cntTag;
    int cntNd;
};

osmFile * osmNew();

static void
way_list_prepend(osmWay *way, osmWay **list)
{
    way->next = *list;
    *list = way;
}

static void
node_list_prepend(osmNode *node, osmNode **list)
{
    node->next = *list;
    *list = node;
}

static void
tag_list_insert_key(osmTag *tag, osmTag **list)
{
    g_assert (tag != NULL);

    osmTag *curr, *prev;

    for (curr = *list, prev = NULL; curr != NULL;
            prev = curr, curr = curr->next)
    {
        if (strcmp(curr->key, tag->key) > 0)
            break;
    }
    tag->next = curr;
    if (prev)
        prev->next = tag;
    else
        *list = tag;
}

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
    mapUserData *data = (mapUserData *) userData;
    osmFile *osm = data->osm;
    GStringChunk *string_chunk = memphis_data_pool_get_string_chunk (data->pool);
    GTree *string_tree = memphis_data_pool_get_string_tree (data->pool);
    
    memphis_debug ("osm05startElement");
    // Parsing Bounds
    if (strncmp((char *) name, "bounds", 6) == 0) {
        memphis_debug ("Parsing Bounds");
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
        memphis_debug ("Parsing Node");
        data->cNode = g_new(osmNode, 1);
        while (*atts != NULL) {
            if(strcmp((char *) *(atts), "id") == 0) {
                sscanf((char *) *(atts+1),"%i",&data->cNode->id);
            } else if(strcmp((char *) *(atts), "lat") == 0) {
                sscanf((char *) *(atts+1),"%f",&data->cNode->lat);
            } else if(strcmp((char *) *(atts), "lon") == 0) {
                sscanf((char *) *(atts+1),"%f",&data->cNode->lon);
            }
            atts+=2;
        }

        data->cNode->tag = NULL;
        data->cNode->layer = 0;

        // Insert Node
        osm->nodecnt++;
        g_hash_table_insert(osm->nodeidx, &data->cNode->id, data->cNode);
        node_list_prepend(data->cNode, &osm->nodes);

        memphis_debug ("NODE: %i %f %f", data->cNode->id,
                data->cNode->lat, data->cNode->lon);
    }
    // Parsing Tags
    else if (strncmp((char *) name, "tag", 4) == 0) {
        memphis_debug ("Parsing Tag");

        if (!data->cNode && !data->cWay) // End if there is nothing to add the tag to
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
                    if (data->cNode)
                        sscanf((char *) *(atts+1),"%hi",& data->cNode->layer);
                    else if (data->cWay)
                        sscanf((char *) *(atts+1),"%hi",& data->cWay->layer);
                    return;
                } else if(strcmp(k, "name") == 0) {
                    if (data->cWay) {
                        data->cWay->name = m_string_chunk_get(string_chunk, string_tree, 
                                                        (char *) *(atts+1));
                    }
                    return;
                }
                v = (char *) *(atts+1);
            }
            atts += 2;
        }

        data->cTag = g_new(osmTag, 1);
        data->cTag->key = m_string_chunk_get(string_chunk, string_tree, k);
        data->cTag->value = m_string_chunk_get(string_chunk, string_tree, v);

        memphis_debug ("Tag: %s => %s", data->cTag->key, data->cTag->value);

        data->cntTag++;
        if (data->cNode)
            tag_list_insert_key(data->cTag, &data->cNode->tag);
        else if (data->cWay)
            tag_list_insert_key(data->cTag, &data->cWay->tag);

        data->cTag = NULL;
    }
    // Parsing Way
    else if (strncmp((char *) name, "way", 3) == 0) {
        memphis_debug ("Parsing Way");
        data->cWay = g_new(osmWay, 1);
        while (*atts != NULL) {
            if(strncmp((char *) *(atts), "id", 2) == 0) {
                sscanf((char *) *(atts+1), "%i", &data->cWay->id);
                break;
            }
            atts+=2;
        }

        data->cWay->tag = NULL;
        data->cWay->nd = NULL;
        data->cWay->name = NULL;
        data->cWay->layer = 0;

        // Insert Way
        osm->waycnt++;
        way_list_prepend(data->cWay, &osm->ways);

        memphis_debug ("WAY(%i)", data->cWay->id);
    }
    // Parsing WayNode
    else if (strncmp((char *) name, "nd", 2) == 0) {
        memphis_debug ("Parsing Nd");
        int ref = 0;
        while (*atts != NULL) {
            if(strncmp((char *) *(atts), "ref", 2) == 0) {
                sscanf((char *) *(atts+1),"%i",&ref);
                break;
            }
            atts+=2;
        }

        if (ref) {
            data->cntNd++;
            osmNode *n;
            
            n = g_hash_table_lookup(osm->nodeidx, &ref);
            if (!n) {
                g_warning ("No node with reference %d found!", ref);
                return;
            }

            // Insert WayNode
            data->cWay->nd = g_slist_prepend(data->cWay->nd, n);

            memphis_debug (" ND( %f %f )", n->lat, n->lon);

            data->cNode = NULL;
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
    mapUserData *data = (mapUserData *) userData;
    
    memphis_debug ("osm05endElement");
    if (strncmp((char *) name, "node", 4) == 0) {
        data->cNode = NULL;
    } else if (strncmp((char *) name, "way", 3) == 0) {
        if (data->cWay->nd != NULL)
            data->cWay->nd = g_slist_reverse(data->cWay->nd);
        data->cWay = NULL;
    }
}

/**
 * rulesetRead
 */
osmFile* osmRead(const char *filename, GError **error) {
    memphis_debug ("osmRead");

    // Reset the locale, otherwise we get in trouble if we convert a string to double
    // in a language that uses strange decimal characters (like German).
    setlocale(LC_ALL, "C");

    // Local Vars
    GTimer *tOsmRead = g_timer_new();
    unsigned int size;
    unsigned int read = 0;
    struct stat filestat;
    int len;
    int done;
    char *buf;
    osmFile *osm;
    mapUserData *data;
    
    // Test file
    if (!g_file_test (filename, G_FILE_TEST_IS_REGULAR)) {
        g_warning ("Error: \"%s\" is not a file.", filename);
        g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_NOENT,
                "Failed to open file: %s", filename);
        return NULL;
    }
    
    g_stat(filename, &filestat);
    size = (int) filestat.st_size;

    // Open file
    FILE *fd = fopen(filename, "r");
    if(fd == NULL) {
        g_warning ("Error: Can't open file \"%s\"", filename);
        g_set_error (error, G_FILE_ERROR, G_FILE_ERROR_ACCES,
                "Failed to open file: %s", filename);
        return NULL;
    }

    // Init vars
    data = g_new(mapUserData, 1);
    data->cTag = NULL;
    data->cNode = NULL;
    data->cWay = NULL;
    data->pool = memphis_data_pool_new ();
    data->cntTag = 0;
    data->cntNd = 0;

    data->osm = osm = osmNew ();

    if (G_UNLIKELY (memphis_debug_get_print_progress ())) {
        g_fprintf (stdout, " OSM parsing   0%%");
        fflush(stdout);
    }
    int progress = 0;

    // Create XML Parser
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetElementHandler(parser, osmStartElement, osmEndElement);
    XML_SetUserData(parser, data);

    // Create Buffer
    buf = g_malloc(BUFFSIZE*sizeof(char));

    // Looping over XML
    while(!feof(fd)) {
        len = (int)fread(buf, 1, BUFFSIZE, fd);
        if (ferror(fd)) {
            g_warning ("OSM read error");
            g_set_error (error, G_MARKUP_ERROR, G_MARKUP_ERROR_PARSE,
                    "Failed to parse file: %s", filename);
            // cleanup
            XML_ParserFree(parser);
            g_free(buf);
            fclose(fd);
            g_free(data);
            osmFree(osm);
            return NULL;
        }
        read += len;
        if (G_UNLIKELY (memphis_debug_get_print_progress ())) {
            int new_progress = (int)((read * 100.0) / size);
            if (new_progress > progress) {
                g_fprintf (stdout, "\r OSM parsing % 3i%%", new_progress);
                fflush(stdout);
                progress = new_progress;
            }
        }

        done = len < sizeof(buf);
        if (XML_Parse(parser, buf, len, done) == XML_STATUS_ERROR) {
            g_warning ("OSM parse error at line %i: %s",
                    (int) XML_GetCurrentLineNumber(parser),
                    XML_ErrorString(XML_GetErrorCode(parser)));
            g_set_error (error, G_MARKUP_ERROR, G_MARKUP_ERROR_PARSE,
                    "OSM parse error at line %i: %s",
                    (int) XML_GetCurrentLineNumber(parser), filename);
            // cleanup
            XML_ParserFree(parser);
            g_free(buf);
            fclose(fd);
            g_free(data);
            osmFree(osm);
            return NULL;
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
        for(node = osm->nodes; node != NULL; node = node->next) {
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
    osm->nodeidx = NULL;

    if (G_UNLIKELY (memphis_debug_get_print_progress ()))
        g_fprintf (stdout, "\r OSM parsing done. (%i/%i/%i/%i) [%fs]\n",
                osm->nodecnt, osm->waycnt, data->cntTag, data->cntNd,
                g_timer_elapsed(tOsmRead, NULL));
    
    g_timer_destroy(tOsmRead);
    g_free(data);

    return osm;
}

osmFile* osmRead_from_buffer (const char *buffer, guint size,
        GError **error)
{
    memphis_debug ("osmRead");

    // Reset the locale, otherwise we get in trouble if we convert a string to double
    // in a language that uses strange decimal characters (like German).
    setlocale(LC_ALL, "C");

    g_assert (buffer != NULL && size > 0);

    // Local Vars
    GTimer *tOsmRead = g_timer_new();
    int isDone = 0;
    osmFile *osm;
    mapUserData *data;

    // Init vars
    data = g_new(mapUserData, 1);
    data->cTag = NULL;
    data->cNode = NULL;
    data->cWay = NULL;
    data->pool = memphis_data_pool_new ();
    data->cntTag = 0;
    data->cntNd = 0;

    data->osm = osm = osmNew ();

    if (G_UNLIKELY (memphis_debug_get_print_progress ())) {
        g_fprintf (stdout, " OSM parsing ...");
        fflush(stdout);
    }

    // Create XML Parser
    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetElementHandler(parser, osmStartElement, osmEndElement);
    XML_SetUserData(parser, data);

    // Parse the buffer
    if (XML_Parse (parser, buffer, size, isDone) == XML_STATUS_ERROR) {
        g_warning ("OSM parse error at line %iu:\n%s",
                (int) XML_GetCurrentLineNumber(parser),
                XML_ErrorString(XML_GetErrorCode(parser)));
        g_set_error (error, G_MARKUP_ERROR, G_MARKUP_ERROR_PARSE,
                "OSM parse error at line %i",
                (int) XML_GetCurrentLineNumber(parser));
        // cleanup
        XML_ParserFree(parser);
        g_free(data);
        osmFree(osm);
        return NULL;
    }

    // Cleaning Memory
    XML_ParserFree(parser);
    g_free(data);

    // No bounds set
    if(osm->minlon == -190 || osm->minlat == -190 ||
        osm->maxlon == -190 || osm->maxlat == -190) {

        osm->minlon = 360.0;
        osm->minlat = 180.0;
        osm->maxlon = -360.0;
        osm->maxlat = -180.0;

        osmNode *node;
        for (node = osm->nodes; node != NULL; node = node->next) {
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
    osm->nodeidx = NULL;

    if (G_UNLIKELY (memphis_debug_get_print_progress ()))
        g_fprintf (stdout, "\r OSM parsing done. (%i/%i/%i/%i) [%fs]\n",
                osm->nodecnt, osm->waycnt, data->cntTag, data->cntNd,
                g_timer_elapsed(tOsmRead, NULL));

    g_timer_destroy(tOsmRead);

    return osm;
}

osmFile * osmNew()
{
    osmFile * osm = g_new(osmFile, 1);
    osm->nodes = NULL;
    osm->nodeidx = g_hash_table_new(g_int_hash, g_int_equal);
    osm->nodecnt = 0;
    osm->ways = NULL;
    osm->waycnt = 0;
    osm->minlon = -190;
    osm->minlat = -190;
    osm->maxlon = -190;
    osm->maxlat = -190;
    return osm;
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
}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

