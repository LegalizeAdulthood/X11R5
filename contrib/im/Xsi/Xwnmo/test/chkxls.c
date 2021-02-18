/*
 * $Id: chkxls.c,v 1.1 1991/09/11 06:48:24 ohm Exp $
 */
/*
 * X11R5 Input Method Test Program
 * chkxis.c v 1.0   Mon Mar 11 14:27:45 JST 1991
 */

/*
 * Copyright 1991 by OMRON Corporation
 *
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include <stdio.h>
#include "exvalue.h"

void	xlstr(ic, event)
XIC	ic;
XEvent	*event;
{
    int		res;
    int		cnt;
    char	buffer[MAX_BUF];
    KeySym	ksret;
    Status	stret;

    res = XmbLookupString(ic, event, buffer, sizeof(buffer), &ksret, &stret);
    cls(prdisp);
    switch (stret) {
      case XLookupBoth:
      case XLookupKeySym:
	prprint("Key Symbol returned.\n");
	fprintf(icfp, "returned Key Symbol : 0x%X\n", ksret);
	if (stret == XLookupKeySym)
	    prprint("\nCheck your own eyes on the test window.\n\n");
	    break;
      case XLookupChars:
	prprint("Character returned.\n");
	prprint("\nCheck your own eyes on the test window.\n\n");
	for (cnt = 0; cnt < res; cnt++) 
	    fprintf(icfp, "returned character code : 0x%X\n", buffer[cnt]);
	fprintf(icfp, "\n");
	break;
      case XLookupNone:
	prprint("returned status : XLookupNone\n\n");
	fprintf(icfp, "returned status : XLookupNone\n\n");
	break;
      case XBufferOverflow:
	prprint("returned status : XBufferOverflow\n\n");
	fprintf(icfp, "returned status : XBufferOverflow\n\n");
	break;
      default:
	prprint("returned status : Unknown Value\n\n");
	fprintf(icfp, "returned status : Unknown Value\n\n");
	break;
    }
}
