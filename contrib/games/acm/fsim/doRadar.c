/*
 *	acm : an aerial combat simulator for X
 *
 *	Written by Riley Rainey,  riley@mips.com
 *
 *	Permission to use, copy, modify and distribute (without charge) this
 *	software, documentation, images, etc. is granted, provided that this 
 *	comment and the author's name is retained.
 *
 */
 
#include <math.h>
#include "pm.h"
#include <stdio.h>

#define scanSlope 	2.1445

void plotPrimaryTarget(), plotNormalTarget();

static char *thirty = "30";

/*
 *  We'll build an array that contains the amount of radar radiation that each
 *  craft can "see" coming from another player.  This is used to build each players
 *  radar threat display.
 *
 *  rval[i][j] will represent the amount of radar energy that player j sees coming from
 *  player i.
 *
 *  For neatness, the rval array has been moved to the ptbl vector.
 */

char *ItoA (n, s)
int	n;
char	*s; {

	if (abs(n) > 999) {
		sprintf (s, "%d,%3.3d", n/1000, abs(n) % 1000);
	}
	else 
		sprintf (s, "%d", n);

	return s;
}

int doRadar (c, u)
craft *c;
viewer *u; {

	register craft *p;
	register int	i, t, x, y;
	register int	xc, yc;
	register double	xs, ys;
	char		*buf;

	t = c->curRadarTarget = getRadarTarget (c, 0);
	xc = (u->radarWidth + 1) / 2;
	yc = (u->radarHeight + 1) / 2;

/*
 *  Set drawing colors and font
 */

	XSetForeground (u->dpy, u->gc, u->v->flags & VPMono ?
			BlackPixel(u->v->dpy, u->v->screen) :
			u->v->pixel[blackPixel]);
	XFillRectangle (u->dpy, u->rwin, u->gc, 0, 0,
		 u->radarWidth, u->radarHeight);
	XSetForeground (u->dpy, u->gc, u->v->flags & VPMono ?
			WhitePixel(u->v->dpy, u->v->screen) :
			u->v->pixel[whitePixel]);
	XSetFont (u->dpy, u->gc, u->rfont->fid);

/*
 *  Draw reference "thing"
 */

	XDrawLine (u->dpy, u->rwin, u->gc, xc, yc, xc - 5, yc + 5);
	XDrawLine (u->dpy, u->rwin, u->gc, xc - 10, yc, xc - 5, yc + 5);
	XDrawLine (u->dpy, u->rwin, u->gc, xc - 10, yc, xc - 18, yc);
	XDrawLine (u->dpy, u->rwin, u->gc, xc, yc, xc + 5, yc + 5);
	XDrawLine (u->dpy, u->rwin, u->gc, xc + 10, yc, xc + 5, yc + 5);
	XDrawLine (u->dpy, u->rwin, u->gc, xc + 10, yc, xc + 18, yc);
	XDrawLine (u->dpy, u->rwin, u->gc, 0, 0, u->radarWidth-1, 0);
	XDrawLine (u->dpy, u->rwin, u->gc, u->radarWidth-1, 0,
		u->radarWidth-1, u->radarHeight-1);
	XDrawLine (u->dpy, u->rwin, u->gc, 0, u->radarHeight-1,
		u->radarWidth-1, u->radarHeight-1);
	XDrawLine (u->dpy, u->rwin, u->gc, 0, u->radarHeight-1, 0, 0);

/*
 *  Radar dead?
 */

	if (isFunctioning (c, SYS_RADAR) == 0)
		return 0;

/*
 *  Radar off?
 */

	if (c->radarMode <= RM_STANDBY) {
		if (c->radarMode == RM_OFF)
			buf = "RADAR OFF";
		else
			buf = "RADAR STBY";
		c->curRadarTarget = -1;
		XDrawString (u->dpy, u->rwin, u->gc, u->rftw * 4,
			u->fth * 2, buf, strlen(buf));
		return 0;
	}

/*
 *  Plot reference lines
 */

	i = (u->radarWidth + 3) / 4;
	y = u->rfth + 3;
	XDrawLine (u->dpy, u->rwin, u->gc, xc, y, xc, y+4);
	y = u->radarHeight - 5 - u->rfth;
	XDrawLine (u->dpy, u->rwin, u->gc, xc, y, xc, y+4);

	x = xc - i;
	y = u->rfth + 6;
	XDrawLine (u->dpy, u->rwin, u->gc, x, y, x, y+4);
	y = u->radarHeight - 6 - u->rfth;
	XDrawLine (u->dpy, u->rwin, u->gc, x, y, x, y+4);
	XDrawString (u->dpy, u->rwin, u->gc, x-u->rftw, y+2+u->rfth,
		thirty, 2);

	x = xc + i;
	y = u->rfth + 6;
	XDrawLine (u->dpy, u->rwin, u->gc, x, y, x, y+4);
	y = u->radarHeight - 6 - u->rfth;
	XDrawLine (u->dpy, u->rwin, u->gc, x, y, x, y+4);
	XDrawString (u->dpy, u->rwin, u->gc, x-u->rftw, y+2+u->rfth,
		thirty, 2);

	i = (u->radarHeight + 3) / 4;
	x = 2;
	XDrawLine (u->dpy, u->rwin, u->gc, x, yc, x+4, yc);
	x = u->radarWidth - 6;
	XDrawLine (u->dpy, u->rwin, u->gc, x, yc, x+4, yc);

	x = 5 + 2 * u->rftw;
	y = yc - i;
	XDrawLine (u->dpy, u->rwin, u->gc, x, y, x+4, y);
	x = 3;
	XDrawString (u->dpy, u->rwin, u->gc, x, y+u->rfth/3,
		thirty, 2);
	x = u->radarWidth - 6;
	XDrawLine (u->dpy, u->rwin, u->gc, x, y, x+4, y);

	x = 5 + 2 * u->rftw;
	y = yc + i;
	XDrawLine (u->dpy, u->rwin, u->gc, x, y, x+4, y);
	x = 3;
	XDrawString (u->dpy, u->rwin, u->gc, x, y+u->rfth/3,
		thirty, 2);
	x = u->radarWidth - 6;
	XDrawLine (u->dpy, u->rwin, u->gc, x, y, x+4, y);
	
/*
 *  Now plot all targets visible to the radar system.
 */

	for ((i=0, p=ptbl); i<MAXPLAYERS; (++i, ++p)) {
		if (c->relValid[i] > 0 && c->relPos[i].x >= 0.0) {

			xs = c->relPos[i].y / c->relPos[i].x / scanSlope;
			ys = c->relPos[i].z / c->relPos[i].x / scanSlope;
			if (fabs(xs) <= 1.0 && fabs(ys) <= 1.0) {
				x = xs * (double) xc + xc;
				y = ys * (double) yc + yc;
				if (t == i)
					plotPrimaryTarget (c, u, i, x, y);
				else
					plotNormalTarget (u, x, y);
			}
		}
	}
	return 0;
}

int isRadarVisible (c, i)
craft *c;
int	i; {

	register double d, xs, ys;
	VPoint	r1;

	if (ptbl[i].type == CT_FREE || i == c->index)
		return 0;

/*
 *  Is the set on ?
 */

	if (c->radarMode <= RM_STANDBY ||
		isFunctioning (c, SYS_RADAR) == 0) {
		ptbl[i].rval[c->index] = 0.0;
		return 0;
	}

/*
 *  Calculate the coordinates of the target relative to the craft's frame.
 */

	r1.x = ptbl[i].Sg.x - c->Sg.x;
	r1.y = ptbl[i].Sg.y - c->Sg.y;
	r1.z = ptbl[i].Sg.z - c->Sg.z;
	VTransform (&r1, &(c->Itrihedral), &(c->relPos[i]));

	ptbl[i].rval[c->index] = 0.0;

	if (c->relPos[i].x <= 0.0)
		return 0;

	d = c->relPos[i].x * scanSlope;
	xs = c->relPos[i].y / d;
	ys = c->relPos[i].z / d;

/*
 *  if the absolute values of xs and ys are both less than 1.0, then
 *  we are painting this target with radar energy.  Estimate the value of
 *  the energy that the target sees.
 */

	if ((fabs(xs) <= 1.0) && (fabs(ys) <= 1.0)) {
		d = mag(c->relPos[i]);
		ptbl[i].rval[c->index] = c->cinfo->radarOutput / (d * d);
		if (d >= c->cinfo->radarDRange)
			return 0;
		else if (d >= c->cinfo->radarTRange)
			return 1;
		else
			return 2;
	}
	else
		return 0;

}

int getRadarTarget (c, new)
craft 	*c;
int	new; {

	register int	i, minID;
	register double minD, d;

	for (i=0; i<MAXPLAYERS; ++i)
		c->relValid[i] = isRadarVisible(c, i);

	if (new == 0) {
		i = c->curRadarTarget;
		if (i != -1 && c->relValid[i] != 0)
			return i;
	}

	minD = 1000.0 * NM;
	minID = -1;
	for (i=0; i<MAXPLAYERS; ++i) {
		if (c->relValid[i] == 2) {
			if (new == 1 && i == c->curRadarTarget)
				continue;
			d = mag (c->relPos[i]);
			if (d < minD) {
				minD = d;
				minID = i;
			}
		}
	}

	return minID;
}

void doDroneRadar (c)
craft *c; {

	register int i;

	for (i=0; i<MAXPLAYERS; ++i)
		(void) isRadarVisible(c, i);
}

void plotNormalTarget (u, x, y)
viewer *u;
int	x, y; {

	XDrawLine (u->dpy, u->rwin, u->gc, x-2, y-2, x-2, y+2);
	XDrawLine (u->dpy, u->rwin, u->gc, x-2, y+2, x+2, y+2);
	XDrawLine (u->dpy, u->rwin, u->gc, x+2, y+2, x+2, y-2);
	XDrawLine (u->dpy, u->rwin, u->gc, x+2, y-2, x-2, y-2);
}

extern double heading();

void plotPrimaryTarget (c, u, i, x, y)
craft  *c;
viewer *u;
int	i;
int	x, y; {

	int	xp, yp;
	char	s[16];
	VPoint	rel, deltaV;
	double	d, cl;

	xp = u->radarWidth - 9 * u->rftw;
	yp = u->radarHeight - 4 * u->rfth;

	XFillRectangle (u->dpy, u->rwin, u->gc, x-3, y-3, 7, 7);

/*
 *  Heading of target
 */

	sprintf (s, "  %3.3d \007", (int) (RADtoDEG(ptbl[i].curHeading)));
	XDrawString (u->dpy, u->rwin, u->gc, xp, yp, s, strlen(s));

/*
 *  Relative heading to target.
 */

	rel.x = ptbl[i].Sg.x - c->Sg.x;
	rel.y = ptbl[i].Sg.y - c->Sg.y;
	rel.z = ptbl[i].Sg.z - c->Sg.z;
	sprintf (s, "  %3.3d R", (int) RADtoDEG(heading(&rel)));
	XDrawString (u->dpy, u->rwin, u->gc, xp, yp+u->rfth, s, strlen(s));

/*
 *  Closure rate
 */

	deltaV.x = ptbl[i].Cg.x - c->Cg.x;
	deltaV.y = ptbl[i].Cg.y - c->Cg.y;
	deltaV.z = ptbl[i].Cg.z - c->Cg.z;
	d = mag (rel);
	cl = - (deltaV.x * rel.x + deltaV.y * rel.y + deltaV.z + rel.z) /
		d / NM * 3600.0;
	c->targetDistance = d;
	c->targetClosure = cl;
	sprintf (s, "%5d C", (int)cl);
	XDrawString (u->dpy, u->rwin, u->gc, xp, yp+2*u->rfth, s, strlen(s));

/*
 *  Range to target
 */

	xp = 40 * u->radarWidth / RADAR_WINDOW_WIDTH;
	yp = u->rfth + 4;	
	sprintf (s, "%d", (int) (d / NM));
	XDrawString (u->dpy, u->rwin, u->gc, xp, yp, s, strlen(s));

/*
 *  Altitude of target
 */

	xp = 150 * u->radarWidth / RADAR_WINDOW_WIDTH;
	yp = u->rfth + 4;	
	sprintf (s, "%d", (int) (- ptbl[i].Sg.z / 1000.0));
	XDrawString (u->dpy, u->rwin, u->gc, xp, yp, s, strlen(s));
}

/*
 *  doTEWS :  update the threat display for player i.
 */

void doTEWS (c, u)
craft	*c;
viewer	*u; {

	register int	i, x, y;
	VPoint	rel, tmp;
	double	m, unit;

	XSetForeground (u->dpy, u->gc, u->v->flags & VPMono ?
			BlackPixel(u->v->dpy, u->v->screen) :
			u->v->pixel[blackPixel]);
	XFillRectangle (u->dpy, u->win, u->gc, u->TEWSx-u->TEWSSize/2,
		u->TEWSy-u->TEWSSize/2, u->TEWSSize, u->TEWSSize);
	XSetForeground (u->dpy, u->gc, u->v->flags & VPMono ?
			WhitePixel(u->v->dpy, u->v->screen) :
			u->v->pixel[whitePixel]);

	XDrawArc(u->dpy, u->win, u->gc, u->TEWSx-(u->TEWSSize+1)/2,
		u->TEWSy-(u->TEWSSize+1)/2, u->TEWSSize, u->TEWSSize,
		0, 360*64);

	for (i=0; i<MAXPLAYERS; ++i) {

		if (c->index == i)
			continue;

		if (c->rval[i] > c->cinfo->TEWSThreshold) {
			tmp.x = ptbl[i].Sg.x - c->Sg.x;
			tmp.y = ptbl[i].Sg.y - c->Sg.y;
			tmp.z = ptbl[i].Sg.z - c->Sg.z;
			VTransform (&tmp, &(c->Itrihedral), &(rel));
			m = mag(rel);
			rel.x /= m;
			rel.y /= m;
			rel.z /= m;
			unit = sqrt (rel.x * rel.x + rel.y * rel.y);
			if (unit == 0.0) {
				rel.x = 1.0;
				rel.y = 0.0;
			}
			x = u->TEWSx + (int) (rel.y * u->TEWSSize * 0.4 / unit);
			y = u->TEWSy - (int) (rel.x * u->TEWSSize * 0.4 / unit);
			if (c->team == ptbl[i].team) {
				XDrawLine (u->dpy, u->win, u->gc, x-2, y-2, x-2, y+2);
				XDrawLine (u->dpy, u->win, u->gc, x-2, y+2, x+2, y+2);
				XDrawLine (u->dpy, u->win, u->gc, x+2, y+2, x+2, y-2);
				XDrawLine (u->dpy, u->win, u->gc, x+2, y-2, x-2, y-2);
			}
			else
				XFillRectangle (u->dpy, u->win, u->gc, x-3, y-3, 7, 7);
		}
	}
}
