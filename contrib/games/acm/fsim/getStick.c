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
#include <math.h>

/*
 *  getStick :  get stick input from mouse
 *		inputs Sa and Se range from -1.0 to 1.0.
 */

int getStick (c, u)
craft *c;
viewer *u; {

	int	rootX, rootY, x, y;
	Window	root, child;
	unsigned int mask;
	double	d, fuzz;

	if (XQueryPointer (u->dpy, u->win, &root, &child, &rootX, &rootY,
		&x, &y, &mask) == True) {

		if (x >= 0 && y >= 0 && x < u->width && y < u->height) {

			fuzz = u->scaleFactor * 20.0;
			x = x - u->xCenter;
			y = y - u->yCenter;
			d = sqrt ((double)(x*x + y*y));

			if (d > fuzz) {
				c->Sa=(double) x / (double) u->xCenter * (d-fuzz) / d;
				c->Se=(double) y / (double) u->yCenter * (d-fuzz) / d;
#ifndef LINEAR_CONTROL_RESPONSE
				if (c->Sa < 0.0)
					c->Sa =  - c->Sa * c->Sa;
				else
					c->Sa = c->Sa * c->Sa;

				if (c->Se < 0.0)
					c->Se = - c->Se * c->Se;
				else
					c->Se = c->Se * c->Se;
#endif
			}
			else {
				c->Sa = 0.0;
				c->Se = 0.0;
			}
			return 1;

		}
	}

	return 0;
}
