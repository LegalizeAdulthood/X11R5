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

typedef struct _entry {
	double	time;
	double	min;
	craft	*c;
	VPoint	Sg;
	struct	_entry *next;
	}	entry;


extern craftType *lookupCraft();
extern int mdebug;
extern void lookForCannonImpacts();
extern void euler();
extern void newExplosion();

int	fireMissile (c, index)
craft	*c;
int	index; {

	register craft	*m;
	register int	i;
	VPoint		s;
	VPoint		cY, mX, mY, mZ;
	double		v;

	for ((i=0, m= &mtbl[0]); i<MAXPROJECTILES; (++i, ++m))
		if (m->type == CT_FREE) {
			m->type = CT_MISSILE;
			break;
		}

	if (i == MAXPROJECTILES)
		return -1;

	m->cinfo = lookupCraft ("aim-9m");
	m->fuel = m->cinfo->maxFuel;
	m->curThrust = m->cinfo->maxThrust;

/*
 *  Build trihedral based on the launching aircraft's current velocity vector
 *  rather than simply it's current direction vector.
 *
 *	(1)  build a unit velocity vector.
 *	(2)  calculate missiles local Z axis from
 *		plane's-y-axis CROSS missile's-unit-velocity-vector
 *	(3)  calculate missile's Y axis.
 */

	if ((v = mag (c->Cg)) == 0.0) {
		m->trihedral = c->trihedral;
		m->Itrihedral = c->Itrihedral;
		m->curRoll = c->curRoll;
		m->curPitch = c->curPitch;
		m->curHeading = c->curHeading;
	}
	else {
		mX = c->Cg;
		mX.x /= v;
		mX.y /= v;
		mX.z /= v;
		cY.x = c->trihedral.m[0][1];
		cY.y = c->trihedral.m[1][1];
		cY.z = c->trihedral.m[2][1];

		VCrossProd (&mX, &cY, &mZ);
		VCrossProd (&mZ, &mX, &mY);

		m->trihedral.m[0][0] = mX.x;
		m->trihedral.m[1][0] = mX.y;
		m->trihedral.m[2][0] = mX.z;
		m->trihedral.m[0][1] = mY.x;
		m->trihedral.m[1][1] = mY.y;
		m->trihedral.m[2][1] = mY.z;
		m->trihedral.m[0][2] = mZ.x;
		m->trihedral.m[1][2] = mZ.y;
		m->trihedral.m[2][2] = mZ.z;

		transpose (&m->trihedral, &m->Itrihedral);
		euler (m);
	}

	m->Cg = c->Cg;
	VTransform (&(c->cinfo->wStation[index]), &(c->trihedral), &s);
	m->Sg.x = c->Sg.x + s.x;
	m->Sg.y = c->Sg.y + s.y;
	m->Sg.z = c->Sg.z + s.z;
	m->armFuse = (int) (m->cinfo->armDelay / deltaT);
	m->flags = FL_HAS_GYRO;
	m->createTime = curTime;

/*
 * kludge
 */

	m->curRadarTarget = c->curRadarTarget;

	return 0;
}

int	killMissile (c)
craft	*c; {
	c->type = CT_FREE;
	return 0;
}

int	lookForImpacts () {

	craft	*c, *m;
	entry	p[MAXPLAYERS], *list, *q, *r, *rprev;
	VPoint	v, s0;
	double	t, d;
	int	i, j;

	for (m=mtbl, i=0; i<MAXPROJECTILES; ++i, ++m) {

		if (m->type == CT_CANNON) {
			lookForCannonImpacts (m);
			continue;
		}
		else if (m->type != CT_MISSILE || m->armFuse > 0)
			continue;

		list = (entry *) NULL;
		for (c=ptbl, j=0; j<MAXPLAYERS; ++j, ++c) {

			if (c->type == CT_FREE)
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

			v.x = c->Sg.x - c->prevSg.x - m->Sg.x + m->prevSg.x;
			v.y = c->Sg.y - c->prevSg.y - m->Sg.y + m->prevSg.y;
			v.z = c->Sg.z - c->prevSg.z - m->Sg.z + m->prevSg.z;
			s0.x = c->prevSg.x - m->prevSg.x;
			s0.y = c->prevSg.y - m->prevSg.y;
			s0.z = c->prevSg.z - m->prevSg.z;

/*
 * Compute time of minimum distance between the two objects (note that units
 * here are UPDATE_INTERVAL seconds).
 */

			t = - (v.x * s0.x + v.y * s0.y + v.z * s0.z) /
				(v.x * v.x + v.y * v.y + v.z * v.z);

			if (mdebug)
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
				if (isMissileHit (d, c)) {
					t = 0.0;
				}
			}

			if (t >= 0.0 && t <= 1.0) {
				q = &p[j];

				q->Sg = m->prevSg;
				q->Sg.x += (m->Sg.x - m->prevSg.x) * t;
				q->Sg.y += (m->Sg.y - m->prevSg.y) * t;
				q->Sg.z += (m->Sg.z - m->prevSg.z) * t;

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
					rprev->next = q;
					q->next = r;
				}
				q->time = t;
				q->c = c;
				q->min = sqrt (pow(v.x * t + s0.x, 2.0) +
					pow (v.y * t + s0.y, 2.0) +
					pow (v.z * t + s0.z, 2.0) );
					if (mdebug)
					printf ("perigee %g feet; craft %d.\n",
						q->min, j);
			}
		}

/*
 *  Now look for missile hits in the list of perigees.
 */

		for (r=list; r != (entry *) NULL; r=r->next)
			if (isMissileHit (r->min, r->c)) {
				newExplosion (&(r->Sg), 15.0, 10.0, 3.0); 
				killMissile (m);
				if (absorbDamage (r->c, 20) == 0)
					killPlayer (r->c);
				break;
			}
	}

	return(0);
}

/*ARGSUSED*/
int isMissileHit (min, c)
double min;
craft	*c; {

	return (min < 50.0) ? 1 : 0;
}

#define IRScanSlope 	0.57735

int isIRVisible (c, m, t)
craft *c;
VMatrix *m;
VPoint	*t; {

	VPoint	relPos;

	if (c->type == CT_FREE)
		return 0;

	VTransform (&(c->Sg), m, t);

	if (t->z <= 0.0)
		return 0;

	relPos.x = t->x / (t->z * IRScanSlope);
	relPos.y = t->y / (t->z * IRScanSlope);

	return (sqrt(relPos.x*relPos.x + relPos.y*relPos.y) > 1.0) ? 0 : 1;
}

int getIRTarget (c, m, t)
craft *c;
VMatrix *m;
VPoint  *t; {

	int	i, n;
	craft	*p;
	VPoint	tNew, tMin;
	double	m1, min;

	if (c->curRadarTarget != -1 && isIRVisible(&ptbl[c->curRadarTarget], m, t))
		return c->curRadarTarget;

/*
 *  Look for a target.  Designate the closest one as a new target.
 */

	min = 1000000.0;
	n = -1;
	for (i=0, p=ptbl; i<MAXPLAYERS; ++i, ++p) {
		if (p == c)
			continue;
		if (p->type != CT_FREE)
		if (isIRVisible (p, m, &tNew)) {
			m1 = mag(tNew);
			if (m1 < min) {
				n = i;
				min = m1;
				tMin = tNew;
			}
		}
	}

	*t = tMin;
	return n;
}

void createMissileEyeSpace (c, eyeSpace)
craft	*c;
VMatrix	*eyeSpace; {

	VPoint	CntrInt, up, C1, C2;
	VMatrix	mtx, es;
	double	Hypotenuse, h1, SinA, CosA;

/*
 *  Create an eyeSpace transformation matrix to convert from global to
 *  local coordinates.
 */

	CntrInt.x = c->Sg.x + c->trihedral.m[0][0];
	CntrInt.y = c->Sg.y + c->trihedral.m[1][0];
	CntrInt.z = c->Sg.z + c->trihedral.m[2][0];

/*
 *  If the missile has a gyroscope, then it can determine which way "down" is
 *  and, hence, compensate for gravity.  Here, we'll compensate by pointing
 *  the nose of the missile up by a maximum of 12 degrees at all times.
 */

	if (c->flags & FL_HAS_GYRO)
		if (c->curThrust > 0.0)
			CntrInt.z += 0.208;
		else
			CntrInt.z += 0.30;

	up.x = c->Sg.x - c->trihedral.m[0][2];
	up.y = c->Sg.y - c->trihedral.m[1][2];
	up.z = c->Sg.z - c->trihedral.m[2][2];

	VIdentMatrix (eyeSpace);
	eyeSpace->m[0][3] = -c->Sg.x;
	eyeSpace->m[1][3] = -c->Sg.y;
	eyeSpace->m[2][3] = -c->Sg.z;
	VTransform(&CntrInt, eyeSpace, &C1);

	VIdentMatrix (&mtx);
	Hypotenuse = sqrt(C1.x * C1.x + C1.y * C1.y);
	if (Hypotenuse > 0.0) {
		CosA = C1.y / Hypotenuse;
		SinA = C1.x / Hypotenuse;
		mtx.m[0][0] = CosA;
		mtx.m[1][0] = SinA;
		mtx.m[0][1] = -SinA;
		mtx.m[1][1] = CosA;
		es = *eyeSpace;
		VMatrixMult(&es, &mtx, eyeSpace);
	}

	VTransform(&CntrInt, eyeSpace, &C2);
	VIdentMatrix (&mtx);
	Hypotenuse = sqrt(C2.y * C2.y + C2.z * C2.z);
	if (Hypotenuse > 0.0) {
		CosA = C2.y / Hypotenuse;
		SinA = -C2.z / Hypotenuse;
		mtx.m[1][1] = CosA;
		mtx.m[2][1] = SinA;
		mtx.m[1][2] = -SinA;
		mtx.m[2][2] = CosA;
		es = *eyeSpace;
		VMatrixMult(&es, &mtx, eyeSpace);
	}

        VTransform (&up, eyeSpace, &C2);
	VIdentMatrix (&mtx);
	h1 = sqrt (C2.y * C2.y + C2.z * C2.z);
	Hypotenuse = sqrt(C2.x * C2.x + h1 * h1);
	if (Hypotenuse > 0.0) {
		CosA = h1 / Hypotenuse;
		SinA = C2.x / Hypotenuse;
		if (C2.z < 0.0) {
			CosA = -CosA;
		}
		mtx.m[0][0] = CosA;
		mtx.m[2][0] = SinA;
		mtx.m[0][2] = -SinA;
		mtx.m[2][2] = CosA;
		es = *eyeSpace;
		VMatrixMult(&es, &mtx, eyeSpace);
	}

	VIdentMatrix (&mtx);
	mtx.m[1][1] = 0.0;
	mtx.m[2][1] = 1.0;
	mtx.m[1][2] = 1.0;
	mtx.m[2][2] = 0.0;
	es = *eyeSpace;
	VMatrixMult(&es, &mtx, eyeSpace);

}

void trackTarget (c)
craft *c; {

	VMatrix eyeSpace, mtx, mtx1;
	VPoint	t, t1;
	double	v, h, m, maxTurnSlope, aMax = 30.0 * a;
	double	deltaRoll, deltaPitch, deltaYaw;
	double	cosR, sinR;

	createMissileEyeSpace (c, &eyeSpace);

/*
 *  Now let's get to target tracking;  the missile won't start tracking until
 *  0.60 seconds has elapsed.  Then, if we don't already have a target
 *  designated, get one.
 */

	if (curTime - c->createTime < 0.60) {
		t.x = 0.0;
		t.y = 0.0;
		t.z = 1.0;
	}
	else if ((c->curRadarTarget = getIRTarget(c, &eyeSpace, &t)) == -1) {
		if (mdebug)
			printf ("Missile elects to self-destruct\n");
		newExplosion (&(c->Sg), 15.0, 10.0, 3.0); 
		killMissile (c);
		return;
	}

	t.x = t.x / t.z;
	t.y = t.y / t.z;

	h = sqrt (t.x*t.x + t.y*t.y);
	v = mag (c->Cg);

/*
 *  We'll constrain missile turns to a 20 degree/second unless it's velocity
 *  would make that greater than 30g's.
 */

	if ((m=(v*v - aMax*aMax/4.0)) > 0.0)
		maxTurnSlope = aMax / (2.0 * sqrt (m)) * deltaT;

	else
		maxTurnSlope = 0.3639 * deltaT;

	if (mdebug)
		printf ("slope = %g; maxTurnSlope = %g\n", h, maxTurnSlope);

	if (h > maxTurnSlope) {
		t.x = t.x * maxTurnSlope / h;
		t.y = t.y * maxTurnSlope / h;
	}

        cosR = cos (c->curRoll);
	sinR = sin (c->curRoll);

	deltaRoll = 0.0;
	deltaPitch = atan (t.y);
	deltaYaw   = atan (t.x);

/*
 *  Re-orient the velocity vector towards our new direction.
 */

        VIdentMatrix (&mtx);
	if (deltaPitch != 0.0)
		VRotate (&mtx, YRotation, - deltaPitch);
	if (deltaYaw != 0.0)
		VRotate (&mtx, ZRotation, deltaYaw);
	VTransform (&(c->Cg), &(c->Itrihedral), &t);
	VTransform (&t, &mtx, &t1);
	VTransform (&t1, &(c->trihedral), &(c->Cg));

	if (mdebug)
		printf ("Missile changes: pitch: %g, yaw: %g.\n",
			RADtoDEG(deltaPitch), RADtoDEG(deltaYaw));

	buildEulerMatrix (deltaRoll, deltaPitch, deltaYaw, &mtx);
	VMatrixMultByRank (&mtx, &c->trihedral, &mtx1, 3);
	c->trihedral = mtx1;

	transpose (&c->trihedral, &c->Itrihedral);
	euler (c);

}
