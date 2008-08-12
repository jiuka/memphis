/*
 *
 */

#ifndef LIBOSM_H
#define LIBOSM_H
 
#define numTiles(z) exp2(z)
#define sec(x)  (1/cos(x))
#define degrees(a) (a*180.0/M_PI)
#define radians(a) (a*M_PI/180.0)

#define LAYERMIN     12
#define LAYERMAX     18

#define TILESIZE        256
#define LINESIZE(z)     (int)(exp2(z)/(z+1))


#define mercatorToLat(Y) degrees(atan(sinh(Y)))

/*
 * Types
 */
typedef struct _coordinates coordinates;
struct _coordinates {
        double x;
        double y;
    };

typedef struct _edges edges;
struct _edges {
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

#endif //LIBOSM_H

/*
 * vim: expandtab shiftwidth=4:
 */