#ifndef lint
static char *rcsid = "$Header: /usr3/emu/menus/RCS/test_menus.c,v 1.7 90/11/20 17:43:46 tom Exp Locker: jkh $";
#endif

/*
 * This file is part of the PCS emu program.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * Test code for emu menus.
 *
 * Author: Thomas Bagli
 * Date: June 12th, 1990.
 * Description: <one or more lines of text>
 *
 * Revision History:
 *
 * $Log:	test_menus.c,v $
 * Revision 1.7  90/11/20  17:43:46  tom
 * Alpha.
 * 
 * Revision 1.6  90/08/27  10:36:27  tom
 * Erste Versuch menubar.
 * 
 * Revision 1.5  90/08/07  10:05:16  tom
 * Deleted unused code.
 * 
 * Revision 1.4  90/07/26  02:25:47  jkh
 * Added new copyright.
 * 
 * Revision 1.3  90/07/18  19:06:23  tom
 * Miscellaneous little tweaks.
 * 
 * Revision 1.2  90/07/10  11:56:04  tom
 * Removed the AddActions and RegisterGrab calls.  Replaced with a
 * single XpEmuInitializeMenus call.  Now emu only needs to
 * initialize menus; funo inner details are known.
 * 
 * Revision 1.1  90/06/28  16:00:14  tom
 * Initial revision
 * 
 * Revision 1.1  90/06/28  16:00:14  tom
 * Initial revision
 * 
 *
 */


#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>

#ifdef MOTIF
#include <Xm/Form.h>
#else /* ATHENA */
#include <X11/Xaw/Form.h>
#include <X11/Xaw/MenuButton.h>
#endif /* MOTIF */

extern Widget XpEmuInitializeMenus ();

static void usage ();

static XrmOptionDescRec optionList[] = {
    {"-cd",     "*coredump",       XrmoptionNoArg,    (caddr_t) "True"},
    {"-dc",     "*discombobulate", XrmoptionNoArg,    (caddr_t) "True"},
    {"-mbar",   "*menuBar",        XrmoptionNoArg,    (caddr_t) "True"},
    {"-help",   "*help",           XrmoptionNoArg,    (caddr_t) "True"},
};

typedef struct _AppResources {
    Boolean core_dump, discombobulate, menu_bar, show_usage;
} AppResourcesRec, *AppResources;

AppResourcesRec appResources;

#define XpNmenuBar "menuBar"

#define offset(field) XtOffset(struct _AppResources *, field)

static XtResource appResourceList[] = {
    { "coredump", XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(core_dump), XtRString, "False"
    },
    { "discombobulate", XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(discombobulate), XtRString, "False"
    },
    { XpNmenuBar, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(menu_bar), XtRString, "False"
    },
    { "help", XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(show_usage), XtRImmediate, FALSE
    },
};


static void
usage (argc, argv)
    char ** argv;
    int argc;
{
    int i;

    static char *syntax[] = {
	"-help",		"Prints this output.",
	"-cd",			"Dumps a core file.",
	"-dc",			"Discombobulates the gonculator.",
	"-mbar",		"Selects menu bar option.",
	NULL,			NULL,
    };
    static char **usage = syntax;

    if (argc > 1) {
	printf("test_menus: unknown arguments:\n");
	for (i = 1; i < argc ; i++)
	    printf("%-30s\n", argv[i]);
    }

    printf("\nKnown arguments are the normal X Toolkit options and:\n");
    while (*usage != NULL) {
	printf("%-30s - %s\n", usage[0], usage[1]);
	usage += 2;
    }

    exit (0);
}


#ifdef MOTIF_ONE_ZERO
/* For Motif 1.0 Xt, from MIT Xt/Intrinsic.h */
typedef char* XtPointer;
#endif /* MOTIF_ONE_ZERO */

#ifdef MOTIF
#define CORE_WIDGET_CLASS widgetClass
#define FORM_WIDGET_CLASS xmFormWidgetClass

#define BOTTOM_GRAVITY_RESOURCE	XmNbottomAttachment
#define LEFT_GRAVITY_RESOURCE	XmNleftAttachment
#define RIGHT_GRAVITY_RESOURCE	XmNrightAttachment
#define TOP_GRAVITY_RESOURCE	XmNtopAttachment
#define POS_WIDGET_RESOURCE	XmNtopWidget

#define BOTTOM_VALUE	XmATTACH_FORM
#define LEFT_VALUE	XmATTACH_FORM
#define RIGHT_VALUE	XmATTACH_FORM
#define TOP_VALUE	XmATTACH_WIDGET

#else /* ATHENA */
#define CORE_WIDGET_CLASS coreWidgetClass
#define FORM_WIDGET_CLASS formWidgetClass

#define BOTTOM_GRAVITY_RESOURCE	XtNbottom
#define LEFT_GRAVITY_RESOURCE	XtNleft
#define RIGHT_GRAVITY_RESOURCE	XtNright
#define TOP_GRAVITY_RESOURCE	XtNtop
#define POS_WIDGET_RESOURCE	XtNfromVert

#define BOTTOM_VALUE	XawChainBottom
#define LEFT_VALUE	XawChainLeft
#define RIGHT_VALUE	XawChainRight
#define TOP_VALUE	XawChainTop
#endif /* MOTIF */

void
main (argc, argv)
    int argc;
    char **argv;
{
    Arg args[5];
    Arg topArgs[] = {
	{ XtNallowShellResize, (XtArgVal) TRUE },
	{ XtNinput, (XtArgVal) TRUE },
    };
    Cardinal nargs = 0;
    Dimension height;
    Widget top, parent, testCore;
    Widget topForm, menuBar;

    top = XtInitialize (argv[0], "Test_menus", optionList,
	XtNumber (optionList), &argc, argv);
    parent = top;

    XtSetValues (top, topArgs, XtNumber (topArgs));

    XtGetApplicationResources (top, (XtPointer)&appResources,
	appResourceList, XtNumber(appResourceList),
	NULL, (Cardinal) 0);

    if ((argc != 1) || (appResources.show_usage))
	usage (argc, argv);

    /* If necessary, initialize the menu_bar. */
    if (appResources.menu_bar) {
	topForm = XtCreateManagedWidget ("topForm", FORM_WIDGET_CLASS,
	    top, (ArgList) NULL, (Cardinal) 0);
	parent = topForm;

	menuBar = XpEmuInitializeMenus (parent, True, &height);

	XtSetArg (args[nargs], POS_WIDGET_RESOURCE, menuBar); nargs++;
	XtSetArg (args[nargs], LEFT_GRAVITY_RESOURCE, LEFT_VALUE); nargs++;
	XtSetArg (args[nargs], RIGHT_GRAVITY_RESOURCE, RIGHT_VALUE); nargs++;
	XtSetArg (args[nargs], TOP_GRAVITY_RESOURCE, TOP_VALUE); nargs++;
	XtSetArg (args[nargs], BOTTOM_GRAVITY_RESOURCE, BOTTOM_VALUE); nargs++;
    } else
	menuBar = XpEmuInitializeMenus (parent, False, &height);

    testCore = XtCreateManagedWidget ("testCore", CORE_WIDGET_CLASS,
	parent, (ArgList) args, nargs);

    XtRealizeWidget (top);

    XtMainLoop ();
}

