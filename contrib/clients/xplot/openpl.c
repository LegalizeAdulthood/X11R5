/* Copyright (C) 1989, 1990, 1991 Free Software Foundation, Inc.

                        All Rights Reserved

In order to better advertise the GNU 'plot' graphics package, the
standard GNU copyleft on this software has been lifted, and replaced
by the following:
   Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both the copyright notice and this permission notice appear in
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

xplot is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY.  No author or distributor accepts responsibility to
anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GNU General Public License for full details. Also:

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.  */

#include "config.h"
#include <fcntl.h> /* Maybe we need to include X11/Xos.h ? */
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <sys/types.h>
#include <sys/time.h>

#define DEFAULTFONT "fixed"

char *getenv();

extern char *prog;
extern char *bgname;
extern char *displayname;
extern char *fgname;
extern char *fontname;
extern char *geom;
extern int killlast;
extern int reverse;
extern FILE *instrm;
extern int numfiles;

extern int filling;
extern int inittile;
extern Pixmap filltile;

int forked_or_ended;
int eoffound;

Display *dpy;
Window win;
Pixmap pixmap;
GC gc;
GC fillgc;
XGCValues gcv;
XWindowAttributes wa;
XFontStruct *font;
int xeventmask = 0;

int rwidth = 500, rheight = 500;
int xwin = 0, ywin = 0;
long geomflag;

int fgcolor = 1; /* Default pixel values... maybe not desirable? */
int bgcolor = 0;
int initfg, initbg;
XFontStruct *initfont;

openpl(initflag)
int initflag;
{
	int fd, argf, in_mask;
	struct timeval timeout; /* How long to wait in selection for data */

	forked_or_ended = 0; /* To tell when input is over */
	eoffound = 0;
	fgcolor = 1; /* Reset the defaults */
	bgcolor = 0;

/* Using a timeout on select is a kludge to get xplot to
   recognize resize events while waiting for input. */
/* Presumably XtAddInputs in the toolkit does exactly the same thing? */

	timeout.tv_sec = 1; /* Wait one second */
	timeout.tv_usec = 0; /* and no microseconds */

	fd = fileno(instrm);
/* Make sure reading is without buffering, delays */
	fcntl(fd,F_GETFL,argf);
	fcntl(fd,F_SETFL, argf|O_NDELAY);

	if (initflag) init_X(); /* Set up the X stuff */
	else {
		
	}

/* Pay attention only to resize events and plot input */

#ifdef Resize_Yes
	xeventmask = StructureNotifyMask;
	XSelectInput(dpy, win, StructureNotifyMask);
#endif /* Resize_Yes */

/* Read in all the data and plot it! */
	for (;;) {
		resize_check();
		in_mask = 1L << fd;
		if (select(fd + 1, &in_mask, NULL, NULL, &timeout) > 0)
					doplot();
		if (forked_or_ended)
			break;
	}
	fcntl(fd, F_SETFL, argf);
}

/* Do the actual X window setup calls and take care of most options */

init_X()
{
	int tmpcolor;
	Window rwindow;
	XSetWindowAttributes swa;
	XSizeHints      sh;

	init_data(); /* Initialize the data buffer too */
	if (displayname == NULL)
		displayname = getenv("DISPLAY");

	if ((dpy = XOpenDisplay(displayname)) == 0) {
		fprintf(stderr, "%s: Can't open display \"%s\"\n",
			prog, XDisplayName(displayname));
		exit(1);
	}

	rwindow = XDefaultRootWindow(dpy);

/* Only kill the other guys if we're not reading from a file */

	if ((numfiles < 2) && (killlast)) destroylastplot(dpy, rwindow);

	XGetWindowAttributes(dpy, rwindow, &wa);
	if (wa.colormap == 0)
		wa.colormap = DefaultColormap(dpy, DefaultScreen(dpy));

	if (geom == NULL) {
		if (geom = XGetDefault(dpy,"xplot","geometry")) {
			getgeom(geom);
		} /* Otherwise just use our default geometry */
	} else
		getgeom(geom);

	if (fgname == NULL) {
		if (fgname = XGetDefault(dpy, "xplot", "foreground")){
			nameToPixel(fgname, &fgcolor);
		}
	} else
		nameToPixel(fgname, &fgcolor);

	if (bgname == NULL) {
		if (bgname = XGetDefault(dpy, "xplot", "background")){
			nameToPixel(bgname, &bgcolor);
		}
	} else
		nameToPixel(bgname,&bgcolor);

	if (!reverse)
		if (XGetDefault(dpy,"xplot","reverseVideo"))
			reverse = 1;

	if (fontname == NULL) {
		if (fontname = XGetDefault(dpy, "xplot", "font")){
			if ((font = XLoadQueryFont(dpy, fontname)) == NULL) {
			    fprintf(stderr, "Font %s does not exist\n",
								 fontname);
			    exit(0);
			}
		} else {
		    if ((font = XLoadQueryFont(dpy, DEFAULTFONT)) == NULL) {
			fprintf(stderr, "Please specify a font\n");
			exit(0);
		    }
		}
	} else {
		if ((font = XLoadQueryFont(dpy, fontname)) == NULL) {
		    fprintf(stderr, "Font %s does not exist\n", fontname);
		    exit(0);
		}
	}

	pixmap = XCreatePixmap(dpy,DefaultRootWindow(dpy), rwidth, rheight,
			DefaultDepth(dpy, DefaultScreen(dpy)));
	gc = XCreateGC(dpy, pixmap, 0L, &gcv);
	fillgc = XCreateGC(dpy, pixmap, 0L, &gcv); /* For filling */
	XSetFont(dpy, gc, font->fid); /* Font must be set by now */
	if (reverse){
		tmpcolor = fgcolor;
		fgcolor = bgcolor;
		bgcolor = tmpcolor;
	}
	XSetForeground(dpy,gc,fgcolor);
	XSetBackground(dpy,gc,bgcolor);

	filling = 0; /* Initialize this properly for each file */
	initfg = fgcolor; /* Need these for proper resizing */
	initbg = bgcolor;
	initfont = font;

	erase();
	swa.background_pixmap = pixmap;
	win = XCreateWindow(dpy, rwindow, (unsigned int) xwin,
		(unsigned int) ywin, (unsigned int) rwidth,
		(unsigned int) rheight, 0, CopyFromParent,
		InputOutput, CopyFromParent,
		CWBackPixmap, &swa);
	XChangeProperty(dpy, win, XA_WM_NAME, XA_STRING, 8,
		PropModeReplace, (/* const */
			unsigned char *) "xplot",
		strlen("xplot"));
	sh.width = rwidth;
	sh.height = rheight;
	sh.flags = USSize;
	if ((geomflag&XValue)&&(geomflag&YValue)) {
		sh.x = xwin;
		sh.y = ywin;
		sh.flags |= USPosition;
	}
	XSetNormalHints(dpy, win, &sh);
	XMapWindow(dpy, win);
	XFlush(dpy);
}

getgeom(ptr)
char *ptr;
{
	geomflag = XParseGeometry(ptr, &xwin, &ywin,
			(unsigned int *) &rwidth,
			(unsigned int *) &rheight);
	if (geomflag&XNegative) {
		xwin = DisplayWidth(dpy, DefaultScreen(dpy)) - xwin - rwidth;
	}
	if (geomflag&YNegative) {
		ywin = DisplayHeight(dpy, DefaultScreen(dpy)) - ywin -rheight;
	}
}

destroylastplot(dpy, win)
Display *dpy;
Window win;
{
	Window root, parent, *children = NULL;
	unsigned int nchildren;
	int n;
	char *name = NULL;

	XQueryTree(dpy, win, &root, &parent, &children, &nchildren);
	for (n=0; n < nchildren; n++) {
		destroylastplot(dpy, children[n]);
		if (XFetchName(dpy, children[n], &name))
			if (!strcmp(name, "xplot")) {
				tellwin(children[n], dpy);
			}
		if (name) free(name);
	}
}

tellwin(w, dpy)
Window w;
Display *dpy;
{
	XButtonEvent ev;

	ev.type = ButtonPress;
	ev.display = dpy;
	ev.window = w;
	ev.x = ev.y = 10;
	ev.state = 0;
	ev.button = Button1;
	XSendEvent(dpy, w, False, ButtonPressMask, (XEvent *) &ev);
}

int nameToPixel(name, pixel)
char *name;
unsigned long *pixel;
{
	XColor color;

	if (!XParseColor(dpy, wa.colormap, name, &color)) {
#ifdef DEBUG
	    fprintf(stderr, "Unknown color '%s'\n", name);
#endif
	    return 0;
	}
	if (!XAllocColor(dpy, wa.colormap, &color)) {
#ifdef DEBUG
	    fprintf(stderr, "Cannot allocate color '%s'\n", name);
#endif
	    return 0;
	}
	*pixel= color.pixel;
	return 1;
}

resize_check()
{
	XEvent evr;

	if (XCheckWindowEvent(dpy, win, xeventmask, &evr) == True) {;
		if (evr.type == ConfigureNotify) /* Resize event */
			resize(evr.xconfigure.width, evr.xconfigure.height);
	}
}

closepl()
{
	XFreeGC(dpy,gc);
	XFreeGC(dpy, fillgc);
	XFreePixmap(dpy,pixmap);
	if (inittile) {
		inittile = 0;
		XFreePixmap(dpy, filltile);
	}
	XDestroyWindow(dpy,win);
}
