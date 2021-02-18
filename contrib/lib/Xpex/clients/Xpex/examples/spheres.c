/* $Header: spheres.c,v 2.2 91/09/11 16:05:18 sinyaw Exp $ */

/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

#ifndef lint
static char     sccsid[] = "@(#)spheres.c 3.1 90/05/29 Copyr 1989,1990  SMI/MIT";
#endif  lint

/***********************************************************
Copyright 1989,1990 by Sun Microsystems, Inc. and the X Consortium.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 * Copyright (c) 1989,1990 by M.I.T. and Sun Microsystems, Inc.
 */

/* tessellate a sphere with triangles

    method: divide into latitute and longitude lines.  This gives
    quadrilateral tesellation, then slash each quad to give triangles.
    Exception: first and last latitude rows are already triangles, with
    the N or S pole as a vertex

*/

#include <stdio.h>
#include <math.h>
#include "Xpexlib.h"

#define MAXLAT 100    /*max number of horiz and vert. divisions of sphere*/
#define MAXLONG 100

#define BIGNUM 999.0
#define PI 3.14159265358979
#define TORAD(x)    ((x)*PI/180.0)

typedef struct {
    XpexCoord3D    upleft, upright, downleft, downright;
} Quad;


static int
tesselate_sphere( radius, nlat, nlong, facets )
    double         radius;
    int            nlat, nlong;    /*number of horiz, vert quads*/
    XpexCoord3D    facets[][3];
{
    int       i, j, k, lat1, lat2, nfacets = 0;
    double    theta, deltay, deltatheta;
    float     sx, sy, sz, tx, ty, tz, rx, ry, rz;
    XpexCoord3D   p1, p2;
    XpexCoord3D   npole, spole;
    Quad     (*q)[MAXLONG];

    if ( nlat < 2 || 0 != nlat % 2 ) {
    fprintf( stderr, "num lats must be even and >=2\n");
    return -1;
    }
    if( nlat >= MAXLAT ) {
    fprintf( stderr, "current num lat limit is %d\n");
    return -2;
    }

    if( nlong < 3 ) {
    fprintf( stderr, "num long must be >= 3\n");
    return -3;
    }
    if( nlong >= MAXLONG ) {
    fprintf( stderr, "current num long limit is %d\n");
    return -4;
    }

    q = (Quad(*)[MAXLONG])malloc( nlat * MAXLONG * sizeof(Quad));
    if ( !q )
    return 0;

    npole.x = 0.0; npole.y = 1.0; npole.z = 0.0;
    spole.x = 0.0; spole.y = -1.0; spole.z = 0.0;
    deltay = 2.0/nlat;    /*y size of horiz. slices*/
    deltatheta = 2.0*PI/nlong;

    for(j=0; j<nlong; ++j){
    q[0][j].upleft =  npole;
    q[0][j].upright.x = BIGNUM;    /*this is a triangle, not a quad*/
    q[nlat-1][j].downleft = spole;
    q[nlat-1][j].downright.x = BIGNUM;
    }

    p1.x = p2.x = 1.0;
    p1.y = p2.y = p1.z = p2.z = 0.0;
    lat2 = nlat/2;
    lat1 = lat2-1;
    theta = 0.0;

    for(j=0; j<nlong; ++j){
    q[lat1][j].downright = q[lat2][j].upright = p1;
    if(j>0) 
        q[lat1][j-1].downleft = q[lat2][j-1].upleft = p1;
    theta += deltatheta;
    p1.x = radius*cos(theta);
    p1.z = radius*sin(theta);
    }
    q[lat1][nlong-1].downleft = q[lat2][nlong-1].upleft = q[lat1][0].downright;

    for(;;) {
    if( lat1 == 0 )
        break;
    --lat1; ++lat2;
    p1.y += deltay; p2.y -= deltay;
    radius = sqrt(1.0 - p1.y*p1.y);
    p1.x = p2.x = radius;
    p1.z = p2.z = 0.0;
    theta = 0.0;

    for(j=0; j<nlong; ++j){
        q[lat1][j].downright = q[lat1+1][j].upright = p1;
        q[lat2][j].upright = q[lat2-1][j].downright = p2;
        if(j>0){
        q[lat1][j-1].downleft = q[lat1+1][j-1].upleft= p1;
        q[lat2][j-1].upleft = q[lat2-1][j-1].downleft= p2;
        }
        theta += deltatheta;
        p1.x = p2.x = radius*cos(theta);
        p1.z = p2.z = radius*sin(theta);
    }
    q[lat1][nlong-1].downleft
        = q[lat1+1][nlong-1].upleft = q[lat1][0].downright;
    q[lat2][nlong-1].upleft
        = q[lat2-1][nlong-1].downleft = q[lat2][0].upright;
    }

    for( i=0; i < nlat; ++i ) {
    for( j = 0; j < nlong; ++j ) {
        if( q[i][j].upright.x == BIGNUM ) {
        facets[nfacets][0] = q[i][j].downleft;
        facets[nfacets][1] = q[i][j].downright;
        facets[nfacets][2] = q[i][j].upleft;
        ++nfacets;
        } else if( q[i][j].downright.x == BIGNUM ) {
        facets[nfacets][0] = q[i][j].downleft;
        facets[nfacets][1] = q[i][j].upleft;
        facets[nfacets][2] = q[i][j].upright;
        ++nfacets;
        } else {
        facets[nfacets][0] = q[i][j].downleft;
        facets[nfacets][1] = q[i][j].downright;
        facets[nfacets][2] = q[i][j].upleft;
        ++nfacets;
        facets[nfacets][0] = q[i][j].downright;
        facets[nfacets][1] = q[i][j].upleft;
        facets[nfacets][2] = q[i][j].upright;
        ++nfacets;
        }
    }
    }

    free(q);
    return nfacets;
}

int
facet_sphere( radius, num_lat, num_long, facetlist )
    float     radius;
    int       num_lat;
    int       num_long;
    XpexListOfCoord3D  **facetlist;
{
    XpexCoord3D    (*facets)[3];
    int        num_facets, farray_size, flist_size;
    char       *buf;
    register int        i;
    register XpexListOfCoord3D *cfp;

    num_facets = (2 + 2 * (num_lat - 2)) * num_long;
    farray_size = num_facets * 3 * sizeof(XpexCoord3D);
    flist_size = num_facets * sizeof(XpexListOfCoord3D);
    buf = (char *) malloc(flist_size + farray_size); 
    if ( buf ) { 
    *facetlist = (XpexListOfCoord3D*)buf;
    facets = (XpexCoord3D(*)[3])(buf + flist_size);
    num_facets = tesselate_sphere( radius, num_lat, num_long, facets);
    for ( i = 0, cfp = *facetlist; i < num_facets; i++, cfp++ ) {
        cfp->num_points = 3;
        cfp->points = facets[i];
    }
    }

    return num_facets;
}
