/***********************************************************************

gdrv.h - header for GDEV back ends

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/
#include <stdio.h>
#include "gdev.h"

/*
   The purpose of this package is to provide a set of device independent
   graphics routines for both input and output.  Graphics, text output,
   keyboard input, mice, and menus are supported.  An application gets
   device independence by only calling routines whose name begins with
   GDEV.

   For each kind of graphic device supported there is a device driver.
   The driver must implement a number of functions.  Those functions
   should be (by convention) prefixed with the name of the device, e.g.
   the function which draws lines using poof is called POOFline.  In
   order for these functions to do their job they will need to call
   functions whose name begins with GDRV.

 Device interface:

    The GDEV kernel and the driver need to refer to multiple
    windows.  They do so by using two handles refered to in this file
    as W and H, declared as follows:
	int H;	   // a window number sent from driver to GDEV 
	Pointer W; // a pointer to (presumably) an internal data
		      structure in the driver for a given window.
    When GDEVinit() is called to create a new window, it chooses a value
    for H to refer to this new window.  It informs the driver of this
    H value via the all DEVinit(H).  DEVinit(H) returns W, the pointer
    to its internal data structure for this window, so GDEV can pass
    W to the driver in subsequent calls (like DEVline).  The driver
    passes H to GDEV in order to tell GDEV that something has happened
    in window H (e.g. GDRVrefresh(H) tells GDEV that window H need
    to be refreshed).  It is necessary for the driver to keep a
    mapping from W to H (easily done by including H in the driver's
    per window data structure).

    Every device driver needs to supply the following functions (DEV is the
    name of the particular device).  These functions are used to initialize
    a table in gdev.c.

	DEVinit(H)	initialize, opens a new window, returns W
	DEVstart(W)	clear screen/new page
				(the first time this must result in GDRVrefresh
				 being called)
	DEVflush(W)	display picture so far
	DEVstop(W)	clean up upon program exit

	DEVline(W, int x1, y1, x2, y2)
	DEVpoint(W, int x, y);
	DEVrect(W, int x1, y1, x2, y2)
	DEVtext(W, int x1, y1; char *string)

	DEVsetdim(W, int width, height) sets page dimensions, it
				may be ignored.  The call

	DEVgetdim(W, int *width, *height) returns the actual dimensions.

	DEVmenuitem(W, char *itemname, int retval, int addflag)
		When addflag is TRUE, this should
		cause itemname to be added to the menu.  
		When addflag is FALSE, itemname should be deleted from
		the menu.
		In both cases, retval is guarenteed to be a small
		integer (and is to be passed to GDRVmenu when the
		item is selected).

	DEVmouseinterest(W, event) int event;
		informs the driver that we are interested in these
		kinds of mouse events.

    Upon initialization, an device driver should do the following:

       For each variable (in the GDEVset sense) an device
       is interested in, it may call:

    	GDRVvar_fcn(H, varname, function, arg) 
		char *varname; int (*function)(); char *arg;

       When the client does a GDEVset call, "function" 
       will be called with three args: the name of the variable,
       value of the variable, and passed arg.

       If the driver just wants one of its variables set when
       the user calls GDEVset, it should call:

       GDRVvar_addr(H, varname, address)
		char *varname; char *address;

	The address is treated as a pointer to a value of the type
	the client specified (i.e. If the client called GDEVseti,
	the address is treated as a pointer to an integer).



      For each file descriptor upon which the device driver wants to be
      notified when input is available he should call
	GDRVfdnotify(fd, function) int fd; int (*function)();
     (*function)(fd) will be called whenever input is available on fd.
     If fd == FD_POLL, the function will be called just before any select
     which waits for input, instead of after the select has returned.

     When a driver sees a mouse event, it should call
	GDRVmouse(H, event, x, y, t) int event; int x, y; int t;
     It is OK for the driver to inform GDEV of mouse events
     in which the client is not interested - these will be
     discarded.

     When a driver needs the screen refreshed, it should call
	GDRVrefresh(H);

     When a driver sees a menu event, it should call
	GDRVmenu(H, retval) int retval;
      where retval is the small integer which refers to the menu item
      chosen by the user.

     When a driver notices the user typed a char he should send
	GDRVwindow(H);
	GDRVputc(c);

 */


/* polling fd for GDRVfdnotify */

#define	FD_POLL		(-1)


/* functions */

void	GDRV00topleft(); /* int H; a driver should call this function during
			    initialization if it thinks 0,0 is the top
			    left of the window, else 0,0 is bottom left */
void	GDRVvar_fcn();	/* H, char *varname; int (*function)(); Pointer arg; */
void	GDRVvar_addr();	/* H, char *varname; Pointer address; */
void	GDRVfdnotify(); /* int fd; int (*function)(); */
void	GDRVmouse(); 	/* H, int event; int x, y; */
void	GDRVrefresh();	/* H */
void	GDRVmenu();	/* H, int retval; */

