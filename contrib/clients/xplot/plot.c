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

extern char *data;
extern int data_len;
extern int data_head;
extern int data_tail;

/* Need the following X values in this file: */
extern Display *dpy;
extern Pixmap pixmap;
extern GC gc;
extern GC fillgc;		/* Need a separate gc for filling... */
extern XGCValues gcv;
extern XWindowAttributes wa;
extern XFontStruct *font;

int tilew = 4; /* These could easily be made bigger... */
int tileh = 4;
int inittile = 0;
Pixmap filltile;
int filling = 0;

extern int rwidth, rheight;
extern int fgcolor, bgcolor;
extern int initfg, initbg;
extern XFontStruct *initfont;

extern int high_byte_first;

short xul, yul, xlr, ylr;
short xmin, ymax;
double xfactor, yfactor;

#define MAPX(x) (int) (((x) - xmin) * xfactor)
#define MAPY(y) (int) ((ymax - (y)) * yfactor)

int spaceset = 0;
short curpt[] = {0,0};

/* Points for doing paths, rather than just line segments */
/* Also, nicely allows us to do filled polygons... */
#define MAXPATH 1024
XPoint curpath[MAXPATH];
int pathind = 0;

				/* swap the lower two bytes */
#define byte_swap(x) ((((x)&0xff)<<8) | (((x)<<8)&0xff00))

space(x0,y0,x1,y1)
short x0,y0,x1,y1;
{
	int xmax, ymin;

	spaceset = 1;
	pathind = 0;
	xmin = xul = x0;
	ymin = yul = y0;
	xmax = xlr = x1;
	ymax = ylr = y1;
/* Kludge to guess byte order if not explicitly given */
	 /* determine the byte order - default is high byte last */
	if ((high_byte_first == 0)
		&& ((xmin > xmax) || (ymin > ymax) ||
		   (xmax < byte_swap (xmax)) || (ymax < byte_swap (ymax)) )) {
			xmin = xul = byte_swap (xmin);
			xmax = xlr = byte_swap (xmax);
			ymin = yul = byte_swap (ymin);
			ymax = ylr = byte_swap (ymax);
			high_byte_first = 1;
	}
	
	xfactor = ((double) rwidth)/(xlr + 1 - xul);
	yfactor = ((double) rheight)/(ylr + 1 - yul);
#ifdef KeepScale
    /* Use the smallest of the two scale factors, and adjust as appropriate */
	if (xfactor < yfactor) {
		yfactor = xfactor;
		ymax = rheight/(2*xfactor) + (yul + ylr)/2;
	} else if (yfactor < xfactor){
		xfactor = yfactor;
		xmin = (xul + xlr)/2 - rwidth/(2*yfactor);
	}
#endif
}

drawellipse(x,y,r1,r2)
int x,y,r1,r2;
{
	if (filling) {
		XFillArc(dpy, pixmap, fillgc, x - r1, y - r2, 2*r1, 2*r2,
				0, 64*180);
		XFillArc(dpy, pixmap, fillgc, x - r1, y - r2, 2*r1, 2*r2,
				64*180, 64*180);
	}
	XDrawArc(dpy, pixmap, gc, x - r1, y - r2, 2*r1, 2*r2,
			0, 64*180);
	XDrawArc(dpy, pixmap, gc, x - r1, y - r2, 2*r1, 2*r2,
			64*180, 64*180);
}

drawarc(x,y,x0,y0,x1,y1)
int x,y,x0,y0,x1,y1;
{
	int a0,b0,a1,b1;
	int a02,b02,a12,b12;
	double ar,br,ar2,br2;
	double theta0,theta1;

	a0 = x0 - x;
	a02 = a0*a0;
	a1 = x1 - x;
	a12 = a1*a1;
	b0 = y0 - y;
	b02 = b0*b0;
	b1 = y1 - y;
	b12 = b1*b1;
	if (b12 == b02) return 0;
	ar2 = (a02*b12 - a12*b02)/(b12 - b02);
	if (ar2 < 0) return 0;
	ar = sqrt(ar2);
	br2 = (b02*a12 - b12*a02)/(a12 - a02);
	if (br2 < 0) return 0;
	br = sqrt(br2);
	theta0 = -atan2(b0/br,a0/ar);
	theta1 = -atan2(b1/br,a1/ar);
	if (theta0 > theta1) theta1 += 2 * M_PI;
	if (theta0 < 0){
		theta0 += 2 * M_PI;
		theta1 += 2 * M_PI;
	}
	XDrawArc(dpy,pixmap,gc,(int) (x - ar), (int) (y - br),
		  (int)(2 * ar), (int) (2 * br), (int) (64* theta0
		  * 180/M_PI), (int) (64 * (theta1 - theta0) * 180/M_PI));
}

erase()
{
	XSetForeground(dpy,gc,bgcolor);
	XFillRectangle(dpy,pixmap,gc,0,0,rwidth,rheight);
	XSetForeground(dpy,gc,fgcolor);

#ifdef Resize_Yes
	if (data_head > 0)
	    if (data[data_head - 1] == 'e') { /* Cause of the erase command */
/* Then don't need earlier data, so overwrite it! */
		if (data_tail > data_head)
			bcopy(data + data_head, data, data_tail - data_head);
		data_tail -= data_head;
		data_head = 0;
	    }
	initfg = fgcolor; /* Need these for proper resizing */
	initbg = bgcolor;
	initfont = font;
#endif /* Resize_Yes */

}

label(s)
char *s;
{
	XDrawString(dpy,pixmap,gc,MAPX(curpt[0]),MAPY(curpt[1]),s,
			strlen(s));
#ifdef WideLabel
	curpt[0] += (strlen(s) * font->max_bounds.width)/xfactor;
#endif
}

line(x1,y1,x2,y2)
short x1,y1,x2,y2;
{
	curpt[0] = x2;
	curpt[1] = y2;
	if (spaceset == 0) return;
	XDrawLine(dpy,pixmap,gc, MAPX(x1), MAPY(y1), MAPX(x2), MAPY(y2));
}

circle(x,y,r)
short x,y,r;
{
	if (spaceset == 0) return;
	drawellipse(MAPX(x), MAPY(y), (int) (r * xfactor), (int) (r*yfactor));
}

arc(x,y,x0,y0,x1,y1)
short x,y,x0,y0,x1,y1;
{
	if (spaceset == 0) return;
	drawarc(MAPX(x), MAPY(y), MAPX(x0), MAPY(y0), MAPX(x1), MAPY(y1));
}

move(x,y)
short x,y;
{
	curpt[0] = x;
	curpt[1] = y;
}

point(x,y)
short x,y;
{
	curpt[0] = x;
	curpt[1] = y;
	if (spaceset == 0) return;
	XDrawPoint(dpy,pixmap,gc, MAPX(x), MAPY(y));
}

/* Now to control line styles */

char *linemode_labels[] ={
  "solid",
  "dotted",
  "shortdashed",
  "dotdashed",
  "longdashed",
  "disconnected",
};
int no_of_linemodes = sizeof(linemode_labels) / sizeof(linemode_labels[0]);

char dashes[][5] = {
	{0},			/* solid */
	{1,1,0},		/* dotted */
	{2,2,0},		/* shortdashed */
	{4,2,1,2,0},		/* dotdashed */
	{4,4,0},		/* longdashed */
	{1,8,0}			/* disconnected */
};

linemod(s)
char *s;
{
	int i=0;
/* search through the linemode names for a match.  */
	while ((i < no_of_linemodes) && (strcmp(s, linemode_labels[i]) != 0))
							 i++;
	if (i == no_of_linemodes) {
		special_linemod(s); /* Check to see if it's extended mode */
	} else {
		if (dashes[i][0])
			XSetDashes(dpy, gc, 0, dashes[i], strlen (dashes[i]));
		XSetLineAttributes (dpy, gc, 0 /* width */,
                      (i == 0) ? LineSolid : LineOnOffDash,
                      CapButt, JoinBevel);
	}
}

/* Not one of the standard line modes. Try something different */

special_linemod(s)
char *s;
{
	if (strlen(s) < 3) return 0;

	/* Try the following: colors first */
	if (color_by_name(s)) return 1;
	if (color_by_name(s + 1)) return 1;
	if (color_by_name(s + 2)) return 1;

	/* Then try font change */
	if (font_by_name(s)) return 2;
	if (font_by_name(s + 1)) return 2;
	if (font_by_name(s + 2)) return 2;
	return 0;
}

/* The following fixes a minor bug (incorrect dashed lines) and
   allows a major enhancement (filled polygons) and so I believe
   is definitely worth it! */

cont(x,y)
short x,y;
{
	if (pathind == 0){
		curpath[pathind].x = MAPX(curpt[0]);
		curpath[pathind++].y = MAPY(curpt[1]);
	}
	curpath[pathind].x = MAPX(curpt[0] = x);
	curpath[pathind++].y = MAPY(curpt[1] = y);

	if (pathind >= MAXPATH) /* Can't hold too many points at once */
		plotpath();
}

plotpath() /* Plot the current path */
{
	if (filling)
		if ((curpath[pathind - 1].x == curpath[0].x)&&
		    (curpath[pathind - 1].y == curpath[0].y))
			XFillPolygon(dpy, pixmap, fillgc, curpath, pathind,
				Complex, CoordModeOrigin); 
	XDrawLines(dpy, pixmap, gc, curpath, pathind, CoordModeOrigin);
	pathind = 0;
}

/* FILL sets the intensity of the filler for closed paths.  LEVEL ranges
   from 1 to 0xFFFF. A value of 1 represents black and a value of 0xFFFF
   indicates white. A value of 0 represents no fill - transparent. */

fill(level)
unsigned short level;
{
	double fracfill;
	int wd, ht,i, j, ind, n_ind;

	if (level == 0) {
		filling = 0;
		return 0;
	}
	filling = 1;
	fracfill = ((double) level - 1)/0xfffe;
	if (fracfill > 1) {
		fracfill = 1;
		XSetFillStyle(dpy, fillgc, FillSolid);
		return 1;
	}
	if (fracfill < 0)
		fracfill = 0;

	if (inittile == 0) {
/* Create the tile for filling: */
		XQueryBestStipple(dpy, pixmap,
			(unsigned int) tilew,
			(unsigned int) tileh,
			(unsigned int *) &wd,
			(unsigned int *) &ht);
		tilew = wd;
		tileh = ht;
		filltile = XCreatePixmap(dpy,DefaultRootWindow(dpy),
			 	wd, ht, DefaultDepth(dpy, DefaultScreen(dpy)));
		inittile = 1;
	}
	XSetForeground(dpy,fillgc,bgcolor);
	XFillRectangle(dpy,filltile,fillgc,0,0,tilew,tileh);
	XSetForeground(dpy,fillgc,fgcolor);

/* The following is completely ad-hoc. Feel free to replace by
   a better method. */
	ind = 0;
	for (i=0; i < tilew; i++) {
		for (j=0; j < tileh; j++, ind++) {
			n_ind = (int) floor(ind*fracfill);
			if (ind*fracfill - n_ind < fracfill)
				XDrawPoint(dpy, filltile, fillgc, i, j);
		}
	}
	XSetTile(dpy, fillgc, filltile);
	XSetFillStyle(dpy, fillgc, FillTiled);
	return 1;
}

/* Note the range of red, green, blue is 0 to 65535. This
   agrees with what is expected in the plot2ps version of the program */

color_by_number(red, green, blue)
short red, green, blue;
{
	XColor col;
	col.red = red;
	col.green = green;
	col.blue = blue;
	col.flags = DoRed | DoGreen | DoBlue;
	if (XAllocColor (dpy, wa.colormap, &col)) {
		fgcolor = col.pixel;
		XSetForeground(dpy, gc, fgcolor);
	} else {
		fprintf(stderr, "Warning: Cannot allocate color %d %d %d\
			(RGB).\n", red, green, blue);
	}
}

color_by_name(colname)
char *colname;
{
	if (nameToPixel(colname, &fgcolor)) {
		XSetForeground(dpy, gc, fgcolor);
		return 1;
	}
	return 0;
}
