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

extern char *ItoA();
extern void doDroneRadar(), doTEWS();
extern void placeCraft();
extern void doScale(), doCompassScale();

#define lim	((double) -1052500.0)

#define MAXPOLYGONS	8192

static VPolygon *poly[MAXPOLYGONS];

static craft *sortList;

void insertCraft();

void doViews() {

	int	polyCount;
	register unsigned long curPixel;
	register craft	*c, *p;
	register int	i, j, k;
	register viewer *q;
	VPoint	 	vp, fwd, up;
	VPoint		horiz[4];
	int		mono;

/*
 *  Build a vector of polygons for all objects in the scene.
 *
 *  This vector should be ordered from "most distant" to "closest" so that
 *  the final display will end up correct.  Rather than generalizing this
 *  to death, we'll use a few heuristics to get very close to what we need:
 *
 *  (0) Build a single polygon to represent the ground.
 *  (1) Objects on the surface (stbl) are collected first.
 *  (2) Planes and projectiles (ptbl and mtbl) are first sorted in descending
 *      order by their distance from the observer and then polygons are
 *	collected.
 */


    for ((i=0, c=ptbl); i<MAXPLAYERS; (++i, ++c)) {

	if (c->type == CT_DRONE)
		doDroneRadar (c);

	if (c->type != CT_PLANE)
		continue;

/*
 *  Set up the eyespace transformation for this viewpoint
 */

	VTransform (&c->cinfo->viewPoint, &(c->trihedral), &vp);
	vp.x += c->Sg.x;
	vp.y += c->Sg.y;
	vp.z += c->Sg.z;

	VTransform (&c->viewDirection, &(c->trihedral), &fwd);
	fwd.x += vp.x;
	fwd.y += vp.y;
	fwd.z += vp.z;

	VTransform (&c->viewUp, &(c->trihedral), &up);
	up.x += vp.x;
	up.y += vp.y;
	up.z += vp.z;

	VGetEyeSpace (c->vl->v, vp, fwd, up);

	polyCount = 0;
	sortList = (craft *) NULL;

	horiz[0].x = c->Sg.x - lim;
	horiz[0].y = c->Sg.y - lim;
	horiz[0].z = 0.0;
	horiz[1].x = c->Sg.x - lim;
	horiz[1].y = c->Sg.y + lim;
	horiz[1].z = 0.0;
	horiz[2].x = c->Sg.x + lim;
	horiz[2].y = c->Sg.y + lim;
	horiz[2].z = 0.0;
	horiz[3].x = c->Sg.x + lim;
	horiz[3].y = c->Sg.y - lim;
	horiz[3].z = 0.0;
	
	poly[0] = VCreatePolygon (4, &horiz[0], groundColor);
	polyCount = 1;

	for ((j=0, p=stbl); j<MAXSURFACE; (++j, ++p)) {
	    if (p->type != CT_FREE)
		insertCraft (c, p);
	}

	for ((j=0, p=ptbl); j<MAXPLAYERS; (++j, ++p)) {
	    if (p->type != CT_FREE && p != c) {
		insertCraft (c, p);
	    }
	}

	for ((j=0, p=mtbl); j<MAXPROJECTILES; (++j, ++p)) {
	    if (p->type != CT_FREE)
		insertCraft (c, p);
	}

	for (p=sortList; p != (craft *) NULL; p=(craft *)p->next)
		placeCraft (p, poly, &polyCount);

/*
 *  Clip all polygons
 */

        for (j=0; j<polyCount; ++j) {
            VTransformPolygon (poly[j], &(c->vl->v->eyeSpace));
            poly[j] = VClipPolygon(poly[j], c->vl->v->clipPoly);
	}

/*
 *  Display this image for each viewer associated with this craft
 */

	for (q=c->vl; q != NULL; q = q->next) {

            mono = q->v->flags & VPMono;
	    curPixel = -1;
            for (j=0; j<polyCount; ++j) {
                if (poly[j]) {
                    if (mono == 0 &&
			curPixel != (k=q->v->pixel[poly[j]->color->index]))
		    {
                        XSetForeground (q->dpy, q->gc, k);
                        curPixel = k;
                    }
		    if (mono)
			VDrawPolygon (q->v, q->win, q->gc, poly[j]);
		    else
			VFillPolygon (q->v, q->win, q->gc, poly[j]);
                }

           }

/*  Draw Head-Up Display and instrument panel */

	    doHUD(c, q);
	    doRadar (c, q);
	    doTEWS (c, q);		/* doRadar must be called first */
	    doRPM(c, q);
	    doPanel (c,q);

/*  Expose the completed drawing  */

            VExposeBuffer (q->v, q->gc);

/*
 * Erase the un-displayed planes.
 */

	    if (mono == 0) {
                curPixel = *(q->v->pixel);
                XSetForeground (q->dpy, q->gc, curPixel);
#ifdef USE_PIXMAP_ANIMATION
		if (q->v->flags & VPPixmap)
			XFillRectangle (q->dpy, q->v->monoPixmap, q->gc,
				0, 0, q->v->width, q->v->height);
		else
#endif
                	XFillRectangle (q->dpy, q->win, q->gc,
				0, 0, q->width, q->height);
	    }

	}

/*
 *  Release polygons
 */

        for (j=0; j<polyCount; ++j)
            if (poly[j])
	        VDestroyPolygon (poly[j]);

    }
}

/*
 *  insertCraft :  Insert a craft into sortList in descending order.
 */

void insertCraft (c, p)
craft *c, *p; {

    double dist, offset;
    VPoint ds;
    craft  *q, *prev;

/*
 *  Here's a kludge for you:  to avoid polygon clipping, I'm going to 
 *  cheat and hack a way to get ground objects to display properly.
 *  if the Z coordinate of an object is zero (i.e. on ground objects),
 *  I'll add a huge offset to their distance values to force them to be
 *  plotted first -- and in roughly their correct drawing order.
 *
 */

    offset = (p->Sg.z == 0.0) ? 1000.0 * NM : 0.0;

    ds.x = p->Sg.x - c->Sg.x;
    ds.y = p->Sg.y - c->Sg.y;
    ds.z = p->Sg.z - c->Sg.z;
    p->order = dist = mag (ds) + offset;

    if (sortList != (craft *) NULL) {
        for ((q=sortList, prev=(craft *) NULL); q != (craft *) NULL;) {
	    if (q->order < dist) {
		p->next = (struct _craft *) q;
		if (prev == (craft *) NULL)
		    sortList = p;
		else
		    prev->next = (struct _craft *) p;
		break;
	    }
	    prev = q;
	    q = (craft *) q->next;
        }
        if (q == (craft *) NULL) {
	    prev->next = (struct _craft *) p;
	    p->next = (struct _craft *) NULL;
        }
    }
    else {
        sortList = p;
        p->next = (struct _craft *) NULL;
    }
}

int doHUD (c, u)
craft  *c;
viewer *u; {

	char	buffer[80];
	int	i, x, x1, y;
	Drawable d;

	d = (u->v->flags & VPMono) ? (Drawable) u->v->monoPixmap :
	    (Drawable) u->win;

	if (c->viewDirection.x < 0.90)
		return 0;
	XSetFont (u->dpy, u->gc, u->font->fid);
	if (u->v->flags & VPMono)
	    XSetForeground (u->dpy, u->gc,
			    BlackPixel(u->v->dpy, u->v->screen));
	else
	    XSetForeground (u->dpy, u->gc, u->v->pixel[HUDPixel]);
	sprintf (buffer, "a=%.1f", RADtoDEG(c->alpha));
	x = 730 * u->width / VIEW_WINDOW_WIDTH;
	y = 130 * u->height / VIEW_WINDOW_HEIGHT;
	XDrawString (u->dpy, d, u->gc, x, y, buffer, strlen(buffer));

	x1 = 410 * u->width / VIEW_WINDOW_WIDTH;
	sprintf (buffer, "%4.1f", c->G.z);
	XDrawString (u->dpy, d, u->gc, x1, y, buffer, strlen(buffer));	

	y = 390 * u->height / VIEW_WINDOW_HEIGHT;
	sprintf (buffer, "%7d", (int)(-c->Cg.z * 60.0));
	XDrawString (u->dpy, d, u->gc, x, y, buffer, strlen(buffer));

	x = 410 * u->width / VIEW_WINDOW_WIDTH;
	doCompassScale (u->dpy, d, u->gc, &(u->hdgScale), RADtoDEG(c->curHeading)*100.0,
		u->fth, u->ftw);
	doScale (u->dpy, d, u->gc, &(u->altScale),
		- (c->Sg.z + c->cinfo->groundingPoint.z), u->fth, u->ftw);
	doScale (u->dpy, d, u->gc, &(u->velScale), FPStoKTS(mag(c->Cg)),
		u->fth, u->ftw);

	sprintf (buffer, "%3d%% %s", (int)((double)c->throttle/327.68),
		(c->flags & FL_AFTERBURNER) ? "AB" : "");
	XDrawString (u->dpy, d, u->gc, x, y, buffer, strlen(buffer));

	x = 380 * u->width / VIEW_WINDOW_WIDTH;
	y = 420 * u->height / VIEW_WINDOW_HEIGHT;

	doLadder (c, u);

	for (i=0; i<3; ++i) {
		XDrawString (u->dpy, d, u->gc, x, y, c->leftHUD[i],
			strlen (c->leftHUD[i]));
		y += u->fth;
	}

	return 0;

}


int	doLadder (c, u)
craft *c;
viewer *u;{

	double 		x = c->curPitch, cx, s;
	double		sinP, cosP, pixelsPerDegree;
	int		a1, a2, a3, a4, a5, plotFPM;
	XSegment	seg[24];
	XRectangle	rect[1];
	int		i, rung, windX, windY, w, tx, ty;
	char		buf[16];
	VPoint		tmp;
	Drawable	d;

	double L_half_width = 90.0 * u->scaleFactor;
	double L_end = 25.0 * u->scaleFactor;
	double L_major_tick_interval = 10.0;
	double L_major_tick_height = 90.0 * u->scaleFactor;

	d = (u->v->flags & VPMono) ? (Drawable) u->v->monoPixmap :
	    (Drawable) u->win;

	rect[0].x = u->xCenter - (int) (125.0 * u->scaleFactor + 0.5);
	rect[0].y = u->yCenter - (int) (110.0 * u->scaleFactor + 0.5);
	rect[0].width = 250.0 * u->scaleFactor + 0.5;
	rect[0].height = 220.0 * u->scaleFactor + 0.5;

	s = ((int)(RADtoDEG(x)) / 10 * 10) - 20;
	rung = s;

	pixelsPerDegree = L_major_tick_height / L_major_tick_interval;
	cx = (RADtoDEG(x) - s) * pixelsPerDegree;
	sinP = cos(c->curRoll);
	cosP = sin(c->curRoll);
	for (i=0; i<5; ++i, cx -= L_major_tick_height, rung +=10) {
		seg[i*2].x1 = L_half_width * sinP + cx * cosP + u->xCenter;
		seg[i*2].y1 = -L_half_width * cosP + cx * sinP + u->yCenter;
		seg[i*2].x2 =  L_end * sinP + cx * cosP + u->xCenter;
		seg[i*2].y2 = -L_end * cosP + cx * sinP + u->yCenter;
		seg[i*2+1].x1 = -L_end * sinP + cx * cosP + u->xCenter;
		seg[i*2+1].y1 = L_end * cosP + cx * sinP + u->yCenter;
		seg[i*2+1].x2 =  -L_half_width * sinP + cx * cosP + u->xCenter;
		seg[i*2+1].y2 = L_half_width * cosP + cx * sinP + u->yCenter;
		sprintf (buf, "%d", rung);
		if (seg[i*2].x1 >= rect[0].x && seg[i*2].x1 <
			rect[0].x+rect[0].width &&
			seg[i*2].y1 >= rect[0].y && seg[i*2].y1 <
			rect[0].y+rect[0].height)
			XDrawString (u->dpy, d, u->gc, 
				seg[i*2].x1 + 2, seg[i*2].y1 + 5,
				buf, strlen(buf));
	}

	i = i * 2;

	XSetClipRectangles (u->dpy, u->gc, 0, 0, rect, 1, YXBanded);

/*
 *  Determine the location of the flight path marker
 */

	VTransform (&c->Cg, &c->Itrihedral, &tmp);

	plotFPM = 1;

	if (mag(c->Cg) == 0.0 || tmp.x == 0.0) {
		windX = u->xCenter;
		windY = u->yCenter;
	}
	else if (tmp.x > 0.0) {
		windX = u->xCenter + tmp.y * u->v->Scale.x / tmp.x + 0.5;
		windY = u->yCenter + tmp.z * u->v->Scale.y / tmp.x + 0.5;
	}
	else
		plotFPM = 0;

	if (plotFPM) {

		a1 = 18.0 * u->scaleFactor + 0.5;
		a2 = 9.0 * u->scaleFactor + 0.5;
		a3 = 15.0 * u->scaleFactor + 0.5;

		seg[i].x1 = windX - a1;
		seg[i].y1 = windY;
		seg[i].x2 = windX - a2;
		seg[i++].y2 = windY;

		seg[i].x1 = windX + a1;
		seg[i].y1 = windY;
		seg[i].x2 = windX + a2;
		seg[i++].y2 = windY;

		seg[i].x1 = windX;
		seg[i].y1 = windY - a2;
		seg[i].x2 = windX;
		seg[i++].y2 = windY - a1;
	}

/*
 *  Gather weapon display info (and maybe draw a reticle).
 */

	doWeaponDisplay (c, u);

/*
 *  Draw a target designator around the current primary radar target.
 */

	if (c->curRadarTarget >= 0) {

		w = DESIGNATOR_SIZE * u->height / VIEW_WINDOW_HEIGHT * 0.5;

		VTransform (&(ptbl[c->curRadarTarget].Sg),
			&u->v->eyeSpace, &tmp);

/* radar target is assumed to be ahead of us (tmp.z > 0.0) */
		
		tx = u->v->Middl.x + tmp.x * u->v->Scale.x / tmp.z + 0.5;
		ty = u->v->Middl.y - tmp.y * u->v->Scale.y / tmp.z + 0.5;

		seg[i].x1 = tx - w;
		seg[i].y1 = ty - w;
		seg[i].x2 = tx + w;
		seg[i++].y2 = ty - w;

		seg[i].x1 = tx + w;
		seg[i].y1 = ty - w;
		seg[i].x2 = tx + w;
		seg[i++].y2 = ty + w;

		seg[i].x1 = tx + w;
		seg[i].y1 = ty + w;
		seg[i].x2 = tx - w;
		seg[i++].y2 = ty + w;

		seg[i].x1 = tx - w;
		seg[i].y1 = ty + w;
		seg[i].x2 = tx - w;
		seg[i++].y2 = ty - w;
	}

	XDrawSegments (u->dpy, d, u->gc, seg, i);

	XDrawArc (u->dpy, d, u->gc, u->xCenter-2, u->yCenter-2,
		4, 4, 0, 360*64);

	if (plotFPM) {
		a4 = 16.0 * u->scaleFactor + 0.5;
		a5 = (a4 + 1) / 2;
		XDrawArc (u->dpy, d, u->gc, windX-a5, windY-a5,
			a4, a4, 0, 360*64);
	}

	XSetClipMask (u->dpy, u->gc, None);

	return 0;
}

int	doRPM (c, u)
craft *c;
viewer *u;{

	int	x, y, x1, y1, len, i;
	double  percent, p;
	char	s[32], s1[32];

	x = ENG_X * u->width / FS_WINDOW_WIDTH;
	y = ENG_Y * u->width / FS_WINDOW_WIDTH;
	len = eng_x_hot - 6;

	XSetForeground (u->dpy, u->gc, u->v->flags & VPMono ?
			WhitePixel(u->v->dpy, u->v->screen) :
			u->v->pixel[whitePixel]);
	XSetBackground (u->dpy, u->gc, u->v->flags & VPMono ?
			BlackPixel(u->v->dpy, u->v->screen) :
			u->v->pixel[blackPixel]);
	XCopyPlane (u->dpy, u->eng, u->win, u->gc, 0, 0, eng_width,eng_height,
		x - eng_x_hot, y - eng_y_hot, 1);

	if (isFunctioning (c, SYS_ENGINE1))
		percent = (double) c->throttle / 32768.0 * 100.0;
	else
		percent = 0.0;
	if (percent < 60.0) {
		p = (60.0 - percent) / 120.0 * pi;
		x1 = x + len * cos(p);
		y1 = y - len * sin(p);
	}
	else {
		p = (percent - 60.0) / 40.0 * pi;
		x1 = x + len * cos(p);
		y1 = y + len * sin(p); 
	}

	XDrawLine (u->dpy, u->win, u->gc, x, y, x1, y1);

/*
 *  Print Fuel total and Fuel consumption rate
 */

	XSetBackground (u->dpy, u->gc, (u->v->flags & VPMono) ?
		BlackPixel (u->dpy, u->v->screen) :
		VConstantColor (u->v, blackPixel));

	sprintf (s, "%7s lbs Total", ItoA ((int) c->fuel, s1));
	XDrawImageString (u->dpy, u->win, u->gc, x-30, y+68, s, strlen(s));
	sprintf (s, "%7s lbs/hour", ItoA ((int) (fuelUsed(c)*3600.0/deltaT), s1));
	XDrawImageString (u->dpy, u->win, u->gc, x-30, y+68+u->fth, s, strlen(s));

/*
 *  Display Flap Setting
 */

	if (c->flapSetting > DEGtoRAD(29.0))
		i=3;
	else if (c->flapSetting > DEGtoRAD(19.0))
		i=2;
	else if (c->flapSetting > DEGtoRAD(9.0))
		i=1;
	else
		i=0;

	x = u->xCenter - (u->radarWidth + 1) / 2 - flaps0_width -
		(int) (20.0 * u->scaleFactor);
	y = FLAP_Y * u->width / FS_WINDOW_WIDTH;

	XCopyPlane (u->dpy, u->flap[i], u->win, u->gc, 0, 0, flaps0_width,
		flaps0_height, x, y, 1);
	sprintf (s, "Flaps: %d", (int) RADtoDEG(c->curFlap));
	XDrawImageString (u->dpy, u->win, u->gc, x+flaps0_x_hot, y+flaps0_y_hot,
		s, strlen(s));

	return 0;
}
