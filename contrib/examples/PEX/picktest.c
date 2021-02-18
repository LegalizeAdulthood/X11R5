/* $XConsortium: picktest.c,v 5.1 91/02/16 09:32:28 rws Exp $ */

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
***  Sample pick program to test picking of polylines, polymarkers,
***  text and annotation text.
***/

#include "phigs/phigs.h"

static Plimit viewport = {100.0,500.0,100.0,500.0};
static Ppoint points[5] = {0.01,0.01, 0.01,0.99, 0.99,0.99, 0.99,0.01, 0.01,0.01};
static Ppoint_list line, markers;
static Ppoint mpoints[5] = {0.21,0.21, 0.23,0.23, 0.26,0.26, 0.24,0.20, 0.30,0.24};
static Ppoint torigins[] = {0.1,0.1, 0.5,0.5, 0.1,0.8, 0.8,0.1, 0.9,0.9};
static Ppoint3 txpta = {0.5,0.5,0.0};
static Pvec3 txdir[2] = {1.0, 0.0, 0.0,
			 0.0, 1.0, 0.0};
static Ppoint3 txpt2 = {0.2,0.2,0.0};
static Ppoint3 offset2 = {-0.2,0.4,0.0};
static Ppoint txpt3 = {0.5,0.5};
static Ppoint offset3 = {-0.2,-0.2};
static   Plimit            ea={100.0, 500.0, 100.0, 500.0 };

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

    /* Set up markers */

    markers.num_points = 5;
    markers.points = &mpoints[0];

    /* Initialize PHIGS and start up */

    popen_phigs( (char *)NULL, PDEF_MEM_SIZE );

    /* Build the CSS */

    popen_struct( 1 );
	plabel( 3 );
	pset_pick_id( 5 );
	pexec_struct( 2 );
    pclose_struct();
    popen_struct( 2 );
	names[0] = 1;
	filt.incl_set.num_ints = 1;
	filt.incl_set.ints = names;
	padd_names_set( &filt.incl_set );

        pset_line_colr_ind(2);  /* Set the line color to red */
	ppolyline( &line );     /* #3 */

	pset_pick_id( 10 );
        pset_marker_colr_ind(7);  /* Set the marker color to magenta */
        pset_marker_size(2.0);
	ppolymarker( &markers );  /* #7 */

	pset_pick_id( 20 );
	pset_text_colr_ind (5);
	pset_char_expan (-2.0);
        ptext(&torigins[0], "PEX-SI's 2D Text can be PICKED !");  /* #11 */

	pset_pick_id( 30 );
        pset_text_path (PPATH_UP);  
	pset_char_ht (0.02);
	pset_text_colr_ind (1);
        ptext3(&txpta,txdir,"3D Text PICK TEST");  /* #16 */
	pset_line_colr_ind(6);

	pset_pick_id( 40 );
	pset_linetype (3);
        pset_anno_path (PPATH_RIGHT);  
	pset_anno_char_ht (-0.020);
	pset_anno_style (2);
	pset_text_colr_ind (6);
        panno_text_rel(&txpt3,&offset3,"Testing Anno2D Text Picking");/* #24 */

	pset_pick_id( 50 );
	pset_anno_char_ht (-0.015);
	pset_char_expan (-2.0);
	pset_anno_style (1);
	pset_text_colr_ind (3);
        panno_text_rel3(&txpt2,&offset2,"Picking Anno3D Text"); /* #30 */
    pclose_struct();

    /* Open a X-tool workstation */

    popen_ws( 1, (char *)NULL, phigs_ws_type_x_tool );
    pset_ws_vp( 1, &viewport );

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
	pet = 1;
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



