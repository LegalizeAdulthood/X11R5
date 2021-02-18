#ifndef lint
static char     sccsid[] = "@(#)fb_stru.c 1.1 91/09/07 FJ";
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

/* Miscellaneous Structure functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"
 


/* SET INDIVIDUAL ASF */
void
psiasf_( aspcid, asfval)
Pint	*aspcid;	/* attribute identifier	*/
Pint	*asfval;	/* attribute source	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_indiv_asf)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
  
	} else if ( (*aspcid < PLN || *aspcid > PSAPCR) ||
	            (*asfval < PBUNDL || *asfval > PINDIV)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2000);
       
        } else {
	    args->el_type = PELEM_INDIV_ASF;
	    ed.asf_info.attr_id = (Paspect)*aspcid;
	    ed.asf_info.asf = (Pasf)*asfval;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET LOCAL TRANSFORMATION 3 */
void
pslmt3_( xfrmt, ctype)
Pmatrix3	xfrmt;		/* transformation matrix	*/
Pint		*ctype;		/* composition type	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Pmatrix3	xform;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_local_tran3)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *ctype < PCPRE || *ctype > PCREPL) {
	    ERR_REPORT(phg_cur_cph->erh, ERR2000);
	
	} else {
	    args->el_type = PELEM_LOCAL_MODEL_TRAN3;
	    ed.local_xform3.comptype = (Pcompose_type)*ctype;
            MATRIX_DATA_TRANSLATE( 3, xfrmt, xform);
	    ed.local_xform3.mat3 = (Pfloat *)xform;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET LOCAL TRANSFORMATION */
void
pslmt_( xfrmt, ctype)
Pmatrix		xfrmt;		/* transformation matrix	*/
Pint		*ctype;		/* composition type	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Pmatrix		xform;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_local_tran)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *ctype < PCPRE || *ctype > PCREPL) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2000);
	
	} else {
	    args->el_type = PELEM_LOCAL_MODEL_TRAN;
	    ed.local_xform.comptype = (Pcompose_type)*ctype;
            MATRIX_DATA_TRANSLATE( 2, xfrmt, xform);
	    ed.local_xform.mat = (Pfloat *)xform;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET GLOBAL TRANSFORMATION 3 */
void
psgmt3_( xfrmt)
Pmatrix3	xfrmt;	/* transformation matrix	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Pmatrix3	xform;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_global_tran3)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

	} else {
	    args->el_type = PELEM_GLOBAL_MODEL_TRAN3;
            MATRIX_DATA_TRANSLATE( 3, xfrmt, xform);
            ed.mat3 = (Pfloat *)xform;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET GLOBAL TRANSFORMATION */
void
psgmt_( xfrmt)
Pmatrix		xfrmt;	/* transformation matrix	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Pmatrix	xform;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_global_tran)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_GLOBAL_MODEL_TRAN;
            MATRIX_DATA_TRANSLATE( 2, xfrmt, xform);
            ed.mat = (Pfloat *)xform;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET VIEW INDEX */
void
psvwi_( viewi)
Pint	*viewi; 	/* view index	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_view_ind)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *viewi < 0) {
	    ERR_REPORT(phg_cur_cph->erh, ERR114);
	
	} else {
	    args->el_type = PELEM_VIEW_IND;
	    ed.idata = *viewi;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* EXECUTE STRUCTURE */
void
pexst_( strid)
Pint	*strid;		/* structure identifier	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_exec_struct)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_EXEC_STRUCT;
	    ed.idata = *strid;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* LABEL */
void
plb_( label)
Pint	*label;		/* label identifier	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_label)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type =  PELEM_LABEL;
	    ed.idata = *label;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* APPLICATION DATA */
void
pap_( ldr, datrec)
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record 	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_appl_data)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
        } else if ( *ldr <= 0) {
	    ERR_REPORT(phg_cur_cph->erh, ERR2003);

	} else {
	    args->el_type = PELEM_APPL_DATA;
	    ed.appl_data.size = (size_t)(*ldr * 80);
	    ed.appl_data.data = datrec;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET PICK IDENTIFIER */
void
pspkid_( pkid)
Pint	*pkid;		/* pick identifier	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_pick_id)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

	} else {
	    args->el_type = PELEM_PICK_ID;
	    ed.idata = *pkid;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET MODELLING CLIPPING VOLUME 3 */
void
psmcv3_( op, nhalfs, halfsp)
Pint	*op;		/* operator	*/
Pint	*nhalfs;	/* number of half-spaces in list	*/
Pfloat	*halfsp;	/* list of half-spaces	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_model_clip_vol3)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else if ( *nhalfs < 0) {
            ERR_REPORT( phg_cur_cph->erh, ERR2004);

        } else {
            args->el_type = PELEM_MODEL_CLIP_VOL3;
            ed.mclip_vol3.op = *op;
            ed.mclip_vol3.hsplst.num_half_spaces = *nhalfs;
            ed.mclip_vol3.hsplst.half_spaces = (Phalf_space3 *)halfsp;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}


/* SET MODELLING CLIPPING VOLUME */
void
psmcv_( op, nhalfs, halfsp)
Pint	*op;		/* operator	*/
Pint	*nhalfs;	/* number of half-spaces in list	*/
Pfloat	*halfsp;	/* list of half-spaces	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_model_clip_vol)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else if ( *nhalfs < 0) {
            ERR_REPORT( phg_cur_cph->erh, ERR2004);

        } else {
            args->el_type = PELEM_MODEL_CLIP_VOL;
            ed.mclip_vol.op = *op;
            ed.mclip_vol.hsplst.num_half_spaces = *nhalfs;
            ed.mclip_vol.hsplst.half_spaces = (Phalf_space *)halfsp;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}


/* SET MODELLING CLIPPING INDICATOR */
void
psmcli_( mclipi)
Pint	*mclipi;	/* modelling clipping indicator	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_model_clip_ind)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

	} else if ( *mclipi < PNCLIP || *mclipi > PCLIP) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else {
            args->el_type = PELEM_MODEL_CLIP_IND;
	    ed.clip_ind = (Pclip_ind)*mclipi;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}


/* RESTORE MODELLING CLIPPING VOLUME */
void
prmcv_()
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_restore_model_clip_vol)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

	} else {
	    args->el_type = PELEM_RESTORE_MODEL_CLIP_VOL;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


#ifdef EXT_GDP
static int
find_id( id,  n, list)
Pint    id;
Pint    n;
Pint    *list;
{
   	register int i;
    for (i = 0 ; i < n ; i++)
      if (list[i] == id) return i;    /* 0 is a valid return value */
    return -1;
}
#endif

/* GENERALIZED DRAWING PRIMITIVE 3 */
void
pgdp3_( n, pxa,pya, pza, primid, ldr, datrec)
Pint	*n;		/* number of points	*/
Pfloat	*pxa;		/* coordinates of points X	*/
Pfloat	*pya;		/* coordinates of points Y	*/
Pfloat	*pza;		/* coordinates of points Z	*/
Pint	*primid;	/* GDP 3 identifier	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record 	*/
{
#ifdef EXT_GDP |
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Ppoint3		*points;
    Pgdp_data3		gdp_data;
    Pdata_rec		rec;
    Pint		w_ia = 0;
    Pint		*ints = NULL;
    int	 		i;
    Pint		err = 0;
    Pint		size = 0;
    Pgdp_facet_vdata_arr3	vdata;
    Pint		*coli;
    Pfloat		*colr,*norm;
    Pint		work1;
    Pint		work2;
    Pint		work3;
    Pint		*ptr_i;
    Pfloat		*ptr_f1;
    Pfloat		*ptr_f2;
    Pfloat		*ptr_f3;
    Pint		ix,fx;
    Pint		float_entry = 0;
    Pint		ind_tbl[16];	/* start and end number of indices */
    Wst_phigs_dt	*dt ;
    Pint		ind ;

    bzero( (char *)ind_tbl, sizeof(Pint)*16 );

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_gdp3)) {
    dt = &phg_cur_cph -> wst_list -> wst -> desc_tbl.phigs_dt ;
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP){
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else if ( *n < 0){
            ERR_REPORT( phg_cur_cph->erh, ERR2004);

        } else if ( (ind = find_id( *primid,
                                    dt->out_dt.num_gdp3,
                                    dt->out_dt.gdp3_ids))< 0){
            ERR_REPORT( phg_cur_cph->erh, ERRN810);

        } else {
            if ( !fb_unpack_datrec( *ldr, datrec, &err, &rec)) {
                switch ( *primid){
                case PGDP3_POLYLINE_SET:
                    if ( rec.data.ia[0] < 1){
                        err = ERR2005;
                    } else if ( rec.data.ia[rec.data.ia[0]] > *n) {
                        err = ERR2005;
                    } else {
                        gdp_data.gdp3_polylineset.polylinelist.num_ints =
                            rec.data.ia[0];
                        PMALLOC( err, Pint, rec.data.ia[0], ints);
                        if ( !err) {
                            gdp_data.gdp3_polylineset.polylinelist.ints = ints;
                            w_ia = 0;
                            for ( i = 1; i <= rec.data.ia[0]; i++) {
                                if ( rec.data.ia[i] < w_ia) {
                                    err = ERR2005;
                                    break;
                                } else {
                                    ints[i - 1] = rec.data.ia[i] - w_ia;
                                    w_ia = rec.data.ia[i];
                                }
                            }
                        }
                    }
                    break;

                case PGDP3_ELLIPSE:
                    if ( *n != 3){
                        err = ERRN811;
                    }
                    /* datrec is unused */
                    break;

                case PGDP3_ELLIPSE_ARC_EP:
                    if ( *n != 7){
                        err = ERRN811;
                    }
                    /* datrec is unused */
                    break;

                case PGDP3_ELLIPSE_ARC_CLOSE_EP:
                    if ( *n != 7){
                        err = ERRN811;
                    } else if ( rec.data.il != 1) {
                        err = ERR2003;
                    } else if ( rec.data.ia[0] < PACFAN ||
                        rec.data.ia[0] > PACCHD ){
                        err = ERR2000;
                    } else {
                        gdp_data.gdp3_ellipsearcclose_ep.closetype
                            = (Parc_close_type)rec.data.ia[0];
                    }
                    break;

                case PGDP3_DISJOINT_POLYLINE:
                    /* datrec is unused */
                    break;

                case PGDP3_FILLAREA_SET_HOLLOW:
                    if ( rec.data.il < 21) {
                        err = ERR2003;
                        break;
                    }
                    gdp_data.gdp3_fillareaset.fflag = rec.data.ia[16];
                    gdp_data.gdp3_fillareaset.eflag = rec.data.ia[17];
                    gdp_data.gdp3_fillareaset.vflag = rec.data.ia[18];
                    gdp_data.gdp3_fillareaset.color_model = rec.data.ia[19];
                    gdp_data.gdp3_fillareaset.fillarealist.num_ints =
                        rec.data.ia[20];
                    /* work1 = number of vertex data */
                    /* work2 = number of edge data */
                    /* work3 = number of facet data */
                    if ( 0 > (work3 = rec.data.ia[20])) {
                        break;
                    }
                    work1 = rec.data.ia[20 + work3];
                    /* check start and end number of indices */
                    ix = 22 + work3;
                    fx = 1;
                    if ( rec.data.ia[17] == PEVIS) {
                        ind_tbl[2] = ix;
                        ix += work3;
                        ind_tbl[3] = ix - 1;
                        work2 = rec.data.ia[20 + (2 * work3)];
                    }
                    switch ( rec.data.ia[16]) {
                    case PNOF:
                        break;
                    case PFCOLR:
                        switch ( rec.data.ia[19]) {
                        case PINDCT:
                            ind_tbl[0] = ix;
                            ix += work3;
                            ind_tbl[1] = ix - 1;
                            break;
                        case PRGB: case PCIE: case PHSV: case PHLS:
                            ind_tbl[8] = fx;
                            fx += work3 * 3;
                            ind_tbl[9] = fx - 1;
                        }
                        break;
                    case PFNORM:
                        ind_tbl[10] = fx;
                        fx += work3 * 3;
                        ind_tbl[11] = fx - 1;
                        break;
                    case PFCONO:
                        switch ( rec.data.ia[19]) {
                        case PINDCT:
                            ind_tbl[0] = ix;
                            ix += work3;
                            ind_tbl[1] = ix - 1;
                            break;
                        case PRGB: case PCIE: case PHSV: case PHLS:
                            ind_tbl[8] = fx;
                            fx += work3 * 3;
                            ind_tbl[9] = fx - 1;
                        }
                        ind_tbl[10] = fx;
                        fx += work3 * 3;
                        ind_tbl[11] = fx - 1;
                        break;
                    }
                    if ( rec.data.ia[17] == PEVIS) {
                        ind_tbl[4] = ix;
                        ix += work2;
                        ind_tbl[5] = ix - 1;
                    }
                    switch ( rec.data.ia[18]) {
                    case PCOORD:
                        break;
                    case PCCOLR:
                        switch ( rec.data.ia[19]) {
                        case PINDCT:
                            ind_tbl[6] = ix;
                            ix += work1;
                            ind_tbl[7] = ix - 1;
                            break;
                        case PRGB: case PCIE: case PHSV: case PHLS:
                            ind_tbl[12] = fx;
                            fx += work1 * 3;
                            ind_tbl[13] = fx - 1;
                        }
                        break;
                    case PCNORM:
                        ind_tbl[14] = fx;
                        fx += work1 * 3;
                        ind_tbl[15] = fx - 1;
                        break;
                    case PCCONO:
                        switch ( rec.data.ia[19]) {
                        case PINDCT:
                            ind_tbl[6] = ix;
                            ix += work1;
                            ind_tbl[7] = ix - 1;
                            break;
                        case PRGB: case PCIE: case PHSV: case PHLS:
                            ind_tbl[12] = fx;
                            fx += work1 * 3;
                            ind_tbl[13] = fx - 1;
                        }
                        ind_tbl[14] = fx;
                        fx += work1 * 3;
                        ind_tbl[15] = fx - 1;
                        break;
                    }
                    /* indices error check */
                    for ( i = 0; i < 16; i++) {
                        if ( ind_tbl[i] != rec.data.ia[i]) {
                            err = ERR2003;
                        }
                    }
                    if ( err) {
                        break;
                    }

                    /* number of float entry error check */
                    float_entry = MAX(float_entry, ind_tbl[9]);
                    float_entry = MAX(float_entry, ind_tbl[11]);
                    float_entry = MAX(float_entry, ind_tbl[13]);
                    float_entry = MAX(float_entry, ind_tbl[15]);
                    if ( float_entry != rec.data.rl) {
                        err = ERR2003;
                    } 
                    if ( err) {
                        break;
                    }

                    /* convert GDP data */    
                    PMALLOC( err, Pint, work3, ints);
                    if ( err) {
                        break;
                    }
                    gdp_data.gdp3_fillareaset.fillarealist.ints = ints;
                    w_ia = 0;
                    ptr_i = &(rec.data.ia[21]);
                    for ( i = 0; i < work3; i++) {
                        if ( ptr_i[i] < (w_ia + 3)) {
                            err = ERR2005;
                            break;
                        } else {
                            ints[i] = ptr_i[i] - w_ia;
                            w_ia = ptr_i[i];
                        }
                    }
                    if ( err) {
                        break;
                    }
                    if ( rec.data.ia[20 + work3] > *n) {
                        err = ERR2005;
                        break; 
                    }

                    coli = rec.data.ia[0] ?
					&(rec.data.ia[rec.data.ia[0] - 1]): NULL;
                    colr = rec.data.ia[8] ?
					&(rec.data.ra[rec.data.ia[8] - 1]): NULL;
                    norm = rec.data.ia[10] ?
					&(rec.data.ra[rec.data.ia[10] - 1]): NULL;
                    GDP_FDATA_TRAN( gdp_data.gdp3_fillareaset.fflag,
                        work3, gdp_data.gdp3_fillareaset.color_model,
                        coli, colr, norm, err, gdp_data.gdp3_fillareaset.fdata);
                    if ( err) {
                        break;
                    }
                    if ( gdp_data.gdp3_fillareaset.fflag < PNOF || 
                        gdp_data.gdp3_fillareaset.fflag > PFCONO) {
                        gdp_data.gdp3_fillareaset.fflag = PNOF;
                    }  

                    gdp_data.gdp3_fillareaset.edata = NULL;
                    switch ( gdp_data.gdp3_fillareaset.eflag) {
                    case PNOE:
                        break;
                    case PEVIS:
                        ptr_i = &(rec.data.ia[rec.data.ia[4] - 1]);
                        for ( i = 0; i < work2; i++) {
                            if ( ptr_i[i] < POFF || ptr_i[i] > PON) {
                                err = ERR2000;
                                break;
                            }
                        }
                        if ( err) {
                            break;
                        }
                        PMALLOC( err, Pedge_data_list, work3,
                            gdp_data.gdp3_fillareaset.edata);
                        if ( !err) {
                            w_ia = 0;
                            ptr_i = &(rec.data.ia[rec.data.ia[2] - 1]);
                            for ( i = 0; i < work3; i++) {
                                if ( ptr_i[i] < (w_ia + 3)) {
                                    err = ERR2005;
                                    break;
                                } else {
                                    gdp_data.gdp3_fillareaset.edata[i].num_edges
                                        = ptr_i[i] - w_ia;
                                    gdp_data.gdp3_fillareaset.edata[i].edgedata.
                                        edges = (Pedge_flag*)&(rec.data.ia[
											rec.data.ia[4] - 1 + w_ia]);
                                    w_ia = ptr_i[i];
                                }
                            }
                        }
                        break;
                    default:
                        gdp_data.gdp3_fillareaset.eflag = PNOE;
                    }
                    if ( err) {
                        break;
                    }

                    coli = rec.data.ia[6] ?
                        &(rec.data.ia[rec.data.ia[6] - 1]): NULL;
                    colr = rec.data.ia[12] ?
                        &(rec.data.ra[rec.data.ia[12] - 1]): NULL;
                    norm = rec.data.ia[14] ?
                        &(rec.data.ra[rec.data.ia[14] - 1]): NULL;
                    PMALLOC( err, Pgdp_facet_vdata_arr3, work3,
                        gdp_data.gdp3_fillareaset.vdata);
                    if ( err) {
                        break;
                    }
                    GDP_VDATA_TRAN( gdp_data.gdp3_fillareaset.vflag, work1,
                        gdp_data.gdp3_fillareaset.color_model, coli, colr, norm,
                        err, vdata);
                    if ( err) {
                        break;
                    }
                    w_ia = 0;
                    switch ( gdp_data.gdp3_fillareaset.vflag) {
                    case PCOORD:
                        break;
                    case PCCOLR:
                        for ( i = 0; i < work3; i++) {
                            gdp_data.gdp3_fillareaset.vdata[i].colrs =
                                &(vdata.colrs[w_ia]);
                            w_ia = rec.data.ia[21 + i];
                        }
                        break;
                    case PCNORM:
                        for ( i = 0; i < work3; i++) {
                            gdp_data.gdp3_fillareaset.vdata[i].normals =
                                &(vdata.normals[w_ia]);
                            w_ia = rec.data.ia[21 + i];
                        }
                        break;
                    case PCCONO:
                        for ( i = 0; i < work3; i++) {
                            gdp_data.gdp3_fillareaset.vdata[i].conorms =
                                &(vdata.conorms[w_ia]);
                            w_ia = rec.data.ia[21 + i];
                        }
                        break;
                    default:
                        gdp_data.gdp3_fillareaset.vflag = PCOORD;
                    }

                    if ( gdp_data.gdp3_fillareaset.fflag != PFNORM) {
                        err = ERRN812;

                    } else if ( gdp_data.gdp3_fillareaset.eflag != PEVIS){
                        err = ERRN812;

                    } else if ( gdp_data.gdp3_fillareaset.vflag != PCOORD) {
                        err = ERRN812;

                    } else if ( gdp_data.gdp3_fillareaset.color_model != PRGB) {
                        err = ERRN813;

                    }
                    break;

                }
            }

            if ( err) {
                ERR_REPORT( phg_cur_cph->erh, err);

            } else {
                PMALLOC( err, Ppoint3, *n, points);
                if ( err) {
                    ERR_REPORT( phg_cur_cph->erh, err);
                } else {
                    POINT3_DATA_TRANSLATE( *n, pxa, pya, pza, points);
                    args->el_type = PELEM_GDP3;
                    ed.gdp3.id = *primid;
                    ed.gdp3.pts.num_points = *n;
                    ed.gdp3.pts.points = points;
                    ed.gdp3.rec = gdp_data;
                    if ( CB_BUILD_OC( args->el_type, &ed, &args->pex_oc ) )
                        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args,
                            NULL);
                    PFREE( *n, points);
                }
            }
            switch ( *primid) {
            case PGDP3_POLYLINE_SET:
                PFREE( rec.data.ia[0], ints);
                break;
            case PGDP3_FILLAREA_SET_HOLLOW:
                PFREE( rec.data.ia[0], ints);
                if ( gdp_data.gdp3_fillareaset.eflag == PEVIS) {
                    PFREE( work3, gdp_data.gdp3_fillareaset.edata);
                }
                switch ( gdp_data.gdp3_fillareaset.fflag) { 
                case PNOF:
                    break;
                case PFCOLR:
                    PFREE( work3, gdp_data.gdp3_fillareaset.fdata.colrs);
                    break;
                case PFNORM:
                    PFREE( work3, gdp_data.gdp3_fillareaset.fdata.norms);
                    break;
                case PFCONO:
                    PFREE( work3, gdp_data.gdp3_fillareaset.fdata.conorms);
                }
                switch ( gdp_data.gdp3_fillareaset.vflag) { 
                case PCOORD:
                    break;
                case PCCOLR:
                    PFREE( work1, vdata.colrs);
                    break;
                case PCNORM:
                    PFREE( work1, vdata.normals);
                    break;
                case PCCONO:
                    PFREE( work1, vdata.conorms);
                }
                PFREE( work3, gdp_data.gdp3_fillareaset.vdata);
            }
            fb_del_datrec( &rec);  
        }
    }
#endif
}


/* GENERALIZED DRAWING PRIMITIVE */
void
pgdp_( n, pxa,pya, primid, ldr, datrec)
Pint	*n;		/* number of points	*/
Pfloat	*pxa;		/* coordinates of points X	*/
Pfloat	*pya;		/* coordinates of points Y	*/
Pint	*primid;	/* GDP identifier	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record 	*/
{
#ifdef EXT_GDP |
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Ppoint		*points;
    Pgdp_data		gdp_data;
    Pdata_rec		rec;
    Pint		w_ia = 0;
    Pint		*ints = NULL;
    int                 i;
    Pint		err = 0;
    Wst_phigs_dt	*dt;
    Pint		ind;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_gdp)){
    dt = &phg_cur_cph -> wst_list -> wst -> desc_tbl.phigs_dt ;
        if ( PSL_STRUCT_STATE(phg_cur_cph->psl) != PSTOP){
            ERR_REPORT( phg_cur_cph->erh, ERR5);
 
        } else if ( *n < 0){
            ERR_REPORT( phg_cur_cph->erh, ERR2004);

        } else if ( (ind = find_id( *primid,
                                    dt->out_dt.num_gdp,
                                    dt->out_dt.gdp_ids))< 0){
            ERR_REPORT( phg_cur_cph->erh, ERRN810);

        } else {
            if ( !fb_unpack_datrec( *ldr, datrec, &err, &rec)) {
                switch ( *primid) {
                case PGDP_POLYLINE_SET:
                    if ( rec.data.ia[0] < 1){
                        err = ERR2005;
                    } else if ( rec.data.ia[rec.data.ia[0]] > *n) {
                        err = ERR2005;
                    } else {
                        gdp_data.gdp_polylineset.polylinelist.num_ints =
                            rec.data.ia[0];
                        PMALLOC( err, Pint, rec.data.ia[0], ints);
                        if ( !err) {
                            gdp_data.gdp_polylineset.polylinelist.ints = ints;
                            w_ia = 0;
                            for ( i = 1; i <= rec.data.ia[0]; i++) {
                                if ( rec.data.ia[i] < w_ia) {
                                    err = ERR2005;
                                    break;
                                } else {
                                    ints[i - 1] = rec.data.ia[i] - w_ia;
                                    w_ia = rec.data.ia[i];
                                }
                            }
                        }
                    }
                    break;

                case PGDP_CIRCLE:
                    if ( *n != 1) {
                        err = ERRN811;
                    } else if ( rec.data.rl != 1) {
                        err = ERR2003;
                    } else {
                        gdp_data.gdp_circle.radius = rec.data.ra[0];
                    }
                    break;
  
                case PGDP_ELLIPSE:
                    if ( *n != 3) {
                        err = ERRN811;
                    }
                    /* datrec is unused */
                    break;
  
                case PGDP_CIRCLE_ARC_EP:
                    if ( *n != 5) {
                        err = ERRN811;
                    } else if ( rec.data.rl != 1) {
                        err = ERR2003;
                    } else {
                        gdp_data.gdp_circlearc_ep.radius = rec.data.ra[0];
                    }
                    break;
  
                case PGDP_CIRCLE_ARC_CLOSE_EP:
                    if ( *n != 5) {
                        err = ERRN811;
                    } else if ( rec.data.il != 1 || rec.data.rl != 1) {
                        err = ERR2003;
                    } else if ( rec.data.ia[0] < PACFAN ||
                        rec.data.ia[0] > PACCHD ){
                        err = ERR2000;
                    } else {
                        gdp_data.gdp_circlearcclose_ep.radius = rec.data.ra[0];
                        gdp_data.gdp_circlearcclose_ep.closetype
                            = (Parc_close_type)rec.data.ia[0];
                    }
                    break;
  
                case PGDP_ELLIPSE_ARC_EP:
                    if ( *n != 7) {
                        err = ERRN811;
                    }
                    /* datrec is unused */
                    break;
  
                case PGDP_ELLIPSE_ARC_CLOSE_EP:
                    if ( *n != 7) {
                        err = ERRN811;
                    } else if ( rec.data.il != 1) {
                        err = ERR2003;
                    } else if ( rec.data.ia[0] < PACFAN ||
                        rec.data.ia[0] > PACCHD ){
                        err = ERR2000;
                    } else {
                        gdp_data.gdp_ellipsearcclose_ep.closetype
                            = (Parc_close_type)rec.data.ia[0];
                    }
                    break;
  
                case PGDP_DISJOINT_POLYLINE:
                    /* datrec is unused */
                    break;

                }
            }

            if ( err) {
	        ERR_REPORT( phg_cur_cph->erh, err);

            } else {
                PMALLOC( err, Ppoint, *n, points);
                if ( err) {
	            ERR_REPORT( phg_cur_cph->erh, err);
                } else {
                    POINT_DATA_TRANSLATE( *n, pxa, pya, points);
                    args->el_type = PELEM_GDP;
                    ed.gdp.id = *primid;
                    ed.gdp.pts.num_points = *n;
                    ed.gdp.pts.points = points;
                    ed.gdp.rec = gdp_data;

                    if ( CB_BUILD_OC( args->el_type, &ed, &args->pex_oc ) )
                        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args,
                            NULL);
                    PFREE( *n, points);
                }
            }
            if ( *primid == PGDP_POLYLINE_SET) {
                PFREE( rec.data.ia[0], ints);
            }
            fb_del_datrec( &rec);
        }
    }
#endif
}


/* GENERALIZED STRUCTURE ELEMENT */
void
pgse_( gseid, ldr, datrec)
Pint	*gseid;		/* GSE identifier	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record	*/
{
#ifdef EXT_GSE | EXT_INPUT
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Pgse_data		gse;
    Pdata_rec		rec;
    Pint		err = 0;
    Wst_phigs_dt	*dt;
    Pint		ind;


    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_gse)) {
    dt = &phg_cur_cph -> wst_list -> wst -> desc_tbl.phigs_dt ;
    if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
      ERR_REPORT( phg_cur_cph->erh, ERR5);

    } else if ( (ind = find_id( *gseid,
                                dt->out_dt.num_gse,
                                dt->out_dt.gse_ids))< 0){
        ERR_REPORT( phg_cur_cph->erh, ERRN814);

    } else {
            if ( !fb_unpack_datrec( *ldr, datrec, &err, &rec)) {
                switch ( *gseid) {
#ifdef EXT_GSE
                case PGSE_POLYLINE_SP_ATTR:
                    if ( rec.data.il != 1) {
                        err = ERR2003;
                    } else if ( rec.data.ia[0] < PSPCIR ||
                        rec.data.ia[0] > PSPFLA) {
                        err = ERR2000;
                    } else {
                        gse.gse_polyline_sp.sidepoint = rec.data.ia[0];
                    }
                    break;

                case PGSE_EDGE_SP_ATTR:
                    if ( rec.data.il != 1) {
                        err = ERR2003;
                    } else if ( rec.data.ia[0] < PSPCIR ||
                        rec.data.ia[0] > PSPFLA) {
                        err = ERR2000;
                    } else {
                        gse.gse_edge_sp.sidepoint = rec.data.ia[0];
                    }
                    break;
#endif

#ifdef EXT_INPUT
                case PGSE_LOCAL_INPUT_INDEX:
                    if ( rec.data.il != 2) {
                        err = ERR2003;
                    } else if ( rec.data.ia[0] < 0) {
                        err = ERRN815;
                    } else if ( rec.data.ia[1] < PCPRE ||
                        rec.data.ia[1] > PCREPL) {
                        err = ERR2000;
                    } else {
                        gse.gse_localinput_index.index = rec.data.ia[0];
                        gse.gse_localinput_index.type =
                            (Pcompose_type)rec.data.ia[1];
                    }
                    break;
#endif

                }
            } 
            if ( err) {
	        ERR_REPORT( phg_cur_cph->erh, err);

            } else {
	        args->el_type = PELEM_GSE;
	        ed.gse.id = *gseid;
	        ed.gse.rec = gse;
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	            CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
            }
            fb_del_datrec( &rec);
	}
    }
#endif
}


/* SET PARAMETRIC SURFACE CHARCTERISTICS */
void
pspsch_( pscty, ldr, datrec)
Pint	*pscty;		/* parametric surface characteristics type	*/
Pint	*ldr;		/* length of data record	*/
char	*datrec;	/* data record	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Ppara_surf_characs	data;
    Pdata_rec	r;
    Pint		err = 0;
    register int	i,j;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_para_surf_characs)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else if ( fb_unpack_datrec( ldr, datrec, &err, &r)) {
            ERR_REPORT( phg_cur_cph->erh, err);

        } else { 
            switch ( *pscty) {
            case PSC_NONE:
            case PSC_WS_DEP:
                break;
            case PSC_ISOPARAMETRIC_CURVES:
                if ( r.data.il != 3) {
                    err = ERR2003;
                } else {
                    data.psc_3.placement = (Pcurve_placement)r.data.ia[0];
                    data.psc_3.u_count = r.data.ia[1];
                    data.psc_3.v_count = r.data.ia[2];
                }
                break;
            case PSC_LEVEL_CURVES_MC:
                if ( r.data.il != 1 || r.data.rl < 6) {
                    err = ERR2003;
                } else {
                    data.psc_4.params.num_floats = r.data.ia[0];
                    data.psc_4.origin.x = r.data.ra[0];
                    data.psc_4.origin.y = r.data.ra[1];
                    data.psc_4.origin.z = r.data.ra[2];
                    data.psc_4.direction.delta_x = r.data.ra[3];
                    data.psc_4.direction.delta_y = r.data.ra[4];
                    data.psc_4.direction.delta_z = r.data.ra[5];
                    data.psc_4.params.floats = &(r.data.ra[6]);
                }
                break;
            case PSC_LEVEL_CURVES_WC:
                if ( r.data.il != 1 || r.data.rl < 6) {
                    err = ERR2003;
                } else {
                    data.psc_5.params.num_floats = r.data.ia[0];
                    data.psc_5.origin.x = r.data.ra[0];
                    data.psc_5.origin.y = r.data.ra[1];
                    data.psc_5.origin.z = r.data.ra[2];
                    data.psc_5.direction.delta_x = r.data.ra[3];
                    data.psc_5.direction.delta_y = r.data.ra[4];
                    data.psc_5.direction.delta_z = r.data.ra[5];
                    data.psc_5.params.floats = &(r.data.ra[6]);
                }
                break;
            }
            if ( err) {
                ERR_REPORT( phg_cur_cph->erh, err);

            } else {
                args->el_type = PELEM_PARA_SURF_CHARACS;
                ed.psc.type = *pscty;
                ed.psc.data = data;
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                    CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
            }
            fb_del_datrec( &r);
        }
    }
}


/* INQUIRE STRUCTURE STATE VALUE */
void
pqstrs_( strsta)
Pint	*strsta;	/* OUT structure state value	*/
{
    if ( CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *strsta = PSL_STRUCT_STATE( phg_cur_cph->psl);
    } else {
        *strsta = PSTCL;
    }
}


/* INQUIRE OPEN STRUCTURE */
void
pqopst_( errind, stype, strid)
Pint	*errind;	/* OUT error indicator	*/
Pint	*stype;		/* OUT open structure status	*/
Pint	*strid;		/* OUT structure identifier	*/
{
    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else {
	*errind = 0;
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    *stype = PNONST;
	} else {
            *stype = POPNST;
            *strid = phg_cur_cph->psl->open_struct;
	}
    }
}


/* INQUIRE STRUCTURE STATUS */
void
pqstst_( strid, errind, strsti)
Pint	*strid;		/* structure identifier	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*strsti;	/* OUT strucure status indicator	*/
{
    Phg_args				cp_args;
    Phg_ret				ret;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else {
	cp_args.data.idata = *strid;
	ret.err = 0;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_STRUCT_STATUS, &cp_args, &ret);
	if ( ret.err) {
	    *errind = ret.err;
	} else {
	    *errind = 0;
	    *strsti = ret.data.idata;
	}
    }
}

static void
inq_content( cp_args, store, ret )
    Phg_args            *cp_args;
    _Pstore             *store;
    Phg_ret             *ret;
{
    int                 size;
    pexElementInfo      *oc;

    ret->err = 0;
    CP_FUNC(phg_cur_cph, CP_FUNC_OP_INQ_EL_CONTENT, cp_args, ret);
    if ( !ret->err ) {
        if ( ret->data.el_info.op != PELEM_NIL ) {
            oc = ret->data.el_info.pex_oc.oc;
            size = phg_utx_compute_el_size( oc, oc );
            if ( CB_STORE_SPACE( store, size, &ret->err ) )
                phg_utx_el_data_from_pex( oc, store->buf,
                    &store->data.elem_data );
        }
    }
}

static void
fb_elem_type_size( cp_args, store, ret, errind, eltype, il, rl, sl)
Phg_args	*cp_args;
Pstore		store;
Phg_ret		*ret;
Pint		*errind;	/* OUT error indicator	*/
Pint		*eltype;	/* OUT element type	*/
Pint		*il;		/* OUT dimension of integer array	*/
Pint		*rl;		/* OUT dimension of real array	*/
Pint		*sl;		/* OUT dimension of character array	*/
{
    Pelem_data		*data;
    Pint		work1 = 0;
    Pint		work2 = 0;
    Pint		work3 = 0;
    Pint		work4 = 0;
    register int	i,j;

    ret->err = 0;
    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_EL_TYPE_SIZE, cp_args, ret);
    if ( ret->err) {
        *errind = ret->err;
    } else {
        *eltype = ret->data.el_type_size.type;
        ret->err = 0;
        inq_content( cp_args, ((_Pstore *)store), ret);
        if ( ret->err) {
            *errind = ret->err;
        } else {
            *errind = 0;
            data = &((_Pstore *)store)->data.elem_data;
            *il = 0;
            *rl = 0;
            *sl = 0;
            switch ( *eltype) {
            case PENIL:
            case PERMCV:
            case PEALL:
                break;
                break;
            case PEPL3:
            case PEPM3:
            case PEFA3:
                *il = 1;
                *rl = 3 * data->point_list3.num_points;
                break;
            case PEPL:
            case PEPM:
            case PEFA:
                *il = 1;
                *rl = 2 * data->point_list.num_points;
                break;
            case PETX3:
                *rl = 9;
                *sl = 1;
                break;
            case PETX:
                *rl = 2;
                *sl = 1;
                break;
            case PEATR3:
                *rl = 6;
                *sl = 1;
                break;
            case PEATR:
                *rl = 4;
                *sl = 1;
                break;
            case PEFAS3:
                *il = data->point_list_list3.num_point_lists;
                for ( i = 0; i < *il; i++) {
                    *rl += data->point_list_list3.point_lists[i].num_points;
                }
                *rl = *rl * 3;
                break;
            case PEFAS:
                *il = data->point_list_list.num_point_lists;
                for ( i = 0; i < *il; i++) {
                    *rl += data->point_list_list.point_lists[i].num_points;
                }
                *rl = *rl * 2;
                break;
            case PECA3:
                *il = 2 + (data->cell_array3.colr_array.dims.size_x
                        * data->cell_array3.colr_array.dims.size_y);
                *rl = 9;
                break;
            case PECA:
                *il = 2 + (data->cell_array3.colr_array.dims.size_x
                        * data->cell_array3.colr_array.dims.size_y);
                *rl = 4;
                break;
            case PEGDP3:
#ifdef EXT_GDP
                *il = 2;
                *rl = data->gdp3.point_list.num_points * 3;
                switch ( data->gdp3.id) {
                case PGDP3_POLYLINE_SET:
                    work1 = sizeof(Pint) * ( data->gdp3.data.gdp3_polylineset.
                                             polylinelist.num_ints + 5);
                    *sl = (work1 - 1) / 80 + 1;
                case PGDP3_ELLIPSE:
                case PGDP3_ELLIPSE_ARC_EP:
                case PGDP3_ELLIPSE_ARC_CLOSE_EP:
                case PGDP3_DISJOINT_POLYLINE:
                    *sl = 1;
                    break;
                case PGDP3_FILLAREA_SET_HOLLOW:
                    work2 = 0;
                    work4 = 0;
                    work3 = data->gdp3.data.gdp3_fillareaset.fillarealist.
                        num_ints;
                    work1 = 21 + work3;
                    for ( i = 0; i < work3; i++) {
                        work4 += data->gdp3.data.gdp3_fillareaset.fillarealist.
                            ints[i];
                    }
                    if ( data->gdp3.data.gdp3_fillareaset.eflag ==
                        PEVIS) {
                        work1 += work3;
                        for ( i = 0; i < work3; i++) {
                            work1 += data->gdp3.data.gdp3_fillareaset.edata[i].
                                num_edges;
                        }
                    }
                    switch ( data->gdp3.data.gdp3_fillareaset.fflag) {
                    case PFCOLR:
                        switch ( data->gdp3.data.gdp3_fillareaset.color_model) {
                        case PINDCT:
                            work1 += work3;
                            break;
                        case PRGB: case PCIE: case PHLS: case PHSV:
                            work2 += work3 * 3;
                        }
                        break;
                    case PFNORM:
                        work2 += work3 * 3;
                        break;
                    case PFCONO:
                        work2 += work3 * 3;
                        switch ( data->gdp3.data.gdp3_fillareaset.color_model) {
                        case PINDCT:
                            work1 += work3;
                            break;
                        case PRGB: case PCIE: case PHLS: case PHSV:
                            work2 += work3 * 3;
                        }
                    }
                    switch ( data->gdp3.data.gdp3_fillareaset.vflag) {
                    case PCCOLR:
                        switch ( data->gdp3.data.gdp3_fillareaset.color_model) {
                        case PINDCT:
                            work1 += work4;
                            break;
                        case PRGB: case PCIE: case PHLS: case PHSV:
                            work2 += work4 * 3;
                        }
                        break;
                    case PCNORM:
                        work2 += work4 * 3;
                        break;
                    case PCCONO:
                        switch ( data->gdp3.data.gdp3_fillareaset.color_model) {
                        case PINDCT:
                            work1 += work4;
                            break;
                        case PRGB: case PCIE: case PHLS: case PHSV:
                            work2 += work4 * 3;
                        }
                    }
                    work1 = (sizeof(Pint) * (work1 + 4)) +
                            (sizeof(Pfloat) * work2);
                    *sl = (work1 - 1) / 80 + 1;
                    break;
                default:
                    *sl = 1;
                }
#endif
                break;
            case PEGDP:
#ifdef EXT_GDP
                *il = 2;
                *rl = data->gdp.point_list.num_points * 2;
                switch ( data->gdp.id) {
                case PGDP_POLYLINE_SET:
                    work1 = sizeof(Pint) * ( data->gdp.data.gdp_polylineset.
                                             polylinelist.num_ints + 5);
                    *sl = (work1 - 1) / 80 + 1;
                    break;
                case PGDP_CIRCLE:
                case PGDP_ELLIPSE:
                case PGDP_CIRCLE_ARC_EP:
                case PGDP_CIRCLE_ARC_CLOSE_EP:
                case PGDP_ELLIPSE_ARC_EP:
                case PGDP_ELLIPSE_ARC_CLOSE_EP:
                case PGDP_DISJOINT_POLYLINE:
                default:
                    *sl = 1;
                }
#endif
                break;
            case PEPLI:
            case PEPMI:
            case PETXI:
            case PEII:
            case PEEDI:
            case PELN:
            case PEPLCI:
            case PEMK:
            case PEPMCI:
            case PETXFN:
            case PETXPR:
            case PETXCI:
            case PETXP:
            case PEATP:
            case PEANST:
            case PEIS:
            case PEISI:
            case PEICI:
            case PEEDFG:
            case PEEDT:
            case PEEDCI:
            case PEHRID:
            case PEMCLI:
            case PEVWI:
            case PEEXST:
            case PELB:
            case PEPKID:
                *il = 1;
                break;
            case PELWSC:
            case PEMKSC:
            case PECHXP:
            case PECHSP:
            case PECHH:
            case PEATCH:
            case PEEWSC:
                *rl = 1;
                break;
            case PECHUP:
            case PEATCU: 
            case PEPA:
            case PEPARF:
                *rl = 2;
                break;
            case PETXAL:
            case PEATAL:
            case PEIASF:
                *il = 2;
                break;
            case PEPRPV:
                *rl = 9;
                break;
            case PEADS:
            case PERES:
                *il = data->names.num_ints;
                break;
            case PELMT3:
                *il = 1;
                *rl = 16;
                break;
            case PELMT:
                *il = 1;
                *rl = 9;
                break;
            case PEGMT3:
                *rl = 16;
                break;
            case PEGMT:
                *rl = 9;
                break;
            case PEMCV3:
                *il = 2;
                *rl = 6 * data->model_clip3.half_spaces.num_half_spaces;
                break;
            case PEMCV:
                *il = 2;
                *rl = 4 * data->model_clip.half_spaces.num_half_spaces;
                break;
            case PEAP:
                *sl = data->appl_data.size / 80;
                break;
            case PEGSE:
#ifdef EXT_GSE | EXT_INPUT
                *il = 1;
                switch ( data->gse.id) {
#ifdef EXT_GSE
                case PGSE_POLYLINE_SP_ATTR:
                case PGSE_EDGE_SP_ATTR:
#endif
#ifdef EXT_INPUT
                case PGSE_LOCAL_INPUT_INDEX:
#endif
                default:
                    *sl = 1;
                }
#endif
                break;
            
            /* PHIGS+ functions */
            case PEPLS3:
                *il = 7 + data->plsd3.npl;
                for ( i = 0; i < data->plsd3.npl; i++)
                    work1 += data->plsd3.vdata[i].num_vertices;
                *rl = work1 * 3;
                switch ( data->plsd3.vflag) {
                case PCCOLR:
                    switch( data->plsd3.colr_model) {
                    case PINDCT:
                        *il += work1;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += work1 * 3;
                        break;
                    }
                    break;
                }
                break;
            case PEFSD3:        
                *il = 21 + data->fasd3.nfa;
                for ( i = 0; i < data->fasd3.nfa; i++)
                    work1 += data->fasd3.vdata[i].num_vertices;
                *rl = work1 * 3;
                switch ( data->fasd3.fflag) {
                case PFCOLR:
                    switch( data->fasd3.colr_model) {
                    case PINDCT:
                        *il += 1;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += 3;
                        break;
                    }
                    break;
                case PFNORM:
                    *rl += 3;
                    break;
                case PFCONO:
                    switch( data->fasd3.colr_model) {
                    case PINDCT:
                        *il += 1;
                        *rl += 3;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += 6;
                        break;
                    }
                    break;
                }
                switch ( data->fasd3.eflag) {
                case PEVIS:
                    *il += data->fasd3.nfa;
                    for ( i = 0; i < data->fasd3.nfa; i++)
                        work2 += data->fasd3.edata[i].num_edges;
                    *il += work2;
                    break;
                }
                switch ( data->fasd3.vflag) {
                case PCOORD:
                    break;
                case PCCOLR:
                    switch( data->fasd3.colr_model) {
                    case PINDCT:
                        *il += work1;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += work1 * 3;
                        break;
                    }
                    break;
                case PCNORM:
                    *rl += work1 * 3;
                    break;
                case PCCONO:
                    *rl += work1 * 3;
                    switch( data->fasd3.colr_model) {
                    case PINDCT:
                        *il += work1;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += work1 * 3;
                        break;
                    }
                    break;
                }
                break;
            case PETRS3:
                *il = 16;
                *rl = data->tsd3.nv * 3;
                switch ( data->tsd3.fflag) {
                case PFCOLR:
                    switch( data->tsd3.colr_model) {
                    case PINDCT:
                        *il += data->tsd3.nv - 2;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += (data->tsd3.nv - 2) * 3;
                        break;
                    }
                    break;
                case PFNORM:
                    *rl += (data->tsd3.nv - 2) * 3;
                    break;
                case PFCONO:
                    *rl += (data->tsd3.nv - 2) * 3;
                    switch( data->tsd3.colr_model) {
                    case PINDCT:
                        *il += data->tsd3.nv - 2;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += (data->tsd3.nv - 2) * 3;
                        break;
                    }
                    break;
                }
                switch ( data->tsd3.vflag) {
                case PCCOLR:
                    switch( data->tsd3.colr_model) {
                    case PINDCT:
                        *il += data->tsd3.nv;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += data->tsd3.nv * 3;
                        break;
                    }
                    break;
                case PCNORM:
                    *rl += data->tsd3.nv * 3;
                    break;
                case PCCONO:
                    *rl += data->tsd3.nv * 3;
                    switch( data->tsd3.colr_model) {
                    case PINDCT:
                        *il += data->tsd3.nv;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += data->tsd3.nv * 3;
                        break;
                    }
                    break;
                }
                break;
            case PEQMD3:
                *il = 17;
                work1 = (data->qmd3.dim.size_x - 1)*(data->qmd3.dim.size_y - 1);
                work2 = data->qmd3.dim.size_x * data->qmd3.dim.size_y;
                *rl = work2;
                switch ( data->qmd3.fflag) {
                case PFCOLR:
                    switch( data->qmd3.colr_model) {
                    case PINDCT:
                        *il += work1;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += work1 * 3;
                        break;
                    }
                    break;
                case PFNORM:
                    *rl += work1 * 3;
                    break;
                case PFCONO:
                    *rl += work1 * 3;
                    switch( data->qmd3.colr_model) {
                    case PINDCT:
                        *il += work1;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += work1 * 3;
                        break;
                    }
                    break;
                }
                switch ( data->qmd3.vflag) {
                case PCCOLR:
                    switch( data->qmd3.colr_model) {
                    case PINDCT:
                        *il += work2;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += work2 * 3;
                        break;
                    }
                    break;
                case PCNORM:
                    *rl += work2 * 3;
                    break;
                case PCCONO:
                    *rl += work2 * 3;
                    switch( data->qmd3.colr_model) {
                    case PINDCT:
                        *il += work2;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += work2 * 3;
                        break;
                    }
                    break;
                }
                break;
            case PESFS3:
                work1 = data->sofas3.num_sets;
                work2 = data->sofas3.vdata.num_vertices;
                *il = 26 + data->sofas3.num_sets;
                *rl = work2;
                for ( i = 0; i < data->sofas3.num_sets; i++) {
                    *il += data->sofas3.vlist[i].num_lists;
                    for ( j = 0; j < data->sofas3.vlist[i].num_lists; j++) {
                        *il += data->sofas3.vlist[i].lists[j].num_ints;
                    }
                }
                if ( data->sofas3.eflag == PNOE) {
                    for ( i = 0; i < data->sofas3.num_sets; i++) {
                        *il += data->sofas3.edata[i].num_lists;
                        for ( j = 0; j < data->sofas3.edata[i].num_lists; j++) {
                            *il += data->sofas3.edata[i].edgelist[j].num_edges;
                        }
                    }
                }
                switch ( data->sofas3.fflag) {
                case PFCOLR:
                    switch( data->sofas3.colr_model) {
                    case PINDCT:
                        *il += work1;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += work1 * 3;
                        break;
                    }
                    break;
                case PFNORM:
                    *rl += work1 * 3;
                    break;
                case PFCONO:
                    *rl += work1 * 3;
                    switch( data->sofas3.colr_model) {
                    case PINDCT:
                        *il += work1;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += work1 * 3;
                        break;
                    }
                    break;
                }
                switch ( data->sofas3.vflag) {
                case PCCOLR:
                    switch( data->sofas3.colr_model) {
                    case PINDCT:
                        *il += work2;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += work2 * 3;
                        break;
                    }
                    break;
                case PCNORM:
                    *rl += work2 * 3;
                    break;
                case PCCONO:
                    *rl += work2 * 3;
                    switch( data->sofas3.colr_model) {
                    case PINDCT:
                        *il += work2;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        *rl += work2 * 3;
                        break;
                    }
                    break;
                }
                break;
            case PENBSC:
                *il = 4;
                *rl = 2 + data->nurb_curve.knots.num_floats +
                    (data->nurb_curve.cpts.num_points * 4);
                break;
            case PENBSS:
                *il = 44;
                for ( i = 0; i < data->nurb_surf.num_trim_loops; i++)
                    work1 += data->nurb_surf.trim_loops[i].num_curves;
                *il = work1 * 7;
                /* 7 -> 1. visibule     2. rationality   3. order
                 *      4. approx_type  5. approx_val    6. knots.num_floats
                 *      7. cpts.num_points
                 */
                *rl = data->nurb_surf.uknots.num_floats +
                      data->nurb_surf.vknots.num_floats;
                work2 = data->nurb_surf.rationality ? PNONRA: PRATIO;
                if ( work2 == PRATIO) {
                    *rl += data->nurb_surf.grid.num_points.u_dim *
                           data->nurb_surf.grid.num_points.v_dim * 3;
                } else {
                    *rl += data->nurb_surf.grid.num_points.u_dim *
                           data->nurb_surf.grid.num_points.v_dim * 4;
                }
                *rl += work1 * 2;
                /* 2 -> 1. tmin        2. tmax
                 */
                for ( i = 0; i < data->nurb_surf.num_trim_loops; i++) {
                    for ( j = 0;
                        j < data->nurb_surf.trim_loops[i].num_curves; j++) {
                        *rl += data->nurb_surf.trim_loops[i].curves[j].
                            knots.num_floats;
                        work2 = data->
                            nurb_surf.trim_loops[i].curves[j].rationality ?
                                 PNONRA: PRATIO;
                        if ( work2 == PRATIO) {
                            *rl += data->nurb_surf.trim_loops[i].curves[j].
                                cpts.num_points * 2;
                        } else {
                            *rl += data->nurb_surf.trim_loops[i].curves[j].
                                cpts.num_points * 3;
                        }
                    }
                }
                break;
            case PECAP3:
                *il = 3;
                *rl = 9;
                work1 = data->cell_array_plus.colr_array.dims.size_x *
                        data->cell_array_plus.colr_array.dims.size_y;
                switch(data->cell_array_plus.colr_array.type) {
                case PINDCT:
                    *il += work1;
                    break;
                case PRGB: case PCIE: case PHSV: case PHLS:    
                    *rl += work1 * 3;
                    break;
                }
                break;
            case PETXCL:
            case PEPMCL:
            case PEEDCL:
            case PEPLCL:
            case PEICL:
            case PEBICL:
                switch(data->colr.type) {
                case PINDCT:
                    *il = 2;
                    break;
                case PRGB: case PCIE: case PHSV: case PHLS:    
                    *il = 1;
                    *rl = 3;
                    break;
                }
                break;
            case PECAPC:
                *il = 1;
                *rl = 1;
                break;
            case PEPLSM:
            case PEBISY:
            case PEBISI:
            case PEISM:
            case PEBISM:
            case PEIRFE:
            case PEBIRE:
            case PEFDGM:
            case PEFCUM:
            case PEDPCI:
            case PECMI:
            case PERCLM:
                *il = 1;
                break;
            case PERFP:
            case PEBRFP:
                switch(data->props.specular_colr.type) {
                case PINDCT:
                    *il = 2;
                    *rl = 5;
                    break;
                case PRGB: case PCIE: case PHSV: case PHLS:    
                    *il = 1;
                    *rl = 8;
                    break;
                }
                break;
            case PESAPC:
                *il = 1;
                *rl = 2;
                break;
            case PEPSCH:
                *il = 1;
                switch ( data->para_surf_characs.type) {
                case PSC_NONE:
                case PSC_WS_DEP:
                    *sl = 1;
                    break;
                case PSC_ISOPARAMETRIC_CURVES:
                    work1 = sizeof(Pint) * 5;
                    *sl = (work1 - 1) / 80 + 1; 
                    break;
                case PSC_LEVEL_CURVES_MC:
                    work1 = (sizeof(Pint) * 4) + (sizeof(Pfloat) *
                    (6 + data->para_surf_characs.data.psc_4.params.num_floats));
                    *sl = (work1 - 1) / 80 + 1;
                    break;
                case PSC_LEVEL_CURVES_WC:
                    work1 = (sizeof(Pint) * 4) + (sizeof(Pfloat) *
                    (6 + data->para_surf_characs.data.psc_5.params.num_floats));
                    *sl = (work1 - 1) / 80 + 1;
                    break;
                }
            case PELSST:
                *il = 2 + data->lss.activation.num_ints +
                      data->lss.deactivation.num_ints;
                break;
            }
        }
    }
}


/* INQUIRE CURRENT ELEMENT TYPE AND SIZE */
void
pqcets_( errind, eltype, il, rl, sl)
Pint	*errind;	/* OUT error indicator	*/
Pint	*eltype;	/* OUT element type	*/
Pint	*il;		/* OUT dimension of integer array	*/
Pint	*rl;		/* OUT dimension of real array	*/
Pint	*sl;		/* OUT dimension of character array	*/
{
    Phg_args				cp_args;
    register Phg_args_q_el_data		*args = &cp_args.data.q_el_data;
    Phg_ret	ret;
    Pstore	store;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR5;

    } else if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
        *errind = ERR5;

    } else {
        *errind = 0;
        args->el_id = -1;	/* signal to use the current element */
        pcreate_store( errind, &store);
        if ( !*errind) {
            fb_elem_type_size( &cp_args, store, &ret, errind, eltype, il, rl,
                sl);
            pdel_store( store);
        }
    }
}


/* INQUIRE ELEMENT TYPE AND SIZE */
void
pqets_( strid, elenum, errind, eltype, il, rl, sl)
Pint	*strid;		/* structure identifier	*/
Pint	*elenum;	/* element number	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*eltype;	/* OUT element type	*/
Pint	*il;		/* OUT dimension of integer array	*/
Pint	*rl;		/* OUT dimension of real array	*/
Pint	*sl;		/* OUT dimension of character array	*/
{
    Phg_args				cp_args;
    register Phg_args_q_el_data		*args = &cp_args.data.q_el_data;
    Phg_ret				ret;
    Pstore	store;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if ( *elenum < 0) {
        *errind = ERR202;

    } else {
        *errind = 0;
        args->struct_id = *strid;
        args->el_id = *elenum;
        pcreate_store( errind, &store);
        if ( !*errind) {
            fb_elem_type_size( &cp_args, store, &ret, errind, eltype, il, rl,
                sl);
            pdel_store( store);
        }
    }
}


static void
fb_elem_content( store, eltype, il, rl, sl, errind, ia, ra, lstr, str)
Pstore		store;
Pint		eltype;		/* element type	*/
Pint		il;		/* number of integer entries	*/
Pint		rl;		/* number of real entries	*/
Pint		sl;		/* number of charcter entrie	*/
Pint		*errind;	/* OUT error indicator	*/
Pint		*ia;		/* OUT array containing integer entries	*/
Pfloat		*ra;		/* OUT array containing real entries	*/
Pint		*lstr;		/* OUT length of each character entrie	*/
char		*str;		/* OUT character entrie	*/
{
    Pelem_data	*data;
    register int	i,j,k;
    Pint	w_ia = 0;
    Pint	work1 = 0;
    Pint	work2 = 0;
    Pint	work3 = 0;
    Pint	ix = 0;
    Pint	fx = 0;
    Pint	*ptr_i;
    Pint	*ptr_i1;
    Pint	*ptr_i2;
    Pfloat	*ptr_f;
    Pfloat	*ptr_f1;
    Pfloat	*ptr_f2;
    Pfloat	*ptr_f3;
    Pfloat	*ptr_f4;

    *errind = 0;
    data = &((_Pstore *)store)->data.elem_data;
    switch ( eltype) {
    case PENIL:
    case PERMCV:
    case PEALL:
        break;
    case PEPL3:
    case PEPM3:
    case PEFA3:
        ia[0] = data->point_list3.num_points;
        for ( i = 0; i < ia[0]; i++) {
            ra[i] = data->point_list3.points[i].x;
            ra[ia[0] + i] = data->point_list3.points[i].y;
            ra[ia[0] * 2 + i] = data->point_list3.points[i].z;
        }
        break;
    case PEPL:
    case PEPM:
    case PEFA:
        ia[0] = data->point_list.num_points;
        for ( i = 0; i < ia[0]; i++) {
            ra[i] = data->point_list.points[i].x;
            ra[ia[0] + i] = data->point_list.points[i].y;
        }
        break;
    case PETX3:
        ra[0] = data->text3.pos.x;
        ra[1] = data->text3.pos.y;
        ra[2] = data->text3.pos.z;
        ra[3] = data->text3.dir[0].delta_x;
        ra[4] = data->text3.dir[0].delta_y;
        ra[5] = data->text3.dir[0].delta_z;
        ra[6] = data->text3.dir[1].delta_x;
        ra[7] = data->text3.dir[1].delta_y;
        ra[8] = data->text3.dir[1].delta_z;
        lstr[0] = strlen(data->text3.char_string);
        bcopy((char *)data->text3.char_string,
            (char *)str, sizeof(char) * lstr[0]);
        break;
    case PETX:
        ra[0] = data->text.pos.x;
        ra[1] = data->text.pos.y;
        lstr[0] = strlen(data->text.char_string);
        bcopy((char *)data->text.char_string,
            (char *)str, sizeof(char) * lstr[0]);
        break;
    case PEATR3:
        ra[0] = data->anno_text_rel3.ref_point.x;
        ra[1] = data->anno_text_rel3.ref_point.y;
        ra[2] = data->anno_text_rel3.ref_point.z;
        ra[3] = data->anno_text_rel3.offset.delta_x;
        ra[4] = data->anno_text_rel3.offset.delta_y;
        ra[5] = data->anno_text_rel3.offset.delta_z;
        lstr[0] = strlen(data->anno_text_rel3.char_string);
        bcopy((char *)data->anno_text_rel3.char_string,
            (char *)str, sizeof(char) * lstr[0]);
        break;
    case PEATR:
        ra[0] = data->anno_text_rel.ref_point.x;
        ra[1] = data->anno_text_rel.ref_point.y;
        ra[2] = data->anno_text_rel.offset.delta_x;
        ra[3] = data->anno_text_rel.offset.delta_y;
        lstr[0] = strlen(data->anno_text_rel.char_string);
        bcopy((char *)data->anno_text_rel.char_string,
            (char *)str, sizeof(char) * lstr[0]);
        break;
    case PEFAS3:
        k = 0;
        w_ia = 0;
        work1 = rl / 3;
        work2 = work1 * 2;
        for ( i = 0; i < il; i++) {
            ia[i] = w_ia + data->point_list_list3.point_lists[i].num_points;
            for ( j = 0; j < data->point_list_list3.point_lists[i].num_points;
                j++) {
                ra[k] = data->point_list_list3.point_lists[i].points[j].x;
                ra[work1 + k] = data->point_list_list3.point_lists[i].points[j].y;
                ra[work2 + k] = data->point_list_list3.point_lists[i].points[j].z;
            }
            w_ia = ia[i];
        }
        break;
    case PEFAS:
        k = 0;
        w_ia = 0;
        work1 = rl / 3;
        for ( i = 0; i < il; i++) {
            ia[i] = w_ia + data->point_list_list.point_lists[i].num_points;
            for ( j = w_ia; j < ia[i];j++) {
                ra[k] = data->point_list_list.point_lists[i].points[j].x;
                ra[work1 + k] = data->point_list_list.point_lists[i].points[j].y;
            }
            w_ia = ia[i];
        }
        break;
    case PECA3:
        ia[0] = data->cell_array3.colr_array.dims.size_x;
        ia[1] = data->cell_array3.colr_array.dims.size_y;
        for ( i = 0; i < (ia[0]*ia[1]); i++) {
            ia[i+2] = data->cell_array3.colr_array.colr_array[i];
        }
        ra[0] = data->cell_array3.paral.p.x;
        ra[1] = data->cell_array3.paral.p.y;
        ra[2] = data->cell_array3.paral.p.z;
        ra[3] = data->cell_array3.paral.q.x;
        ra[4] = data->cell_array3.paral.q.y;
        ra[5] = data->cell_array3.paral.q.z;
        ra[6] = data->cell_array3.paral.r.x;
        ra[7] = data->cell_array3.paral.r.y;
        ra[8] = data->cell_array3.paral.r.z;
        break;
    case PECA:
        ia[0] = data->cell_array.colr_array.dims.size_x;
        ia[1] = data->cell_array.colr_array.dims.size_y;
        for ( i = 0; i < (ia[0]*ia[1]); i++) {
            ia[i+2] = data->cell_array.colr_array.colr_array[i];
        }
        ra[0] = data->cell_array.rect.p.x;
        ra[1] = data->cell_array.rect.p.y;
        ra[2] = data->cell_array.rect.q.x;
        ra[3] = data->cell_array.rect.q.y;
        break;

    case PEGDP3:
#ifdef EXT_GDP
        ia[0] = work1 = data->gdp3.point_list.num_points;
        ia[1] = data->gdp3.id;
        ptr_f1 = ra;
        ptr_f2 = ptr_f1 + work1;
        ptr_f3 = ptr_f2 + work1;
        for ( i = 0; i < ia[0]; i++) {
            ptr_f1[i] = data->gdp3.point_list.points[i].x;
            ptr_f2[i] = data->gdp3.point_list.points[i].y;
            ptr_f3[i] = data->gdp3.point_list.points[i].z;
        }
        ptr_i = (Pint *)str;
        switch ( ia[1]) {
        case PGDP3_POLYLINE_SET:
            for ( i = 0; i < sl; i++) {
                lstr[0] = 80;
            }
            work1 = data->gdp3.data.gdp3_polylineset.polylinelist.num_ints;
            ptr_i[0] = sizeof(Pint) * ( work1 + 4); /* total record length */
            ptr_i[1] = work1 + 1; /* number of integer entries */
            ptr_i[2] = work1;
            ptr_i += 3;
            w_ia = 0;
            for ( i = 0; i < work1; i++) {
                ptr_i[i] = data->gdp3.data.gdp3_polylineset.polylinelist.ints[i]
                    + w_ia;
                w_ia = ptr_i[i];
            }
            ptr_i += work1; 
            ptr_i[0] = 0; /* number of real entries */
            ptr_i[1] = 0; /* number of character entries */
            break;
        case PGDP3_ELLIPSE:
            lstr[0] = 80;
            ptr_i[0] = 0; /* total record length */
            break;
        case PGDP3_ELLIPSE_ARC_EP:
            lstr[0] = 80;
            ptr_i[0] = 0; /* total record length */
            break;
        case PGDP3_ELLIPSE_ARC_CLOSE_EP:
            lstr[0] = 80;
            ptr_i[0] = sizeof(Pint) * 4; /* total record length */
            ptr_i[1] = 1; /* number of integer entries */
            ptr_i[2] = data->gdp3.data.gdp3_ellipsearcclose_ep.closetype;
            ptr_i[3] = 0; /* number of real entries */
            ptr_i[4] = 0; /* number of character entries */
            break;
        case PGDP3_DISJOINT_POLYLINE:
            lstr[0] = 80;
            ptr_i[0] = 0; /* total record length */
            break;
        case PGDP3_FILLAREA_SET_HOLLOW:
            for ( i = 0; i < sl; i++) {
                lstr[i] = 80;
            }
            ptr_i1 = ptr_i + 2;
            ptr_i1[0] = 0; /* start number facet colour indices */
            ptr_i1[1] = 0; /* end number facet colour indices */
            ptr_i1[2] = 0; /* start number end indices for edges */
            ptr_i1[3] = 0; /* end number end indices for edges */
            ptr_i1[4] = 0; /* start number edge data */
            ptr_i1[5] = 0; /* end number edge data */
            ptr_i1[6] = 0; /* start number vertex colour indices */
            ptr_i1[7] = 0; /* end number vertex colour indices */
            ptr_i1[8] = 0; /* start number facet colour components */
            ptr_i1[9] = 0; /* end number facet colour components */
            ptr_i1[10] = 0; /* start number facet normals */
            ptr_i1[11] = 0; /* end number facet normals */
            ptr_i1[12] = 0; /* start number vertex colour components */
            ptr_i1[13] = 0; /* end number vertex colour components */
            ptr_i1[14] = 0; /* start number vertex normals */
            ptr_i1[15] = 0; /* end number vertex normals */
            ptr_i1[16] = data->gdp3.data.gdp3_fillareaset.fflag;
            ptr_i1[17] = data->gdp3.data.gdp3_fillareaset.eflag;
            ptr_i1[18] = data->gdp3.data.gdp3_fillareaset.vflag;
            ptr_i1[19] = data->gdp3.data.gdp3_fillareaset.color_model;
            ptr_i1[20] = data->gdp3.data.gdp3_fillareaset.fillarealist.num_ints;
            ix = 21;
            ptr_i2 = ptr_i1 + 21;
            w_ia = 0;
            for ( i = 0; i < ptr_i1[20]; i++) {
                *ptr_i2 = data->gdp3.data.gdp3_fillareaset.fillarealist.
                    ints[i] + w_ia;
                w_ia = *ptr_i2;
                ptr_i2++;
                ix++;
            }
            work1 = *(ptr_i2 - 1); /* total number of vertex data */
            if ( ptr_i1[17] == PEVIS) {
                ptr_i1[2] = ix + 1;  /* start number end indices for edges */
                w_ia = 0;
                for ( i = 0; i < ptr_i1[20]; i++) {
                    *ptr_i2 = data->gdp3.data.gdp3_fillareaset.edata[i].
                        num_edges + w_ia;
                    w_ia = *ptr_i2;
                    ptr_i2++;
                    ix++;
                }
                ptr_i1[3] = ix; /* end number end indices for edges */
            }
            if ( ptr_i1[19] == PINDCT) {
                switch ( ptr_i1[16]) {
                case PFCOLR:
                    ptr_i1[0] = ix + 1;  /* start number facet colour indices */
                    for ( i = 0; i < ptr_i1[20]; i++) {
                        *ptr_i2 = data->gdp3.data.gdp3_fillareaset.fdata.
                            colrs[i].ind;
                        ptr_i2++;
                        ix++;
                    }
                    ptr_i1[1] = ix;  /* end number facet colour indices */
                    break;
                case PFCONO:
                    ptr_i1[0] = ix + 1;  /* start number facet colour indices */
                    for ( i = 0; i < ptr_i1[20]; i++) {
                        *ptr_i2 = data->gdp3.data.gdp3_fillareaset.fdata.
                            conorms[i].colr.ind;
                        ptr_i2++;
                        ix++;
                    }
                    ptr_i1[1] = ix;  /* end number facet colour indices */
                }
            }
            if ( ptr_i1[17] == PEVIS) {
                ptr_i1[4] = ix + 1;  /* start number edge data */
                for ( i = 0; i < ptr_i1[20]; i++) {
                    for ( j = 0; j < data->gdp3.data.gdp3_fillareaset.edata[i].
                        num_edges; j++) {
                        *ptr_i2 = data->gdp3.data.gdp3_fillareaset.edata[i].
                            edgedata.edges[j];
                        ptr_i2++;
                        ix++;
                    }
                }
                ptr_i1[5] = ix; /* end number edge data */
            }
            if ( ptr_i1[19] == PINDCT) {
                switch ( ptr_i1[18]) {
                case PCCOLR:
                    ptr_i1[6] = ix + 1; /* start number vertex colour indices */
                    for ( i = 0; i < ptr_i1[20]; i++) {
                        for ( j = 0; j < data->gdp3.data.gdp3_fillareaset.
                            fillarealist.ints[i]; j++) {
                            *ptr_i2 = data->gdp3.data.gdp3_fillareaset.vdata[i].
                                colrs[j].ind;
                            ptr_i2++;
                            ix++;
                        }
                    }
                    ptr_i1[7] = ix;  /* end number vertex colour indices */
                    break;
                case PCCONO:
                    ptr_i1[6] = ix + 1; /* start number vertex colour indices */
                    for ( i = 0; i < ptr_i1[20]; i++) {
                        for ( j = 0; j < data->gdp3.data.gdp3_fillareaset.
                            fillarealist.ints[i]; j++) {
                            *ptr_i2 = data->gdp3.data.gdp3_fillareaset.vdata[i].
                                conorms[j].colr.ind;
                            ptr_i2++;
                            ix++;
                        }
                    }
                    ptr_i1[7] = ix;  /* end number vertex colour indices */
                }
            }
            ptr_i[1] = ix; /* number of integer entries */

            fx = 0;
            ptr_f = (Pfloat *)(ptr_i2 + 1);
            if ( ptr_i1[19] != PINDCT) {
                switch ( ptr_i1[16]) {
                case PFCOLR:
                    ptr_i1[8] = fx + 1; /* start number facet colour
                                         * components */
                    for ( i = 0; i < ptr_i1[20]; i++) {
                        *ptr_f = data->gdp3.data.gdp3_fillareaset.fdata.colrs[i].direct.rgb.red;
                        *(ptr_f + 1) = data->gdp3.data.gdp3_fillareaset.fdata.colrs[i].direct.rgb.green;
                        *(ptr_f + 2) = data->gdp3.data.gdp3_fillareaset.fdata.colrs[i].direct.rgb.blue;
                        ptr_f = ptr_f + 3;
                    }
                    fx += ptr_i1[20] * 3;
                    ptr_i1[9] = fx;  /* end number facet colour components */
                    break;
                case PFCONO:
                    ptr_i1[8] = fx + 1; /* start number facet colour
                                         * components */
                    for ( i = 0; i < ptr_i1[20]; i++) {
                        *ptr_f = data->gdp3.data.gdp3_fillareaset.fdata.conorms[i].colr.direct.rgb.red;
                        *(ptr_f + 1) = data->gdp3.data.gdp3_fillareaset.fdata.conorms[i].colr.direct.rgb.green;
                        *(ptr_f + 2) = data->gdp3.data.gdp3_fillareaset.fdata.conorms[i].colr.direct.rgb.blue;
                        ptr_f = ptr_f + 3;
                    }
                    fx += ptr_i1[20] * 3;
                    ptr_i1[9] = fx;  /* end number facet colour components */
                }
            }
            switch ( ptr_i1[16]) {
            case PFNORM:
                ptr_i1[10] = fx + 1; /* start number facet normals */
                ptr_f1 = ptr_f;
                ptr_f2 = ptr_f1 + ptr_i1[20];
                ptr_f3 = ptr_f2 + ptr_i1[20];
                for ( i = 0; i < ptr_i1[20]; i++) {
                    *ptr_f1 = data->gdp3.data.gdp3_fillareaset.fdata.
                        norms[i].delta_x;
                    *ptr_f2 = data->gdp3.data.gdp3_fillareaset.fdata.
                        norms[i].delta_y;
                    *ptr_f3 = data->gdp3.data.gdp3_fillareaset.fdata.
                        norms[i].delta_z;
                    ptr_f1++;
                    ptr_f2++;
                    ptr_f3++;
                }
                ptr_f = ptr_f3;
                fx += ptr_i1[20] * 3;
                ptr_i1[11] = fx;  /* end number facet normals */
                break;
            case PFCONO:
                ptr_i1[10] = fx + 1; /* start number facet normals */
                ptr_f1 = ptr_f;
                ptr_f2 = ptr_f1 + ptr_i1[20];
                ptr_f3 = ptr_f2 + ptr_i1[20];
                for ( i = 0; i < ptr_i1[20]; i++) {
                    *ptr_f1 = data->gdp3.data.gdp3_fillareaset.fdata.
                        conorms[i].norm.delta_x;
                    *ptr_f2 = data->gdp3.data.gdp3_fillareaset.fdata.
                        conorms[i].norm.delta_y;
                    *ptr_f3 = data->gdp3.data.gdp3_fillareaset.fdata.
                        conorms[i].norm.delta_z;
                    ptr_f1++;
                    ptr_f2++;
                    ptr_f3++;
                }
                ptr_f = ptr_f3;
                fx += ptr_i1[20] * 3;
                ptr_i1[11] = fx;  /* end number facet normals */
            }
            if ( ptr_i1[19] != PINDCT) {
                switch ( ptr_i1[18]) {
                case PCCOLR:
                    ptr_i1[12] = fx + 1; /* start number vertex colour 
                                          * components */
                    for ( i = 0; i < ptr_i1[20]; i++) {
                        for ( j = 0; j < data->gdp3.data.gdp3_fillareaset.
                            fillarealist.ints[i]; j++) {
                            *ptr_f = data->gdp3.data.gdp3_fillareaset.vdata[i].colrs[j].direct.rgb.red;
                            *(ptr_f + 1) = data->gdp3.data.gdp3_fillareaset.vdata[i].colrs[j].direct.rgb.green;
                            *(ptr_f + 2) = data->gdp3.data.gdp3_fillareaset.vdata[i].colrs[j].direct.rgb.blue;
                            ptr_f = ptr_f + 3;
                        }
                    }
                    fx += work1 * 3;
                    ptr_i1[13] = fx;  /* end number vertex colour components */
                    break;
                case PCCONO:
                    ptr_i1[12] = fx + 1; /* start number vertex colour 
                                          * components */
                    for ( i = 0; i < ptr_i1[20]; i++) {
                        for ( j = 0; j < data->gdp3.data.gdp3_fillareaset.
                            fillarealist.ints[i]; j++) {
                            *ptr_f = data->gdp3.data.gdp3_fillareaset.vdata[i].
                                conorms[j].colr.direct.rgb.red;
                            *(ptr_f + 1) = data->gdp3.data.gdp3_fillareaset.vdata[i].conorms[j].colr.direct.rgb.green;
                            *(ptr_f + 2) = data->gdp3.data.gdp3_fillareaset.vdata[i].conorms[j].colr.direct.rgb.blue;
                            ptr_f = ptr_f + 3;
                        }
                    }
                    fx += work1 * 3;
                    ptr_i1[13] = fx;  /* end number vertex colour componrnts */
                }
            }
            switch ( ptr_i1[18]) {
            case PCNORM:
                ptr_i1[14] = fx + 1; /* start number vertex normals */
                ptr_f1 = ptr_f;
                ptr_f2 = ptr_f1 + work1;
                ptr_f3 = ptr_f2 + work1;
                for ( i = 0; i < ptr_i1[20]; i++) {
                    for ( j = 0; j < data->gdp3.data.gdp3_fillareaset.
                        fillarealist.ints[i]; j++) {
                        *ptr_f1 = data->gdp3.data.gdp3_fillareaset.vdata[i].
                            normals[j].delta_x;
                        *ptr_f2 = data->gdp3.data.gdp3_fillareaset.vdata[i].
                            normals[j].delta_y;
                        *ptr_f3 = data->gdp3.data.gdp3_fillareaset.vdata[i].
                            normals[j].delta_z;
                        ptr_f1++;
                        ptr_f2++;
                        ptr_f3++;
                    }
                }
                ptr_f = ptr_f3;
                fx += work1 * 3;
                ptr_i1[15] = fx;  /* end number vertex normals */
                break;
            case PCCONO:
                ptr_i1[14] = fx + 1; /* start number vertex normals */
                ptr_f1 = ptr_f;
                ptr_f2 = ptr_f1 + work1;
                ptr_f3 = ptr_f2 + work1;
                for ( i = 0; i < ptr_i1[20]; i++) {
                    for ( j = 0; j < data->gdp3.data.gdp3_fillareaset.
                        fillarealist.ints[i]; j++) {
                        *ptr_f1 = data->gdp3.data.gdp3_fillareaset.vdata[i].
                            conorms[j].norm.delta_x;
                        *ptr_f2 = data->gdp3.data.gdp3_fillareaset.vdata[i].
                            conorms[j].norm.delta_y;
                        *ptr_f3 = data->gdp3.data.gdp3_fillareaset.vdata[i].
                            conorms[j].norm.delta_z;
                        ptr_f1++;
                        ptr_f2++;
                        ptr_f3++;
                    }
                }
                ptr_f = ptr_f3;
                fx += work1 * 3;
                ptr_i1[15] = fx;  /* end number vertex normals */
            }
            *ptr_i2 = fx; /* number of real entries */
            ptr_i2 = (Pint *)ptr_f;
            *ptr_i2 = 0; /* number of character entries */
            /* total record length */
            *ptr_i = (sizeof(Pint) * ( 3 + ix)) + (sizeof(Pfloat) * fx);
            break;
        default:
            lstr[0] = 80;
            ptr_i[0] = 0; /* total record length */
        }
#endif
        break;

    case PEGDP:
#ifdef EXT_GDP
        ia[0] = work1 = data->gdp.point_list.num_points;
        ia[1] = data->gdp.id;
        ptr_f1 = ra;
        ptr_f2 = ptr_f1 + work1;
        for ( i = 0; i < ia[0]; i++) {
            ptr_f1[i] = data->gdp.point_list.points[i].x;
            ptr_f2[i] = data->gdp.point_list.points[i].y;
        }
        ptr_i = (Pint *)str;
        switch ( ia[1]) {
        case PGDP_POLYLINE_SET:
            for ( i = 0; i < sl; i++) {
                lstr[0] = 80;
            }
            work1 = data->gdp.data.gdp_polylineset.polylinelist.num_ints;
            ptr_i[0] = sizeof(Pint) * ( work1 + 4); /* total record length */
            ptr_i[1] = work1 + 1; /* number of integer entries */
            ptr_i[2] = work1;
            ptr_i += 3;
            w_ia = 0;
            for ( i = 0; i < work1; i++) {
                ptr_i[i] = data->gdp.data.gdp_polylineset.polylinelist.ints[i] +
                    w_ia;
                w_ia = ptr_i[i];
            }
            ptr_i += work1; 
            ptr_i[0] = 0; /* number of real entries */
            ptr_i[1] = 0; /* number of character entries */
            break;
        case PGDP_CIRCLE:
            lstr[0] = 80;
            ptr_i[0] = sizeof(Pint) * 3 + sizeof(Pfloat) * 1; /* total record
                                                               * length */
            ptr_i[1] = 0; /* number of integer entries */
            ptr_i[2] = 1; /* number of real entries */
            ptr_f = (Pfloat *)(ptr_i + 3);
            ptr_f[0] = data->gdp.data.gdp_circle.radius;
            ptr_i = (Pint *)(ptr_f + 1);
            ptr_i[0] = 0; /* number of character entries */
            break;
        case PGDP_ELLIPSE:
            lstr[0] = 80;
            ptr_i[0] = 0; /* total record length */
            break;
        case PGDP_CIRCLE_ARC_EP:
            lstr[0] = 80;
            ptr_i[0] = sizeof(Pint) * 3 + sizeof(Pfloat) * 1; /* total record
                                                               * length */
            ptr_i[1] = 0; /* number of integer entries */
            ptr_i[2] = 1; /* number of real entries */
            ptr_f = (Pfloat *)(ptr_i + 3);
            ptr_f[0] = data->gdp.data.gdp_circlearc_ep.radius;
            ptr_i = (Pint *)(ptr_f + 1);
            ptr_i[0] = 0; /* number of character entries */
            break;
        case PGDP_CIRCLE_ARC_CLOSE_EP:
            lstr[0] = 80;
            ptr_i[0] = sizeof(Pint) * 4 + sizeof(Pfloat) * 1; /* total record
                                                               * length */
            ptr_i[1] = 1; /* number of integer entries */
            ptr_i[2] = data->gdp.data.gdp_circlearcclose_ep.closetype;
            ptr_i[3] = 1; /* number of real entries */
            ptr_f = (Pfloat *)(ptr_i + 4);
            ptr_f[0] = data->gdp.data.gdp_circlearcclose_ep.radius;
            ptr_i = (Pint *)(ptr_f + 1);
            ptr_i[0] = 0; /* number of character entries */
            break;
        case PGDP_ELLIPSE_ARC_EP:
            lstr[0] = 80;
            ptr_i[0] = 0; /* total record length */
            break;
        case PGDP_ELLIPSE_ARC_CLOSE_EP:
            lstr[0] = 80;
            ptr_i[0] = sizeof(Pint) * 4; /* total record length */
            ptr_i[1] = 1; /* number of integer entries */
            ptr_i[2] = data->gdp.data.gdp_ellipsearcclose_ep.closetype;
            ptr_i[3] = 0; /* number of real entries */
            ptr_i[4] = 0; /* number of character entries */
            break;
        case PGDP_DISJOINT_POLYLINE:
            lstr[0] = 80;
            ptr_i[0] = 0; /* total record length */
            break;
        default:
            lstr[0] = 80;
            ptr_i[0] = 0; /* total record length */
        }
#endif
        break;

    case PEPLI:
    case PEPMI:
    case PETXI:
    case PEII:
    case PEEDI:
    case PELN:
    case PEPLCI:
    case PEMK:
    case PEPMCI:
    case PETXFN:
    case PETXCI:
    case PEANST:
    case PEISI:
    case PEICI:
    case PEEDT:
    case PEEDCI:
    case PEHRID:
    case PEMCLI:
    case PEVWI:
    case PEEXST:
    case PELB:
    case PEPKID:
        ia[0] = data->int_data;
        break;
    case PETXPR:
        ia[0] = (Pint)data->text_prec;
        break;
    case PETXP:
    case PEATP:
        ia[0] = (Pint)data->text_path;
        break;
    case PEIS:
        ia[0] = (Pint)data->int_style;
        break;
    case PEEDFG:
        ia[0] = (Pint)data->edge_flag;
        break;
    case PELWSC:
    case PEMKSC: 
    case PECHXP: 
    case PECHSP:
    case PECHH:
    case PEATCH:
    case PEEWSC:
        ra[0] = data->float_data;
        break;
    case PECHUP:
    case PEATCU:
        ra[0] = data->char_up_vec.delta_x;
        ra[1] = data->char_up_vec.delta_y;
        break;
    case PEPA:
        ra[0] = data->pat_size.size_x;
        ra[1] = data->pat_size.size_y;
        break;
    case PEPARF:
        ra[0] = data->pat_ref_point.x;
        ra[1] = data->pat_ref_point.y;
        break;
    case PETXAL:
    case PEATAL:
        ia[0] = data->text_align.hor;
        ia[1] = data->text_align.vert;
        break;
    case PEIASF:
        ia[0] = (Pint)data->asf.id;
        ia[1] = (Pint)data->asf.source;
        break;
    case PEPRPV:
        ra[0] = data->pat_ref_point_vecs.ref_point.x;
        ra[1] = data->pat_ref_point_vecs.ref_point.y;
        ra[2] = data->pat_ref_point_vecs.ref_point.z;
        ra[3] = data->pat_ref_point_vecs.ref_vec[0].delta_x;
        ra[4] = data->pat_ref_point_vecs.ref_vec[0].delta_y;
        ra[5] = data->pat_ref_point_vecs.ref_vec[0].delta_z;
        ra[6] = data->pat_ref_point_vecs.ref_vec[1].delta_x;
        ra[7] = data->pat_ref_point_vecs.ref_vec[1].delta_y;
        ra[8] = data->pat_ref_point_vecs.ref_vec[1].delta_z;
        break;
    case PEADS:
    case PERES:
        for ( i = 0; i < il; i++) {
            ia[i] = data->names.ints[i];
        }
        break;
    case PELMT3:
        k = 0;
        ia[0] = (Pint)data->local_tran3.compose_type;
        for ( i = 0; i < 4 ; i++) {
            for ( j = 0; j < 4; j++) {
                ra[k] = data->local_tran3.matrix[j][i];
                k ++;
            }
        }
        break;
    case PELMT:
        k = 0;
        ia[0] = (Pint)data->local_tran.compose_type;
        for ( i = 0; i < 3; i++) {
            for ( j = 0; j < 3; j++) {
                ra[k] = data->local_tran.matrix[j][i];
                k ++;
            }
        }
        break;
    case PEGMT3:
        k = 0;
        for ( i = 0; i < 4; i++) {
            for ( j = 0; j < 4; j++) {
                ra[k] = data->global_tran3[j][i];
                k ++;
            }
        }
        break;
    case PEGMT:
        k = 0;
        for ( i = 0; i < 3; i++) {
            for ( j = 0; j < 3; j++) {
                ra[k] = data->global_tran[j][i];
                k ++;
            }
        }
        break;
    case PEMCV3:
        ia[0] = data->model_clip3.op;
        ia[1] = data->model_clip3.half_spaces.num_half_spaces;
        for ( i = 0; i < ia[1]; i++) {
            work1 = 6 * i;
            ra[work1] =
                data->model_clip3.half_spaces.half_spaces[i].point.x;
            ra[work1 + 1] =
                data->model_clip3.half_spaces.half_spaces[i].point.y;
            ra[work1 + 2] =
                data->model_clip3.half_spaces.half_spaces[i].point.z;
            ra[work1 + 3] =
                data->model_clip3.half_spaces.half_spaces[i].norm.delta_x;
            ra[work1 + 4] =
                data->model_clip3.half_spaces.half_spaces[i].norm.delta_y;
            ra[work1 + 5] =
                data->model_clip3.half_spaces.half_spaces[i].norm.delta_z;
        }
        break;
    case PEMCV:
        ia[0] = data->model_clip.op;
        ia[1] = data->model_clip.half_spaces.num_half_spaces;
        for ( i = 0; i < ia[1]; i++) {
            work1 = 4 * i;
            ra[work1] =
                data->model_clip.half_spaces.half_spaces[i].point.x;
            ra[work1 + 1] =
                data->model_clip.half_spaces.half_spaces[i].point.y;
            ra[work1 + 2] =
                data->model_clip.half_spaces.half_spaces[i].norm.delta_x;
            ra[work1 + 3] =
                data->model_clip.half_spaces.half_spaces[i].norm.delta_y;
        }
        break;
    case PEAP:
        for ( i = 0; i < sl; i++) {
            lstr[i] = 80;
        }
        bcopy((char *)data->appl_data.data,
            (char *)str, sizeof(char) * sl * 80);
        break;

    case PEGSE:
#ifdef EXT_GSE | EXT_INPUT
        ia[0] = data->gse.id;
        ptr_i = (Pint *)str;
        switch ( data->gse.id) {
#ifdef EXT_GSE
        case PGSE_POLYLINE_SP_ATTR:
            lstr[0] = 80;
            ptr_i[0] = sizeof(Pint) * 4; /* total decoad length */
            ptr_i[1] = 1; /* number of integer entris */
            ptr_i[2] = data->gse.data.gse_polyline_sp.sidepoint;
            ptr_i[3] = 0; /* number of real entris */
            ptr_i[4] = 0; /* number of character entris */
            break;
        case PGSE_EDGE_SP_ATTR:
            lstr[0] = 80;
            ptr_i[0] = sizeof(Pint) * 4; /* total decoad length */
            ptr_i[1] = 1; /* number of integer entris */
            ptr_i[2] = data->gse.data.gse_edge_sp.sidepoint;
            ptr_i[3] = 0; /* number of real entris */
            ptr_i[4] = 0; /* number of character entris */
            break;
#endif
#ifdef EXT_INPUT
        case PGSE_LOCAL_INPUT_INDEX:
            lstr[0] = 80;
            ptr_i[0] = sizeof(Pint) * 5; /* total decoad length */
            ptr_i[1] = 2; /* number of integer entris */
            ptr_i[2] = data->gse.data.gse_localinput_index.index;
            ptr_i[3] = data->gse.data.gse_localinput_index.type;
            ptr_i[4] = 0; /* number of real entris */
            ptr_i[5] = 0; /* number of character entris */
            break;
#endif
        default:
            lstr[0] = 80;
            ptr_i[0] = 0; /* total decoad length */
        }
#endif
        break;
            
    /* PHIGS+ functions */
    case PEPLS3:
        ia[4] = data->plsd3.vflag;
        ia[5] = data->plsd3.colr_model;
        ia[6] = data->plsd3.npl;
        if ( ia[6] <= 0) {
            break;
        }
        w_ia = 0;
        switch ( ia[4]) {
        case PCOORD:
            ia[0] = 0; /* start number vertex colour indices */
            ia[1] = 0; /* end number vertex colour indices */
            ia[2] = 0; /* start number vertex colour components */
            ia[3] = 0; /* end number vertex colour components */
            ptr_i = &(ia[7]);
            ix = 7;
            for ( i = 0; i < ia[6]; i++) {
                *ptr_i = data->plsd3.vdata[i].num_vertices + w_ia;
                w_ia = *ptr_i;
                ptr_i++;
                ix++;
            }
            work1 = ia[ix - 1];
            ptr_f1 = ra;
            ptr_f2 = ptr_f1 + work1;
            ptr_f3 = ptr_f2 + work1;
            for ( i = 0; i < ia[6]; i++) {
                for ( j = 0; j < data->plsd3.vdata[i].num_vertices; j++) {
                    *ptr_f1 = data->plsd3.vdata[i].vertex_data.points[j].x;
                    *ptr_f2 = data->plsd3.vdata[i].vertex_data.points[j].y;
                    *ptr_f3 = data->plsd3.vdata[i].vertex_data.points[j].z;
                    ptr_f1++;
                    ptr_f2++;
                    ptr_f3++;
                }
            }
            break;
        case PCCOLR:
            ptr_i = &(ia[7]);
            ix = 7;
            for ( i = 0; i < ia[6]; i++) {
                *ptr_i = data->plsd3.vdata[i].num_vertices + w_ia;
                w_ia = *ptr_i;
                ptr_i++;
                ix++;
            }
            work1 = ia[ix - 1];
            ptr_f1 = ra;
            ptr_f2 = ptr_f1 + work1;
            ptr_f3 = ptr_f2 + work1;
            fx = 0;
            for ( i = 0; i < ia[6]; i++) {
                for ( j = 0; j < data->plsd3.vdata[i].num_vertices; j++) {
                    *ptr_f1 =
                        data->plsd3.vdata[i].vertex_data.ptcolrs[j].point.x;
                    *ptr_f2 =
                        data->plsd3.vdata[i].vertex_data.ptcolrs[j].point.y;
                    *ptr_f3 =
                        data->plsd3.vdata[i].vertex_data.ptcolrs[j].point.z;
                    ptr_f1++;
                    ptr_f2++;
                    ptr_f3++;
                    fx++;
                }
            }
            fx = work1 * 3;
            ptr_f = ptr_f3;
            switch ( ia[5]) {
            case PINDCT:
                ia[0] = ix + 1; /* start number vertex colour indices */
                ia[2] = 0; /* start number vertex colour components */
                ia[3] = 0; /* end number vertex colour components */
                for ( i = 0; i < ia[6]; i++) {
                    for ( j = 0; j < data->plsd3.vdata[i].num_vertices; j++) {
                        *ptr_i =
                           data->plsd3.vdata[i].vertex_data.ptcolrs[j].colr.ind;
                        ptr_i++;
                        ix++;
                    }
                }
                ia[1] = ix; /* end number vertex colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[0] = 0; /* start number vertex colour indices */
                ia[1] = 0; /* end number vertex colour indices */
                ia[2] = fx + 1; /* start number vertex colour components */
                for ( i = 0; i < ia[6]; i++) {
                    for ( j = 0; j < data->plsd3.vdata[i].num_vertices; j++) {
                        *ptr_f = data->plsd3.vdata[i].vertex_data.ptcolrs[j].colr.direct.rgb.red;
                        *(ptr_f + 1) = data->plsd3.vdata[i].vertex_data.ptcolrs[j].colr.direct.rgb.green;
                        *(ptr_f + 2)= data->plsd3.vdata[i].vertex_data.ptcolrs[j].colr.direct.rgb.blue;
                        ptr_f = ptr_f + 3;
                    }
                }
                fx += work1 * 3;
                ia[3] = fx; /* end number vertex colour components */
            }
        }
        break;

    case PEFSD3:
        ia[0] = 0; /* start number facet colour indices */
        ia[1] = 0; /* end number facet colour indices */
        ia[2] = 0; /* start number array of end indices for edges */
        ia[3] = 0; /* end number array of end indices for edges */
        ia[4] = 0; /* start number edge data */
        ia[5] = 0; /* end number edge data */
        ia[6] = 0; /* start number vertex colour indices */
        ia[7] = 0; /* end number vertex colour indices */
        ia[8] = 0; /* start number facet colour components */
        ia[9] = 0; /* end number facet colour components */
        ia[10] = 0; /* start number facet normal data */
        ia[11] = 0; /* end number facet normal data */
        ia[12] = 0; /* start number vertex colour components */
        ia[13] = 0; /* end number vertex colour components */
        ia[14] = 0; /* start number vertex normal data */
        ia[15] = 0; /* end number vertex normal data */
        ia[16] = data->fasd3.fflag;
        ia[17] = data->fasd3.eflag;
        ia[18] = data->fasd3.vflag;
        ia[19] = data->fasd3.colr_model;
        ia[20] = data->fasd3.nfa;
        if ( ia[20] <= 0) {
            break;
        }
        ix = 21; /* integer table index */
        ptr_i = &(ia[21]);
        w_ia = 0; 
        for ( i = 0; i < ia[20]; i++) {
            *ptr_i = data->fasd3.vdata[i].num_vertices + w_ia;
            w_ia = *ptr_i;
            ptr_i++;
            ix++;
        }
        work1 = ia[ix - 1]; /* number of vertex data */
        fx = 0; /* float table index */
        ptr_f1 = ra;
        ptr_f2 = ptr_f1 + work1;
        ptr_f3 = ptr_f2 + work1;
        switch ( ia[18]) {
        case PCOORD:
            for ( i = 0; i < ia[20]; i++) {
                for ( j = 0; j < data->fasd3.vdata[i].num_vertices; j++) {
                    *ptr_f1 = data->fasd3.vdata[i].vertex_data.points[j].x;
                    *ptr_f2 = data->fasd3.vdata[i].vertex_data.points[j].y;
                    *ptr_f3 = data->fasd3.vdata[i].vertex_data.points[j].z;
                    ptr_f1++;
                    ptr_f2++;
                    ptr_f3++;
                }
            }
            break;
        case PCCOLR:
            for ( i = 0; i < ia[20]; i++) {
                for ( j = 0; j < data->fasd3.vdata[i].num_vertices; j++) {
                    *ptr_f1 =
                        data->fasd3.vdata[i].vertex_data.ptcolrs[j].point.x;
                    *ptr_f2 =
                        data->fasd3.vdata[i].vertex_data.ptcolrs[j].point.y;
                    *ptr_f3 =
                        data->fasd3.vdata[i].vertex_data.ptcolrs[j].point.z;
                    ptr_f1++;
                    ptr_f2++;
                    ptr_f3++;
                }
            }
            break;
        case PCNORM:
            for ( i = 0; i < ia[20]; i++) {
                for ( j = 0; j < data->fasd3.vdata[i].num_vertices; j++) {
                    *ptr_f1 =
                        data->fasd3.vdata[i].vertex_data.ptnorms[j].point.x;
                    *ptr_f2 =
                        data->fasd3.vdata[i].vertex_data.ptnorms[j].point.y;
                    *ptr_f3 =
                        data->fasd3.vdata[i].vertex_data.ptnorms[j].point.z;
                    ptr_f1++;
                    ptr_f2++;
                    ptr_f3++;
                }
            }
            break;
        case PCCONO:
            for ( i = 0; i < ia[20]; i++) {
                for ( j = 0; j < data->fasd3.vdata[i].num_vertices; j++) {
                    *ptr_f1 =
                        data->fasd3.vdata[i].vertex_data.ptconorms[j].point.x;
                    *ptr_f2 =
                        data->fasd3.vdata[i].vertex_data.ptconorms[j].point.y;
                    *ptr_f3 =
                        data->fasd3.vdata[i].vertex_data.ptconorms[j].point.z;
                    ptr_f1++;
                    ptr_f2++;
                    ptr_f3++;
                }
            }
            break;
        }
        fx = work1 * 3; /* float table index */
        ptr_f = ptr_f3;

        switch ( ia[16]) {
        case PNOF:
            break;
        case PFCOLR:
            switch ( ia[19]) {
            case PINDCT:
                ia[0] = ix + 1; /* start number facet colour indices */
                *ptr_i = data->fasd3.fdata.colr.ind;
                ptr_i++;
                ix++;
                ia[1] = ix; /* end number facet colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[8] = fx + 1; /* start number facet colour components */
                ptr_f[0] = data->fasd3.fdata.colr.direct.rgb.red;
                ptr_f[1] = data->fasd3.fdata.colr.direct.rgb.green;
                ptr_f[2] = data->fasd3.fdata.colr.direct.rgb.blue;
                ptr_f = ptr_f + 3;
                fx = fx + 3;
                ia[9] = fx; /* end number facet colour components */
            }
            break;
        case PFNORM:
            ia[10] = fx + 1; /* start number facet normal data */
            ptr_f[0] = data->fasd3.fdata.norm.delta_x;
            ptr_f[1] = data->fasd3.fdata.norm.delta_y;
            ptr_f[2] = data->fasd3.fdata.norm.delta_z;
            ptr_f = ptr_f + 3;
            fx = fx + 3;
            ia[11] = fx; /* end number facet normal data */
            break;
        case PFCONO:
            switch ( ia[19]) {
            case PINDCT:
                ia[0] = ix + 1; /* start number facet colour indices */
                *ptr_i = data->fasd3.fdata.conorm.colr.ind;
                ptr_i++;
                ix++;
                ia[1] = ix; /* end number facet colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[8] = fx + 1; /* start number facet colour components */
                ptr_f[0] = data->fasd3.fdata.conorm.colr.direct.rgb.red;
                ptr_f[1] = data->fasd3.fdata.conorm.colr.direct.rgb.green;
                ptr_f[2] = data->fasd3.fdata.conorm.colr.direct.rgb.blue;
                ptr_f = ptr_f + 3;
                fx = fx + 3;
                ia[9] = fx; /* end number facet colour components */
            }
            ia[10] = fx + 1; /* start number facet normal data */
            ptr_f[0] = data->fasd3.fdata.conorm.norm.delta_x;
            ptr_f[1] = data->fasd3.fdata.conorm.norm.delta_y;
            ptr_f[2] = data->fasd3.fdata.conorm.norm.delta_z;
            ptr_f = ptr_f + 3;
            fx = fx + 3;
            ia[11] = fx; /* end number facet normal data */
            break;
        }

        switch ( ia[17]) {
        case PNOE:
            break;
        case PEVIS:
            ia[2] = ix + 1; /* start number array of end indices for edges */
            w_ia = 0; 
            for ( i = 0; i < ia[20]; i++) {
                *ptr_i = data->fasd3.edata[i].num_edges + w_ia;
                w_ia = *ptr_i;
                ptr_i++;
                ix++;
            }
            ia[3] = ix; /* end number array of end indices for edges */
            ia[4] = ix + 1; /* start number edge data */
            for ( i = 0; i < ia[20]; i++) {
                for ( j = 0; j < data->fasd3.edata[i].num_edges; j++) {
                    *ptr_i = (Pint)data->fasd3.edata[i].edgedata.edges[j];
                    ptr_i++;
                    ix++;
                }
            }
            ia[5] = ix; /* end number edge data */
        }

        switch ( ia[18]) {
        case PCOORD:
            break;
        case PCCOLR:
            switch ( ia[19]) {
            case PINDCT:
                ia[6] = ix + 1; /* start number vertex colour indices */
                for ( i = 0; i < ia[20]; i++) {
                    for ( j = 0; j < data->fasd3.vdata[i].num_vertices; j++) {
                        *ptr_i = data->
                            fasd3.vdata[i].vertex_data.ptcolrs[j].colr.ind;
                        ptr_i++;
                        ix++;
                    }
                }
                ia[7] = ix; /* end number vertex colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[12] = fx + 1; /* start number vertex colour components */
                for ( i = 0; i < ia[20]; i++) {
                    for ( j = 0; j < data->fasd3.vdata[i].num_vertices; j++) {
                        *ptr_f = data->fasd3.vdata[i].vertex_data.ptcolrs[j].
                            colr.direct.rgb.red;
                        *(ptr_f + 1) = data->fasd3.vdata[i].vertex_data.ptcolrs[j].colr.direct.rgb.green;
                        *(ptr_f + 2) = data->fasd3.vdata[i].vertex_data.ptcolrs[j].colr.direct.rgb.blue;
                        ptr_f = ptr_f + 3;
                    }
                }
                fx += work1 * 3;
                ia[13] = fx; /* end number vertex colour components */
            }
            break;
        case PCNORM:
            ia[14] = fx + 1; /* start number vertex normal data */
            ptr_f1 = ptr_f;
            ptr_f2 = ptr_f1 + work1;
            ptr_f3 = ptr_f2 + work1;
            for ( i = 0; i < ia[20]; i++) {
                for ( j = 0; j < data->fasd3.vdata[i].num_vertices; j++) {
                    *ptr_f1 = data->fasd3.vdata[i].vertex_data.ptnorms[j].
                        norm.delta_x;
                    *ptr_f2 = data->fasd3.vdata[i].vertex_data.ptnorms[j].
                        norm.delta_y;
                    *ptr_f3 = data->fasd3.vdata[i].vertex_data.ptnorms[j].
                        norm.delta_z;
                    ptr_f1++;
                    ptr_f2++;
                    ptr_f3++;
                }
            }
            fx += work1 * 3;
            ia[15] = fx; /* end number vertex normal data */
            ptr_f = ptr_f3;
            break;
        case PCCONO:
            switch ( ia[19]) {
            case PINDCT:
                ia[6] = ix + 1; /* start number vertex colour indices */
                for ( i = 0; i < ia[20]; i++) {
                    for ( j = 0; j < data->fasd3.vdata[i].num_vertices; j++) {
                        *ptr_i = data->
                            fasd3.vdata[i].vertex_data.ptconorms[j].colr.ind;
                        ptr_i++;
                        ix++;
                    }
                }
                ia[7] = ix; /* end number vertex colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[12] = fx + 1; /* start number vertex colour components */
                for ( i = 0; i < ia[20]; i++) {
                    for ( j = 0; j < data->fasd3.vdata[i].num_vertices; j++) {
                        *ptr_f = data->fasd3.vdata[i].vertex_data.ptconorms[j].colr.direct.rgb.red;
                        *(ptr_f + 1) = data->fasd3.vdata[i].vertex_data.ptconorms[j].colr.direct.rgb.green;
                        *(ptr_f + 2) = data->fasd3.vdata[i].vertex_data.ptconorms[j].colr.direct.rgb.blue;
                        ptr_f = ptr_f + 3;
                    }
                }
                fx += work1 * 3;
                ia[13] = fx; /* end number vertex colour components */
            }
            ia[14] = fx + 1; /* start number vertex normal data */
            ptr_f1 = ptr_f;
            ptr_f2 = ptr_f1 + work1;
            ptr_f3 = ptr_f2 + work1;
            for ( i = 0; i < ia[20]; i++) {
                for ( j = 0; j < data->fasd3.vdata[i].num_vertices; j++) {
                    *ptr_f1 = data->fasd3.vdata[i].vertex_data.ptconorms[j].
                        norm.delta_x;
                    *ptr_f2 = data->fasd3.vdata[i].vertex_data.ptconorms[j].
                        norm.delta_y;
                    *ptr_f3 = data->fasd3.vdata[i].vertex_data.ptconorms[j].
                        norm.delta_z;
                    ptr_f1++;
                    ptr_f2++;
                    ptr_f3++;
                }
            }
            fx += work1 * 3;
            ia[15] = fx; /* end number vertex normal data */
            ptr_f = ptr_f3;
            break;
        }
        break;

    case PETRS3:
        ia[0] = 0; /* start number facet colour indices */
        ia[1] = 0; /* end number facet colour indices */
        ia[2] = 0; /* start number vertex colour indices */
        ia[3] = 0; /* end number vertex colour indices */
        ia[4] = 0; /* start number facet colour components */
        ia[5] = 0; /* end number facet colour components */
        ia[6] = 0; /* start number facet normal data */
        ia[7] = 0; /* end number facet normal data */
        ia[8] = 0; /* start number vertex colour components */
        ia[9] = 0; /* end number vertex colour components */
        ia[10] = 0; /* start number vertex normal data */
        ia[11] = 0; /* end number vertex normal data */
        ia[12] = data->tsd3.fflag;
        ia[13] = data->tsd3.vflag;
        ia[14] = data->tsd3.colr_model;
        ia[15] = data->tsd3.nv;
        if ( ia[15] <= 0) {
            break;
        }
        ix = 16; /* integer table index */
        ptr_i = &(ia[16]);
        ptr_f1 = ra;
        ptr_f2 = ptr_f1 + ia[15];
        ptr_f3 = ptr_f2 + ia[15];
        switch ( ia[13]) {
        case PCOORD:
            for ( i = 0; i < ia[15]; i++) {
                *ptr_f1 = data->tsd3.vdata.points[i].x;
                *ptr_f2 = data->tsd3.vdata.points[i].y;
                *ptr_f3 = data->tsd3.vdata.points[i].z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            break;
        case PCCOLR:
            for ( i = 0; i < ia[15]; i++) {
                *ptr_f1 = data->tsd3.vdata.ptcolrs[i].point.x;
                *ptr_f2 = data->tsd3.vdata.ptcolrs[i].point.y;
                *ptr_f3 = data->tsd3.vdata.ptcolrs[i].point.z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            break;
        case PCNORM:
            for ( i = 0; i < ia[15]; i++) {
                *ptr_f1 = data->tsd3.vdata.ptnorms[i].point.x;
                *ptr_f2 = data->tsd3.vdata.ptnorms[i].point.y;
                *ptr_f3 = data->tsd3.vdata.ptnorms[i].point.z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            break;
        case PCCONO:
            for ( i = 0; i < ia[15]; i++) {
                *ptr_f1 = data->tsd3.vdata.ptconorms[i].point.x;
                *ptr_f2 = data->tsd3.vdata.ptconorms[i].point.y;
                *ptr_f3 = data->tsd3.vdata.ptconorms[i].point.z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            break;
        }
        fx = ia[15] * 3; /* float table index */
        ptr_f = ptr_f3;

        switch ( ia[12]) {
        case PNOF:
            break;
        case PFCOLR:
            switch ( ia[14]) {
            case PINDCT:
                ia[0] = ix + 1; /* start number facet colour indices */
                for ( i = 0; i < ia[15] - 2; i++) {
                    *ptr_i = data->tsd3.fdata.colrs[i].ind;
                    ptr_i++;
                    ix++;
                }
                ia[1] = ix; /* end number facet colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[4] = fx + 1; /* start number facet colour components */
                for ( i = 0; i < ia[15] - 2; i++) {
                    *ptr_f = data->tsd3.fdata.colrs[i].direct.rgb.red;
                    *(ptr_f + 1) = data->tsd3.fdata.colrs[i].direct.rgb.green;
                    *(ptr_f + 2) = data->tsd3.fdata.colrs[i].direct.rgb.blue;
                    ptr_f = ptr_f + 3;
                }
                fx += (ia[15] - 2) * 3;
                ia[5] = fx; /* end number facet colour components */
            }
            break;
        case PFNORM:
            ia[6] = fx + 1; /* start number facet normal data */
            ptr_f1 = ptr_f;
            ptr_f2 = ptr_f1 + (ia[15] - 2);
            ptr_f3 = ptr_f2 + (ia[15] - 2);
            for ( i = 0; i < ia[15] - 2; i++) {
                *ptr_f1 = data->tsd3.fdata.norms[i].delta_x;
                *ptr_f2 = data->tsd3.fdata.norms[i].delta_y;
                *ptr_f3 = data->tsd3.fdata.norms[i].delta_z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            ptr_f = ptr_f3;
            fx += (ia[15] - 2) * 3;
            ia[7] = fx; /* end number facet normal data */
            break;
        case PFCONO:
            ia[6] = fx + 1; /* start number facet normal data */
            ptr_f1 = ptr_f;
            ptr_f2 = ptr_f1 + (ia[15] - 2);
            ptr_f3 = ptr_f2 + (ia[15] - 2);
            for ( i = 0; i < ia[15] - 2; i++) {
                *ptr_f1 = data->tsd3.fdata.conorms[i].norm.delta_x;
                *ptr_f2 = data->tsd3.fdata.conorms[i].norm.delta_y;
                *ptr_f3 = data->tsd3.fdata.conorms[i].norm.delta_z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            ptr_f = ptr_f3;
            fx += (ia[15] - 2) * 3;
            ia[7] = fx; /* end number facet normal data */

            switch ( ia[14]) {
            case PINDCT:
                ia[0] = ix + 1; /* start number facet colour indices */
                for ( i = 0; i < ia[15] - 2; i++) {
                    *ptr_i = data->tsd3.fdata.conorms[i].colr.ind;
                    ptr_i++;
                    ix++;
                }
                ia[1] = ix; /* end number facet colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[4] = fx + 1; /* start number facet colour components */
                for ( i = 0; i < ia[15] - 2; i++) {
                    *ptr_f = data->tsd3.fdata.conorms[i].colr.direct.rgb.red;
                    *(ptr_f + 1) = data->tsd3.fdata.conorms[i].colr.direct.rgb.green;
                    *(ptr_f + 2) = data->tsd3.fdata.conorms[i].colr.direct.rgb.blue;
                    ptr_f = ptr_f + 3;
                }
                fx += (ia[15] - 2) * 3;
                ia[5] = fx; /* end number facet colour components */
            }
            break;
        }

        switch ( ia[13]) {
        case PCOORD:
            break;
        case PCCOLR:
            switch ( ia[14]) {
            case PINDCT:
                ia[2] = ix + 1; /* start number vertex colour indices */
                for ( i = 0; i < ia[15]; i++) {
                    *ptr_i = data->tsd3.vdata.ptcolrs[i].colr.ind;
                    ptr_i++;
                    ix++;
                }
                ia[3] = ix; /* end number vertex colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[8] = fx + 1; /* start number vertex colour components */
                for ( i = 0; i < ia[15]; i++) {
                    *ptr_f = data->tsd3.vdata.ptcolrs[i].colr.direct.rgb.red;
                    *(ptr_f + 1) = data->tsd3.vdata.ptcolrs[i].colr.direct.rgb.green;
                    *(ptr_f + 2) = data->tsd3.vdata.ptcolrs[i].colr.direct.rgb.blue;
                    ptr_f = ptr_f + 3;
                }
                fx += ia[15] * 3;
                ia[9] = fx; /* end number vertex colour components */
            }
            break;
        case PCNORM:
            ia[10] = fx + 1; /* start number vertex nomal data */
            ptr_f1 = ptr_f;
            ptr_f2 = ptr_f1 + ia[15];
            ptr_f3 = ptr_f2 + ia[15];
            for ( i = 0; i < ia[15]; i++) {
                *ptr_f1 = data->tsd3.vdata.ptnorms[i].norm.delta_x;
                *ptr_f2 = data->tsd3.vdata.ptnorms[i].norm.delta_y;
                *ptr_f3 = data->tsd3.vdata.ptnorms[i].norm.delta_z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            ptr_f = ptr_f3;
            fx += ia[15] * 3;
            ia[11] = fx; /* end number vertex nomal data */
            break;
        case PCCONO:
            switch ( ia[14]) {
            case PINDCT:
                ia[2] = ix + 1; /* start number vertex colour indices */
                for ( i = 0; i < ia[15]; i++) {
                    *ptr_i = data->tsd3.vdata.ptconorms[i].colr.ind;
                    ptr_i++;
                    ix++;
                }
                ia[3] = ix; /* end number vertex colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[8] = fx + 1; /* start number vertex colour components */
                for ( i = 0; i < ia[15]; i++) {
                    *ptr_f = data->tsd3.vdata.ptconorms[i].colr.direct.rgb.red;
                    *(ptr_f + 1) = data->tsd3.vdata.ptconorms[i].colr.direct.rgb.green;
                    *(ptr_f + 2) = data->tsd3.vdata.ptconorms[i].colr.direct.rgb.blue;
                    ptr_f = ptr_f + 3;
                }
                fx += ia[15] * 3;
                ia[9] = fx; /* end number vertex colour components */
            }
            ia[10] = fx + 1; /* start number vertex nomal data */
            ptr_f1 = ptr_f;
            ptr_f2 = ptr_f1 + ia[15];
            ptr_f3 = ptr_f2 + ia[15];
            for ( i = 0; i < ia[15]; i++) {
                *ptr_f1 = data->tsd3.vdata.ptconorms[i].norm.delta_x;
                *ptr_f2 = data->tsd3.vdata.ptconorms[i].norm.delta_y;
                *ptr_f3 = data->tsd3.vdata.ptconorms[i].norm.delta_z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            ptr_f = ptr_f3;
            fx += ia[15] * 3;
            ia[11] = fx; /* end number vertex nomal data */
        }
        break;

    case PEQMD3:
        ia[0] = 0; /* start number facet colour indices */
        ia[1] = 0; /* end number facet colour indices */
        ia[2] = 0; /* start number vertex colour indices */
        ia[3] = 0; /* end number vertex colour indices */
        ia[4] = 0; /* start number facet colour components */
        ia[5] = 0; /* end number facet colour components */
        ia[6] = 0; /* start number facet normal data */
        ia[7] = 0; /* end number facet normal data */
        ia[8] = 0; /* start number vertex colour components */
        ia[9] = 0; /* end number vertex colour components */
        ia[10] = 0; /* start number vertex normal data */
        ia[11] = 0; /* end number vertex normal data */
        ia[12] = data->qmd3.fflag;
        ia[13] = data->qmd3.vflag;
        ia[14] = data->qmd3.colr_model;
        ia[15] = data->qmd3.dim.size_x;
        ia[16] = data->qmd3.dim.size_y;
        work1 = (ia[15] - 1) * (ia[16] - 1); /* number of facet data */
        work2 = ia[15] * ia[16]; /* number of vertex data */
        if ( work2 <= 0) {
            break;
        }
        ix = 17; /* integer table index */
        ptr_i = &(ia[17]);
        ptr_f1 = ra;
        ptr_f2 = ptr_f1 + work2;
        ptr_f3 = ptr_f2 + work2;
        k = 0;
        switch ( ia[13]) {
        case PCOORD:
            for ( i = 0; i < ia[16]; i++) {
                for ( j = 0; j < ia[15]; j++) {
                    ptr_f1[k] = data->qmd3.vdata.points[k].x;
                    ptr_f2[k] = data->qmd3.vdata.points[k].y;
                    ptr_f3[k] = data->qmd3.vdata.points[k].z;
                    k++;
                }
            }
            break;
        case PCCOLR:
            for ( i = 0; i < ia[16]; i++) {
                for ( j = 0; j < ia[15]; j++) {
                    ptr_f1[k] = data->qmd3.vdata.ptcolrs[k].point.x;
                    ptr_f2[k] = data->qmd3.vdata.ptcolrs[k].point.y;
                    ptr_f3[k] = data->qmd3.vdata.ptcolrs[k].point.z;
                    k++;
                }
            }
            break;
        case PCNORM:
            for ( i = 0; i < ia[16]; i++) {
                for ( j = 0; j < ia[15]; j++) {
                    ptr_f1[k] = data->qmd3.vdata.ptnorms[k].point.x;
                    ptr_f2[k] = data->qmd3.vdata.ptnorms[k].point.y;
                    ptr_f3[k] = data->qmd3.vdata.ptnorms[k].point.z;
                    k++;
                }
            }
            break;
        case PCCONO:
            for ( i = 0; i < ia[16]; i++) {
                for ( j = 0; j < ia[15]; j++) {
                    ptr_f1[k] = data->qmd3.vdata.ptconorms[k].point.x;
                    ptr_f2[k] = data->qmd3.vdata.ptconorms[k].point.y;
                    ptr_f3[k] = data->qmd3.vdata.ptconorms[k].point.z;
                    k++;
                }
            }
            break;
        }
        fx = work2 * 3; /* float table index */
        ptr_f = ra + (work2 * 3);

        switch ( ia[12]) {
        case PNOF:
            break;
        case PFCOLR:
            k = 0;
            switch ( ia[14]) {
            case PINDCT:
                ia[0] = ix + 1; /* start number facet colour indices */
                for ( i = 0; i < ia[16] - 1; i++) {
                    for ( j = 0; j < ia[15] - 1; j++) {
                        ptr_i[k] = data->qmd3.fdata.colrs[k].ind;
                        k++;
                    }
                }
                ix += work1;
                ptr_i += work1;
                ia[1] = ix; /* end number facet colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[4] = fx + 1; /* start number facet colour components */
                for ( i = 0; i < ia[16] - 1; i++) {
                    for ( j = 0; j < ia[15] - 1; j++) {
                        *ptr_f = data->qmd3.fdata.colrs[k].direct.rgb.red;
                        *(ptr_f + 1) = data->qmd3.fdata.colrs[k].direct.rgb.green;
                        *(ptr_f + 2) = data->qmd3.fdata.colrs[k].direct.rgb.blue;
                        ptr_f = ptr_f + 3;
                        k++;
                    }
                }
                fx += work1 * 3;
                ia[5] = fx; /* end number facet colour components */
            }
            break;
        case PFNORM:
            k = 0;
            ia[6] = fx + 1; /* start number facet normal data */
            ptr_f1 = ptr_f;
            ptr_f2 = ptr_f1 + work1;
            ptr_f3 = ptr_f2 + work1;
            for ( i = 0; i < ia[16] - 1; i++) {
                for ( j = 0; j < ia[15] - 1; j++) {
                    ptr_f1[k] = data->qmd3.fdata.norms[k].delta_x;
                    ptr_f2[k] = data->qmd3.fdata.norms[k].delta_y;
                    ptr_f3[k] = data->qmd3.fdata.norms[k].delta_z;
                    k++;
                }
            }
            ptr_f += work1 * 3;
            fx += work1 * 3;
            ia[7] = fx; /* end number facet normal data */
            break;
        case PFCONO:
            k = 0;
            ia[6] = fx + 1; /* start number facet normal data */
            ptr_f1 = ptr_f;
            ptr_f2 = ptr_f1 + work1;
            ptr_f3 = ptr_f2 + work1;
            for ( i = 0; i < ia[16] - 1; i++) {
                for ( j = 0; j < ia[15] - 1; j++) {
                    ptr_f1[k] = data->qmd3.fdata.conorms[k].norm.delta_x;
                    ptr_f2[k] = data->qmd3.fdata.conorms[k].norm.delta_y;
                    ptr_f3[k] = data->qmd3.fdata.conorms[k].norm.delta_z;
                    k++;
                }
            }
            ptr_f += work1 * 3;
            fx += work1 * 3;
            ia[7] = fx; /* end number facet normal data */

            k = 0;
            switch ( ia[14]) {
            case PINDCT:
                ia[0] = ix + 1; /* start number facet colour indices */
                for ( i = 0; i < ia[16] - 1; i++) {
                    for ( j = 0; j < ia[15] - 1; j++) {
                        ptr_i[k] = data->qmd3.fdata.conorms[k].colr.ind;
                        k++;
                    }
                }
                ptr_i += work1;
                ix += work1;
                ia[1] = ix; /* end number facet colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[4] = fx + 1; /* start number facet colour components */
                for ( i = 0; i < ia[16] - 1; i++) {
                    for ( j = 0; j < ia[15] - 1; j++) {
                        *ptr_f = data->qmd3.fdata.conorms[k].colr.direct.rgb.red;
                        *(ptr_f + 1) = data->qmd3.fdata.conorms[k].colr.direct.rgb.green;
                        *(ptr_f + 2) = data->qmd3.fdata.conorms[k].colr.direct.rgb.blue;
                        ptr_f = ptr_f + 3;
                        k++;
                    }
                }
                fx += work1 * 3;
                ia[5] = fx; /* end number facet colour components */
            }
            break;
        }

        switch ( ia[13]) {
        case PCOORD:
            break;
        case PCCOLR:
            k = 0;
            switch ( ia[14]) {
            case PINDCT:
                ia[2] = ix + 1; /* start number vertex colour indices */
                for ( i = 0; i < ia[16]; i++) {
                    for ( j = 0; j < ia[15]; j++) {
                        ptr_i[k] = data->qmd3.vdata.ptcolrs[k].colr.ind;
                        k++;
                    }
                }
                ptr_i += work2;
                ix += work2;
                ia[3] = ix; /* end number vertex colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[8] = fx + 1; /* start number vertex colour components */
                for ( i = 0; i < ia[16]; i++) {
                    for ( j = 0; j < ia[15]; j++) {
                        *ptr_f = data->qmd3.vdata.ptcolrs[k].colr.direct.rgb.red;
                        *(ptr_f + 1)= data->qmd3.vdata.ptcolrs[k].colr.direct.rgb.green;
                        *(ptr_f + 2)= data->qmd3.vdata.ptcolrs[k].colr.direct.rgb.blue;
                        ptr_f = ptr_f + 3;
                        k++;
                    }
                }
                fx += work2 * 3;
                ia[9] = fx; /* end number vertex colour components */
            }
            break;
        case PCNORM:
            k = 0;
            ia[10] = fx + 1; /* start number vertex nomal data */
            ptr_f1 = ptr_f;
            ptr_f2 = ptr_f1 + work2;
            ptr_f3 = ptr_f2 + work2;
            for ( i = 0; i < ia[16]; i++) {
                for ( j = 0; j < ia[15]; j++) {
                    ptr_f1[k] = data->qmd3.vdata.ptnorms[k].norm.delta_x;
                    ptr_f2[k] = data->qmd3.vdata.ptnorms[k].norm.delta_y;
                    ptr_f3[k] = data->qmd3.vdata.ptnorms[k].norm.delta_z;
                    k++;
                }
            }
            ptr_f += work2 * 3;
            fx += work2 * 3;
            ia[11] = fx; /* end number vertex nomal data */
            break;
        case PCCONO:
            k = 0;
            switch ( ia[14]) {
            case PINDCT:
                ia[2] = ix + 1; /* start number vertex colour indices */
                for ( i = 0; i < ia[16]; i++) {
                    for ( j = 0; j < ia[15]; j++) {
                        ptr_i[k] = data->qmd3.vdata.ptconorms[k].colr.ind;
                        k++;
                    }
                }
                ptr_i += work2;
                ix += work2;
                ia[3] = ix; /* end number vertex colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[8] = fx + 1; /* start number vertex colour components */
                for ( i = 0; i < ia[16]; i++) {
                    for ( j = 0; j < ia[15]; j++) {
                        *ptr_f = data->qmd3.vdata.ptconorms[k].colr.direct.rgb.red;
                        *(ptr_f + 1) = data->qmd3.vdata.ptconorms[k].colr.direct.rgb.green;
                        *(ptr_f + 2) = data->qmd3.vdata.ptconorms[k].colr.direct.rgb.blue;
                        ptr_f = ptr_f + 3;
                        k++;
                    }
                }
                fx += work2 * 3;
                ia[9] = fx; /* end number vertex colour components */
            }
            k = 0;
            ia[10] = fx + 1; /* start number vertex nomal data */
            ptr_f1 = ptr_f;
            ptr_f2 = ptr_f1 + work3;
            ptr_f3 = ptr_f2 + work3;
            for ( i = 0; i < ia[16]; i++) {
                for ( j = 0; j < ia[15]; j++) {
                    ptr_f1[k] = data->qmd3.vdata.ptconorms[k].norm.delta_x;
                    ptr_f2[k] = data->qmd3.vdata.ptconorms[k].norm.delta_y;
                    ptr_f3[k] = data->qmd3.vdata.ptconorms[k].norm.delta_z;
                    k++;
                }
            }
            ptr_f += work2 * 3;
            fx += work2 * 3;
            ia[11] = fx; /* end number vertex nomal data */
        }
        break;

    case PESFS3:
        ia[0] = 0; /* start number end indices for vertex */
        ia[1] = 0; /* end number end indices for vertex */
        ia[2] = 0; /* start number vertex indices */
        ia[3] = 0; /* end number vertex indices */
        ia[4] = 0; /* start number end indices for edge */
        ia[5] = 0; /* end number end indices for edge */
        ia[6] = 0; /* start number facet colour indices */
        ia[7] = 0; /* end number facet colour indices */
        ia[8] = 0; /* start number edge data */
        ia[9] = 0; /* end number edge data */
        ia[10] = 0; /* start number vertex colour indices */
        ia[11] = 0; /* end number vertex colour indices */
        ia[12] = 0; /* start number facet colour components */
        ia[13] = 0; /* end number facet colour components */
        ia[14] = 0; /* start number facet normal data */
        ia[15] = 0; /* end number facet normal data */
        ia[16] = 0; /* start number vertex colour components */
        ia[17] = 0; /* end number vertex colour components */
        ia[18] = 0; /* start number vertex normal data */
        ia[19] = 0; /* end number vertex normal data */
        ia[20] = data->sofas3.fflag;
        ia[21] = data->sofas3.eflag;
        ia[22] = data->sofas3.vflag;
        ia[23] = data->sofas3.colr_model;
        ia[24] = data->sofas3.num_sets;
        ia[25] = data->sofas3.vdata.num_vertices;
        if ( ia[25] <= 0) {
            break;
        }
        ix = 20;
        ptr_i = &(ia[20]);
        for ( i = 0; i < ia[24]; i++) { /* number of facet */
            *ptr_i = data->sofas3.vlist[i].num_lists;
            ptr_i++;
            ix++;
        }
        w_ia = 0;
        ia[0] = ix + 1; /* start number end indices for vertex */
        for ( i = 0; i < ia[24]; i++) {
            for ( j = 0; j < data->sofas3.vlist[i].num_lists; j++) {
                *ptr_i = data->sofas3.vlist[i].lists[j].num_ints - w_ia;
                w_ia = *ptr_i;
                ptr_i++;
                ix++;
            }
        }
        ia[1] = ix; /* end number end indices for vertex */
        ia[2] = ix + 1; /* start number vertex indices */
        for ( i = 0; i < ia[24]; i++) {
            for ( j = 0; j < data->sofas3.vlist[i].num_lists; j++) {
                for ( k = 0; k < data->sofas3.vlist[i].lists[j].num_ints; k++) {
                    *ptr_i = data->sofas3.vlist[i].lists[j].ints[k];
                    ptr_i++;
                    ix++;
                }
            }
        }
        ia[3] = ix + 1; /* end number vertex indices */
        switch ( ia[21]) { 
        case PNOE:
            break;
        case PEVIS:
            w_ia = 0;
            ia[4] = ix + 1; /* start number end indices for edge */
            for ( i = 0; i < ia[24]; i++) {
                for ( j = 0; j < data->sofas3.edata[i].num_lists; j++) {
                    *ptr_i = data->sofas3.edata[i].edgelist[j].num_edges - w_ia;
                    w_ia = *ptr_i;
                    ptr_i++;
                    ix++;
                }
            }
            ia[5] = ix; /* end number end indices for edge */
            ia[8] = ix + 1; /* start number edge indices */
            for ( i = 0; i < ia[24]; i++) {
                for ( j = 0; j < data->sofas3.edata[i].num_lists; j++) {
                    for ( k = 0;
                        k < data->sofas3.edata[i].edgelist[j].num_edges; k++) {
                        *ptr_i =
                            data->sofas3.edata[i].edgelist[j].edgedata.edges[k];
                        ptr_i++;
                        ix++;
                    }
                }
            }
            ia[9] = ix + 1; /* end number edge indices */
        }

        ptr_f1 = ra;
        ptr_f2 = ptr_f1 + ia[25];
        ptr_f3 = ptr_f2 + ia[25];
        switch ( ia[22]) {
        case PCOORD:
            for ( i = 0; i < ia[25]; i++) {
                *ptr_f1 = data->sofas3.vdata.vertex_data.points[i].x;
                *ptr_f2 = data->sofas3.vdata.vertex_data.points[i].y;
                *ptr_f3 = data->sofas3.vdata.vertex_data.points[i].z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            break;
        case PCCOLR:
            for ( i = 0; i < ia[25]; i++) {
                *ptr_f1 = data->sofas3.vdata.vertex_data.ptcolrs[i].point.x;
                *ptr_f2 = data->sofas3.vdata.vertex_data.ptcolrs[i].point.y;
                *ptr_f3 = data->sofas3.vdata.vertex_data.ptcolrs[i].point.z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            break;
        case PCNORM:
            for ( i = 0; i < ia[25]; i++) {
                *ptr_f1 = data->sofas3.vdata.vertex_data.ptnorms[i].point.x;
                *ptr_f2 = data->sofas3.vdata.vertex_data.ptnorms[i].point.y;
                *ptr_f3 = data->sofas3.vdata.vertex_data.ptnorms[i].point.z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            break;
        case PCCONO:
            for ( i = 0; i < ia[25]; i++) {
                *ptr_f1 = data->sofas3.vdata.vertex_data.ptconorms[i].point.x;
                *ptr_f2 = data->sofas3.vdata.vertex_data.ptconorms[i].point.y;
                *ptr_f3 = data->sofas3.vdata.vertex_data.ptconorms[i].point.z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            break;
        }
        fx = ia[25] * 3; /* float table index */
        ptr_f = ptr_f3;

        switch ( ia[20]) {
        case PNOF:
            break;
        case PFCOLR:
            switch ( ia[23]) {
            case PINDCT:
                ia[6] = ix + 1; /* start number facet colour indices */
                for ( i = 0; i < ia[24]; i++) {
                    *ptr_i = data->sofas3.fdata.colrs[i].ind;
                    ptr_i++;
                    ix++;
                }
                ia[7] = ix; /* end number facet colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[12] = fx + 1; /* start number facet colour components */
                for ( i = 0; i < ia[24]; i++) {
                    *ptr_f = data->sofas3.fdata.colrs[i].direct.rgb.red;
                    *(ptr_f + 1) = data->sofas3.fdata.colrs[i].direct.rgb.green;
                    *(ptr_f + 2) = data->sofas3.fdata.colrs[i].direct.rgb.blue;
                    ptr_f = ptr_f + 3;
                }
                fx += ia[24] * 3;
                ia[13] = fx; /* end number facet colour components */
            }
            break;
        case PFNORM:
            ia[14] = fx + 1; /* start number facet normal data */
            ptr_f1 = ptr_f;
            ptr_f2 = ptr_f1 + ia[24];
            ptr_f3 = ptr_f2 + ia[24];
            for ( i = 0; i < ia[24]; i++) {
                *ptr_f1 = data->sofas3.fdata.norms[i].delta_x;
                *ptr_f2 = data->sofas3.fdata.norms[i].delta_y;
                *ptr_f3 = data->sofas3.fdata.norms[i].delta_z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            ptr_f = ptr_f3;
            fx += ia[24] * 3;
            ia[15] = fx; /* end number facet normal data */
            break;
        case PFCONO:
            ia[14] = fx + 1; /* start number facet normal data */
            for ( i = 0; i < ia[24]; i++) {
                *ptr_f = data->sofas3.fdata.conorms[i].norm.delta_x;
                *(ptr_f + 1)= data->sofas3.fdata.conorms[i].norm.delta_y;
                *(ptr_f + 2) = data->sofas3.fdata.conorms[i].norm.delta_z;
                ptr_f = ptr_f + 3;
            }
            fx += ia[24] * 3;
            ia[15] = fx; /* end number facet normal data */

            switch ( ia[23]) {
            case PINDCT:
                ia[6] = ix + 1; /* start number facet colour indices */
                for ( i = 0; i < ia[24]; i++) {
                    *ptr_i = data->sofas3.fdata.conorms[i].colr.ind;
                    ptr_i++;
                    ix++;
                }
                ia[7] = ix; /* end number facet colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[12] = fx + 1; /* start number facet colour components */
                for ( i = 0; i < ia[24]; i++) {
                    *ptr_f = data->sofas3.fdata.conorms[i].colr.direct.rgb.red;
                    *(ptr_f + 1) = data->sofas3.fdata.conorms[i].colr.direct.rgb.green;
                    *(ptr_f + 2) = data->sofas3.fdata.conorms[i].colr.direct.rgb.blue;
                    ptr_f = ptr_f + 3;
                }
                fx += ia[24] * 3;
                ia[13] = fx; /* end number facet colour components */
            }
            break;
        }

        switch ( ia[22]) {
        case PCOORD:
            break;
        case PCCOLR:
            switch ( ia[23]) {
            case PINDCT:
                ia[10] = ix + 1; /* start number vertex colour indices */
                for ( i = 0; i < ia[25]; i++) {
                    *ptr_i = data->sofas3.vdata.vertex_data.ptcolrs[i].colr.ind;
                    ptr_i++;
                    ix++;
                }
                ia[11] = ix; /* end number vertex colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[16] = fx + 1; /* start number vertex colour components */
                for ( i = 0; i < ia[25]; i++) {
                    *ptr_f = data->sofas3.vdata.vertex_data.ptcolrs[i].
                        colr.direct.rgb.red;
                    *(ptr_f + 1) = data->sofas3.vdata.vertex_data.ptcolrs[i].colr.direct.rgb.green;
                    *(ptr_f + 2) = data->sofas3.vdata.vertex_data.ptcolrs[i].colr.direct.rgb.blue;
                    ptr_f = ptr_f + 3;
                }
                fx += ia[25] * 3;
                ia[17] = fx; /* end number vertex colour components */
            }
            break;
        case PCNORM:
            ia[18] = fx + 1; /* start number vertex nomal data */
            ptr_f1 = ptr_f;
            ptr_f2 = ptr_f1 + ia[25];
            ptr_f3 = ptr_f2 + ia[25];
            for ( i = 0; i < ia[25]; i++) {
                *ptr_f1 = data->sofas3.vdata.vertex_data.ptnorms[i].
                    norm.delta_x;
                *ptr_f2 = data->sofas3.vdata.vertex_data.ptnorms[i].
                    norm.delta_y;
                *ptr_f3 = data->sofas3.vdata.vertex_data.ptnorms[i].
                    norm.delta_z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            ptr_f = ptr_f3;
            fx += ia[25] * 3;
            ia[19] = fx; /* end number vertex nomal data */
            break;
        case PCCONO:
            switch ( ia[23]) {
            case PINDCT:
                ia[10] = ix + 1; /* start number vertex colour indices */
                for ( i = 0; i < ia[25]; i++) {
                    *ptr_i = data->sofas3.vdata.vertex_data.ptconorms[i].
                        colr.ind;
                    ptr_i++;
                    ix++;
                }
                ia[11] = ix; /* end number vertex colour indices */
                break;
            case PRGB: case PCIE: case PHSV: case PHLS:
                ia[16] = fx + 1; /* start number vertex colour components */
                for ( i = 0; i < ia[25]; i++) {
                    *ptr_f = data->sofas3.vdata.vertex_data.ptconorms[i].colr.direct.rgb.red;
                    *(ptr_f + 1) = data->sofas3.vdata.vertex_data.ptconorms[i].colr.direct.rgb.green;
                    *(ptr_f + 2) = data->sofas3.vdata.vertex_data.ptconorms[i].colr.direct.rgb.blue;
                    ptr_f = ptr_f + 3;
                }
                fx += ia[25] * 3;
                ia[17] = fx; /* end number vertex colour components */
            }
            ia[18] = fx + 1; /* start number vertex nomal data */
            ptr_f1 = ptr_f;
            ptr_f2 = ptr_f1 + ia[25];
            ptr_f3 = ptr_f2 + ia[25];
            for ( i = 0; i < ia[25]; i++) {
                *ptr_f1 = data->sofas3.vdata.vertex_data.ptconorms[i].
                    norm.delta_x;
                *ptr_f2 = data->sofas3.vdata.vertex_data.ptconorms[i].
                    norm.delta_y;
                *ptr_f3 = data->sofas3.vdata.vertex_data.ptconorms[i].
                    norm.delta_z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            ptr_f = ptr_f3;
            fx += ia[25] * 3;
            ia[19] = fx + 1; /* end number vertex nomal data */
        }
        break;

    case PENBSC:
        ia[0] = data->nurb_curve.order;
        ia[1] = data->nurb_curve.knots.num_floats;
        ia[2] = data->nurb_curve.rationality ? PNONRA: PRATIO;
        ia[3] = data->nurb_curve.cpts.num_points;
        bcopy ((char *)data->nurb_curve.knots.floats, (char *)ra,
           ( sizeof(Pfloat) * ia[1]));
        ptr_f = ra + ia[1];
        ptr_f[0] = data->nurb_curve.min;
        ptr_f[1] = data->nurb_curve.max;
        ptr_f += 2;
        ptr_f1 = ptr_f;
        ptr_f2 = ptr_f1 + ia[3];
        ptr_f3 = ptr_f2 + ia[3];
        if ( ia[2] == PNONRA) {
            for ( i = 0; i < ia[3]; i ++) {
                *ptr_f1 = data->nurb_curve.cpts.points.point3d[i].x;
                *ptr_f2 = data->nurb_curve.cpts.points.point3d[i].y;
                *ptr_f3 = data->nurb_curve.cpts.points.point3d[i].x;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
        } else if ( ia[2] == PRATIO) {
            ptr_f4 = ptr_f3 + ia[3];
            for ( i = 0; i < ia[3]; i ++) {
                *ptr_f1 = data->nurb_curve.cpts.points.point4d[i].x;
                *ptr_f2 = data->nurb_curve.cpts.points.point4d[i].y;
                *ptr_f3 = data->nurb_curve.cpts.points.point4d[i].x;
                *ptr_f4 = data->nurb_curve.cpts.points.point4d[i].z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
                ptr_f4++;
            }
        }
        break;

    case PENBSS:
        ia[0] = 0; /* start number u knots */
        ia[1] = 0; /* end number u knots */
        ia[2] = 0; /* start number v knots */
        ia[3] = 0; /* end number v knots */
        ia[4] = 0; /* start number control points X */
        ia[5] = 0; /* end number control points X */
        ia[6] = 0; /* start number control points Y */
        ia[7] = 0; /* end number control points Y */
        ia[8] = 0; /* start number control points Z */
        ia[9] = 0; /* end number control points Z */
        ia[10] = 0; /* start number control points W */
        ia[11] = 0; /* end number control points W */
        ia[12] = 0; /* start number curve approx type */
        ia[13] = 0; /* end number curve approxr type */
        ia[14] = 0; /* start number curve approx value */
        ia[15] = 0; /* end number curve approx value */
        ia[16] = 0; /* start number curve visibility */
        ia[17] = 0; /* end number curve visibility */
        ia[18] = 0; /* start number curve order */
        ia[19] = 0; /* end number curve order */
        ia[20] = 0; /* start number curve knots array */
        ia[21] = 0; /* end number curve knots array */
        ia[22] = 0; /* start number curve knots vecter */
        ia[23] = 0; /* end number curve knots vecter */
        ia[24] = 0; /* start number curve rational type */
        ia[25] = 0; /* end number curve rational type */
        ia[26] = 0; /* start number curve number of control points */
        ia[27] = 0; /* end number curve number of control points */
        ia[28] = 0; /* start number curve control points X */
        ia[29] = 0; /* end number curve control points X */
        ia[30] = 0; /* start number curve control points Y */
        ia[31] = 0; /* end number curve control points Y */
        ia[32] = 0; /* start number curve control points W */
        ia[33] = 0; /* end number curve control points W */
        ia[34] = 0; /* start number curve range limits */
        ia[35] = 0; /* end number curve range limits */
        ia[36] = data->nurb_surf.u_order;
        ia[37] = data->nurb_surf.v_order;
        ia[38] = data->nurb_surf.rationality ? PNONRA: PRATIO;
        ia[39] = data->nurb_surf.uknots.num_floats;
        ia[40] = data->nurb_surf.vknots.num_floats;
        ia[41] = data->nurb_surf.grid.num_points.u_dim;
        ia[42] = data->nurb_surf.grid.num_points.v_dim;
        ia[43] = data->nurb_surf.num_trim_loops;
        ix = 44;
        ptr_i = &(ia[44]);
        w_ia = 0;
        for ( i = 0; i < ia[43]; i++) {
            *ptr_i = data->nurb_surf.trim_loops[i].num_curves - w_ia;
            w_ia = *ptr_i;
            ptr_i++;
            ix++;
        }
        ia[12] = ix + 1; /* start number curve approx type */
        for ( i = 0; i < ia[43]; i++) {
            for ( j = 0; j < data->nurb_surf.trim_loops[i].num_curves; j++) {
                *ptr_i = data->nurb_surf.trim_loops[i].curves[j].approx_type;
                ptr_i++;
                ix++;
            }
        }
        ia[13] = ix; /* end number curve approx type */
        ia[14] = ix + 1; /* start number curve approx value */
        for ( i = 0; i < ia[43]; i++) {
            for ( j = 0; j < data->nurb_surf.trim_loops[i].num_curves; j++) {
                *ptr_i = data->nurb_surf.trim_loops[i].curves[j].approx_val;
                ptr_i++;
                ix++;
            }
        }
        ia[15] = ix; /* end number curve approx velue */
        ia[16] = ix + 1; /* start number curve visibility */
        for ( i = 0; i < ia[43]; i++) {
            for ( j = 0; j < data->nurb_surf.trim_loops[i].num_curves; j++) {
                *ptr_i = data->nurb_surf.trim_loops[i].curves[j].visible;
                ptr_i++;
                ix++;
            }
        }
        ia[17] = ix; /* end number curve visibility */
        ia[18] = ix + 1; /* start number curve order */
        for ( i = 0; i < ia[43]; i++) {
            for ( j = 0; j < data->nurb_surf.trim_loops[i].num_curves; j++) {
                *ptr_i = data->nurb_surf.trim_loops[i].curves[j].order;
                ptr_i++;
                ix++;
            }
        }
        ia[19] = ix; /* end number curve order */
        ia[20] = ix + 1; /* start number curve knots array */
        for ( i = 0; i < ia[43]; i++) {
            for ( j = 0; j < data->nurb_surf.trim_loops[i].num_curves; j++) {
                *ptr_i =
                    data->nurb_surf.trim_loops[i].curves[j].knots.num_floats;
                ptr_i++;
                ix++;
            }
        }
        ia[21] = ix; /* end number curve knots array */
        ia[24] = ix + 1; /* start number curve rational type */
        for ( i = 0; i < ia[43]; i++) {
            for ( j = 0; j < data->nurb_surf.trim_loops[i].num_curves; j++) {
                *ptr_i = data->nurb_surf.trim_loops[i].curves[j].rationality ?
                    PNONRA: PRATIO;
                ptr_i++;
                ix++;
            }
        }
        ia[25] = ix; /* end number curve rational type */
        work2 = 0;
        ia[26] = ix + 1; /* start number curve number of control points */
        for ( i = 0; i < ia[43]; i++) {
            for ( j = 0; j < data->nurb_surf.trim_loops[i].num_curves; j++) {
                *ptr_i =
                    data->nurb_surf.trim_loops[i].curves[j].cpts.num_points;
                ptr_i++;
                ix++;
                work2++;
            }
        }
        ia[27] = ix; /* end number curve number of control points */

        fx = 0;
        ptr_f = ra;
        ia[0] = fx + 1; /* start number u knots */
        for ( i = 0; i < ia[38]; i++) {
            *ptr_f = data->nurb_surf.uknots.floats[i];
            ptr_f++;
            fx++;
        }
        ia[1] = fx; /* end number u knots */
        ia[2] = fx + 1; /* start number v knots */
        for ( i = 0; i < ia[39]; i++) {
            *ptr_f = data->nurb_surf.vknots.floats[i];
            ptr_f++;
            fx++;
        }
        ia[3] = fx; /* end number v knots */
        work1 = ia[41] * ia[42];
        ia[4] = fx + 1; /* start number control points X */
        ia[6] = fx + work1 + 1; /* start number control points Y */
        ia[8] = fx + (work1 * 2) + 1; /* start number control points Z */
        ptr_f1 = ptr_f;
        ptr_f2 = ptr_f1 + work1;
        ptr_f3 = ptr_f2 + work1;
        if ( ia[40] == PRATIO) { 
            ptr_f4 = ptr_f3 + work1;
            ia[10] = fx + (work1 * 3) + 1; /* start number control points W */
            for ( i = 0; i < work1; i++) {
                *ptr_f1 = data->nurb_surf.grid.points.point4d[i].x;
                *ptr_f2 = data->nurb_surf.grid.points.point4d[i].y;
                *ptr_f3 = data->nurb_surf.grid.points.point4d[i].z;
                *ptr_f4 = data->nurb_surf.grid.points.point4d[i].w;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
                ptr_f4++;
            }
            ptr_f = ptr_f4;
            ia[5] = (fx += work1); /* end number control points X */
            ia[7] = (fx += work1); /* end number control points Y */
            ia[9] = (fx += work1); /* end number control points Z */
            ia[11] = (fx += work1); /* end number control points W */
        } else { /* PNONRA */
            for ( i = 0; i < work1; i++) {
                *ptr_f1 = data->nurb_surf.grid.points.point3d[i].x;
                *ptr_f2 = data->nurb_surf.grid.points.point3d[i].y;
                *ptr_f3 = data->nurb_surf.grid.points.point3d[i].z;
                ptr_f1++;
                ptr_f2++;
                ptr_f3++;
            }
            ptr_f = ptr_f3;
            ia[5] = (fx += work1); /* end number control points X */
            ia[7] = (fx += work1); /* end number control points Y */
            ia[9] = (fx += work1); /* end number control points Z */
        }
        ia[22] = fx + 1; /* start number curve knots vecter */
        for ( i = 0; i < ia[45]; i++) {  
            for ( j = 0; j < data->nurb_surf.trim_loops[i].num_curves; j++) {
                for ( k = 0; k < data->nurb_surf.trim_loops[i].curves[j].knots.
                    num_floats; k++) {
                    *ptr_f =
                        data->nurb_surf.trim_loops[i].curves[j].knots.floats[k];
                    ptr_f++;
                    fx++;
                }
            }
        }
        ia[23] = fx; /* end number curve knots vecter */
        ia[28] = fx + 1; /* start number curve control points X */
        ia[30] = fx + work2 + 1; /* start number curve control points Y */
        ia[32] = fx + (work2 * 2) + 1; /* start number curve control points W */
        ptr_f1 = ptr_f;
        ptr_f2 = ptr_f1 + work2;
        ptr_f3 = ptr_f2 + work2;
        for ( i = 0; i < ia[45]; i++) {  
            for ( j = 0; j < data->nurb_surf.trim_loops[i].num_curves; j++) {
                if ( data->nurb_surf.trim_loops[i].curves[j].rationality ==
                    PRATIO) { 
                    for ( k = 0; k < data->nurb_surf.trim_loops[i].curves[j].
                        cpts.num_points; k++) {
                        *ptr_f1 = data->nurb_surf.trim_loops[i].curves[j].cpts.
                            points.point3d[k].x;
                        *ptr_f2 = data->nurb_surf.trim_loops[i].curves[j].cpts.
                            points.point3d[k].y;
                        *ptr_f3 = data->nurb_surf.trim_loops[i].curves[j].cpts.
                            points.point3d[k].z;
                        ptr_f1++;
                        ptr_f2++;
                        ptr_f3++;
                    }
                } else { /* PNONRA */
                    for ( k = 0; k < data->nurb_surf.trim_loops[i].curves[j].
                        cpts.num_points; k++) {
                        *ptr_f1 = data->nurb_surf.trim_loops[i].curves[j].cpts.
                            points.point2d[k].x;
                        *ptr_f2 = data->nurb_surf.trim_loops[i].curves[j].cpts.
                            points.point2d[k].y;
                        *ptr_f3 = 1; /* no used */
                        ptr_f1++;
                        ptr_f2++;
                        ptr_f3++;
                    }
                }
            }
        }
        ptr_f = ptr_f3;
        ia[29] = (fx += work2); /* end number curve control points X */
        ia[31] = (fx += work2); /* end number curve control points Y */
        ia[33] = (fx += work2); /* end number curve control points W */
        ia[34] = fx + 1; /* start number curve range limits */
        for ( i = 0; i < ia[45]; i++) {  
            for ( j = 0; j < data->nurb_surf.trim_loops[i].num_curves; j++) {
                ptr_f[0] = data->nurb_surf.trim_loops[i].curves[j].tmin;
                ptr_f[1] = data->nurb_surf.trim_loops[i].curves[j].tmax;
                ptr_f +=2;
                fx +=2;
            }
        }
        ia[35] = fx; /* end number curve range limits */
        break;
            
    case PECAP3:
        ia[0] = data->cell_array_plus.colr_array.dims.size_x;
        ia[1] = data->cell_array_plus.colr_array.dims.size_y;
        ia[2] = data->cell_array_plus.colr_array.type;
        ra[0] = data->cell_array_plus.paral.p.x;
        ra[1] = data->cell_array_plus.paral.q.x;
        ra[2] = data->cell_array_plus.paral.r.x;
        ra[3] = data->cell_array_plus.paral.p.y;
        ra[4] = data->cell_array_plus.paral.q.y;
        ra[5] = data->cell_array_plus.paral.r.y;
        ra[6] = data->cell_array_plus.paral.p.z;
        ra[7] = data->cell_array_plus.paral.q.z;
        ra[8] = data->cell_array_plus.paral.r.z;
        switch ( ia[2]) {
        case PINDCT:
            k = 0;
            ptr_i = &(ia[3]);
            for ( i = 0; i < ia[0]; i++) {
                for ( j = 0; j < ia[1]; j++) {
                    *ptr_i = data->cell_array_plus.colr_array.colr_array[k].ind;
                    ptr_i++;
                    k++;
                }
            }
            break;
        case PRGB: case PCIE: case PHSV: case PHLS:
            k = 0;
            ptr_f = &(ra[9]);
            for ( i = 0; i < ia[0]; i++) {
                for ( j = 0; j < ia[1]; j++) {
                    *ptr_f = data->cell_array_plus.colr_array.colr_array[k].direct.rgb.red;
                    *(ptr_f + 1) = data->cell_array_plus.colr_array.colr_array[k].direct.rgb.green;
                    *(ptr_f + 2) = data->cell_array_plus.colr_array.colr_array[k].direct.rgb.blue;
                    ptr_f = ptr_f + 3;
                }
            }
        }
        break;
    case PETXCL:
    case PEPMCL:
    case PEEDCL:
    case PEPLCL:
    case PEICL:
    case PEBICL:
        ia[0] = data->colr.type;
        switch( data->colr.type) {
        case PINDCT:
            ia[1] = data->colr.val.ind;
            break;
        case PRGB: case PCIE: case PHSV: case PHLS:
            ra[0] = data->colr.val.general.x;
            ra[1] = data->colr.val.general.y;
            ra[2] = data->colr.val.general.z;
            break;
        }
        break;
    case PECAPC:
        ia[0] = data->curv_approx.type;
        ra[0] = data->curv_approx.value;
        break;
    case PEPLSM:
    case PEBISY:
    case PEBISI:
    case PEISM:
    case PEBISM:
    case PEIRFE:
    case PEBIRE:
    case PEDPCI:
    case PECMI:
    case PERCLM:
        ia[0] = data->int_data;
        break;
    case PEFDGM:
        ia[0] = (Pint)data->disting_mode;
        break;
    case PEFCUM:
        ia[0] = (Pint)data->cull_mode;
        break;
    case PERFP:
    case PEBRFP:
        ra[0] = data->props.ambient_coef;
        ra[1] = data->props.diffuse_coef;
        ra[2] = data->props.specular_coef;
        ra[3] = data->props.specular_exp;
        ia[0] = data->props.specular_colr.type;
        switch( data->props.specular_colr.type) {
        case PINDCT:
            ia[1] = data->props.specular_colr.val.ind;
            break;
        case PRGB: case PCIE: case PHSV: case PHLS:    
            ra[4] = data->props.specular_colr.val.general.x;
            ra[5] = data->props.specular_colr.val.general.y;
            ra[6] = data->props.specular_colr.val.general.z;
            break;
        }
        break;
    case PESAPC:
        ia[0] = data->surf_approx.type;
        ra[0] = data->surf_approx.u_val;
        ra[1] = data->surf_approx.v_val;
        break;
    case PEPSCH:
        ptr_i = (Pint *)str;
        for ( i = 0; i < sl; i++) {
            lstr[i] = 80;
        }
        ia[0] = data->para_surf_characs.type;
        switch ( ia[0]) {
        case PSC_NONE:
        case PSC_WS_DEP:
            *ptr_i = 0;
            break;
        case PSC_ISOPARAMETRIC_CURVES:
            *ptr_i = sizeof(Pint) * 4;
            *(ptr_i++) = 3;
            *(ptr_i++) = (Pint)data->para_surf_characs.data.psc_3.placement;
            *(ptr_i++) = data->para_surf_characs.data.psc_3.u_count;
            *(ptr_i++) = data->para_surf_characs.data.psc_3.v_count;
            break;
        case PSC_LEVEL_CURVES_MC:
            *ptr_i = sizeof(Pint) * 3 + sizeof(Pfloat) *
                (6 + data->para_surf_characs.data.psc_4.params.num_floats);
            *(ptr_i++) = 1;
            *(ptr_i++) = data->para_surf_characs.data.psc_4.params.num_floats;
            *(ptr_i++) =
                6 + data->para_surf_characs.data.psc_4.params.num_floats;
            ptr_f = (Pfloat *)ptr_i;
            *(ptr_f++) = data->para_surf_characs.data.psc_4.origin.x;
            *(ptr_f++) = data->para_surf_characs.data.psc_4.origin.y;
            *(ptr_f++) = data->para_surf_characs.data.psc_4.origin.z;
            *(ptr_f++) = data->para_surf_characs.data.psc_4.direction.delta_x;
            *(ptr_f++) = data->para_surf_characs.data.psc_4.direction.delta_y;
            *(ptr_f++) = data->para_surf_characs.data.psc_4.direction.delta_z;
            for ( i = 0; i <
                data->para_surf_characs.data.psc_4.params.num_floats; i++) {
                *(ptr_f++) =data->para_surf_characs.data.psc_4.params.floats[i];
            }
            break;
        case PSC_LEVEL_CURVES_WC:
            *ptr_i = sizeof(Pint) * 3 + sizeof(Pfloat) *
                (6 + data->para_surf_characs.data.psc_5.params.num_floats);
            *(ptr_i++) = 1;
            *(ptr_i++) = data->para_surf_characs.data.psc_5.params.num_floats;
            *(ptr_i++) =
                6 + data->para_surf_characs.data.psc_5.params.num_floats;
            ptr_f = (Pfloat *)ptr_i;
            *(ptr_f++) = data->para_surf_characs.data.psc_5.origin.x;
            *(ptr_f++) = data->para_surf_characs.data.psc_5.origin.y;
            *(ptr_f++) = data->para_surf_characs.data.psc_5.origin.z;
            *(ptr_f++) = data->para_surf_characs.data.psc_5.direction.delta_x;
            *(ptr_f++) = data->para_surf_characs.data.psc_5.direction.delta_y;
            *(ptr_f++) = data->para_surf_characs.data.psc_5.direction.delta_z;
            for ( i = 0; i <
                data->para_surf_characs.data.psc_5.params.num_floats; i++) {
                *(ptr_f++) =data->para_surf_characs.data.psc_5.params.floats[i];
            }
            break;
        } 
        break;
    case PELSST:
        ia[0] = data->lss.activation.num_ints;
        for( i = 0, k = 1; i < data->lss.activation.num_ints; i++, k++) {
            ia[k] = data->lss.activation.ints[i];
        }
        ia[k] = data->lss.deactivation.num_ints;
        k++;
        for( i = 0; i < data->lss.deactivation.num_ints; i++, k++) {
            ia[k] = data->lss.deactivation.ints[i];
        }
        break;
    }
}


/* INQUIRE CURRENT ELEMENT CONTENT */
void
pqceco_( iil, irl, isl, errind, il, ia, rl, ra, sl, lstr, str)
Pint	*iil;		/* dimension of integer array	*/
Pint	*irl;		/* dimension of real array	*/
Pint	*isl;		/* dimension of charcter array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*il;		/* OUT number of integer entries	*/
Pint	*ia;		/* OUT array containing integer entries	*/
Pint	*rl;		/* OUT number of real entries	*/
Pfloat	*ra;		/* OUT array containing real entries	*/
Pint	*sl;		/* OUT number of charcter entrie	*/
Pint	*lstr;		/* OUT length of each character entrie	*/
char	*str;		/* OUT character entrie	*/
{
    Phg_args				cp_args;
    register Phg_args_q_el_data		*args = &cp_args.data.q_el_data;
    Phg_ret	ret;

    Pint	eltype;
    Pstore      store;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR5;

    } else if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
        *errind = ERR5;

    } else {
        *errind = 0;
        args->el_id = -1;	/* signal to use the current element */
        pcreate_store( errind, &store);
        if ( !*errind) {
            fb_elem_type_size( &cp_args, store, &ret, errind, &eltype, il, rl,
                sl);
            if ( !*errind) {
                if ( *iil < *il || *irl < *rl || *isl < *sl) {
                    *errind = ERR2001;

                } else {
                    fb_elem_content( store, eltype, *il, *rl, *sl, errind, ia,
                        ra, lstr, str);
                }
            }
            pdel_store( store);
        }
    }
}


/* INQUIRE ELEMENT CONTENT */
void
pqeco_( strid, elenum, iil, irl, isl, errind, il, ia, rl, ra, sl, lstr, str)
Pint	*strid;		/* structure identifier	*/
Pint	*elenum;	/* element number	*/
Pint	*iil;		/* dimension of integer array	*/
Pint	*irl;		/* dimension of real array	*/
Pint	*isl;		/* dimension of charcter array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*il;		/* OUT number of integer entries	*/
Pint	*ia;		/* OUT array containing integer entries	*/
Pint	*rl;		/* OUT number of real entries	*/
Pfloat	*ra;		/* OUT array containing real entries	*/
Pint	*sl;		/* OUT number of charcter entrie	*/
Pint	*lstr;		/* OUT length of each character entrie	*/
char	*str;		/* OUT character entrie	*/
{
    Phg_args				cp_args;
    register Phg_args_q_el_data		*args = &cp_args.data.q_el_data;
    Phg_ret	ret;

    Pint	eltype;
    Pstore      store;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if ( *elenum < 0) {
        *errind = ERR202;

    } else {
        *errind = 0;
        args->struct_id = *strid;
        args->el_id = *elenum;
        pcreate_store( errind, &store);
        if ( !*errind) {
            fb_elem_type_size( &cp_args, store, &ret, errind, &eltype, il, rl,
                sl);
            if ( !*errind) {
                if ( *iil < *il || *irl < *rl || *isl < *sl) {
                    *errind = ERR2001;

                } else {
                    fb_elem_content( store, eltype, *il, *rl, *sl, errind, ia,
                        ra, lstr, str);
                }
            }
            pdel_store( store);
        }
    }
}


/* INQUIRE STRUCTURE IDENTIFIERS */
void
pqsid_( n, errind, number, strid)
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*number;	/* OUT number of structure identifiers	*/
Pint	*strid;		/* OUT Nth structure identifiers	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else {
	ret.err = 0;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_STRUCT_IDS, &cp_args, &ret);
	if ( ret.err) {
	    *errind = ret.err;
	} else {
	    *errind = 0;
	    *number = ret.data.int_list.num_ints;
	    if ( *number > 0) {
		if ( *n < 0 || *n > *number) {
		    *errind = ERR2002;
                } else if ( *n > 0) {
		    *strid = ret.data.int_list.ints[*n - 1];
                }
	    }
	}
    }
}


/* INQUIRE PATHS TO ANCESTORS */
void
pqpan_( strid, pthord, pthdep, ipthsz, n, errind, ol, apthsz, paths)
Pint	*strid;		/* structure identifier	*/
Pint	*pthord;	/* path order	*/
Pint	*pthdep;	/* path depth	*/
Pint	*ipthsz;	/* size of path buffer	*/
Pint	*n;		/* element of the list paths	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of paths available	*/
Pint	*apthsz;	/* OUT size of the Nth structure path	*/
Pint	*paths;		/* OUT the Nth structure path	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if ( *pthdep < 0) {
	*errind = ERR207;

    } else if ( *pthord < PPOTOP || *pthord > PPOBOT) {
	*errind = ERR2000;

    } else {
	cp_args.data.q_hierarchy.dir = PHG_ARGS_HIER_ANCESTORS;
	cp_args.data.q_hierarchy.struct_id = *strid;
	cp_args.data.q_hierarchy.order = (Ppath_order)*pthord;
	cp_args.data.q_hierarchy.depth = *pthdep;
	ret.err = 0;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_HIERARCHY, &cp_args, &ret);
	if ( !(*errind = ret.err) ) {
	    phg_fb_copy_hierarchy( &ret.data.hierarchy, *ipthsz, *n, errind,
		ol, apthsz, paths );
	}
    }
}


/* INQUIRE PATHS TO DESCENDANTS */
void
pqpde_( strid, pthord, pthdep, ipthsz, n, errind, ol, apthsz, paths)
Pint	*strid;		/* structure identifier	*/
Pint	*pthord;	/* path order	*/
Pint	*pthdep;	/* path depth	*/
Pint	*ipthsz;	/* size of path buffer	*/
Pint	*n;		/* element of the list paths	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of paths available	*/
Pint	*apthsz;	/* OUT size of the Nth structure path	*/
Pint	*paths;		/* OUT the Nth structure path	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if ( *pthdep < 0) {
	*errind = ERR207;

    } else if ( *pthord < PPOTOP || *pthord > PPOBOT) {
	*errind = ERR2000;

    } else {
	cp_args.data.q_hierarchy.dir = PHG_ARGS_HIER_DESCENDANTS;
	cp_args.data.q_hierarchy.struct_id = *strid;
	cp_args.data.q_hierarchy.order = (Ppath_order)*pthord;
	cp_args.data.q_hierarchy.depth = *pthdep;
	ret.err = 0;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_HIERARCHY, &cp_args, &ret);
	if ( !(*errind = ret.err) ) {
	    phg_fb_copy_hierarchy( &ret.data.hierarchy, *ipthsz, *n, errind,
		ol, apthsz, paths );
	}
    }
}


/* INQUIRE SET member OF WORKSTATIONS TO WHICH POSTED */
void
pqwkpo_( strid, n, errind, ol, wkid)
Pint   	*strid;		/* structure identifier	*/
Pint   	*n;		/* set number requested	*/
Pint   	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of workstation	*/
Pint	*wkid;		/* OUT Nth element of workstation	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else {
	cp_args.data.idata = *strid;
	ret.err = 0;
        CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_WSS_POSTED_TO, &cp_args, &ret);
        if ( ret.err) {
	    *errind = ret.err;
	} else {
	    *errind = 0;
	    *ol = ret.data.int_list.num_ints;
	    if ( *ol > 0) {
		if ( *n < 0 || *n > *ol) {
		    *errind = ERR2002;
                } else if ( *n > 0) {
		    *wkid = ret.data.int_list.ints[*n - 1];
		}
            }
	}
    }
}


/* ELEMENT SEARCH */
void
pels_( strid, strtep, srcdir, eisn, eis, eesn, ees, errind, status, fndep)
Pint	*strid;		/* structure identifier	*/
Pint	*strtep;	/* start element position	*/
Pint	*srcdir;	/* search direction	*/
Pint	*eisn;		/* number of elements in inclusion set	*/
Pint	*eis;		/* element inclusion set	*/
Pint	*eesn;		/* number of elements in exclusion set	*/
Pint	*ees;		/* element exclusion set	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*status;	/* OUT status indicator	*/
Pint	*fndep;		/* OUT found element position	*/
{
    Phg_args			cp_args;
    register Phg_args_el_search	*args = &cp_args.data.el_search;
    Phg_ret			ret;
    register int		i;
    Pint				*work = NULL;
    Pelem_type			*incl_set = NULL;
    Pelem_type			*excl_set = NULL;

    if ( ! CB_ENTRY_CHECK(phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else if ( *srcdir < PBWD || *srcdir > PFWD ) {
        *errind = ERR2000;
    
    } else if ( *eisn < 0 || *eesn < 0 ) {
	*errind = ERR2004;

    } else {
        /* chance element type  FORTRAN -> C */
        *errind = 0;
        PMALLOC( *errind, Pint, (*eisn + *eesn), work);
        if ( !*errind) {
            incl_set = (Pelem_type *)work;
            excl_set = incl_set + *eisn;
	    for ( i = 0; i < *eisn; i++) {
                if ( eis[i] >= PEALL && eis[i] <= PERCLM) {
                    incl_set[i] = (Pelem_type)eis[i];

                } else {
                    *errind = ERR2000;
                    PFREE( (*eisn + *eesn), work);
                    return;
                }
            }

            for ( i = 0; i < *eesn; i++) {
                if ( ees[i] >= PEALL && ees[i] <= PERCLM) {
                    excl_set[i] = (Pelem_type)ees[i];

                } else {
                    *errind = ERR2000;
                    PFREE( (*eisn + *eesn), work);
                    return;
                }
            }
	    args->struct_id = *strid;
	    args->start_el = *strtep;
	    args->dir = (Psearch_dir)*srcdir;
	    args->incl.num_elem_types = *eisn;
	    args->incl.elem_types = incl_set;
	    args->excl.num_elem_types = *eesn;
	    args->excl.elem_types = excl_set;
	    ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_EL_SEARCH, &cp_args, &ret);
	    if ( ret.err) {
	        *errind = ret.err;
	    } else {
	        *errind = 0;
	        *status = (Pint)ret.data.el_search.status;
	        *fndep = ret.data.el_search.found_el;
            }
            PFREE( (*eisn + *eesn), work);
	}
    }
}


static void
fb_incr_spa_search( srpx, srpy, srpz, sdist, spthsz, spath, mclipf, srchci,
nfln, nflisx, nflis, nflesx, nfles, ifln, iflisx, iflis, iflesx, ifles, ipthsz,
errind, fpthsz, fpath)
Pfloat	*srpx;		/* search reference point X	*/
Pfloat	*srpy;		/* search reference point Y	*/
Pfloat	*srpz;		/* search reference point Z	*/
Pfloat	*sdist;		/* search distance	*/
Pint	*spthsz;	/* number of elements in starting path	*/
Pint	*spath;		/* starting path	*/
Pint	*mclipf;	/* model clip flag	*/
Pint	*srchci;	/* search ceiling index	*/
Pint	*nfln;		/* number of normal filters	*/
Pint	*nflisx;	/* end indices of normal filter inclusion sets	*/
Pint	*nflis;		/* normal filter inclusion sets	*/
Pint	*nflesx;	/* end indices of normal filter exclusion sets	*/
Pint	*nfles;		/* normal filter exclusion sets	*/
Pint	*ifln;		/* number of inverted filters	*/
Pint	*iflisx;	/* end indices of inverted filter inclusion sets */
Pint	*iflis;		/* inverted filter inclusion sets	*/
Pint	*iflesx;	/* end indices of inverted filter exclusion sets */
Pint	*ifles;		/* inverted filter exclusion sets	*/
Pint	*ipthsz;	/* size of found path array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*fpthsz;	/* OUT found path size	*/
Pint	*fpath;		/* OUT found path	*/
{
    Phg_args            cp_args;
    unsigned            num_names;
    Pint		num_nrm, num_inv;
    Phg_ret             ret;
    Pelem_ref_list	*path = &ret.data.inc_spa_search.path;
    Pelem_ref		*refs;

    register Phg_args_inc_spa_search	*args = &cp_args.data.inc_spa_search;
    register int	i; 
    register int	index; 

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR2;

    } else if ( *mclipf < PNCLIP || *mclipf > PCLIP) {
        *errind = ERR2000;

    } else if ( *spthsz < 0) {
        *errind = ERR2004;

    } else if ( *nfln < 1 || *ifln < 1) {
        *errind = ERR2006;

    } else {
        *errind = 0;
	args->ref_pt.x = *srpx;
	args->ref_pt.y = *srpy;
	args->ref_pt.z = *srpz;
        args->distance = *sdist;
        args->start_path.num_elem_refs = *spthsz;
        PMALLOC( *errind, Pelem_ref, *spthsz, refs);
        if ( !*errind) {
            for ( i = 0; i < *spthsz; i++) {
                index = i * 2;
                refs[i].struct_id = spath[index];
                refs[i].elem_pos = spath[index + 1];
            }
            args->start_path.elem_refs = refs;
            args->mclip_flag = (Pclip_ind)*mclipf;
            args->ceiling = *srchci;

            /* Silently clamp the number of filters to that supported. */
            num_nrm = MIN( *nfln,
                phg_cur_cph->pdt.max_length_normal_iss_filter);
            num_inv = MIN( *ifln,
                phg_cur_cph->pdt.max_length_inverted_iss_filter);
            args->nrm_filt.incl_set.num_ints = nflisx[num_nrm - 1];
            args->nrm_filt.excl_set.num_ints = nflesx[num_nrm - 1];
            args->inv_filt.incl_set.num_ints = iflisx[num_inv - 1];
            args->inv_filt.excl_set.num_ints = iflesx[num_inv - 1];
            num_names = args->nrm_filt.incl_set.num_ints
                + args->nrm_filt.excl_set.num_ints
                + args->inv_filt.incl_set.num_ints
                + args->inv_filt.excl_set.num_ints;
            if ( num_names <= 0) {
                /* Get rid of any empty lists. */
                args->nrm_filt.incl_set.num_ints = 0;
                args->nrm_filt.excl_set.num_ints = 0;
                args->inv_filt.incl_set.num_ints = 0;
                args->inv_filt.excl_set.num_ints = 0;
            } else {
                args->nrm_filt.incl_set.ints = nflis;
                args->nrm_filt.excl_set.ints = nfles;
                args->inv_filt.incl_set.ints = iflis;
                args->inv_filt.excl_set.ints = ifles;
	    }

	        ret.err = 0;
            CP_FUNC( phg_cur_cph, CP_FUNC_OP_INC_SPA_SEARCH, &cp_args, &ret );
            if ( ret.err) {
                *errind = ret.err;
            } else {
                *errind = 0;
                if ( *ipthsz < path->num_elem_refs) {
                    *errind = ERR2001;
                } else {
                    *fpthsz = path->num_elem_refs;
                    for ( i = 0; i < *fpthsz; i++) {
                        index = i * 2;
                        fpath[index] = path->elem_refs[i].struct_id;
                        fpath[index + 1] = path->elem_refs[i].elem_pos;
                    }
                }
            }
            PFREE( *spthsz, refs);
        }
    }
}


/* INCREMENTAL SPATIAL SEARCH 3 */
void
piss3_( srpx, srpy, srpz, sdist, spthsz, spath, mclipf, srchci, nfln, nflisx,
nflis, nflesx, nfles, ifln, iflisx, iflis, iflesx, ifles, ipthsz, errind,
fpthsz, fpath)
Pfloat	*srpx;		/* search reference point X	*/
Pfloat	*srpy;		/* search reference point Y	*/
Pfloat	*srpz;		/* search reference point Z	*/
Pfloat	*sdist;		/* search distance	*/
Pint	*spthsz;	/* number of elements in starting path	*/
Pint	*spath;		/* starting path	*/
Pint	*mclipf;	/* model clip flag	*/
Pint	*srchci;	/* search ceiling index	*/
Pint	*nfln;		/* number of normal filters	*/
Pint	*nflisx;	/* end indices of normal filter inclusion sets	*/
Pint	*nflis;		/* normal filter inclusion sets	*/
Pint	*nflesx;	/* end indices of normal filter exclusion sets	*/
Pint	*nfles;		/* normal filter exclusion sets	*/
Pint	*ifln;		/* number of inverted filters	*/
Pint	*iflisx;	/* end indices of inverted filter inclusion sets */
Pint	*iflis;		/* inverted filter inclusion sets	*/
Pint	*iflesx;	/* end indices of inverted filter exclusion sets */
Pint	*ifles;		/* inverted filter exclusion sets	*/
Pint	*ipthsz;	/* size of found path array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*fpthsz;	/* OUT found path size	*/
Pint	*fpath;		/* OUT found path	*/
{
    fb_incr_spa_search( srpx, srpy, srpz, sdist, spthsz, spath, mclipf, srchci,
        nfln, nflisx, nflis, nflesx, nfles, ifln, iflisx, iflis, iflesx, ifles,
        ipthsz, errind, fpthsz, fpath);
}


/* INCREMENTAL SPATIAL SEARCH */
void
piss_( srpx, srpy, sdist, spthsz, spath, mclipf, srchci, nfln, nflisx, nflis,
nflesx, nfles, ifln, iflisx, iflis, iflesx, ifles, ipthsz, errind, fpthsz,
fpath)
Pfloat	*srpx;		/* search reference point X	*/
Pfloat	*srpy;		/* search reference point Y	*/
Pfloat	*sdist;		/* search distance	*/
Pint	*spthsz;	/* number of elements in starting path	*/
Pint	*spath;		/* starting path	*/
Pint	*mclipf;	/* model clip flag	*/
Pint	*srchci;	/* search ceiling index	*/
Pint	*nfln;		/* number of normal filters	*/
Pint	*nflisx;	/* end indices of normal filter inclusion sets	*/
Pint	*nflis;		/* normal filter inclusion sets	*/
Pint	*nflesx;	/* end indices of normal filter exclusion sets	*/
Pint	*nfles;		/* normal filter exclusion sets	*/
Pint	*ifln;		/* number of inverted filters	*/
Pint	*iflisx;	/* end indices of inverted filter inclusion sets */
Pint	*iflis;		/* inverted filter inclusion sets	*/
Pint	*iflesx;	/* end indices of inverted filter exclusion sets */
Pint	*ifles;		/* inverted filter exclusion sets	*/
Pint	*ipthsz;	/* size of found path array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*fpthsz;	/* OUT found path size	*/
Pint	*fpath;		/* OUT found path	*/
{
    Pfloat	srpz = 0;

    fb_incr_spa_search( srpx, srpy, &srpz, sdist, spthsz, spath, mclipf, srchci,
        nfln, nflisx, nflis, nflesx, nfles, ifln, iflisx, iflis, iflesx, ifles,
        ipthsz, errind, fpthsz, fpath);
}


/* SET COLOUR MAPPING INDEX */
void
pscmi_( cmi)
Pint	*cmi;	/* colour mapping index	*/
{
    Phg_args            cp_args;
    Phg_el_data         ed;
    Phg_args_add_el     *args = &cp_args.data.add_el;

    if (CB_ENTRY_CHECK(phg_cur_cph, ERR5, Pfn_set_colr_map_ind)) {
        if (PSL_STRUCT_STATE(phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT(phg_cur_cph->erh, ERR5);
        
        } else if ( *cmi < 0) {
            ERR_REPORT(phg_cur_cph->erh, ERR121);
        
        } else {
            args->el_type = PELEM_COLR_MAP_IND;
            ed.idata = *cmi;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}


/* SET HLHSR IDENTIFIER */
void
pshrid_( hrid)
Pint    *hrid;          /* HLHSR identifier     */
{
    Phg_args            cp_args;
    Phg_el_data         ed;
    Phg_args_add_el     *args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5,  Pfn_set_hlhsr_id)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else {
            args->el_type = PELEM_HLHSR_ID;
            ed.idata = *hrid;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}
