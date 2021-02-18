#ifndef lint
static char     sccsid[] = "@(#)fb_nset.c 1.1 91/09/07 FJ";
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

/* Name set functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"
 


/* ADD NAMES TO SET */
void
pads_( n, namset)
Pint	*n;		/* number of names in the set	*/
Pint	*namset;	/* names set	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_add_names_set)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *n < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);
        
	} else {
	    args->el_type = PELEM_ADD_NAMES_SET;
	    ed.name_set.num_ints = *n;
	    ed.name_set.ints = namset;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* REMOVE NAMES FROM SET */
void
pres_( n, namset)
Pint	*n;		/* number of names in the set	*/
Pint	*namset;	/* names set	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_remove_names_set)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *n < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);
        
	} else {
	    args->el_type = PELEM_REMOVE_NAMES_SET;
	    ed.name_set.num_ints = *n;
	    ed.name_set.ints = namset;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}
