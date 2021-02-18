/* $XConsortium: linesmarks.c,v 5.1 91/02/16 09:32:27 rws Exp $ */

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
/* attlinesmarkers.c                                                  */
/**********************************************************************/

#include <phigs/phigs.h>

/**********************************************************************/
/* Open struct 1.  Draw some lines and markers.  Close the struct.    */
/**********************************************************************/

void DrawLinesAndMarkers()
{
    Ppoint3 line[2];
    Ppoint3 points[5];
    Ppoint_list3 colored_line1, colored_line2;

    line[0].x = 0.1; line[0].y = 0.5; line[0].z = 0.0;
    line[1].x = 0.9; line[1].y = 0.5; line[1].z = 0.0;
    colored_line1.num_points = 2;
    colored_line1.points = line;

    points[0].x = 0.2; points[0].y = 0.6; points[0].z = 0.0;
    points[1].x = 0.8; points[1].y = 0.6; points[1].z = 0.0;
    points[2].x = 0.8; points[2].y = 0.9; points[2].z = 0.0;
    points[3].x = 0.2; points[3].y = 0.9; points[3].z = 0.0;
    points[4].x = 0.2; points[4].y = 0.6; points[4].z = 0.0;
    colored_line2.num_points = 5;
    colored_line2.points = points;

    popen_struct(1);
	pset_line_colr_ind(2);	/* red in the default colormap */
	pset_linetype(PLINE_DASH);
	pset_linewidth(4.0);
	ppolyline3(&colored_line1);

	pset_line_colr_ind(3);	/* green in the default colormap */
	pset_linetype(PLINE_DOT_DASH);
	pset_linewidth(3.0);
	ppolyline3(&colored_line2);

	pset_marker_size(3.0);
	pset_marker_type(PMARKER_CROSS);
	pset_marker_colr_ind(4); /* blue in the default colormap */
	ppolymarker3(&colored_line2);

    pclose_struct();
}

/**********************************************************************/
/* main                                                               */
/**********************************************************************/

main()
{
    popen_phigs((char*)NULL, PDEF_MEM_SIZE);	/* open phigs */
    popen_ws(1,NULL,phigs_ws_type_x_tool); 
                                                        /* station 1  */
    DrawLinesAndMarkers();

    ppost_struct(1,1,1.0);		/* post struct 1         */
    sleep(10);
    pclose_ws(1);			/* close the workstation */
    pclose_phigs();			/* close phigs           */
}
/**********************************************************************/

