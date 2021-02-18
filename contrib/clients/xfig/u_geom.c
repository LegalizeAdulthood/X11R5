/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 *
 * "Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty."
 *
 */

/*
 * Routines dealing with geometry under the following headings:
 *	COMPUTE NORMAL, CLOSE TO VECTOR, COMPUTE ARC CENTER,
 *	COMPUTE ANGLE, COMPUTE DIRECTION, LATEX LINE ROUTINES.
 */

#include "fig.h"
#include "object.h"
#include "math.h"

#define			PI		3.14159

/*************************** COMPUTE NORMAL **********************

Input arguments :
        (x1,y1)(x2,y2) : the vector
        direction : direction of the normal vector to (x1,y1)(x2,y2)
Output arguments :
        (*x,*y)(x2,y2) : a normal vector.
Return value : none

******************************************************************/

compute_normal(x1, y1, x2, y2, direction, x, y)
    float           x1, y1;
    int             x2, y2, direction, *x, *y;
{
    if (direction) {		/* counter clockwise  */
	*x = round(x2 - (y2 - y1));
	*y = round(y2 - (x1 - x2));
    } else {
	*x = round(x2 + (y2 - y1));
	*y = round(y2 + (x1 - x2));
    }
}

/******************** CLOSE TO VECTOR **************************

Input arguments:
        (x1,y1)(x2,y2) : the vector
        (xp,yp) : the point
        d : tolerance (max. allowable distance from the point to the vector)
        dd : d * d
Output arguments:
        (*px,*py) : a point on the vector which is not far from (xp,yp)
                by more than d. Normally the vector (*px,*py)(xp,yp)
                is normal to vector (x1,y1)(x2,y2) except when (xp,yp)
                is within d from (x1,y1) or (x2,y2), in which cases,
                (*px,*py) = (x1,y1) or (x2,y2) respectively.
Return value :
        0 : No point on the vector is within d from (xp, yp)
        1 : (*px, *py) is such a point.

******************************************************************/

close_to_vector(x1, y1, x2, y2, xp, yp, d, dd, px, py)
    int             x1, y1, x2, y2, xp, yp, d;
    float           dd;
    int            *px, *py;
{
    int             xmin, ymin, xmax, ymax;
    float           x, y, slope, D2, dx, dy;

    if (abs(xp - x1) <= d && abs(yp - y1) <= d) {
	*px = x1;
	*py = y1;
	return (1);
    }
    if (abs(xp - x2) <= d && abs(yp - y2) <= d) {
	*px = x2;
	*py = y2;
	return (1);
    }
    if (x1 < x2) {
	xmin = x1 - d;
	xmax = x2 + d;
    } else {
	xmin = x2 - d;
	xmax = x1 + d;
    }
    if (xp < xmin || xmax < xp)
	return (0);

    if (y1 < y2) {
	ymin = y1 - d;
	ymax = y2 + d;
    } else {
	ymin = y2 - d;
	ymax = y1 + d;
    }
    if (yp < ymin || ymax < yp)
	return (0);

    if (x2 == x1) {
	x = x1;
	y = yp;
    } else if (y1 == y2) {
	x = xp;
	y = y1;
    } else {
	slope = ((float) (x2 - x1)) / ((float) (y2 - y1));
	y = (slope * (xp - x1 + slope * y1) + yp) / (1 + slope * slope);
	x = ((float) x1) + slope * (y - y1);
    }
    dx = ((float) xp) - x;
    dy = ((float) yp) - y;
    D2 = dx * dx + dy * dy;
    if (D2 < dd) {
	*px = (int) (x + .5);
	*py = (int) (y + .5);
	return (1);
    }
    return (0);
}

/********************* COMPUTE ARC CENTER ******************

Input arguments :
        p1, p2, p3 : 3 points on the arc
Output arguments :
        (*x,*y) : Center of the arc
Return value :
        0 : if p1, p2, p3 are co-linear.
        1 : if they are not.

*************************************************************/

int
compute_arccenter(p1, p2, p3, x, y)
    F_pos           p1, p2, p3;
    float          *x, *y;
{
    float           s12, s13, len1, len2, len3, dx12, dy12, dx13, dy13;

    if (p1.x == p3.x && p1.y == p3.y)
	return 0;

    dx12 = p1.x - p2.x;
    dy12 = p1.y - p2.y;
    dx13 = p1.x - p3.x;
    dy13 = p1.y - p3.y;

    s12 = asin((double) (dy12 / sqrt((double) (dx12 * dx12 + dy12 * dy12))));
    s13 = asin((double) (dy13 / sqrt((double) (dx13 * dx13 + dy13 * dy13))));
    if (fabs(s12 - s13) < .01)
	return 0;

    len1 = p1.x * p1.x + p1.y * p1.y;
    len2 = p2.x * p2.x + p2.y * p2.y;
    len3 = p3.x * p3.x + p3.y * p3.y;
    *y = (dx12 * (len3 - len1) - dx13 * (len2 - len1)) /
	(2 * (dx13 * dy12 - dx12 * dy13));
    if (p1.x != p3.x)
	*x = (len3 + 2 * (*y) * dy13 - len1) / (2 * (-dx13));
    else
	*x = (len2 + 2 * (*y) * dy12 - len1) / (2 * (-dx12));
    return 1;
}

/********************* COMPUTE ANGLE ************************

Input arguments :
        (dx,dy) : the vector (0,0)(dx,dy)
Output arguments : none
Return value : the angle of the vector in the range [0, 2PI)

*************************************************************/

float
compute_angle(dx, dy)		/* compute the angle between 0 to 2PI  */
    float           dx, dy;
{
    float           alpha;

    if (dx == 0) {
	if (dy > 0)
	    alpha = PI / 2;
	else
	    alpha = 3 * PI / 2;
    } else if (dy == 0) {
	if (dx > 0)
	    alpha = 0;
	else
	    alpha = PI;
    } else {
	alpha = atan((double) (dy / dx));	/* range = -PI/2 to PI/2 */
	if (dx < 0)
	    alpha += PI;
	else if (dy < 0)
	    alpha += 2 * PI;
    }
    return (alpha);
}


/********************* COMPUTE DIRECTION ********************

Input arguments :
        p1, p2, p3 : 3 points of an arc with p1 the first and p3 the last.
Output arguments : none
Return value :
        0 : if the arc passes p1, p2 and p3 (in that order) in
                clockwise direction
        1 : if direction is counterclockwise

*************************************************************/

int
compute_direction(p1, p2, p3)
    F_pos           p1, p2, p3;
{
    float           diff, dx, dy, alpha, theta;

    dx = p2.x - p1.x;
    dy = p1.y - p2.y;		/* because origin of the screen is on the
				 * upper left corner */

    alpha = compute_angle(dx, dy);

    dx = p3.x - p2.x;
    dy = p2.y - p3.y;
    theta = compute_angle(dx, dy);

    diff = theta - alpha;
    if ((0 < diff && diff < PI) || diff < -PI) {
	return (1);		/* counterclockwise */
    }
    return (0);			/* clockwise */
}

/*********************** LATEX LINE ROUTINES ***************************/

int
pgcd(a, b)
    int             a, b;

/*
 * compute greatest common divisor, assuming 0 < a <= b
 */
{
    b = b % a;
    return (b) ? gcd(b, a) : a;
}

int
gcd(a, b)
    int             a, b;

/*
 * compute greatest common divisor
 */
{
    if (a < 0)
	a = -a;
    if (b < 0)
	b = -b;
    return (a <= b) ? pgcd(a, b) : pgcd(b, a);
}


int
lcm(a, b)
    int             a, b;

/*
 * compute least common multiple
 */
{
    return abs(a * b) / gcd(a, b);
}


double          rad2deg = 57.295779513082320877;

struct angle_table {
    int             x, y;
    double          angle;
};

struct angle_table line_angles[25] =
{{0, 1, 90.0},
{1, 0, 0.0},
{1, 1, 45.0},
{1, 2, 63.434948822922010648},
{1, 3, 71.565051177077989351},
{1, 4, 75.963756532073521417},
{1, 5, 78.690067525979786913},
{1, 6, 80.537677791974382609},
{2, 1, 26.565051177077989351},
{2, 3, 56.309932474020213086},
{2, 5, 68.198590513648188229},
{3, 1, 18.434948822922010648},
{3, 2, 33.690067525979786913},
{3, 4, 53.130102354155978703},
{3, 5, 59.036243467926478582},
{4, 1, 14.036243467926478588},
{4, 3, 36.869897645844021297},
{4, 5, 51.340191745909909396},
{5, 1, 11.309932474020213086},
{5, 2, 21.801409486351811770},
{5, 3, 30.963756532073521417},
{5, 4, 38.659808254090090604},
{5, 6, 50.194428907734805993},
{6, 1, 9.4623222080256173906},
{6, 5, 39.805571092265194006}
};

struct angle_table arrow_angles[13] =
{{0, 1, 90.0},
{1, 0, 0.0},
{1, 1, 45.0},
{1, 2, 63.434948822922010648},
{1, 3, 71.565051177077989351},
{1, 4, 75.963756532073521417},
{2, 1, 26.565051177077989351},
{2, 3, 56.309932474020213086},
{3, 1, 18.434948822922010648},
{3, 2, 33.690067525979786913},
{3, 4, 53.130102354155978703},
{4, 1, 14.036243467926478588},
{4, 3, 36.869897645844021297},
};

get_slope(dx, dy, sxp, syp, arrow)
    int             dx, dy, *sxp, *syp, arrow;
{
    double          angle;
    int             i, s, max;
    double          d, d1;
    struct angle_table *st;

    if (dx == 0) {
	*sxp = 0;
	*syp = signof(dy);
	return;
    }
    angle = atan((double) abs(dy) / (double) abs(dx)) * rad2deg;
    if (arrow) {
	st = arrow_angles;
	max = 13;
    } else {
	st = line_angles;
	max = 25;
    }
    s = 0;
    d = 9.9e9;
    for (i = 0; i < max; i++) {
	d1 = fabs(angle - st[i].angle);
	if (d1 < d) {
	    s = i;
	    d = d1;
	}
    }
    *sxp = st[s].x;
    if (dx < 0)
	*sxp = -*sxp;
    *syp = st[s].y;
    if (dy < 0)
	*syp = -*syp;
}

latex_endpoint(x1, y1, x2, y2, xout, yout, arrow, magnet)
    int             x1, y1, x2, y2;
    int            *xout, *yout;
    int             arrow, magnet;
{
    int             dx, dy, sx, sy, ds, dsx, dsy;

    dx = x2 - x1;
    dy = y2 - y1;
    get_slope(dx, dy, &sx, &sy, arrow);
    if (abs(sx) >= abs(sy)) {
	ds = lcm(sx, magnet * gcd(sx, magnet));
	dsx = (2 * abs(dx) / ds + 1) / 2;
	dsx = (dx >= 0) ? dsx * ds : -dsx * ds;
	*xout = x1 + dsx;
	*yout = y1 + dsx * sy / sx;
    } else {
	ds = lcm(sy, magnet * gcd(sy, magnet));
	dsy = (2 * abs(dy) / ds + 1) / 2;
	dsy = (dy >= 0) ? dsy * ds : -dsy * ds;
	*yout = y1 + dsy;
	*xout = x1 + dsy * sx / sy;
    }
}
