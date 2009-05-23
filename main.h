/*
 * Memphis - Cairo Rederer for OSM in C
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

#ifndef MAIN_H
#define MAIN_H

#define MEMPHIS_VERSION "0.1"

#define MEMPHIS_MIN_LAYER 12
#define MEMPHIS_MAX_LAYER 17

#define MEMPHIS_DEFAULT_OUTPUT_DIRECTORY "tiles"
/*
 * Enums
 */
typedef enum memphis_modes_e {
    MODE_MAP,                   /* Render howl bound. */
    MODE_TILE,                  /* Render given tile */
} memphis_modes_e;

typedef struct memphisOpt_ memphisOpt; 
struct memphisOpt_ {
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
        char            *outdir;    /* tiles output directory */
    };

#endif /* MAIN_H */

/*
 * vim: expandtab shiftwidth=4:
 */

