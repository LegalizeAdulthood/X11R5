/*--------------------------- XOColorNames.c ---------------------------*/

/*
 *  X-based color name manipulation routines
 *
 *  Author:  Miles O'Neal
 *
 *  Release: 1.0 meo - FCS release
 *
 *  COPYRIGHT 1990, 1991, Systems & Software Solutions, Inc, Marietta, GA
 *
 *  Permission is hereby granted to use, copy, modify and/or distribute
 *  this software and supporting documentation, for any purpose and
 *  without fee, as long as this copyright notice and disclaimer are
 *  included in all such copies and derived works, and as long as
 *  neither the author's nor the copyright holder's name are used
 *  in publicity or distribution of said software and/or documentation
 *  without prior, written permission.
 *
 *  This software is presented as is, with no warranties expressed or
 *  implied, including implied warranties of merchantability and
 *  fitness. In no event shall the author be liable for any special,
 *  direct, indirect or consequential damages whatsoever resulting
 *  from loss of use, data or profits, whether in an action of
 *  contract, negligence or other tortious action, arising out of
 *  or in connection with the use or performance of this software.
 *  In other words, you are on your own. If you don't like it, don't
 *  use it!
*/
static char _SSS_XOColorNames_copyright[] =
    "XOColorNames 1.0 Copyright 1990, 1991, Systems & Software Solutions, Inc, Marietta, GA";

#include <stdio.h>
#include <ctype.h>

#include "Intrinsic.h"

#include "libXO.h"

static void BubbleSort();

/*
 *  XONormalizeColorName - convert color names to a standard format
 *
 *  Convert leading chars of words to upper case.
 *  Get rid of white space.
 */
int
XONormalizeColorName(color)

char *color;		/* unnormalized color name */
{
    register char *p1, *p2;	/* snickers to Ray[0] */
    register int new = 1;

    for (p1 = color, p2 = color; *p1; p2++) {
	if (new) {
	    if (*p2 == ' ' || *p2 == '\t') {
				/* skip more white space */
	    } else if (islower(*p2)) {
		*p1++ = toupper(*p2);	/* cvt to UC */
	    } else {
		*p1++ = *p2;		/* just copy it */
	    }
	    new = 0;
	} else if (*p2 == ' ' || *p2 == '\t') {
		new = 1;	/* skip white space, new word starts */
	} else {
	    *p1++ = *p2;	/* copy rest of word */
	}
    }
    return (p1 - color);
}


/*
 *  XOGetStdColorNames() - get the standard color names for the server
 *
 *  Read the colors in from the file. Get no more than MAXCOLORS,
 *  and skip anything that starts with gr[ae]y (gray && grey are
 *  not skipped - gray1, etc are) if showgray is not set. Sort the
 *  colors alphabetically by name, skipping redundant names. Copy
 *  the colors into the array <colors>.
 */

int
XOGetStdColorNames(colors, maxcolors, maxlen, showgray, namefile)

char *colors[];		/* array to hold normalized color names */
int maxcolors;		/* # of elements in colors */
int *maxlen;		/* max # of chars in a normalized name */
int showgray;		/* 1 if grays wanted, else 0 */
char *namefile;		/* file from which to read color names */
{
    register int i, n;
    char buf[81], *p;
    FILE *fp, *fopen();
    int ncolors = 0;

    *maxlen = 0;
    n = 0;
    if (namefile && *namefile && (fp = fopen(namefile, "r"))) {
	while (fgets(buf, 80, fp)) {
	    for (i = 0; !isalpha(buf[i]); i++) {
		/* skip to start of color name */
	    }
	    p = buf + i;
	    buf[strlen(buf)-1] = NULL;

	    /* skip grays unless they are asked for */

	    if (!showgray && (!strncmp(p, "gray", 4) ||
		!strncmp(p, "grey", 4) && strlen(p) > 4)) {
		    continue;
	    } else {

	    /* normalize the color & malloc space for the copy */

		n = XONormalizeColorName(p);
		if (n > *maxlen)
		    *maxlen = n;
		if (!(colors[ncolors] = XtMalloc(n + 1))) {
		    XtError("no more memory available");
		    /* EXITS */
		}

		/* skip duplicate color names, including Gr[ae]y<N> */

		for (i = 0; i < ncolors; i++) {
		    if (!strcmp(colors[i], p))
			break;
		    if (!strncmp(colors[i], "Gr", 2) && (colors[i][2] == 'a' ||
			colors[i][2] == 'e') && !strcmp(&colors[i][3], &p[3]))
			    break;
		}

		/* if under the limit, copy the color, otherwise yell & split */

		if (i == ncolors)
		    strcpy(colors[ncolors++], p);
		if (ncolors == maxcolors) {
		    XtWarning("extra colors not displayed");
		    break;
		}
	    }
	}
	fclose(fp);
	BubbleSort(colors, ncolors);
    }
    return ncolors;
}


/*
	Quick N Dirty string sort
*/
static void
BubbleSort(data, num)
char *data[];
int num;
{
    register int i, j;
    register char *t;		/* temp for swap */

    for (i = 0; i < num; i++) {
	for (j = i; j < num; j++) {
	    if (strcmp(data[i], data[j]) > 0) {
		t = data[i];
		data[i] = data[j];
		data[j] = t;
	    }
	}
    }
}
