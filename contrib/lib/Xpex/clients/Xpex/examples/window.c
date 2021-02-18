/* $Header: window.c,v 2.3 91/09/11 16:05:10 sinyaw Exp $ */

/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

/* window.c */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

static int x = 50;
static int y = 50;
static int width = 400;
static int height = 400;
static char *window_label = "PEXlib Demo";
static char *icon_label = "PEXlib Demo";

#define BORDER_WIDTH	2

Window
CreateWindow(dpy)
	Display *dpy;
{
	XSizeHints theSizeHints;
	unsigned long theWindowMask;
	Window theNewWindow;
	int theScreen = DefaultScreen(dpy);
	XEvent event;
	Visual best_visual;
	XVisualInfo *best_info, template;
	int nvisuals;
	XSetWindowAttributes theWindowAttributes;
	Colormap cmap;

	if (!XpexFindBestVisual(dpy, &best_visual, &cmap)) {
		return (0);
	}
	/* Need the depth of this visual */
	template.visualid = best_visual.visualid;
	best_info = XGetVisualInfo(dpy, VisualIDMask, &template, &nvisuals);

/*	theWindowAttributes.background_pixel = WhitePixel(dpy, theScreen);
	theWindowAttributes.border_pixel = BlackPixel(dpy, theScreen); */
	theWindowAttributes.background_pixel = BlackPixel(dpy, theScreen);
	theWindowAttributes.border_pixel = WhitePixel(dpy, theScreen);
	theWindowAttributes.colormap = cmap;
	theWindowAttributes.backing_store = NotUseful;
	theWindowAttributes.event_mask = ExposureMask | StructureNotifyMask;

	theWindowMask = 
		CWColormap | 
		CWBackPixel | 
		CWBorderPixel | 
		CWBackingStore | 
		CWEventMask;

	theNewWindow = XCreateWindow(dpy,
		RootWindow(dpy, theScreen),
		x, y,
		width, height,
		BORDER_WIDTH,
		best_info->depth,
		InputOutput,
		best_info->visual,
		theWindowMask,
		&theWindowAttributes);

	theSizeHints.flags = PPosition | PSize;
	theSizeHints.x = x;
	theSizeHints.y = y;
	theSizeHints.width = width;
	theSizeHints.height = height;

	XSetStandardProperties(dpy, theNewWindow, window_label,
	icon_label, None, NULL, 0, &theSizeHints); 

	XSelectInput(dpy, theNewWindow, (unsigned long) ExposureMask);
	XMapWindow(dpy, theNewWindow);

	XSync(dpy, 0);

	XWindowEvent(dpy, theNewWindow, ExposureMask, &event);
	XSelectInput(dpy, theNewWindow, (unsigned long) 0);

	return(theNewWindow);
}

void
SetWindowLabel(label)
	char *label;
{
	window_label = label;
}

void
SetIconLabel(label)
	char *label;
{
	icon_label = label;
}
