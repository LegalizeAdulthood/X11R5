/* $XConsortium: cpa_pm.c,v 5.1 91/02/16 09:48:38 rws Exp $ */

/***********************************************************
Copyright 1989, 1990, 1991 by Sun Microsystems, Inc. and the X Consortium.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include "phg.h"
#include "cp.h"
#include "ws.h"
#include "cp_priv.h"
#include "PEX.h"
#include "PEXmacs.h"
#include "PEXfuncs.h"
#include "PEXproto.h"
#include "phigspex.h"


static void
cpa_pm_load_funcs( cph, css_srvr )
    Cp_handle		cph;
    Cpx_css_srvr	*css_srvr;
{
    extern void 	phg_cpa_pm_close_ws();

    css_srvr->open_ws = phg_wsa_pm_open_ws;
    css_srvr->close_ws = phg_cpa_pm_close_ws;
    css_srvr->destroy = phg_cpa_pm_destroy;
    css_srvr->set_filter = phg_cpa_pm_set_filter;
    css_srvr->message = phg_cpa_pm_message;
    css_srvr->redraw_regions = phg_cpa_ws_redraw_regions;
}


Cpx_css_srvr*
phg_cpa_pm_init( cph, display, base, shift, pex_info )
    Cp_handle		cph;
    Display		*display;
    long		base;
    int			shift;
    Phg_pex_ext_info	*pex_info;
{
    Cpx_css_srvr	*css_srvr;

    if ( !(css_srvr = (Cpx_css_srvr *)calloc(1,sizeof(Cpx_css_srvr)))) {
	ERR_BUF( cph->erh, ERR900 );

    } else if ( !(PHG_SCRATCH_SPACE( &css_srvr->model.a.scratch, 8096 ) ) ) {
	free( (char *)css_srvr );
	css_srvr = (Cpx_css_srvr *)NULL;
	ERR_BUF( cph->erh, ERR900 );

    } else {
	css_srvr->display = display;
	css_srvr->model.a.dpy_resource_base = base;
	css_srvr->model.a.dpy_resource_shift = shift;
	css_srvr->pex_info = *pex_info;
	css_srvr->type = CPX_SRVR_SS;
	cpa_pm_load_funcs( cph, css_srvr );
    }

    return css_srvr;
}


void
phg_cpa_pm_destroy( cph, css_srvr )
    Cp_handle		cph;
    Cpx_css_srvr	*css_srvr;
{
    if ( css_srvr->model.a.scratch.size >  0 ) {
	free( (char *)css_srvr->model.a.scratch.buf );
	css_srvr->model.a.scratch.buf = NULL;
	css_srvr->model.a.scratch.size = 0;
    }
    free( (char *)css_srvr );
}


void
phg_cpa_pm_close_ws( cph, cp_args, ws )
    Cp_handle		cph;
    Phg_args		*cp_args;
    Ws_handle		ws;
{
    if ( ws->type->desc_tbl.phigs_dt.ws_category == PCAT_OUTIN ) {
	if ( SIN_Q_OVERFLOWED((Sin_event_queue*)ws->in_ws.input_queue) ) {
	    ERR_BUF( cph->erh, ERR256);
	}
	phg_sin_q_flush_ws( ws->in_ws.input_queue, ws->id );
    }
    (*ws->close)( ws );
}

void
phg_cpa_pm_set_filter( cph, cp_args, ws )
    Cp_handle	cph;
    Phg_args	*cp_args;
    Ws		*ws;
{
    Phg_args_set_filter	*args = &cp_args->data.set_filter;

    /* Only the pick filter is set in the PM for type A. */
    assure( args->type == PHG_ARGS_FLT_PICK )
    if ( args->type == PHG_ARGS_FLT_PICK ) {
	phg_wsx_set_name_set( ws, args->type, args->devid, &args->inc_set,
	    &args->exc_set );
	WS_FLUSH( ws );
    }
}

void
phg_cpa_pm_message( cph, cp_args, ws )
    Cp_handle	cph;
    Phg_args	*cp_args;
    Ws		*ws;
{
    if ( ws->message )
	(*ws->message)( ws, &cp_args->data.message );
}
