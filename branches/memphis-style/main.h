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

#ifndef MAIN_H
#define MAIN_H

#define MEMPHIS_VERSION "0.1"

#define MEMPHIS_MIN_LAYER 12
#define MEMPHIS_MAX_LAYER 17

/*
 * Enums
 */
typedef enum memphis_modes_e {
    MODE_MAP,                   /* Render howl bound. */
    MODE_TILE,                  /* Render given tile */
} memphis_modes_e;

typedef struct _memphisOpt memphisOpt; 
struct _memphisOpt {
        memphis_modes_e mode;   /* render mode */
        short int       debug;  /* 0: Be quiet
                                 * 1: Normal Output (Default)
                                 * 2: Be verbose */
        short int       tile_x;
        short int       tile_y;
        short int       minlayer;
        short int       maxlayer;
        char            *cfgfn;
        char            *osmfn;
    };

#endif /* MAIN_H */

/*
 * vim: expandtab shiftwidth=4:
 */
