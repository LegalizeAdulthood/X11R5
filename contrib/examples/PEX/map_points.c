/* $XConsortium: map_points.c,v 5.1 91/02/16 09:32:25 rws Exp $ */

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

/* an example of using X input in a PHIGS program. The input is translated
 * to PHIGS World Coordinates 
 */
#include "phigs/phigs.h"

static Display		*display;

static void
open_ws( ws )
        int ws;
{
    Visual			visual;
    XSetWindowAttributes	xswa;
    XEvent			exposureEvent;
    Pconnid_x_drawable		conn;
    Window			window;

    display = XOpenDisplay("");
    xswa.backing_store = NotUseful;
    xswa.event_mask = ExposureMask | StructureNotifyMask;
    xswa.background_pixel = BlackPixel(display, DefaultScreen(display));
    xswa.border_pixel = WhitePixel(display, DefaultScreen(display));
    visual.visualid = CopyFromParent;
    window = XCreateWindow(display,RootWindow(display,DefaultScreen(display)),
	500, 200, 600, 600, 1,
	DefaultDepth(display, DefaultScreen(display)),InputOutput,
	&visual,CWEventMask|CWBackingStore|CWBorderPixel|CWBackPixel,
	&xswa);
    XMapWindow(display,window);
    XSync(display,0);
    XWindowEvent(display, window, ExposureMask, &exposureEvent);
    XSelectInput( display, window, ButtonPressMask | KeyPressMask );

    conn.display = display;
    conn.drawable_id = window;
    popen_ws(ws,&conn,phigs_ws_type_x_drawable);
}

static void
map_it( ws, count, events )
    Pint	ws;
    int		count;
    XEvent	*events;
{
    Pescape_in_data	esc_in;
    Pescape_out_data	esc_out;
    Ppoint3		pts[10];
    int			i;

    esc_in.escape_in_u5.ws_id = ws;
    esc_in.escape_in_u5.points.num_points = count;
    esc_in.escape_in_u5.points.points = pts;
    for ( i = 0; i < count; i++ ) {
	pts[i].x = events[i].xbutton.x;
	pts[i].y = events[i].xbutton.y;
	pts[i].z = 1.0;	/* DC value */
    }

    esc_out.escape_out_u5.points.points = pts;

    pescape( PUESC_DRAWABLE_POINTS_TO_WC, &esc_in, (Pstore)NULL, &esc_out);

    fprintf( stdout, "%d points, view %d\n",
	esc_out.escape_out_u5.points.num_points,
	esc_out.escape_out_u5.view_index );
    for ( i = 0; i < esc_out.escape_out_u5.points.num_points; i++ )
	fprintf( stdout, "\t( %f, %f, %f)\n", pts[i].x, pts[i].y, pts[i].z );
}

main( argc, argv )
    int		argc;
    char	*argv[];
{
    XEvent	events[5];
    int		done;

    static Plimit	viewport = {100.0,500.0,100.0,500.0};
    static Ppoint	line[] = {0.0,0.0, 0.0,1.0, 1.0,1.0, 1.0,0.0, 0.0,0.0};
    static Ppoint_list	pl = {sizeof(line)/sizeof(line[0]), line};

    popen_phigs( (char *)NULL, PDEF_MEM_SIZE );
    popen_struct( 1 );
	plabel( 3 );
	pset_pick_id( 4 );
	pexec_struct( 2 );
    pclose_struct();
    popen_struct( 2 );
	ppolyline( &pl );
    pclose_struct();

    open_ws( 1 );
    pset_ws_vp( 1, &viewport );
    ppost_struct( 1, 1, 1.0 );

/*     pset_loc_mode( 1, 1, POP_EVENT, PSWITCH_ECHO ); */

    done = 0;
    while ( !done ) {
	XNextEvent( display, events );
	switch ( events[0].type ) {
	    case ButtonPress:
		map_it( 1, 1, events );
		break;
	    case KeyPress:
		done = 1;
		break;
	}
    }

    pclose_ws( 1 );
    pclose_phigs();
}


