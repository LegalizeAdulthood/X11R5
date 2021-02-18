/* $XConsortium: curve.c,v 5.1 91/02/16 09:32:16 rws Exp $ */

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

/* simple program to draw a curve */

#include "phigs/phigs.h"
main(argc,argv)
        int argc; char  ** argv;
{
        Pint            err;
        
	Pint 		order = 4;
	Pfloat_list 	knots;
        static int 	numknots = 20;
        static Pfloat 	kpts[] = {  1.0,  2.0,  3.0,  4.0,  5.0, 
				    6.0,  7.0,  8.0,  9.0, 10.0,
				   11.0, 12.0, 13.0, 14.0, 15.0,
				   16.0, 17.0, 18.0, 19.0, 20.0 };
	Prational 	rationality = PNON_RATIONAL;
	Ppoint_list34	cpoints;
        static Ppoint3 	cpts[] = {
				        0.1,0.5,0.0, 
			       		0.2,0.7,0.0,
			       		0.3,0.3,0.0,
			       		0.4,0.7,0.0,
			       		0.5,0.3,0.0,
			       		0.6,0.7,0.0,
			       		0.7,0.3,0.0,
			       		0.8,0.7,0.0,
			       		0.9,0.5,0.0,
			        };
	Pfloat		min = 5.0,
			max = 9.0;

	Pint		curve_approx_type
				= PCURV_CONSTANT_PARAMETRIC_BETWEEN_KNOTS;
	Pfloat		curve_approx_tolerance = 10.0;

        popen_phigs(NULL,0);
        popen_ws(1,NULL,phigs_ws_type_x_tool);

        popen_struct(1);
	     cpoints.num_points = 9;
	     cpoints.points.point3d = cpts;

	     knots.num_floats = cpoints.num_points + order;
	     knots.floats = kpts;

	     pset_line_colr_ind(2);
	     pset_curve_approx(curve_approx_type, curve_approx_tolerance);
	     pnuni_bsp_curv(order, &knots, rationality, &cpoints, min, max);
	pclose_struct(1);
        ppost_struct(1,1,1.0);

        printf("should now see a curve...");
        getchar();
}

