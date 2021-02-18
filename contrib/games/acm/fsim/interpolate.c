#include "interpolate.h"
#include <stdio.h>

/*
 *  interpolate :  determine the value of a function of one variable
 *		   by interpolation.  Interpolation tables are built by
 *		   the ibuild utility.
 */

static char *ierrmsg = "interpolate: x value is out of bounds\n";

float_t interpolate (table, x)
ITable	*table;
double	x; {

	register int i, count = table->count;

	if (x < table->minX) {
		fprintf (stderr, ierrmsg);
		return (float_t) I_NaN;
	}

	for (i=0; i<count; ++i) {
	    if (x <= table->entry[i].x)
		return (float_t)(table->entry[i].m * x + table->entry[i].b);
	}

	fprintf (stderr, ierrmsg);

	return (float_t) I_NaN;
}
