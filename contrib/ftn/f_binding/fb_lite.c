#ifndef lint
static char     sccsid[] = "@(#)fb_lite.c 1.1 91/09/07 FJ";
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

/* Lighting functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"



/* SET INTERIOR REFLECTANCE EQUATION */
void
psire_( rfeq)
Pint	*rfeq;		/* reflectance equation	*/
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_refl_eqn)) {
    	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_INT_REFL_EQN;
	    ed.idata = *rfeq;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET BACK INTERIOR REFLECTANCE EQUATION */
void
psbire_( rfeq)
Pint	*rfeq;		/* back reflectance equation */
{
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_back_refl_eqn)) {
    	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);
	
	} else {
	    args->el_type = PELEM_BACK_INT_REFL_EQN;
	    ed.idata = *rfeq;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


static int
fb_list_indices_valid( nacti, alst, ndeai, dlst)
Pint		nacti;
register Pint	*alst;
Pint		ndeai;
register Pint	*dlst;
{
    register	int	i, j;

    /* Check for invalid index. */
    for ( i = 0; i < nacti; i++) {
        if ( alst[i] < 1)
            return ERR133;
    }
    for ( j = 0; j < ndeai; j++) {
        if ( dlst[j] < 1)
            return ERR133;
    }
    /* Check for common index. */
    for ( i = 0; i < nacti; i++) {
        for ( j = 0; j < ndeai; j++) {
            if ( alst[i] == dlst[j])
                return ERR135;
        }
    }
    return 0;
}


/* SET LIGHT SOURCE STATE */
void
pslss_( nacti, actlst, ndeai, dealst)
Pint	*nacti;		/* number of activation index	*/
Pint	*actlst;	/* activation list	*/
Pint	*ndeai;		/* number of deactivation index	*/
Pint	*dealst;	/* deactivation list */
{
    int			err;
    Phg_args		cp_args;
    Phg_el_data		ed;
    Phg_args_add_el	*args = &cp_args.data.add_el;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR5, Pfn_set_light_src_state)) {
    	if ( PSL_STRUCT_STATE( phg_cur_cph->psl) != PSTOP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR5);

        } else if ( *nacti < 0 || *ndeai < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);

	} else if ( err = fb_list_indices_valid(
            *nacti, actlst, *ndeai, dealst)) {
	    ERR_REPORT( phg_cur_cph->erh, err);

	} else {
	    args->el_type = PELEM_LIGHT_SRC_STATE;
	    ed.light_state.act_set.num_ints = *nacti;
	    ed.light_state.act_set.ints = actlst;
	    ed.light_state.deact_set.num_ints = *ndeai;
	    ed.light_state.deact_set.ints = dealst;
            if ( CB_BUILD_OC(args->el_type, &ed, &args->pex_oc) )
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_ADD_EL, &cp_args, NULL);
	}
    }
}


/* SET LIGHT SOURCE REPRESENTATION */
void
pslsr_( wkid, lsi, lstyp, ldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*lsi;		/* light source index	*/
Pint	*lstyp;		/* light source type	*/
Pint	*ldr;		/* length of data record	*/
char	*datrec;	/* data record	*/
{
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    register Wst_phigs_dt	*dt;
    Psl_ws_info			*wsinfo;
    Plight_src_bundle		*rep;
    Pdata_rec		r;
    int				i, colr_int, colr_float;
    int				err = 0;
    
    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR3, Pfn_set_light_src_rep)) {
        if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR3);
        
        } else if (!(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
            ERR_REPORT( phg_cur_cph->erh, ERR54);

        } else {
            dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
            if ( !( dt->ws_category == POUTIN || dt->ws_category == POUTPT
                || dt->ws_category == PMO) ) {
                ERR_REPORT( phg_cur_cph->erh, ERR59);

            } else if ( *lsi < 1) {
                ERR_REPORT( phg_cur_cph->erh, ERR129);

            } else if ( !CB_LIGHT_SRC_TYPE_SUPPORTED(*lstyp)) {
                ERR_REPORT( phg_cur_cph->erh, ERR131);

            } else if ( fb_unpack_datrec( ldr, datrec, &err, &r)) {
                ERR_REPORT( phg_cur_cph->erh, err);

            } else {
                args->wsid = *wkid;
                args->type = PHG_ARGS_LIGHTSRCREP;
                args->rep.index = *lsi;
                rep = &args->rep.bundl.lightsrcrep;
                rep->type = *lstyp;
                if ( r.data.il < 0) {
                    ERR_REPORT( phg_cur_cph->erh, ERR2003);

                } else if ( !CB_COLOUR_MODEL_SUPPORTED(r.data.ia[0])) {
                    ERR_REPORT( phg_cur_cph->erh, ERR110);

                } else {
                    switch ( r.data.ia[0]) {
                    case PINDCT:
                        colr_int = 2;
                        colr_float = 0;
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
                        colr_int = 1;
                        colr_float = 3;
                    }
                    switch ( *lstyp) {
                    case PLIGHT_AMBIENT:
                        if ( r.data.il != colr_int || r.data.rl != colr_float) {
                            err = ERR2003;

                        } else {
                            GCOLR_DATA_TRANSLATE( r.data.ia[0], r.data.ia[1],
                            r.data.ra, rep->rec.ambient.colr);
                        }
                        break;
                    case PLIGHT_DIRECTIONAL:
                        if ( r.data.il != colr_int ||
                            r.data.rl != (colr_float + 3)) { 
                            err = ERR2003;

                        } else {
                            rep->rec.directional.dir.delta_x = r.data.ra[0];
                            rep->rec.directional.dir.delta_y = r.data.ra[1];
                            rep->rec.directional.dir.delta_z = r.data.ra[2];
                            GCOLR_DATA_TRANSLATE( r.data.ia[0], r.data.ia[1],
                                &(r.data.ra[3]), rep->rec.directional.colr);
                        }
                        break;
                    case PLIGHT_POSITIONAL:
                        if ( r.data.il != colr_int ||
                            r.data.rl != (colr_float + 5)) { 
                            err = ERR2003;

                        } else {
                            rep->rec.positional.pos.x = r.data.ra[0];
                            rep->rec.positional.pos.y = r.data.ra[1];
                            rep->rec.positional.pos.z = r.data.ra[2];
                            rep->rec.positional.coef[0] = r.data.ra[3];
                            rep->rec.positional.coef[1] = r.data.ra[4];
                            GCOLR_DATA_TRANSLATE( r.data.ia[0], r.data.ia[1],
                                &(r.data.ra[5]), rep->rec.positional.colr);
                        }
                        break;
                    case PLIGHT_SPOT:
                        if ( r.data.il != colr_int ||
                            r.data.rl != (colr_float + 10)) { 
                            err = ERR2003;

                        } else  if ( r.data.ra[9] < 0 || r.data.ra[9] > M_PI) {
                            err = ERR132;

                        } else {
                            rep->rec.spot.pos.x = r.data.ra[0];
                            rep->rec.spot.pos.y = r.data.ra[1];
                            rep->rec.spot.pos.z = r.data.ra[2];
                            rep->rec.spot.dir.delta_x = r.data.ra[3];
                            rep->rec.spot.dir.delta_y = r.data.ra[4];
                            rep->rec.spot.dir.delta_z = r.data.ra[5];
                            rep->rec.spot.exp = r.data.ra[6];
                            rep->rec.spot.coef[0] = r.data.ra[7];
                            rep->rec.spot.coef[1] = r.data.ra[8];
                            rep->rec.spot.angle = r.data.ra[9];
                            GCOLR_DATA_TRANSLATE( r.data.ia[0], r.data.ia[1],
                                &(r.data.ra[10]), rep->rec.spot.colr);
                        }
                        break;
                    }
                    if ( err) {
                        ERR_REPORT( phg_cur_cph->erh, err);
                    } else {
                        CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args,
                            NULL);
                    }
                }
            }
			fb_del_datrec( &r);
        }
    }
}


/* INQUIRE LIGHT SOURCE REPRESENTATION */
void
pqlsr_( wkid, lsi, type, mldr, errind, lstype, ldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*lsi;		/* light source index	*/
Pint	*type;		/* type of returned value	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*lstype;	/* OUT ligth source type	*/
Pint	*ldr;		/* OUT length of data record	*/
char	*datrec;	/* OUT data record	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;
    Psl_ws_info		*wsinfo;
    Wst_phigs_dt	*dt;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	ia[2];		/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	ra[NUM_COLR_COMP+10]; /* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries	*/

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

	} else if ( *lsi < 1) {
	    *errind = ERR129;
 
	} else if ( *type < PSET || *type > PREALI) {
	    *errind = ERR2000;
 
	}
	if ( !*errind) {
	    cp_args.data.q_rep.wsid = *wkid;
	    cp_args.data.q_rep.index = *lsi;
	    cp_args.data.q_rep.type = (Pinq_type)*type;
	    cp_args.data.q_rep.rep_type = PHG_ARGS_LIGHTSRCREP;
            ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
			&ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else {
		Plight_src_bundle *rep = &ret.data.rep.lightsrcrep;

		*lstype = rep->type;
                switch ( *lstype) {
                case PLIGHT_AMBIENT:
                    GCOLR_DATA_RE_TRANSLATE( rep->rec.ambient.colr, il, ia, rl,
                        ra);
                    break;
                case PLIGHT_DIRECTIONAL:
	            ra[0] = rep->rec.directional.dir.delta_x;
	            ra[1] = rep->rec.directional.dir.delta_y;
	            ra[2] = rep->rec.directional.dir.delta_z;
                    rl = 3;
                    GCOLR_DATA_RE_TRANSLATE( rep->rec.directional.colr, il, ia,
                        rl, &(ra[3]));
                    break;
                case PLIGHT_POSITIONAL:
	            ra[0] = rep->rec.positional.pos.x;
	            ra[1] = rep->rec.positional.pos.y;
	            ra[2] = rep->rec.positional.pos.z;
	            ra[3] = rep->rec.positional.coef[0];
	            ra[4] = rep->rec.positional.coef[1];
                    rl = 5;
                    GCOLR_DATA_RE_TRANSLATE( rep->rec.positional.colr, il, ia,
                        rl, &(ra[5]));
                    break;
                case PLIGHT_SPOT:
	            ra[0] = rep->rec.spot.pos.x;
	            ra[1] = rep->rec.spot.pos.y;
	            ra[2] = rep->rec.spot.pos.z;
	            ra[3] = rep->rec.spot.dir.delta_x;
	            ra[4] = rep->rec.spot.dir.delta_y;
	            ra[5] = rep->rec.spot.dir.delta_z;
	            ra[6] = rep->rec.spot.exp;
	            ra[7] = rep->rec.spot.coef[0];
	            ra[8] = rep->rec.spot.coef[1];
	            ra[9] = rep->rec.spot.angle;
                    rl = 10;
                    GCOLR_DATA_RE_TRANSLATE( rep->rec.spot.colr, il, ia, rl,
                        &(ra[10]));
                    break;
                }
                fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr,
                    errind, ldr, datrec);
	    }
	}
    }
}
