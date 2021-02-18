/* -*-C-*-
********************************************************************************
*
* File:         w_utils.c
* RCS:          $Header: w_utils.c,v 1.5 91/03/24 18:49:22 mayer Exp $
* Description:  Various X Functionality
* Author:       Niels Mayer, HPLabs
* Created:      Fri Sep 29 01:24:38 1989
* Modified:     Thu Oct  3 21:23:14 1991 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r5 contrib tape release
*
* WINTERP Copyright 1989, 1990, 1991 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
*
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: w_utils.c,v 1.5 91/03/24 18:49:22 mayer Exp $";

#include <stdio.h>
#include <Xm/Xm.h>
#include <X11/cursorfont.h>	/* defines XC_crosshair */
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"


/******************************************************************************
 ** (GET_MOUSE_LOCATION)
 ** 
 ** [ NEW ]:  it returns a dotted pair ... (root_x . root_y)
 **
 ** Primitive written by Richard Hess, Consilium, uunet!cimshop!rhess.
 ** Fixes applied by Niels Mayer...
 ******************************************************************************/
LVAL Wut_Prim_GET_MOUSE_LOCATION()
{
  extern Display* display;	/* global in winterp.c */
  extern Window   root_win;	/* global in winterp.c */
  LVAL            lval_result, lval_x, lval_y;
  int             x_rtn, y_rtn;
  Window          junk1, junk2;
  int             junk3, junk4;
  unsigned int    junk5;
  
  /* protect some pointers -- added by NPM */
  xlstkcheck(3);
  xlsave(lval_x);
  xlsave(lval_y);
  xlsave(lval_result);

  if (!XQueryPointer(display, root_win, &junk1, &junk2,
		     &x_rtn, &y_rtn, &junk3, &junk4, &junk5))
    xlerror("XQueryPointer() failed...");
  lval_x = cvfixnum(x_rtn);
  lval_y = cvfixnum(y_rtn);
  lval_result = cons(lval_x, lval_y);

  /* restore the stack */
  xlpopn(3);

  return (lval_result);
}


#ifdef hpux /* make this HPUX-only since sleepms() not portable */
/******************************************************************************
 * (X_REFRESH_DISPLAY [<sleep>])
 * A kludgy hack to work around the Motif bug with refreshing and displaying popup
 * status dialogues before embarking on a long computation. Use this function to
 * work around cases where (send <widget> :update_display) isn't doing the right
 * thing.
 *
 * The optional FIXNUM argument <sleep> is the number of milliseconds of sleep 
 * time after popping up a shell before further expose events are procesed.
 * The latter set of expose events correspond to drawing the "insides" of a
 * popup dialog, e.g., the text and pixmaps.
 *
 * Note that making <sleep> too small means that the expose events generated
 * by popping up a shell will not have time to round trip to the X server and
 * back to the Motif client (WINTERP).
 *
 * If <sleep> is ommitted, then the sleeptime defaults to 300 millisecods.
 * This time was empirically determined to be ok for my applications, but
 * may not work if your workstation, X server, or network is slower than mine...
 * As I said, this is a hack to work around a motif bug.
******************************************************************************/
LVAL Wut_Prim_X_REFRESH_DISPLAY()
{
  extern sleepms();		/* from utils.c */
  extern Display* display;	/* global in winterp.c */
  extern LVAL true;
  int sleeptime;
  XEvent event;

  /* get optional <sleep> arg */
  if (moreargs())
    sleeptime = (int) getfixnum(xlgafixnum());
  else
    sleeptime = 300;		/* default value for <sleep> */

  xllastarg();

  XSync(display, FALSE);
  while (XCheckMaskEvent(display, ExposureMask, &event))
    XtDispatchEvent(&event);

  sleepms(sleeptime);

  XSync(display, FALSE);
  while (XCheckMaskEvent(display, ExposureMask, &event))
    XtDispatchEvent(&event);

  return (true);
}
#endif				/* hpux */


/******************************************************************************
 * (X_ALLOC_COLOR <color>)
 * where <color> is a string, either a colorname from /usr/lib/X11/rgb.txt
 * or a hexadecimal color specification "#RRGGBB".
 * it returns a Pixel-value for the color.
******************************************************************************/
LVAL Wut_Prim_XAllocColor()
{
  extern Display* display;	/* global in winterp.c */
  extern Screen*  screen;	/* global in winterp.c */
  extern Colormap colormap;	/* global in winterp.c */
  XColor        screenColor;
  LVAL          str_color;

  str_color = xlgastring();
  xllastarg();
  
  if (!XParseColor(display, colormap, (String) getstring(str_color), &screenColor))
    xlerror("XParseColor() couldn't parse color specification.", str_color);
  if (!XAllocColor(display, colormap, &screenColor))
    xlerror("XAllocColor() couldn't allocate specified color.", str_color);
  return (cv_pixel(screenColor.pixel));
}


/******************************************************************************
 * (X_STORE_COLOR <pixel> <color>)    [nicer would be (send <pixel> :store <color>)]
 * where <color> is a string, either a colorname from /usr/lib/X11/rgb.txt
 * or a hexadecimal color specification "#RRGGBB".
 * it returns a Pixel-value for the color.
******************************************************************************/
LVAL Wut_Prim_X_STORE_COLOR()
{
  extern Display* display;	/* global in winterp.c */
  extern Screen*  screen;	/* global in winterp.c */
  extern Colormap colormap;	/* global in winterp.c */
  XColor        screenColor;
  LVAL          str_color;
  LVAL		lval_pixel;

  lval_pixel = xlga_pixel();
  str_color = xlgastring();
  xllastarg();
  
  screenColor.pixel = get_pixel(lval_pixel);
  if (!XParseColor(display, colormap, (String) getstring(str_color), &screenColor))
    xlerror("XParseColor() couldn't parse color specification.", str_color);
  if (!XStoreColor(display, colormap, &screenColor))
    xlerror("XStoreColor() couldn't allocate specified color.", str_color);
  return (lval_pixel);
}


/******************************************************************************
 * (X_ALLOC_N_COLOR_CELLS_NO_PLANES <num-cells>)
 * returns an array of <num-cells> <pixel-objects> see Oliver Jones, p. 278
******************************************************************************/
LVAL Wut_Prim_X_ALLOC_N_COLOR_CELLS_NO_PLANES()
{
  extern Display* display;	/* global in winterp.c */
  extern Colormap colormap;	/* global in winterp.c */
  Pixel*        pixels;
  int		i, num_cells;
  LVAL		result;

  num_cells = getfixnum(xlgafixnum());
  xllastarg();
  if (num_cells <= 0)
    return (NIL);
  
  pixels = (Pixel*) XtMalloc((unsigned) (num_cells * sizeof(Pixel)));
  XAllocColorCells(display, colormap, FALSE, NULL, 0, pixels, num_cells);

  xlsave1(result);
  result = newvector(num_cells);
  for (i = 0; i < num_cells; i++)
    setelement(result, i, cv_pixel(pixels[i]));
  xlpop();
  XtFree(pixels);
  return (result);
}


/******************************************************************************
 * (GET_MOUSED_WIDGET)
 * evaluating this function will change the cursor to a crossbar, indicating
 * that the user is to 'click' the mouse to designate an object on the screen.
 * If the user clicks on a visual item within WINTERP, this fucntion will
 * return the WIDGETOBJ associated with the visual item. 
 ******************************************************************************/
LVAL Wut_UserClick_To_WidgetObj()
{
  extern Display* display;	/* global in winterp.c */
  extern Window   root_win;	/* global in winterp.c */
  extern LVAL     Wcls_WidgetID_To_WIDGETOBJ();	/* from w_classes.c */
  extern XmGadget _XmInputInGadget(); /* in Xm/GadetUtils.c extern'd in XmP.h */
  Cursor	  cursor = XCreateFontCursor(display, XC_crosshair);
  Window          parent_win, cur_win, child_win;
  int             win_x, win_y;
  Widget          widget_id, gadget_id;
  XEvent	  event;
  Bool            xtc_ok;
  
  xllastarg();

  if (GrabSuccess != XGrabPointer(display, root_win, 0, ButtonPressMask|ButtonReleaseMask,
				  GrabModeAsync, GrabModeAsync, None, cursor,
#ifdef WINTERP_MOTIF_11
				  XtLastTimestampProcessed(display)
#else
				  CurrentTime
#endif				/* WINTERP_MOTIF_11 */
				  ))
    xlfail("GET_MOUSED_WIDGET -- couldn't grab pointer (XGrabPointer() failed).");
  
  XWindowEvent(display, root_win, ButtonPressMask, &event); /* remove the buttonpress from the queue*/
  XWindowEvent(display, root_win, ButtonReleaseMask, &event); /* get the buttonrelease event */
  XUngrabPointer(display,
#ifdef WINTERP_MOTIF_11
		 XtLastTimestampProcessed(display)
#else
		 CurrentTime
#endif				/* WINTERP_MOTIF_11 */
		 );
  XFlush(display);

  if (!event.xbutton.subwindow)
    xlfail("GET_MOUSED_WIDGET aborted -- you clicked on the root window.");

  parent_win = event.xbutton.window; /* ASSERT event.xbutton.window == root_win, due to using XWindowEvent(root_win) */
  win_x      = event.xbutton.x;
  win_y      = event.xbutton.y;
  cur_win    = event.xbutton.subwindow;
  while ((xtc_ok = XTranslateCoordinates(display,
					 parent_win, cur_win,
					 win_x, win_y, /* give the x,y coords of event in parent_w */
					 &win_x, &win_y, /* return the x,y coords relative to cur_win */
					 &child_win)) /* returns child window of cur_win if that contains coords, else nil */
	 && child_win) {
#ifdef DEBUG_WINTERP_1
    fprintf(stderr, "parent_win=%lx, cur_win=%lx, child_win=%lx\n", parent_win, cur_win, child_win);
#endif
    parent_win = cur_win;
    cur_win    = child_win;
  }

#ifdef DEBUG_WINTERP_1
  fprintf(stderr, "	Smallest window containing userclick is %lx\n", cur_win);
#endif

  if (!xtc_ok)
    xlfail("Bug in GET_MOUSED_WIDGET -- XTranslateCoordinates() failed.");

  if (!(widget_id = XtWindowToWidget(display, cur_win)))
    xlfail("GET_MOUSED_WIDGET -- Couldn't find widget associated with window.\n	(Is the selected widget/window inside a different application?).\n");

  /* if the widget is a composite it may be managing a gadget -- attempt to retrieve it by looking up x,y coords in manager */
  if (XtIsComposite(widget_id) &&
      (gadget_id = (Widget) _XmInputInGadget(widget_id, win_x, win_y)))
    return (Wcls_WidgetID_To_WIDGETOBJ(gadget_id)); /* then return the WIDGETOBJ assoc'd with gadget */
  else
    return (Wcls_WidgetID_To_WIDGETOBJ(widget_id)); /* otherwise, we return the WIDGETOBJ assoc'd with smallest window */
}


/******************************************************************************
 * (load <fname> [:verbose] [:print])
 *
 * This function overrides xlisp/xlsys.c:xload(). All it does is check
 * <fname> for '/' or '.' as the first character. If those don't exist, then
 * the value of X resource "lispLibDir" is prepended and used as the filename.
 * Note that "lispLibDir" should be the path to an existing directory with
 * a trailing '/', e.g. "/usr/local/winterp/lisp-lib/"
 ******************************************************************************/
LVAL Wut_Prim_LOAD()
{
  extern LVAL k_verbose,k_print,true;
  extern int xlgetkeyarg();	/* from xlisp/xlsubr.c */
  extern char temptext[];	/* from winterp.c */
  unsigned char *name;
  int vflag,pflag;
  LVAL arg;

  /* get the file name */
  name = getstring(xlgetfname());

  /* get the :verbose flag */
  if (xlgetkeyarg(k_verbose,&arg))
    vflag = (arg != NIL);
  else
    vflag = TRUE;

  /* get the :print flag */
  if (xlgetkeyarg(k_print,&arg))
    pflag = (arg != NIL);
  else
    pflag = FALSE;

  /* load the file */
  if ((name[0] != '/') && (name[0] != '.')) {
    strcpy(temptext, user_prefs.lisp_lib_dir); /* prepend Xdefault 'lispLibDir', assume it has trailing '/' */
    strcat(temptext, name);
    return (xlload(temptext, vflag, pflag) ? true : NIL);
  }
  else
    return (xlload(name, vflag, pflag) ? true : NIL);
}
