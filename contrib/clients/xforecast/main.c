/*
 * Copyright 1991 by Frank Adelstein.  All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the authors name not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The author makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, 
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT 
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Cool program to display a map and print the local weather.
 *
 * by Frank Adelstein
 *
 */

#include "types.h"

XtAppContext
  Appcon;

Widget
  Toplevel;

GC
  drawGC,
  xorGC;

Places
  City[1000];
  
int 
  CityIndex;

main (argc, argv)
int    argc;
char **argv;

{

  /* initialize toolkit */
  Toplevel = XtAppInitialize (&Appcon, TITLE, NULL, 0, &argc, argv,
			      NULL, NULL, 0);

  /* do all the dirty work */
  ReadInCities();
  SetupInterface();

  /* sit back and process events */
  XtAppMainLoop(Appcon);
}
