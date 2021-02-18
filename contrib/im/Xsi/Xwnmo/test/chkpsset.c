/*
 * $Id: chkpsset.c,v 1.1 1991/09/11 06:48:20 ohm Exp $
 */
/*
 * X11R5 Input Method Test Program
 * chkpsset.c v 1.0   Wed Mar 13 11:16:00 JST 1991
 */

/*
 * Copyright 1991 by OMRON Corporation
 *
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 */

#include <stdio.h>

#include "exvalue.h"
#include "func.h"

FLAG	sarea(ic, mode)		/* Set AREA */
XIC	ic;
FLAG	mode;
{
    XRectangle	gparea, gsarea;
    VALUABLE	*area[CHKAREA];
    XVaNestedList	spe_nelist, sst_nelist;
    XVaNestedList	gpe_nelist, gst_nelist;
    int		res, cnt, sum;
    
    if ((mode == ONSP) || (mode == ROOTW)) {
	prerrfunc();
	return (COMP);
    } else {
	cls(prdisp);		/* clear screen */
				/* create nestedlist for set values */
	prstatus("Creating NestedList for set values \"Area\"...");
	if (usta.area.width == ST_WIDTH) {
	    usta.area.width += WIN_ITV;
	} else {
	    usta.area.width -= WIN_ITV;
	}
	if (mode == OVERSP) {
	    upea.area.width = (upea.area.width == WPOVER1) ? WPOVER2 : WPOVER1;
	} else if (mode == OFFSP) { 
	    upea.area.x = usta.area.width + 2 * WIN_ITV;
	}
	spe_nelist = XVaCreateNestedList(DUMMY, XNArea, &upea.area, NULL);
	sst_nelist = XVaCreateNestedList(DUMMY, XNArea, &usta.area, NULL);
	prstatus("done.");
				/* test of XSetICValues */
	prstatus("Test of XSetICValues() for change \"Area\"...");
	XSetICValues(ic, XNPreeditAttributes, spe_nelist,
		         XNStatusAttributes, sst_nelist,
		     NULL);
	prstatus("done.");
				/* create nestedlist for get values */
	prstatus("Creating NestedList for get values \"Area\" ...");
	gpe_nelist = XVaCreateNestedList(DUMMY, XNArea, &gparea, NULL);
	gst_nelist = XVaCreateNestedList(DUMMY, XNArea, &gsarea, NULL);
	prstatus("done.");
				/* test of XGetICValues */
	prstatus("Test of XGetICValues() \"Area\"...");
	XGetICValues(ic, XNPreeditAttributes, gpe_nelist,
		         XNStatusAttributes, gst_nelist,
		     NULL);
	prstatus("done.");
				/* verify values */
	res = sum = 0;
	area[0] = mkstruct(PAX, DEC, upea.area.x, gparea.x);
	area[1] = mkstruct(PAY, DEC, upea.area.y, gparea.y);
	area[2] = mkstruct(PAW, DEC, upea.area.width, gparea.width);
	area[3] = mkstruct(PAH, DEC, upea.area.height, gparea.height);
	area[4] = mkstruct(SAX, DEC, usta.area.x, gsarea.x);
	area[5] = mkstruct(SAY, DEC, usta.area.y, gsarea.y);
	area[6] = mkstruct(SAW, DEC, usta.area.width, gsarea.width);
	area[7] = mkstruct(SAH, DEC, usta.area.height, gsarea.height);
	for (cnt = 0; cnt < CHKAREA; cnt++) {
	    res = verval(area[cnt]);
	    sum += res;
	}
				/* output of results */
	cls(prdisp);
	prveres(sum);
	if (sum != NOERR) {
	    prprint("Test of XSetICValues() \"Preedit/Status Area\" is failed.\n");
	    prprint("Preedit/Status Area has not changed.\n\n");
	    prprint("I will destroy IC and continue test.\n");
	    prprint("Check log file after this test.\n\n");
	    fprintf(icfp, "...Test of XSetICValues() \"Preedit/Status Area\" is failed.\n\n");
	    XFree((char *)spe_nelist);
	    XFree((char *)sst_nelist);
	    XFree((char *)gpe_nelist);
	    XFree((char *)gst_nelist);
	    prpng();
	    return (DIC);
	}
	prprint("Test of XSetICValues() \"Preedit/Status Area\" is succeeded.\n");
	prprint("Preedit/Status Area has changed.\n\n");
	prprint("Check on your own eyes and move next test.\n");
	fprintf(icfp, "...Test of XSetICValues() \"Preedit/Status Area\" is succeeded.\n\n");
	XFree((char *)spe_nelist);
	XFree((char *)sst_nelist);
	XFree((char *)gpe_nelist);
	XFree((char *)gst_nelist);
	prpok();
	return (COMP);
    }
}

FLAG	sneeded(ic, mode)	/* Set areaNEEDED */
XIC	ic;
FLAG	mode;
{
    XRectangle	gpneeded, gsneeded;
    XRectangle	wparea, wsarea, gparea, gsarea;
    VALUABLE	*needed[CHKNEEDED];
    VALUABLE	*area[CHKAREA];
    XVaNestedList	spe_nelist, sst_nelist;
    XVaNestedList	gpe_nelist, gst_nelist;
    int		res, cnt, sum;
    
    if (mode != OFFSP) {
	prerrfunc();
	return (COMP);
    } else {
	cls(prdisp);		/* clear screen */
				/* create nestedlist for set values */
	prstatus("Creating NestedList for set values \"AreaNeeded\"...");
	upea.needed.width = 100;
	upea.needed.height = 100;
	usta.needed.width = 100;
	usta.needed.height = 100;
	spe_nelist = XVaCreateNestedList(DUMMY, XNAreaNeeded, &upea.needed, NULL);
	sst_nelist = XVaCreateNestedList(DUMMY, XNAreaNeeded, &upea.needed, NULL);
	prstatus("done.");
				/* test of XSetICValues */
	prstatus("Test of XSetICValues() for change \"AreaNeeded\"...");
	XSetICValues(ic, XNPreeditAttributes, spe_nelist,
		         XNStatusAttributes, sst_nelist,
		     NULL);
	prstatus("done.");
				/* create nestedlist for get values */
	prstatus("Creating NestedList for get values \"AreaNeeded\" ...");
	gpe_nelist = XVaCreateNestedList(DUMMY, XNAreaNeeded, &gpneeded, NULL);
	gst_nelist = XVaCreateNestedList(DUMMY, XNAreaNeeded, &gsneeded, NULL);
	prstatus("done.");
				/* test of XGetICValues */
	prstatus("Test of XGetICValues() \"AreaNeeded\"...");
	XGetICValues(ic, XNPreeditAttributes, gpe_nelist,
		         XNStatusAttributes, gst_nelist,
		     NULL);
	prstatus("done.");
				/* verify values */
	res = sum = 0;
	needed[0] = mkstruct(PNW, DEC, upea.needed.width, gpneeded.width);
	needed[1] = mkstruct(PNH, DEC, upea.needed.height, gpneeded.height);
	needed[2] = mkstruct(SNW, DEC, usta.needed.width, gsneeded.width);
	needed[3] = mkstruct(SNH, DEC, usta.needed.height, gsneeded.height);
	for (cnt = 0; cnt < CHKNEEDED; cnt++) {
	    res = verval(needed[cnt]);
	}
	wparea = upea.area;
	wsarea = usta.area;
	upea.area.width = gpneeded.width;
	upea.area.height = gpneeded.height;
	usta.area.width = gsneeded.width;
	usta.area.height = gsneeded.height;
	spe_nelist = XVaCreateNestedList(DUMMY, XNArea, &upea.area, NULL);
	sst_nelist = XVaCreateNestedList(DUMMY, XNArea, &usta.area, NULL);
	prstatus("done.");
				/* test of XSetICValues */
	prstatus("Test of XSetICValues() for change \"Area\"...");
	XSetICValues(ic, XNPreeditAttributes, spe_nelist,
		         XNStatusAttributes, sst_nelist,
		     NULL);
	prstatus("done.");
				/* create nestedlist for get values */
	prstatus("Creating NestedList for get values \"Area\" ...");
	gpe_nelist = XVaCreateNestedList(DUMMY, XNArea, &gparea, NULL);
	gst_nelist = XVaCreateNestedList(DUMMY, XNArea, &gsarea, NULL);
	prstatus("done.");
				/* test of XGetICValues */
	prstatus("Test of XGetICValues() \"Area\"...");
	XGetICValues(ic, XNPreeditAttributes, gpe_nelist,
		         XNStatusAttributes, gst_nelist,
		     NULL);
	prstatus("done.");
				/* verify values */
	res = sum = 0;
	area[0] = mkstruct(PAX, DEC, upea.area.x, gparea.x);
	area[1] = mkstruct(PAY, DEC, upea.area.y, gparea.y);
	area[2] = mkstruct(PAW, DEC, upea.area.width, gparea.width);
	area[3] = mkstruct(PAH, DEC, upea.area.height, gparea.height);
	area[4] = mkstruct(SAX, DEC, usta.area.x, gsarea.x);
	area[5] = mkstruct(SAY, DEC, usta.area.y, gsarea.y);
	area[6] = mkstruct(SAW, DEC, usta.area.width, gsarea.width);
	area[7] = mkstruct(SAH, DEC, usta.area.height, gsarea.height);
	for (cnt = 0; cnt < CHKAREA; cnt++) {
	    res = verval(area[cnt]);
	    sum += res;
	}
					/* output of results */
	cls(prdisp);
	prveres(sum);
	if (sum != NOERR) {
	    prprint("Test of XSetICValues() \"Preedit/Status AreaNeeded\" is failed.\n");
	    prprint("Preedit/Status Area has not changed, ");
	    prprint("but I will continue test.\n\n");
	    prprint("Check log file after this test.\n\n");
	    fprintf(icfp, "...Test of XSetICValues() \"Preedit/Status AreaNeeded\" is failed.\n\n");
	} else {
	    prprint("Test of XSetICValues() \"Preedit/Status AreaNeeded\" is succeeded.\n");
	    prprint("Preedit/Status Area has changed.\n\n");
	    prprint("Check on your own eyes and move next test.\n");
	    prprint("Check log file after this test.\n\n");
	    fprintf(icfp, "...Test of XSetICValues() \"Preedit/Status AreaNeeded\" is succeeded.\n\n");
	}
	upea.area = wparea;
	usta.area = wsarea;
	spe_nelist = XVaCreateNestedList(DUMMY, XNArea, &upea.area, NULL);
	sst_nelist = XVaCreateNestedList(DUMMY, XNArea, &usta.area, NULL);
	prstatus("done.");
				/* test of XSetICValues */
	prstatus("Test of XSetICValues() for change \"Area\"...");
	XSetICValues(ic, XNPreeditAttributes, spe_nelist,
		         XNStatusAttributes, sst_nelist,
		     NULL);
	prstatus("done.");
	XFree((char *)spe_nelist);
	XFree((char *)sst_nelist);
	XFree((char *)gpe_nelist);
	XFree((char *)gst_nelist);
        prpok();
	return (COMP);
    }
}

FLAG	sspot(ic, mode)		/* Set SPOTlocation */
XIC	ic;
FLAG	mode;
{
    XPoint	gspot;
    VALUABLE	*spot[CHKSPOT];
    XVaNestedList	spe_nelist;
    XVaNestedList	gpe_nelist;
    int		res, cnt, sum;
    
    if (mode != OVERSP) {
	prerrfunc();
	return (COMP);
    } else {
	cls(prdisp);		/* clear screen */
				/* create nestedlist for set values */
	prstatus("Creating NestedList for set values \"SpotLocation\"...");
	upea.spot.x = (upea.spot.x == XPSL1) ? XPSL2 : XPSL1;
	upea.spot.y = YPSL;
	spe_nelist = XVaCreateNestedList(DUMMY, XNSpotLocation, &upea.spot, NULL);
	prstatus("done.");
				/* test of XSetICValues */
	prstatus("Test of XSetICValues() for change \"SpotLocation\"...");
	XSetICValues(ic, XNPreeditAttributes, spe_nelist, NULL);
	prstatus("done.");
				/* create nestedlist for get values */
	prstatus("Creating NestedList for get values \"SpotLocation\" ...");
	gpe_nelist = XVaCreateNestedList(DUMMY, XNSpotLocation, &gspot, NULL);
	prstatus("done.");
				/* test of XGetICValues */
	prstatus("Test of XGetICValues() \"Area\"...");
	XGetICValues(ic, XNPreeditAttributes, gpe_nelist, NULL);
	prstatus("done.");
				/* verify values */
	res = sum = 0;
	spot[0] = mkstruct(PSLX, DEC, upea.spot.x, gspot.x);
	spot[1] = mkstruct(PSLY, DEC, upea.spot.y, gspot.y);
	for (cnt = 0; cnt < CHKSPOT; cnt++) {
	    res = verval(spot[cnt]);
	    sum += res;
	}
				/* output of results */
	cls(prdisp);
	prveres(sum);
	if (sum != NOERR) {
	    prprint("Test of XSetICValues() \"Preedit SpotLocation\" is failed.\n");
	    prprint("Preedit SpotLocation has not changed.\n\n");
	    prprint("I will destroy IC and continue test.\n");
	    prprint("Check log file after this test.\n\n");
	    fprintf(icfp, "...Test of XSetICValues() \"Preedit SpotLocation\" is failed.\n\n");
	    XFree((char *)spe_nelist);
	    XFree((char *)gpe_nelist);
	    prpng();
	    return (DIC);
	}
	prprint("Test of XSetICValues() \"Preedit SpotLocation\" is succeeded.\n");
	prprint("Preedit SpotLocation has changed.\n\n");
	prprint("Check on your own eyes and move next test.\n");
	fprintf(icfp, "...Test of XSetICValues() \"Preedit SpotLocation\" is succeeded.\n\n");
	XFree((char *)spe_nelist);
	XFree((char *)gpe_nelist);
	prpok();
	return (COMP);
    }
}


FLAG	scmap(ic, mode)		/* Set ColorMAP */
XIC	ic;
FLAG	mode;
{
    Colormap	gpcmap, gscmap;
    VALUABLE	*colormap[CHKCMAP];
    XVaNestedList	spe_nelist, sst_nelist;
    XVaNestedList	gpe_nelist, gst_nelist;
    int		res, cnt, sum;

    if (mode == ONSP) {
	prerrfunc();
	return (COMP);
    } else {
	cls(prdisp);		/* clear screen */
				/* create nestedlist for get values */
	prstatus("Creating NestedList for set values \"Colormap\"...");
	cmflag = (cmflag == 1) ? 0 : 1;
	upea.cmap = usta.cmap = cmap[cmflag];
	spe_nelist = XVaCreateNestedList(DUMMY, XNColormap, upea.cmap, NULL);
	sst_nelist = XVaCreateNestedList(DUMMY, XNColormap, usta.cmap, NULL);
	prstatus("done.");
				/* test of XSetICValues */
	prstatus("Test of XSetICValues() for change \"Colormap\"...");
	XSetICValues(ic, XNPreeditAttributes, spe_nelist,
		         XNStatusAttributes, sst_nelist,
		     NULL);
	prstatus("done.");
				/* create nestedlist for get values */
	prstatus("Creating NestedList for get values \"Colormap\" ...");
	gpe_nelist = XVaCreateNestedList(DUMMY, XNColormap, &gpcmap, NULL);
	gst_nelist = XVaCreateNestedList(DUMMY, XNColormap, &gscmap, NULL);
	prstatus("done.");
				/* test of XGetICValues */
	prstatus("Test of XGetICValues() \"Colormap\"...");
	XGetICValues(ic, XNPreeditAttributes, gpe_nelist,
		         XNStatusAttributes, gst_nelist,
		     NULL);
	prstatus("done.");
				/* verify values */
	res = sum = 0;
	colormap[0] = mkstruct(PCM, HEX, upea.cmap, gpcmap);
	colormap[1] = mkstruct(SCM, HEX, usta.cmap, gscmap);
	for (cnt = 0; cnt < CHKCMAP; cnt++) {
	    res = verval(colormap[cnt]);
	    sum += res;
	}
				/* output of results */
	cls(prdisp);
	prveres(sum);
	if (sum != NOERR) {
	    prprint("Test of XSetICValues() for change \"Preedit/Status Colormap\" is failed.\n");
	    prprint("Preedit/Status Colormap has not changed.\n\n");
	    prprint("I will destroy IC and continue test.\n");
	    prprint("Check log file after this test.\n\n");
	    fprintf(icfp, "...Test of XSetICValues() for change \"Preedit/Status Cmap\" is failed.\n\n");
	    XFree((char *)spe_nelist);
	    XFree((char *)sst_nelist);
	    XFree((char *)gpe_nelist);
	    XFree((char *)gst_nelist);
	    prpng();
	    return (DIC);
	}
	prprint("Test of XSetICValues() for change \"Preedit/Status Colormap\" is succeeded.\n");
	prprint("Preedit/Status Colormap has changed.\n\n");
	prprint("Check on your own eyes and move next test.\n");
	prprint("Check log file after this test.\n\n");
	fprintf(icfp, "...Test of XSetICValues() for change \"Preedit/Status Colormap\" is succeeded.\n\n");
	XFree((char *)spe_nelist);
	XFree((char *)sst_nelist);
	XFree((char *)gpe_nelist);
	XFree((char *)gst_nelist);
	prpok();
	return (COMP);
    }
}    

FLAG	sground(ic, mode)	/* Set fore/backGROUND */
XIC	ic;
FLAG	mode;
{
    u_long	gpfg, gpbg, gsfg, gsbg;
    VALUABLE	*ground[CHKFBGND];
    XVaNestedList	spe_nelist, sst_nelist;
    XVaNestedList	gpe_nelist, gst_nelist;
    int		res, cnt, sum;
    static int	i = 0, j = PIX_MAX -1;

    if (mode == ONSP) {
	prerrfunc();
	return (COMP);
    } else {
	cls(prdisp);		/* clear screen */
	i++, j--;
	if (i == PIX_MAX) {
	    i = 0;
	}
	if (j == -1) {
	    j = PIX_MAX -1;
	}
	upea.fg = pixel[cmflag][j];
	upea.bg = pixel[cmflag][i];
	usta.fg = pixel[cmflag][i];
	usta.bg = pixel[cmflag][j];
	/* Create nestedlist for set values */
	prstatus("Creating NestedList for set values \"Fg/Bg\"...");
	spe_nelist = XVaCreateNestedList(DUMMY, XNForeground, upea.fg,
					        XNBackground, upea.bg,
					 NULL);
	sst_nelist = XVaCreateNestedList(DUMMY, XNForeground, usta.fg,
					        XNBackground, usta.bg,
					 NULL);
	prstatus("done.");
				/* test of XSetICValues */
	prstatus("Using XSetICValues() for change \"Fg/Bg\"...");
	XSetICValues(ic, XNPreeditAttributes, spe_nelist,
		         XNStatusAttributes, sst_nelist,
		     NULL);
	prstatus("done.");
				/* create nestedlist for get values */
	prstatus("Creating NestedList for get values \"Fg/Bg\" ...");
	gpe_nelist = XVaCreateNestedList(DUMMY, XNForeground, &gpfg,
					        XNBackground, &gpbg,
					 NULL);
	gst_nelist = XVaCreateNestedList(DUMMY, XNForeground, &gsfg,
					        XNBackground, &gsbg,
					 NULL);
	prstatus("done.");
				/* test of XGetICValues */
	prstatus("Test of XGetICValues() \"Fg/Bg\"...");
	XGetICValues(ic, XNPreeditAttributes, gpe_nelist,
		         XNStatusAttributes, gst_nelist,
		     NULL);
	prstatus("done.");
				/* verify values */
	res = sum = 0;
	ground[0] = mkstruct(PFG, HEX, upea.fg, gpfg);
	ground[1] = mkstruct(PBG, HEX, upea.bg, gpbg);
	ground[2] = mkstruct(SFG, HEX, usta.fg, gsfg);
	ground[3] = mkstruct(SBG, HEX, usta.bg, gsbg);
	for (cnt = 0; cnt < CHKFBGND; cnt++) {
	    res = verval(ground[cnt]);
	    sum += res;
	}
				/* output of results */
	cls(prdisp);
	prveres(sum);
	if (sum != NOERR) {
	    prprint("Test of XSetICValues() for change \"Preedit/Status Fg/Bg\" is failed.\n");
	    prprint("Preedit/Status Fg/Bg has not changed.\n\n");
	    prprint("I will destroy IC and continue test.\n");
	    prprint("Check log file after this test.\n\n");
	    fprintf(icfp, "...Test of XSetICValues() for change \"Preedit/Status Fg/Bg\" is failed.\n\n");
	    XFree((char *)spe_nelist);
	    XFree((char *)sst_nelist);
	    XFree((char *)gpe_nelist);
	    XFree((char *)gst_nelist);
	    prpng();
	    return (DIC);
	}
	prprint("Test of XSetICValues() for change \"Preedit/Status Fg/Bg\" is succeeded.\n");
	prprint("Preedit/Status Fg/Bg has changed.\n\n");
	prprint("Check on your own eyes and move next test.\n");
	prprint("Check log file after this test.\n\n");
	fprintf(icfp, "...Test of XSetICValues() for change \"Preedit/Status Fg/Bg\" is succeeded.\n\n");
	XFree((char *)spe_nelist);
	XFree((char *)sst_nelist);
	XFree((char *)gpe_nelist);
	XFree((char *)gst_nelist);
	prpok();
	return (COMP);
    }
}    

FLAG	spixmap(ic, mode)	/* Set bg/ PIXMAP */
XIC	ic;
FLAG	mode;
{
    Pixmap	gppix, gspix;
    VALUABLE	*pixmap[CHKPIX];
    XVaNestedList	spe_nelist, sst_nelist;
    XVaNestedList	gpe_nelist, gst_nelist;
    int		res, cnt, sum;
    
    if (mode == ONSP) {
	prerrfunc();
	return (COMP);
    } else {
	cls(prdisp);		/* clear screen */
					/* create nestedlist for get values */
	prstatus("Creating NestedList for set values \"Bg/ Pixmap\"...");
	upea.bgpm = (upea.bgpm == pix[0]) ? pix[1] : pix[0]; 
	usta.bgpm = (usta.bgpm == pix[0]) ? pix[1] : pix[0]; 
	spe_nelist = XVaCreateNestedList(DUMMY, XNBackgroundPixmap, upea.bgpm, NULL);
	sst_nelist = XVaCreateNestedList(DUMMY, XNBackgroundPixmap, usta.bgpm, NULL);
	prstatus("done.");
				/* test of XSetICValues */
	prstatus("Test of XSetICValues() \"Bg/ Pixmap\"...");
	XSetICValues(ic, XNPreeditAttributes, spe_nelist,
		         XNStatusAttributes, sst_nelist,
		     NULL);
	prstatus("done.");
				/* create nestedlist for get values */
	prstatus("Creating NestedList for get values \"Bg/ Pixmap\" again...");
	gpe_nelist = XVaCreateNestedList(DUMMY, XNBackgroundPixmap, &gppix, NULL);
	gst_nelist = XVaCreateNestedList(DUMMY, XNBackgroundPixmap, &gspix, NULL);
	prstatus("done.");
				/* test of XGetICValues */
	prstatus("Test of XGetICValues() \"Bg/ Pixmap\"...");
	XGetICValues(ic, XNPreeditAttributes, gpe_nelist,
		         XNStatusAttributes, gst_nelist,
		     NULL);
	prstatus("done.");
				/* verify values */
	res = sum = 0;
	pixmap[0] = mkstruct(PBP, HEX, upea.bgpm, gppix);
	pixmap[1] = mkstruct(SBP, HEX, usta.bgpm, gspix);
	for (cnt = 0; cnt < CHKPIX; cnt++) {
	    res = verval(pixmap[cnt]);
	    sum += res;
	}
				/* output of results */
	cls(prdisp);
	prveres(sum);
	if (sum != NOERR) {
	    prprint("Test of XSetICValues() for change \"Preedit/Status Bg/ Pixmap\" is failed.\n");
	    prprint("Preedit/Status Bg/ Pixmap has not changed. ");
	    prprint("but I will continue test.\n");
	    prprint("Check log file after this test.\n\n");
	    fprintf(icfp, "...Test of XSetICValues() for change \"Preedit/Status Bg/ Pixmap\" is failed.\n\n");
	} else {
	    prprint("Test of XSetICValues() for change \"Preedit/Status Bg/ Pixmap\" is succeeded.\n");
	    prprint("Preedit/Status Bg/ Pixmap has changed.\n\n");
	    prprint("Check on your own eyes and move next test.\n");
	    prprint("Check log file after this test.\n\n");
	    fprintf(icfp, "...Test of XSetICValues() for change \"Preedit/Status Bg/ Pixmap\" is succeeded.\n\n");
	}
	XFree((char *)spe_nelist);
	XFree((char *)sst_nelist);
	XFree((char *)gpe_nelist);
	XFree((char *)gst_nelist);
	prpok();
	return (COMP);
    }
}    

FLAG	sfontset(ic, mode)	/* Set FONTSET */
XIC	ic;
FLAG	mode;
{
    XFontSet	gpfont, gsfont;
    VALUABLE	*font[CHKFONT];
    XVaNestedList	spe_nelist, sst_nelist;
    XVaNestedList	gpe_nelist, gst_nelist;
    int		res, cnt, sum;
    
    if (mode == ONSP) {
	prerrfunc();
	return (COMP);
    } else {
	cls(prdisp);		/* clear screen */
				/* create nestedlist for get values */
	prstatus("Creating NestedList for set values \"FontSet\"...");
	upea.fset = (upea.fset == fontset[0]) ? fontset[1] : fontset[0]; 
	usta.fset = (usta.fset == fontset[0]) ? fontset[1] : fontset[0]; 
	spe_nelist = XVaCreateNestedList(DUMMY, XNFontSet, upea.fset, NULL);
	sst_nelist = XVaCreateNestedList(DUMMY, XNFontSet, usta.fset, NULL);
	prstatus("done.");
				/* test of XSetICValues */
	prstatus("Test of XSetICValues() for change \"FontSet\"...");
	XSetICValues(ic, XNPreeditAttributes, spe_nelist,
		         XNStatusAttributes, sst_nelist,
		     NULL);
	prstatus("done.");
				/* create nestedlist for get values */
	prstatus("Creating NestedList for get values \"FontSet\" ...");
	gpe_nelist = XVaCreateNestedList(DUMMY, XNFontSet, &gpfont, NULL);
	gst_nelist = XVaCreateNestedList(DUMMY, XNFontSet, &gsfont, NULL);
	prstatus("done.");
				/* test of XGetICValues */
	prstatus("Test of XGetICValues() \"FontSet\"...");
	XGetICValues(ic, XNPreeditAttributes, gpe_nelist,
		         XNStatusAttributes, gst_nelist,
		     NULL);
	prstatus("done.");
				/* verify values */
	res = sum = 0;
	font[0] = mkstruct(PBP, HEX, upea.fset, gpfont);
	font[1] = mkstruct(SBP, HEX, usta.fset, gsfont);
	for (cnt = 0; cnt < CHKFONT; cnt++) {
	    res = verval(font[cnt]);
	    sum += res;
	}
				/* output of results */
	cls(prdisp);
	prveres(sum);
	if (sum != NOERR) {
	    prprint("Test of XSetICValues() for change \"Preedit/Status FontSet\" is failed.\n");
	    prprint("Preedit/Status FontSet has not changed.\n\n");
	    prprint("I will destroy IC and continue test.\n");
	    prprint("Check log file after this test.\n\n");
	    fprintf(icfp, "...Test of XSetICValues() for change \"Preedit/Status FontSet\" is failed.\n\n");
	    XFree((char *)spe_nelist);
	    XFree((char *)sst_nelist);
	    XFree((char *)gpe_nelist);
	    XFree((char *)gst_nelist);
	    prpng();
	    return (DIC);
	}
	prprint("Test of XSetICValues() for change \"Preedit/Status FontSet\" is succeeded.\n");
	prprint("Preedit/Status FontSet has changed.\n\n");
	prprint("Check on your own eyes and move next test.\n");
	prprint("Check log file after this test.\n\n");
	fprintf(icfp, "...Test of XSetICValues() for change \"Preedit/Status FontSet\" is succeeded.\n\n");
	XFree((char *)spe_nelist);
	XFree((char *)sst_nelist);
	XFree((char *)gpe_nelist);
	XFree((char *)gst_nelist);
	prpok();
	return (COMP);
    }
}

FLAG	slinespc(ic, mode)	/* Set LINESPaCing */
XIC	ic;
FLAG	mode;
{
    XRectangle	*gplspc;
    VALUABLE	*plspc;
    XVaNestedList	spe_nelist;
    XVaNestedList	gpe_nelist;
    int		res, cnt, sum;
    
    if (mode != OVERSP) {
	prerrfunc();
	return (COMP);
    } else {
	cls(prdisp);		/* clear screen */
				/* create nestedlist for set values */
	prstatus("Creating NestedList for set values \"LineSpacing\"...");
	upea.lspc = (upea.lspc == LSPC1) ? LSPC2 : LSPC1;
	spe_nelist = XVaCreateNestedList(DUMMY, XNLineSpace, upea.lspc, NULL);
	prstatus("done.");
				/* test of XSetICValues */
	prstatus("Test of XSetICValues() for change \"LineSpacing\"...");
	XSetICValues(ic, XNPreeditAttributes, spe_nelist, NULL);
	prstatus("done.");
				/* create nestedlist for get values */
	prstatus("Creating NestedList for get values \"LineSpacing\" ...");
	gpe_nelist = XVaCreateNestedList(DUMMY, XNLineSpace, &gplspc, NULL);
	prstatus("done.");
				/* test of XGetICValues */
	prstatus("Test of XGetICValues() \"LineSpacing\"...");
	XGetICValues(ic, XNPreeditAttributes, gpe_nelist, NULL);
	prstatus("done.");
				/* verify values */
	res = sum = 0;
	plspc = mkstruct(PLS, DEC, upea.lspc, gplspc);
	res = verval(plspc);
					/* output of results */
	cls(prdisp);
	prveres(sum);
	if (sum != NOERR) {
	    prprint("Test of XSetICValues() for change \"Preedit LineSpacing\" is failed.\n");
	    prprint("Preedit LineSpacing has not changed.\n\n");
	    prprint("I will destroy IC and continue test.\n");
	    prprint("Check log file after this test.\n\n");
	    fprintf(icfp, "...Test of XSetICValues() for change \"Preedit LineSpacing\" is faled.\n\n");
	    XFree((char *)spe_nelist);
	    XFree((char *)gpe_nelist);
	    prpng();
	    return (DIC);
	}
	prprint("Test of XSetICValues() for change \"Preedit LineSpacing\" is succeeded.\n");
	prprint("Status LineSpacing has changed.\n\n");
	prprint("Check on your own eyes and move next test.\n");
	prprint("Check log file after this test.\n\n");
	fprintf(icfp, "...Test of XSetICValues() for change \"Preedit LineSpacing\" is succeeded.\n\n");
	XFree((char *)spe_nelist);
	XFree((char *)gpe_nelist);
	prpok();
	return (COMP);
    }
}

FLAG	scursor(ic, mode)	/* Set CURSOR */
XIC	ic;
FLAG	mode;
{
    Cursor	gpcursor, gscursor;
    VALUABLE	*cur[CHKCUR];
    XVaNestedList	spe_nelist, sst_nelist;
    XVaNestedList	gpe_nelist, gst_nelist;
    int		res, cnt, sum;
    
    if (mode == ONSP) {
	prerrfunc();
	return (COMP);
    } else {
	cls(prdisp);		/* clear screen */
				/* create nestedlist for get values */
	prstatus("Creating NestedList for set values \"Cursor\"...");
	upea.cursor = (upea.cursor == cursor[0]) ? cursor[1] : cursor[0]; 
	usta.cursor = (usta.cursor == cursor[0]) ? cursor[1] : cursor[0]; 
	spe_nelist = XVaCreateNestedList(DUMMY, XNCursor, upea.cursor, NULL);
	sst_nelist = XVaCreateNestedList(DUMMY, XNCursor, usta.cursor, NULL);
	prstatus("done.");
				/* test of XSetICValues */
	prstatus("Test of XSetICValues() for change \"Cursor\"...");
	XSetICValues(ic, XNPreeditAttributes, spe_nelist,
		         XNStatusAttributes, sst_nelist,
		     NULL);
	prstatus("done.");
				/* create nestedlist for get values */
	prstatus("Creating NestedList for get values \"Cursor\" ...");
	gpe_nelist = XVaCreateNestedList(DUMMY, XNCursor, &gpcursor, NULL);
	gst_nelist = XVaCreateNestedList(DUMMY, XNCursor, &gscursor, NULL);
	prstatus("done.");
				/* test of XGetICValues */
	prstatus("Test of XGetICValues() \"Cursor\"...");
	XGetICValues(ic, XNPreeditAttributes, gpe_nelist,
		         XNStatusAttributes, gst_nelist,
		     NULL);
	prstatus("done.");
				/* verify values */
	res = sum = 0;
	cur[0] = mkstruct(PBP, HEX, upea.cursor, gpcursor);
	cur[1] = mkstruct(SBP, HEX, usta.cursor, gscursor);
	for (cnt = 0; cnt < CHKCUR; cnt++) {
	    res = verval(cur[cnt]);
	    sum += res;
	}
	/* output of results */
	cls(prdisp);
	prveres(sum);
	if (sum != NOERR) {
	    prprint("Test of XSetICValues() for change \"Preedit/Status Cursor\" is failed.\n");
	    prprint("Preedit/Status Cursor has not changed, ");
	    prprint("but I will continue test.\n\n");
	    prprint("Check log file after this test.\n\n");
	    fprintf(icfp, "...Test of XSetICValues() for change \"Preedit/Status Cursor\" is failed.\n\n");
	} else {
	    prprint("Test of XSetICValues() for change \"Preedit/Status Cursor\" is succeeded.\n");
	    prprint("Preedit/Status Cursor has changed.\n\n");
	    prprint("Check on your own eyes and move next test.\n");
	    prprint("Check log file after this test.\n\n");
	    fprintf(icfp, "...Test of XSetICValues() for change \"Preedit/Status Cursor\" is succeeded.\n\n");
	}
	XFree((char *)spe_nelist);
	XFree((char *)sst_nelist);
	XFree((char *)gpe_nelist);
	XFree((char *)gst_nelist);
	prpok();
	return (COMP);
    }
}
