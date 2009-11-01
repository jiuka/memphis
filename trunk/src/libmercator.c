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

#include <glib.h>
#include <math.h>

#include "libmercator.h"


coordinates coord2xy(double lat, double lon, int z, int tilesize) {
    coordinates result;
    result.x = numTiles(z) * tilesize * (lon + 180.0) / 360.0;
    result.y = numTiles(z) * tilesize * (1.0 - log(tan(radians(lat))
            + sec(radians(lat))) / G_PI) / 2.0;
    return result;
}

coordinates latlon2relativeXY(double lat, double lon) {
    coordinates result;
    result.x = (lon + 180) / 360;
    result.y = (1 - log(tan(radians(lat)) + sec(radians(lat))) / G_PI) / 2;

    return result;
}


coordinates latlon2xy(double lat, double lon, int z) {
    coordinates result;
    result.x = numTiles(z) * (lon + 180) / 360;
    result.y = numTiles(z) * (1 - log(tan(radians(lat))
            + sec(radians(lat))) / G_PI) / 2;

    return result;
}

coordinates latEdges(int y, int z) {
    coordinates result;

    float unit = 1 / numTiles(z);
    result.x = mercatorToLat(G_PI * (1 - 2 * (y * unit)));
    result.y = mercatorToLat(G_PI * (1 - 2 * (y * unit + unit)));

    return result;
}

coordinates lonEdges(int x, int z) {
    coordinates result;

    float unit = 360 / numTiles(z);
    result.x = -180 + (x * unit);
    result.y = -180 + (x * unit) + unit;

    return result;
}

edges tile2edges(int x, int y, int z) {
    edges result;
    coordinates ret;

    ret = latEdges(y,z);
    result.N = ret.x;
    result.S = ret.y;

    ret = lonEdges(x,z);
    result.W = ret.x;
    result.E = ret.y;

    return result;
}

/* converts 'slippy maps' tile number to lat & lon in degrees */
coordinates tile2latlon (int x, int y, int z) {
    coordinates ret; /* (lat_deg, lon_deg) */
    int n;
    double lat_rad;
    
    n = numTiles (z);
    ret.y = (double) x / (double) n * 360.0 - 180.0;
    lat_rad = atan (sinh (G_PI * (1.0 - 2.0 * (double) y / (double) n)));
    ret.x = lat_rad * 180.0 / G_PI;

    return ret; 
}

/* converts lon in degrees to a 'slippy maps' x tile number */
int lon2tilex (double lon_deg, int z) {
    double ret;
    ret = ((lon_deg + 180.0) / 360.0) * numTiles (z);

    return floor (ret);
}

/* converts lat in degrees to a 'slippy maps' y tile number */
int lat2tiley (double lat_deg, int z) {
    int n;
    double ret, lat_rad;
    
    n = numTiles (z);
    lat_rad = lat_deg * G_PI / 180.0;
    ret = (1.0 - (log (tan (lat_rad) + sec (lat_rad)) / G_PI))
            / 2.0 * n;

    return floor (ret);
}

/*
 * vim: expandtab shiftwidth=4 tabstop=4:
 */

