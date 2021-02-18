/* $XConsortium: six_views.c,v 5.1 91/02/16 09:32:21 rws Exp $ */

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

/*  six_views.c - This program demonstrates the six default
		   view representations setup by the PEX-SI API.
*/

#include	<phigs/phigs.h>

#define		WS	1 	/* workstation id */
#define		LINE_1	10 	/* structure ids */
#define		LINE_2	20
#define		DEF_VIEW 7
#define		VIEW_1	1
#define		VIEW_2	2 	
#define		VIEW_3	3 
#define		VIEW_4	4
#define		VIEW_5	5

static Ppoint3	octagon[] = {
		    {0.25, 0.0, 0.5},
		    {0.75, 0.0, 0.5},
		    {1.0, 0.25, 0.5},
		    {1.0, 0.75, 0.5},
		    {0.75, 1.0, 0.5},
		    {0.25, 1.0, 0.5},
		    {0.0,  0.75, 0.5},
		    {0.0, 0.25, 0.5},
		};

main(argc, argv)
int	argc;
char	*argv[];
{
    Ppoint_list3 colored_line, oct_list;

    popen_phigs(0,PDEF_MEM_SIZE);
    popen_ws(1,NULL,phigs_ws_type_x_tool);

    oct_list.num_points = 8;
    oct_list.points = octagon;
 
    popen_struct(DEF_VIEW);
    	pset_int_colr_ind(1);
    	pset_int_style(PSTYLE_HOLLOW);
    	pfill_area3(&oct_list);
    pclose_struct();

    popen_struct(VIEW_1);
    	pset_view_ind(1);		
    	pset_int_colr_ind(1);
    	pset_int_style(PSTYLE_SOLID);
    	pfill_area3(&oct_list);
    pclose_struct();

    popen_struct(VIEW_2);
    	pset_view_ind(2);		
    	pset_int_colr_ind(2);
	pset_int_style(PSTYLE_HOLLOW);
    	pfill_area3(&oct_list);
    pclose_struct();

    popen_struct(VIEW_3);
    	pset_view_ind(3);		
    	pset_int_colr_ind(3);
	pset_int_style(PSTYLE_HOLLOW);
    	pfill_area3(&oct_list);
    pclose_struct();

    popen_struct(VIEW_4);
    	pset_view_ind(4);		
    	pset_int_colr_ind(4);
    	pfill_area3(&oct_list);
    pclose_struct();

    popen_struct(VIEW_5);
    	pset_view_ind(5);		
    	pset_int_colr_ind(5);
    	pfill_area3(&oct_list);
    pclose_struct();

    pmessage(WS, "please position this message window\n");
    sleep(10);

    ppost_struct(WS, DEF_VIEW, 0.0);
    pmessage(WS, " Default View - front  \n" );
    sleep(5);
    punpost_struct(WS, DEF_VIEW);

    ppost_struct(WS, VIEW_1, 0.0);
    pmessage(WS, " View 1 - front scaled \n" );
    sleep(5);
    punpost_struct(WS, VIEW_1);

    ppost_struct(WS, VIEW_2, 0.0);
    pmessage(WS, " View 2 - top \n" );
    sleep(5);
    punpost_struct(WS, VIEW_2);

    ppost_struct(WS, VIEW_3, 0.0);
    pmessage(WS, " View 3 - right side \n" );
    sleep(5);
    punpost_struct(WS, VIEW_3);

    ppost_struct(WS, VIEW_4, 0.0);
    pmessage(WS, " View 4 - off-axis \n" );
    sleep(5);
    punpost_struct(WS, VIEW_4);

    ppost_struct(WS, VIEW_5, 0.0);
    pmessage(WS, " View 5 - perspective off-axis \n" );
    sleep(5);
    punpost_struct(WS, VIEW_5);

    pclose_ws(WS);
    pclose_phigs();
}


