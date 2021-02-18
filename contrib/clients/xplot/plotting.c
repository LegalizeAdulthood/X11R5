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
#include <X11/Xlib.h>

#define LINE 512
char buf[LINE];

/* The data buffer - form depends on whether we allow resizing */
#ifdef Resize_Yes /* Whether or not to do buffering and proper resizes here */
char *data = NULL; /* Stores the input data since last erase */
int data_len = 0; /* Size of allocated data region */
#else
char data[LINE];
int data_len = LINE;
#endif /* Resize_Yes */
int data_head = 0; /* Location of next plot command in data */
int data_tail = 0; /* Location for storing next input characters */

/* The only X stuff we need here: */
extern Display *dpy;
extern Window win;
extern Pixmap pixmap;
extern GC gc;
extern GC fillgc;
extern int xeventmask;

extern XFontStruct *font;
extern int fgcolor, bgcolor;
extern int initfg, initbg;
extern XFontStruct *initfont;

extern FILE *instrm;
extern int forkthistime;
extern int forked_or_ended;
extern int eoffound;
extern int endinp;

extern int rwidth, rheight;
extern short xul, yul, xlr, ylr;
extern int filling;

extern int pathind;

extern int debug_flag;

doplot()
{
	while(readit(instrm)){
		do_graphics();
	}
	XClearArea(dpy,win,0,0,0,0,True); /* Show the image! */
	XFlush(dpy);
	if (eoffound) {
		forked_or_ended = 1;
		if (forkthistime) {
			if (fork()) return;
			buttonwait();  /* Child waits for a button press */
		}
	}
}

do_graphics()
{
	char c;
	short x[6];

	while((data_tail > data_head)&&(!endinp)){
		c = data[data_head++];
#ifdef DEBUG
		if (debug_flag) fprintf(stderr,"%c",c);
#endif

	/* Check if we've completed a path: */
		if ((pathind > 0)&&(c != 'n')) plotpath();

	/* Then do the plotting... */
		switch(c){
			case 'm': /* Move */
				if (getcoords(x,2,instrm))
					move(x[0],x[1]);
				break;
			case 'n': /* Continue */
				if (getcoords(x,2,instrm))
					cont(x[0],x[1]);
				break;
			case 'p': /* Plot */
				if (getcoords(x,2,instrm))
					point(x[0],x[1]);
				break;
			case 'l': /* Draw Line */
				if (getcoords(x,4,instrm))
					line(x[0],x[1],x[2],x[3]);
				break;
			case 't': /* Label */
				if (getstring(buf,instrm))
					label(buf);
				break;
		        case 'T':              /* Adjusted Label */
       			{
			        char x_justify, y_justify;
				if (data_tail - data_head < 2) {
					data_head--;
					break;
				}
			        x_justify = data[data_head++];
			        y_justify = data[data_head];
			/* cover up the justify characters */
				data[data_head--] = 'a';
				data[data_head] = 'a';
#ifdef DEBUG
            if (debug_flag) fprintf (ostrm, " %d %d", x_justify, y_justify);
#endif
			        if (getstring (buf, instrm))
			            alabel (x_justify, y_justify, buf + 2);
			}
			        break;

		       case 'a': /* Arc */
				if (getcoords(x,6,instrm))
					arc(x[0],x[1],x[2],x[3],x[4],x[5]);
				break;
			case 'c': /* Circle */
				if (getcoords(x,3,instrm))
					circle(x[0],x[1],x[2]);
				break;
			case 'e': /* Erase */
				erase();
				break;
			case 'f': /* Linemod */
				if (getstring(buf,instrm))
					linemod(buf);
				break;
			case 's': /* Space */
				if (getcoords(x,4,instrm))
					space(x[0],x[1],x[2],x[3]);
				break;
			case 'C': /* Color */
				if (getcoords(x, 3, instrm))
					color_by_number(x[0], x[1], x[2]);
				break;
			case 'F': /* Font name */
				if (getstring(buf, instrm))
					font_by_name(buf);
				break;
			case 'S': /* Font size */
				if (getcoords(x, 1, instrm))
					fontsize(x[0]);
				break;
			case 'r': /* rotate */
				if (getcoords ( x, 3, instrm))
					rotate(x[0], x[1], x[2]);
				break;
			case 'L': /* fill */
				if (getcoords(x, 1, instrm))
					fill(x[0]);
				break;
			default:
#ifdef DEBUG
				fprintf(stderr,
		"Unrecognized plot command '%c' (%o octal) ignored.\n",c);
#endif
				break;
		}
	}
	if (pathind > 0) plotpath();
}

buttonwait()
{
	XEvent evr;

	xeventmask |= ButtonPressMask;
	while (1) {
		XSelectInput(dpy, win, xeventmask);
		XWindowEvent(dpy, win, xeventmask, &evr);
		if (evr.type == ButtonPress) {
			closepl();
			exit(0);
		} else if (evr.type == ConfigureNotify) { /* Resize event */
			resize(evr.xconfigure.width, evr.xconfigure.height);
		}
	}
}

resize(w_new, h_new) /* Handle resize events - only occasion for repainting */
int w_new, h_new;
{
#ifdef Resize_Yes
	if ((w_new == rwidth) && (h_new == rheight))
		return; /* No real change in size */
	rwidth = w_new;
	rheight = h_new;

	filling = 0; /* Reset the things set initially. */
	XSetFillStyle(dpy, fillgc, FillSolid);
	font = initfont;
	fgcolor = initfg;
	bgcolor = initbg;
	XSetFont(dpy, gc, initfont->fid);
	XSetForeground(dpy,gc,initfg);
	XSetBackground(dpy,gc,initbg);

/* We also should restore the original font, color, line_mode, etc.? */

	XFreePixmap(dpy, pixmap); /* Free up the old pixmap */
	/* Make a new pixmap, more fitting for the new window size! */
	pixmap = XCreatePixmap(dpy,DefaultRootWindow(dpy), rwidth, rheight,
			DefaultDepth(dpy, DefaultScreen(dpy)));

	erase(); /* Clear the pixmap */
		/* And assign it to the window */
	XSetWindowBackgroundPixmap(dpy, win, pixmap);

	space(xul,yul,xlr,ylr); /* Make sure new scale factors are set up */
	data_head = 0; /* Start from beginning of data again */
	endinp = 0; /* Reset this flag */
	do_graphics(); /* Fill it with what's been drawn since last erase */
	XClearArea(dpy,win,0,0,0,0,True); /* Show the image! */
	XFlush(dpy);
#endif
}

init_data()
{
	data_head = data_tail = 0; /* Kill all the old data (new file) */
}
