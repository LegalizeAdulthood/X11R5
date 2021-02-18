#include <X11/Xlib.h>
#include <stdio.h>
#include "scale.h"

static char *skipBlanks (s)
char *s; {

	for (; *s == ' '; s++) ;
	return s;
}

void doScale (dpy, win, gc, s, value, fth, ftw)
Display		*dpy;
Window		win;
GC		gc;
scaleControl	*s;
double		value;
int		fth;
int		ftw; {

	XPoint		p;
	XSegment	seg[32];
	int		is = 0;
	double		tickDelta;
	int		top = s->yorg - s->length;
	int		yRef, tickDeltaPixels;
	int		y, w, v, n, hticks;
	int		ytick = (double) s->minorInterval / s->scale + 0.5;
	int		doMajor;
	char		str[16], *q;

	yRef = s->yorg - (int) ((double) s->length / 2.0 + 0.5);

	tickDelta = value - 
		(double) ((int) value / s->minorInterval * s->minorInterval);

	tickDeltaPixels = (int) (tickDelta / s->scale + 0.5);
	hticks = s->length / (2 * ytick);
	y = yRef + hticks * ytick + tickDeltaPixels;

	if (value >= 0.0)
		v = value - tickDelta - hticks * s->minorInterval;
	else
		printf ("can't do negative value scales, yet\n");

	if (tickDeltaPixels != 0) {
		y -= ytick;
		v += s->minorInterval;
	}

	v = v / s->minorInterval * s->minorInterval;

/*
 *  Draw the index mark
 */

	seg[is].y1 = seg[is].y2 = yRef;
	if (s->orientation & orientRight) {
		seg[is].x1 = s->xorg - 2;
		seg[is].x2 = seg[is].x1 - s->indexSize;
	}
	else {
		seg[is].x1 = s->xorg + 2;
		seg[is].x2 = seg[is].x1 + s->indexSize;
	}
	++ is;

/*
 *  step through the doMajor and minor ticks
 */

	for (; y > top; (y -= ytick, v += s->minorInterval)) {

/*  we don't mark negative ticks */

		if (v < 0)
			continue;

		seg[is].x1 = s->xorg;
		seg[is].y1 = seg[is].y2 = p.y = y;

		if ((v % s->majorInterval) == 0) {
			doMajor = 1;
			w = s->majorSize;
			if (s->orientation & orientRight)
				seg[is].x2 = s->xorg + w;
			else
				seg[is].x2 = s->xorg - w;
			sprintf (str, s->format, (double) v / s->divisor);
			q = skipBlanks (str);
			p.y += 0.30 * fth + 0.5;
			if (s->orientation & orientRight) {
				p.x = seg[is].x2 + 3;
			}
			else {
				p.x = seg[is].x2 - 3 - ftw * strlen(q);
			}
		}
		else {
			doMajor = 0;
			w = s->minorSize;
			if (s->orientation & orientRight)
				seg[is].x2 = s->xorg + w;
			else
				seg[is].x2 = s->xorg - w;
		}

		if (doMajor)
			XDrawString (dpy, win, gc, p.x, p.y, q,
				strlen (q));
		++ is;	 
	}

	XDrawSegments (dpy, win, gc, seg, is);

}

void doCompassScale (dpy, win, gc, s, value, fth, ftw)
Display		*dpy;
Window		win;
GC		gc;
scaleControl	*s;
double		value;
int		fth;
int		ftw; {

	XPoint		p;
	XSegment	seg[32];
	int		is = 0;
	int		top = s->xorg + s->length;
	int		x, w, v, n;
	int		xtick = (double) s->minorInterval / s->scale + 0.5;
	int		doMajor;
	char		str[16], *q;
	double		tickDelta;
	int		tickDeltaPixels, xRef, hticks;

	xRef = s->xorg + (int) ((double) s->length / 2.0 + 0.5);

	tickDelta = value - 
		(double) ((int) value / s->minorInterval * s->minorInterval);

	tickDeltaPixels = (int) (tickDelta / s->scale + 0.5);
	hticks = s->length / (2 * xtick);
	x = xRef - hticks * xtick - tickDeltaPixels;

	if (value >= 0.0)
		v = value - tickDelta - hticks * s->minorInterval;
	else
		printf ("can't do negative value scales, yet\n");

	if (v <= 0)
		v += 36000;

	if (tickDeltaPixels != 0) {
		x += xtick;
		v += s->minorInterval;
	}

	v = v / s->minorInterval * s->minorInterval;

/*
 *  Draw the index mark
 */

	seg[is].x1 = seg[is].x2 = xRef;
	if (s->orientation & orientRight) {
		seg[is].y1 = s->yorg - 2;
		seg[is].y2 = seg[is].y1 - s->indexSize;
	}
	else {
		seg[is].y1 = s->yorg + 2;
		seg[is].y2 = seg[is].y1 + s->indexSize;
	}
	++ is;

/*
 *  step through the doMajor and minor ticks
 */

	for (; x < top; (x += xtick, v += s->minorInterval)) {

		if (v > 36000)
			v -= 36000;

		seg[is].y1 = s->yorg;
		seg[is].x1 = seg[is].x2 = p.x = x;

		if ((v % s->majorInterval) == 0) {
			doMajor = 1;
			w = s->majorSize;
			if (s->orientation & orientRight)
				seg[is].y2 = s->yorg + w;
			else
				seg[is].y2 = s->yorg - w;
			sprintf (str, s->format, (double) v / s->divisor);
			q = skipBlanks (str);
			p.x -= (strlen(q) * ftw) / 2;
			if (s->orientation & orientRight) {
				p.y = seg[is].y2 + 3 + fth;
			}
			else {
				p.y = seg[is].y2 - 3;
			}
		}
		else {
			doMajor = 0;
			w = s->minorSize;
			if (s->orientation & orientRight)
				seg[is].y2 = s->yorg + w;
			else
				seg[is].y2 = s->yorg - w;
		}

		if (doMajor)
			XDrawString (dpy, win, gc, p.x, p.y, q,
				strlen (q));
		++ is;	 
	}

	XDrawSegments (dpy, win, gc, seg, is);

}
