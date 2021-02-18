/* $Header: XpexScale.c,v 2.2 91/09/11 15:51:25 sinyaw Exp $ */
/* 
 *	XpexScale.c
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

void
XpexScale(scaleVector, transformationMatrix)
    XpexVector3D  *scaleVector; 
    register XpexMatrix  transformationMatrix;
{
    transformationMatrix[0][0] = scaleVector->x;
    transformationMatrix[1][1] = scaleVector->y;
    transformationMatrix[2][2] = scaleVector->z;
    transformationMatrix[3][3] = 1.0;
    transformationMatrix[0][1] = transformationMatrix[0][2] = 
	transformationMatrix[0][3] = transformationMatrix[1][0] = 
	transformationMatrix[1][2] = transformationMatrix[1][3] = 
	transformationMatrix[2][0] = transformationMatrix[2][1] = 
	transformationMatrix[2][3] = transformationMatrix[3][0] = 
	transformationMatrix[3][1] = transformationMatrix[3][2] = 0.0;
}

void
XpexScale2D(scaleVector, transformationMatrix)
    XpexVector2D  *scaleVector; 
    register XpexMatrix3X3  transformationMatrix;
{
    transformationMatrix[0][0] = scaleVector->x;
    transformationMatrix[1][1] = scaleVector->y;
    transformationMatrix[2][2] = 1.0;
    transformationMatrix[0][1] = transformationMatrix[0][2] = 
	transformationMatrix[1][0] = transformationMatrix[1][2] = 
	transformationMatrix[2][0] = transformationMatrix[2][1] = 0.0;
}
