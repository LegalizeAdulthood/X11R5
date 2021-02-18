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
 
#include <stdio.h>
#include <math.h>
#define ALLOCATE_SPACE
#include "pm.h"

int debug = 0;

/*
 *  We keep a table of atmospheric constants for different altitudes.
 *  These values are important to flight calculations.
 */

struct {
	double	alt;		/* altitude in feet */
	double	rho;		/* rho value (air density) */
	double	mach1;		/* speed of sound in feet per second */
	} *rhop, rhoTable[] = {
#ifdef notdef
	0.0, 23.77, 1116.9,
	2.0, 22.41, 1109.2,
	4.0, 21.11, 1101.4,
	6.0, 19.87, 1093.6,
	8.0, 18.68, 1085.7,
	10.0, 17.55, 1077.8,
	15.0, 14.96, 1057.7,
	20.0, 12.66, 1037.3,
	25.0, 10.65, 1016.4,
	30.0,  8.89,  995.1,
	35.0,  7.365, 973.3,
	40.0,  5.851, 968.5,
	50.0,  3.618, 968.5,
#endif
	60.0,  2.238, 968.5,
	80.0,  0.9065, 980.0,
	100.0, 0.3371, 1015.0,
	120.0, 0.1340, 1053.0,
	160.0, 0.02622, 1083.0,
	100000.0, 0.02622, 1083.0};	/* a large value for alt at the end */

double		deltaT;			/* Update interval in seconds */
double		halfDeltaTSquared;	/* 0.5 * deltaT * deltaT */
double		CM, CN, CY;

/*
 *  calcRho : Calculate air density and the speed of sound by interpolation.
 */

double calcRho (alt, mach)
double alt;
double *mach; {

	double	deltaAlt, b;
	extern	void airProperties();
	double	rho;

	if (alt <= 60000.0)	{
		airProperties (alt, &rho, mach);
		return rho;
	}

	alt = alt / 1000.0;

	for (rhop=rhoTable; alt>(rhop+1)->alt; ++rhop) ;
	deltaAlt = (rhop+1)->alt - rhop->alt;
	b = ((rhop+1)->mach1 - rhop->mach1) / deltaAlt;
	*mach = rhop->mach1 + b * (alt - rhop->alt);
	b = ((rhop+1)->rho - rhop->rho) / deltaAlt;
	return (rhop->rho + b * (alt - rhop->alt)) / 10000.0;

}

/*
 *  twoorder:
 */

void twoOrder (k, d, y, v, newy, newv)
double	k, d, y, v;
double	*newy, *newv; {

	double	s, t, ac, x, c1;

	ac = d * d + 4.0 * k;
	if (ac < 0.0) {
		s = d / 2.0;
		t = sqrt (-ac) / 2.0;
	}
	else {
		s = - d - sqrt(ac) / 2.0;
		t = 0.0;
	}

	if (t == 0.0 || y == 0.0)
		x = 0.0;
	else
		x = atan2 ( y * s - v, t * y ) / t;

	if (x == 0.0)
		c1 = y;
	else if (cos (t * x) != 0.0)
		c1 = y / ( exp (s * x) * cos (t * x) );
	else {
		*newy = 0.0;
		*newv = v;
	}

/*
 *  Now we can compute the values of y and v at the end of this
 *  time interval;
 */

#ifdef notdef
	printf ("s = %g, t = %g, x = %g, y = %g, c1 = %g\n", s, t, x,
		y, c1);

	if (fabs (y - (exp(s*x) * c1 * cos(t * x))) > 0.001)
		printf ("*** possible error ***\n");

	*newv = exp(s*x) * c1 * (s * cos(t*x) - t*sin(t*x));
	if (fabs (v - *newv) > 0.001)
		printf ("*** possible v error *** %g %g\n", v, *newv);
#endif

	x += deltaT;

	*newy = exp (s * x) * c1 * cos (t * x);
	*newv = exp (s * x) * c1 * (s * cos (t * x) - t * sin (t * x));

#ifdef notdef
	printf ("ny = %g, nv = %g\n", *newy, *newv);
#endif

}


/*
 *  calcCoefficients :  Calculate CLift and friends
 */

void calcCoefficients (c, CLift, CDrag)
craft 	*c;
double  *CLift, *CDrag; {

	double CDAlpha, CDBeta;
	double curGear;

/*
 *  We used to interpolate these values, but now use several characteristic
 *  equations to compute these values for a given alpha value. The basic
 *  formulas are:
 *
 *
 *   C  = C        + (alpha * (C       + sin(curFlap) * cFlap ))
 *    L    LOrigin	        LSlope
 *
 *
 *   C  = zero-lift-wave-and-body-drag + induced-drag +
 *    D		speed-brake-drag + flap drag + landing-gear-drag +
 *		drag-based-on-sideslip
 *
 *  There are independent equations defining drag resulting from alpha
 *  and beta values.  The hypoteneuse of those two values becomes the
 *  resultant CDrag value.
 */

	if (c->alpha < c->cinfo->CLNegStall || c->alpha > c->cinfo->CLPosStall) {

		*CLift = 0.0;
		CM = c->cinfo->cmFactor + c->damageCM;
	}
	else {
		*CLift = c->cinfo->CLOrigin + 
			(c->cinfo->CLSlope + sin (c->curFlap) * c->cinfo->cFlap)
			* c->alpha;
		CM = c->cinfo->cmSlope + c->damageCM;
	}

	CDAlpha = (*c->cinfo->CDb)(c) +
		*CLift * *CLift / (pi * c->cinfo->aspectRatio);
	CDAlpha += sin (c->curSpeedBrake) * c->cinfo->cSpeedBrake;
	CDAlpha += sin (c->curFlap) * c->cinfo->cFlapDrag;
	curGear = (c->curGear[0] + c->curGear[1] + c->curGear[2]) / 3.0;
	CDAlpha += sin (curGear) * c->cinfo->cGearDrag;

	if (fabs(c->beta) > c->cinfo->betaStall)
		CN = (*c->cinfo->CnBeta)(c) * sin (fabs(c->beta));
	else
		CN = (*c->cinfo->CnBeta)(c);

	CDBeta = c->cinfo->CDBOrigin + c->cinfo->CDBFactor * 
		cos (c->beta + c->cinfo->CDBPhase);

	*CDrag = sqrt (CDAlpha * CDAlpha + CDBeta * CDBeta);

	CY = c->cinfo->CYbeta * c->beta * fabs (cos (c->beta));

}

double heading (x)
VPoint	*x; {

	double	m;

	if (x->x == 0.0 && x->y == 0.0)
		return 0.0;

	if ((m = atan2 (x->y, x->x)) < 0.0)
		return (pi * 2.0 + m);
	else
		return m;
}

void euler (c)
craft	*c; {

	register double	i, j, k, m;

/*
 *  Compute the heading ...
 */

	i = c->trihedral.m[0][0];
	j = c->trihedral.m[1][0];
	k = c->trihedral.m[2][0];

	if (i == 0.0 && j == 0.0)
		c->curHeading = 0.0;
	else if ((m = atan2 (j, i)) < 0.0)
		c->curHeading = pi * 2.0 + m;
	else
		c->curHeading = m;

/*
 *  and Pitch ...
 */

	c->curPitch = - asin (k);

/*
 *  and Roll ...
 */

	c->curRoll = atan2 (c->trihedral.m[2][1], c->trihedral.m[2][2]);

}

void craftToGround (c, p, g)
craft  *c;
VPoint *p;
VPoint *g; {

	VTransform (p, &(c->trihedral), g);

}

void calcGForces (c, f, w)
craft *c;
VPoint *f;
double w; {

	VPoint t;

	t = *f;
	t.z -= w;

	VTransform (&t, &(c->Itrihedral), &(c->G));
	c->G.x = - c->G.x / w;
	c->G.y = - c->G.y / w;
	c->G.z = - c->G.z / w;
}

void calcAlphaBeta (c, alpha, beta)
craft *c;
double *alpha, *beta; {

	VPoint	C;
	double	h;

	if (mag(c->Cg) > 0.0) {
		VTransform (&(c->Cg), &(c->Itrihedral), &C);
		*beta = atan2 (C.y, C.x);
		h = sqrt (C.y * C.y + C.x * C.x);
		*alpha = atan (C.z / h);
	}
	else {
		*alpha = 0.0;
		*beta = 0.0;
	}

}

/*
 *  buildEulerMatrix :  Build a transformation matrix based on the supplied
 *			euler angles.
 */

void buildEulerMatrix (roll, pitch, heading, m)
double	roll, pitch, heading;
VMatrix	*m; {

	register double sinPhi, cosPhi, sinTheta, cosTheta, sinPsi, cosPsi;

	sinPhi = sin (roll);
	cosPhi = cos (roll);
	sinTheta = sin (pitch);
	cosTheta = cos (pitch);
	sinPsi = sin (heading);
	cosPsi = cos (heading);

	m->m[0][0] = cosTheta * cosPsi;
	m->m[0][1] = sinPhi * sinTheta * cosPsi - cosPhi * sinPsi;
	m->m[0][2] = cosPhi * sinTheta * cosPsi + sinPhi * sinPsi;
	m->m[1][0] = cosTheta * sinPsi;
	m->m[1][1] = sinPhi * sinTheta * sinPsi + cosPhi * cosPsi;
	m->m[1][2] = cosPhi * sinTheta * sinPsi - sinPhi * cosPsi;
	m->m[2][0] = - sinTheta;
	m->m[2][1] = sinPhi * cosTheta;
	m->m[2][2] = cosPhi * cosTheta;
	m->m[0][3] = m->m[1][3] = m->m[2][3] = 0.0;
	m->m[3][0] = m->m[3][1] = m->m[3][2] = 0.0;
	m->m[3][3] = 1.0;

}

double elevatorSetting (c)
craft *c; {

	register double s;

	s = c->Se + c->SeTrim;

	if (s > 1.0)
		s = 1.0;
	else if (s < -1.0)
		s = -1.0;
	
	return s;
}

int  flightCalculations (c)
craft *c; {

	double	q, s, CLift, CDrag;
	double	ClBeta;
	double	FLift, FDrag, FWeight, FSideForce;
	double	Vmag, D, angle;
	double	ar, ap, aq, cosR, sinR;
	double  deltaRoll, deltaPitch, deltaYaw;
	double	muStatic, muKinetic;
	double	ke, be;
	double	y, newy;
	VPoint	F, Fg, V, tmpPt, r;
	VMatrix turn, mtx, mtx1;
	int	positionUpdated = 0;

	c->prevSg = c->Sg;

	c->rho = calcRho (-(c->Sg.z), &c->mach1);
	calcAlphaBeta(c, &(c->alpha), &(c->beta));

/*
 *  A note about thrust:  Normal thrust diminishes in proportion to the
 *  decrease in air density.
 */

	Vmag = mag(c->Cg);
	c->mach = Vmag / c->mach1;

	if (c->fuel <= 0.0 || isFunctioning (c, SYS_ENGINE1) == 0)
		c->curThrust = 0.0;
	else
		c->curThrust = (*c->cinfo->thrust)(c);

	calcCoefficients (c, &CLift, &CDrag);
	ClBeta = (*c->cinfo->ClBeta)(c);

#ifdef FLIGHTDEBUG
	if (debug) {
		printf ("\n------\ntime = %g secs.\n", curTime);
		printf ("alpha = %g deg, beta = %g deg\nCL = %g, CD = %g\n",
			RADtoDEG(c->alpha), RADtoDEG(c->beta), CLift, CDrag);
		printf ("CnBeta = %g, ClBeta = %g\n", CN, ClBeta);
	}
#endif

/*
 *  Compute the resultant force vector on the aircraft.  By the way, the
 *  variable "q" should more properly be named "qS" -- it is the dynamic
 *  pressure times S, the reference wing area.
 */

	q = c->rho * c->cinfo->wingS * Vmag * Vmag * 0.5;
	s = c->cinfo->wings;
	FLift = CLift * q;
	FDrag = CDrag * q;
	FSideForce = CY * q;

#ifdef FLIGHTDEBUG
	if (debug) {
		printf ("rho = %g, FLift = %g lbs, FThrust = %g lbs, ",
			c->rho, FLift, c->curThrust);
		printf ("FDrag = %g lbs\n", FDrag);
	}
#endif

	F.x = c->curThrust - sin(c->alpha) * cos(c->beta) * FLift -
		cos(c->alpha) * cos(c->beta) * FDrag;
	F.y = sin(c->alpha) * sin(c->beta) * FLift - sin(c->alpha) *
		sin(c->beta) * FDrag + FSideForce;
	F.z = -cos(c->alpha) * cos(c->beta) * FLift - sin(c->alpha) *
		cos(c->beta) * FDrag;

/*
 *  Get ground friction coefficients
 */

	if (c->flags & FL_GND_CONTACT)
		if (c->flags & FL_BRAKES) {
			muStatic = c->cinfo->muBStatic;
			muKinetic = c->cinfo->muBKinetic;
		}
		else {
			muStatic = c->cinfo->muStatic;
			muKinetic = c->cinfo->muKinetic;
		}

/*
 *  Now calculate changes in position (Sg) and velocity (Cg).
 */

	if ((Vmag > c->cinfo->maxNWS) || ((c->flags & FL_GND_CONTACT) == 0))
		c->flags &= ~FL_NWS;
	else
		c->flags |= FL_NWS;

	if (c->flags & FL_NWS) {

	    c->curNWDef = c->Sa * c->cinfo->maxNWDef;

	    if (c->curNWDef != 0.0) {

		r.x = c->cinfo->gearD2;
		r.y = c->cinfo->gearD1 / tan(c->curNWDef);
		r.z = 0.0;
		angle = Vmag / r.y * deltaT;

/*
 *  Nose wheel steering mode.
 *  Relocate the aircraft and its trihedral (this code assumes that the
 *  plane is rolling on a flat surface (i.e. z is constant).
 */

		tmpPt = r;
		VTransform(&tmpPt, &(c->trihedral), &r);

		VIdentMatrix (&turn);
		turn.m[0][3] = - c->Sg.x - r.x;
		turn.m[1][3] = - c->Sg.y - r.y;
		turn.m[2][3] = - c->Sg.z;
		VRotate (&turn, ZRotation, angle);
		turn.m[0][3] = turn.m[0][3] + c->Sg.x + r.x;
		turn.m[1][3] = turn.m[1][3] + c->Sg.y + r.y;
		turn.m[2][3] = turn.m[2][3] + c->Sg.z;
		VTransform (&(c->Sg), &turn, &tmpPt);
		c->Sg = tmpPt;

		VIdentMatrix (&turn);
		VRotate (&turn, ZRotation, angle);
		mtx = c->trihedral;
		VMatrixMultByRank (&mtx, &turn, &(c->trihedral), 3);
		VTransform (&(c->Cg), &turn, &tmpPt);
		c->Cg = tmpPt;

	        transpose (&c->trihedral, &c->Itrihedral);

		euler (c);
		positionUpdated = 1;
	    }

	    craftToGround (c, &F, &Fg);
	    FWeight = c->cinfo->emptyWeight + c->fuel;

	    if ((c->fuel -= fuelUsed(c) + c->leakRate * deltaT) <= 0.0) {
		c->fuel = 0.0;
		c->curThrust = 0.0;
		c->throttle = 0;
	    }

	    Fg.z += FWeight;

/*
 *  Factor in ground friction for both static and moving planes.
 */

	    if (c->Cg.x + c->Cg.y == 0.0 && sqrt (Fg.x*Fg.x + Fg.y*Fg.y) <=
		muStatic * Fg.z) {
		    Fg.x = 0.0;
		    Fg.y = 0.0;
	    }
	    else {

/*
 *  Okay, the plane is moving.  Quantify the current kinetic energy of the
 *  moving craft and add the energy added this period by all forces EXCEPT
 *  ground friction (we'll name this "ke").  If the energy removed by the friction
 *  force is greater than ke, then the craft will stop sometime during this
 *  period -- we won't bother to calculate exactly where we stop; just zero out
 *  the x and y force and velocity components.
 */

		ke = 0.5 * FWeight / a * sqrt (c->Cg.x * c->Cg.x +
			c->Cg.y * c->Cg.y);
		ke += pow(sqrt(Fg.x * Fg.x + Fg.y * Fg.y), 2.0) *
			halfDeltaTSquared * a / FWeight;
		be = pow(Fg.z * muKinetic, 2.0) * halfDeltaTSquared * a / FWeight;
		if (be >= ke) {
			Fg.x = 0.0;
			Fg.y = 0.0;
			c->Cg.x = 0.0;
			c->Cg.y = 0.0;
		}
		else {

/*
 *  Getting to this point means that we're rolling along the ground (and not stopping)
 *  -- make sure our roll is zeroed, then cancel the local Y-component of our
 *  velocity vector (tires don't roll sideways) and then calculate the drag
 *  contributed by the rolling wheels.
 */

			c->curRoll = 0.0;
			VTransform (&(c->Cg), &(c->Itrihedral), &V);
			V.y = 0.0;
			VTransform (&V, &(c->trihedral), &(c->Cg));
			D = Fg.z * muKinetic;
			Vmag = mag (c->Cg);
			if (Vmag > 0.0) {
				Fg.x -= D * c->Cg.x / Vmag;
				Fg.y -= D * c->Cg.y / Vmag;
				Fg.z -= D * c->Cg.z / Vmag;
			}
		}
	    }


/* Nose wheel steering is only active when we cannot lift off -- cancel z */

	    Fg.z = 0.0;
	    calcGForces (c, &Fg, FWeight);

	}
	else {

/*
 *  Resolve moments
 */

	    ap = fsign (c->p);
	    aq = fsign (c->q);
	    ar = fsign (c->r);
	    if ((c->flags & FL_GND_CONTACT) == 0) {
		ap = (c->Sa * c->cinfo->effAileron * q - c->cinfo->LDamp *
			ap * c->p * c->p * 0.5 );
		ap += (c->beta - c->Sr * c->cinfo->effRudder) *
			ClBeta * q * s;
		ap += c->damageCL * q;
		ap /= c->cinfo->I.m[0][0];
	    }
	    else
		ap = 0.0;

	    deltaRoll  = c->p * deltaT + ap * halfDeltaTSquared;
	    c->p = c->p + ap * deltaT;

/*
 *  Resolve pitch-axis (Y-axis) changes
 */

	    y = c->alpha - elevatorSetting(c) * c->cinfo->effElevator;
	    twoOrder (CM * q / c->cinfo->I.m[1][1],
		c->cinfo->MDamp, y, c->q, &newy, &(c->q));
	    deltaPitch = newy - y;

/*
 *  Resolve yaw-axis (Z-axis) changes.
 *
 *  We do some trickery here.
 *  If the absolute value of the sideslip angle is greater than 90 degrees,
 *  we trick the code into believing that the sideslip angle is the negative
 *  of its reciprocal value (e.g. -176 becomes -4 degrees).  We do this with
 *  the (somewhat inaccurate) assumption that the CN value for that angle is
 *  roughly equal to the other.
 */

	    y = c->beta - c->Sr * c->cinfo->effRudder;

	    if (y > pi / 2.0)
		y = pi - y;
	    else if (y > pi / 2.0)
		y = -pi - y;

	    twoOrder (CN * q * s / c->cinfo->I.m[2][2],
		c->cinfo->NDamp, y, c->r, &newy, &(c->r));
	    deltaYaw = y - newy;

#ifdef FLIGHTDEBUG
	    if (debug) {
		printf ("p = %g deg/sec, q = %g\
 deg/sec, r = %g deg/sec\n", RADtoDEG(c->p), RADtoDEG(c->q), RADtoDEG(- c->r));
	    }
#endif

/*
 *  Compute new transformation matrices
 */

	    buildEulerMatrix (deltaRoll, deltaPitch, deltaYaw, &mtx);
	    VMatrixMultByRank (&mtx, &c->trihedral, &mtx1, 3);
	    c->trihedral = mtx1;
	    
	    transpose (&c->trihedral, &c->Itrihedral);
	    euler (c);

	    craftToGround (c, &F, &Fg);
	    FWeight = c->cinfo->emptyWeight + c->fuel;

	    if ((c->fuel -= fuelUsed(c) + c->leakRate * deltaT) <= 0.0) {
		c->fuel = 0.0;
		c->curThrust = 0.0;
	    }

	    Fg.z += FWeight;

/*
 *  If we are on the ground, level the wings and compute wheel drag forces.  Wheel
 *  drag always acts in the opposite direction of the velocity vector.
 */

	    if (c->flags & FL_GND_CONTACT) {
		c->curRoll = 0.0;
	        buildEulerMatrix (c->curRoll, c->curPitch,
			c->curHeading, &(c->trihedral));
	        transpose (&c->trihedral, &c->Itrihedral);

		VTransform (&(c->Cg), &(c->Itrihedral), &V);
		V.y = 0.0;
		VTransform (&V, &(c->trihedral), &(c->Cg));
		D = Fg.z * muKinetic;
		Vmag = mag (c->Cg);
		Fg.x -= D * c->Cg.x / Vmag;
		Fg.y -= D * c->Cg.y / Vmag;
		Fg.z -= D * c->Cg.z / Vmag;
	    }

#ifdef FLIGHTDEBUG
	    if (debug) {
	    	printf ("v = %g kts (Mach %.3g), Fg = { %g, %g, %g }\n",
			FPStoKTS(Vmag), c->mach, Fg.x, Fg.y, Fg.z);
	    	printf ("F = { %g, %g, %g }\n",
			F.x, F.y, F.z);
	    }
#endif


/*
 *  Are we on the ground without the prospect of gaining altitude?
 *  If so, cancel the vertical force component.
 */

	    if ((c->flags & FL_GND_CONTACT) && Fg.z > 0.0)
		Fg.z = 0.0;

	    calcGForces (c, &Fg, FWeight);


	}

/*
 *  Update our position (in flight mode).
 */

	if (positionUpdated == 0) {

	    c->Sg.x += c->Cg.x * deltaT + Fg.x / FWeight
			* a * halfDeltaTSquared;
	    c->Sg.y += c->Cg.y * deltaT + Fg.y / FWeight
			* a * halfDeltaTSquared;
	    c->Sg.z += c->Cg.z * deltaT + Fg.z / FWeight
			* a * halfDeltaTSquared;

	}

	c->Cg.x += Fg.x / FWeight * a * deltaT;
	c->Cg.y += Fg.y / FWeight * a * deltaT;
	c->Cg.z += Fg.z / FWeight * a * deltaT;

#ifdef FLIGHTDEBUG
	if (debug) {
		printf ("Altitude = %g ft\n", -c->Sg.z);
		printf ("Euler angles RPY { %g, %g, %g }\n",
			RADtoDEG(c->curRoll),
			RADtoDEG(c->curPitch), RADtoDEG(c->curHeading));
		printf ("Cg = { %g, %g, %g }  ", c->Cg.x, c->Cg.y, c->Cg.z);
		printf ("Sg = { %g, %g, %g }\n", c->Sg.x, c->Sg.y, c->Sg.z);
	}
#endif


/*
 *  Normalize the vertical position.  If our altitude is now below our
 *  contact threshold, mark us as having ground contact and adjust the
 *  altitude.
 */

	if (c->Sg.z >= - c->cinfo->groundingPoint.z) {
		c->flags |= FL_GND_CONTACT;
		c->Sg.z = - c->cinfo->groundingPoint.z;

/*
 *  If the vertical velocity component is too great, the plane has crashed ...
 */

		if (c->Cg.z > c->cinfo->crashC)
			return 1;
		else
			c->Cg.z = 0.0;
/*
 *  Level the wings.
 */

		c->curRoll = 0.0;
	        buildEulerMatrix (c->curRoll, c->curPitch,
			c->curHeading, &(c->trihedral));
	        transpose (&c->trihedral, &c->Itrihedral);

	}
	else
		c->flags &= ~(FL_NWS | FL_GND_CONTACT);

	return 0;
}
