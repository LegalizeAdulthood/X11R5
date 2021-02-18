/* $Header: XpexBuild.c,v 2.2 91/09/11 15:51:14 sinyaw Exp $ */

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
_XpexBuildTransform(pt, shift, ax, ay, az, scl, m)
    register XpexCoord3D    *pt;            /* fixed point  */
    XpexVector3D            *shift;         /* shift vector */
    float               ax;             /* rotation angle X     */
    float               ay;             /* rotation angle Y     */
    float               az;             /* rotation angle Z     */
    register XpexVector3D   *scl;           /* scale vector */
    XpexMatrix              m;              /* OUT transformation matrix    */
{
    /* Translate pt to the origin, scale, rotate, translate back to pt,
     * shift:
     *                  T * Tf~ * Rz * Ry * Rx * S * Tf.
     *
     *    where:        T is the "shift" transform,
     *                  Tf ia the translation of pt to the origin and
     *                  Tf~ is it's inverse,
     *                  Ri is the rotation transform about the i'th axis,
     *                  S is the scaling transform.
     */
    register float      *r;
    register float      cz, sz, cx, sx, cy, sy;

    cx = cos(ax); sx = sin(ax);
    cy = cos(ay); sy = sin(ay);
    cz = cos(az); sz = sin(az);

    r = m[0];
    r[0] = cz * cy * scl->x;
    r[1] = (cz * sx * sy - sz * cx) * scl->y;
    r[2] = (cz * sy * cx + sz * sx) * scl->z;
    r[3] = shift->x + pt->x - (r[0] * pt->x + r[1] * pt->y + r[2] * pt->z);
    r = m[1];
    r[0] = sz * cy * scl->x;
    r[1] = (sz * sx * sy + cz * cx) * scl->y;
    r[2] = (sz * sy * cx - cz * sx) * scl->z;
    r[3] = shift->y + pt->y - (r[0] * pt->x + r[1] * pt->y + r[2] * pt->z);
    r = m[2];
    r[0] = -sy * scl->x;
    r[1] = cy * sx * scl->y;
    r[2] = cy * cx * scl->z;
    r[3] = shift->z + pt->z - (r[0] * pt->x + r[1] * pt->y + r[2] * pt->z);
    r = m[3];
    r[0] = r[1] = r[2] = 0.0;
    r[3] = 1.0;
}

void
XpexBuildTransformationMatrix(pt, shift, 
	x_angle, y_angle, z_angle, scale, 
	matrix)
    XpexCoord3D *pt;            /* fixed point  */
    XpexVector3D *shift;         /* shift vector */
    XpexFloat x_angle;        /* rotation angle X     */
    XpexFloat y_angle;        /* rotation angle Y     */
    XpexFloat z_angle;        /* rotation angle Z     */
    XpexVector3D *scale;         /* scale vector */
    XpexMatrix matrix;         /* OUT transformation matrix    */
{
    _XpexBuildTransform(pt, shift, 
	x_angle, y_angle, z_angle, scale, matrix);
}

void
_XpexBuildTransform2D(pt, shift, ang, scl, m)
    XpexCoord2D      *pt;            /* fixed point  */
    XpexVector2D     *shift;         /* shift vector */
    float        ang;            /* rotation angle       */
    XpexVector2D     *scl;           /* scale vector */
    XpexMatrix3X3     m;              /* OUT transformation matrix    */
{
    /* Translate pt to the origin, scale, rotate, translate back to pt,
     * shift:
     *                  T * Tf~ * R * S * Tf.
     *
     *    where:        T is the "shift" transform,
     *                  Tf ia the translation of pt to the origin and
     *                  Tf~ is it's inverse,
     *                  R is the rotation transform,
     *                  S is the scaling transform.
     */
    register float      *r;
    register float      c, s;

    c = cos(ang); s = sin(ang);

    r = m[0];
    r[0] = c * scl->x;
    r[1] = -s * scl->y;
    r[2] = shift->x + pt->x - c * scl->x * pt->x + s * scl->y * pt->y;
    r = m[1];
    r[0] = s * scl->x;
    r[1] = c * scl->y;
    r[2] = shift->y + pt->y - (s * scl->x * pt->x + c * scl->y * pt->y);
    r = m[2];
    r[0] = r[1] = 0.0;
    r[2] = 1.0;
}

void
XpexBuildTransformationMatrix2D(pt, shift, angle, scale, matrix)
    XpexCoord2D      *pt;            /* fixed point  */
    XpexVector2D     *shift;         /* shift vector */
    XpexFloat        angle;          /* rotation angle       */
    XpexVector2D     *scale;         /* scale vector */
    XpexMatrix3X3    matrix;         /* OUT transformation matrix    */
{
    _XpexBuildTransform2D(pt, shift, angle, scale, matrix);
}
