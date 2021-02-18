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
#include "damage.h"

extern long random();

double	frandom () {

	return (double) (random() & 0xffff) / 65536.0;
}

/*
 *  Select a subsystem to receive damage.
 */

long	selectSystem () {

	double	r;
	long	i;

	if ((r = frandom()) < 0.25)
		i = SYS_ENGINE1;
	else if (r < 0.35)
		i = SYS_RADAR;
	else if (r < 0.40)
		i = SYS_TEWS;
	else if (r < 0.45)
		i = SYS_HYD1;
	else if (r < 0.50)
		i = SYS_HYD2;
	else if (r < 0.52)
		i = SYS_GEN1;
	else if (r < 0.54)
		i = SYS_GEN2;
	else if (r < 0.59)
		i = SYS_FLAPS;
	else if (r < 0.63)
		i = SYS_SPEEDBRAKE;
	else if (r < 0.75)
		i = SYS_FUEL;
	else if (r < 0.80)
		i = SYS_HUD;
	else if (r < 0.80)
		i = SYS_LEFTMAIN;
	else if (r < 0.85)
		i = SYS_RIGHTMAIN;
	else if (r < 0.95)
		i = SYS_NOSEGEAR;
	return i;
}

void	damageSystem (c, sys)
craft	*c;
long	sys; {

	if ((c->damageBits & sys) || (sys == SYS_FUEL)) {

		c->damageBits &= ~ sys;

		switch (sys) {

		case SYS_ENGINE1:
			c->throttle = 0;
			break;

		case SYS_RADAR:
			c->curRadarTarget = -1;
			break;

/*
 *  Fuel leaks can be up to 40 pounds per second here.
 */

		case SYS_FUEL:
			c->leakRate += (frandom() + frandom()) * 20.0;
			break;
		
		case SYS_HYD1:
		case SYS_HYD2:
			if ((c->damageBits & (SYS_HYD1 | SYS_HYD2)) == 0) {
				c->damageBits &= ~ SYS_SPEEDBRAKE;
				c->damageBits &= ~ SYS_FLAPS;
			}
			break;

		case SYS_GEN1:
		case SYS_GEN2:
			if ((c->damageBits & (SYS_GEN1 | SYS_GEN2)) == 0) {
				c->damageBits &= ~
				    (SYS_HUD | SYS_RADAR | SYS_TEWS);
				break;
			}
		}

	}
}

/*
 * absorbDamage :  craft c is hit with d points of damage.
 */

int	absorbDamage (c, d)
craft	*c;
int	d; {

	double	n, x;
	register long	sys;

	x = frandom() + frandom() + frandom() / 3.0 * 2.0;

	d = d * x + 0.5;

	for (; d > 0; --d) {

/*
 *  For each damage point absorbed, there is a 30 percent chance that
 *  it will be absorbed by some subsystem other than the actual
 *  airframe.
 */

		if (frandom() <= 0.30) {
			sys = selectSystem();
			damageSystem (c, sys);
		}

/*
 *  For each point absorbed by the airframe, there is a 20% chance that
 *  it'll be absorbed by the wing and induce a rolling moment or a 10 
 *  percent chance that it will hit a horizontal stabilizer and induce
 *  a pitching and rolling moment.
 */

		else {

		if ((n = frandom()) <= 0.20) {
			c->damageCL += (frandom() - 0.5) * 0.20;
		}
		else if (n <= 0.30) {
			c->damageCL += (frandom() - 0.5) * 0.10;
			c->damageCM += (frandom() - 0.5) * 0.20;
		}

		if (--(c->structurePts) <= 0)
			return 0;
		}

	}

	return 1;

}

void	initDamage (c)
craft	*c; {

	c->damageBits = c->cinfo->damageBits;
	c->structurePts = c->cinfo->structurePts;
	c->leakRate = 0.0;
	c->damageCL = 0.0;
	c->damageCM = 0.0;
}
