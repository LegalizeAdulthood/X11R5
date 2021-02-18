/* $XConsortium: pickplus.c,v 5.1 91/02/16 09:32:43 rws Exp $ */

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
***  Sample pick program to test picking of FillAreas, Nurb Curve, QuadMesh
***  and Triangle Strips.
***/

#include "phigs/phigs.h"

#define ROWS    20
#define COLUMNS 20

static Plimit viewport = {100.0,500.0,100.0,500.0};
static Ppoint points[5] = {0.01,0.01, 0.01,0.99, 0.99,0.99, 0.99,0.01, 0.01,0.01};
static Ppoint_list   line;
static   Plimit            ea={100.0, 500.0, 100.0, 500.0 };

/* Fill Area Data */
static Ppoint3 farea[] = { 0.01,0.01,0.0, 0.8,0.8,0.0, 0.7,0.01,0.0};
static Ppoint_list3 fset = {3,farea};
static Pview_rep3 viewrep = {
       {1.0,0.0,0.0,0.0},
       {0.0,1.0,0.0,0.0},
       {0.0,0.0,1.0,0.0},
       {0.0,0.0,0.0,1.0},

       {1.0,0.0,0.0,0.0},
       {0.0,1.0,0.0,0.0},
       {0.0,0.0,1.0,0.0},
       {0.0,0.0,0.0,1.0},

       {0.0,0.5, 0.0,1.0, 0.0,1.0},
       PIND_CLIP,
       PIND_CLIP,
       PIND_CLIP
};
static Plimit win = {0.0,0.5,0.0,1.0};
static Plimit3 vport = {0.0,0.5, 0.0,1.0, 0.0,1.0};
static Pview_map3 viewmap;
static Ppoint3 prp = {0.0,0.0,20.0};

/* Quad Mesh Data */
static Pint_size    dim={COLUMNS,ROWS};
        Pfacet_vdata_arr3 vdata;
        Pfacet_data_arr3 fdata;
        Ppoint3 *pts;
        Ppoint3 *ptr;

        Pcoval *colarr;
        Pcoval *color;

/* Tri Strip Data */
static Ppoint3 pts1[] = {0.3,0.6,0.0, 0.5,0.5,0.5, 0.7,0.6,0.0, 0.7,0.4,0.0};
static Ppoint3 pts2[] = {0.3,0.6,0.0, 0.5,0.5,0.5, 0.3,0.4,0.0, 0.7,0.4,0.0};

        Pint            err;

        Pfacet_vdata_arr3 vdata1;
        Pfacet_vdata_arr3 vdata2;

	int i,j;

/* Nurb Curve Data */
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
			  0.05,0.7,0.0, 
			  0.10,0.9,0.0,
			  0.15,0.5,0.0,
			  0.20,0.9,0.0,
			  0.25,0.5,0.0,
			  0.30,0.9,0.0,
			  0.35,0.5,0.0,
			  0.40,0.9,0.0,
			  0.45,0.7,0.0,
			 };
	Pfloat		min = 5.0,
			max = 9.0;

	Pint		curve_approx_type
				= PCURV_CONSTANT_PARAMETRIC_BETWEEN_KNOTS;
	Pfloat		curve_approx_tolerance = 10.0;

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

    /* Initialize the curve data */

    cpoints.num_points = 9;
    cpoints.points.point3d = cpts;

    knots.num_floats = cpoints.num_points + order;
    knots.floats = kpts;

    /* Set up Quad Mesh */

    ptr = pts = (Ppoint3 *)malloc(ROWS*COLUMNS*sizeof(Ppoint3));
    color = colarr = (Pcoval *)malloc(ROWS*COLUMNS*sizeof(Pcoval));

    for (i = 0; i < ROWS; i++) {
	for (j = 0; j < COLUMNS; j++) {
	    ptr->x     = 0.5 + j * (0.5/(COLUMNS));
	    ptr->y     = 0.5 + i * (0.5/(ROWS));
	    (ptr++)->z = 0.0;
	}
    }

    for (i = 0; i < (ROWS-1); i++) {
	for (j = 0; j < (COLUMNS-1); j++) {
	    color->direct.rgb.red        = j * (1.0/(COLUMNS-2));
	    color->direct.rgb.green      = i * (1.0/(ROWS-2));
	    (color++)->direct.rgb.blue  = 0.0;
	}
    }

    fdata.colrs = colarr;
    vdata.points = pts;

    /* Set up Tri Strip */

    vdata1.points = pts1;
    vdata2.points = pts2;

    /* Initialize PHIGS and start up */

    popen_phigs( (char *)NULL, PDEF_MEM_SIZE );

    /* Set up the view */

    viewmap.win = win;
    viewmap.vp = vport;
    viewmap.proj_type = PTYPE_PERSPECT;
    viewmap.proj_ref_point = prp;
    viewmap.view_plane = 0.0;
    viewmap.front_plane = 1.0;
    viewmap.back_plane = 0.0;
    peval_view_map_matrix3(&viewmap,&err,viewrep.map_matrix);
    if (err) printf("error %d in evalviewmappingmatrix3",err);

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
        pset_line_colr_ind(1);      /* Set the line color to white */
	ppolyline( &line );         /* #3 */

        pset_int_style(PSTYLE_SOLID);
        pset_int_colr_ind(2);
        pset_edge_flag(PEDGE_ON);
        pset_edge_colr_ind(3);
        pquad_mesh3_data(PFACET_COLOUR,PVERT_COORD,PMODEL_RGB,&dim,&fdata,&vdata);                                 /* #8 */
        pset_int_colr_ind(4);
        pset_edgewidth (2.0);
        pset_edge_colr_ind(5);
        pset_view_ind(1);
        pfill_area_set3(1,&fset);   /* #13 */

        pset_view_ind(0);
        pset_int_colr_ind(6);
        pset_edge_colr_ind(7);
        pset_int_shad_meth(PSD_NONE);
        pset_refl_eqn(PREFL_AMBIENT);
        ptri_strip3_data(PFACET_NONE,PVERT_COORD,PMODEL_RGB,4,NULL,&vdata1);
                                    /* #19 */
        ptri_strip3_data(PFACET_NONE,PVERT_COORD,PMODEL_RGB,4,NULL,&vdata2);
                                    /* #20 */

        pset_line_colr_ind(5);      /* Set the curve color to yellow */
	pset_curve_approx(curve_approx_type, curve_approx_tolerance);
	pnuni_bsp_curv(order, &knots, rationality, &cpoints, min, max);
                                    /* #23 */
    pclose_struct();

    /* Open a X-tool workstation */

    popen_ws( 1, (char *)NULL, phigs_ws_type_x_tool );
    pset_ws_vp( 1, &viewport );
    pset_view_rep3(1,1,&viewrep);

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



