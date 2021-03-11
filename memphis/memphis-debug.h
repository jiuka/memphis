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

#ifndef __MEMPHIS_DEBUG_H__
#define __MEMPHIS_DEBUG_H__

#include <glib.h>
#include <glib/gstdio.h>

G_BEGIN_DECLS

void memphis_info (const gchar *format, ...) G_GNUC_PRINTF (1, 2);

void memphis_debug_set_print_progress (gboolean show_progress);
gboolean memphis_debug_get_print_progress ();

G_END_DECLS

#endif /* __MEMPHIS_DEBUG_H__ */
