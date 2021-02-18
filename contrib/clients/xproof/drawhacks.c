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
/* Create a "dithacks" font for displaying some of the funny characters
 * used by ditroff but not included in the Adobe fonts.  These are primarily
 * the characters used by tbl and eqn for drawing boxes, large braces, etc.
 * See "glyphs" below for a complete list.
 *
 * The output is a "bdf" font.
 *
 * Author: Marvin Solomon, University of Wisconsin--Madison, Feb 1989.
 *
 * The code here is extremely gross.  It is intended to get the
 * job done in (what appeared to me to be) the simplest and most straightforard
 * manner possible.
 */
#ifndef lint
static char *rcsid="$Header: /src/X11/uw/xproof/RCS/drawhacks.c,v 1.4 1991/10/04 22:05:44 tim Exp $";
#endif

#include <stdio.h>
#include "bdfheader.h"

double scale_factor; /* units * scale_factor = pixels */
double rint(), floor(), ceil(); /* math.h on some systems ieee.h on others */

#define scale(x) (scale_factor * (double)x)
#define unscale(x) (((double)x)/scale_factor)
#define R(x) ((int)rint(x))
#define F(x) ((int)floor(x))
#define C(x) ((int)ceil(x))

#define MAX 50
int Minx, Maxx, Miny, Maxy;
int debug;

/* This is the "canvas" for drawing.  In scaled coordinates, the pixel
 * centered at (x,y) is in arr[x+MAX][y+MAX].  X coordinates increase from
 * left to right, and y coordinates increase from bottom to top (note this
 * latter is the opposite of the X window convention).
 */
char arr[2*MAX][2*MAX];

/* For circle drawing, we darken a cell if any part of it overlaps the
 * circle to be drawn.  To make that test, we check that distance from the
 * near corner to the center is <= radius and the distance from the far
 * corner to the center is >= radius.  The definition of "near corner"
 * depends on the relative locations of the center of the cell and the
 * center of the circle.  For example, if the center of the cell is above
 * and to the right of the center, the "near" corner is the lower-left
 * corner of the cell: (x-0.5,y-0.5).  In general, the near corner of
 * unit cell centered at (x,y) is at (x+near[quad].x, y+near[quad].y).
 * The numbering of quadrants is as follows:
 *
 *   1 | 0
 *   --+--
 *   3 | 2
 */
struct { double x,y; }
	near[] = { {-0.5,-0.5}, {0.5,-0.5}, {-0.5,0.5}, {0.5,0.5} },
	far[]  = { {0.5,0.5}, {-0.5,0.5}, {0.5,-0.5}, {-0.5,-0.5} };

clear() {
	bzero((char*)arr, sizeof arr);
	Minx = Miny = MAX;
	Maxx = Maxy = -MAX;
}
			
set(x,y) {
	if (x < Minx) Minx = x;
	if (y < Miny) Miny = y;
	if (x > Maxx) Maxx = x;
	if (y > Maxy) Maxy = y;
	arr[x+MAX][y+MAX] = 1;
}

print() {
	int x,y;

	if (debug) {
		for (x = Minx; x<=Maxx; x++) printf("-");
		printf("+(%d,%d)\n",Maxx,Maxy);
		for (y=Maxy; y>=Miny; --y) {
			for (x = Minx; x<=Maxx; x++)
				printf("%s",arr[x+MAX][y+MAX] ? "@" : " ");
			printf("|\n");
		}
		for (x = Minx; x<=Maxx; x++) printf("-");
		printf("+\n(%d,%d)\n",Minx,Miny);
	}
	else {
		int width = Maxx - Minx + 1;  /* assume width <= bits/int ! */
		int chars = 2*((width+7)/8);
		int pad = (-width) & 7;
		int val,x,y;

		for (y=Maxy; y>=Miny; --y) {
			val = 0;
			for (x = Minx; x<=Maxx; x++) {
				val <<= 1;
				if (arr[x+MAX][y+MAX]) val++;
			}
			val <<= pad;
			printf("%0*X\n",chars,val);
		}
	}
}

/* line from (x,miny) to (x,maxy); dimensions are in units. */
vline(miny,maxy,x)
{
	int y;
	x = R(scale(x));
	maxy = R(scale(maxy));
	for (y = R(scale(miny)); y<=maxy; y++) set(x,y);
}

/* line from (minx,y) to (maxx,y); dimensions are in units. */
hline(minx,maxx,y)
{
	int x;
	y = R(scale(y));
	maxx = R(scale(maxx));
	for (x = R(scale(minx)); x<=maxx; x++) set(x,y);
}

/* distance from point (x0,y0) to point (x1,y1) squared.
 * All values in pixels (unrounded).
 */
double
dist(x0,y0,x1,y1)
double x0, y0, x1, y1;
{
	x0 -= x1;
	y0 -= y1;
	return x0*x0 + y0*y0;
}

/* The following code draws quarter-circle arcs.  It really should use
 * the same algorithm as circle(), which is cleaner, but was written
 * earlier and seems to work, so I'm not going to mess with it.
 * Arguments are a starting point at a quadrant (all dimensions in unscaled
 * units).  The encoding of "quad" is illustrated by the picture at
 * the start of each case.  In these pictures, 'o' denotes the starting point
 * and '+' denotes the center of the arc.
 */

arc(x0,y0,runits,quad)
{
	double r = scale(runits);
	double rsq = r*r;
	double xc, yc;
	int minx,maxx,miny,maxy,x,y;

	switch(quad) {
	case 0:
	/*
	 *    ***
	 *   *
	 *  *
	 *  o   +
	 */
		minx = F(scale(x0)); maxx = C(scale(x0)+r);
		miny = F(scale(y0)); maxy = C(scale(y0)+r);
		xc = scale(x0)+r; yc = scale(y0);
		for (x=minx; x<=maxx; x++) {
			for (y=miny; y<=maxy; y++) {
				if (dist((double)x+0.5,(double)y-0.5,xc,yc) <= rsq
					&& dist((double)x-0.5,(double)y+0.5,xc,yc) >= rsq)
						set(x,y);
			}
		}
		break;
	case 1:
	/*
	 *  ***
	 *     *
	 *  	*
	 *  +   o
	 */
		minx = F(scale(x0)-r); maxx = C(scale(x0));
		miny = F(scale(y0)); maxy = C(scale(y0)+r);
		xc = scale(x0)-r; yc = scale(y0);
		for (x=minx; x<=maxx; x++) {
			for (y=miny; y<=maxy; y++) {
				if (dist((double)x-0.5,(double)y-0.5,xc,yc) <= rsq
					&& dist((double)x+0.5,(double)y+0.5,xc,yc) >= rsq)
						set(x,y);
			}
		}
		break;
	case 2:
	/*
	 *  o   +
	 *  *
	 *   *
	 *    ***
	 */
		minx = F(scale(x0)); maxx = C(scale(x0)+r);
		miny = F(scale(y0)-r); maxy = C(scale(y0));
		xc = scale(x0)+r; yc = scale(y0);
		for (x=minx; x<=maxx; x++) {
			for (y=miny; y<=maxy; y++) {
				if (dist((double)x+0.5,(double)y+0.5,xc,yc) <= rsq
					&& dist((double)x-0.5,(double)y-0.5,xc,yc) >= rsq)
						set(x,y);
			}
		}
		break;
	case 3:
	/*
	 *  +  	o
	 *  	*
	 *     *
	 *  ***
	 */
		minx = F(scale(x0)-r); maxx = C(scale(x0));
		miny = F(scale(y0)-r); maxy = C(scale(y0));
		xc = scale(x0)-r; yc = scale(y0);
		for (x=minx; x<=maxx; x++) {
			for (y=miny; y<=maxy; y++) {
				if (dist((double)x-0.5,(double)y+0.5,xc,yc) <= rsq
					&& dist((double)x+0.5,(double)y-0.5,xc,yc) >= rsq)
						set(x,y);
			}
		}
		break;
	}
} /* arc */

/* circle with center (x,y) and radius ri */
circle(x,y,ri,fill)
{
	double r = scale(ri); /* scaled radius */
	double rsq = r*r;
	double xc, yc; /* scaled center point */
	int minx,maxx,miny,maxy;
	int quad;

	/* adjust center to fall on a grid point */
	xc = rint(scale(x));
	yc = rint(scale(y));

	minx = F(xc-r); maxx = C(xc+r);
	miny = F(yc-r); maxy = C(yc+r);

	for (x=minx; x<=maxx; x++) {
		for (y=miny; y<=maxy; y++) {
			if (x < xc) {
				if (y < yc) quad = 3; else quad = 1;
			}
			else {
				if (y < yc) quad = 2; else quad = 0;
			}
			/* I warned you it was gross! */
			if (dist(x+near[quad].x,y+near[quad].y,xc,yc) > rsq) continue;
			if (!fill
				&& dist(x+far[quad].x,y+far[quad].y,xc,yc) < rsq) continue;
			set(x,y);
		}
	}
}

/* square with ll corner at x0,y0 and ur corner at x1,y1 */
square(x0,y0,x1,y1,fill)
{
	int x,y;
	x0 = F(scale(x0)); y0 = F(scale(y0));
	x1 = C(scale(x1)); y1 = C(scale(y1));
	for (x=x0;x<=x1;x++)
		for (y=y0;y<=y1;y++)
			if (fill || x==x0 || y==y0 || x==x1 || y==y1) set(x,y);
}

doheader(size,res)
{
	char **p, *s;
	struct _font_sizes *q;

	for (q=font_sizes ; q->point_size; q++) {
		if ((size == q->point_size) && (res == q->resolution)) break;
	}
	for (p=header; *p; p++) {
		for (s= *p; *s; s++) {
			if (*s=='%') printf("%d",size);
			else if (*s=='@') printf("%d",res);
			else if (*s=='#') printf("%d",q->pixel_size);
			else if (*s=='$') printf("%d",q->average_width);
			else (void)putchar(*s);
		}
		putchar('\n');
	}
}

int
	rc(), lt(), bv(), lk(), lb(), rt(), rk(), rb(), rf(), lf(),
	lc(), sq(), bx(), ci(), br(), rn(), vr(), ob(), bu(), ru(), ul();
struct glyphinfo {
	int code;
	int (*proc)();
	char *name;
	char *comment;
} glyphs[] = {
	0110, rc, "rc", "right ceil",
	0111, lt, "lt", "left  top curl",
	0112, bv, "bv", "bold vert",
	0113, lk, "lk", "left  mid curl",
	0114, lb, "lb", "left  bot curl",
	0115, rt, "rt", "right top curl",
	0116, rk, "rk", "right mid curl",
	0117, rb, "rb", "right bot curl",
	0120, rf, "rf", "right floor",
	0121, lf, "lf", "left  floor",
	0122, lc, "lc", "left  ceil",
	0140, sq, "sq", "square",
	0141, bx, "bx", "box",
	0142, ci, "ci", "circle",
	0143, br, "br", "box rule",
	0144, rn, "rn", "root extender",
	0145, vr, "vr", "vertical rule",
	0146, ob, "ob", "outline bullet",
	0147, bu, "bu", "bullet",
	0150, ru, "ru", "rule",
	0151, ul, "ul", "underline",
	0,0,0,0
};

main(argc,argv)
char **argv;
{
	int pointsize;
	int resolution;
	struct glyphinfo *p;
	int arg;

	for (arg=1; arg<argc && argv[arg][0]=='-'; arg++) {
		switch(argv[arg][1]) {
			case 'd': debug++; break;
			default: goto usage;
		}
	}
	if (argc != arg+2) {
	usage:
		fprintf(stderr,"usage: %s pointsize resolution\n",argv[0]);
		exit(1);
	}
	pointsize = atoi(argv[arg]);
	resolution = atoi(argv[arg+1]);
	scale_factor = (double)pointsize;
	scale_factor = scale_factor*(double)resolution/72000.0;
	doheader(pointsize,resolution);
	for (p=glyphs; p->code; p++) {
		clear();
		(*(p->proc))();
		if (debug)
			printf("STARTCHAR %s %s\n",p->name, p->comment);
		else
			printf("STARTCHAR %s\n",p->name);
		printf("ENCODING %d\n",p->code);
		printf("SWIDTH 0 0\nDWIDTH 0 0\nBBX %d %d %d %d\nBITMAP\n",
			Maxx - Minx + 1, Maxy - Miny + 1, Minx, Miny);
		print();
		printf("ENDCHAR\n");
	}
	printf("ENDFONT\n");
	exit(0);
}

rc() {	
	/* /rc{0 -200 moveto 0 1000 rlineto s4 neg 0 rls}def */
	vline(-200,800,0);
	hline(-250,0,800);
}

lt() {	
/* /lt{0 -200 moveto 0 550 rlineto currx 800 2cx s4 add exch s4 a4p stroke}def */
	vline(-200,550,0);
	arc(0,550,250,0);
}

bv() {	
/* /bv{0 800 moveto 0 -1000 rls}def */
	vline(-200,800,0);
}

lk() {	
/* /lk{0 800 moveto 0 300 -300 300 s4 arcto pop pop 1000 sub */
/*     0 300 4 2 roll s4 a4p 0 -200 lineto stroke}def */
	vline(-200,50,0);
	vline(550,800,0);
	arc(0,50,250,1);
	arc(0,550,250,3);
}

lb() {	
/* /lb{0 800 moveto 0 -550 rlineto currx -200 2cx s4 add exch s4 a4p stroke}def */
	vline(50,800,0);
	arc(0,50,250,2);
}

rt() {	
/* /rt{0 -200 moveto 0 550 rlineto currx 800 2cx s4 sub exch s4 a4p stroke}def */
	vline(-200,800-250,0);
	arc(0,800-250,250,1);
}

rk() {	
/* /rk{0 800 moveto 0 300 s2 300 s4 arcto pop pop 1000 sub */
/*     0 300 4 2 roll s4 a4p 0 -200 lineto stroke}def */
	vline(-200,50,0);
	vline(550,800,0);
	arc(0,50,250,0);
	arc(0,550,250,2);
}

rb() {	
/* /rb{0 800 moveto 0 -500 rlineto currx -200 2cx s4 sub exch s4 a4p stroke}def */
	vline(50,800,0);
	arc(0,50,250,3);
}

rf() {	
/* /rf{0 800 moveto 0 -1000 rlineto s4 neg 0 rls}def */
	vline(-200,800,0);
	hline(-250,0,-200);
}

lf() {	
/* /lf{0 800 moveto 0 -1000 rlineto s4 0 rls}def */
	vline(-200,800,0);
	hline(0,250,-200);
}

lc() {	
/* /lc{0 -200 moveto 0 1000 rlineto s4 0 rls}def */
	vline(-200,800,0);
	hline(0,250,800);
}

sq() {	
/* /sq{80 0 rmoveto currentpoint dround newpath moveto */
/*     640 0 rlineto 0 640 rlineto -640 0 rlineto closepath stroke}def */
	square(80,0,80+640,640,0);
}

bx() {	
/* /bx{80 0 rmoveto currentpoint dround newpath moveto */
/*     640 0 rlineto 0 640 rlineto -640 0 rlineto closepath fill}def */
	square(80,0,80+640,640,1);
}

ci() {	
/* /ci{500 360 rmoveto currentpoint newpath 333 0 360 arc */
/*     50 setlinewidth stroke}def */
	circle(500,360,333,0);
}

br() {	
/* /br{0 850 moveto 0 -1000 rls}def */
	vline(-150,850,0);
}

rn() {	
/* /rn{0 840 moveto 500 0 rls}def */
	hline(0,500,840);
}

vr() {	
/* /vr{0 800 moveto 0 -770 rls}def */
	vline(30,770,0);
}

ob() {	
/* /ob{200 250 rmoveto currentpoint newpath 200 0 360 arc closepath stroke}def */
	circle(200,250,200,0);
}

bu() {	
/* /bu{200 250 rmoveto currentpoint newpath 200 0 360 arc closepath fill}def */
	circle(200,250,200,1);
}

ru() {	
/* /ru{500 0 rls}def */
	hline(0,500,0);
}

ul() {	
/* /ul{0 -140 moveto 500 0 rls}def */
	hline(0,500,-140);
}
