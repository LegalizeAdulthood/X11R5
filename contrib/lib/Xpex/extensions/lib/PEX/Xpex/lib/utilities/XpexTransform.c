/* $Header: XpexTransform.c,v 2.2 91/09/11 15:51:09 sinyaw Exp $ */
/* 
 *	XpexTransform.c
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

int
XpexTransformPoint(point, transformationMatrix, transformedPoint)
	register XpexCoord3D  *point;
	register XpexMatrix  transformationMatrix;
	XpexCoord3D  *transformedPoint;   /* RETURN */
{
    /* TODO: need error code for w = 0. */
    register XpexFloat      w;      /* homogeneous coordinate */
	int error_ind;

    if (XPEX_NEAR_ZERO( w = transformationMatrix[3][0] * 
	point->x + transformationMatrix[3][1] * 
	point->y + transformationMatrix[3][2] *
	point->z + transformationMatrix[3][3])) {
        error_ind = 1;
    } else {
        error_ind = 0;
        w = 1.0 / w;
        transformedPoint->x = w * (transformationMatrix[0][0] * 
		point->x + transformationMatrix[0][1] * 
		point->y + transformationMatrix[0][2] * 
		point->z + transformationMatrix[0][3]);
        transformedPoint->y = w * (transformationMatrix[1][0] * 
		point->x + transformationMatrix[1][1] * 
		point->y + transformationMatrix[1][2] * 
		point->z + transformationMatrix[1][3]);
        transformedPoint->z = w * (transformationMatrix[2][0] * 
		point->x + transformationMatrix[2][1] * 
		point->y + transformationMatrix[2][2] * 
		point->z + transformationMatrix[2][3]);
    }
	return error_ind;
}

int
XpexTransformPoint2D(point, transformationMatrix, transformedPoint)
	register XpexCoord2D  *point;
	register XpexMatrix3X3  transformationMatrix; 
	XpexCoord2D  *transformedPoint;  /* RETURN */
{
    /* TODO: need error code for w = 0. */
    register XpexFloat      w;      /* homogenous coordinate */
	int error_ind;

    if (XPEX_NEAR_ZERO( w = transformationMatrix[2][0] * point->x + 
	transformationMatrix[2][1] * point->y + 
	transformationMatrix[2][2])) {
        error_ind = 1;
    } else {
        error_ind = 0;
        w = 1.0 / w;
        transformedPoint->x = w * (transformationMatrix[0][0] * 
		transformedPoint->x + transformationMatrix[0][1] * 
		transformedPoint->y + transformationMatrix[0][2]);
        transformedPoint->y = w * (transformationMatrix[1][0] * 
		transformedPoint->x + transformationMatrix[1][1] * 
		transformedPoint->y + transformationMatrix[1][2]);
    }
	return error_ind;
}
