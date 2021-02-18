#ifndef lint
static char     sccsid[] = "@(#)fb_util.c 1.1 91/09/07 FJ";
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

/* Utility functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"


void
phg_fb_copy_hierarchy( ret_hier, ipthsz, n, error_ind, ol, apthsz, paths )
Phg_ret_hierarchy       *ret_hier;	/* returned hierarchy */
Pint			ipthsz;		/* sizs of path buffer	*/
Pint			n;		/* element of the list of paths	*/
Pint			*error_ind;
Pint			*ol;		/* OUT number of paths	*/
Pint			*apthsz;	/* OUT size of Nth path	*/
Pint			*paths;		/* OUT Nth path	*/
{
    register	Pelem_ref	*retp;
    register	int		i;

    *error_ind = 0;
    retp = ret_hier->paths;
    *ol = ret_hier->counts.num_ints;
    if ( *ol > 0) {
        if ( n < 0 || n > *ol) {
            *error_ind = ERR2002;
        } else { 
            for ( i = 0; i < (n - 1); i++) {
                retp += ret_hier->counts.ints[i];
            }
            *apthsz = ret_hier->counts.ints[i];
            if ( ipthsz < *apthsz) {
                *error_ind = ERR2001;

            } else {
                for ( i = 0; i < *apthsz; i++) {
                    paths[i * 2] = retp[i].struct_id;
                    paths[i * 2 + 1] = retp[i].elem_pos;
                }
            }
        }
    }
}


int
phg_fb_colours_valid( count, colour_type, colr_ind, colr_val)
register	int	count;
		int	colour_type;
register	int	*colr_ind;
register	float	*colr_val;
{
    register	int	i;

    if ( colour_type == PINDCT) {
        for ( i = 0; i < count; i++, colr_ind++)
            if ( *colr_ind < 0)
	        return 0;
    } else {
	for ( i = 0; i < count * 3; i++, colr_val++)
	    if ( *colr_val < 0.0 || *colr_val > 1.0 )
	        return 0;
    }
    return 1;
}

static _Pws_id  *ws_link = (_Pws_id *)NULL;
static Pint     max_ws_type = 0;

Pint
phg_fb_set_ws_type_table()
{
    _Pws_id		*tool, *draw;

    if ( ( draw = (_Pws_id *)malloc( sizeof(_Pws_id) * 2))) {
        ws_link = draw;
        draw->next = tool = draw + 1;
        draw->ws_type = 1;
        draw->wst = (Wst *)phigs_ws_type_x_drawable;
        tool->next = (_Pws_id *)NULL;
        tool->ws_type = 0;
        tool->wst = (Wst *)phigs_ws_type_x_tool;
        max_ws_type = 1;
        return 1;
    }
    return 0; /* error 900 */
}

Pint
phg_fb_add_ws_type( wst, wtype)
Wst		*wst;          /* workstation type table pointer */
Pint	*wtype;        /* OUT FORTRAN workstation type */
{
    _Pws_id             *node;

    if ( !(node = (_Pws_id *)malloc( sizeof(_Pws_id) )) ) {
        return 0;

    } else {
        max_ws_type++;
        node->next = ws_link;
        *wtype = node->ws_type = max_ws_type;
        node->wst = wst;
        ws_link = node;
        return 1;
    }
}

Wst*
phg_fb_wst_exists( wtype)
Pint    wtype;          /* FORTRAN workstation type */
{
    Wst             *type = NULL;
    _Pws_id         *node;

    for ( node = ws_link; node; node = node->next) {
        if ( (node->ws_type == wtype)) {
            type = node->wst;
            return type;
        }
    }    
    return type;        /* workstation type not found */
}

Pint
phg_fb_inq_ws_type( wst)
Wst    *wst;          /* workstation type */
{
    Pint            wtype = -1;
    _Pws_id         *node;

    for ( node = ws_link; node; node = node->next) {
        if ( (node->wst == wst)) {
            wtype = node->ws_type;
            return wtype;
        }
    }    
    return wtype;        /* workstation type not found */
}


void
phg_fb_delete_ws_type( wtype)
Pint    wtype;
{
    _Pws_id **node, *del_id;

    for ( node = &ws_link; *node; node = &((*node)->next)) {
        if ( (*node)->ws_type == wtype) {
            del_id = *node;
            (*node) = (*node)->next;  /* remove from list. */
            free((char *)del_id);
        }
    }
}


void
phg_fb_delete_all_ws_type()
{
    _Pws_id	*node, *next;

    for ( node = ws_link; node; node = next) {
        next = node->next;
        free( (char *)node);
    }
}

static _Pdisplay_id    *display_link = (_Pdisplay_id *)NULL;
static _Pdisp_wind_id  *disp_wind_link = (_Pdisp_wind_id *)NULL;
static int count_disp_id = 1;
static int count_conn_id = 1;


/* DISPLAY IDENTIFIER CREATE */
void
pdisp_id_create( dyp, errind, dispid)
Display	*dyp;		/* display pointer	*/
int		*errind;	/* OUT error indicator	*/
int		*dispid;	/* OUT display identifier	*/
{
    _Pdisplay_id    *disp;

    if ( !(disp = (_Pdisplay_id *)calloc( 1, sizeof(_Pdisplay_id) )) ) {
        *errind = ERR900;

    } else {
        *errind = 0;
        *dispid = count_disp_id;
        disp->next = display_link;
        disp->disp_id = count_disp_id++;
        disp->disp_p = dyp;
        display_link = disp;
    }
}


/* CONNECTION IDENTIFIER CREATE */
void
pconn_id_create( dyp, wdid, errind, connid)
Display	*dyp;		/* display pointer	*/
Window	wdid;		/* window identifier	*/
int		*errind;	/* OUT error indicator	*/
int		*connid;	/* OUT connection identifier	*/
{
    _Pdisp_wind_id    *conn;

    if ( !(conn = (_Pdisp_wind_id *)calloc( 1, sizeof(_Pdisp_wind_id) )) ) {
        *errind = ERR900;

    } else {
        *errind = 0;
        *connid = count_conn_id;
        conn->next = disp_wind_link;
        conn->connid = count_conn_id++;
        conn->disp_p = dyp;
        conn->wind_id = wdid;
        disp_wind_link = conn;
    }
}


void
phg_fb_delete_all_display_id()
{
    _Pdisplay_id *node, *next;

    for ( node = display_link; node; node = next) {
        next = node->next;
        free( (char *)node );
    }
}


void
phg_fb_delete_all_conn_id()
{
    _Pdisp_wind_id *node, *next;

    for ( node = disp_wind_link; node; node = next) {
        next = node->next;
        free( (char *)node );
    }
}


Pint
phg_fb_inq_display_id( disp_p)
Display		*disp_p;		/* display identifier	*/
{
    _Pdisplay_id	*node;

    for ( node = display_link; node; node = node->next) {
        if ( node->disp_p == disp_p)
            return node->disp_id;
    }
    return -1;
}

F_dy_id
phg_fb_search_display( disp_id)
Pint	disp_id;		/* display identifier	*/
{
    _Pdisplay_id	*node;

    for ( node = display_link; node; node = node->next) {
        if ( node->disp_id == disp_id)
            return node;
    }
    return (F_dy_id)NULL;
}


Pint
phg_fb_inq_connection_id( conn_id)
Pconnid_x_drawable	*conn_id;		/* connection identifier	*/
{
    _Pdisp_wind_id	*node;

    for ( node = disp_wind_link; node; node = node->next) {
        if ( node->disp_p == conn_id->display &&
            node->wind_id == conn_id->drawable_id)
            return node->connid;
    }
    return -1;
}

F_dw_id
phg_fb_search_connection( conn_id)
Pint	conn_id;		/* connection identifier	*/
{
    _Pdisp_wind_id	*node;

    for ( node = disp_wind_link; node; node = node->next) {
        if ( node->connid == conn_id)
            return node;
    }
    return (F_dw_id)NULL;
}
