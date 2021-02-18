#ifndef lint
static char *rcsid = "$Header: /usr3/emu/canvas/RCS/main.c,v 1.6 90/10/12 12:40:50 me Exp $";
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
 * Description: Test Wrapper for the TermCanvas Widget
 *
 * Author: Michael Elbel
 * Date: March 20th, 1990.
 *
 * Revision History:
 *
 * $Log:	main.c,v $
 * Revision 1.6  90/10/12  12:40:50  me
 * checkmark
 * 
 * Revision 1.5  90/08/31  19:51:51  me
 * Checkpoint vor dem Urlaub
 * 
 * Revision 1.4  90/07/26  02:36:34  jkh
 * Added new copyright.
 * 
 * Revision 1.3  90/05/04  13:39:48  me
 * Fixed interpolation of delete and insert line requests.
 * 
 * Revision 1.2  90/04/24  15:49:46  me
 * Got Input Focus to work by using XtSetInputFocus
 * 
 *
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/keysym.h>
#include <X11/Shell.h>
#include "canvas.h"
#include <signal.h>
#include <stdio.h>

Widget toplevel, term;

void
main (argc, argv, envp)
int argc;
char *argv [];
char *envp [];
{
     Local void rparse();
     void exit();
     
     Import void key_handler();
     
     Arg args[5];
     int n = 0;
     Dimension cell_width, cell_height;

     toplevel = XtInitialize(argv[0],"emu", NULL, 0, &argc, argv);
     n = 0;
     XtSetArg(args[n], XtNwidth, 80);		n++;
     XtSetArg(args[n], XtNheight, 24);		n++;
     term = XtCreateManagedWidget("screen", termCanvasWidgetClass,
					toplevel, args, n);
     
     n = 0;
     XtSetArg(args[n], XpNcellHeight, &cell_height);	n++;
     XtSetArg(args[n], XpNcellWidth, &cell_width);	n++;
     XtGetValues(term, args, n);

     printf("CellWidth = %d, CellHeight = %d\n", cell_width, cell_height);
     
     n = 0;
     XtSetArg(args[n], XtNheightInc, cell_height);	n++;
     XtSetArg(args[n], XtNwidthInc, cell_width);	n++;
     XtSetValues(toplevel, args, n);

     n = 0;
     XtSetArg(args[n], XpNoutput, rparse);	n++;
     XtSetValues(term, args, n);
     
     n = 0;
     XtSetArg(args[n], XpNtermType, "emu");	n++;
     XtSetValues(term, args, n);
     
     XtRealizeWidget(toplevel);
/* XSynchronize(XtDisplay(term), True); */
     XtSetKeyboardFocus(toplevel, term);

     signal(SIGINT, exit);
     
     XtMainLoop();
}

Local void
rparse(widget, block)
Widget widget;
ComBlockPtr block;
{
     char chr;
     
     
     switch (cb_opcode(block)) {
	case ROP_INSERT:
	  XpTermCanvasDispatch(term, block);
	  break;
	case ROP_INSERT_CHAR:
	  cb_opcode(block) = -ROP_INSERT;
	  chr = (char) cb_reg_data(block, 0);
	  cb_nbytes(block) = 1;
	  *cb_buffer(block) = chr;
	  XpTermCanvasDispatch(term, block);
	  break;
	case 99:
	  XtDestroyWidget(toplevel);
	  break;
	       
	default:
	  warn("Unknown reverse parser opcode %d",cb_opcode(block));
     }
}

