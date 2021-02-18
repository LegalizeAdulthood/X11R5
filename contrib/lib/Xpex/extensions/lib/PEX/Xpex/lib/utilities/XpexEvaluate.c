/* $Header: XpexEvaluate.c,v 2.2 91/09/11 15:51:20 sinyaw Exp $ */
/* 
 *	XpexEvaluate.c
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
#include "Xpexutil.h"

#include "XpexErr.h"
#include "Xpexmatrixutil.h"

int
XpexEvaluateViewMappingMatrix(map, m)
	XpexViewMap3D  *map;  /* view mapping */
	register XpexMatrix  m;  /* OUT view mapping matrix */
{
    /* Procedure:
      (Perspective):
         - Translate to PRP,                    Tc
         - Convert to left handed coords,       Tlr
         - Shear,                               H
         - Scale to canonical view volume,      S
         - Normalize perspective view volume,   Ntp
         - Scale to viewport,                   Svp
         - Convert to right handed coords,      Tlr
         - Translate to viewport,               Tvp

      (Parallel):
         - Shear,                               H
         - Translate window to origin,          Tl
         - Scale to canonical view volume,      S
         - Scale to viewport,                   Svp
         - Translate to viewport,               Tvp

         See pevalviewmappingmatrix3_debug for the matrices.
     */

    register XpexFloat     *r;
    register XpexCoord3D   *prp = &map->proj_ref_point;
    register XpexLimit3D   *vp = &map->viewport;
    register XpexLimit2D   *win = &map->window;

    /* These are ordered roughly by the number of times used, the most
     * used is first.  Those used twice or less aren't declared register.
     */
    register double     sz, sx, sy;
    register double     zf;
    register double     dx = vp->x_max - vp->x_min;
    register double     dy = vp->y_max - vp->y_min;
    register double     hx, hy;
    register double     d;
             double     dz = vp->z_max - vp->z_min;
             double     vvz = map->front_plane - map->back_plane;

	int error_ind = 0;

    if (!(win->x_min < win->x_max) || !(win->y_min < win->y_max)) {
        error_ind = ERR151;

    } else if (!(vp->x_min < vp->x_max) || !(vp->y_min < vp->y_max)
        || !(vp->z_min <= vp->z_max)) {
        error_ind = ERR152;

    } else if (XPEX_NEAR_ZERO( vvz) && vp->z_min != vp->z_max) {
        error_ind = ERR158;

    } else if (map->proj_type == Xpex_PERSPECTIVE
        && prp->z < map->front_plane && prp->z > map->back_plane) {
        error_ind = ERR162;

    } else if (prp->z == map->view_plane) {
        error_ind = ERR163;

    } else if (map->front_plane < map->back_plane) {
        error_ind = ERR164;

    } else if (!XPEX_IN_RANGE( XPEX_NPC_XMIN, XPEX_NPC_XMAX, vp->x_min)
        || !XPEX_IN_RANGE( XPEX_NPC_XMIN, XPEX_NPC_XMAX, vp->x_max)
        || !XPEX_IN_RANGE( XPEX_NPC_YMIN, XPEX_NPC_YMAX, vp->y_min)
        || !XPEX_IN_RANGE( XPEX_NPC_YMIN, XPEX_NPC_YMAX, vp->y_max)
        || !XPEX_IN_RANGE( XPEX_NPC_ZMIN, XPEX_NPC_ZMAX, vp->z_min)
        || !XPEX_IN_RANGE( XPEX_NPC_ZMIN, XPEX_NPC_ZMAX, vp->z_max)) {
        error_ind = ERR155;

    } else if (map->proj_type == Xpex_PERSPECTIVE) {
        error_ind = 0;
        d = prp->z - map->view_plane;
        sz = 1.0 / (prp->z - map->back_plane);
        sx = sz * d * 2.0 / (win->x_max - win->x_min);
        sy = sz * d * 2.0 / (win->y_max - win->y_min);
        hx = (prp->x - 0.5 * (win->x_min + win->x_max)) / d;
        hy = (prp->y - 0.5 * (win->y_min + win->y_max)) / d;

        r = m[0];
        r[0] = 0.5 * dx * sx;
        r[1] = 0.0;
        r[2] = -(0.5 * dx * (sx * hx + sz) + sz * vp->x_min);
        r[3] = -(0.5 * dx * sx * (prp->x - hx * prp->z)
	    - sz * prp->z * (0.5 * dx + vp->x_min));

        r = m[1];
        r[0] = 0.0;
        r[1] = 0.5 * dy * sy;
        r[2] = -(0.5 * dy * (sy * hy + sz) + sz * vp->y_min);
        r[3] = -(0.5 * dy * sy * (prp->y - hy * prp->z)
            - sz * prp->z * (0.5 * dy + vp->y_min));

        r = m[2];
        r[0] = r[1] = 0.0;
        zf = (prp->z - map->front_plane) / (prp->z - map->back_plane);
        if ( XPEX_NEAR_ZERO( 1.0 - zf)) {
            r[2] = 0.0;
            r[3] = sz * prp->z * vp->z_max;
        } else {
            r[2] = sz * ((dz / (1.0 - zf)) - vp->z_max);
            r[3] = sz * prp->z * vp->z_max - (dz/(1.0-zf)) * (sz * prp->z - zf);
        }

        r = m[3];
        r[0] = r[1] = 0.0;
        r[2] = -sz;
        r[3] = sz * prp->z;

    } else {    /* parallel */
        error_ind = 0;
        sx = dx / (win->x_max - win->x_min);
        sy = dy / (win->y_max - win->y_min);
        hx = (prp->x - 0.5 * (win->x_min + win->x_max))
            / (map->view_plane - prp->z);
        hy = (prp->y - 0.5 * (win->y_min + win->y_max))
            / (map->view_plane - prp->z);

        r = m[0];
        r[0] = sx;
        r[1] = 0.0;
        r[2] = sx * hx;
        r[3] = vp->x_min - sx * win->x_min;

        r = m[1];
        r[0] = 0.0;
        r[1] = sy;
        r[2] = sy * hy;
        r[3] = vp->y_min - sy * win->y_min;

        r  = m[2];
        r[0] = r[1] = 0.0;
        if ( XPEX_NEAR_ZERO(vvz))
            r[2] = 0.0;
        else
           r[2] = dz / vvz;
        r[3] = vp->z_min - r[2] * map->back_plane;

        r = m[3];
        r[0] = r[1] = r[2] = 0.0;
        r[3] = 1.0;
    }
	return error_ind;
}

int
XpexEvaluateViewMappingMatrix2D(map, m)
	XpexViewMap2D  *map;  /* view mapping */
	register XpexMatrix3X3  m;  /* OUT view mapping matrix */
{
    register XpexLimit2D   *win = &map->window;
    register XpexLimit2D   *vp = &map->viewport;
	int error_ind = 0;

    if (!(win->x_min < win->x_max) || !(win->y_min < win->y_max)) {
        error_ind = ERR151;

    } else if (!(vp->x_min < vp->x_max) || !(vp->y_min < vp->y_max)) {
        error_ind = ERR152;

    } else if (!XPEX_IN_RANGE( XPEX_NPC_XMIN, XPEX_NPC_XMAX, vp->x_min)
        || !XPEX_IN_RANGE( XPEX_NPC_XMIN, XPEX_NPC_XMAX, vp->x_max)
        || !XPEX_IN_RANGE( XPEX_NPC_YMIN, XPEX_NPC_YMAX, vp->y_min)
        || !XPEX_IN_RANGE( XPEX_NPC_YMIN, XPEX_NPC_YMAX, vp->y_max)) {
        error_ind = ERR155;

    } else {
        register XpexFloat  sx, sy;         /* scale factors: len(vp) / len(win) */

        error_ind = 0;
        sx = (vp->x_max - vp->x_min) / (win->x_max - win->x_min);
        sy = (vp->y_max - vp->y_min) / (win->y_max - win->y_min);
        m[0][0] = sx;   
	m[0][1] = 0.0;  
	m[0][2] = sx * (-win->x_min) + vp->x_min;
        m[1][0] = 0.0;  
	m[1][1] = sy;   
	m[1][2] = sy * (-win->y_min) + vp->y_min;
        m[2][0] = 0.0;  m[2][1] = 0.0;  m[2][2] = 1.0;
    }
	return error_ind;
}

int
XpexEvaluateViewOrientationMatrix(vrp, vpn, vup, m)
	XpexCoord3D  *vrp;  /* view reference point */
	register XpexVector3D  *vpn;  /* view plane normal    */
	XpexVector3D  *vup;  /* view up vector       */
	XpexMatrix  m;  /* OUT view orientation matrix  */
{
    /*  Translate to VRP then change the basis.
     *  The old basis is: e1 = < 1, 0, 0>,  e2 = < 0, 1, 0>, e3 = < 0, 0, 1>.
     * The new basis is: ("x" means cross product)
                e3' = VPN / |VPN|
                e1' = VUP x VPN / |VUP x VPN|
                e2' = e3' x e1'
     * Therefore the transform from old to new is x' = ATx, where:

             | e1' 0 |         | 1 0 0 -vrp.x |
         A = |       |,    T = | 0 1 0 -vrp.y |
             | e2' 0 |         | 0 0 1 -vrp.z |
             |       |         | 0 0 0    1   |
             | e3' 0 |
             |       |
             | -0-  1|
     */

    /* These ei's are really ei primes. */
    register XpexFloat  *e1 = m[0], *e3 = m[2], *e2 = m[1];
    register double     s, mag_vpn;
	int error_ind = 0;

    if (XPEX_ZERO_MAG( mag_vpn = XPEX_MAG_V3(vpn))) {
        error_ind = ERR159;

    } else if (XPEX_ZERO_MAG( XPEX_MAG_V3(vup))) {
        error_ind = ERR160;

    } else {
        /* e1' = VUP x VPN / |VUP x VPN|, but do the division later. */
        e1[0] = vup->y * vpn->z - vup->z * vpn->y;
        e1[1] = vup->z * vpn->x - vup->x * vpn->z;
        e1[2] = vup->x * vpn->y - vup->y * vpn->x;
        s = sqrt( e1[0] * e1[0] + e1[1] * e1[1] + e1[2] * e1[2]);

        /* Check for vup and vpn colinear (zero dot product). */
        if ( XPEX_ZERO_MAG( s) ) {
            error_ind = ERR161;
       } else {
            error_ind = 0;

            /* Normalize e1 */
            s = 1.0 / s;
            e1[0] *= s; e1[1] *= s; e1[2] *= s;

            /* e3 = VPN / |VPN| */
            s = 1.0 / mag_vpn;
            e3[0] = s * vpn->x; e3[1] = s * vpn->y; e3[2] = s * vpn->z;

            /* e2 = e3 x e1 */
            e2[0] = e3[1] * e1[2] - e3[2] * e1[1];
            e2[1] = e3[2] * e1[0] - e3[0] * e1[2];
            e2[2] = e3[0] * e1[1] - e3[1] * e1[0];

            /* Add the translation */
            e1[3] = -( e1[0] * vrp->x + e1[1] * vrp->y + e1[2] * vrp->z);
            e2[3] = -( e2[0] * vrp->x + e2[1] * vrp->y + e2[2] * vrp->z);
            e3[3] = -( e3[0] * vrp->x + e3[1] * vrp->y + e3[2] * vrp->z);

            /* Homogeneous entries */
            m[3][0] = m[3][1] = m[3][2] = 0.0;
            m[3][3] = 1.0;
        }
    }
	return error_ind;
}

int
XpexEvaluateViewOrientationMatrix2D(vrp, vup, m)
	XpexCoord2D   *vrp;  /* view reference point */
	register XpexVector2D  *vup;  /* view up vector */
	register XpexMatrix3X3  m;  /* OUT view orientation matrix */
{
    /* The old basis is: e1 = < 1, 0>,  e2 = < 0, 1>
     * The new basis is: e1' = < vup.y, -vup.x> / |vup|,  e2' = vup / |vup|.
     * Therefore the transform for old to new is x' = ATx, where:

             | e1' 0 |         | 1 0 -vrp.x |
         A = |       |,    T = | 0 1 -vrp.y |
             | e2' 0 |         | 0 0    1   |
             |       |
             | -0-  1|
     */

    register double     s;
	int error_ind = 0;

    if (XPEX_ZERO_MAG( s = XPEX_MAG_V2( vup))) {
        error_ind = ERR160;

    } else {
        error_ind = 0;

        /* Compute the new basis, note that m[0] is e1' and m[1] is e2'. */
        s = 1.0 / s;
        m[0][0] = s * vup->y;
        m[0][1] = s * -vup->x;
        m[1][0] = s * vup->x;
        m[1][1] = s * vup->y;

        /* Add the translation */
        m[0][2] = -( m[0][0] * vrp->x + m[0][1] * vrp->y);
        m[1][2] = -( m[1][0] * vrp->x + m[1][1] * vrp->y);

        /* Homogeneous entries */
        m[2][0] = m[2][1] = 0.0;
        m[2][2] = 1.0;
    }
	return error_ind;
}
