/* $XConsortium: pickmore.c,v 5.1 91/02/16 09:32:42 rws Exp $ */

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

/**
***  Sample pick program to test picking of Cell Arrays, Nurb Surfaces 
***  and SOFAS.
***/

#include "phigs/phigs.h"

#define ROWS   5 
#define COLUMNS 7 

static Plimit viewport = {100.0,500.0,100.0,500.0};
static Ppoint points[5] = {0.01,0.01, 0.01,0.99, 0.99,0.99, 0.99,0.01, 0.01,0.01};
static Ppoint_list   line;
static   Plimit            ea={100.0, 500.0, 100.0, 500.0 };

/* Cell Array Data */
	Ppat_rep	color_grid; 
	Pint           *colorptr;
 
static Pparal 		outline =
				{0.1, 0.5, 0.1,
				 0.5, 0.6, 0.1,
				 0.1, 0.9, 0.1};  

/* Surface Data */
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

static Ppoint3 	cpts[] = {0.2,0.5,0.2, 0.4,0.5,0.2, 0.6,0.5,0.2, 0.8,0.5,0.2,
                          0.2,0.5,0.4, 0.4,0.7,0.4, 0.6,0.3,0.4, 0.8,0.5,0.4,
                          0.2,0.5,0.6, 0.4,0.3,0.6, 0.6,0.7,0.6, 0.8,0.5,0.6,
                          0.2,0.5,0.8, 0.4,0.5,0.8, 0.6,0.5,0.8, 0.8,0.5,0.8};

	Pint		curve_approx_type
				= PCURV_CONSTANT_PARAMETRIC_BETWEEN_KNOTS;
	Pfloat		ucurve_approx_tolerance = 10.0,
			vcurve_approx_tolerance = 10.0;

/* SOFAS Data */
static Ppoint3 sofa_data[] = {0.2, 0.2, 0.0, 	/* 0 */
                              0.4, 0.2, 0.0,	/* 1 */
                              0.6, 0.2, 0.0,	/* 2 */
                              0.8, 0.2, 0.0,	/* 3 */
                              0.2, 0.4, 0.0,	/* 4 */
                              0.4, 0.4, 0.0,	/* 5 */
                              0.6, 0.4, 0.0,	/* 6 */
                              0.8, 0.4, 0.0,	/* 7 */
                              0.2, 0.6, 0.0,	/* 8 */
                              0.4, 0.6, 0.0,	/* 9 */
                              0.6, 0.6, 0.0,	/*10 */
                              0.8, 0.6, 0.0,	/*11 */
                              0.2, 0.8, 0.0,	/*12 */
                              0.4, 0.8, 0.0,	/*13 */
                              0.6, 0.8, 0.0,	/*14 */
                              0.8, 0.8, 0.0 };	/*15 */
    Pint            fflag = PFACET_NONE; /* data specified per facet */
    Pint            eflag = PEDGE_NONE; /* data specified per edge */
    Pint            vflag = PVERT_COORD; /* data specified per vertex */
    Pint            colour_model = PINDIRECT; /* colour model */
    Pint            num_sets = 5;             /* number of fill area sets */
    Pfacet_data_arr3     *fdata;         /* facet data */
    Pedge_data_list_list *edata;         /* array of L(L(E)) edge data */
    Pint_list_list       vlistlist[5];  /* array of L(L(I)) vertex indices */
    Pint_list		 vlist[5];		
static Pint		 v0[]= { 0,  1,  5,  4 };
static Pint		 v1[]= { 2,  3,  7,  6 };
static Pint		 v2[]= { 5,  6, 10,  9 };
static Pint		 v3[]= { 8,  9, 13, 12 };
static Pint		 v4[]= {10, 11, 15, 14 };
    Pfacet_vdata_list3   vdata;         /* facet vertex data */

static Pview_rep viewrep = {
		{1.0,0.0,0.0},
		{0.0,1.0,0.0},
		{0.0,0.0,1.0},

		{1.0,0.0,0.0},
		{0.0,1.0,0.0},
		{0.0,0.0,1.0},

		{0.75,1.0,0.75,1.0},
		PIND_CLIP
};
        

    Ppoint3 p[5];
    Pvec3 xlv;
    Pmatrix3 mx;
    Pvec3 scale;
    Pvec3 shift;
    Pint err;
    Pfloat xang, yang, zang;

main( argc, argv )
    int		argc;
    char	*argv[];
{
    Pin_status		status;
    Pint		pet, err, names[5];
    Ppick_data          drec;   
    Ppick_path		path;
    Ppick_path_elem     path_els[2];  
    Pfilter		filt;
    int                 i;

    /* Set up line */

    line.num_points = 5;
    line.points = &points[0];

    /* Initialize PHIGS and start up */

    popen_phigs( (char *)NULL, PDEF_MEM_SIZE );

    /* Open a X-tool workstation */

    popen_ws( 1, (char *)NULL, phigs_ws_type_x_tool );
    pset_ws_vp( 1, &viewport );

    /* Initialize Cell Array Data */

    color_grid.dims.size_x = COLUMNS; 
    color_grid.dims.size_y = ROWS; 

    color_grid.colr_array = (Pint *)malloc(ROWS*COLUMNS*sizeof(Pint));
    colorptr = color_grid.colr_array;

    for (i=0; i<ROWS*COLUMNS; i++, colorptr++) {
	*colorptr = 0;
    }
 
    /* Initialize the SOFAS data */

    fdata = NULL;
    edata = NULL;

    /* Initialize the surface data */

    grid.num_points.u_dim = 4;
    grid.num_points.v_dim = 4;
    grid.points.point3d = cpts;

    uknots.num_floats = grid.num_points.u_dim + uorder;
    uknots.floats = kpts;

    vknots.num_floats = grid.num_points.v_dim + vorder;
    vknots.floats = kpts;

    /* Set up the view 1 for use later */

    pset_view_rep(1,1,&viewrep);

    /* Build the CSS */

    popen_struct( 1 );
	plabel( 3 );
	pset_pick_id( 4 );
	pexec_struct( 2 );
    pclose_struct();
    popen_struct( 2 );
	names[0] = 1;
	filt.incl_set.num_ints = 1;
	filt.incl_set.ints = names;
	padd_names_set( &filt.incl_set );
        pset_line_colr_ind(5);  /* Set the line color to yellow */
	ppolyline( &line );     /* #3 */ 

	pset_surf_approx(curve_approx_type, 
			 ucurve_approx_tolerance, vcurve_approx_tolerance);

	pset_int_style(PSTYLE_SOLID);
	pset_int_colr_ind(4);

	pnuni_bsp_surf( uorder, vorder,
			&uknots, &vknots,
			rationality, 
			&grid, 
			0, NULL);   /* #7 */ 

    /*
     * third quadrant - hollow
     */
    vlistlist[0].num_lists = 1; vlistlist[0].lists = &vlist[0];
    vlist[0].num_ints = 4;   vlist[0].ints = v0;
    vlistlist[1].num_lists = 1; vlistlist[1].lists = &vlist[1];
    vlist[1].num_ints = 4;   vlist[1].ints = v1;
    vlistlist[2].num_lists = 1; vlistlist[2].lists = &vlist[2];
    vlist[2].num_ints = 4;   vlist[2].ints = v2;
    vlistlist[3].num_lists = 1; vlistlist[3].lists = &vlist[3];
    vlist[3].num_ints = 4;   vlist[3].ints = v3;
    vlistlist[4].num_lists = 1; vlistlist[4].lists = &vlist[4];
    vlist[4].num_ints = 4;   vlist[4].ints = v4;

    vdata.num_vertices = 16; vdata.vertex_data.points = sofa_data;

    xlv.delta_x = 0.5; xlv.delta_y = 0.5; xlv.delta_z = 0.0;
    scale.delta_x = 0.5; scale.delta_y = 0.5; scale.delta_z = 1.0;         
    xang = 0.0; yang = 0.0; zang = 0.0; 
    shift.delta_x = -0.25; shift.delta_y = -0.25; shift.delta_z = 0;

    pbuild_tran_matrix3( &xlv, &shift, xang, yang, zang,
			 &scale, &err, mx);
        pset_local_tran3(mx, PTYPE_REPLACE);
	pset_int_colr_ind(6);
	pset_int_style(PSTYLE_HOLLOW);
        pset_of_fill_area_set3_data( fflag, eflag, vflag, colour_model, 
				     num_sets,
				     fdata, edata, vlistlist, &vdata );
                                   /* #11 */
    /*
     * fourth quadrant - solid
     */
    vlistlist[0].num_lists = 1; vlistlist[0].lists = &vlist[0];
    vlist[0].num_ints = 4;   vlist[0].ints = v0;
    vlistlist[1].num_lists = 1; vlistlist[1].lists = &vlist[1];
    vlist[1].num_ints = 4;   vlist[1].ints = v1;
    vlistlist[2].num_lists = 1; vlistlist[2].lists = &vlist[2];
    vlist[2].num_ints = 4;   vlist[2].ints = v2;
    vlistlist[3].num_lists = 1; vlistlist[3].lists = &vlist[3];
    vlist[3].num_ints = 4;   vlist[3].ints = v3;
    vlistlist[4].num_lists = 1; vlistlist[4].lists = &vlist[4];
    vlist[4].num_ints = 4;   vlist[4].ints = v4;

    vdata.num_vertices = 16; vdata.vertex_data.points = sofa_data;

    xlv.delta_x = 0.5; xlv.delta_y = 0.5; xlv.delta_z = 0.0;
    scale.delta_x = 0.5; scale.delta_y = 0.5; scale.delta_z = 1.0;         
    xang = 0.0; yang = 0.0; zang = 0.0; 
    shift.delta_x = 0.25; shift.delta_y = -0.25; shift.delta_z = 0;

    pbuild_tran_matrix3( &xlv, &shift, xang, yang, zang,
			 &scale, &err, mx);

        pset_local_tran3(mx, PTYPE_REPLACE);
        pset_edgetype(PSTYLE_SOLID);
        pset_edge_colr_ind(3);
        pset_edgewidth(2.0);
        pset_edge_flag(PEDGE_ON);
	pset_int_colr_ind(2);
	pset_int_style(PSTYLE_SOLID);
        pset_of_fill_area_set3_data( fflag, eflag, vflag, colour_model, 
				     num_sets,
				     fdata, edata, vlistlist, &vdata );
                                   /* #19 */
    /*
     * first quadrant - clipped
     */
    vlistlist[0].num_lists = 1; vlistlist[0].lists = &vlist[0];
    vlist[0].num_ints = 4;   vlist[0].ints = v0;
    vlistlist[1].num_lists = 1; vlistlist[1].lists = &vlist[1];
    vlist[1].num_ints = 4;   vlist[1].ints = v1;
    vlistlist[2].num_lists = 1; vlistlist[2].lists = &vlist[2];
    vlist[2].num_ints = 4;   vlist[2].ints = v2;
    vlistlist[3].num_lists = 1; vlistlist[3].lists = &vlist[3];
    vlist[3].num_ints = 4;   vlist[3].ints = v3;
    vlistlist[4].num_lists = 1; vlistlist[4].lists = &vlist[4];
    vlist[4].num_ints = 4;   vlist[4].ints = v4;

    vdata.num_vertices = 16; vdata.vertex_data.points = sofa_data;

    xlv.delta_x = 0.5; xlv.delta_y = 0.5; xlv.delta_z = 0.0;
    scale.delta_x = 0.5; scale.delta_y = 0.5; scale.delta_z = 1.0;         
    xang = 0.0; yang = 0.0; zang = 0.0; 
    shift.delta_x = 0.25; shift.delta_y = 0.25; shift.delta_z = 0;

    pbuild_tran_matrix3( &xlv, &shift, xang, yang, zang,
			 &scale, &err, mx);

        pset_local_tran3(mx, PTYPE_REPLACE);
        pset_view_ind(1);
	pset_int_colr_ind(7);
        pset_of_fill_area_set3_data( fflag, eflag, vflag, colour_model, 
				     num_sets,
				     fdata, edata, vlistlist, &vdata );
                                   /* #23 */

    xlv.delta_x = 0.5; xlv.delta_y = 0.5; xlv.delta_z = 0.0;
    scale.delta_x = 0.5; scale.delta_y = 0.5; scale.delta_z = 1.0;         
    xang = 0.0; yang = 0.0; zang = 0.0; 
    shift.delta_x = -0.20; shift.delta_y = 0.10; shift.delta_z = 0;

    pbuild_tran_matrix3( &xlv, &shift, xang, yang, zang,
			 &scale, &err, mx);

        pset_local_tran3(mx, PTYPE_REPLACE);
        pset_view_ind(0);
        pset_line_colr_ind(1);  /* Set the line color to white */
        pcell_array3(&outline,&color_grid);  
                                   /* #27 */
    pclose_struct();

    /* Post the structure */

    ppost_struct( 1, 1, 1.0 );

    /* Make everything detectable */

    if (argc == 1) {
	names[0] = 1;
	filt.incl_set.num_ints = 1;
	filt.incl_set.ints = names;
	filt.excl_set.num_ints = 0;
	filt.excl_set.ints = (Pint *)NULL;
	pset_pick_filter( 1, 1, &filt );

	/* Initialize and enable the Pick device */

	path.depth = 0;
	path.path_list = path_els;
	pet =1;
	pinit_pick( 1, 1, PIN_STATUS_NONE, &path, pet, &ea, &drec, PORDER_TOP_FIRST );
	pset_pick_mode( 1, 1, POP_REQ, PSWITCH_NO_ECHO );

	/* Set up a Picking Loop */

	printf ("Type '^c' to quit; else keep picking:\n");
	while (1) {

	    /* Request Pick */

	    status = PIN_STATUS_NONE;
	    preq_pick(1, 1, 2, &status, &path);

	    /* Check if anything got picked; if so, print it */

	    if (status == PIN_STATUS_OK) {
		printf ("Path depth = %d\n", path.depth);
		for (i=0; i<path.depth; i++) {
		    printf ("Structure ID = %d\n", path.path_list[i].struct_id);
		    printf ("Pick ID = %d\n", path.path_list[i].pick_id);
		    printf ("Element Number = %d\n", path.path_list[i].elem_pos);
		}
		printf ("\n");
	    }
	    else {
		printf ("Nothing detected ! !\n");
		printf ("\n");
	    }
	}
    }

    /* Close up */

    pclose_ws( 1 );
    pclose_phigs();
}



