/* $Header: ico.c,v 1.1 87/09/11 08:23:25 toddb Exp $ */
/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/************************************************************
Copyright 1988 by Sun Microsystems, Inc. Mountain View, CA.

		All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific prior written permission. Sun and M.I.T. 
make no representations about the suitability of this software for 
any purpose. It is provided "as is" without any express or implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. IN NO EVENT SHALL SUN BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.
************************************************************/

/******************************************************************************
 * Description
 *	Display a wire-frame rotating icosahedron, with hidden lines removed
 *
 * Arguments:
 *	=wxh+x+y		X geometry for window (default 600x600 centered)
 *	host:display (multiple)	X display(s) on which to run
 *****************************************************************************/

/*
 * hacks added for multiple window/display
 * Dave Lemke
 * lemke@sun.com
 * Thu Jun 23 19:31:17 PDT 1988
 */

/* NOTE this flag makes psycho do a lot of the clipping work
 * itself, instead of downloadiung all of it to the server.
 * it also causes a lot of extra net traffic to be removed.
 * However, it also makes things a bit jerky, since two windows
 * are gettiing packets when it is crossing a boundary.
 *
 * so if you want it fast, leave it defined.  if you want it
 * smooth, take it out.
 */
/* #define	OPTIMIZE */


#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <stdio.h>



typedef struct
	{
	double x, y, z;
	} Point3D;

typedef double Transform3D[4][4];



extern GC XCreateGC();
extern long time();
extern long random();

#define	min(a,b)	((a) > (b) ? (b) : (a))
#define	max(a,b)	((a) < (b) ? (b) : (a))

int winX, winY, winW, winH;
int	pwidth;		/* width of each partial window */

typedef struct _disp {
	Window	win;
	Display	*dpy;
	char	*dname;
	long	fg, bg;
	GC	gc;
	Atom	kill_atom, protocol_atom;
}	disp;

#define	MAX_DISPLAYS	16

disp	displays[MAX_DISPLAYS];
int	num_displays = 0;

/******************************************************************************
 * Description
 *	Main routine.  Process command-line arguments, then bounce a bounding
 *	box inside the window.  Call DrawIco() to redraw the icosahedron.
 *****************************************************************************/

main(argc, argv)
int argc;
char **argv;
{
	char *display = NULL;
	char *geom = NULL;
	int invert = 0;
	int dash = 0;
	register int	i;

	XSetWindowAttributes xswa;

	int icoX, icoY;
	int icoDeltaX, icoDeltaY;
	int icoW, icoH;
	char	wname[32];

	XEvent xev;
	XGCValues xgcv;

	XSizeHints	sizehint;


	/* Process arguments: */

	while (*++argv)
		{
		if (**argv == '=') 
			geom = *argv;
		else if (index(*argv, ':'))	{
			if (num_displays < MAX_DISPLAYS)
				displays[num_displays++].dname = *argv;
			}
		else if (!strcmp (*argv, "-d"))
			if (*++argv)
				dash = atoi(*argv);
			else
				dash = 0;
		else if (!strcmp(*argv, "-i"))
			invert = 1;
		else if (!strncmp(*argv, "-h", 2))	{
			 printf("usage: %s [-d dash-pattern] [=geometry] [display0 [display1...]]\n", argv[0]);
			exit(0);
			}
		}

	/* init in case we have no display args */
	if (num_displays == 0)	{
		displays[0].dname = NULL;
		num_displays++;
		}


	for (i = 0; i < num_displays; i++)	{
		if (!(displays[i].dpy = XOpenDisplay(displays[i].dname))) {
			perror("Cannot open displays %s\n", displays[i].dname);
			exit(-1);
			}

		if (invert) {
			displays[i].fg =BlackPixel(displays[i].dpy, 
				DefaultScreen(displays[i].dpy));
			displays[i].bg =WhitePixel(displays[i].dpy, 
				DefaultScreen(displays[i].dpy));
			}
		else {
			displays[i].fg =WhitePixel(displays[i].dpy, 
				DefaultScreen(displays[i].dpy));
			displays[i].bg =BlackPixel(displays[i].dpy, 
				DefaultScreen(displays[i].dpy));
			}

		/* Set up window parameters, create and map window if 
		 * necessary: */

		winW = 600;
		winH = 600;
		winX = (DisplayWidth(displays[i].dpy, 
			DefaultScreen(displays[i].dpy)) - winW) >> 1;
		winY = (DisplayHeight(displays[i].dpy, 
			DefaultScreen(displays[i].dpy)) - winH) >> 1;
		if (geom) 
			XParseGeometry(geom, &winX, &winY, &winW, &winH);

		pwidth = winW / num_displays;
		xswa.event_mask = 0;
		xswa.background_pixel = displays[i].bg;
		xswa.border_pixel = displays[i].fg;
		displays[i].win = XCreateWindow(displays[i].dpy, 
			DefaultRootWindow(displays[i].dpy), 
			winX + (i * (pwidth + 50)), winY, 
			pwidth, winH, 0, 
			DefaultDepth(displays[i].dpy, 
			DefaultScreen(displays[i].dpy)), 
			InputOutput, DefaultVisual(displays[i].dpy, 
			DefaultScreen(displays[i].dpy)),
			CWEventMask | CWBackPixel | CWBorderPixel, &xswa);

		sizehint.flags = PPosition | PSize;
		XSetNormalHints(displays[i].dpy, displays[i].win, &sizehint);

		displays[i].protocol_atom = XInternAtom(displays[i].dpy, "WM_PROTOCOLS", False);
		displays[i].kill_atom = XInternAtom(displays[i].dpy, "WM_DELETE_WINDOW", False);
		XSetWMProtocols(displays[i].dpy, displays[i].win, &displays[i].kill_atom, 1);

		if (num_displays > 1)
		    sprintf(wname, "psycho %1d", i);
		else
		    sprintf(wname, "Ico");
		XChangeProperty(displays[i].dpy, displays[i].win,
			XA_WM_NAME, XA_STRING, 8, PropModeReplace, wname, 
			strlen(wname));

		XMapWindow(displays[i].dpy, displays[i].win);

		/* Set up a graphics context: */

		displays[i].gc = XCreateGC(displays[i].dpy, displays[i].win, 0, NULL);
		XSetForeground(displays[i].dpy, displays[i].gc, displays[i].fg);
		XSetBackground(displays[i].dpy, displays[i].gc, displays[i].bg);

		if (dash) {
			xgcv.line_style = LineDoubleDash;
			xgcv.dashes = dash;
			XChangeGC(displays[i].dpy, displays[i].gc, 
				GCLineStyle | GCDashList, &xgcv);
			}
		}

	/* Get the initial position, size, and speed of the bounding-box: */

	icoW = icoH = 150;
	srandom((int) time(0) % 231);
	icoX = ((winW - icoW) * (random() & 0xFF)) >> 8;
	icoY = ((winH - icoH) * (random() & 0xFF)) >> 8;
	icoDeltaX = 13;
	icoDeltaY = 9;


	/* Bounce the box in the window: */

	for (;;)
		{
		int prevX;
		int prevY;


		prevX = icoX;
		prevY = icoY;

		icoX += icoDeltaX;
		if (icoX < 0 || icoX + icoW > winW)
			{
			icoX -= (icoDeltaX << 1);
			icoDeltaX = - icoDeltaX;
			}
		icoY += icoDeltaY;
		if (icoY < 0 || icoY + icoH > winH)
			{
			icoY -= (icoDeltaY << 1);
			icoDeltaY = - icoDeltaY;
			}

		for (i = 0; i < num_displays; i++)	{
			if (XPending(displays[i].dpy))
				XNextEvent(displays[i].dpy, &xev);
				if (xev.type == ClientMessage)	{
				XClientMessageEvent	*ev = (XClientMessageEvent *)&xev;
				    if (ev->message_type == displays[i].protocol_atom && ev->data.l[0] == displays[i].kill_atom)
					exit(0);


				}
			}
		drawIco(icoX, icoY, icoW, icoH, prevX, prevY);
		}
}



/******************************************************************************
 * Description
 *	Undraw previous icosahedron (by erasing its bounding box).
 *	Rotate and draw the new icosahedron.
 *
 * Input
 *	win		window on which to draw
 *	gc		X11 graphics context to be used for drawing
 *	icoX, icoY	position of upper left of bounding-box
 *	icoW, icoH	size of bounding-box
 *	prevX, prevY	position of previous bounding-box
 *****************************************************************************/

drawIco(icoX, icoY, icoW, icoH, prevX, prevY)
int icoX, icoY, icoW, icoH;
int prevX, prevY;
{
	static int initialized = 0;
	static Point3D v[] =	/* icosahedron vertices */
		{
		{ 0.00000000,  0.00000000, -0.95105650},
		{ 0.00000000,  0.85065080, -0.42532537},
		{ 0.80901698,  0.26286556, -0.42532537},
		{ 0.50000000, -0.68819095, -0.42532537},
		{-0.50000000, -0.68819095, -0.42532537},
		{-0.80901698,  0.26286556, -0.42532537},
		{ 0.50000000,  0.68819095,  0.42532537},
		{ 0.80901698, -0.26286556,  0.42532537},
		{ 0.00000000, -0.85065080,  0.42532537},
		{-0.80901698, -0.26286556,  0.42532537},
		{-0.50000000,  0.68819095,  0.42532537},
		{ 0.00000000,  0.00000000,  0.95105650}
		};
	static int f[] =	/* icosahedron faces (indices in v) */
		{
		 0,  2,  1,
		 0,  3,  2,
		 0,  4,  3,
		 0,  5,  4,
		 0,  1,  5,
		 1,  6, 10,
		 1,  2,  6,
		 2,  7,  6,
		 2,  3,  7,
		 3,  8,  7,
		 3,  4,  8,
		 4,  9,  8,
		 4,  5,  9,
		 5, 10,  9,
		 5,  1, 10,
		10,  6, 11,
		 6,  7, 11,
		 7,  8, 11,
		 8,  9, 11,
		 9, 10, 11
		};
#	define NV (sizeof(v) / sizeof(v[0]))
#	define NF (sizeof(f) / (3 * sizeof(f[0])))

	static Transform3D xform;
	static Point3D xv[2][NV];
	static int buffer;
	register int p0;
	register int p1;
	register int p2;
	register XPoint *pv2;
	XSegment *pe;
	char drawn[NV][NV];
	register Point3D *pxv;
	static double wo2, ho2;
	XPoint v2[NV];
	XSegment edges[30];
	register int i, j;
	register int *pf;


	/* Set up points, transforms, etc.:  */

	if (!initialized)	
		{
		Transform3D r1;
		Transform3D r2;

		FormatRotateMat('x', 5 * 3.1416 / 180.0, r1);
		FormatRotateMat('y', 5 * 3.1416 / 180.0, r2);
		ConcatMat(r1, r2, xform);

		bcopy((char *) v, (char *) xv[0], NV * sizeof(Point3D));
		buffer = 0;

		wo2 = icoW / 2.0;
		ho2 = icoH / 2.0;

		initialized = 1;
		}


	/* Switch double-buffer and rotate vertices: */

	buffer = !buffer;
	PartialNonHomTransform(NV, xform, xv[!buffer], xv[buffer]);


	/* Convert 3D coordinates to 2D window coordinates: */

	pxv = xv[buffer];
	pv2 = v2;
	for (i = NV - 1; i >= 0; --i)
		{
		pv2->x = (int) ((pxv->x + 1.0) * wo2) + icoX;
		pv2->y = (int) ((pxv->y + 1.0) * ho2) + icoY;
		++pxv;
		++pv2;
		}


	/* Accumulate edges to be drawn, eliminating duplicates for speed: */

	pxv = xv[buffer];
	pv2 = v2;
	pf = f;
	pe = edges;
	bzero(drawn, sizeof(drawn));
	for (i = NF - 1; i >= 0; --i)
		{
		p0 = *pf++;
		p1 = *pf++;
		p2 = *pf++;

		/* If facet faces away from viewer, don't consider it: */
		if (pxv[p0].z + pxv[p1].z + pxv[p2].z < 0.0)
			continue;

		if (!drawn[p0][p1])
			{
			drawn[p0][p1] = 1;
			drawn[p1][p0] = 1;
			pe->x1 = pv2[p0].x;
			pe->y1 = pv2[p0].y;
			pe->x2 = pv2[p1].x;
			pe->y2 = pv2[p1].y;
			++pe;
			}
		if (!drawn[p1][p2])
			{
			drawn[p1][p2] = 1;
			drawn[p2][p1] = 1;
			pe->x1 = pv2[p1].x;
			pe->y1 = pv2[p1].y;
			pe->x2 = pv2[p2].x;
			pe->y2 = pv2[p2].y;
			++pe;
			}
		if (!drawn[p2][p0])
			{
			drawn[p2][p0] = 1;
			drawn[p0][p2] = 1;
			pe->x1 = pv2[p2].x;
			pe->y1 = pv2[p2].y;
			pe->x2 = pv2[p0].x;
			pe->y2 = pv2[p0].y;
			++pe;
			}
		}


	/* Erase previous, draw current icosahedrons; sync for smoothness. */

	for (i = 0; i < num_displays; i++)	{
#ifdef OPTIMIZE
		int farx, nearx;

		farx = min(prevX, icoX) - (i * pwidth);
		nearx = max(prevX, icoX) - (i * pwidth) + icoW + 1;
		/* cheap clipping - but should be num_displays times
		 * as fast as letting the server do all the clipping.
		 * and cut way down on net traffic, which is the real win */
		if ((farx < (pwidth)) && (nearx > (0)))	{
		    XClearArea(displays[i].dpy, displays[i].win, 
			prevX - (i * pwidth), prevY, icoW + 1, 
			icoH + 1, 0);
		    XDrawSegments(displays[i].dpy, displays[i].win, 
			displays[i].gc, edges, pe - edges);
		    XSync(displays[i].dpy, 0);
		    }
#else
		XClearArea(displays[i].dpy, displays[i].win, 
			prevX - (i * pwidth), prevY, icoW + 1, 
			icoH + 1, 0);
		XDrawSegments(displays[i].dpy, displays[i].win, 
			displays[i].gc, edges, pe - edges);
		XSync(displays[i].dpy, 0);
#endif
		/* slide the edges over for the next window */
		if (i < (num_displays - 1)) /* no need to do it the last time */
		    for (j = 0; j < (pe - edges); j++)	{
			edges[j].x1 -= pwidth;
			edges[j].x2 -= pwidth;
			}
		}
}



/******************************************************************************
 * Description
 *	Concatenate two 4-by-4 transformation matrices.
 *
 * Input
 *	l		multiplicand (left operand)
 *	r		multiplier (right operand)
 *
 * Output
 *	*m		Result matrix
 *****************************************************************************/

ConcatMat(l, r, m)
register Transform3D l;
register Transform3D r;
register Transform3D m;
{
register int i;
register int j;
register int k;

for (i = 0; i < 4; ++i)
	for (j = 0; j < 4; ++j)
		m[i][j] = l[i][0] * r[0][j]
		    + l[i][1] * r[1][j]
		    + l[i][2] * r[2][j]
		    + l[i][3] * r[3][j];
}



/******************************************************************************
 * Description
 *	Format a matrix that will perform a rotation transformation
 *	about the specified axis.  The rotation angle is measured
 *	counterclockwise about the specified axis when looking
 *	at the origin from the positive axis.
 *
 * Input
 *	axis		Axis ('x', 'y', 'z') about which to perform rotation
 *	angle		Angle (in radians) of rotation
 *	A		Pointer to rotation matrix
 *
 * Output
 *	*m		Formatted rotation matrix
 *****************************************************************************/

FormatRotateMat(axis, angle, m)
char axis;
double angle;
register Transform3D m;
{
double s, c;
double sin(), cos();

IdentMat(m);

s = sin(angle);
c = cos(angle);

switch(axis)
	{
	case 'x':
		m[1][1] = m[2][2] = c;
		m[1][2] = s;
		m[2][1] = -s;
		break;
	case 'y':
		m[0][0] = m[2][2] = c;
		m[2][0] = s;
		m[0][2] = -s;
		break;
	case 'z':
		m[0][0] = m[1][1] = c;
		m[0][1] = s;
		m[1][0] = -s;
		break;
	}
}



/******************************************************************************
 * Description
 *	Format a 4x4 identity matrix.
 *
 * Output
 *	*m		Formatted identity matrix
 *****************************************************************************/

IdentMat(m)
register Transform3D m;
{
register int i;
register int j;

for (i = 3; i >= 0; --i)
	{
	for (j = 3; j >= 0; --j)
		m[i][j] = 0.0;
	m[i][i] = 1.0;
	}
}



/******************************************************************************
 * Description
 *	Perform a partial transform on non-homogeneous points.
 *	Given an array of non-homogeneous (3-coordinate) input points,
 *	this routine multiplies them by the 3-by-3 upper left submatrix
 *	of a standard 4-by-4 transform matrix.  The resulting non-homogeneous
 *	points are returned.
 *
 * Input
 *	n		number of points to transform
 *	m		4-by-4 transform matrix
 *	in		array of non-homogeneous input points
 *
 * Output
 *	*out		array of transformed non-homogeneous output points
 *****************************************************************************/

PartialNonHomTransform(n, m, in, out)
register int n;
register Transform3D m;
register Point3D *in;
register Point3D *out;
{
for (; n > 0; --n, ++in, ++out) {
	out->x = in->x * m[0][0] + in->y * m[1][0] + in->z * m[2][0];
	out->y = in->x * m[0][1] + in->y * m[1][1] + in->z * m[2][1];
	out->z = in->x * m[0][2] + in->y * m[1][2] + in->z * m[2][2];
	}
}
