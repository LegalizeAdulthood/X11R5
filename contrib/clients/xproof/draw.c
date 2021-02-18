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
 */
#ifndef lint
static char rcsid[]="$Header: /src/X11/uw/xproof/RCS/draw.c,v 3.4 1991/10/04 22:05:44 tim Exp $";
#endif lint

/* Graphics functions for xproof.  Each 'D' command encountered by ShowPage
 * generates a call to the corresponding draw*() routine here.  Many of these
 * routines generate point lists and call drawX*() routines in Proof.c to do
 * the actual drawing.  Therefore, each one has a ProofWidget argument.
 * Position and size parameters to all the draw*() routines defined here are
 * specified in device units.  These values are always converted to pixels
 * before calling the drawX*() routines.
 */
#include "xproof.h"
#include <ctype.h>
#define MAXPOINTS 500

/* we really should include <math.h> here, but it contains all sorts of
 * stuff that gets lint upset, so we just declare what we need below
 */
#define  PI		3.14159265358979324
double atan2(), sqrt(), pow();

static int getint(/* f, result */);
static void drawpic(/* w, x, y, npts */);
static void drawgremlin(/* w, x, y, numpoints */);
static void Paramaterize(/* x, y, h, n */);
static void PeriodicSpline(/* h, z, dz, d2z, d3z, npoints */);
static void NaturalEndSpline(/* h, z, dz, d2z, d3z, npoints */);

/* Each of the following routines has the current point (hpos, vpos) as
 * an implicit prameter.  In comments below, "CurrentPoint" means the
 * value of (hpos,vpos) at the time of call, and "NewPoint" means the
 * value of (hpos,vpos) at the time of return.
 */

/* Draw a line (in the current style and thickness) from CurrentPoint to
 * to NewPoint = (hpos+deltax,vpos+deltay).
 */
void
drawline(w, deltax, deltay)
ProofWidget w;
int deltax, deltay;
{
	int newx = hpos+deltax;
	int newy = vpos+deltay;

	if (deltax || deltay)
		drawXline(w, SCALE(hpos), SCALE(vpos), SCALE(newx), SCALE(newy));
	else /* zero-length line */
		drawXpoint(w, SCALE(hpos), SCALE(vpos));
	hpos = newx;
	vpos = newy;
}

/* Draw a circle of diameter diam, with CurrentPoint at 9 o'clock and
 * NewPoint at 3 o'clock.
 */
void
drawcirc(w, diam)
ProofWidget w;
int diam;
{
	int radius = diam/2;

	drawXarc(w,
		SCALE(hpos), SCALE(vpos-radius), /* upper-left corner */
		SCALE(diam), SCALE(diam),        /* width and height */
		0, 360*64);                      /* start, angle */
	hpos += diam;
}

/* Draw an ellipse with horizontal and vertical axes ah and av,
 * with CurrentPoint at 9 o'clock and NewPoint at 3 o'clock.
 *
 * Warning:  This code is not tested.  Gremlin specifies "ellipses" as
 * 5-point splines, so it never generates any D'e...' commands.
 * I guess I'll have to use pic to generate a test.
 */
void
drawellipse(w, ah, av)
ProofWidget w;
int ah, av;
{
	int voffset = av/2;

	drawXarc(w,
		SCALE(hpos), SCALE(vpos-voffset), /* upper-left corner */
		SCALE(ah), SCALE(av),             /* width and height */
		0, 360*64);                       /* start, angle */
	hpos += ah;
}

/* Draw a circular arc centered about CenterPoint, starting at the line
 * between CenterPoint and CurrentPoint, and continuing counterclockwise
 * to NewPoint.
 * CenterPoint = CurrentPoint+(cdh,cdv) and NewPoint = CenterPoint+(pdh,pdv).
 */
void
drawarc(w, cdh, cdv, pdh, pdv)
ProofWidget w;
int cdh, cdv, pdh, pdv;
{
	int  radius = sqrt((double)(pdh*pdh+pdv*pdv))+0.5;
	double angle1 = atan2((double)cdv, -(double)cdh);
	double angle2 = atan2(-(double)pdv, (double)pdh)-angle1;
	int cx = hpos + cdh;
	int cy = vpos + cdv;

	if (angle2 < 0) angle2 += 2*PI;
	drawXarc(w,
		SCALE(cx-radius), SCALE(cy-radius),    /* upper-left corner */
		SCALE(2*radius), SCALE(2*radius),      /* width and height */
		(int)(angle1*(180.0*64/PI)),           /* starting angle */
		(int)(angle2*(180.0*64/PI))); /* deflection */
	hpos = cx+pdh;
	vpos = cy+pdv;
}

/* Draw a (filled) polygon.  
 * 'Fill' is a fill indication, whose semantics is (unfortunately) rather
 * vague.  In some versions of troff, it is a character in a (separately-
 * set) "stipple" font.  We take it to be a darkness level from zero (white)
 * to BLACK.  In any case, a 'fill' of 0 means no fill.
 * 'Border' is non-zero if the outline of the polygon is to be displayed
 * (in the current line thickness and style).
 * Otherwise, only the interior fill is drawn.
 *
 * The first point in the path is CurrentPoint.  Subsequent points
 * are defined by (deltax,deltay) pairs read from file f, separated by white
 * space and terminated by a newline.
 */
void
drawpolygon(w, f, fill, border)
ProofWidget w;
FILE *f;
int fill, border;
{
	int n;
	XPoint points[MAXPOINTS], *p;

	p = points;
	for (;;) {
		if (p >= points+MAXPOINTS) { /* too many points */
			while ((n=getc(f))!=EOF && n!='\n') /* skip */;
			break;
		}
		p->x = SCALE(hpos);
		p->y = SCALE(vpos);
		p++;
		if (getint(f,&n)) break;
		hpos += n;
		if (getint(f,&n)) { /* odd number of values? */
			break;
		}
		vpos += n;
	}
	drawXpoly(w, fill, border, points, p-points);
}

/* Draw a spline. 'Style' 0 for a 'gremlin' spline (Bezier spline?) and
 * 1 for a 'pic' spline (B spline?).
 * 
 * The points are specified as in drawpoly().
 */
void
drawcurve(w, f, style)
ProofWidget w;
FILE *f;
int style;
{
	int n, npoints;
	int x[MAXPOINTS+1], y[MAXPOINTS+1];

	npoints = 0;
	for (;;) {
		if (++npoints >= MAXPOINTS) { /* too many points */
			while ((n=getc(f))!=EOF && n!='\n') /* skip */;
			break;
		}
		x[npoints] = hpos;
		y[npoints] = vpos;
		if (getint(f,&n)) break;
		hpos += n;
		if (getint(f,&n)) { /* odd number of values? */
			break;
		}
		vpos += n;
	}
	if (style) drawpic(w, x, y, npoints);
	else drawgremlin(w, x, y, npoints);
}

/* Set the line thickness for subsequent drawing */
void
drawthick(w, s)
ProofWidget w;
int s;
{
	/* The thickness number is too big if interpreted as pixels, so
	 * we scale it down.  Since the current version of Gremlin uses
	 * 5 for thick, 3 for medium, and 1 for thin, the following function
	 * gives the thinest lines that still show some difference.
	 */
	s = (s + 1)/2;
	drawXthick(w, s);
}

/* Set the line style for subsequent drawing */
void
drawstyle(w, mask)
ProofWidget w;
int mask;
{
	/* Translate from troff's way of specifying a dashed line to 
	 * X's way.
	 *
	 * It would appear that in troff, bit i of the path is black if
	 * (mask & i != 0).
	 * (We take a bit of liberty with this and reflect the pattern,
	 * so that bit i is black if (mask & (patlen-i) != 0)).
	 *
	 * In X11, the style is more straightforwardly specified as a list
	 * of one-byte values specifying, alternately, the lengths of
	 * dashes and gaps.
	 *
	 * The following may look like magic, but it seems to work.  Here's
	 * the idea:
	 * A mask of length k corresponds to a bit string that repeats with
	 *     period 2**k.
	 * A mask of a 1 followed by k zeros corresponds to an X11 line style
	 *     of [2**k, 2**k].
	 * If mask w, of length k, corresponds to [x1, x2, ..., xi],
	 *     then the mask '1w' corresponds to [x1+2**k, x2, ... , xi]
	 *     (that is, the pattern of '1w' is the pattern of 'w' followed by
	 *     2**k dark bits, which merge with the initial dash).
	 * If mask w corrspondes to [whatever], then '0w' corrsponds to
	 *     [whatever] cat [whatever] (that is, two copies of the pattern,
	 *     laid end-to-end).
	 *
	 * The case of -1 (draw solid) is handled separately.  The case
	 * of 0 (don't draw anything) is (currently) flagged as an error.
	 *
	 */
	char buf[1024];
	int dashlen = 1, components, i;

	if (mask==0) {
		(void)fprintf(stderr,"internal error: null dash mask\n");
		return;
	}
	if (mask < 0) components = 0;
	else {
		/* To make things look clearer under the low-resolution of the display,
		 * we shift the mask right first, which has the effect of making
		 * all dashes half as long.
		 */
		mask >>= 1;
		while((mask&1)==0) {
			mask >>= 1;
			dashlen <<= 1;
		}
		buf[0] = buf[1] = dashlen;
		components = 2;
		for(;;) {
			mask >>= 1;
			dashlen <<= 1;
			if (mask==0) break;
			if (mask&1)
				buf[0] += dashlen;
			else {
				for (i=0;i<components;i++) buf[i+components] = buf[i];
				components <<= 1;
			}
		}
	}
	drawXlinestyle(w, buf, components);
}

/* Utility routines */

/* Read an integer from file f into *result, skipping white space.
 * Return nonzero if a newline or EOF is encountered first.
 */
static int
getint(f, result)
register FILE *f;
int *result;
{
	register int c, n = 0, sign=0;

	for (;;) {
		c = getc(f);
		if (c==EOF || c=='\n') return -1;
		if (c != ' ') break;
	}
	if (c=='-')
		sign = 1;
	for (;;) {
		if (isdigit(c)) /* ignore other characters! */
			n = 10*n + c-'0';
		c = getc(f);
		if (c==' ' || c=='\n' || c==EOF) {
			(void)ungetc(c,f);
			*result = sign ? -n : n;
			return 0;
		}
	}
}

/* Spline routines */

/* Draw a "pic style" curve about the points specified in (x,y).
 * NB:  the points are specified in positions 1..npts rather than 0..npts-1.
 */
static void
drawpic(win, x, y, npts)
ProofWidget win;
int x[MAXPOINTS];
int y[MAXPOINTS];
int npts;
{
	register int i;			/* line segment traverser */
	register float w;		/* position factor */
	register int xp;		/* current point (and intermediary) */
	register int yp;
	register int j;			/* inner curve segment traverser */
	register int nseg;		/* effective resolution for each curve */
	float t1, t2, t3;		/* calculation temps */
	XPoint v[MAXPOINTS], *vp;


	if (x[1] == x[npts] && y[1] == y[npts]) {
		x[0] = x[npts - 1];	/* if the lines' ends meet, make */
		y[0] = y[npts - 1];	/* sure the curve meets */
		x[npts + 1] = x[2];
		y[npts + 1] = y[2];
	} else {				/* otherwise, make the ends of the */
		x[0] = x[1];		/* curve touch the ending points of */
		y[0] = y[1];		/* the line segments */
		x[npts + 1] = x[npts];
		y[npts + 1] = y[npts];
	}

	vp = v;
	vp->x = SCALE((x[0]+x[1]+1)>>1);
	vp->y = SCALE((y[0]+y[1]+1)>>1);
	vp++;
	for (i = 0; i < npts; i++) {	/* traverse the line segments */
		xp = x[i] - x[i+1];
		yp = y[i] - y[i+1];
		nseg = (int) sqrt((double)(xp * xp + yp * yp));
		xp = x[i+1] - x[i+2];
		yp = y[i+1] - y[i+2];/* "nseg" is the number of line */
							/* segments that will be drawn for */
							/* each curve segment.  ">> 3" is */
							/* dropping the resolution ( == / 8) */
		nseg = (nseg + (int) sqrt((double)(xp * xp + yp * yp))) >> 3;

		for (j = 1; j <= nseg; j++) {
			w = (float) j / (float) nseg;
			t1 = 0.5 * w * w;
			w -= 0.5;
			t2 = 0.75 - w * w ;
			w -= 0.5;
			t3 = 0.5 * w * w;
			xp = t1 * x[i+2] + t2 * x[i+1] + t3 * x[i] + 0.5;
			yp = t1 * y[i+2] + t2 * y[i+1] + t3 * y[i] + 0.5;
			vp->x = SCALE(xp);
			vp->y = SCALE(yp);
			if (++vp - v == MAXPOINTS) {
				drawXpoly(win, 0, 1, v, MAXPOINTS);
				vp = v;
			}
		}
	}
	drawXpoly(win, 0, 1, v, vp-v);
}

/* Generate a smooth curve through a set of points.
 * The method used is the parametric spline curve on unit knot
 * mesh described in "Spline Curve Techniques" by Patrick
 * Baudelaire, Robert Flegal, and Robert Sproull -- Xerox Parc.
 */
#define PointsPerInterval 32

static void
drawgremlin(w, x, y, numpoints)
ProofWidget w;
int x[MAXPOINTS];
int y[MAXPOINTS];
int numpoints;
{
	float h[MAXPOINTS], dx[MAXPOINTS], dy[MAXPOINTS];
	float d2x[MAXPOINTS], d2y[MAXPOINTS], d3x[MAXPOINTS], d3y[MAXPOINTS];
	float t, t2, t3;
	register int j;
	register int k;
	register int nx;
	register int ny;
	XPoint v[MAXPOINTS], *vp;

	/* Solve for derivatives of the curve at each point 
	 * separately for x and y (parametric).
	 */
	Paramaterize(x, y, h, numpoints);
	if ((x[1] == x[numpoints]) && (y[1] == y[numpoints])) {
		/* closed curve */
		PeriodicSpline(h, x, dx, d2x, d3x, numpoints);
		PeriodicSpline(h, y, dy, d2y, d3y, numpoints);
	} else {
		NaturalEndSpline(h, x, dx, d2x, d3x, numpoints);
		NaturalEndSpline(h, y, dy, d2y, d3y, numpoints);
	}

	/* generate the curve using the above information and 
	 * PointsPerInterval vectors between each specified knot.
	 */

	vp = v;
	vp->x = SCALE(x[1]);
	vp->y = SCALE(y[1]);
	vp++;
	for (j=1; j<numpoints; ++j) {
		if ((x[j] == x[j+1]) && (y[j] == y[j+1])) continue;
		for (k=1; k<=PointsPerInterval; ++k) {
			t = (float) k * h[j] / (float) PointsPerInterval;
			t2 = t * t;
			t3 = t * t * t;
			nx = x[j] + (int) (t * dx[j] + t2 * d2x[j]/2 + t3 * d3x[j]/6);
			ny = y[j] + (int) (t * dy[j] + t2 * d2y[j]/2 + t3 * d3y[j]/6);
			vp->x = SCALE(nx);
			vp->y = SCALE(ny);
			if (++vp - v == MAXPOINTS) {
				drawXpoly(w, 0, 1, v, MAXPOINTS);
				vp = v;
			}
		}  /* end for k */
	}  /* end for j */
	drawXpoly(w, 0, 1, v, vp-v);
}

/* Calculate parameteric values for use in calculating curves.  The parametric
 * values are returned in the array h.  The values are an approximation of
 * cumulative arc lengths of the curve (uses cord length).
 * For additional information, see paper cited above.
 */
static void
Paramaterize(x, y, h, n)
int x[MAXPOINTS];
int y[MAXPOINTS];
float h[MAXPOINTS];
int n;
{
	register int dx;
	register int dy;
	register int i;
	register int j;
	float u[MAXPOINTS];


	for (i=1; i<=n; ++i) {
		u[i] = 0;
		for (j=1; j<i; j++) {
			dx = x[j+1] - x[j];
			dy = y[j+1] - y[j];
			u[i] += sqrt ((double) (dx * dx + dy * dy));
		}
	}
	for (i=1; i<n; ++i)  h[i] = u[i+1] - u[i];
}  /* end Paramaterize */

/* Solve for the cubic polynomial to fit a spline curve to the the points 
 * specified by the list of values.  The Curve generated is periodic.  The
 * algorithms for this curve are from the "Spline Curve Techniques" paper
 * cited above.
 */
static void
PeriodicSpline(h, z, dz, d2z, d3z, npoints)
float h[MAXPOINTS];                   /* paramaterization  */
int z[MAXPOINTS];                     /* point list */
float dz[MAXPOINTS];                  /* to return the 1st derivative */
float d2z[MAXPOINTS], d3z[MAXPOINTS]; /* 2nd and 3rd derivatives */
int npoints;                          /* number of valid points */
{
	float d[MAXPOINTS]; 
	float deltaz[MAXPOINTS], a[MAXPOINTS], b[MAXPOINTS];
	float c[MAXPOINTS], r[MAXPOINTS], s[MAXPOINTS];
	int i;

	/* step 1 */
	for (i=1; i<npoints; ++i) {
		deltaz[i] = h[i] ? ((double) (z[i+1] - z[i])) / h[i] : 0;
	}
	h[0] = h[npoints-1];
	deltaz[0] = deltaz[npoints-1];

	/* step 2 */
	for (i=1; i<npoints-1; ++i) {
		d[i] = deltaz[i+1] - deltaz[i];
	}
	d[0] = deltaz[1] - deltaz[0];

	/* step 3a */
	a[1] = 2 * (h[0] + h[1]);
	b[1] = d[0];
	c[1] = h[0];
	for (i=2; i<npoints-1; ++i) {
		a[i] = 2*(h[i-1]+h[i]) - pow ((double) h[i-1],(double)2.0) / a[i-1];
		b[i] = d[i-1] - h[i-1] * b[i-1]/a[i-1];
		c[i] = -h[i-1] * c[i-1]/a[i-1];
	}

	/* step 3b */
	r[npoints-1] = 1;
	s[npoints-1] = 0;
	for (i=npoints-2; i>0; --i) {
		r[i] = -(h[i] * r[i+1] + c[i])/a[i];
		s[i] = (6 * b[i] - h[i] * s[i+1])/a[i];
	}

	/* step 4 */
	d2z[npoints-1] = (6 * d[npoints-2] - h[0] * s[1] 
					   - h[npoints-1] * s[npoints-2]) 
					 / (h[0] * r[1] + h[npoints-1] * r[npoints-2] 
						+ 2 * (h[npoints-2] + h[0]));
	for (i=1; i<npoints-1; ++i) {
		d2z[i] = r[i] * d2z[npoints-1] + s[i];
	}
	d2z[npoints] = d2z[1];

	/* step 5 */
	for (i=1; i<npoints; ++i) {
		dz[i] = deltaz[i] - h[i] * (2 * d2z[i] + d2z[i+1])/6;
		d3z[i] = h[i] ? (d2z[i+1] - d2z[i])/h[i] : 0;
	}
}  /* end PeriodicSpline */

/* Solve for the cubic polynomial to fit a spline curve the the points 
 * specified by the list of values.  The alogrithms for this curve are from
 * the "Spline Curve Techniques" paper cited above.
 */
static void
NaturalEndSpline(h, z, dz, d2z, d3z, npoints)
float h[MAXPOINTS];                   /* parameterization */
int z[MAXPOINTS];                     /* Point list */
float dz[MAXPOINTS];                  /* to return the 1st derivative */
float d2z[MAXPOINTS], d3z[MAXPOINTS]; /* 2nd and 3rd derivatives */
int npoints;                          /* number of valid points */
{
	float d[MAXPOINTS];
	float deltaz[MAXPOINTS], a[MAXPOINTS], b[MAXPOINTS];
	int i;

	/* step 1 */
	for (i=1; i<npoints; ++i) {
		deltaz[i] = h[i] ? ((double) (z[i+1] - z[i])) / h[i] : 0;
	}
	deltaz[0] = deltaz[npoints-1];

	/* step 2 */
	for (i=1; i<npoints-1; ++i) {
		d[i] = deltaz[i+1] - deltaz[i];
	}
	d[0] = deltaz[1] - deltaz[0];

	/* step 3 */
	a[0] = 2 * (h[2] + h[1]);
	b[0] = d[1];
	for (i=1; i<npoints-2; ++i) {
		a[i] = 2*(h[i+1]+h[i+2]) - pow((double) h[i+1],(double) 2.0)/a[i-1];
		b[i] = d[i+1] - h[i+1] * b[i-1]/a[i-1];
	}

	/* step 4 */
	d2z[npoints] = d2z[1] = 0;
	for (i=npoints-1; i>1; --i) {
		d2z[i] = (6 * b[i-2] - h[i] *d2z[i+1])/a[i-2];
	}

	/* step 5 */
	for (i=1; i<npoints; ++i) {
		dz[i] = deltaz[i] - h[i] * (2 * d2z[i] + d2z[i+1])/6;
		d3z[i] = h[i] ? (d2z[i+1] - d2z[i])/h[i] : 0;
	}
}  /* end NaturalEndSpline */
