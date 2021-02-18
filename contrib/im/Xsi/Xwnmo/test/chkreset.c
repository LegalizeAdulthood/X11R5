/*
 * $Id: chkreset.c,v 1.1 1991/09/11 06:48:21 ohm Exp $
 */
/*
 * X11R5 Input Method Test Program
 * ximtest.c v 1.0   Mon Mar 18 15:05:15 JST 1991
 */

/*
 * Copyright 1991 by OMRON Corporation
 *
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include <stdio.h>
#include "exvalue.h"

FLAG	preset(ic, mode)		/* Preedit area RESET */
XIC	ic;
FLAG	mode;
{
    if (mode == ONSP) {
	prerrfunc();
	return (COMP);
    } else {
	cls(prdisp);
	prstatus("Test of XmbResetIC()...");
	XmbResetIC(ic);
	prstatus("done.");
	prprint("Check your own eyes if preedit area on the test window is clear.\n\n");
	prprint("And re-input string.\n\n");
	fprintf(icfp, "...XmbResetIC returned.\n\n");
	return (COMP);
    }
}



