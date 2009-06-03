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

#ifndef LIBMERCATOR_H
#define LIBMERCATOR_H

#define numTiles(z) exp2(z)
#define sec(x)  (1/cos(x))
#define degrees(a) (a*180.0/G_PI)
#define radians(a) (a*G_PI/180.0)

#define TILESIZE        256
#define LINESIZE(z)     (int)(exp2(z-12)/(z-12+1))


#define mercatorToLat(Y) degrees(atan(sinh(Y)))

/*
 * Types
 */
typedef struct coordinates_ coordinates;
struct coordinates_ {
        double x;
        double y;
    };

typedef struct edges_ edges;
struct edges_ {
        double N;
        double E;
        double S;
        double W;
    };

/*
 * Function Prototype
 */
coordinates coord2xy(double lat, double lon, int z);
coordinates latlon2relativeXY(double lat, double lon);
coordinates latlon2xy(double lat, double lon, int z);
coordinates latEdges(int y, int z);
coordinates lonEdges(int x, int z);
edges tileEdges(int x, int y, int z);

#endif //LIBMERCATOR_H

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */
