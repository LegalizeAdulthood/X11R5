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

#define	GM	1.4		/* ratio of specific heats for air */
#define P0	2116.22		/* sea-level pressure */
#define TA	459.67		/* CONVERSION TEMPERATURE FROM F TO R */
#define T0	(TA + 59.0)	/* sea-level temperature */
#define	G	32.1741		/* gravitational acceleration [ft/sec^2] */
#define RU	(1545.31 * G)	/* universal gas constant */
#define MA	28.9644		/* molecular weight for air [lbm/lbm-mole] */
#define	RA	(RU / MA)	/* gas constant for air [ft^2/sec^2/R] */

/*
 *  airProperties :  Compute rho and the speed of sound for the standard
 *		atmosphere up to 60,000 feet.
 */

void airProperties (h, rho, mach1)
double	h;
double	*rho, *mach1; {

	double	t, p;

	if (h < 36089.2) {
		t = T0 - 3.56616e-3 * h;
		p = P0 * pow (t / T0, 5.255912);
	}
	else {
		t = 389.97;
		p = 472.6773 * exp (- G * (h - 36089.2) / (RA * t));
	}

	*rho = p / (RA * t);
	*mach1 = sqrt (GM * RA * t);
}
