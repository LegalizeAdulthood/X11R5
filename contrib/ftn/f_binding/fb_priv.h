/*      @(#)fb_priv.h 1.1 91/01/07 FJ     */

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

#include "fb_mcr.h"
#include "enum.h"
#ifndef PHG_CB_INTERNAL_H_INCLUDED
#define PHG_CB_INTERNAL_H_INCLUDED

/*  The global CP handle only the binding functions use.  Defined in the
 *  open phigs module, cb_control.c
 */
extern Cp_handle	phg_cur_cph;

extern int		phg_cb_entry_check();
extern int		phg_cb_int_in_list();
extern Wst_phigs_dt	*phg_cb_check_set_rep();
extern void		phg_cb_copy_hierarchy();
extern void		phg_cb_format_err_msg();
extern int		phg_cb_wst_exists();
extern int		phg_cb_inp_assoc_devs();
extern void		phg_cb_ws_set_cursor();
extern Psl_ws_info*	phg_cb_ws_open();
extern void		phg_cb_inq_ws_type_support();
extern void		phg_cb_update_DC_size();
extern int		phg_colours_valid();
extern int              phg_cb_echo_limits_valid();

/* Max error message length includes terminator. */
#define CB_MAX_ERR_MSG_LENGTH	256

/* Names of the files used during error logging */
#define ERRMSG_FILE_NAME "PHIGSerr.txt"
#define FUNCNAME_FILE_NAME "PHIGSfnc.txt"

#define PHG_C_BINDING		0
#define PHG_FTN_BINDING		1

#define CB_PHIGS_OPEN( cph ) \
    ((cph) && (cph)->psl->phg_sys_state == PSYS_ST_PHOP)

#define CB_ENTRY_CHECK( cph, err, funcnum ) \
    phg_cb_entry_check( (cph), (err), (funcnum))

#define CB_BUILD_OC( _et, _ed, _oc ) \
    (phg_utx_build_pex_oc( phg_cur_cph->erh, (_et), (_ed), \
	&phg_cur_cph->scratch, (_oc) ))

#define CB_COLOUR_MODEL_SUPPORTED( colour ) \
    (((colour) == PINDCT) || ((colour) == PRGB))
    
#define CB_LINE_SHADING_SUPPORTED( shading ) \
    (((shading) == PSD_NONE) || ((shading) == PSD_COLOUR) || \
	((shading) == 0))

#define CB_INT_SHADING_SUPPORTED( shading ) \
    (((shading) == PSD_NONE) || ((shading) == PSD_COLOUR) || ((shading) == 0))

#define CB_REFL_EQ_SUPPORTED( eq ) \
    (((eq) >= PREFL_NONE) && ((eq) <= PREFL_AMB_DIFF_SPEC))

#define CB_LIGHT_SRC_TYPE_SUPPORTED( type ) \
    (((type) >= PLIGHT_AMBIENT) && ((type) <= PLIGHT_SPOT))

#define CB_MAT_3D_TO_2D_COPY( ma, mb) \
    mb[0][0] = ma[0][0]; \
    mb[0][1] = ma[0][1]; \
    mb[0][2] = ma[0][3]; \
    mb[1][0] = ma[1][0]; \
    mb[1][1] = ma[1][1]; \
    mb[1][2] = ma[1][3]; \
    mb[2][0] = ma[3][0]; \
    mb[2][1] = ma[3][1]; \
    mb[2][2] = ma[3][3];

#define CB_IN_RANGE( low, high, val) \
    ((val) >= (low) && (val) <= (high))

#define CB_ECHO_VOLUME_TO_AREA( _ev, _ea ) \
    (_ea).x_min = (_ev).x_min; (_ea).x_max = (_ev).x_max; \
    (_ea).y_min = (_ev).y_min; (_ea).y_max = (_ev).y_max


#define CB_ECHO_VOLUME_IN_RANGE( _ea, _dc ) \
    ( phg_cur_cph->flags.ignore_DC_errors || \
      (CB_IN_RANGE( 0.0, (_dc)[0], (_ea)->x_min) \
    && CB_IN_RANGE( 0.0, (_dc)[0], (_ea)->x_max) \
    && CB_IN_RANGE( 0.0, (_dc)[1], (_ea)->y_min) \
    && CB_IN_RANGE( 0.0, (_dc)[1], (_ea)->y_max) \
    && CB_IN_RANGE( 0.0, (_dc)[2], (_ea)->z_min) \
    && CB_IN_RANGE( 0.0, (_dc)[2], (_ea)->z_max)) )

#define CB_ECHO_AREA_IN_RANGE( _ea, _dc ) \
    ( phg_cur_cph->flags.ignore_DC_errors || \
      (CB_IN_RANGE( 0.0, (_dc)[0], (_ea)->x_min) \
    && CB_IN_RANGE( 0.0, (_dc)[0], (_ea)->x_max) \
    && CB_IN_RANGE( 0.0, (_dc)[1], (_ea)->y_min) \
    && CB_IN_RANGE( 0.0, (_dc)[1], (_ea)->y_max)) )

#define CB_ECHO_VOLUME_VALID( _ev ) \
    (   (_ev)->x_min < (_ev)->x_max \
     && (_ev)->y_min < (_ev)->y_max \
     && (_ev)->z_min <= (_ev)->z_max \
    )

#define CB_ECHO_AREA_TO_VOLUME( _ea, _ev ) \
    (_ev)->x_min = (_ea)->x_min; (_ev)->x_max = (_ea)->x_max; \
    (_ev)->y_min = (_ea)->y_min; (_ev)->y_max = (_ea)->y_max
/* No need to do the z, it stays the same in the ws. */

typedef struct {
    Ploc_data   drec;
    Pint_list   pets;
} Pstore_loc_info;

typedef struct {
    Ploc_data3  drec;
    Pint_list   pets;
} Pstore_loc_info3;

typedef struct {
    Pstroke_data        drec;
    Ppoint_list         init_stroke;
    Pint_list           pets;
} Pstore_stroke_info;

typedef struct {
    Pstroke_data3       drec;
    Ppoint_list3        init_stroke;
    Pint_list           pets;
} Pstore_stroke_info3;

typedef struct {
    Pval_data   drec;
    Pint_list   pets;
} Pstore_val_info;

typedef struct {
    Pval_data3  drec;
    Pint_list   pets;
} Pstore_val_info3;

typedef struct {
    Pchoice_data        drec;
    Pint_list           pets;
} Pstore_choice_info;

typedef struct {
    Pchoice_data3       drec;
    Pint_list           pets;
} Pstore_choice_info3;

typedef struct {
    Ppick_data  drec;
    Pfilter     filter;
    Ppick_path  init_pick;
    Pint_list   pets;
} Pstore_pick_info;

typedef struct {
    Ppick_data3 drec;
    Pfilter     filter;
    Ppick_path  init_pick;
    Pint_list   pets;
} Pstore_pick_info3;

typedef struct {
    Pstring_data        drec;
    Pint_list           pets;
} Pstore_string_info;

typedef struct {
    Pstring_data3       drec;
    Pint_list           pets;
} Pstore_string_info3;

typedef struct _Pstore {
    int			size;
    union {
            Ppat_rep                    pat_rep;
            Ppat_rep_plus               ext_pat_rep;
            Pstore_loc_info             loc_data;
            Pstore_loc_info3            loc_data3;
            Pstore_stroke_info          stroke_data;
            Pstore_stroke_info3         stroke_data3;
            Pstore_val_info             val_data;
            Pstore_val_info3            val_data3;
            Pstore_choice_info          choice_data;
            Pstore_choice_info3         choice_data3;
            Pstore_pick_info            pick_data;
            Pstore_pick_info3           pick_data3;
            Pstore_string_info          string_data;
            Pstore_string_info3         string_data3;
            Par_file_list               ar_files;
            Pfilter                     filter;
            Pelem_data                  elem_data;
            Pelem_ref_list_list         struct_paths;
            Pescape_out_data            escape_out_data;
            Pcolr_map_data              colr_map_rec;
    }                   data; /* all pointers are into "buf," below */
#ifdef __STDC__
    void		*buf;
#else
    char		*buf;
#endif
    struct _Pstore	*next;	/* for internal linking */
} _Pstore;

typedef struct {
    Pint	size;
    struct {
        Pint	il;	/* number of integer entries	*/
        Pint	*ia;	/* array containing integer entries	*/
        Pint	rl;	/* number of real entries	*/
        Pfloat	*ra;	/* array containing real entries	*/
        Pint	sl;	/* number of character string entries	*/
        Pint	*lstr;	/* length of character string entries	*/
        char	*str;	/* character string entries		*/
    } data;
#ifdef __STDC__
    void		*buf;
#else
    char		*buf;
#endif
} Pdata_rec;

#define CB_STORE_SPACE( _s, _sz, _err ) \
    ( (_s)->size >= (_sz) || phg_resize_store( (_s), (_sz), (_err) ) )

#endif

#define NUM_COLR_COMP	3

#define PMALLOC( _err, _type, _size, _point) \
    if ( !(_err) && (_size) > 0) { \
        if (!( (_point) = (_type *)malloc( sizeof(_type) * (_size) ))) { \
            (_err) = ERR900; \
        } \
    }

#define PFREE( _size, _point) \
    if ( _size > 0) { \
        if ( _point) { \
            free((char *) _point); \
        } \
    }


typedef struct _Pws_id {
    struct _Pws_id     *next;    /* for internal linking */
    Pint               ws_type;  /* workstation type for FORTRAN binding */
    Wst                *wst;     /* workstation type */
} _Pws_id;

typedef struct _Pdisplay_id {
	struct _Pdisplay_id    *next;    /* for internal linking */
	Pint                  disp_id;   /* display id for FORTRAN binding */
	Display               *disp_p;   /* display id */
} _Pdisplay_id;

typedef struct _Pdisp_wind_id {
	struct _Pdisp_wind_id  *next;   /* for internal linking */
	Pint                   connid;  /* connection id for FORTRAN binding */
	Display                *disp_p; /* display id   */
	Window                 wind_id; /* window id    */
} _Pdisp_wind_id;

typedef struct _Pws_id         *F_ws_id;
typedef struct _Pdisplay_id    *F_dy_id;
typedef struct _Pdisp_wind_id  *F_dw_id;

/* use for FORTRAN binding */
extern Wst				*phg_fb_wst_exists();
extern int              phg_fb_colours_valid();
extern void             phg_fb_copy_hierarchy();
extern Pint             fb_set_ws_type_table();
extern Pint             phg_fb_create_ws_type();
extern Pint             phg_fb_inq_ws_type();
extern void             phg_fb_delete_ws_type();
extern void             phg_fb_delete_all_ws_type();
extern void             phg_fb_delete_all_display_id();
extern void             phg_fb_delete_all_conn_id();
extern void             phg_fb_open_phigs();
extern F_dy_id          phg_fb_search_display();
extern F_dw_id          phg_fb_search_connection();

#define UNSUPPORTED_FUNCTION( _cph, _fn_id) \
  { \
    if ( CB_ENTRY_CHECK((_cph), ERR2, (_fn_id))) { \
        ERR_REPORT((_cph)->erh, ERRN152); \
    } \
    return; \
  }

#define UNSUPPORTED_INQ_FUNCTION( _cph, _errind) \
  { \
    if ( !CB_ENTRY_CHECK((_cph), 0 ,Pfn_INQUIRY)) { \
        (_errind) = ERR2; \
    } else { \
        (_errind) = ERRN152; \
    } \
    return; \
  }

#define GDP3_ID_SUPPORTED( _gdp3_id) \
    (((_gdp3_id) == PGDP3_POLYLINE_SET) || \
     ((_gdp3_id) == PGDP3_ELLIPSE) || \
     ((_gdp3_id) == PGDP3_ELLIPSE_ARC_EP) || \
     ((_gdp3_id) == PGDP3_ELLIPSE_ARC_CLOSE_EP) || \
     ((_gdp3_id) == PGDP3_DISJOINT_POLYLINE) || \
     ((_gdp3_id) == PGDP3_FILLAREA_SET_HOLLOW))

#define GDP_ID_SUPPORTED( _gdp_id) \
    (((_gdp_id) == PGDP_ELLIPSE) || \
     ((_gdp_id) == PGDP_CIRCLE_ARC_CLOSE_EP))


#define GSE_ID_SUPPORTED( _gse_id) \
    (((_gse_id) == PGSE_POLYLINE_SP_ATTR) || \
     ((_gse_id) == PGSE_EDGE_SP_ATTR) || \
     ((_gse_id) == PGSE_LOCAL_INPUT_INDEX))
