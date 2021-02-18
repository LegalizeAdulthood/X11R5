#ifndef lint
static char     sccsid[] = "@(#)fb_pmrk.c 1.1 91/09/07 FJ";
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

/* Polymarker functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"



/* POLYMARKER 3 */
void
ppm3_( n, pxa, pya, pza)
Pint	*n;		/* number of points	*/
Pfloat	*pxa;		/* coordinates of points X	*/
Pfloat	*pya;		/* coordinates of points Y	*/
Pfloat	*pza;		/* coordinates of points Z	*/
{   
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Ppoint3		*points = NULL;
    int			err = 0;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_polymarker3)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else if ( *n < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);	
	
	} else {
            PMALLOC( err, Ppoint3, *n, points);
            if ( err) {
	        ERR_REPORT( phg_cur_cph->erh, err);	
	
	    } else {
                POINT3_DATA_TRANSLATE( *n, pxa, pya, pza, points);
	        args->el_type = PELEM_POLYMARKER3;
	        ed.ptlst3.num_points = *n;
	        ed.ptlst3.points = points;
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	            CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
                PFREE( *n, points);
            }
	}
    }
}


/* POLYMARKER */
void
ppm_( n, pxa, pya)
Pint	*n;		/* number of points	*/
Pfloat	*pxa;		/* coordinates of points X	*/
Pfloat	*pya;		/* coordinates of points Y	*/
{   
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Ppoint		*points = NULL;
    int			err = 0;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_polymarker)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else if ( *n < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);	
	
	} else {
            PMALLOC( err, Ppoint, *n, points);
            if ( err) {
	        ERR_REPORT(phg_cur_cph->erh, ERR900);	
	
	    } else {
                POINT_DATA_TRANSLATE(*n, pxa, pya, points);
	        args->el_type = PELEM_POLYMARKER;
	        ed.ptlst.num_points = *n;
	        ed.ptlst.points = points;
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	            CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
                PFREE( *n, points);
            }
	}
    }
}


/* SET MARKER TYPE */
void
psmk_( mtype)
Pint	*mtype;		/* markertype	*/
{   
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_marker_type)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else {
	    args->el_type = PELEM_MARKER_TYPE;
	    ed.idata = *mtype;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET MARKER SIZE SCALE FACTOR */
void
psmksc_( mszsf)
Pfloat	*mszsf;		/* markersize scale factor	*/
{   
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_marker_size)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else {
	    args->el_type = PELEM_MARKER_SIZE;
	    ed.fdata = *mszsf;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET POLYMARKER COLOUR INDEX */
void
pspmci_( coli)
Pint	*coli;		/* polymarker colour index	*/
{   
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_marker_colr_ind)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else if ( *coli < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR113);
	
	} else {
	    args->el_type = PELEM_MARKER_COLR_IND;
	    ed.idata = *coli;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET POLYMARKER COLOUR */
void
pspmc_( ctype, coli, colr)
Pint	*ctype;		/* polymarker colour type	*/
Pint	*coli;		/* colour index	*/
Pfloat	*colr;		/* colour components	*/
{   
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_marker_colr)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else {
	    args->el_type = PELEM_MARKER_COLR;
            GCOLR_DATA_TRANSLATE( *ctype, *coli, colr, ed.colour);
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET POLYMARKER INDEX */
void
pspmi_( pmi)
Pint	*pmi;		/* polymarker index	*/
{   
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_marker_ind)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else if ( *pmi < 1) {
	    ERR_REPORT( phg_cur_cph->erh, ERR100);	
	
	} else {
	    args->el_type = PELEM_MARKER_IND;
	    ed.idata = *pmi;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}
