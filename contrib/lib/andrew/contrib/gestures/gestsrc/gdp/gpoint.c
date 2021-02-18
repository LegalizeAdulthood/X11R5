/***********************************************************************

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "util.h"
#include <gestures/bool.h>

#define	NLINES	50
#define	NPOINTS	500

/* extraneouse lines, not part of gesture */
static	int	nlines;
static	struct lines { int x1, y1, x2, y2; } line[NLINES];

/* endpoints of line segments which form gesture */
static	int	npoints;
static	struct point { int x, y; } point[NPOINTS];

Sreset() { Greset(); }
Serase() { Gerase(); }

Greset()
{
	nlines = 0;
	npoints = 0;
}

Gerase()	/* just redraws, assumes XOR */
{
	register int i;

	GDEVsets("currentlinetype", "solid");
	for(i = 0; i < nlines; i++)
		GDEVline(	  line[i].x1, line[i].y1,
				  line[i].x2, line[i].y2);
	for(i = 1; i < npoints; i++)
		GDEVline(	  point[i-1].x, point[i-1].y,
				  point[i].x, point[i].y);
	GDEVflush();
}

Gpoint(x, y)
int x, y;
{
	if(npoints >= NPOINTS)
		error("Gpoint");
	point[npoints].x = x;
	point[npoints].y = y;
	if(npoints > 0) {
		GDEVsets("currentlinetype", "solid");
		GDEVline( point[npoints-1].x, point[npoints-1].y, x, y);
	}
	npoints++;
}

Gline(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
	if(nlines >= NLINES)
		error("Gline");
	line[nlines].x1 = x1;
	line[nlines].y1 = y1;
	line[nlines].x2 = x2;
	line[nlines].y2 = y2;
	nlines++;
	GDEVsets("currentlinetype", "solid");
	GDEVline(x1, y1, x2, y2);
}
