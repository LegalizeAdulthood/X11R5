/*------------------------------- palette.c ---------------------------------*/
/*
 *  an X client to help pick foreground/background colors
 *
 *  Author:  Miles O'Neal
 *
 *  Release: 1.0 meo - FCS release
 *
 *  COPYRIGHT 1990, 1991, Systems & Software Solutions, Inc, Marietta, GA
 *
 *  Permission is hereby granted to use, copy, modify and/or distribute
 *  this software and supporting documentation, for any purpose and
 *  without fee, as long as this copyright notice and disclaimer are
 *  included in all such copies and derived works, and as long as
 *  neither the author's nor the copyright holder's name are used
 *  in publicity or distribution of said software and/or documentation
 *  without prior, written permission.
 *
 *  This software is presented as is, with no warranties expressed or
 *  implied, including implied warranties of merchantability and
 *  fitness. In no event shall the author be liable for any special,
 *  direct, indirect or consequential damages whatsoever resulting
 *  from loss of use, data or profits, whether in an action of
 *  contract, negligence or other tortious action, arising out of
 *  or in connection with the use or performance of this software.
 *  In other words, you are on your own. If you don't like it, don't
 *  use it!
 */
static char _SSS_palette_copyright[] =
"palette 1.0 Copyright 1990, 1991, Systems & Software Solutions, Inc, Marietta, GA";


#include <stdio.h>
#include <ctype.h>

#include "Xatom.h"
#include "IntrinsicP.h"
#include "Xos.h"
#if XT_REVISION == 2 || XT_REVISION == 3
#include "XawMisc.h"
#endif
#include "StringDefs.h"

#include "Shell.h"
#include "Box.h"
#include "Form.h"
#include "Label.h"
#include "Command.h"
#include "Text.h"
#include "AsciiText.h"
#if XT_REVISION == 2 || XT_REVISION == 3
#include "VPaned.h"
#define PANED_WIDGET_CLASS vPanedWidgetClass
#else
#define PANED_WIDGET_CLASS panedWidgetClass
#include "Paned.h"
#endif
#include "Viewport.h"

#include <libXO.h>

#include "palette.xbm"

#ifndef NAMELEN
#define NAMELEN 30		/* max characters allowed in color name */
#endif /* NAMELEN */
char labelname[NAMELEN+1];

#ifndef MAXCOLORS
#define MAXCOLORS (2 * 256)		/* 2 sets of names for 8 planes */
#endif /* MAXCOLORS */

char *colors[MAXCOLORS];


/*
 *	new translations & actions:
 *
 *	[Bb] key sets background of text to border color of current button
 *	[Ff] key sets foreground of text to border color of current button
 *	[Ss] key swaps current foreground and background of color comb. widget
 *	Btn1 (same as F key)
 *	Btn2 (same as S key)
 *	Btn3 (same as B key)
*/

XtTranslations ColorKT;		/* color button translations */
String ColorKTS =
   "<Key>F:	setfg()\n\
     <Key>f:	setfg()\n\
     <Key>B:	setbg()\n\
     <Key>b:	setbg()\n\
     <Key>S:	swapfgbg()\n\
     <Key>s:	swapfgbg()\n\
     <Btn3Down>:	set()\n\
     <Btn1Up>:	notify() setfg() unset()\n\
     <Btn2Up>:	swapfgbg()\n\
     <Btn3Up>:	notify() setbg() unset()";

XtTranslations SwapCKT;		/* everybody else's translations */
String SwapCKTS =
   "<Key>S:	swapfgbg()\n\
     <Key>s:	swapfgbg()\n\
     <Btn2Up>:	swapfgbg()";

XtActionProc SetFG(), SetBG(), SwapFGBG();

XtActionsRec ColorKB[] = {	/* new actions */
    { "setfg", (XtActionProc) SetFG },
    { "setbg", (XtActionProc) SetBG },
    { "swapfgbg", (XtActionProc) SwapFGBG },
};


Widget toplevel;	/* take a guess! */
Arg topArg[] = {	/* toplevel stuff (duh) */
    { XtNminWidth,		(XtArgVal) 400 },
    { XtNminHeight,		(XtArgVal) 200 },
};
Arg iconArg[] = {	/* icon args for toplevel widget */
    { XtNiconPixmap,		(XtArgVal) NULL },
};

/*
 *  main container widget
 */
Widget vpaned1;
Arg vpaned1Arg[] = {
    { XtNwidth,			(XtArgVal) 1000 },
    { XtNheight,		(XtArgVal) 480 },
    { XtNborderWidth,		(XtArgVal) 1 },
};

/*
 *  color button box viewport widget
 */
Widget vp1;
Arg vp1Arg[] = {
    { XtNwidth,			(XtArgVal) 1000 },
    { XtNheight,		(XtArgVal) 380 },
    { XtNborderWidth,		(XtArgVal) 0 },
    { XtNallowHoriz,		(XtArgVal) TRUE },
    { XtNallowVert,		(XtArgVal) TRUE },
    { XtNmin,			(XtArgVal) 100 },
};

/*
 *  color button box widget
 */
Widget boxCB;
Arg boxCBArg[] = {
    { XtNwidth,			(XtArgVal) 1000 },
    { XtNheight,		(XtArgVal) 380 },
    { XtNorientation,		(XtArgVal) XtorientHorizontal },
    { XtNborderWidth,		(XtArgVal) 0 },
};

/*
 *  color button widgets
 */
Widget CB;
Arg CBArg[] = {
    { XtNborderWidth,		(XtArgVal) 7 },
    { XtNborderColor,		(XtArgVal) NULL },
};

/*
 *  viewport widget for everything besides color buttons
 */
Widget vp2;
Arg vp2Arg[] = {
    { XtNwidth,			(XtArgVal) 1000 },
    { XtNheight,		(XtArgVal) 100 },
    { XtNborderWidth,		(XtArgVal) 0 },
    { XtNallowHoriz,		(XtArgVal) TRUE },
    { XtNallowVert,		(XtArgVal) TRUE },
    { XtNmin,			(XtArgVal) 90 },
    { XtNmax,			(XtArgVal) 150 },
    { XtNskipAdjust,		(XtArgVal) False },
};

/*
 *  form widget for everything besides color buttons
 */
Widget form1;
Arg form1Arg[] = {
    { XtNheight,		(XtArgVal) 100 },
    { XtNborderWidth,		(XtArgVal) 0 },
};

/*
 *  labelCC widget - shows color combinations
 */
Widget labelCC;
char TheText[] =
    "COLOR TESTING AREA. No explosives, radios, or open flames.";

Arg labelCCArg[] = {
    { XtNlabel,			(XtArgVal) TheText },
    { XtNinternalHeight,	(XtArgVal) 15 },
    { XtNinternalWidth,		(XtArgVal) 15 },
};


/*
 *  color button & labelCC widget border color Arg
 */
Arg ColorArg[] = {
    { XtNborderColor,		(XtArgVal) NULL },
};

/*
 *  current color text label widgets
 */
Widget
    fglCC,		/* fg current color label */
    fgnCC,		/* fg current color name */
    bglCC,		/* bg current color label */
    bgnCC;		/* bg current color name */

Arg lCCArg[] = {		/* fg & bg label args */
    { XtNlabel,			(XtArgVal) NULL },
    { XtNborderWidth,		(XtArgVal) 0 },
    { XtNinternalHeight,	(XtArgVal) 3 },
    { XtNfromVert,		(XtArgVal) NULL },
    { XtNfromHoriz,		(XtArgVal) NULL },
    { XtNjustify,		(XtArgVal) XtJustifyLeft },
};
Arg nCCArg[] = {		/* fg & bg name args */
    { XtNlabel,			(XtArgVal) NULL },
    { XtNborderWidth,		(XtArgVal) 1 },
    { XtNinternalHeight,	(XtArgVal) 2 },
    { XtNfromVert,		(XtArgVal) NULL },
    { XtNfromHoriz,		(XtArgVal) NULL },
    { XtNjustify,		(XtArgVal) XtJustifyLeft },
};

Display *dpy;
Screen *scr;
Colormap *cmap, *cmap2;

/*
 *  special action button widgets
 */
Widget help, quit;
Arg btnwArg[] = {
    { XtNfromVert,		(XtArgVal) NULL },
    { XtNfromHoriz,		(XtArgVal) NULL },
};
char helpText[] =
"Left Mouse Button:   changes foreground\n\
Middle Mouse Button: swaps colors\n\
Right Mouse Button:  changes background\n\
\n\
'F' or 'f' key: changes foreground\n\
'S' or 's' key: swaps colors\n\
'B' or 'b' key: changes background";

int OutputValues = 0;

static void QuitCB ();

main (argc, argv)

int argc;
char *argv[];
{
    register int i, j;
    int ncolors, maxlen;
    Pixel black, white;
    char **ap;
    int NoGray = 1;
    int scrW, scrH, height;
    char buf[256];

    Pixmap
	icon_bitmap,
	icon_pixmap;

    XColor
	rgbcolor,	/* rgb.txt database color */
	color;		/* server closest color */
/*
 *  Create the top level widget and containers. Add the swapping
 *  translations everywhere except the top level, viewport, and
 *  help box. If any widget cannot be created, we clean up and
 *  exit.
 */
    toplevel = XtInitialize (argv[0], "Palette", NULL, 0, &argc, argv);
    if (!toplevel)
	goto BOOGIE;
    XtSetValues (toplevel, topArg, XtNumber (topArg));
    if (argc) {
	for (ap = argv; argc; ap++, argc--) {
	    if (!strncmp(*ap, "gray", 4) || !strncmp(*ap, "grey", 4) ||
		!strncmp(*ap, "+gray", 5) || !strncmp(*ap, "+grey", 5)) {
		    NoGray = 0;
	    } else if (!strncmp(*ap, "-gray", 5) || !strncmp(*ap, "-grey", 5)) {
		    NoGray = 1;
	    } else if (!strncmp(*ap, "-o", 2)) {
		    OutputValues = 1;
	    } else if (*ap[0] == '-') {
		(void) Usage(argv[0]);
	    }
	}
    }
    dpy = XtDisplay (toplevel);
    scr = XtScreen (toplevel);
    cmap = (Colormap *) DefaultColormapOfScreen (scr);
    XOAddIconToWidget (toplevel, palette_bits, palette_width, palette_height);

    if ((vpaned1 = XtCreateManagedWidget ("vpaned1", PANED_WIDGET_CLASS,
    toplevel, vpaned1Arg, XtNumber (vpaned1Arg)))) {
	SwapCKT = XtParseTranslationTable (SwapCKTS);
	XtAddActions (ColorKB, XtNumber (ColorKB));
	XtOverrideTranslations (vpaned1, SwapCKT);

	if (!(vp1 = XtCreateManagedWidget ("vp1", viewportWidgetClass,
	    vpaned1, vp1Arg, XtNumber (vp1Arg))))
		goto BOOGIE;

	if (!(boxCB = XtCreateManagedWidget ("boxCB", boxWidgetClass,
	    vp1, boxCBArg, XtNumber (boxCBArg))))
		goto BOOGIE;
	XtOverrideTranslations (boxCB, SwapCKT);

	if (!(ncolors = XOGetStdColorNames(colors, MAXCOLORS, &maxlen,
	    !NoGray, COLORFILE)))
		goto BOOGIE;
/*
 *	Create a widget for each color. Make them the same size by
 *	padding the names out (I *detest* random-length buttons scattered
 *	willy-nilly about in a box).
 */
	ColorKT = XtParseTranslationTable (ColorKTS);
	for (i = 0; i < ncolors; i++) {
	    strcpy (labelname, colors[i]);
	    for (j = strlen (colors[i]); j < maxlen; j++)
		labelname[j] = ' ';
	    labelname[j] = '\0';

	    XLookupColor (dpy, cmap, colors[i], &rgbcolor, &color);
	    XAllocColor (dpy, cmap, &color);
	    CBArg[1].value = (XtArgVal) color.pixel;
	    if (!(CB = XtCreateManagedWidget (labelname, commandWidgetClass,
		boxCB, CBArg, XtNumber (CBArg))))
		    goto BOOGIE;
	    XtOverrideTranslations (CB, ColorKT);
	}

/*
 *	create the form to hold all the stuff at the bottom
 */
	if (!(vp2 = XtCreateManagedWidget ("vp2", viewportWidgetClass,
	    vpaned1, vp2Arg, XtNumber (vp2Arg))))
		goto BOOGIE;

	if (!(form1 = XtCreateManagedWidget ("form1", formWidgetClass, vp2,
	    form1Arg, XtNumber (form1Arg))))
		goto BOOGIE;
	XtOverrideTranslations (form1, SwapCKT);

/*
 *	add the labelCC area that tests the color combinations
 */
	if (!(labelCC = XtCreateManagedWidget ("TEXT", labelWidgetClass,
	    form1, labelCCArg, XtNumber (labelCCArg))))
		goto BOOGIE;
	XtOverrideTranslations (labelCC, SwapCKT);

/*
 *	Add quit button
 */
	btnwArg[0].value = (XtArgVal) labelCC;
	if (!(quit = (Widget) XOCreateQuitBtn(form1, toplevel, QuitCB,
	    btnwArg, 1)))
		goto BOOGIE;

/*
 *	Add help button
 */
	btnwArg[0].value = (XtArgVal) labelCC;
	btnwArg[1].value = (XtArgVal) quit;
	if (!(help = (Widget) XOCreateHelpBtn(form1, toplevel, NULL,
	    btnwArg, 2, helpText)))
		goto BOOGIE;

/*
 *	add the labels that display names of current fg & bg colors
 */
	lCCArg[0].value = (XtArgVal) " BG:";
	lCCArg[3].value = (XtArgVal) labelCC;
	lCCArg[4].value = (XtArgVal) help;
	if (!(bglCC = XtCreateManagedWidget ("bglCC", labelWidgetClass,
	    form1, lCCArg, XtNumber (lCCArg))))
		goto BOOGIE;
	XtOverrideTranslations (bglCC, SwapCKT);

	strcpy (buf, "White");
	for (i = 5; i < maxlen; i++)
	    buf[i] = ' ';
	buf[i] = NULL;
	nCCArg[0].value = (XtArgVal) buf;
	nCCArg[3].value = (XtArgVal) labelCC;
	nCCArg[4].value = (XtArgVal) bglCC;
	if (!(bgnCC = XtCreateManagedWidget ("bgnCC", labelWidgetClass,
	    form1, nCCArg, XtNumber (nCCArg))))
		goto BOOGIE;
	XtOverrideTranslations (bgnCC, SwapCKT);

	lCCArg[0].value = (XtArgVal) " FG:";
	lCCArg[4].value = (XtArgVal) bgnCC;
	if (!(fglCC = XtCreateManagedWidget ("fglCC", labelWidgetClass,
	    form1, lCCArg, XtNumber (lCCArg))))
		goto BOOGIE;
	XtOverrideTranslations (fglCC, SwapCKT);

	strcpy (buf, "Black");
	for (i = 5; i < maxlen; i++)
	    buf[i] = ' ';
	buf[i] = NULL;
	nCCArg[0].value = (XtArgVal) buf;
	nCCArg[4].value = (XtArgVal) fglCC;
	if (!(fgnCC = XtCreateManagedWidget ("fgnCC", labelWidgetClass,
	    form1, nCCArg, XtNumber (nCCArg))))
		goto BOOGIE;
	XtOverrideTranslations (fgnCC, SwapCKT);

	XtSetMappedWhenManaged (toplevel, False);
	XtRealizeWidget (toplevel);
	XtMapWidget (toplevel);
	XtMainLoop (toplevel);
    } else {
BOOGIE:			/* oops - something didn't work right */
	if (toplevel)
	    XtCloseDisplay (dpy); 
    }
    exit (1);
}



static char *bgn;
static char *fgn;

static Arg CNArg[] = {
    { XtNlabel, NULL },
};

/*
 *  User pressed Quit button
 */
static void
QuitCB (w, call_data, event)

Widget w;
caddr_t call_data;
XEvent *event;
{
    register int i;

    CNArg[0].value = (XtArgVal) &bgn;		/* get bg name */
    XtGetValues (bgnCC, CNArg, 1);
    for (i = strlen (bgn) - 1; i > 0 && *(bgn + i) == ' '; i--)
	*(bgn + i) = '\0';

    CNArg[0].value = (XtArgVal) &fgn;		/* get the fg name */
    XtGetValues (fgnCC, CNArg, 1);
    for (i = strlen (fgn) - 1; i > 0 && *(fgn + i) == ' '; i--)
	*(fgn + i) = '\0';

    XSync (XtDisplay (w));
    XtCloseDisplay (XtDisplay (w)); 

    if (OutputValues) {
	printf ("-bg %s -fg %s", bgn, fgn);
	fflush (stdout);
    }
    exit (0);
}



/*
 * Usage (name) - explain how to invoke <name>, and exit
 */
void
Usage (name)

char *name;
{
    fprintf (stderr,
	"\nUsage: %s [-o] [+gray | -gray]\n\
	-o	: output values when Quit button pressed\n\
	-gray	: do not show GrayN values\n\
	+gray	: show GrayN values\n", name);
    fflush (stderr);
    exit (2);
}
