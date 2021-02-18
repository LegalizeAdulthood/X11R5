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

#include <string.h>
#include "pm.h"

/*
 *  countOrdinance : count a particular type of ordinance on an
 *		aircraft.
 */

int countOrdinance (c, type)
craft	*c;
int	type; {

	register int i, count = 0;

	for (i=0; i<c->cinfo->sCount; ++i) {
		if (c->station[i].type == type)
			++count;
	}

	return count;
}

/*
 *  readyStation:  return the index of the first station with
 *		   a particular type of ordinance.
 */

int readyStation (c, type)
craft	*c;
int	type; {

	register int i;

	for (i=0; i<c->cinfo->sCount; ++i) {
		if (c->station[i].type == type)
			return i;
	}

	return -1;
}

int selectWeapon(c)
craft *c; {

	register int n, m;

	m = c->curWeapon;
	n = (c->curWeapon + 1) % WEAPONTYPES;
	for (; n != m; n = (n + 1) % WEAPONTYPES) {
		if (wtbl[n].select != NULL)
			if ((*wtbl[n].select)(c) == 1) {
				c->curWeapon = n;
				return 1;
			}
	}

	return 0;
}

int fireWeapon (c)
craft *c; {

	if (wtbl[c->curWeapon].firePress != NULL)
		return (*wtbl[c->curWeapon].firePress)(c);

	return 0;
}

int ceaseFireWeapon (c)
craft *c; {

	if (wtbl[c->curWeapon].fireRelease != NULL)
		return (*wtbl[c->curWeapon].fireRelease)(c);

	return 0;
}

int doWeaponDisplay (c, u)
craft *c;
viewer *u; {

	int	i;

	if (wtbl[c->curWeapon].display != NULL)
		return (*wtbl[c->curWeapon].display)(c, wtbl[c->curWeapon].w, u);
	else {
		for (i=0; i<3; i++)
			strcpy (c->leftHUD[i], "");
	}

	return 0;
}

int doWeaponUpdate (c)
craft *c; {

	if (wtbl[c->curWeapon].update != NULL)
		return (*wtbl[c->curWeapon].update)(c);

	return 0;
}
