/* $XConsortium: wheels.c,v 5.2 91/04/02 17:13:00 hersh Exp $ */

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

/**********************************************************************/
/* an example of a structure network, with variations showing
 * the use of transformations, editing structures and using bundles
/**********************************************************************/
#define A	/* use this to define transformations */
#define B	/* use this to include structure edits */
#if 0
#define C	/* use this to use bundle table */
#endif

#include "phigs/phigs.h"
#include "colors.h"

#define PI 3.14159265358

#define PICSTRUCT 1
#define MERRYGOROUNDSTRUCT 2
#define FOURWHEELSSTRUCT 3
#define SPINNINGWHEELSTRUCT 4
#define WHEELSTRUCT 5

/**********************************************************************/
/* Structure Definitions                                              */
/**********************************************************************/

DefinePicture()
{
    popen_struct(PICSTRUCT);
	pset_view_ind(1);		/* set the view representation */
	pexec_struct(MERRYGOROUNDSTRUCT);
    pclose_struct();
}

/**********************************************************************/
/* Add a structure element containing a 3-D local modelling           */
/* transformation matrix.  This transformation can just be an identity*/
/* matrix (e.g. just rotate 0 degrees around the y axis).  This       */
/* structure element should be before the EXECUTE STRUCTURE structure */
/* element.                                                           */
/**********************************************************************/

DefineMerryGoRound()
{
    Pvec3 p;
    Pmatrix3 trans;
    Pint err;
    popen_struct(MERRYGOROUNDSTRUCT);
#ifdef B
	protate_y(0.0, &err, trans);
	pset_local_tran3(trans, PTYPE_REPLACE);
#endif
	pexec_struct(FOURWHEELSSTRUCT);
    pclose_struct();
}

#ifdef C
SetBundle()
{
    Pline_bundle rep;

    rep.type = PLINE_SOLID;
    rep.width = 1.0;
    rep.colr_ind = MAGENTA;
    pset_line_rep( 1, 1, &rep );

    rep.type = PLINE_DASH;
    rep.colr_ind = CYAN;
    pset_line_rep( 1, 2, &rep );

    rep.type = PLINE_SOLID;
    rep.colr_ind = RED;
    pset_line_rep( 1, 3, &rep );

    rep.type = PLINE_DOT_DASH;
    rep.colr_ind = YELLOW;
    pset_line_rep( 1, 4, &rep );
}
#endif

Define4Wheels()
{
    Pvec3 p;
    Pmatrix3 trans;
    Pint err;

    popen_struct(FOURWHEELSSTRUCT);
#ifdef A
	p.delta_x = 10.0;
	p.delta_y = 0.0;
	p.delta_z = 0.0;
	ptranslate3(&p, &err, trans);
	pset_local_tran3(trans, PTYPE_REPLACE);
#endif
#ifdef C
        SetBundle();
        pset_line_ind(1);
#endif
	pexec_struct(SPINNINGWHEELSTRUCT);

#ifdef A
	p.delta_x = -10.0;
	p.delta_y = 0.0;
	p.delta_z = 0.0;
	ptranslate3(&p, &err, trans);
	pset_local_tran3(trans, PTYPE_REPLACE);
#endif
#ifdef D
        pset_line_ind(2);
#endif
	pexec_struct(SPINNINGWHEELSTRUCT);

#ifdef A
	p.delta_x = 0.0;
	p.delta_y = 0.0;
	p.delta_z = -10.0;
	ptranslate3(&p, &err, trans);
	pset_local_tran3(trans, PTYPE_REPLACE);
#endif
#ifdef D
        pset_line_ind(3);
#endif
	pexec_struct(SPINNINGWHEELSTRUCT);

#ifdef A
	p.delta_x = 0.0;
	p.delta_y = 0.0;
	p.delta_z = 10.0;
	ptranslate3(&p, &err, trans);
	pset_local_tran3(trans, PTYPE_REPLACE);
#endif
#ifdef D
        pset_line_ind(4);
#endif
	pexec_struct(SPINNINGWHEELSTRUCT);
    pclose_struct();
}

/**********************************************************************/
/* Add a structure element containing a 3-D local modelling           */
/* transformation matrix.  This transformation can just be an identity*/
/* matrix (e.g. just rotate 0 degrees around the y axis).  This       */
/* structure element should be before the EXECUTE STRUCTURE structure */
/* element.                                                           */
/**********************************************************************/

DefineSpinningWheel()
{
    Pvec3 p;
    Pmatrix3 trans;
    Pint err;
    popen_struct(SPINNINGWHEELSTRUCT);
#ifdef B
	protate_y(0.0, &err, trans);
	pset_local_tran3(trans, PTYPE_REPLACE);
#endif
	pexec_struct(WHEELSTRUCT);
    pclose_struct();
}

DefineWheel()
{
    Ppoint_list3 ppts;
    Ppoint3 pts[9];
    Pmatrix3 trans;
    Pint err;

    ppts.num_points = 9;
    ppts.points = pts;
    pts[0].x = -1.5;     pts[0].y = -3.6213;    pts[0].z = 0.0;
    pts[1].x = 1.5;      pts[1].y = -3.6213;    pts[1].z = 0.0;
    pts[2].x = 3.6213;   pts[2].y = -1.5;       pts[2].z = 0.0;
    pts[3].x = 3.6213;   pts[3].y = 1.5;        pts[3].z = 0.0;
    pts[4].x = 1.5;      pts[4].y = 3.6213;     pts[4].z = 0.0;
    pts[5].x = -1.5;     pts[5].y = 3.6213;     pts[5].z = 0.0;
    pts[6].x = -3.6213;  pts[6].y = 1.5;        pts[6].z = 0.0;
    pts[7].x = -3.6213;  pts[7].y = -1.5;       pts[7].z = 0.0;
    pts[8].x = -1.5;     pts[8].y = -3.6213;    pts[8].z = 0.0;
    popen_struct(WHEELSTRUCT);

#ifdef C
        pset_indiv_asf(PASPECT_LINETYPE, PASF_BUNDLED);
        pset_indiv_asf(PASPECT_LINE_COLR_IND, PASF_BUNDLED);
        pset_linewidth( 2.0 );
#endif

	protate_x( PI/2, &err, trans );
	pset_local_tran3( trans, PTYPE_REPLACE );
    	pset_line_colr_ind( MAGENTA );

	ppolyline3( &ppts );

#ifdef C
        pset_indiv_asf(PASPECT_LINETYPE, PASF_INDIV);
        pset_indiv_asf(PASPECT_LINE_COLR_IND, PASF_INDIV);
#endif
	ppts.num_points = 2;
	pts[1] = pts[4];
    	pset_line_colr_ind( RED );
	ppolyline3( &ppts );

	pts[0].x = 1.5; pts[0].y = -3.6213; pts[0].z = 0.0;
	pts[1] = pts[5];
    	pset_line_colr_ind( GREEN );
	ppolyline3( &ppts );

	pts[0] = pts[2];
	pts[1] = pts[6];
    	pset_line_colr_ind( BLUE );
	ppolyline3( &ppts );

	pts[0] = pts[3];
	pts[1] = pts[7];
    	pset_line_colr_ind( YELLOW );
	ppolyline3( &ppts );
    pclose_struct();
}

/**********************************************************************/
/* View Information                                                   */
/**********************************************************************/

DefineAViewRep()
{
    static Ppoint3  vrp;
    static Pvec3 vpn;
    static Pvec3 vup;
    Pview_map3 viewmap;
    Pint	err;
    Pview_rep3	vrep;
    
    vrp.x = 23.0; vrp.y = 23.0; vrp.z = 23.0;
    vpn.delta_x = 1.0; vpn.delta_y = 1.0; vpn.delta_z = 1.0;
    vup.delta_x = 0.0; vup.delta_y = 1.0; vup.delta_z = 0.0;

    peval_view_ori_matrix3(&vrp, &vpn, &vup, &err,
	vrep.ori_matrix);

    viewmap.win.x_min = -30.0; viewmap.win.x_max = 30.0;
    viewmap.win.y_min = -30.0; viewmap.win.y_max = 30.0;
    viewmap.view_plane = 0.0;
    viewmap.back_plane = -60.0;
    viewmap.front_plane = 30.0;
    viewmap.proj_ref_point.x = 0.0; viewmap.proj_ref_point.y = 0.0; viewmap.proj_ref_point.z = 35.0;
    viewmap.proj_type = PTYPE_PARAL;
    viewmap.vp.x_min = 0.0; viewmap.vp.x_max = 1.0;
    viewmap.vp.y_min = 0.0; viewmap.vp.y_max = 1.0;
    viewmap.vp.z_min = 0.0; viewmap.vp.z_max = 1.0;

    peval_view_map_matrix3(&viewmap, &err, vrep.map_matrix);

    vrep.clip_limit = viewmap.vp;
    vrep.xy_clip = vrep.back_clip = vrep.front_clip = PIND_NO_CLIP;
    pset_view_rep3(1, 1, &vrep);
}

/**********************************************************************/
/* Main                                                               */
/**********************************************************************/

main()
{
    int i;
    Pint err;
    Pmatrix3 trans;

    popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE);	/* open phigs */
    popen_ws(1, (Pconnid)NULL, phigs_ws_type_x_tool);  /* open work- */
                                                        /* station 1  */
    DefineAViewRep();

    DefinePicture();
    DefineMerryGoRound();
    Define4Wheels();
    DefineSpinningWheel();
    DefineWheel();

    ppost_struct(1,PICSTRUCT,1.0);

#ifdef B
#ifndef C
    pset_edit_mode(PEDIT_REPLACE);  /* replace structure elements */

/**********************************************************************/
/* Create a for-loop that iterates about 100 times.                  */
/* In the for-loop,                                                   */
/*	Open the SPINNINGWHEELSTRUCT and change the first (1)         */
/*	    structure element to contain a new transformation matrix. */
/*	    Rotate around the y-axis so that the wheel will spin.     */
/*	Open the MERRYGOROUNDSTRUCT and change the first (1)          */
/*	    structure element to contain a new transformation matrix. */
/*	    rotate around the y-axis so that all four tops will spin  */
/*	    in a circle.                                              */
/**********************************************************************/

    for (i = 0; i < 100; i++) {
	popen_struct(SPINNINGWHEELSTRUCT);
	    pset_elem_ptr(1);
	    protate_y(i*PI/10, &err, trans);
	    pset_local_tran3(trans, PTYPE_REPLACE);
	pclose_struct();
	popen_struct(MERRYGOROUNDSTRUCT);
	    pset_elem_ptr(1);
	    protate_y(-i*PI/20, &err, trans);
	    pset_local_tran3(trans, PTYPE_REPLACE);
	pclose_struct();
    }
#endif
#endif

    sleep(10);
    pclose_ws(1);			/* close the workstation */
    pclose_phigs();			/* close phigs           */

}

/**********************************************************************/
/* Read this whole program and make sure you understand how and why   */
/* it works.                                                          */
/**********************************************************************/
