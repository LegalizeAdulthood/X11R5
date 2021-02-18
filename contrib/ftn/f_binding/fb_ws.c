#ifndef lint
static char     sccsid[] = "@(#)fb_ws.c 1.1 91/09/07 FJ";
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

/* Workstation functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"

static Pconnid_x_drawable	conn_id;

static int
fb_valid_connection_id( wst, conid)
Wst         *wst;         /* workstation type       */
Pint        conid;        /* connection identifier  */
{
    int                 status = 0;
    XWindowAttributes   wattr;
    F_dy_id		disp;
    F_dw_id		conn;

    bzero( (char *)&conn_id, sizeof(Pconnid_x_drawable) );
    switch( wst->base_type ) {
    case WST_BASE_TYPE_X_TOOL:
        /* Nothing really to check.  The existence and PEX support of
         * the server is checked in lower level code.
         */
        if ( conid) {
            status = 0;
        } else {
            status = 1;
        }
        break;
    case WST_BASE_TYPE_X_DRAWABLE:
        /* Ensure the display pointer and drawable id are non-zero and
         * that the window exists.
         */
        if ( conn = phg_fb_search_connection( conid)) {
            if ( conn->disp_p && conn->wind_id && XGetWindowAttributes(
                conn->disp_p, conn->wind_id, &wattr ) ) {
                conn_id.display = conn->disp_p;
                conn_id.drawable_id = conn->wind_id;
                status = 1;
            }
        }
        break;
    }
    return status;
}


/* OPEN WORKSTATION */
void
popwk_( wkid, conid, wtype)
Pint    *wkid;		/* workstation identifier	*/
Pint	*conid;		/* connection identifier	*/
Pint	*wtype;		/* workstation type	*/
{
    Phg_args			cp_args;
    register Phg_args_open_ws	*args = &cp_args.data.open_ws;
    Phg_ret			ret;
    Wst				*wst;
    char			*work_conn;
    int				w;	/* not used */

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_open_ws)) {
        if ( phg_psl_inq_ws_open( phg_cur_cph->psl, *wkid)) {
            ERR_REPORT( phg_cur_cph->erh, ERR53);

        } else if ( !(wst = phg_fb_wst_exists( *wtype))) {
            ERR_REPORT( phg_cur_cph->erh, ERR52);

        } else if ( !fb_valid_connection_id( wst, (*conid))) {
            ERR_REPORT( phg_cur_cph->erh, ERR50);

        } else if ( !phg_psl_ws_free_slot( phg_cur_cph->psl)){
            ERR_REPORT( phg_cur_cph->erh, ERR63);

        } else {
            bzero( (char *)args, sizeof(*args));
            args->wsid = *wkid;
            args->type = wst;
            args->wst_display_name = wst->desc_tbl.xwin_dt.display_name;
            args->wst_display_name_length =
                wst->desc_tbl.xwin_dt.display_name_length;
            args->wst_buffer = wst->buffer;
            args->wst_buffer_size = wst->buffer_size;
            switch ( wst->base_type) {
            case WST_BASE_TYPE_X_DRAWABLE:
                args->conn_info.drawable_id = conn_id.drawable_id;
                args->conn_info.display = conn_id.display;
                args->conn_info.display_name =
                    DisplayString(args->conn_info.display);
                args->conn_info.display_name_length =
                    strlen( args->conn_info.display_name) + 1;
                work_conn = (char *)(&conn_id);
                break;
            case WST_BASE_TYPE_X_TOOL:
                args->conn_info.display_name =
                    wst->desc_tbl.xwin_dt.display_name;
                args->conn_info.display_name_length =
                    strlen( args->conn_info.display_name) + 1;
                work_conn = (char *)NULL;
                break;
            }

            ret.err = 0;
            CP_FUNC( phg_cur_cph, CP_FUNC_OP_OPEN_WS, &cp_args, &ret);

			if ( !ret.err) {
                if ( !phg_cp_add_wst( phg_cur_cph, ret.data.open_ws.wstype)) {
                    cp_args.data.idata = *wkid;
                    CP_FUNC( phg_cur_cph, CP_FUNC_OP_CLOSE_WS, &cp_args, NULL);
                    ERR_REPORT( phg_cur_cph->erh, ERR900);

                } else if ( !phg_fb_add_ws_type( ret.data.open_ws.wstype, &w)) {
                    cp_args.data.idata = *wkid;
                    CP_FUNC( phg_cur_cph, CP_FUNC_OP_CLOSE_WS, &cp_args, NULL);
                    ERR_REPORT( phg_cur_cph->erh, ERR900);

                } else {
                    (void)phg_psl_add_ws( phg_cur_cph->psl, *wkid,
                        work_conn, ret.data.open_ws.wstype);
                    PSL_WS_STATE( phg_cur_cph->psl) = PWSOP;
                }
            }
            ERR_FLUSH( phg_cur_cph->erh);
        }
    }
}



/* CLOSE WORKSTATION */
void
pclwk_( wkid)
Pint	*wkid;		/* workstation identifier       */
{
    Phg_args		cp_args;
    Psl_ws_info		*ws;

    if ( ws = phg_cb_ws_open( phg_cur_cph, *wkid, Pfn_close_ws)) {
        phg_cp_rmv_wst( phg_cur_cph, ws->wstype);
        cp_args.data.idata = *wkid;
        CP_FUNC( phg_cur_cph, CP_FUNC_OP_CLOSE_WS, &cp_args, NULL);
        /* psl_rem_ws() sets the ws state to WSCL if no more ws's open */
        phg_psl_rem_ws( phg_cur_cph->psl, *wkid);
        phg_fb_delete_ws_type( ws->wstype);
    }
}


/* MESSAGE */
void
fmsg_( wkid, mess, length)
Pint	*wkid;		/* workstation identifier	*/
char	*mess;		/* message string	*/
int	*length;        /* string length        */
{
    Phg_args		cp_args;
    Phg_args_message	*args = &cp_args.data.message;

    if ( phg_cb_ws_open( phg_cur_cph, *wkid, Pfn_message)) {
	args->wsid = *wkid;
	if ( args->msg = mess) {
	    args->msg_length = *length + 1;
	} else {
	    args->msg_length = 0;
        }
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_MESSAGE, &cp_args, NULL);
    }
}


/* POST STRUCTURE */
void
ppost_( wkid, strid, priort)
Pint	*wkid;		/* workstation identifier	*/
Pint	*strid;		/* structure identifier	*/
Pfloat	*priort;	/* prioriyty	*/
{
    Phg_args			cp_args;
    Phg_args_post_struct	*args = &cp_args.data.post_struct;
    Psl_ws_info			*wsinfo;
    Wst_phigs_dt		*dt;

    if ( wsinfo = phg_cb_ws_open( phg_cur_cph, *wkid, Pfn_post_struct) ) {
        dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
        if ( !(dt->ws_category == POUTPT ||
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO )) {
	    ERR_REPORT(phg_cur_cph->erh, ERR59);
	 
        } else if ( *priort >= 0.0 && *priort <=1.0 ){
	    args->wsid = *wkid;
	    args->struct_id = *strid;
	    args->disp_pri = *priort;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_POST_STRUCT, &cp_args, NULL);
        } else {
		ERR_REPORT( phg_cur_cph->erh, ERR208);
		}
    }
}


/* UNPOST STRUCTURE */
void
pupost_( wkid, strid)
Pint	*wkid;		/* workstation identifier	*/
Pint	*strid;		/* structure identifier	*/
{
    Phg_args			cp_args;
    Phg_args_unpost_struct	*args = &cp_args.data.unpost_struct;
    Psl_ws_info			*wsinfo;
    Wst_phigs_dt		*dt;

    if ( wsinfo = phg_cb_ws_open( phg_cur_cph, *wkid, Pfn_unpost_struct) ) {
        dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
        if ( !(dt->ws_category == POUTPT ||
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO )) {
	    ERR_REPORT(phg_cur_cph->erh, ERR59);
	 
        } else {
	    args->wsid = *wkid;
	    args->struct_id = *strid;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_UNPOST_STRUCT, &cp_args, NULL);
        }
    }
}


/* UNPOST ALL STRUCTURE */
void
pupast_( wkid)
Pint	*wkid;		/* workstation identifier	*/
{
    Phg_args			cp_args;
    Psl_ws_info			*wsinfo;
    Wst_phigs_dt		*dt;

    if ( wsinfo = phg_cb_ws_open( phg_cur_cph, *wkid, Pfn_unpost_all_structs)) {
        dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
        if ( !(dt->ws_category == POUTPT ||
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO )) {
	    ERR_REPORT(phg_cur_cph->erh, ERR59);
	 
        } else {
	    cp_args.data.idata = *wkid;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_UNPOST_ALL, &cp_args, NULL);
        }
    }
}


/* REDRAW ALL STRUCTURES */
void
prst_( wkid, cofl)
Pint	*wkid;		/* workstation identifier	*/
Pint	*cofl;		/* control flag	*/
{
    Phg_args			cp_args;
    Phg_args_ws_redraw_all	*args = &cp_args.data.ws_redraw_all;
    Psl_ws_info			*wsinfo;
    Wst_phigs_dt		*dt;

    if ( wsinfo = phg_cb_ws_open( phg_cur_cph, *wkid, Pfn_redraw_all_structs)) {
        dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( !(dt->ws_category == POUTPT ||
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO )) {
	    ERR_REPORT(phg_cur_cph->erh, ERR59);
	 
        } else if ( *cofl < PCONDI || *cofl > PALWAY) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2000);
        
        } else {
	    args->wsid = *wkid;
	    args->flag = (Pctrl_flag)*cofl;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_WS_REDRAW_ALL, &cp_args, NULL);
        }
    }
}


/* UPDATE WORKSTATION */
void
puwk_( wkid, regfl)
Pint	*wkid;		/* workstation identifier	*/
Pint	*regfl;		/* regeneration flag	*/
{
    Phg_args			cp_args;
    Phg_args_ws_update		*args = &cp_args.data.ws_update;
    Psl_ws_info			*wsinfo;
    Wst_phigs_dt		*dt;

    if ( wsinfo = phg_cb_ws_open( phg_cur_cph, *wkid, Pfn_upd_ws)) {
        dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
        if ( !(dt->ws_category == POUTPT ||
               dt->ws_category == POUTIN ||
               dt->ws_category == PMO )) {
            ERR_REPORT(phg_cur_cph->erh, ERR59);
         
        } else if ( *regfl < PPOSTP || *regfl > PPERFO) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);
        
        } else {
            args->wsid = *wkid;
            args->flag = (Pregen_flag)*regfl;
            CP_FUNC( phg_cur_cph, CP_FUNC_OP_WS_UPDATE, &cp_args, NULL);
        }
    }
}


/* SET DISPLAY UPDATE STATE */
void
psdus_( wkid, defmod, modmod)
Pint	*wkid;		/* workstation identifier	*/
Pint	*defmod;	/* deferral mode	*/
Pint	*modmod;	/* modification mode	*/
{
    Phg_args			cp_args;
    Phg_args_set_disp_state	*args = &cp_args.data.set_disp_state;
    Psl_ws_info			*wsinfo;
    Wst_phigs_dt		*dt;

    if ( wsinfo = phg_cb_ws_open( phg_cur_cph, *wkid, Pfn_set_disp_upd_st)) {
        dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
        if ( !(dt->ws_category == POUTPT ||
               dt->ws_category == POUTIN ||
               dt->ws_category == PMO )) {
            ERR_REPORT(phg_cur_cph->erh, ERR59);
         
        } else if (( *defmod < PASAP || *defmod > PWAITD) ||
                   ( *modmod < PNIVE || *modmod > PUQUM)) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);
        
        } else {
            args->wsid = *wkid;
            args->mode = (Pdefer_mode)*defmod;
            args->mod_mode = (Pmod_mode)*modmod;
            CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_DISP_STATE, &cp_args, NULL);
        }
    }
}

static int
valid_clip_box( xy, f, b, lim)
    Pint	xy, f, b;
    register Plimit3    *lim;
{
    /* Don't check the limit unless the corresponding clip plane is on. */

    if ( xy == PCLIP) {
        if (   !CB_IN_RANGE( PDT_NPC_XMIN, PDT_NPC_XMAX, lim->x_min)
            || !CB_IN_RANGE( PDT_NPC_XMIN, PDT_NPC_XMAX, lim->x_max)
            || !CB_IN_RANGE( PDT_NPC_YMIN, PDT_NPC_YMAX, lim->y_min)
            || !CB_IN_RANGE( PDT_NPC_YMIN, PDT_NPC_YMAX, lim->y_max)) {
            ERR_REPORT( phg_cur_cph->erh, ERR154);
            return 0;

        } else if ( !( lim->x_min < lim->x_max  && lim->y_min < lim->y_max)) {
            ERR_REPORT( phg_cur_cph->erh, ERR153);
            return 0;
        }
    }

    if ( f == PCLIP) {
        if ( !CB_IN_RANGE( PDT_NPC_ZMIN, PDT_NPC_ZMAX, lim->z_max)) {
            ERR_REPORT( phg_cur_cph->erh, ERR154);
            return 0;
        }
    }

    if ( b == PCLIP) {
        if ( !CB_IN_RANGE( PDT_NPC_ZMIN, PDT_NPC_ZMAX, lim->z_min)) {
            ERR_REPORT( phg_cur_cph->erh, ERR154);
            return 0;

        } else if ( f == PCLIP) {
            if ( !( lim->z_min <= lim->z_max)) {
                ERR_REPORT( phg_cur_cph->erh, ERR153);
                return 0;
            }
        }
    }
    return 1;
}


/* SET VIEW REPRESENTATION 3 */
void
psvwr3_( wkid, viewi, vwormt, vwmpmt, vwcplm, xyclpi, bclipi, fclipi)
Pint		*wkid;		/* workstation id	*/
Pint		*viewi;		/* view index	*/
Pmatrix3	vwormt;		/* view orientation matorix	*/
Pmatrix3	vwmpmt;		/* view mapping matorix	*/
Plimit3		*vwcplm;	/* view clipping limits	*/
Pint		*xyclpi;	/* view x-y clipping indicator	*/
Pint		*bclipi;	/* view back clipping indicator	*/
Pint		*fclipi;	/* view front clipping indicator	*/
{
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    Psl_ws_info			*wsinfo;
    Wst_phigs_dt		*dt;

    if ( wsinfo = phg_cb_ws_open( phg_cur_cph, *wkid, Pfn_set_view_rep3)) {
        dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
        if ( dt->ws_category == PMI) {
            ERR_REPORT( phg_cur_cph->erh, ERR57);

        } else if ( *viewi < 1) {
            ERR_REPORT( phg_cur_cph->erh, ERR115);

        } else if ( *viewi >= dt->num_view_indices) {
            ERR_REPORT( phg_cur_cph->erh, ERR150);

        } else if (( *xyclpi < PNCLIP || *xyclpi > PCLIP) ||
                   ( *bclipi < PNCLIP || *bclipi > PCLIP) ||
                   ( *fclipi < PNCLIP || *fclipi > PCLIP)) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else if ( valid_clip_box( *xyclpi, *fclipi, *bclipi, vwcplm)) {

            args->wsid = *wkid;
            args->type = PHG_ARGS_VIEWREP;
            args->rep.index = *viewi;
            MATRIX_DATA_TRANSLATE( 3, vwormt,
                args->rep.bundl.viewrep.ori_matrix);
            MATRIX_DATA_TRANSLATE( 3, vwmpmt,
                args->rep.bundl.viewrep.map_matrix);
            args->rep.bundl.viewrep.clip_limit = *vwcplm;
            args->rep.bundl.viewrep.xy_clip = (Pclip_ind)*xyclpi;
            args->rep.bundl.viewrep.back_clip = (Pclip_ind)*bclipi;
            args->rep.bundl.viewrep.front_clip = (Pclip_ind)*fclipi;
            CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
        }
    }
}

static int
valid_clip_rect( xy, lim)
    Pint	xy;
    register Plimit     *lim;
{
    if ( xy == PCLIP) {
        if (   !CB_IN_RANGE( PDT_NPC_XMIN, PDT_NPC_XMAX, lim->x_min)
            || !CB_IN_RANGE( PDT_NPC_XMIN, PDT_NPC_XMAX, lim->x_max)
            || !CB_IN_RANGE( PDT_NPC_YMIN, PDT_NPC_YMAX, lim->y_min)
            || !CB_IN_RANGE( PDT_NPC_YMIN, PDT_NPC_YMAX, lim->y_max)) {
            ERR_REPORT( phg_cur_cph->erh, ERR154);
            return 0;

        } else if ( !( lim->x_min < lim->x_max  && lim->y_min < lim->y_max)) {
            ERR_REPORT( phg_cur_cph->erh, ERR153);
            return 0;
        }
    }
    return 1;
}

static void
expand_transform( m2, m3)
    register Pmatrix    m2;
    register Pmatrix3   m3;
{
    m3[2][2] = 1.0;
    m3[0][2] = m3[1][2] = m3[2][0] = m3[2][1] = m3[2][3] = m3[3][2] = 0.0;
    m3[0][0] = m2[0][0]; m3[0][1] = m2[0][1]; m3[0][3] = m2[0][2];
    m3[1][0] = m2[1][0]; m3[1][1] = m2[1][1]; m3[1][3] = m2[1][2];
    m3[3][0] = m2[2][0]; m3[3][1] = m2[2][1]; m3[3][3] = m2[2][2];
}


/* SET VIEW REPRESENTATION */
void
psvwr_( wkid, viewi, vwormt, vwmpmt, vwcplm, xyclpi)
Pint		*wkid;		/* workstation id	*/
Pint		*viewi;		/* view index	*/
Pmatrix		vwormt;		/* view orientation matrix	*/
Pmatrix		vwmpmt;		/* view mapping matrix	*/
Plimit		*vwcplm;	/* view clipping limits	*/
Pint		*xyclpi;	/* view x-y clipping indicator	*/
{
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    Psl_ws_info			*wsinfo;
    Wst_phigs_dt		*dt;
    Pmatrix		om;
    Pmatrix		mm;

    if ( wsinfo = phg_cb_ws_open( phg_cur_cph, *wkid, Pfn_set_view_rep)) {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( dt->ws_category == PMI) {
	    ERR_REPORT( phg_cur_cph->erh, ERR57);

        } else if ( *viewi < 1) {
            ERR_REPORT( phg_cur_cph->erh, ERR115);

        } else if ( *viewi >= dt->num_view_indices) {
            ERR_REPORT( phg_cur_cph->erh, ERR150);

        } else if ( *xyclpi < PNCLIP || *xyclpi > PCLIP) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else if ( valid_clip_rect( *xyclpi, vwcplm)) {
            register Pview_rep3		*r = &args->rep.bundl.viewrep;

            /* Copy and expand the 2d rep to 3d. */
            r->clip_limit.x_min = vwcplm->x_min;
            r->clip_limit.x_max = vwcplm->x_max;
            r->clip_limit.y_min = vwcplm->y_min;
            r->clip_limit.y_max = vwcplm->y_max;
	    r->clip_limit.z_min = PDT_NPC_ZMIN;
	    r->clip_limit.z_max = PDT_NPC_ZMAX;
	    r->xy_clip = (Pclip_ind)*xyclpi;
	    r->front_clip = PCLIP;
	    r->back_clip = PCLIP;
            MATRIX_DATA_TRANSLATE( 2, vwormt, om);
	    expand_transform( om, r->ori_matrix);
            MATRIX_DATA_TRANSLATE( 2, vwmpmt, mm);
	    expand_transform( mm, r->map_matrix);

	    args->wsid = *wkid;
	    args->type = PHG_ARGS_VIEWREP;
	    args->rep.index = *viewi;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
	}
    }
}


/* SET VIEW TRANSFORMATION INPUT PRIORITY */
void
psvtip_( wkid, viewi, rfvwix, relpri)
Pint	*wkid;		/* workstation id	*/
Pint	*viewi;		/* view index	*/
Pint	*rfvwix;	/* reference view index	*/
Pint	*relpri;	/* relative priority	*/
{
    Phg_args			cp_args;
    register Phg_args_set_view_input_prio 
				*args = &cp_args.data.set_view_input_prio;
    Psl_ws_info			*wsinfo;
    Wst_phigs_dt		*dt;

    if (wsinfo = phg_cb_ws_open(phg_cur_cph, *wkid, Pfn_set_view_tran_in_pri)) {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( dt->ws_category == PMI) {
	    ERR_REPORT( phg_cur_cph->erh, ERR57);

	} else if ( *viewi < 0 || *rfvwix < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR114);

	} else if ( *viewi >= dt->num_view_indices) {
	    ERR_REPORT( phg_cur_cph->erh, ERR101);

	} else if ( *rfvwix >= dt->num_view_indices) {
	    ERR_REPORT( phg_cur_cph->erh, ERR101);

	} else if ( *relpri < PHIGHR || *relpri > PLOWER) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else if ( *rfvwix != *viewi) {
	    args->wsid = *wkid;
	    args->idx = *viewi;
	    args->ref_idx = *rfvwix;
	    args->priority = (Prel_pri)*relpri;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_VIEW_INPUT_PRIO, &cp_args,
	       NULL);
	}
    }
}


/* SET WORKSTATION WINDOW 3 */
void
pswkw3_( wkid, wkwn)
Pint	*wkid;		/* workstation id	*/
Plimit3	*wkwn;		/* workstation window limits	*/
{
    Phg_args				cp_args;
    register Phg_args_set_ws_winvp	*args = &cp_args.data.set_ws_winvp;
    Psl_ws_info				*wsinfo;

    if ( wsinfo = phg_cb_ws_open( phg_cur_cph, *wkid, Pfn_set_ws_win3)) {
	if ( ((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt.ws_category == PMI) {
	    ERR_REPORT( phg_cur_cph->erh, ERR57);

	} else if ( !CB_IN_RANGE( PDT_NPC_XMIN, PDT_NPC_XMAX, wkwn->x_min)
	    || !CB_IN_RANGE( PDT_NPC_XMIN, PDT_NPC_XMAX, wkwn->x_max)
	    || !CB_IN_RANGE( PDT_NPC_YMIN, PDT_NPC_YMAX, wkwn->y_min)
	    || !CB_IN_RANGE( PDT_NPC_YMIN, PDT_NPC_YMAX, wkwn->y_max)
	    || !CB_IN_RANGE( PDT_NPC_ZMIN, PDT_NPC_ZMAX, wkwn->z_min)
	    || !CB_IN_RANGE( PDT_NPC_ZMIN, PDT_NPC_ZMAX, wkwn->z_max) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR156);

	} else if ( !(wkwn->x_min < wkwn->x_max) ||
                    !(wkwn->y_min < wkwn->y_max) ||
                    !(wkwn->z_min <= wkwn->z_max) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR151);

	} else {
	    args->wsid = *wkid;
	    args->two_d = 0;
	    args->limits = *wkwn;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_WS_WIN, &cp_args, NULL);
	}
    }
}


/* SET WORKSTATION WINDOW */
void
pswkw_( wkid, xmin, xmax, ymin, ymax)
Pint	*wkid;		/* workstation id	*/
Pfloat	*xmin;		/* workstation window limits X-MIN	*/
Pfloat	*xmax;		/* workstation window limits X-MAN	*/
Pfloat	*ymin;		/* workstation window limits Y-MIN	*/
Pfloat	*ymax;		/* workstation window limits Y-MAX	*/
{
    Phg_args				cp_args;
    register Phg_args_set_ws_winvp	*args = &cp_args.data.set_ws_winvp;
    Psl_ws_info				*wsinfo;

    if ( wsinfo = phg_cb_ws_open( phg_cur_cph, *wkid, Pfn_set_ws_win)) {
	if ( ((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt.ws_category == PMI) {
	    ERR_REPORT( phg_cur_cph->erh, ERR57);

	} else if ( !CB_IN_RANGE( PDT_NPC_XMIN, PDT_NPC_XMAX, *xmin)
	    || !CB_IN_RANGE( PDT_NPC_XMIN, PDT_NPC_XMAX, *xmax)
	    || !CB_IN_RANGE( PDT_NPC_YMIN, PDT_NPC_YMAX, *ymin)
	    || !CB_IN_RANGE( PDT_NPC_YMIN, PDT_NPC_YMAX, *ymax)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR156);

	} else if ( !(*xmin < *xmax) || !(*ymin < *ymax)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR151);

	} else {
	    args->wsid = *wkid;
	    args->two_d = ~0;
	    args->limits.x_min = *xmin;
	    args->limits.x_max = *xmax;
	    args->limits.y_min = *ymin;
	    args->limits.y_max = *ymax;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_WS_WIN, &cp_args, NULL);
	}
    }
}


/* SET WORKSTATION VIEWPORT 3 */
void
pswkv3_( wkid, wkvp)
Pint	*wkid;		/* workstation id	*/
Plimit3	*wkvp;		/* workstation viewport limits	*/
{
    Phg_args				cp_args;
    register Phg_args_set_ws_winvp	*args = &cp_args.data.set_ws_winvp;
    Psl_ws_info				*wsinfo;
    Wst_phigs_dt			*dt;

    if ( wsinfo = phg_cb_ws_open( phg_cur_cph, *wkid, Pfn_set_ws_vp3)) {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	phg_cb_update_DC_size( wsinfo);
	if ( dt->ws_category == PMI) {
	    ERR_REPORT( phg_cur_cph->erh, ERR57);

	} else if ( !phg_cur_cph->flags.ignore_DC_errors
	    && (!CB_IN_RANGE( 0.0, dt->dev_coords[0], wkvp->x_min)
	    ||  !CB_IN_RANGE( 0.0, dt->dev_coords[0], wkvp->x_max)
	    ||  !CB_IN_RANGE( 0.0, dt->dev_coords[1], wkvp->y_min)
	    ||  !CB_IN_RANGE( 0.0, dt->dev_coords[1], wkvp->y_max)
	    ||  !CB_IN_RANGE( 0.0, dt->dev_coords[2], wkvp->z_min)
	    ||  !CB_IN_RANGE( 0.0, dt->dev_coords[2], wkvp->z_max)) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR157);

	} else if ( !(wkvp->x_min < wkvp->x_max) ||
                    !(wkvp->y_min < wkvp->y_max) ||
                    !(wkvp->z_min <= wkvp->z_max) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR152);

	} else {
	    args->wsid = *wkid;
	    args->two_d = 0;
	    args->limits = *wkvp;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_WS_VP, &cp_args, NULL);
	}
    }
}


/* SET WORKSTATION VIEWPORT */
void
pswkv_( wkid, xmin, xmax, ymin, ymax)
Pint	*wkid;		/* workstation id	*/
Pfloat	*xmin;		/* workstation viewport limits X-MIN	*/
Pfloat	*xmax;		/* workstation viewport limits X-MAX	*/
Pfloat	*ymin;		/* workstation viewport limits Y-MIN	*/
Pfloat	*ymax;		/* workstation viewport limits Y-MAX	*/
{
    Phg_args				cp_args;
    register Phg_args_set_ws_winvp	*args = &cp_args.data.set_ws_winvp;
    Psl_ws_info				*wsinfo;
    Wst_phigs_dt		*dt;

    if ( wsinfo = phg_cb_ws_open( phg_cur_cph, *wkid, Pfn_set_ws_vp)) {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	phg_cb_update_DC_size( wsinfo);
	if ( dt->ws_category == PMI) {
	    ERR_REPORT( phg_cur_cph->erh, ERR57);

	} else if ( !phg_cur_cph->flags.ignore_DC_errors
	    && (!CB_IN_RANGE( 0.0, dt->dev_coords[0], *xmin)
	    ||  !CB_IN_RANGE( 0.0, dt->dev_coords[0], *xmax)
	    ||  !CB_IN_RANGE( 0.0, dt->dev_coords[1], *ymin)
	    ||  !CB_IN_RANGE( 0.0, dt->dev_coords[1], *ymax)) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR157);

	} else if ( !(*xmin < *xmax) || !(*ymin < *ymax)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR152);

	} else {
	    args->wsid = *wkid;
	    args->two_d = ~0;
	    args->limits.x_min = *xmin;
	    args->limits.x_max = *xmax;
	    args->limits.y_min = *ymin;
	    args->limits.y_max = *ymax;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_WS_VP, &cp_args, NULL);
	}
    }
}

static void
fb_set_up_filter( ws, isn, is, esn, es, filter_type, function_code)
Pint	ws;			/* workstation identifier	*/
Pint	isn;			/* number of elements in the inclusion set */
Pint	*is;			/* inclusion set	*/
Pint	esn;			/* number of elements in the exclusion set */
Pint	*es;			/* exclusion set	*/
Phg_args_flt_type filter_type;	/* which filter type?	*/
int 	function_code;		/* Which Pfn_ code?	*/
{
    Phg_args				cp_args;
    Psl_ws_info				*wsinfo;
    register Phg_args_set_filter	*args = &cp_args.data.set_filter;
    Wst_phigs_dt			*dt;

    if ( wsinfo = phg_cb_ws_open( phg_cur_cph, ws, function_code)) {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( !(dt->ws_category == POUTPT ||
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO )) {
	    ERR_REPORT(phg_cur_cph->erh, ERR59);

	} else  if ( isn < 0 || esn < 0) {
            ERR_REPORT( phg_cur_cph->erh, ERR2004);
        
	} else {
	    args->wsid = ws;
	    args->type = filter_type;
	    args->inc_set.num_ints = isn;
	    args->inc_set.ints = is;
	    args->exc_set.num_ints = esn;
	    args->exc_set.ints = es;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_FILTER, &cp_args, NULL);
	}
    }
}


/* SET HIGHLIGHTING FILTER */
void
pshlft_( wkid, isn, is, esn, es)
Pint	*wkid;		/* workstation identifier	*/
Pint	*isn;		/* number of elements in the inclusion set	*/
Pint	*is;		/* inclusion set	*/
Pint	*esn;		/* number of elements in the exclusion set	*/
Pint	*es;		/* exclusion set	*/
{
    fb_set_up_filter( *wkid, *isn, is, *esn, es, PHG_ARGS_FLT_HIGH,
       Pfn_set_highl_filter);
}


/* SET INVISIBILITY FILTER */
void
psivft_( wkid, isn, is, esn, es)
Pint	*wkid;		/* workstation identifier	*/
Pint	*isn;		/* number of elements in the inclusion set	*/
Pint	*is;		/* inclusion set	*/
Pint	*esn;		/* number of elements in the exclusion set	*/
Pint	*es;		/* exclusion set	*/
{
    fb_set_up_filter( *wkid, *isn, is, *esn, es, PHG_ARGS_FLT_INVIS,
        Pfn_set_invis_filter);
}


/* SET HLHSR MODE */
void
pshrm_( wkid, hrm)
Pint	*wkid;		/* workstation id	*/
Pint	*hrm;		/* HLHSR mode */
{
    Phg_args				cp_args;
    register Phg_args_set_hlhsr_mode	*args = &cp_args.data.set_hlhsr_mode;
    Psl_ws_info				*wsinfo;
    Wst_phigs_dt			*dt;
    int					i;

    if ( wsinfo = phg_cb_ws_open( phg_cur_cph, *wkid, Pfn_set_hlhsr_mode)) {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( !(dt->ws_category == POUTPT ||
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO )) {
	    ERR_REPORT(phg_cur_cph->erh, ERR59);
	} 
	for ( i = 0; i < dt->num_hlhsr_modes; i++) {
	    if ( *hrm == dt->hlhsr_modes[i]) {
		args->wsid = *wkid;
		args->mode = *hrm;
		CP_FUNC(phg_cur_cph, CP_FUNC_OP_SET_HLHSR_MODE, &cp_args,NULL);
		return;
	    }
	}
	/* if we get to here, the mode is not available on this workstation */
	ERR_REPORT(phg_cur_cph->erh, ERR111);
    }
}


/* SET POLYLINE REPRESENTATION */
void
psplr_( wkid, pli, ltype, lwidth, coli)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pli;		/* polyline index	*/
Pint	*ltype;		/* line type	*/
Pfloat	*lwidth;	/* linewidth scale factor	*/
Pint	*coli;		/* polyline colour index	*/
{
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    register Wst_phigs_dt	*dt;
    
    if ( dt = phg_cb_check_set_rep( phg_cur_cph, Pfn_set_line_rep, *wkid, *pli,
	*coli)) {

	if ( !phg_cb_int_in_list( *ltype,
            dt->out_dt.num_linetypes, dt->out_dt.linetypes)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR104);

	} else {
	    args->wsid = *wkid;
	    args->type = PHG_ARGS_LNREP;
	    args->rep.index = *pli;
	    args->rep.bundl.lnrep.type = *ltype;
	    args->rep.bundl.lnrep.width = *lwidth;
	    args->rep.bundl.lnrep.colr_ind = *coli;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
	}
    }
}


/* SET POLYLINE REPRESENTATION PLUS */
void
psplrp_( wkid, pli, ltype, lwidth, ctype, coli, colr, plsm, acri, aval)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pli;		/* polyline index	*/
Pint	*ltype;		/* linetype	*/
Pfloat	*lwidth;	/* linewidth scale factor	*/
Pint	*ctype;		/* polyline colour type	*/
Pint	*coli;		/* polyline colour index	*/
Pfloat	*colr;		/* polyline colour components	*/
Pint	*plsm;		/* polyline sharing method	*/
Pint	*acri;		/* curve approximation criteria type	*/
Pfloat	*aval;		/* curve approximation criteria values	*/
{
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    register Wst_phigs_dt	*dt;
    
    if ( dt = phg_cb_check_set_rep( phg_cur_cph, Pfn_set_line_rep_plus, *wkid,
        *pli, *ctype == PINDCT ? *coli : 1 ) ) {

	if ( !phg_cb_int_in_list( *ltype,
	    dt->out_dt.num_linetypes, dt->out_dt.linetypes)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR104);

	} else if ( !CB_COLOUR_MODEL_SUPPORTED(*ctype)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR110);

	} else if ( !CB_LINE_SHADING_SUPPORTED(*plsm)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR122);

	} else {
	    args->wsid = *wkid;
	    args->type = PHG_ARGS_EXTLNREP;
	    args->rep.index = *pli;
	    args->rep.bundl.extlnrep.type = *ltype;
	    args->rep.bundl.extlnrep.width = *lwidth;
            GCOLR_DATA_TRANSLATE( *ctype, *coli, colr,
                args->rep.bundl.extlnrep.colr);
	    args->rep.bundl.extlnrep.shad_meth = *plsm;
	    args->rep.bundl.extlnrep.approx_type = *acri;
	    args->rep.bundl.extlnrep.approx_val = *aval;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
	}
    }
}


/* SET POLYMARKER REPRESENTATION */
void
pspmr_( wkid, pmi, mtype, mszsf, coli)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pmi;		/* polymarker index	*/
Pint	*mtype;		/* marker type	*/
Pfloat	*mszsf;		/* marker size scale factor	*/
Pint	*coli;		/* polymarker colour index	*/
{
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    register Wst_phigs_dt	*dt;
    
    if ( dt = phg_cb_check_set_rep( phg_cur_cph, Pfn_set_marker_rep, *wkid,
        *pmi, *coli)) {

	if ( !phg_cb_int_in_list( *mtype,
	    dt->out_dt.num_marker_types, dt->out_dt.marker_types)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR105);

	} else {
	    args->wsid = *wkid;
	    args->type = PHG_ARGS_MKREP;
	    args->rep.index = *pmi;
            args->rep.bundl.mkrep.type = *mtype;
            args->rep.bundl.mkrep.size = *mszsf;
            args->rep.bundl.mkrep.colr_ind = *coli;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
	}
    }
}


/* SET POLYMARKER REPRESENTATION PLUS */
void
pspmrp_( wkid, pmi, mtype, mszsf, ctype, coli, colr)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pmi;		/* polymarker index	*/
Pint	*mtype;		/* marker type	*/
Pfloat	*mszsf;		/* marker size scale factor	*/
Pint	*ctype;		/* polymarker colour type	*/
Pint	*coli;		/* polymarker colour index	*/
Pfloat	*colr;		/* polymarker colour components	*/
{
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    register Wst_phigs_dt	*dt;
    
    if ( dt = phg_cb_check_set_rep( phg_cur_cph, Pfn_set_marker_rep_plus, *wkid,
        *pmi, *ctype == PINDCT ? *coli : 1)) {

	if ( !phg_cb_int_in_list( *mtype,
	    dt->out_dt.num_marker_types, dt->out_dt.marker_types)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR105);

	} else if ( !CB_COLOUR_MODEL_SUPPORTED(*ctype)) {
	    ERR_REPORT(phg_cur_cph->erh, ERR110);

	} else {
	    args->wsid = *wkid;
	    args->type = PHG_ARGS_EXTMKREP;
	    args->rep.index = *pmi;
	    args->rep.bundl.extmkrep.type = *mtype;
	    args->rep.bundl.extmkrep.size = *mszsf;
            GCOLR_DATA_TRANSLATE( *ctype, *coli, colr,
                args->rep.bundl.extmkrep.colr);
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
	}
    }
}


/* SET TEXT REPRESENTATION */
void
pstxr_( wkid, txi, font, prec, chxp, chsp, coli)
Pint	*wkid;		/* workstation identifier	*/
Pint	*txi;		/* text index	*/
Pint	*font;		/* text font	*/
Pint	*prec;		/* text precision	*/
Pfloat	*chxp;		/* character expansion factor	*/
Pfloat	*chsp;		/* character spacing	*/ 
Pint	*coli; 		/* text colour index	*/
{
    int				phg_cb_valid_text_pair();
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    register Wst_phigs_dt	*dt;

    if ( dt = phg_cb_check_set_rep( phg_cur_cph, Pfn_set_text_rep, *wkid, *txi,
	*coli)) {

	if ( *prec < PSTRP ||  *prec > PSTRKP) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

	} else if ( !phg_cb_valid_text_pair( *font, *prec,
	    dt->out_dt.num_text_pairs[0], dt->out_dt.text_pairs[0])) {
	    ERR_REPORT( phg_cur_cph->erh, ERR106);

        } else {
            args->wsid = *wkid;
	    args->type = PHG_ARGS_TXREP;
	    args->rep.index = *txi;
	    args->rep.bundl.txrep.font = *font;
            args->rep.bundl.txrep.prec = (Ptext_prec)*prec;
            args->rep.bundl.txrep.char_expan = *chxp;
            args->rep.bundl.txrep.char_space = *chsp;
            args->rep.bundl.txrep.colr_ind = *coli;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
	}
    }
}


/* SET TEXT REPRESENTATION PLUS */
void
pstxrp_( wkid, txi, font, prec, chxp, chsp, ctype, coli, colr)
Pint	*wkid;		/* workstation identifier	*/
Pint	*txi;		/* text index	*/
Pint	*font;		/* text font	*/
Pint	*prec;		/* text precision	*/
Pfloat	*chxp;		/* character expansion factor	*/
Pfloat	*chsp;		/* character spacing	*/
Pint	*ctype;		/* text colour type	*/
Pint	*coli;		/* text colour index	*/
Pfloat	*colr;		/* text colour components	*/
{
    int				phg_cb_valid_text_pair();
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    register Wst_phigs_dt	*dt;

    if ( dt = phg_cb_check_set_rep( phg_cur_cph, Pfn_set_text_rep_plus, *wkid,
       	*txi, *ctype == PINDCT ? *coli : 1 ) ) {

	if ( *prec < PSTRP ||  *prec > PSTRKP) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

	} else if ( !phg_cb_valid_text_pair( *font, *prec,
	    dt->out_dt.num_text_pairs[0], dt->out_dt.text_pairs[0])) {
	    ERR_REPORT( phg_cur_cph->erh, ERR106);

	} else if ( !CB_COLOUR_MODEL_SUPPORTED(*ctype)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR110);

	} else {
	    args->wsid = *wkid;
	    args->type = PHG_ARGS_EXTTXREP;
	    args->rep.index = *txi;
	    args->rep.bundl.exttxrep.font = *font;
	    args->rep.bundl.exttxrep.prec = (Ptext_prec)*prec;
	    args->rep.bundl.exttxrep.char_expan = *chxp;
	    args->rep.bundl.exttxrep.char_space = *chsp;
            GCOLR_DATA_TRANSLATE( *ctype, *coli, colr,
                args->rep.bundl.exttxrep.colr);
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
	}
    }
}


/* SET EDGE REPRESENTATION */
void
psedr_( wkid, edi, edflag, edtype, ewidth, coli)
Pint	*wkid;	        /* workstation identifier	*/
Pint	*edi;		/* edge index	*/
Pint	*edflag;	/* edge flag	*/
Pint	*edtype;	/* edgetype	*/
Pfloat	*ewidth;	/* edgewidth scale factor	*/
Pint 	*coli;		/* edge colour index		*/
{
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    register Wst_phigs_dt	*dt;

    if ( dt = phg_cb_check_set_rep( phg_cur_cph, Pfn_set_edge_rep, *wkid, *edi,
	*coli)) {

	if ( *edflag < POFF || *edflag > PON) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2000);
        
	} else if ( *edflag == PON && !phg_cb_int_in_list( *edtype,
	    dt->out_dt.num_edge_types, dt->out_dt.edge_types)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR107);

        } else {
	    args->wsid = *wkid;
	    args->type = PHG_ARGS_EDGEREP;
	    args->rep.index = *edi;
	    args->rep.bundl.edgerep.flag = (Pedge_flag)*edflag;
            args->rep.bundl.edgerep.type = *edtype;
            args->rep.bundl.edgerep.width = *ewidth;
            args->rep.bundl.edgerep.colr_ind = *coli;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
	}
    }
}


/* SET EDGE REPRESENTATION PLUS */
void
psedrp_( wkid, edi, edflag, edtype, ewidth, ctype, coli, colr)
Pint	*wkid;		/* workstation identifier	*/
Pint	*edi;		/* edge index	*/
Pint	*edflag;	/* edge style	*/
Pint	*edtype;	/* edgetype	*/
Pfloat	*ewidth;	/* edgewidth scale factor	*/
Pint	*ctype;		/* edge colour type	*/
Pint	*coli;		/* edge colour index	*/
Pfloat	*colr;		/* edge colour components	*/
{
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    register Wst_phigs_dt	*dt;

    if ( dt = phg_cb_check_set_rep( phg_cur_cph, Pfn_set_edge_rep_plus, *wkid,
        *edi, *ctype == PINDCT ? *coli : 1) ) {

	if ( *edflag < POFF || *edflag > PON) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2000);

	} else if ( *edflag == PON && !phg_cb_int_in_list( *edtype,
	    dt->out_dt.num_edge_types, dt->out_dt.edge_types)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR107);

	} else if ( !CB_COLOUR_MODEL_SUPPORTED(*ctype)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR110);

	} else {
	    args->wsid = *wkid;
	    args->type = PHG_ARGS_EXTEDGEREP;
	    args->rep.index = *edi;
	    args->rep.bundl.extedgerep.flag = (Pedge_flag)*edflag;
	    args->rep.bundl.extedgerep.type = *edtype;
	    args->rep.bundl.extedgerep.width = *ewidth;
            GCOLR_DATA_TRANSLATE( *ctype, *coli, colr,
                args->rep.bundl.extedgerep.colr);
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
	}
    }
}


/* SET INTERIOR REPRESENTATION */
void
psir_( wkid, ii, ints, styli, coli)
Pint	*wkid;		/* workstation identifier	*/
Pint	*ii;		/* interior index	*/
Pint	*ints;		/* interior style	*/
Pint	*styli;		/* interior style index	*/
Pint	*coli;		/* interior colour index	*/
{
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    register Wst_phigs_dt	*dt;

    if ( dt = phg_cb_check_set_rep( phg_cur_cph, Pfn_set_int_rep, *wkid, *ii,
	*coli)) {

	if ( *ints < PHOLLO || *ints > PISEMP) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2000);

	} else if ( !phg_cb_int_in_list( *ints, dt->out_dt.num_interior_styles,
	    (Pint*)dt->out_dt.interior_styles)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR108);

	} else if ( *ints == PPATTR && *styli < 1) {
	    ERR_REPORT( phg_cur_cph->erh, ERR112);
	
        } else {
            args->wsid = *wkid;
	    args->type = PHG_ARGS_INTERREP;
	    args->rep.index = *ii;
	    args->rep.bundl.interrep.style = (Pint_style)*ints;
            args->rep.bundl.interrep.style_ind = *styli;
            args->rep.bundl.interrep.colr_ind = *coli;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
	}
    }
}


/* SET INTERIOR REPRESENTATION PLUS */
void
psirp_( wkid, ii, ictyp, icoli, icolr, bictyp, bicoli, bicolr, ints, bints,
styli, bstyli, irfeq, birfeq, ism, bism, amrc, dirc, sprc, sctyp, scoli, scolr,
spex, bamrc, bdirc, bsprc, bsctyp, bscoli, bscolr, bspex, acri, aval)
Pint	*wkid;		/* workstation id	*/
Pint	*ii;		/* interior index	*/
Pint	*ictyp;		/* interior coluor type	*/
Pint	*icoli;		/* interior coluor index	*/
Pfloat	*icolr;		/* interior coluor components	*/
Pint	*bictyp;	/* back interior coluor type	*/
Pint	*bicoli;	/* back interior coluor index	*/
Pfloat	*bicolr;	/* back interior coluor components	*/
Pint	*ints;		/* interior style	*/
Pint	*bints; 	/* back interior style	*/
Pint	*styli;		/* interior style index */
Pint	*bstyli;	/* back interior style index */
Pint	*irfeq;		/* interior reflectance equation	*/
Pint	*birfeq;	/* back interior reflectance equation	*/
Pint	*ism;		/* interior shading method	*/
Pint	*bism;		/* back interior shading method	*/
Pfloat	*amrc;		/* ambient reflection coefficent	*/
Pfloat	*dirc;		/* diffuse reflection coefficent	*/
Pfloat	*sprc;		/* specular reflection coefficent	*/
Pint	*sctyp;		/* specular colour type	*/
Pint	*scoli;		/* specular colour index	*/
Pfloat	*scolr;		/* specular colour components	*/
Pfloat	*spex;		/* specular exponent	*/
Pfloat	*bamrc;		/* back ambient reflection coefficent	*/
Pfloat	*bdirc;		/* back diffuse reflection coefficent	*/
Pfloat	*bsprc;		/* back specular reflection coefficent	*/
Pint	*bsctyp;	/* back specular colour type	*/
Pint	*bscoli;	/* back specular colour index	*/
Pfloat	*bscolr;	/* back specular colour components	*/
Pfloat	*bspex;		/* back specular exponent	*/
Pint	*acri;		/* surface approximation criteria type	*/
Pfloat	*aval;		/* surface approximation criteria values	*/
{
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    register Wst_phigs_dt	*dt;

    if ( dt = phg_cb_check_set_rep( phg_cur_cph, Pfn_set_int_rep_plus, *wkid,
        *ii, *ictyp == PINDCT ? *icoli : 1) ) {

	if ((*bictyp == PINDCT ? *bicoli : 1) < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR113);

	} else if ((*sctyp == PINDCT ? *scoli : 1) < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR113);

	} else if ((*bsctyp == PINDCT ? *bscoli : 1) < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR113);

	} else if (( *ints < PHOLLO || *ints > PISEMP) ||
	          ( *bints < PHOLLO || *bints > PISEMP)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2000);

	} else if ( !phg_cb_int_in_list( *ints, dt->out_dt.num_interior_styles,
					(Pint*)dt->out_dt.interior_styles) ||
		    !phg_cb_int_in_list( *bints, dt->out_dt.num_interior_styles,
	    				(Pint*)dt->out_dt.interior_styles)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR108);

	} else if (( *ints == PPATTR && *styli < 1) ||
		   ( *bints == PPATTR && *bstyli < 1)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR112);

	} else if ( !CB_COLOUR_MODEL_SUPPORTED(*ictyp) ||
		    !CB_COLOUR_MODEL_SUPPORTED(*bictyp) ||
	            !CB_COLOUR_MODEL_SUPPORTED(*sctyp) ||
		    !CB_COLOUR_MODEL_SUPPORTED(*bsctyp)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR110);

	} else if ( !CB_INT_SHADING_SUPPORTED(*ism) ||
	            !CB_INT_SHADING_SUPPORTED(*bism)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR123);

	} else if ( !CB_REFL_EQ_SUPPORTED(*irfeq) ||
	            !CB_REFL_EQ_SUPPORTED(*birfeq)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR124);

	} else {
	    args->wsid = *wkid;
	    args->type = PHG_ARGS_EXTINTERREP;
	    args->rep.index = *ii;
	    args->rep.bundl.extinterrep.style = (Pint_style)*ints;
	    args->rep.bundl.extinterrep.style_ind = *styli;
            GCOLR_DATA_TRANSLATE( *ictyp, *icoli, icolr,
	        args->rep.bundl.extinterrep.colr);
	    args->rep.bundl.extinterrep.refl_eqn = *irfeq;
	    args->rep.bundl.extinterrep.shad_meth = *ism;
	    args->rep.bundl.extinterrep.refl_props.ambient_coef = *amrc;
	    args->rep.bundl.extinterrep.refl_props.diffuse_coef = *dirc;
	    args->rep.bundl.extinterrep.refl_props.specular_coef = *sprc;
            GCOLR_DATA_TRANSLATE( *sctyp, *scoli, scolr,
	        args->rep.bundl.extinterrep.refl_props.specular_colr);
	    args->rep.bundl.extinterrep.refl_props.specular_exp = *spex;

	    args->rep.bundl.extinterrep.back_style = (Pint_style)*bints;
	    args->rep.bundl.extinterrep.back_style_ind = *bstyli;
            GCOLR_DATA_TRANSLATE( *bictyp, *bicoli, bicolr,
	        args->rep.bundl.extinterrep.back_colr);
	    args->rep.bundl.extinterrep.back_refl_eqn = *birfeq;
	    args->rep.bundl.extinterrep.back_shad_meth = *bism;
	    args->rep.bundl.extinterrep.back_refl_props.ambient_coef = *bamrc;
	    args->rep.bundl.extinterrep.back_refl_props.diffuse_coef = *bdirc;
	    args->rep.bundl.extinterrep.back_refl_props.specular_coef = *bsprc;
            GCOLR_DATA_TRANSLATE( *bsctyp, *bscoli, bscolr,
	        args->rep.bundl.extinterrep.back_refl_props.specular_colr);
	    args->rep.bundl.extinterrep.back_refl_props.specular_exp = *bspex;

	    args->rep.bundl.extinterrep.approx_type = *acri;
	    args->rep.bundl.extinterrep.approx_val[0] = aval[0];
	    args->rep.bundl.extinterrep.approx_val[1] = aval[1];
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
	}
    }
}

static int
valid_pattern_colours( s, a)
register Pint	s;
register Pint	*a;
{
    register int       i;
    for ( i = 0; i < s; i++) {
        if ( a[i] < 0)
            return 0;
    }
    return 1;
}


/* SET PATTERN REPRESENTATION */
void
pspar_( wkid, pai, dimx, dimy, isc, isr, dx, dy, colia)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pai;		/* pattern index	*/
Pint	*dimx;		/* dimension of pattern colour index array X	*/
Pint	*dimy;		/* dimension of pattern colour index array Y	*/
Pint	*isc;		/* indices to start column	*/
Pint	*isr;		/* indices to start row	*/
Pint	*dx;		/* number of columns	*/
Pint	*dy;		/* number of rows	*/
Pint	*colia;		/* pattern colour index		*/
{
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    Ppat_rep			rep;
    int				err = 0;

    /* Pattern uses a different error code for an index less than one so we
     * have to fake out check_set_rep and check it ourselves.  Also need to
     * do this for the colour, that gets checked separately..
     */

    if ( phg_cb_check_set_rep( phg_cur_cph, Pfn_set_pat_rep, *wkid, 1, 1)) {
        if ( *pai < 1) {
            ERR_REPORT( phg_cur_cph->erh, ERR112);

        } else if ( *dx < 1 || *dy < 1) {
            ERR_REPORT( phg_cur_cph->erh, ERR116);
        
        } else if ( *dimx < 0 || *dimy < 0) {
            ERR_REPORT( phg_cur_cph->erh, ERR2004);
        
        } else if (  *isc < 1 || *isr < 1) {
            ERR_REPORT( phg_cur_cph->erh, ERR2004);

        } else if ( *dimx < ((*isc)+(*dx)-1) || *dimy < ((*isr)+(*dy)-1)) {
            ERR_REPORT( phg_cur_cph->erh, ERR2004);

        } else if ( !valid_pattern_colours(((*dimx) * (*dimy)), colia) ) {
            ERR_REPORT( phg_cur_cph->erh, ERR113);
           
        } else {
            PMALLOC( err, Pint, ((*dx) * (*dy)), rep.colr_array);
            if ( err) {
                ERR_REPORT( phg_cur_cph->erh, err);
            
            } else {
                COLR_IND_DATA_TRANSLATE( *isc, *isr, *dx, *dy, *dimx, *dimy,
                    colia, rep);
                args->wsid = *wkid;
                args->type = PHG_ARGS_PTREP;
                args->rep.index = *pai;
                args->rep.bundl.ptrep = rep;
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
                PFREE( ((*dx)*(*dy)), rep.colr_array);
            }
        }
    }
}


/* SET PATTERN REPRESENTATION PLUS */
void
psparp_( wkid, pai, dimx, dimy, isc, isr, dx, dy, ctype, coli, colr)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pai;		/* pattern index	*/
Pint	*dimx;		/* diemension of pattern array X	*/
Pint	*dimy;		/* diemension of pattern array Y	*/
Pint	*isc;		/* indices to start column	*/
Pint	*isr;		/* indices to start row	*/
Pint	*dx;		/* number of columns	*/
Pint	*dy;		/* number of rows	*/
Pint	*ctype;		/* pattern colour type	*/
Pint	*coli;		/* pattern colour indices array	*/
Pfloat	*colr;		/* pattern colour components	*/
{
    Phg_args			cp_args;
    register Phg_args_set_rep	*args = &cp_args.data.set_rep;
    Pcoval			*colrs;
    int				err = 0;

    /* Pattern uses a different error code for an index less than one so we
     * have to fake out check_set_rep and check it ourselves.  Also need to
     * do this for the colour, that gets checked separately..
     */
    
    if ( phg_cb_check_set_rep(phg_cur_cph, Pfn_set_pat_rep_plus, *wkid, 1, 1)) {
	if ( *pai < 1) {
	    ERR_REPORT( phg_cur_cph->erh, ERR112);

	} else if ( !CB_COLOUR_MODEL_SUPPORTED(*ctype)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR110);

	} else if ( *dx < 1 || *dy < 1) {
	    ERR_REPORT( phg_cur_cph->erh, ERR116);
	
	} else if ( *dimx < 0 || *dimy < 0) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);
	
        } else if ( *isc < 1 || *isr < 1) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);

        } else if ( *dimx < ((*isc) +(*dx)-1) || *dimy < ((*isr)+(*dy)-1)) {
	    ERR_REPORT( phg_cur_cph->erh, ERR2004);

	} else if ( !phg_fb_colours_valid(((*dimx) * (*dimy)),
		*ctype, coli, colr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR136);

	} else {
            PMALLOC( err, Pcoval, ((*dx) * (*dy)), colrs);
            if ( err) {
	        ERR_REPORT( phg_cur_cph->erh, err);

            } else {
                COLR_VAL_DATA_TRANSLATE( *ctype, *isc, *isr, *dx, *dy, *dimx,
                    *dimy, coli, colr, colrs);
	        args->wsid = *wkid;
	        args->type = PHG_ARGS_EXTPTREP;
	        args->rep.index = *pai;
	        args->rep.bundl.extptrep.dims.size_x = *dx;
	        args->rep.bundl.extptrep.dims.size_y = *dy;
	        args->rep.bundl.extptrep.type = *ctype;
	        args->rep.bundl.extptrep.colr_array = colrs;
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args, NULL);
                PFREE( ((*dx)*(*dy)), colrs);
            }
	}
    }
}


/* INQUIRE WORKSTATION STATE VALUE */
void
pqwkst_( wksta)
Pint	*wksta;		/* OUT workstation state value	*/
{
    if ( CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY))
	*wksta = PSL_WS_STATE( phg_cur_cph->psl);
    else
	*wksta = PWSCL;
}


/* INQUIRE WORKSTATION CONNECTION AND TYPE */
void
pqwkc_( wkid, errind, conid, wtype )
Pint	*wkid;		/* workstation identifier	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*conid;		/* OUT connection identifier	*/
Pint	*wtype;		/* OUT workstation type	*/
{
    Psl_ws_info		*ws_info;
    Wst				*wst;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY) ) {
        *errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
        *errind = ERR3;

    } else if ( !(ws_info = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid)) ) {
        *errind = ERR54;

    } else {
        *errind = 0;
        wst = ws_info->wstype;
        *wtype = phg_fb_inq_ws_type( wst); /* not return -1 */
        switch( wst->base_type) {
        case WST_BASE_TYPE_X_TOOL:
            if ( !ws_info->connid) {
                *conid = 0;
            }
            break;
        case WST_BASE_TYPE_X_DRAWABLE:
            *conid = phg_fb_inq_connection_id( ws_info->connid);
            break;
        }
    }
}


/* INQUIRE SET member OF OPEN WORKSTATIONS */
void
pqopwk_( n, errind, ol, wkid)
Pint	*n;		/* set member requestet	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of open workstations	*/
Pint	*wkid;		/* OUT Nth member of set of open	*/
{
    Pint	wsids[MAX_NO_OPEN_WS];

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR2;

    } else {
	*errind = 0;
	*ol = phg_psl_inq_wsids( phg_cur_cph->psl, wsids);
	if ( *ol > 0) {
	    if ( *n < 0 || *n > *ol) {
		*errind = ERR2002;
            } else if ( *n > 0) {
                *wkid = wsids[*n - 1];
            }
        }
    }
}


/* This function performs the inquiry for INQUIRE LIST OF xxx INDICES
 * and formats the results.
 * Error checking and cp_args set up must already be done.
 */
static void
fb_ws_inq_table_indices( cp_args, n, error_ind, indices, total_length)
Phg_args	*cp_args;	/* Pointer to cp_args set up by caller */
Pint		n;		/* list element requested	*/
Pint		*error_ind;	/* OUT error indicator	*/
Pint		*indices;	/* OUT Nth element of list of defined	*/
Pint		*total_length;	/* OUT number of table	*/
{
    Phg_ret		ret;

    ret.err = 0;
    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_INDICES, cp_args, &ret);
    if ( ret.err) {
	*error_ind = ret.err;
    } else {
	*error_ind = 0;
	*total_length = ret.data.int_list.num_ints;
	if ( *total_length > 0) {
	    if ( n < 0 || n > *total_length) {
		*error_ind = ERR2002;
            } else if ( n > 0) {
		*indices = ret.data.int_list.ints[n - 1];
            }
        }
    }
}


/* INQUIRE LIST element OF VIEW INDICES */
void
pqevwi_( wkid, n, errind, nvwix, viewi)
Pint	*wkid;		/* workstation identifier	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*nvwix;		/* OUT number of bundle table	*/
Pint	*viewi;		/* OUT Nth element of list of defined	*/
{
    Phg_args		cp_args;
    Psl_ws_info		*wsinfo;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
	*errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
	*errind = ERR54;

    } else if ( ((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt.ws_category == PMI) {
	*errind = ERR57;

    } else {
	cp_args.data.q_indices.wsid = *wkid;
	cp_args.data.q_indices.type = PHG_ARGS_VIEWREP;
	fb_ws_inq_table_indices( &cp_args, *n, errind, viewi, nvwix);
    }
}


/* INQUIRE POSTED STRUCTURES */
void
pqpost_( wkid, n, errind, number, strid, priort)
Pint	*wkid;		/* workstation identifier	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*number;	/* OUT number of structure posted	*/
Pint	*strid;		/* OUT Nth structure posted	*/
Pfloat	*priort;	/* OUT priority of the Nth structure posted	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;
    Psl_ws_info		*wsinfo;
    Wst_phigs_dt	*dt;
    Pposted_struct	list;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
	*errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
	*errind = ERR54;

    } else {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( !(dt->ws_category == POUTPT || 
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO) ) {
	    *errind = ERR59;

	} else {
	    cp_args.data.idata = *wkid;
	    ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_POSTED, &cp_args, &ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else {
		*errind = 0;
		*number = ret.data.postlist.num_postings;
		if ( *number > 0) {
		    if ( *n < 0 || *n > *number){
			*errind = ERR2002;
                    } else if ( *n > 0) {
			*strid = ret.data.postlist.postings[*n - 1].id;
			*priort = ret.data.postlist.postings[*n - 1].disp_pri;
                    }
                }
	    }
	}
    }
}


/* INQUIRE VIEW REPRESENTATION */
void
pqvwr_( wkid, viewi, curq, errind, vwupd, vwormt, vwmpmt, vwcplm, xyclpi,
bclipi, fclipi)
Pint		*wkid;		/* workstation identifier	*/
Pint		*viewi;		/* view index	*/
Pint		*curq;		/* current or reqested	*/
Pint		*errind;	/* OUT error indicator	*/
Pint		*vwupd;		/* OUT transformation update state	*/
Pmatrix3	vwormt;		/* OUT view orientation matrix	*/
Pmatrix3	vwmpmt;		/* OUT view mapping matrix	*/
Plimit3		*vwcplm;	/* OUT view clipping limits	*/
Pint		*xyclpi;	/* OUT x-y clipping indicator	*/
Pint		*bclipi;	/* OUT back clipping indicator	*/
Pint		*fclipi;	/* OUT front clipping indicator	*/
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
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( dt->ws_category == PMI) {
	    *errind = ERR57;

	} else if ( *viewi < 0) {
            *errind = ERR114;

	} else if ( *viewi >= dt->num_view_indices) {
	    *errind = ERR101;

	} else if ( *curq < PCURVL || *curq > PRQSVL) {
	    *errind = ERR2000;

	} else {
	    cp_args.data.q_view_rep.wsid = *wkid;
	    cp_args.data.q_view_rep.index = *viewi;
	    ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_VIEW_REP, &cp_args, &ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else {
		*errind = 0;
		*vwupd = (Pint)ret.data.view_rep.update_state;
                if ( *curq == PCURVL) {
		    MATRIX_DATA_TRANSLATE( 3,
                        ret.data.view_rep.cur_rep->ori_matrix, vwormt);
		    MATRIX_DATA_TRANSLATE( 3,
		        ret.data.view_rep.cur_rep->map_matrix, vwmpmt);
		    *vwcplm = ret.data.view_rep.cur_rep->clip_limit;
		    *xyclpi = (Pint)ret.data.view_rep.cur_rep->xy_clip;
		    *bclipi = (Pint)ret.data.view_rep.cur_rep->back_clip;
		    *fclipi = (Pint)ret.data.view_rep.cur_rep->front_clip;
                } else if ( *curq == PRQSVL) {
		    MATRIX_DATA_TRANSLATE( 3,
		        ret.data.view_rep.req_rep->ori_matrix, vwormt);
		    MATRIX_DATA_TRANSLATE( 3,
		        ret.data.view_rep.req_rep->map_matrix, vwmpmt);
		    *vwcplm = ret.data.view_rep.req_rep->clip_limit;
		    *xyclpi = (Pint)ret.data.view_rep.req_rep->xy_clip;
		    *bclipi = (Pint)ret.data.view_rep.req_rep->back_clip;
		    *fclipi = (Pint)ret.data.view_rep.req_rep->front_clip;
                }
	    }
	}
    }
}


/* INQUIRE WORKSTATION TRANSFORMATION 3 */
void
pqwkt3_( wkid, errind, tus, rwindo, cwindo, rviewp, cviewp)
Pint	*wkid;		/* workstation identifier	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*tus;		/* OUT update state	*/
Plimit3	*rwindo;	/* OUT requested workstation window */
Plimit3	*cwindo;	/* OUT current workstation window   */
Plimit3	*rviewp;	/* OUT requested workstation viewport */
Plimit3	*cviewp;	/* OUT current workstation viewport */
{
    Phg_args		cp_args;
    Phg_ret		ret;
    Psl_ws_info		*wsinfo;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
	*errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
	*errind = ERR54;

    } else if ( ((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt.ws_category == PMI) {
	*errind = ERR57;

    } else {
	cp_args.data.idata = *wkid;
	ret.err = 0;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_WS_XFORM, &cp_args, &ret);
	if ( ret.err) {
	    *errind = ret.err;
	} else {
	    *errind = 0;
	    *tus = (Pint)ret.data.ws_xform.state;
	    *rwindo = ret.data.ws_xform.req_window;
	    *cwindo = ret.data.ws_xform.cur_window;
	    *rviewp = ret.data.ws_xform.req_viewport;
	    *cviewp = ret.data.ws_xform.cur_viewport;
	}
    }
}


/* INQUIRE WORKSTATION TRANSFORMATION */
void
pqwkt_( wkid, errind, tus, rwindo, cwindo, rviewp, cviewp)
Pint	*wkid;		/* workstation identifier	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*tus;		/* OUT update state	*/
Plimit	*rwindo;	/* OUT requested workstation window	*/
Plimit	*cwindo;	/* OUT current workstation window	*/
Plimit	*rviewp;	/* OUT requested workstation viewport	*/
Plimit	*cviewp;	/* OUT current workstation viewport	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;
    Psl_ws_info		*wsinfo;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
        *errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
	*errind = ERR54;

    } else if ( ((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt.ws_category == PMI) {
	*errind = ERR57;

    } else {
	cp_args.data.idata = *wkid;
	ret.err = 0;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_WS_XFORM, &cp_args, &ret);
	if ( ret.err) {
	    *errind = ret.err;
	} else {
	    *errind = 0;
	    *tus = (Pint)ret.data.ws_xform.state;
	    rwindo->x_min = ret.data.ws_xform.req_window.x_min;
	    rwindo->x_max = ret.data.ws_xform.req_window.x_max;
	    rwindo->y_min = ret.data.ws_xform.req_window.y_min;
	    rwindo->y_max = ret.data.ws_xform.req_window.y_max;
	    rviewp->x_min = ret.data.ws_xform.req_viewport.x_min;
	    rviewp->x_max = ret.data.ws_xform.req_viewport.x_max;
	    rviewp->y_min = ret.data.ws_xform.req_viewport.y_min;
	    rviewp->y_max = ret.data.ws_xform.req_viewport.y_max;

	    cwindo->x_min = ret.data.ws_xform.cur_window.x_min;
	    cwindo->x_max = ret.data.ws_xform.cur_window.x_max;
	    cwindo->y_min = ret.data.ws_xform.cur_window.y_min;
	    cwindo->y_max = ret.data.ws_xform.cur_window.y_max;
	    cviewp->x_min = ret.data.ws_xform.cur_viewport.x_min;
	    cviewp->x_max = ret.data.ws_xform.cur_viewport.x_max;
	    cviewp->y_min = ret.data.ws_xform.cur_viewport.y_min;
	    cviewp->y_max = ret.data.ws_xform.cur_viewport.y_max;
	}
    }
}


/* INQUIRE DISPLAY UPDATE STATE */
void
pqdus_( wkid, errind, defmod, modmod, dempty, stofvr)
Pint	*wkid;		/* workstation identifier       */
Pint	*errind;	/* OUT error indicator  */
Pint	*defmod;	/* OUT deferral mode    */
Pint	*modmod;	/* OUT modification mode        */
Pint	*dempty;	/* OUT display surface empty    */
Pint	*stofvr;	/* OUT state of visual representation */
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
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( !(dt->ws_category == POUTPT || 
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO) ) {
	    *errind = ERR59;

	} else {
	    cp_args.data.idata = *wkid;
	    ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_DISP_UPDATE_STATE,
		&cp_args, &ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else {
		*errind = 0;
		*defmod = (Pint)ret.data.update_state.def_mode;
		*modmod = (Pint)ret.data.update_state.mod_mode;
		*dempty = (Pint)ret.data.update_state.display_surf;
		*stofvr = (Pint)ret.data.update_state.state;
	    }
	}
    }
}


/* INQUIRE LIST element OF POLYLINE INDICES */
void
pqepli_( wkid, n, errind, ol, pli)
Pint	*wkid;		/* workstation identifier	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of polyline bundle table	*/
Pint	*pli;		/* OUT Nth element of list of polyline indices	*/
{
    Phg_args		cp_args;
    Psl_ws_info		*wsinfo;
    Wst_phigs_dt	*dt;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
	*errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
	*errind = ERR54;

    } else {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( !(dt->ws_category == POUTPT || 
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO) ) {
	    *errind = ERR59;

	} else {
	    cp_args.data.q_indices.wsid = *wkid;
	    cp_args.data.q_indices.type = PHG_ARGS_LNREP;
	    fb_ws_inq_table_indices( &cp_args, *n, errind, pli, ol);
	}
    }
}


/* INQUIRE LIST element OF LIGHT SOURCE INDICES */
void
pqelsi_( wkid, n, errind, ol, lsi)
Pint	*wkid;		/* workstation identifier	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of light source table	*/
Pint	*lsi;		/* OUT Nth element of list of light source indices */
{
    Phg_args		cp_args;
    Psl_ws_info		*wsinfo;
    Wst_phigs_dt	*dt;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
	*errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
	*errind = ERR54;

    } else {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( !(dt->ws_category == POUTPT || 
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO) ) {
	    *errind = ERR59;

	} else {
	    cp_args.data.q_indices.wsid = *wkid;
	    cp_args.data.q_indices.type = PHG_ARGS_LIGHTSRCREP;
	    fb_ws_inq_table_indices( &cp_args, *n, errind, lsi, ol);
	}
    }
}


/* INQUIRE LIST element OF DEPTH CUE INDICES */
void
pqedci_( wkid, n, errind, ol, dci)
Pint	*wkid;		/* workstation identifier	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of depth cue bundle table	*/
Pint	*dci;		/* OUT Nth element of list of depth cue indices	*/
{
    Phg_args		cp_args;
    Psl_ws_info		*wsinfo;
    Wst_phigs_dt	*dt;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
	*errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
	*errind = ERR54;

    } else {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( !(dt->ws_category == POUTPT || 
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO) ) {
	    *errind = ERR59;

	} else {
	    cp_args.data.q_indices.wsid = *wkid;
	    cp_args.data.q_indices.type = PHG_ARGS_DCUEREP;
	    fb_ws_inq_table_indices( &cp_args, *n, errind, dci, ol);
	}
    }
}


/* INQUIRE LIST element OF POLYMARKER INDICES */
void
pqepmi_( wkid, n, errind, ol, pmi)
Pint	*wkid;		/* workstation identifier	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of polymarker bundle table	*/
Pint	*pmi;		/* OUT Nth element of list of polymarker indices */
{
    Phg_args		cp_args;
    Psl_ws_info		*wsinfo;
    Wst_phigs_dt	*dt;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
	*errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
	*errind = ERR54;

    } else {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( !(dt->ws_category == POUTPT || 
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO) ) {
	    *errind = ERR59;

	} else {
	    cp_args.data.q_indices.wsid = *wkid;
	    cp_args.data.q_indices.type = PHG_ARGS_MKREP;
	    fb_ws_inq_table_indices( &cp_args, *n, errind, pmi, ol);
	}
    }
}


/* INQUIRE LIST element OF TEXT INDICES */
void
pqetxi_( wkid, n, errind, ol, txi)
Pint	*wkid;		/* workstation identifier	*/
Pint	*n;		/* list element requested	*/	
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of text bundle table	*/
Pint	*txi;		/* OUT Nth element of list of text entries	*/
{
    Phg_args		cp_args;
    Psl_ws_info		*wsinfo;
    Wst_phigs_dt	*dt;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
	*errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
	*errind = ERR54;

    } else {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( !(dt->ws_category == POUTPT || 
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO) ) {
	    *errind = ERR59;

	} else {
	    cp_args.data.q_indices.wsid = *wkid;
	    cp_args.data.q_indices.type = PHG_ARGS_TXREP;
	    fb_ws_inq_table_indices( &cp_args, *n, errind, txi, ol);
	}
    }
}


/* INQUIRE LIST element OF INTERIOR INDICES */
void
pqeii_( wkid, n, errind, ol, ii)
Pint	*wkid;		/* workstation identifier	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of interior bundle table	*/
Pint	*ii;		/* OUT Nth element of list of interior indices	*/
{
    Phg_args		cp_args;
    Psl_ws_info		*wsinfo;
    Wst_phigs_dt	*dt;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
	*errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
	*errind = ERR54;

    } else {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( !(dt->ws_category == POUTPT || 
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO) ) {
	    *errind = ERR59;

	} else {
	    cp_args.data.q_indices.wsid = *wkid;
	    cp_args.data.q_indices.type = PHG_ARGS_INTERREP;
	    fb_ws_inq_table_indices( &cp_args, *n, errind, ii, ol);
	}
    }
}


/* INQUIRE LIST element OF EDGE INDICES */
void
pqeedi_( wkid, n, errind, ol, edi)
Pint	*wkid;		/* workstation identifier	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of edge bundle table	*/
Pint	*edi;		/* OUT Nth element of list of edge indices	*/
{
    Phg_args		cp_args;
    Psl_ws_info		*wsinfo;
    Wst_phigs_dt	*dt;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
	*errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
	*errind = ERR54;

    } else {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( !(dt->ws_category == POUTPT || 
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO) ) {
	    *errind = ERR59;

	} else {
	    cp_args.data.q_indices.wsid = *wkid;
	    cp_args.data.q_indices.type = PHG_ARGS_EDGEREP;
	    fb_ws_inq_table_indices( &cp_args, *n, errind, edi, ol);
	}
    }
}


/* INQUIRE LIST element OF PATTERN INDICES */
void
pqepai_( wkid, n, errind, ol, pai)
Pint	*wkid;		/* workstation identifier	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of pattern table	*/
Pint	*pai;		/* OUT Nth element of list of pattern indices	*/
{
    Phg_args		cp_args;
    Psl_ws_info		*wsinfo;
    Wst_phigs_dt	*dt;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
	*errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
	*errind = ERR54;

    } else {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( !(dt->ws_category == POUTPT || 
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO) ) {
	    *errind = ERR59;

	} else {
	    cp_args.data.q_indices.wsid = *wkid;
	    cp_args.data.q_indices.type = PHG_ARGS_PTREP;
	    fb_ws_inq_table_indices( &cp_args, *n, errind, pai, ol);
	}
    }
}


/* INQUIRE LIST element OF COLOUR INDICES */
void
pqeci_( wkid, n, errind, ol, coli)
Pint	*wkid;		/* workstation identifier	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of colour table	*/
Pint	*coli;		/* OUT Nth element of list of colour indices	*/
{
    Phg_args		cp_args;
    Psl_ws_info		*wsinfo;
    Wst_phigs_dt	*dt;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
	*errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
	*errind = ERR54;

    } else {
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( !(dt->ws_category == POUTPT || 
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO) ) {
	    *errind = ERR59;

	} else {
	    cp_args.data.q_indices.wsid = *wkid;
	    cp_args.data.q_indices.type = PHG_ARGS_COREP;
	    fb_ws_inq_table_indices( &cp_args, *n, errind, coli, ol);
	}
    }
}


/* INQUIRE TEXT REPRESENTATION */
void
pqtxr_( wkid, txi, type, errind, font, prec, chxp, chsp, coli)
Pint	*wkid;		/* workstation identifier	*/
Pint	*txi;		/* text index	*/
Pint	*type;		/* type of returned value	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*font;		/* OUT text font	*/
Pint	*prec;		/* OUT text precision	*/	
Pfloat	*chxp;		/* OUT character expansion factor	*/
Pfloat	*chsp;		/* OUT character spacing	*/
Pint	*coli;		/* OUT text colour index	*/
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

	} else if ( *txi < 1) {
	    *errind = ERR100;

	} else if ( *type < PSET || *type > PREALI) {
	    *errind = ERR2000;

        }

	if ( !*errind) {
	    cp_args.data.q_rep.wsid = *wkid;
	    cp_args.data.q_rep.index = *txi;
	    cp_args.data.q_rep.type = (Pinq_type)*type;
	    cp_args.data.q_rep.rep_type = PHG_ARGS_EXTTXREP;
	    ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
	        &ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else if ( ret.data.rep.exttxrep.colr.type != PINDCT) {
		*errind = ERR134;
            } else {
		*font = ret.data.rep.exttxrep.font;
		*prec = (Pint)ret.data.rep.exttxrep.prec;
		*chxp = ret.data.rep.exttxrep.char_expan;
		*chsp = ret.data.rep.exttxrep.char_space;
		*coli = ret.data.rep.exttxrep.colr.val.ind;
	    }
	}
    }
}


/* INQUIRE TEXT REPRESENTATION PLUS */
void
pqtxrp_( wkid, txi, ccsbsz, type, errind, font, prec, chxp, chsp, ctype, coli,
ol, colr)
Pint	*wkid;		/* workstation identifier	*/
Pint	*txi;		/* text index	*/
Pint	*ccsbsz;	/* colour component buffer size	*/
Pint	*type;		/* type of returned value	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*font;		/* OUT text font	*/
Pint	*prec;		/* OUT text precision	*/
Pfloat	*chxp;		/* OUT character expansion factor	*/
Pfloat	*chsp;		/* OUT character spacing	*/
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

	} else if ( *txi < 1) {
	    *errind = ERR100;

	} else if ( *type < PSET || *type > PREALI) {
	    *errind = ERR2000;

        }

	if ( !*errind) {
	    cp_args.data.q_rep.wsid = *wkid;
	    cp_args.data.q_rep.index = *txi;
	    cp_args.data.q_rep.type = (Pinq_type)*type;
	    cp_args.data.q_rep.rep_type = PHG_ARGS_EXTTXREP;
	    ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
		&ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else {
		*font = ret.data.rep.exttxrep.font;
		*prec = (Pint)ret.data.rep.exttxrep.prec;
		*chxp = ret.data.rep.exttxrep.char_expan;
		*chsp = ret.data.rep.exttxrep.char_space;
                GCOLR_DATA_RE_TRANSLATE_PLUS( ret.data.rep.exttxrep.colr,
                   *ccsbsz, *errind, *ctype, *coli, *ol, colr);
	    }
	}
    }
}


/* INQUIRE INTERIOR REPRESENTATION */
void
pqir_( wkid, ii, type, errind, ints, istyli, coli)
Pint	*wkid;		/* workstation identifier	*/
Pint	*ii;		/* interior index	*/
Pint	*type;		/* type of returned value	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ints;		/* OUT interior style	*/
Pint	*istyli;	/* OUT interior style index	*/
Pint	*coli;		/* OUT interior colour index	*/
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

	} else if ( *ii < 1) {
	    *errind = ERR100;

	} else if ( *type < PSET || *type > PREALI) {
	    *errind = ERR2000;

        }

	if ( !*errind) {
	    cp_args.data.q_rep.wsid = *wkid;
	    cp_args.data.q_rep.index = *ii;
	    cp_args.data.q_rep.type = (Pinq_type)*type;
	    cp_args.data.q_rep.rep_type = PHG_ARGS_EXTINTERREP;
	    ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
			&ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else if ( ret.data.rep.extinterrep.colr.type != PINDCT) {
		*errind = ERR134;
	    } else {
		*ints = (Pint)ret.data.rep.extinterrep.style;
		*istyli = ret.data.rep.extinterrep.style_ind;
	        *coli = ret.data.rep.extinterrep.colr.val.ind;
	    }
	}
    }
}


/* INQUIRE INTERIOR REPRESENTATION PLUS */
void
pqirp_( wkid, ii, iccss, biccss, sccss, bsccss, type, errind, ictyp, icoli, iol,
icolr, bictyp, bicoli, biol, bicolr, ints, bints, styli, bstyli, irfeq, birfeq,
ism, bism, amrc, dirc, sprc, sctyp, scoli, sol, scolr, spex, bamrc, bdirc,
bsprc, bsctyp, bscoli, bsol, bscolr, bspex, acri, aval)
Pint	*wkid;		/* workstation id	*/
Pint	*ii;		/* interior index	*/
Pint	*iccss;		/* interior colour component buffer size	*/
Pint	*biccss;	/* back interior colour component buffer size	*/
Pint	*sccss;		/* specular colour component buffer size	*/
Pint	*bsccss;	/* back specular colour component buffer size	*/
Pint	*type;		/* type of returned value	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ictyp;		/* OUT interior colour type	*/
Pint	*icoli;		/* OUT interior colour index	*/
Pint	*iol;		/* OUT number of interior colour components	*/
Pfloat	*icolr;		/* OUT interior colour specification	*/
Pint	*bictyp;	/* OUT back interior colour type	*/
Pint	*bicoli;	/* OUT back interior colour index	*/
Pint	*biol;		/* OUT number of back interior colour components */
Pfloat	*bicolr;	/* OUT back interior colour specification	*/
Pint	*ints;		/* OUT interior style	*/
Pint	*bints;		/* OUT back interior style	*/
Pint	*styli;		/* OUT interior style index */
Pint	*bstyli;	/* OUT back interior style index */
Pint	*irfeq;		/* OUT interior reflectance equation	*/
Pint	*birfeq;	/* OUT back interior reflectance equation	*/
Pint	*ism;		/* OUT interior shading method	*/
Pint	*bism;		/* OUT back interior shading method	*/
Pfloat	*amrc;		/* OUT ambient reflection coefficent	*/
Pfloat	*dirc;		/* OUT diffuse reflection coefficent	*/
Pfloat	*sprc;		/* OUT specular reflection coefficent	*/
Pint	*sctyp;		/* OUT specular colour type	*/
Pint	*scoli;		/* OUT specular colour index	*/
Pint	*sol;		/* OUT number of specular colour components	*/
Pfloat	*scolr;		/* OUT specular colour specification	*/
Pfloat	*spex;		/* OUT specular exponent	*/
Pfloat	*bamrc;		/* OUT back ambient reflection coefficent	*/
Pfloat	*bdirc;		/* OUT back diffuse reflection coefficent	*/
Pfloat	*bsprc;		/* OUT back specular reflection coefficent	*/
Pint	*bsctyp;	/* OUT back specular colour type	*/
Pint	*bscoli;	/* OUT back specular colour index	*/
Pint	*bsol;		/* OUT number of back specular colour components */
Pfloat	*bscolr;	/* OUT specular back colour specification	*/
Pfloat	*bspex;		/* OUT back specular exponent	*/
Pint	*acri;		/* OUT surface approximation criteria type	*/
Pfloat	*aval;		/* OUT surface approximation criteria values	*/
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

	} else if ( *ii < 1) {
	    *errind = ERR100;

	} else if ( *type < PSET || *type > PREALI) {
	    *errind = ERR2000;

        }

	if ( !*errind) {
	    cp_args.data.q_rep.wsid = *wkid;
	    cp_args.data.q_rep.index = *ii;
	    cp_args.data.q_rep.type = (Pinq_type)*type;
	    cp_args.data.q_rep.rep_type = PHG_ARGS_EXTINTERREP;
	    ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
		&ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else {
                GCOLR_DATA_RE_TRANSLATE_PLUS( ret.data.rep.extinterrep.colr, 
                    *iccss, *errind, *ictyp, *icoli, *iol, icolr);
                if ( *errind) {
                    return;
                }
                GCOLR_DATA_RE_TRANSLATE_PLUS( ret.data.rep.extinterrep.
                  back_colr, *biccss, *errind, *bictyp, *bicoli, *biol, bicolr);
                if ( *errind) {
                    return;
                }
		*ints = (Pint)ret.data.rep.extinterrep.style;
		*bints = (Pint)ret.data.rep.extinterrep.back_style;
		*styli = ret.data.rep.extinterrep.style_ind;
		*bstyli = ret.data.rep.extinterrep.back_style_ind;
		*irfeq = ret.data.rep.extinterrep.refl_eqn;
		*birfeq = ret.data.rep.extinterrep.back_refl_eqn;
		*ism = ret.data.rep.extinterrep.shad_meth;
		*bism = ret.data.rep.extinterrep.back_shad_meth;
		*amrc = ret.data.rep.extinterrep.refl_props.ambient_coef;
		*dirc = ret.data.rep.extinterrep.refl_props.diffuse_coef;
		*sprc = ret.data.rep.extinterrep.refl_props.specular_coef;
                GCOLR_DATA_RE_TRANSLATE_PLUS( ret.data.rep.extinterrep.
                    refl_props.specular_colr, *sccss, *errind, *sctyp, *scoli,
                    *sol, scolr);
                if ( *errind) {
                    return;
                }
		*spex = ret.data.rep.extinterrep.refl_props.specular_exp;

		*bamrc = ret.data.rep.extinterrep.back_refl_props.ambient_coef;
		*bdirc = ret.data.rep.extinterrep.back_refl_props.diffuse_coef;
		*bsprc = ret.data.rep.extinterrep.back_refl_props.specular_coef;
                GCOLR_DATA_RE_TRANSLATE_PLUS( ret.data.rep.extinterrep.
                    back_refl_props.specular_colr, *bsccss, *errind, *bsctyp,
                    *bscoli, *bsol, bscolr);
                if ( *errind) {
                    return;
                }
		*bspex = ret.data.rep.extinterrep.back_refl_props.specular_exp;
		*acri = ret.data.rep.extinterrep.approx_type;
		aval[0] = ret.data.rep.extinterrep.approx_val[0];
		aval[1] = ret.data.rep.extinterrep.approx_val[1];
	    }
	}
    }
}


/* INQUIRE EDGE REPRESENTATION */
void
pqedr_( wkid, edi, type, errind, edflag, edtype, ewidth, coli)
Pint	*wkid;		/* workstation identifier	*/
Pint	*edi;		/* edge index	*/
Pint	*type;		/* type of returned value	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*edflag;	/* OUT edge flag	*/
Pint	*edtype;	/* OUT edge type	*/
Pfloat	*ewidth;	/* OUT edgewidth scale factor	*/
Pint	*coli;		/* OUT edge colour index	*/
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

	} else if ( *edi < 1) {
	    *errind = ERR100;

	} else if ( *type < PSET || *type > PREALI) {
	    *errind = ERR2000;

        }

	if ( !*errind) {
	    cp_args.data.q_rep.wsid = *wkid;
	    cp_args.data.q_rep.index = *edi;
	    cp_args.data.q_rep.type = (Pinq_type)*type;
	    cp_args.data.q_rep.rep_type = PHG_ARGS_EXTEDGEREP;
	    ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
		&ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else if ( ret.data.rep.extedgerep.colr.type != PINDCT) {
		*errind = ERR134;
	    } else {
		*edflag = (Pint)ret.data.rep.extedgerep.flag;
		*edtype = ret.data.rep.extedgerep.type;
		*ewidth = ret.data.rep.extedgerep.width;
		*coli = ret.data.rep.extedgerep.colr.val.ind;
	    }
	}
    }
}


/* INQUIRE EDGE REPRESENTATION PLUS */
void
pqedrp_( wkid, edi, ccsbsz, type, errind, edflg, edtype, ewidth, ctype, coli,
ol, colr)
Pint	*wkid;		/* workstation identifier	*/
Pint	*edi;		/* edge index	*/
Pint	*ccsbsz;	/* colour component buffer size	*/
Pint	*type;		/* type of returned value	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*edflg;		/* OUT edge flag	*/
Pint	*edtype;	/* OUT edge type	*/
Pfloat	*ewidth;	/* OUT edgewidth scale factor	*/
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

	} else if ( *edi < 1) {
	    *errind = ERR100;

	} else if ( *type < PSET || *type > PREALI) {
	    *errind = ERR2000;

        }

	if ( !*errind) {
	    cp_args.data.q_rep.wsid = *wkid;
	    cp_args.data.q_rep.index = *edi;
	    cp_args.data.q_rep.type = (Pinq_type)*type;
	    cp_args.data.q_rep.rep_type = PHG_ARGS_EXTEDGEREP;
	    ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
		&ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else {
		*edflg = (Pint)ret.data.rep.extedgerep.flag;
		*edtype = ret.data.rep.extedgerep.type;
		*ewidth = ret.data.rep.extedgerep.width;
                GCOLR_DATA_RE_TRANSLATE_PLUS( ret.data.rep.extedgerep.colr,
                    *ccsbsz, *errind, *ctype, *coli, *ol, colr);
	    }
	}
    }
}


/* INQUIRE COLOUR REPRESENTATION */
void
pqcr_( wkid, coli, ccsbsz, type, errind, ol, cspec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*coli;		/* colour index	*/
Pint	*ccsbsz;	/* colour component buffer size	*/
Pint	*type;		/* type of returned value	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of colour components	*/
Pfloat	*cspec;		/* OUT colour specification	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;
    Psl_ws_info		*wsinfo;
    Wst_phigs_dt	*dt;
    Pint		colr_type = PRGB;	/* only RGB suppoted */
    static Pint		colr_table[5] = {0, 3, 3, 3, 3}; /* colour table */
    Pfloat		*pt;
    int			i;

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

	} else if ( *coli < 0) {
	    *errind = ERR113;

	} else if ( *type < PSET || *type > PREALI) {
	    *errind = ERR2000;

	} else if ( *ccsbsz < colr_table[colr_type]) {
	    *errind = ERR2001;

        }
	if ( !*errind) {
	    cp_args.data.q_rep.wsid = *wkid;
	    cp_args.data.q_rep.index = *coli;
	    cp_args.data.q_rep.type = (Pinq_type)*type;
	    cp_args.data.q_rep.rep_type = PHG_ARGS_COREP;
	    ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
		&ret);
	    *errind = ret.err;
	    if ( !ret.err) {
                *ol = colr_table[colr_type];
                pt = (Pfloat *)(&ret.data.rep.corep);
                for ( i = 0; i < colr_table[colr_type]; i++) {
		    cspec[i] = pt[i];
                }
            }
	}
    }
}


/* INQUIRE PATTERN REPRESENTATION */
void
pqpar_( wkid, pai, type, dimx, dimy, errind, dx, dy, colia)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pai;		/* pattern index		*/
Pint	*type;		/* type of returned value	*/
Pint	*dimx;		/* MAX pattern array dimensions X	*/
Pint	*dimy;		/* MAX pattern array dimensions Y	*/
Pint	*errind;	/* OUT error indicator		*/
Pint	*dx;		/* OUT colour index array dimensions X	*/
Pint	*dy;		/* OUT colour index array dimensions Y	*/
Pint	*colia;		/* OUT pattern colour index array	*/
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

        } else if ( *pai < 1) {
            *errind = ERR112;

        } else if ( *type < PSET || *type > PREALI) {
            *errind = ERR2000;

        }

        if ( !*errind) {
            cp_args.data.q_rep.wsid = *wkid;
            cp_args.data.q_rep.index = *pai;
            cp_args.data.q_rep.type = (Pinq_type)*type;
            cp_args.data.q_rep.rep_type = PHG_ARGS_EXTPTREP;
			ret.err = 0;
            CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
           	    &ret);
            if ( ret.err) {
                *errind = ret.err;
            } else if ( ret.data.rep.extptrep.type != PINDCT) {
                *errind = ERR134;
            } else {
                Ppat_rep_plus	*rep = &ret.data.rep.extptrep;

                *dx = rep->dims.size_x;
                *dy = rep->dims.size_y;
                if ( *dimx < *dx || *dimy < *dy) {
                    *errind = ERR2001;

                } else {
                    COLR_IND_DATA_RE_TRANSLATE( rep, *dx, *dy, *dimx, *dimy,
                        colia);
                }
            }
        }
    }
}


/* INQUIRE PATTERN REPRESENTATION PLUS */
void
pqparp_( wkid, pai, ccsbsz, type, dimx, dimy, errind, dx, dy, ctype, colia, ol,
colra)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pai;		/* pattern index		*/
Pint	*ccsbsz;	/* colour component buffer size	*/
Pint	*type;		/* type of returned value	*/
Pint	*dimx;		/* MAX pattern array dimensions X	*/
Pint	*dimy;		/* MAX pattern array dimensions Y	*/
Pint	*errind;	/* OUT error indicator		*/
Pint	*dx;		/* OUT pattern array dimensions X	*/
Pint	*dy;		/* OUT pattern array dimensions Y	*/
Pint	*ctype;		/* OUT colour type	*/
Pint	*colia;		/* OUT colour indices array	*/
Pint	*ol;		/* OUT number of colour components	*/
Pfloat	*colra;		/* OUT colour specification	array	*/
{
    Phg_args		cp_args;
    Phg_ret			ret;
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

	    } else if ( *pai < 1) {
	        *errind = ERR112;

	    } else if ( *type < PSET || *type > PREALI) {
	        *errind = ERR2000;

        }

        if ( !*errind) {
	        cp_args.data.q_rep.wsid = *wkid;
	        cp_args.data.q_rep.index = *pai;
	        cp_args.data.q_rep.type = (Pinq_type)*type;
	        cp_args.data.q_rep.rep_type = PHG_ARGS_EXTPTREP;
			ret.err = 0;
	        CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
		        &ret);
	        if ( ret.err) {
                *errind = ret.err;
            } else {
                Ppat_rep_plus	*rep = &ret.data.rep.extptrep;

                *dx = rep->dims.size_x;
                *dy = rep->dims.size_y;
                *ctype = rep->type;
                if ( *dimx < *dx || *dimy < *dy) {
	                *errind = ERR2001;

                } else {
                    switch ( *ctype) {
                    case PINDCT:
                        COLR_IND_DATA_RE_TRANSLATE( rep, *dx, *dy, *dimx, *dimy,
                            colia);
                        break;
                    case PRGB: case PCIE: case PHSV: case PHLS:
						*ol = 3;
                        if ( *ccsbsz < *ol) {
	                        *errind = ERR2001;

                        } else {
                            COLR_VAL_DATA_RE_TRANSLATE( rep, *dx, *dy, *dimx,
                                *dimy, colra);
                        }
                        break;
		            }
                }
	        }
	    }
    }
}


static void
fb_inq_filter( type, ws, isbsz, esbsz, error_ind, isn, is, esn, es)
Phg_args_flt_type	type;
Pint	ws;
Pint	isbsz;		/* inclusion set buffer size	*/
Pint	esbsz;		/* exclusion set buffer size	*/
Pint	*error_ind;	/* OUT error indicator	*/
Pint	*isn;		/* OUT number of names in the inclusion set */
Pint	*is;		/* OUT inclusion set	*/
Pint	*esn;		/* OUT number of names in the exclusion set */
Pint	*es;		/* OUT exclusion set	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;
    Phg_ret_filter	*filt = &ret.data.filter;
    Psl_ws_info		*wsinfo;
    Wst_phigs_dt	*dt;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *error_ind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
        *error_ind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, ws))) {
        *error_ind = ERR54;

    } else {
        *error_ind = 0;
        dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
        if ( !(dt->ws_category == POUTPT || 
               dt->ws_category == POUTIN ||
               dt->ws_category == PMO) ) {
            *error_ind = ERR59;

        }
        if ( !*error_ind) {
            cp_args.data.q_filter.wsid = ws;
            cp_args.data.q_filter.type = type;
            ret.err = 0;
            CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_FILTER, &cp_args, &ret);
            if ( ret.err) {
                *error_ind = ret.err;

            } else {
                if ( isbsz < filt->incl.num_ints ||
                     esbsz < filt->excl.num_ints) {
                    *error_ind = ERR2001;

                } else {
                    *isn = filt->incl.num_ints;
                    *esn = filt->excl.num_ints;
                    if ( *isn > 0) {
                        bcopy( (char*)filt->incl.ints, (char*)is,
                            *isn * sizeof(Pint));
                    }
                    if ( *esn > 0) {
                        bcopy( (char*)filt->excl.ints, (char*)es,
                            *esn * sizeof(Pint));
                    }
                }
            }
        }
    }
}


/* INQUIRE HIGHLIGHTING FILTER */
void
pqhlft_( wkid, isbsz, esbsz, errind, isn, is, esn, es)
Pint	*wkid;		/* workstation identifier	*/
Pint	*isbsz;		/* inclusion set buffer size	*/
Pint	*esbsz;		/* exclusion set buffer size	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*isn;		/* OUT number of names in the inclusion set */
Pint	*is;		/* OUT inclusion set	*/
Pint	*esn;		/* OUT number of names in the exclusion set */
Pint	*es;		/* OUT exclusion set	*/
{
    fb_inq_filter( PHG_ARGS_FLT_HIGH, *wkid, *isbsz, *esbsz, errind, isn, is,
        esn, es);
}


/* INQUIRE INVISIBILITY FILTER */
void
pqivft_( wkid, isbsz, esbsz, errind, isn, is, esn, es)
Pint	*wkid;		/* workstation identifier	*/
Pint	*isbsz;		/* inclusion set buffer size	*/
Pint	*esbsz;		/* exclusion set buffer size	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*isn;		/* OUT number of names in the inclusion set */
Pint	*is;		/* OUT inclusion set	*/
Pint	*esn;		/* OUT number of names in the exclusion set */
Pint	*es;		/* OUT exclusion set	*/
{
    fb_inq_filter( PHG_ARGS_FLT_INVIS, *wkid, *isbsz, *esbsz, errind, isn, is,
        esn, es);
}


/* INQUIRE COLOUR MODEL */
void
pqcmd_( wkid, errind, cmodel)
Pint	*wkid;		/* workstation identifier	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*cmodel;	/* OUT current colour model	*/
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
	dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
	if ( !(dt->ws_category == POUTPT || 
	       dt->ws_category == POUTIN ||
	       dt->ws_category == PMO) ) {
	    *errind = ERR59;

	} else {
	    cp_args.data.idata = *wkid;
            ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_COLOUR_MODEL, &cp_args, &ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else {
		*errind = 0;
		*cmodel = ret.data.idata;
	    }
	}
    }
}


/* INQUIRE HLHSR MODE */
void
pqhrm_( wkid, errind, hupd, chrm, rhrm)
Pint	*wkid;		/* workstation identifier	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*hupd;		/* OUT HLHSR mode update state	*/
Pint	*chrm;		/* OUT current HLHSR mode	*/
Pint	*rhrm;		/* OUT requested HLHSR mode	*/
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

    } else if ( ((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt.ws_category == PMI) {
        *errind = ERR57;

    } else {
	cp_args.data.idata = *wkid;
	ret.err = 0;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_HLHSR_MODE, &cp_args, &ret);
	if ( ret.err) {
	    *errind = ret.err;
	} else {
	    *errind = 0;
	    *hupd = (Pint)ret.data.hlhsr_mode.state;
	    *chrm = ret.data.hlhsr_mode.cur_mode;
	    *rhrm = ret.data.hlhsr_mode.req_mode;
	}
    }
}


/* INQUIRE POLYLINE REPRESENTATION */
void
pqplr_( wkid, pli, type, errind, ltype, lwidth, coli)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pli;		/* polyline index	*/
Pint	*type;		/* type of returned value	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ltype;		/* OUT linetype	*/
Pfloat	*lwidth;	/* OUT linewidth scale factor	*/
Pint	*coli;		/* OUT polyline colour index	*/
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

	} else if ( *pli < 1) {
	    *errind = ERR100;

	} else if ( *type < PSET || *type > PREALI) {
	    *errind = ERR2000;

	}

	if ( !*errind) {
	    cp_args.data.q_rep.wsid = *wkid;
	    cp_args.data.q_rep.index = *pli;
	    cp_args.data.q_rep.type = (Pinq_type)*type;
	    cp_args.data.q_rep.rep_type = PHG_ARGS_EXTLNREP;
            ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
		&ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else  if ( ret.data.rep.extlnrep.colr.type != PINDCT) {
		*errind = ERR134;
	    } else {
		*ltype = ret.data.rep.extlnrep.type;
		*lwidth = ret.data.rep.extlnrep.width;
		*coli = ret.data.rep.extlnrep.colr.val.ind;
	    }
	}
    }
}


/* INQUIRE POLYLINE REPRESENTATION PLUS */
void
pqplrp_( wkid, pli, ccsbsz, type, errind, ltype, lwidth, ctype, coli, ol, colr,
plsm, acri, aval)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pli;		/* polyline index	*/
Pint	*ccsbsz;	/* colour component buffer size	*/
Pint	*type;		/* type of returned value	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ltype;		/* OUT linetype	*/
Pfloat	*lwidth;	/* OUT linewidth scale factor	*/
Pint	*ctype;		/* OUT colour type	*/
Pint	*coli;		/* OUT colour index	*/
Pint	*ol;		/* OUT number of colour components	*/
Pfloat	*colr;		/* OUT colour specification	*/
Pint	*plsm;		/* OUT polyline shading method	*/
Pint	*acri;		/* OUT curve approximation criteria type */
Pfloat	*aval;		/* OUT curve approximation criteria values */
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

	} else if ( *pli < 1) {
	    *errind = ERR100;

	} else if ( *type < PSET || *type > PREALI) {
	    *errind = ERR2000;

        }

	if ( !*errind) {
	    cp_args.data.q_rep.wsid = *wkid;
	    cp_args.data.q_rep.index = *pli;
	    cp_args.data.q_rep.type = (Pinq_type)*type;
	    cp_args.data.q_rep.rep_type = PHG_ARGS_EXTLNREP;
        ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
		&ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else {
		*ltype = ret.data.rep.extlnrep.type;
		*lwidth = ret.data.rep.extlnrep.width;
		*plsm = ret.data.rep.extlnrep.shad_meth;
		*acri = ret.data.rep.extlnrep.approx_type;
		*aval = ret.data.rep.extlnrep.approx_val;
                GCOLR_DATA_RE_TRANSLATE_PLUS( ret.data.rep.extlnrep.colr,
                    *ccsbsz, *errind, *ctype, *coli, *ol, colr);
	    }
	}
    }
}


/* INQUIRE POLYMARKER REPRESENTATION */
void
pqpmr_( wkid, pmi, type, errind, mtype, mszsf, coli)
Pint 	*wkid;		/* workstation identifier	*/
Pint 	*pmi;		/* polymarker index	*/
Pint	*type;		/* type of returned value	*/
Pint 	*errind;	/* OUT error indicator	*/
Pint 	*mtype;		/* OUT marker type	*/
Pfloat	*mszsf;		/* OUT marker size scale factor	*/
Pint	*coli;		/* OUT polymarker colour index	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;
    Psl_ws_info		*wsinfo;
    Wst_phigs_dt	*dt;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) !=PWSOP) {
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

	} else if ( *pmi < 1) {
	    *errind = ERR100;

	} else if ( *type < PSET || *type > PREALI) {
	    *errind = ERR2000;

        }

	if ( !*errind) {
	    cp_args.data.q_rep.wsid = *wkid;
	    cp_args.data.q_rep.index = *pmi;
	    cp_args.data.q_rep.type = (Pinq_type)*type;
	    cp_args.data.q_rep.rep_type = PHG_ARGS_EXTMKREP;
        ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
		&ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else if ( ret.data.rep.extmkrep.colr.type != PINDCT) {
		*errind = ERR134;
	    } else {
		*mtype = ret.data.rep.extmkrep.type;
		*mszsf = ret.data.rep.extmkrep.size;
		*coli = ret.data.rep.extmkrep.colr.val.ind;
	    }
	}
    }
}


/* INQUIRE POLYMARKER REPRESENTATION PLUS */
void
pqpmrp_( wkid, pmi, ccsbsz, type, errind, mtype, mszsf, ctype, coli, ol, colr) 
Pint	*wkid;		/* workstation identifier	*/
Pint	*pmi;		/* polymarker index	*/
Pint	*ccsbsz;	/* colour component buffer size	*/
Pint	*type;		/* type of returned value	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mtype;		/* OUT markertype	*/
Pint	*mszsf;		/* OUT marker size scale factor	*/
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
	       dt->ws_category == PMO )) {
	    *errind = ERR59;

	} else if ( *pmi < 1) {
	    *errind = ERR100;

	} else if ( *type < PSET || *type > PREALI) {
	    *errind = ERR2000;

        }

	if ( !*errind) {
	    cp_args.data.q_rep.wsid = *wkid;
	    cp_args.data.q_rep.index = *pmi;
	    cp_args.data.q_rep.type = (Pinq_type)*type;
	    cp_args.data.q_rep.rep_type = PHG_ARGS_EXTMKREP;
        ret.err = 0;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
		&ret);
	    if ( ret.err) {
		*errind = ret.err;
	    } else {
		*mtype = ret.data.rep.extmkrep.type;
		*mszsf = ret.data.rep.extmkrep.size;
                GCOLR_DATA_RE_TRANSLATE_PLUS( ret.data.rep.extmkrep.colr,
                    *ccsbsz, *errind, *ctype, *coli, *ol, colr);
	    }
	}
    }
}


static Pint
check_pseudo_N_colours( colr_model, colrs )
    Pint                colr_model;
    Pfloat_list_list    *colrs;
{
    Pint        err = 0;

    register int        i, j;

    switch ( colr_model ) {
        case PRGB: case PCIE: case PHSV: case PHLS:
            if ( colrs->num_lists != 3 )
                err = ERR138;
            else {
                for ( i = 0; i < colrs->num_lists; i++ )
                    for ( j = 0; j < colrs->lists[i].num_floats; j++ )
                        if ( colrs->lists[i].floats[j] < 0.0
                                || colrs->lists[i].floats[j] > 1.0 )
                            return ERR136;
            }
            break;
        default:
            err = ERR110;
            break;
    }

    return err;
}


static int
colr_mapping_data_rec_valid( xdt, map_method, map_data )
    Wst_xwin_dt         *xdt;
    Pint                map_method;
    Pcolr_map_data      *map_data;
{
    Pint        err = 0;
    Pint        cm;

    switch ( map_method ) {
        case PCOLR_MAP_TRUE:
            /* no data record contents */
            break;

        case PCOLR_MAP_PSEUDO:
            cm = map_data->meth_r2.colr_model;
            if ( !phg_cb_int_in_list( cm, xdt->num_colour_approx_models,
                    xdt->colour_approx_models ) )
                err = ERR110;
            else if ( (cm == PRGB || cm == PCIE ||
                       cm == PHLS || cm == PHSV)
                    && map_data->meth_r2.weights.num_floats != 3 )
                err = ERR138;
            else if ( !phg_colours_valid( map_data->meth_r2.colrs.num_colr_reps,
                    map_data->meth_r2.colr_model,
                    (Pcolr_rep *)map_data->meth_r2.colrs.colr_reps ) )
                err = ERR136;
            break;

        case PCOLR_MAP_PSEUDO_N:
            if ( !phg_cb_int_in_list( map_data->meth_r3.colr_model,
                    xdt->num_colour_approx_models,
                    xdt->colour_approx_models ) )
                err = ERR110;
            else
                err = check_pseudo_N_colours( map_data->meth_r3.colr_model,
                    &map_data->meth_r3.colr_lists );
            break;
    }

    if ( err ) {
        ERR_REPORT(phg_cur_cph->erh, err);
    }
    return (err == 0 ? 1 : 0);
}


/* SET COLOUR MAPPING REPRESENTATION */
void
pscmr_( wkid, cmi, cmm, ldr, darec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*cmi;		/* colour mapping index	*/
Pint	*cmm;		/* mapping method	*/
Pint	*ldr;		/* length og data record array	*/
char	*darec;		/* data record	*/
{
    Phg_args                    cp_args;
    register Phg_args_set_rep   *args = &cp_args.data.set_rep;
    register Wst_phigs_dt       *dt;
    register Wst_xwin_dt        *xdt;
    Psl_ws_info                 *wsinfo;
    Pcolr_map_data		map;
    Pint			w_ia = 0;
    int				i;
    Pint			*i_ptr = NULL;
    Pdata_rec		r;
    int				err = 0;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR3, Pfn_set_colr_map_rep)) {
        if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR3);

        } else if (!(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
            ERR_REPORT( phg_cur_cph->erh, ERR54);

        } else {
            dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
            xdt = &((Wst*)wsinfo->wstype)->desc_tbl.xwin_dt;
            if ( !( dt->ws_category == POUTIN || dt->ws_category == POUTPT
                || dt->ws_category == PMO) ) {
                ERR_REPORT( phg_cur_cph->erh, ERR59);

            } else if ( *cmi < 0) {
                ERR_REPORT( phg_cur_cph->erh, ERR121);

            } else if ( !phg_cb_int_in_list( *cmm,
                dt->out_dt.num_colr_mapping_methods,
                dt->out_dt.colr_mapping_methods)) {
                ERR_REPORT( phg_cur_cph->erh, ERR126);

            } else if ( fb_unpack_datrec( ldr, darec, &err, &r)) {
                ERR_REPORT( phg_cur_cph->erh, err);

            } else {
                switch ( *cmm) {
                case PCOLR_MAP_TRUE:
                    /* no data record contents */
                    break;
                case PCOLR_MAP_PSEUDO:
                    if ( r.data.il != 3 ||
                        r.data.rl != (r.data.ia[1] + (r.data.ia[2] * 3))) {
                        err = ERR2003;
                    } else {
                        map.meth_r2.colr_model = r.data.ia[0];
                        map.meth_r2.weights.num_floats = r.data.ia[1];
                        map.meth_r2.colrs.num_colr_reps = r.data.ia[2];
                        map.meth_r2.weights.floats = r.data.ra;
                        if ( !err) {
                            /* bad case PINDCT */
                            if ( map.meth_r2.colr_model != PINDCT) {
                                map.meth_r2.colrs.colr_reps =
                                    (Pcolr_rep *)&(r.data.ra[r.data.ia[1]]);
                            }
                        }
                    }
                    break;
                case PCOLR_MAP_PSEUDO_N:
                    if ( r.data.il < 2 ||
                        r.data.rl < (r.data.ia[r.data.ia[1] + 1])) {
                        err = ERR2003;
                    } else {
                        map.meth_r3.colr_model = r.data.ia[0];
                        map.meth_r3.colr_lists.num_lists = r.data.ia[1];
                        PMALLOC( err, Pfloat_list,
                            map.meth_r3.colr_lists.num_lists,
                            map.meth_r3.colr_lists.lists);
                        if ( !err) {
                            i_ptr = &(r.data.ia[2]);
                            w_ia = 0;
                            /* bad case PINDCT */
                            if ( map.meth_r3.colr_model != PINDCT) {
                                for ( i = 0; i < r.data.ia[1];i++) {
                                    map.meth_r3.colr_lists.lists[i].num_floats =
                                        i_ptr[i] - w_ia;
                                    map.meth_r3.colr_lists.lists[i].floats =
                                        r.data.ra + w_ia;
                                    w_ia = i_ptr[i];
                                }
                            }
                        }
                    }
                    break;
                }
                if ( err) {
                    ERR_REPORT( phg_cur_cph->erh, err);

                } else {
                    if ( colr_mapping_data_rec_valid( xdt, *cmm, &map)) {
                        /* Error 125 is detected in the WS code. */
                        args->wsid = *wkid;
                        args->type = PHG_ARGS_COLRMAPREP;
                        args->rep.index = *cmi;
                        args->rep.bundl.colrmaprep.method = *cmm;
                        args->rep.bundl.colrmaprep.rec = map;
                        CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_REP, &cp_args,
                            NULL);
                    }
                }
                switch( *cmm) {
                case PCOLR_MAP_PSEUDO_N:
                    PFREE( map.meth_r3.colr_lists.num_lists,
                        map.meth_r3.colr_lists.lists);
                }
				fb_del_datrec( &r);
            }
        }
    }
}



/* INQUIRE LIST element OF COLOUR MAPPING INDICES */
void
pqecmi_( wkid, n, errind, ol, cmi)
Pint	*wkid;		/* workstation identifier	*/
Pint	*n;		/* list element requested	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ol;		/* OUT number of colur mapping table entries	*/
Pint	*cmi;		/* OUT Nth element of list of colur mapping indices */
{
    Phg_args            cp_args;
    Psl_ws_info         *wsinfo;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
        *errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
        *errind = ERR54;

    } else if ( ((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt.ws_category == PMI) {
        *errind = ERR57;

    } else {
        cp_args.data.q_indices.wsid = *wkid;
        cp_args.data.q_indices.type = PHG_ARGS_COLRMAPREP;
        fb_ws_inq_table_indices( &cp_args, *n, errind, cmi, ol);
    }
}


/* INQUIRE COLOUR MAPPING REPRESENTATION */
void
pqcmr_( wkid, cmi, type, mldr, errind, cmm, ldr, darec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*cmi;		/* colour mapping index	*/
Pint	*type;		/* type of return value	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator  */
Pint	*cmm;		/* OUT colour mapping method	*/
Pint	*ldr;		/* OUT length of data record array	*/
char	*darec;		/* OUT data record	*/
{
    Phg_args            cp_args;
    Phg_ret             ret;
    Psl_ws_info         *wsinfo;
    Wst_phigs_dt        *dt;
    Pcolr_map_data	*map;
    Pint		*i_ptr = NULL;
    Pfloat		*f_ptr = NULL;
    Pint		w_ia = 0;
    int			i;

    Pint        il = 0;         /* number of integer entries    */
    Pint        *ia = NULL;     /* array containing integer entries     */
    Pint        rl = 0;         /* number of real entries       */
    Pfloat      *ra = NULL;	/* array containing real entries     */
    Pint        sl = 0;         /* number of character string entries   */
    Pint        *lstr = NULL;   /* length of character string entries   */
    char        *str = NULL;    /* character string entries     */

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

        } else if ( *cmi < 0) {
            *errind = ERR121;

        } else if ( *type < PSET || *type > PREALI) {
            *errind = ERR2000;

        }

        if ( !*errind) {
            cp_args.data.q_rep.wsid = *wkid;
            cp_args.data.q_rep.index = *cmi;
            cp_args.data.q_rep.type = (Pinq_type)*type;
            cp_args.data.q_rep.rep_type = PHG_ARGS_COLRMAPREP;
            ret.err = 0;
            CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_REPRESENTATION, &cp_args,
                        &ret);
            if ( ret.err) {
                *errind = ret.err;
                return;
            }

            *cmm = ret.data.rep.colrmaprep.method;
            map = &(ret.data.rep.colrmaprep.rec);
            switch ( *cmm) {
            case PCOLR_MAP_TRUE:
                /* no data record contents */
                break;
            case PCOLR_MAP_PSEUDO:
                il = 3;
                rl = map->meth_r2.weights.num_floats + 
                     map->meth_r2.colrs.num_colr_reps * 3; 
                PMALLOC( *errind, Pint, il, ia);
                PMALLOC( *errind, Pfloat, rl, ra);
                if ( !*errind) {
                    ia[0] = map->meth_r2.colr_model;
                    ia[1] = map->meth_r2.weights.num_floats;
                    ia[2] = map->meth_r2.colrs.num_colr_reps;
                    bcopy ((char *)map->meth_r2.weights.floats, (char *)ra,    
                        map->meth_r2.weights.num_floats * sizeof(Pfloat));
                    /* bad case PINDCT */
                    if ( map->meth_r2.colr_model != PINDCT) {
                        bcopy( (char *)map->meth_r2.colrs.colr_reps,
                            (char *)&ra[ia[1]], (sizeof(Pfloat) * ia[2]) );
                    }
                }
                break;
            case PCOLR_MAP_PSEUDO_N:
                il = 2 + map->meth_r3.colr_lists.num_lists;
                for ( i = 0; i < map->meth_r3.colr_lists.num_lists; i++) {
                    rl += map->meth_r3.colr_lists.lists[i].num_floats;
                }
                PMALLOC( *errind, Pint, il, ia);
                PMALLOC( *errind, Pfloat, rl, ra);
                if ( !*errind) {
                    ia[0] = map->meth_r3.colr_model;
                    ia[1] = map->meth_r3.colr_lists.num_lists;
                    i_ptr = &(ia[2]);
                    f_ptr = ra;
                    w_ia = 0;
                    for ( i = 0; i < map->meth_r3.colr_lists.num_lists; i++) {
                        i_ptr[i] = map->meth_r3.colr_lists.lists[i].num_floats
                                 + w_ia;
                        bcopy ((char *)map->meth_r3.colr_lists.lists[i].floats,
                            (char *)f_ptr,
                            map->meth_r3.colr_lists.lists[i].num_floats *
                            sizeof(Pfloat));
                        w_ia = i_ptr[i];
                        f_ptr += map->meth_r3.colr_lists.lists[i].num_floats;
                    }
                }
                break;
            }                    
            if ( !*errind) {
                fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr,
                    errind, ldr, darec);
            }
            PFREE( il, ia);
            PFREE( rl, ra);
        }
    }
}


/* INQUIRE COLOUR MAPPING STATE */
void
pqcms_( wkid, cmm, mldr, errind, ldr, darec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*cmm;		/* colour mapping method	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*ldr;		/* OUT length of data record array	*/
char	*darec;		/* OUT data record	*/
{
    Phg_args                    cp_args;
    Phg_ret                     ret;
    register Wst_phigs_dt       *dt;
    Psl_ws_info                 *wsinfo;

    Pint        il = 0;         /* number of integer entries    */
    Pint        ia[1];          /* array containing integer entries     */
    Pint        rl = 0;         /* number of real entries       */
    Pfloat      *ra = NULL;     /* array containing real entries     */
    Pint        sl = 0;         /* number of character string entries   */
    Pint        *lstr = NULL;   /* length of character string entries   */
    char        *str = NULL;    /* character string entries     */

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY)) {
        *errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
        *errind = ERR3;

    } else if ( !(wsinfo = phg_psl_get_ws_info( phg_cur_cph->psl, *wkid))) {
        *errind = ERR54;

    } else {
        dt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
        if ( !(dt->ws_category == POUTPT ||
               dt->ws_category == POUTIN ||
               dt->ws_category == PMO) ) {
            *errind = ERR59;

        } else if ( !phg_cb_int_in_list( *cmm,
            dt->out_dt.num_colr_mapping_methods,
            dt->out_dt.colr_mapping_methods ) ) {
            *errind = ERR126;

        } else {
            *errind = 0;
            switch ( *cmm) {
            case PCOLR_MAP_TRUE:
                il = 1;
                ia[0] = dt->out_dt.num_true_colours;
                break;
            case PCOLR_MAP_PSEUDO:
                cp_args.data.q_colr_map_meth_st.wsid = *wkid;
                cp_args.data.q_colr_map_meth_st.map_method = *cmm;
                ret.err = 0;
                CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_COLR_MAP_METH_ST, &cp_args,
                    &ret);
                if ( ret.err) {
                    *errind = ret.err;
                } else {
                    il = 1;
                    ia[0] = ret.data.idata;
                }
                break;
            case PCOLR_MAP_PSEUDO_N:
                break;
            }
            if ( !*errind) {
                fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr,
                    errind, ldr, darec);
            }
        }
    }
}
