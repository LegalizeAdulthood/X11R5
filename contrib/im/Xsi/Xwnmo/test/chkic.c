/*
 * $Id: chkic.c,v 1.1 1991/09/11 06:48:17 ohm Exp $
 */
/*
 * X11R5 Input Method Test Program
 * chkic.c v 1.0   Tue Mar  5 19:20:16 JST 1991
 */

/*
 * Copyright 1991 by OMRON Corporation
 *
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 *
 *    		Shoji kamada	 OMRON Corporation
 *			 	 kamada@ari.ncl.omron.co.jp
 *
 */

#include <stdio.h>

#include "exvalue.h"
#include "func.h"

XIC	createic(im, mode, lfdir, flag)	/* check of XCreateIC */
XIM	im;
FLAG	mode;
char	*lfdir;
FLAG	*flag;
{
    XIC	ic;
    int	cnt;
    XIC onmust(), onmay(),
	overmust(), overmay(),
	rootmust(), rootmay();
    
    static CRICMF ftbl[MODENUM] = {
	{ONSP, onmust, onmay},
	{OVERSP, overmust, overmay},
	{OFFSP, offmust, offmay},
	{ROOTW, rootmust, rootmay},
	{NULL, NULL, NULL}
    };
    
    for (cnt = 0; ftbl[cnt].mode != NULL; cnt++) {
	if (mode == ftbl[cnt].mode) {
	    ic = (*ftbl[cnt].fmust)(im, lfdir, flag);
	    if (ic == NULL) {
		return (ic);
	    }
	    ic = (*ftbl[cnt].fmay)(im, flag);
	    return(ic);
	}
    }
}

XIC	onmust(im, lfdir, flag)	/* for ON-the-spot mode (MUST) */
XIM	im;
char	*lfdir;
FLAG	*flag;    
{
    XIC		ic;
    static int	cntlog = 1;
    char	logfile[MAX_BUF];
    int		res;
    int		cnt;
    int		sum;
    XIMStyle	gistyle;
    CALLBACK	gpec;
    CALLBACK	gstc;
    XVaNestedList	cpe_nelist, cst_nelist;
    XVaNestedList	gpe_nelist, gst_nelist;
    VALUABLE	*von[ONMUST];
    	
    prmode("X11R5 Input Method Test Program : <<on-the-spot mode>>");
    prstatus("Creating log file for ic..."); /* open log file for IC test */
    sprintf(logfile, "%s/ticon.log%d", lfdir, cntlog);
    icfp = fopen(logfile, "w");
    cntlog++;
    prstatus("done.");
    prstatus("Creating NestedList for set...");
				/* create NestedList for create IC (must) */
    uistyle = (XIMPreeditCallbacks | XIMStatusCallbacks); /* initialize valuables */
    upec.start.client_data = NULL;
    upec.start.callback = (XIMProc)pe_start;
    upec.done.client_data = NULL;
    upec.done.callback = (XIMProc)pe_done;
    upec.draw.client_data = NULL;
    upec.draw.callback = (XIMProc)pe_draw;
    upec.caret.client_data = NULL;
    upec.caret.callback = (XIMProc)pe_caret;
    ustc.start.client_data = NULL;
    ustc.start.callback = (XIMProc)st_start;
    ustc.done.client_data = NULL;
    ustc.done.callback = (XIMProc)st_done;
    ustc.draw.client_data = NULL;
    ustc.draw.callback = (XIMProc)st_draw;
    cpe_nelist = XVaCreateNestedList(DUMMY, XNPreeditStartCallback, &upec.start,
				            XNPreeditDoneCallback, &upec.done,
				            XNPreeditDrawCallback, &upec.draw,
				            XNPreeditCaretCallback, &upec.caret,
				     NULL);
    cst_nelist = XVaCreateNestedList(DUMMY, XNStatusStartCallback, &ustc.start,
				            XNStatusDoneCallback, &ustc.done,
				            XNStatusDrawCallback, &ustc.draw,
				     NULL);
    prstatus("done.");

    prstatus("Creating ic...");	/* test of XCreateIC() (must) */
    ic = XCreateIC(im, XNInputStyle, uistyle,
		       XNPreeditAttributes, cpe_nelist,
		       XNStatusAttributes, cst_nelist,
		       NULL);
    prstatus("done.");
    if (ic == NULL) {		/* transaction of error */
	cls(prdisp);
	prprint("Test of XCreateIC() <must> is failed.\n");
	prprint("Cannot create IC. NULL returned.\n");
	prprint("This program will shutdown soon.\n\n");
	fprintf(icfp, "Test of XCreateIC <must> is failed.\n");
	fprintf(icfp, "Cannot create IC. NULL returned.\n\n");
	*flag = CRICERR;
	return (NULL);
    }
    prstatus("Creating NestedList for get...");
			/* Create NestedList for GetICValues() (must) */
    gpe_nelist = XVaCreateNestedList(DUMMY, XNPreeditStartCallback, &gpec.start,
				            XNPreeditDoneCallback, &gpec.done,
				            XNPreeditDrawCallback, &gpec.draw,
				            XNPreeditCaretCallback, &gpec.caret,
			     	    NULL);
    gst_nelist = XVaCreateNestedList(DUMMY, XNStatusStartCallback, &gstc.start,
				            XNStatusDoneCallback, &gstc.done,
				            XNStatusDrawCallback, &gstc.draw,
				     NULL);
    prstatus("done.");

    prstatus("Test of GetICValues()...");    /* test of GetICValues() (must) */
    XGetICValues(ic, XNInputStyle, &gistyle,
		     XNPreeditAttributes, gpe_nelist,
		     XNStatusAttributes, gst_nelist,
		     NULL);
    prstatus("done.");

    res = 0;			/* verify valuables */
    sum = 0;
    von[0] = mkstruct(IS, HEX, uistyle, gistyle);
    von[1] = mkstruct(PSL, HEX, upec.start.client_data, gpec.start.client_data);
    von[2] = mkstruct(PSA, HEX, upec.start.callback, gpec.start.callback);
    von[3] = mkstruct(POL, HEX, upec.done.client_data, gpec.done.client_data);
    von[4] = mkstruct(POA, HEX, upec.done.callback, gpec.done.callback);
    von[5] = mkstruct(PRL, HEX, upec.draw.client_data, gpec.draw.client_data);
    von[6] = mkstruct(PRA, HEX, upec.draw.callback, gpec.draw.callback);
    von[7] = mkstruct(PCL, HEX, upec.caret.client_data, gpec.caret.client_data);
    von[8] = mkstruct(PCA, HEX, upec.caret.callback, gpec.caret.callback);
    von[9] = mkstruct(SSL, HEX, ustc.start.client_data, gstc.start.client_data);
    von[10] = mkstruct(SSA, HEX, ustc.start.callback, gstc.start.callback);
    von[11] = mkstruct(SOL, HEX, ustc.done.client_data, gstc.done.client_data);
    von[12] = mkstruct(SOA, HEX, ustc.done.callback, gstc.done.callback);
    von[13] = mkstruct(SRL, HEX, ustc.draw.client_data, gstc.draw.client_data);
    von[14] = mkstruct(SRA, HEX, ustc.draw.callback, gstc.draw.callback);
    for (cnt = 0; cnt < ONMUST; cnt++) {
	res = verval(von[cnt]);
	sum += res;
    }

    cls(prdisp);
    prveres(sum);
    if (sum != NOERR) {
	prprint("Test of XCreateIC() <must> is failed.\n");
	prprint("This program will shutdown soon.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <must> is failed.\n\n");
	*flag = GICERR;
	return (NULL);
    } else {
	prprint("Test of XCreateIC() <must> is succeeded.\n");
	prprint("move next test.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <must> is succeeded.\n\n");
    }
    XFree((char *)cpe_nelist);
    XFree((char *)cst_nelist);
    XFree((char *)gpe_nelist);
    XFree((char *)gst_nelist);
    return (ic);
}

XIC	onmay(im, flag)		/* for ON-the-spot mode (MAY) */
XIM	im;
FLAG	*flag;
{
    XIC		ic;
    static int	cntlog = 1;
    char	logfile[MAX_BUF];
    int		res;
    int		cnt;
    int		sum;
    XIMStyle	gistyle;
    Window	gclient;
    Window	gfocus;
    CALLBACK	gpec;
    CALLBACK	gstc;
    XVaNestedList	cpe_nelist, cst_nelist;
    XVaNestedList	gpe_nelist, gst_nelist;
    VALUABLE	*von[ONMUST];
    	
    prmode("X11R5 Input Method Test Program : <<on-the-spot mode>>");
    prstatus("Creating NestedList for set...");
				/* create NestedList for create IC (must) */
    uistyle = (XIMPreeditCallbacks | XIMStatusCallbacks); /* initialize valuables */
    uclient = window1.win;
    ufocus = window3.win;
    ufname = window3.name;
    upec.start.client_data = NULL;
    upec.start.callback = (XIMProc)pe_start;
    upec.done.client_data = NULL;
    upec.done.callback = (XIMProc)pe_done;
    upec.draw.client_data = NULL;
    upec.draw.callback = (XIMProc)pe_draw;
    upec.caret.client_data = NULL;
    upec.caret.callback = (XIMProc)pe_caret;
    ustc.start.client_data = NULL;
    ustc.start.callback = (XIMProc)st_start;
    ustc.done.client_data = NULL;
    ustc.done.callback = (XIMProc)st_done;
    ustc.draw.client_data = NULL;
    ustc.draw.callback = (XIMProc)st_draw;
    cpe_nelist = XVaCreateNestedList(DUMMY, XNPreeditStartCallback, &upec.start,
				            XNPreeditDoneCallback, &upec.done,
				            XNPreeditDrawCallback, &upec.draw,
				            XNPreeditCaretCallback, &upec.caret,
				     NULL);
    cst_nelist = XVaCreateNestedList(DUMMY, XNStatusStartCallback, &ustc.start,
				            XNStatusDoneCallback, &ustc.done,
				            XNStatusDrawCallback, &ustc.draw,
				     NULL);
    prstatus("done.");

    prstatus("Creating ic...");	/* test of XCreateIC() (must) */
    ic = XCreateIC(im, XNInputStyle, uistyle,
		       XNClientWindow, uclient,
		       XNFocusWindow, ufocus,
		       XNPreeditAttributes, cpe_nelist,
		       XNStatusAttributes, cst_nelist,
		       NULL);
    prstatus("done.");
    if (ic == NULL) {		/* transaction of error */
	cls(prdisp);
	prprint("Test of XCreateIC() <must> is failed.\n");
	prprint("Cannot create IC. NULL returned.\n");
	prprint("This program will shutdown soon.\n\n");
	fprintf(icfp, "Test of XCreateIC <must> is failed.\n");
	fprintf(icfp, "Cannot create IC. NULL returned.\n\n");
	*flag = CRICERR;
	return (NULL);
    }
    prstatus("Creating NestedList for get...");
			/* Create NestedList for GetICValues() (must) */
    gpe_nelist = XVaCreateNestedList(DUMMY, XNPreeditStartCallback, &gpec.start,
				            XNPreeditDoneCallback, &gpec.done,
				            XNPreeditDrawCallback, &gpec.draw,
				            XNPreeditCaretCallback, &gpec.caret,
			     	     NULL);
    gst_nelist = XVaCreateNestedList(DUMMY, XNStatusStartCallback, &gstc.start,
				            XNStatusDoneCallback, &gstc.done,
				            XNStatusDrawCallback, &gstc.draw,
				     NULL);
    prstatus("done.");

    prstatus("Test of GetICValues()...");    /* test of GetICValues() (must) */
    XGetICValues(ic, XNInputStyle, &gistyle,
		     XNClientWindow, &gclient,
		     XNFocusWindow, &gfocus,
		     XNPreeditAttributes, gpe_nelist,
		     XNStatusAttributes, gst_nelist,
		     NULL);
    prstatus("done.");

    res = 0;			/* verify valuables */
    sum = 0;
    von[0] = mkstruct(IS, HEX, uistyle, gistyle);
    von[1] = mkstruct(CW, HEX, uclient, gclient);
    von[2] = mkstruct(FW, HEX, ufocus, gfocus);
    von[3] = mkstruct(PSL, HEX, upec.start.client_data, gpec.start.client_data);
    von[4] = mkstruct(PSA, HEX, upec.start.callback, gpec.start.callback);
    von[5] = mkstruct(POL, HEX, upec.done.client_data, gpec.done.client_data);
    von[6] = mkstruct(POA, HEX, upec.done.callback, gpec.done.callback);
    von[7] = mkstruct(PRL, HEX, upec.draw.client_data, gpec.draw.client_data);
    von[8] = mkstruct(PRA, HEX, upec.draw.callback, gpec.draw.callback);
    von[9] = mkstruct(PCL, HEX, upec.caret.client_data, gpec.caret.client_data);
    von[10] = mkstruct(PCA, HEX, upec.caret.callback, gpec.caret.callback);
    von[11] = mkstruct(SSL, HEX, ustc.start.client_data, gstc.start.client_data);
    von[12] = mkstruct(SSA, HEX, ustc.start.callback, gstc.start.callback);
    von[13] = mkstruct(SOL, HEX, ustc.done.client_data, gstc.done.client_data);
    von[14] = mkstruct(SOA, HEX, ustc.done.callback, gstc.done.callback);
    von[15] = mkstruct(SRL, HEX, ustc.draw.client_data, ustc.draw.client_data);
    von[16] = mkstruct(SRA, HEX, ustc.draw.callback, gstc.draw.callback);
    for (cnt = 0; cnt < ONMAY; cnt++) {
	res = verval(von[cnt]);
	sum += res;
    }

    cls(prdisp);
    prveres(sum);
    if (sum != NOERR) {
	prprint("Test of XCreateIC() <must> is failed.\n");
	prprint("This program will shutdown soon.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <must> is failed.\n\n");
	*flag = GICERR;
	return (NULL);
    } else {
	prprint("Test of XCreateIC() <must> is succeeded.\n");
	prprint("move next test.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <must> is succeeded.\n\n");
    }
    XFree((char *)cpe_nelist);
    XFree((char *)cst_nelist);
    XFree((char *)gpe_nelist);
    XFree((char *)gst_nelist);
    return (ic);
}

XIC	overmust(im, lfdir, flag)/* for OVER-the-spot mode (MUST) */
XIM	im;
char	*lfdir;
FLAG	*flag;
{
    XIC		ic;
    static int	cntlog = 1;
    char	logfile[MAX_BUF];
    int		res;
    int		cnt;
    int		sum;
    XIMStyle	gistyle;
    XPoint	gspot;
    XFontSet	gpfset, gsfset;
    XVaNestedList	cpe_nelist, cst_nelist;
    XVaNestedList	gpe_nelist, gst_nelist;
    VALUABLE	*vover[OVERMUST];
    	
    prmode("X11R5 Input Method Test Program : <<over-the-spot mode>>");
    prstatus("Creating log file for ic..."); /* open log file for IC test */
    sprintf(logfile, "%s/ticover.log%d", lfdir, cntlog);
    icfp = fopen(logfile, "w");
    cntlog++;
    prstatus("done.");
    prstatus("Creating NestedList for set...");
				/* create NestedList for create IC (must) */
    uistyle = (XIMPreeditPosition | XIMStatusArea); /* initialize valuables */
    upea.spot.x = XPSL1;
    upea.spot.y = YPSL;
    upea.fset = fontset[0];
    usta.fset = fontset[1];
    cpe_nelist = XVaCreateNestedList(DUMMY, XNSpotLocation, &upea.spot,
				            XNFontSet, upea.fset,
				            NULL);
    cst_nelist = XVaCreateNestedList(DUMMY, XNFontSet, usta.fset, NULL);
    prstatus("done.");

    prstatus("Creating ic...");	/* test of XCreateIC() (must) */
    ic = XCreateIC(im, XNInputStyle, uistyle,
		       XNPreeditAttributes, cpe_nelist,
		       XNStatusAttributes, cst_nelist,
		       NULL);
    prstatus("done.");
    if (ic == NULL) {		/* transaction of error */
	cls(prdisp);
	prprint("Test of XCreateIC() <must> is failed.\n");
	prprint("Cannot create IC. NULL returned.\n");
	prprint("This program will shutdown soon.\n\n");
	fprintf(icfp, "Test of XCreateIC <must> is failed.\n");
	fprintf(icfp, "Cannot create IC. NULL returned.\n\n");
	*flag = CRICERR;
	return (NULL);
    }
    prstatus("Creating NestedList for get...");
			/* Create NestedList for GetICValues() (must) */
    gpe_nelist = XVaCreateNestedList(DUMMY, XNSpotLocation, &gspot,
				     	    XNFontSet, &gpfset,
				     	    NULL);
    gst_nelist = XVaCreateNestedList(DUMMY, XNFontSet, &gsfset, NULL);
    prstatus("done.");

    prstatus("Test of GetICValues()...");    /* test of GetICValues() (must) */
    XGetICValues(ic, XNInputStyle, &gistyle,
		     XNPreeditAttributes, gpe_nelist,
		     XNStatusAttributes, gst_nelist,
		     NULL);
    prstatus("done.");

    res = 0;			/* verify valuables */
    sum = 0;
    vover[0] = mkstruct(IS, HEX, uistyle, gistyle);
    vover[1] = mkstruct(PSLX, DEC, upea.spot.x, gspot.x);
    vover[2] = mkstruct(PSLY, DEC, upea.spot.y, gspot.y);
    vover[3] = mkstruct(PFS, HEX, upea.fset, gpfset);
    vover[4] = mkstruct(SFS, HEX, usta.fset, gsfset);
    for (cnt = 0; cnt < OVERMUST; cnt++) {
	res = verval(vover[cnt]);
	sum += res;
    }

    cls(prdisp);
    prveres(sum);
    if (sum != NOERR) {
	prprint("Test of XCreateIC() <must> is failed.\n");
	prprint("This program will shutdown soon.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <must> is failed.\n\n");
	*flag = GICERR;
	return (NULL);
    } else {
	prprint("Test of XCreateIC() <must> is succeeded.\n");
	prprint("move next test.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <must> is succeeded.\n\n");
    }
    XFree((char *)cpe_nelist);
    XFree((char *)cst_nelist);
    XFree((char *)gpe_nelist);
    XFree((char *)gst_nelist);
    return (ic);
}


XIC	overmay(im, flag)	/* for OVER-the-spot mode (MAY) */
XIM	im;
FLAG 	*flag;
{
    XIC		ic;
    int		res;
    int		cnt;
    int		sum;
    XIMStyle	gistyle;
    Window	gclient;
    Window	gfocus;
    RESOURCE	gres;
    XRectangle	gparea, gsarea;
    XPoint	gspot;
    Colormap	gpcmap, gscmap;
    u_long	gpfg, gpbg, gsfg, gsbg;
    Pixmap	gppix, gspix;
    XFontSet	gpfset, gsfset;
    int		glspc;
    Cursor	gpcursor, gscursor;
    XVaNestedList	cpe_nelist, cst_nelist;
    XVaNestedList	gpe_nelist, gst_nelist;
    VALUABLE	*vover[OVERMAY];
    	
    prmode("X11R5 Input Method Test Program : <<over-the-spot mode>>");
    prstatus("Creating NestedList for set...");
				/* create NestedList for create IC (may) */
    uistyle = (XIMPreeditPosition | XIMStatusArea); /* initialize valuables */
    uclient = window1.win;
    ufocus = window3.win;
    ufname = window3.name;
    ures.name = "xim";
    ures.class = "Xim";
    usta.area.x = WIN_ITV;
    usta.area.y = CALCY((CALCY(W1_HEIGHT, NW_HEIGHT)), PS_HEIGHT);
    usta.area.width = ST_WIDTH;
    usta.area.height = PS_HEIGHT;
    usta.cmap = cmap[0];
    usta.fg = pixel[1][0];
    usta.bg = pixel[1][3];
    usta.bgpm = pix[1];
    usta.fset = fontset[1];
    usta.cursor = cursor[1];
    upea.area.x = XPOVER;
    upea.area.y = YPOVER;
    upea.area.width = WPOVER1;
    upea.area.height = HPOVER;
    upea.spot.x = XPSL1;
    upea.spot.y = YPSL;
    upea.cmap = cmap[0];
    upea.fg = pixel[1][3];
    upea.bg = pixel[1][0];
    upea.bgpm = pix[0];
    upea.fset = fontset[0];
    upea.lspc = LSPC1;
    upea.cursor = cursor[0];
    cpe_nelist = XVaCreateNestedList(DUMMY, XNArea, &upea.area,
				            XNSpotLocation, &upea.spot,
				            XNColormap, upea.cmap,
				            XNForeground, upea.fg,
				            XNBackground, upea.bg,
				            XNBackgroundPixmap, upea.bgpm,
				            XNFontSet, upea.fset,
				            XNLineSpace, upea.lspc,
				            XNCursor, upea.cursor,
				     NULL);
    cst_nelist = XVaCreateNestedList(DUMMY, XNArea, &usta.area,
				            XNColormap, usta.cmap,
				            XNForeground, usta.fg,
				            XNBackground, usta.bg,
				            XNBackgroundPixmap, usta.bgpm,
				            XNFontSet, usta.fset,
				            XNCursor, usta.cursor,
				     NULL);
    prstatus("done.");

    prstatus("Creating ic...");	/* test of XCreateIC() (may) */
    ic = XCreateIC(im, XNInputStyle, uistyle,
		       XNClientWindow, uclient,
		       XNFocusWindow, ufocus,
		       XNResourceName, ures.name,
		       XNResourceClass, ures.class,
		       XNPreeditAttributes, cpe_nelist,
		       XNStatusAttributes, cst_nelist,
		       NULL);
    prstatus("done.");
    if (ic == NULL) {		/* transaction of error */
	cls(prdisp);
	prprint("Test of XCreateIC() <must> is failed.\n");
	prprint("Cannot create IC. NULL returned.\n");
	prprint("This program will shutdown soon.\n\n");
	fprintf(icfp, "Test of XCreateIC <must> is failed.\n");
	fprintf(icfp, "Cannot create IC. NULL returned.\n\n");
	*flag = CRICERR;
	return (NULL);
    }
    
    prstatus("Creating NestedList for get...");
			/* Create NestedList for GetICValues() (may) */
    gpe_nelist = XVaCreateNestedList(DUMMY, XNArea, &gparea,
				            XNSpotLocation, &gspot,
				            XNColormap, &gpcmap,
				            XNForeground, &gpfg,
				            XNBackground, &gpbg,
				            XNBackgroundPixmap, &gppix,
				            XNFontSet, &gpfset,
				            XNLineSpace, &glspc,
				            XNCursor, &gpcursor,
				     NULL);
    gst_nelist = XVaCreateNestedList(DUMMY, XNArea, &gsarea,
				            XNColormap, &gscmap,
				            XNForeground, &gsfg,
				            XNBackground, &gsbg,
				            XNBackgroundPixmap, &gspix,
				            XNFontSet, &gsfset,
				            XNCursor, &gscursor,
				     NULL);
    prstatus("done.");
    prstatus("Test of XGetICValues()..."); /* test of GetICValues() (may) */
    XGetICValues(ic, XNInputStyle, &gistyle,
		     XNClientWindow, &gclient,
		     XNFocusWindow, &gfocus,
		     XNResourceName, &gres.name,
		     XNResourceClass, &gres.class,
		     XNPreeditAttributes, gpe_nelist,
		     XNStatusAttributes, gst_nelist,
		 NULL);
    prstatus("done.");

    res = 0;			/* verify valuables */
    sum = 0;
    vover[0] = mkstruct(IS, HEX, uistyle, gistyle);
    vover[1] = mkstruct(CW, HEX, uclient, gclient);
    vover[2] = mkstruct(FW, HEX, ufocus, gfocus);
    vover[3] = mkstruct(RN, STR, ures.name, gres.name);
    vover[4] = mkstruct(RC, STR, ures.class, gres.class);
    vover[5] = mkstruct(PAX,DEC, upea.area.x, gparea.x);
    vover[6] = mkstruct(PAY,DEC, upea.area.y,gparea.y);
    vover[7] = mkstruct(PAW, DEC, upea.area.width, gparea.width);
    vover[8] = mkstruct(PAH, DEC, upea.area.height, gparea.height);
    vover[9] = mkstruct(PSLX, DEC, upea.spot.x, gspot.x);
    vover[10] = mkstruct(PSLY, DEC, upea.spot.y, gspot.y);
    vover[11] = mkstruct(PCM, HEX, upea.cmap, gpcmap);
    vover[12] = mkstruct(PFG, HEX, upea.fg, gpfg);
    vover[13] = mkstruct(PBG, HEX, upea.bg, gpbg);
    vover[14] = mkstruct(PBP, HEX, upea.bgpm, gppix);
    vover[15] = mkstruct(PFS, HEX, upea.fset, gpfset);
    vover[16] = mkstruct(PLS, DEC, upea.lspc, glspc);
    vover[17] = mkstruct(PCU, HEX, upea.cursor, gpcursor);
    vover[18] = mkstruct(SAX, DEC, usta.area.x, gsarea.x);
    vover[19] = mkstruct(SAY, DEC, usta.area.y, gsarea.y);
    vover[20] = mkstruct(SAW, DEC, usta.area.width, gsarea.width);
    vover[21] = mkstruct(SAH, DEC, usta.area.height, gsarea.height);
    vover[22] = mkstruct(SCM, HEX, usta.cmap, gscmap);
    vover[23] = mkstruct(SFG, HEX, usta.fg, gsfg);
    vover[24] = mkstruct(SBG, HEX, usta.bg, gsbg);
    vover[25] = mkstruct(SBP, HEX, usta.bgpm, gspix);
    vover[26] = mkstruct(SFS, HEX, usta.fset, gsfset);
    vover[27] = mkstruct(SCU, HEX, usta.cursor, gscursor);
    for (cnt = 0; cnt < OVERMAY; cnt++) {
	res = verval(vover[cnt]);
	sum += res;
    }

    cls(prdisp);
    prveres(sum);
    if (sum != NOERR) {
	prprint("Test of XCreateIC() <may> is failed.\n");
	prprint("This program will shutdown soon.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <may> is failed.\n\n");
	*flag = GICERR;
	return (NULL);
    } else {
	prprint("Test of XCreateIC() <may> is succeeded.\n");
	prprint("move next test.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <may> is succeeded.\n\n");
    }
    XFree((char *)cpe_nelist);
    XFree((char *)cst_nelist);
    XFree((char *)gpe_nelist);
    XFree((char *)gst_nelist);
    return (ic);
}

XIC	offmust(im, lfdir,flag)	/* for OFF-the-spot mode (MUST) */
XIM	im;
char	*lfdir;
FLAG	*flag;
{
    XIC		ic;
    static int	cntlog = 1;
    char	logfile[MAX_BUF];
    int		res;
    int		cnt;
    int		sum;
    XIMStyle	gistyle;
    XFontSet	gpfset, gsfset;
    XVaNestedList	cpe_nelist, cst_nelist;
    XVaNestedList	gpe_nelist, gst_nelist;
    VALUABLE	*voff[OFFMUST];
    	
    prmode("X11R5 Input Method Test Program : <<off-the-spot mode>>");
    prstatus("Creating log file for ic..."); /* open log file for IC test */
    sprintf(logfile, "%s/ticoff.log%d", lfdir, cntlog);
    icfp = fopen(logfile, "w");
    prstatus("done.");
    cntlog++;
    prstatus("Creating NestedList for set...");
				/* create NestedList for create IC (must) */
    uistyle = (XIMPreeditArea | XIMStatusArea); /* initialize valuables */
    upea.fset = fontset[0];
    usta.fset = fontset[1];
    cpe_nelist = XVaCreateNestedList(DUMMY, XNFontSet, upea.fset, NULL);
    cst_nelist = XVaCreateNestedList(DUMMY, XNFontSet, usta.fset, NULL);
    prstatus("done.");

    prstatus("Creating ic...");	/* test of XCreateIC() (must) */
    ic = XCreateIC(im, XNInputStyle, uistyle,
		       XNPreeditAttributes, cpe_nelist,
		       XNStatusAttributes, cst_nelist,
		       NULL);
    prstatus("done.");
    if (ic == NULL) {		/* transaction of error */
	cls(prdisp);
	prprint("Test of XCreateIC() <must> is failed.\n");
	prprint("Cannot create IC. NULL returned.\n");
	prprint("This program will shutdown soon.\n\n");
	fprintf(icfp, "Test of XCreateIC <must> is failed.\n");
	fprintf(icfp, "Cannot create IC. NULL returned.\n\n");
	*flag = CRICERR;
	return (NULL);
    }
    prstatus("Creating NestedList for get...");
			/* Create NestedList for GetICValues() (must) */
    gpe_nelist = XVaCreateNestedList(DUMMY, XNFontSet, &gpfset, NULL);
    gst_nelist = XVaCreateNestedList(DUMMY, XNFontSet, &gsfset, NULL);
    prstatus("done.");

    prstatus("Test of GetICValues()...");    /* test of GetICValues() (must) */
    XGetICValues(ic, XNInputStyle, &gistyle,
		     XNPreeditAttributes, gpe_nelist,
		     XNStatusAttributes, gst_nelist,
		     NULL);
    prstatus("done.");

    res = 0;			/* verify valuables */
    sum = 0;
    voff[0] = mkstruct(IS, HEX, uistyle, gistyle);
    voff[1] = mkstruct(PFS, HEX, upea.fset, gpfset);
    voff[2] = mkstruct(SFS, HEX, usta.fset, gsfset);
    for (cnt = 0; cnt < OFFMUST; cnt++) {
	res = verval(voff[cnt]);
	sum += res;
    }

    cls(prdisp);
    prveres(sum);
    if (sum != NOERR) {
	prprint("Test of XCreateIC() <must> is failed.\n");
	prprint("This program will shutdown soon.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <must> is failed.\n\n");
	*flag = GICERR;
	return (NULL);
    } else {
	prprint("Test of XCreateIC() <must> is succeeded.\n");
	prprint("move next test.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <must> is succeeded.\n\n");
    }
    XFree((char *)cpe_nelist);
    XFree((char *)cst_nelist);
    XFree((char *)gpe_nelist);
    XFree((char *)gst_nelist);
    return (ic);
}


XIC	offmay(im, flag)	/* for OFF-the-spot mode (MAY) */
XIM	im;
FLAG 	*flag;
{
    XIC		ic;
    int		res;
    int		cnt;
    int		sum;
    XIMStyle	gistyle;
    Window	gclient;
    Window	gfocus;
    RESOURCE	gres;
    XRectangle	gparea, gsarea;
    Colormap	gpcmap, gscmap;
    u_long	gpfg, gpbg, gsfg, gsbg;
    Pixmap	gppix, gspix;
    XFontSet	gpfset, gsfset;
    Cursor	gpcursor, gscursor;
    XVaNestedList	cpe_nelist, cst_nelist;
    XVaNestedList	gpe_nelist, gst_nelist;
    VALUABLE 	*voff[OFFMAY];
    	
    prmode("X11R5 Input Method Test Program : <<off-the-spot mode>>");
    prstatus("Creating NestedList for set...");
				/* create NestedList for create IC (may) */
    uistyle = (XIMPreeditArea | XIMStatusArea); /* initialize valuables */
    uclient = window1.win;
    ufocus = window3.win;
    ufname = window3.name;
    ures.name = "xim";
    ures.class = "Xim";
    usta.area.x = WIN_ITV;
    usta.area.y = CALCY((CALCY(W1_HEIGHT, NW_HEIGHT)), PS_HEIGHT);
    usta.area.width = ST_WIDTH;
    usta.area.height = PS_HEIGHT;
    usta.cmap = cmap[0];
    usta.fg = pixel[1][0];
    usta.bg = pixel[1][3];
    usta.fset = fontset[1];
    usta.bgpm = pix[1];
    usta.cursor = cursor[1];
    upea.area.x = ST_WIDTH + 2 * WIN_ITV;
    upea.area.y = usta.area.y;
    upea.area.width = WPOFF1;
    upea.area.height = PS_HEIGHT;
    upea.cmap = cmap[0];
    upea.fg = pixel[1][3];
    upea.bg = pixel[1][0];
    upea.bgpm = pix[0];
    upea.fset = fontset[0];
    upea.cursor = cursor[0];
    cpe_nelist = XVaCreateNestedList(DUMMY, XNArea, &upea.area,
				            XNColormap, upea.cmap,
				            XNForeground, upea.fg,
				            XNBackground, upea.bg,
				            XNBackgroundPixmap, upea.bgpm,
				            XNFontSet, upea.fset,
				            XNCursor, upea.cursor,
				     NULL);
    cst_nelist = XVaCreateNestedList(DUMMY, XNArea, &usta.area,
				            XNColormap, usta.cmap,
				            XNForeground, usta.fg,
				            XNBackground, usta.bg,
				            XNBackgroundPixmap, usta.bgpm,
				            XNFontSet, usta.fset,
				            XNCursor, usta.cursor,
				     NULL);
    prstatus("done.");

    prstatus("Creating ic...");	/* test of XCreateIC() (may) */
    ic = XCreateIC(im, XNInputStyle, uistyle,
		       XNClientWindow, uclient,
		       XNFocusWindow, ufocus,
		       XNResourceName, ures.name,
		       XNResourceClass, ures.class,
		       XNPreeditAttributes, cpe_nelist,
		       XNStatusAttributes, cst_nelist,
		       NULL);
    prstatus("done.");
    if (ic == NULL) {		/* transaction of error */
	cls(prdisp);
	prprint("Test of XCreateIC() <must> is failed.\n");
	prprint("Cannot create IC. NULL returned.\n");
	prprint("This program will shutdown soon.\n\n");
	fprintf(icfp, "Test of XCreateIC <must> is failed.\n");
	fprintf(icfp, "Cannot create IC. NULL returned.\n\n");
	*flag = CRICERR;
	return (NULL);
    }
    prstatus("Creating NestedList for get...");
			/* Create NestedList for GetICValues() (may) */
    gpe_nelist = XVaCreateNestedList(DUMMY, XNArea, &gparea,
				            XNColormap, &gpcmap,
				            XNForeground, &gpfg,
				            XNBackground, &gpbg,
				            XNBackgroundPixmap, &gppix,
				            XNFontSet, &gpfset,
				            XNCursor, &gpcursor,
				     NULL);
    gst_nelist = XVaCreateNestedList(DUMMY, XNArea, &gsarea,
				            XNColormap, &gscmap,
				            XNForeground, &gsfg,
				            XNBackground, &gsbg,
				            XNBackgroundPixmap, &gspix,
				            XNFontSet, &gsfset,
				            XNCursor, &gscursor,
				     NULL);
    prstatus("done.");
    prstatus("Test of XGetICValues()..."); /* test of GetICValues() (may) */
    XGetICValues(ic, XNInputStyle, &gistyle,
		     XNClientWindow, &gclient,
		     XNFocusWindow, &gfocus,
		     XNResourceName, &gres.name,
		     XNResourceClass, &gres.class,
		     XNPreeditAttributes, gpe_nelist,
		     XNStatusAttributes, gst_nelist,
		 NULL);
    prstatus("done.");

    res = 0;			/* verify valuables */
    sum = 0;
    voff[0] = mkstruct(IS, HEX, uistyle, gistyle);
    voff[1] = mkstruct(CW, HEX, uclient, gclient);
    voff[2] = mkstruct(FW, HEX, ufocus, gfocus);
    voff[3] = mkstruct(RN, STR, ures.name, gres.name);
    voff[4] = mkstruct(RC, STR, ures.name, gres.name);
    voff[5] = mkstruct(PAX,DEC, upea.area.x, gparea.x);
    voff[6] = mkstruct(PAY,DEC, upea.area.y,gparea.y);
    voff[7] = mkstruct(PAW, DEC, upea.area.width, gparea.width);
    voff[8] = mkstruct(PAH, DEC, upea.area.height, gparea.height);
    voff[9] = mkstruct(PCM, HEX, upea.cmap, gpcmap);
    voff[10] = mkstruct(PFG, HEX, upea.fg, gpfg);
    voff[11] = mkstruct(PBG, HEX, upea.bg, gpbg);
    voff[12] = mkstruct(PBP, HEX, upea.bgpm, gppix);
    voff[13] = mkstruct(PFS, HEX, upea.fset, gpfset);
    voff[14] = mkstruct(PCU, HEX, upea.cursor, gpcursor);
    voff[15] = mkstruct(SAX, DEC, usta.area.x, gsarea.x);
    voff[16] = mkstruct(SAY, DEC, usta.area.y, gsarea.y);
    voff[17] = mkstruct(SAW, DEC, usta.area.width, gsarea.width);
    voff[18] = mkstruct(SAH, DEC, usta.area.height, gsarea.height);
    voff[19] = mkstruct(SCM, HEX, usta.cmap, gscmap);
    voff[20] = mkstruct(SFG, HEX, usta.fg, gsfg);
    voff[21] = mkstruct(SBG, HEX, usta.bg, gsbg);
    voff[22] = mkstruct(SBP, HEX, usta.bgpm, gspix);
    voff[23] = mkstruct(SFS, HEX, usta.fset, gsfset);
    voff[24] = mkstruct(SCU, HEX, usta.cursor, gscursor);
    for (cnt = 0; cnt < OFFMAY; cnt++) {
	res = verval(voff[cnt]);
	sum += res;
    }
    cls(prdisp);
    prveres(sum);
    if (sum != NOERR) {
	prprint("Test of XCreateIC() <may> is failed.\n");
	prprint("This program will shutdown soon.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <may> is failed.\n\n");
	*flag = GICERR;
	return (NULL);
    } else {
	prprint("Test of XCreateIC() <may> is succeeded.\n");
	prprint("move next test.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <may> is succeeded.\n\n");
    }
    XFree((char *)cpe_nelist);
    XFree((char *)cst_nelist);
    XFree((char *)gpe_nelist);
    XFree((char *)gst_nelist);
    return (ic);
}

XIC	rootmust(im, lfdir, flag)/* for ROOT-window mode (MUST) */
XIM	im;
char	*lfdir;
FLAG 	*flag;
{
    XIC		ic;
    static int	cntlog = 1;
    char	logfile[MAX_BUF];
    int		res;
    int		cnt;
    int		sum;
    XIMStyle	gistyle;
    VALUABLE	*vroot[ROOTMUST];
    
    prmode("X11R5 Input Method Test Program : <<root-window mode>>");
    prstatus("Creating log file for ic..."); /* open log file for IC test */
    sprintf(logfile, "%s/ticroot.log%d", lfdir, cntlog);
    icfp = fopen(logfile, "w");
    cntlog++;
    prstatus("done.");
    uistyle = (XIMPreeditNothing | XIMStatusNothing); /* initialize valuables */

    prstatus("Creating ic...");	/* test of XCreateIC() (must) */
    ic = XCreateIC(im, XNInputStyle, uistyle, NULL);
    prstatus("done.");
    if (ic == NULL) {		/* transaction of error */
	cls(prdisp);
	prprint("Test of XCreateIC() <must> is failed.\n");
	prprint("Cannot create IC. NULL returned.\n");
	prprint("This program will shutdown soon.\n\n");
	fprintf(icfp, "Test of XCreateIC <must> is failed.\n");
	fprintf(icfp, "Cannot create IC. NULL returned.\n\n");
	*flag = CRICERR;
	return (NULL);
    }
    prstatus("Test of GetICValues()...");    /* test of GetICValues() (must) */
    XGetICValues(ic, XNInputStyle, &gistyle, NULL);
    prstatus("done.");

    res = 0;			/* verify valuables */
    sum = 0;
    vroot[0] = mkstruct(IS, HEX, uistyle, gistyle);
    for (cnt = 0; cnt < ROOTMUST; cnt++) {
	res = verval(vroot[cnt]);
	sum += res;
    }
    cls(prdisp);
    prveres(sum);
    if (sum != NOERR) {
	prprint("Test of XCreateIC() <must> is failed.\n");
	prprint("This program will shutdown soon.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <must> is failed.\n\n");
	*flag = GICERR;
	return (NULL);
    } else {
	prprint("Test of XCreateIC() <must> is succeeded.\n");
	prprint("move next test.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <must> is succeeded.\n\n");
    }
    return (ic);
}


XIC	rootmay(im, flag)	/* for ROOT-window mode (MAY) */
XIM	im;
FLAG	*flag;
{
    XIC		ic;
    int		res;
    int		cnt;
    int		sum;
    XIMStyle	gistyle;
    Window	gclient;
    Window	gfocus;
    RESOURCE	gres;
    Colormap	gpcmap, gscmap;
    u_long	gpfg, gpbg, gsfg, gsbg;
    Pixmap		gppix, gspix;
    XFontSet		gpfset, gsfset;
    Cursor		gpcursor, gscursor;
    XVaNestedList	cpe_nelist, cst_nelist;
    XVaNestedList	gpe_nelist, gst_nelist;
    VALUABLE 	*vroot[ROOTMAY];

    prmode("X11R5 Input Method Test Program : <<root-window mode>>");
    prstatus("Creating NestedList for set...");
				/* create NestedList for create IC (may) */
    uistyle = (XIMPreeditNothing | XIMStatusNothing); /* initialize valuables */
    uclient = window1.win;
    ufocus = window3.win;
    ufname = window3.name;
    ures.name = "xim";
    ures.class = "Xim";
    usta.cmap = cmap[0];
    usta.fg = pixel[1][0];
    usta.bg = pixel[1][3];
    usta.bgpm = pix[1];
    usta.fset = fontset[1];
    usta.cursor = cursor[1];
    upea.cmap = cmap[0];
    upea.fg = pixel[1][3];
    upea.bg = pixel[1][0];
    upea.bgpm = pix[0];
    upea.fset = fontset[0];
    upea.cursor = cursor[0];
    cpe_nelist = XVaCreateNestedList(DUMMY, XNColormap, upea.cmap,
				            XNForeground, upea.fg,
				            XNBackground, upea.bg,
				            XNBackgroundPixmap, upea.bgpm,
				            XNFontSet, upea.fset,
				            XNCursor, upea.cursor,
				     NULL);
    cst_nelist = XVaCreateNestedList(DUMMY, XNColormap, usta.cmap,
				            XNForeground, usta.fg,
				            XNBackground, usta.bg,
				            XNBackgroundPixmap, usta.bgpm,
				            XNFontSet, usta.fset,
				            XNCursor, usta.cursor,
				     NULL);
    prstatus("done.");

    prstatus("Creating ic...");	/* test of XCreateIC() (may) */
    ic = XCreateIC(im, XNInputStyle, uistyle,
		       XNClientWindow, uclient,
		       XNFocusWindow, ufocus,
		       XNResourceName, ures.name,
		       XNResourceClass, ures.class,
		       XNPreeditAttributes, cpe_nelist,
		       XNStatusAttributes, cst_nelist,
		       NULL);
    prstatus("done.");
    if (ic == NULL) {		/* transaction of error */
	cls(prdisp);
	prprint("Test of XCreateIC() <must> is failed.\n");
	prprint("Cannot create IC. NULL returned.\n");
	prprint("This program will shutdown soon.\n\n");
	fprintf(icfp, "Test of XCreateIC <must> is failed.\n");
	fprintf(icfp, "Cannot create IC. NULL returned.\n\n");
	*flag = CRICERR;
	return (NULL);
    }
    
    prstatus("Creating NestedList for get...");
			/* Create NestedList for GetICValues() (may) */
    gpe_nelist = XVaCreateNestedList(DUMMY, XNColormap, &gpcmap,
				            XNForeground, &gpfg,
				            XNBackground, &gpbg,
				            XNBackgroundPixmap, &gppix,
				            XNFontSet, &gpfset,
				            XNCursor, &gpcursor,
				     NULL);
    gst_nelist = XVaCreateNestedList(DUMMY, XNColormap, &gscmap,
				            XNForeground, &gsfg,
				            XNBackground, &gsbg,
				            XNBackgroundPixmap, &gspix,
				            XNFontSet, &gsfset,
				            XNCursor, &gscursor,
				     NULL);
    prstatus("done.");
    prstatus("Test of XGetICValues()..."); /* test of GetICValues() (may) */
    XGetICValues(ic, XNInputStyle, &gistyle,
		     XNClientWindow, &gclient,
		     XNFocusWindow, &gfocus,
		     XNResourceName, &gres.name,
		     XNResourceClass, &gres.class,
		     XNPreeditAttributes, gpe_nelist,
		     XNStatusAttributes, gst_nelist,
		 NULL);
    prstatus("done.");

    res = 0;			/* verify valuables */
    sum = 0;
    vroot[0] = mkstruct(IS, HEX, uistyle, gistyle);
    vroot[1] = mkstruct(CW, HEX, uclient, gclient);
    vroot[2] = mkstruct(FW, HEX, ufocus, gfocus);
    vroot[3] = mkstruct(RN, STR, ures.name, gres.name);
    vroot[4] = mkstruct(RC, STR, ures.class, gres.class);
    vroot[5] = mkstruct(PCM, HEX, upea.cmap, gpcmap);
    vroot[6] = mkstruct(PFG, HEX, upea.fg, gpfg);
    vroot[7] = mkstruct(PBG, HEX, upea.bg, gpbg);
    vroot[8] = mkstruct(PBP, HEX, upea.bgpm, gppix);
    vroot[9] = mkstruct(PFS, HEX, upea.fset, gpfset);
    vroot[10] = mkstruct(PCU, HEX, upea.cursor, gpcursor);
    vroot[11] = mkstruct(SCM, HEX, usta.cmap, gscmap);
    vroot[12] = mkstruct(SFG, HEX, usta.fg, gsfg);
    vroot[13] = mkstruct(SBG, HEX, usta.bg, gsbg);
    vroot[14] = mkstruct(SBP, HEX, usta.bgpm, gspix);
    vroot[15] = mkstruct(SFS, HEX, usta.fset, gsfset);
    vroot[16] = mkstruct(SCU, HEX, usta.cursor, gscursor);
    for (cnt = 0; cnt < ROOTMAY; cnt++) {
	res = verval(vroot[cnt]);
	sum += res;
    }
    cls(prdisp);
    prveres(sum);
    if (sum != NOERR) {
	prprint("Test of XCreateIC() <may> is failed.\n");
	prprint("This program will shutdown soon.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <may> is failed.\n\n");
	*flag = GICERR;
	return (NULL);
    } else {
	prprint("Test of XCreateIC() <may> is succeeded.\n");
	prprint("move next test.\n\n");
	prverlog(sum);
	fprintf(icfp, "...Test of XCreateIC <may> is succeeded.\n\n");
    }
    XFree((char *)cpe_nelist);
    XFree((char *)cst_nelist);
    XFree((char *)gpe_nelist);
    XFree((char *)gst_nelist);
    return (ic);
}

void	destic(ic)		/* DESTroy IC */
XIC	ic;
{
    if (focuson == OK) {
	XUnsetICFocus(ic);
	focuson = NG;
	cwcolor(ufname, pixel[1][1]);
    }
    XDestroyIC(ic);
}

void	fclic()		/* log File Close for IC */
{
    fclose(icfp);
}
