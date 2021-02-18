/* $Header: XpexCompose.c,v 2.2 91/09/11 15:51:16 sinyaw Exp $ */
/* 
 *	XpexCompose.c
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

/*
 * Multiply matrices: m = a * b;
 *
 * This simple-minded explicit form is about 40% faster than
 * nested loops and indexes.
 */

static void
_XpexMatMul(m , a, b)
	XpexMatrix	m;
	XpexMatrix	a;
	register	XpexMatrix	b;
{
    register XpexFloat	*r;
    register int	i;

    
    if ((m != a) && (m != b)) {
	for (i = 0; i < 4; i++) {
	    r = a[i];
	    m[i][0] = r[0]*b[0][0] + r[1]*b[1][0] + r[2]*b[2][0] +r[3]*b[3][0];
	    m[i][1] = r[0]*b[0][1] + r[1]*b[1][1] + r[2]*b[2][1] +r[3]*b[3][1];
	    m[i][2] = r[0]*b[0][2] + r[1]*b[1][2] + r[2]*b[2][2] +r[3]*b[3][2];
	    m[i][3] = r[0]*b[0][3] + r[1]*b[1][3] + r[2]*b[2][3] +r[3]*b[3][3];
	}
    }
    else {
		XpexMatrix	t;
		register	XpexFloat       *tp, *mp;
	
	for (i = 0; i < 4; i++) {
	    r = a[i];
	    t[i][0] = r[0]*b[0][0] + r[1]*b[1][0] + r[2]*b[2][0] +r[3]*b[3][0];
	    t[i][1] = r[0]*b[0][1] + r[1]*b[1][1] + r[2]*b[2][1] +r[3]*b[3][1];
	    t[i][2] = r[0]*b[0][2] + r[1]*b[1][2] + r[2]*b[2][2] +r[3]*b[3][2];
	    t[i][3] = r[0]*b[0][3] + r[1]*b[1][3] + r[2]*b[2][3] +r[3]*b[3][3];
	}
	tp = &t[0][0];
	mp = &m[0][0];
	for (i = 0; i < 16; i++)
	    *mp++ = *tp++;
    }
}

static void
_XpexMatMul3X3(m, a, b)
	XpexMatrix3X3	m;	/* OUT result matrix	*/
	XpexMatrix3X3	a;      /* matrix a	*/
	register	XpexMatrix3X3	b;	/* matrix b	*/
{
    register float	*r;
    register int	i;

    if ((m != a) && (m != b)) {
	for (i = 0; i < 3; i++) {
	    r = a[i];
	    m[i][0] = r[0]*b[0][0] + r[1]*b[1][0] + r[2]*b[2][0];
	    m[i][1] = r[0]*b[0][1] + r[1]*b[1][1] + r[2]*b[2][1];
	    m[i][2] = r[0]*b[0][2] + r[1]*b[1][2] + r[2]*b[2][2];
	}
    }
    else {
		XpexMatrix3X3	t;
		register	float	*tp, *mp;
	
	for (i = 0; i < 3; i++) {
	    r = a[i];
	    t[i][0] = r[0]*b[0][0] + r[1]*b[1][0] + r[2]*b[2][0];
	    t[i][1] = r[0]*b[0][1] + r[1]*b[1][1] + r[2]*b[2][1];
	    t[i][2] = r[0]*b[0][2] + r[1]*b[1][2] + r[2]*b[2][2];
	}
	tp = &t[0][0];
	mp = &m[0][0];
	for (i = 0; i < 9; i++)
	    *mp++ = *tp++;
    }
}

void
XpexComposeMatrix(a, b, m)
    XpexMatrix    a;              /* matrix a     */
    XpexMatrix    b;              /* matrix b     */
    XpexMatrix    m;              /* OUT result matrix    */
{
    _XpexMatMul(m, a, b);
}

void
XpexComposeMatrix2D(a, b, m)
    XpexMatrix3X3     a;              /* matrix a     */
    XpexMatrix3X3     b;              /* matrix b     */
    XpexMatrix3X3     m;              /* OUT result matrix    */
{
    _XpexMatMul3X3(m, a, b);
}

void
XpexComposeTransformationMatrix(m, 
	pt, shift, x_ang, y_ang, z_ang, scale, 
	result)
    XpexMatrix      m;              /* transformation matrix        */
    XpexCoord3D     *pt;            /* fixed point  */
    XpexVector3D    *shift;         /* shift vector */
    XpexFloat       x_ang;          /* rotation angle X     */
    XpexFloat       y_ang;          /* rotation angle Y     */
    XpexFloat       z_ang;          /* rotation angle Z     */
    XpexVector3D    *scale;         /* scale vector */
    XpexMatrix      result;         /* OUT transformation matrix    */
{
	extern void _XpexBuildTransform();

    XpexMatrix    xform;

    _XpexBuildTransform(pt, shift, x_ang, y_ang, z_ang, scale, xform);
    /* Assuming pre-multiplication of old by new. */
    _XpexMatMul(result, xform, m);
}

void
XpexComposeTransformationMatrix2D(m, pt, shift, angle, scale, 
	result)
    XpexMatrix3X3    m;      /* transformation matrix        */
    XpexCoord2D      *pt;    /* fixed point  */
    XpexVector2D     *shift; /* shift vector */
    XpexFloat        angle;  /* rotation angle       */
    XpexVector2D     *scale; /* scale vector */
    XpexMatrix3X3    result; /* OUT transformation matrix    */
{
	extern void _XpexBuildTransform2D();

    XpexMatrix3X3     xform;

    _XpexBuildTransform2D(pt, shift, angle, scale, xform);
    /* Assuming pre-multiplication of old by new. */
    _XpexMatMul3X3(result, xform, m);
}
