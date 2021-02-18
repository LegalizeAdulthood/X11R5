/* $XConsortium: clippolygon.c,v 5.1 91/02/16 09:32:17 rws Exp $ */

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
/* a simple program which draws and clips some fill areas */

#include "phigs/phigs.h"
main()
{

	static Ppoint line1[] = { 0.1,0.3, 
				  0.9,0.3,
				  0.9,0.8,
				  0.1,0.8,
				  0.1,0.3};
	static Ppoint line2[] = { 0.7,0.4, 
				  0.3,0.4,
				  0.3,0.7,
				  0.7,0.7,
				  0.7,0.4};
	static Ppoint line3[] = { 0.75,0.75, 
				  0.4,0.75,
				  0.75,0.4,
				  0.75,0.75};
	Ppoint_list list;
	static Pview_rep viewrep = {
		{1.0,0.0,0.0},
		{0.0,1.0,0.0},
		{0.0,0.0,1.0},

		{1.0,0.0,0.0},
		{0.0,1.0,0.0},
		{0.0,0.0,1.0},

		{0.5,1.0,0.5,1.0},
		PIND_CLIP
	};

	popen_phigs(NULL,PDEF_MEM_SIZE);
	popen_ws(1,NULL,phigs_ws_type_x_tool);
	pset_view_rep(1,1,&viewrep);

	popen_struct(1);
	    pset_view_ind(1);

	    list.num_points = 5;	list.points = line1;
	    pset_int_colr_ind(2);
	    pfill_area(&list);
	    list.num_points = 5;	list.points = line2;
	    pset_int_colr_ind(3);
	    pfill_area(&list);
	    list.num_points = 4;	list.points = line3;
	    pset_int_colr_ind(4);
	    pfill_area(&list);
        pclose_struct(1);
	ppost_struct(1,1,1.0);

	printf("hit carriage return to quit");
	getchar();
}

