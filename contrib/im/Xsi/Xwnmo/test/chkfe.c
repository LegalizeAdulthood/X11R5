/*
 * $Id: chkfe.c,v 1.1 1991/09/11 06:48:16 ohm Exp $
 */
/*
 * X11R5 Input Method Test Program
 * chkfe.c v 1.0   Fri Mar  8 09:13:21 JST 1991
 */

/*
 * Copyright 1991 by OMRON Corporation
 *
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include 	"exvalue.h"

FLAG	fltev(event)
XEvent	*event;
{
    Bool	res;

/*  prstatus("Calling XFilterEvent()..."); */
    res = XFilterEvent(event, event->xany.window);
/*  prstatus("done."); */
    cls(prdisp);
    if (res == False) {
/*	prprint("The result of XFilterEvent() is \"False\".\n");  */
	fprintf(icfp, "The result of XFilterEvent() is \"False\".\n");
	return (NG);
    } else {
/*	prprint("The result of XFilterEvent() is \"True\".\n");  */
	fprintf(icfp, "The result of XFilterEvent() is \"True\".\n");
    	return (OK);
    }
}

