/*
 * Copyright 1990 by Tektronix, Inc., Beaverton, Oregon, and
 * the Massachusetts Institute of Technology, Cambridge, Massachusetts.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Tektronix or M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Tektronix and M.I.T. make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * TEKTRONIX AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL TEKTRONIX OR M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: /usr/local/src/x11r5.mvex/RCS/MVEXRect.c,v 2.0 1991/07/23 02:05:35 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#include "MVEXlib.h"

/*
 *    NAME
 *        MVEXMatchRectangle - is a rectangle within a range?
 *
 *    SYNOPSIS
 */
Bool
MVEXMatchRectangle (given, range, enclosed, enclosing)
    XRectangle *given;		/* in: rectangle to be checked for */
    MVEXRectangleRange *range;	/* in: the range to be checked */
    XRectangle *enclosed;	/* out: largest enclosed rectangle */
    XRectangle *enclosing;	/* out: smallest enclosing rectangle */
/*
 *    DESCRIPTION
 *        Compute the x, y, width and height of the returned rectangles.
 *
 *    RETURNS
 *        TRUE if the given rectangle exactly matches one in the range,
 *        FALSE otherwise.
 */
{
    register int j0;
    int min, max, temp, edge;

/* Get the x */

    if (given->x < range->base.x) {
	enclosed->x = enclosing->x = range->base.x;
    } else if (given->x > range->limit.x) {
	enclosed->x = enclosing->x = range->limit.x;
    } else {
	temp = (given->x - range->base.x) % range->x_inc;
	if (temp == 0) {
	    enclosed->x = enclosing->x = given->x;
	} else {
	    enclosed->x = given->x - temp + range->x_inc;
	    enclosing->x = given->x - temp;
	}
    }

/* Get the y */

    if (given->y < range->base.y) {
	enclosed->y = enclosing->y = range->base.y;
    } else if (given->y > range->limit.y) {
	enclosed->y = enclosing->y = range->limit.y;
    } else {
	temp = (given->y - range->base.y) % range->y_inc;
	if (temp == 0) {
	    enclosed->y = enclosing->y = given->y;
	} else {
	    enclosed->y = given->y - temp + range->y_inc;
	    enclosing->y = given->y - temp;
	}
    }

/* Linear range for width and height */

    if (range->type == MVEXLinearRange) {

/* Get the width (linear) */

	if (given->width < range->base.width) {
	    enclosed->width = enclosing->width = range->base.width;
	} else if (given->width > range->limit.width) {
	    enclosed->width = enclosing->width = range->limit.width;
	} else {
	    temp = (given->width - range->base.width) % range->width_inc;
	    if (temp == 0) {
		enclosed->width = enclosing->width = given->width;
	    } else {
		enclosed->width = given->width - temp;
		enclosing->width = enclosed->width + range->width_inc;
	    }
	    temp = given->x + given->width - enclosed->x;
	    while (enclosed->width > temp) {
		enclosed->width -= range->width_inc;
	    }
	    if (enclosed->width < range->base.width) {
		enclosed->width = range->base.width;
	    }
	    temp = given->x + given->width - enclosing->x;
	    while (enclosing->width < temp) {
		enclosing->width += range->width_inc;
	    }
	    if (enclosing->width > range->limit.width) {
		enclosing->width = range->limit.width;
	    }
	}

/* Get the height (linear) */

	if (given->height < range->base.height) {
	    enclosed->height = enclosing->height = range->base.height;
	} else if (given->height > range->limit.height) {
	    enclosed->height = enclosing->height = range->limit.height;
	} else {
	    temp = (given->height - range->base.height) % range->height_inc;
	    if (temp == 0) {
		enclosed->height = enclosing->height = given->height;
	    } else {
		enclosed->height = given->height - temp;
		enclosing->height = enclosed->height + range->height_inc;
	    }
	    temp = given->y + given->height - enclosed->y;
	    while (enclosed->height > temp) {
		enclosed->height -= range->height_inc;
	    }
	    if (enclosed->height < range->base.height) {
		enclosed->height = range->base.height;
	    }
	    temp = given->y + given->height - enclosing->y;
	    while (enclosing->height < temp) {
		enclosing->height += range->height_inc;
	    }
	    if (enclosing->height > range->limit.height) {
		enclosing->height = range->limit.height;
	    }
	}
    }

/* Geometric range for width and height */
    else {

/* Get the width (geometric) */

	edge = given->x + given->width;
	temp = 1;
	for (j0 = 0; j0 < range->base.width; j0++) {
	    temp *= range->width_inc;
	}
	enclosed->width = enclosing->width = temp;
	while (j0 <= range->limit.width) {
	    if (enclosed->x + temp <= edge) {
		enclosed->width = enclosing->width = temp;
	    }
	    if (enclosing->x + temp >= edge) {
		enclosing->width = temp;
		break;
	    }
	    temp *= range->width_inc;
	    j0++;
	}

/* Get the height (geometric) */

	edge = given->y + given->height;
	temp = 1;
	for (j0 = 0; j0 < range->base.height; j0++) {
	    temp *= range->height_inc;
	}
	enclosed->height = enclosing->height = temp;
	while (j0 <= range->limit.height) {
	    if (enclosed->y + temp <= edge) {
		enclosed->height = enclosing->height = temp;
	    }
	    if (enclosing->y + temp >= edge) {
		enclosing->height = temp;
		break;
	    }
	    temp *= range->height_inc;
	    j0++;
	}
    }

/* See if it is a match (if it is, enclosing == enclosed) */

    if (enclosed->x == given->x && enclosed->y == given->y &&
	enclosed->width == given->width && enclosed->height == given->height) {
	return True;
    } else {
	return False;
    }
}
