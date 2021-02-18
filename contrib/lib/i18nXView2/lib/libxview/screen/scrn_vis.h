/*	@(#)scrn_vis.h 20.12 90/03/29 SMI	*/

/****************************************************************************/
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/****************************************************************************/

#ifndef _xview_screen_visual_h_already_included
#define _xview_screen_visual_h_already_included

#include <xview/base.h>
#include <X11/Xlib.h>

typedef struct screen_visual {
    Xv_object	server;
    Xv_object	screen;
    Xv_object	root_window;
    short		depth;
    Display		*display;
    GC			gc;
    XImage		*image_1;
#ifndef SVR4
    XImage		*image_8;
#else SVR4
    XImage		*image_n;
#endif SVR4
    struct screen_visual *next;
} Screen_visual;

#endif
