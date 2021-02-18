/* $XConsortium: stroke.c,v 5.1 91/02/16 09:32:30 rws Exp $ */

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

/* a program to demonstrate choice and stroke input */

#include "phigs/phigs.h"

static void
print_stroke( view, pts )
    Pint		view;
    Ppoint_list3	*pts;
{
    int	i;

    fprintf( stderr, " view = %d, %d points\n", view, pts->num_points );
    for ( i = 0; i < pts->num_points; i++ )
	fprintf( stderr, "\t( %f, %f, %f)\n",
	    pts->points[i].x, pts->points[i].y, pts->points[i].z );
}

static Ppoint3	init_pts[] = {{0.0,0.0,0.0}, {0.5,0.5,0.5}};

main( argc, argv )
    int		argc;
    char	*argv[];
{
    Pin_status		status;
    Pin_class		class;
    Pint		view_index, wsid, dev_num;
    Ppoint3		wc_pts[100];
    Ppoint_list3	stroke_pts;
    Plimit3		echo_vol;
    Pstroke_data3	rec;
    int			count;
    char		msg[512];

    popen_phigs( NULL, 0 );
    popen_ws( 1, NULL, phigs_ws_type_x_tool );

    if (argc == 1) {
    pset_choice_mode( 1, 1, POP_REQ, PSWITCH_ECHO );
    preq_choice( 1, 1, &status, &count );

    sprintf(msg, "Choice %d was selected.\nReady to test stroke input.\nSelect a point with the LEFT_MOUSE button.\nSelect the last point with SHIFT-LEFT_MOUSE.\nErase the previous point with CTRL-LEFT_MOUSE.\nEnter 6 sequences of stroke points.\n", count);
    pmessage(1, msg); 
    }
    
    /* first select 5 sets of stroke points */
    stroke_pts.points = wc_pts;
    count = 0;
    pset_stroke_mode( 1, 1, POP_EVENT, PSWITCH_ECHO );
    while ( count < 5 ) {
	pawait_event( 600.0, &wsid, &class, &dev_num );
	pget_stroke3( &view_index, &stroke_pts );
	print_stroke( view_index, &stroke_pts );
	++count;
    }
    pset_stroke_mode( 1, 1, POP_REQ, PSWITCH_ECHO );

    /*  end by displaying 2 points and replacing them with selection */
    echo_vol.x_min = echo_vol.y_min = 0.0;
    echo_vol.x_max = echo_vol.y_max = 600.0;
    echo_vol.z_min = 0.0; echo_vol.z_max = 1.0;
    stroke_pts.num_points = 2; stroke_pts.points = init_pts;
    rec.buffer_size = 20;
    rec.init_pos = 1;
    rec.x_interval = rec.y_interval = rec.z_interval = rec.time_interval = 0.0;
    pinit_stroke3( 1, 1, 0, &stroke_pts, 1, echo_vol, &rec );
    preq_stroke3( 1, 1, &status, &view_index, &stroke_pts );
    print_stroke( view_index, &stroke_pts );

    pclose_ws( 1 );
    pclose_phigs();
}


