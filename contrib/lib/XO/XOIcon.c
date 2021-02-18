/*------------------------------ XOIcon.c --------------------------------*/

/*
 *  simple function to create icon for Xt-based program
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
static char _SSS_XOIcon_copyright[] =
    "XOIcon 1.0 Copyright 1990, 1991, Systems & Software Solutions, Inc, Marietta, GA";

#include "IntrinsicP.h"
#include "StringDefs.h"
#include "Shell.h"

#include "libXO.h"

/*
 *  add an icon to an Xt-based widget
 */

int
XOAddIconToWidget (w, bits, width, height)

Widget w;		/* shellClass or other class that takes iconBitmap */
char *bits;		/* xbm format _bits variable name */
int width, height;	/* xbm format _width & _height fields */
{
    Display *dpy;
    Screen *scr;
    Pixmap icon_pixmap;
    Arg iconArg[] = {
	{ XtNiconPixmap,	(XtArgVal) NULL },
    };

    dpy = XtDisplay (w);
    scr = XtScreen (w);
    icon_pixmap = XCreateBitmapFromData (dpy, (XtScreen (w))->root,
	bits, width, height);
    iconArg[0].value = icon_pixmap;
    XtSetValues (w, iconArg, XtNumber (iconArg));
}
