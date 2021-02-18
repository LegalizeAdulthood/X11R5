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

/*
 *  resupplyCheck :  If a player is on the airport grounds and motionless,
 *		     then invoke the plane's resupply procedure.
 */

void resupplyCheck () {

	craft	*c;
	int	i;
	double	d, x, y, z;

	for ((i=0, c = &ptbl[0]); i<MAXPLAYERS; (++i, ++c)) {

		if (c->type != CT_PLANE)
			continue;

		if (mag(c->Cg) == 0.0) {
			x = c->Sg.x - teamLoc[c->team].x;
			y = c->Sg.y - teamLoc[c->team].y;
			z = c->Sg.z - teamLoc[c->team].z;
			d = sqrt (x * x + y * y + z * z);
			if (d <= MAX_GROUND_DISTANCE)
				(*c->cinfo->resupply)(c);
			(*wtbl[c->curWeapon].select)(c);
		}
	}
}
