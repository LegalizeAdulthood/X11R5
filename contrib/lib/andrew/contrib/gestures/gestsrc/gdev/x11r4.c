/***********************************************************************

x11r4.c - X11 R4 back end for GDEV graphics routines

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.


**********************************************************************/
/*

 7/7/91	Dean Rubine	created
 7/17/91 Dean Rubine	found why twm was ignoring XCreateSimpleWindow position:
			   XSetStandardProperties wasn't telling.


  X11 R4  driver for GDEV device independent graphics routines

  WARNING: Very Incomplete.  This file is a complete hack job of xdev.c,
  the x10 driver I've been using.  Many parts are unimplemented.  Menus
  aren't there, for example.  Most of the variables didn't make it either.

 Variables supported (assigned via GDEVsets or GDEVseti):

	program  	name of program to use
	currentfunction	"copy" or "xor"  
	thickness	in-pixels
	currentlinetype  "solid" or "dashed"
	currentcolor  "white" or "black"
 */

#include <stdio.h>
#include "gdrv.h"
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#ifdef BUFFERLINES	/* buffer up lines before calling XDraw, needs -loldX */
#	include <X11/X10.h>
#endif

#ifdef HUH
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/cursorfont.h>
#endif

#ifndef TRUE
#	define TRUE 1
#	define FALSE  0
#endif


#undef DEBUG

#define MENU_EVENT	MOUSE_EVENT(MIDDLE_BUTTON, DOWN_TRANSITION)

typedef int COORD;

/* statics */

#define	INIT_WIDTH	197
#define	INIT_HEIGHT	189
#define	INIT_XCORNER	1
#define	INIT_YCORNER	2

/* line drawing */

#define	NVERTEX	10240

int linecalls = 0;

#define	NWINDOWS	10
static int nwindows = 0;

#define NALTERNATEFONTS	10

static Display *display = NULL;
static int screen_id;
static Window root_id;
static unsigned long black_pixel, white_pixel;
static XGCValues values_struct;

typedef struct gwindow {
	Window win;
	int gdevhandle;
	char *program;
	char *font;
	COORD height, width;
	int xcorner, ycorner;
	/* FontInfo *wfontinfo; */
	unsigned long inputmask;
	int pane;
	int  currentcolor;
	int  currentfunction;
	int  thickness;
	int  xlineshift, ylineshift;
	int linetype;
	char *Xgeometry;
	char *Xdefault;
#ifdef BUFFER
	Vertex *vertex;
	int curvertex;
#endif
	Cursor cursor;
	int nalternatefonts;
	struct alternatefont {
		int	 key;	
		char	 *font;
		/* FontInfo *wfontinfo; */
		short	*widths;
	} alternatefont[NALTERNATEFONTS];
	GC gc;
} *Gwindow;

static struct gwindow initialw = {
	NULL,				/* win */
	-1,				/* gdevhandle */
	"xdev",				/* program */
	"timrom12b",			/* font */
	INIT_HEIGHT, INIT_WIDTH,		/* height, width */	
	INIT_XCORNER, INIT_YCORNER,		/* xcorner, ycorner */
	/* NULL,				/* wfontinfo */
	ExposureMask | KeyPressMask,	/* inputmask */
	0,				/* pane */
	0,				/* currentcolor */
	GXcopy,				/* currentfunction */
	1,				/* thickness */
	0, 0,				/* xlineshift, ylineshift */
	LineSolid,			/* linetype */
	NULL,				/* Xgeometry */
	"=1000x300-0+460",		/* Xdefault */
#ifdef BUFFER
	NULL,				/* vertex */
	0,				/* curvertex */
#endif
	0,				/* cursor */
};

static struct gwindow window[NWINDOWS];

static
X11currentcolor(w, var, value, arg)
Gwindow w;
char *var, *value, *arg;
{
	lineflush(w);
	if(DisplayPlanes(display, screen_id) == 1) {	/* monochrome */
		switch(value[0]) {
		case 'w': w->currentcolor = white_pixel;
		default: w->currentcolor = black_pixel;
		}
	} else {			/* color */
		Colormap cmap = DefaultColormap(display, screen_id);
		XColor xc, xdbc;

		if(! XAllocNamedColor(display, cmap, value, &xc, &xdbc)) {
			printf("%s: Unknown color, ignoring\n", value);
			return;
		}
		printf("Color %s: RGB=(%d %d %d)\n", value, xc.red, xc.green, xc.blue);
		w->currentcolor = xc.pixel;
	}
	X11updateGC(w);
}

static
X11currentfunction(w, var, value, arg)
Gwindow w;
char *var, *value, *arg;
{
	lineflush(w);
	switch(value[0]) {
	case 'x':	w->currentfunction = GXinvert;	break;
	default:	w->currentfunction = GXcopy;	break;
	}
	X11updateGC(w);
}

static
X11currentlinetype(w, var, value, arg)
Gwindow w;
char *var, *value, *arg;
{
	lineflush(w);
	switch(value[0]) {
	case 's':	w->linetype = LineSolid;	break;
	case 'd':	w->linetype = LineDoubleDash;	break;
	}
	X11updateGC(w);
}

#ifdef HUH


static
X11currentcursor(w, var, value, arg)
Gwindow w;
char *var, *arg;
int value;
{
	/* printf("cursor = %d\n", value); */
	XDefineCursor(w->win, value);
}
#endif

static
X11thickness(w, var, value, arg)
Gwindow w;
char *var, *arg;
int value;
{
	lineflush(w);

	w->thickness = value;
	w->xlineshift = -value / 2;
	w->ylineshift = -value / 2;
	X11updateGC(w);
}


static
X11map(w, var, value, arg)
Gwindow w;
char *var, *arg, *value;
{
	XMapWindow(display, w->win);
}

static
X11unmap(w, var, value, arg)
Gwindow w;
char *var, *arg, *value;
{
	XUnmapWindow(display, w->win);
}

#ifdef HUH
static int
X11stringwidth(w, var, value, arg)
Gwindow w;
char *var, *value, *arg;
{
	static short _widths[128];
	register short *widths = _widths;
	register int sum;
	register char *p;
	register struct alternatefont *af;

	if(_widths['A'] == 0 ) {
		computewidths(w->wfontinfo, widths);
	}

	for(af = &w->alternatefont[0];
	    af < &w->alternatefont[w->nalternatefonts]; af++)
		if(*value == af->key) {
			widths = af->widths;
			value++;
			break;
		}


	for(sum = 0, p = value; *p != '\0'; )
		sum += widths[*p++ & 0177];

	return sum;
}

static
computewidths(fontinfo, widths)
FontInfo *fontinfo;
short *widths;
{

/*
	XStringWidth(value, w->wfontinfo, 0, 0);
	XQueryWidth(value, w->wfontinfo->id);
	XCharWidths...

	all have this bizzare off-by-one error, e.g. they treat
		IBM as HAL
*/
	char chars[128];
	register char *p;
	for(p = chars; p < &chars[127]; p++)
		*p = p - chars - 1;  /* handle bizzare bug */
	XCharWidths(chars, 127, fontinfo->id, widths);
}

static int
X11stringheight(w, var, value, arg)
Gwindow w;
char *var, *value, *arg;
{
	return w->wfontinfo->height;
}

static
X11eventstring(w, var, value, arg)
Gwindow w;
char *var, *value, *arg;
{
	switch(*value) {
	case 'c': case 'C':
		XCompressEvents();
		break;
	case 'e': case 'E':
		XExpandEvents();
		break;
	default:
		printf("X11eventstring(%s,%s)???\n", var, value);
		break;
	}
}

#endif

static
X11program(w, var, value, arg)
Gwindow w;
char *var, *value, *arg;
{
	w->program = value;

	if(w->win) {
		int argc = 1; char *argv[1];
		static XSizeHints z;
		argv[0] = w->program;
		z.flags = 0;
		if(w->xcorner != INIT_XCORNER || w->ycorner != INIT_YCORNER) {
			z.x = w->xcorner, z.y = w->ycorner;
			z.flags |= USPosition;
		}
		if(w->width != INIT_WIDTH || w->height != INIT_HEIGHT) {
			z.width = w->width, z.height = w->height;
			z.flags |= USSize;
		}
		XSetStandardProperties(display, w->win, w->program,
			w->program, None, argv, argc, &z);
	}
}

static
X11font(w, var, value, arg)
Gwindow w;
char *var, *value, *arg;
{
	w->font = value;
}

#ifdef HUH
static
X11alternatefont(w, var, value, arg)
Gwindow w;
char *var, *value, *arg;
{
	register struct alternatefont *af =
		&w->alternatefont[w->nalternatefonts++];

	if(w->nalternatefonts > NALTERNATEFONTS)
		xerror("too many fonts");
	af->font = value;
	if ((af->wfontinfo = XOpenFont (af->font)) == NULL)
		xerror("XOpenFont (%s) failed!\n", af->font);
	af->key = w->nalternatefonts;	/* changable via X11assignfont */
	af->widths = (short *)malloc(128*sizeof(short));
	if(af->widths == NULL)
		xerror("X11alternatefont: no mem");
	computewidths(af->wfontinfo, af->widths);
}
#endif

/* if a text string begins (first char) with value, use current font */
static
X11assignfont(w, var, value, arg)
Gwindow w;
char *var; int value; char *arg;
{
	register struct alternatefont *af =
		&w->alternatefont[w->nalternatefonts - 1];

	if(w->nalternatefonts == 0)
		xerror("X11assignfonts: no current alternate font");
	af->key = value;
}

static
X11Xgeometry(w, var, value, arg)
Gwindow w;
char *var, *value, *arg;
{
	w->Xgeometry = value;
}

static
X11Xdefault(w, var, value, arg)
Gwindow w;
char *var, *value, *arg;
{
	w->Xdefault = value;
}
#ifdef HUH
#define	PSTACKSIZE	10
static int pstacktop;
static struct pstack {
	Pixmap pixmap;
	int x, y, w, h;
} pstack[PSTACKSIZE];

static
X11pushregion(w, var, value, arg)
Gwindow w;
char *var, *value, *arg;
{
	register struct pstack *p = &pstack[pstacktop++];
	if(pstacktop > PSTACKSIZE)
		xerror("PSTACKSIZE");
	if(sscanf(value, "%d %d %d %d", &p->x, &p->y, &p->w, &p->h) != 4)
		xerror("X11Xpushregion");
	p->w++, p->h++;	/* can't really hurt */
	X11getsizes(w);
	if(p->w > w->width - p->x) p->w = w->width - p->x;
	if(p->h > w->height - p->y) p->h = w->height - p->y;

	p->pixmap = XPixmapSave(w->win, p->x, p->y, p->w, p->h);
	XPixSet(w->win, p->x, p->y, p->w, p->h, WhitePixel);
}

static
X11popregion(w, var, value, arg)
Gwindow w;
char *var, *value, *arg;
{
	register struct pstack *p = &pstack[--pstacktop];
	if(pstacktop < 0) xerror("X11Xpopregion");
	XPixmapPut(w->win, 0, 0, p->x, p->y, p->w, p->h, p->pixmap,
		GXcopy, AllPlanes);
	XFreePixmap(p->pixmap);
}
#endif

/*
  initilization - doesn't create a window, since the program may supply
  variables which alter the window's shape
 */

#ifdef HUH
#define cursor_width 16
#define cursor_height 16
#define cursor_x_hot 0
#define cursor_y_hot 0
static short cursor_bits[] = {
   0x001f, 0x0007, 0x000f, 0x001d,
   0x0039, 0x0070, 0x00e0, 0x01c0,
   0x0380, 0x0700, 0x0e00, 0x1c00,
   0x3800, 0x1000, 0x0000, 0x0000
};

#define cursor_mask_width 16
#define cursor_mask_height 16
#define cursor_mask_x_hot 15
#define cursor_mask_y_hot 0
static short cursor_mask_bits[] = {
   0x003f, 0x003f, 0x001f, 0x003f,
   0x007f, 0x00fb, 0x01f0, 0x03e0,
   0x07c0, 0x0f80, 0x1f00, 0x3e00,
   0x7c00, 0x3800, 0x1000, 0x0000
};
#endif

Pointer
X11init(gdevhandle)
int gdevhandle;
{
	int X11input();
	Gwindow w;

	if(nwindows == 0) {	/* first time called */
		if ((display = XOpenDisplay (NULL)) == NULL)
			xerror("could not open display!");

		screen_id = DefaultScreen(display);
		/* root_id = DefaultRootWindow(display); /* works ! */
		/* root_id = RootWindowOfScreen(screen_id); /* doesn't work */
		root_id = RootWindow(display, screen_id);  /* works! */
		black_pixel = BlackPixel(display, screen_id);
		white_pixel = WhitePixel(display, screen_id);

#ifdef KAKA
/* there's some fishyness about screens I haven't figured out.
	Sometimes they're integers, and sometimes pointers to Screen
	structures, and it's not clear which macros expect which */
		screen_id = XDefaultScreenOfDisplay(display);
		root_id = XRootWindowOfScreen(screen_id);
		black_pixel = XBlackPixelOfScreen(screen_id);
		white_pixel = XWhitePixelOfScreen(screen_id);
#endif

		GDRV00topleft(gdevhandle);

		GDRVvar_fcn(gdevhandle, "program", X11program, NULL);
		GDRVvar_fcn(gdevhandle, "font", X11font, NULL);
	
#ifdef HUH
		GDRVvar_fcn(gdevhandle,
				"alternatefont", X11alternatefont, NULL);
#endif
		GDRVvar_fcn(gdevhandle, "assignfont", X11assignfont, NULL);
		GDRVvar_fcn(gdevhandle, "Xgeometry", X11Xgeometry, NULL);
		GDRVvar_fcn(gdevhandle, "Xdefault", X11Xdefault, NULL);
		GDRVvar_fcn(gdevhandle,
				"currentfunction", X11currentfunction, NULL);
		GDRVvar_fcn(gdevhandle,
				"thickness", X11thickness, NULL);
		GDRVvar_fcn(gdevhandle,
				"currentlinetype", X11currentlinetype, NULL);
		GDRVvar_fcn(gdevhandle,
				"currentcolor", X11currentcolor, NULL);
#ifdef HUH
		GDRVvar_fcn(gdevhandle, "Xevents", X11eventstring, NULL);

		GDRVvar_fcn(gdevhandle, "pushregion", X11pushregion, NULL);
		GDRVvar_fcn(gdevhandle, "popregion", X11popregion, NULL);

		GDRVvar_fcn(gdevhandle,
				"currentcursor", X11currentcursor, NULL);
		GDRVvar_fcn(gdevhandle,
				"stringwidth", X11stringwidth, NULL);
		GDRVvar_fcn(gdevhandle,
				"stringheight", X11stringheight, NULL);
		GDRVvar_fcn(gdevhandle, "map", X11map, NULL);
		GDRVvar_fcn(gdevhandle, "unmap", X11unmap, NULL);
#endif
		GDRVfdnotify(FD_POLL, X11input);
		GDRVfdnotify(ConnectionNumber(display), X11input); 

		initialw.currentcolor = black_pixel;
#ifdef HUH
		initialw.cursor = XCreateCursor(cursor_width, cursor_height,
				    cursor_bits, cursor_mask_bits,
				    cursor_x_hot, cursor_y_hot,
				    WhitePixel, BlackPixel, GXxor);
#endif

	}

	if(nwindows >= NWINDOWS)
		xerror("NWINDOWS");
	w = &window[nwindows++];
	*w = initialw;
	w->gdevhandle = gdevhandle;
	return (Pointer) w;
}

static
X11getsizes(w)
Gwindow w;
{
	XWindowAttributes win_attr;

	if (! XGetWindowAttributes(display, w->win, &win_attr))
		xerror("XGetWindowAttributes");

	w->width = win_attr.width;
	w->height = win_attr.height;
	w->xcorner = win_attr.x;
	w->ycorner = win_attr.y;
}
 
X11start(w)
Gwindow w;
{
	if(w->win == 0) {
		char *X11GetDefault();
		int x = w->xcorner, y = w->ycorner;
		int width = w->width, height = w->height;
		int r = 0;
		if(w->Xgeometry == NULL)
			w->Xgeometry = X11GetDefault(w->program, "Geometry");
		if(w->Xgeometry != NULL)
			r = XParseGeometry(w->Xgeometry, &x, &y, &width, &height);
/*
   My twm seems to ignore X and Y in CreateSimpleWindow, but I can't figure out
   why.  All I know is that I really hate X.
*/
/*
		printf("'%s' r=%x  x=%d y=%d, width=%d height=%d\n",
			w->Xgeometry? w->Xgeometry : "NULL", r,
			x, y, width, height);
*/

		w->win = XCreateSimpleWindow(display, root_id,
					    x, y, width, height,
					    2,
					    black_pixel, white_pixel);
		if (w->win == 0)
			xerror("XCreateSimpleWindow failed!");

		w->xcorner = x, w->ycorner = y;
		w->width = width, w->height = height;

		X11program(w, "program", w->program, NULL);

		{ char *p;
		  p = X11GetDefault(w->program, "foreground");
		  if(p) X11currentcolor(w, "currentcolor",  p, NULL);
		  p = X11GetDefault(w->program, "thickness");
		  if(p) X11thickness(w, "thickness", atoi(p), NULL);
		}

		w->gc = XCreateGC(display, w->win, 0L, &values_struct);
		if(w->gc == NULL)
			xerror("XCreateGC");
		X11updateGC(w);

#ifdef BUFFER
		w->vertex = (Vertex *) malloc((NVERTEX+2) * sizeof(Vertex));
		if(w->vertex == NULL)
			xerror("Vertex malloc");
#endif

#ifdef HUH
		if ((w->wfontinfo = XOpenFont (w->font)) == NULL)
			xerror("XOpenFont (%s) failed!\n", w->font);
#endif

#ifdef HUH
		XDefineCursor(w->win, w->cursor);
#endif

		XSelectInput(display, w->win, w->inputmask);

		XClearWindow(display, w->win);
		XMapWindow(display, w->win);

		/* wait for expose event */
		{ XEvent xevent;
		  do {
			XNextEvent(display, &xevent);
			if(gdevdebug > 1)
			   printf("[[%d]] ", xevent.xany.type), fflush(stdout);
		  } while(xevent.xany.type != Expose);
		}
		
		X11getsizes(w);
	}
	else {
		XClearWindow(display, w->win);
		XMapWindow(display, w->win);
	}
}

X11flush(w)
Gwindow w;
{
	XRaiseWindow(display, w->win);	/* try it */
	lineflush(w);
	XFlush(display);
}

X11stop(w)
Gwindow w;
{
	X11flush(w);
}

static
lineflush(w)
register Gwindow w;
{
#ifdef BUFFER
	if(w->curvertex > 0) {
		XDraw (display, w->win, w->gc, w->vertex, w->curvertex);
		linecalls = 0;
		w->curvertex = 0;
	}
#endif
}

X11line(w, x1, y1, x2, y2)
register Gwindow w;
COORD x1, y1, x2, y2;
{

#ifdef BUFFER
	register Vertex *v;
	static push1first = TRUE;
	linecalls++;
		/* heuristic:
			if push1first = TRUE client likely to do:
				line(x1, y1, x2, y2);
				line(x2, y2, x3, y3);
			    (so we first push x1,y1 then x2,y2 so
				the duplicated point is pushed last)
			if push1first = FALSE client likely to do:
				line(x1, y1, x2, y2);
				line(x3, y3, x1, y1);
			    (so we first push x2,y2 then x1,y1 so
				the duplicated point is pushed last)
		*/

	if(w->curvertex >= NVERTEX)
		lineflush(w);
	v = &w->vertex[w->curvertex-1];

	x1 += w->xlineshift; x2 += w->xlineshift;
	y1 += w->ylineshift; y2 += w->ylineshift;

	/* fancy line caching doesn't work if w->currentfunction == GXinvert */
	if(w->currentfunction == GXinvert) {
			++v; ++w->curvertex;
			v->x = x1; v->y = y1; v->flags = VertexDontDraw;
			++v; ++w->curvertex;
			v->x = x2; v->y = y2; v->flags = 0;
	}
	else if(w->curvertex > 0 && v->x == x1 && v->y == y1) {
		++v; ++w->curvertex;
		v->x = x2; v->y = y2; v->flags = 0;
	}
	else if(w->curvertex > 0 && v->x == x2 && v->y == y2) {
		++v; ++w->curvertex;
		v->x = x1; v->y = y1; v->flags = 0;
	}
	else {
		if(w->curvertex > 1) {
			--v;
			if(v->x == x1 && v->y == y1)
				push1first = FALSE;
			else if(v->x == x2 && v->y == y2)
				push1first = TRUE;
			++v;
		}
		if(push1first) {
			++v; ++w->curvertex;
			v->x = x1; v->y = y1; v->flags = VertexDontDraw;
			++v; ++w->curvertex;
			v->x = x2; v->y = y2; v->flags = 0;
		} else {
			++v; ++w->curvertex;
			v->x = x2; v->y = y2; v->flags = VertexDontDraw;
			++v; ++w->curvertex;
			v->x = x1; v->y = y1; v->flags = 0;
		}
	}
#else
	linecalls++;
	XDrawLine(display, w->win, w->gc, x1, y1, x2, y2);
#endif

#ifdef DEBUG
	printf("line %d %d  %d %d\n", x1, y1, x2, y2);
#endif

}

X11rect(w, x, y, x2, y2)
Gwindow w;
COORD x, y, x2, y2;
{
#ifdef BUFFER
	int rh = y2 - y;
	int rw = x2 - x;
	static Vertex v[5];

	v[0].x = x; v[0].y = y; v[0].flags = 0;
	v[1].x = rw; v[1].y = 0; v[1].flags = VertexRelative;
	v[2].x = 0; v[2].y = rh; v[2].flags = VertexRelative;
	v[3].x = -rw; v[3].y = 0; v[3].flags = VertexRelative;
	v[4] = v[0];

	XDraw (display, w->win, w->gc, v, 5);
#else
	X11line(w, x, y, x, y2);
	X11line(w, x, y2, x2, y2);
	X11line(w, x2, y2, x2, y);
	X11line(w, x2, y, x, y);
#endif
}

X11text(w, x, y, text)
Gwindow w;
COORD x, y;
register char *text;
{
	int xoff = 0, yoff = -15;
	register struct alternatefont *af;

#ifdef DEBUG
	printf("text %d %d %s\n", x, y, text);
#endif

#ifdef HUH
	register int fid;
	fid = w->wfontinfo->id;
	for(af = &w->alternatefont[0];
	    af < &w->alternatefont[w->nalternatefonts]; af++)
		if(*text == af->key) {
			fid = af->wfontinfo->id;
			text++;
			break;
		}
#endif

	XDrawString(display, w->win, w->gc, x+xoff, y+yoff,
			text, strlen(text));
}

X11point(w, x, y)
Gwindow w;
COORD x, y;
{
#ifdef DEBUG
	printf("point  %d %d\n", x, y);
#endif

	X11rect(w, x + w->xlineshift, y + w->ylineshift,
		x+w->thickness, y+w->thickness);
}

X11setdim(w, width, height)
Gwindow w;
COORD width, height;
{
	XWindowChanges v;

	v.width = width;
	v.height = height;
	XConfigureWindow(display, w->win, CWWidth | CWHeight, &v);
}

X11getdim(w, wp, hp)
Gwindow w;
COORD *wp, *hp;
{
	COORD ox = w->width, oy = w->height;

	X11getsizes(w);

	if(ox != INIT_WIDTH && (ox != w->width || oy != w->height))
		printf("X11: Size changed unexpectedly!\n");
	/* If the above printf never comes up, we can get rid of the call to
	   X11getsizes and everything else above this line */

	*wp = w->width;
	*hp = w->height;
}

X11menuitem(w, itemname, retval, addflag)
Gwindow w;
char *itemname;
int retval;
int addflag;
{
#ifdef HUH
    char *data;
    int r;

    if(w->menu == NULL) {
	int X11event();

	if( (w->menu = XMenuCreate(RootWindow, w->program)) == NULL)
		xerror("XMenuCreate");
	if((w->pane = XMenuAddPane(w->menu, w->program, TRUE)) == XM_FAILURE)
		xerror("XMenuAddPane");
	XMenuEventHandler(X11event);
	X11mouseinterest(w, MENU_EVENT);
    }

    if(addflag) {
	data = (char *) retval;
	if(XMenuAddSelection(w->menu, w->pane, data, itemname, TRUE) == XM_FAILURE)
		xerror("XMenuAddSelection");
    }
    else {
	if((r = XMenuFindSelection(w->menu, w->pane, itemname)) == XM_FAILURE)
		xerror("XMenuFindSelection");
	if( XMenuDeleteSelection(w->menu, w->pane, r) == XM_FAILURE)
		xerror("XMenuDeleteSelection");
   }
#endif
}

X11mouseinterest(w, event)
Gwindow w;
int event;
{
	register oldmask = w->inputmask;

	if(event & DOWN_TRANSITION)
		w->inputmask |= ButtonPressMask;
	if(event & UP_TRANSITION)
		w->inputmask |= ButtonReleaseMask;
	if(event & DOWN_MOVEMENT) {
		switch(event & BUTTON_MASK) {
		case LEFT_BUTTON:
			w->inputmask |= Button1MotionMask; break;
		case MIDDLE_BUTTON:
			w->inputmask |= Button2MotionMask; break;
		case RIGHT_BUTTON:
			w->inputmask |= Button3MotionMask; break;
		}
	}

	if(w->inputmask != oldmask)
		XSelectInput(display, w->win, w->inputmask);
}

/* map x window handles into internal structure */

static Gwindow
X11LookupGwindow(win)
Window win;
{
	register Gwindow w;
	for(w = window; w < &window[nwindows]; w++)
		if(w->win == win)
			return w;
	/* xerror("LookupGwindow"); */
	printf("LookupGwindow\n");
	return &window[0];
}

/* structure used to communicate between X11input and X11event */

struct menubutton {
	int	activate;
	int	x, y;
	Window	win;
};

static
X11input()
{
	XEvent xevent, moved_event;
	struct menubutton menubutton;

	menubutton.activate = FALSE;
	moved_event.xany.type = 0;
	while(XPending(display) > 0) {
	    XNextEvent(display, &xevent);
	    if(gdevdebug > 1) printf("[[%d]] ", xevent.xany.type), fflush(stdout);
	    /* we try to process only the last MotionNotify, but if the input
	       window changes, we punt on this idea */
	    if(moved_event.xany.type == MotionNotify &&
	       moved_event.xany.window != xevent.xany.window) {
	    		X11event(&moved_event, &menubutton);
			moved_event.xany.type = 0;
	    }
	    /* similarly for menu button */
	    if( menubutton.activate &&
		menubutton.win != xevent.xany.window ) {
			DoMenu(&menubutton);
			menubutton.activate = FALSE;
		}

	    switch((int) xevent.xany.type) {
	    case MotionNotify:
		/* only the last mouse move event is saved so that tracking
		   appears faster */
		moved_event = xevent;
		break;
	    case ButtonPress:
	    case ButtonRelease:
		moved_event.xany.type = 0;
		/* fall into ... */
	    default:
	    	X11event(&xevent, &menubutton);
	    }
	}

	if(moved_event.xany.type == MotionNotify)
	    X11event(&moved_event, &menubutton);

	if( menubutton.activate)
		DoMenu(&menubutton);

}


#include <sys/time.h>

static
X11event(xe, menubuttonp)
register XEvent *xe;
struct menubutton *menubuttonp;
{
    struct timeval tv;
    register Gwindow w = X11LookupGwindow(xe->xany.window);

    if(xe->type == ButtonPress || xe->type == ButtonRelease ||
       xe->type == MotionNotify)
    {
	register XButtonEvent *xb = (XButtonEvent *) xe;

	register int event;
	int x, y;
	static int lastbutton = 0;

	if(xb->type == ButtonPress || xb->type == ButtonRelease) {
		switch(xb->button) {
		case Button1: lastbutton = LEFT_BUTTON; break;
		case Button2: lastbutton = MIDDLE_BUTTON; break;
		case Button3: lastbutton = RIGHT_BUTTON; break;
		default: lastbutton = -1; break;
		}
	}

	x = xb->x; y = xb->y;
	gettimeofday(&tv, NULL);
	event = MOUSE_EVENT(lastbutton,
		xb->type == ButtonPress ? DOWN_TRANSITION :
		xb->type == ButtonRelease ? UP_TRANSITION :
					    DOWN_MOVEMENT);

	/* printf("event=%x x=%d y=%d\n", event, x, y); */

	GDRVmouse(w->gdevhandle,
		  event, x, y, (tv.tv_sec * 1000) + (tv.tv_usec / 1000));

	/* special check for middle button to bring up menu */

#ifdef HUH
	if(event == MENU_EVENT) {
		static Window dummy;
		menubuttonp->activate = TRUE;
		XInterpretLocator(RootWindow, &menubuttonp->x, &menubuttonp->y,
			&dummy, xb->location);
		menubuttonp->win = xe->window;
	}
#endif
  }
  else if(xe->type == Expose)
  {
	X11getsizes(w);
	GDRVrefresh(w->gdevhandle);
   }
  else if(xe->type == KeyPress)
  {
	char buffer[20];
	register int i, nbytes;
	register XKeyReleasedEvent *xk = (XKeyReleasedEvent *) xe;

	nbytes = XLookupString(xk, buffer, sizeof(buffer), NULL, NULL);
	for(i = 0; i < nbytes; i++) {
		GDRVwindow(w->gdevhandle);
		GDRVputc(buffer[i]);
	}
  }
  else printf("Funny X event %d (0x%x)\n", xe->type, xe->type);
}

DoMenu(mb)
struct menubutton *mb;
{
#ifdef HUH
        register Gwindow w = X11LookupGwindow(mb->win);
	char *retval;
	int a_pane = w->pane;
	int a_sel = 0;
	int mx, my, mw, mh;
	int menux, menuy;

	WindowInfo wi, ri;	/* plot window, and root window, resp */
	int wrightx, wlowery, rrightx, rlowery;
 
	if(w->menu == NULL)
		return;

	if( ! XQueryWindow(w->win, &wi) )
		xerror("XQueryWindow win");
	if( ! XQueryWindow(RootWindow, &ri) )
		xerror("XQueryWindow root");
	if(XMenuLocate(w->menu, a_pane, a_sel, mb->x, mb->y,
		&mx, &my, &mw, &mh) == XM_FAILURE) {
			printf("XMenuLocate\n");
			return;
	}

	mw+=2; mh+=2; wi.width+=2; wi.height+=2 ;  /* avoid boundary conds */

	wrightx = wi.x + wi.width; wlowery = wi.y + wi.height;
	rrightx = ri.x + ri.width; rlowery = ri.y + ri.height;

	menux = mx; menuy = my;


	if(wi.x - mw > ri.x) /* Try to fix menu to the left of win */
		menux = wi.x - mw;
	else if(wrightx + mw < rrightx) /* right */
		menux = wrightx;
	else if(wi.y - mh > ri.y) /* above */
		menuy = wi.y - mh;
	else if(wlowery + mh < rlowery) /* below */
		menuy = wlowery;

	menux += mb->x - mx;
	menuy += mb->y - my;

	/* Try to locate menu off screen */
	XWarpMouse(RootWindow, menux, menuy);
	if(XMenuActivate(w->menu, &a_pane, &a_sel,
		    menux, menuy,
		    ButtonReleased, &retval) == XM_SUCCESS)
			GDRVmenu(w->gdevhandle,  (int) retval );
	XWarpMouse(RootWindow, mb->x, mb->y);
	X11flush(w);
#endif
}

/*VARARGS1*/
xerror(a, b, c, d)
char *a;
{
	printf("X11 error: ");
	printf(a, b, c, d);

#ifdef HUH
	if(!strncmp(a, "XMenu", 5))
		printf("(%s)", XMenuError());
#endif
	printf("\n");
	exit(1);
}

/*
 * XGetDefault ignores the program name in all but the first call to it.
 * Thus, if XGetDefault("prog1", "Geometry") is called first,
 * XGetDefault("prog2", "Geometry") will necessarily return the same
 * result.  Since there's no easy way to get around this, we first look
 * up XGetDefault("prog2", "prog2_Geometry") before we look up
 * XGetDefault("prog2", "Geometry"), so in .Xdefaults the user can have
 * entries like:
	prog1.Geometry:=500x500+0+0
	prog1.prog2_Geometry:=500x500+0-0
 */


char *
X11GetDefault(program, name)
char *program, *name;
{
	char n[200], *r;

	sprintf(n, "%s_%s", program, name);
	if((r = XGetDefault(display, program, n)) == NULL)
		r = XGetDefault(display, program, name);
	return r;
}

X11updateGC(w)
Gwindow w;
{
	if(w->gc == NULL)	/* window not created yet */
		return;
	values_struct.line_width = w->thickness;
	/* values_struct.line_width = 20;	/* debug */
	values_struct.line_style = w->linetype;
	if(w->currentfunction == GXinvert &&
	   DisplayPlanes(display, screen_id) != 1) {
		values_struct.function = GXxor;
		values_struct.foreground = w->currentcolor ^ white_pixel;
		values_struct.background = 0;
		/* Who knows? */
	}
	else {
		values_struct.function = w->currentfunction;
		/* values_struct.function = GXcopy; /* debug */
		values_struct.foreground = w->currentcolor;
		values_struct.background = white_pixel;
	}
	values_struct.plane_mask = AllPlanes;
	values_struct.dashes = 3;

	XChangeGC(display, w->gc,
		GCPlaneMask | GCDashList | GCBackground |
		GCLineStyle | GCLineWidth | GCFunction | GCForeground,
	   &values_struct);
}
