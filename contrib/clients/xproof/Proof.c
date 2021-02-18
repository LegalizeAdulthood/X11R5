/*
 * Copyright 1991 University of Wisconsin-Madison
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of Wisconsin-Madison not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  The University of
 * Wisconsin-Madison makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF WISCONSIN-MADISON DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF WISCONSIN-MADISON BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:   Marvin Solomon
 *           Computer Sciences Department
 *           University of Wisconsin, Madison WI
 *           solomon@cs.wisc.edu or seismo!uwvax!solomon
 *
 * Current Maintainer:
 *
 *           Tim Theisen           Systems Programmer
 * Internet: tim@cs.wisc.edu       Department of Computer Sciences
 *     UUCP: uwvax!tim             University of Wisconsin-Madison
 *    Phone: (608)262-0438         1210 West Dayton Street
 *      FAX: (608)262-9777         Madison, WI   53706
 *
 * $Source: /src/X11/uw/xproof/RCS/Proof.c,v $
 *
 * Device-independent troff (ditroff) proof widget.
 *
 * A proof widget displays a device-independent troff output file in a window.
 * The "file" property (default NULL) is the name of a file to be displayed.
 * (If NULL, display stdin).
 * This class is a direct child of the root widget class (widgetClass).
 *
 * See also the following source files
 *    showpage.c:   the main loop that interprets ditroff output format
 *    draw.c:       graphics functions
 *    getdevinfo.c: read and interpret ditroff device descriptions
 *    xfontinfo.c:  find appropriate X fonts
 *
 * For your viewing pleasure, set tabs at 4-column intervals.
 */

#include <X11/Xos.h>
#include <X11/Xlib.h>                   /* for Xlib definitions */
#include <X11/cursorfont.h>             /* for cursor constants */
#include <X11/IntrinsicP.h>             /* for toolkit stuff */
#include <X11/StringDefs.h>             /* for useful atom names */
#include <X11/Xaw/Cardinals.h>
#include <ctype.h>
#include "xproof.h"                     /* other application stuff */
#include "ProofP.h"                     /* for implementation proof stuff */
#ifndef lint
static char rcsid[] =
	"$Header: /src/X11/uw/xproof/RCS/Proof.c,v 3.17 1991/10/04 22:05:44 tim Exp $";
#endif

/* Initialization of defaults */

#define PAPERWIDTH 8.5
#define PAPERLENGTH 11.0

static Dimension def_width = 700;
static Dimension def_height = 1000;
static Dimension def_scale = 10;

/* UsePixmap is set upon widget initialization, if it appears that the
 * server is one that does not adequately support pixmaps.  (The DEC qdss
 * server is such a server in the X.V11R3 distribution).  See the README
 * file for more details.
 */
static int UsePixmap = 0;

#ifndef USEPIXMAP
#define USEPIXMAP="false"
#endif USEPIXMAP

static XtResource resources[] = {
#define offset(field) XtOffsetOf(ProofRec,proof.field)
#define goffset(field) XtOffsetOf(ProofRec,core.field)
	/* {name, class, type, size,
		offset, default_type, default_addr}, */
	{ XtNwidth, XtCWidth, XtRDimension, sizeof (Dimension), 
		goffset(width), XtRDimension, (caddr_t)&def_width },
	{ XtNheight, XtCHeight, XtRDimension, sizeof (Dimension),
		goffset(height), XtRDimension, (caddr_t)&def_height },
	{ XtNforeground, XtCForeground, XtRPixel, sizeof (Pixel),
		offset(foreground_pixel), XtRString, "black" },
	{ XtNbackground, XtCBackground, XtRPixel, sizeof (Pixel),
		goffset(background_pixel), XtRString, "white" },
	{ XtNreverseVideo, XtCReverseVideo, XtRBoolean, sizeof (Boolean),
		offset(reverse_video), XtRString, "FALSE" },
	{ XtNusePixmap, XtCUsePixmap, XtRBoolean, sizeof (Boolean),
		offset(use_pixmap), XtRString, USEPIXMAP },
	{ XtNidleCursor, XtCCursor, XtRCursor, sizeof(Cursor),
		offset(idle_cursor), XtRString, "mouse"},
	{ XtNbusyCursor, XtCCursor, XtRCursor, sizeof(Cursor),
		offset(busy_cursor), XtRString, "watch"},
	{ XtNfile, XtCFile, XtRString, sizeof (String),
		offset(file), XtRString, NULL },
	{ XtNscale, XtCScale, XtRDimension, sizeof (Dimension),
		offset(scale), XtRDimension, (caddr_t)&def_scale },
#undef offset
#undef goffset
};

static void Destroy(), Initialize(), Realize(), Redisplay(), ClearWindow();

static void KeyEvent(), StringEvent();

static void ClearCanvas();

static Boolean SetValues();

static XtActionsRec actions[] =
{
  {"key",		KeyEvent},
  {"string",		StringEvent},
};

static char translations[] =
    "<Key>:	 key() \n\
     <Btn1Down>: string(b) \n\
     <Btn2Down>: string(f) \n\
     <Btn3Down>: string(q)";


ProofClassRec proofClassRec = {
	{ /* core fields */
	/* superclass               */      (WidgetClass) &widgetClassRec,
	/* class_name               */      "Proof",
	/* widget_size              */      sizeof(ProofRec),
	/* class_initialize         */      NULL,
	/* class_part_initialize    */      NULL,
	/* class_inited             */      FALSE,
	/* initialize               */      Initialize,
	/* initialize_hook          */      NULL,
	/* realize                  */      Realize,
	/* actions                  */      actions,
	/* num_actions              */      XtNumber(actions),
	/* resources                */      resources,
	/* resource_count           */      XtNumber(resources),
	/* xrm_class                */      NULLQUARK,
	/* compress_motion          */      TRUE,
	/* compress_exposure        */      TRUE,
	/* compress_enterleave      */      TRUE,
	/* visible_interest         */      FALSE,
	/* destroy                  */      Destroy,
	/* resize                   */      NULL,
	/* expose                   */      Redisplay,
	/* set_values               */      SetValues,
	/* set_values_hook          */      NULL,
	/* set_values_almost        */      XtInheritSetValuesAlmost,
	/* get_values_hook          */      NULL,
	/* accept_focus             */      NULL,
	/* version                  */      XtVersion,
	/* callback_private         */      NULL,
	/* tm_table                 */      translations,
	/* query_geometry           */      XtInheritQueryGeometry,
	/* display_accelerator      */		XtInheritDisplayAccelerator,
	/* extension                */		NULL
	},
	{ /* proof fields */
	/* dummy                    */      0
	}
};

WidgetClass proofWidgetClass = (WidgetClass) &proofClassRec;

/*
 * exported procedures
 */

/* Set the input file for the proof widget.
 * This must be done before the widget is realized.
 */
void XtProofSetFile(w, f)
ProofWidget w;
FILE *f;
{
	w->proof.file = f;
	w->proof.offset = -1;
}

/* Interpret the page starting at offset "offset" in
 * the input file, and display it.
 * If offset<0, don't seek, just start from the current location.
 */
void XtProofShowPage(w, offset)
ProofWidget w;
long offset;
{
	w->proof.offset = offset >= 0 ? offset : ftell(w->proof.file);
	w->proof.shown = 0;
	ClearWindow(w);  /* this will trigger an exposure event */
}

void XtProofSetScale(w, val, change)
ProofWidget w;
int val, change;
{
	Arg arg;
	Dimension scale;

	if (change==0) scale = val;
	else {
		double factor = change<0 ? 10.0/(double)val : (double)val/10.0;
		XtSetArg(arg, XtNscale, &scale);
		XtGetValues((Widget)w, &arg, ONE);
		scale = (Dimension)(scale*factor + 0.5);
	}

	XtSetArg(arg, XtNscale, scale);
	XtSetValues((Widget)w, &arg, ONE);
}


/*
 * private procedures
 */

/* ARGSUSED */
static void Initialize(request, new)
Widget request, new;
{
	ProofWidget w = (ProofWidget) new;
	XGCValues gcv;  /* for creating graphics contexts */

	UsePixmap = w->proof.use_pixmap;
	if (w->proof.reverse_video) {
		Pixel tmp = w->proof.foreground_pixel;
		w->proof.foreground_pixel = w->core.background_pixel;
		w->core.background_pixel = tmp;
	}
	screen_res = (int) ((((double) WidthOfScreen(XtScreen(w))) * 25.4) /
		((double) WidthMMOfScreen(XtScreen(w))));
	screen_res = screen_res >= 88 ? 100 : 75;
	scaled_res = (w->proof.scale * screen_res) / 10;

	w->core.width = (PAPERWIDTH * screen_res * w->proof.scale)/10;
	w->core.height = (PAPERLENGTH * screen_res * w->proof.scale)/10;

	/* Defaults for other state */
	w->proof.line_style = LineSolid;
	w->proof.thickness = 1;

	/* Create some graphics contexts */

	gcv.foreground = w->proof.foreground_pixel;
	gcv.background = w->core.background_pixel;

	/* DrawGc is used for drawing line and text.  The line_width, line_style,
	 * and dashes attributes are modified as necessary (by drawXthick() and
	 * drawXstyle()) for drawing lines, and the font is modified as necessary
	 * (in ShowChar()) for displaying text.
	 */
	w->proof.DrawGc = XtGetGC((Widget)w, GCForeground | GCBackground, &gcv);

	/* The fill context is used for filled polygons.
	 * Its stipple pixmap is changed, as necessary, by drawXpoly().
	 */
	gcv.fill_style = FillStippled;
	w->proof.FillGc = XtGetGC((Widget)w,
				  GCForeground | GCBackground | GCFillStyle, &gcv);

	if (UsePixmap) {
		/* The canvas-clear context is used to fill the canvas with the current
		 * background color.
		 */
		gcv.foreground = gcv.background; /* !! */
		w->proof.ClearGc = XtGetGC((Widget)w, GCForeground, &gcv);

		/* The copy context is used to copy the canvas to the window.  It
		 * is the default in every way.
		 */
		w->proof.CopyGc = XtGetGC((Widget)w, 0, &gcv);
	}
}

/* process one keyboard characater */
static void dochar(w, c)
Widget w;
int c;
{
	static int digits = 0;
	static int num = 0;

	if (isdigit(c)) {
		digits++;
		if (num != INFINITY)
			num = num*10 + c - '0';
		return;
	}
	if (!digits) num = 1;
	switch (c) {
		default:
			BEEP(w);
			break;
		case 's': case 'S':
			if (digits) XtProofSetScale((ProofWidget)w, num, 0);
			else BEEP(w);
			break;
			
		case 'r': case 'R':
		case '\f':
			SetPage(w, 0, True);
			break;
		case 'f': case 'F':
		case 'd': case 'D':
		case 'n': case 'N':
		case '+':
			SetPage(w, num, True);
			break;
		case 'b': case 'B':
		case 'u': case 'U':
		case 'p': case 'P':
		case '-':
			SetPage(w, -num, True);
			break;
		case '\r':
			if (digits)
				SetPage(w, num, False); /* go to page num */
			else
				SetPage(w, 1, True); /* go to next page */
			break;
		case 'g':
			if (digits)
				SetPage(w, num, False); /* go to page num */
			else
				SetPage(w, -INFINITY, True); /* go to first page */
			break;
		case 'G':
			if (digits)
				SetPage(w, num, False); /* go to page num */
			else
				SetPage(w, INFINITY, True); /* go to last page */
			break;
		case '$':
			num = INFINITY;
			digits++;
			return;
		case 'q': case 'Q':
		case 'x': case 'X':
			exit(0);
	}
	digits = num = 0;
}

/* ARGSUSED */
static void KeyEvent(gw,event,params,num_params)
Widget gw;
XKeyEvent *event;
String *params;        /* unused */
Cardinal *num_params;  /* unused */
{
	char trans[BUFSIZ];
	int len = XLookupString(event, trans, sizeof trans, (KeySym *)NULL,
					(XComposeStatus *)NULL);
	int i;

	for (i=0; i<len; i++) dochar(gw, trans[i]);
}

/* ARGSUSED */
static void StringEvent(gw,event,params,num_params)
Widget gw;
XEvent *event;		/* unused */
String *params;
Cardinal *num_params;
{
	char *s;

	if(*num_params == 0)
		return;
	for(s = params[0]; *s; s++)
		dochar(gw, *s);
}

static void Realize(gw, valuemaskp, attr)
Widget gw;
XtValueMask *valuemaskp;
XSetWindowAttributes *attr;
{
	ProofWidget w = (ProofWidget) gw;

	/* If there isn't an input file, there's no point in continuing */
	if (!w->proof.file) {
		(void)fprintf(stderr,"ProofWidget: no input file!\n");
		XtDestroyWidget((Widget)w);
	}

	/* Create a window */

	*valuemaskp |= CWBitGravity;
	attr->bit_gravity = ForgetGravity;

    if ((attr->cursor = w->proof.idle_cursor) != None)
		*valuemaskp |= CWCursor;

	XtCreateWindow(gw, InputOutput, (Visual *) CopyFromParent,
					*valuemaskp, attr);

	if (!UsePixmap)
		w->proof.canvas = XtWindow(gw);
	else {
		/* Create a "canvas" pixmap for holding the primary copy of a page */

		w->proof.canvas = XCreatePixmap(XtDisplay(w),
			DefaultRootWindow(XtDisplay(w)),
			w->core.width, w->core.height,
			DefaultDepth(XtDisplay(w), DefaultScreen(XtDisplay(w))));
	}

	/* Now that everything is in place, we can finish scanning the start
	 * of the file (up to the start of the first page).  The reason for
	 * all this complexity is that there is a bug in troff:  under certain
	 * circumtances, some output can be produced before "the first page".
	 */

	(void) ShowPage(w, w->proof.file);

	/* Request first page of file */
	XtProofShowPage(w, -1L);
}

static void Destroy(gw)
Widget gw;
{
	ProofWidget w = (ProofWidget) gw;

	XtReleaseGC((Widget)w,w->proof.DrawGc);
	XtReleaseGC((Widget)w,w->proof.FillGc);
	if (UsePixmap) {
		XtReleaseGC((Widget)w,w->proof.ClearGc);
		XtReleaseGC((Widget)w,w->proof.CopyGc);
		XFreePixmap(XtDisplay(w),w->proof.canvas);
	}
	return;
}

/* Redisplay is called on an expose event.  It may also be called explicitly
 * from elsewhere in this widget (in which case 'event' is nil).
 */
/* ARGSUSED */
static void Redisplay(gw, event, region)
Widget gw;
XEvent *event;
Region region;
{
	ProofWidget w = (ProofWidget)gw;
	XSetWindowAttributes attr;

	if (!UsePixmap) { /* try to redraw the window from scratch */
		if (w->proof.shown) {
			/* This is a re-display. Try to back up to the start of the page. */
			if (w->proof.offset<0
				|| fseek(w->proof.file, w->proof.offset, 0)<0)
			{
				/* can't back up, so can't redraw! */
				return;
			}
		}
		else if (w->proof.offset>=0)
			(void)fseek(w->proof.file, w->proof.offset, 0);

		if ((attr.cursor = w->proof.busy_cursor) != None)
			XChangeWindowAttributes(XtDisplay(w), XtWindow(w), CWCursor, &attr);

		if (ShowPage(w, w->proof.file)) {
			BEEP(w);
		}

		if ((attr.cursor = w->proof.idle_cursor) != None)
			XChangeWindowAttributes(XtDisplay(w), XtWindow(w), CWCursor, &attr);
		w->proof.shown = 1;
	}
	else { /* make sure the convas is up-to-date and copy it to the window */
		register XExposeEvent *e = (XExposeEvent *)event;

		if (!w->proof.shown) {
			/* The first the current page is requested, we interpret it to
			 * the canvas.  On subsequent exposures, we simply copy from
			 * canvas to screen.
			 */
			if (w->proof.offset>=0
				&& fseek(w->proof.file, w->proof.offset, 0)<0)
			{
				/* Something's very wrong! */
				perror("seek");
				return;
			}
			ClearCanvas(w);

			if ((attr.cursor = w->proof.busy_cursor) != None)
				XChangeWindowAttributes(XtDisplay(w), XtWindow(w),
					CWCursor, &attr);

			if (ShowPage(w, w->proof.file)) {
				BEEP(w);
			}

			if ((attr.cursor = w->proof.idle_cursor) != None)
				XChangeWindowAttributes(XtDisplay(w), XtWindow(w),
					CWCursor, &attr);

			w->proof.shown = 1;
		}
		XCopyArea(XtDisplay(gw), w->proof.canvas, XtWindow(gw),
			w->proof.CopyGc,
			e->x,e->y,                                 /* src offset */
			(Dimension)e->width, (Dimension)e->height, /* dimensions */
			e->x,e->y                                  /* dest offset */
		);
	}
}

/* ARGSUSED */
static Boolean SetValues(gcurrent, grequest, gnew)
Widget gcurrent, grequest, gnew;
{
	ProofWidget current = (ProofWidget) gcurrent;
	ProofWidget new = (ProofWidget) gnew;
	unsigned long fg, bg;  /* pixel values */

	if (current->proof.foreground_pixel != new->proof.foreground_pixel ||
			current->core.background_pixel != new->core.background_pixel ||
			current->proof.reverse_video != new->proof.reverse_video) {
		if (new->proof.reverse_video) {
			bg = new->proof.foreground_pixel;
			fg = new->core.background_pixel;
		}
		else {
			fg = new->proof.foreground_pixel;
			bg = new->core.background_pixel;
		}
		XSetForeground(XtDisplay(new), new->proof.DrawGc, fg);
		XSetForeground(XtDisplay(new), new->proof.FillGc, fg);
		if (UsePixmap)
			XSetForeground(XtDisplay(new), new->proof.ClearGc, bg);

		XSetBackground(XtDisplay(new), new->proof.DrawGc, bg);
		XSetBackground(XtDisplay(new), new->proof.FillGc, bg);
	}
	if (current->proof.scale != new->proof.scale) {
		Dimension wid = (PAPERWIDTH * screen_res * new->proof.scale)/10;
		Dimension hgt = (PAPERLENGTH * screen_res * new->proof.scale)/10;
		Dimension neww, newh;
		XtGeometryResult result;
		scaled_res = (new->proof.scale * screen_res) / 10;
		result = XtMakeResizeRequest((Widget)new,wid,hgt,&neww,&newh);
		if (result == XtGeometryNo) {
			/* debug */ printf("Can't change size!\n");
			new->proof.scale = current->proof.scale;
			return FALSE;
		}
		if (result == XtGeometryAlmost) {
			if (XtMakeResizeRequest((Widget)new,neww,newh,NULL,NULL) != XtGeometryYes) {
				/* debug */ printf("Parent renegged on size!\n");
				new->proof.scale = current->proof.scale;
				return FALSE;
			}
			/* If the geometry has changed, SetValues will do a redraw anyhow.
			 * The following forces a redraw even if the size hasn't changed.
			 */
			wid = neww; hgt = newh;
		}
		if (UsePixmap) {
			XFreePixmap(XtDisplay(new),new->proof.canvas);
			new->proof.canvas = XCreatePixmap(XtDisplay(new),
				DefaultRootWindow(XtDisplay(new)), wid, hgt,
				DefaultDepth(XtDisplay(new), DefaultScreen(XtDisplay(new))));
			new->proof.shown = 0;
		}
		UnloadXFonts(new);
	}
	return TRUE;
}

/*
 * drawing code
 */
static void ClearCanvas(w)
ProofWidget w;
{
	XFillRectangle(XtDisplay(w), w->proof.canvas, w->proof.ClearGc,
		0, 0, w->core.width, w->core.height);
}

static void ClearWindow(w)
ProofWidget w;
{
	XClearArea(XtDisplay(w), XtWindow(w), 0,0,0,0,1);
}

XTextItem item = {
	"x",	/* chars */
	1,		/* nchars */
	0,		/* delta */
	None,	/* font */
};

/* Draw a filled polygon using the indicated points.
 * 'Fill' is a fill color, interpreted as a gray scale from
 * WHITE (0) to BLACK (actually, WHITE means "background" and BLACK means
 * "foreground").
 * If 'border' is non-zero, draw the border as a path using the current
 * line style and thickness.
 */
void
drawXpoly(w, fill, border, points, npoints)
ProofWidget w;
int fill, border;
XPoint *points;
int npoints;
{
	int i,j;

	/* Draw the border */
	if (border) {
		/* Squish out zero-length vectors.  This routine is used by the
		 * spline routines, and they can produce many zero-length steps
		 * as a result of round-off.  Not only are these zero-length vectors
		 * a potential performance problem, they cause the current (X11R2)
		 * qdss server to crash!  We do the squishing here, rather than
		 * in the spline routines themselves, just in case the user
		 * directly specified a polygon with zero-length elements.
		 */
		i = 0;
		for (j=1;j<npoints;j++) {
			if (points[j].x==points[i].x && points[j].y==points[i].y)
				continue;
			i++;
			if (i!=j) points[i] = points[j];
		}
		npoints = i+1;
		XDrawLines(XtDisplay(w), w->proof.canvas, w->proof.DrawGc,
			points, npoints, CoordModeOrigin);
	}
	if (fill) {
		/* Update of the fill stipple is not yet working.  For now,
		 * use all ones (solid foreground) all the time.
		 */
		XFillPolygon(XtDisplay(w), w->proof.canvas, w->proof.FillGc,
			points, npoints, Complex, CoordModeOrigin);
	}
}

/* Display character 'c' in the current face and size, at the current
 * (hpos,vpos) coordinates.  This is complicated by the possibility that
 * 'c' may not be present in the current face, but some other (probably
 * the "special" font).  We also see to it here that the X font corresponding
 * to the current face and size has been opened.
 *
 * This procedure is called from ShowPage() (in showpage.c), but it is
 * included here since it is intimately tied to X.
 */
ShowChar(w, c)
ProofWidget w;
register int c;
{
	register int code, oldmount=curmount;

	if ((c -= 32)==0) {
		/* special hack for space: no need to do anything */
		return;
	}
	if ((code = curface->pos[c]) < 0) {
		/* Not available in the current typeface.  Run down the list
		 * of all mounted faces, looking for one that has this character.
		 * (We could look at all faces we've ever seen, not just those
		 * mounted, but that wouldn't be upward compatible!)
		 */
		for(;;) {
			curmount = curmount%maxmount + 1;
			if (curmount == oldmount) {
				return; /* not found anywhere! */
			}
			if ((code = mounted[curmount]->pos[c]) >=0) {
				curface = mounted[curmount];
				break;
			}
		}
	}
	/* Check to see that we have the appropriate X font (face+size) loaded */
	if (curface->xfont[cursize] == None) {
		/* Have to open an X font corresponding to the current face and size. */
		curface->xfont[cursize] =
			LoadXfont(w,
				curface->name,
				(int)(w->proof.scale * size[cursize] / 10));
		if (curface->xfont[cursize] == None) return;
	}
	item.chars[0] = code;
	item.font = curface->xfont[cursize];

	XDrawText(XtDisplay(w), w->proof.canvas, w->proof.DrawGc,
		SCALE(hpos), SCALE(vpos), &item, 1);
	if (curmount != oldmount) {
		curface = mounted[curmount=oldmount];
	}
}

/* drawX*() routines called from draw.c */

/* Draw a line from (x0,y0) to (x1,y1) in the current line style and thickness.
 * Add coordinates are in pixels.
 */
void
drawXline(w, x0, y0, x1, y1)
ProofWidget w;
int x0, y0, x1, y1;
{
	XDrawLine(XtDisplay(w), w->proof.canvas, w->proof.DrawGc, x0, y0, x1, y1);
}

void
drawXpoint(w, x0, y0)
ProofWidget w;
int x0, y0;
{
	XDrawPoint(XtDisplay(w), w->proof.canvas, w->proof.DrawGc, x0, y0);
}



/* Change the line thickness.  Other attributes are left at their
 * default values.
 */
void drawXthick(w, thickness)
ProofWidget w;
int thickness;
{
	w->proof.thickness = thickness;
	XSetLineAttributes(XtDisplay(w), w->proof.DrawGc,
		w->proof.thickness, w->proof.line_style, CapButt, JoinMiter);
}

/* Change the line dash style.  Ndashes==0 means use solid lines */
void drawXlinestyle(w, dashes, ndashes)
ProofWidget w;
char *dashes;
int ndashes;
{
	if (ndashes) {
		if (w->proof.line_style == LineSolid) {
			w->proof.line_style = LineOnOffDash;
			XSetLineAttributes(XtDisplay(w), w->proof.DrawGc,
				w->proof.thickness, w->proof.line_style, CapButt, JoinMiter);
		}
		XSetDashes(XtDisplay(w), w->proof.DrawGc,
			0,      /* dash_offset */
			dashes, /* dash_list */
			ndashes /* n */
		);
	}
	else {
		if (w->proof.line_style != LineSolid) {
			w->proof.line_style = LineSolid;
			XSetLineAttributes(XtDisplay(w), w->proof.DrawGc,
				w->proof.thickness, w->proof.line_style, CapButt, JoinMiter);
		}
	}
}

/* Draw part or all of a circular or eliptical arc.  The parameters have
 * been messaged into the format expected by XDrawArc:  (x,y) is the
 * upper-left corner and (width,heigth) are the dimentions of a bounding
 * rectagle.  Angle1 is the angle, counterclockwise from 3 o'clock, measured
 * in 64ths of a degree of the start of the arc, and angle2 is the size
 * (distance from start to end) of the the arc.
 */
void drawXarc(w, x, y, width, height, angle1, angle2)
ProofWidget w;
int x, y, width, height, angle1, angle2;
{
	XDrawArc(XtDisplay(w), w->proof.canvas, w->proof.DrawGc,
		x, y, (Dimension)width, (Dimension)height, angle1, angle2);
}
