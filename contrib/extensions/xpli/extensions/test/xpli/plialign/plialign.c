/*
 * Copyright IBM Corporation 1991
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/* 
 * This program simply makes the PLIAlign call after opening a connection to 
 * the display and registering the extension.  It uses wsimple.c simply to
 * parse the command line and open the display.  This program can be invoked
 * from a window manager menu. - Michael Sacks - 05/31/89
 *****   updated for x11r5/rs6k protocol   -  Doris Chow  - 10/03/91
 */
#include <stdio.h>
  /* standard X11 includes */
#define NEED_EVENTS
#include <X11/Xlibint.h>        /* includes Xproto.h and Xlib.h */
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Xos.h>
/* 
 * PLI extension includes:  XPLIlib.h includes PLIproto.h which includes 
 * PLIsstk.h which includes PLIcoord.h
 */
#include <XPLIlib.h>
  /* 
   * defaults that will be used in wsimple.h - note that these must be defined 
   * before wsimple.h is included
   */
#define TITLE_DEFAULT "PLIalign"
#define ICON_NAME_DEFAULT "PLIalign"
#define DEFX_DEFAULT 50
#define DEFY_DEFAULT 50
#define DEFWIDTH_DEFAULT 120
#define DEFHEIGHT_DEFAULT 30
#define BORDER_WIDTH_DEFAULT 4
#define REVERSE_DEFAULT 0
#define BORDER_DEFAULT "black"
#define BACK_COLOR_DEFAULT "white"
#define FORE_COLOR_DEFAULT "black"
#define BODY_FONT_DEFAULT "ncenb18"
#define MIN_X_SIZE 120
#define MAX_X_SIZE 120
#define MIN_Y_SIZE 30
#define MAX_Y_SIZE 30
  
#include "wsimple.h"

/*
 * Report the syntax for program; X_USAGE is a macro defined twice in 
 * wsimple.h.  Obviously, the second declaration is what counts
 */
void usage()
{
  fprintf(stderr,
	  "%s usage:  %s %s ", program_name,program_name,X_USAGE);
  exit(1);
}


void main(argc, argv)
     int argc;
     char **argv;
     
{
  
  /* returned by PLIRegisterExtension() - declared in XPLIlib.h */
  PLIExtData *pextdata;
  int i;
  

  /* macro from wsimple.h to get program name */
  INIT_NAME;

  /* 
   * open the display connection, get the Xdefaults, parse the standard X11 
   * command line args - in wsimple.h.  The variables dpy and screen are 
   * defined in wsimple.h and are initialized by this routine
   */
   Get_X_Options(&argc, argv);

  /* now parse the application specific args */
  for (i = 1; i < argc; i++) {
    if (!strcmp("-help", argv[i])) usage ();
    if (!strcmp("-", argv[i]))
      continue;
  }
  
  
    /* register the extension */
  if ( (  pextdata = PLIRegisterExtension(dpy) ) == (PLIExtData *)NULL)
    {
      printf("PLIalign:  could not open extensions\n");
      exit(1);
    }
  PLIAlign(dpy);
  exit(0);
}
