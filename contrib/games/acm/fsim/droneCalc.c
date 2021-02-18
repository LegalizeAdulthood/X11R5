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

/*
 *  Drone flight management
 */

int droneCalculations (c)
craft *c; {

	c->prevSg = c->Sg;

	c->Sg.x += c->Cg.x * deltaT;
	c->Sg.y += c->Cg.y * deltaT;
	c->Sg.z += c->Cg.z * deltaT;

/*
 *  Don't let drones get above 60 thousand feet
 */

	if (c->Sg.z < -60000.0)
		return 1;

/*
 *  Drone crash detection
 */

	if (c->Sg.z > 0.0)
		return 1;

	return 0;
}
