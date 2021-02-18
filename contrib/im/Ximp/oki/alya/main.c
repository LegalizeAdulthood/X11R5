/*
 * Copyright 1991 Oki Technosystems Laboratory, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Oki Technosystems Laboratory not
 * be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * Oki Technosystems Laboratory makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Oki TECHNOSYSTEMS LABORATORY DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL OKI TECHNOSYSTEMS LABORATORY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Yasuhiro Kawai,	Oki Technosystems Laboratory, Inc.
 *				kawai@otsl.oki.co.jp
 */
#ifndef lint
static char sccsid[] = "@(#)main.c	1.2";
#endif
/*
 * main.c -
 *
 * @(#)1.2 91/09/28 17:12:17
 */
#include <stdio.h>
#include <locale.h>
#include <X11/Intrinsic.h>	

#include <X11/Xaw/Box.h>	
#include <X11/Xaw/Label.h>	
#include <X11/Xaw/Command.h>	
#include <X11/Xaw/Cardinals.h>	
#include <X11/Shell.h>	
#include <X11/StringDefs.h>	
#include "Preedit.h"

static void Quit();

main(argc, argv)
int argc;
char **argv;
{
    Widget toplevel, label, box1, quit;
    int ac;
    Arg args[10];
    extern void Handler();
    extern void PropHandler();
    char *locale;

    locale = "ja_JP.EUC";

    if (setlocale(LC_CTYPE, "") == NULL) {
	fprintf(stderr, "Error : setlocale() \n");
        exit(1);
    }
    toplevel = XtInitialize(NULL, "Ims", NULL, 0, &argc, argv);

    ac = 0;
    box1 =  XtCreateManagedWidget("box1", boxWidgetClass, toplevel, 
				   args, ac);
    quit =  XtCreateManagedWidget("quit", commandWidgetClass, box1, 
				   NULL, ZERO);
    XtAddCallback(quit, XtNcallback, Quit, NULL);
    RootInitialize(box1);
    XtRealizeWidget(toplevel);

    IMSInit(toplevel, "_XIMP_ja_JP");
    XtAddEventHandler(toplevel, NoEventMask, TRUE, Handler, box1);
    XtAddEventHandler(toplevel, PropertyChangeMask, FALSE, PropHandler, NULL);
    XtMainLoop();
}

/* ARGUSED */
static void
Quit(w, closure, client_data)
Widget w;
XtPointer closure;
XtPointer client_data;
{
    exit(0);
}
