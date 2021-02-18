/* $XConsortium: five_views.c,v 5.1 91/02/16 09:32:22 rws Exp $ */

/***********************************************************
Copyright (c) 1991 by Sun Microsystems, Inc. and the X Consortium.

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

/* an example of the predefined views defined in PEX-SI
 * This example creates its own workstation type to define specific
 * window attributes.
 * It then shows four views of a cube and the x,y,z axes. These four
 * views are four of the predefined views. Each view only uses 1/4 of the window.
 * Next, the program shows predefined view 5, which is a full view of the
 * cube.
 * Notice that this example uses direct colours.
 */

#include "phigs/phigs.h"

static	Pview_rep3	views[6], cur_views[6];
static	Ppoint3		center = {0.5, 0.5, 0.5};
static	Ppoint3		x_axis = {0.9, 0.5, 0.5};
static	Ppoint3		y_axis = {0.5, 0.9, 0.5};
static	Ppoint3		z_axis = {0.5, 0.5, 0.9};
static	Ppoint3		box_top[] = {
				{0.0, 1.0, 0.0},
				{1.0, 1.0, 0.0},
				{1.0, 1.0, 1.0},
				{0.0, 1.0, 1.0},
				{0.0, 1.0, 0.0}};
static	Ppoint3		box_bot[] = {
				{0.0, 0.0, 0.0},
				{1.0, 0.0, 0.0},
				{1.0, 0.0, 1.0},
				{0.0, 0.0, 1.0},
				{0.0, 0.0, 0.0}};
static	Ppoint3		box_fl[] = { {0.0, 0.0, 1.0}, {0.0, 1.0, 1.0}};
static	Ppoint3		box_fr[] = { {1.0, 0.0, 1.0}, {1.0, 1.0, 1.0}};
static	Ppoint3		box_br[] = { {1.0, 0.0, 0.0}, {1.0, 1.0, 0.0}};
static	Ppoint3		box_bl[] = { {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}};

static	Ppoint3		vrp = {0.0, 0.0, 0.0};
static	Ppoint		border[5] = {
			    {0.05, 0.05},
			    {0.45, 0.05},
			    {0.45, 0.45},
			    {0.05, 0.45},
			    {0.05, 0.05}
			};

main()
{
    Pint		err, wst;
    Pupd_st		upd_st;
    Ppoint3		vrp, line[2];
    Ppoint_list3	pl;
    Ppoint_list		pl2;
    Pvec3		vpn, vup;
    Pvec		trans;
    Pmatrix		trans_mat;
    Pview_map3		map;
    Pgcolr		colr;
    int			i;

    extern double	sqrt();

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );
    wst = phigs_ws_type_create( phigs_ws_type_x_tool, (char *)NULL );
    phigs_ws_type_set( wst,
	(char*)PHIGS_TOOL_X, (char*)400,
	(char*)PHIGS_TOOL_Y, (char*)100,
	(char*)PHIGS_TOOL_WIDTH, (char*)600,
	(char*)PHIGS_TOOL_HEIGHT, (char*)600,
	(char*)PHIGS_TOOL_LABEL, "views",
	(char*)0 );
    popen_ws( 1, NULL, wst );

    popen_struct( 1 );
	pset_view_ind( 1 );
	    pexec_struct( 2 );
	    pexec_struct( 4 );
	pset_view_ind( 2 );
	    pexec_struct( 2 );
	    pexec_struct( 4 );
	pset_view_ind( 3 );
	    pexec_struct( 2 );
	    pexec_struct( 4 );
	pset_view_ind( 4 );
	    pexec_struct( 2 );
	    pexec_struct( 4 );
    pclose_struct();

    popen_struct( 2 );
	pl.num_points = 2; pl.points = line;
	colr.type = PMODEL_RGB;

	line[0] = center; line[1] = x_axis;
	colr.val.general.x = 1.0; colr.val.general.y = 0.0;
	    colr.val.general.z = 0.0;
	pset_line_colr( &colr );
	ppolyline3( &pl );

	line[0] = center; line[1] = y_axis;
	colr.val.general.x = 0.0; colr.val.general.y = 1.0;
	    colr.val.general.z = 0.0;
	pset_line_colr( &colr );
	ppolyline3( &pl );

	line[0] = center; line[1] = z_axis;
	colr.val.general.x = 0.0; colr.val.general.y = 0.0;
	    colr.val.general.z = 1.0;
	pset_line_colr( &colr );
	ppolyline3( &pl );
    pclose_struct();

    popen_struct( 4 );
	colr.type = PMODEL_RGB;

	pl.num_points = 5; pl.points = box_top;
	colr.val.general.x = 1.0; colr.val.general.y = 1.0;
	    colr.val.general.z = 1.0;
	pset_line_colr( &colr );
	ppolyline3( &pl );

	pl.num_points = 5; pl.points = box_bot;
	colr.val.general.x = 0.0; colr.val.general.y = 1.0;
	    colr.val.general.z = 1.0;
	pset_line_colr( &colr );
	ppolyline3( &pl );

	pl.num_points = 2; pl.points = box_fl;
	colr.val.general.x = 1.0; colr.val.general.y = 0.0;
	    colr.val.general.z = 0.0;
	pset_line_colr( &colr );
	ppolyline3( &pl );

	pl.num_points = 2; pl.points = box_fr;
	colr.val.general.x = 0.0; colr.val.general.y = 1.0;
	    colr.val.general.z = 0.0;
	pset_line_colr( &colr );
	ppolyline3( &pl );

	pl.num_points = 2; pl.points = box_br;
	colr.val.general.x = 0.0; colr.val.general.y = 0.0;
	    colr.val.general.z = 1.0;
	pset_line_colr( &colr );
	ppolyline3( &pl );

	pl.num_points = 2; pl.points = box_bl;
	colr.val.general.x = 1.0; colr.val.general.y = 1.0;
	    colr.val.general.z = 0.0;
	pset_line_colr( &colr );
	ppolyline3( &pl );
    pclose_struct();

    popen_struct( 3 );
	pset_view_ind( 0 );
	pl2.num_points = 5; pl2.points = border;
	colr.val.general.x = colr.val.general.y = colr.val.general.z = 1.0;
	pset_line_colr( &colr );

	ppolyline( &pl2 );

	trans.delta_x = 0.5; trans.delta_y = 0.0;
	ptranslate( &trans, &err, trans_mat );
	pset_local_tran( trans_mat, PTYPE_REPLACE );
	ppolyline( &pl2 );

	trans.delta_x = 0.5; trans.delta_y = 0.5;
	ptranslate( &trans, &err, trans_mat );
	pset_local_tran( trans_mat, PTYPE_REPLACE );
	ppolyline( &pl2 );

	trans.delta_x = 0.0; trans.delta_y = 0.5;
	ptranslate( &trans, &err, trans_mat );
	pset_local_tran( trans_mat, PTYPE_REPLACE );
	ppolyline( &pl2 );
    pclose_struct();

    popen_struct( 5 );
	pset_view_ind( 5 );
	pexec_struct( 2 );
	pexec_struct( 4 );
    pclose_struct();

    ppost_struct( 1, 1, 1.0 );
    ppost_struct( 1, 3, 1.0 );

    printf("views 1-4. return...");
    getchar();

    punpost_struct( 1, 1 );
    punpost_struct( 1, 3 );
    ppost_struct( 1, 5, 1.0 );

    printf("view 5. return...");
    getchar();

    pclose_ws( 1 );
    pclose_phigs();
}


