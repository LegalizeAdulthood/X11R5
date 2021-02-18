/*
 * $Id: chkcset.c,v 1.1 1991/09/11 06:48:14 ohm Exp $
 */
/*
 * X11R5 Input Method Test Program
 * chkcset.c v 1.0   Tue Mar 12 20:35:00 JST 1991
 */

/*
 * Copyright 1991 by OMRON Corporation
 *
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include "exvalue.h"
#include "func.h"

FLAG	xsuicf(ic, mode)	/* XSet/Unset IC Focus */
XIC	ic;
FLAG	mode;
{
    cls(prdisp);
    prstatus("Test of XSet/UnsetICFocus()...");
    switch (focuson) {
      case OK:
	XUnsetICFocus(ic);
	focuson = NG;
	prprint("The IC Focus is unset by calling XUnsetICFocus.\n\n");
	prprint("Check your own eyes.\n\n");
	prprint("If you want to set IC,\n");
 	prprint("you have to click your mouse ");
	prprint("\"Focus1\" on menu window again.\n\n");
	cwcolor(ufname, pixel[1][1]);
	break;
      default:
	XSetICFocus(ic);
	focuson = OK;
       	prprint("The IC Focus is set by calling XSetICFocus.\n\n");
     	prprint("Check your own eyes.\n\n");
   	prprint("If you want to unset IC,\n");
	prprint("you have to click your mouse ");
	prprint("\"Focus1\" on menu window again.\n\n");
	cwcolor(ufname, pixel[1][0]);
	break;
    }
    prstatus("done.");
    prprint("Move next test.\n\n");
    prpok();
    return (COMP);
}

FLAG	sfocus(ic, mode)	/* Set FOCUS window */
XIC	ic;
FLAG	mode;
{
    Window	gfocus;
    VALUABLE	*focus;
    int		res;

    if ((mode != DUMMY) && (focuson == OK)) {
	cls(prdisp);
	prstatus("Test of XSetICValues() for change \"Focus Window\"...");
	if (ufocus == window1.win) {
	    cwcolor(ufname, pixel[1][1]);
	    ufocus = window2.win;
	    ufname = window2.name;
	    cwcolor(ufname, pixel[1][0]);
	}
	else if (ufocus == window2.win) {
	    cwcolor(ufname, pixel[1][1]);
	    ufocus = window3.win;
	    ufname = window3.name;
	    cwcolor(ufname, pixel[1][0]);
	}
	else if (ufocus == window3.win) {
	    cwcolor(ufname, pixel[1][1]);
	    ufocus = window1.win;
	    ufname = window1.name;
	    cwcolor(ufname, pixel[1][0]);
	}
	XSetICValues(ic, XNFocusWindow, ufocus, NULL);
	prstatus("done.");
	prstatus("Test of XGetICValues() \"FocusWindow\"...");
	XGetICValues(ic, XNFocusWindow, &gfocus, NULL);
	prstatus("done.");
	res = 0;
	focus = mkstruct(FW, HEX, ufocus, gfocus);
	res = verval(focus);
	cls(prdisp);
	prveres(res);
	if (res != NOERR) {
	    prprint("Test of XSetICValues() \"FocusWindow\" is failed.\n");
	    prprint("FocusWindow has not changed.\n\n");
	    prprint("I will destroy IC and continue test.\n");
	    prprint("Check log file after this test.\n\n");
	    fprintf(icfp, "...Test of XSetICValues() \"FocusWindow\" is failed.\n\n");
	    prpng();
	    return (DIC);
	}
	prprint("Test of XSetICValues() \"FocusWindow\" is succeeded.\n");
	prprint("FocusWindow has changed.\n\n");
	prprint("Check your own eyes and move next test.\n\n");
	fprintf(icfp, "...Test of XSetICValues() \"FocusWindow\" is succeeded.\n\n");
	prpok();
	return (COMP);
    } else {
	cls(prdisp);
	prprint("You can't check this test.\n\n");
	prprint("Retry after \"Focus1\" test.\n\n");
	fprintf(icfp, "...Retry after \"Focus1\" test.\n\n");
    }
    return(COMP);
}


FLAG	sresource(ic, mode)	/* Set RESOURCE name/class */
XIC	ic;
FLAG	mode;
{
    int 	resn, resc, sum;
    int		cnt, res;
    RESOURCE	gres;
    VALUABLE	*resource[ARRAY_MAX];

    if (mode == ONSP) {
	prerrfunc();
	return (COMP);
    } else {
	cls(prdisp);
	prstatus("Test of XSetICValues() for change \"Resource Name\",\"Resource Class\"...");
	if (ures.name == "xim")
	    ures.name = "xima";
	else if (ures.name == "xima")
	    ures.name = "ximb";
	else if (ures.name == "ximb")
	    ures.name = "xim";
	if (ures.class == "Xim")
	    ures.class = "Xima";
	else if (ures.class == "Xima")
	    ures.class = "Ximb";
	else if (ures.class == "Ximb")
	    ures.class = "Xim";
	XSetICValues(ic, XNResourceName, ures.name,
	                 XNResourceClass, ures.class,
		     NULL);
	prstatus("done.");
	prstatus("Test of XGetICValues() \"resource name/class\"...");
	XGetICValues(ic, XNResourceName, &gres.name,
 		         XNResourceClass, &gres.class,
		     NULL);
	prstatus("done.");
	sum = 0;
	resource[0] = mkstruct(RN, STR, ures.name, gres.name);
	resource[1] = mkstruct(RC, STR, ures.class, gres.class);
        for (cnt = 0; cnt < CHKRES; cnt++) {
	    res = verval(resource[cnt]);
	    sum += res;
	}
	cls(prdisp);
	prveres(sum);
	if (sum != NOERR) {
	    prprint("Test of XSetICValues() \"ResourceName/Class\" is failed.\n");
	    prprint("ResourceName/ResourceClass has not changed.\n\n");
	    prprint("I will destroy IC and continue test.\n");
	    prprint("Check log file after this test.\n\n");
	    fprintf(icfp, "...Test of XSetICValues() \"ResourceName/Class\" is failed.\n\n");
	    prpng();
	    return (DIC);
	}
	prprint("Test of XSetICValues() \"ResourceName/Class\" is succeeded.\n");
	prprint("ResourceName/ResourceClass has changed.\n\n");
	prprint("Check your own eyes and move next test\n\n");
	fprintf(icfp, "...Test of XSetICValues() \"ResourceName/Class\" is succeeded.\n\n");
	prpok();
	return (COMP);
    }
}
