/*
 * $Id: keyev.c,v 1.1 1991/09/11 06:48:34 ohm Exp $
 */
/*
 * X11R5 Input Method Test Program
 * keyev.c v 1.0   Thu Mar  7 19:30:17 JST 1991
 */

/*
 * Copyright 1991 by OMRON Corporation
 *
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include "exvalue.h"

FLAG	keyev(ic, mode)	/* transaction of KEY EVent */
XIC	ic;
FLAG 	mode;
{
    int		cnt;
    FLAG	res;
    FLAG	funend;

    XEvent	event;

    for(;;) {
	XNextEvent(dpy, &event);
	switch(event.type) {
	  case Expose:
	    dstrwin();
	    break;
	  case KeyPress:
	    if (fltev(&event) == NG)
		xlstr(ic, &event);
	    break;
	  case ButtonPress:
	    funend = NG;
	    for (cnt = 0; (ftbl[cnt].mname != NULL) && (funend != OK); cnt++) {
		if (event.xbutton.window == *ftbl[cnt].mwin) {
		    if (ftbl[cnt].flag == CHMOD) {
			XClearArea(dpy, window1.win, 0, 0, 0, 0, True);
			return (MENU);
		    }
		    else {
			cwcolor(*ftbl[cnt].mwin, pixel[1][0]);
			res = (*ftbl[cnt].func)(ic, mode);
			cwcolor(*ftbl[cnt].mwin, pixel[1][1]);
			if (res == COMP)
			    funend = OK;
			else
			    return (res);
		    }
		}
	    }
	    break;
	}
    }
}























