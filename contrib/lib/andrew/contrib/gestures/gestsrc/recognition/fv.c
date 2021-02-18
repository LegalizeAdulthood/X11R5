
/***********************************************************************

fv.c - Creates a feature vector, useful for gesture classification,
	from a sequence of points (e.g. mouse points).

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

***********************************************************************/

#include <stdio.h>
#include <math.h>
#include "matrix.h"	/* contains Vector and associated functions */
#include "util.h"	/* for allocate() */
#include "fv.h"	
#include "zdebug.h"	

/* runtime settable parameters */
double dist_sq_threshold = DIST_SQ_THRESHOLD;
double se_th_rolloff = SE_TH_ROLLOFF;

#define	EPS	(1.0e-4)

double invcos2();

FV
FvAlloc()
{
	register FV fv;
	
	fv = allocate(1, struct fv);
	fv->y = NewVector(NFEATURES);
	FvInit(fv);
	return fv;
}
void
FvFree(fv)
FV fv;
{
	FreeVector(fv->y);
	free((char *) fv);
}

void
FvInit(fv)
register FV fv;
{
	register int i;

	fv->npoints = 0;
	fv->initial_sin = fv->initial_cos = 0.0;
	fv->maxv = 0;
	fv->path_r = 0;
	fv->path_th = 0;
	fv->abs_th = 0;
	fv->sharpness = 0;
	fv->maxv = 0;
	for(i = 0; i < NFEATURES; i++)
		fv->y[i] = 0.0;
}

void
FvAddPoint(fv, x, y, t)
register FV fv; int x, y; long t;
{
	double dx1, dy1, magsq1;
	double th, absth, d;
#ifdef PF_MAXV
	long lasttime;
#endif

	++fv->npoints;
	ZZ('F') printf("FVAddPoint[%d] (%d,%d,%d) ", fv->npoints, x,y,t);

	if(fv->npoints == 1) {
		fv->starttime = fv->endtime = t;
		fv->startx = fv->endx = fv->minx = fv->maxx = x;
		fv->starty = fv->endy = fv->miny = fv->maxy = y;
		fv->endx = x; fv->endy = y;
		Z('F') printf("\n");
		return;
	}

	dx1 = x - fv->endx; dy1 = y - fv->endy;
	magsq1 = dx1 * dx1 + dy1 * dy1;

	if(magsq1 <= dist_sq_threshold) {
		fv->npoints--;
		ZZ('F') printf("[ignored]\n");
		return;		/* ignore this point */
	}

	if(x < fv->minx) fv->minx = x;
	if(x > fv->maxx) fv->maxx = x;
	if(y < fv->miny) fv->miny = y;
	if(y > fv->maxy) fv->maxy = y;

#ifdef PF_MAXV
	lasttime = fv->endtime;
#endif
	fv->endtime = t;

	d = sqrt(magsq1);
	fv->path_r += d;

	/* calculate initial theta */
	if(fv->npoints == 3) {
		double magsq, dx, dy, recip;
		dx = x - fv->startx; dy = y - fv->starty;
		magsq = dx * dx + dy * dy;
		if(magsq > dist_sq_threshold)  {
			/* find angle w.r.t. positive x axis e.g. (1,0) */
			recip = 1 / sqrt(magsq);
			fv->initial_cos = dx * recip;
			fv->initial_sin = dy * recip;
		}
	}

	if(fv->npoints >= 3) {
		th = absth = atan2(dx1 * fv->dy2 - fv->dx2 * dy1,
				     dx1 * fv->dx2 + dy1 * fv->dy2);
		if(absth < 0) absth = -absth;
		fv->path_th += th;
		fv->abs_th += absth;
		fv->sharpness += th*th;

#ifdef PF_MAXV
		if(fv->endtime > lasttime &&
		   (v = d / (fv->endtime - lasttime)) > fv->maxv)
			fv->maxv = v;
#endif
	}

	fv->endx = x; fv->endy = y;
	fv->dx2 = dx1; fv->dy2 = dy1;
	fv->magsq2 = magsq1;

	ZZ('F') printf("\n");
	return;
}

Vector
FvCalc(fv)
register FV fv;
{
	double bblen, selen, factor;

	if(fv->npoints <= 1)
		return fv->y;		/* a feature vector of all zeros */

	fv->y[PF_INIT_COS] = fv->initial_cos;
	fv->y[PF_INIT_SIN] = fv->initial_sin;

	bblen = hypot(fv->maxx - fv->minx, fv->maxy - fv->miny);

	fv->y[PF_BB_LEN] = bblen;

	if(bblen * bblen > dist_sq_threshold) 
		fv->y[PF_BB_TH] = atan2(fv->maxy - fv->miny,
					fv->maxx - fv->minx);

	selen = hypot(fv->endx - fv->startx, fv->endy - fv->starty);
	fv->y[PF_SE_LEN] = selen;

	factor = selen * selen / se_th_rolloff;
	if(factor > 1.0) factor = 1.0;
	factor = selen > EPS ? factor/selen : 0.0;
	fv->y[PF_SE_COS] = (fv->endx - fv->startx) * factor;
	fv->y[PF_SE_SIN] = (fv->endy - fv->starty) * factor;

	fv->y[PF_LEN] =  fv->path_r;
	fv->y[PF_TH] = fv->path_th;
	fv->y[PF_ATH] = fv->abs_th;
	fv->y[PF_SQTH] = fv->sharpness;

#ifdef PF_DUR
	fv->y[PF_DUR] = (fv->endtime - fv->starttime)*.01;
#endif

#ifdef PF_MAXV
	fv->y[PF_MAXV] = fv->maxv * 10000;
#endif

	Z('F') PrintVector(fv->y, "FvCalc: ");

	return fv->y;
}

/*
  given m = cos^2(t) * sign(cos(t)) return t
  m must be between -1 and 1
  t will be between 0 and pi 
*/

double
invcos2(m)
double m;
{
	double x = m < 0 ? -sqrt(-m) : sqrt(m);
	double y = sqrt( 1.0 - x * x);	/* y = sin (acos(x)) */
	return atan2(y, x);
}

