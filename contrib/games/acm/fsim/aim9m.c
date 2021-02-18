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

int select_aim9m();
int display_aim9m();
int update_aim9m();
int fire_aim9m();
int getIRTarget();
extern int fireMissile ();
extern void createMissileEyeSpace();
extern FILE * acm_fopen ();
extern int countOrdinance();
extern int readyStation();

weaponDesc aim9mDesc = {
	WK_AIM9M,
	select_aim9m,		/* select */
	update_aim9m,		/* update */
	display_aim9m,		/* display procedure */
	fire_aim9m,		/* fire */
	(int (*)()) NULL,	/* fire button release */
	};

int	hasFired[MAXPLAYERS];
static	int count[MAXPLAYERS];

/*
 *  aim9m selection function
 *
 *  A selection function normally determines whether there are any weapons
 *  of this type on-board.  If so, and the weapon system is functional
 *  (in other words, undamaged) then return 1; otherwise return 0.
 */

/*ARGSUSED*/
int select_aim9m (c)
craft *c; {

	hasFired[c->index] = 0;
	count[c->index] = countOrdinance(c, WK_AIM9M);
	return 1;

}

/*ARGSUSED*/
int update_aim9m (c)
craft *c; {

	register int i;

	if (hasFired[c->index] && count[c->index] > 0) {
		i = readyStation (c, WK_AIM9M);
		if (i < 0)
			fprintf (stderr, "Oops. Can't find an AIM-9\n");
		else {
			c->station[i].type = 0;
			fireMissile (c, i);
			count[c->index] --;
		}
		hasFired[c->index] = 0;
	}
}

/*ARGSUSED*/
int fire_aim9m (c)
craft *c; {

	hasFired[c->index] = 1;
}

/*
 * aim9m display function
 *
 *  Update the HUD display strings associated with this weapon system.
 */

/*ARGSUSED*/
int display_aim9m (c, w, u)
craft *c;
craftType *w;
viewer *u; {

	char	s[16];
	double	d, a1, v, r, root1, root2, n, t;
	VPoint	tmp;
	VMatrix	m;
	int	target;

	sprintf (s, "%d AIM-9M", count[c->index]);
	strcpy (c->leftHUD[1], s);

	v = mag (c->Cg);
	a1 = (w->maxThrust - 0.5 * c->rho * w->CDOrigin * v * v)
		/ (w->emptyWeight + w->maxFuel) * a;

	if (c->curRadarTarget >= 0 && a1 >= 0.0) {

	d = c->targetDistance;
	r = c->targetClosure;

	n = 4.0 * d / a1 + (r * r) / (a1 * a1);
	if (n > 0) {
		n = sqrt (n);
		root1 = (- r / a + n) / 2.0;
		root2 = (- r / a - n) / 2.0;
		if (root1 >= 0.0)
			if (root2 >= 0.0)
				if (root1 < root2)
					t = root1;
				else
					t = root2;
			else
				t = root1;
		else if (root2 >= 0.0)
			t = root2;
		else
			t = -1.0;
	}
	else
		t = -1.0;
	}

	else
		t = -1.0;

/*
 *  See if the missiles can lock onto any target.
 */

	if (count[c->index] > 0) {
		createMissileEyeSpace (c, &m);
		target = getIRTarget (c, &m, &tmp);
	}
	else
		target = -1;

	if (target >= 0 && t <= 15.0)
		sprintf (s, "LOCKED   %d", (int)(t+0.5));
	else if (t < 0.0)
		sprintf (s, "ARM      --");
	else if (t <= 15.0)
		sprintf (s, "IN RANGE %d", (int)(t+0.5));
	else 
		sprintf (s, "ARM      %d", (int)(t+0.5));

	strcpy (c->leftHUD[0], s);

	strcpy (c->leftHUD[2], "");

	return(0);
}

extern craftType * newCraft ();

void initaim9()
{

	craftType	*c;
	FILE    	*f;

	c = newCraft();
	c->name = strdup("aim-9m");

	wtbl[0] = aim9mDesc;
	wtbl[0].w = c;

	c->CLOrigin = 0.0;
	c->CLSlope = 4.41235;
	c->CLNegStall = -15.0 * pi / 180.0;
	c->CLPosStall =  15.0 * pi / 180.0;

	c->CDOrigin = 0.1963494;	/* 4" radius of body */
	c->CDFactor = -2.56694;
	c->CDPhase = 0.0;

	c->CDBOrigin = 0.0;
	c->CDBFactor = 0.0;
	c->CDBPhase = 0.0;

	VIdentMatrix(&(c->I));
	c->I.m[0][0] =  0.0;
	c->I.m[1][1] = 0.0;
	c->I.m[2][2] = 0.0;
	c->LDamp =  700000.0;
	c->MDamp = 1000000.0;
	c->NDamp = 1000000.0;
	c->cmSlope = -1.88;
	c->cmFactor = -1.00;
	c->betaStall = 15.0 * pi / 180.0;

	c->wingS = 1.0;

/*
 *  Assume 150.0 lbs of weight is fuel and that it burns for 20 seconds.
 *  That yields a fuel burn rate of 7.5 lbs/second.
 */

	c->emptyWeight = 40.0;
	c->maxFuel = 150.0;
	c->maxThrust = 791.0;
	c->spFuelConsump = 34.134;

/*
 *  Three second arming delay
 */

	c->armDelay = 3.0;

	c->groundingPoint.x = 0.0;
	c->groundingPoint.y = 0.0;
	c->groundingPoint.z = 0.0;

	c->viewPoint.x = 0.0;
	c->viewPoint.y = 0.0;
	c->viewPoint.z = 0.0;

	c->crashC = 1.0;

	c->muStatic = 0.0;
	c->muKinetic = 0.0;
	c->muBStatic = 0.0;
	c->muBKinetic = 0.0;

	c->maxNWDef = 0.0;
	c->NWIncr = 0.0;
	c->maxNWS = 0.0;
	c->gearD1 = 0.0;
	c->gearD2 = 0.0;

	f = acm_fopen ("aim-9", "r");
	c->object = VReadObject(f);
	fclose (f);

}
