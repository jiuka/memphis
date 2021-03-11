/*
 * Memphis - Cairo Rederer for OSM in C
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

/**
 * SECTION:memphis-debug
 * @short_description: Debugging helper functions.
 *
 * These debug messages can be enabled with ./configure --enable-debug.
 */

#include "memphis-debug.h"

gboolean memphis_print_progress_messages = FALSE;

/**
 * memphis_debug_set_print_progress:
 * @show_progress: progress
 *
 * Define if progress messages should be printed.
 *
 * Since: 0.1
 */
void memphis_debug_set_print_progress (gboolean show_progress)
{
    memphis_print_progress_messages = show_progress;
}

/**
 * memphis_debug_get_print_progress:
 *
 * Returns: if progress messages should be shown.
 *
 * Since: 0.1
 */
gboolean memphis_debug_get_print_progress ()
{
    return memphis_print_progress_messages;
}

/**
 * memphis_info:
 * @format: a printf format string
 * @...: optional arguments
 *
 * Prints stuff.
 *
 * Since: 0.1
 */
void
memphis_info (const gchar *format, ...)
{
    if (G_UNLIKELY (memphis_debug_get_print_progress ())) {
        va_list args;
        va_start (args, format);
        g_logv (G_LOG_DOMAIN, G_LOG_LEVEL_INFO, format, args);
        va_end (args);
    }
}
