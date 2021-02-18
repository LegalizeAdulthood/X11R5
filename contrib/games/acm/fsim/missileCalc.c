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
#include "pm.h"

int mdebug = 0;
extern double calcRho();
extern void trackTarget();
extern void craftToGround();

int  missileCalculations (c)
craft *c; {

	double	q, rho;
	double	FLift, FDrag, FWeight;
	double	Vmag;
	VPoint	F, Fg;
	VMatrix mtx;

/*
 *  Check for ground impact.  We do this at the beginning to permit us to
 *  kill ground targets.
 */

	if (c->Sg.z > 0.0) {
		killMissile (c);
		return 1;
	}

	trackTarget (c);

	-- c->armFuse;

/*
 *  Re-orient the body of the missile towards it's intended target.
 */

	c->prevSg = c->Sg;

	rho = calcRho (-(c->Sg.z), &q);

/*
 *  Compute the resultant force vector on the missile.
 */

	Vmag = mag(c->Cg);
	q = rho * c->cinfo->wingS * Vmag * Vmag * 0.5;
	FLift = 0.0;
	FDrag = c->cinfo->CDOrigin * q;

	if (mdebug) {
	printf ("rho = %g, FLift = %g, FDrag = %g\n", rho, FLift, FDrag);
	printf ("FThrust = %g\n", c->curThrust);
	}

	F.x = c->curThrust - FDrag;
	F.y = 0.0; 
	F.z = 0.0; 

/*
 *  Now calculate changes in position (Sg) and velocity (Cg).
 */

	if ((c->fuel -= fuelUsed(c)) <= 0.0) {
		if (c->curThrust > 0.0)
			if (mdebug)
			printf ("Missile burnout; velocity = %g fps (%g kts)\n", Vmag,
				FPStoKTS(Vmag));
		c->fuel = 0.0;
		c->curThrust = 0.0;
	}

/*
 *  The missile's trihedral and Itrihedral matrices are managed by 
 *  trackTarget().
 */

	craftToGround (c, &F, &Fg);
	FWeight = c->cinfo->emptyWeight + c->fuel;
	Fg.z += FWeight;

	if (mdebug) {
	    	printf ("v = %g kts, Fg = { %g, %g, %g }\n", FPStoKTS(Vmag),
			Fg.x, Fg.y, Fg.z);
	    	printf ("F = { %g, %g, %g }\n", F.x, F.y, F.z);
	}



/*
 *  Update the missile's position and velocity.
 */


	c->Sg.x += c->Cg.x * deltaT + Fg.x / FWeight * a * halfDeltaTSquared;
	c->Sg.y += c->Cg.y * deltaT + Fg.y / FWeight * a * halfDeltaTSquared;
	c->Sg.z += c->Cg.z * deltaT + Fg.z / FWeight * a * halfDeltaTSquared;

	c->Cg.x += Fg.x / FWeight * a * deltaT;
	c->Cg.y += Fg.y / FWeight * a * deltaT;
	c->Cg.z += Fg.z / FWeight * a * deltaT;


	if (mdebug) {
		printf ("Altitude = %g\n", -c->Sg.z);
		printf ("Euler angles { %g, %g, %g }\n", RADtoDEG(c->curRoll),
			RADtoDEG(c->curPitch), RADtoDEG(c->curHeading));
		printf ("Cg = { %g, %g, %g }\n", c->Cg.x, c->Cg.y, c->Cg.z);
		printf ("Sg = { %g, %g, %g }\n", c->Sg.x, c->Sg.y, c->Sg.z);
	}

	return 0;
}
