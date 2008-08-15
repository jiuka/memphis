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

#ifndef STRLIST_H
#define STRLIST_H

/*
 * LinkedList Struct for Strings
 */
typedef struct _strList strList;
struct _strList {
        char    *str;
        strList *next;
    };

#define STRLIST_GET(list,target,dest)                                       \
    do {                                                                    \
        if (list == NULL) {                                                 \
            list = malloc(sizeof(strList));                                 \
            list->str = strdup(target);                                     \
            list->next = NULL;                                              \
            (dest) = list->str;                                             \
        } else {                                                            \
            strList *iter, *last;                                           \
            for(iter=list,last=NULL; iter; last=iter,iter=iter->next) {     \
                int r = strcmp(target,iter->str);                           \
                if (r==0) {                                                 \
                    break;                                                  \
                }                                                           \
                if (r>0) {                                                  \
                    iter = malloc(sizeof(strList));                         \
                    iter->str = strdup(target);                             \
                    if (last) {                                             \
                        iter->next = last->next;                            \
                        last->next = iter;                                  \
                    } else {                                                \
                        iter->next = list;                                  \
                        list = iter;                                        \
                    }                                                       \
                    break;                                                  \
                }                                                           \
            }                                                               \
            if(iter == NULL) {                                              \
                iter = malloc(sizeof(strList));                             \
                iter->str = strdup(target);                                 \
                iter->next = last->next;                                    \
                last->next = iter;                                          \
            }                                                               \
            (dest) = iter->str;                                             \
        }                                                                   \
    } while (0)

#endif /* STRLIST_H */

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

