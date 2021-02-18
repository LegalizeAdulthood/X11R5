#ifndef lint
static char     sccsid[] = "@(#)fb_text.c 1.1 91/09/07 FJ";
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

/* Text functions for the PHIGS FORTRAN binding */

/* 
 * ANNOTATION TEXT RELATIVE
 * ANNOTATION TEXT RELATIVE 3 
 * INQUIRE LIST OF TEXT INDICES 
 * SET ANNOTATION TEXT ALIGNMENT 
 * SET ANNOTATION TEXT CHARACTER HEIGHT 
 * SET ANNOTATION TEXT CHARACTER UP VECTOR 
 * SET ANNOTATION TEXT PATH 
 * SET ANNOTATION STYLE
 * SET CHARACTER EXPANSION FACTOR 
 * SET CHARACTER HEIGHT 
 * SET CHARACTER SPACING 
 * SET CHARACTER UP VECTOR 
 * SET TEXT ALIGNMENT 
 * SET TEXT COLOUR INDEX 
 * SET TEXT FONT 
 * SET TEXT INDEX 
 * SET TEXT PATH 
 * SET TEXT REPRESENTATION 
 * TEXT 
 * TEXT 3 
 */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"



/* TEXT 3 */
void
ftx3_( px, py, pz, tdx, tdy, tdz, chars, length)
Pfloat	*px;		/* text position X	*/
Pfloat	*py;		/* text position Y	*/
Pfloat	*pz;		/* text position Z	*/
Pfloat 	*tdx;		/* text direction vectors X	*/
Pfloat 	*tdy;		/* text direction vectors Y	*/
Pfloat 	*tdz;		/* text direction vectors Z	*/
char	*chars;		/* character string	*/
int	*length;	/* string length	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_text3)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	
	
	} else {
	    args->el_type = PELEM_TEXT3;
	    ed.text3.pt.x = *px;
	    ed.text3.pt.y = *py;
	    ed.text3.pt.z = *pz;
	    ed.text3.dir[0].delta_x = tdx[0];
	    ed.text3.dir[0].delta_y = tdy[0];
	    ed.text3.dir[0].delta_z = tdz[0];
	    ed.text3.dir[1].delta_x = tdx[1];
	    ed.text3.dir[1].delta_y = tdy[1];
	    ed.text3.dir[1].delta_z = tdz[1];
	    if ( (ed.text3.string = chars))
		ed.text3.length = *length + 1;
	    else
		ed.text3.length = 0;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* TEXT */
void
ftx_( px, py, chars, length)
Pfloat	*px;     	/* text position X	*/
Pfloat	*py;     	/* text position Y	*/
char	*chars;		/* character string	*/
int	*length;	/* string length	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_text)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_TEXT;
	    ed.text.pt.x = *px;
	    ed.text.pt.y = *py;
	    if ( (ed.text.string = chars))
		ed.text.length = *length + 1;
	    else
		ed.text.length = 0;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET TEXT FONT */
void
pstxfn_( font)
Pint	*font;  	/* text font	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_text_font)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_TEXT_FONT;
	    ed.idata = *font;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET TEXT PRECISION */
void
pstxpr_( prec)
Pint	*prec;	/* text precision	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_text_prec)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *prec < PSTRP || *prec > PSTRKP) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);
        
        } else {
            args->el_type = PELEM_TEXT_PREC;
	    ed.txprec = (Ptext_prec)*prec;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}


/* SET CHARACTER EXPANSION */
void
pschxp_( chxp)
Pfloat	*chxp;		/* character expansion factor	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_char_expan)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_CHAR_EXPAN;
	    ed.fdata = *chxp;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET CHARACTER SPACING */
void
pschsp_( chsp)
Pfloat	*chsp;		/* character spacing	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_char_space)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_CHAR_SPACE;
	    ed.fdata = *chsp;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET TEXT COLOUR INDEX */
void
pstxci_( coli)
Pint	*coli;		/* text colour index	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_text_colr_ind)) {
	    if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	        ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	    } else if ( *coli < 0) {
	        ERR_REPORT( phg_cur_cph->erh, ERR113);
	
	    } else {
	        args->el_type = PELEM_TEXT_COLR_IND;
	        ed.idata = *coli;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	            CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	    }
    }
}


/* SET TEXT COLOUR */
void
pstxc_( ctype, coli, colr)
Pint	*ctype;		/* text colour type	*/
Pint	*coli;		/* colour index	*/
Pfloat	*colr;		/* colour components	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_text_colr)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_TEXT_COLR;
            GCOLR_DATA_TRANSLATE( *ctype, *coli, colr, ed.colour);
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET CHARACTER HEIGHT */
void
pschh_( chh)
Pfloat	*chh;		/* character height	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_char_ht)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_CHAR_HT;
	    ed.fdata = *chh;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET CHARACTER UP VECTOR */
void
pschup_( chux, chuy)
Pfloat  *chux;          /* character up vector X	*/
Pfloat  *chuy;          /* character up vector Y	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_char_up_vec)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_CHAR_UP_VEC;
	    ed.char_up.delta_x = *chux;
	    ed.char_up.delta_y = *chuy;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET TEXT PATH */
void
pstxp_( txp)
Pint	*txp;	/* text path	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_text_path)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *txp < PRIGHT || *txp > PDOWN) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);
        
        } else {
            args->el_type = PELEM_TEXT_PATH;
	    ed.txpath = (Ptext_path)*txp;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}


/* SET TEXT ALIGNMENT */
void
pstxal_( txalh, txalv)
Pint	*txalh;         /* text alignment horizontal */
Pint	*txalv;         /* text alignment vertical   */
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_text_align)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if (( *txalh < PAHNOR || *txalh > PARITE) ||
                  ( *txalv < PAVNOR || *txalv > PABOTT)) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);
        
        } else {
            args->el_type = PELEM_TEXT_ALIGN;
	    ed.txalign.hor = (Phor_text_align)*txalh;
            ed.txalign.vert = (Pvert_text_align)*txalv;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}


/* SET TEXT INDEX */
void
pstxi_( txi)
Pint	*txi;	/* text index	*/
{   
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_text_ind)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *txi < 1) {
	    ERR_REPORT(phg_cur_cph->erh, ERR100);
	
	} else {
	    args->el_type = PELEM_TEXT_IND;
	    ed.idata = *txi;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* ANNOTATION TEXT RELATIVE 3 */
void
fatr3_( rpx, rpy, rpz, apx, apy, apz, chars, length)
Pfloat 	*rpx;		/* reference point X	*/
Pfloat 	*rpy;		/* reference point Y	*/
Pfloat 	*rpz;		/* reference point Z	*/
Pfloat 	*apx;		/* annotation offset X	*/
Pfloat 	*apy;		/* annotation offset Y	*/
Pfloat 	*apz;		/* annotation offset Z	*/
char	*chars;		/* character string	*/
int	*length;	/* string length	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_anno_text_rel3)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	

	} else {
	    args->el_type = PELEM_ANNO_TEXT_REL3;
	    ed.anno_text_rel3.ref_pt.x = *rpx;
	    ed.anno_text_rel3.ref_pt.y = *rpy;
	    ed.anno_text_rel3.ref_pt.z = *rpz;
	    ed.anno_text_rel3.offset.x = *apx;
	    ed.anno_text_rel3.offset.y = *apy;
	    ed.anno_text_rel3.offset.z = *apz;
	    if ( (ed.anno_text_rel3.string = chars))
		ed.anno_text_rel3.length = *length + 1;
	    else
		ed.anno_text_rel3.length = 0;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* ANNOTATION TEXT RELATIVE */
void
fatr_( rpx, rpy, apx, apy, chars, length)
Pfloat	*rpx;		/* reference point X	*/
Pfloat	*rpy;		/* reference point Y	*/
Pfloat	*apx;		/* annotation offset X	*/
Pfloat	*apy;		/* annotation offset Y	*/
char	*chars;		/* characters string	*/
int	*length;	/* string length	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_anno_text_rel)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);	

	} else {
	    args->el_type = PELEM_ANNO_TEXT_REL;
	    ed.anno_text_rel.ref_pt.x = *rpx;
	    ed.anno_text_rel.ref_pt.y = *rpy;
	    ed.anno_text_rel.offset.x = *apx;
	    ed.anno_text_rel.offset.y = *apy;
	    if ( (ed.anno_text_rel.string = chars))
		ed.anno_text_rel.length = *length + 1;
	    else
		ed.anno_text_rel.length = 0;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET ANNOTATION TEXT CHARACTER HEIGHT */
void
psatch_( atchh)
Pfloat	*atchh;		/* annotation text character height	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_anno_char_ht)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_ANNO_CHAR_HT;
	    ed.fdata = *atchh;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET ANNOTATION TEXT CHARACTER UP VECTOR */
void
psatcu_( atchux, atchuy)
Pfloat  *atchux;      /* annotation text character up vector X	*/
Pfloat  *atchuy;      /* annotation text character up vector Y	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_anno_char_up_vec)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_ANNO_CHAR_UP_VEC;
	    ed.char_up.delta_x = *atchux;
	    ed.char_up.delta_y = *atchuy;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET ANNOTATION TEXT PATH */
void
psatp_( atp)
Pint	*atp;	/* annotation text path	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_anno_path)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *atp < PRIGHT || *atp > PDOWN) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else {
            args->el_type = PELEM_ANNO_PATH;
	    ed.txpath = (Ptext_path)*atp;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}


/* SET ANNOTATION TEXT ALIGNMENT */
void
psatal_( atalh, atalv)
Pint	*atalh;    /* annotation text alignment horizontal */
Pint	*atalv;    /* annotation text alignment vertical   */
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_anno_align)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if (( *atalh < PAHNOR || *atalh > PARITE) ||
                  ( *atalv < PAVNOR || *atalv > PABOTT)) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else {
            args->el_type = PELEM_ANNO_ALIGN;
	    ed.txalign.hor = (Phor_text_align)*atalh;
	    ed.txalign.vert = (Pvert_text_align)*atalv;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}


/* SET ANNOTATION STYLE */
void
psans_( astyle)
Pint	*astyle;	/* annotation style	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_anno_style)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_ANNO_STYLE;
	    ed.idata = *astyle;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}
