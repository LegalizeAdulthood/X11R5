/* $Header: XpexTranslate.c,v 2.2 91/09/11 15:51:27 sinyaw Exp $ */
/* 
 * XpexTranslate.c
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
XpexTranslate(trans_vector, m)
    XpexVector3D          *trans_vector; /* translation vector */
    register XpexMatrix   m;             /* OUT transformation matrix */
{
        m[0][3] = trans_vector->x;
        m[1][3] = trans_vector->y;
        m[2][3] = trans_vector->z;
        m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0;
        m[0][1] = m[0][2] = m[1][0] = m[1][2] = m[2][0] = m[2][1]
            = m[3][0] = m[3][1] = m[3][2] = 0.0;
}

void
XpexTranslate2D(trans_vector, m)
    XpexVector2D  *trans_vector;  /* translation vector   */
    register XpexMatrix3X3 m;  /* OUT transformation matrix    */
{
        m[0][2] = trans_vector->x;
        m[1][2] = trans_vector->y;
        m[0][0] = m[1][1] = m[2][2] = 1.0;
        m[0][1] = m[1][0] = m[2][0] = m[2][1] = 0.0;
}
