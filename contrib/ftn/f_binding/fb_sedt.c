#ifndef lint
static char     sccsid[] = "@(#)fb_sedt.c 1.1 91/09/07 FJ";
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

/* Structure edit functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"



/* OPEN STRUCTURE */
void
popst_( strid)
Pint	*strid;		/* structure identifier	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR6, Pfn_open_struct)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTCL) {
	    ERR_REPORT( phg_cur_cph->erh, ERR6);

	} else {
	    cp_args.data.idata = *strid;
        ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_OPEN_STRUCT, &cp_args, &ret);
	    if ( !ret.err)
		PSL_STRUCT_STATE( phg_cur_cph->psl) = PSTOP;
	    ERR_FLUSH( phg_cur_cph->erh);
	}
    }
}


/* CLOSE STRUCTURE */
void
pclst_()
{
    Phg_args		cp_args;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_close_struct)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

	} else {
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_CLOSE_STRUCT, &cp_args, NULL);
	    PSL_STRUCT_STATE( phg_cur_cph->psl) = PSTCL;
	}
    }
}


/* COPY ALL ELEMENTS FROM STRUCTURE */
void
pcelst_( strid)
Pint	*strid;		/* structure identifier	*/
{
    Phg_args		cp_args;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_copy_all_elems_struct)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

	} else {
	    cp_args.data.idata = *strid;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_COPY_ALL_ELS, &cp_args, NULL);
	}
    }
}


/* DELETE ELEMENT */
void
pdel_()
{
    Phg_args		cp_args;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_del_elem)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

	} else {
	    cp_args.data.del_el.op = PHG_ARGS_DEL_CURRENT;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_DELETE_EL, &cp_args, NULL);
	}
    }
}


/* DELETE ELEMENT RANGE */
void
pdelra_( ep1, ep2)
Pint	*ep1;		/* element pointer 1	*/
Pint	*ep2;		/* element pointer 2	*/
{
    Phg_args			cp_args;
    register Phg_args_del_el	*args = &cp_args.data.del_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_del_elem_range)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

	} else {
	    args->op = PHG_ARGS_DEL_RANGE;
	    args->data.ep_values.ep1 = MAX( 0, *ep1); /* filter < 0 */
	    args->data.ep_values.ep2 = *ep2;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_DELETE_EL, &cp_args, NULL);
	}
    }
}


/* DELETE ELEMENTS BETWEEN LABELS */
void
pdellb_( label1, label2)
Pint	*label1;	/* label identifier 1	*/
Pint	*label2;	/* label identifier 2	*/
{
    Phg_args			cp_args;
    register Phg_args_del_el	*args = &cp_args.data.del_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_del_elems_labels)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

	} else {
	    args->op = PHG_ARGS_DEL_LABEL;
	    args->data.label_range.label1 = *label1;
	    args->data.label_range.label2 = *label2;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_DELETE_EL, &cp_args, NULL);
	}
    }
}


/* EMPTY STRUCTURE */
void
pemst_( strid)
Pint	*strid;		/* structure identifier	*/
{
    Phg_args			cp_args;
    register Phg_args_del_el	*args = &cp_args.data.del_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_empty_struct)) {
	args->op = PHG_ARGS_EMPTY_STRUCT;
	args->data.struct_id = *strid;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_DELETE_EL, &cp_args, NULL);
    }
}


/* DELETE STRUCTURE */
void
pdst_( strid)
Pint	*strid;		/* structure identifier	*/
{
    Phg_args			cp_args;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_del_struct)) {
	cp_args.data.idata = *strid;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_DELETE_STRUCT, &cp_args, NULL);
    }
}


/* DELETE STRUCTURE NETWORK */
void
pdsn_( strid, refhnf)
Pint	*strid;		/* structure identifier	*/
Pint	*refhnf;	/* reference handling flag	*/
{
    Phg_args				cp_args;
    register Phg_args_del_struct_net	*args = &cp_args.data.del_struct_net;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_del_struct_net)) {
        if ( *refhnf < PDELE || *refhnf > PKEEP) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else {
            args->id = *strid;
	    args->flag = (Pref_flag)*refhnf;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_DELETE_STRUCT_NET, &cp_args, NULL);
        }
    }
}


/* DELETE ALL STRUCTURES */
void
pdas_()
{
    Phg_args	cp_args;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_del_all_struct)) {
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_DELETE_ALL_STRUCTS, &cp_args, NULL);
    }
}


/* CHANGE STRUCTURE IDENTIFIER */
void
pcstid_( oldsid, newsid)
Pint	*oldsid;	/* original structure identifier	*/
Pint	*newsid;	/* resulting structure identifier	*/
{
    Phg_args				cp_args;
    register Phg_args_change_struct	*args = &cp_args.data.change_struct;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_change_struct_id)) {
	args->orig_id = *oldsid;
	args->new_id = *newsid;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_CHANGE_STRUCT_ID, &cp_args, NULL);
    }
}


/* CHANGE STRUCTURE REFERENCES */
void
pcstrf_( oldsid, newsid)
Pint	*oldsid;	/* original structure identifier	*/
Pint	*newsid;	/* resulting structure identifier	*/
{
    Phg_args				cp_args;
    register Phg_args_change_struct	*args = &cp_args.data.change_struct;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_change_struct_refs)) {
	args->orig_id = *oldsid;
	args->new_id = *newsid;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_CHANGE_STRUCT_REFS, &cp_args, NULL);
    }
}


/* CHANGE STRUCTURE IDENTIFIER AND REFERENCES */
void
pcstir_( oldsid, newsid)
Pint	*oldsid;	/* original structure identifier	*/
Pint	*newsid;	/* resulting structure identifier	*/
{
    Phg_args				cp_args;
    register Phg_args_change_struct	*args = &cp_args.data.change_struct;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_change_struct_id_refs)) {
	args->orig_id = *oldsid;
	args->new_id = *newsid;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_CHANGE_STRUCT_IDREFS, &cp_args, NULL);
    }
}


/* SET ELEMENT POINTER */
void
psep_( ep)
Pint	*ep;		/* element position	*/
{
    Phg_args				cp_args;
    register Phg_args_set_el_ptr	*args = &cp_args.data.set_el_ptr;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_elem_ptr)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

	} else {
	    args->op = PHG_ARGS_SETEP_ABS;
	    args->data = *ep;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_EL_PTR, &cp_args, NULL);
	}
    }
}


/* OFFSET ELEMENT POINTER */
void
posep_( epo)
Pint	*epo;		/* element position offset	*/
{
    Phg_args				cp_args;
    register Phg_args_set_el_ptr	*args = &cp_args.data.set_el_ptr;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_offset_elem_ptr)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

	} else {
	    args->op = PHG_ARGS_SETEP_REL;
	    args->data = *epo;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_EL_PTR, &cp_args, NULL);
	}
    }
}


/* SET ELEMENT POINTER AT LABEL */
void
pseplb_( label)
Pint	*label;		/* label identifier	*/
{
    Phg_args				cp_args;
    register Phg_args_set_el_ptr	*args = &cp_args.data.set_el_ptr;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5,  Pfn_set_elem_ptr_label)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

	} else {
	    args->op = PHG_ARGS_SETEP_LABEL;
	    args->data = *label;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_EL_PTR, &cp_args, NULL);
	}
    }
}


/* SET EDIT MODE */
void
psedm_( editmo)
Pint	*editmo;	/* edit mode	*/
{
    Phg_args	cp_args;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_set_edit_mode)) {
        if ( *editmo < PINSRT || *editmo > PREPLC) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else {
            cp_args.data.idata = *editmo;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_EDIT_MODE, &cp_args, NULL);
            phg_cur_cph->psl->edit_mode = *editmo;
        }
    }
}


/* INQUIRE ELEMENT POINTER */
void
pqep_( errind, ep)
Pint	*errind;	/* OUT error indicator	*/
Pint	*ep;		/* OUT element position value	*/
{
    Phg_args	cp_args;
    Phg_ret	ret;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR5;

    } else if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	*errind = ERR5;

    } else {
	ret.err = 0;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_EL_PTR, &cp_args, &ret);
	if ( ret.err) {
	    *errind = ret.err;
	} else {
	    *errind = 0;
	    *ep = ret.data.idata;
	}
    }
}


/* INQUIER EDIT MODE */
void
pqedm_( errind, editmo)
Pint	*errind;	/* OUT error indicator	*/
Pint	*editmo;	/* OUT edit mode	*/
{

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else {
        *errind = 0;
        *editmo = (Pint)phg_cur_cph->psl->edit_mode;
    }
}
