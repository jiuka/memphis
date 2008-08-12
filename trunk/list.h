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


#ifndef LIST_H
#define LIST_H

#include "ruleset.h"

// Generic
#define LIST_FOREACH(iter,list) \
    for ((iter) = (list); (iter); (iter) = (iter)->next)
    
#define STRLL_GET(list,target,result)    \
    do {                                                        \
        LIST_FOREACH ((result), (list)) {                       \
            int i = strcmp((result)->str, (target));            \
            if (i > 0)                                          \
                (result) = NULL;                                \
            if (i >= 0)                                         \
                break;                                          \
        }                                                       \
    } while (0)
    
#define STRLL_ADD(list, node_)                                  \
    do {                                                        \
        __typeof__(list) _node = (node_);                   \
        __typeof__(_node) _ptr, _prev;                      \
        for (_ptr = (list), _prev = NULL; _ptr;         \
             _prev = _ptr, _ptr = _ptr->next            \
        ) {                                             \
            if (strcmp(_node->str, _ptr->str) < 0)      \
                break;                                  \
        }                                               \
        _node->next = _ptr;                             \
        if (_prev)                                      \
            _prev->next = _node;                        \
        else                                            \
            (list) = _node;                             \
    } while (0)
    
char* strlist_getpointer(cfgStr *list, char *str);

#endif /* LIST_H */

/*
 * vim: expandtab shiftwidth=4:
 */