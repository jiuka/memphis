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

#include "memphis-debug.h"
#include "config.h"

gboolean memphis_print_progress_messages = FALSE;

void memphis_debug_set_print_progress (gboolean show_progress)
{
    memphis_print_progress_messages = show_progress;
}

gboolean memphis_debug_get_print_progress ()
{
    return memphis_print_progress_messages;
}

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

#ifdef ENABLE_DEBUG

void
memphis_debug (const gchar *format, ...)
{
    va_list args;
    va_start (args, format);
    g_logv (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, format, args);
    va_end (args);
}

#else

void
memphis_debug (const gchar *format, ...)
{
}

#endif /* ENABLE_DEBUG */
