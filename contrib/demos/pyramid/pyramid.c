/* $XConsortium: pyramid.c,v 5.1 91/02/16 09:32:57 rws Exp $ */

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

#include "pyramid.h"

Frame	    frame, panelFrame, subFrame;
Canvas	    canvas, subCanvas;
Panel	    panel;
Panel_item   pn_x_rotation, pn_y_rotation, pn_z_rotation, pn_scale;
Panel_item   pn_auto_rotate_toggle, pn_auto_rotate_button;
Panel_item   pn_vpn_x, pn_vpn_y, pn_vpn_z;
Panel_item   pn_vup_x, pn_vup_y, pn_vup_z;
Panel_item   pn_vrp_x, pn_vrp_y, pn_vrp_z;
Panel_item   pn_pyr_depth, pn_pyr_children, pn_pyr_random, pn_pyr_probability;
Panel_item   pn_cube_text, pn_cube_row[3];
Panel_item   pn_build_button, pn_pyr_text, pn_pyr_type;

Panel_item   pn_win_xmin, pn_win_xmax, pn_win_ymin, pn_win_ymax;
Panel_item   pn_viewp_xmin, pn_viewp_xmax, pn_viewp_ymin, pn_viewp_ymax, 
	     pn_viewp_zmin, pn_viewp_zmax;
Panel_item   pn_prp_x, pn_prp_y, pn_prp_z, pn_proj_type;
Panel_item   pn_pln_front, pn_pln_back, pn_pln_view;


float	    minSize;
float	    global_scale    = (float)(SCALE_INIT_VALUE) / 10.0;
float	    global_x_ang    = (float)ROT_INIT_X_VALUE;
float	    global_y_ang    = (float)ROT_INIT_Y_VALUE;
float	    global_z_ang    = (float)ROT_INIT_Z_VALUE;
int	    increaseScale   = TRUE;
int	    auto_on	    = FALSE;
int	    use_mt	    = FALSE;

extern Pmatrix3 identity;

Pgcolr      black, white, red, green, blue, yellow, cyan, magenta,
		   purple, taupe, ltblue, pink, ltyellow, periwinkle, 
		   peach, ltgreen;

#define PANEL_GET_VAL(item, divideBy)  \
    (((float)(int)panel_get_value(item)) / (float)divideBy)

createSingleGrid()
{
    int		    i;
    float	    x, z;
    Pline_vdata_list3  vdata[2 * GRID_LINES];
    Ppoint3	    points[4 * GRID_LINES];

    /** This Grid will lie in the xz plane, along y = 0.0 **/
    for (i = 0, x = 0.0; i < GRID_LINES; i++, x += (1.0/(float)(GRID_LINES - 1))) {
	vdata[i].num_vertices = 2;
	vdata[i].vertex_data.points = &points[2 * i];
	points[2 * i].x = x;
	points[2 * i].y = 0.0;
	points[2 * i].z = 0.0;
	points[2 * i + 1].x = x;
	points[2 * i + 1].y = 0.0;
	points[2 * i + 1].z = 1.0;
    }
    for (z = 0.0; i < 2 * GRID_LINES; i++, z += (1.0 / (float)(GRID_LINES - 1))) {
	vdata[i].num_vertices = 2;
	vdata[i].vertex_data.points = &points[2 * i];
	points[2 * i].x = 0.0;
	points[2 * i].y = 0.0;
	points[2 * i].z = z;
	points[2 * i + 1].x = 1.0;
	points[2 * i + 1].y = 0.0;
	points[2 * i + 1].z = z;
    }
	    
    popen_struct(SINGLE_GRID);
	ppolyline_set3_data(PVERT_COORD, PINDIRECT, 2 * GRID_LINES, vdata);
    pclose_struct();
}

createMainGrid()
{
#   define PI		3.141592
    static Ppoint3	transformPt = { 0.0,0.0,0.0 }; 
    static Pvec3	shift = { 0.0,0.0,0.0 }; 
    static Pvec3	scale = { 1.0,1.0,1.0 }; 
    int			err;
    Pmatrix3		transform;
    Pvec3		textDir[2];
    static Ppoint3	pts[] = {
	0.0,0.0,0.0,
	1.0,0.0,0.0,
	0.0,0.0,0.0,
	0.0,1.0,0.0,
	0.0,0.0,0.0,
	0.0,0.0,1.0,
    };
    static Ppoint3	textPt = { 0.0,0.0,0.0 };
    Pint_list		nameSet;
    Pint		nameSet_ints[1];
    static Ppoint_list3 colored_line;

    popen_struct(MAIN_GRID);
	nameSet.num_ints = 1;
	nameSet.ints = nameSet_ints;
	nameSet_ints[0] = MAIN_GRID;
	padd_names_set(&nameSet);
	pset_view_ind(VIEW_1);
	pset_hlhsr_id(PHIGS_HLHSR_ID_ZBUFF);
	pset_line_colr(&green);
	/** insert grid in XZ plane at y = 0 **/
	pexec_struct(SINGLE_GRID);
	/** XY plane at z = 1 **/
	pbuild_tran_matrix3(&transformPt, &shift, -PI/2.0, 0.0, 0.0, &scale, 
	    &err, transform);
	pset_local_tran3(transform, PTYPE_REPLACE);
	pexec_struct(SINGLE_GRID);
	/** YZ plane at x = 0 **/
	pbuild_tran_matrix3(&transformPt, &shift, 0.0, 0.0, PI/2.0, &scale, 
	    &err, transform);
	pset_local_tran3(transform, PTYPE_REPLACE);
	pexec_struct(SINGLE_GRID);

	pset_local_tran3(identity, PTYPE_REPLACE);

	pexec_struct(TEXT_ANNOTATION);
    pclose_struct();	
    
    popen_struct(JUST_AXES);
	pset_view_ind(VIEW_1);
	pset_hlhsr_id(PHIGS_HLHSR_ID_ZBUFF);
	pset_line_colr(&green);
	pset_linewidth(3.0);
        colored_line.num_points = 2;
        colored_line.points = (&(pts[0]));
        ppolyline3( &colored_line );
        colored_line.points = (&(pts[2]));
        ppolyline3( &colored_line );
        colored_line.points = (&(pts[4]));
        ppolyline3( &colored_line );
	pexec_struct(TEXT_ANNOTATION);
    pclose_struct();	
}

void gridProc(item, val, event)
Panel_item  item;
int	    val;
Event	   *event;
{
    static Pint     nameSet_ints[1] = { MAIN_GRID };
    static Pint_list  nameSet = { 1, nameSet_ints };
    static Pint_list  emptySet = { 0, NULL };
    static Pfilter filter;

    
    printf("gridProc: val = %d\n", val);
    switch (val) {
	case 0 :
	    filter.incl_set = nameSet;
	    filter.excl_set = emptySet;
	    break;
	case 1 :
	    filter.incl_set = emptySet;
	    filter.excl_set = nameSet;	    
	    break;
    }
    pset_invis_filter(WSID, &filter);
    pupd_ws(WSID, PUPD_PERFORM);
}


createSinglePyramid(width, height, origin)
float 	width, height;			/* width of base, and height */
Ppoint3	*origin;
{

    Ppoint3	    pyrPoints[8]; 
    Pvec3	    pyrNormals[6];
    Pfacet_data_arr3  pyrFacetData;
    Pfacet_vdata_arr3 pyrVertexData;
    Pfacet_vdata_list3 pyrVertexDataList;
    Ppoint3	    tip, frontLeft, frontRight, backLeft, backRight;
    int		    i, error, flip;
    
    /**  Pyramid is centered at 0.0,0.0,0.0.  The base of the pyramid lies
     ** in the XZ plane.  The tip of the pyramid is on the Y Axis, in the 
     ** positive Y direction from the base.  The actual coordinates are
     ** calculated based on the parameters.  **/
    tip.x = 0.0;  	   tip.y = height;		tip.z = 0.0;
    frontLeft.x = - width; frontLeft.y = - tip.y;	frontLeft.z = width;
    frontRight.x = width;  frontRight.y = - tip.y;	frontRight.z = width;
    backLeft.x = - width;  backLeft.y = - tip.y;	backLeft.z = - width;
    backRight.x = width;   backRight.y = - tip.y;	backRight.z = - width;
    
    pyrPoints[0] = frontLeft;
    pyrPoints[1] = tip;
    pyrPoints[2] = frontRight;
    pyrPoints[3] = backRight;
    pyrPoints[4] = frontLeft;
    pyrPoints[5] = backLeft;
    pyrPoints[6] = tip;
    pyrPoints[7] = backRight;

    for (i=0; i < 8; i++) {
	pyrPoints[i].x += origin->x;
	pyrPoints[i].y += origin->y;
	pyrPoints[i].z += origin->z;
    }
    
    for (i = 0, flip = ~0; i < 6; i++, flip = ~flip) {
	pyrVertexDataList.num_vertices = 3;
	pyrVertexDataList.vertex_data.points = &pyrPoints[i];
	pcomp_fill_area_set_gnorm(PVERT_COORD, 1, &pyrVertexDataList, &error, 
		&pyrNormals[i]);
	if (error) {
	    fprintf(stderr, "Error %d returned from pcomp_fill_area_set_gnorm\n",
		error);
	    exit(1);
	}
	if (flip) {
	    pyrNormals[i].delta_x = -pyrNormals[i].delta_x;
	    pyrNormals[i].delta_y = -pyrNormals[i].delta_y;
	    pyrNormals[i].delta_z = -pyrNormals[i].delta_z;
	}
    }

    pyrFacetData.norms = pyrNormals;
    pyrVertexData.points = pyrPoints;
    
    if (use_mt) {
	popen_struct(SINGLE_PYRAMID);
    }
	/** Create sides and base of pyramid with a 6 triangle triangle strip**/
	ptri_strip3_data(PFACET_NORMAL, PVERT_COORD, PMODEL_RGB, 8, 
		    &pyrFacetData, &pyrVertexData);

    if (use_mt) {
	pclose_struct();
    }

}

unsigned createToggle;
unsigned randomToggle;
unsigned prob;
unsigned numObjects;
unsigned row_toggle[3];

long random();

buildPyramid(x, y, z, size)
register float x, y, z;
float size;
{

    static Ppoint3  transformPt = { 0.0, 0.0, 0.0 };
    
    register float  halfSize = size / 2.0;
    Pvec3	    shift, scale;
    Pint	    err;
    Pmatrix3	    transform;
    Ppoint3	    origin;

#define MAX_RANDOM 2147483647	/** 2^31 - 1 **/
#define RANDOM	(random() <= ((MAX_RANDOM / 100) * prob))

    if (size < minSize) {
	if (use_mt) {
	    shift.delta_x = x;
	    shift.delta_y = y;
	    shift.delta_z = z;
	    scale.delta_x = scale.delta_y = scale.delta_z = size;
	    pbuild_tran_matrix3(&transformPt, &shift, 0.0, 0.0, 0.0, &scale, &err, 
		transform);
	    pset_local_tran3( transform, PTYPE_REPLACE); 
	    pexec_struct(SINGLE_PYRAMID);
	} else {
	    origin.x = x;
	    origin.y = y;
	    origin.z = z;
	    createSinglePyramid(size, size, &origin);
	}
	numObjects++;
    } else {
	if ( (toggle_bit_on(createToggle, 0)) &&
	     ( (!toggle_bit_on(randomToggle, 0)) || RANDOM ))
	    buildPyramid(x - halfSize, y - halfSize, z + halfSize, halfSize);

	if ( (toggle_bit_on(createToggle, 1)) &&
	     ( (!toggle_bit_on(randomToggle, 1)) || RANDOM ))
	    buildPyramid(x + halfSize, y - halfSize, z + halfSize, halfSize);

	if ( (toggle_bit_on(createToggle, 2)) &&
	     ( (!toggle_bit_on(randomToggle, 2)) || RANDOM ))
	    buildPyramid(x - halfSize, y - halfSize, z - halfSize, halfSize);

	if ( (toggle_bit_on(createToggle, 3)) &&
	     ( (!toggle_bit_on(randomToggle, 3)) || RANDOM ))
	    buildPyramid(x + halfSize, y - halfSize, z - halfSize, halfSize);

	if ( (toggle_bit_on(createToggle, 4)) &&
	     ( (!toggle_bit_on(randomToggle, 4)) || RANDOM ))
	    buildPyramid(x, y + halfSize, z, halfSize);
    }
}

buildCube(x, y, z, size)
register float x, y, z;
float size;
{

    static Ppoint3  transformPt = { 0.0, 0.0, 0.0 };
    
    register float  thirdSize = size / 3.0;
    Pvec3	    shift, scale;
    Pint	    err;
    Pmatrix3	    transform;
    int		    xi, yi, zi;
    float	    x1, y1, z1;

    if (size < minSize) {
	shift.delta_x = x;
	shift.delta_y = y;
	shift.delta_z = z;
	scale.delta_x = scale.delta_y = scale.delta_z = size;
	pbuild_tran_matrix3(&transformPt, &shift, 0.0, 0.0, 0.0, &scale, &err, transform);
	pset_local_tran3( transform, PTYPE_REPLACE); 
	pexec_struct(POS_LIGHT_STRUCT);
	numObjects++;
    } else {
	for (yi = 0; yi < 3; yi++) {
	    y1 = y + (yi - 1) * thirdSize;
	    for (zi = 0; zi < 3; zi++) {
		z1 = z + (zi - 1) * thirdSize;
		for (xi = 0; xi < 3; xi++) {
		    x1 = x + (xi - 1) * thirdSize;
		    if (toggle_bit_on(row_toggle[yi], (unsigned)(zi*3 + xi)))
			buildCube(x1, y1, z1, thirdSize);
		}
	    }
	}
    }
}


setup(n, matrix)
int n;
Pmatrix3 matrix;
{
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            matrix[i][j] = 0;
            if (i == j) matrix[i][j] = 1.0;
	}
    }
}

#define SETRGBCOLOR(col, r, g, b) \
    col.type = PMODEL_RGB; \
    col.val.general.x = r; \
    col.val.general.y = g; \
    col.val.general.z = b;
    
setupColors()
{
    SETRGBCOLOR(black,	    0.0,0.0,0.0)
    SETRGBCOLOR(white,	    1.0,1.0,1.0)
    SETRGBCOLOR(red,	    1.0,0.0,0.0)
    SETRGBCOLOR(green,	    0.0,1.0,0.0)
    SETRGBCOLOR(blue,	    0.0,0.0,1.0)
    SETRGBCOLOR(yellow,	    1.0,1.0,0.0)
    SETRGBCOLOR(cyan,	    0.0,1.0,1.0)
    SETRGBCOLOR(magenta,    1.0,0.0,1.0)
    SETRGBCOLOR(purple,	    0.7,0.5,1.0)
    SETRGBCOLOR(taupe,	    0.6,0.6,0.6)
    SETRGBCOLOR(ltblue,	    0.9,1.0,1.0)
    SETRGBCOLOR(pink,	    1.0,0.9,1.0)
    SETRGBCOLOR(ltyellow,   1.0,1.0,0.9)
    SETRGBCOLOR(periwinkle, 0.9,0.9,1.0)
    SETRGBCOLOR(peach,	    1.0,0.9,0.9)
    SETRGBCOLOR(ltgreen,    0.9,1.0,0.9)

}

void
mainObject()
{
#   define		DUMMY_LABEL 999
    int			depth = (int)panel_get_value(pn_pyr_depth);
    int			i;
    char		message[80];
    Pmatrix3		mat;
    int			cube = (int)(panel_get_value(pn_pyr_type));


    numObjects = 0;

    if (cube) {
	for (minSize = 3.0, i = 0; i < depth; i++, minSize /= 3.0);
	row_toggle[0] = (unsigned)panel_get_value(pn_cube_row[0]);
	row_toggle[1] = (unsigned)panel_get_value(pn_cube_row[1]);
	row_toggle[2] = (unsigned)panel_get_value(pn_cube_row[2]);
    } else {
        for (minSize = 1.0, i = 0; i < depth; i++, minSize /= 2.0);
	createToggle = (unsigned)panel_get_value(pn_pyr_children);
	randomToggle = (unsigned)panel_get_value(pn_pyr_random);
	prob	     = (unsigned)panel_get_value(pn_pyr_probability);
    }


    /** Change update state so we can 'see' the structure recursively built **/
    pset_edit_mode(PEDIT_INSERT);
    punpost_struct(WSID, MAIN_PYRAMID);
    pempty_struct(MAIN_PYRAMID);
    popen_struct(MAIN_PYRAMID);

	pset_view_ind(VIEW_1);
	pset_int_style(PSTYLE_SOLID);
	pset_face_cull_mode(PCULL_BACKFACE);
	
	plabel(ADD_PROPS_HERE);
	plabel(DUMMY_LABEL);		/** Area Properties will go here **/
	
	plabel(ADD_INT_COLOR_HERE);
	plabel(DUMMY_LABEL);		/** Interior Colour will go here **/

	pset_refl_eqn(PREFL_AMB_DIFF_SPEC);
	pset_int_shad_meth(PSD_COLOUR);
	
	plabel(ADD_LIGHTS_HERE);
	plabel(DUMMY_LABEL);		/** Light Source State will go here **/

	pset_hlhsr_id(PHIGS_HLHSR_ID_ZBUFF);
	plabel(GLOBAL_TRANSFORM);
	setup(4, mat);
	pset_global_tran3(mat); 
	
	if (cube)
	    buildCube(0.5, 0.5, 0.5, 1.0);
	else
	    buildPyramid(0.5, 0.5, 0.5, 0.5);

    pclose_struct();
    set_prop_proc();
    initialize_lights();
    sliderVPNProc(0,0,0);
    doRotate(global_scale, global_x_ang, global_y_ang, global_z_ang);
    ppost_struct(WSID, MAIN_PYRAMID, 0.0);
    if (cube)
	sprintf(message, "%d Cubes",
		numObjects);
    else
	sprintf(message, "%d Pyramids",
		numObjects);
    xv_set(frame, FRAME_SHOW_FOOTER, TRUE, FRAME_LEFT_FOOTER, message,
	   FRAME_RIGHT_FOOTER,
	   cube ? "(4x3 QUAD MESHes w/6 null facets)"
	        : "(six-element TRIANGLE STRIPs)", 
	   0);

    /** Change update state back **/
    pset_disp_upd_st(WSID, PDEFER_WAIT, PMODE_NIVE);
    pupd_ws(WSID, PUPD_PERFORM);
}


setViewRep()
{
    Pview_rep3           rep;
    Pview_map3       map;
    Ppoint3             vrp;
    Pvec3            vup;
    Pvec3            vpn;
    Pint                err;
 
    vrp.x = vrp.y = vrp.z = 0.5;
 
    map.proj_type = PTYPE_PARAL;
    rep.xy_clip = rep.back_clip = rep.front_clip = PIND_CLIP;
/*    map.win.x_min = -2.0; map.win.x_max = 2.0;
    map.win.y_min = -2.0; map.win.y_max = 2.0; */
    map.win.x_min = -0.80; map.win.x_max = 0.80;
    map.win.y_min = -0.80; map.win.y_max = 0.80;
    map.back_plane = -2.0;
    map.front_plane = 2.0;
    map.view_plane =  1.8;
    map.proj_ref_point.x = (map.win.x_min + map.win.x_max) / 2.0;
    map.proj_ref_point.y = (map.win.y_min + map.win.y_max) / 2.0;
    map.proj_ref_point.z = 10.0;
    map.vp.z_min = 0.0; map.vp.z_max = 1.0;

    /* Initial View -- straight on */
    vup.delta_x =  0.0; vup.delta_y =  1.0; vup.delta_z = 0.0;
    vpn.delta_x =  0.0; vpn.delta_y =  0.0; vpn.delta_z = 0.01;
    peval_view_ori_matrix3( &vrp, &vpn, &vup, &err,
        rep.ori_matrix);
    map.vp.x_min = 0.0; map.vp.x_max = 1.00;
    map.vp.y_min = 0.0; map.vp.y_max = 1.00;
    peval_view_map_matrix3( &map, &err, rep.map_matrix);
    rep.clip_limit = map.vp;
    pset_view_rep3( WSID, VIEW_1, &rep );
    pset_view_rep3( WSID2, VIEW_1, &rep );
    pupd_ws(WSID, PUPD_PERFORM);
    pupd_ws(WSID2, PUPD_PERFORM);
}

doRotate(scale_val, xang, yang, zang)
float scale_val, xang, yang, zang;
{
    Pvec3	    scale;
    static float    anglesToRadians = 3.141592 / 180.0;
    static Pvec3 shift = { 0.0,0.0,0.0 };
    static Ppoint3  transformPt = { 0.5,0.5,0.5 };
    int		    err;
    Pmatrix3	    transform;
    
    global_scale = scale_val;
    global_x_ang = xang;
    global_y_ang = yang;
    global_z_ang = zang;

    scale.delta_x = scale.delta_y = scale.delta_z = scale_val;

    pbuild_tran_matrix3(&transformPt, &shift, 
	xang * anglesToRadians, 
	yang * anglesToRadians, 
	zang * anglesToRadians, 
	&scale, &err, transform);

    popen_struct(MAIN_PYRAMID);
	pset_elem_ptr(0);
	pset_elem_ptr_label(GLOBAL_TRANSFORM);
	poffset_elem_ptr(1);
	pset_edit_mode(PEDIT_REPLACE);
	pset_global_tran3(transform);
	pset_edit_mode(PEDIT_INSERT);
    pclose_struct();    

    pupd_ws(WSID, PUPD_PERFORM);
}

#ifdef USE_ITIMER_FOR_ROTATE
#define ITIMER_NULL ((struct itimerval *)0)
static struct itimerval updtimer;

void
autoRotateProc(item, val, event)
Panel_item  item;
int	    val;
Event	   *event;
{
    Notify_value auto_rotate();

    switch (val) {
      case 0:
	updtimer.it_interval.tv_usec = 15151;
	updtimer.it_interval.tv_sec = 0;
	updtimer.it_value.tv_usec = 15151;
	updtimer.it_value.tv_sec = 0;  
	notify_set_itimer_func(frame,auto_rotate,ITIMER_REAL,
			       &updtimer,ITIMER_NULL);
	break;

      case 1:
	notify_set_itimer_func(frame,NOTIFY_FUNC_NULL,ITIMER_REAL,
			       NULL, ITIMER_NULL);
	panel_set(pn_scale, PANEL_VALUE, (int)(global_scale * 10.0), 0);
	panel_set(pn_x_rotation, PANEL_VALUE, (int)(global_x_ang), 0);
	panel_set(pn_y_rotation, PANEL_VALUE, (int)(global_y_ang), 0);
	panel_set(pn_z_rotation, PANEL_VALUE, (int)(global_z_ang), 0);
	break;
    }
}
#else
void
autoRotateProc(item, val, event)
Panel_item  item;
int	    val;
Event	   *event;
{
    switch (val) {
	case 0 :
	    auto_on = FALSE;
	    break;
	case 1 :
	    auto_on = TRUE;
	    notify_stop();
	    break;
    }
}
#endif

Notify_value
auto_rotate()
{
#   define ANGLE_INCR	5.0
#   define SCALE_FACTOR	0.95
#   define MIN_SCALE	0.15
#   define MAX_SCALE	1.20
#   define ADD_TO_ANGLE(ang) ang += ANGLE_INCR ; if (ang > 360.0) ang -= 360.0;

    unsigned rotateToggle = (unsigned)panel_get_value(pn_auto_rotate_toggle);

    Display *dpy = (Display *) XV_DISPLAY_FROM_WINDOW(frame);
    Window w = (Window) xv_get(canvas, XV_XID, 0);
    XImage *img;

    if (toggle_bit_on(rotateToggle, 0)) {
	ADD_TO_ANGLE(global_x_ang);
    }
    if (toggle_bit_on(rotateToggle, 1)) {
	ADD_TO_ANGLE(global_y_ang);
    }
    if (toggle_bit_on(rotateToggle, 2)) {
	ADD_TO_ANGLE(global_z_ang);
    }
    if (toggle_bit_on(rotateToggle, 3)) {
	if (increaseScale) {
	    global_scale /= SCALE_FACTOR;
	    if (global_scale > MAX_SCALE)
		increaseScale = FALSE;
	} else {
	    global_scale *= SCALE_FACTOR;
	    if (global_scale < MIN_SCALE)
		increaseScale = TRUE;
	}
    }
    doRotate(global_scale, global_x_ang, global_y_ang, global_z_ang);

    /*
     * Wait until the server is idle
     */
    img = XGetImage(dpy, w, 500, 500, 1, 1, ~0, ZPixmap);
    XDestroyImage(img);

    return NOTIFY_DONE;
}

void
sliderRotateProc(item, value, event)
Panel_item  item;
int	    value;
Event	   *event;
{
    float	    scaleVal;
    float	    rotX, rotY, rotZ;

    rotX = (float)((int)panel_get_value(pn_x_rotation));
    rotY = (float)((int)panel_get_value(pn_y_rotation));
    rotZ = (float)((int)panel_get_value(pn_z_rotation));
    scaleVal = (float)((int)panel_get_value(pn_scale)) / 10.0;

    /** Override one of the above depending on which slider is in motion,
     ** since panel_get_value() returns the last SET value, not those
     ** encountered when the slider is in motion **/ 
    if (item == pn_x_rotation)
	rotX = (float)value;
    else if (item == pn_y_rotation)
	rotY = (float)value;
    else if (item == pn_z_rotation)
	rotZ = (float)value;
    else if (item == pn_scale)
	scaleVal = (float)value / 10.0;	
	
    doRotate(scaleVal, rotX, rotY, rotZ);

}


void 
viewMapProc(item, value, event)
Panel_item  item;
int	    value;
Event	   *event;
{
    Pview_map3   map;
    Pview_rep3	    rep, reqRep;
    Pint	    err;
    Pupd_st	    update_state;
    
    pinq_view_rep(WSID, VIEW_1, &err, &update_state, &rep, &reqRep);
    
    map.win.x_min	= PANEL_GET_VAL(pn_win_xmin, 100.0);
    map.win.x_max	= PANEL_GET_VAL(pn_win_xmax, 100.0);
    map.win.y_min	= PANEL_GET_VAL(pn_win_ymin, 100.0);
    map.win.y_max	= PANEL_GET_VAL(pn_win_ymax, 100.0);
    map.vp.x_min	= PANEL_GET_VAL(pn_viewp_xmin, 100.0);
    map.vp.x_max	= PANEL_GET_VAL(pn_viewp_xmax, 100.0);
    map.vp.y_min	= PANEL_GET_VAL(pn_viewp_ymin, 100.0);
    map.vp.y_max	= PANEL_GET_VAL(pn_viewp_ymax, 100.0);
    map.vp.z_min	= PANEL_GET_VAL(pn_viewp_zmin, 100.0);
    map.vp.z_max	= PANEL_GET_VAL(pn_viewp_zmax, 100.0);
    map.proj_ref_point.x		= PANEL_GET_VAL(pn_prp_x, 10.0);
    map.proj_ref_point.y		= PANEL_GET_VAL(pn_prp_y, 10.0);
    map.proj_ref_point.z		= PANEL_GET_VAL(pn_prp_z, 1.0);
    map.proj_type		= (panel_get_value(pn_proj_type) ? PTYPE_PERSPECT 
							 : PTYPE_PARAL);
    map.back_plane	= PANEL_GET_VAL(pn_pln_back, 10.0);
    map.front_plane	= PANEL_GET_VAL(pn_pln_front, 10.0);
    map.view_plane	= PANEL_GET_VAL(pn_pln_view, 10.0);

    if (item == pn_win_xmin)
	map.win.x_min = (float)value / 100.0;
    else if (item == pn_win_xmax)
	map.win.x_max = (float)value / 100.0;
    else if (item == pn_win_ymin)
	map.win.y_min = (float)value / 100.0;
    else if (item == pn_win_ymax)
	map.win.y_max = (float)value / 100.0;
    else if (item == pn_viewp_xmin)
	map.vp.x_min = (float)value / 100.0;
    else if (item == pn_viewp_xmax)
	map.vp.x_max = (float)value / 100.0;
    else if (item == pn_viewp_ymin)
	map.vp.y_min = (float)value / 100.0;
    else if (item == pn_viewp_ymax)
	map.vp.y_max = (float)value / 100.0;
    else if (item == pn_viewp_zmin)
	map.vp.z_min = (float)value / 100.0;
    else if (item == pn_viewp_zmax)
	map.vp.z_max = (float)value / 100.0;
    else if (item == pn_prp_x)
	map.proj_ref_point.x = (float)value / 10.0;
    else if (item == pn_prp_y)
	map.proj_ref_point.y = (float)value / 10.0;
    else if (item == pn_prp_z)
	map.proj_ref_point.z = (float)value;
    else if (item == pn_proj_type)
	map.proj_type = (value ? PTYPE_PERSPECT : PTYPE_PARAL);
    else if (item == pn_pln_back)
	map.back_plane = (float)value / 10.0;
    else if (item == pn_pln_front)
	map.front_plane = (float)value / 10.0;
    else if (item == pn_pln_view)
	map.view_plane = (float)value / 10.0;
    else
	fprintf(stderr, "Shouldn't get here!! File: %s Line %d\n", 
	    __FILE__, __LINE__);
	    
    if (map.vp.x_min >= map.vp.x_max)
	if (item == pn_viewp_xmin) {
	    panel_set_value(pn_viewp_xmin, (int)(map.vp.x_max * 100));
	    map.vp.x_min = map.vp.x_max - 0.01;
	} else {
	    panel_set_value(pn_viewp_xmax, (int)(map.vp.x_min * 100));
	    map.vp.x_max = map.vp.x_min + 0.01;
	}

    if (map.vp.y_min >= map.vp.y_max)
	if (item == pn_viewp_ymin) {
	    panel_set_value(pn_viewp_ymin, (int)(map.vp.y_max * 100));
	    map.vp.y_min = map.vp.y_max - 0.01;
	} else {
	    panel_set_value(pn_viewp_ymax, (int)(map.vp.y_min * 100));
	    map.vp.y_max = map.vp.y_min + 0.01;
	}

    if (map.vp.z_min >= map.vp.z_max)
	if (item == pn_viewp_zmin) {
	    panel_set_value(pn_viewp_zmin, (int)(map.vp.z_max * 100));
	    map.vp.z_min = map.vp.z_max - 0.01;
	} else {
	    panel_set_value(pn_viewp_zmax, (int)(map.vp.z_min * 100));
	    map.vp.z_max = map.vp.z_min + 0.01;
	}


    peval_view_map_matrix3( &map, &err, rep.map_matrix);
    rep.clip_limit = map.vp;
    pset_view_rep3( WSID, VIEW_1, &rep );
    pset_view_rep3( WSID2, VIEW_1, &rep );
    pupd_ws(WSID, PUPD_PERFORM);
    pupd_ws(WSID2, PUPD_PERFORM);
}

void
sliderVPNProc(item, value, event)
Panel_item  item;
int	    value;
Event	   *event;
{
    static Pview_rep3   rep, req_rep;
    static Pupd_st   update_state;
    static Ppoint3     vrp;
    static Pvec3    vup;
    static Pvec3    vpn;
    static Pint        err;

    pinq_view_rep(WSID, VIEW_1, &err, &update_state, &rep, &req_rep);

    vpn.delta_x = PANEL_GET_VAL(pn_vpn_x, 100.0);
    vpn.delta_y = PANEL_GET_VAL(pn_vpn_y, 100.0);
    vpn.delta_z = PANEL_GET_VAL(pn_vpn_z, 100.0);
    
    vup.delta_x = PANEL_GET_VAL(pn_vup_x, 10.0);
    vup.delta_y = PANEL_GET_VAL(pn_vup_y, 10.0);
    vup.delta_z = PANEL_GET_VAL(pn_vup_z, 10.0);

    vrp.x = PANEL_GET_VAL(pn_vrp_x, 10.0);
    vrp.y = PANEL_GET_VAL(pn_vrp_y, 10.0);
    vrp.z = PANEL_GET_VAL(pn_vrp_z, 10.0);
    
    /** Override that value with whichever slider is currently being changed **/
    if (item == pn_vpn_x)
	vpn.delta_x = (float)value / 100.0;
    else if (item == pn_vpn_y)
	vpn.delta_y = (float)value / 100.0;
    else if (item == pn_vpn_z)
	vpn.delta_z = (float)value / 100.0;
    else if (item == pn_vup_x)
	vup.delta_x = (float)value / 10.0;
    else if (item == pn_vup_y)
	vup.delta_y = (float)value / 10.0;
    else if (item == pn_vup_z)
	vup.delta_z = (float)value / 10.0;
    else if (item == pn_vrp_x)
	vrp.x = (float)value / 10.0;
    else if (item == pn_vrp_y)
	vrp.y = (float)value / 10.0;
    else if (item == pn_vrp_z)
	vrp.z = (float)value / 10.0;

    peval_view_ori_matrix3(&vrp, &vpn, &vup, &err, rep.ori_matrix);
    pset_view_rep3(WSID, VIEW_1, &rep);
    pset_view_rep3(WSID2, VIEW_1, &rep);
    pupd_ws(WSID, PUPD_PERFORM);
    pupd_ws(WSID2, PUPD_PERFORM);
}

void
exitProc()
{
    pclose_ws(WSID);
    pclose_ws(WSID2);
    pclose_phigs();
    exit(0);
}

extern Display *XOpenDisplay();

main(argc, argv)
{
    static Pcolr_rep background;
    char *bg_colour;
    static Pxphigs_info xinfo = {NULL, NULL, NULL, NULL, NULL, NULL, {0, 0}};
    static Pconnid_x_drawable conn_id = {NULL, NULL};
    static unsigned long	xinfo_mask;

    background.rgb.red = background.rgb.green = background.rgb.blue = 0.0;

    setupWindow(argc, argv);  /* creates a display connection */

    xinfo.display = (Display *) XV_DISPLAY_FROM_WINDOW(frame);

    xinfo.flags.no_monitor = 1;  /* we're not doing input... */

    xinfo_mask = PXPHIGS_INFO_DISPLAY | PXPHIGS_INFO_FLAGS_NO_MON;

    popen_xphigs( (char*)0, PDEF_MEM_SIZE, xinfo_mask, &xinfo);
    
    setupColors();
    
/*
    conn_id.display = xinfo.display;
    conn_id.drawable_id = (XID) xv_get(canvas, XV_XID, 0);
    popen_ws( WSID, (Pconnid) &conn_id, phigs_ws_type_x_drawable);
 */
    pex_xv_popen_ws( WSID, canvas, phigs_ws_type_x_drawable, frame);

    if (bg_colour = (char *) getenv("PYRAMID_BG_COLOR"))
    {
	sscanf(bg_colour, "%f %f %f", 
	       &background.rgb.red,
	       &background.rgb.green,
	       &background.rgb.blue);

    }

    pset_disp_upd_st(WSID, PDEFER_WAIT, PMODE_NIVE);
    
/*
    conn_id.drawable_id = (XID) xv_get(subCanvas, XV_XID, 0);
    popen_ws( WSID2, (Pconnid) &conn_id, phigs_ws_type_x_drawable);
 */
    pex_xv_popen_ws( WSID2, subCanvas, phigs_ws_type_x_drawable, subFrame);

    pset_disp_upd_st(WSID2, PDEFER_WAIT, PMODE_NIVE);
    
    pset_colr_rep(WSID, 0, &background);
    pset_hlhsr_mode(WSID, PHIGS_HLHSR_MODE_ZBUFF);
    pset_hlhsr_mode(WSID2, PHIGS_HLHSR_MODE_ZBUFF);
    
    setViewRep();
    
    createSingleGrid();
    createMainGrid();
    initLightStuff();

    if (use_mt) {
	createSinglePyramid(1.0, 1.0, 0.0, 0.0, 0.0);
    }
    mainObject();
    
    /** Temp **/
    set_prop_proc();

    ppost_struct(WSID, MAIN_GRID, 0.0);
    ppost_struct(WSID2, JUST_AXES, 0.0);
    
    pupd_ws(WSID, PUPD_PERFORM);
    
#ifdef USE_ITIMER_FOR_ROTATE
    xv_main_loop(frame);
#else
    window_set(frame, WIN_SHOW, TRUE, 0);

    while (1) {
	if (auto_on) {
	    do {
		int i;
		auto_rotate();
		for (i = 1; i < 3; i++)
		    notify_dispatch();
	    } while (auto_on);
	    panel_set(pn_scale, PANEL_VALUE, (int)(global_scale * 10.0), 0);
	    panel_set(pn_x_rotation, PANEL_VALUE, (int)(global_x_ang), 0);
	    panel_set(pn_y_rotation, PANEL_VALUE, (int)(global_y_ang), 0);
	    panel_set(pn_z_rotation, PANEL_VALUE, (int)(global_z_ang), 0);
	} else {
	    notify_start();
	}
    }
#endif    
}
