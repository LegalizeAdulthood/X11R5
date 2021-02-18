#ifndef lint
static char     sccsid[] = "@(#)fb_xfut.c 1.1 91/09/07 FJ";
#endif

/***********************************************************
Copyright 1989, 1990, 1991, by Fujitsu Limited, Sun Microsystems, Inc.
and the X Consortium.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Fujitsu, Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity
pertaining to distribution of the software without specific, written
prior permission.  

FUJITSU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJITSU BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
******************************************************************/

/* Transform Utility functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"
#include "phg_dt.h"



/* TRANSLATE 3 */
void
ptr3_( dx, dy, dz, errind, xfrmt)
Pfloat		*dx;		/* translation vector X	*/
Pfloat		*dy;		/* translation vector Y	*/
Pfloat		*dz;		/* translation vector Z	*/
Pint		*errind;	/* OUT error indicator	*/
register Pmatrix3	xfrmt;	/* OUT transformation matrix	*/
{
    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;
     
    } else {
	*errind = 0;
	xfrmt[3][0] = *dx;
	xfrmt[3][1] = *dy;
	xfrmt[3][2] = *dz;
	xfrmt[0][0] = xfrmt[1][1] = xfrmt[2][2] = xfrmt[3][3] = 1.0;
	xfrmt[1][0] = xfrmt[2][0] = xfrmt[0][1] = xfrmt[2][1] = xfrmt[0][2]
            = xfrmt[1][2] = xfrmt[0][3] = xfrmt[1][3] = xfrmt[2][3] = 0.0;
    }
}


/* TRANSLATE */
void
ptr_( dx, dy, errind, xfrmt)
Pfloat		*dx;		/* translation vector X	*/
Pfloat		*dy;		/* translation vector Y	*/
Pint		*errind;	/* OUT error indicator	*/
register Pmatrix	xfrmt;	/* OUT transformation matrix	*/
{
    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;
     
    } else {
	*errind = 0;
	xfrmt[2][0] = *dx;
	xfrmt[2][1] = *dy;
	xfrmt[0][0] = xfrmt[1][1] = xfrmt[2][2] = 1.0;
	xfrmt[1][0] = xfrmt[0][1] = xfrmt[0][2] = xfrmt[1][2] = 0.0;
    }
}


/* SCALE 3 */
void
psc3_( fx, fy, fz, errind, xfrmt)
Pfloat		*fx;		/* scale factor X	*/
Pfloat		*fy;		/* scale factor Y	*/
Pfloat		*fz;		/* scale factor Z	*/
Pint		*errind;	/* OUT error indicator	*/
register Pmatrix3	xfrmt;	/* OUT transformation matrix	*/
{
    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else {
	*errind = 0;
	xfrmt[0][0] = *fx;
	xfrmt[1][1] = *fy;
	xfrmt[2][2] = *fz;
	xfrmt[3][3] = 1.0;
	xfrmt[1][0] = xfrmt[2][0] = xfrmt[3][0] = xfrmt[0][1] = xfrmt[2][1]
            = xfrmt[3][1] = xfrmt[0][2] = xfrmt[1][2] = xfrmt[3][2]
            = xfrmt[0][3] = xfrmt[1][3] = xfrmt[2][3] = 0.0;
    }
}


/* SCALE */
void
psc_( fx, fy, errind, xfrmt)
Pfloat		*fx;		/* scale factor X	*/
Pfloat		*fy;		/* scale factor Y	*/
Pint		*errind;	/* OUT error indicator	*/
register Pmatrix	xfrmt; 	/* OUT transformation matrix	*/
{
    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;
     
    } else {
	*errind = 0;
	xfrmt[0][0] = *fx;
	xfrmt[1][1] = *fy;
	xfrmt[2][2] = 1.0;
	xfrmt[1][0] = xfrmt[2][0] = xfrmt[0][1] = xfrmt[2][1] = xfrmt[0][2]
            = xfrmt[1][2] = 0.0;
    }
}


/* ROTATE X */
void
prox_( rotang, errind, xfrmt)
Pfloat		*rotang;	/* rotation angle in radians	*/
Pint		*errind;	/* OUT error indicator	*/
register Pmatrix3	xfrmt; 	/* OUT transformation matrix	*/
{
    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;
     
    } else {
	*errind = 0;
	xfrmt[1][1] = xfrmt[2][2] = cos(*rotang);
	xfrmt[2][1] = -(xfrmt[1][2] = sin(*rotang));
	xfrmt[0][0] = xfrmt[3][3] = 1.0;
	xfrmt[1][0] = xfrmt[2][0] = xfrmt[3][0] = xfrmt[0][1] = xfrmt[3][1]
            = xfrmt[0][2] = xfrmt[3][2] = xfrmt[0][3] = xfrmt[1][3]
            = xfrmt[2][3] = 0.0;
    }
}


/* ROTATE Y */
void
proy_( rotang, errind, xfrmt)
Pfloat		*rotang;	/* rotation angle in radians	*/
Pint		*errind;	/* OUT error indicator	*/
register Pmatrix3	xfrmt;	/* OUT transformation matrix	*/
{
    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;
     
    } else {
	*errind = 0;
	xfrmt[0][0] = xfrmt[2][2] = cos(*rotang);
	xfrmt[0][2] = -(xfrmt[2][0] = sin(*rotang));
	xfrmt[1][1] = xfrmt[3][3] = 1.0;
	xfrmt[1][0] = xfrmt[3][0] = xfrmt[0][1] = xfrmt[2][1] = xfrmt[3][1]
            = xfrmt[1][2] = xfrmt[3][2] = xfrmt[0][3] = xfrmt[1][3]
            = xfrmt[2][3] = 0.0;
    }
}


/* ROTATE Z */
void
proz_( rotang, errind, xfrmt)
Pfloat		*rotang;	/* rotation angle in radians	*/
Pint		*errind;	/* OUT error indicator	*/
register Pmatrix3	xfrmt;	/* OUT transformation matrix	*/
{
    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;
     
    } else {
	*errind = 0;
	xfrmt[0][0] = xfrmt[1][1] = cos(*rotang);
	xfrmt[1][0] = -(xfrmt[0][1] = sin(*rotang));
	xfrmt[2][2] = xfrmt[3][3] = 1.0;
	xfrmt[2][0] = xfrmt[3][0] = xfrmt[2][1] = xfrmt[3][1] = xfrmt[0][2]
            = xfrmt[1][2] = xfrmt[3][2] = xfrmt[0][3] = xfrmt[1][3]
            = xfrmt[2][3] = 0.0;
    }
}


/* ROTATE */
void
pro_( rotang, errind, xfrmt)
Pfloat		*rotang;	/* rotation angle in radians	*/
Pint		*errind;	/* OUT error indicator	*/
register Pmatrix	xfrmt;	/* OUT transformation matrix	*/
{
    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;
     
    } else {
	*errind = 0;
	xfrmt[0][0] = xfrmt[1][1] = cos(*rotang);
	xfrmt[1][0] = -(xfrmt[0][1] = sin(*rotang));
	xfrmt[2][2] = 1.0;
	xfrmt[2][0] = xfrmt[2][1] = xfrmt[0][2] = xfrmt[1][2] = 0.0;
    }
}


/* COMPOSE MATRIX 3 */
void
pcom3_( xfrmta, xfrmtb, errind, xfrmt)
Pmatrix3	xfrmta;		/* transformation matrix A	*/
Pmatrix3	xfrmtb;		/* transformation matrix B	*/
Pint		*errind;	/* OUT error indicator	*/
Pmatrix3	xfrmt;		/* OUT composed transformation matrix	*/
{
    Pmatrix3	m;
    Pmatrix3	a;
    Pmatrix3	b;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;
     
    } else {
	*errind = 0;
        MATRIX_DATA_TRANSLATE( 3, xfrmta, a);
        MATRIX_DATA_TRANSLATE( 3, xfrmtb, b);
	phg_mat_mul(m, a, b);
        MATRIX_DATA_TRANSLATE( 3, m, xfrmt);
    }
}


/* COMPOSE MATRIX */
void
pcom_( xfrmta, xfrmtb, errind, xfrmt)
Pmatrix		xfrmta;		/* transformation matrix A	*/
Pmatrix		xfrmtb;		/* transformation matrix B	*/
Pint		*errind;	/* OUT error indicator	*/
Pmatrix		xfrmt;		/* OUT composed transformation matrix	*/
{
    Pmatrix	m;
    Pmatrix	a;
    Pmatrix	b;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;
     
    } else {
	*errind = 0;
        MATRIX_DATA_TRANSLATE( 2, xfrmta, a);
        MATRIX_DATA_TRANSLATE( 2, xfrmtb, b);
	phg_mat_mul_3x3(m, a, b);
        MATRIX_DATA_TRANSLATE( 2, m, xfrmt);
    }
}


/* TRANSFORM POINT 3 */
void
ptp3_( xi, yi, zi, xfrmt, errind, xo, yo, zo)
Pfloat		*xi;		/* point X	*/
Pfloat		*yi;		/* point Y	*/
Pfloat		*zi;		/* point Z	*/
Pmatrix3	xfrmt;		/* transformation matrix	*/
Pint		*errind;	/* OUT error indicator	*/
Pfloat		*xo;		/* OUT transformed point X	*/
Pfloat		*yo;		/* OUT transformed point Y	*/
Pfloat		*zo;		/* OUT transformed point Z	*/
{
    /* TODO: need error code for w = 0. */
    register float	w;		/* homogeneous coordinate */
    register Pmatrix3	m;		/* transformation matrix	*/

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;
    
    } else {
        MATRIX_DATA_TRANSLATE( 3, xfrmt, m);
        if (PHG_NEAR_ZERO( w = m[3][0]*(*xi) + m[3][1]*(*yi)
	    + m[3][2]*(*zi) + m[3][3])) {
	    *errind = ERR908;
     
        } else {
	    *errind = 0;
	    w = 1.0 / w;
	    *xo = w * (m[0][0] * (*xi) + m[0][1] * (*yi) + m[0][2] * (*zi)
                + m[0][3]);
	    *yo = w * (m[1][0] * (*xi) + m[1][1] * (*yi) + m[1][2] * (*zi)
                + m[1][3]);
	    *zo = w * (m[2][0] * (*xi) + m[2][1] * (*yi) + m[2][2] * (*zi)
                + m[2][3]);
        }
    }
}


/* TRANSFORM POINT */
void
ptp_( xi, yi, xfrmt, errind, xo, yo)
Pfloat		*xi;		/* point X	*/
Pfloat		*yi;		/* point Y	*/
Pmatrix		xfrmt;		/* transformation matrix	*/
Pint		*errind;	/* OUT error indicator	*/
Pfloat		*xo;		/* OUT transformed point X	*/
Pfloat		*yo;		/* OUT transformed point Y	*/
{
    /* TODO: need error code for w = 0. */
    register float	w;		/* homogenous coordinate */
    register Pmatrix	m;		/* transformation matrix	*/

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else {
        MATRIX_DATA_TRANSLATE( 2, xfrmt, m);
        if (PHG_NEAR_ZERO( w = m[2][0] * (*xi) + m[2][1] * (*yi) + m[2][2])) {
            *errind = ERR908;
     
        } else {
	    *errind = 0;
	    w = 1.0 / w;
	    *xo = w * (m[0][0] * (*xi) + m[0][1] * (*yi) + m[0][2]);
	    *yo = w * (m[1][0] * (*xi) + m[1][1] * (*yi) + m[1][2]);
        }
    }
}

static void
build_transform3( pt, shift, ax, ay, az, scl, m)
    register Ppoint3    *pt;            /* fixed point  */
    Pvec3               *shift;         /* shift vector */
    Pfloat              ax;             /* rotation angle X     */
    Pfloat              ay;             /* rotation angle Y     */
    Pfloat              az;             /* rotation angle Z     */
    register Pvec3      *scl;           /* scale vector */
    Pmatrix3            m;              /* OUT transformation matrix    */
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
    r[0] = cz * cy * scl->delta_x;
    r[1] = (cz * sx * sy - sz * cx) * scl->delta_y;
    r[2] = (cz * sy * cx + sz * sx) * scl->delta_z;
    r[3] = shift->delta_x + pt->x -
        (r[0] * pt->x + r[1] * pt->y + r[2] * pt->z);
    r = m[1];
    r[0] = sz * cy * scl->delta_x;
    r[1] = (sz * sx * sy + cz * cx) * scl->delta_y;
    r[2] = (sz * sy * cx - cz * sx) * scl->delta_z;
    r[3] = shift->delta_y + pt->y -
        (r[0] * pt->x + r[1] * pt->y + r[2] * pt->z);
    r = m[2];
    r[0] = -sy * scl->delta_x;
    r[1] = cy * sx * scl->delta_y;
    r[2] = cy * cx * scl->delta_z;
    r[3] = shift->delta_z + pt->z -
        (r[0] * pt->x + r[1] * pt->y + r[2] * pt->z);
    r = m[3];
    r[0] = r[1] = r[2] = 0.0;
    r[3] = 1.0;
}


/* BUILD TRANSFORMATION MATRIX 3 */
void
pbltm3_( x0, y0, z0, dx, dy, dz, phix, phiy, phiz, fx, fy, fz, errind, xfrmt)
Pfloat		*x0;		/* fixed point X	*/
Pfloat		*y0;		/* fixed point Y	*/
Pfloat		*z0;		/* fixed point Z	*/
Pfloat		*dx;		/* shift vector X	*/
Pfloat		*dy;		/* shift vector Y	*/
Pfloat		*dz;		/* shift vector Z	*/
Pfloat		*phix;		/* rotation angle in radians X	*/
Pfloat		*phiy;		/* rotation angle in radians Y	*/
Pfloat		*phiz;		/* rotation angle in radians Z	*/
Pfloat		*fx;		/* scale vector X	*/
Pfloat		*fy;		/* scale vector Y	*/
Pfloat		*fz;		/* scale vector Z	*/
Pint		*errind;	/* OUT error indicator	*/
Pmatrix3	xfrmt;		/* OUT transformation matrix	*/
{
    Pmatrix3	m;
    Ppoint3	pt;
    Pvec3	shift;
    Pvec3	scale;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;
     
    } else {
	*errind = 0;
        pt.x = *x0;
        pt.y = *y0;
        pt.z = *z0;
        shift.delta_x = *dx;
        shift.delta_y = *dy;
        shift.delta_z = *dz;
        scale.delta_x = *fx;
        scale.delta_y = *fy;
        scale.delta_z = *fz;
	build_transform3( &pt, &shift, *phix, *phiy, *phiz, &scale, m);
        MATRIX_DATA_TRANSLATE( 3, m, xfrmt);
    }
}

static void
build_transform( pt, shift, ang, scl, m)
    Ppoint      *pt;            /* fixed point  */
    Pvec        *shift;         /* shift vector */
    Pfloat      ang;            /* rotation angle       */
    Pvec        *scl;           /* scale vector */
    Pmatrix     m;              /* OUT transformation matrix    */
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
    r[0] = c * scl->delta_x;
    r[1] = -s * scl->delta_y;
    r[2] = shift->delta_x + pt->x - c * scl->delta_x * pt->x + s
        * scl->delta_y * pt->y;
    r = m[1];
    r[0] = s * scl->delta_x;
    r[1] = c * scl->delta_y;
    r[2] = shift->delta_y + pt->y - (s * scl->delta_x * pt->x + c
        * scl->delta_y * pt->y);
    r = m[2];
    r[0] = r[1] = 0.0;
    r[2] = 1.0;
}


/* BUILD TRANSFORMATION MATRIX */
void
pbltm_( x0, y0, dx, dy, phi, fx, fy, errind, xfrmt)
Pfloat		*x0;		/* fixed pointx	X	*/
Pfloat		*y0;		/* fixed pointx	X	*/
Pfloat		*dx;		/* shift vector	X	*/
Pfloat		*dy;		/* shift vector	Y	*/
Pfloat		*phi;		/* rotation angle in radians	*/
Pfloat		*fx;		/* scale vector	X	*/
Pfloat		*fy;		/* scale vector	Y	*/
Pint		*errind;	/* OUT error indicator	*/
Pmatrix		xfrmt;		/* OUT transformation matrix	*/
{
    Pmatrix	m;
    Ppoint	pt;
    Pvec	shift;
    Pvec	scale;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;
     
    } else {
	*errind = 0;
        pt.x = *x0;
        pt.y = *y0;
        shift.delta_x = *dx;
        shift.delta_y = *dy;
        scale.delta_x = *fx;
        scale.delta_y = *fy;
	build_transform( &pt, &shift, *phi, &scale, m);
        MATRIX_DATA_TRANSLATE( 2, m, xfrmt);
    }
}


/* COMPOSE TRANSFORMATION MATRIX 3 */
void
pcotm3_( xfrmti, x0, y0, z0, dx, dy, dz, phix, phiy, phiz, fx, fy, fz, errind,
xfrmto)
Pmatrix3	xfrmti;		/* transformation matrix	*/
Pfloat		*x0;		/* fixed point X	*/
Pfloat		*y0;		/* fixed point X	*/
Pfloat		*z0;		/* fixed point Z	*/
Pfloat		*dx;		/* shift vector	X	*/
Pfloat		*dy;		/* shift vector	Y	*/
Pfloat		*dz;		/* shift vector	Z	*/
Pfloat		*phix;		/* rotation angle in radians X	*/
Pfloat		*phiy;		/* rotation angle in radians Y	*/
Pfloat		*phiz;		/* rotation angle in radians Z	*/
Pfloat		*fx;		/* scale vector	X	*/
Pfloat		*fy;		/* scale vector	Y	*/
Pfloat		*fz;		/* scale vector	Z	*/
Pint		*errind;	/* OUT error indicator	*/
Pmatrix3	xfrmto;		/* OUT composed transformation matrix	*/
{
    Pmatrix3	mi;
    Pmatrix3	mo;
    Pmatrix3	xform;
    Ppoint3	pt;
    Pvec3	shift;
    Pvec3	scale;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;
     
    } else {
	*errind = 0;
        pt.x = *x0;
        pt.y = *y0;
        pt.z = *z0;
        shift.delta_x = *dx;
        shift.delta_y = *dy;
        shift.delta_z = *dz;
        scale.delta_x = *fx;
        scale.delta_y = *fy;
        scale.delta_z = *fz;
	build_transform3( &pt, &shift, *phix, *phiy, *phiz, &scale, xform);
        MATRIX_DATA_TRANSLATE( 3, xfrmti, mi);
	/* Assuming pre-multiplication of old by new. */
	phg_mat_mul(mo, xform, mi);
        MATRIX_DATA_TRANSLATE( 3, mo, xfrmto);
    }
}


/* COMPOSE TRANSFORMATION MATRIX */
void
pcotm_( xfrmti, x0, y0, dx, dy, phi, fx, fy, errind, xfrmto)
Pmatrix		xfrmti;		/* transformation matrix	*/
Pfloat		*x0;		/* fixed point X	*/
Pfloat		*y0;		/* fixed point Y	*/
Pfloat		*dx;		/* shift vector	X	*/
Pfloat		*dy;		/* shift vector	Y	*/
Pfloat		*phi;		/* rotation angle in radians	*/
Pfloat		*fx;		/* scale vector	 X	*/
Pfloat		*fy;		/* scale vector	 Y	*/
Pint		*errind;	/* OUT error indicator	*/
Pmatrix		xfrmto;		/* OUT composed transformation matrix	*/
{
    Pmatrix	mi;
    Pmatrix	mo;
    Pmatrix	xform;
    Ppoint	pt;
    Pvec	shift;
    Pvec	scale;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;
     
    } else {
	*errind = 0;
        pt.x = *x0;
        pt.y = *y0;
        shift.delta_x = *dx;
        shift.delta_y = *dy;
        scale.delta_x = *fx;
        scale.delta_y = *fy;
	build_transform( &pt, &shift, *phi, &scale, xform);
        MATRIX_DATA_TRANSLATE( 2, xfrmti, mi);
	/* Assuming pre-multiplication of old by new. */
	phg_mat_mul_3x3(mo, xform, mi);
        MATRIX_DATA_TRANSLATE( 2, mo, xfrmto);
    }
}


/* EVALUATE VIEW ORIENTATION MATRIX 3 */
void
pevom3_( vwrx, vwry, vwrz, vpnx, vpny, vpnz, vupx, vupy, vupz, errind, vwormt)
Pfloat		*vwrx;		/* view reference point	X	*/
Pfloat		*vwry;		/* view reference point	Y	*/
Pfloat		*vwrz;		/* view reference point	Z	*/
Pfloat		*vpnx;		/* view plane normal X	*/
Pfloat		*vpny;		/* view plane normal Y	*/
Pfloat		*vpnz;		/* view plane normal Z	*/
Pfloat		*vupx;		/* view up vector X	*/
Pfloat		*vupy;		/* view up vector Y	*/
Pfloat		*vupz;		/* view up vector Z	*/
Pint		*errind;	/* OUT error indicator	*/
Pmatrix3	vwormt;		/* OUT view orientation matrix	*/
{
    register Pvec3	vpn;
    Pvec3		vup;

    /*  Translate to VRP then change the basis.
     *  The old basis is: e1 = < 1, 0, 0>,  e2 = < 0, 1, 0>, e3 = < 0, 0, 1>.
     * The new basis is: ("x" means cross product)
		e3' = VPN / |VPN|
		e1' = VUP x VPN / |VUP x VPN|
		e2' = e3' x e1'
     * Therefore the transform from old to new is x' = ATx, where:

	     | e1' 0 |         | 1 0 0 -vwrx |
	 A = |       |,    T = | 0 1 0 -vwry |
	     | e2' 0 |         | 0 0 1 -vwrz |
	     |       |         | 0 0 0   1   |
	     | e3' 0 |
	     |       |
	     | -0-  1|
     */

    /* These ei's are really ei primes. */
    Pmatrix3		m;
    register Pfloat	*e1 = m[0], *e3 = m[2], *e2 = m[1];
    register double	s, mag_vpn;
   
    vpn.delta_x = *vpnx;
    vpn.delta_y = *vpny;
    vpn.delta_z = *vpnz;
    vup.delta_x = *vupx;
    vup.delta_y = *vupy;
    vup.delta_z = *vupz;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if ( PHG_ZERO_MAG( mag_vpn = PHG_MAG_V3( &vpn)) ) {
	*errind = ERR159;

    } else if ( PHG_ZERO_MAG( PHG_MAG_V3( &vup)) ) {
	*errind = ERR160;
    
    } else {
	/* e1' = VUP x VPN / |VUP x VPN|, but do the division later. */
	e1[0] = vup.delta_y * vpn.delta_z - vup.delta_z * vpn.delta_y;
	e1[1] = vup.delta_z * vpn.delta_x - vup.delta_x * vpn.delta_z;
	e1[2] = vup.delta_x * vpn.delta_y - vup.delta_y * vpn.delta_x;
	s = sqrt( e1[0] * e1[0] + e1[1] * e1[1] + e1[2] * e1[2]);

	/* Check for vup and vpn colinear (zero dot product). */
	if ( PHG_ZERO_MAG( s) ) {
	    *errind = ERR161;

	} else {
	    *errind = 0;

	    /* Normalize e1 */
	    s = 1.0 / s;
	    e1[0] *= s; e1[1] *= s; e1[2] *= s;

	    /* e3 = VPN / |VPN| */
	    s = 1.0 / mag_vpn;
	    e3[0] = s * vpn.delta_x; e3[1] = 
                s * vpn.delta_y; e3[2] = s * vpn.delta_z;

	    /* e2 = e3 x e1 */
	    e2[0] = e3[1] * e1[2] - e3[2] * e1[1];
	    e2[1] = e3[2] * e1[0] - e3[0] * e1[2];
	    e2[2] = e3[0] * e1[1] - e3[1] * e1[0];

	    /* Add the translation */
	    e1[3] = -( e1[0] * (*vwrx) + e1[1] * (*vwry) + e1[2] * (*vwrz));
	    e2[3] = -( e2[0] * (*vwrx) + e2[1] * (*vwry) + e2[2] * (*vwrz));
	    e3[3] = -( e3[0] * (*vwrx) + e3[1] * (*vwry) + e3[2] * (*vwrz));

	    /* Homogeneous entries */
	    m[3][0] = m[3][1] = m[3][2] = 0.0;
	    m[3][3] = 1.0;

            MATRIX_DATA_TRANSLATE( 3, m, vwormt);
	}
    }
}


/* EVALUATE VIEW ORIENTATION MATRIX */
void
pevom_( vwrx, vwry, vupx, vupy, errind, vwormt)
Pfloat		*vwrx;		/* view reference point	X	*/
Pfloat		*vwry;		/* view reference point	Y	*/
Pfloat		*vupx;		/* view up vector X	*/
Pfloat		*vupy;		/* view up vector Y	*/
Pint		*errind;	/* OUT error indicator	*/
Pmatrix		vwormt;		/* OUT view orientation matrix	*/
{
    register Pvec	vup;
    /* The old basis is: e1 = < 1, 0>,  e2 = < 0, 1>
     * The new basis is: e1' = < vup.y, -vup.x> / |vup|,  e2' = vup / |vup|.
     * Therefore the transform for old to new is x' = ATx, where:

	     | e1' 0 |         | 1 0 -vwrx |
	 A = |       |,    T = | 0 1 -vwry |
	     | e2' 0 |         | 0 0   1   |
	     |       |
	     | -0-  1|
     */

    register Pmatrix	m;
    register double	s;

    vup.delta_x = *vupx;
    vup.delta_y = *vupy;
    
    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if ( PHG_ZERO_MAG( s = PHG_MAG_V2( &vup)) ) {
	*errind = ERR160;

    } else {
	*errind = 0;

	/* Compute the new basis, note that m[0] is e1' and m[1] is e2'. */
	s = 1.0 / s;
	m[0][0] = s * vup.delta_y;
	m[0][1] = s * -vup.delta_x;
	m[1][0] = s * vup.delta_x;
	m[1][1] = s * vup.delta_y;

	/* Add the translation */
	m[0][2] = -( m[0][0] * (*vwrx) + m[0][1] * (*vwry));
	m[1][2] = -( m[1][0] * (*vwrx) + m[1][1] * (*vwry));

	/* Homogeneous entries */
	m[2][0] = m[2][1] = 0.0;
	m[2][2] = 1.0;

        MATRIX_DATA_TRANSLATE( 2, m, vwormt);
    }
}


/* EVALUATE VIEW MAPPING MATRIX */

void
pevmm_( vwwnlm, pjvplm, errind, vwmpmt)
register Plimit	*vwwnlm;	/* window limits	*/
register Plimit	*pjvplm;	/* projection viewport limits	*/
Pint		*errind;	/* OUT error indicator	*/
Pmatrix		vwmpmt;		/* OUT view mapping matrix	*/
{
/* 1. Translate window's lower-left-corner to 0,0.
 * 2. Scale size of window to size of viewport.
 * 3. Translate 0,0 to viewport's lower-left-corner.
 *
 * Matrices are:
 * 1:	1 0 -vwwnlm->xmin    2:	 sx	0	0	3: 1 0  pjvplm->xmin
 * 	0 1 -vwwnlm->ymin	 0	sy	0	   0 1  pjvplm->ymin
 * 	0 0	1		 0	0	1	   0 0	    1
 */

    register Pmatrix	m;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;
     
    } else if (!( vwwnlm->x_min < vwwnlm->x_max) ||
               !( vwwnlm->y_min < vwwnlm->y_max)) {
	*errind = ERR151;

    } else if (!( pjvplm->x_min < pjvplm->x_max) ||
               !( pjvplm->y_min < pjvplm->y_max)) {
	*errind = ERR152;

    } else if ( !CB_IN_RANGE( PDT_NPC_XMIN, PDT_NPC_XMAX, pjvplm->x_min)
	|| !CB_IN_RANGE( PDT_NPC_XMIN, PDT_NPC_XMAX, pjvplm->x_max)
	|| !CB_IN_RANGE( PDT_NPC_YMIN, PDT_NPC_YMAX, pjvplm->y_min)
	|| !CB_IN_RANGE( PDT_NPC_YMIN, PDT_NPC_YMAX, pjvplm->y_max)) {
	*errind = ERR155;

    } else {
	register float	sx, sy;	/* scale factors: len(pjvplm) / len(vwwnlm) */

	*errind = 0;
	sx = (pjvplm->x_max - pjvplm->x_min) / (vwwnlm->x_max - vwwnlm->x_min);
	sy = (pjvplm->y_max - pjvplm->y_min) / (vwwnlm->y_max - vwwnlm->y_min);
	m[0][0] = sx;  m[0][1] = 0.0;
        m[0][2] = sx * (-vwwnlm->x_min) + pjvplm->x_min;
	m[1][0] = 0.0; m[1][1] = sy;
        m[1][2] = sy * (-vwwnlm->y_min) + pjvplm->y_min;
	m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0;

        MATRIX_DATA_TRANSLATE( 2, m, vwmpmt);
    }
}


/* EVALUATE VIEW MAPPING MATRIX 3 */
void
pevmm3_( vwwnlm, pjvplm, pjtype, pjrx, pjry, pjrz, vpld, bpld, fpld, errind,
vwmpmt)
register Plimit	*vwwnlm;	/* window limits	*/
register Plimit3	*pjvplm;	/* projection viewport limits	*/
Pint		*pjtype;	/* projection type	*/
Pfloat		*pjrx;		/* projection reference point X	*/
Pfloat		*pjry;		/* projection reference point Y	*/
Pfloat		*pjrz;		/* projection reference point Z	*/
Pfloat		*vpld;		/* view plane distance	*/
Pfloat		*bpld;		/* back plane distance	*/
Pfloat		*fpld;		/* front plane distance	*/
Pint		*errind;	/* OUT error indicator	*/
Pmatrix3	vwmpmt;		/* OUT view mapping matrix	*/
{
    /* Procedure:
      (Perspective):
	 - Translate to PRP,			Tc
	 - Convert to left handed coords,	Tlr
	 - Shear,				H
	 - Scale to canonical view volume,	S
	 - Normalize perspective view volume,	Ntp
	 - Scale to viewport,			Svp
	 - Convert to right handed coords,	Tlr
	 - Translate to viewport,		Tvp

      (Parallel):
         - Translate to view plane,             Tc
         - Shear about the view plane,          H
         - Translate back,                      Tc inverse
	 - Translate window to origin,		Tl
	 - Scale to canonical view volume,	S
	 - Scale to viewport,			Svp
	 - Translate to viewport,		Tvp

	 See pevalviewmappingmatrix3_debug for the matrices.
     */

    register Pmatrix3	m;
    register Pfloat	*r;

    /* These are ordered roughly by the number of times used, the most
     * used is first.  Those used twice or less aren't declared register.
     */
    register double	sz, sx, sy;
    register double	zf;
    register double	dx = pjvplm->x_max - pjvplm->x_min;
    register double	dy = pjvplm->y_max - pjvplm->y_min;
    register double	hx, hy;
    register double	d;
    	     double	dz = pjvplm->z_max - pjvplm->z_min;
    	     double	vvz = (*fpld) - (*bpld);

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if ( !(vwwnlm->x_min < vwwnlm->x_max) ||
                !(vwwnlm->y_min < vwwnlm->y_max)) {
	*errind = ERR151;

    } else if ( !(pjvplm->x_min < pjvplm->x_max) ||
                !(pjvplm->y_min < pjvplm->y_max) ||
                !(pjvplm->z_min <= pjvplm->z_max) ) {
	*errind = ERR152;
    
    } else if ( PHG_NEAR_ZERO( vvz) && pjvplm->z_min != pjvplm->z_max) {
	*errind = ERR158;

    } else if ( *pjtype < PPARL || *pjtype > PPERS) {
	*errind = ERR2000;

    } else if ( *pjtype == PPERS && *pjrz < *fpld && *pjrz > *bpld ) {
	*errind = ERR162;

    } else if ( *pjrz == *vpld) {
	*errind = ERR163;
    
    } else if ( *fpld < *bpld) {
	*errind = ERR164;

    } else if ( !CB_IN_RANGE( PDT_NPC_XMIN, PDT_NPC_XMAX, pjvplm->x_min)
	|| !CB_IN_RANGE( PDT_NPC_XMIN, PDT_NPC_XMAX, pjvplm->x_max)
	|| !CB_IN_RANGE( PDT_NPC_YMIN, PDT_NPC_YMAX, pjvplm->y_min)
	|| !CB_IN_RANGE( PDT_NPC_YMIN, PDT_NPC_YMAX, pjvplm->y_max)
	|| !CB_IN_RANGE( PDT_NPC_ZMIN, PDT_NPC_ZMAX, pjvplm->z_min)
	|| !CB_IN_RANGE( PDT_NPC_ZMIN, PDT_NPC_ZMAX, pjvplm->z_max) ) {
	*errind = ERR155;
    
    } else if ( *pjtype == PPERS) {
	*errind = 0;
	d = (*pjrz) - (*vpld);
	sz = 1.0 / ((*pjrz) - (*bpld));
	sx = sz * d * 2.0 / (vwwnlm->x_max - vwwnlm->x_min);
	sy = sz * d * 2.0 / (vwwnlm->y_max - vwwnlm->y_min);
	hx = ((*pjrx) - 0.5 * (vwwnlm->x_min + vwwnlm->x_max)) / d;
	hy = ((*pjry) - 0.5 * (vwwnlm->y_min + vwwnlm->y_max)) / d;

	r = m[0];
	r[0] = 0.5 * dx * sx;
	r[1] = 0.0;
	r[2] = -(0.5 * dx * (sx * hx + sz) + sz * pjvplm->x_min);
	r[3] = -(0.5 * dx * sx * ((*pjrx) - hx * (*pjrz))
	    - sz * (*pjrz) * (0.5 * dx + pjvplm->x_min));

	r = m[1];
	r[0] = 0.0;
	r[1] = 0.5 * dy * sy;
	r[2] = -(0.5 * dy * (sy * hy + sz) + sz * pjvplm->y_min);
	r[3] = -(0.5 * dy * sy * ((*pjry) - hy * (*pjrz))
	    - sz * (*pjrz) * (0.5 * dy + pjvplm->y_min));

	r = m[2];
	r[0] = r[1] = 0.0;
	zf = ((*pjrz) - (*fpld)) / ((*pjrz) - (*bpld));
	if ( PHG_NEAR_ZERO( 1.0 - zf)) {
	    r[2] = 0.0;
	    r[3] = sz * (*pjrz) * pjvplm->z_max;
	} else {
	    r[2] = sz * ((dz / (1.0 - zf)) - pjvplm->z_max);
	    r[3] = sz * (*pjrz) * pjvplm->z_max
                 - (dz/(1.0-zf)) * (sz * (*pjrz) - zf);
	}

	r = m[3];
	r[0] = r[1] = 0.0;
	r[2] = -sz;
	r[3] = sz * (*pjrz);

        MATRIX_DATA_TRANSLATE( 3, m, vwmpmt);

    } else {	/* parallel */
	*errind = 0;
	sx = dx / (vwwnlm->x_max - vwwnlm->x_min);
	sy = dy / (vwwnlm->y_max - vwwnlm->y_min);
	hx = ((*pjrx) - 0.5 * (vwwnlm->x_min + vwwnlm->x_max))
	    / ((*vpld) - (*pjrz));
	hy = ((*pjry) - 0.5 * (vwwnlm->y_min + vwwnlm->y_max))
	    / ((*vpld) - (*pjrz));

	r = m[0];
	r[0] = sx;
	r[1] = 0.0;
	r[2] = sx * hx;
	r[3] = pjvplm->x_min - sx * (hx * (*vpld) + vwwnlm->x_min);

	r = m[1];
	r[0] = 0.0;
	r[1] = sy;
	r[2] = sy * hy;
	r[3] = pjvplm->y_min - sy * (hy * (*vpld) + vwwnlm->y_min);

	r  = m[2];
	r[0] = r[1] = 0.0;
	if ( PHG_NEAR_ZERO(vvz))
	    r[2] = 0.0;
	else
	    r[2] = dz / vvz;
	r[3] = pjvplm->z_min - r[2] * (*bpld);

	r = m[3];
	r[0] = r[1] = r[2] = 0.0;
	r[3] = 1.0;

        MATRIX_DATA_TRANSLATE( 3, m, vwmpmt);
    }
}


#define COINCIDE(pt1, pt2)                      \
    (PHG_NEAR_ZERO((pt1)->x - (pt2)->x) &&      \
     PHG_NEAR_ZERO((pt1)->y - (pt2)->y) &&      \
     PHG_NEAR_ZERO((pt1)->z - (pt2)->z))

static int
compute_normal( pts, normal)
Ppoint3         pts[];
Pvec3   *normal;
{
    Pvec3       u, v;
    float       mag;

    u.delta_x = pts[1].x - pts[0].x;
    u.delta_y = pts[1].y - pts[0].y;
    u.delta_z = pts[1].z - pts[0].z;
    if (!PHG_ZERO_MAG(mag = PHG_MAG_V3(&u))) {
        u.delta_x /= mag;
        u.delta_y /= mag;
        u.delta_z /= mag;
    }

    v.delta_x = pts[2].x - pts[0].x;
    v.delta_y = pts[2].y - pts[0].y;
    v.delta_z = pts[2].z - pts[0].z;
    if (!PHG_ZERO_MAG(mag = PHG_MAG_V3(&v))) {
        v.delta_x /= mag;
        v.delta_y /= mag;
        v.delta_z /= mag;
    }

    normal->delta_x = u.delta_y * v.delta_z - u.delta_z * v.delta_y;
    normal->delta_y = u.delta_z * v.delta_x - u.delta_x * v.delta_z;
    normal->delta_z = u.delta_x * v.delta_y - u.delta_y * v.delta_x;

    mag = PHG_MAG_V3(normal);
    if (PHG_ZERO_MAG(mag)) {
        return(0); /* the points are colinear */
    }

    normal->delta_x /= mag;
    normal->delta_y /= mag;
    normal->delta_z /= mag;

    return(1);

}


/* COMPUTE FILL AREA SET GEOMETRIC NORMAL */
void
pcfasn_( npl, ixa, pxa, pya, pza, errind, vxa, vya, vza)
Pint	*npl;		/* number of point lists        */
Pint	*ixa;		/* array of indicies for points lists   */
Pfloat	*pxa;		/* coordinates of points X      */
Pfloat	*pya;		/* coordinates of points Y      */
Pfloat	*pza;		/* coordinates of points Z      */
Pint	*errind;	/* OUT error indicator  */
Pfloat	*vxa;		/* OUT unit normal vector X     */
Pfloat	*vya;		/* OUT unit normal vector Y     */
Pfloat	*vza;		/* OUT unit normal vector Z     */
{
    register int        i, j;
    int                 pt_found = 0;
    Ppoint3             pt[3];
    Ppoint3             point;
    Pvec3               normal;
    Pint                w_ixa;

    *errind = 0;
    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if ( *npl < 1) {
        *errind = ERR505;

    } else {
        w_ixa = 0;
        for ( i = 0; i < *npl; i++) {
            pt[0].x = pxa[w_ixa];
            pt[0].y = pya[w_ixa];
            pt[0].z = pza[w_ixa];
            pt_found = 1;
            for ( j = w_ixa + 1; j < ixa[i]; j++) {
                point.x = pxa[j];
                point.y = pya[j];
                point.z = pza[j];
                if ( !COINCIDE(&pt[0], &point) ) {
                    pt[1] = point;
                    pt_found = 2;
                    j++;
                    break;
                }
            }
            if ( pt_found == 2) {
                for ( ; j < ixa[i]; j++) {
                    pt[2].x = pxa[j];
                    pt[2].y = pya[j];
                    pt[2].z = pza[j];
                    if ( compute_normal( pt, &normal)) {
                        pt_found = 3;
                        break;
                    }
                }
            }
            if ( pt_found == 3) {
                *vxa = normal.delta_x;
                *vya = normal.delta_y;
                *vza = normal.delta_z;
                return;
            }
            w_ixa = ixa[i];
        }
        if ( pt_found < 3)
            *errind = ERR505;
    }
}
