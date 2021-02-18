#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)svr_set.c 20.40 90/03/01";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <sys/types.h>
#include <sys/time.h>
#include <xview/win_event.h>
#include <xview_private/svr_impl.h>

Xv_private void xv_scheduler_internal();
Xv_private void server_journal_sync_event();


Pkg_private     Xv_opaque
server_set_avlist(server_public, avlist)
    Xv_Server       server_public;
    Attr_attribute  avlist[];
{
    Attr_avlist     attrs;
    Server_info    *server = SERVER_PRIVATE(server_public);

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch (attrs[0]) {
	  case SERVER_NTH_SCREEN:{
		int             number = (int) attrs[1];
		Xv_Screen       screen = (Xv_Screen) attrs[2];

		if ((number < 0) || (number >= MAX_SCREENS))
		    return (Xv_opaque) attrs[0];

		/*
		 * destroy the old screen if overwriting, unless new screen
		 * is null, in which case caller must already have destroyed
		 * old.
		 */
		if (server->screens[number] != screen) {
		    if (screen && server->screens[number])
			(void) xv_destroy(server->screens[number]);
		    server->screens[number] = screen;
		}
	    }
	    break;
	  case SERVER_SYNC:
	    XSync((Display *) server->xdisplay, (int) attrs[1]);
	    break;
	  case SERVER_SYNC_AND_PROCESS_EVENTS:
	    {
		/*
		 * sync with the server to make sure we have all outstanding
		 * events in the queue. Then process the events, then finally
		 * map the frame.
		 */
		Display        *display = (Display *) server->xdisplay;

		XSync(display, 0);
		do {
		    xv_scheduler_internal();	/* process pending queued
						 * events */
		    XSync(display, 0);
		} while (QLength(display) != 0);
	    }
	    break;
	  case SERVER_JOURNAL_SYNC_ATOM:
	    server->atom[(int) SERVER_JOURNAL_SYNC_ATOM_TYPE] = 
		(Xv_opaque) attrs[1];
	    break;
	  case SERVER_JOURNAL_SYNC_EVENT:
	    if (server->journalling)
		server_journal_sync_event(server_public, (int) attrs[1]);
	    break;
	  case SERVER_JOURNALLING:
	    server->journalling = (int) attrs[1];
	    break;
	  case SERVER_MOUSE_BUTTONS:
	    server->nbuttons = (short) attrs[1];
	    break;
	  case SERVER_BUTTON2_MOD:
	    server->but_two_mod = (unsigned int) attrs[1];
	    break;
	  case SERVER_BUTTON3_MOD:
	    server->but_three_mod = (unsigned int) attrs[1];
	    break;
	  case SERVER_EXTENSION_PROC: 
	    server->extensionProc = (void (*) ()) attrs[1];
	    break;
	  case XV_NAME: 
	    server->display_name = (char *) attrs[1];
	    break;
	  default:
	    (void) xv_check_bad_attr(&xv_server_pkg, (Attr_attribute)attrs[0]);
	    break;
	}
    }
    return (Xv_opaque) XV_OK;
}


Xv_private void
server_set_timestamp(server_public, ev_time, xtime)
    Xv_Server       server_public;
    struct timeval *ev_time;
    unsigned long   xtime;
{
    Server_info    *server = SERVER_PRIVATE(server_public);

    server->xtime = (Xv_opaque) xtime;

    /* Set the time stamp in the event struct */
    if (ev_time) {
	ev_time->tv_sec = ((unsigned long)xtime)/1000;
	ev_time->tv_usec = (((unsigned long) xtime) % 1000) * 1000;
    }
}

Xv_private      void
server_set_fullscreen(server_public, in_fullscreen)
    Xv_Server       server_public;
    int             in_fullscreen;
{
    Server_info    *server = SERVER_PRIVATE(server_public);
    server->in_fullscreen = in_fullscreen;
}
