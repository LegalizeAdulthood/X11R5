#ifndef lint
static char     sccsid[] = "@(#)fb_cell.c 1.1 91/09/07 FJ";
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

/* Cell array functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"



/* CELL ARRAY 3 */
void
pca3_( cpxa, cpya, cpza, dimx, dimy, isc, isr, dx, dy, colia)
Pfloat	*cpxa;		/* cell parallelogram(P,Q,R) X	 */
Pfloat	*cpya;		/* cell parallelogram(P,Q,R) Y	 */
Pfloat	*cpza;		/* cell parallelogram(P,Q,R) Z	 */
Pint	*dimx;		/* dimension of cell array X	*/
Pint	*dimy;		/* dimension of cell array Y	*/
Pint	*isc;		/* indices of start column	*/
Pint	*isr;		/* indices of start rows	*/
Pint	*dx;		/* number of columns	*/
Pint	*dy;		/* number of rows	*/
Pint	*colia;		/* colour index array	*/
{
    Phg_args			cp_args;
    Phg_el_data			ed;
    Phg_args_add_el		*args = &cp_args.data.add_el;
    Ppat_rep			rep;
    register int		i;
    int				err = 0;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_cell_array3)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else if ( *dx < 0 || *dy < 0) { 
            ERR_REPORT( phg_cur_cph->erh, ERR117);

        } else if ( *dimx < 0 || *dimy < 0) { 
            ERR_REPORT( phg_cur_cph->erh, ERR2004);
	
        } else if ( *isc < 1 || *isr < 1) {
                ERR_REPORT( phg_cur_cph->erh, ERR2004);

        } else if ( *dimx < ((*isc)+(*dx)-1) || *dimy < ((*isr)+(*dy)-1)) {
                ERR_REPORT( phg_cur_cph->erh, ERR2004);
         
        } else {
            for ( i = 0; i < (*dimx)*(*dimy); i++) {
                if ( colia[i] < 0) {
                    ERR_REPORT( phg_cur_cph->erh, ERR113);
                        return;
                }
            }

            PMALLOC( err, Pint, ((*dx)*(*dy)), rep.colr_array);
            if ( err) {
                ERR_REPORT( phg_cur_cph->erh, err);

            } else {
                COLR_IND_DATA_TRANSLATE( *isc, *isr, *dx, *dy, *dimx, *dimy,
                    colia, rep);
                args->el_type = PELEM_CELL_ARRAY3;
                ed.cell_array3.paral.p.x = cpxa[0];
                ed.cell_array3.paral.p.y = cpya[0];
                ed.cell_array3.paral.p.z = cpza[0];
                ed.cell_array3.paral.q.x = cpxa[1];
                ed.cell_array3.paral.q.y = cpya[1];
                ed.cell_array3.paral.q.z = cpza[1];
                ed.cell_array3.paral.r.x = cpxa[2];
                ed.cell_array3.paral.r.y = cpya[2];
                ed.cell_array3.paral.r.z = cpza[2];
                ed.cell_array3.dim = rep.dims;
                ed.cell_array3.colr = rep.colr_array;
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                    CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
                PFREE( ((*dx)*(*dy)), rep.colr_array);
            }
        }
    }
}


/* CELL ARRAY */
void
pca_( px, py, qx, qy, dimx, dimy, isc, isr, dx, dy, colia)
Pfloat	*px;  		/* point(P) X	*/
Pfloat	*py;  		/* point(P) Y	*/
Pfloat	*qx;  		/* point(Q) X	*/
Pfloat	*qy;  		/* point(Q) Y	*/
Pint	*dimx;		/* dimension of cell array X	*/
Pint	*dimy;		/* dimension of cell array Y	*/
Pint	*isc;		/* indices of start column	*/
Pint	*isr;		/* indices of start rows	*/
Pint	*dx;		/* number of columns	*/
Pint	*dy;		/* number of rows	*/
Pint	*colia;		/* colour index array	*/
{
    Phg_args			cp_args;
    Phg_el_data			ed;
    Phg_args_add_el		*args = &cp_args.data.add_el;
    Ppat_rep			rep;
    register	int		i;
    int				err = 0;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_cell_array)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else if ( *dx < 0 || *dy < 0) { 
            ERR_REPORT( phg_cur_cph->erh, ERR117);

        } else if ( *dimx < 0 || *dimy < 0) { 
            ERR_REPORT( phg_cur_cph->erh, ERR2004);

        } else if ( *isc < 1 || *isr < 1) {
            ERR_REPORT( phg_cur_cph->erh, ERR2004);

        } else if ( *dimx < ((*isc)+(*dx)-1) || *dimy < ((*isr)+(*dy)-1)) {
            ERR_REPORT( phg_cur_cph->erh, ERR2004);

        } else {
            for ( i = 0; i < (*dimx)*(*dimy); i++) {
                if ( colia[i] < 0) {
                    ERR_REPORT( phg_cur_cph->erh, ERR113);
                    return;
                }
            }

            PMALLOC( err, Pint, ((*dx)*(*dy)), rep.colr_array);
            if ( err) {
                ERR_REPORT( phg_cur_cph->erh, err);

            } else {
                COLR_IND_DATA_TRANSLATE( *isc, *isr, *dx, *dy, *dimx, *dimy,
                    colia, rep);
                args->el_type = PELEM_CELL_ARRAY;
                ed.cell_array.rect.p.x = *px;
                ed.cell_array.rect.p.y = *py;
                ed.cell_array.rect.q.x = *qx;
                ed.cell_array.rect.q.y = *qy;
                ed.cell_array.dim = rep.dims;
                ed.cell_array.colr = rep.colr_array;
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                    CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
                PFREE( ((*dx)*(*dy)), rep.colr_array);
            }
        }
    }
}


/* CELL ARRAY PLUS 3 */
void
pcap3_( cpxa, cpya, cpza, dimx, dimy, cvsc, cvsr, dx, dy, ctype, coli, colr)
Pfloat	*cpxa;		/* cell parallelogram(P,Q,R) X	*/
Pfloat	*cpya;		/* cell parallelogram(P,Q,R) Y	*/
Pfloat	*cpza;		/* cell parallelogram(P,Q,R) Z	*/
Pint	*dimx;		/* dimension of cell array X	*/
Pint	*dimy;		/* dimension of cell array Y	*/
Pint	*cvsc;		/* indices of start column	*/
Pint	*cvsr;		/* indices of start rows	*/
Pint	*dx;		/* number of columns	*/
Pint	*dy;		/* number of rows	*/
Pint	*ctype;		/* colour type	*/
Pint	*coli;         	/* colour indices array	*/
Pfloat	*colr;         	/* colour component	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Pcoval		*colrs;
    int			err = 0;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_cell_array3_plus)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);
        
        } else if ( *dx < 0 || *dy < 0) {
            ERR_REPORT( phg_cur_cph->erh, ERR117);
        
        } else if ( *dimx < 0 || *dimy < 0) {
            ERR_REPORT( phg_cur_cph->erh, ERR2004);
        
        } else if ( *cvsc < 1 || *cvsr < 1) {
            ERR_REPORT( phg_cur_cph->erh, ERR2004);

        } else if ( *dimx < ((*cvsc)+(*dx)-1) || *dimy < ((*cvsr)+(*dy)-1)) {
            ERR_REPORT( phg_cur_cph->erh, ERR2004);
         
        } else {
            PMALLOC( err, Pcoval, ((*dx)*(*dy)), colrs);
            if ( err) {
                ERR_REPORT( phg_cur_cph->erh, err);
        
            } else {
                COLR_VAL_DATA_TRANSLATE( *ctype, *cvsc, *cvsr, *dx, *dy, *dimx,
                    *dimy, coli, colr, colrs);
                args->el_type = PELEM_CELL_ARRAY3_PLUS;
                ed.ext_cell_arr3.paral.p.x = cpxa[0];
                ed.ext_cell_arr3.paral.p.y = cpya[0];
                ed.ext_cell_arr3.paral.p.z = cpza[0];
                ed.ext_cell_arr3.paral.q.x = cpxa[1];
                ed.ext_cell_arr3.paral.q.y = cpya[1];
                ed.ext_cell_arr3.paral.q.z = cpza[1];
                ed.ext_cell_arr3.paral.r.x = cpxa[2];
                ed.ext_cell_arr3.paral.r.y = cpya[2];
                ed.ext_cell_arr3.paral.r.z = cpza[2];
                ed.ext_cell_arr3.dim.size_x = *dx;
                ed.ext_cell_arr3.dim.size_y = *dy;
                ed.ext_cell_arr3.colour_model = *ctype;
                ed.ext_cell_arr3.colr = colrs;
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                    CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
                PFREE( ((*dx)*(*dy)), colrs);
            }
        }
    }    
}
