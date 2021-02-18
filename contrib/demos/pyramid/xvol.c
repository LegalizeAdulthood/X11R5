/* $XConsortium: xvol.c,v 5.1 91/02/16 09:33:01 rws Exp $ */

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
 *	Greg Schechter  ---  August, 1989
 *
 *      (XView/Open Look conversion by Rich Berlin -- February, 1990)
 *
 */

#include "pyramid.h"
#include "pm_ui.h"
int INSTANCE;

static short icon_image[] = {
#   include "pyramid.icon"    
};

Frame subFrame;
Icon pyramid_icon;
/*
 * Data from DevGuide.
 */
static pm_pyr_frame_objects		*pm_pyr_frame;
static pm_light_sources_frame_objects	*pm_light_sources_frame;
static pm_rot_panel_frame_objects	*pm_rot_panel_frame;
static pm_prop_panel_frame_objects	*pm_prop_panel_frame;
static pm_light_panel_frame_objects	*pm_light_panel_frame;
static pm_orient_panel_frame_objects	*pm_orient_panel_frame;
static pm_viewmap_panel_frame_objects	*pm_viewmap_panel_frame;
static pm_object_panel_frame_objects	*pm_object_panel_frame;

static
repaint_subcanvas()
{
    predraw_all_structs(WSID2, PFLAG_ALWAYS);
}

static
repaint_main_canvas()
{
    predraw_all_structs(WSID, PFLAG_ALWAYS);
}

setupWindow(argc, argv)
int argc;
char *argv[];
{
    void set_item_values();

    (void) xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);
    INSTANCE = xv_unique_key();

    /*
     * Create DevGuide objects
     */
    pm_pyr_frame = pm_pyr_frame_objects_initialize(NULL, NULL);
    pm_light_sources_frame =
	pm_light_sources_frame_objects_initialize(NULL, pm_pyr_frame->pyr_frame);
    pm_rot_panel_frame =
	pm_rot_panel_frame_objects_initialize(NULL, pm_pyr_frame->pyr_frame);
    pm_prop_panel_frame =
	pm_prop_panel_frame_objects_initialize(NULL, pm_pyr_frame->pyr_frame);
    pm_light_panel_frame =
	pm_light_panel_frame_objects_initialize(NULL, pm_pyr_frame->pyr_frame);
    pm_orient_panel_frame =
	pm_orient_panel_frame_objects_initialize(NULL, pm_pyr_frame->pyr_frame);
    pm_viewmap_panel_frame =
	pm_viewmap_panel_frame_objects_initialize(NULL, pm_pyr_frame->pyr_frame);
    pm_object_panel_frame =
	pm_object_panel_frame_objects_initialize(NULL, pm_pyr_frame->pyr_frame);

    /*
     * Alias needed frame objects
     */
    frame = pm_pyr_frame->pyr_frame;
    canvas = pm_pyr_frame->canvas;
    subFrame = pm_light_sources_frame->light_sources_frame;
    subCanvas = xv_create(subFrame, CANVAS, 
			  XV_SHOW, TRUE, 
			  XV_X, 0, XV_Y, 0,
			  XV_WIDTH, 300,
			  XV_HEIGHT, 300,
			  CANVAS_AUTO_CLEAR, FALSE,
			  CANVAS_REPAINT_PROC, repaint_subcanvas,
			  WIN_DYNAMIC_VISUAL, TRUE,

			  0);

    xv_set(canvas, CANVAS_AUTO_CLEAR, FALSE, 
	           CANVAS_REPAINT_PROC, repaint_main_canvas, 0);

    /*
     * Set panel item defaults and aliases
     */
    set_item_values();

    /*
     * Make main windows visible
     */
    xv_set(frame, WIN_SHOW, TRUE, 0);
    xv_set(pm_light_sources_frame->light_sources_frame, WIN_SHOW, TRUE, 0);
}


/*
 * For compatibility, the actual items created are aliased to these.
 */
Panel_item   lnum_item, lonoff_item, ltype_item, lcolr_item, lcolg_item, 
		    lcolb_item, ldir_item, ldirx_item, ldiry_item, ldirz_item, 
		    lpos_item, latten_item, lexp_item, lspread_item, 
		    ambcoef_item, diffcoef_item, speccoef_item, specexp_item, 
		    transpar_item, diffcol_item, diffcolr_item, diffcolg_item, 
		    diffcolb_item, speccol_item, speccolr_item, speccolg_item, 
		    speccolb_item;

void
set_item_values()
{
    xv_set(pn_x_rotation = pm_rot_panel_frame->pn_x_rotation,
	   PANEL_VALUE,	       ROT_INIT_X_VALUE,
	   PANEL_MIN_VALUE,    ROT_MIN_VALUE,
	   PANEL_MAX_VALUE,    ROT_MAX_VALUE,
	   PANEL_NOTIFY_LEVEL, PANEL_ALL,
	   0);

    xv_set(pn_y_rotation = pm_rot_panel_frame->pn_y_rotation,
	PANEL_VALUE,	    ROT_INIT_Y_VALUE,
	PANEL_MIN_VALUE,    ROT_MIN_VALUE,
	PANEL_MAX_VALUE,    ROT_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);	   

    xv_set(pn_z_rotation = pm_rot_panel_frame->pn_z_rotation,
	PANEL_VALUE,	    ROT_INIT_Z_VALUE,
	PANEL_MIN_VALUE,    ROT_MIN_VALUE,
	PANEL_MAX_VALUE,    ROT_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);

    xv_set(pn_scale = pm_rot_panel_frame->pn_scale,
	PANEL_VALUE,	    SCALE_INIT_VALUE,
	PANEL_MIN_VALUE,    SCALE_MIN_VALUE,
	PANEL_MAX_VALUE,    SCALE_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
     
    pn_auto_rotate_toggle = pm_rot_panel_frame->pn_auto_rotate_toggle;
    pn_pyr_type = pm_object_panel_frame->pn_pyr_type;

    xv_set(pn_pyr_depth = pm_object_panel_frame->pn_pyr_depth,
	PANEL_VALUE,	    2,
	0);

    xv_set(pn_cube_row[0] = pm_object_panel_frame->pn_cube_row0,
	PANEL_TOGGLE_VALUE,	0, TRUE, 
	PANEL_TOGGLE_VALUE,	2, TRUE, 
	PANEL_TOGGLE_VALUE,	6, TRUE, 
	PANEL_TOGGLE_VALUE,	8, TRUE, 
	XV_SHOW,	FALSE,
	0);

    xv_set(pn_cube_row[1] = pm_object_panel_frame->pn_cube_row1,
	PANEL_TOGGLE_VALUE,	4, TRUE, 
	XV_SHOW,	FALSE,
	0);

    xv_set(pn_cube_row[2] = pm_object_panel_frame->pn_cube_row2,
	PANEL_TOGGLE_VALUE,	0, TRUE, 
	PANEL_TOGGLE_VALUE,	2, TRUE, 
	PANEL_TOGGLE_VALUE,	6, TRUE, 
	PANEL_TOGGLE_VALUE,	8, TRUE, 
	XV_SHOW,	FALSE,
	0);

    xv_set(pn_pyr_children = pm_object_panel_frame->pn_pyr_children,
	PANEL_TOGGLE_VALUE,	0, TRUE,
	PANEL_TOGGLE_VALUE,	1, TRUE,
	PANEL_TOGGLE_VALUE,	2, TRUE,
	PANEL_TOGGLE_VALUE,	3, TRUE,
	PANEL_TOGGLE_VALUE,	4, TRUE,
	0);

    xv_set(pn_pyr_random = pm_object_panel_frame->pn_pyr_random,
	PANEL_TOGGLE_VALUE,	0, FALSE,
	PANEL_TOGGLE_VALUE,	1, FALSE,
	PANEL_TOGGLE_VALUE,	2, FALSE,
	PANEL_TOGGLE_VALUE,	3, FALSE,
	PANEL_TOGGLE_VALUE,	4, FALSE,
	0);

    xv_set(pn_pyr_probability = pm_object_panel_frame->pn_pyr_probability,
	PANEL_VALUE,	    50,
	PANEL_MIN_VALUE,    0,
	PANEL_MAX_VALUE,    100,
	0);

    pn_build_button = pm_object_panel_frame->pn_build_button;

    lnum_item = pm_light_panel_frame->lnum_item;
    lonoff_item = pm_light_panel_frame->lonoff_item;
    ltype_item = pm_light_panel_frame->ltype_item,
    xv_set(ltype_item, PANEL_CHOICE_STRINGS,
	   "Ambient", "Directional", "Positional", 0, 0);

    xv_set(lcolr_item = pm_light_panel_frame->lcolr_item,
        PANEL_MIN_VALUE,                0,
        PANEL_MAX_VALUE,                100,
        PANEL_SLIDER_WIDTH,             60,
        0);
    xv_set(lcolg_item = pm_light_panel_frame->lcolg_item,
        PANEL_MIN_VALUE,                0,
        PANEL_MAX_VALUE,                100,
        PANEL_SLIDER_WIDTH,             60,
        0);
    xv_set(lcolb_item = pm_light_panel_frame->lcolb_item,
        PANEL_MIN_VALUE,                0,
        PANEL_MAX_VALUE,                100,
        PANEL_SLIDER_WIDTH,             60,
        0);
    ldir_item = pm_light_panel_frame->ldir_item;

    xv_set(ldirx_item = pm_light_panel_frame->ldirx_item,
        PANEL_MIN_VALUE,                -10,
        PANEL_MAX_VALUE,                10,
        0);
    xv_set(ldiry_item = pm_light_panel_frame->ldiry_item,
        PANEL_MIN_VALUE,                -10,
        PANEL_MAX_VALUE,                10,
        0);
    xv_set(ldirz_item = pm_light_panel_frame->ldirz_item,
        PANEL_MIN_VALUE,                -10,
        PANEL_MAX_VALUE,                10,
        0);
    
    lpos_item = pm_light_panel_frame->lpos_item;
    latten_item = pm_light_panel_frame->latten_item;
    lexp_item = pm_light_panel_frame->lexp_item;
    lspread_item = pm_light_panel_frame->lspread_item;

    xv_set(ambcoef_item = pm_prop_panel_frame->ambcoef_item,
        PANEL_MIN_VALUE,                0,
        PANEL_MAX_VALUE,                100,
	PANEL_VALUE,			(int)(PROP_INIT_AMBIENT*100),
        0);
    xv_set(diffcoef_item = pm_prop_panel_frame->diffcoef_item,
        PANEL_MIN_VALUE,                0,
        PANEL_MAX_VALUE,                100,
	PANEL_VALUE,			(int)(PROP_INIT_DIFFUSE*100),
        0);
    xv_set(speccoef_item = pm_prop_panel_frame->speccoef_item,
        PANEL_MIN_VALUE,                0,
        PANEL_MAX_VALUE,                100,
	PANEL_VALUE,			(int)(PROP_INIT_SPECULAR*100),
        0);
    xv_set(specexp_item = pm_prop_panel_frame->specexp_item,
        PANEL_MIN_VALUE,                0,
        PANEL_MAX_VALUE,                100,
	PANEL_VALUE,			(int)(PROP_INIT_SPECULAR_EXP*100),
        0);
    xv_set(transpar_item = pm_prop_panel_frame->transpar_item,
        PANEL_MIN_VALUE,                0,
        PANEL_MAX_VALUE,                100,
	PANEL_VALUE,			(int)(PROP_INIT_TRANSPAR*100),
        0);
    xv_set(diffcolr_item = pm_prop_panel_frame->diffcolr_item,
        PANEL_MIN_VALUE,                0,
        PANEL_MAX_VALUE,                100,
	PANEL_VALUE,			(int)(PROP_INIT_DIFF_RED*100),
        0);
    xv_set(diffcolg_item = pm_prop_panel_frame->diffcolg_item,
        PANEL_MIN_VALUE,                0,
        PANEL_MAX_VALUE,                100,
	PANEL_VALUE,			(int)(PROP_INIT_DIFF_GRN*100),
        0);
    xv_set(diffcolb_item = pm_prop_panel_frame->diffcolb_item,
        PANEL_MIN_VALUE,                0,
        PANEL_MAX_VALUE,                100,
	PANEL_VALUE,			(int)(PROP_INIT_DIFF_BLU*100),
        0);
    xv_set(speccolr_item = pm_prop_panel_frame->speccolr_item,
        PANEL_MIN_VALUE,                0,
        PANEL_MAX_VALUE,                100,
	PANEL_VALUE,			(int)(PROP_INIT_SPEC_RED*100),
        0);
    xv_set(speccolg_item = pm_prop_panel_frame->speccolg_item,
        PANEL_MIN_VALUE,                0,
        PANEL_MAX_VALUE,                100,
        PANEL_SLIDER_WIDTH,             60,
        0);
    xv_set(speccolb_item = pm_prop_panel_frame->speccolb_item,
        PANEL_MIN_VALUE,                0,
        PANEL_MAX_VALUE,                100,
	PANEL_VALUE,			(int)(PROP_INIT_SPEC_BLU*100),
        0);


    xv_set(pn_vpn_x = pm_orient_panel_frame->pn_vpn_x,
	PANEL_VALUE,	    VPN_INIT_X_VALUE,
	PANEL_MIN_VALUE,    VPN_MIN_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	PANEL_MAX_VALUE,    VPN_MAX_VALUE,
	0);	   
    xv_set(pn_vpn_y = pm_orient_panel_frame->pn_vpn_y,
	PANEL_VALUE,	    VPN_INIT_Y_VALUE,
	PANEL_MIN_VALUE,    VPN_MIN_VALUE,
	PANEL_MAX_VALUE,    VPN_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);	   
    xv_set(pn_vpn_z = pm_orient_panel_frame->pn_vpn_z,
	PANEL_VALUE,	    VPN_INIT_Z_VALUE,
	PANEL_MIN_VALUE,    VPN_MIN_VALUE,
	PANEL_MAX_VALUE,    VPN_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_vup_x = pm_orient_panel_frame->pn_vup_x,
	PANEL_VALUE,	    VUP_INIT_X_VALUE,
	PANEL_MIN_VALUE,    VUP_MIN_VALUE,
	PANEL_MAX_VALUE,    VUP_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);	   
    xv_set(pn_vup_y = pm_orient_panel_frame->pn_vup_y,
	PANEL_VALUE,	    VUP_INIT_Y_VALUE,
	PANEL_MIN_VALUE,    VUP_MIN_VALUE,
	PANEL_MAX_VALUE,    VUP_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);	   
    xv_set(pn_vup_z = pm_orient_panel_frame->pn_vup_z,
	PANEL_VALUE,	    VUP_INIT_Z_VALUE,
	PANEL_MIN_VALUE,    VUP_MIN_VALUE,
	PANEL_MAX_VALUE,    VUP_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);	   
    xv_set(pn_vrp_x = pm_orient_panel_frame->pn_vrp_x,
	PANEL_VALUE,	    VRP_INIT_X_VALUE,
	PANEL_MIN_VALUE,    VRP_MIN_VALUE,
	PANEL_MAX_VALUE,    VRP_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_vrp_y = pm_orient_panel_frame->pn_vrp_y,
	PANEL_VALUE,	    VRP_INIT_Y_VALUE,
	PANEL_MIN_VALUE,    VRP_MIN_VALUE,
	PANEL_MAX_VALUE,    VRP_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_vrp_z = pm_orient_panel_frame->pn_vrp_z,
	PANEL_VALUE,	    VRP_INIT_Z_VALUE,
	PANEL_MIN_VALUE,    VRP_MIN_VALUE,
	PANEL_MAX_VALUE,    VRP_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);


    xv_set(pn_win_xmin = pm_viewmap_panel_frame->pn_win_xmin,
	PANEL_VALUE,	    WIN_INIT_MIN_X_VAL,
	PANEL_MIN_VALUE,    WIN_MIN_VALUE,
	PANEL_MAX_VALUE,    WIN_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_win_xmax = pm_viewmap_panel_frame->pn_win_xmax,
	PANEL_VALUE,	    WIN_INIT_MAX_X_VAL,
	PANEL_MIN_VALUE,    WIN_MIN_VALUE,
	PANEL_MAX_VALUE,    WIN_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_win_ymin = pm_viewmap_panel_frame->pn_win_ymin,
	PANEL_VALUE,	    WIN_INIT_MIN_Y_VAL,
	PANEL_MIN_VALUE,    WIN_MIN_VALUE,
	PANEL_MAX_VALUE,    WIN_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_win_ymax = pm_viewmap_panel_frame->pn_win_ymax,
	PANEL_VALUE,	    WIN_INIT_MAX_Y_VAL,
	PANEL_MIN_VALUE,    WIN_MIN_VALUE,
	PANEL_MAX_VALUE,    WIN_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_viewp_xmin = pm_viewmap_panel_frame->pn_viewp_xmin,
	PANEL_VALUE,	    VWP_INIT_MIN_X_VAL,
	PANEL_MIN_VALUE,    VWP_MIN_VALUE,
	PANEL_MAX_VALUE,    VWP_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_viewp_xmax = pm_viewmap_panel_frame->pn_viewp_xmax,
	PANEL_VALUE,	    VWP_INIT_MAX_X_VAL,
	PANEL_MIN_VALUE,    VWP_MIN_VALUE,
	PANEL_MAX_VALUE,    VWP_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_viewp_ymin = pm_viewmap_panel_frame->pn_viewp_ymin,
	PANEL_VALUE,	    VWP_INIT_MIN_Y_VAL,
	PANEL_MIN_VALUE,    VWP_MIN_VALUE,
	PANEL_MAX_VALUE,    VWP_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_viewp_ymax = pm_viewmap_panel_frame->pn_viewp_ymax,
	PANEL_VALUE,	    VWP_INIT_MAX_Y_VAL,
	PANEL_MIN_VALUE,    VWP_MIN_VALUE,
	PANEL_MAX_VALUE,    VWP_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_viewp_zmin = pm_viewmap_panel_frame->pn_viewp_zmin,
	PANEL_VALUE,	    VWP_INIT_MIN_Z_VAL,
	PANEL_MIN_VALUE,    VWP_MIN_VALUE,
	PANEL_MAX_VALUE,    VWP_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_viewp_zmax = pm_viewmap_panel_frame->pn_viewp_zmax,
	PANEL_VALUE,	    VWP_INIT_MAX_Z_VAL,
	PANEL_MIN_VALUE,    VWP_MIN_VALUE,
	PANEL_MAX_VALUE,    VWP_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_prp_x = pm_viewmap_panel_frame->pn_prp_x,
	PANEL_VALUE,	    PRP_INIT_X_VAL,
	PANEL_MIN_VALUE,    PRP_MIN_VALUE,
	PANEL_MAX_VALUE,    PRP_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_prp_y = pm_viewmap_panel_frame->pn_prp_y,
	PANEL_VALUE,	    PRP_INIT_Y_VAL,
	PANEL_MIN_VALUE,    PRP_MIN_VALUE,
	PANEL_MAX_VALUE,    PRP_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_prp_z = pm_viewmap_panel_frame->pn_prp_z,
	PANEL_VALUE,	    PRP_INIT_Z_VAL,
	PANEL_MIN_VALUE,    PRP_MIN_VALUE,
	PANEL_MAX_VALUE,    PRP_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    pn_proj_type = pm_viewmap_panel_frame->pn_proj_type;
    xv_set(pn_pln_front = pm_viewmap_panel_frame->pn_pln_front,
	PANEL_VALUE,	    PLN_INIT_FRONT_VAL,
	PANEL_MIN_VALUE,    PLN_MIN_VALUE,
	PANEL_MAX_VALUE,    PLN_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_pln_back = pm_viewmap_panel_frame->pn_pln_back,
	PANEL_VALUE,	    PLN_INIT_BACK_VAL,
	PANEL_MIN_VALUE,    PLN_MIN_VALUE,
	PANEL_MAX_VALUE,    PLN_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
    xv_set(pn_pln_view = pm_viewmap_panel_frame->pn_pln_view,
	PANEL_VALUE,	    PLN_INIT_VIEW_VAL,
	PANEL_MIN_VALUE,    PLN_MIN_VALUE,
	PANEL_MAX_VALUE,    PLN_MAX_VALUE,
	PANEL_NOTIFY_LEVEL, PANEL_ALL,
	0);
}


void
light_type_proc(item)
Panel_item item;
{
    switch((int) xv_get(ltype_item, PANEL_VALUE)) {
	case 0:
	    xv_set(ldir_item, PANEL_INACTIVE, TRUE, 0);
	    xv_set(ldirx_item, PANEL_INACTIVE, TRUE, 0);
	    xv_set(ldiry_item, PANEL_INACTIVE, TRUE, 0);
	    xv_set(ldirz_item, PANEL_INACTIVE, TRUE, 0);
	    xv_set(lpos_item, PANEL_INACTIVE, TRUE, 0);
	    xv_set(latten_item, PANEL_INACTIVE, TRUE, 0);
	    xv_set(lexp_item, PANEL_INACTIVE, TRUE, 0);
	    xv_set(lspread_item, PANEL_INACTIVE, TRUE, 0);
	    break;
	case 1:
	    xv_set(ldir_item, PANEL_INACTIVE, FALSE, 0);
	    xv_set(ldirx_item, PANEL_INACTIVE, FALSE, 0);
	    xv_set(ldiry_item, PANEL_INACTIVE, FALSE, 0);
	    xv_set(ldirz_item, PANEL_INACTIVE, FALSE, 0);
	    xv_set(lpos_item, PANEL_INACTIVE, TRUE, 0);
	    xv_set(latten_item, PANEL_INACTIVE, TRUE, 0);
	    xv_set(lexp_item, PANEL_INACTIVE, TRUE, 0);
	    xv_set(lspread_item, PANEL_INACTIVE, TRUE, 0);
	    break;
	case 2:
	    xv_set(ldir_item, PANEL_INACTIVE, TRUE, 0);
	    xv_set(ldirx_item, PANEL_INACTIVE, TRUE, 0);
	    xv_set(ldiry_item, PANEL_INACTIVE, TRUE, 0);
	    xv_set(ldirz_item, PANEL_INACTIVE, TRUE, 0);
	    xv_set(lpos_item, PANEL_INACTIVE, FALSE, 0);
	    xv_set(latten_item, PANEL_INACTIVE, FALSE, 0);
	    xv_set(lexp_item, PANEL_INACTIVE, TRUE, 0);
	    xv_set(lspread_item, PANEL_INACTIVE, TRUE, 0);
	    break;
	case 3:
	    xv_set(ldir_item, PANEL_INACTIVE, FALSE, 0);
	    xv_set(ldirx_item, PANEL_INACTIVE, FALSE, 0);
	    xv_set(ldiry_item, PANEL_INACTIVE, FALSE, 0);
	    xv_set(ldirz_item, PANEL_INACTIVE, FALSE, 0);
	    xv_set(lpos_item, PANEL_INACTIVE, FALSE, 0);
	    xv_set(latten_item, PANEL_INACTIVE, FALSE, 0);
	    xv_set(lexp_item, PANEL_INACTIVE, FALSE, 0);
	    xv_set(lspread_item, PANEL_INACTIVE, FALSE, 0);
	    break;
	default:
	    break;
    }
    
    if (item)
	set_light_proc();
   
}


objTypeProc(item, value, event)
Panel_item  item;
int	    value;
Event	   *event;
{
    switch (value) {
	case 0 :
	    /** Pyramid **/
	    xv_set(pn_cube_row[0],     XV_SHOW, FALSE, 0);
	    xv_set(pn_cube_row[1],     XV_SHOW, FALSE, 0);
	    xv_set(pn_cube_row[2],     XV_SHOW, FALSE, 0);
	    xv_set(pn_pyr_children,    XV_SHOW, TRUE, 0);
	    xv_set(pn_pyr_random,      XV_SHOW, TRUE, 0);
	    xv_set(pn_pyr_probability, XV_SHOW, TRUE, 0);
	    break;

	case 1 :
	    /** Cube **/
	    xv_set(pn_pyr_children,    XV_SHOW, FALSE, 0);
	    xv_set(pn_pyr_random,      XV_SHOW, FALSE, 0);
	    xv_set(pn_pyr_probability, XV_SHOW, FALSE, 0);
	    xv_set(pn_cube_row[0],     XV_SHOW, TRUE, 0);
	    xv_set(pn_cube_row[1],     XV_SHOW, TRUE, 0);
	    xv_set(pn_cube_row[2],     XV_SHOW, TRUE, 0);
	    break;
    }
}



/*
 * Notify function for each item in a pinnable menu.
 */
void
pinned_menu_notify(menu, item)
	Menu		menu;
	Menu_item	item;
{
	void	(*menu_notify)() = (void (*)()) xv_get(menu, MENU_GEN_PROC);
	void	(*item_notify)() = (void (*)()) xv_get(item, MENU_GEN_PROC);
	
	if (xv_get(xv_get(menu, MENU_PIN_WINDOW), XV_SHOW)) {
		if (menu_notify)
			(*menu_notify)(menu, MENU_NOTIFY);
		if (item_notify)
			(*item_notify)(item, MENU_NOTIFY);
		if (item_notify)
			(*item_notify)(item, MENU_NOTIFY_DONE);
		if (menu_notify)
			(*menu_notify)(menu, MENU_NOTIFY_DONE);
	}
}

/*
 * Menu handler for `view_menu (Hide Grid)'.
 */
Menu_item
toggle_grid(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	int on;
	char *str;

	pm_pyr_frame_objects * ip =
	    (pm_pyr_frame_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);


	
	switch (op) {
	case MENU_DISPLAY:
	case MENU_DISPLAY_DONE:
	case MENU_NOTIFY_DONE:
		break;

	case MENU_NOTIFY:
		str = (char *) xv_get(item, MENU_STRING, 0);
		on = (strcmp(str, "Hide Grid") == 0) ? 1 : 0;
		on = !on;
		xv_set(item, MENU_STRING, (on ? "Hide Grid" : "Show Grid"), 0);
		gridProc(NULL, on, NULL);
		break;
	}
	return item;
}

/*
 * Menu handler for `view_menu (Show Light Sources)'.
 */
Menu_item
pu_light_sources(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	pm_pyr_frame_objects * ip =
	    (pm_pyr_frame_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
	case MENU_DISPLAY_DONE:
	case MENU_NOTIFY_DONE:
		break;

	case MENU_NOTIFY:
		xv_set(pm_light_sources_frame->light_sources_frame,
		           FRAME_CMD_PUSHPIN_IN, TRUE, 
		           XV_SHOW, TRUE,
		       0);
		break;
	}
	return item;
}

/*
 * Menu handler for `view_menu (Rotation and Scaling...)'.
 */
Menu_item
pu_rot_panel(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	pm_pyr_frame_objects * ip =
	    (pm_pyr_frame_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
	case MENU_DISPLAY_DONE:
	case MENU_NOTIFY_DONE:
		break;

	case MENU_NOTIFY:
		xv_set(pm_rot_panel_frame->rot_panel_frame,
		           FRAME_CMD_PUSHPIN_IN, TRUE, 
		           XV_SHOW, TRUE,
		       0);
		break;
	}
	return item;
}

/*
 * Menu handler for `view_menu (Lighting....)'.
 */
Menu_item
pu_light_panel(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	pm_pyr_frame_objects * ip = (pm_pyr_frame_objects *)
	    xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
	case MENU_DISPLAY_DONE:
	case MENU_NOTIFY_DONE:
		break;

	case MENU_NOTIFY:
		xv_set(pm_light_panel_frame->light_panel_frame,
		           FRAME_CMD_PUSHPIN_IN, TRUE, 
		           XV_SHOW, TRUE,
		       0);
		break;

	}
	return item;
}

/*
 * Menu handler for `view_menu (View Orientation...)'.
 */
Menu_item
pu_orient_panel(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	pm_pyr_frame_objects * ip =
	    (pm_pyr_frame_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
	case MENU_DISPLAY_DONE:
	case MENU_NOTIFY_DONE:
		break;

	case MENU_NOTIFY:
		xv_set(pm_orient_panel_frame->orient_panel_frame,
		           FRAME_CMD_PUSHPIN_IN, TRUE, 
		           XV_SHOW, TRUE,
		       0);
		break;
	}
	return item;
}

/*
 * Menu handler for `view_menu (View Mapping...)'.
 */
Menu_item
pu_viewmap_panel(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	pm_pyr_frame_objects * ip =
	    (pm_pyr_frame_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
	case MENU_DISPLAY_DONE:
	case MENU_NOTIFY_DONE:
		break;

	case MENU_NOTIFY:
		xv_set(pm_viewmap_panel_frame->viewmap_panel_frame,
		           FRAME_CMD_PUSHPIN_IN, TRUE, 
		           XV_SHOW, TRUE,
		       0);
		break;
	}
	return item;
}

/*
 * Menu handler for `edit_menu (Build Settings...)'.
 */
Menu_item
pu_object_panel(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	pm_pyr_frame_objects * ip =
	    (pm_pyr_frame_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
	case MENU_DISPLAY_DONE:
	case MENU_NOTIFY_DONE:
	    break;

	case MENU_NOTIFY:
		xv_set(pm_object_panel_frame->object_panel_frame,
		           FRAME_CMD_PUSHPIN_IN, TRUE, 
		           XV_SHOW, TRUE,
		       0);
		break;
	}
	return item;
}

/*
 * Menu handler for `props_menu (Surface Properties...)'.
 */
Menu_item
pu_prop_panel(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	pm_pyr_frame_objects * ip = (pm_pyr_frame_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
	case MENU_DISPLAY_DONE:
	case MENU_NOTIFY_DONE:
		break;

	case MENU_NOTIFY:
		xv_set(pm_prop_panel_frame->prop_panel_frame,
		           FRAME_CMD_PUSHPIN_IN, TRUE, 
		           XV_SHOW, TRUE,
		       0);
		fputs("pm: pu_prop_panel: MENU_NOTIFY\n", stderr);
		break;
	}
	return item;
}

/*
 * Notify callback function for `lrbutton'.
 */
void
rs_light_panel(item, event)
	Panel_item	item;
	Event		*event;
{
	pm_light_panel_frame_objects	*ip =
	    (pm_light_panel_frame_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	fputs("pm: don't know how to reset light panel yet.\n", stderr);
}

/*
 * Notify callback function for `pn_pyr_reset'.
 */
void
rs_object_panel(item, event)
	Panel_item	item;
	Event		*event;
{
	pm_object_panel_frame_objects	*ip = (pm_object_panel_frame_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	fputs("pm: don't know how to reset object panel yet.\n", stderr);
}


/*
 * Notify callback function for `pn_auto_rotate_toggle'.
 */
void
select_auto_rotate(item, value, event)
	Panel_item	item;
	unsigned int	value;
	Event		*event;
{
	pm_rot_panel_frame_objects *ip = 
	    (pm_rot_panel_frame_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);

	unsigned rotateToggle = (unsigned)
	    panel_get_value(ip->pn_auto_rotate_toggle);

#ifdef USE_ITIMER_FOR_ROTATE
	autoRotateProc(NULL, (rotateToggle == 0), NULL);
#else
 	if (rotateToggle == 0)
 	    auto_on = FALSE;
 	else if (!auto_on)
 	{
 	    auto_on = TRUE;
 	    notify_stop();
 	}
#endif
}

/*
 * Menu handler for `edit_menu (Build)'.
 */
Menu_item
cmd_build(item, op)
	Menu_item	item;
	Menu_generate	op;
{
	pm_pyr_frame_objects * ip =
	    (pm_pyr_frame_objects *) xv_get(item, XV_KEY_DATA, INSTANCE);
	
	switch (op) {
	case MENU_DISPLAY:
	case MENU_DISPLAY_DONE:
	case MENU_NOTIFY_DONE:
		break;

	case MENU_NOTIFY:
		mainObject();
		break;
	}
	return item;
}
