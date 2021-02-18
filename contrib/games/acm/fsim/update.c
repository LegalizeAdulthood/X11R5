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
 
#include "pm.h"
#include <X11/Xutil.h>

extern int flightCalculations();
extern int missileCalculations();
extern void doEvents(), doViews(), flapControl(), resupplyCheck();

int cur = 0;

int redraw () {

	int	i;
	craft	*p;

	for ((i=0, p=ptbl); i<MAXPLAYERS; (++i, ++p)) {
		if (p->type == CT_PLANE) {
			doEvents (p);
			if (flightCalculations (p) == 1)
				killPlayer (p);
			doWeaponUpdate (p);
			flapControl (p);
		}
		else if (p->type == CT_DRONE)
			if (droneCalculations (p) == 1)
				killPlayer (p);
	}

	for ((i=0, p=mtbl); i<MAXPROJECTILES; (++i, ++p)) {
		if (p->type == CT_MISSILE) {
			if (missileCalculations (p) == 1)
				killMissile (p);
		}
		else if (p->type == CT_CANNON) {
			if (cannonCalculations (p) == 1)
				killMissile (p);
		}
		else if (p->type == CT_EXPLOSION) {
			-- p->flameDuration;
			if ((-- p->duration) <= 0)
				p->type = CT_FREE;
		}
	}

	lookForImpacts ();

	if (cur % REDRAW_EVERY == 0)
		doViews ();

	if (cur++ % RESUPPLY_EVERY == 0)
		resupplyCheck();

	curTime += deltaT;

	return 0;

}
