/* $Header: XpexCompute.c,v 2.3 91/09/11 15:51:18 sinyaw Exp $ */
/* 
 *	XpexCompute.c
 */

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

#include <stdio.h>
#include <math.h>
#include "Xpexlib.h"

#include "XpexErr.h"
#include "Xpexmatrixutil.h"

#define COINCIDE(pt1, pt2)			\
    (XPEX_NEAR_ZERO((pt1)->x - (pt2)->x) && 	\
     XPEX_NEAR_ZERO((pt1)->y - (pt2)->y) && 	\
     XPEX_NEAR_ZERO((pt1)->z - (pt2)->z))

static int
compute_normal(pts, normal)
	XpexCoord3D		pts[];
	XpexVector3D	*normal;
{
    XpexVector3D	u, v;
    float	mag;

    u.x = pts[1].x - pts[0].x;
    u.y = pts[1].y - pts[0].y;
    u.z = pts[1].z - pts[0].z;
    if (!XPEX_ZERO_MAG(mag = XPEX_MAG_V3(&u))) {
    	u.x /= mag;
    	u.y /= mag;
    	u.z /= mag;
    }

    v.x = pts[2].x - pts[0].x;
    v.y = pts[2].y - pts[0].y;
    v.z = pts[2].z - pts[0].z;
    if (!XPEX_ZERO_MAG(mag = XPEX_MAG_V3(&v))) {
    	v.x /= mag;
    	v.y /= mag;
    	v.z /= mag;
    }

    normal->x = u.y * v.z - u.z * v.y;
    normal->y = u.z * v.x - u.x * v.z;
    normal->z = u.x * v.y - u.y * v.x;

    mag = XPEX_MAG_V3(normal);
    if (XPEX_ZERO_MAG(mag)) {
	return(0); /* the points are colinear */
    }

    normal->x /= mag;
    normal->y /= mag;
    normal->z /= mag;

    return(1);

}

int
XpexComputeGeometricNormal(colorType, vertAttr, vertexLists, 
	numLists, geometricNormal)
	XpexColorType colorType;
	XpexBitmaskShort vertAttr;
	XpexListOfVertex  *vertexLists;
	int  numLists;
	XpexVector3D  *geometricNormal; /* RETURN */
{  
	int error_ind = 0;
#ifdef INCOMPLETE
    register int	i, j;
    int			pt_found = 0;
    XpexCoord3D	pt[3];
	XpexBitmaskShort vc = vertAttr & XpexGA_Color;
	XpexBitmaskShort vn = vertAttr & XpexGA_Normal;
    
	if (numLists < 1) {
		error_ind = 1;
    } else {
		if( !vc && !vn ) {
	    for (i = 0; i < numLists ; i++) {
	        pt[0] = vertexLists[i].vertices.pts[0].point;
			pt_found = 1;
	    	for (j = 1; j < vertexLists[i].numVertices ; j++) {
		    if (!COINCIDE(&pt[0], 
				&(vertexLists[i].vertices.pts[j].point))) {
		    	pt[1] = vertexLists[i].vertices.pts[j++].point;
				pt_found = 2;
		    	break;
		    }
		}
		if (pt_found == 2) {
		    for ( ; j < vertexLists[i].numVertices; j++ ) {
			pt[2] = vertexLists[i].vertices.pts[j].point;
			if (compute_normal(pt, geometricNormal)) {
			    pt_found = 3;
			    break;
			}
		    }
		}
		if (pt_found == 3)
		    break;
	    }
		if (pt_found < 3)
			error_ind = 1;
		} else if( vc && !vn ) {
	    for (i = 0; i < numLists ; i++) {
	        pt[0] = vertexLists[i].vertices.pc[0].p;
			pt_found = 1;
	    	for (j = 1; j < vertexLists[i].numVertices; j++) {
		    if (!COINCIDE(&pt[0], 
				&(vertexLists[i].vertices.pc[j].p))) {
		    	pt[1] = vertexLists[i].vertices.pc[j++].p;
				pt_found = 2;
		    	break;
		    }
		}
		if (pt_found == 2) {
		    for ( ; j < vertexLists[i].numVertices ; j++ ) {
			pt[2] = vertexLists[i].numVertices.pc[j].p;
			if (compute_normal(pt, geometricNormal)) {
			    pt_found = 3;
			    break;
			}
		    }
		}
		if (pt_found == 3)
		    break;
	    }
	    if (pt_found < 3)
			error_ind = 1;
	} else if( !vc && vn ) {
	    for (i = 0; i < numLists ; i++) {
	        pt[0] = vertexLists[i].vertices.pts_normal[0].point;
			pt_found = 1;
	    	for (j = 1; j < vertexLists[i].numVertices ; j++) {
		    if (!COINCIDE(&pt[0], 
				&(vertexLists[i].vertices.pts_normal[j].point))) {
		    	pt[1] = vertexLists[i].vertices.pts_normal[j++].point;
				pt_found = 2;
		    	break;
		    }
		}
		if (pt_found == 2) {
		    for ( ; j < vertexLists[i].numVertices; j++ ) {
			pt[2] = vertexLists[i].vertices.pts_normal[j].point;
			if (compute_normal(pt, geometricNormal)) {
			    pt_found = 3;
			    break;
			}
		    }
		}
		if (pt_found == 3)
		    break;
	    }
	    if (pt_found < 3)
			error_ind = 1;
	} else if( vc && vn ) {
	    for (i = 0; i < numLists ; i++) {
	        pt[0] = vertexLists[i].vertices.pcn[0].p;
			pt_found = 1;
	    	for (j = 1; j < vertexLists[i].num_vertices; j++) {
		    if (!COINCIDE(&pt[0], 
				&(vertexLists[i].vertices.pcn[j].p))) {
		    	pt[1] = vertexLists[i].vertices.pcn[j++].p;
				pt_found = 2;
		    	break;
		    }
		}
		if (pt_found == 2) {
		    for ( ; j < vertexLists[i].numVertices ; j++ ) {
			pt[2] = vertexLists[i].vertices.pcn[j].p;
			if (compute_normal(pt, geometricNormal)) {
			    pt_found = 3;
			    break;
			}
		    }
		}
		if (pt_found == 3) {
		    break;
		}
	    } /* end-for */
	    if (pt_found < 3)
			error_ind = 1;
		}
	}
#endif /* INCOMPLETE */
	return error_ind;
}
