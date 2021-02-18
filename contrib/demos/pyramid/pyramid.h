/* $XConsortium: pyramid.h,v 5.2 91/05/06 11:24:06 hersh Exp $ */

/***********************************************************
Copyright (c) 1989-1991 by Sun Microsystems, Inc. and the X Consortium.

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

/*
 *	Pyramid - PHIGS PLUS demo program
 *
 *	Greg Schechter  ---  August, 1989
 *
 */

#include <phigs/phigs.h>
#include <stdio.h>
#include <math.h>
#include <xview/xview.h>
#include <xview/canvas.h>
#include <xview/panel.h>
#include <xview/textsw.h>
#define PHIGS_HLHSR_ID_ZBUFF PHIGS_HLHSR_ID_ON

#define toggle_bit_on(value, bit)  ((value) & (1 << (bit)))

#define WSID		    1
#define WSID2		    2

#define SINGLE_PYRAMID	    101
#define MAIN_PYRAMID	    102
#define PYR_LIGHTS_STRUCT   103
#define PYR_PROPS_STRUCT    104
#define SINGLE_GRID	    200
#define MAIN_GRID	    201
#define TEXT_ANNOTATION	    202
#define JUST_AXES	    203

#define POS_LIGHT_STRUCT    300
#define DIR_LIGHT_STRUCT    301
#define SPOT_LIGHT_STRUCT   302
#define LIGHT_STRUCT	    305

#define VIEW_1		    1

#define GLOBAL_TRANSFORM    201
#define ADD_LIGHTS_HERE	    202
#define ADD_PROPS_HERE	    203
#define ADD_INT_COLOR_HERE  204

#define GRID_LINES	    16

#define ROT_INIT_X_VALUE    10
#define ROT_INIT_Y_VALUE    305
#define ROT_INIT_Z_VALUE    350
#define ROT_MIN_VALUE	    0
#define ROT_MAX_VALUE	    360

#define SCALE_INIT_VALUE    6
#define SCALE_MIN_VALUE	    1
#define SCALE_MAX_VALUE	    20

#define VPN_INIT_X_VALUE    -232
#define VPN_INIT_Y_VALUE    235
#define VPN_INIT_Z_VALUE    302
#define VPN_MIN_VALUE	   -400
#define VPN_MAX_VALUE	    400

#define VUP_INIT_X_VALUE    0
#define VUP_INIT_Y_VALUE    10
#define VUP_INIT_Z_VALUE    0
#define VUP_MIN_VALUE	   -10
#define VUP_MAX_VALUE	    10

#define VRP_INIT_X_VALUE    5
#define VRP_INIT_Y_VALUE    5
#define VRP_INIT_Z_VALUE    5
#define VRP_MIN_VALUE	   -40
#define VRP_MAX_VALUE	    40

#define WIN_INIT_MIN_X_VAL  -80
#define WIN_INIT_MIN_Y_VAL  -80
#define WIN_INIT_MAX_X_VAL   80
#define WIN_INIT_MAX_Y_VAL   80
#define WIN_MIN_VALUE	    -400
#define WIN_MAX_VALUE	     400

#define VWP_INIT_MIN_X_VAL   0
#define VWP_INIT_MIN_Y_VAL   0
#define VWP_INIT_MIN_Z_VAL   0
#define VWP_INIT_MAX_X_VAL   100
#define VWP_INIT_MAX_Y_VAL   100
#define VWP_INIT_MAX_Z_VAL   100
#define VWP_MIN_VALUE	     0
#define VWP_MAX_VALUE	     100

#define PRP_INIT_X_VAL	     0
#define PRP_INIT_Y_VAL	     0
#define PRP_INIT_Z_VAL	     10
#define PRP_MIN_VALUE	    -40
#define PRP_MAX_VALUE	     40

#define PLN_INIT_FRONT_VAL   20
#define PLN_INIT_BACK_VAL   -20
#define PLN_INIT_VIEW_VAL    18
#define PLN_MIN_VALUE	    -60
#define PLN_MAX_VALUE	     60


#define POSITIONAL_1	    1
#define POSITIONAL_2	    2
#define POSITIONAL_3	    3
#define POSITIONAL_4	    4
#define POSITIONAL_5	    5
#define POSITIONAL_6	    6
#define AMBIENT		    2

/** Initial Surface Property Values **/
#define PROP_INIT_AMBIENT	0.5
#define PROP_INIT_DIFFUSE	1.0
#define PROP_INIT_SPECULAR	0.0
#define PROP_INIT_SPECULAR_EXP	1.0
#define PROP_INIT_TRANSPAR	1.0
#define PROP_INIT_DIFF_RED	1.0
#define PROP_INIT_DIFF_GRN	1.0
#define PROP_INIT_DIFF_BLU	1.0
#define PROP_INIT_SPEC_RED	1.0
#define PROP_INIT_SPEC_GRN	1.0
#define PROP_INIT_SPEC_BLU	1.0
    
extern Frame	    frame, panelFrame, xtra_frame;
extern Canvas	    canvas, subCanvas;
extern Textsw	    textsw;
extern Panel	    panel, light_panel, prop_panel;
extern Panel_item   pn_x_rotation, pn_y_rotation, pn_z_rotation, pn_scale;
extern Panel_item   pn_auto_rotate_toggle, pn_auto_rotate_button;
extern Panel_item   pn_vpn_x, pn_vpn_y, pn_vpn_z;
extern Panel_item   pn_vup_x, pn_vup_y, pn_vup_z;
extern Panel_item   pn_vrp_x, pn_vrp_y, pn_vrp_z;
extern Panel_item   pn_pyr_depth, pn_pyr_children, pn_pyr_random;
extern Panel_item   pn_pyr_probability, pn_build_button;
extern Panel_item   pn_cube_text, pn_cube_row[];
extern Panel_item   pn_pyr_text, pn_pyr_type;

extern Panel_item   pn_win_xmin, pn_win_xmax, pn_win_ymin, pn_win_ymax;
extern Panel_item   pn_viewp_xmin, pn_viewp_xmax, pn_viewp_ymin, pn_viewp_ymax, 
		    pn_viewp_zmin, pn_viewp_zmax;
extern Panel_item   pn_prp_x, pn_prp_y, pn_prp_z, pn_proj_type;
extern Panel_item   pn_pln_front, pn_pln_back, pn_pln_view;

extern Panel_item   lnum_item, lonoff_item, ltype_item, lcolr_item, lcolg_item, 
		    lcolb_item, ldir_item, ldirx_item, ldiry_item, ldirz_item, 
		    lpos_item, latten_item, lexp_item, lspread_item, 
		    ambcoef_item, diffcoef_item, speccoef_item, specexp_item, 
		    transpar_item, diffcol_item, diffcolr_item, diffcolg_item, 
		    diffcolb_item, speccol_item, speccolr_item, speccolg_item, 
		    speccolb_item;

extern float	    minSize;
extern float	    global_scale;
extern float	    global_x_ang;
extern float	    global_y_ang;
extern float	    global_z_ang;
extern int	    increaseScale;
extern int	    auto_on;

extern Pgcolr	    black, white, red, green, blue, yellow, cyan, magenta,
		    purple, taupe, ltblue, pink, ltyellow, periwinkle, 
		    peach, ltgreen;

extern void	    addLights();
extern void	    sliderRotateProc();
extern void	    autoRotateProc();
extern void	    sliderVPNProc();
extern void	    viewMapProc();
extern void	    mainObject();
extern void	    exitProc();

extern void	    light_num_proc();
extern void	    light_onoff_proc();
extern void	    light_type_proc();
extern void	    set_light_proc();
extern void	    set_prop_proc();

extern void	    make_lights_effective();
