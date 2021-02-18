#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)scrn_vis.c 20.16 90/03/29";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/scrn_impl.h>
#include <xview/server.h>

static Screen_visual *screen_find_visual();

Screen_visual  *
screen_add_visual(screen_public, xid, depth)
    Xv_Screen       screen_public;
    XID             xid;
    int             depth;
{
    register Screen_info *screen = SCREEN_PRIVATE(screen_public);
    register Screen_visual *visual = screen_find_visual(screen, depth);
    register Display *display;
    GC              gc;
    XGCValues       changes;

    if (visual) {
	return visual;
    }
    display = (Display *) xv_get(screen->server, (Attr_attribute)XV_DISPLAY);
    /* Make sure colors are correct in gc. */
    /* BUG ALERT!  This is not handling color properly!!! */
    changes.foreground = BlackPixel(display, screen->number);
    changes.background = WhitePixel(display, screen->number);
    gc = XCreateGC(display, xid, GCForeground | GCBackground,
		   &changes);
    if (!gc) {
	return (Screen_visual *) 0;
    }
    /*
     * add a new entry for this depth.
     */
    visual = xv_alloc(Screen_visual);
    visual->screen = screen_public;
    visual->root_window = screen->root_window;
    visual->server = screen->server;
    visual->display = display;
    visual->depth = depth;
    visual->gc = gc;
    visual->image_1 = NULL;
#ifndef SVR4
    visual->image_8 = NULL;
#else SVR4
    visual->image_n = NULL;
#endif SVR4

    visual->next = screen->first_visual;
    screen->first_visual = visual;

    return visual;
}


static Screen_visual *
screen_find_visual(screen, depth)
    register Screen_info *screen;
    register int    depth;
{
    register Screen_visual *visual;

    for (visual = screen->first_visual; visual; visual = visual->next) {
	if (visual->depth == depth)
	    return visual;
    }
    return 0;
}
