#ifndef lint
static char     sccsid[] = "@(#)fb_colr.c 1.1 91/09/07 FJ";
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

/* Colour functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"



/* SET COLOUR REPRESENTATION */
void
pscr_( wkid, ci, nccs, cspec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*ci;		/* colour index	*/
Pint	*nccs;		/* number of components of colour specification	*/
Pfloat	*cspec;		/* colour specification	*/
{
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    register Wst_phigs_dt	*dt;

    Pint	colr_type = PRGB;		/* only RGB supported	*/
    static Pint	colr_table[5] = {0, 3, 3, 3, 3}; /* colour model table */
    Pfloat	*pt;
    int		i;

    if ( dt = phg_cb_check_set_rep( phg_cur_cph, Pfn_set_colr_rep, *wkid, 1,
        *ci)) {

        if ( *nccs < 0 || *nccs > colr_table[colr_type]) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);

        } else {
            switch ( colr_type) {
            case PRGB:
                for ( i = 0; i < colr_table[colr_type]; i++) {
                    if ( cspec[i] < 0.0 || cspec[i] > 1.0) {
	                ERR_REPORT( phg_cur_cph->erh, ERR118);
                        return;
                    }
                }
                break;
            /* TODO: other case... */
            }
	    /* Valid color values depend on the color model in use so we
	     * have to catch invalid values in the ws code.
	     */
	    args->wsid = *wkid;
	    args->type = PHG_ARGS_COREP;
	    args->rep.index = *ci;
            pt = (Pfloat *)(&(args->rep.bundl.corep));
            for ( i = 0; i < colr_table[colr_type]; i++) {
                pt[i] = cspec[i];
            }
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
	}
    }
}


/* SET COLOUR MODEL */
void
pscmd_( wkid, cmodel)
Pint	*wkid;		/* workstation identifier	*/
Pint	*cmodel;	/* colour model	*/
{
    Phg_args			cp_args;
    Phg_args_set_colour_model	*args = &cp_args.data.set_colour_model;
    Wst_phigs_dt		*dt;
    Psl_ws_info			*wsinfo;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR3, Pfn_set_colr_model)) {
        if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR3);

	} else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
	    ERR_REPORT( phg_cur_cph->erh, ERR54);	/* ws not open */

	} else {
	    dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
            if ( !(dt->ws_category == POUTIN
                || dt->ws_category == POUTPT
                || dt->ws_category == PMO) ) {
		ERR_REPORT( phg_cur_cph->erh, ERR59);
	    
	    } else if ( !phg_cb_int_in_list( *cmodel,
		dt->out_dt.num_colour_models, dt->out_dt.colour_models)) {
		ERR_REPORT( phg_cur_cph->erh, ERR110);

	    } else {
		args->wsid = *wkid;
		args->model = *cmodel;
		CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_COLOUR_MODEL, &cp_args,
		    NULL);
	    }
	}
    }
}


/* SET RENDERING COLOUR MODEL */
void
psrcm_( rcolm)
Pint    *rcolm;     /* rendering colour model   */
{
    Phg_args            cp_args;
    Phg_el_data         ed;
    Phg_args_add_el     *args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_rendering_colr_model)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else {
            args->el_type = PELEM_RENDERING_COLR_MODEL;
            ed.idata = *rcolm;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}
