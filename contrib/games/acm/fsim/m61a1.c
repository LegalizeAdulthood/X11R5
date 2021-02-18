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
#include <string.h>

#define BORE_CROSS_SIZE	7
#define MUZZLE_VELOCITY (3.2808 * 1036.0)
#define TRACER_MOD	10	
#define OFFSET_ANGLE	(DEGtoRAD(3.0))		/* the gun points up at this angle */
#define RATE_OF_FIRE	(3000.0 / 60.0)		/* rounds per second */
#define FUZZ		(DEGtoRAD(1.0))		/* distribution due to vibration */

int select_m61a1();
int display_m61a1();
int update_m61a1();
int press_m61a1();
int release_m61a1();
extern craftType * lookupCraft();
extern FILE * acm_fopen();
void lcos();

int	cdebug = 0;		/* set to 1 to debug cannon fire */

weaponDesc m61a1Desc = {
	WK_M61A1,
	select_m61a1,		/* select */
	update_m61a1,		/* update */
	display_m61a1,		/* display procedure */
	press_m61a1,		/* fire */
	release_m61a1,		/* fire button release */
	};

/*
 *  We'll take a running average of our pitch and yaw rates to keep the
 *  aiming reticle from jumping all over the screen.
 */

#define HIST	4

typedef struct {
	double	pitch_hist[HIST];
	double	yaw_hist[HIST];
	int	cur;		/* current entry in the histories */
	int	count;		/* number of valid entries */
	double	pitch_total;
	double	yaw_total;
	}	hist_t;

static hist_t	history[MAXPLAYERS];

/*
 *  m61a1 selection function
 *
 *  A selection function normally determines whether there are any weapons
 *  of this type on-board.  If so, and the weapon system is functional
 *  (in other words, undamaged) then return 1; otherwise return 0.
 */

int select_m61a1 (c)
craft *c; {

	hist_t	*p;

	p = &history[c->index];
	p->count = p->cur = 0;
	p->pitch_total = p->yaw_total = 0.0;

	if (c->station[0].type == WK_M61A1 && c->station[0].info > 0)
		return 1;
	else
		return 0;

}

/*
 *  m61a1 display function
 *
 *  Update the HUD display strings associated with this weapon system.
 */

/*ARGSUSED*/
int display_m61a1 (c, w, u)
craft *c;
craftType *w;
viewer *u; {

	char	s[16];
	register int tx, ty, m;
	XSegment seg[2];
	Drawable d;
	hist_t	*p;

	p = &history[c->index];
	if ((m = p->cur = p->cur + 1) >= HIST)
		m = p->cur = 0;
	if (p->count != HIST)
		(p->count) ++;
	else {	
		p->pitch_total -= p->pitch_hist[m];
		p->yaw_total -= p->yaw_hist[m];
	}
	p->pitch_hist[m] = c->q;
	p->yaw_hist[m] = c->r;
	p->pitch_total += c->q;
	p->yaw_total += c->r;

        d = (u->v->flags & VPMono) ? (Drawable) u->v->monoPixmap :
	    (Drawable) u->win;

	strcpy (c->leftHUD[0], "LCOS");
	sprintf (s, "%.3d M-61A1", c->station[0].info);
	strcpy (c->leftHUD[1], s);
	if (c->station[0].info3)
		strcpy (c->leftHUD[2], "FIRING");
	else
		strcpy (c->leftHUD[2], "");

/*
 *  Draw the boresight cross
 */

	m = (double) BORE_CROSS_SIZE * (double) u->width /
		(double) VIEW_WINDOW_WIDTH + 0.5;

	tx = u->xCenter;
	ty = u->yCenter - 3 * 9;	/* kludged, for now */
	seg[0].x1 = tx - m;
	seg[0].x2 = tx + m;
	seg[0].y1 = seg[0].y2 = ty;
	seg[1].x1 = seg[1].x2 = tx;
	seg[1].y1 = ty - m;
	seg[1].y2 = ty + m;
	XDrawSegments (u->dpy, d, u->gc, seg, 2);

/*
 *  Plot the reticle.
 */

	lcos (c, u);

}

int	press_m61a1 (c)
craft	*c; {
	c->station[0].info3 |= 1;
	return 0;
}

int	release_m61a1 (c)
craft	*c; {
	c->station[0].info3 &= ~1;
	return 0;
}

extern long random ();

int	update_m61a1 (c)
craft	*c; {

	craft	*m;
	int	i;
	VPoint	tmp, mvel;
	double	fuzz, fuzzAngle, phiY, phiZ, tm, mv = MUZZLE_VELOCITY;

	if (c->station[0].info3 == 0)
		return 0;

/*
 *  Got any ammunition ?
 */

	if (c->station[0].info <= 0)
		return -1;

/*
 *  Allocate a projectile record
 */

	for ((i=0, m= &mtbl[0]); i<MAXPROJECTILES; (++i, ++m))
		if (m->type == CT_FREE) {
			m->type = CT_CANNON;
			break;
		}

	if (i == MAXPROJECTILES)
		return -1;

/*
 *  Determine how far we're fuzzed off the ideal boresight.
 */

	fuzz = ((double)(random() & 32767) +
		(double)(random() & 32767)) / 32768.0 - 1.0;
	fuzz = fuzz * FUZZ;
	fuzzAngle = 2.0 * pi * (double)(random() & 511) / 512.0;

/*
 *  Determine the initial velocity of the projectile stream.
 */

	phiZ = fuzz * sin (fuzzAngle);
	phiY = fuzz * cos (fuzzAngle);
	tm = mv * cos (OFFSET_ANGLE + phiZ);

	tmp.x = tm * cos (phiY); 
	tmp.y = tm * sin (phiY); 
	tmp.z = mv * sin (-OFFSET_ANGLE + phiZ);
	VTransform (&tmp, &(c->trihedral), &mvel);

	m->owner = c->index;
	m->createTime = curTime;
	m->Cg = c->Cg;
	m->curRoll = c->curRoll;
	m->curPitch = c->curPitch;
	m->curHeading = c->curHeading;
	m->Cg.x += mvel.x;
	m->Cg.y += mvel.y;
	m->Cg.z += mvel.z;

/*
 *  Determine the initial position.
 */

	VTransform (&(c->cinfo->wStation[0]), &(c->trihedral), &tmp);
	m->Sg.x = c->Sg.x + tmp.x;
	m->Sg.y = c->Sg.y + tmp.y;
	m->Sg.z = c->Sg.z + tmp.z;
	m->prevSg = m->Sg;

/*
 *  Subtract the number of rounds fired.
 */

	m->rounds = RATE_OF_FIRE * deltaT;
	m->rounds = (m->rounds > c->station[0].info) ? c->station[0].info : m->rounds;
	m->tracerMod = TRACER_MOD;
	m->tracerVal = c->station[0].info2 % m->tracerMod;
	c->station[0].info -= m->rounds;
	c->station[0].info2 += m->rounds;

	m->cinfo = lookupCraft ("m61a1 cannon");

	return 0;

}

extern craftType * newCraft ();

int	placeCannon(c, m, poly, cnt)
craft	*c;
VMatrix	*m;
VPolygon **poly;
int	*cnt; {

	double	t, intervalT;
	VPoint	v, s;
	int	i, j, k, n;
	VPoint	 *q, tmp;
	VPolygon **p;

/*
 *  Reduce the shell path to a set of parametric equations.
 */

	v.x = c->Sg.x - c->prevSg.x;
	v.y = c->Sg.y - c->prevSg.y;
	v.z = c->Sg.z - c->prevSg.z;

/*
 *  Now add each tracer shell to the view.
 */

	intervalT = 1.0 / c->rounds;
	for (t=intervalT*c->tracerVal; t <= 1.0; t += intervalT * TRACER_MOD) {
		s.x = -v.x * t;
		s.y = -v.y * t;
		s.z = -v.z * t;
		if (c->Sg.z + s.z > 0.0)	/* underground? don't plot it */
			continue;
		n = c->cinfo->object->numPolys;
		p = c->cinfo->object->polygon;
		j = *cnt;
		for (i=0; i<n; ++i) {
			poly[j] = VCopyPolygon(p[i]);
			for ((k=0, q=poly[j]->vertex); k<poly[j]->numVtces; (++k, ++q)) {
				VTransform(q, m, &tmp);
				tmp.x += s.x;
				tmp.y += s.y;
				tmp.z += s.z;
				*q = tmp;
			}
			++j;
		}
		*cnt = j;
	}

	return(0);
}

void initm61a1()
{

	craftType	*c;
	FILE    	*f;

	wtbl[1] = m61a1Desc;

	c = newCraft();
	c->name = strdup("m61a1 cannon");

	c->placeProc = placeCannon;

	f = acm_fopen ("bullet", "r");
	c->object = VReadObject(f);
	fclose (f);

}

int cannonCalculations (c)
craft *c; {


/*
 *  Kill projectile streams after 10.0 seconds of flight or when
 *  they strike the ground.
 */

	if (curTime - c->createTime > 10.0)
		return 1;

	if (c->Sg.z > 0.0)
		return 1;

	c->prevSg = c->Sg;

	c->Sg.x += c->Cg.x * deltaT;
	c->Sg.y += c->Cg.y * deltaT;
	c->Sg.z += c->Cg.z * deltaT + a * halfDeltaTSquared;

	c->Cg.z += a * deltaT;
	return 0;

}

/*
 *  plotReticle :  draws the aiming reticle onto the HUD.
 *
 *  An aiming reticle is a circle with an inner arc that clues the
 *  pilot as to the range of the target.  Twelve tic marks are used
 *  to demarcate the range -- each tic represents 1000 feet of
 *  distance.
 */

static struct {
	double	x, y;
	} ticTable[12] = {
 { 0.0, -1.0 },
 { 0.5, -0.866 },
 { 0.866, -0.5 },
 { 1.0, 0.0 },
 { 0.866, 0.5 },
 { 0.5, 0.866 },
 { 0.0, 1.0 },
 { -0.5, 0.866 },
 { -0.866, 0.5 },
 { -1.0, 0.0 },
 { -0.866, -0.5 },
 { -0.5, -0.866 } };

void plotReticle (c, u, x, y, range)
craft	*c;
viewer	*u;
int	x;
int	y;
int	range; {

#define RETICLE_SIZE	65
#define RANGE_SIZE	59
#define TICK_SIZE	(RETICLE_SIZE + 12)

	register int	i, size, xt, yt, size1, xt1, yt1, nseg, arc;
	XSegment	seg[16];
	register double rs, rt;
	Drawable	d;

	d = (u->v->flags & VPMono) ? (Drawable) u->v->monoPixmap :
	    (Drawable) u->win;

	size = RETICLE_SIZE * u->scaleFactor + 0.5;
	if ((size & 1) == 0)
		++size;		/* insure it is an odd value */
	xt = x - size / 2;
	yt = y - size / 2;
	rs = size / 2;
	rt = ((TICK_SIZE + 1) / 2) * u->scaleFactor;

	for (i=0; i < 12; i++) {
		seg[i].x1 = x +
			(int) (rs * ticTable[i].x + 0.5);
		seg[i].y1 = y +
			(int) (rs * ticTable[i].y + 0.5);
		seg[i].x2 = x +
			(int) (rt * ticTable[i].x + 0.5);
		seg[i].y2 = y +
			(int) (rt * ticTable[i].y + 0.5);
	}

	nseg = 12;
	XDrawArc (u->dpy, d, u->gc, xt, yt, size, size, 0,
		360 * 64);

	if (range != -1) {
		if (range > 12000)
			range = 12000;
		arc = - range * 23040 / 12000;
		size1 = RANGE_SIZE * u->scaleFactor + 0.5;
		if ((size1 & 1) == 0)
			++ size1;
		if (size == size1)
			-- size1;
		xt1 = x - size1 / 2;
		yt1 = y - size1 / 2;
		XDrawArc (u->dpy, d, u->gc, xt1, yt1, size1, size1,
			90 * 64, arc);
	}

	XDrawSegments (u->dpy, d, u->gc, seg, nseg);

}

void lcos (c, u)
craft *c;
viewer	*u; {


/*
 *  How does this LCOS thing work, anyway?
 *
 *  First, let me say that this may or, more probably, may not look
 *  anything like the technique used in a real fighter's gun sighting
 *  system.  Having said that, here's the basic assumptions and steps
 *  used to generate the reticle on the HUD:
 *
 *  1)  Using radar, we know the distance to the target.  Assuming that the
 *	radar is in working order and is locked onto something.
 *
 *  2)  We know the muzzle velocity of the shells we're firing and can
 *	use that value to closely estimate the flight time of the shells
 *	to a target that's a certain distance away (the target).  Call
 *	this value "fTime".
 *
 *  3)	Use the muzzle velocity vector to determine the
 *	relative position (w.r.t. out craft) of a shell "fTime" seconds into
 *	its flight.  Call this vector "pos".
 *
 *  4)	Gravity will accelerate the shells. Use d = 0.5 * g * t ^ 2
 *	to add a distance (along the Z axis that will approximate the
 *	effect of gravity.  Add that to "pos".
 *
 *  5)	We know our pitch and yaw rates.  These rotations will have the
 *	effect of visually "bending" the cannon stream, from the pilot's
 *	perspective.  Use the values to generate a matrix to transform
 *	"pos" by an amount proportional to our "fTime" value.
 *
 *  6)	Convert our "pos" vector to screen x,y coordinates and call
 *	plotReticle.
 */

	double	fTime, range, gm, pitch_rate, yaw_rate;
	VPoint	pos, tmp, tmp1;
	VPoint	zg, z;
	VMatrix	rotation;
	register int x, y, clue;
	hist_t	*hist;

	hist = &history[c->index];

/*  Step 1  */

	if (c->curRadarTarget >= 0)
		range = c->targetDistance;
	else
		range = 2500.0;

/*  Step 2  */

	fTime = range / MUZZLE_VELOCITY;

/*  Step 3  */

	pos.x = fTime * MUZZLE_VELOCITY * cos (OFFSET_ANGLE);
	pos.y = 0.0;
	pos.z = - fTime * MUZZLE_VELOCITY * sin (OFFSET_ANGLE);

/*  Step 4  */

	zg.x = zg.y = 0.0;
	zg.z = 1.0;

	VTransform (&zg, &(c->Itrihedral), &z);

	gm = 0.5 * a * fTime * fTime;
	pos.x += z.x * gm;
	pos.y += z.y * gm;
	pos.z += z.z * gm;

/*  Step 5  */

	VIdentMatrix (&rotation);

	pitch_rate = hist->pitch_total / (double) hist->count;
	yaw_rate = hist->yaw_total / (double) hist->count;

	if (pitch_rate != 0.0)
		VRotate (&rotation, YRotation, pitch_rate * fTime);
	if (yaw_rate != 0.0)
		VRotate (&rotation, ZRotation, yaw_rate * fTime);

	VTransform (&pos, &rotation, &tmp);

/*  Step 6  */

	tmp1 = tmp;
	x = u->v->Middl.x + tmp1.y * u->v->Scale.x / tmp1.x + 0.5;
	y = u->v->Middl.y + tmp1.z * u->v->Scale.y / tmp1.x + 0.5;

	if (c->curRadarTarget >= 0)
		clue = c->targetDistance + 0.5;
	else
		clue = -1;

	plotReticle (c, u, x, y, clue);
}

/*
 *  lookForCannonImpacts : Track cannon shells and look for impacts with
 *			   aircraft.  This algorithm tracks each shell's
 *			   flight.
 */

typedef struct _entry {
	double	time;
	double	min;
	VPoint	Sg;
	craft	*c;
	struct	_entry *next;
	}	entry;


extern int cdebug;

void	lookForCannonImpacts (m)
craft	*m; {

	craft	*c;
	entry	p[MAXPLAYERS], *list, *q, *r, *rprev;
	VPoint	v, s0, prevSg, Vm;
	double	t, d, roundT, intervalT;
	int	j;

	intervalT = 1.0 / m->rounds;
	for (roundT=0.0; roundT < 1.0; roundT += intervalT) {

		Vm.x = m->Sg.x - m->prevSg.x;
		Vm.y = m->Sg.y - m->prevSg.y;
		Vm.z = m->Sg.z - m->prevSg.z;

		prevSg.x = m->prevSg.x - Vm.x * (1.0 - roundT);
		prevSg.y = m->prevSg.y - Vm.y * (1.0 - roundT);
		prevSg.z = m->prevSg.z - Vm.z * (1.0 - roundT);

		list = (entry *) NULL;
		for (c=ptbl, j=0; j<MAXPLAYERS; ++j, ++c) {

		if (c->type == CT_FREE || m->owner == c->index)
			continue;

/*
 * Reduce the relative motion of this object to a the parametric system
 * of equations:
 *		x(t) = vx * t + s0x
 *		y(t) = vy * t + s0y
 *		z(t) = vz * t + s0z
 *
 * We can then compute the time of perigee (closest pass) along with
 * the associated minimum distance.
 */

			v.x = c->Sg.x - c->prevSg.x - Vm.x;
			v.y = c->Sg.y - c->prevSg.y - Vm.y;
			v.z = c->Sg.z - c->prevSg.z - Vm.z;
			s0.x = c->prevSg.x - prevSg.x;
			s0.y = c->prevSg.y - prevSg.y;
			s0.z = c->prevSg.z - prevSg.z;

/*
 * Compute time of minimum distance between the two objects (note that units
 * here are UPDATE_INTERVAL seconds).
 */

			t = - (v.x * s0.x + v.y * s0.y + v.z * s0.z) /
				(v.x * v.x + v.y * v.y + v.z * v.z);

			if (cdebug)
			printf ("perigee in %g seconds with player %d\n",
				t*deltaT, j);

/*
 *  If the closest pass occurs during this update interval, check for a hit.
 *  We'll build a linked list of all craft that this projectile may strike
 *  during this period, arranged in ascending order by time of "perigee"
 *  (closest pass).  We'll then test for strikes.  If a projectile misses
 *  the first object, then it may have struck subsequent objects in the
 *  list ...
 */

/*
 *  One special case occurs when a target or missile's turn suddenly
 *  changes the perigee time from positive to negative.  If the missile
 *  is within hitting range at t=0 and the time of perigee is negative,
 *  then zap 'em.
 */

			if (t < 0.0) {
				d = sqrt (s0.x *s0.x + s0.y * s0.y +
					s0.z * s0.z);
				if (isCannonHit (d, c)) {
					printf ("detected a \"curve hit\"\n");
					t = 0.0;
				}
			}

			if (t >= 0.0 && t <= 1.0) {
				q = &p[j];
				q->Sg = c->prevSg;
				q->Sg.x += (c->Sg.x - c->prevSg.x) * t;
				q->Sg.y += (c->Sg.y - c->prevSg.y) * t;
				q->Sg.z += (c->Sg.z - c->prevSg.z) * t;

				if (list == (entry *) NULL) {
					q->next = list;
					list = q;
				}
				else if (list->time > t) {
					q->next = list;
					list = q;
				}
				else {
					for (rprev=list, r=list->next; r != (entry *) NULL;) {
						if (r->time > t) break;
						rprev = r;
						r = r->next;
					}
					if (rprev != list) 
						rprev->next = q;
					q->next = r;
				}
				q->time = t;
				q->c = c;
				q->min = sqrt (pow(v.x * t + s0.x, 2.0) +
					pow (v.y * t + s0.y, 2.0) +
					pow (v.z * t + s0.z, 2.0) );
					if (cdebug)
					printf ("perigee %g feet; craft %d.\n",
						q->min, j);
			}
		}

/*
 *  Now look for cannon hits in the list of perigees.
 */

		for (r=list; r != (entry *) NULL; r=r->next)
			if (isCannonHit (r->min, r->c)) {
				newExplosion (&(r->Sg), 3.0, 2.0, 0.5);
				if (absorbDamage (r->c, 3) == 0)
					killPlayer (r->c);
				break;
			}
	}
}

/*ARGSUSED*/
int isCannonHit (min, c)
double min;
craft	*c; {

	return (min < 30.0) ? 1 : 0;
}
