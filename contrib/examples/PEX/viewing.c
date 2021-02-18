/* $XConsortium: viewing.c,v 5.1 91/02/16 09:32:18 rws Exp $ */

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

/* an example of how changes to the view data affect a scene */

/*  define one of these for each example */

#define EXAMPLE_A /* a parallel view of a cube */
#if 0
#define EXAMPLE_B /* a perspective view of a cube */
#define EXAMPLE_C /* a closer view of a cube */
#endif

#include <phigs/phigs.h>
#include "colors.h"

/**********************************************************************/
/* Structure Definitions                                              */
/**********************************************************************/

DefineCube()
{
    Ppoint3 pts[7];
    Ppoint_list3 colored_line;

    /* since we don't have HLHSR, really only define the front
     * of a cube.
     */
    pts[0].x = 0.0; pts[0].y = 0.0; pts[0].z = 0.0;
    pts[1].x = 0.0; pts[1].y = 24.0; pts[1].z = 0.0;
    pts[2].x = 0.0; pts[2].y = 24.0; pts[2].z = -24.0;
    pts[3].x = 24.0; pts[3].y = 24.0; pts[3].z = -24.0;
    pts[4].x = 24.0; pts[4].y = 0.0; pts[4].z = -24.0;
    pts[5].x = 24.0; pts[5].y = 0.0; pts[5].z = 0.0;
    pts[6].x = 0.0; pts[6].y = 0.0; pts[6].z = 0.0;

    colored_line.points = pts;

    popen_struct(1);
	pset_view_ind(1);		/* set the view representation */
    	pset_line_colr_ind( GREEN ); 
        pset_linewidth( 3.0 );
	colored_line.num_points = 7;
	ppolyline3(&colored_line);	/* draw ! */

	pts[0].x = 24.0; pts[0].y = 24.0; pts[0].z = 0.0;
	pts[1].x = 0.0; pts[1].y = 24.0; pts[1].z = 0.0;
	colored_line.num_points = 2;
	ppolyline3(&colored_line);

	pts[1].x = 24.0; pts[1].y = 24.0; pts[1].z = -24.0;
        colored_line.num_points = 2;	/* not really necessary */
        ppolyline3(&colored_line); 
 
	pts[1].x = 24.0; pts[1].y = 0.0; pts[1].z = 0.0;
        colored_line.num_points = 2;    /* not really necessary */
        ppolyline3(&colored_line); 
 
    pclose_struct();
}

/**********************************************************************/
/* View Information                                                   */
/**********************************************************************/

DefineAViewRep()
{
	/* In World Coordinates */

    static Ppoint3  vrp = {23.0, 23.0, 0.0};	/* view reference point */
    static Pvec3 vpn = {1.0, 1.0, 1.0};	/* view plane normal    */
    static Pvec3 vup = {0.0, 1.0, 0.0};	/* view up vector       */

	/* In VR Coordinates */

    static Pview_map3 viewmap = {
	{-30.0, 30.0, -30.0, 30.0}, 		/* window */
	{0.0, 1.0, 0.0, 1.0, 0.0, 1.0},	 	/* viewport */
#ifdef EXAMPLE_A
	PTYPE_PARAL, 				/* viewport type */
	{0.0, 0.0, 45.0}, 			/* prp */
	0.0, 					/* view plane dist */
#endif
#ifdef EXAMPLE_B
	PTYPE_PERSPECT, 			/* viewport type */
	{0.0, 0.0, 45.0}, 			/* prp */
	0.0, 					/* view plane dist */
#endif
#ifdef EXAMPLE_C
	PTYPE_PERSPECT, 			/* viewport type */
	{0.0, 0.0, 45.0}, 			/* prp */
	-40.0, 					/* view plane dist */
#endif
#ifdef EXAMPLE_D
	PTYPE_PERSPECT, 			/* viewport type */
	{0.0, 0.0, 400.0}, 			/* prp */
	0.0, 					/* view plane dist */
#endif
	-40.0, 					/* back plane dist */
	30.0 					/* front plane dist */
    };
    Pint	err;
    Pview_rep3	vrep;
    
    peval_view_ori_matrix3(&vrp, &vpn, &vup, &err,
	vrep.ori_matrix);
    if (err) {
	fprintf(stderr, "error from eval orientation %d\n", err);
	exit(3);
    }
    peval_view_map_matrix3(&viewmap, &err, vrep.map_matrix);
    if (err) {
	fprintf(stderr, "error from eval mapping %d\n", err);
	exit(4);
    }
    vrep.clip_limit = viewmap.vp;
    vrep.xy_clip = vrep.back_clip = vrep.front_clip = PIND_CLIP;
    pset_view_rep3(1, 1, &vrep);
}

/**********************************************************************/
/* Main                                                               */
/**********************************************************************/

main()
{
    popen_phigs((char*)NULL, PDEF_MEM_SIZE);	/* open phigs */
    popen_ws(1,NULL,phigs_ws_type_x_tool);
                                                        /* station 1  */
    DefineAViewRep();

    DefineCube();

    ppost_struct(1,1,1.0);		/* post struct 1         */

    sleep(10);
    pclose_ws(1);			/* close the workstation */
    pclose_phigs();			/* close phigs           */

}



