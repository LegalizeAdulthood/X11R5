/*
 * xco.c	Routines to manipulate selection of foreground and background
 *              colors
 * 
 * author	J. Tim Korb	jtk@cs.purdue.edu
 *              Richard Bingle	bingle@cs.purdue.edu
 *
 * warning      The source for this program isn't very pretty, but the
 *              output is.  
*/

/***********************************************************************
* Copyright 1990 by Purdue University, J. Tim Korb, and Richard Bingle.
* All rights reserved.  Some individual files may be covered by other 
* copyrights.
* 
* Redistribution and use in source and binary forms are permitted
* provided that this entire copyright notice is duplicated in all such
* copies, and that any documentation, announcements, and other
* materials related to such distribution and use acknowledge that the
* software was developed at Purdue University, W. Lafayette, IN.  
* Neither the name of the University nor the name of the authors may 
* be used to endorse or promote products derived from this material without 
* specific prior written permission.  
*
* THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
* MERCHANTIBILITY AND FITNESS FOR ANY PARTICULAR PURPOSE.
************************************************************************/

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xos.h>

#ifndef RGBDATABASE
#define RGBDATABASE	"/usr/lib/X11/rgb.txt"
#endif  RGBDATABASE
#define CHARMAX		50
#define COLMAX		1000
#define DEFGEOM		"750x750+100+100"
#define FORE		-1
#define BACK		-2
#define pdBrdr 		2

#include "lleft.bm"
#include "lright.bm"

struct rgb_struct {
    int red;
    int green;
    int blue;
};

Display *pdp;			/* pointer to open display 		*/
Colormap cm;			/* colormap 				*/
Window wnPrevious, wnNext;	/* previous and next button windows 	*/
int iBase;			/* base offset into color arrays 	*/
int MaxColor;			/* number of colors 			*/
int cColorMax;			/* number of colors to show at once 	*/
int cColorScroll;		/* number of colors to scroll 		*/
Window rgwn[COLMAX];		/* array of choosable color windows 	*/
unsigned long rgco[COLMAX];	/* array of pixel values for colors 	*/
struct rgb_struct rgb[COLMAX];	/* array of RGB values 			*/
struct rgb_struct rgb_fg;	/* RGB values of original foreground 	*/
struct rgb_struct rgb_bg;	/* RGB values of original background 	*/
char *rgsbColor[COLMAX];	/* array of color names 		*/
XContext cxNum;			/* maps windows to number in rgsbColor 	*/

extern Display *ParseXArgs();
extern char *malloc();
char *GetColorName();

main(csb, rgsb)
int csb;
char *rgsb[];
{
    Window wn;			/* main window 				*/
    Window fore_win;		/* foreground window 			*/
    Window back_win;		/* background window 			*/
    char *fore;			/* foreground color name		*/
    char *back;			/* background color name		*/
    GC gc;			/* graphics context			*/
    int depth;			/* depth of display			*/
    XEvent ev;			/* event structure 			*/
    Pixmap pix;			/* pixmap for prev and next buttons	*/
    int sn;			/* screen number			*/
    int i;			/* loop variable			*/
    int j,k,l;			/* red, green, and blue components 	*/
    int found;			/* true if (r,g,b) already in table	*/
    int x;			/* loop variable 			*/
    int coFg, coBg;		/* pixel values 			*/
    XColor xc;			/* color structure for storing colors	*/
    FILE *fp;			/* file pointer for rgb database	*/
    KeySym ks;			/* keysym for keyboard input		*/
    XSizeHints sh;		/* size hints structure 		*/
    XClassHint ch;		/* class hints structure		*/
    int flags;			/* return flags 			*/
    char *sb;			/* buffer for reading in data 		*/
    char *sbdisplay;		/* display name 			*/
    char *sbname;		/* program name 			*/
    char *sbtitle;		/* window and icon name 		*/
    char *sbgeometry;		/* geometry string 			*/
    char *sbhelp;		/* help on or off 			*/
    char *sbColorScroll;	/* number to scroll 			*/
    char *sbColorMax;		/* number to show 			*/
    char *sbfg;			/* default foreground 			*/
    char *sbbg;			/* default background 			*/

	/* get space for string buffers */

    sb = 	     malloc(CHARMAX);
    sbdisplay =      malloc(CHARMAX);
    sbname =         malloc(CHARMAX);
    sbtitle =        malloc(CHARMAX);
    sbgeometry =     malloc(CHARMAX);
    sbhelp =         malloc(CHARMAX);
    sbColorScroll =  malloc(CHARMAX);
    sbColorMax =     malloc(CHARMAX);
    sbfg =           malloc(CHARMAX);
    sbbg =           malloc(CHARMAX);

    for (i=0; i<500; i++)
	rgwn[i] = None;
    
	/* intialize X options */

    InitXOption("-display", &sbdisplay, "", "display to use", "display", 
			"display", "Display");
    InitXOption("-name", &sbname, rgsb[0], "name of command", "name",
			"name", "Name");
    InitXOption("-title", &sbtitle, "xco", "title of window and icon", "title",
			 "title", "Title");
    InitXOption("-geometry", &sbgeometry, "", "geometry string", 
			"geometry", "geometry", "Geometry");
    InitXOption("-help", &sbhelp, "off", "print help", "help", "help",
			 "Help");
    InitXOption("-scroll", &sbColorScroll, "64", "number of colors to scroll",
		         "scroll", "scroll", "Scroll");
    InitXOption("-show", &sbColorMax, "64", "number of colors to show",
			 "show", "show", "Show");
    InitXOption("-fg", &sbfg, "black", "default foreground color",
			"foreground", "foreground", "Foreground");
    InitXOption("-bg", &sbbg, "white", "default background color",
			"background", "background", "Background");

	/* parse arguments */

    pdp = ParseXArgs("xco", "Xco", &csb, rgsb);
    sn = DefaultScreen(pdp);
    cm = DefaultColormap(pdp, sn);
    depth = DefaultDepth(pdp, sn);

    if (!strcmp(sbhelp, "on") || csb > 1) {
	PrintOptions();
	exit(0);
    }

	/* set up geometry */

    flags = XGeometry(pdp, sn, sbgeometry, DEFGEOM, 2, 1, 1, 0, 0,
		&(sh.x), &(sh.y), &(sh.width), &(sh.height));
    if ((flags & XValue) || (flags & YValue)) {
	sh.flags = USPosition;
    }
    else {
	sh.flags = PPosition;
    }
    if ((flags & WidthValue) || (flags & HeightValue)) {
	sh.flags = sh.flags | USSize;
    }
    else {
        sh.flags = sh.flags | PSize;
    }

    {
    int ec, rgco1[2];

    ec = XAllocColorCells(pdp, cm, False, NULL, 0, rgco1, 2);
    if (ec == 0)
	fprintf(stderr, "%s: Not enough color cells available\n", rgsb[0]), 
		exit(1);

    coFg = rgco1[0];
    coBg = rgco1[1];
    XParseColor(pdp, cm, sbfg, &xc);
    xc.pixel = coFg;
    rgb_fg.red = xc.red; rgb_fg.green = xc.green; rgb_fg.blue = xc.blue;
    XStoreColor(pdp, cm, &xc);
    XParseColor(pdp, cm, sbbg, &xc);
    xc.pixel = coBg;
    rgb_bg.red = xc.red; rgb_bg.green = xc.green; rgb_bg.blue = xc.blue;
    XStoreColor(pdp, cm, &xc);
    }

    cColorMax = atoi(sbColorMax);
    if (cColorMax > 128)
	cColorMax = 128;
    if (cColorMax % 2)
	cColorMax++;
    cColorScroll = atoi(sbColorScroll);
    
    wn = XCreateSimpleWindow(pdp, DefaultRootWindow(pdp),
			     sh.x, sh.y, sh.width, sh.height, 2, coFg, coBg);
    ch.res_name = sbtitle;
    ch.res_class = sbtitle;
    XSetStandardProperties(pdp, wn, sbtitle, sbtitle,
			   None, rgsb, csb, NULL);
    XSetClassHint(pdp, wn, &ch);
    XSetNormalHints(pdp, wn, &sh);
    
    gc = XCreateGC(pdp, wn, 0, NULL);
    XSetState(pdp, gc, coFg, coBg, GXcopy, ~0);

    wnPrevious = XCreateSimpleWindow(pdp, wn, 10, 10, lleft_width, lleft_height,
 					2, coFg, coBg);
    wnNext = XCreateSimpleWindow(pdp, wn, 100, 10, lright_width, lright_height,
					2, coFg, coBg);

    pix = XCreatePixmapFromBitmapData(pdp, wnPrevious, lleft_bits, lleft_width,
				      lleft_height, coFg, coBg, depth);
    XSetWindowBackgroundPixmap(pdp, wnPrevious, pix);

    pix = XCreatePixmapFromBitmapData(pdp, wnNext, lright_bits, lright_width,
				      lright_height, coFg, coBg, depth);
    XSetWindowBackgroundPixmap(pdp, wnNext, pix);

    XSelectInput(pdp, wnPrevious, ButtonPressMask);
    XSelectInput(pdp, wnNext, ButtonPressMask);

    
    if ((fp = fopen(RGBDATABASE, "r")) == NULL) {
	fprintf(stderr, "Cannot open %s\n", RGBDATABASE);
	exit(1);
    }

    i=0;
    while (fscanf(fp, "%d %d %d ", &j, &k, &l) != EOF) {
	found = 0;
	sb = fgets(sb, 80, fp);
	for (x=0; x<i; x++)
	    if (rgb[x].red == j && rgb[x].green == k && rgb[x].blue == l) {
		found++;
		break;
	    }
        if (!found) {
	    rgsbColor[i] = (char *) malloc ((unsigned) strlen(sb)+1);
	    strncpy(rgsbColor[i], sb, strlen(sb));
	    rgsbColor[i][strlen(sb)-1] = '\0';
	    rgb[i].red = j; rgb[i].green = k; rgb[i].blue = l;
	    i++;
        }
    }
    MaxColor = i;

    /* first page of colors */

    iBase = 0;
    cxNum = XUniqueContext();
    fore_win = XCreateSimpleWindow(pdp, wn, 0, 0, 1, 1, 0, coFg, coFg);
    XSaveContext(pdp, fore_win, cxNum, FORE);
    back_win = XCreateSimpleWindow(pdp, wn, 0, 0, 1, 1, 0, coBg, coBg);
    XSaveContext(pdp, back_win, cxNum, BACK);

    fore = malloc((unsigned) strlen(sbfg)+1); strcpy(fore,sbfg);
    back = malloc((unsigned) strlen(sbbg)+1); strcpy(back,sbbg);

    CreateColorWindows(pdp, wn);

    XSelectInput(pdp, wn, StructureNotifyMask | ExposureMask | KeyPressMask);
    XMapRaised(pdp, wn);
    free(sb);

    while (True) {
	XNextEvent(pdp, &ev);
	switch (ev.type) {
	  case MappingNotify:
	    XRefreshKeyboardMapping(&ev);
	    break;
	  case ConfigureNotify:
	    ResizeColorWindows(pdp, &ev, gc); /* could call less often */
	    for (i=0; i<cColorMax; i++)
		XMapWindow(pdp, rgwn[i]);
	    XMapWindow(pdp, wnPrevious);
	    XMapWindow(pdp, wnNext);
	    break;
	  case Expose:
	    if (ev.xexpose.count == 0)
		DrawLabels(pdp, wn, gc);
		PrintFore(fore, wn, gc);
		PrintBack(back, wn, gc);
	    break;
	  case ButtonPress:
	    if (ev.xbutton.window == wnPrevious)
		{
	    	    XFreeColors(pdp, cm, rgco, cColorMax, NULL);
	    	    iBase += MaxColor - cColorScroll;
	    	    XClearArea(pdp, wn, 0, 0, 0, 0, True);
	    	    CreateColorWindows(pdp, wn);
	    	    DrawLabels(pdp, wn, gc);
		}
            else if (ev.xbutton.window == wnNext)
		{
		    XFreeColors(pdp, cm, rgco, cColorMax, NULL);
		    iBase += cColorScroll;
		    XClearArea(pdp, wn, 0, 0, 0, 0, True);
		    CreateColorWindows(pdp, wn);
		    DrawLabels(pdp, wn, gc);
		}
	    else
	      if (ev.xbutton.button == 1) {
		  SetColor(coFg, ev.xbutton.window);
		  free(fore);
		  fore = GetColorName(ev.xbutton.window);
		  PrintFore(fore,wn,gc);
	          }
	      else if (ev.xbutton.button == 3) {
		  SetColor(coBg, ev.xbutton.window);
		  free(back);
		  back = GetColorName(ev.xbutton.window);
		  PrintBack(back,wn,gc);
	          }
	      else if (ev.xbutton.button == 2) {
		  SetColor(coFg, fore_win);
		  SetColor(coBg, back_win);
		  free(fore); free(back);
		  fore = malloc((unsigned) strlen(sbfg)+1); strcpy(fore, sbfg);
		  back = malloc((unsigned) strlen(sbbg)+1); strcpy(back, sbbg);
		  PrintFore(fore, wn, gc);
		  PrintBack(back, wn, gc);
		  }
	    break;
	  case EnterNotify:
	    if (ev.xcrossing.state & Button1Mask) {
		  SetColor(coFg, ev.xbutton.window);
		  free(fore);
		  fore = GetColorName(ev.xbutton.window);
		  PrintFore(fore,wn,gc);
		  }
	    else if (ev.xcrossing.state & Button3Mask) {
		  SetColor(coBg, ev.xbutton.window);
		  free(back);
		  back = GetColorName(ev.xbutton.window);
		  PrintBack(back,wn,gc);
		  }
	    break;
	  case KeyPress:
	    ks = XLookupKeysym(&ev, 0);
	    if (ks == XK_q)
		XCloseDisplay(pdp), exit(0);
	    else if (ks == XK_r) {
		sb = malloc((unsigned) strlen(fore)+1); strcpy(sb, fore);
		free(fore);
		fore = malloc((unsigned) strlen(back)+1); strcpy(fore, back);
		free(back);
		back = malloc((unsigned) strlen(sb)+1); strcpy(back, sb);
		free(sb);
		XStoreNamedColor(pdp, cm, fore, coFg, DoRed|DoGreen|DoBlue);
		XStoreNamedColor(pdp, cm, back, coBg, DoRed|DoGreen|DoBlue);
		PrintFore(fore, wn, gc);
		PrintBack(back, wn, gc);
	        }
	    break;
	    }
	}
}


/* ================================================================ */

/*
 * StoreColor(co, red, green, blue) - store red, green, and blue
 *                                    components in pixel co
*/

StoreColor(co, red, green, blue)
int co, red, green, blue;
{
    XColor xc;

    xc.pixel = co;
    xc.red = red<<8;
    xc.green = green<<8;
    xc.blue = blue<<8;
    xc.flags = DoRed|DoGreen|DoBlue;
    XStoreColor(pdp, cm, &xc);
}

/*
 * CreateColorWindows(pdp, wnTop) - create the color windows as children of
 *                                  wnTop on display pdp
*/

CreateColorWindows(pdp, wnTop)
Display *pdp;
Window wnTop;
{
    int i;
    unsigned long vm;	/* value mask for wa */
    XSetWindowAttributes wa;
    XColor xcExact, xcColor;

    vm = CWBorderPixel | CWBackPixel | CWWinGravity;
    wa.border_pixel = BlackPixel(pdp, DefaultScreen(pdp));
    wa.win_gravity = UnmapGravity;

    for (i = 0; i < cColorMax; i++) {
	if (XAllocNamedColor(pdp, cm, rgsbColor[(iBase+i) % MaxColor], 
	&xcExact, &xcColor) == 0) {
	    fprintf(stderr, "Could not allocate color '%s'\n", 
		rgsbColor[(iBase+i) % MaxColor]); exit(1);
	    }
	if (xcExact.red != xcColor.red || xcExact.green != xcColor.green ||
	    xcExact.blue != xcColor.blue)
	    fprintf(stderr, "color '%s' not found exactly\n",
		 rgsbColor[(iBase+i) % MaxColor]);
	wa.background_pixel = rgco[i] = xcColor.pixel;
	if (rgwn[i] == 0)
		rgwn[i] = XCreateWindow(pdp, wnTop, 0, 0, 1, 1, pdBrdr,
					     CopyFromParent, InputOutput,
					     CopyFromParent, vm, &wa);
	else
	    	XSetWindowBackground(pdp, rgwn[i], rgco[i]);
	XClearArea(pdp, rgwn[i], 0, 0, 0, 0, True);
	
	if (rgwn[i] == None)
	    fprintf(stderr, "Window creation failed!\n"), exit(1);

	XDeleteContext(pdp, rgwn[i], cxNum);
	XSaveContext(pdp, rgwn[i], cxNum, i);
	XSelectInput(pdp, rgwn[i], EnterWindowMask | 
		     ButtonPressMask | OwnerGrabButtonMask);
	}
}

/*
 * SetColor(co, wn) - determines what rgb triple to set the pixel co to and
 *                    calls StoreColor
*/

SetColor(co, wn)
int co;
Window wn;
{
    int i, red, green, blue;

    XFindContext(pdp, wn, cxNum, &i);
    if (i == FORE) {
	red = rgb_fg.red;
	green = rgb_fg.green;
	blue = rgb_fg.blue;
    }
    else if (i == BACK) {
	red = rgb_bg.red;
	green = rgb_bg.green;
	blue = rgb_bg.blue;
    }
    else {
        red = rgb[(iBase+i)%MaxColor].red;
        green = rgb[(iBase+i)%MaxColor].green;
        blue = rgb[(iBase+i)%MaxColor].blue;
    }
    StoreColor(co, red, green, blue);
}


int dxcWin, dycWin;
int dxcBox, dycBox;
int dxcBrdr, dycBrdr;

/*
 * ResizeColorWindows(pdp, pev, gc) - determines how to resize the
 *                                    color windows 
*/

ResizeColorWindows(pdp, pev, gc)
Display *pdp;
XEvent *pev;
GC gc;
{
    int i, xc, yc;
    static XFontStruct *pfs = NULL;
    char *sbFont;
    XConfigureEvent *pevCon = (XConfigureEvent *) pev;

    if (pfs != NULL) {
	XFreeFont(pdp, pfs);
	pfs = NULL;
    }

    dxcWin = pevCon->width - 50;
    dycWin = pevCon->height - 50;

    dxcBox = dxcWin/7;
    dycBox = 2*(dycWin - (pdBrdr*(cColorMax+4)/2))/(cColorMax+4);
    dxcBrdr = dycBox/2;
    dycBrdr = 2*(dycBox+pdBrdr) + 20;

    if (dycBox < 10) sbFont = "*-bold-r-normal--8-*-*-*-*-*-iso8859-*";
    else if (dycBox < 12) sbFont = "*-bold-r-normal--10-*-*-*-*-*-iso8859-*";
    else if (dycBox < 14) sbFont = "*-bold-r-normal--12-*-*-*-*-*-iso8859-*";
    else if (dycBox < 18) sbFont = "*-bold-r-normal--14-*-*-*-*-*-iso8859-*";
    else sbFont = "*-bold-r-normal--18-*-*-*-*-*-iso8859-*";
    pfs = XLoadQueryFont(pdp, sbFont);
    if (pfs == NULL) {
	sbFont = "fixed";
	fprintf(stderr, "Couldn't get resizable font, getting %s instead\n",
		sbFont);
	pfs = XLoadQueryFont(pdp, sbFont);
	if (pfs == NULL) 
	    fprintf(stderr, "Couldn't get font %s\n", sbFont), 
		exit(1);
	}
    XSetFont(pdp, gc, pfs->fid);

    for (i = 0; i < cColorMax; i++) {
	Bool fLeft = i < cColorMax/2;
	xc = dxcBrdr + (fLeft ? 0 : dxcWin/2);
	yc = dycBrdr + (fLeft ? i : i-cColorMax/2)*(dycBox+pdBrdr);
	XMoveResizeWindow(pdp, rgwn[i], xc, yc, dxcBox, dycBox);
	}
    XMoveResizeWindow(pdp, wnPrevious, dxcBrdr, dycWin+25, 
			lleft_width, lright_height);
    XMoveResizeWindow(pdp, wnNext, pevCon->width-dxcBrdr-lright_width, 
			dycWin+25, lright_width, lright_height);
}

/*
 * DrawLabels(pdp, wn, gc) - draws the color labels for the color windows
*/

DrawLabels(pdp, wn, gc)
Display *pdp;
Window wn;
GC gc;
{
    int i;

    for (i = 0; i < cColorMax; i++) {
	Bool fLeft = i < cColorMax/2;
	XDrawString(pdp, wn, gc, dxcBrdr+dxcBox+dxcBrdr+(fLeft ? 0 : dxcWin/2),
		    dycBrdr+(fLeft ? i : i-cColorMax/2)*(dycBox+pdBrdr)+dycBox,
		    rgsbColor[(iBase+i) % MaxColor],
		    strlen(rgsbColor[(iBase+i) % MaxColor]));
	}
}

/*
 * PrintFore(sb, wnTop, gc) - print the name of the current foreground color
*/

PrintFore(sb, wnTop, gc)
char *sb;
Window wnTop;
GC gc;
{
    char print[40];
    
    XDrawImageString(pdp, wnTop, gc, dxcBrdr, dycBrdr-2*dycBox, 
	"                                        ", 40);
    sprintf(print, "Foreground: %s", sb);
    XDrawString(pdp, wnTop, gc, dxcBrdr, dycBrdr-2*dycBox, print, 
	strlen(print));
}

/*
 * PrintBack(sb, wnTop, gc) - print the name of the current background color
*/

PrintBack(sb,wnTop,gc)
char *sb;
Window wnTop;
GC gc;
{
    char print[40];

    XDrawImageString(pdp, wnTop, gc, dxcBrdr, dycBrdr-dycBox, 
	"                                         ", 40);
    sprintf(print, "Background: %s", sb);
    XDrawString(pdp, wnTop, gc, dxcBrdr, dycBrdr-dycBox, print, strlen(print));
}

/*
 * char *GetColorName(wn) - find the color name for the given window
*/

char *GetColorName(wn)
Window wn;
{
    int i;
    char *sb;

    XFindContext(pdp, wn, cxNum, &i);
    sb = (char *) malloc ((unsigned) strlen(rgsbColor[(iBase+i) % MaxColor])+1);
    strcpy(sb, rgsbColor[(iBase+i) % MaxColor]);
    return(sb);
}
	
