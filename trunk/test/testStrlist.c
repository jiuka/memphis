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
#include <stdio.h>

#include "../strlist.h"

int main(int argc, char **argv) {
    
    strList *list;
    strListNode *node;
    
    list = strlist_init();
    
    strlist_get(list, "bString");
    strlist_get(list, "dString");
    
    for(node = list->next;node;node=node->next) {
        fprintf(stdout,"%s\n",node->str);
    }
    fprintf(stdout,"-------\n");
    
    strlist_get(list, "aString");
    strlist_get(list, "cString");
    strlist_get(list, "eString");
    
    for(node = list->next;node;node=node->next) {
        fprintf(stdout,"%s\n",node->str);
    }
    	
	return(0);
}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */
 