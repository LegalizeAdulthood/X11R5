/* $XConsortium: multisurf.c,v 5.1 91/02/16 09:32:26 rws Exp $ */

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

#define PI 3.141592654

#include "phigs/phigs.h"
main(argc,argv)
        int argc; char  ** argv;
{
	static Pview_map3 view_map = {
				{ 0.0, 1.0, 0.0, 1.0},
				{ 0.0, .5, 0.5, 1.0, 0.0, 1.0},
				PTYPE_PARAL,
				{ 0.5, 0.5, -20.0},
				0.0,
				0.0,
				1.0
			      };

	static Pview_map3 view_map1 = {
				{ 0.0, 1.0, 0.0, 1.0},
				{ 0.5, 1.0, 0.5, 1.0, 0.0, 1.0},
				PTYPE_PARAL,
				{ 0.5, 0.5, -20.0},
				0.0,
				0.0,
				1.0
			      };

	static Pview_map3 view_map2 = {
				{ 0.0, 1.0, 0.0, 1.0},
				{ 0.0, .5, 0.0, 0.5, 0.0, 1.0},
				PTYPE_PARAL,
				{ 0.5, 0.5, -20.0},
				0.0,
				0.0,
				1.0
			      };

	static Pview_map3 view_map3 = {
				{ 0.0, 1.0, 0.0, 1.0},
				{ 0.5, 1.0, 0.0, 0.5, 0.0, 1.0},
				PTYPE_PARAL,
				{ 0.5, 0.5, -20.0},
				0.0,
				0.0,
				1.0
			      };

	static Ppoint3 vrp = {0., 0., 0 };	/*  origin */
	static Pvec3 vpn = {0.0, 0.0, 1.0 };    /*  view plane normal */
	static Pvec3 vup = {0.0, 1.0, 0.0 };    /*  view up vector */
	
	Pview_rep3 vrep;			/*  view structure */

        Pint            err;
        
	Pint 		uorder = 4,
			vorder = 4;
	Pfloat_list 	uknots,
			vknots;
        static int 	unumknots = 20,
        		vnumknots = 20;
        static Pfloat 	kpts[] = {  1.0,  2.0,  3.0,  4.0,  5.0, 
				    6.0,  7.0,  8.0,  9.0, 10.0,
				   11.0, 12.0, 13.0, 14.0, 15.0,
				   16.0, 17.0, 18.0, 19.0, 20.0 };
	Prational 	rationality = PNON_RATIONAL;
	Ppoint_grid34	grid;
        static Ppoint3 	cpts[] = {

		0.2,0.5,0.2, 0.4,0.5,0.2, 0.6,0.5,0.2, 0.8,0.5,0.2,
		0.2,0.5,0.4, 0.4,0.7,0.4, 0.6,0.3,0.4, 0.8,0.5,0.4,
		0.2,0.5,0.6, 0.4,0.3,0.6, 0.6,0.7,0.6, 0.8,0.5,0.6,
		0.2,0.5,0.8, 0.4,0.5,0.8, 0.6,0.5,0.8, 0.8,0.5,0.8,

			        };

	static Ppoint	trim_curve_cpts[] = {
		4.5,	4.25,	1.0,
		4.5,	4.25,	1.0,
		4.625,	4.25,	1.0,
		4.75,	4.375,	1.0,
		4.75,	4.5,	1.0, 
		4.75,	4.625,	1.0,
		4.625,	4.75,	1.0,
		4.5,	4.75,	1.0,
		4.375,	4.75,	1.0,
		4.25,	4.625,	1.0,
		4.25,	4.5,	1.0,
		4.25,	4.375,	1.0,
		4.375,	4.25,	1.0,
		4.5,	4.25,	1.0,
		4.5,	4.25,	1.0
	};

	static Pfloat_list trim_knot_list = {
		19,			/* number of knots */
		kpts			/* knot list */
	};

	Ppoint_list23	trim_curve_points;

	Ptrimcurve_list	trim_curve_list;

	Ptrimcurve trim_curves[4];

	Pint		curve_approx_type
				= PCURV_CONSTANT_PARAMETRIC_BETWEEN_KNOTS;
	Pfloat		ucurve_approx_tolerance = 10.0,
			vcurve_approx_tolerance = 10.0;
	Ppoint3	    pt;
	Pvec3	    shift;
	Pfloat	    x_ang, y_ang, z_ang;
	Pvec3	    scale;
	Pmatrix3    bldmat;

        popen_phigs(NULL,0);
        popen_ws(1,NULL,phigs_ws_type_x_tool);

	/* set up views */
	vrep.clip_limit =  view_map.vp;
	vrep.xy_clip = PIND_NO_CLIP;
	vrep.back_clip = PIND_NO_CLIP;
	vrep.front_clip = PIND_NO_CLIP;
	peval_view_ori_matrix3( &vrp, &vpn, &vup, &err, vrep.ori_matrix);

	peval_view_map_matrix3(&view_map, &err, vrep.map_matrix);
	pset_view_rep3( 1, 1, &vrep);

	peval_view_map_matrix3(&view_map1, &err, vrep.map_matrix);
	pset_view_rep3( 1, 2, &vrep);

	peval_view_map_matrix3(&view_map2, &err, vrep.map_matrix);
	pset_view_rep3( 1, 3, &vrep);

	peval_view_map_matrix3(&view_map3, &err, vrep.map_matrix);
	pset_view_rep3( 1, 4, &vrep);

	/* render path */
        popen_struct(1);

	trim_curve_points.num_points = 15;
	trim_curve_points.points.point2d = trim_curve_cpts;

	trim_curve_list.num_curves = 1;
	trim_curve_list.curves = trim_curves;

	trim_curves[0].visible = PEDGE_ON;		/* visibility */
	trim_curves[0].rationality = PRATIONAL;		/* rational curve */
	trim_curves[0].order = 4;			/* cubic curve */
	trim_curves[0].approx_type = PCURV_WS_DEP;	/* approx type (1) */
	trim_curves[0].approx_val = 1.0;		/* approx tolerance */
	trim_curves[0].knots = trim_knot_list;		/* trim curve knots */
	trim_curves[0].tmin = 4.0; 
	trim_curves[0].tmax = 16.0;			/* parameter range */
	trim_curves[0].cpts = trim_curve_points;	/* control points */

        grid.num_points.u_dim = 4;
        grid.num_points.v_dim = 4;
        grid.points.point3d = cpts;

	uknots.num_floats = grid.num_points.u_dim + uorder;
	uknots.floats = kpts;

	vknots.num_floats = grid.num_points.v_dim + vorder;
	vknots.floats = kpts;

	pset_surf_approx(curve_approx_type, 
			 ucurve_approx_tolerance, vcurve_approx_tolerance);

	pset_int_style(PSTYLE_SOLID);

        pset_view_ind(1);
	pset_int_colr_ind(2);

	pnuni_bsp_surf( uorder, vorder,
			&uknots, &vknots,
			rationality, 
			&grid, 
			1, &trim_curve_list);

	pt.x =  0.5; pt.y = 0.5; pt.z = 0.5;
	shift.delta_x = 0; shift.delta_y = 0; shift.delta_z = 0;
	scale.delta_x = 1.0; scale.delta_y = 1.0; scale.delta_z = 1.0;
	x_ang = 2.0*PI/4.0; y_ang = 0.0; z_ang = 0.0;

	pbuild_tran_matrix3(	&pt, &shift, x_ang, y_ang, z_ang,
				&scale, &err, bldmat);
	pset_local_tran3( bldmat, PTYPE_REPLACE);

        pset_view_ind(2);
	pset_int_colr_ind(3);

	pnuni_bsp_surf( uorder, vorder,
			&uknots, &vknots,
			rationality, 
			&grid, 
			1, &trim_curve_list);

        pset_view_ind(3);
	pset_int_colr_ind(4);

	pnuni_bsp_surf( uorder, vorder,
			&uknots, &vknots,
			rationality, 
			&grid, 
			1, &trim_curve_list);

        pset_view_ind(4);
	pset_int_colr_ind(5);

	pnuni_bsp_surf( uorder, vorder,
			&uknots, &vknots,
			rationality, 
			&grid, 
			1, &trim_curve_list);

        ppost_struct(1,1,1.0);

        printf("should now see some patches...");
        getchar();
}

