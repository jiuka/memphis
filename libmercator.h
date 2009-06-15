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

#ifndef LIBMERCATOR_H
#define LIBMERCATOR_H

#define numTiles(z) exp2(z)
#define sec(x)  (1/cos(x))
#define degrees(a) (a*180.0/G_PI)
#define radians(a) (a*G_PI/180.0)

//#define LINESIZE(z)     (int)(exp2(z-12)/(z-12+1))
//#define LINESIZE(z)     ((z)<=12 ? 1 : (z)<=14 ? 2 : (z)<=15 ? 3 : (z)<=16 ? 3 : (z) <= 17 ? 5 : (z)<=18 ? 6 : 6)
// FIXME: Improve scaling factors for zoom levels < 12
#define LINESIZE(z)     ((z)<12 ? 1 : (z)==18 ? 6 : (int)(exp2(z-12)/(z-12+1)))


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
coordinates coord2xy(double lat, double lon, int z, int tilesize);
coordinates latlon2relativeXY(double lat, double lon);
coordinates latlon2xy(double lat, double lon, int z);
coordinates latEdges(int y, int z);
coordinates lonEdges(int x, int z);
edges tileEdges(int x, int y, int z);
coordinates tile2latlon (int x, int y, int z);
int lon2tilex (double lon_deg, int z);
int lat2tiley (double lat_deg, int z);

#endif //LIBMERCATOR_H

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

