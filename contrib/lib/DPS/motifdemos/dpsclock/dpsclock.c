/* dpsclock.c - Sample clock program

Copyright 1989 Massachusetts Institute of Technology

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of M.I.T. not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission.  M.I.T. makes no representations about the
suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.

M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/

/*
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

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include "clock.h"
#include "clock.bit"

static XrmOptionDescRec options[] = {
  {"-chime",	"*chime",	XrmoptionNoArg,		"TRUE"},
  {"-update",	"*update",	XrmoptionSepArg,	NULL},
  {"-padding",	"*padding",	XrmoptionSepArg,	NULL},
  {"-d",	"*analog",	XrmoptionNoArg,		"FALSE"},
  {"-digital",	"*analog",	XrmoptionNoArg,		"FALSE"},
  {"-analog",	"*analog",	XrmoptionNoArg,		"TRUE"},
  {"-font",	"*font",	XrmoptionSepArg,	NULL},
  };

static XtResource resources[] = {
  {XtNfont, XtCString, XtRString, sizeof(char *),
  XtOffset(ClockParams *, font), XtRString, "Times-Roman"},
  {"analog", XtCBoolean, XtRBoolean, sizeof(Boolean),
  XtOffset(ClockParams *, analog), XtRString, "TRUE"},
  {"chime", XtCBoolean, XtRBoolean, sizeof(Boolean),
  XtOffset(ClockParams *, chime), XtRString, "FALSE"},
  {XtNupdate, XtCInterval, XtRInt, sizeof(int),
  XtOffset(ClockParams *, update), XtRString, "60"},
  {"padding", XtCMargin, XtRInt, sizeof(int),
  XtOffset(ClockParams *, padding), XtRString, "8"}
  };
 
/*
 * Report the syntax for calling xclock.
 */
Syntax(call)
  char *call;
  {
  (void) printf("Usage: %s [-analog] [-digital]\n", call);
  (void) printf("       [-font <font_name>] [-help] [-padding <pixels>]\n");
  (void) printf("       [-update <seconds>] [-display displayname]\n");
  (void) printf("       [-bg <color>] [-geometry geom]\n\n");
  exit(1);
  }

void main(argc, argv)
  int argc;
  char **argv;
  {
  Arg arg;
  ClockParams params;
  Widget topLevel, w;

  topLevel = XtInitialize(
    argv[0], "DPSClock", options, XtNumber(options), &argc, argv);

  if (argc != 1)
    Syntax(argv[0]);

  arg.name = XtNiconPixmap;
  arg.value = (XtArgVal)XCreateBitmapFromData(
    XtDisplay(topLevel), XtScreen(topLevel)->root,
    clock_bits, clock_width, clock_height);
  XtSetValues(topLevel, &arg, 1);

  XtGetApplicationResources(
    topLevel, &params, resources, XtNumber(resources), NULL, 0);

  w = CreateClock(topLevel, &params);

  XtRealizeWidget(topLevel);

  XtMainLoop ();
  }
