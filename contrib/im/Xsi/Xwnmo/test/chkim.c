/*
 * $Id: chkim.c,v 1.1 1991/09/11 06:48:18 ohm Exp $
 */
/*
 * X11R5 Input Method Test Program
 * chkim.c v 1.0   Tue Mar  5 16:18:15 JST 1991
 */

/*
 * Copyright 1991 by OMRON Corporation
 *
 *      Author: Takashi Inoue    OMRON Corporation
 *                               takashi@ari.ncl.omron.co.jp
 *
 *    		Shoji kamada	 OMRON Corporation
 *			 	 kamada@ari.ncl.omron.co.jp
 */

#include "exvalue.h"

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/Xresource.h>

XIM	openim(lfdir, flag)		/* check of XOpenIM */
char	*lfdir;
FLAG	*flag;
{
    XIM		im;
    char	*home;
    char	logfile[MAX_BUF];
    char	dbsrc[MAX_BUF];
    XrmDatabase rdb;
    XIMStyles	ximstyle;
    XIMStyle	*style;
    char	*languages;
    int		cnt;
    
    char	*getenv();

    prmode("X11R5 Input Method Test Program : <<IM Mode>>");
    prstatus("Creating log file for im..."); /* open log file for IM test */
    sprintf(logfile, "%s/tim.log", lfdir);
    imfp = fopen(logfile, "w");
    prstatus("done.");
    if (imfp == NULL) {
	cls(prdisp);
	prerrld(logfile);
    }

    prstatus("Creating resource database..."); /* create resource database */
    home = getenv("HOME");
    sprintf(dbsrc, "%s/.Xdefaults", home);
    rdb = XrmGetFileDatabase(dbsrc);
    prstatus("done.");

    prstatus("Creating input method..."); /* open IM */
    im = XOpenIM(dpy, rdb, "xim", "XIM");
    prstatus("done.");
    cls(prdisp);
    if (im == NULL) {	/* for XOpenIM error */
	prprint("Test of XOpenIM() is failed.\n\n");
	prprint("Check environmental valuable \"XNLSPATH\"\n\n");
	prprint("and make sure to run \"xwnmo\".\n\n");
	prprint("This program will shutdown soon.\n\n");
	fprintf(imfp, "Test of XOpenIM() is failed.\n");
	fprintf(imfp, "\"NULL\" returned.\n\n");
	*flag = OPIMERR;
	return (im);
    }
    prprint("Test of XOpenIM() is succeeded.\n"); /* for XOpenIM success */
    prprint("Move next test.\n\n");
    fprintf(imfp, "Test of XOpenIM() is succeeded.\n");
    fprintf(imfp, "im : 0x%X\n\n", im);

    languages = 0;		/* initialization of valuables */
    prstatus("Test of XGetIMValues()..."); /* test of XGetIMValues */
    XGetIMValues(im, XNQueryInputStyle, &ximstyle,
		     XNQueryLanguage, &languages,
		     NULL);
    prstatus("done.");
    if (ximstyle.count_styles == 0) {	/* for XGetIMValues error */
	prprint("Test of XGetIMValues() is failed.\n");
	prprint("This program will shutdown soon.\n\n");
	fprintf(imfp, "Test of XGetIMValues() is failed. ");
	fprintf(imfp, "ximstyle returned \"NULL\".\n\n");
	*flag = GIMERR;
	return (NULL);
    }
    prprint("Test of XGetIMValues() is succeeded.\n");
    prprint("Move next test.\n\n");
    				/* for XGetIMValues success */
    fprintf(imfp, "Test of XGetIMValues() is succeeded.\n");
    for (cnt = 0, style = ximstyle.supported_styles; 
	 cnt < ximstyle.count_styles; cnt++, style++)  
	fprintf(imfp, "input style : 0x%X\n", *style);	/* write values to log file */
    fprintf(imfp, "You can use languages in this IM : %s\n\n", languages);
    return (im);
}

void	closeim(im)		/* CLOSE IM */
XIM    im;
{
    XCloseIM(im);  
}

void	fclim()		/* log File CLose for IM */
{
    fclose(imfp);
}






