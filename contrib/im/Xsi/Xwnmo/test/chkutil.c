/*
 * $Id: chkutil.c,v 1.1 1991/09/11 06:48:22 ohm Exp $
 */
/*
 * X11R5 Input Method Test Program
 * chkutil.c v 1.0   Thu Mar  7 18:27:37 JST 1991
 */

/*
 * Copyright 1991 by OMRON Corporation
 *
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include <stdio.h>
#include "exvalue.h"

void	chkutil(im, ic, locale)	/* CHecK UTILity funcions */
XIM	im;
XIC	ic;
char	*locale;
{
    XIM		retim;
    Display	*retdpy;
    char	*retlocale;
    char	tmplocale[MAX_BUF];
    char	*cnt;
    
    cls(prdisp);		/* clear screen */

    prstatus("Test of XIMOfIC()..."); /* test of XIMOfIC() */
    retim = XIMOfIC(ic);
    prstatus("done.");
    if (retim != im) {
	prprint("Test of XIMOfIC() is failed.\n\n");
	fprintf(icfp, "Test of XIMOfIC() is failed.\n");
	fprintf(icfp, "id of im : 0x%-10X, id of retim : 0x%-10X\n\n", im, retim);
    } else {
	prprint("Test of XIMOfIC() is succeeded.\n\n");
	fprintf(icfp, "Test of XIMOfIC() is succeeded.\n");
	fprintf(icfp, "id of im : 0x%-10X, id of returned ic : 0x%-10X\n\n", im, retim);
    }

    prstatus("Test of XDisplayOfIM()..."); /* test of XDisplayOfIM() */
    retdpy = XDisplayOfIM(im);
    prstatus("done.");
    if (retdpy != dpy) {
	prprint("Test of XDisplayOfIM() is failed.\n\n");
	fprintf(icfp, "Test of XDisplayOfIM() is failed.\n\n");
	fprintf(icfp, "id of Display : 0x%-10X, id of retuened Display : 0x%-10X\n\n", dpy, retdpy);
    } else {
	prprint("Test of XDisplayOfIM() is succeeded.\n\n");
	fprintf(icfp, "Test of XDisplayOfIM() is succeeded.\n");
	fprintf(icfp, "id of Display : 0x%-10X, id of returned Display : 0x%-10X\n\n", dpy, retdpy);
    }

    prstatus("Test of XLocaleOfIM()..."); /* test of XLocaleOfIM() */
    strcpy(tmplocale, locale);
    for (cnt = tmplocale; (*cnt != '\0') && (*cnt != '@'); cnt++)
	;
    *cnt = '\0';
    retlocale = XLocaleOfIM(im);
    prstatus("done.");
    if (strcmp(retlocale, tmplocale)) {
	prprint("Test of XLocaleOfIM() is failed.\n\n");
	fprintf(icfp, "Test of XLocaleOfIM() is failed.\n");
	fprintf(icfp, "name of locale : %s, name of retuened locale : %s\n\n", tmplocale, retlocale);
    } else {
	prprint("Test of XLocaleOfIM() is succeeded.\n\n");
	fprintf(icfp, "Test of XLocaleOfIM() is succeeded.\n");
	fprintf(icfp, "name of locale : %s, name of retruned locale : %s\n\n", tmplocale, retlocale);
    }
    prpok();
}




