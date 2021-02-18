#ifndef lint
static char     sccsid[] = "@(#)fb_nurb.c 1.1 91/09/07 FJ";
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

/* NURB functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"
#include "alloc.h"

#define RANGE_INCONSIST( _min, _max, _order, _num_knots, _knots ) \
    ( (_min) >= (_max) \
	|| (_min) < (_knots)[(_order) - 1] \
	|| (_max) > (_knots)[(_num_knots) - (_order)] \
    )


static int
values_are_non_decreasing( n, value)
    int             n;
    Pfloat         *value;
{
    --n;                        /* for N knots, N-1 comparisons will do */
    while ( --n >= 0 ) {
        if ( value[1] < value[0] )
            return 0;
        else
            value++;
    }
    return 1;
}


/* NON-UNIFORM B-SPLINE CURVE */
void
pnubsc_( sord, nka, knots, rtype, ncp, pxa, pya, pza, pwa, parl)
Pint	*sord;		/* spline order	*/
Pint	*nka;		/* number of knots array	*/
Pfloat	*knots;		/* knots	*/
Pint	*rtype;		/* rationality	*/
Pint	*ncp;		/* number of control points	*/
Pfloat	*pxa;		/* control points X	*/
Pfloat	*pya;		/* control points Y	*/
Pfloat	*pza;		/* control points Z	*/
Pfloat	*pwa;		/* control points W	*/
Pfloat	*parl;		/* parameter range limits	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    int			i;
    Pint		err = 0;
    Ppoint3		*pt3;
    Ppoint4		*pt4;

    register	Phg_nurb_curve_data	*curve;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_nuni_bsp_curv) ) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else if ( *sord < 1) {
            ERR_REPORT( phg_cur_cph->erh, ERR500);

        } else if ( *nka < 0 || *ncp < 0) {
            ERR_REPORT( phg_cur_cph->erh, ERR2004);

        } else if ( *ncp < *sord) {
            ERR_REPORT( phg_cur_cph->erh, ERR501);

        } else if ( *nka < *ncp + *sord) {
            ERR_REPORT( phg_cur_cph->erh, ERR502);

        } else if ( !values_are_non_decreasing( *nka, knots)) {
            ERR_REPORT( phg_cur_cph->erh, ERR503);

        } else if ( RANGE_INCONSIST( parl[0], parl[1], *sord, *nka, knots)) {
            ERR_REPORT( phg_cur_cph->erh, ERR506);

        } else if ( *rtype < PRATIO || *rtype > PNONRA) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else {
            /* Set up the fixed-length header fields */
            args->el_type = PELEM_NUNI_BSP_CURVE;
            curve = &ed.nurb_curve.data;
            curve->order = *sord;
            /* convert rationality    f_type -> c_type */
            curve->rationality = *rtype ?
                PNON_RATIONAL: PRATIONAL;
            curve->tstart = parl[0];
            curve->tend = parl[1];
            curve->knots.num_floats = *nka;
            curve->knots.floats = knots;
            curve->npts = *ncp;
            PMALLOC( err, Ppoint4, *ncp, curve->points);
            if ( err) {
                ERR_REPORT(phg_cur_cph->erh, err);

            } else {
                if ( *rtype == PNONRA) {
                    pt3 = (Ppoint3 *)curve->points;
                    for ( i = 0; i < *ncp; i++) {
                        pt3[i].x = pxa[i];
                        pt3[i].y = pya[i];
                        pt3[i].z = pza[i];
                    }
                } else {
                    pt4 = curve->points;
                    for ( i = 0; i < *ncp; i++) {
                        pt4[i].x = pxa[i];
                        pt4[i].y = pya[i];
                        pt4[i].z = pza[i];
                        pt4[i].w = pwa[i];
                    }
                }
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                    CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
                PFREE( *ncp, curve->points);
            }
        }
    }
}


/* SET CURVE APPROXIMATION CRITERIA */
void
pscac_( acri, aval)
Pint	*acri;		/* curve approximation criteria type	*/
Pfloat	*aval;		/* approximation value	*/ 
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_curve_approx) ) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

	} else {
	    args->el_type = PELEM_CURVE_APPROX_CRIT;
	    ed.curv_approx.type = *acri;
	    ed.curv_approx.value = *aval;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* NON-UNIFORM B-SPLINE SURFACE */
void
pnubss_( usord, vsord, unka, vnka, uknots, vknots, rtype, uncp, vncp, pxa, pya,
pza, pwa, ntl, itl, tcat, tcadr, tcvf, tsord, tnka, tknots, ttype, tncp, tpxa,
tpya, tpwa, tparl)
Pint	*usord;		/* U spline order	*/
Pint	*vsord;		/* V spline order	*/
Pint	*unka;		/* number of knots array for U	*/
Pint	*vnka;		/* number of knots array for V	*/
Pfloat	*uknots;	/* U knots	*/
Pfloat	*vknots;	/* V knots	*/
Pint	*rtype;		/* rationality	*/
Pint	*uncp;		/* U number of control points demension	*/
Pint	*vncp;		/* V number of control points demension	*/
Pfloat	*pxa;		/* control points X	*/
Pfloat	*pya;		/* control points Y	*/
Pfloat	*pza;		/* control points Z	*/
Pfloat	*pwa;		/* control points W	*/
Pint	*ntl;		/* number of trim loops definitions	*/
Pint	*itl;		/* array of indicies for trim loops	*/
Pint	*tcat;		/* curve approximation type	*/
Pfloat	*tcadr;		/* curve approximation data record	*/
Pint	*tcvf;		/* curve edge flag	*/
Pint	*tsord;		/* curve order	*/
Pint	*tnka;		/* number of knots array for curve knots */
Pfloat	*tknots;	/* curve knots vector	*/
Pint	*ttype;		/* curve type	*/
Pint	*tncp;		/* number of curve control points	*/
Pfloat	*tpxa;		/* curve control points X	*/
Pfloat	*tpya;		/* curve control points Y	*/
Pfloat	*tpwa;		/* curve control points W	*/
Pfloat	*tparl;		/* curve parameter ranres limits	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    register	Phg_nurb_surf_data	*surf = &ed.nurb_surf.data;
    register	int			dim = (*uncp) * (*vncp);
    register    int			i, j, k;
    Pint 				err = 0;
    Ptrimcurve		*tc = NULL;
    Ppoint		*pt2 = NULL;
    Ppoint3		*pt3 = NULL;
    Ppoint4		*pt4 = NULL;
    Ppoint		*w_pt2;
    Ppoint3		*w_pt3;
    Ptrimcurve		*w_tc;
    Pint		w_itl = 0;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_nuni_bsp_surf) ) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

	} else if ( *usord < 1 || *vsord < 1) {
	    ERR_REPORT( phg_cur_cph->erh, ERR500);

        } else if ( *unka < 0 || *vnka < 0 || *uncp < 0 || *vncp < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);

	} else if ( *uncp < *usord || *vncp < *vsord) {
	    ERR_REPORT( phg_cur_cph->erh, ERR501);

	} else if ( *unka < *uncp + *usord || *vnka < *vncp + *vsord) {
	    ERR_REPORT( phg_cur_cph->erh, ERR502);

        } else if ( !values_are_non_decreasing( *unka, uknots)
            ||  !values_are_non_decreasing( *vnka, vknots)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR503);

	} else if ( *rtype < PRATIO || *rtype > PNONRA) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2000);

	/* TODO: Check for errors 507 through 512. */
	} else {
	    args->el_type = PELEM_NUNI_BSP_SURF;
	    surf->u_order = *usord;
	    surf->v_order = *vsord;
            /* convert rationality    f_type -> c_type */
	    surf->rationality = *rtype ?
                PNON_RATIONAL: PRATIONAL;
	    surf->uknots.num_floats = *unka;
	    surf->uknots.floats = uknots;
	    surf->vknots.num_floats = *vnka;
	    surf->vknots.floats = vknots;
	    surf->npts.u_dim = *uncp;
	    surf->npts.v_dim = *vncp;
	    surf->nloops = *ntl;

	    /* grid array assumed contiguous in memory */
            PMALLOC( err, Ppoint4, dim, surf->grid);
            if ( err) {
	        ERR_REPORT( phg_cur_cph->erh, err);

            } else {
	        if ( *rtype == PNONRA) {
                    pt3 = (Ppoint3 *)surf->grid;
                    for ( i = 0; i < dim; i++) {
                        pt3[i].x = pxa[i];
                        pt3[i].y = pya[i];
                        pt3[i].z = pza[i];
                    }
	        } else {
                    pt4 = surf->grid;
                    for ( i = 0; i < dim; i++) {
                        pt4[i].x = pxa[i];
                        pt4[i].y = pya[i];
                        pt4[i].z = pza[i];
                        pt4[i].w = pwa[i];
                    }
                }

                if ( surf->nloops > 0) {

                    /* Count everything. */
                    surf->num_tcurves = itl[*ntl - 1];
                    surf->num_tknots = 0;
                    surf->num_2D_tpoints = 0;
                    surf->num_3D_tpoints = 0;
                    for ( i = 0; i < surf->num_tcurves; i++) {
                        surf->num_tknots += tnka[i];
                        if ( ttype[i] == PNONRA)
                            surf->num_2D_tpoints += tncp[i];
                        else
                            surf->num_3D_tpoints += tncp[i];
                    }

                    PMALLOC( err, Ptrimcurve_list, *ntl, surf->trimloops);
                    PMALLOC( err, Ptrimcurve, surf->num_tcurves, tc);
                    PMALLOC( err, Ppoint, surf->num_2D_tpoints, pt2);
                    PMALLOC( err, Ppoint3, surf->num_3D_tpoints, pt3);
                    if ( err) {
                        ERR_REPORT( phg_cur_cph->erh, err);
                        PFREE( dim, surf->grid);
                        PFREE( *ntl, surf->trimloops);
                        PFREE( surf->num_tcurves, tc);
                        PFREE( surf->num_2D_tpoints, pt2);
                        return;

                    } else {
                        /* Copy data */
                        k = 0;
                        w_pt2 = pt2;
                        w_pt3 = pt3;
                        for ( i = 0; i < *ntl; i++) {
                            if ( ttype[i] == PNONRA) {
                                for ( j = 0; j < tncp[i]; j++){
                                    w_pt2->x = tpxa[k];
                                    w_pt2->y = tpya[k];
                                    w_pt2++;
                                }
                            } else {
                                for ( j = 0; j < tncp[i]; j++){
                                    w_pt3->x = tpxa[k];
                                    w_pt3->y = tpya[k];
                                    w_pt3->z = tpwa[k];
                                    w_pt3++;
                                }
                            }
                            k++;
                        }
                        
                        /* make trimming data */
                        w_tc = tc;
                        w_itl = 0;
                        k = 0;
                        w_pt2 = pt2;
                        w_pt3 = pt3;
                        for ( i = 0; i < *ntl; i++) {
                            surf->trimloops[i].num_curves = itl[i] - w_itl;
                            surf->trimloops[i].curves = w_tc;
                            for ( j = 0; j < surf->trimloops[i].num_curves;
                                j++) {
                                w_tc->visible = (Pedge_flag)tcvf[k];
                                /* convert rationality    f_type -> c_type */
                                w_tc->rationality = ttype[k] ?
                                    PNON_RATIONAL: PRATIONAL;
                                w_tc->order = tsord[k];
                                w_tc->approx_type = tcat[k];
                                w_tc->approx_val = tcadr[k];
                                w_tc->tmin = tparl[2 * k];
                                w_tc->tmax = tparl[2 * k + 1];
                                w_tc->knots.num_floats = tnka[k];
                                w_tc->knots.floats = tknots;
                                w_tc->cpts.num_points = tncp[k];
                                if ( ttype[k] == PNONRA) {
                                    w_tc->cpts.points.point2d = w_pt2;
                                    w_pt2 += tncp[k];
                                } else {
                                    w_tc->cpts.points.point3d = w_pt3;
                                    w_pt3 += tncp[k];
                                }
                                tknots += tnka[k];
                                w_tc++;
                                k++;
                            }
                            w_itl = itl[i];
                        }
                    }

                } else {
                    surf->trimloops = (Ptrimcurve_list *)NULL;
                    surf->num_tcurves = 0;
                    surf->num_tknots = 0;
                    surf->num_3D_tpoints = 0;
                    surf->num_2D_tpoints = 0;
                }
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
		    CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
                PFREE( dim, surf->grid);
                PFREE( *ntl, surf->trimloops);
                PFREE( surf->num_tcurves, tc);
                PFREE( surf->num_2D_tpoints, pt2);
                PFREE( surf->num_3D_tpoints, pt3);
            }
	}
    }
}


/* SET SURFACE APPROXIMATION CRITERIA */
void
pssac_( acri, aval)
Pint	*acri;		/* surface approximation criteria type	*/
Pfloat	*aval;		/* approximation value	*/ 
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_surf_approx)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else {
	    args->el_type = PELEM_SURF_APPROX_CRIT;
	    ed.surf_approx.type = *acri;
	    ed.surf_approx.u_val = aval[0];
	    ed.surf_approx.v_val = aval[1];
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}
