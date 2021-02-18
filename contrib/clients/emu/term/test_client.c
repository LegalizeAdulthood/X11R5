#ifndef lint
     static char *rcsid = "$Header: /usr3/Src/emu/term/RCS/sample_client.c,v 1.5 91/09/30 21:39:59 jkh Exp Locker: jkh $";
#endif

/*
 * This file is part of the PCS emu system.
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
 * This builds a very simple test application of the emu widget.
 *
 * Author: Jordan K. Hubbard
 * Date: June 6th, 1990.
 * Description: Simple client application which puts an term widget
 *		inside a shell widget.
 *
 * Revision History:
 *
 * $Log:	sample_client.c,v $
 * Revision 1.5  91/09/30  21:39:59  jkh
 * Checkin prior to DECWRL merge.
 * 
 * Revision 1.4  90/07/26  01:57:09  jkh
 * Added new copyright.
 * 
 * Revision 1.2  90/06/26  19:10:58  jkh
 * Checkin for canvas testing.
 * 
 * Revision 1.1  90/06/26  17:40:53  jkh
 * Initial revision
 * 
 *
 */


#include "common.h"
#include "Term.h"
#include "TermCanvas.h"

Widget top, xterm, canvas;

/* Our process death handler. Just exits */
void
do_quit(w, pid, fail)
Widget w;
int pid, fail;
{
     exit(0);
}

int
main (ac, av, ep)
int ac;
char **av, **ep;
{
     int nargs;
     Dimension cell_width, cell_height;
     Arg args[8];
     char *cp;
     Import char *getenv();
     Import void rparse(), tty_set_size();
     
     top = XtInitialize(av[0], "test_client", NULL, 0, &ac, av);
     
     nargs = 0;
     if ((cp = getenv("SHELL")) != NULL) {
     	XtSetArg(args[nargs], XpNcommand, cp);			nargs++;
     }
     xterm = XtCreateManagedWidget("term", termWidgetClass,
				   top, args, nargs);
     XtAddCallback(xterm, XpNprocessDeath, do_quit, NULL);
     
     nargs = 0;
     XtSetArg(args[nargs], XtNwidth, 80);			nargs++;
     XtSetArg(args[nargs], XtNheight, 24);			nargs++;
     XtSetArg(args[nargs], XtNborderWidth, 0);			nargs++;
     XtSetArg(args[nargs], XpNoutput, (Generic)rparse);		nargs++;
     XtSetArg(args[nargs], XpNsetSize, (Generic)tty_set_size);	nargs++;
     XtSetArg(args[nargs], XpNcomBlock, XpTermComBlock(xterm));	nargs++;
     canvas = XtCreateManagedWidget("screen", termCanvasWidgetClass,
				    xterm, args, nargs);
     
     /* Find out the character size so we can set the WM hints */
     nargs = 0;
     XtSetArg(args[nargs], XpNcellHeight, &cell_height);	nargs++;
     XtSetArg(args[nargs], XpNcellWidth, &cell_width);		nargs++;
     XtGetValues(canvas, args, nargs);

     /* Set them on the shell widget */
     nargs = 0;
     XtSetArg(args[nargs], XtNheightInc, cell_height);		nargs++;
     XtSetArg(args[nargs], XtNwidthInc, cell_width);		nargs++;
     XtSetArg(args[nargs], XtNallowShellResize, TRUE);		nargs++;
     XtSetValues(top, args, nargs);

     /* Wank up the menus */
#ifdef NO_MENUS
     printf("Menus not compiled in.\n");
#else
     XpEmuInitializeMenus();
#endif 

     /* Florp up the whole mess */
     XtRealizeWidget(top);
     
     XtMainLoop();
     
     /* Keep GCC happy */
     return 0;
}
