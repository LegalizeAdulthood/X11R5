#ifndef lint
static char     sccsid[] = "@(#)fb_inp.c 1.1 91/09/07 FJ";
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

/* Input functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"

#define CB_WS_CATEGORY( _wsinfo) \
    (((Wst*)_wsinfo->wstype)->desc_tbl.phigs_dt.ws_category)

#define DEVICE_EXISTS( _idt, _class, _num) \
    ((_num) > 0 && (_num) <= (_idt)->num_devs._class)

#define LINE_BUNDLE_VALID( _lb, _dt ) \
    ( (_lb)->colour >= 0 \
     && \
      (_lb)->colour < (_dt)->out_dt.num_colour_indices \
     && \
      phg_cb_int_in_list( (_lb)->type, (_dt)->out_dt.num_linetypes, \
	  (_dt)->out_dt.linetypes) \
    )

/* Special values used for arguments 4 and 5 to input_ws_open */
#define NO_DT_NEEDED	((Wst_phigs_dt	**) NULL)
#define REPORT_ERROR	((Pint	*) NULL)

static Wst_input_wsdt *
input_ws_open( cph, ws, fnid, dtp, err_ind )
    Cp_handle           cph;
    Pint                ws;
    Pint                fnid;
    Wst_phigs_dt        **dtp;  /* non-null if the whole dt is also needed */
    Pint                *err_ind; /* if non-null store error, else report */
{
    Psl_ws_info         *wsinfo;
    Wst_input_wsdt      *idt = NULL;

    if (CB_ENTRY_CHECK( cph, ERR3, fnid)) {
        if ( PSL_WS_STATE( cph->psl) != PWSOP) {
            if ( !err_ind ) {
                ERR_REPORT( cph->erh, ERR3);
            } else
                *err_ind = ERR3;

        } else if ( !(wsinfo = phg_psl_get_ws_info( cph->psl, ws))) {
            if ( !err_ind ) {
                ERR_REPORT( cph->erh, ERR54);
            } else
                *err_ind = ERR54;

        } else if ( !(CB_WS_CATEGORY( wsinfo) == POUTIN
            || CB_WS_CATEGORY( wsinfo) == PINPUT) ) {
			if ( !err_ind ) {
                ERR_REPORT( cph->erh, ERR61);
            } else
                *err_ind = ERR61;

        } else {
            idt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt.in_dt;
            if ( dtp )
                *dtp = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
        }

    } else if ( err_ind )
        *err_ind = ERR3;

    return idt;
}

static Wst_input_wsdt *
outin_ws_open( cph, ws, fnid, dtp, err_ind )
    Cp_handle           cph;
    Pint                ws;
    Pint                fnid;
    Wst_phigs_dt        **dtp;  /* non-null if the whole dt is also needed */
    Pint                *err_ind; /* if non-null store error, else report */
{
    Psl_ws_info         *wsinfo;
    Wst_input_wsdt      *idt = NULL;

    if (CB_ENTRY_CHECK( cph, ERR3, fnid)) {
        if ( PSL_WS_STATE( cph->psl) != PWSOP) {
            if ( !err_ind ) {
                ERR_REPORT( cph->erh, ERR3);
            } else
                *err_ind = ERR3;

        } else if ( !(wsinfo = phg_psl_get_ws_info( cph->psl, ws))) {
            if ( !err_ind ) {
                ERR_REPORT( cph->erh, ERR54);
            } else
                *err_ind = ERR54;

        } else if ( CB_WS_CATEGORY( wsinfo) != POUTIN) {
            if ( !err_ind ) {
                ERR_REPORT( cph->erh, ERR60);
            } else
                *err_ind = ERR60;

        } else {
            idt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt.in_dt;
            if ( dtp )
                *dtp = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
        }

    } else if ( err_ind )
        *err_ind = ERR3;

    return idt;
}

static void
set_mode( ws, class, dev, mode, echo)
    Pint                        ws;
    Phg_args_idev_class         class;
    Pint                        dev;
    Pop_mode                    mode;
    Pecho_switch                echo;
{
    Phg_args                            cp_args;
    register Phg_args_inp_set_mode      *args = &cp_args.data.inp_set_mode;

    args->wsid = ws;
    args->data.class = class;
    args->data.dev = dev;
    args->data.mode = mode;
    args->data.echo = echo;
    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INP_SET_MODE, &cp_args, NULL);
}



/* SET LOCATOR MODE */
void
pslcm_( wkid, lcdnr, mode, esw)
Pint	*wkid;	/* workstation identifier	*/
Pint	*lcdnr;	/* locator device number	*/
Pint	*mode;	/* operating mode	*/
Pint	*esw;	/* echo switch	*/
{
    Wst_input_wsdt		*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_set_loc_mode,
	NO_DT_NEEDED, REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, loc, *lcdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

	} else if (( *mode < PREQU || *mode > PEVENT) ||
                    ( *esw < PNECHO || *esw > PECHO)) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else {
            set_mode( *wkid, PHG_ARGS_INP_LOC, *lcdnr, *mode, *esw);
	}
    }
}


/* SET STROKE MODE */
void
psskm_( wkid, skdnr, mode, esw)
Pint	*wkid;	/* workstation identifier	*/
Pint	*skdnr;	/* stroke device number	*/
Pint	*mode;	/* operating mode	*/
Pint	*esw;	/* echo switch	*/
{
    Wst_input_wsdt		*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_set_stroke_mode,
        NO_DT_NEEDED, REPORT_ERROR) ) {
	if ( !DEVICE_EXISTS( idt, stroke, *skdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

        } else if (( *mode < PREQU || *mode > PEVENT) ||
                    ( *esw < PNECHO || *esw > PECHO)) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else {
	    set_mode( *wkid, PHG_ARGS_INP_STK, *skdnr, *mode, *esw);
	}
    }
}


/* SET VALUATOR MODE */
void
psvlm_( wkid, vldnr, mode, esw)
Pint	*wkid;	/* workstation identifier	*/
Pint	*vldnr;	/* valuator device number	*/
Pint	*mode;	/* operating mode	*/
Pint	*esw;	/* echo switch	*/
{
    Wst_input_wsdt		*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_set_val_mode,
        NO_DT_NEEDED, REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, val, *vldnr) ) {
		ERR_REPORT( phg_cur_cph->erh, ERR250);

        } else if (( *mode < PREQU || *mode > PEVENT) ||
                    ( *esw < PNECHO || *esw > PECHO)) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else {
	    set_mode( *wkid, PHG_ARGS_INP_VAL, *vldnr, *mode, *esw);
	}
    }
}


/* SET CHOICE MODE */
void
pschm_( wkid, chdnr, mode, esw)
Pint	*wkid;	/* workstation identifier	*/
Pint	*chdnr;	/* choice device number	*/
Pint	*mode;	/* operating mode	*/
Pint	*esw;	/* echo switch	*/
{
    Wst_input_wsdt		*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_set_choice_mode,
        NO_DT_NEEDED, REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, choice, *chdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

        } else if (( *mode < PREQU || *mode > PEVENT) ||
                    ( *esw < PNECHO || *esw > PECHO)) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else {
	    set_mode( *wkid, PHG_ARGS_INP_CHC, *chdnr, *mode, *esw);
	}
    }
}


/* SET PICK MODE */
void
pspkm_( wkid, pkdnr, mode, esw)
Pint	*wkid;	/* workstation identifier	*/
Pint	*pkdnr;	/* pick device number	*/
Pint	*mode;	/* operating mode	*/
Pint	*esw;	/* echo switch	*/
{
    Wst_input_wsdt		*idt;

    if ( idt = outin_ws_open( phg_cur_cph, *wkid, Pfn_set_pick_mode,
        NO_DT_NEEDED, REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, pick, *pkdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

        } else if (( *mode < PREQU || *mode > PEVENT) ||
                    ( *esw < PNECHO || *esw > PECHO)) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else {
	    set_mode( *wkid, PHG_ARGS_INP_PIK, *pkdnr, *mode, *esw);
	}
    }
}


/* SET STRING MODE */
void
psstm_( wkid, stdnr, mode, esw)
Pint	*wkid;	/* workstation identifier	*/
Pint	*stdnr;	/* string device number	*/
Pint	*mode;	/* operating mode	*/
Pint	*esw;	/* echo switch	*/
{
    Wst_input_wsdt		*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_set_string_mode,
        NO_DT_NEEDED, REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, string, *stdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

        } else if (( *mode < PREQU || *mode > PEVENT) ||
                    ( *esw < PNECHO || *esw > PECHO)) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else {
            set_mode( *wkid, PHG_ARGS_INP_STR, *stdnr, *mode, *esw);
	}
    }
}


/* AWAIT EVENT */
void
pwait_( tout, wkid, icl, idnr)
Pfloat	*tout;		/* time out (seconds)	*/
Pint	*wkid;		/* OUT workstation identifier	*/
Pint	*icl;		/* OUT input class	*/
Pint	*idnr;		/* OUT logical input device number	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;
    Phg_ret_inp_event	*revt = &ret.data.inp_event;
    int			size;
    Ppoint3		*pts;
    Ppick_path_elem	*path;
    Pint		f_status;

    if ( CB_ENTRY_CHECK( phg_cur_cph, ERR3, Pfn_await_event)) {
        if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
            ERR_REPORT( phg_cur_cph->erh, ERR3);

        } else {
            cp_args.data.fdata = *tout;
            ret.err = 0;
            CP_FUNC( phg_cur_cph, CP_FUNC_OP_INP_AWAIT, &cp_args, &ret);
            if ( !ret.err) {
                *wkid = revt->id.ws;
                *icl = (Pint)revt->id.class;
                *idnr = revt->id.dev;
                /* Some devices need additional work done. */
                switch ( revt->id.class) {
                    case PSTROK:
                        /* Get space for the current event report. */
                        size = revt->data.stk.num_points * sizeof(Ppoint3);
                        if ( size > 0) {
                            if ( !(pts = (Ppoint3*)malloc((unsigned)size)) ) {
                                ERR_REPORT( phg_cur_cph->erh, ERR900);
                                revt->data.stk.num_points = 0;
                            } else {
                                bcopy( (char*)revt->data.stk.points, (char*)pts,
                                        size);
                                revt->data.stk.points = pts;
                            }
                        }
                        break;
                    case PPICK:
                        IN_ST_CONVERT_C_TO_F( revt->data.pik.status, f_status);
                        if ( f_status == POK) {
                            size = revt->data.pik.pick_path.depth
                                * sizeof(Ppick_path_elem);
                            if ( size > 0) {
                                if ( !(path = (Ppick_path_elem*)
                                                malloc((unsigned)size)) ) {
                                    ERR_REPORT( phg_cur_cph->erh, ERR900);
                                    IN_ST_CONVERT_F_TO_C( PNONE,
                                        revt->data.pik.status);
                                } else {
                                    bcopy(
                                      (char*)revt->data.pik.pick_path.path_list,
                                      (char*)path, size);
                                    revt->data.pik.pick_path.path_list = path;
                                }
                            }
                        }
                        break;
                    case PSTRIN: {
                        char        *str;
                        if ( revt->data.str.length > 0) {
                            if (!( str =
								malloc((unsigned)revt->data.str.length) )) {
                                ERR_REPORT( phg_cur_cph->erh, ERR900);
                            } else {
                                strcpy(str, revt->data.str.string);
                                revt->data.str.string = str;
                            }
                        }
                        } break;
                }
                PSL_CLEAR_CUR_EVENT( phg_cur_cph->psl)
                PSL_SET_CUR_EVENT_ID( phg_cur_cph->psl, revt->id);
                if ( revt->id.class != PNCLAS) 
                    PSL_SET_CUR_EVENT_DATA( phg_cur_cph->psl, revt->data);

            } else {
                /* Report errors immediately so user doesn't try to
                 * read garbage.
                 */
                ERR_FLUSH( phg_cur_cph->erh);
            }
        }
    }
}

static int
pf_device_exists( idt, class, num )
    Wst_input_wsdt      *idt;
    Pint                class;
    Pint                num;
{
    int         status = 0;

    switch ( class ) {
        case PLOCAT:
            status = DEVICE_EXISTS(idt,loc,num) ? 1 : 0;
            break;
        case PPICK:
            status = DEVICE_EXISTS(idt,pick,num) ? 1 : 0;
            break;
        case PSTROK:
            status = DEVICE_EXISTS(idt,stroke,num) ? 1 : 0;
            break;
        case PCHOIC:
            status = DEVICE_EXISTS(idt,choice,num) ? 1 : 0;
            break;
        case PVALUA:
            status = DEVICE_EXISTS(idt,val,num) ? 1 : 0;
            break;
        case PSTRIN:
            status = DEVICE_EXISTS(idt,string,num) ? 1 : 0;
            break;
    }
    return status;
}



/* FLUSH DEVICE EVENTS */
void
pflush_( wkid, icl, idnr)
Pint	*wkid;	/* workstation identifier	*/
Pint	*icl;	/* input class	*/
Pint	*idnr;	/* logical input device number	*/
{
    Phg_args		cp_args;
    Phg_args_inp_flush	*args = &cp_args.data.inp_flush;
    Wst_input_wsdt	*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_flush_events,
        NO_DT_NEEDED, REPORT_ERROR)) {
        if ( *icl < PLOCAT || *icl > PSTRIN) {
            ERR_REPORT( phg_cur_cph->erh, ERR2000);

        } else if ( !pf_device_exists( idt, *icl, *idnr) ) {
            ERR_REPORT( phg_cur_cph->erh, ERR250);

        } else {
            args->class = (Pin_class)*icl;
            args->wsid = *wkid;
            args->dev = *idnr;
            CP_FUNC( phg_cur_cph, CP_FUNC_OP_FLUSH_DEV, &cp_args, NULL);
        }
    }
}

static int
gevt_state_and_class_ok( cph, class, fnid)
    Cp_handle   cph;
    Pin_class   class;
{
    int         status = 1;

    if ( !CB_ENTRY_CHECK( cph, ERR3, fnid)) {
        status = 0;

    } else if ( PSL_WS_STATE( cph->psl) != PWSOP) {
        ERR_REPORT( cph->erh, ERR3);
        status = 0;

    } else if ( PSL_CUR_EVENT_CLASS( cph->psl) != class) {
        ERR_REPORT( cph->erh, ERR259);
        status = 0;
    }

    return status;
}


/* GET LOCATOR 3 */
void
pgtlc3_( viewi, lpx, lpy, lpz)
Pint	*viewi;		/* OUT view index       */
Pfloat	*lpx;		/* OUT locator position X */
Pfloat	*lpy;		/* OUT locator position Y */
Pfloat	*lpz;		/* OUT locator position Z */
{
    if ( gevt_state_and_class_ok( phg_cur_cph, PLOCAT, Pfn_get_loc3)) {
	*viewi = PSL_CUR_EVENT_DATA( phg_cur_cph->psl, loc).view_ind;
	*lpx = PSL_CUR_EVENT_DATA( phg_cur_cph->psl, loc).position.x;
	*lpy = PSL_CUR_EVENT_DATA( phg_cur_cph->psl, loc).position.y;
	*lpz = PSL_CUR_EVENT_DATA( phg_cur_cph->psl, loc).position.z;
    }
}


/* GET LOCATOR */
void
pgtlc_( viewi, lpx, lpy)
Pint	*viewi;		/* OUT view index       */
Pfloat	*lpx;		/* OUT locator position X */
Pfloat	*lpy;		/* OUT locator position Y */
{
    if ( gevt_state_and_class_ok( phg_cur_cph, PLOCAT, Pfn_get_loc) ) {
	*viewi = PSL_CUR_EVENT_DATA( phg_cur_cph->psl, loc).view_ind;
	*lpx = PSL_CUR_EVENT_DATA( phg_cur_cph->psl, loc).position.x;
	*lpy = PSL_CUR_EVENT_DATA( phg_cur_cph->psl, loc).position.y;
    }
}


/* GET STROKE 3 */
void
pgtsk3_( n, viewi, np, pxa, pya, pza)
Pint	*n;		/* dimension of arrays for stroke points */
Pint	*viewi;		/* OUT view index       */
Pint	*np;		/* OUT number of points	*/
Pfloat	*pxa;		/* OUT coordinates of points in stroke X	*/
Pfloat	*pya;		/* OUT coordinates of points in stroke Y	*/
Pfloat	*pza;		/* OUT coordinates of points in stroke Z	*/
{
    register Pstroke3	*stroke;

    if ( gevt_state_and_class_ok( phg_cur_cph, PSTROK, Pfn_get_stroke3)) {
	stroke = &PSL_CUR_EVENT_DATA( phg_cur_cph->psl, stk);
        if ( *n < stroke->num_points) {
            ERR_REPORT( phg_cur_cph->erh, ERR2001);
        
        } else {
	    *viewi = stroke->view_ind;
	    *np = stroke->num_points;
	    if ( *np > 0) {
                POINT3_DATA_RE_TRANSLATE( stroke->points, *np, pxa, pya, pza);
            }
        }
    }
}


/* GET STROKE */
void
pgtsk_( n, viewi, np, pxa, pya)
Pint	*n;		/* dimension of arrays for stroke points */
Pint	*viewi;		/* OUT view index       */
Pint	*np;		/* OUT number of points	*/
Pfloat	*pxa;		/* OUT coordinates of points in stroke X	*/
Pfloat	*pya;		/* OUT coordinates of points in stroke Y	*/
{
    register Pstroke3		*stroke;
    register int		i;

    if ( gevt_state_and_class_ok( phg_cur_cph, PSTROK, Pfn_get_stroke)) {
	stroke = &PSL_CUR_EVENT_DATA( phg_cur_cph->psl, stk);
        if ( *n < stroke->num_points) {
            ERR_REPORT( phg_cur_cph->erh, ERR2001);
        
        } else {
	    *viewi = stroke->view_ind;
	    *np = stroke->num_points;
	    for ( i = 0; i < *np; i++) {
	        pxa[i] = stroke->points[i].x;
	        pya[i] = stroke->points[i].y;
            }
	}
    }
}


/* GET VALUATOR */
void
pgtvl_( val)
Pfloat	*val;	/* OUT value	*/
{
    if ( gevt_state_and_class_ok( phg_cur_cph, PVALUA, Pfn_get_val)) {
	*val = PSL_CUR_EVENT_DATA( phg_cur_cph->psl, val);
    }
}


/* GET CHOICE */
void
pgtch_( stat, chnr)
Pint	*stat;		/* OUT status	*/
Pint	*chnr;		/* OUT choice number	*/
{
    if ( gevt_state_and_class_ok( phg_cur_cph, PCHOIC, Pfn_get_choice)) {
	IN_ST_CONVERT_C_TO_F( PSL_CUR_EVENT_DATA( phg_cur_cph->psl,chc).status,
            *stat);
        if ( *stat == POK) {
	    *chnr = PSL_CUR_EVENT_DATA( phg_cur_cph->psl,chc).choice;
        } else {
            *stat = PNCHOI;
        }
    }
}


/* GET PICK */
void
pgtpk_( ippd, stat, ppd, pp)
Pint	*ippd;		/* depth of pick path to return	*/
Pint	*stat;		/* OUT pick status	*/
Pint	*ppd;		/* OUT depth of actual pick path	*/
Pint	*pp;		/* OUT pick path	*/
{
    register Ppick	*pick;
    int			i;

    if ( gevt_state_and_class_ok( phg_cur_cph, PPICK, Pfn_get_pick)) {
	pick = &PSL_CUR_EVENT_DATA( phg_cur_cph->psl, pik);
        if ( *ippd < pick->pick_path.depth) {
            ERR_REPORT( phg_cur_cph->erh, ERR2001);

        } else {
	    IN_ST_CONVERT_C_TO_F( pick->status, *stat);
	    if ( *stat == POK) {
	        /* Return the real path depth but only return the number of
	         * elements that the user wants.
	         */
	        *ppd = pick->pick_path.depth;
                for ( i = 0; i < *ppd; i++) {
                    pp[i * 3] = pick->pick_path.path_list[i].struct_id;
                    pp[i * 3 + 1] = pick->pick_path.path_list[i].pick_id;
                    pp[i * 3 + 2] = pick->pick_path.path_list[i].elem_pos;
                }
            } else {
                *stat = PNPICK;
            }
	}
    }
}


/* GET STRING */
void
fgtst_( lostr, str, length)
Pint	*lostr;		/* OUT number of characters returned */
char	*str;		/* OUT string	*/
int	*length;	/* string length	*/
{
    Phg_string	*string;

    if ( gevt_state_and_class_ok( phg_cur_cph, PSTRIN, Pfn_get_string)) {
        string = &PSL_CUR_EVENT_DATA( phg_cur_cph->psl, str);
        *lostr = (string->length < 1) ? 0: (string->length - 1);
        if ( *length < *lostr) {
            ERR_REPORT( phg_cur_cph->erh, ERR2001);

        } else if ( *lostr > 0) {
            strcpy( str, string->string);

        }
    }
}

static void
sample_device( ws, dev, class, ret)
    Pint                        ws;
    Pint                        dev;
    Phg_args_idev_class         class;
    Phg_ret                     *ret;
{
    Phg_args                            cp_args;
    register Phg_args_inp_sample        *args = &cp_args.data.inp_sample;

    args->wsid = ws;
    args->dev = dev;
    args->class = class;
    ret->err = 0;
    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INP_SAMPLE, &cp_args, ret);
    if ( ret->err ) {
        /* Report errors immediately so user doesn't try to read garbage.  */
        ERR_FLUSH( phg_cur_cph->erh);
    }
}


/* SAMPLE LOCATOR 3 */
void
psmlc3_( wkid, lcdnr, viewi, lpx, lpy, lpz)
Pint	*wkid;		/* workstation identifier	*/
Pint	*lcdnr;		/* locator device number	*/
Pint	*viewi;		/* OUT view index	*/
Pfloat 	*lpx;		/* OUT locator position X	*/
Pfloat 	*lpy;		/* OUT locator position Y	*/
Pfloat 	*lpz;		/* OUT locator position Z	*/
{
    Phg_ret			ret;
    Wst_input_wsdt		*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_sample_loc3, NO_DT_NEEDED,
	REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, loc, *lcdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

	} else {
	    sample_device( *wkid, *lcdnr, PHG_ARGS_INP_LOC3, &ret);
	    if ( !ret.err) {
		*viewi = ret.data.inp_event.data.loc.view_ind;
		*lpx = ret.data.inp_event.data.loc.position.x;
		*lpy = ret.data.inp_event.data.loc.position.y;
		*lpz = ret.data.inp_event.data.loc.position.z;
	    }
	}
    }
}


/* SAMPLE LOCATOR */
void
psmlc_( wkid, lcdnr, viewi, lpx, lpy)
Pint	*wkid;		/* workstation identifier	*/
Pint	*lcdnr;		/* locator device number	*/
Pint	*viewi;		/* OUT view index	*/
Pfloat	*lpx;		/* OUT locator position X	*/
Pfloat	*lpy;		/* OUT locator position Y	*/
{
    Phg_ret			ret;
    Wst_input_wsdt		*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_sample_loc, NO_DT_NEEDED,
	REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, loc, *lcdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

	} else {
	    sample_device( *wkid, *lcdnr, PHG_ARGS_INP_LOC, &ret);
	    if ( !ret.err) {
		*viewi = ret.data.inp_event.data.loc.view_ind;
		*lpx = ret.data.inp_event.data.loc.position.x;
		*lpy = ret.data.inp_event.data.loc.position.y;
	    }
	}
    }
}


/* SAMPLE STROKE 3 */
void
psmsk3_( wkid, skdnr, n, viewi, np, pxa, pya, pza)
Pint	*wkid;		/* workstation identifier	*/
Pint	*skdnr;		/* stroke device number	*/
Pint	*n;		/* dimension of arrays for stroke points	*/
Pint	*viewi;		/* OUT view index	*/
Pint	*np;		/* OUT number of points	*/
Pfloat	*pxa;		/* OUT coordinates of points in stroke	X	*/
Pfloat	*pya;		/* OUT coordinates of points in stroke	Y	*/
Pfloat	*pza;		/* OUT coordinates of points in stroke	Z	*/
{
    Phg_ret			ret;
    Wst_input_wsdt		*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_sample_stroke3,
        NO_DT_NEEDED, REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, stroke, *skdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

	} else {
	    sample_device( *wkid, *skdnr, PHG_ARGS_INP_STK3, &ret);
	    if ( !ret.err) {
    		Pstroke3	*stk = &ret.data.inp_event.data.stk;
                if ( *n < stk->num_points) {
                    ERR_REPORT( phg_cur_cph->erh, ERR2001);

                } else {
		    *viewi = stk->view_ind;
		    *np = stk->num_points;
		    if ( *np > 0) {
                        POINT3_DATA_RE_TRANSLATE( stk->points, *np, pxa, pya,
                            pza);
                    }
                }
	    } else {
		/* Assign a safe value in case errors are not synchronous. */
		*np = 0;
            }
	}
    }
}


/* SAMPLE STROKE */
void
psmsk_( wkid, skdnr, n, viewi, np, pxa, pya)
Pint	*wkid;		/* workstation identifier	*/
Pint	*skdnr;		/* stroke device number	*/
Pint	*n;		/* arrays for stroke points	*/
Pint	*viewi;		/* OUT view index	*/
Pint	*np;		/* OUT number of points	*/
Pfloat	*pxa;		/* OUT coordinates of points in stroke X	*/
Pfloat	*pya;		/* OUT coordinates of points in stroke Y	*/
{
    Phg_ret			ret;
    Wst_input_wsdt		*idt;
    register int		i;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_sample_stroke,
        NO_DT_NEEDED, REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, stroke, *skdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

	} else {
	    sample_device( *wkid, *skdnr, PHG_ARGS_INP_STK, &ret);
	    if ( !ret.err) {
    		Pstroke3	*stk = &ret.data.inp_event.data.stk;
	        if ( *n < stk->num_points) {
	            ERR_REPORT( phg_cur_cph->erh, ERR2001);

                } else {
		    *viewi = stk->view_ind;
		    *np = stk->num_points;
		    for ( i = 0; i < *np; i++) {
		        pxa[i] = stk->points[i].x;
		        pya[i] = stk->points[i].y;
                    }
		}
	    } else {
		/* Assign a safe value in case errors are not synchronous. */
		*np = 0;
            }
	}
    }
}


/* SAMPLE VALUATOR */
void
psmvl_( wkid, vldnr, val)
Pint	*wkid;		/* workstation identifier	*/
Pint	*vldnr;		/* valuator device number	*/
Pfloat	*val;		/* OUT value	*/
{
    Phg_ret			ret;
    Wst_input_wsdt		*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_sample_val, NO_DT_NEEDED,
	REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, val, *vldnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

	} else {
	    sample_device( *wkid, *vldnr, PHG_ARGS_INP_VAL, &ret);
	    if ( !ret.err)
		*val = ret.data.inp_event.data.val;
	}
    }
}


/* SAMPLE CHOICE */
void
psmch_( wkid, chdnr, stat, chnr)
Pint	*wkid;		/* workstation identifier	*/
Pint	*chdnr;		/* choice device number	*/
Pint	*stat;		/* OUT status	*/
Pint	*chnr;		/* OUT choice number	*/
{
    Phg_ret			ret;
    Wst_input_wsdt		*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_sample_choice,
        NO_DT_NEEDED, REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, choice, *chdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

	} else {
	    sample_device( *wkid, *chdnr, PHG_ARGS_INP_CHC, &ret);
	    if ( !ret.err) {
		IN_ST_CONVERT_C_TO_F( ret.data.inp_event.data.chc.status,
                    *stat);
                if ( *stat == POK) {
                    *chnr = ret.data.inp_event.data.chc.choice;
                } else {
                    *stat = PNCHOI;
                }
	    } else {
		/* Assign a safe value in case errors are not synchronous. */
                *stat = PNCHOI;
            }
	}
    }
}


/* SAMPLE PICK */
void
psmpk_( wkid, pkdnr, ippd, stat, ppd, pp)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pkdnr;		/* pick device number	*/
Pint	*ippd;		/* depth of pick path to return	*/
Pint	*stat;		/* OUT pick input status	*/
Pint	*ppd;		/* OUT depth of actual pick path	*/
Pint	*pp;		/* OUT pick path	*/
{
    Phg_ret			ret;
    Wst_input_wsdt		*idt;
    Ppick			*pick;
    int				i;

    if ( idt = outin_ws_open( phg_cur_cph, *wkid, Pfn_sample_pick, NO_DT_NEEDED,
	REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, pick, *pkdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

	} else {
	    sample_device( *wkid, *pkdnr, PHG_ARGS_INP_PIK, &ret);
	    if ( !ret.err) {
		pick = &ret.data.inp_event.data.pik;
	        if ( *ippd < pick->pick_path.depth) {
	            ERR_REPORT( phg_cur_cph->erh, ERR2001);

                } else {
		    IN_ST_CONVERT_C_TO_F( pick->status, *stat);
		    if ( *stat == POK) {
		        /* Return the real path depth but only return the number
		         * of elements that the user wants.
		         */
		        *ppd = pick->pick_path.depth;
                        for ( i = 0; i < *ppd; i++) {
			    pp[i * 3] = pick->pick_path.path_list[i].struct_id;
			    pp[i * 3 + 1] =
                                pick->pick_path.path_list[i].pick_id;
			    pp[i * 3 + 2] =
                                pick->pick_path.path_list[i].elem_pos;
                        }
                    } else {
                        *stat = PNPICK;
                    }
		}
	    } else {
		/* Assign a safe value in case errors are not synchronous. */
                *stat = PNPICK;
            }
	}
    }
}


/* SAMPLE STRING */
void
fsmst_( wkid, stdnr, lostr, str, length)
Pint	*wkid;		/* workstation identifier	*/
Pint	*stdnr;		/* string device number	*/
Pint	*lostr;		/* OUT number of chracters returned	*/
char	*str;		/* OUT character string	*/
int	*length;	/* string length	*/
{
    Phg_ret			ret;
    Wst_input_wsdt		*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_sample_string,
        NO_DT_NEEDED, REPORT_ERROR)) {
        if ( !DEVICE_EXISTS( idt, string, *stdnr) ) {
            ERR_REPORT( phg_cur_cph->erh, ERR250);

        } else {
            sample_device( *wkid, *stdnr, PHG_ARGS_INP_STR, &ret);
            if ( !ret.err) {
                *lostr = (ret.data.inp_event.data.str.length < 1) ?
                    0: (ret.data.inp_event.data.str.length - 1);
                if ( *length < *lostr) {
                    ERR_REPORT( phg_cur_cph->erh, ERR2001);

                } else if ( *lostr > 0) {
                    strcpy( str, ret.data.inp_event.data.str.string);
                }
            } else {
                /* Assign a safe value in case errors are not synchronous. */
                *lostr = 0;
            }
        }
    }
}

static void
request_device( ws, dev, class, ret)
    Pint                        ws;
    Pint                        dev;
    Phg_args_idev_class         class;
    Phg_ret                     *ret;
{
    Phg_args                            cp_args;
    register Phg_args_inp_request       *args = &cp_args.data.inp_request;

    args->wsid = ws;
    args->dev = dev;
    args->class = class;
    ret->err = 0;
    CP_FUNC( phg_cur_cph, CP_FUNC_OP_INP_REQUEST, &cp_args, ret);
    if ( ret->err ) {
        /* Report errors immediately so user doesn't try to read garbage.  */
        ERR_FLUSH( phg_cur_cph->erh);
    }
}


/* REQUEST LOCATOR 3 */
void
prqlc3_( wkid, lcdnr, stat, viewi, px, py, pz)
Pint	*wkid;		/* workstation identifier	*/
Pint	*lcdnr;		/* locator device number	*/
Pint	*stat;		/* OUT status	*/
Pint 	*viewi;		/* OUT view index	*/
Pfloat	*px;		/* OUT locator position X	*/
Pfloat	*py;		/* OUT locator position Y	*/
Pfloat	*pz;		/* OUT locator position Z	*/
{
    Phg_ret				ret;
    register Phg_ret_inp_request	*req = &ret.data.inp_request;
    Wst_input_wsdt			*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_req_loc3, NO_DT_NEEDED,
	REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, loc, *lcdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

	} else {
	    request_device( *wkid, *lcdnr, PHG_ARGS_INP_LOC3, &ret);
	    if ( !ret.err) {
		IN_ST_CONVERT_C_TO_F( req->status.istat, *stat);
		if ( *stat == POK) {
		    *viewi = req->event.data.loc.view_ind;
		    *px = req->event.data.loc.position.x;
		    *py = req->event.data.loc.position.y;
		    *pz = req->event.data.loc.position.z;
		} else {
                    *stat = PNONE; 
                }
	    } else {
		/* The Standard doesn't say to do this but it's the only
		 * way the caller can detect a bad locator value without
		 * synchronizing errors.
		 */
		*stat = PNONE;
            }
	}
    }
}


/* REQUEST LOCATOR */
void
prqlc_( wkid, lcdnr, stat, viewi, px, py)
Pint	*wkid;		/* workstation identifier	*/
Pint	*lcdnr;		/* locator device number	*/
Pint	*stat;		/* OUT status	*/
Pint	*viewi;		/* OUT view index	*/
Pfloat	*px;		/* OUT locator position X	*/
Pfloat	*py;		/* OUT locator position Y	*/
{
    Phg_ret				ret;
    register Phg_ret_inp_request	*req = &ret.data.inp_request;
    Wst_input_wsdt			*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_req_loc, NO_DT_NEEDED,
	REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, loc, *lcdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

	} else {
	    request_device( *wkid, *lcdnr, PHG_ARGS_INP_LOC, &ret);
	    if ( !ret.err) {
                IN_ST_CONVERT_C_TO_F( req->status.istat, *stat);
		if ( *stat == POK) {
		    *viewi = req->event.data.loc.view_ind;
		    *px = req->event.data.loc.position.x;
		    *py = req->event.data.loc.position.y;
		} else {
                    *stat = PNONE;
                }
	    } else {
		/* Assign a safe value in case errors are not synchronous. */
		*stat = PNONE;
            }
	}
    }
}


/* REQUEST STROKE 3 */
void
prqsk3_( wkid, skdnr, n, stat, viewi, np, pxa, pya, pza)
Pint	*wkid;		/* workstation identifier	*/
Pint	*skdnr;		/* stroke device number	*/
Pint	*n;		/* dimension of arrays for stroke points */
Pint	*stat;		/* OUT status	*/
Pint	*viewi;		/* OUT view index	*/
Pint	*np;		/* OUT number of points	*/
Pfloat	*pxa;		/* OUT coordinates of points of stroke X */
Pfloat	*pya;		/* OUT coordinates of points of stroke Y */
Pfloat	*pza;		/* OUT coordinates of points of stroke Z */
{
    Phg_ret				ret;
    register Phg_ret_inp_request	*req = &ret.data.inp_request;
    Wst_input_wsdt			*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_req_stroke3, NO_DT_NEEDED,
	REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, stroke, *skdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

	} else {
	    request_device( *wkid, *skdnr, PHG_ARGS_INP_STK3, &ret);
	    if ( !ret.err) {
		IN_ST_CONVERT_C_TO_F( req->status.istat, *stat);
		if ( *stat == POK) {
		    if ( *n <  req->event.data.stk.num_points) {
	                ERR_REPORT( phg_cur_cph->erh, ERR2001);

                    } else {
	                *viewi = req->event.data.stk.view_ind;
		        *np = req->event.data.stk.num_points;
                        if ( *np > 0) 
                            POINT3_DATA_RE_TRANSLATE(req->event.data.stk.points,
                                *np, pxa, pya, pza);
                    }
		} else {
                    *stat = PNONE;
                }
	    } else {
		/* Assign a safe value in case errors are not synchronous. */
		*stat = PNONE;
	    }
	}
    }
}


/* REQUEST STROKE */
void
prqsk_( wkid, skdnr, n, stat, viewi, np, pxa, pya)
Pint	*wkid;		/* workstation identifier	*/
Pint	*skdnr;		/* stroke device number	*/
Pint	*n;		/* arrays for stroke points	*/
Pint	*stat;		/* OUT status	*/
Pint	*viewi;		/* OUT view index	*/
Pint	*np;		/* OUT number of points	*/
Pfloat	*pxa;		/* OUT coordinates of points of stroke X */
Pfloat	*pya;		/* OUT coordinates of points of stroke Y */
{
    Phg_ret				ret;
    register Phg_ret_inp_request	*req = &ret.data.inp_request;
    Wst_input_wsdt			*idt;
    register int			i;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_req_stroke, NO_DT_NEEDED,
	REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, stroke, *skdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

	} else {
	    request_device( *wkid, *skdnr, PHG_ARGS_INP_STK, &ret);
	    if ( !ret.err) {
		IN_ST_CONVERT_C_TO_F( req->status.istat, *stat);
		if ( *stat == POK) {
	 	    if ( *n < req->event.data.stk.num_points) {
	                ERR_REPORT( phg_cur_cph->erh, ERR2001);

                    } else {
		        *viewi = req->event.data.stk.view_ind;
	 	        *np = req->event.data.stk.num_points;
		        for ( i = 0; i < *np; i++) {
			    pxa[i] = req->event.data.stk.points[i].x;
			    pya[i] = req->event.data.stk.points[i].y;
                        }
		    }
		} else {
                    *stat = PNONE;
                }
	    } else {
		/* Assign a safe value in case errors are not synchronous. */
		*stat = PNONE;
	    }
	}
    }
}


/* REQUEST VALUATOR */
void
prqvl_( wkid, vldnr, stat, val)
Pint	*wkid;		/* workstation identifier	*/
Pint	*vldnr;		/* valuator device number	*/
Pint	*stat;		/* OUT status	*/
Pfloat	*val;		/* OUT value	*/
{
    Phg_ret				ret;
    register Phg_ret_inp_request	*req = &ret.data.inp_request;
    Wst_input_wsdt			*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_req_val, NO_DT_NEEDED,
	REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, val, *vldnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

	} else {
	    request_device( *wkid, *vldnr, PHG_ARGS_INP_VAL, &ret);
	    if ( !ret.err) {
		IN_ST_CONVERT_C_TO_F( req->status.istat, *stat);
		if ( *stat == POK) {
		    *val = req->event.data.val;
                } else {
                    *stat = PNONE;
                }
	    } else {
		/* Assign a safe value in case errors are not synchronous. */
		*stat = PNONE;
            }
	}
    }
}


/* REQUEST CHOICE */
void
prqch_( wkid, chdnr, stat, chnr)
Pint	*wkid;		/* workstation identifier	*/
Pint	*chdnr;		/* choice device number	*/
Pint	*stat;		/* OUT status	*/
Pint	*chnr;		/* OUT choice number 	*/
{
    Phg_ret				ret;
    register Phg_ret_inp_request	*req = &ret.data.inp_request;
    Wst_input_wsdt			*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_req_choice, NO_DT_NEEDED,
	REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, choice, *chdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

	} else {
	    request_device( *wkid, *chdnr, PHG_ARGS_INP_CHC, &ret);
	    if ( !ret.err) {
		IN_ST_CONVERT_C_TO_F( req->status.chstat, *stat);
		if ( *stat == POK) {
		    *chnr = req->event.data.chc.choice;
		}
	    } else {
		/* Assign a safe value in case errors are not synchronous. */
		*stat = PNCHOI;
            }
	}
    }
}


/* REQUEST PICK */
void
prqpk_( wkid, pkdnr, ippd, stat, ppd, pp)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pkdnr;		/* pick device number	*/
Pint	*ippd;		/* depth of pick path to return	*/
Pint	*stat;		/* OUT status	*/
Pint	*ppd;		/* OUT depth of actual pick path	*/
Pint	*pp;		/* OUT pick path	*/
{
    Phg_ret				ret;
    register Phg_ret_inp_request	*req = &ret.data.inp_request;
    Wst_input_wsdt			*idt;
    int					i;

    if ( idt = outin_ws_open( phg_cur_cph, *wkid, Pfn_req_pick, NO_DT_NEEDED,
	REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, pick, *pkdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);

	} else {
	    request_device( *wkid, *pkdnr, PHG_ARGS_INP_PIK, &ret);
	    if ( !ret.err) {
		Ppick	*pick = &req->event.data.pik;

		IN_ST_CONVERT_C_TO_F( req->status.pkstat, *stat);
		if ( *stat == POK) {
		    /* Return the real path depth but only return the number
		     * of elements that the user wants.
		     */
		    if ( *ippd < pick->pick_path.depth) {
	                ERR_REPORT( phg_cur_cph->erh, ERR2001);
              
                    } else {
		        *ppd = pick->pick_path.depth;
                        for ( i = 0; i < *ppd; i++) {
			    pp[i * 3] = pick->pick_path.path_list[i].struct_id;
			    pp[i * 3 + 1] =
                                pick->pick_path.path_list[i].pick_id;
			    pp[i * 3 + 2] =
                                pick->pick_path.path_list[i].elem_pos;
                        }
                    }
		}
	    } else {
		/* Assign a safe value in case errors are not synchronous. */
		*stat = PNPICK;
            }
	}
    }
}


/* REQUEST STRING */
void
frqst_( wkid, stdnr, stat, lostr, str, length)
Pint	*wkid;		/* workstation identifier	*/
Pint	*stdnr;		/* string device number	*/
Pint	*stat;		/* OUT status	*/
Pint	*lostr;		/* OUT nunber of characters returned	*/
char	*str;		/* OUT character string	*/
int	*length;	/* string length	*/
{
    Phg_ret			ret;
    register Phg_ret_inp_request	*req = &ret.data.inp_request;
    Wst_input_wsdt		*idt;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_req_string, NO_DT_NEEDED,
        REPORT_ERROR)) {
        if ( !DEVICE_EXISTS( idt, string, *stdnr) ) {
            ERR_REPORT( phg_cur_cph->erh, ERR250);

        } else {
            request_device( *wkid, *stdnr, PHG_ARGS_INP_STR, &ret);
            if ( !ret.err) {
                IN_ST_CONVERT_C_TO_F( req->status.istat, *stat);
                if ( *stat == POK) {
                    *lostr = (req->event.data.str.length < 1) ?
                        0: (req->event.data.str.length - 1);
                    if ( *length < *lostr) {
                        ERR_REPORT( phg_cur_cph->erh, ERR2001);

                    } else if ( *lostr > 0){
        	            strcpy( str, req->event.data.str.string);
                    }
                } else {
                    *stat = PNONE;
                }
            } else {
                /* Assign a safe value in case errors are not synchronous. */
                *stat = PNONE;
            }
        }
    }
}

static int
loc_data_rec_ok( cph, pet, rec, dt, ddt )
    Cp_handle                   cph;
    Pint                        pet;
    Ploc_data                   *rec;
    Wst_phigs_dt                *dt;
    Wst_defloc                  *ddt;
{
    int         status = 0;
    Pint        err;

    switch ( pet ) {
        case 1:
        case 2:
        case 3:
            /* No data */
            status = !0;
            break;

        default:
            /* Shouldn't get here, pet should be verified before calling. */
             err = ERR260;
            break;
    }

    if ( !status ) {
        ERR_REPORT( cph->erh, err);
    }
    return status;
}


static void
fb_init_loc( cph, func_id, ws, dev, init_view, init_pos, pet, ev, ldr, datrec,
cp_args)
Cp_handle	cph;
Pint		func_id;
Pint		ws;		/* workstation identifier */
Pint		dev;		/* valuator device number */
Pint		init_view;
Ppoint3		*init_pos;
Pint		pet;		/* prompt and echo type	*/
Plimit3		*ev;		/* echo volume 	*/
Pint		ldr;		/* dimension of data record array	*/
char		*datrec;	/* data record 	*/
Phg_args	*cp_args;
{
    register Phg_args_inp_init_dev	*args = &cp_args->data.inp_init_dev;
    Wst_input_wsdt			*idt;
    Wst_phigs_dt			*dt;
    Ploc_data3				rec;
    Pint            err = 0;
	Pdata_rec       r;

    if ( idt = input_ws_open( cph, ws, func_id, &dt, REPORT_ERROR)) {
        if ( !DEVICE_EXISTS( idt, loc, dev) ) {
            ERR_REPORT( cph->erh, ERR250);

        } else if ( !phg_cb_echo_limits_valid( cph, func_id, ws, ev, dt) ) {
            /* Error has been reported, just fall out of the if. */
            ;
        } else if ( init_view < 0 || init_view >= dt->num_view_indices) {
            ERR_REPORT( cph->erh, ERR114);

        } else {
            Wst_defloc                *ddt = &idt->locators[dev - 1];

            if ( !phg_cb_int_in_list( pet, ddt->num_pets, ddt->pets) ) {
                /* Bad pet, use pet 1 with default data. */
                ERR_REPORT( cph->erh, ERR253);
                rec = ddt->record;
                pet = 1;

            } else if ( fb_unpack_datrec( ldr, datrec, &err, &r)) {
                ERR_REPORT( cph->erh, err);

            } else {
                /* change FORTRAN data decord to C data record */
                switch ( pet) {
                case 4:
                    if ( r.data.il != 7 || r.data.rl != 1) {
                        ERR_REPORT( cph->erh, ERR260);
                        err = ERR260;
                    } else {
                        /* r.data.ia[0] is unused */  
                        rec.pets.pet_r4.line_attrs.type_asf =
                            (Pasf)r.data.ia[1];
                        rec.pets.pet_r4.line_attrs.width_asf =
                            (Pasf)r.data.ia[2];
                        rec.pets.pet_r4.line_attrs.colr_ind_asf =
                            (Pasf)r.data.ia[3];
                        rec.pets.pet_r4.line_attrs.ind = r.data.ia[4];
                        rec.pets.pet_r4.line_attrs.bundle.type = r.data.ia[5];
                        rec.pets.pet_r4.line_attrs.bundle.colr_ind =
                            r.data.ia[6];
                        rec.pets.pet_r4.line_attrs.bundle.width = r.data.ra[0];
                    }
                    break;
                case 5:
                    switch ( r.data.ia[0]) {
                    case PPLINE:
                        if ( r.data.il != 8 || r.data.rl != 1) {
                            ERR_REPORT( cph->erh, ERR260);
                            err = ERR260;
                        } else {
                            rec.pets.pet_r5.line_fill_ctrl_flag =
                                (Pline_fill_ctrl_flag)r.data.ia[0];
                            /* r.data.ia[1] is unused */  
                            rec.pets.pet_r5.attrs.line_attrs.type_asf =
                                (Pasf)r.data.ia[2];
                            rec.pets.pet_r5.attrs.line_attrs.width_asf =
                                (Pasf)r.data.ia[3];
                            rec.pets.pet_r5.attrs.line_attrs.colr_ind_asf =
                                (Pasf)r.data.ia[4];
                            rec.pets.pet_r5.attrs.line_attrs.ind = r.data.ia[5];
                            rec.pets.pet_r5.attrs.line_attrs.bundle.type =
                                r.data.ia[6];
                            rec.pets.pet_r5.attrs.line_attrs.bundle.colr_ind =
                                r.data.ia[7];
                            rec.pets.pet_r5.attrs.line_attrs.bundle.width =
                                r.data.ra[0];
                        }
                        break;
                    case PFILLA:
                        if ( r.data.il != 9) {
                            ERR_REPORT( cph->erh, ERR260);
                            err = ERR260;
                        } else {
                            rec.pets.pet_r5.line_fill_ctrl_flag =
                                (Pline_fill_ctrl_flag)r.data.ia[0];
                            /* ia[1] is unused */  
                            rec.pets.pet_r5.attrs.int_attrs.style_asf =
                                (Pasf)r.data.ia[2];
                            rec.pets.pet_r5.attrs.int_attrs.style_ind_asf =
                                (Pasf)r.data.ia[3];
                            rec.pets.pet_r5.attrs.int_attrs.colr_ind_asf =
                                (Pasf)r.data.ia[4];
                            rec.pets.pet_r5.attrs.int_attrs.ind = r.data.ia[5];
                            rec.pets.pet_r5.attrs.int_attrs.bundle.style =
                                (Pint_style)r.data.ia[6];
                            rec.pets.pet_r5.attrs.int_attrs.bundle.style_ind =
                                r.data.ia[7];
                            rec.pets.pet_r5.attrs.int_attrs.bundle.colr_ind =
                                r.data.ia[8];
                        }
                        break;
                    case PFILAS:
                        if ( r.data.il != 17 || r.data.rl != 1) {
                            ERR_REPORT( cph->erh, ERR260);
                            err = ERR260;
                        } else {
                            rec.pets.pet_r5.line_fill_ctrl_flag =
                                (Pline_fill_ctrl_flag)r.data.ia[0];
                            /* r.data.ia[1] is unused */  
                            rec.pets.pet_r5.attrs.fill_set.int_attrs.style_asf =
                                (Pasf)r.data.ia[2];
                            rec.pets.pet_r5.attrs.fill_set.int_attrs.style_ind_asf =
                                (Pasf)r.data.ia[3];
                            rec.pets.pet_r5.attrs.fill_set.int_attrs.colr_ind_asf =
                                (Pasf)r.data.ia[4];
                            rec.pets.pet_r5.attrs.fill_set.int_attrs.ind =
                                r.data.ia[5];
                            rec.pets.pet_r5.attrs.fill_set.int_attrs.bundle.style =
                                (Pint_style)r.data.ia[6];
                            rec.pets.pet_r5.attrs.fill_set.int_attrs.bundle.style_ind
                                = r.data.ia[7];
                            rec.pets.pet_r5.attrs.fill_set.int_attrs.bundle.colr_ind
                                = r.data.ia[8];

                            rec.pets.pet_r5.attrs.fill_set.edge_attrs.flag_asf =
                                (Pasf)r.data.ia[9];
                            rec.pets.pet_r5.attrs.fill_set.edge_attrs.type_asf =
                                (Pasf)r.data.ia[10];
                            rec.pets.pet_r5.attrs.fill_set.edge_attrs.width_asf =
                                (Pasf)r.data.ia[11];
                            rec.pets.pet_r5.attrs.fill_set.edge_attrs.colr_ind_asf =
                                (Pasf)r.data.ia[12];
                            rec.pets.pet_r5.attrs.fill_set.edge_attrs.ind =
                                r.data.ia[13];
                            rec.pets.pet_r5.attrs.fill_set.edge_attrs.bundle.flag =
                                (Pedge_flag)r.data.ia[14];
                            rec.pets.pet_r5.attrs.fill_set.edge_attrs.bundle.type =
                                r.data.ia[15];
                            rec.pets.pet_r5.attrs.fill_set.edge_attrs.bundle.colr_ind = 
                                r.data.ia[16];
                            rec.pets.pet_r5.attrs.fill_set.edge_attrs.bundle.width =
                                r.data.ra[0];
                        }
                        break;
                    default:
                        ERR_REPORT( cph->erh, ERR260);
                        err = ERR260;
                        break;
                    }
                    break;
                default:
                    /* Data record is not used */
                    break;
                }
            }

            if ( !err) {
                if ( loc_data_rec_ok( cph, pet, &rec, dt, ddt ) ) {
                    args->wsid = ws;
                    args->dev = dev;
                    args->pet = pet;
                    args->echo_volume = *ev;
                    args->data.loc.init.view_ind = init_view;
                    args->data.loc.init.position = *init_pos;
                    args->data.loc.rec = rec;
                    args->class = func_id == Pfn_init_loc3 ?
                        PHG_ARGS_INP_LOC3 : PHG_ARGS_INP_LOC ;
                    CP_FUNC( cph, CP_FUNC_OP_INP_INIT_DEV, cp_args, NULL);
                }
            }
            fb_del_datrec( &r);
        }
    }
}


/* INITIALIZE LOCATOR 3 */
void
pinlc3_( wkid, lcdnr, iviewi, ipx, ipy, ipz, pet, evol, ldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*lcdnr;		/* locator device number	*/
Pint	*iviewi;	/* initial view indicator	*/
Pfloat	*ipx;		/* initial locator position X	*/
Pfloat	*ipy;		/* initial locator position Y	*/
Pfloat	*ipz;		/* initial locator position Z	*/
Pint	*pet;		/* prompt and echo type	*/
Plimit3	*evol;		/* echo volume 	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record 	*/
{
    Phg_args		cp_args;
    Ppoint3		init_pos;

    init_pos.x = *ipx;
    init_pos.y = *ipy;
    init_pos.z = *ipz;

    fb_init_loc( phg_cur_cph, Pfn_init_loc3, *wkid, *lcdnr, *iviewi, &init_pos,
        *pet, evol, *ldr, datrec, &cp_args);
}

/* INITIALIZE LOCATOR */
void
pinlc_( wkid, lcdnr, iviewi, ipx, ipy, pet, xmin, xmax, ymin, ymax, ldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*lcdnr;		/* locator device number	*/
Pint	*iviewi;	/* initial view indicator */
Pfloat	*ipx;		/* initial locator position X	*/
Pfloat	*ipy;		/* initial locator position Y	*/
Pint	*pet;		/* prompt and echo type	*/
Pfloat	*xmin;		/* echo area X MIN	*/
Pfloat	*xmax;		/* echo area X MAX	*/
Pfloat	*ymin;		/* echo area Y MIN	*/
Pfloat	*ymax;		/* echo area Y MAX	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record 	*/
{
    Phg_args		cp_args;
    Plimit3		evol;
    Ppoint3		init_pos;

    /* Convert to 3D and pass to generic initialization function. */
    evol.x_min = *xmin;
    evol.x_max = *xmax;
    evol.y_min = *ymin;
    evol.y_max = *ymax;
    evol.z_min = 0;
    evol.z_max = 0;
    init_pos.x = *ipx;
    init_pos.y = *ipy;
    init_pos.z = 0;

    fb_init_loc( phg_cur_cph, Pfn_init_loc, *wkid, *lcdnr, *iviewi, &init_pos,
        *pet, &evol, *ldr, datrec, &cp_args);
}

static int
stroke_data_record_ok( pet, rec, ddt)
    Pint                pet;
    Pstroke_data3       *rec;
    Wst_defstroke       *ddt;
{
    int         status = 1;

    if ( rec->buffer_size < 1 || rec->buffer_size > ddt->max_bufsize)
        status = 0;
    else if ( rec->init_pos < 1 || rec->init_pos > rec->buffer_size )
        status = 0;

    return status;
}


/* INITIALIZE STROKE 3 */
void
pinsk3_( wkid, skdnr, iviewi, n, ipx, ipy, ipz, pet, evol, ldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*skdnr;		/* stroke device number	*/
Pint	*iviewi;	/* initial view indicator	*/
Pint	*n;		/* number of popints	*/
Pfloat	*ipx;		/* point in initial stroke X	*/
Pfloat	*ipy;		/* point in initial stroke Y	*/
Pfloat	*ipz;		/* point in initial stroke Z	*/
Pint	*pet;		/* prompt and echo type	*/
Plimit3	*evol;		/* echo volume 	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record	*/
{
    Phg_args				cp_args;
    register Phg_args_inp_init_dev	*args = &cp_args.data.inp_init_dev;
    Wst_input_wsdt			*idt;
    Wst_phigs_dt			*dt;
    Pstroke_data3			rec;
    Ppoint3                 *points;
    Pint		err = 0;
    Pdata_rec    r;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_init_stroke3, &dt,
        REPORT_ERROR)) {
        if ( !DEVICE_EXISTS( idt, stroke, *skdnr) ) {
            ERR_REPORT( phg_cur_cph->erh, ERR250);

        } else if ( !phg_cb_echo_limits_valid( phg_cur_cph, Pfn_init_stroke3,
            *wkid, evol, dt) ) {
            /* Error has been reported, just fall out of the if. */
            ;
        } else if ( *iviewi < 0 || *iviewi >= dt->num_view_indices) {
            ERR_REPORT( phg_cur_cph->erh, ERR114);

        } else {
            Wst_defstroke        *ddt = &idt->strokes[*skdnr - 1];

            if ( !phg_cb_int_in_list( *pet, ddt->num_pets, ddt->pets) ) {
                /* Bad pet, use pet 1 with default data. */
                ERR_REPORT( phg_cur_cph->erh, ERR253);
                rec = ddt->record;
                *pet = 1;

            } else if ( fb_unpack_datrec( *ldr, datrec, &err, &r)) {
                ERR_REPORT( phg_cur_cph->erh, err);

            } else {
                /* change FORTRAN data decord to C data record */
                switch ( *pet) {
                case 1:
                case 2:
                    if ( r.data.il != 2 || r.data.rl != 4) {
                        ERR_REPORT( phg_cur_cph->erh, ERR260);
                        err = ERR260;
                    } else {
                        rec.buffer_size = r.data.ia[0];
                        rec.init_pos = r.data.ia[1];
                        rec.x_interval = r.data.ra[0];
                        rec.y_interval = r.data.ra[1];
                        rec.z_interval = r.data.ra[2];
                        rec.time_interval = r.data.ra[3];
                    }
                    break;
                case 3:
                    if ( r.data.il != 9 || r.data.rl != 5) {
                        ERR_REPORT( phg_cur_cph->erh, ERR260);
                        err = ERR260;
                    } else {
                        rec.buffer_size = r.data.ia[0];
                        rec.init_pos = r.data.ia[1];
                        /* r.data.ia[2] is unused */
                        rec.pets.pet_r3.marker_attrs.type_asf =
                            (Pasf)r.data.ia[3];
                        rec.pets.pet_r3.marker_attrs.size_asf =
                            (Pasf)r.data.ia[4];
                        rec.pets.pet_r3.marker_attrs.colr_ind_asf =
                            (Pasf)r.data.ia[5];
                        rec.pets.pet_r3.marker_attrs.ind = r.data.ia[6];
                        rec.pets.pet_r3.marker_attrs.bundle.type = r.data.ia[7];
                        rec.pets.pet_r3.marker_attrs.bundle.colr_ind =
                            r.data.ia[8];
                        rec.x_interval = r.data.ra[0];
                        rec.y_interval = r.data.ra[1];
                        rec.z_interval = r.data.ra[2];
                        rec.time_interval = r.data.ra[3];
                        rec.pets.pet_r3.marker_attrs.bundle.size = r.data.ra[4];
                    }
                    break;
                case 4:
                    if ( r.data.il != 9 || r.data.rl != 5) {
                        ERR_REPORT( phg_cur_cph->erh, ERR260);
                        err = ERR260;
                    } else {
                        rec.buffer_size = r.data.ia[0];
                        rec.init_pos = r.data.ia[1];
                        rec.pets.pet_r4.line_attrs.type_asf =
                            (Pasf)r.data.ia[3];
                        rec.pets.pet_r4.line_attrs.width_asf =
                            (Pasf)r.data.ia[4];
                        rec.pets.pet_r4.line_attrs.colr_ind_asf =
                            (Pasf)r.data.ia[5];
                        rec.pets.pet_r4.line_attrs.ind = r.data.ia[6];
                        rec.pets.pet_r4.line_attrs.bundle.type = r.data.ia[7];
                        rec.pets.pet_r4.line_attrs.bundle.colr_ind =
                            r.data.ia[8];
                        rec.x_interval = r.data.ra[0];
                        rec.y_interval = r.data.ra[1];
                        rec.z_interval = r.data.ra[2];
                        rec.time_interval = r.data.ra[3];
                        rec.pets.pet_r4.line_attrs.bundle.width = r.data.ra[4];
                    }
                    break;
                default:
                /* Data record is not used */
                    break;
                }
            }

            if ( !err) {
                if ( !stroke_data_record_ok( *pet, &rec, ddt) ) {
                    ERR_REPORT( phg_cur_cph->erh, ERR260);

                } else if ( *n > rec.buffer_size) {
                    ERR_REPORT( phg_cur_cph->erh, ERR262);

                } else {
                    PMALLOC( err, Ppoint3, *n, points);
                    if ( err) {
                        ERR_REPORT( phg_cur_cph->erh, err);

                    } else {
                        POINT3_DATA_TRANSLATE( *n, ipx, ipy, ipz, points);
                        args->wsid = *wkid;
                        args->dev = *skdnr;
                        args->pet = *pet;
                        args->echo_volume = *evol;
                        args->data.stk.init.view_ind = *iviewi;
                        args->data.stk.init.num_points = *n;
                        args->data.stk.init.points = points;
                        args->data.stk.rec = rec;
                        args->class = PHG_ARGS_INP_STK3;
                        CP_FUNC( phg_cur_cph, CP_FUNC_OP_INP_INIT_DEV, &cp_args,
                            NULL);
                        PFREE( *n, points);
                    }
                }
            }
            fb_del_datrec( &r);
        }
    }
}


/* INITIALIZE STROKE */
void
pinsk_( wkid, skdnr, iviewi, n, ipx, ipy, pet, xmin, xmax, ymin, ymax, ldr,
datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*skdnr;		/* stroke device number	*/
Pint	*iviewi;	/* initial view indicator */
Pint	*n;		/* number of points	*/
Pfloat	*ipx;		/* point in initial stroke X	*/
Pfloat	*ipy;		/* point in initial stroke Y	*/
Pint	*pet;		/* prompt and echo type	*/
Pfloat	*xmin;		/* echo area X min	*/
Pfloat	*xmax;		/* echo area X max	*/
Pfloat	*ymin;		/* echo area Y min	*/
Pfloat	*ymax;		/* echo area Y max	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record	*/
{
    Phg_args				cp_args;
    register Phg_args_inp_init_dev	*args = &cp_args.data.inp_init_dev;
    Wst_input_wsdt			*idt;
    Wst_phigs_dt			*dt;
    Psl_ws_info				*wsinfo;
    Plimit3				evol;
    Ppoint3				*points;
    Pstroke_data3			rec;
    register int			i;
    Pint	err = 0;
    Pdata_rec	r;

    evol.x_min = *xmin;
    evol.x_max = *xmax;
    evol.y_min = *ymin;
    evol.y_max = *ymax;
    evol.z_min = 0;
    evol.z_max = 0;

    if ( idt = input_ws_open( phg_cur_cph, *wkid, Pfn_init_stroke, &dt,
        REPORT_ERROR)) {
        if ( !DEVICE_EXISTS( idt, stroke, *skdnr) ) {
            ERR_REPORT( phg_cur_cph->erh, ERR250);

        } else if ( !phg_cb_echo_limits_valid( phg_cur_cph, Pfn_init_stroke,
            *wkid, &evol, dt) ) {
            /* Error has been reported, just fall out of the if. */
            ;
        } else if ( *iviewi < 0 || *iviewi >= dt->num_view_indices) {
            ERR_REPORT( phg_cur_cph->erh, ERR114);

        } else {
            Wst_defstroke        *ddt = &idt->strokes[*skdnr - 1];

            if ( !phg_cb_int_in_list( *pet, ddt->num_pets, ddt->pets) ) {
                /* Bad pet, use pet 1 with default data. */
                ERR_REPORT( phg_cur_cph->erh, ERR253);
                rec = ddt->record;
                *pet = 1;

            } else if ( fb_unpack_datrec( *ldr, datrec, &err, &r)) {
                ERR_REPORT( phg_cur_cph->erh, err);

            } else {
                /* change FORTRAN data decord to C data record */
                switch ( *pet) {
                case 1:
                case 2:
                    if ( r.data.il != 2 || r.data.rl != 3) {
                        ERR_REPORT( phg_cur_cph->erh, ERR260);
                        err = ERR260;
                    } else {
                        rec.buffer_size = r.data.ia[0];
                        rec.init_pos = r.data.ia[1];
                        rec.x_interval = r.data.ra[0];
                        rec.y_interval = r.data.ra[1];
                        rec.z_interval = 0;
                        rec.time_interval = r.data.ra[2];
                    }
                    break;
                case 3:
                    if ( r.data.il != 9 || r.data.rl != 4) {
                        ERR_REPORT( phg_cur_cph->erh, ERR260);
                        err = ERR260;
                    } else {
                        rec.buffer_size = r.data.ia[0];
                        rec.init_pos = r.data.ia[1];
                        /* r.data.ia[2] is unused */
                        rec.pets.pet_r3.marker_attrs.type_asf =
                            (Pasf)r.data.ia[3];
                        rec.pets.pet_r3.marker_attrs.size_asf =
                            (Pasf)r.data.ia[4];
                        rec.pets.pet_r3.marker_attrs.colr_ind_asf =
                            (Pasf)r.data.ia[5];
                        rec.pets.pet_r3.marker_attrs.ind = r.data.ia[6];
                        rec.pets.pet_r3.marker_attrs.bundle.type = r.data.ia[7];
                        rec.pets.pet_r3.marker_attrs.bundle.colr_ind =
                            r.data.ia[8];
                        rec.x_interval = r.data.ra[0];
                        rec.y_interval = r.data.ra[1];
                        rec.z_interval = 0;
                        rec.time_interval = r.data.ra[2];
                        rec.pets.pet_r3.marker_attrs.bundle.size = r.data.ra[3];
                    }
                    break;
                case 4:
                    if ( r.data.il != 9 || r.data.rl != 4) {
                        ERR_REPORT( phg_cur_cph->erh, ERR260);
                        err = ERR260;
                    } else {
                        rec.buffer_size = r.data.ia[0];
                        rec.init_pos = r.data.ia[1];
                        /* r.data.ia[2] is unused */
                        rec.pets.pet_r4.line_attrs.type_asf =
                            (Pasf)r.data.ia[3];
                        rec.pets.pet_r4.line_attrs.width_asf =
                            (Pasf)r.data.ia[4];
                        rec.pets.pet_r4.line_attrs.colr_ind_asf =
                            (Pasf)r.data.ia[5];
                        rec.pets.pet_r4.line_attrs.ind = r.data.ia[6];
                        rec.pets.pet_r4.line_attrs.bundle.type = r.data.ia[7];
                        rec.pets.pet_r4.line_attrs.bundle.colr_ind =
                            r.data.ia[8];
                        rec.x_interval = r.data.ra[0];
                        rec.y_interval = r.data.ra[1];
                        rec.z_interval = 0;
                        rec.time_interval = r.data.ra[2];
                        rec.pets.pet_r4.line_attrs.bundle.width = r.data.ra[3];
                    }
                    break;
                default:
                /* Data record is not used */
                    break;
                }
            }

            if ( !err) {
                if ( !stroke_data_record_ok( *pet, &rec, ddt) ) {
                    ERR_REPORT( phg_cur_cph->erh, ERR260);

                } else if ( *n > rec.buffer_size) {
                    ERR_REPORT( phg_cur_cph->erh, ERR262);

                } else {
                    PMALLOC( err, Ppoint3, *n, points);
                    if ( err) {
                        ERR_REPORT( phg_cur_cph->erh, err);

                    } else {
                        for ( i = 0; i < *n; i++) {
                            points[i].x = ipx[i];
                            points[i].y = ipy[i];
                            points[i].z = 0;
                        }
                        args->wsid = *wkid;
                        args->dev = *skdnr;
                        args->pet = *pet;
                        args->echo_volume = evol;
                        args->data.stk.init.view_ind = *iviewi;
                        args->data.stk.init.num_points = *n;
                        args->data.stk.init.points = points;
                        args->data.stk.rec = rec;
                        args->class = PHG_ARGS_INP_STK;
                        CP_FUNC( phg_cur_cph, CP_FUNC_OP_INP_INIT_DEV, &cp_args,
                            NULL);
                        PFREE( *n, points);
                    }
                }
            }
            fb_del_datrec( &r);
        }
    }
}

static int
val_data_rec_ok( cph, pet, rec, ddt, init)
    Cp_handle                   cph;
    Pint                        pet;
    Pval_data3                  *rec;
    Wst_defval                  *ddt;
    Pfloat                      init;
{
    int                 status = 1;

    switch ( pet) {
        default:
            break;
    }

    if ( !status) {
        ERR_REPORT( cph->erh, ERR260);
        status = 0;

    } else if ( init < rec->low || init > rec->high) {
        ERR_REPORT( cph->erh, ERR261);
        status = 0;
    }
    return status;
}


static void
fb_init_val( cph, func_id, ws, dev, init, pet, ev, ldr, datrec, cp_args)
Cp_handle	cph;
Pint		func_id;
Pint		ws;		/* workstation identifier */
Pint		dev;		/* valuator device number */
Pfloat		init;		/* initial value	*/
Pint		pet;		/* prompt and echo type	*/
Plimit3		*ev;		/* echo volume pointer	*/
Pint		ldr;		/* dimension of data record array	*/
char		*datrec;	/* data record pointer	*/
Phg_args	*cp_args;
{
    register Phg_args_inp_init_dev	*args = &cp_args->data.inp_init_dev;
    Wst_input_wsdt			*idt;
    Wst_phigs_dt			*dt;
    Pval_data3				rec;
    Pint		err = 0;
    Pdata_rec	r;
    char		*w_str;
    int	        size = 0;

    if ( idt = input_ws_open( cph, ws, func_id, &dt, REPORT_ERROR)) {
        if ( !DEVICE_EXISTS( idt, val, dev) ) {
            ERR_REPORT( cph->erh, ERR250);

        } else if ( !phg_cb_echo_limits_valid( cph, func_id, ws, ev, dt) ) {
            /* Error has been reported, just fall out of the if. */
            ;
        } else {
            Wst_defval                *ddt = &idt->valuators[dev - 1];

            if ( !phg_cb_int_in_list( pet, ddt->num_pets, ddt->pets) ) {
                /* Bad pet, use pet 1 with default data. */
                ERR_REPORT( cph->erh, ERR253);
                rec = ddt->record;
                pet = 1;

            } else if ( fb_unpack_datrec( ldr, datrec, &err, &r)) {
                ERR_REPORT( cph->erh, err);

            } else {
                /* change FORTRAN data decord to C data record */
                switch ( pet) {
                case -1:
                    if ( r.data.rl != 2 || r.data.sl != 4) {
                        ERR_REPORT( cph->erh, ERR260);
                        err = ERR260;
                    } else {
                        rec.low = r.data.ra[0];
                        rec.high = r.data.ra[1];
                        size = r.data.lstr[0] + r.data.lstr[1] +
                        r.data.lstr[2] + r.data.lstr[3] + 4;
                        PMALLOC( err, char, size, w_str);
                        if ( err) {
                            ERR_REPORT( cph->erh, err);

                        } else {
                            rec.pets.pet_u1.label = w_str;
                            rec.pets.pet_u1.format =
                                rec.pets.pet_u1.label + r.data.lstr[0] + 1;
                            rec.pets.pet_u1.low_label =
                                rec.pets.pet_u1.format + r.data.lstr[1] + 1;
                            rec.pets.pet_u1.high_label =
                                 rec.pets.pet_u1.low_label + r.data.lstr[2] + 1;
                            bcopy( r.data.str, rec.pets.pet_u1.label,
                                r.data.lstr[0] * sizeof(char));
                            rec.pets.pet_u1.label[r.data.lstr[0]] = '\0';
                            r.data.str += r.data.lstr[0];
                            bcopy( r.data.str, rec.pets.pet_u1.format,
                                r.data.lstr[1] * sizeof(char));
                            rec.pets.pet_u1.format[r.data.lstr[1]] = '\0';
                            r.data.str += r.data.lstr[1];
                            bcopy( r.data.str, rec.pets.pet_u1.low_label,
                                r.data.lstr[2] * sizeof(char));
                            rec.pets.pet_u1.low_label[r.data.lstr[2]] = '\0';
                            r.data.str += r.data.lstr[2];
                            bcopy( r.data.str, rec.pets.pet_u1.high_label,
                                r.data.lstr[3] * sizeof(char));
                            rec.pets.pet_u1.high_label[r.data.lstr[3]] = '\0';
                        }
                    }
                    break;
#ifdef EXT_INPUT
                case -2:
                    if ( r.data.rl != 3) {
                        ERR_REPORT( cph->erh, ERR260);
                        err = ERR260;
                    } else {
                        rec.low = r.data.ra[0];
                        rec.high = r.data.ra[1];
                        rec.pets.pet_d1.dial = r.data.ra[2];
                    }
                    break;
#endif
                default:
                    if ( r.data.rl != 2) {
                        ERR_REPORT( cph->erh, ERR260);
                        err = ERR260;
                    } else {
                        rec.low = r.data.ra[0];
                        rec.high = r.data.ra[1];
                    }
                    break;
                }
            }

            if ( !err) {
                if ( val_data_rec_ok( cph, pet, &rec, ddt, init)) {
                    args->wsid = ws;
                    args->dev = dev;
                    args->pet = pet;
                    args->echo_volume = *ev;
                    args->data.val.init = init;
                    args->data.val.rec = rec;
                    switch ( pet) {
                    case -1:
                        args->data.val.counts[0] = rec.pets.pet_u1.label ?
                            strlen( rec.pets.pet_u1.label) + 1: 0;
                        args->data.val.counts[1] = rec.pets.pet_u1.format ?
                            strlen( rec.pets.pet_u1.format) + 1: 0;
                        args->data.val.counts[2] = rec.pets.pet_u1.low_label ?
                            strlen( rec.pets.pet_u1.low_label) + 1: 0;
                        args->data.val.counts[3] = rec.pets.pet_u1.high_label ?
                            strlen( rec.pets.pet_u1.high_label) + 1: 0;
                        break;
                    }
                    args->class = func_id == Pfn_init_val3 ?
                        PHG_ARGS_INP_VAL3 : PHG_ARGS_INP_VAL ;
                    CP_FUNC( cph, CP_FUNC_OP_INP_INIT_DEV, cp_args, NULL);
                }
            }
            switch ( pet) {
            case -1:
                PFREE( size, w_str);
                break;
            }
            fb_del_datrec( &r);
        }
    }
}


/* INITIALIZE VALUATOR 3 */
void
pinvl3_( wkid, vldnr, ival, pet, evol, ldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*vldnr;		/* valuator device number	*/
Pfloat	*ival;		/* initial value	*/
Pint	*pet;		/* prompt and echo type	*/
Plimit3	*evol;		/* echo volume pointer	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record 	*/
{
    Phg_args				cp_args;

    fb_init_val( phg_cur_cph, Pfn_init_val3, *wkid, *vldnr, *ival, *pet,
	evol, *ldr, datrec, &cp_args);
}

/* INITIALIZE VALUATOR  */
void
pinvl_( wkid, vldnr, ival, pet, xmin, xmax, ymin, ymax, ldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*vldnr;		/* valuator device number	*/
Pfloat	*ival;		/* initial value	*/
Pint	*pet;		/* prompt and echo type	*/
Pfloat	*xmin;		/* echo area X MIN	*/
Pfloat	*xmax;		/* echo area X MAX	*/
Pfloat	*ymin;		/* echo area Y MIN	*/
Pfloat	*ymax;		/* echo area Y MAX	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record	*/
{
    Phg_args		cp_args;
    Plimit3		evol;

    evol.x_min = *xmin;
    evol.x_max = *xmax;
    evol.y_min = *ymin;
    evol.y_max = *ymax;
    evol.z_min = 0;
    evol.z_max = 0;

    fb_init_val( phg_cur_cph, Pfn_init_val, *wkid, *vldnr, *ival, *pet,
	&evol, *ldr, datrec, &cp_args);
}

static int
choice_data_rec_ok( cph, pet, rec, dt, ddt, istat, init, args)
    Cp_handle                   cph;
    Pint                        pet;
    Pchoice_data3               *rec;
    Wst_phigs_dt                *dt;
    Wst_defchoice               *ddt;
    Pint			istat;
    Pint                        init;
    Phg_args_inp_init_dev       *args;
{
    register int        i, size, cnt;
    register char       **strs, *strlist, *str;
    int                 list_count, status = 0;
    int         chk_flag = 0;

    switch ( pet) {
        case 1:
            /* No user-specified data. */
            if ( istat == POK && (init < 1 || init > ddt->choices) ) {
                ERR_REPORT( cph->erh, ERR261);
            } else
                status = !0;
            break;

        case 2:
            /* Count and list of ON/OFF values. */
            args->data.cho.rec = *rec;
            list_count = rec->pets.pet_r2.num_prompts;
            if ( istat == POK && (init < 1 || init > ddt->choices) ) {
                ERR_REPORT( cph->erh, ERR261);

            } else if ( list_count < 0 || list_count > ddt->choices) {
                ERR_REPORT( cph->erh, ERR260);

            } else {
                for ( i = 0; i < list_count; i++ ) {
                   if ( rec->pets.pet_r2.prompts[i] < 0 ||
                        rec->pets.pet_r2.prompts[i] > 1) {
                       chk_flag = 1;
                       ERR_REPORT( cph->erh, ERR260);
                       break;
                    }
                }
                if ( chk_flag == 0 )
                    status = !0;
                }
            break;

        case 3:
            /* Count and list of choice strings. */
            /* Package the choice strings into the internal format: a
             * single long array of null terminated strings and a count of the
             * total length of this array.
             */
            args->data.cho.rec = *rec;
            args->data.cho.string_list_size = 0;
            list_count = rec->pets.pet_r3.num_strings;
            if ( list_count > ddt->choices || list_count < 1) {
                /* too many or too few choice strings */
                ERR_REPORT( cph->erh, ERR260);

            /* init <= list_count ==> init <= ddt->choices, by above test. */
            } else if (istat == POK && (init > list_count || init < 1)) {
                ERR_REPORT( cph->erh, ERR261);

            } else {
                strs = rec->pets.pet_r3.strings;
                /* Compute total size of all strings and get space. */
                for ( size = 0, cnt = list_count; cnt > 0; cnt--, strs++ )
                    size += strlen(*strs) + 1; /* 1 for the terminator */
                    args->data.cho.string_list_size = size;
                    args->data.cho.rec.pets.pet_r3.strings =
                        (char **)(*rec->pets.pet_r3.strings);
                    status = !0;
            }
            break;

        case 5:
            /* Structure id, + count and list of pickids. */
            args->data.cho.rec = *rec;
            list_count = rec->pets.pet_r5.num_pick_ids;
            if ( istat == POK && (init < 1 || init > ddt->choices) ) {
                ERR_REPORT( cph->erh, ERR261);

            } else if ( list_count < 0 || list_count > ddt->choices ) {
                ERR_REPORT( cph->erh, ERR261);

            } else
                status = !0;
            break;
    }
    return status;
}


static void
fb_init_choice( cph, func_id, ws, dev, istat, init, pet, ev, ldr, datrec,
cp_args)
Cp_handle	cph;
Pint		func_id;
Pint		ws;		/* workstation identifier */
Pint		dev;		/* choice device number	*/
Pint		istat;		/* initial choice status */
Pint		init;		/* initial choice	*/
Pint		pet;		/* prompt and echo type	*/
Plimit3		*ev;		/* echo volume pointer	*/
Pint		ldr;		/* dimension of data record array	*/
char		*datrec;	/* data record pointer	*/
Phg_args	*cp_args;
{
    register Phg_args_inp_init_dev	*args = &cp_args->data.inp_init_dev;
    Wst_input_wsdt			*idt;
    Wst_phigs_dt			*dt;
    Pchoice_data3			rec;
    char				    *strings;
    char				    **ptr;
    register int			i;
    Pint        err = 0;
    Pdata_rec   r;
    Pint	total = 0;
    Pint	size;

    if ( idt = input_ws_open( cph, ws, func_id, &dt, REPORT_ERROR)) {
        if ( !DEVICE_EXISTS( idt, choice, dev) ) {
            ERR_REPORT( cph->erh, ERR250);

        } else if ( !phg_cb_echo_limits_valid( cph, func_id, ws, ev, dt) ) {
            /* Error has been reported, just fall out of the if. */
            ;
        } else if ( istat < POK || istat > PNCHOI) {
            ERR_REPORT( cph->erh, ERR2000);

        } else {
            Wst_defchoice        *ddt = &idt->choices[dev - 1];

            if ( !phg_cb_int_in_list( pet, ddt->num_pets, ddt->pets) ) {
                /* Bad pet, use pet 1 with default data. */
                ERR_REPORT( cph->erh, ERR253);
                rec = ddt->record;
                pet = 1;

            } else if ( fb_unpack_datrec( ldr, datrec, &err, &r)) {
                ERR_REPORT( cph->erh, err);

            } else {
                /* change FORTRAN data decord to C data record */
                switch ( pet) {
                case 2:
                    if ( r.data.il < 0) {
                        ERR_REPORT( cph->erh, ERR260);
                        err = ERR260;
                    } else {
                        rec.pets.pet_r2.num_prompts = r.data.il;
                        PMALLOC( err, Ppr_switch, r.data.il,
                            rec.pets.pet_r2.prompts);
                        if ( err) {
                            ERR_REPORT( cph->erh, err);
                        } else {
                            bcopy((char *)r.data.ia,
                                (char *)rec.pets.pet_r2.prompts,
                                r.data.il * sizeof(Pint));
                        }
                    }
                    break;
                case 3:
                    if ( r.data.sl < 0) {
                        ERR_REPORT( cph->erh, ERR260);
                        err = ERR260;
                    } else {
                        rec.pets.pet_r3.num_strings = r.data.sl;
                        for ( i = 0; i < r.data.sl; i++) {
                            total = r.data.lstr[i] + 1;
                        }
                        size = (sizeof(char *) * r.data.sl) +
                               (sizeof(char) * total);
                        if ( size > 0 && !(ptr = (char **)malloc(size))) {
                            ERR_REPORT( cph->erh, ERR900);
                        } else if ( size > 0) {
                            rec.pets.pet_r3.strings = (char **)ptr;
                            strings = (char *)(rec.pets.pet_r3.strings
                                    + r.data.sl);
                            for ( i = 0; i < r.data.sl; i++) {
                                rec.pets.pet_r3.strings[i] = strings;
                                bcopy((char *)r.data.str, (char *)strings,
                                    r.data.lstr[i] * sizeof(char));
                                strings[r.data.lstr[i]] = '\0';
                                r.data.str += r.data.lstr[i];
                                strings += r.data.lstr[i] + 1;
                            }
                        }
                    }
                    break;
                case 4:
                    if ( r.data.sl < 0) {
                        ERR_REPORT( cph->erh, ERR260);
                        err = ERR260;
                    } else {
                        rec.pets.pet_r4.num_strings = r.data.sl;
                        for ( i = 0; i < r.data.sl; i++) {
                            total = r.data.lstr[i] + 1;
                        }
                        size = (sizeof(char *) * r.data.sl) +
                               (sizeof(char) * total);
                        if ( size >  0 && !(ptr = (char **)malloc(size))) {
                            ERR_REPORT( cph->erh, ERR900);
                        } else if ( size > 0) {
                            rec.pets.pet_r4.strings = (char **)ptr;
                            strings = (char *)(rec.pets.pet_r4.strings
                                    + r.data.sl);
                            for ( i = 0; i < r.data.sl; i++) {
                                rec.pets.pet_r4.strings[i] = strings;
                                bcopy((char *)r.data.str, (char *)strings,
                                    r.data.lstr[i] * sizeof(char));
                                strings[r.data.lstr[i]] = '\0';
                                r.data.str += r.data.lstr[i];
                                strings += r.data.lstr[i] + 1;
                            }
                        }
                    }
                    break;
                case 5:
                    if ( r.data.il < 1) {
                        ERR_REPORT( cph->erh, ERR260);
                        err = ERR260;
                    } else {
                        rec.pets.pet_r5.struct_id = r.data.ia[0];
                        rec.pets.pet_r5.num_pick_ids = r.data.il - 1;
                        PMALLOC( err, Pint, (r.data.il - 1),
                            rec.pets.pet_r5.pick_ids);
                        if ( err) {
                            ERR_REPORT( cph->erh, err);
                        } else {
                            bcopy((char *)(r.data.ia + 1),
                                (char *)rec.pets.pet_r5.pick_ids,
                                (r.data.il - 1) * sizeof(Pint));
                        }
                    }
                    break;
                default:
                /* Data record is not used */
                    break;
                }
            }

            if ( !err) {
                if ( choice_data_rec_ok(cph, pet, &rec, dt, ddt, istat, init,
                    args)){
                    args->wsid = ws;
                    args->dev = dev;
                    args->pet = pet;
                    args->echo_volume = *ev;
                    IN_ST_CONVERT_F_TO_C( istat, args->data.cho.status);
                    args->data.cho.init = init;
                    args->class = func_id == Pfn_init_choice3 ?
                        PHG_ARGS_INP_CHC3 : PHG_ARGS_INP_CHC ;
                    CP_FUNC( cph, CP_FUNC_OP_INP_INIT_DEV, cp_args, NULL);

                    /* Free any data allocated when the data record was
                       reformatted (in choice_data_rec_ok()).
                    */
                }
            }
            switch ( pet) {
            case 2:
                PFREE( r.data.il, rec.pets.pet_r2.prompts);
                break;
            case 3:
                PFREE( size, ptr);
                break;
            case 4:
                PFREE( size, ptr);
                break;
            case 5:
                PFREE( (r.data.il - 1), rec.pets.pet_r5.pick_ids);
                break;
            }
            fb_del_datrec( &r);
        }
    }
}


/* INITIALIZE CHOICE 3 */
void
pinch3_( wkid, chdnr, istat, ichnr, pet, evol, ldr, datrec)
Pint	*wkid;		/* workstation identifier */
Pint	*chdnr;		/* choice device number	*/
Pint	*istat;		/* initial choice status */
Pint	*ichnr;		/* initial choice	*/
Pint	*pet;		/* prompt and echo type	*/
Plimit3	*evol;		/* echo volume	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record 	*/
{
    Phg_args				cp_args;

    fb_init_choice( phg_cur_cph, Pfn_init_choice3, *wkid, *chdnr, *istat,
        *ichnr, *pet, evol, *ldr, datrec, &cp_args);
}

/* INITIALIZE CHOICE */
void 
pinch_( wkid, chdnr, istat, ichnr, pet, xmin, xmax, ymin, ymax, ldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*chdnr;		/* choice device number	*/
Pint	*istat;		/* initial choice status	*/
Pint	*ichnr;		/* initial choice	*/
Pint	*pet;		/* prompt and echo type	*/
Pfloat	*xmin;		/* echo area X min	*/
Pfloat	*xmax;		/* echo area X max	*/
Pfloat	*ymin;		/* echo area Y min	*/
Pfloat	*ymax;		/* echo area Y max	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record	*/
{
    Phg_args		cp_args;
    Plimit3		evol;

    evol.x_min = *xmin;
    evol.x_max = *xmax;
    evol.y_min = *ymin;
    evol.y_max = *ymax;
    evol.z_min = 0;
    evol.z_max = 0;

    fb_init_choice( phg_cur_cph, Pfn_init_choice, *wkid, *chdnr, *istat, *ichnr,
	*pet, &evol, *ldr, datrec, &cp_args);
}

static int
pick_data_rec_ok( cph, pet, rec, dt, ddt, func_id)
    Cp_handle                   cph;
    Pint                        pet;
    Ppick_data3                 *rec;
    Wst_phigs_dt                *dt;
    Wst_defpick                 *ddt;
    Pint                        func_id;
{
    Pint        status = 1;
    Pfloat      dc_lim;

    switch ( pet) {
#ifdef EXT_INPUT
    case -1:
        dc_lim = MIN( dt->dev_coords[0], dt->dev_coords[1]);
        dc_lim = dc_lim / 2;
        if ( rec->pets.pet_u1.ap_size < 0.0 ||
             rec->pets.pet_u1.ap_size > dc_lim) {
            ERR_REPORT( cph->erh, ERR260);
            status = 0;
        }
        break;
#endif
    default:
        break;
    }

    return status;
}


static void
fb_init_pick( cph, func_id, ws, dev, istat, ippd, pp, pet, ev, ldr, datrec,
order, cp_args)
Cp_handle	cph;
Pint		func_id;
Pint		ws;		/* workstation identifier */
Pint		dev;		/* choice device number	*/
Pint		istat;		/* initial pick status	*/
Pint		ippd;		/* depth of initial pick path	*/
Pint		*pp;		/* initial pick path	*/
Pint		pet;		/* prompt and echo type	*/
Plimit3		*ev;		/* echo volume pointer	*/
Pint		ldr;		/* dimension of data record prray */
char		*datrec;	/* data record 	*/
Pint		order;		/* pick path order	*/
Phg_args	*cp_args;
{
    register Phg_args_inp_init_dev	*args = &cp_args->data.inp_init_dev;
    Wst_input_wsdt			*idt;
    Wst_phigs_dt			*dt;
    Wst_defpick				*ddt;
    Ppick_data3				rec;
    Pint				err = 0;
    Pdata_rec			r;
    register int		i;

    if ( idt = outin_ws_open( cph, ws, func_id, &dt, REPORT_ERROR)) {
        if ( !DEVICE_EXISTS( idt, pick, dev) ) {
            ERR_REPORT( cph->erh, ERR250);

        } else if ( !phg_cb_echo_limits_valid( cph, func_id, ws, ev, dt) ) {
            /* Error has been reported, just fall out of the if. */
            ;

        } else if (( istat < POK || istat > PNPICK) ||
            ( order < PPOTOP || order > PPOBOT)) {
            ERR_REPORT( cph->erh, ERR2000);

        } else if ( ippd < 0) {
            ERR_REPORT( cph->erh, ERR2004);

        } else {
            ddt = &idt->picks[dev-1];
            if ( !phg_cb_int_in_list( pet, ddt->num_pets, ddt->pets) ) {
                /* Bad pet, use pet 1 with default data. */
                ERR_REPORT( cph->erh, ERR253);
                rec = ddt->record;
                pet = 1;

            } else if ( fb_unpack_datrec( ldr, datrec, &err, &r)) {
                ERR_REPORT( cph->erh, err);
                
            } else {
                switch( pet) {
#ifdef EXT_INPUT
                case -1:
                    if ( r.data.rl != 1) {
                        ERR_REPORT( cph->erh, ERR260);
                        err = ERR260;
                    } else {
                        rec.pets.pet_u1.ap_size = r.data.ra[0];
                    }
                    break;
#endif
                default:
                /* Data record is not used */
                    break;
                }
                if ( !err) { 
                    if ( pick_data_rec_ok( cph, pet, &rec, dt, ddt, func_id) ) {
                        args->wsid = ws;
                        args->dev = dev;
                        args->pet = pet;
                        args->echo_volume = *ev;
                        args->class = func_id == Pfn_init_pick3 ?
                            PHG_ARGS_INP_PIK3 : PHG_ARGS_INP_PIK ;
                        IN_ST_CONVERT_F_TO_C(istat, args->data.pik.init.status);
                        if ( istat == POK) {
                            args->data.pik.init.pick_path.depth = ippd;
                            args->data.pik.init.pick_path.path_list
                                = (Ppick_path_elem *)pp;
                        } else {
                            args->data.pik.init.pick_path.depth = 0;
                        }
                        args->data.pik.rec = rec;
                        args->data.pik.porder = (Ppath_order)order;
                        CP_FUNC( cph, CP_FUNC_OP_INP_INIT_DEV, cp_args, NULL);
                    }
                }
                fb_del_datrec( &r);
            }
        }
    }
}


/* INITIALIZE PICK 3 */
void
pinpk3_( wkid, pkdnr, istat, ippd, pp, pet, evol, ldr, datrec, ppordr)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pkdnr;		/* pick device number	*/
Pint	*istat;		/* initial pick status	*/
Pint	*ippd;		/* depth of initial pick path	*/
Pint	*pp;		/* initial pick path	*/
Pint	*pet;		/* prompt and echo type	*/
Plimit3	*evol;		/* echo volume	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record pointer	*/
Pint	*ppordr;	/* pick path order	*/
{
    Phg_args				cp_args;

    fb_init_pick( phg_cur_cph, Pfn_init_pick3, *wkid, *pkdnr, *istat, *ippd, pp,
        *pet, evol, *ldr, datrec, *ppordr, &cp_args);
}

/* INITIALIZE PICK */
void
pinpk_( wkid, pkdnr, istat, ippd, pp, pet, xmin, xmax, ymin, ymax, ldr, datrec,
ppordr)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pkdnr;		/* pick device number	*/
Pint	*istat;		/* initial pick status	*/
Pint	*ippd;		/* depth of initial pick path	*/
Pint	*pp;		/* initial pick path	*/
Pint	*pet;		/* prompt and echo type	*/
Pfloat	*xmin;		/* echo area X min	*/
Pfloat	*xmax;		/* echo area X max	*/
Pfloat	*ymin;		/* echo area Y min	*/
Pfloat	*ymax;		/* echo area Y max	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record	*/
Pint	*ppordr;	/* pick path order	*/
{
    Phg_args		cp_args;
    Plimit3		evol;

    evol.x_min = *xmin;
    evol.x_max = *xmax;
    evol.y_min = *ymin;
    evol.y_max = *ymax;
    evol.z_min = 0;
    evol.z_max = 0;

    fb_init_pick( phg_cur_cph, Pfn_init_pick, *wkid, *pkdnr, *istat, *ippd, pp,
       *pet, &evol, *ldr, datrec, *ppordr, &cp_args);
}

static int
string_data_ok( cph, pet, rec, init_length, ddt)
    Cp_handle                   cph;
    Pint                        pet;
    Pstring_data3               *rec;
    int                         init_length;
    Wst_defstring               *ddt;
{
    int         status = 1;

    switch ( pet) {
        default:
            break;
    }

    if ( !status) {
        ERR_REPORT( cph->erh, ERR260);

    } else {
        if ( rec->buffer_size > ddt->max_bufsize)
            rec->buffer_size = ddt->max_bufsize;

        if ( rec->buffer_size < 1 ) {
			ERR_REPORT( cph->erh, ERR260);
            status = 0;

        } else if ( rec->init_pos > rec->buffer_size) {
            ERR_REPORT( cph->erh, ERR260);
            status = 0;

        } else if ( rec->init_pos < 1) {
            ERR_REPORT( cph->erh, ERR260);
            status = 0;

        /* buffer size does not include the string terminator. */
        } else if ( init_length > rec->buffer_size) {
            ERR_REPORT( cph->erh, ERR263);
            status = 0;

        } else if ( rec->init_pos > (init_length + 1) ) {
			ERR_REPORT( cph->erh, ERR260);
            status = 0;

        }
    }
    return status;
}


static void
fb_init_string( cph, func_id, ws, dev, listr, istr, pet, ev, ldr, datrec,
length, cp_args)
Cp_handle	cph;
Pint		func_id;
Pint		ws;		/* workstation identifier */
Pint		dev;		/* choice device number	*/
Pint		listr;		/* length of the initial string */
char		*istr;		/* initial string */
Pint		pet;		/* prompt and echo type	*/
Plimit3		*ev;		/* echo volume pointer	*/
Pint		ldr;		/* dimension of data record array */
char		*datrec;	/* data record pointer	*/
int		length;		/* string length	*/
Phg_args	*cp_args;
{
    register Phg_args_inp_init_dev	*args = &cp_args->data.inp_init_dev;
    Wst_input_wsdt			*idt;
    Wst_phigs_dt			*dt;
    Pstring_data3			rec;
    Pint        err = 0;
    Pdata_rec   r;

    if ( idt = input_ws_open( cph, ws, func_id, &dt, REPORT_ERROR)) {
        if ( !DEVICE_EXISTS( idt, string, dev) ) {
            ERR_REPORT( cph->erh, ERR250);

        } else if ( !phg_cb_echo_limits_valid( cph, func_id, ws, ev, dt) ) {
            /* Error has been reported, just fall out of the if. */
            ;
        } else if ( listr < 0 || listr > length) {
            ERR_REPORT( cph->erh, ERR2004);
        
        } else {
            Wst_defstring        *ddt = &idt->strings[dev - 1];

            if ( !phg_cb_int_in_list( pet, ddt->num_pets, ddt->pets) ) {
                /* Bad pet, use pet 1 with default data. */
                ERR_REPORT( cph->erh, ERR253);
                rec = ddt->record;
                pet = 1;

            } else if ( fb_unpack_datrec( ldr, datrec, &err, &r)) {
                ERR_REPORT( cph->erh, err);

            } else {
                /* change FORTRAN data decord to C data record */
                if ( r.data.il != 2) {
                    ERR_REPORT( cph->erh, ERR260);
                    err = ERR260;
                } else {
                    rec.buffer_size = r.data.ia[0];
                    rec.init_pos = r.data.ia[1];
                }
            }

            if ( !err) {
                if ( string_data_ok( cph, pet, &rec, listr, ddt)) {
                    args->data.str.init.string = istr;
                    args->data.str.init.string[listr] = '\0';
                    args->data.str.init.length = listr ? listr + 1 : 0;
                    args->wsid = ws;
                    args->dev = dev;
                    args->pet = pet;
                    args->echo_volume = *ev;
                    args->class = func_id == Pfn_init_string3 ?
                        PHG_ARGS_INP_STR3 : PHG_ARGS_INP_STR ;
                    args->data.str.rec = rec;
                    CP_FUNC( cph, CP_FUNC_OP_INP_INIT_DEV, cp_args, NULL);
                }
            }
            fb_del_datrec( &r);
        }
    }
}


/* INITIALIZED STRING 3 */
void
finst3_( wkid, stdnr, lstr, istr, pet, evol, ldr, datrec, length)
Pint	*wkid;		/* workstation identifier	*/
Pint	*stdnr;		/* string device number	*/
Pint	*lstr;		/* length of the initial string	*/
char	*istr;		/* initial string	*/
Pint	*pet;		/* prompt and echo type	*/
Plimit3	*evol;		/* echo volume 	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record 	*/
int	*length;	/* string length	*/
{
    Phg_args				cp_args;

    fb_init_string( phg_cur_cph, Pfn_init_string3, *wkid, *stdnr, *lstr, istr,
        *pet, evol, *ldr, datrec, *length, &cp_args);
}

/* INITIALIZED STRING */
void
finst_( wkid, stdnr, lstr, istr, pet, xmin, xmax, ymin, ymax, ldr, datrec,
length)
Pint	*wkid;		/* workstation identifier	*/
Pint	*stdnr;		/* string device number	*/
Pint	*lstr;		/* length of the initial string	*/
char	*istr;		/* initial string	*/
Pint	*pet;		/* prompt and echo type	*/
Pfloat	*xmin;		/* echo area X min	*/
Pfloat	*xmax;		/* echo area X max	*/
Pfloat	*ymin;		/* echo area Y min	*/
Pfloat	*ymax;		/* echo area Y max	*/
Pint	*ldr;		/* dimension of data record array	*/
char	*datrec;	/* data record	*/
int	*length;	/* string length	*/
{
    Phg_args		cp_args;
    Plimit3		evol;

    evol.x_min = *xmin;
    evol.x_max = *xmax;
    evol.y_min = *ymin;
    evol.y_max = *ymax;
    evol.z_min = 0;
    evol.z_max = 0;

    fb_init_string( phg_cur_cph, Pfn_init_string, *wkid, *stdnr, *lstr, istr,
        *pet, &evol, *ldr, datrec, *length, &cp_args);
}


/* SET PICK FILTER */
void
pspkft_( wkid, pkdnr, isn, is, esn, es)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pkdnr;		/* pick device number	*/
Pint	*isn;		/* number of names in the inclusion set */ 
Pint	*is;		/* inclusion set */ 
Pint	*esn;		/* number of names in the exclusion set */ 
Pint	*es;		/* exclusion set */ 
{
    Phg_args				cp_args;
    Wst_input_wsdt			*idt;
    register Phg_args_set_filter	*args = &cp_args.data.set_filter;

    if ( idt = outin_ws_open( phg_cur_cph, *wkid, Pfn_set_pick_filter,
        NO_DT_NEEDED, REPORT_ERROR)) {
	if ( !DEVICE_EXISTS( idt, pick, *pkdnr) ) {
	    ERR_REPORT( phg_cur_cph->erh, ERR250);
        
        } else if ( *isn < 0 || *esn < 0) {
            ERR_REPORT( phg_cur_cph->erh, ERR2004);
	
	} else {
	    args->wsid = *wkid;
	    args->devid = *pkdnr;
	    args->type = PHG_ARGS_FLT_PICK;
	    args->inc_set.num_ints = *isn;
	    args->inc_set.ints = is;
	    args->exc_set.num_ints = *esn;
	    args->exc_set.ints = es;
	    CP_FUNC( phg_cur_cph, CP_FUNC_OP_SET_FILTER, &cp_args, NULL);
	}
    }
}

#define INQ_STATE( _cph, _cp_args, _ws, _dev, _class, _ret) \
  { \
    (_cp_args).data.q_inp_state.wsid = (_ws); \
    (_cp_args).data.q_inp_state.dev = (_dev); \
    (_cp_args).data.q_inp_state.class = (_class); \
    CP_FUNC((_cph),CP_FUNC_OP_INQ_INP_DEV_STATE,&(_cp_args),(_ret)); \
  }

static void
fb_inq_loc_state( ws, dev, type, err, ret)
Pint	ws;		/* workstation identifier	*/
Pint	dev;		/* locator device number	*/
Pint	type;		/* type of returned value	*/
Pint	*err;		/* OUT error indicator	*/
Phg_ret	*ret;
{
    Phg_args			cp_args;
    Wst_input_wsdt		*idt;

    if (idt = input_ws_open( phg_cur_cph, ws, Pfn_INQUIRY, NO_DT_NEEDED, err)) {
	if ( !DEVICE_EXISTS( idt, loc, dev) ) {
	    *err = ERR250;

        } else if ( type < PSET || type > PREALI) {
            *err = ERR2000;

	} else {
	    cp_args.data.q_inp_state.inq_type = (Pinq_type)type;
	    INQ_STATE( phg_cur_cph, cp_args, ws, dev, PHG_ARGS_INP_LOC3, ret)
	    if ( ret->err)
		*err = ret->err;
	    else
		*err = 0;
	}
    }
}


/* INQUIRE LOCATOR DEVICE STATE 3 */
void
pqlcs3_( wkid, lcdnr, type, mldr, errind, mode, esw, iviewi, ipx, ipy, ipz,
pet, evol, ldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*lcdnr;		/* locator device number	*/
Pint	*type;		/* type of returned value	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mode;		/* OUT operating mode	*/
Pint	*esw;		/* OUT echo switch	*/
Pint	*iviewi;	/* OUT initial view indicator	*/
Pfloat	*ipx;		/* OUT initial locator position X	*/
Pfloat	*ipy;		/* OUT initial locator position Y	*/
Pfloat	*ipz;		/* OUT initial locator position Z	*/
Pint	*pet;		/* OUT prompt/echo type	*/
Plimit3	*evol;		/* OUT echo volume	*/
Pint	*ldr;		/* OUT number of array elements data record */
char	*datrec;	/* OUT data record	*/
{
    		Phg_ret		ret;
    register	Plocst3		*state = &ret.data.inp_state.loc;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries		*/

    fb_inq_loc_state( *wkid, *lcdnr, *type, errind, &ret);
    if ( !*errind) {
	*mode = (Pint)state->mode;
	*esw = (Pint)state->esw;
	*iviewi = state->loc.view_ind;
	*ipx = state->loc.position.x;
	*ipy = state->loc.position.y;
	*ipz = state->loc.position.z;
	*pet = state->pet;
	*evol = state->e_volume;
	switch ( *pet) {
	case 4:
            il = 7;
            rl = 1;
	    PMALLOC( *errind, Pint, il, ia);
	    PMALLOC( *errind, Pfloat, rl, ra);
            if ( !*errind) {
                ia[0] = 0; /* ia[0] is unused */
                ia[1] = (Pint)state->record.pets.pet_r4.line_attrs.type_asf;
                ia[2] = (Pint)state->record.pets.pet_r4.line_attrs.width_asf;
                ia[3] = (Pint)state->record.pets.pet_r4.line_attrs.colr_ind_asf;
                ia[4] = state->record.pets.pet_r4.line_attrs.ind;
	        ia[5] = state->record.pets.pet_r4.line_attrs.bundle.type;
                ia[6] = state->record.pets.pet_r4.line_attrs.bundle.colr_ind;
                ra[0] = state->record.pets.pet_r4.line_attrs.bundle.width;
            }
            break;
        case 5:
            switch ( state->record.pets.pet_r5.line_fill_ctrl_flag) {
            case PPLINE:
                il = 8;
                rl = 1;
                PMALLOC( *errind, Pint, il, ia);
                PMALLOC( *errind, Pfloat, rl, ra);
                if ( !*errind) {
                    ia[0] = (Pint)state->record.pets.pet_r5.line_fill_ctrl_flag;
                    ia[1] = 0; /* ia[1] is unused */
                    ia[2] = (Pint)state->record.pets.pet_r5.attrs.line_attrs.type_asf;
                    ia[3] = (Pint)state->record.pets.pet_r5.attrs.line_attrs.width_asf;
                    ia[4] = (Pint)state->record.pets.pet_r5.attrs.line_attrs.colr_ind_asf;
                    ia[5] = state->record.pets.pet_r5.attrs.line_attrs.ind;
                    ia[6] = state->record.pets.pet_r5.attrs.line_attrs.bundle.type;
                    ia[7] = state->record.pets.pet_r5.attrs.line_attrs.bundle.colr_ind;
                    ra[0] = state->record.pets.pet_r5.attrs.line_attrs.bundle.width;
                }
                break;
            case PFILLA:
                il = 9;
                PMALLOC( *errind, Pint, il, ia);
                if ( !*errind) {
                    ia[0] = (Pint)state->record.pets.pet_r5.line_fill_ctrl_flag;
                    ia[1] = 0; /* ia[1] is unused */
                    ia[2] = (Pint)state->record.pets.pet_r5.attrs.int_attrs.style_asf;
                    ia[3] = (Pint)state->record.pets.pet_r5.attrs.int_attrs.style_ind_asf;
                    ia[4] = (Pint)state->record.pets.pet_r5.attrs.int_attrs.colr_ind_asf;
                    ia[5] = state->record.pets.pet_r5.attrs.int_attrs.ind;
                    ia[6] = (Pint)state->record.pets.pet_r5.attrs.int_attrs.bundle.style;
                    ia[7] = state->record.pets.pet_r5.attrs.int_attrs.bundle.style_ind;
                    ia[8] = state->record.pets.pet_r5.attrs.int_attrs.bundle.colr_ind;
                }
                break;
            case PFILAS:
                il = 17;
                rl = 1;
                PMALLOC( *errind, Pint, il, ia);
                PMALLOC( *errind, Pfloat, rl, ra);
                if ( !*errind) {
                    ia[0] = (Pint)state->record.pets.pet_r5.line_fill_ctrl_flag;
                    ia[1] = 0; /* ia[1] is unused */
                    ia[2] = (Pint)state->record.pets.pet_r5.attrs.fill_set.int_attrs.style_asf;
                    ia[3] = (Pint)state->record.pets.pet_r5.attrs.fill_set.int_attrs.style_ind_asf;
                    ia[4] = (Pint)state->record.pets.pet_r5.attrs.fill_set.int_attrs.colr_ind_asf;
                    ia[5] = state->record.pets.pet_r5.attrs.fill_set.int_attrs.ind;
                    ia[6] = (Pint)state->record.pets.pet_r5.attrs.fill_set.int_attrs.bundle.style;
                    ia[7] = state->record.pets.pet_r5.attrs.fill_set.int_attrs.bundle.style_ind;
                    ia[8] = state->record.pets.pet_r5.attrs.fill_set.int_attrs.bundle.colr_ind;
                    
                    ia[9] = (Pint)state->record.pets.pet_r5.attrs.fill_set.edge_attrs.flag_asf;
                    ia[10] = (Pint)state->record.pets.pet_r5.attrs.fill_set.edge_attrs.type_asf;
                    ia[11] = (Pint)state->record.pets.pet_r5.attrs.fill_set.edge_attrs.width_asf;
                    ia[12] = (Pint)state->record.pets.pet_r5.attrs.fill_set.edge_attrs.colr_ind_asf;
                    ia[13] = state->record.pets.pet_r5.attrs.fill_set.edge_attrs.ind;
                    ia[14] = (Pint)state->record.pets.pet_r5.attrs.fill_set.edge_attrs.bundle.flag;
                    ia[15] = state->record.pets.pet_r5.attrs.fill_set.edge_attrs.bundle.type;
                    ia[16] = state->record.pets.pet_r5.attrs.fill_set.edge_attrs.bundle.colr_ind;
                    ra[0] = state->record.pets.pet_r5.attrs.fill_set.edge_attrs.bundle.width;
                }
                break;
            }
            break;
        default:
            break;
        }
        if ( !*errind) {
            fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr,
                errind, ldr, datrec);
        }
        PFREE( il, ia);
        PFREE( rl, ra);
    }
}


/* INQUIRE LOCATOR DEVICE STATE */
void
pqlcs_( wkid, lcdnr, type, mldr, errind, mode, esw, iviewi, ipx, ipy, pet,
earea, ldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*lcdnr;		/* locator device number	*/
Pint	*type;		/* type of returned value	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mode;		/* OUT operating mode	*/
Pint	*esw;		/* OUT echo switch	*/
Pint	*iviewi;	/* OUT initial view indicator	*/
Pfloat	*ipx;		/* OUT initial locator position X	*/
Pfloat	*ipy;		/* OUT initial locator position Y	*/
Pint	*pet;		/* OUT prompt/echo type	*/
Plimit	*earea;		/* OUT echo area	*/
Pint	*ldr;		/* OUT number of array elements data record */
char	*datrec;	/* OUT data record	*/
{
    		Phg_ret		ret;
    register	Plocst3		*state = &ret.data.inp_state.loc;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries		*/

    fb_inq_loc_state( *wkid, *lcdnr, *type, errind, &ret);
    if ( !*errind) {
	CB_ECHO_VOLUME_TO_AREA( state->e_volume, *earea);
	*mode = (Pint)state->mode;
	*esw = (Pint)state->esw;
	*pet = state->pet;
	*iviewi = state->loc.view_ind;
	*ipx = state->loc.position.x;
	*ipy = state->loc.position.y;
	switch ( *pet) {
	case 4:
            il = 7;
            rl = 1;
	    PMALLOC( *errind, Pint, il, ia);
	    PMALLOC( *errind, Pfloat, rl, ra);
            if ( !*errind) {
                ia[0] = 0; /* ia[0] is unused */
	        ia[1] = (Pint)state->record.pets.pet_r4.line_attrs.type_asf;
	        ia[2] = (Pint)state->record.pets.pet_r4.line_attrs.width_asf;
		ia[3] = (Pint)state->record.pets.pet_r4.line_attrs.colr_ind_asf;
		ia[4] = state->record.pets.pet_r4.line_attrs.ind;
		ia[5] = state->record.pets.pet_r4.line_attrs.bundle.type;
		ia[6] = state->record.pets.pet_r4.line_attrs.bundle.colr_ind;
		ra[0] = state->record.pets.pet_r4.line_attrs.bundle.width;
            }
            break;
        case 5:
            switch ( state->record.pets.pet_r5.line_fill_ctrl_flag) {
	    case PPLINE:
                il = 8;
                rl = 1;
	        PMALLOC( *errind, Pint, il, ia);
	        PMALLOC( *errind, Pfloat, rl, ra);
                if ( !*errind) {
                    ia[0] = (Pint)state->record.pets.pet_r5.line_fill_ctrl_flag;
                    ia[1] = 0; /* ia[1] is unused */
                    ia[2] = (Pint)state->record.pets.pet_r5.attrs.line_attrs.type_asf;
                    ia[3] = (Pint)state->record.pets.pet_r5.attrs.line_attrs.width_asf;
                    ia[4] = (Pint)state->record.pets.pet_r5.attrs.line_attrs.colr_ind_asf;
                    ia[5] = state->record.pets.pet_r5.attrs.line_attrs.ind;
                    ia[6] = state->record.pets.pet_r5.attrs.line_attrs.bundle.type;
                    ia[7] = state->record.pets.pet_r5.attrs.line_attrs.bundle.colr_ind;
                    ra[0] = state->record.pets.pet_r5.attrs.line_attrs.bundle.width;
                }
                break;
	    case PFILLA:
                il = 9;
	        PMALLOC( *errind, Pint, il, ia);
                if ( !*errind) {
                    ia[0] = (Pint)state->record.pets.pet_r5.line_fill_ctrl_flag;
                    ia[1] = 0; /* ia[1] is unused */
                    ia[2] = (Pint)state->record.pets.pet_r5.attrs.int_attrs.style_asf;
                    ia[3] = (Pint)state->record.pets.pet_r5.attrs.int_attrs.style_ind_asf;
                    ia[4] = (Pint)state->record.pets.pet_r5.attrs.int_attrs.colr_ind_asf;
                    ia[5] = state->record.pets.pet_r5.attrs.int_attrs.ind;
                    ia[6] = (Pint)state->record.pets.pet_r5.attrs.int_attrs.bundle.style;
                    ia[7] = state->record.pets.pet_r5.attrs.int_attrs.bundle.style_ind;
                    ia[8] = state->record.pets.pet_r5.attrs.int_attrs.bundle.colr_ind;
                }
                break;
	    case PFILAS:
                il = 16;
                rl = 1;
	        PMALLOC( *errind, Pint, il, ia);
	        PMALLOC( *errind, Pfloat, rl, ra);
                if ( !*errind) {
                    ia[0] = (Pint)state->record.pets.pet_r5.line_fill_ctrl_flag;
                    ia[1] = 0; /* ia[1] is unused */
                    ia[2] = (Pint)state->record.pets.pet_r5.attrs.fill_set.int_attrs.style_asf;
                    ia[3] = (Pint)state->record.pets.pet_r5.attrs.fill_set.int_attrs.style_ind_asf;
                    ia[4] = (Pint)state->record.pets.pet_r5.attrs.fill_set.int_attrs.colr_ind_asf;
                    ia[5] = state->record.pets.pet_r5.attrs.fill_set.int_attrs.ind;
                    ia[6] = (Pint)state->record.pets.pet_r5.attrs.fill_set.int_attrs.bundle.style;
                    ia[7] = state->record.pets.pet_r5.attrs.fill_set.int_attrs.bundle.style_ind;
                    ia[8] = state->record.pets.pet_r5.attrs.fill_set.int_attrs.bundle.colr_ind;
                    ia[9] = (Pint)state->record.pets.pet_r5.attrs.fill_set.edge_attrs.flag_asf;
                    ia[10] = (Pint)state->record.pets.pet_r5.attrs.fill_set.edge_attrs.width_asf;
                    ia[11] = (Pint)state->record.pets.pet_r5.attrs.fill_set.edge_attrs.colr_ind_asf;
                    ia[12] = state->record.pets.pet_r5.attrs.fill_set.edge_attrs.ind;
                    ia[13] = (Pint)state->record.pets.pet_r5.attrs.fill_set.edge_attrs.bundle.flag;
                    ia[14] = state->record.pets.pet_r5.attrs.fill_set.edge_attrs.bundle.type;
                    ia[15] = state->record.pets.pet_r5.attrs.fill_set.edge_attrs.bundle.colr_ind;
                    ra[0] = state->record.pets.pet_r5.attrs.fill_set.edge_attrs.bundle.width;
                }
                break;
            }
	    break;
        default:
	    break;
	}
        if ( !*errind) {
            fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr,
               errind, ldr, datrec);
        }
        PFREE( il, ia);
        PFREE( rl, ra);
    }
}


static void
fb_inq_stroke_state( ws, dev, type, n, err, ret)
Pint	ws;		/* workstation identifier	*/
Pint	dev;		/* locator device number	*/
Pint	type;		/* type of returned value	*/
Pint	n;		/* maximum number of points	*/
Pint	*err;		/* OUT error indicator	*/
Phg_ret	*ret;
{
    Phg_args			cp_args;
    Wst_input_wsdt		*idt;

    if (idt = input_ws_open( phg_cur_cph, ws, Pfn_INQUIRY, NO_DT_NEEDED, err)) {
	if ( !DEVICE_EXISTS( idt, stroke, dev) ) {
	    *err = ERR250;

        } else if ( type < PSET || type > PREALI) {
            *err = ERR2000;

	} else {
	    cp_args.data.q_inp_state.inq_type = (Pinq_type)type;
	    INQ_STATE( phg_cur_cph, cp_args, ws, dev, PHG_ARGS_INP_STK3, ret)
	    if ( ret->err)
		*err = ret->err;
	    else
		*err = 0;
	}
    }
}


/* INQUIRE STROKE DEVICE STATE 3 */
void
pqsks3_( wkid, skdnr, type, n, mldr, errind, mode, esw, iviewi, np, ipxa, ipya,
ipza, pet, evol, ldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*skdnr;		/* stroke device number	*/
Pint	*type;		/* type of returned value	*/
Pint	*n;		/* MAX number of points	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mode;		/* OUT operating mode	*/
Pint	*esw;		/* OUT echo switch	*/
Pint	*iviewi;	/* OUT initial view indicator */
Pint	*np;		/* OUT number of points	*/
Pfloat	*ipxa;		/* OUT coordinate of initial stroke X	*/
Pfloat	*ipya;		/* OUT coordinate of initial stroke Y	*/
Pfloat	*ipza;		/* OUT coordinate of initial stroke Z	*/
Pint	*pet;		/* OUT prompt/echo type	*/
Plimit3	*evol;		/* OUT echo volume	*/
Pint	*ldr;		/* OUT number of array elements data record */
char	*datrec;	/* OUT data record	*/
{
    		Phg_ret		ret;
    register	Pstrokest3	*state = &ret.data.inp_state.stroke;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries		*/

    fb_inq_stroke_state( *wkid, *skdnr, *type, *n, errind, &ret);
    if ( !*errind) {
	*mode = (Pint)state->mode;
	*esw = (Pint)state->esw;
	*iviewi = state->stroke.view_ind;
	*np = state->stroke.num_points;
        if ( *n < *np) {
            *errind = ERR2001;
            return;
        }
        POINT3_DATA_RE_TRANSLATE( state->stroke.points, *np, ipxa, ipya, ipza);
	*pet = state->pet;
	*evol = state->e_volume;
	switch ( *pet) {
	case 1:
	case 2:
            il = 2;
            rl = 4;
	    PMALLOC( *errind, Pint, il, ia);
	    PMALLOC( *errind, Pfloat, rl, ra);
            if ( !*errind) {
	        ia[0] = state->record.buffer_size;
	        ia[1] = state->record.init_pos;
	        ra[0] = state->record.x_interval;
	        ra[1] = state->record.y_interval;
	        ra[2] = state->record.z_interval;
	        ra[3] = state->record.time_interval;
            }
	    break;
	case 3:
            il = 9;
            rl = 5;
	    PMALLOC( *errind, Pint, il, ia);
	    PMALLOC( *errind, Pfloat, rl, ra);
            if ( !*errind) {
	        ia[0] = state->record.buffer_size;
	        ia[1] = state->record.init_pos;
                ia[2] = 0; /* ia[2] = unused */
                ia[3] = (Pint)state->record.pets.pet_r3.marker_attrs.type_asf;
                ia[4] = (Pint)state->record.pets.pet_r3.marker_attrs.size_asf;
                ia[5] = 
                    (Pint)state->record.pets.pet_r3.marker_attrs.colr_ind_asf;
                ia[6] = state->record.pets.pet_r3.marker_attrs.ind;
                ia[7] = state->record.pets.pet_r3.marker_attrs.bundle.type;
                ia[8] = state->record.pets.pet_r3.marker_attrs.bundle.colr_ind;
	        ra[0] = state->record.x_interval;
	        ra[1] = state->record.y_interval;
	        ra[2] = state->record.z_interval;
	        ra[3] = state->record.time_interval;
                ra[4] = state->record.pets.pet_r3.marker_attrs.bundle.size;
            }
            break; 
	case 4:
            il = 9;
            rl = 5;
	    PMALLOC( *errind, Pint, il, ia);
	    PMALLOC( *errind, Pfloat, rl, ra);
            if ( !*errind) {
	        ia[0] = state->record.buffer_size;
	        ia[1] = state->record.init_pos;
                ia[2] = 0; /* ia[2] = unused */
                ia[3] = (Pint)state->record.pets.pet_r4.line_attrs.type_asf;
                ia[4] = (Pint)state->record.pets.pet_r4.line_attrs.width_asf;
                ia[5] = (Pint)state->record.pets.pet_r4.line_attrs.colr_ind_asf;
                ia[6] = state->record.pets.pet_r4.line_attrs.ind;
                ia[7] = state->record.pets.pet_r4.line_attrs.bundle.type;
                ia[8] = state->record.pets.pet_r4.line_attrs.bundle.colr_ind;
	        ra[0] = state->record.x_interval;
	        ra[1] = state->record.y_interval;
	        ra[2] = state->record.z_interval;
	        ra[3] = state->record.time_interval;
                ra[4] = state->record.pets.pet_r4.line_attrs.bundle.width;
            }
	    break;
	default:
	    break;
	}
        if ( !*errind) {
            fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr,
                errind, ldr, datrec);
        }
        PFREE( il, ia);
        PFREE( rl, ra);
    }
}


/* INQUIRE STROKE DEVICE STATE */
void
pqsks_( wkid, skdnr, type, n, mldr, errind, mode, esw, iviewi, np, ipxa, ipya,
pet, earea, ldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*skdnr;		/* stroke device number	*/
Pint	*type;		/* type of returned value	*/
Pint	*n;		/* maximum mumber of points	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mode;		/* OUT operating mode	*/
Pint	*esw;		/* OUT echo switch	*/
Pint	*iviewi;	/* OUT initial view indicator	*/
Pint	*np;		/* OUT number of points	*/
Pfloat	*ipxa;		/* OUT coordinate of initial stroke X	*/
Pfloat	*ipya;		/* OUT coordinate of initial stroke Y	*/
Pint	*pet;		/* OUT prompt/echo type	*/
Plimit	*earea;		/* OUT echo area	*/
Pint	*ldr;		/* OUT number of array elements data record */
char	*datrec;	/* OUT data record	*/
{
    		Phg_ret		ret;
    register	Pstrokest3	*state = &ret.data.inp_state.stroke;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries		*/

    fb_inq_stroke_state( *wkid, *skdnr, *type, *n, errind, &ret);
    if ( !*errind) {
	CB_ECHO_VOLUME_TO_AREA( state->e_volume, *earea);
	*mode = (Pint)state->mode;
	*esw = (Pint)state->esw;
	*iviewi = state->stroke.view_ind;
	*np = state->stroke.num_points;
        if ( *n < *np) {
            *errind = ERR2001;
            return;
        }
        POINT_DATA_RE_TRANSLATE( state->stroke.points, *np, ipxa, ipya);
	*pet = state->pet;
	switch ( *pet) {
	case 1:
	case 2:
            il = 2;
            rl = 3;
	    PMALLOC( *errind, Pint, il, ia);
	    PMALLOC( *errind, Pfloat, rl, ra);
            if ( !*errind) {
	        ia[0] = state->record.buffer_size;
	        ia[1] = state->record.init_pos;
	        ra[0] = state->record.x_interval;
	        ra[1] = state->record.y_interval;
	        ra[2] = state->record.time_interval;
            }
	    break;
	case 3:
            il = 9;
            rl = 4;
	    PMALLOC( *errind, Pint, il, ia);
	    PMALLOC( *errind, Pfloat, rl, ra);
            if ( !*errind) {
	        ia[0] = state->record.buffer_size;
	        ia[1] = state->record.init_pos;
                ia[2] = 0; /* ia[2] is unused */
                ia[3] = (Pint)state->record.pets.pet_r3.marker_attrs.type_asf;
                ia[4] = (Pint)state->record.pets.pet_r3.marker_attrs.size_asf;
                ia[5] =
                    (Pint)state->record.pets.pet_r3.marker_attrs.colr_ind_asf;
                ia[6] = state->record.pets.pet_r3.marker_attrs.ind;
                ia[7] = state->record.pets.pet_r3.marker_attrs.bundle.type;
                ia[8] = state->record.pets.pet_r3.marker_attrs.bundle.colr_ind;
	        ra[0] = state->record.x_interval;
	        ra[1] = state->record.y_interval;
	        ra[2] = state->record.time_interval;
                ra[3] = state->record.pets.pet_r3.marker_attrs.bundle.size;
            }
            break; 
	case 4:
            il = 9;
            rl = 4;
	    PMALLOC( *errind, Pint, il, ia);
	    PMALLOC( *errind, Pfloat, rl, ra);
            if ( !*errind) {
	        ia[0] = state->record.buffer_size;
	        ia[1] = state->record.init_pos;
                ia[2] = 0; /* ia[2] is unused */
                ia[3] = (Pint)state->record.pets.pet_r4.line_attrs.type_asf;
                ia[4] = (Pint)state->record.pets.pet_r4.line_attrs.width_asf;
                ia[5] = (Pint)state->record.pets.pet_r4.line_attrs.colr_ind_asf;
                ia[6] = state->record.pets.pet_r4.line_attrs.ind;
                ia[7] = state->record.pets.pet_r4.line_attrs.bundle.type;
                ia[8] = state->record.pets.pet_r4.line_attrs.bundle.colr_ind;
	        ra[0] = state->record.x_interval;
	        ra[1] = state->record.y_interval;
	        ra[2] = state->record.time_interval;
                ra[3] = state->record.pets.pet_r4.line_attrs.bundle.width;
            }
	    break;
	default:
	    break;
	}
        if ( !*errind) {
            fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr,
                errind, ldr, datrec);
        }
        PFREE( il, ia);
        PFREE( rl, ra);
    }
}


static void
fb_inq_val_state( ws, dev, err, ret)
Pint	ws;		/* workstation identifier	*/
Pint	dev;		/* locator device number	*/
Pint	*err;		/* OUT error indicator	*/
Phg_ret	*ret;
{
    Phg_args			cp_args;
    Wst_input_wsdt		*idt;

    if (idt = input_ws_open( phg_cur_cph, ws, Pfn_INQUIRY, NO_DT_NEEDED, err)) {
	if ( !DEVICE_EXISTS( idt, val, dev) ) {
	    *err = ERR250;
	} else {
	    INQ_STATE( phg_cur_cph, cp_args, ws, dev, PHG_ARGS_INP_VAL3, ret)
	    if ( ret->err)
		*err = ret->err;
	    else
		*err = 0;
	}
    }
}

static void
fb_copy_val_rec( state, err, mldr, ldr, datrec)
Pvalst3	*state;
Pint	*err;
Pint	mldr;
Pint	*ldr;
char	*datrec;
{
    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries		*/

    Pint	total = 0;
    char	*w_str;
    int		i;

    switch ( state->pet) {
    case -1:
        rl = 2;
        sl = 4;
        PMALLOC( *err, Pfloat, rl, ra);
        PMALLOC( *err, Pint, sl, lstr);
        if ( !*err) {
            for ( i = 0; i < 4; i++) {
                if ( state->counts[i] > 0) {
                    lstr[i] = state->counts[i] - 1;
                } else if ( state->counts[i] == 0) {
                    lstr[i] = 0;
                }
                total += lstr[i];
            }
            PMALLOC( *err, char, total, str);
            if ( *err) {
                PFREE( rl, ra);
                PFREE( sl, lstr);
                return;
            } else {    
                ra[0] = state->record.low;
                ra[1] = state->record.high;
                    w_str = str;
                bcopy( state->record.pets.pet_u1.label, w_str, lstr[0]);
                    w_str += lstr[0];
                bcopy( state->record.pets.pet_u1.format, w_str, lstr[1]);
                    w_str += lstr[1];
                bcopy( state->record.pets.pet_u1.low_label, w_str, lstr[2]);
                    w_str += lstr[2];
                bcopy( state->record.pets.pet_u1.high_label, w_str, lstr[3]);
            }
        }
        break;
#ifdef EXT_INPUT
    case -2:
        rl = 3;
        PMALLOC( *err, Pfloat, rl, ra);
        if ( !*err) {
            ra[0] = state->record.low;
            ra[1] = state->record.high;
            ra[2] = state->record.pets.pet_d1.dial;
        }
	    break;
#endif
    default:
        rl = 2;
        PMALLOC( *err, Pfloat, rl, ra);
        if ( !*err) {
            ra[0] = state->record.low;
            ra[1] = state->record.high;
        }
        break;
    }
    if ( !*err) {
        fb_pack_rec( il, ia, rl, ra, sl, lstr, str, mldr,
            err, ldr, datrec);
    }
    PFREE( il, ia);
    PFREE( rl, ra);
    PFREE( sl, lstr);
    PFREE( total, str);
}


/* INQUIRE VALUATOR DEVICE STATE 3 */
void
pqvls3_( wkid, vldnr, mldr, errind, mode, esw, ival, pet, evol, ldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*vldnr;		/* valuator device number	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mode;		/* OUT operating mode	*/
Pint	*esw;		/* OUT echo switch	*/
Pfloat	*ival;		/* OUT initial value	*/
Pint	*pet;		/* OUT prompt/echo type	*/
Plimit3	*evol;		/* OUT echo volume	*/
Pint	*ldr;		/* OUT number of array elements data record */
char	*datrec;	/* OUT data record	*/    
{
    		Phg_ret		ret;
    register	Pvalst3		*state = &ret.data.inp_state.val;

    fb_inq_val_state( *wkid, *vldnr, errind, &ret);
    if ( !*errind) {
	*mode = (Pint)state->mode;
	*esw = (Pint)state->esw;
	*ival = state->val;
	*pet = state->pet;
	*evol = state->e_volume;
	fb_copy_val_rec( state, errind, *mldr, ldr, datrec);
    }
}


/* INQUIRE VALUATOR DEVICE STATE */
void
pqvls_( wkid, vldnr, mldr, errind, mode, esw, ival, pet, earea, ldr, datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*vldnr;		/* valuator device number	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mode;		/* OUT operating mode	*/
Pint	*esw;		/* OUT echo switch	*/
Pfloat	*ival;		/* OUT initial value	*/
Pint	*pet;		/* OUT prompt/echo type	*/
Plimit	*earea;		/* OUT echo area	*/
Pint	*ldr;		/* OUT number of array elements data record */
char	*datrec;	/* OUT data record	*/    
{
    Phg_ret		ret;
    register Pvalst3	*state = &ret.data.inp_state.val;

    fb_inq_val_state( *wkid, *vldnr, errind, &ret);
    if ( !*errind) {
	CB_ECHO_VOLUME_TO_AREA( state->e_volume, *earea);
	*mode = (Pint)state->mode;
	*esw = (Pint)state->esw;
	*ival = state->val;
	*pet = state->pet;
	fb_copy_val_rec( state, errind, *mldr, ldr, datrec);
    }
}

static void
fb_inq_choice_state( ws, dev, err, ret)
Pint	ws;		/* workstation identifier	*/
Pint	dev;		/* choice device number */
Pint	*err;		/* OUT error indicator	*/
Phg_ret	*ret;
{
    Phg_args		cp_args;
    Wst_input_wsdt	*idt;

    if (idt = input_ws_open( phg_cur_cph, ws, Pfn_INQUIRY, NO_DT_NEEDED, err)) {
	if ( !DEVICE_EXISTS( idt, choice, dev) ) {
	    *err = ERR250;
	} else {
	    INQ_STATE( phg_cur_cph, cp_args, ws, dev, PHG_ARGS_INP_CHC3, ret)
	    if ( ret->err)
		*err = ret->err;
	    else
		*err = 0;
	}
    }
}


/* INQUIRE CHOICE DEVICE STATE 3 */
void
pqchs3_( wkid, chdnr, mldr, errind, mode, esw, istat, ichnr, pet, evol, ldr,
datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*chdnr;		/* choice device number	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mode;		/* OUT operating mode	*/
Pint	*esw;		/* OUT echo switch	*/
Pint	*istat;		/* OUT initial choice status	*/
Pint	*ichnr;		/* OUT initial choice	*/
Pint	*pet;		/* OUT prompt/echo type	*/
Plimit3	*evol;		/* OUT echo volume	*/
Pint	*ldr;		/* OUT number of array elements data record */
char	*datrec;	/* OUT data record      */
{
    int		i;
    char	*strptr;
    Phg_ret	ret;
    Pchoicest3	*state = &ret.data.inp_state.choice.state;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries		*/

    Pint	total = 0;
    char	*w_str;

    fb_inq_choice_state( *wkid, *chdnr, errind, &ret);
    if ( !*errind) {
	*mode = (Pint)state->mode;
	*esw = (Pint)state->esw;
	IN_ST_CONVERT_C_TO_F( state->choice.status, *istat);
        if ( *istat == PNONE)
            *istat = PNCHOI;
	*ichnr = state->choice.choice;
	*evol = state->e_volume;
	*pet = state->pet;
	switch ( *pet) {
	case 2:
            il = state->record.pets.pet_r2.num_prompts;
            PMALLOC( *errind, Pint, il, ia);
            if ( !*errind) {
                ia[0] = state->record.pets.pet_r2.num_prompts;
		bcopy((char *)state->record.pets.pet_r2.prompts, (char *)ia,
		    il * sizeof(Pint));
            }
	    break;
        case 3:
            sl = state->record.pets.pet_r3.num_strings;
            PMALLOC( *errind, Pint, sl, lstr);
            if ( !*errind) {
	        strptr = ret.data.inp_state.choice.strings;
                for ( i = 0; i < sl; i++) {
	            lstr[i] = strlen(strptr);
                    strptr += lstr[i] + 1;
                    total += lstr[i];
                }
                PMALLOC( *errind, char, total, str);
                if ( *errind) {
                    PFREE( sl, lstr);
                    return;
                } else {
	            strptr = ret.data.inp_state.choice.strings;
	            w_str = str;
                    for ( i = 0; i < sl; i++) {
	                bcopy((char *)strptr, (char *)w_str, lstr[i]);
                        strptr += lstr[i] + 1;
                        w_str += lstr[i];
                    }
                }
	    }
	    break;
        case 4:
            sl = state->record.pets.pet_r4.num_strings;
            PMALLOC( *errind, Pint, sl, lstr);
            if ( !*errind) {
	        strptr = ret.data.inp_state.choice.strings;
                for ( i = 0; i < sl; i++) {
	            lstr[i] = strlen(strptr);
                    strptr += lstr[i] + 1;
                    total += lstr[i];
                }
                PMALLOC( *errind, char, total, str);
                if ( *errind) {
                    PFREE( sl, lstr);
                    return;
                } else {
	            strptr = ret.data.inp_state.choice.strings;
	            w_str = str;
                    for ( i = 0; i < sl; i++) {
	                bcopy((char *)strptr, (char *)w_str, lstr[i]);
                        strptr += lstr[i] + 1;
                        w_str += lstr[i];
                    }
                }
	    }
	    break;
        case 5:
            il = state->record.pets.pet_r5.num_pick_ids + 1;
            PMALLOC( *errind, Pint, il, ia);
            if ( !*errind) {
                ia[0] = state->record.pets.pet_r5.struct_id;
                bcopy((char *)state->record.pets.pet_r5.pick_ids, (char *)(ia + 1),
                    state->record.pets.pet_r5.num_pick_ids * sizeof(Pint));
            }
            break;
        default:
	    break;
        }
        if ( !*errind) {
            fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr,
                errind, ldr, datrec);
        } 
        PFREE( il, ia);
        PFREE( sl, lstr);
        PFREE( total, str);
    }
}


/* INQUIRE CHOICE DEVICE STATE */
void
pqchs_( wkid, chdnr, mldr, errind, mode, esw, istat, ichnr, pet, earea, ldr,
datrec)
Pint	*wkid;		/* workstation identifier	*/
Pint	*chdnr;		/* choice device number	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mode;		/* OUT operating mode	*/
Pint	*esw;		/* OUT echo switch	*/
Pint	*istat;		/* OUT initial choice status	*/
Pint	*ichnr;		/* OUT initial choice	*/
Pint	*pet;		/* OUT prompt/echo type	*/
Plimit	*earea;		/* OUT echo area	*/
Pint	*ldr;		/* OUT number of array elements data record */
char	*datrec;	/* OUT data record      */
{
    int		i;
    char	*strptr;
    Phg_ret	ret;
    Pchoicest3	*state = &ret.data.inp_state.choice.state;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries		*/

    Pint	total = 0;
    char	*w_str;

    fb_inq_choice_state( *wkid, *chdnr, errind, &ret);
    if ( !*errind) {
	CB_ECHO_VOLUME_TO_AREA( state->e_volume, *earea);
	*mode = (Pint)state->mode;
	*esw = (Pint)state->esw;
	*pet = state->pet;
	IN_ST_CONVERT_C_TO_F( state->choice.status, *istat);
        if ( *istat == PNONE)
            *istat = PNCHOI;
	*ichnr = state->choice.choice;
	switch ( *pet) {
	case 2:
            il = state->record.pets.pet_r2.num_prompts;
            PMALLOC( *errind, Pint, il, ia);
            if ( !*errind) {
		bcopy((char *)state->record.pets.pet_r2.prompts, (char *)ia,
		    il * sizeof(Pint));
            }
	    break;
        case 3:
            sl = state->record.pets.pet_r3.num_strings;
            PMALLOC( *errind, Pint, sl, lstr);
            if ( !*errind) {
	        strptr = ret.data.inp_state.choice.strings;
                for ( i = 0; i < sl; i++) {
	            lstr[i] = strlen(strptr);
                    strptr += lstr[i] + 1;
                    total += lstr[i];
                }
                PMALLOC( *errind, char, total, str);
                if ( *errind) {
                    PFREE( sl, lstr);
                    return;
                } else {
	            strptr = ret.data.inp_state.choice.strings;
	            w_str = str;
                    for ( i = 0; i < sl; i++) {
	                bcopy((char *)strptr, (char *)w_str, lstr[i]);
                        strptr += lstr[i] + 1;
                        w_str += lstr[i];
                    }
                }
	    }
	    break;
        case 4:
            sl = state->record.pets.pet_r4.num_strings;
            PMALLOC( *errind, Pint, sl, lstr);
            if ( !*errind) {
	        strptr = ret.data.inp_state.choice.strings;
                for ( i = 0; i < sl; i++) {
	            lstr[i] = strlen(strptr);
                    strptr += lstr[i] + 1;
                    total += lstr[i];
                }
                PMALLOC( *errind, char, total, str);
                if ( *errind) {
                    PFREE( sl, lstr);
                    return;
                } else {
	            strptr = ret.data.inp_state.choice.strings;
	            w_str = str;
                    for ( i = 0; i < sl; i++) {
	                bcopy((char *)strptr, (char *)w_str, lstr[i]);
                        strptr += lstr[i] + 1;
                        w_str += lstr[i];
                    }
                }
	    }
	    break;
        case 5:
            il = state->record.pets.pet_r5.num_pick_ids + 1;
            PMALLOC( *errind, Pint, il, ia);
            if ( !*errind) {
                ia[0] = state->record.pets.pet_r5.struct_id;
                bcopy((char *)state->record.pets.pet_r5.pick_ids, (char *)(ia + 1),
                    state->record.pets.pet_r5.num_pick_ids * sizeof(Pint));
            }
	    break;
        default:
	    break;
        }
        if ( !*errind) {
            fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr,
                errind, ldr, datrec);
        }
        PFREE( il, ia);
        PFREE( sl, lstr);
        PFREE( total, str);
    }
}

static void
fb_inq_pick_state( ws, dev, type, err, ret)
Pint	ws;		/* workstation identifier	*/
Pint	dev;		/* string device number	*/
Pint	type;		/* type of returned value	*/
Pint	*err;		/* OUT error indicator	*/
Phg_ret	*ret;
{
    Phg_args			cp_args;
    Wst_input_wsdt		*idt;

    if (idt = outin_ws_open( phg_cur_cph, ws, Pfn_INQUIRY, NO_DT_NEEDED, err)) {
	if ( !DEVICE_EXISTS( idt, pick, dev) ) {
	    *err = ERR250;

	} else if ( type < PSET || type > PREALI) {
	    *err = ERR2000;

	} else {
	    cp_args.data.q_inp_state.inq_type = (Pinq_type)type;
	    INQ_STATE( phg_cur_cph, cp_args, ws, dev, PHG_ARGS_INP_PIK3, ret)
	    if ( ret->err)
		*err = ret->err;
	    else
		*err = 0;
	}
    }
}

static void
fb_copy_pick_data( err, state, ipissz, ipessz, ippsz, pissz, pins, pessz, pes,
ppd, pp)
Pint		*err;
Ppickst3	*state;
Pint		ipissz;		/* pick inclusion set of buffer size	*/
Pint		ipessz;		/* pick exclusion set of buffer size	*/
Pint		ippsz;		/* pick path buffer size	*/
Pint		*pissz;		/* OUT pick inclusion set size	*/
Pint		*pins;		/* OUT pick inclusion set	*/
Pint		*pessz;		/* OUT pick exclusion set size	*/
Pint		*pes;		/* OUT pick exclusion set	*/
Pint		*ppd;		/* OUT initial pick path depth	*/
Pint		*pp;		/* OUT initial pick path	*/
{
    int		i;

    *ppd = (state->pick.status == PIN_STATUS_OK ?
        state->pick.pick_path.depth : 0);
    if ( ippsz < *ppd) {
        *err = ERR2001;
        return;
    } else if ( *ppd > 0) {
        for ( i = 0; i < *ppd; i++) {
            pp[i * 3] = state->pick.pick_path.path_list[i].struct_id;
            pp[i * 3 + 1] = state->pick.pick_path.path_list[i].pick_id;
            pp[i * 3 + 2] = state->pick.pick_path.path_list[i].elem_pos;
        }
    }
    *pissz = state->inclusion_filter.num_ints;
    if ( ipissz < *pissz) {
        *err = ERR2001;
        return;
    } else if ( *pissz > 0) {
        bcopy( (char*)state->inclusion_filter.ints, (char*)pins,
            *pissz * sizeof(Pint));
    }
    *pessz = state->exclusion_filter.num_ints;
    if ( ipessz < *pessz) {
        *err = ERR2001;
        return;
    } else if ( *pessz > 0) {
        bcopy( (char*)state->exclusion_filter.ints, (char*)pes,
            *pessz * sizeof(Pint));
    }
}


/* INQUIRE PICK DEVICE STATE 3 */
void
pqpks3_( wkid, pkdnr, type, mldr, ipissz, ipessz, ippsz, errind, mode, esw,
pissz, pins, pessz, pes, istat, ppd, pp, pet, evol, ldr, datrec, ppordr)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pkdnr;		/* pick device number	*/
Pint	*type;		/* type of returned value	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*ipissz;	/* pick inclusion set of buffer size	*/
Pint	*ipessz;	/* pick exclusion set of buffer size	*/
Pint	*ippsz;		/* pick path buffer size	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mode;		/* OUT operating mode	*/
Pint	*esw;		/* OUT echo switch	*/
Pint	*pissz;		/* OUT pick inclusion set size	*/
Pint	*pins;		/* OUT pick inclusion set	*/
Pint	*pessz;		/* OUT pick exclusion set size	*/
Pint	*pes;		/* OUT pick exclusion set	*/
Pint	*istat;		/* OUT initial pick status	*/
Pint	*ppd;		/* OUT initial pick path depth	*/
Pint	*pp;		/* OUT initial pick path	*/
Pint	*pet;		/* OUT prompt/echo type	*/
Plimit3	*evol;		/* OUT echo volume	*/
Pint	*ldr;		/* OUT number of array elements data record */
char	*datrec;	/* OUT data record	*/
Pint	*ppordr;	/* OUT pick path order	*/
{
    		Phg_ret		ret;
    register	Ppickst3	*state = &ret.data.inp_state.pick;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries		*/

    fb_inq_pick_state( *wkid, *pkdnr, *type, errind, &ret);
    if ( !*errind) {
	*mode = (Pint)state->mode;
	*esw = (Pint)state->esw;
	IN_ST_CONVERT_C_TO_F( state->pick.status, *istat);
        if ( *istat == PNONE)
            *istat = PNPICK;
	*evol = state->e_volume;
	*pet = state->pet;
	*ppordr = (Pint)state->order;
	switch ( *pet) {
#ifdef EXT_INPUT
        case -1:
            rl = 1;
            PMALLOC( *errind, Pfloat, rl, ra);
            if ( !*errind) {
		ra[0] = state->record.pets.pet_u1.ap_size;
            }
	    break;
#endif
        default:
	    break;
	}
	fb_copy_pick_data( errind, state, *ipissz, *ipessz, *ippsz, pissz, pins,            pessz, pes, ppd, pp);
        if ( !*errind)
            fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr, errind, ldr,
                datrec);
        PFREE( il, ia);
    }
}


/* INQUIRE PICK DEVICE STATE */
void
pqpks_( wkid, pkdnr, type, mldr, ipissz, ipessz, ippsz, errind, mode, esw,
pissz, pins, pessz, pes, istat, ppd, pp, pet, earea, ldr, datrec, ppordr)
Pint	*wkid;		/* workstation identifier	*/
Pint	*pkdnr;		/* pick device number	*/
Pint	*type;		/* type of returned value	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*ipissz;	/* pick inclusion set of buffer size	*/
Pint	*ipessz;	/* pick exclusion set of buffer size	*/
Pint	*ippsz;		/* pick path buffer size	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mode;		/* OUT operating mode	*/
Pint	*esw;		/* OUT echo switch	*/
Pint	*pissz;		/* OUT pick inclusion set size	*/
Pint	*pins;		/* OUT pick inclusion set	*/
Pint	*pessz;		/* OUT pick exclusion set size	*/
Pint	*pes;		/* OUT pick exclusion set	*/
Pint	*istat;		/* OUT initial pick status	*/
Pint	*ppd;		/* OUT initial pick path depth	*/
Pint	*pp;		/* OUT initial pick path	*/
Pint	*pet;		/* OUT prompt/echo type	*/
Plimit	*earea;		/* OUT echo area	*/
Pint	*ldr;		/* OUT number of array elements data record */
char	*datrec;	/* OUT data record	*/
Pint	*ppordr;	/* OUT pick path order	*/
{
    		Phg_ret		ret;
    register	Ppickst3	*state = &ret.data.inp_state.pick;
  
    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries		*/

    fb_inq_pick_state( *wkid, *pkdnr, *type, errind, &ret);
    if ( !*errind) {
	CB_ECHO_VOLUME_TO_AREA( state->e_volume, *earea);
	*mode = (Pint)state->mode;
	*esw = (Pint)state->esw;
	IN_ST_CONVERT_C_TO_F( state->pick.status, *istat);
        if ( *istat == PNONE)
            *istat = PNPICK;
	*pet = state->pet;
	*ppordr = (Pint)state->order;
	switch ( *pet) {
#ifdef EXT_INPUT
        case -1:
            rl = 1;
            PMALLOC( *errind, Pfloat, rl, ra);
            if ( !*errind) {
		ra[0] = state->record.pets.pet_u1.ap_size;
            }
	    break;
#endif
	    default:
		break;
	}
	fb_copy_pick_data( errind, state, *ipissz, *ipessz, *ippsz, pissz, pins,
            pessz, pes, ppd, pp);
        if ( !*errind)
            fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr, errind, ldr,
                datrec);
        PFREE( il, ia);
    }
}


static void
fb_inq_string_state( ws, dev, err, ret)
Pint	ws;		/* workstation identifier	*/
Pint	dev;		/* string device number	*/
Pint	*err;		/* OUT error indicator	*/
Phg_ret	*ret;
{
    Phg_args			cp_args;
    Wst_input_wsdt		*idt;

    if (idt = input_ws_open( phg_cur_cph, ws, Pfn_INQUIRY, NO_DT_NEEDED, err)) {
	if ( !DEVICE_EXISTS( idt, string, dev) ) {
	    *err = ERR250;
	} else {
	    INQ_STATE( phg_cur_cph, cp_args, ws, dev, PHG_ARGS_INP_STR3, ret)
	    if ( ret->err)
		*err = ret->err;
	    else
		*err = 0;
	}
    }
}


/* INQUIRE STRING DEVICE STATE 3 */
void
fqsts3_( wkid, stdnr, mldr, errind, mode, esw, lostr, istr, pet, evol, ldr,
datrec, length)
Pint	*wkid;		/* workstation identifier	*/
Pint	*stdnr;		/* string device number	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mode;		/* OUT operating mode	*/
Pint	*esw;		/* OUT echo switch	*/
Pint	*lostr;		/* OUT number of characters returned	*/
char	*istr;		/* OUT initial string	*/
Pint	*pet;		/* OUT prompt/echo type	*/
Plimit3	*evol;		/* OUT echo volume	*/
Pint	*ldr;		/* OUT number of array elements data record */
char	*datrec;	/* OUT data record	*/
int	*length;	/* string length	*/
{
    		Phg_ret		ret;
    register	Pstringst3	*state = &ret.data.inp_state.string.state;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries		*/

    fb_inq_string_state( *wkid, *stdnr, errind, &ret);
    if ( !*errind) {
	*mode = (Pint)state->mode;
	*esw = (Pint)state->esw;
	*pet = state->pet;
	*evol = state->e_volume;
        if ( *length < ret.data.inp_state.string.length - 1) {
            *errind = ERR2001;
            return;
        }
	*lostr = ret.data.inp_state.string.length ?
		ret.data.inp_state.string.length - 1 : 0;
	if ( ret.data.inp_state.string.state.string)
	    strcpy( istr, ret.data.inp_state.string.state.string);
	switch ( *pet) {
	default:
            il = 2;
            PMALLOC( *errind, Pint, il, ia);
            if ( !*errind) {
	        ia[0] = state->record.buffer_size;
	        ia[1] = state->record.init_pos;
            }
            break;
	}
        if ( !*errind) {
            fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr,
                errind, ldr, datrec);
        }
        PFREE( il, ia);
    }
}


/* INQUIRE STRING DEVICE STATE */
void
fqsts_( wkid, stdnr, mldr, errind, mode, esw, lostr, istr, pet, earea, ldr,
datrec, length)
Pint	*wkid;		/* workstation identifier	*/
Pint	*stdnr;		/* string device number	*/
Pint	*mldr;		/* dimension of data record array	*/
Pint	*errind;	/* OUT error indicator	*/
Pint	*mode;		/* OUT operating mode	*/
Pint	*esw;		/* OUT echo switch	*/
Pint	*lostr;		/* OUT number of characters returned	*/
char	*istr;		/* OUT initial string	*/
Pint	*pet;		/* OUT prompt/echo type	*/
Plimit	*earea;		/* OUT echo area	*/
Pint	*ldr;		/* OUT number of array elements data record */
char	*datrec;	/* OUT data record	*/
int	*length;	/* string length	*/
{
    		Phg_ret		ret;
    register	Pstringst3	*state = &ret.data.inp_state.string.state;

    Pint	il = 0;		/* number of integer entries	*/
    Pint	*ia = NULL;	/* array containing integer entries	*/
    Pint	rl = 0;		/* number of real entries	*/
    Pfloat	*ra = NULL;	/* array containing real entries	*/
    Pint	sl = 0;		/* number of character string entries	*/
    Pint	*lstr = NULL;	/* length of character string entries	*/
    char	*str = NULL;	/* character string entries		*/

    fb_inq_string_state( *wkid, *stdnr, errind, &ret);
    if ( !*errind) {
	CB_ECHO_VOLUME_TO_AREA( state->e_volume, *earea);
	*mode = (Pint)state->mode;
	*esw = (Pint)state->esw;
	*pet = state->pet;
        if ( *length < ret.data.inp_state.string.length - 1) {
            *errind = ERR2001;
            return;
        }
	*lostr = ret.data.inp_state.string.length ?
		ret.data.inp_state.string.length - 1 : 0;
	if ( ret.data.inp_state.string.state.string)
	    strcpy( istr, ret.data.inp_state.string.state.string);
	switch ( *pet) {
	default:
            il = 2;
            PMALLOC( *errind, Pint, il, ia);
            if ( !*errind) {
	        ia[0] = state->record.buffer_size;
	        ia[1] = state->record.init_pos;
            }
            break;
	}
        if ( !*errind) {
            fb_pack_rec( il, ia, rl, ra, sl, lstr, str, *mldr,
                errind, ldr, datrec);
        }
        PFREE( il, ia);
    }
}


/* INQUIRE MORE SIMULTANEOUS EVENTS */
void
pqsim_( errind, flag)
Pint	*errind;	/* OUT error indicator	*/
Pint	*flag;		/* OUT simultaneous events	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY) ) {
	*errind = ERR2;

    } else {
        ret.err = 0;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_MORE_EVENTS, &cp_args, &ret);
	if ( ret.err)
	    *errind = ret.err;
	else {
	    *errind = 0;
	    *flag = ret.data.idata;
	}
    }
}


/* INQUIRE INPUT QUEUE OVERFLOW */
void
pqiqov_( errind, wkid, icl, idn)
Pint	*errind;	/* OUT error indicator	*/
Pint	*wkid;		/* OUT workstation identifier	*/
Pint	*icl;		/* OUT input class	*/
Pint	*idn;		/* OUT input device number	*/
{
    Phg_args		cp_args;
    Phg_ret		ret;

    if ( !CB_ENTRY_CHECK( phg_cur_cph, 0, Pfn_INQUIRY) ) {
	*errind = ERR3;

    } else if ( PSL_WS_STATE( phg_cur_cph->psl) != PWSOP) {
	*errind = ERR3;

    } else {
        ret.err = 0;
	CP_FUNC( phg_cur_cph, CP_FUNC_OP_INQ_INP_OVERFLOW, &cp_args, &ret);
	if ( ret.err)
	    *errind =  ret.err;
	else {
	    *errind = 0;
	    *wkid = ret.data.oflow_event.ws;
	    *icl = (Pint)ret.data.oflow_event.class;
	    *idn = ret.data.oflow_event.dev;
	}
    }
}
