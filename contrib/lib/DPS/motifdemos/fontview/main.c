/* main.c
 *
 * Copyright (C) 1990-1991 by Adobe Systems Incorporated.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notices appear in all copies and that
 * both those copyright notices and this permission notice appear in
 * supporting documentation and that the name of Adobe Systems
 * Incorporated not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.
 * 
 * ADOBE MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THE SOFTWARE FOR
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
 * ADOBE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO EVENT SHALL ADOBE BE LIABLE
 * TO YOU OR ANY OTHER PARTY FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE, STRICT LIABILITY OR ANY OTHER ACTION ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  ADOBE WILL NOT
 * PROVIDE ANY TRAINING OR OTHER SUPPORT FOR THE SOFTWARE.
 * 
 * PostScript, Display PostScript, and Adobe are trademarks of Adobe Systems
 * Incorporated registered in the U.S.A. and other countries.
 *
 * Author: Adobe Systems Incorporated
 */

/* C headers */
#include <stdio.h>

/* Xt toolkit headers */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

/* Motif widget headers */
#include <Xm/Xm.h>
#include <Xm/Form.h>

/* DPS library headers */
#include <DPS/dpsXclient.h>
#include <DPS/dpsops.h>
#include <DPS/psops.h>

#include "globals.h"
#include "wraps.h"

/* global variables*/
Widget toplevel;
GC defaultGC;

DPSContext init_dps (display, debug)
    Display *display;
    int debug;
{
    DPSContext ctxt, txtctxt;

    ctxt = XDPSCreateSimpleContext(display, 0, 0, 0, 0,
          DPSDefaultTextBackstop, DPSDefaultErrorProc, NULL);

    if (ctxt == NULL) {
        fprintf(stderr, "Display PostScript extension not installed.\n");
        exit(1);
    }
    DPSSetContext(ctxt);

    if (debug) {
        txtctxt = DPSCreateTextContext(DPSDefaultTextBackstop, 
              DPSDefaultErrorProc);
        DPSChainContext(ctxt, txtctxt);
    }
}

void main (argc, argv)
    int argc;
    char **argv;
{
    Arg args[10];
    DPSContext ctxt, txtctxt;
    Display *display;
    Screen *screen;
    Widget parent, fontPanel, demoCanvas;
    int i;
   
    toplevel = XtInitialize(NULL, "Fontview", NULL, 0, &argc, argv);


    display = XtDisplay(toplevel);
    screen = XtScreen(toplevel);

    defaultGC = XCreateGC(display, RootWindowOfScreen(screen), 0, NULL);
    XSetForeground(display, defaultGC, WhitePixelOfScreen(screen));
    XSetBackground(display, defaultGC, WhitePixelOfScreen(screen));

    init_dps(display, (argc > 1));

    parent = XmCreateForm(toplevel, "parent", NULL, 0);

    fontPanel = CreateFontPanel(parent);
   
    demoCanvas = CreateDemoCanvas(fontPanel, parent); 

    i=0; XtSetArg(args[i], XmNwidth, 700);
    i++; XtSetValues(parent, args, i);
    XtManageChild(parent);
    XtRealizeWidget(toplevel);

    XtMainLoop();
}
