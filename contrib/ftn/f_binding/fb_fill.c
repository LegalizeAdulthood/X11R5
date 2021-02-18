#ifndef lint
static char     sccsid[] = "@(#)fb_fill.c 1.1 91/09/07 FJ";
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

/* Fill area functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"



/* FILL AREA 3 */
void
pfa3_( n, pxa, pya, pza)
Pint	*n;		/* number of points	*/
Pfloat	*pxa;		/* coordinates of points X	*/
Pfloat	*pya;		/* coordinates of points Y	*/
Pfloat	*pza;		/* coordinates of points Z	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Ppoint3		*points;
    Pint		err = 0;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_fill_area3)) {
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
	        args->el_type = PELEM_FILL_AREA3;
	        ed.ptlst3.num_points = *n;
	        ed.ptlst3.points = points;
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	            CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
                PFREE( *n, points);
            }
	}
    }
}


/* FILL AREA */
void
pfa_( n, pxa, pya)
Pint	*n;		/* number of points	*/
Pfloat	*pxa;		/* coordinates of points X	*/
Pfloat	*pya;		/* coordinates of points Y	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Ppoint		*points;
    Pint		err = 0;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_fill_area)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *n < 0) {
	    ERR_REPORT(phg_cur_cph->erh, ERR2004);
	
	} else {
            PMALLOC( err, Ppoint, *n, points);
            if ( err) {
                ERR_REPORT(phg_cur_cph->erh, err);
	
	    } else {
                POINT_DATA_TRANSLATE( *n, pxa, pya, points);
	        args->el_type = PELEM_FILL_AREA;
	        ed.ptlst.num_points = *n;
	        ed.ptlst.points = points;
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	            CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
                PFREE( *n, points);
            }
	}
    }
}


/* FILL AREA SET 3 */
void
pfas3_( npl, ixa, pxa, pya, pza)
Pint	*npl;		/* number of points lists	*/
Pint	*ixa;		/* array of indices for point lists	*/
Pfloat	*pxa;		/* coordinates of points X	*/
Pfloat	*pya;		/* coordinates of points Y	*/
Pfloat	*pza;		/* coordinates of points Z	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    register	int	i,j;
    register	int	w_ixa = 0;
    Ppoint3		*ptr;
    Ppoint3		*points;
    Ppoint_list3	*sets;
    Pint		err = 0;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_fill_area_set3)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
        } else  if ( *npl < 1) {
            ERR_REPORT( phg_cur_cph->erh, ERR2005);

        } else {
            args->el_type = PELEM_FILL_AREA_SET3;
            ed.fa_set3.num_sets = *npl;
            ed.fa_set3.total_pts = ixa[*npl - 1];
            PMALLOC( err, Ppoint_list3, *npl, sets);
            if ( err) {
                ERR_REPORT( phg_cur_cph->erh, err);

            } else {
                ed.fa_set3.sets = sets;
                for ( i = 0; i < *npl; i++) {
                    if (( sets[i].num_points = (ixa[i] - w_ixa)) < 3) {
                        ERR_REPORT( phg_cur_cph->erh, ERR2005);
                        PFREE( *npl, sets);
                        return;
                    }
                    w_ixa = ixa[i];
                }

                PMALLOC( err, Ppoint3, ed.fa_set3.total_pts, ptr);
                points = ptr;
                if ( err) {
                    ERR_REPORT( phg_cur_cph->erh, err);

                } else {
                    j = 0;
                    for ( i = 0; i < *npl; i++) {
                        ed.fa_set3.sets[i].points = points;
                        for ( ; j < ixa[i]; j++) {
                            points->x = pxa[j];
                            points->y = pya[j];
                            points->z = pza[j];
                            points++;
                        }
                    }
                    
                    if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                        CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
                    PFREE( ed.fa_set3.total_pts, ptr);
                }
                PFREE( *npl, sets);
            }
        }
    }
}


/* FILL AREA SET 3 WITH DATA */
void
pfasd3_( fflag, eflag, vflag, ctype, npl, ixa, pxa, pya, pza, fcoli, fcolr,
fvxa, fvya, fvza, exa, edata, vcoli, vcolr, vvxa, vvya, vvza)
Pint	*fflag;		/* data per facet flag	*/
Pint	*eflag;		/* data per edge flag	*/
Pint	*vflag;		/* data per vertex flag	*/
Pint	*ctype;		/* colour type	*/
Pint	*npl;		/* number of point lists	*/
Pint	*ixa;		/* array of end indices for point lists	*/
Pfloat	*pxa;		/* coordinates of points X	*/
Pfloat	*pya;		/* coordinates of points Y	*/
Pfloat	*pza;		/* coordinates of points Z	*/
Pint	*fcoli;		/* facet colour index	*/
Pfloat	*fcolr;		/* facet colour components	*/
Pfloat	*fvxa;		/* facet vector data X	*/
Pfloat	*fvya;		/* facet vector data Y	*/
Pfloat	*fvza;		/* facet vector data Z	*/
Pint	*exa;		/* array of end indices for edge data lists	*/
Pint	*edata;		/* edge data	*/
Pint	*vcoli;		/* vertex colour indices	*/
Pfloat	*vcolr;		/* vertex colour components	*/
Pfloat	*vvxa;		/* vertex vector data X	*/
Pfloat	*vvya;		/* vertex vector data Y	*/
Pfloat	*vvza;		/* vertex vector data Z	*/
{
    Pedge_data_list	*eddata = NULL;
    Pfacet_vdata_list3	*vdata_list = NULL;
    Pfacet_vdata_arr3	vdata;

    Phg_args            cp_args;
    Phg_el_data         ed;
    Phg_args_add_el     *args = &cp_args.data.add_el;

    register int        i, j, k, size;
    Pint		w_ixa = 0;
    Pint		err = 0;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_fill_area_set3_data)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);
        
        } else if ( *npl < 1) {
            ERR_REPORT( phg_cur_cph->erh, ERR2005);
        
        } else if ( *fflag < PNOF || *fflag > PFCONO) {
            ERR_REPORT( phg_cur_cph->erh, ERRN161);
        
        } else if ( *eflag < PNOE || *eflag > PEVIS) {
            ERR_REPORT( phg_cur_cph->erh, ERRN163);
        
        } else if ( *vflag < PCOORD || *vflag > PCCONO) {
            ERR_REPORT( phg_cur_cph->erh, ERRN162);
        
        } else {
            args->el_type = PELEM_FILL_AREA_SET3_DATA;
            ed.fa_set3_d.fflag = *fflag;
            ed.fa_set3_d.vflag = *vflag;
            ed.fa_set3_d.eflag = *eflag;
            ed.fa_set3_d.colour_model = *ctype;
            ed.fa_set3_d.num_sets = *npl;

            /* check the edge flags. */
            if ( *eflag != PNOE &&
                ixa[*npl - 1] != exa[*npl - 1]) {
                ERR_REPORT( phg_cur_cph->erh, ERR513);
                return;
            }

            /* change facet data */
            switch ( *fflag) {
            case PFCOLR:
                COVAL_DATA_TRANSLATE( *ctype, *fcoli, fcolr,
                    ed.fa_set3_d.fdata.colr);
                break;
            case PFNORM:
                ed.fa_set3_d.fdata.norm.delta_x = *fvxa;
                ed.fa_set3_d.fdata.norm.delta_y = *fvya;
                ed.fa_set3_d.fdata.norm.delta_z = *fvza;
                break;
            case PFCONO:
                COVAL_DATA_TRANSLATE( *ctype, *fcoli, fcolr,
                    ed.fa_set3_d.fdata.conorm.colr);
                ed.fa_set3_d.fdata.conorm.norm.delta_x = *fvxa;
                ed.fa_set3_d.fdata.conorm.norm.delta_y = *fvya;
                ed.fa_set3_d.fdata.conorm.norm.delta_z = *fvza;
                break;
            }

            /* change edge data */
            if ( *eflag == PNOE) { 
                eddata = (Pedge_data_list *)NULL;

            } else {
                PMALLOC( err, Pedge_data_list, *npl, eddata);
                if ( err) {
                    ERR_REPORT( phg_cur_cph->erh, err);
                    return;

                } else {
                    ed.fa_set3_d.edata = eddata;
                    for ( i = 0; i < *npl; i++) {
                        if ( exa[i] < w_ixa) {
                            ERR_REPORT( phg_cur_cph->erh, ERR513);
                            PFREE( *npl, eddata);
                            return;
                        } else {
                            eddata[i].num_edges = exa[i] - w_ixa;
                            eddata[i].edgedata.edges =
                                (Pedge_flag *)(&(edata[w_ixa]));
                        }
                        w_ixa = exa[i];
                    }
                }
            }

            /* change vertex data */
            w_ixa = 0;
            for ( i = 0; i < *npl; i++) {
                if ( (ixa[i] - w_ixa) < 3) {
                    ERR_REPORT( phg_cur_cph->erh, ERR2005);
                    PFREE( *npl, eddata);
                    return;
                }
                w_ixa = ixa[i];
            }
            ed.fa_set3_d.num_vertices = size = ixa[*npl - 1];
            PMALLOC( err, Pfacet_vdata_list3, *npl, vdata_list);
            switch ( *vflag) {
            case PCOORD:
                vdata.points = NULL;
                PMALLOC( err, Ppoint3, size, vdata.points);
                break;
            case PCCOLR:
                vdata.ptcolrs = NULL;
                PMALLOC( err, Pptco3, size, vdata.ptcolrs);
                break;
            case PCNORM:
                vdata.ptnorms = NULL;
                PMALLOC( err, Pptnorm3, size, vdata.ptnorms);
                break;
            case PCCONO:
                vdata.ptconorms = NULL;
                PMALLOC( err, Pptconorm3, size, vdata.ptconorms);
                break;
            }
            if ( err) {
                ERR_REPORT(phg_cur_cph->erh, err);

            } else {
                ed.fa_set3_d.vdata = vdata_list;
                VERTEX_DATA_TRANSLATE( *vflag, ixa[*npl - 1], *ctype, vcoli,
                    vcolr, pxa, pya, pza, vvxa, vvya, vvza, vdata);
                w_ixa = 0;
                switch ( *vflag) {
                case PCOORD:
                    for ( i = 0; i < *npl; i++) {
                        vdata_list[i].num_vertices = ixa[i] - w_ixa;
                        vdata_list[i].vertex_data.points =
                            &(vdata.points[w_ixa]);
                        w_ixa = ixa[i];
                    }
                    break;
                case PCCOLR:
                    for ( i = 0; i < *npl; i++) {
                        vdata_list[i].num_vertices = ixa[i] - w_ixa;
                        vdata_list[i].vertex_data.ptcolrs =
                            &(vdata.ptcolrs[w_ixa]);
                        w_ixa = ixa[i];
                    }
                    break;
                case PCNORM:
                    for ( i = 0; i < *npl; i++) {
                        vdata_list[i].num_vertices = ixa[i] - w_ixa;
                        vdata_list[i].vertex_data.ptnorms =
                            &(vdata.ptnorms[w_ixa]);
                        w_ixa = ixa[i];
                    }
                    break;
                case PCCONO:
                    for ( i = 0; i < *npl; i++) {
                        vdata_list[i].num_vertices = ixa[i] - w_ixa;
                        vdata_list[i].vertex_data.ptconorms =
                            &(vdata.ptconorms[w_ixa]);
                        w_ixa = ixa[i];
                    }
                    break;
                }
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                    CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
            }
            PFREE( *npl, eddata);
            PFREE ( *npl, vdata_list);
            switch ( *vflag) {
            case PCOORD:
                PFREE ( size, vdata.points);
                break;
            case PCCOLR:
                PFREE ( size, vdata.ptcolrs);
                break;
            case PCNORM:
                PFREE ( size, vdata.ptnorms);
                break;
            case PCCONO:
                PFREE ( size, vdata.ptconorms);
                break;
            }
        }
    }
}


/* FILL AREA SET */
void
pfas_( npl, ixa, pxa, pya)
Pint	*npl;		/* number of point lists	*/
Pint	*ixa;		/* array of indices for the point lists	*/
Pfloat	*pxa;		/* coordinates of points X	*/
Pfloat	*pya;		/* coordinates of points Y	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    register	int	i,j;
    register	int	w_ixa = 0;
    Ppoint		*ptr;
    Ppoint		*points;
    Ppoint_list		*sets;
    Pint		err = 0; 
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_fill_area_set)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *npl < 1) {
            ERR_REPORT( phg_cur_cph->erh, ERR2005);
	
        } else {
	    args->el_type = PELEM_FILL_AREA_SET;
            ed.fa_set.num_sets = *npl;
            ed.fa_set.total_pts = ixa[*npl - 1];
            PMALLOC( err, Ppoint_list, *npl, sets);
	    if ( err) {
	        ERR_REPORT( phg_cur_cph->erh, err);
		
            } else {
		ed.fa_set.sets = sets;
                for ( i = 0; i < *npl; i++) {
		    if (( sets[i].num_points = (ixa[i] - w_ixa)) < 3) {
			ERR_REPORT( phg_cur_cph->erh, ERR2005);
			PFREE( *npl, sets);
			return;
		    }
                    w_ixa = ixa[i];
		}

                PMALLOC( err, Ppoint, ed.fa_set.total_pts, ptr);
                points = ptr;
		if ( err) {
		    ERR_REPORT( phg_cur_cph->erh, err);
		
		} else {
                    j = 0;
		    for ( i = 0; i < *npl; i++) {
                        ed.fa_set.sets[i].points = points;
                        for ( ; j < ixa[i]; j++) {
		            points->x = pxa[j];
		            points->y = pya[j];
                            points++;
                        }
                    }

                    if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
		        CP_FUNC(phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
		    PFREE( ed.fa_set.total_pts, ptr);
		}
		PFREE( *npl, sets);
	    }
	}
    }
}


/* TRIANGLE STRIP 3 WITH DATA */
void
ptsd3_( fflag, vflag, ctype, n, pxa, pya, pza, fcoli, fcolr, fvxa, fvya, fvza,
vcoli, vcolr, vvxa, vvya, vvza)
Pint	*fflag;		/* data per facet flag	*/
Pint	*vflag;		/* data per vertex flag	*/
Pint	*ctype;		/* colour type	*/
Pint	*n;		/* number of points	*/
Pfloat	*pxa;		/* coordinattes of points X	*/
Pfloat	*pya;		/* coordinattes of points Y	*/
Pfloat	*pza;		/* coordinattes of points Z	*/
Pint	*fcoli;		/* facet colour index	*/
Pfloat	*fcolr;		/* facet colour component	*/
Pfloat	*fvxa;		/* facet vector data X	*/
Pfloat	*fvya;		/* facet vector data Y	*/
Pfloat	*fvza;		/* facet vector data Z	*/
Pint	*vcoli;		/* vertex facet colour index	*/
Pfloat	*vcolr;		/* vertex colour component	*/
Pfloat	*vvxa;		/* vertex vector data X	*/
Pfloat	*vvya;		/* vertex vector data Y	*/
Pfloat	*vvza;		/* vertex vector data Z	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Pfacet_data_arr3	fdata;
    Pfacet_vdata_arr3	vdata;
    Pint		err = 0;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_tri_strip3_data)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);
        
        } else if ( *n < 0) {
            ERR_REPORT( phg_cur_cph->erh, ERR2004);
        
#ifdef SX_G
        } else if ( *n < 3) {
            ERR_REPORT( phg_cur_cph->erh, ERRN850);
#endif

        } else if ( *fflag < PNOF || *fflag > PFCONO) {
            ERR_REPORT( phg_cur_cph->erh, ERRN161);
        
        } else if ( *vflag < PCOORD || *vflag > PCCONO) {
            ERR_REPORT( phg_cur_cph->erh, ERRN162);
        
        } else {
            args->el_type = PELEM_TRI_STRIP3_DATA;
            ed.tri_strip3.fflag = *fflag;
            ed.tri_strip3.vflag = *vflag;
            ed.tri_strip3.colour_model = *ctype;

            switch ( *fflag) {
            case PFCOLR:
                fdata.colrs = NULL;
                PMALLOC( err, Pcoval, (*n - 2), fdata.colrs);
                break;
            case PFNORM:
                fdata.norms = NULL;
                PMALLOC( err, Pvec3, (*n - 2), fdata.norms);
                break;
            case PFCONO:
                fdata.conorms = NULL;
                PMALLOC( err, Pconorm3, (*n - 2), fdata.conorms);
                break;
            }
            switch ( *vflag) {
            case PCOORD:
                vdata.points = NULL;
                PMALLOC( err, Ppoint3, *n, vdata.points);
                break;
            case PCCOLR:
                vdata.ptcolrs = NULL;
                PMALLOC( err, Pptco3, *n, vdata.ptcolrs);
                break;
            case PCNORM:
                vdata.ptnorms = NULL;
                PMALLOC( err, Pptnorm3, *n, vdata.ptnorms);
                break;
            case PCCONO:
                vdata.ptconorms = NULL;
                PMALLOC( err, Pptconorm3, *n, vdata.ptconorms);
                break;
            }
            if ( err) {
                ERR_REPORT(phg_cur_cph->erh, err);

            } else {
                /* change facet data */
                if ( *fflag != PNOF) {
                    FACET_DATA_TRANSLATE( *fflag, (*n - 2), *ctype, fcoli,
                        fcolr, fvxa, fvya, fvza, fdata);
                    ed.tri_strip3.fdata.num_facets = (*n - 2);
                    ed.tri_strip3.fdata.facetdata = fdata;
                } else {
                    ed.tri_strip3.fdata.num_facets = 0;
                }
                /* change vertex data */
                VERTEX_DATA_TRANSLATE( *vflag, *n, *ctype, vcoli, vcolr, pxa,
                    pya, pza, vvxa, vvya, vvza, vdata);
                ed.tri_strip3.vdata.num_vertices = *n;
                ed.tri_strip3.vdata.vertex_data = vdata;
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                    CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
            }

            switch ( *fflag) {
            case PFCOLR:
                PFREE( (*n - 2), fdata.colrs);
                break;
            case PFNORM:
                PFREE( (*n - 2), fdata.norms);
                break;
            case PFCONO:
                PFREE( (*n - 2), fdata.conorms);
                break;
            }
            switch ( *vflag) {
            case PCOORD:
                PFREE( *n, vdata.points);
                break;
            case PCCOLR:
                PFREE( *n, vdata.ptcolrs);
                break;
            case PCNORM:
                PFREE( *n, vdata.ptnorms);
                break;
            case PCCONO:
                PFREE( *n, vdata.ptconorms);
                break;
            }
        }
    }
}

	
/* QUADRILATERAL MESH 3 WITH DATA */
void
pqmd3_( fflag, vflag, ctype, nc, nr, pxa, pya, pza, fcoli, fcolr, fvxa, fvya,
fvza, vcoli, vcolr, vvxa, vvya, vvza)
Pint	*fflag;		/* data per facet flag	*/
Pint	*vflag;		/* data per vertex flag	*/
Pint	*ctype;		/* colour type	*/
Pint	*nc;		/* colum number of points dimensions	*/
Pint	*nr;		/* row number of points dimensions	*/
Pfloat	*pxa;		/* coordinattes of points X	*/
Pfloat	*pya;		/* coordinattes of points Y	*/
Pfloat	*pza;		/* coordinattes of points Z	*/
Pint	*fcoli;		/* facet colour index	*/
Pfloat	*fcolr;		/* facet colour component	*/
Pfloat	*fvxa;		/* facet vector data X	*/
Pfloat	*fvya;		/* facet vector data Y	*/
Pfloat	*fvza;		/* facet vector data Z	*/
Pint	*vcoli;		/* vertex facet colour index	*/
Pfloat	*vcolr;		/* vertex colour component	*/
Pfloat	*vvxa;		/* vertex vector data X	*/
Pfloat	*vvya;		/* vertex vector data Y	*/
Pfloat	*vvza;		/* vertex vector data Z	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    Pfacet_data_arr3	fdata;
    Pfacet_vdata_arr3	vdata;
    Pint		dim1, dim2;
    Pint		err = 0;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_quad_mesh3_data)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);
        
        } else if ( *nc < 0 || *nr < 0) {
            ERR_REPORT( phg_cur_cph->erh, ERRN164);
        
#ifdef SX_G
        } else if ( *nc < 2 || *nr < 2) {
            ERR_REPORT( phg_cur_cph->erh, ERRN850);
#endif
      
        } else if ( *fflag < PNOF || *fflag > PFCONO) {
            ERR_REPORT( phg_cur_cph->erh, ERRN161);
        
        } else if ( *vflag < PCOORD || *vflag > PCCONO) {
            ERR_REPORT( phg_cur_cph->erh, ERRN162);
        
        } else {
            args->el_type = PELEM_QUAD_MESH3_DATA;
            ed.quad_mesh3.fflag = *fflag;
            ed.quad_mesh3.vflag = *vflag;
            ed.quad_mesh3.colour_model = *ctype;
            ed.quad_mesh3.dim.size_x = *nc;
            ed.quad_mesh3.dim.size_y = *nr;
            dim1 = (*nc - 1) * (*nr - 1);
            dim2 = (*nc) * (*nr);

            switch ( *fflag) {
            case PFCOLR:
                fdata.colrs = NULL;
                PMALLOC( err, Pcoval, dim1, fdata.colrs);
                break;
            case PFNORM:
                fdata.norms = NULL;
                PMALLOC( err, Pvec3, dim1, fdata.norms);
                break;
            case PFCONO:
                fdata.conorms = NULL;
                PMALLOC( err, Pconorm3, dim1, fdata.conorms);
                break;
            }
            switch ( *vflag) {
            case PCOORD:
                vdata.points = NULL;
                PMALLOC( err, Ppoint3, dim2, vdata.points);
                break;
            case PCCOLR:
                vdata.ptcolrs = NULL;
                PMALLOC( err, Pptco3, dim2, vdata.ptcolrs);
                break;
            case PCNORM:
                vdata.ptnorms = NULL;
                PMALLOC( err, Pptnorm3, dim2, vdata.ptnorms);
                break;
            case PCCONO:
                vdata.ptconorms = NULL;
                PMALLOC( err, Pptconorm3, dim2, vdata.ptconorms);
                break;
            }
            if ( err) {
                ERR_REPORT( phg_cur_cph->erh, err);

            } else {
                /* change facet data */
                if ( *fflag != PNOF) {
                    FACET_DATA_TRANSLATE( *fflag, dim1,
                        *ctype, fcoli, fcolr, fvxa, fvya, fvza, fdata);
                    ed.quad_mesh3.fdata.num_facets = dim1;
                    ed.quad_mesh3.fdata.facetdata = fdata;
                } else {
                    ed.quad_mesh3.fdata.num_facets = 0;
                }
                /* change vertex data */
                VERTEX_DATA_TRANSLATE( *vflag, dim2, *ctype, vcoli,
                    vcolr, pxa, pya, pza, vvxa, vvya, vvza, vdata);
                ed.quad_mesh3.vdata.num_vertices = dim2;
                ed.quad_mesh3.vdata.vertex_data = vdata;
                if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                    CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
            }
            switch ( *fflag) {
            case PFCOLR:
                PFREE( dim1, fdata.colrs);
                break;
            case PFNORM:
                PFREE( dim1, fdata.norms);
                break;
            case PFCONO:
                PFREE( dim1, fdata.conorms);
                break;
            }
            switch ( *vflag) {
            case PCOORD:
                PFREE( dim2, vdata.points);
                break;
            case PCCOLR:
                PFREE( dim2, vdata.ptcolrs);
                break;
            case PCNORM:
                PFREE( dim2, vdata.ptnorms);
                break;
            case PCCONO:
                PFREE( dim2, vdata.ptconorms);
                break;
            }
        }
    }
}

        
/* SET OF FILL AREA SET 3 WITH DATA */
void
psofa3_( fflag, eflag, vflag, ctype, nfs, nf, np, ixa, pxa, pya, pza, fcoli,
fcolr, fvxa, fvya, fvza, exa, edata, vcoli, vcolr, vvxa, vvya, vvza, vind)
Pint	*fflag;		/* data per facet flag	*/
Pint	*eflag;		/* data per edge flag	*/
Pint	*vflag;		/* data per vertex flag	*/
Pint	*ctype;		/* colour type	*/
Pint	*nfs;		/* number of fill area sets	*/
Pint	*nf;		/* number of facets	*/
Pint	*np;		/* number of points	*/
Pint	*ixa;		/* array of indicices for vertex indices	*/
Pfloat	*pxa;		/* coordinattes of points X	*/
Pfloat	*pya;		/* coordinattes of points Y	*/
Pfloat	*pza;		/* coordinattes of points Z	*/
Pint	*fcoli;		/* facet colour index	*/
Pfloat	*fcolr;		/* facet colour component	*/
Pfloat	*fvxa;		/* facet vector data X	*/
Pfloat	*fvya;		/* facet vector data Y	*/
Pfloat	*fvza;		/* facet vector data Z	*/
Pint	*exa;		/* array of indicices for edge flag lists	*/
Pint	*edata;		/* edge data	*/
Pint	*vcoli;		/* vertex facet colour index	*/
Pfloat	*vcolr;		/* vertex colour component	*/
Pfloat	*vvxa;		/* vertex vector data X	*/
Pfloat	*vvya;		/* vertex vector data Y	*/
Pfloat	*vvza;		/* vertex vector data Z	*/
Pint	*vind;		/* vertex indices	*/
{
    Pfacet_data_arr3		fdata;
    Pfacet_vdata_arr3		vdata;
    Pedge_data_list_list	*edge = NULL;
    Pedge_data_list		*edgelist = NULL;
    Pint_list_list		*vlist = NULL;
    Pint_list			*lists = NULL;
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    register int	i, j, k;
    Pint		num_contours = 0;
    Pint		w_ixa = 0;
    Pint		work = 0;
    Pint		err = 0;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_of_fill_area_set3_data)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else if ( *fflag < PNOF || *fflag > PFCONO) {
            ERR_REPORT(phg_cur_cph->erh, ERRN161);
        
        } else if ( *eflag < PNOE || *eflag > PEVIS) {
            ERR_REPORT(phg_cur_cph->erh, ERRN163);
        
        } else if ( *vflag < PCOORD || *vflag > PCCONO) {
            ERR_REPORT(phg_cur_cph->erh, ERRN162);
        
        } else if ( *nfs < 1 || *np < 1) {
            ERR_REPORT(phg_cur_cph->erh, ERR2005);
        
        } else {
            for ( i = 0; i < *nfs; i++) {
                if ( nf[i] < 1) {
                    ERR_REPORT(phg_cur_cph->erh, ERR2005);
                    return;
                }
                num_contours += nf[i];
            }
            w_ixa = 0;
            for ( i = 0; i < num_contours; i++) {
                if ( (ixa[i]  -  w_ixa) < 3) {
                    ERR_REPORT(phg_cur_cph->erh, ERR2005);
                    return;
                }
                w_ixa = ixa[i];
            }
            for ( i = 0; i < ixa[num_contours - 1]; i++) {
                if ( vind[i] < 0 || vind[i] > *np) {
                    ERR_REPORT(phg_cur_cph->erh, ERR504);
                    return;
                }
            }
            if ( *eflag != PNOE) {
                if ( ixa[num_contours - 1] != exa[num_contours - 1]) {
                    ERR_REPORT(phg_cur_cph->erh, ERR513);
                    return;
                }
            }
                
            args->el_type = PELEM_SET_OF_FILL_AREA_SET3_DATA;
            ed.sofas3.fflag = *fflag;
            ed.sofas3.eflag = *eflag;
            ed.sofas3.vflag = *vflag;
            ed.sofas3.colour_model = *ctype;
            ed.sofas3.num_sets = *nfs;
            ed.sofas3.num_contours = num_contours;
            ed.sofas3.num_vindices = ixa[num_contours - 1];

            /* malloc data */
            switch ( *fflag) {
            case PFCOLR:
                fdata.colrs = NULL;
                PMALLOC( err, Pcoval, *nfs, fdata.colrs);
                break;
            case PFNORM:
                fdata.norms = NULL;
                PMALLOC( err, Pvec3, *nfs, fdata.norms);
                break;
            case PFCONO:
                fdata.conorms = NULL;
                PMALLOC( err, Pconorm3, *nfs, fdata.conorms);
                break;
            }
            switch ( *vflag) {
            case PCOORD:
                vdata.points = NULL;
                PMALLOC( err, Ppoint3, *np, vdata.points);
                break;
            case PCCOLR:
                vdata.ptcolrs = NULL;
                PMALLOC( err, Pptco3, *np, vdata.ptcolrs);
                break;
            case PCNORM:
                vdata.ptnorms = NULL;
                PMALLOC( err, Pptnorm3, *np, vdata.ptnorms);
                break;
            case PCCONO:
                vdata.ptconorms = NULL;
                PMALLOC( err, Pptconorm3, *np, vdata.ptconorms);
                break;
            }
            PMALLOC( err, Pint_list_list, *nfs, vlist);
            PMALLOC( err, Pint_list, num_contours, lists);
            if ( *eflag != PNOE) {
                PMALLOC( err, Pedge_data_list_list, *nfs, edge);
                PMALLOC( err, Pedge_data_list, num_contours, edgelist);
            }
            if ( err) {
                ERR_REPORT (phg_cur_cph->erh, err);
                switch ( *fflag) {
                case PFCOLR:
                    PFREE( *nfs, fdata.colrs);
                    break;
                case PFNORM:
                    PFREE( *nfs, fdata.norms);
                    break;
                case PFCONO:
                    PFREE( *nfs, fdata.conorms);
                    break;
                }
                switch ( *vflag) {
                case PCOORD:
                    PFREE( *np, vdata.points);
                    break;
                case PCCOLR:
                    PFREE( *np, vdata.ptcolrs);
                    break;
                case PCNORM:
                    PFREE( *np, vdata.ptnorms);
                    break;
                case PCCONO:
                    PFREE( *np, vdata.ptconorms);
                    break;
                }
                PFREE( *nfs, vlist);
                PFREE( num_contours, lists);
                if ( *eflag != PNOE) {
                    PFREE( *nfs, edge);
                    PFREE( num_contours, edgelist);
                }
                return;
            }
            /* change facet data */
            if ( *fflag != PNOF) {
                FACET_DATA_TRANSLATE( *fflag, *nfs, *ctype, fcoli, fcolr,
                    fvxa, fvya, fvza, fdata);
                ed.sofas3.fdata.num_facets = *nfs;
                ed.sofas3.fdata.facetdata = fdata;
            } else {
                ed.sofas3.fdata.num_facets = *nfs;
            }

            /* change vertex data */
            VERTEX_DATA_TRANSLATE( *vflag, *np, *ctype, vcoli, vcolr, pxa, pya,
                pza, vvxa, vvya, vvza, vdata);
                ed.sofas3.vdata.num_vertices = *np;
                ed.sofas3.vdata.vertex_data = vdata;

            /* change vertex indices */
            k = 0;
            w_ixa = 0;
            work = 0;
            ed.sofas3.vlist = vlist;
            for ( i = 0; i < *nfs; i++) {
                ed.sofas3.vlist[i].num_lists = nf[i];
                ed.sofas3.vlist[i].lists = &lists[work];
                for ( j = 0; j < nf[i]; j++) {
                    ed.sofas3.vlist[i].lists[j].num_ints = ixa[k] - w_ixa;
                    ed.sofas3.vlist[i].lists[j].ints = vind + w_ixa;
                    w_ixa = ixa[k];
                    k++;
                }
                work += nf[i];
            }
           
            /* change edge data */
            if ( *eflag != PNOE) {
                k = 0;
                w_ixa = 0;
                work = 0;
                ed.sofas3.edata = edge;
                for ( i = 0; i < *nfs; i++) {
                    ed.sofas3.edata[i].num_lists = nf[i];
                    ed.sofas3.edata[i].edgelist = &edgelist[work];
                    for ( j = 0; j < nf[i]; j++) {
                        ed.sofas3.edata[i].edgelist[j].num_edges =
                            exa[k] - w_ixa;
                        ed.sofas3.edata[i].edgelist[j].edgedata.edges =
                            edata + w_ixa;
                        w_ixa = exa[k];
                        k++;
                    }
                    work += nf[i];
                }
            }
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
            switch ( *fflag) {
            case PFCOLR:
                PFREE( *nfs, fdata.colrs);
                break;
            case PFNORM:
                PFREE( *nfs, fdata.norms);
                break;
            case PFCONO:
                PFREE( *nfs, fdata.conorms);
                break;
            }
            switch ( *vflag) {
            case PCOORD:
                PFREE( *np, vdata.points);
                break;
            case PCCOLR:
                PFREE( *np, vdata.ptcolrs);
                break;
            case PCNORM:
                PFREE( *np, vdata.ptnorms);
                break;
            case PCCONO:
                PFREE( *np, vdata.ptconorms);
                break;
            }
            PFREE( *nfs, vlist);
            PFREE( num_contours, lists);
            if ( *eflag != PNOE) {
                PFREE( *nfs, edge);
                PFREE( num_contours, edgelist);
            }
        }
    }
}


/* SET INTERIOR INDEX */
void
psii_( ii)
Pint	*ii;		/* interior index	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_int_ind)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *ii < 1) {
	    ERR_REPORT(phg_cur_cph->erh, ERR100);
	
	} else {
	    args->el_type = PELEM_INT_IND;
	    ed.idata = *ii;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET EDGE INDEX */
void
psedi_( edi)
Pint	*edi;		/* edge index	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_edge_ind)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *edi < 1) {
	    ERR_REPORT(phg_cur_cph->erh, ERR100);
	
	} else {
	    args->el_type = PELEM_EDGE_IND;
	    ed.idata = *edi;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET INTERIOR STYLE */

void
psis_( ints)
Pint	*ints;		/* interior style	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_int_style)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
        } else if ( *ints < PHOLLO || *ints > PISEMP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2000);
        
	} else {
	    args->el_type = PELEM_INT_STYLE;
	    ed.interstyle = (Pint_style)*ints;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET BACK INTERIOR STYLE */
void
psbis_( ints)
Pint	*ints;	/* back interior style	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_back_int_style)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
        } else if ( *ints < PHOLLO || *ints > PISEMP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2000);
        
	} else {
	    args->el_type = PELEM_BACK_INT_STYLE;
	    ed.interstyle = (Pint_style)*ints;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET INTERIOR STYLE INDEX */
void
psisi_( istyli)
Pint	*istyli; 	/* interior style index	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_int_style_ind)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_INT_STYLE_IND;
	    ed.idata = *istyli;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET BACK INTERIOR STYLE INDEX */
void
psbisi_( istyli)
Pint	*istyli;	/* back interior sytle index	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_back_int_style_ind)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_BACK_INT_STYLE_IND;
	    ed.idata = *istyli;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET INTERIOR COLOUR INDEX */
void
psici_( coli)
Pint	*coli;		/* interior colour index	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_int_colr_ind)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *coli < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR113);
	
	} else {
	    args->el_type = PELEM_INT_COLR_IND;
	    ed.idata = *coli;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET INTERIOR COLOUR */
void
psic_( ctype, coli, colr)
Pint	*ctype;		/* interior colour type	*/
Pint	*coli;		/* colour index	*/
Pfloat	*colr;		/* colour components	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_int_colr)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_INT_COLR;
            GCOLR_DATA_TRANSLATE( *ctype, *coli, colr, ed.colour);
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET BACK INTERIOR COLOUR */
void
psbic_( ctype, coli, colr)
Pint	*ctype;		/* back interior colour type	*/
Pint	*coli;		/* colour index	*/
Pfloat	*colr;		/* colour components	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_back_int_colr)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_BACK_INT_COLR;
            GCOLR_DATA_TRANSLATE( *ctype, *coli, colr, ed.colour);
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET EDGE FLAG */
void
psedfg_( edflag)
Pint	*edflag;	/* edge flag	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_edge_flag)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *edflag < POFF || *edflag > PON) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);
            
        } else {
            args->el_type = PELEM_EDGE_FLAG;
	    ed.edgef = (Pedge_flag)*edflag;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}


/* SET EDGETYPE */
void
psedt_( edtype)
Pint	*edtype;	/* edgetype	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_edgetype)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_EDGETYPE;
	    ed.idata = *edtype;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET EDGEWIDTH SCALE FACTOR */
void
psewsc_( ewidth)
Pfloat	*ewidth; 	/* edgewidth scale factor	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_edgewidth)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_EDGEWIDTH;
	    ed.fdata = *ewidth;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET EDGE COLOUR INDEX */
void
psedci_( coli)
Pint	*coli;		/* edge colour index	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_edge_colr_ind)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else if ( *coli < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR113);	    
	
	} else {
	    args->el_type = PELEM_EDGE_COLR_IND;
	    ed.idata = *coli;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET EDGE COLOUR */
void
psedc_( ctype, coli, colr)
Pint	*ctype;		/* edge colour type	*/
Pint	*coli;		/* colour index	*/
Pfloat	*colr;		/* colour components	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_edge_colr)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_EDGE_COLR;
            GCOLR_DATA_TRANSLATE( *ctype, *coli, colr, ed.colour);
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET PATTERN SIZE */
void
pspa_( szx, szy)
Pfloat  *szx;         /* pattern size X	*/ 
Pfloat  *szy;         /* pattern size Y	*/ 
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_pat_size)) {
	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_PAT_SIZE;
	    ed.pt.x = *szx;
	    ed.pt.y = *szy;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET PATTERN REFERENCE POINT AND VECTORS */
void
psprpv_( rfx, rfy, rfz, rfvx, rfvy, rfvz)
Pfloat	*rfx;		/* pattern reference point X	*/
Pfloat	*rfy;		/* pattern reference point Y	*/
Pfloat	*rfz;		/* pattern reference point Z	*/
Pfloat	*rfvx;		/* pattern reference vectors X	*/
Pfloat	*rfvy;		/* pattern reference vectors Y	*/
Pfloat	*rfvz;		/* pattern reference vectors Z	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_pat_ref_point_vecs)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else {
            args->el_type = PELEM_PAT_REF_POINT_VECS;
            ed.pat_pt_vecs.pt.x = *rfx;
            ed.pat_pt_vecs.pt.y = *rfy;
            ed.pat_pt_vecs.pt.z = *rfz;
            ed.pat_pt_vecs.vecs[0].delta_x = rfvx[0];
            ed.pat_pt_vecs.vecs[0].delta_y = rfvy[0];
            ed.pat_pt_vecs.vecs[0].delta_z = rfvz[0];
            ed.pat_pt_vecs.vecs[1].delta_x = rfvx[1];
            ed.pat_pt_vecs.vecs[1].delta_y = rfvy[1];
            ed.pat_pt_vecs.vecs[1].delta_z = rfvz[1];
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}


/* SET PATTERN REFERENCE POINT */
void
psparf_( rfx, rfy)
Pfloat  *rfx;		/* pattern reference point X	*/
Pfloat  *rfy;		/* pattern reference point Y	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_pat_ref_point)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else {
            args->el_type = PELEM_PAT_REF_POINT;
            ed.pt.x = *rfx;     
            ed.pt.y = *rfy;     
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
} 


/* SET INTERIOR SHADING METHOD */
void
psism_( ism)
Pint	*ism;		/* shading method	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_int_shad_meth)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {    
            ERR_REPORT( phg_cur_cph->erh, ERR5); 
        
        } else {
            args->el_type = PELEM_INT_SHAD_METH;
            ed.idata = *ism;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    } 
}


/* SET BACK INTERIOR SHADING METHOD */
void
psbism_( ism)
Pint	*ism;		/* back shading method	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_back_int_shad_meth)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {    
            ERR_REPORT( phg_cur_cph->erh, ERR5); 
        
        } else {
            args->el_type = PELEM_BACK_INT_SHAD_METH;
            ed.idata = *ism;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    } 
}


/* SET FACE DISTINGUISHING MODE */
void
psfdm_( dmode)
Pint	*dmode;         /* distinguishing mode */
{
    Phg_args            cp_args;
    Phg_el_data         ed;
    Phg_args_add_el     *args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_face_disting_mode)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else if ( *dmode < PDSNO || *dmode > PDSYES) {
            ERR_REPORT(phg_cur_cph->erh, ERR2000);

        } else {
            args->el_type = PELEM_FACE_DISTING_MODE;
            ed.idata = *dmode;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}


/* SET FACE CULLING MODE */
void
psfcm_( cmode)
Pint	*cmode;         /* culling mode */
{
    Phg_args            cp_args;
    Phg_el_data         ed;
    Phg_args_add_el     *args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_face_cull_mode)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else if ( *cmode < PNCUL || *cmode > PFFAC) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else {
            args->el_type = PELEM_FACE_CULL_MODE;
            ed.idata = *cmode;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}


/* SET REFLECTANCE PROPERTIES */
void
psrfp_( amrc, dirc, sprc, ctype, coli, colr, spex)
Pfloat  *amrc;          /* ambient reflection coefficient       */
Pfloat  *dirc;          /* diffuse reflection coefficient       */
Pfloat  *sprc;          /* specular reflection coefficient      */
Pint    *ctype;         /* reflectance colour type      */
Pint    *coli;          /* colour index */
Pfloat  *colr;         /* colour components    */
Pfloat  *spex;          /* specular exponent    */
{
    Phg_args            cp_args;
    Phg_el_data         ed;
    Phg_args_add_el     *args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_area_prop)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else {
            args->el_type = PELEM_REFL_PROPS;
            ed.refl_props.ambient_coef = *amrc;
            ed.refl_props.diffuse_coef = *dirc;
            ed.refl_props.specular_coef = *sprc;
            GCOLR_DATA_TRANSLATE( *ctype, *coli, colr,
                ed.refl_props.specular_colr);
            ed.refl_props.specular_exp = *spex;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}


/* SET BACK REFLECTANCE PROPERTIES */
void
psbrfp_( amrc, dirc, sprc, ctype, coli, colr, spex)
Pfloat  *amrc;          /* ambient reflection coefficient       */
Pfloat  *dirc;          /* diffuse reflection coefficient       */
Pfloat  *sprc;          /* specular reflection coefficient      */
Pint    *ctype;         /* reflectance colour type      */
Pint    *coli;          /* colour index */
Pfloat  *colr;         /* colour components    */
Pfloat  *spex;          /* specular exponent    */
{
    Phg_args            cp_args;
    Phg_el_data         ed;
    Phg_args_add_el     *args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_back_area_prop)) {
        if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else {
            args->el_type = PELEM_BACK_REFL_PROPS;
            ed.refl_props.ambient_coef = *amrc;
            ed.refl_props.diffuse_coef = *dirc;
            ed.refl_props.specular_coef = *sprc;
            GCOLR_DATA_TRANSLATE( *ctype, *coli, colr,
                ed.refl_props.specular_colr);
            ed.refl_props.specular_exp = *spex;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
        }
    }
}
