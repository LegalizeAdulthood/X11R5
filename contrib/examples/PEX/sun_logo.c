/* $XConsortium: sun_logo.c,v 5.1 91/02/16 09:32:31 rws Exp $ */

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
/* coordlogo.c                                                        */
/**********************************************************************/

#include <phigs/phigs.h>

#define PI 3.14159265358

/**********************************************************************/
/* Structure Definitions                                              */
/**********************************************************************/

DefineAxes()
{
    Ppoint3 pts[2];
    Pvec3 dir[2];
    Ppoint_list3 colored_line;

    pts[0].x = pts[0].y = pts[0].z = 0.0;
    pts[1].x = pts[1].y = pts[1].z = 0.0;

    /* x direction */
    dir[0].delta_x = 1.0; 
    dir[0].delta_y = 0.0; 
    dir[0].delta_z = 0.0;   

    /* y direction */
    dir[1].delta_x = 0.0; 
    dir[1].delta_y = 1.0; 
    dir[1].delta_z = 0.0;   

    colored_line.num_points = 2;       /* single line */
    colored_line.points = pts;

    popen_struct(100);
	pset_view_ind(1);
	pset_text_colr_ind(4); /* blue */
	pset_char_ht(1.0);
/*	pset_text_font(PFONT_ROMAN_SIMPLEX); */
	pset_line_colr_ind(4); /* blue */
	pts[1].x = 15.0;
	ppolyline3(&colored_line);
	pts[1].x = 15.5;
	pts[1].y = -0.5;
	ptext3(&pts[1], dir, "X");
	pts[1].x = 0.0;
	pts[1].y = 15.0;
        ppolyline3(&colored_line);
	ptext3(&pts[1], dir, "Y");
	pts[1].y = 0.0;
	pts[1].z = 15.0;
        ppolyline3(&colored_line);
	dir[0].delta_x = 0.0; dir[0].delta_z = -1.0; 
	dir[1].delta_x = 0.0; dir[1].delta_y = 1.0;
	pts[1].z = 16.0;
	pts[1].y = -0.5;
	ptext3(&pts[1], dir, "Z");
    pclose_struct();
}

DefineObject()
{
    popen_struct(1);
	pset_view_ind(1);
    	pexec_struct(5);
    pclose_struct();
}

DefineU()
{
    Ppoint3 pts[11];
    Pvec3 p;
    Pmatrix3 trans;
    Pint err;
    Ppoint_list3 colored_line;

    pts[0].x = -1.5; pts[0].y = -5.5; pts[0].z = 0.0;
    pts[1].x = 1.5; pts[1].y = -5.5; pts[1].z = 0.0;
    pts[2].x = 2.5; pts[2].y = -4.5; pts[2].z = 0.0;
    pts[3].x = 2.5; pts[3].y = 5.5; pts[3].z = 0.0;
    pts[4].x = 0.5; pts[4].y = 5.5; pts[4].z = 0.0;
    pts[5].x = 0.5; pts[5].y = -3.5; pts[5].z = 0.0;
    pts[6].x = -0.5; pts[6].y = -3.5; pts[6].z = 0.0;
    pts[7].x = -0.5; pts[7].y = 5.5; pts[7].z = 0.0;
    pts[8].x = -2.5; pts[8].y = 5.5; pts[8].z = 0.0;
    pts[9].x = -2.5; pts[9].y = -4.5; pts[9].z = 0.0;
    pts[10].x = -1.5; pts[10].y = -5.5; pts[10].z = 0.0;

    colored_line.num_points = 11;
    colored_line.points = pts;


    p.delta_x = 2.5;
    p.delta_y = 5.5;
    p.delta_z = 0.0;
    ptranslate3(&p, &err, trans);
    popen_struct(2);
	pset_local_tran3(trans, PTYPE_REPLACE);
        ppolyline3(&colored_line); 
    pclose_struct();
}

DefineUAStruct()
{   
    Pmatrix3	trans;
    Pvec3	p;
    Pint	err; 
    popen_struct(3);
	pset_line_colr_ind(1); /* white */
    	pexec_struct(2);
    	pset_line_colr_ind(7); /* magenta */
	protate_z(PI, &err, trans);
	pset_local_tran3(trans, PTYPE_REPLACE);
	p.delta_x = -11.0;
	p.delta_y = -11.0;
	p.delta_z = 0.0;
	ptranslate3(&p, &err, trans);
	pset_local_tran3(trans, PTYPE_PRECONCAT);
	pexec_struct(2);
    pclose_struct();
}

DefineLogoStruct()
{
    Pmatrix3	trans;
    Pvec3	p;
    Pint	err; 

    popen_struct(4);
	pexec_struct(3);	/* lower left */

	protate_z(-PI/2, &err, trans);
	pset_local_tran3(trans, PTYPE_PRECONCAT);
	p.delta_x = -11.0;
	p.delta_y = 12.0;
	p.delta_z = 0.0;
	ptranslate3(&p, &err, trans);
	pset_local_tran3(trans, PTYPE_PRECONCAT);
	pexec_struct(3);	/* lower right */

	protate_z(PI, &err, trans);
	pset_local_tran3(trans, PTYPE_REPLACE);
	p.delta_x = -23.0;
	p.delta_y = -23.0;
	p.delta_z = 0.0;
	ptranslate3(&p, &err, trans);
	pset_local_tran3(trans, PTYPE_PRECONCAT);
	pexec_struct(3);	/* upper right */

	protate_z(PI/2, &err, trans);
	pset_local_tran3(trans, PTYPE_REPLACE);
	p.delta_x = 12.0;
	p.delta_y = -11.0;
	p.delta_z = 0.0;
	ptranslate3(&p, &err, trans);
	pset_local_tran3(trans, PTYPE_PRECONCAT);
	pexec_struct(3);	/* upper left */
    pclose_struct();
}

DefineCubeStruct() 
{   
    Pmatrix3	rot, trans, composite;
    Pvec3	p;
    Pint	err;

    protate_y(PI/2, &err, rot);        /* just to be different */
    p.delta_x = 23.5;
    p.delta_y = 0.0;
    p.delta_z = -0.5;
    ptranslate3(&p, &err, trans);
    pcompose_matrix3(trans, rot, &err, composite); /* just to be different */
    
    popen_struct(5);
	pexec_struct(4);			/* left face */
    	pset_local_tran3(composite, PTYPE_REPLACE);
    	pexec_struct(4);			/* right face */
	protate_x(-PI/2, &err, trans);
	pset_local_tran3(trans, PTYPE_REPLACE);
	p.delta_x = 0.0;
	p.delta_y = .5;
	p.delta_z = 23.5;
	ptranslate3(&p, &err, trans);
	pset_local_tran3(trans, PTYPE_PRECONCAT);
	pexec_struct(4);
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
    
    vrp.x = 23.0; vrp.y = 23.0; vrp.z = 0.0;
    vpn.delta_x = 1.0; vpn.delta_y = 1.0; vpn.delta_z = 1.0;
    vup.delta_x = 0.0; vup.delta_y = 1.0; vup.delta_z = 0.0;
    peval_view_ori_matrix3(&vrp, &vpn, &vup, &err,
	vrep.ori_matrix);

    viewmap.win.x_min = -30.0; viewmap.win.x_max = 30.0;
    viewmap.win.y_min = -30.0; viewmap.win.y_max = 30.0;

    viewmap.view_plane = 0.0;
    viewmap.back_plane = -40.0;
    viewmap.front_plane = 30.0;

    viewmap.proj_ref_point.x = 0.0; 
    viewmap.proj_ref_point.y = 0.0; 
    viewmap.proj_ref_point.z = 45.0;
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
    popen_phigs(PDEF_ERR_FILE, PDEF_MEM_SIZE);	/* open phigs */
    popen_ws(1,NULL,phigs_ws_type_x_tool); 	/* station 1  */
    DefineAViewRep();

    DefineAxes();
    DefineObject();
    DefineU();
    DefineUAStruct();
    DefineLogoStruct();
    DefineCubeStruct();

    ppost_struct(1,100,1.0);
    ppost_struct(1,1,1.0);		/* post struct 1         */

    sleep(10);
    pclose_ws(1);			/* close the workstation */
    pclose_phigs();			/* close phigs           */

}



