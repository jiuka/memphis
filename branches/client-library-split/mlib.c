/*
 * MLib - GLib additions
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
#include <string.h>

/*
 *
 */
gchar* m_string_chunk_get(GStringChunk *chunk, GTree *tree,
                           const gchar *string) {

    char* lookup;

    g_return_val_if_fail(chunk != NULL, NULL);
    g_return_val_if_fail(tree != NULL, NULL);

    lookup = (char*) g_tree_lookup(tree, (gchar *)string);

    if (!lookup) {
        lookup = g_string_chunk_insert(chunk, string);
        g_tree_insert(tree, lookup, lookup);
    }

    return lookup;
}

gint m_tree_strcmp(gconstpointer  a, gconstpointer  b) {
    return strcmp((char *)a,(char *)b);
}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */
