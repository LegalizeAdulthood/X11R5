#ifndef lint
static char     sccsid[] = "@(#)fb_dcue.c 1.1 91/09/07 FJ";
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

/* Depth Cue functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"



/* SET DEPTH CUE INDEX */
void
psdci_( dci)
Pint	*dci;		/* depth cue index	*/
{    
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_dcue_ind)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else if ( *dci < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR119);
	
	} else {
	    args->el_type = PELEM_DCUE_IND;
	    ed.idata = *dci;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET DEPTH CUE REPRESENTATION */
void
psdcr_( wkid, dci, dcmode, dcmin, dcmax, dcsfmi, dcsfmx, ctype, coli, colr)
Pint	*wkid;		/* workstation identifier	*/
Pint	*dci;		/* depth cue bundle index	*/
Pint	*dcmode;	/* depth cue mode	*/
Pfloat	*dcmin;		/* depth cue reference plane MIN	*/
Pfloat	*dcmax;		/* depth cue reference plane MAX	*/
Pfloat	*dcsfmi;	/* depth cue scale factor MIN	*/
Pfloat	*dcsfmx;	/* depth cue scale factor MAX	*/
Pint	*ctype;		/* depth cue colour type	*/
Pint	*coli;		/* colour index	*/
Pfloat	*colr;		/* colour components	*/
{
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    register Wst_phigs_dt	*dt;
    Psl_ws_info			*wsinfo;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR3, Pfn_set_dcue_rep)) {
        if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR3);

        } else if (!(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
            ERR_REPORT( phg_cur_cph->erh, ERR54);

        } else {
            dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
            if ( !( dt->ws_category == POUTIN || dt->ws_category == POUTPT
                || dt->ws_category == PMO) ) {
                ERR_REPORT( phg_cur_cph->erh, ERR59);

            } else if ( !CB_COLOUR_MODEL_SUPPORTED(*ctype)) {
                ERR_REPORT( phg_cur_cph->erh, ERR110);

            } else if ( *dci < 1) {
                ERR_REPORT( phg_cur_cph->erh, ERR120);

            } else if ( *dcmin > *dcmax) {
                ERR_REPORT( phg_cur_cph->erh, ERR130);

#ifdef SX_G
            } else if ( *dcmin < 0 || *dcmax > 1) {
                ERR_REPORT( phg_cur_cph->erh, ERRN840);

            } else if ( *dcsfmi < 0.0 || *dcsfmi > 1.0 ||
                        *dcsfmx < 0.0 || *dcsfmx > 1.0) {
                ERR_REPORT( phg_cur_cph->erh, ERRN841);
#endif

            } else if ( *dcmode < PSUPPR || *dcmode > PALLOW) {
                ERR_REPORT( phg_cur_cph->erh, ERR2000);

            } else {
                args->wsid = *wkid;
                args->type = PHG_ARGS_DCUEREP;
                args->rep.index = *dci;
                args->rep.bundl.dcuerep.mode = (Pdcue_mode)*dcmode;
                args->rep.bundl.dcuerep.ref_planes[0] = *dcmin;
                args->rep.bundl.dcuerep.ref_planes[1] = *dcmax;
                args->rep.bundl.dcuerep.scaling[0] = *dcsfmi;
                args->rep.bundl.dcuerep.scaling[1] = *dcsfmx;
                GCOLR_DATA_TRANSLATE( *ctype, *coli, colr,
                    args->rep.bundl.dcuerep.colr);
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
            }
        }
    }
}


/* INQUIRE DEPTH CUE REPRESENTATION */
void
pqdcr_( wkid, dci, ccsbsz, type, errind, dcmode, dcmin, dcmax, dcsfmi, dcsfmx,
ctype, coli, ol, colr)
Pint	*wkid;		/* workstation identifier	*/
Pint	*dci;		/* depth cue index	*/
Pint	*ccsbsz;	/* colour component buffer size	*/
Pint	*type;		/* type of returned value	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*dcmode;	/* OUT depth cue mode	*/
Pfloat	*dcmin;		/* OUT depth cue reference plane MIN	*/
Pfloat	*dcmax;		/* OUT depth cue reference plane MAX	*/
Pfloat	*dcsfmi;	/* OUT depth cue scale factor MIN	*/
Pfloat	*dcsfmx;	/* OUT depth cue scale factor MAX	*/
Pint	*ctype;		/* OUT colour type	*/
Pint	*coli;		/* OUT colour index	*/
Pint	*ol;		/* OUT number of colour components	*/
Pfloat	*colr;		/* OUT colour specification	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;
    Psl_ws_info		*wsinfo;
    Wst_phigs_dt	*dt;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
	*errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
	*errind = ERR54;

    } else {
	*errind = 0;
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
        if ( !(dt->ws_category == POUTPT ||
               dt->ws_category == POUTIN ||
               dt->ws_category == PMO) ) {
	    *errind = ERR59;

	} else if ( *dci < 0) {
	    *errind = ERR119;

        } else if ( *type < PSET || *type > PREALI) {
	    *errind = ERR2000;

	}

	if ( !*errind) {
	    cp_args.data.q_rep.wsid = *wkid;
	    cp_args.data.q_rep.index = *dci;
	    cp_args.data.q_rep.type = (Pinq_type)*type;
	    cp_args.data.q_rep.rep_type = PHG_ARGS_DCUEREP;
            ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
		&ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else {
		*dcmode = (Pint)ret.data.rep.dcuerep.mode;
		*dcmin = ret.data.rep.dcuerep.ref_planes[0];
		*dcmax = ret.data.rep.dcuerep.ref_planes[1];
		*dcsfmi = ret.data.rep.dcuerep.scaling[0];
		*dcsfmx = ret.data.rep.dcuerep.scaling[1];
                GCOLR_DATA_RE_TRANSLATE_PLUS( ret.data.rep.dcuerep.colr,
                    *ccsbsz, *errind, *ctype, *coli, *ol, colr);
	    }
	}
    }
}
