#ifndef lint
static char     sccsid[] = "@(#)fb_plin.c 1.1 91/09/07 FJ";
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

/* Polyline functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"



/* POLYLINE 3 */
void
ppl3_( n, pxa, pya, pza)
Pint	*n;		/* number of points	*/
Pfloat  *pxa;		/* coordinates of points X	*/
Pfloat  *pya;		/* coordinates of points Y	*/
Pfloat  *pza;		/* coordinates of points Z	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Ppoint3		*points = NULL;
    int			err = 0;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_polyline3)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else if ( *n < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);	
	
	} else {
            PMALLOC( err, Ppoint3, *n, points);
            if ( err) {
	        ERR_REPORT(phg_cur_cph->erh, err);	
	
	    } else {
	        POINT3_DATA_TRANSLATE( *n, pxa, pya, pza, points);
                args->el_type = PELEM_POLYLINE3;
	        ed.ptlst3.num_points = *n;
	        ed.ptlst3.points = points;
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	            CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
                PFREE( *n, points);
            }
	}
    }
}


/* POLYLINE SET 3 WITH DATA */
void
pplsd3_( vflag, ctype, npl, ixa, pxa, pya, pza, vcoli, vcolr)
Pint	*vflag;		/* data per vertex flag */
Pint	*ctype;		/* colour model */
Pint	*npl;		/* number of point lists	*/
Pint	*ixa;		/* array of indices for point lists	*/
Pfloat  *pxa;	       	/* coordinate of points X	*/
Pfloat  *pya;	       	/* coordinate of points Y	*/
Pfloat  *pza;	       	/* coordinate of points Z	*/
Pint 	*vcoli;       	/* colour indices	*/
Pfloat	*vcolr;       	/* colour components	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    register int		w_ixa = 0;
    register int		i,j,k;
    Pint			err = 0;
    Pfacet_vdata_list3		*vdata = NULL;
    Ppoint3			*points = NULL;
    Pptco3			*ptcolrs = NULL;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_polyline_set3_data)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else if ( *npl < 1) {
            ERR_REPORT( phg_cur_cph->erh, ERR2005);

        } else {
            PMALLOC( err, Pfacet_vdata_list3, *npl, vdata);
            if ( err) {
                ERR_REPORT( phg_cur_cph->erh, err);
                return;
            }
            ed.pl_set3_d.vdata = vdata;
            w_ixa = 0;
            for ( i = 0; i < *npl; i++) {
                if ( (vdata[i].num_vertices = ixa[i] - w_ixa) < 2) {
                    ERR_REPORT( phg_cur_cph->erh, ERR2005);
                    PFREE( *npl, vdata);
                    return;
                }
                w_ixa = ixa[i];
            }
            args->el_type = PELEM_POLYLINE_SET3_DATA;
            ed.pl_set3_d.vflag = *vflag;
            ed.pl_set3_d.colour_model = *ctype;
            ed.pl_set3_d.num_sets = *npl;
            ed.pl_set3_d.num_vertices = ixa[*npl - 1];

            switch ( *vflag) {
            case PCOORD:
                PMALLOC( err, Ppoint3, ed.pl_set3_d.num_vertices, points);
                if ( err) {
                    ERR_REPORT( phg_cur_cph->erh, err);
                    PFREE( *npl, vdata);
                    return;

                } else {
                    w_ixa = 0;
                    for ( i = 0; i < *npl; i++) {
                        for ( j = w_ixa; j < ixa[i]; j++) {
                            points[j].x = pxa[j];
                            points[j].y = pya[j];
                            points[j].z = pza[j];
                        }
                        vdata[i].vertex_data.points = &(points[w_ixa]);
                        w_ixa = ixa[i];
                    }
                }
                break;

            case PCCOLR:
                PMALLOC( err, Pptco3, ed.pl_set3_d.num_vertices, ptcolrs);
                if ( err) {
                    ERR_REPORT( phg_cur_cph->erh, err);
                    PFREE( *npl, vdata);
                    return;

                } else {
                    w_ixa = 0;
                    for ( i = 0; i < *npl; i++) {
                        for ( j = w_ixa; j < ixa[i]; j++) {
                            ptcolrs[j].point.x = pxa[j];
                            ptcolrs[j].point.y = pya[j];
                            ptcolrs[j].point.z = pza[j];
                        }
                        vdata[i].vertex_data.ptcolrs = &(ptcolrs[w_ixa]);
                        w_ixa = ixa[i];
                    }
                    w_ixa = 0;
                    k = 0;
                    switch ( *ctype) {
                    case PINDCT:
                        for ( i = 0; i < *npl; i++) {
                            for ( j = w_ixa; j < ixa[i]; j++) {
                                ptcolrs[j].colr.ind = vcoli[j];
                            }
                            w_ixa = ixa[i];
                        }
                        break;
                    case PRGB:
                        for ( i = 0; i < *npl; i++) {
                            for ( j = w_ixa; j < ixa[i]; j++) {
                                ptcolrs[j].colr.direct.rgb.red =
                                    vcolr[k];
                                ptcolrs[j].colr.direct.rgb.green =
                                    vcolr[k + 1];
                                ptcolrs[j].colr.direct.rgb.blue = 
                                    vcolr[k + 2];
                                k += 3;
                            }
                            w_ixa = ixa[i];
                        }
                        break;
                    case PCIE:
                        for ( i = 0; i < *npl; i++) {
                            for ( j = w_ixa; j < ixa[i]; j++) {
                                ptcolrs[j].colr.direct.cieluv.cieluv_x =
                                    vcolr[k];
                                ptcolrs[j].colr.direct.cieluv.cieluv_y =
                                    vcolr[k + 1];
                                ptcolrs[j].colr.direct.cieluv.cieluv_y_lum =
                                    vcolr[k + 2];
                                k += 3;
                            }
                            w_ixa = ixa[i];
                        }
                        break;
                    case PHSV:
                        for ( i = 0; i < *npl; i++) {
                            for ( j = w_ixa; j < ixa[i]; j++) {
                                ptcolrs[j].colr.direct.hsv.hue =
                                    vcolr[k];
                                ptcolrs[j].colr.direct.hsv.satur =
                                    vcolr[k + 1];
                                ptcolrs[j].colr.direct.hsv.value =
                                    vcolr[k + 1];
                                k += 3;
                            }
                            w_ixa = ixa[i];
                        }
                        break;
                    case PHLS:
                        for ( i = 0; i < *npl; i++) {
                            for ( j = w_ixa; j < ixa[i]; j++) {
                                ptcolrs[j].colr.direct.hls.hue =
                                    vcolr[k];
                                ptcolrs[j].colr.direct.hls.lightness =
                                    vcolr[k + 1];
                                ptcolrs[j].colr.direct.hls.satur =
                                    vcolr[k + 2];
                                k += 3;
                            }
                            w_ixa = ixa[i];
                        }
                        break;
                    }
                }
                break;
            }
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);

            PFREE( *npl, vdata);
            switch ( *vflag) {
            case PCOORD:
                PFREE( ed.pl_set3_d.num_vertices, points);
                break;
            case PCCOLR:
                PFREE( ed.pl_set3_d.num_vertices, ptcolrs);
                break;
            }
        }
    }
}


/* POLYLINE */
void
ppl_( n, pxa, pya)
Pint	*n;		/* number of points	*/
Pfloat	*pxa;		/* coordinates of points X	*/
Pfloat	*pya;		/* coordinates of points Y	*/
{   
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Ppoint		*points = NULL;
    int			err = 0;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_polyline)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else if ( *n < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);	
	
	} else {
	    PMALLOC( err, Ppoint, *n, points);
            if ( err) { 
	        ERR_REPORT( phg_cur_cph->erh, err);	
	
	    } else {
                POINT_DATA_TRANSLATE( *n, pxa, pya, points);
	        args->el_type = PELEM_POLYLINE;
	        ed.ptlst.num_points = *n;
	        ed.ptlst.points = points;
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	            CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
                PFREE( *n, points);
            }
	}
    }
}


/* SET LINETYPE */
void
psln_( ltype)
Pint	*ltype;		/* linetype	*/
{   
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_linetype)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else {
	    args->el_type = PELEM_LINETYPE;
	    ed.idata = *ltype;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET LINEWIDTH SCALE FACTOR */
void
pslwsc_( lwidth)
Pfloat	*lwidth;	/* linewidth scale factor	*/
{   
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_linewidth)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else {
	    args->el_type = PELEM_LINEWIDTH;
	    ed.fdata = *lwidth;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET POLYLINE COLOUR INDEX */
void
psplci_( coli)
Pint	*coli;		/* polyline colour index	*/
{   
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_line_colr_ind)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else if ( *coli < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR113);
	
	} else {
	    args->el_type = PELEM_LINE_COLR_IND;
	    ed.idata = *coli;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET POLYLINE COLOUR */
void
psplc_( ctype, coli, colr)
Pint	*ctype;		/* polyline colour type	*/
Pint	*coli;		/* colour index	*/
Pfloat	*colr;		/* colour components	*/
{   
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_line_colr)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else {
	    args->el_type = PELEM_LINE_COLR;
            GCOLR_DATA_TRANSLATE( *ctype, *coli, colr, ed.colour);
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET POLYLINE INDEX */
void
pspli_( pli)
Pint	*pli;		/* polyline index	*/
{   
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_line_ind)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else if ( *pli < 1) {
	    ERR_REPORT(phg_cur_cph->erh, ERR100);	
	
	} else {
	    args->el_type = PELEM_LINE_IND;
	    ed.idata = *pli;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET POLYLINE SHADING METHOD */
void
psplsm_( plsm)
Pint	*plsm;		/* shading method	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_line_shad_meth)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else {
	    args->el_type = PELEM_LINE_SHAD_METH;
	    ed.idata = *plsm;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }    
}
