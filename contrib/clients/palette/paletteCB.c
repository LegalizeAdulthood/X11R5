/*
    paletteCB.c - pallete client's callback & action routines

    Author:  Miles O'Neal

    Release: 1.0 meo - FCS release

    COPYRIGHT 1990, 1991, Systems & Software Solutions, Inc, Marietta, GA

    Permission is hereby granted to use, copy, modify and/or distribute
    this software and supporting documentation, for any purpose and
    without fee, as long as this copyright notice and disclaimer are
    included in all such copies and derived works, and as long as
    neither the author's nor the copyright holder's name are used
    in publicity or distribution of said software and/or documentation
    without prior, written permission.

    This software is presented as is, with no warranties expressed or
    implied, including implied warranties of merchantability and
    fitness. In no event shall the author be liable for any special,
    direct, indirect or consequential damages whatsoever resulting
    from loss of use, data or profits, whether in an action of
    contract, negligence or other tortious action, arising out of
    or in connection with the use or performance of this software.
    In other words, you are on your own. If you don't like it, don't
    use it!
*/
static char _SSS_paletteCB_copyright[] =
"paletteCB 1.0 Copyright 1990, 1991, \
Systems & Software Solutions, Inc, Marietta, GA";

#include "Intrinsic.h"
#include "StringDefs.h"

extern Widget
    bglCC,
    bgnCC,
    fglCC,
    fgnCC,
    labelCC;

static Arg BCArg[] = {
    { XtNborderColor, NULL },
};

static Arg BGArg[] = {
    { XtNbackground, NULL },
};

static Arg FGArg[] = {
    { XtNforeground, NULL },
};

static Arg CNArg[] = {
    { XtNlabel, NULL },
};



/*
    SetFG - set foreground action routine
*/
XtActionProc
SetFG (w, e, p, np)

Widget w;
XEvent *e;
String p;
Cardinal *np;
{
    static Pixel newfg;
    static char *newfgn;
/*
    set FG of label (color test) area to new color of selected button &
    write the color name to the FG colorname label.
*/
    BCArg[0].value = (XtArgVal) &newfg;
    XtGetValues (w, BCArg, 1);

    FGArg[0].value = (XtArgVal) newfg;
    XtSetValues (labelCC, FGArg, 1);

    CNArg[0].value = (XtArgVal) &newfgn;
    XtGetValues (w, CNArg, 1);

    CNArg[0].value = (XtArgVal) newfgn;
    XtSetValues (fgnCC, CNArg, 1);
}


/*
    SetBG - set background action routine
*/
XtActionProc
SetBG (w, e, p, np)

Widget w;
XEvent *e;
String p;
Cardinal *np;
{
    static Pixel newbg;
    static char *newbgn;
/*
    set BG of label (color test) area to new color of selected button &
    write the color name to the BG colorname label.
*/
    BCArg[0].value = (XtArgVal) &newbg;
    XtGetValues (w, BCArg, 1);

    BGArg[0].value = (XtArgVal) newbg;
    XtSetValues (labelCC, BGArg, 1);

    CNArg[0].value = (XtArgVal) &newbgn;
    XtGetValues (w, CNArg, 1);

    CNArg[0].value = (XtArgVal) newbgn;
    XtSetValues (bgnCC, CNArg, 1);
}



/*
    fg/bg swap resources
*/

Arg FgBgArg[] = {
    {XtNbackground, (XtArgVal) NULL},
    {XtNforeground, (XtArgVal) NULL},
};


/*
    SwapFGBG - swap fg & bg colors, names in the label area
*/
XtActionProc
SwapFGBG (w, e, p, np)

Widget w;
XEvent *e;
String p;
Cardinal *np;
{
    static Pixel bg;
    static Pixel fg;
    static char *bgn;
    static char *fgn;
    static char tmpn[81];

    FgBgArg[0].value = (XtArgVal) &bg;		/* get old bg & fg colors */
    FgBgArg[1].value = (XtArgVal) &fg;
    XtGetValues (labelCC, FgBgArg, 2);

    FgBgArg[0].value = (XtArgVal) fg;		/* and swap them */
    FgBgArg[1].value = (XtArgVal) bg;
    XtSetValues (labelCC, FgBgArg, 2);

    CNArg[0].value = (XtArgVal) &bgn;		/* get bg name */
    XtGetValues (bgnCC, CNArg, 1);
    CNArg[0].value = (XtArgVal) &fgn;		/* get the fg name */
    XtGetValues (fgnCC, CNArg, 1);

    strcpy (tmpn, bgn);

    CNArg[0].value = (XtArgVal) fgn;		/* old fg is new bg */
    XtSetValues (bgnCC, CNArg, 1);
    CNArg[0].value = (XtArgVal) tmpn;		/* old bg is new fg */
    XtSetValues (fgnCC, CNArg, 1);
}
