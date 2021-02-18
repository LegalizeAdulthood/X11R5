/* $XConsortium: surface.c,v 5.1 91/02/16 09:32:32 rws Exp $ */

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

/* a simple program to draw a bspline surface */

#include "phigs/phigs.h"
main(argc,argv)
        int argc; char  ** argv;
{
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

	Pint		curve_approx_type
				= PCURV_CONSTANT_PARAMETRIC_BETWEEN_KNOTS;
	Pfloat		ucurve_approx_tolerance = 10.0,
			vcurve_approx_tolerance = 10.0;

        popen_phigs(NULL,0);
        popen_ws(1,NULL,phigs_ws_type_x_tool);
        popen_struct(1);
        ppost_struct(1,1,1.0);

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
	pset_int_colr_ind(3);

	pnuni_bsp_surf( uorder, vorder,
			&uknots, &vknots,
			rationality, 
			&grid, 
			0, NULL);

        printf("should now see a patch...");
        getchar();
}

