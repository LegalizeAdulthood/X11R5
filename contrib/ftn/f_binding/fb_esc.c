#ifndef lint
static char     sccsid[] = "@(#)fb_esc.c 1.1 91/09/07 FJ";
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

/* Escape functions for the PHIGS FORTRAN binding */

#include "phg.h"
#include "cp.h"
#include "fb_priv.h"

#ifdef EXT_INPUT
/* check whether specified input device number is valid */
#define DEVICE_EXISTS( _idt, _class, _num) \
   ((_num) > 0 && ((_num) <= (_idt)->num_devs._class))
#define CB_WS_CATEGORY(_wsinfo) \
   (((Wst *)_wsinfo->wstype)->desc_tbl.phigs_dt.ws_category)
#endif

/* SET ERROR SYNCRONIZATION MODE */
static void
fb_esc_u1( cph, lidr, idr)
Cp_handle	cph;
Pint	lidr;
char	*idr;
{
    Phg_args        cp_args;
    int		    err = 0;
    Pdata_rec	    rec;
   
    /* Error synchronization */
    if ( CB_ENTRY_CHECK( cph, ERR2, Pfn_escape)) {
        if ( fb_unpack_datrec( lidr, idr, &err, &rec)) {
            ERR_REPORT( cph->erh, err);
        
        } else if ( rec.data.il != 1) {
            ERR_REPORT( cph->erh, ERR351);

        } else if ( rec.data.ia[0] < PESOFF || rec.data.ia[0] > PESON) {
            ERR_REPORT( cph->erh, ERR2000);

        } else {
            cp_args.data.idata = rec.data.ia[0];
            CP_FUNC( cph, CP_FUNC_OP_ERROR_SYNC, &cp_args, (Phg_ret *)NULL );
        }
        fb_del_datrec( &rec);
    }
}

static void
esc_u2( cph, in, store )
    Cp_handle           cph;
    Pescape_in_data     *in;
    _Pstore             *store;
{
    Phg_args            args;
    Phg_ret             ret;
    int                 size;

#define OUT store->data.escape_out_data.escape_out_u2

    if (phg_cb_ws_open(cph, in->escape_in_u2.ws_id, Pfn_escape)) {
        args.data.idata = in->escape_in_u2.ws_id;
        CP_FUNC( cph, CP_FUNC_OP_XX_INQ_DPY_AND_DRAWABLE, &args, &ret );
        if ( !(OUT.err_ind = ret.err) ) {
            OUT.display = ret.data.dpy_and_drawable.display;
            OUT.drawable_id = ret.data.dpy_and_drawable.drawable_id;
            OUT.input_overlay_id = ret.data.dpy_and_drawable.overlay_id;
            size = strlen( ret.data.dpy_and_drawable.display_name ) + 1;
            if ( CB_STORE_SPACE( store, size, &OUT.err_ind ) ) {
                OUT.display_name = store->buf;
                strcpy( OUT.display_name,
                    ret.data.dpy_and_drawable.display_name );
            }
        }
    }
#undef OUT
}

/* SET DC BOUNDS CHECK INDICATOR */
static void
fb_esc_u3( cph, lidr, idr)
Cp_handle	cph;
Pint	lidr;
char	*idr;
{
    Phg_args        cp_args;
    int             err = 0;
    Pdata_rec	    rec;
   
    /* Disable/Enable detection of DC errors. */
    if ( CB_ENTRY_CHECK( cph, ERR2, Pfn_escape)) {
        if ( fb_unpack_datrec( lidr, idr, &err, &rec)) {
            ERR_REPORT( cph->erh, err);
        
        } else if ( rec.data.il != 1) {
            ERR_REPORT( cph->erh, ERR351);

        } else {
            cph->flags.ignore_DC_errors = rec.data.ia[0] ? 1 : 0;
        }
        fb_del_datrec( &rec);
    }
}

static void
esc_u4( cph, in, store )
    Cp_handle           cph;
    Pescape_in_data     *in;
    _Pstore             *store;
{
    Phg_args            cp_args;
    Phg_ret             ret;
    Psl_ws_info         *wsinfo;
    Wst_phigs_dt        *dt;
    Pint                size;

#define OUT store->data.escape_out_data.escape_out_u4

    register Phg_args_drawable_pick     *args = &cp_args.data.drawable_pick;

    OUT.status = PIN_STATUS_NO_IN;
    OUT.pick.depth = 0;

    if ( !(wsinfo = phg_cb_ws_open(cph, in->escape_in_u2.ws_id, Pfn_escape)) )
        return;

    dt = &wsinfo->wstype->desc_tbl.phigs_dt;
    if ( !phg_cb_echo_limits_valid( cph, Pfn_escape, wsinfo->wsid,
            &in->escape_in_u4.echo_volume, dt ) ) {
        /* Error has been reported, just fall out of the if. */
        return;
    }

    args->wsid = in->escape_in_u4.ws_id;
    args->point = in->escape_in_u4.point;
    args->ap_size = in->escape_in_u4.ap_size;
    args->order = in->escape_in_u4.order;
    args->pet = in->escape_in_u4.pet;
    args->esw = in->escape_in_u4.echo_switch;
    args->echo_volume = in->escape_in_u4.echo_volume;
    args->filter = in->escape_in_u4.filter;
    CP_FUNC( cph, CP_FUNC_OP_DRAWABLE_PICK, &cp_args, &ret );

    if ( ret.err ) {
        /* Report errors immediately so user doesn't try to read garbage.  */
        ERR_FLUSH( phg_cur_cph->erh);

    } else {
        OUT.status = ret.data.drawable_pick.status;
        if ( OUT.status == PIN_STATUS_OK ) {
            OUT.pick.depth = MIN(in->escape_in_u4.depth,
                                 ret.data.drawable_pick.pick.depth);
            size = OUT.pick.depth * sizeof(Ppick_path_elem);
            if ( CB_STORE_SPACE( store, size, &ret.err ) ) {
                OUT.pick.path_list = (Ppick_path_elem *)store->buf;
                bcopy( (char *)ret.data.drawable_pick.pick.path_list,
                       (char *)OUT.pick.path_list, size );
            } else {
                ERR_REPORT( cph->erh, ret.err );
            }
        }
    }
#undef OUT
}

static void
esc_u5( cph, in, store )
    Cp_handle           cph;
    Pescape_in_data     *in;
    _Pstore             *store;
{
    Phg_args            cp_args;
    Phg_ret             ret;
    Pint                size;

#define OUT store->data.escape_out_data.escape_out_u5

    register Phg_args_map_points        *args = &cp_args.data.map_points;

    if ( !phg_cb_ws_open(cph, in->escape_in_u5.ws_id, Pfn_escape) )
        return;

    args->wsid = in->escape_in_u5.ws_id;
    args->points = in->escape_in_u5.points;
    CP_FUNC( cph, CP_FUNC_OP_MAP_POINTS, &cp_args, &ret );

    if ( ret.err ) {
        /* Report errors immediately so user doesn't try to read garbage.  */
        ERR_FLUSH( phg_cur_cph->erh);
        OUT.points.num_points = 0;
    } else {
        OUT.view_index = ret.data.map_points.view_index;
        OUT.points.num_points = ret.data.map_points.points.num_points;
        if ( OUT.points.num_points > 0 ) {
            size = OUT.points.num_points * sizeof(Ppoint3);
            if ( CB_STORE_SPACE( store, size, &ret.err ) ) {
                OUT.points.points = (Ppoint3 *)store->buf;
                bcopy( (char *)ret.data.map_points.points.points,
                       (char *)OUT.points.points, size );
            } else {
                ERR_REPORT( cph->erh, ret.err);
            }
        }
    }
#undef OUT
}

static void
esc_u6( cph, in )
    Cp_handle           cph;
    Pescape_in_data     *in;
{
    Phg_args            cp_args;

    register Phg_args_redraw_regions    *args = &cp_args.data.redraw_regions;

    if ( !phg_cb_ws_open(cph, in->escape_in_u6.ws_id, Pfn_escape) )
        return;

    args->wsid = in->escape_in_u6.ws_id;
    args->num_regions = in->escape_in_u6.num_regions;
    args->regions = in->escape_in_u6.regions;
    CP_FUNC( cph, CP_FUNC_OP_REDRAW_REGIONS, &cp_args, (Phg_ret *)NULL );
}

static void
esc_u7( cph, in )
    Cp_handle           cph;
    Pescape_in_data     *in;
{
    Phg_args            cp_args;

    if ( !phg_cb_ws_open(cph, in->escape_in_u7.ws_id, Pfn_escape) )
        return;

    cp_args.data.idata = in->escape_in_u7.ws_id;
    CP_FUNC( cph, CP_FUNC_OP_WS_SYNCH, &cp_args, (Phg_ret *)NULL );
}

#ifdef EXT_JAPK
static void
fb_esc_u101( cph, lidr, idr)
Cp_handle	cph;
Pint	lidr;
char	*idr;
{
    Phg_args        cp_args;
    int		    err = 0;
    Pdata_rec	    rec;
   
    if ( CB_ENTRY_CHECK( cph, ERR2, Pfn_escape)) {
        if ( fb_unpack_datrec( lidr, idr, &err, &rec)) {
            ERR_REPORT( cph->erh, err);
        
        } else if ( rec.data.il != 1) {
            ERR_REPORT( cph->erh, ERR351);

        } else {
            cph->psl->output_char_set = rec.data.ia[0];

        }
        fb_del_datrec( &rec);
    }
}
#endif

#ifdef EXT_INPUT
static Wst_input_wsdt *
fb_outin_ws_open( cph, ws, dtp)
Cp_handle       cph;
Pint            ws;
Wst_phigs_dt    **dtp;  /* non-null if the whole dt is also needed */
{
    Psl_ws_info     *wsinfo;
    Wst_input_wsdt  *idt = NULL;
 
    if ( PSL_WS_STATE( cph->psl) != PWS_ST_WSOP) {
        ERR_REPORT( cph->erh, ERR3);

    } else if ( !(wsinfo = phg_psl_get_ws_info( cph->psl, ws))) {
        ERR_REPORT( cph->erh, ERR54);

    } else if ( CB_WS_CATEGORY( wsinfo) != PCAT_OUTIN ) {
        ERR_REPORT( cph->erh, ERR60);

    } else {
        idt = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt.in_dt;
        if ( dtp)
            *dtp = &((Wst*)wsinfo->wstype)->desc_tbl.phigs_dt;
    }
    return idt;
}

/* PEX lib functions                                                        */
/*   fb_esc_u109()        : OPEN LOCAL OPERATION function                   */
/*   fb_esc_u110()        : CLOSE LOCAL OPERATION function                  */
/*   fb_esc_u111()        : STATICAL TRANSFORMATION 3 function              */
/*   fb_esc_u112()        : VALIABLE TRANSFORMATION 3 function              */
/*   fb_esc_u113()        : ROTATE TRANSFORMATION 3 function                */
/*   fb_esc_u114()        : SET LOCAL VIEW TRANSFORMATION function          */
/*   fb_esc_u115()        : RESET LOCAL VIEW TRANSFORMATION function        */
/*   fb_esc_u116()        : SET LOCAL INPUT VALUE VALID function            */
/*   phg_cpx_loin_req()   : calling phg_cpx_loin_req()                      */
/*   phg_cpc_class_loin() : calling phg_cpc_class_B() or phg_cpc_class_C()  */
 
/* fb_esc_u109 : OPEN LOCAL OPERATION */
static void
fb_esc_u109( cph, lidr, idr) 
Cp_handle	cph;
Pint	lidr;
char	*idr;
{
    Wst_input_wsdt                   *idt;
    Wst_phigs_dt                     *dt;
    Phg_args                         cp_args;
    register Phg_args_loin           *args = &cp_args.data.local_inp;
    register Phg_args_loin_open_ope
             *u109_args = &args->class_data.loin_open_ope;
    int			err = 0;
    Pdata_rec		rec;

    /* unpack data record */
    if ( CB_ENTRY_CHECK( cph, ERR3, Pfn_escape)) {
        if ( fb_unpack_datrec( lidr, idr, &err, &rec)) {
            ERR_REPORT( cph->erh, err);

        } else if ( rec.data.il != 4) {
            ERR_REPORT( cph->erh, ERR351);

        } else {
            /* check function requested states */
            if ( idt = fb_outin_ws_open( cph, rec.data.ia[0], &dt)) {
     
                /* check whether local input transformation is available */
                if ( idt->num_local_input_index_entries <= 0) {
                    /* local input transformation is not available */
                    ERR_REPORT( cph->erh, ERRN820);
     
                /* check enumeration value (local input index type & create type) */
                } else if ( rec.data.ia[1] < PLCMOD ||
                            rec.data.ia[1] > PLCVIW ||
                            rec.data.ia[3] < PLCACC ||
                            rec.data.ia[3] > PLCGEN) {
                    /* enumeration value is invalid */
                    ERR_REPORT( cph->erh, ERR2000);
         
                /* check local transformation index */
                } else if ( rec.data.ia[2] < 1) {
                    /* local input transformation index is invalid */
                    ERR_REPORT( cph->erh, ERRN821);
     
                /* check local input index for modeling */
                } else if (( rec.data.ia[1] == PMODEL) &&
                    ( rec.data.ia[2] <= 0 ||
                      rec.data.ia[2] >= idt->num_local_input_index_entries )) {
                    /* local input transformation index is invalid for modelling */
                    ERR_REPORT( cph->erh, ERRN822);
     
                } else {
                    /* set up Phg_args_loin table */
                    args->wsid = rec.data.ia[0];
                    args->loin_class = PHG_ARGS_LOIN_OPEN_OPE;
                    u109_args->loin_index_type = rec.data.ia[1];
                    u109_args->loin_tran_index = rec.data.ia[2];
                    u109_args->loin_cre_type = rec.data.ia[3];
         
                    /* call phg_cpx_loin_req() by CP_FUNC */
                    CP_FUNC( cph, CP_FUNC_OP_ESCAPE, &cp_args, NULL );
                }
            }
        }
        fb_del_datrec( &rec);
    }
}

/* fb_esc_u110 : CLOSE LOCAL OPERATION */
static void
fb_esc_u110( cph, lidr, idr) 
Cp_handle	cph;
Pint	lidr;
char	*idr;
{
    Wst_input_wsdt              *idt;
    Wst_phigs_dt                *dt;
    Phg_args                    cp_args;
    register Phg_args_loin      *args = &cp_args.data.local_inp;
    Pdata_rec           rec;
    int		     	err = 0;
 
    /* unpack data record */
    if ( CB_ENTRY_CHECK( cph, ERR3, Pfn_escape)) {
        if ( fb_unpack_datrec( lidr, idr, &err, &rec)) {
            ERR_REPORT( cph->erh, err);

        } else if ( rec.data.il != 1) {
            ERR_REPORT( cph->erh, ERR351);

        } else {
            /* check function requested states */
            if ( idt = fb_outin_ws_open( cph, rec.data.ia[0], &dt)) {
     
                /* check whether local input transformation is available */
                if ( idt->num_local_input_index_entries <= 0) {
                /* local input transformation is not available */
                    ERR_REPORT( cph->erh, ERRN820);
     
                } else {
                /* set up Phg_args_loin table */
                    args->wsid = rec.data.ia[0];
                    args->loin_class = PHG_ARGS_LOIN_CLOSE_OPE;
         
                    /* call phg_cpx_loin_req() by CP_FUNC */
                    CP_FUNC( cph, CP_FUNC_OP_ESCAPE, &cp_args, NULL );
                }
            }
        }
        fb_del_datrec( &rec);
    }
}

/* fb_esc_u111 : STATICAL TRANSFORMATION 3 */
static void
fb_esc_u111( cph, lidr, idr) 
Cp_handle	cph;
Pint	lidr;
char	*idr;
{
    Wst_input_wsdt                        *idt;
    Wst_phigs_dt                          *dt;
    Phg_args                              cp_args;
    register Phg_args_loin                *args = &cp_args.data.local_inp;
    register Phg_args_loin_static_tran3
             *u111_args = &args->class_data.loin_static_tran3;
    Pmatrix3		m;
    Pdata_rec           rec;
    int			err = 0;
    
    if ( CB_ENTRY_CHECK( cph, ERR3, Pfn_escape)) {
        if ( fb_unpack_datrec( lidr, idr, &err, &rec)) {
        ERR_REPORT( cph->erh, err);

        } else if ( rec.data.il != 1 || rec.data.rl != 16) {
            ERR_REPORT( cph->erh, ERR351);

        } else {
            /* check function requested states */
            if ( idt = fb_outin_ws_open( cph, rec.data.ia[0], &dt)) {
     
                /* check whether local input transformation is available */
                if ( idt->num_local_input_index_entries <= 0 ) {
                    /* local input transformation is not available */
                    ERR_REPORT( cph->erh, ERRN820);
           
                } else {
                    /* set up Phg_args_loin table */
                    args->wsid = rec.data.ia[0];
                    args->loin_class = PHG_ARGS_LOIN_STATIC_TRAN3;
                    bcopy( (char *)rec.data.ra, (char *)m, (sizeof(Pfloat) *16));
                    MATRIX_DATA_TRANSLATE( 3, m, u111_args->static_tran3);
                    /* call phg_cpx_loin_req() by CP_FUNC */
                    CP_FUNC( cph, CP_FUNC_OP_ESCAPE, &cp_args, NULL ) ;
                }
            }
        }
        fb_del_datrec( &rec);
    }
}

/* fb_esc_u112 : VALIABLE TRANSFORMATION 3 */
static void
fb_esc_u112( cph, lidr, idr) 
Cp_handle	cph;
Pint	lidr;
char	*idr;
{
    Wst_input_wsdt                    *idt;
    Wst_phigs_dt                      *dt;
    Phg_args                          cp_args;
    register Phg_args_loin            *args = &cp_args.data.local_inp;
    register Phg_args_loin_valiable_tran3
             *u112_args = &args->class_data.loin_valiable_tran3;
    Pint                mi[4][4];
    Pmatrix3            mf;
    Pint                *ints;
    Pdata_rec           rec;
    int                 err = 0;
    int                 i;

    if ( CB_ENTRY_CHECK( cph, ERR3, Pfn_escape)) {
        if ( fb_unpack_datrec( lidr, idr, &err, &rec)) {
            ERR_REPORT( cph->erh, err);

        } else if ( rec.data.il != 18 || rec.data.rl != 16) {
            ERR_REPORT( cph->erh, ERR351);

        } else {
            /* check function requested states */
            if ( idt = fb_outin_ws_open( cph, rec.data.ia[0], &dt)) { 

                /* check whether local input transformation is available */
                if ( idt->num_local_input_index_entries <= 0 ) {
                    /* local input transformation is not available */
                    ERR_REPORT( cph->erh, ERRN820);
         
                /* check enumeration value (conflation type) */
                } else if ( rec.data.ia[1] < PLCABU ||
                            rec.data.ia[1] > PLCPRU) {
                    /* enumeration value is invalid */
                    ERR_REPORT( cph->erh, ERR2000);
         
                } else {
                    /* check input device number */
                    ints = &(rec.data.ia[2]);
                    for ( i = 0; i < 16; i++ ) {
                        if ( !DEVICE_EXISTS( idt, val, ints[i] ) &&
					         ints[i] != 0 ) {
                            /* input device number is invalid */
                            ERR_REPORT( cph->erh, ERR250);
                            fb_del_datrec( &rec);
                            return;
                        }
                    }
                    /* set up Phg_args_loin table */
                    args->wsid = rec.data.ia[0];
                    args->loin_class = PHG_ARGS_LOIN_VALIABLE_TRAN3;
                    u112_args->conf_type = rec.data.ia[1];
                    bcopy( (char *)rec.data.ra, (char *)mf, (sizeof(Pfloat) *16));
                    MATRIX_DATA_TRANSLATE( 3, mf, u112_args->coe_mat3);
                    bcopy( (char *)ints, (char *)mi, (sizeof(Pint) *16));
                    MATRIX_DATA_TRANSLATE( 3, mi, u112_args->idev_mat3);
      
                    /* call phg_cpx_loin_req() by CP_FUNC */
                    CP_FUNC( cph, CP_FUNC_OP_ESCAPE, &cp_args, NULL );
                }     
            }
        }
        fb_del_datrec( &rec);
    }
}

/* fb_esc_u113 : ROTATE TRANSFORMATION 3 */
static void
fb_esc_u113( cph, lidr, idr) 
Cp_handle	cph;
Pint	lidr;
char	*idr;
{
    Wst_input_wsdt                       *idt;
    Wst_phigs_dt                         *dt;
    Phg_args                             cp_args;
    register Phg_args_loin               *args = &cp_args.data.local_inp;
    register Phg_args_loin_rotate_tran3
             *u113_args = &args->class_data.loin_rotate_tran3;
    Pdata_rec           rec;
    int			err = 0;

    if ( CB_ENTRY_CHECK( cph, ERR3, Pfn_escape)) {
        /* unpack data record */
        if ( fb_unpack_datrec( lidr, idr, &err, &rec)) {
            ERR_REPORT( cph->erh, err);

        } else if ( rec.data.il != 4 || rec.data.rl !=1) {
            ERR_REPORT( cph->erh, ERR351);

        } else {
            /* check function requested states */
            if ( idt = fb_outin_ws_open( cph, rec.data.ia[0], &dt)) {
     
                /* check whether local input transformation is available */
                if ( idt->num_local_input_index_entries <= 0 ) {
                    /* local input transformation is not available */
                    ERR_REPORT( cph->erh, ERRN820);
 
                /* check enumeration value (conflation type and axis value) */
                } else if ( rec.data.ia[1] < PLCABU ||
                            rec.data.ia[1] > PLCPRU ||
                            rec.data.ia[2] < PLCFIR ||
                            rec.data.ia[2] > PLCTHI) {
                    /* enumeration value is invalid */
                    ERR_REPORT( cph->erh, ERR2000);
 
                /* check input device number */
                } else if ( !DEVICE_EXISTS( idt, val, rec.data.ia[3] )) {
                    /* input device number is invalid */
                    ERR_REPORT( cph->erh, ERR250);
 
                } else {
                    /* set up Phg_args_loin table */
                    args->wsid = rec.data.ia[0];
                    args->loin_class = PHG_ARGS_LOIN_ROTATE_TRAN3;
                    u113_args->conf_type = rec.data.ia[1];
                    u113_args->axis = rec.data.ia[2];
                    u113_args->coeficient = rec.data.ra[0];
                    u113_args->idev = rec.data.ia[3];
     
                    /* call phg_cpx_loin_req() by CP_FUNC */
                    CP_FUNC( cph, CP_FUNC_OP_ESCAPE, &cp_args, NULL ) ;
                }
            }
        }
        fb_del_datrec( &rec);
    }
}

/* fb_esc_u114 : SET LOCAL VIEW TRANSFORMATION */
static void
fb_esc_u114( cph, lidr, idr) 
Cp_handle	cph;
Pint	lidr;
char	*idr;
{
    Wst_input_wsdt                        *idt;
    Wst_phigs_dt                          *dt;
    Phg_args                               cp_args;
    register Phg_args_loin                *args = &cp_args.data.local_inp;
    register Phg_args_loin_set_view_tran
             *u114_args = &args->class_data.loin_set_view_tran;
    Pdata_rec           rec;
    int			err = 0;
    
    if ( CB_ENTRY_CHECK( cph, ERR3, Pfn_escape)) {
        if ( fb_unpack_datrec( lidr, idr, &err, &rec)) {
            ERR_REPORT( cph->erh, err);

        } else if ( rec.data.il != 4) {
            ERR_REPORT( cph->erh, ERR351);

        } else {
            /* check function requested states */
            if ( idt = fb_outin_ws_open( cph, rec.data.ia[0], &dt)) { 
                /* check whether local input transformation is available */
                if ( idt->num_local_input_index_entries <= 0 ) {
                    /* local input transformation is not available */
                    ERR_REPORT( cph->erh, ERRN820 );
         
                /* check view index */
                } else if ( rec.data.ia[1] < 1 ) {
                    /* view index is invalid */
                    ERR_REPORT( cph->erh, ERR115);
         
                } else if ( rec.data.ia[1] >= dt->num_view_indices ) {
                    /* view index is invalid */
                    ERR_REPORT( cph->erh, ERR150);
         
                /* check local transformation index */
                } else if ( rec.data.ia[2] < 1 ) {
                    /* local input transformation index is invalid */
                    ERR_REPORT( cph->erh, ERRN821);
     
                /* check effect mode */
                } else if ( rec.data.ia[3] < PNPC || rec.data.ia[3] > PVRC ) {
                    /* enumeration value is invalid */
                    ERR_REPORT( cph->erh, ERR2000);
         
                } else {
                    /* set up Phg_args_loin table */
                    args->wsid = rec.data.ia[0];
                    args->loin_class = PHG_ARGS_LOIN_SET_VIEW_TRAN;
                    u114_args->view_index = rec.data.ia[1];
                    u114_args->loin_tran_index = rec.data.ia[2];
                    u114_args->effect_mode = rec.data.ia[3];
         
                    /* call phg_cpx_loin_req() by CP_FUNC */
                    CP_FUNC( cph, CP_FUNC_OP_ESCAPE, &cp_args, NULL );
                }
            }
        }
        fb_del_datrec( &rec);
    }
}

/* fb_esc_u115 : RESET LOCAL VIEW TRANSFORMATION */
static void
fb_esc_u115( cph, lidr, idr) 
Cp_handle	cph;
Pint	lidr;
char	*idr;
{
    Wst_input_wsdt                          *idt;
    Wst_phigs_dt                            *dt;
    Phg_args                                cp_args;
    register Phg_args_loin                  *args = &cp_args.data.local_inp;
    register Phg_args_loin_reset_view_tran
             *u115_args = &args->class_data.loin_reset_view_tran; 
    Pdata_rec           rec;
    int			err = 0;
    
    if ( CB_ENTRY_CHECK( cph, ERR3, Pfn_escape)) {
        if ( fb_unpack_datrec( lidr, idr, &err, &rec)) {
            ERR_REPORT( cph->erh, err);
    
        } else if ( rec.data.il != 2) {
            ERR_REPORT( cph->erh, ERR351);
    
        } else {
            /* check function requested states */
            if ( idt = fb_outin_ws_open( cph, rec.data.ia[0], &dt)) {
     
                /* check whether local input transformation is available */
                if ( idt->num_local_input_index_entries <= 0 ) {
                    /* local input transformation index is invalid */
                    ERR_REPORT( cph->erh, ERRN820);
         
                /* check view index */
                } else if ( rec.data.ia[1] < 1 ) {
                    /* view index is invalid */
                    ERR_REPORT( cph->erh, ERR115 );
         
                } else if ( rec.data.ia[1] >= dt->num_view_indices ) {
                    /* view index is invalid */
                    ERR_REPORT( cph->erh, ERR150 );
    
                } else {
                    /* set up Phg_args_loin table */
                    args->wsid = rec.data.ia[0];
                    args->loin_class = PHG_ARGS_LOIN_RESET_VIEW_TRAN;
                    u115_args->view_index = rec.data.ia[1];
           
                    /* call phg_cpx_loin_req by CP_FUNC */
                    CP_FUNC( cph, CP_FUNC_OP_ESCAPE, &cp_args, NULL );
                }
            }
        }
        fb_del_datrec( &rec);
    }
}

/* fb_esc_u116 : SET LOCAL INPUT VALUE VALID */
static void
fb_esc_u116( cph, lidr, idr) 
Cp_handle	cph;
Pint	lidr;
char	*idr;
{
    Wst_input_wsdt                    *idt;
    Wst_phigs_dt                      *dt;
    Phg_args                          cp_args;
    register Phg_args_loin            *args = &cp_args.data.local_inp;
    register Phg_args_loin_set_valid
             *u116_args = &args->class_data.loin_set_valid;
    Pint		*ints;
    Pdata_rec           rec;
    int			err = 0;
    int                 i;
    
    if ( CB_ENTRY_CHECK( cph, ERR3, Pfn_escape)) {
        if ( fb_unpack_datrec( lidr, idr, &err, &rec)) {
            ERR_REPORT( cph->erh, err);
    
        } else if ( rec.data.il < 3) {
            ERR_REPORT( cph->erh, ERR351);
    
        } else {
            /* check function requested states */
            if ( idt = fb_outin_ws_open( cph, rec.data.ia[0], &dt)) {
     
                /* check whether local input transformation is available */
                if ( idt->num_local_input_index_entries <= 0 ) {
                    /* local input transformation index is invalid */
                    ERR_REPORT( cph->erh, ERRN820);
     
                /* check reference mode */
                } else if ( rec.data.ia[1] < PINVAL || rec.data.ia[1] > PVAL) {
                    /* emumeration value is invalid */
                    ERR_REPORT( cph->erh, ERR2000);
         
                } else if ( rec.data.ia[2] < 0 ) {
                    /* number of local transformation index is invalid */
                    ERR_REPORT( cph->erh, ERRN823);
         
                } else {
                    ints = &(rec.data.ia[3]);
                    for( i = 0; i < rec.data.ia[2]; i++ ) {
                        if ( ints[i] < 1 ) {
                            /* local input transformation index is invalid */
                            ERR_REPORT( cph->erh, ERRN821);
                            fb_del_datrec( &rec);
                            return;
                         }
                    }
                    /* set up Phg_args_loin table */
                    args->wsid = rec.data.ia[0];
                    args->loin_class = PHG_ARGS_LOIN_SET_VALID;
                    u116_args->ref_mode = rec.data.ia[1];
                    u116_args->index_sets.num_ints = rec.data.ia[2];
                    u116_args->index_sets.ints = ints;
         
                    /* call phg_cpx_loin_req by CP_FUNC */
                    CP_FUNC( cph, CP_FUNC_OP_ESCAPE, &cp_args, NULL );
                }
            }
        }
        fb_del_datrec( &rec);
    }
}
#endif

        
/* ESCAPE */
void
pesc_( fctid, lidr, idr, mlodr, lodr, odr)
Pint	*fctid;		/* function identification	*/
Pint	*lidr;		/* dimension of input data record array	*/
char	*idr;		/* input data record array	*/
Pint	*mlodr;		/* MAX length of output data record	*/
Pint	*lodr;		/* OUT dimension of output data record array	*/
char	*odr;		/* OUT output data record array	*/
{
    switch ( *fctid) {
    case PUESC_ERRSYNC:
        fb_esc_u1( phg_cur_cph, *lidr, idr);
        break;

    case PUESC_IGNORE_DC_ERRORS:
        fb_esc_u3( phg_cur_cph, *lidr, idr);
        break;

    /* not suppoted
    case PUESC_DPYINFO:
        esc_u2( phg_cur_cph, in, (_Pstore *)store );
        *out = &((_Pstore *)store)->data.escape_out_data;
        break;

    case PUESC_DRAWABLE_POINT_TO_PICK:
        esc_u4( phg_cur_cph, in, (_Pstore *)store );
        *out = &((_Pstore *)store)->data.escape_out_data;
        break;

    case PUESC_DRAWABLE_POINTS_TO_WC:
        esc_u5( phg_cur_cph, in, (_Pstore *)store );
        *out = &((_Pstore *)store)->data.escape_out_data;
        break;

    case PUESC_REDRAW_REGIONS:
        esc_u6( phg_cur_cph, in );
        break;

    case PUESC_WS_SYNCH:
        esc_u7( phg_cur_cph, in );
        break;
    */
#ifdef EXT_JAPK
    case PUESC_OUTPUT_CHAR_SET:
        fb_esc_u101( phg_cur_cph, *lidr, idr);
        break;
#endif

#ifdef EXT_INPUT
    case PUESC_OPEN_LOCAL_OPE:
        fb_esc_u109( phg_cur_cph, *lidr, idr);
        break;

    case PUESC_CLOSE_LOCAL_OPE:
        fb_esc_u110( phg_cur_cph, *lidr, idr);
        break;

    case PUESC_STATICAL_TRAN3:
        fb_esc_u111( phg_cur_cph, *lidr, idr);
        break;

    case PUESC_VALIABLE_TRAN3:
        fb_esc_u112( phg_cur_cph, *lidr, idr);
        break;

    case PUESC_ROTATE_TRAN3:
        fb_esc_u113( phg_cur_cph, *lidr, idr);
        break;

    case PUESC_SET_VIEW_TRAN:
        fb_esc_u114( phg_cur_cph, *lidr, idr);
        break;

    case PUESC_RESET_VIEW_TRAN:
        fb_esc_u115( phg_cur_cph, *lidr, idr);
        break;

    case PUESC_VALUE_VALID:
        fb_esc_u116( phg_cur_cph, *lidr, idr);
        break;
#endif

    default:
        if ( CB_ENTRY_CHECK( phg_cur_cph, ERR2, Pfn_escape))
            ERR_REPORT( phg_cur_cph->erh, ERR350);
        break;
    }
}
