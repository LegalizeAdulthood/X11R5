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

#include "types.h"

/*
 * build up what we see
 */


static int alarmclock = 1;

#include "xforecast.xbm"
#include "xfmask.xbm"
#include "usa.xbm"

void
SetupInterface()

{
  int cnt;

  Arg args[15];

  Widget
    Frame,
    thing,
    label,
    draw;

  Display
    *display;

  int
    screen, depth,
    xhot, yhot;

  unsigned long
    background,
    foreground;

  unsigned int
    uwidth,
    uheight;

  Pixmap
    backgroundpix, icon, iconmask;


  static XtActionsRec actionsTable[] = {
    {"quit", quit},
  };

  static char defaultTranslations[] = "<Key>Q:  quit()";

  XtTranslations trans_table;

  /* create a graphics context (GC) */

  display    = XtDisplay (Toplevel);
  screen     = DefaultScreen (display);

  background = WhitePixel (display, screen);
  foreground = BlackPixel (display, screen);

  drawGC     =   XCreateGC (display, XDefaultRootWindow(display), 0, 0);
  xorGC      =   XCreateGC (display, XDefaultRootWindow(display), 0, 0);

  XSetBackground (display, drawGC, background);
  XSetForeground (display, drawGC, foreground);

  XSetBackground (display, xorGC, background);
  XSetForeground (display, xorGC, foreground);
  XSetFunction   (display, xorGC, GXxor);

  cnt = 0;
  XtSetArg(args[cnt], XtNdepth, &depth);            cnt++; 
  XtSetArg(args[cnt], XtNbackground,  &background); cnt++;
  XtSetArg(args[cnt], XtNborderColor, &foreground); cnt++;
  XtGetValues (Toplevel, args, cnt);

  /* create a frame widget to hold things */
  cnt = 0;
  Frame = XtCreateManagedWidget ("Frame", formWidgetClass, 
				 Toplevel, args, cnt);

  backgroundpix = XCreatePixmapFromBitmapData (XtDisplay(Toplevel),
				 XDefaultRootWindow(XtDisplay(Toplevel)),
					 usa_bits, usa_width, usa_height,
					 foreground, background, depth);

  cnt = 0;
  XtSetArg (args[cnt], XtNborder, 2);                       cnt++;
  XtSetArg (args[cnt], XtNright, XtChainRight);             cnt++;
  XtSetArg (args[cnt], XtNlabel, "Press to Quit");          cnt++;
  XtSetArg (args[cnt], XtNwidth, usa_width);                cnt++;

  thing = XtCreateManagedWidget ("quitbutton", 
				 commandWidgetClass,
				 Frame, args, cnt);
  XtAddCallback (thing, XtNcallback, quit, NULL);

  cnt = 0;
  XtSetArg (args[cnt], XtNfromVert, thing);                   cnt++;
  XtSetArg (args[cnt], XtNborder, 2);                         cnt++;
  XtSetArg (args[cnt], XtNwidth, usa_width);                  cnt++;
  XtSetArg (args[cnt], XtNheight, usa_height);                cnt++;
  XtSetArg (args[cnt], XtNbackgroundPixmap, backgroundpix);   cnt++; 
  XtSetArg (args[cnt], XtNdepth, depth);                      cnt++; 

  draw = XtCreateManagedWidget ("drawingarea", formWidgetClass,
				Frame, args, cnt);
  XtAddEventHandler (draw, PointerMotionMask, False, MotionHandle, NULL);
  XtAddEventHandler (draw, ExposureMask,      False, RedrawHandle, NULL);
  XtAddEventHandler (draw, ButtonPressMask,   False, InputHandle,  NULL);
  XtManageChild (draw);

  cnt = 0;
  XtSetArg (args[cnt], XtNborder, 2);                       cnt++;
  XtSetArg (args[cnt], XtNfromVert, draw);                  cnt++;
  XtSetArg (args[cnt], XtNright, XtChainRight);             cnt++;
  XtSetArg (args[cnt], XtNwidth, usa_width);                cnt++;
  XtSetArg (args[cnt], XtNlabel,"      ");                  cnt++;
  
  label = XtCreateManagedWidget ("label", 
				 labelWidgetClass,
				 Frame, args, cnt);

  XtRealizeWidget (Toplevel);

  XtAppAddActions(Appcon, actionsTable, XtNumber(actionsTable));
  trans_table = XtParseTranslationTable (defaultTranslations);
  XtOverrideTranslations (thing, trans_table);
  XtOverrideTranslations (Frame, trans_table);


  icon = XCreateBitmapFromData (display, XtWindow (Toplevel),
				xforecast_bits, xforecast_width, 
				xforecast_height);
  iconmask = XCreateBitmapFromData (display, XtWindow (Toplevel),
				xfmask_bits, xfmask_width, xfmask_height);
  cnt = 0;
  XtSetArg (args[cnt], XtNiconPixmap, icon);   cnt++;
  XtSetArg (args[cnt], XtNiconMask, icon);   cnt++;
  XtSetValues (Toplevel, args, cnt);

  return;

}

void
Redraw (w)
Widget w;
{

  int i;

  Window
    drawwindow;

  Display
    *display;

  display    = XtDisplay(w);
  drawwindow = XtWindow (w);

  XClearWindow (display, drawwindow);

  for (i = 0; i < CityIndex; i++)
    {
      XFillArc (display, drawwindow, drawGC, 
		City[i].x-POINTRADIUS, 
		City[i].y-POINTRADIUS, 
		2*POINTRADIUS, 2*POINTRADIUS, 0, 360*64);

    }
  alarmclock = 1;
  XFlush(display);
}

void
RedrawHandle(w, client_data, event)

Widget      w;
caddr_t     client_data;
XEvent *event;

{
  if (event->xexpose.count == 0)
    {
      Redraw(w);
    }
  return;
}

void
InputHandle(w, client_data, event)
Widget w;
caddr_t client_data;
XEvent *event;

{
  char *weather;
  char message[200];
  int i;
  
  if (event->xbutton.type == ButtonPress)
    {
#ifdef DEBUG
      (void) fprintf (stderr, "Button was clicked at [%d,%d]\n",
		      event->xbutton.x, event->xbutton.y);
#endif

      if ((i = FindCity(event->xbutton.x, 
			event->xbutton.y)) != -1)
	{
	  if (event->xbutton.button == 1)
	    {
	      (void) sprintf (message, "Getting forecast for %s, %s (%s)",
			     City[i].cityname, City[i].state, City[i].citycode);
	      PopupMessage(message);
	      weather = GetForecast(City[i].citycode);
	      PopdownMessage(); 
	      PopItUp(City[i].cityname, weather);
	      XtFree (weather);
	    }
	  else if (event->xbutton.button == 2)
	    { 
	      (void) sprintf (message, "Getting forecast for state of %s",
			      City[i].state);
	      PopupMessage(message);
	      weather = GetForecast(City[i].state);
	      PopdownMessage();
	      PopItUp(City[i].state, weather);
	      XtFree (weather);
	    }
	}
    }
  return;
}
void
MotionHandle(w, client_data, event)
Widget w;
caddr_t client_data;
XMotionEvent *event;

{
  char message[200];
  int i, cnt;
  static int last = -1;
  Arg args[10];
  Widget label;
  Display *display;
  Window  drawwindow;

  if ((i = FindCity(event->x, event->y)) != -1)
	{
	  (void) sprintf (message, "Pointer now near %s, %s", 
			  City[i].cityname, City[i].state);
	} else {
	  strcpy (message, "    ");
	}

  if (i != last)
    {
      display    = XtDisplay (w);
      drawwindow = XtWindow (w);
      cnt = 0;
      XtSetArg (args[cnt], XtNlabel, message);      cnt++;
      label = XtNameToWidget (Toplevel, "*label");
      XtSetValues (label, args, cnt);
      XtUnmanageChild (label);
      XtManageChild (label);

      if (last != -1)
	{
	  XDrawArc (display, drawwindow, xorGC, 
		    City[last].x-POINTRADIUS-1, 
		    City[last].y-POINTRADIUS-1, 
		    2*POINTRADIUS+2, 2*POINTRADIUS+2, 0, 360*64);

	  XFillArc (display, drawwindow, xorGC, 
		    City[last].x-POINTRADIUS, 
		    City[last].y-POINTRADIUS, 
		    2*POINTRADIUS, 2*POINTRADIUS, 0, 360*64);
	}

      XFillArc (display, drawwindow, xorGC, 
		City[i].x-POINTRADIUS, 
		City[i].y-POINTRADIUS, 
		2*POINTRADIUS, 2*POINTRADIUS, 0, 360*64);
      
      XDrawArc (display, drawwindow, xorGC, 
		City[i].x-POINTRADIUS-1, 
		City[i].y-POINTRADIUS-1, 
		2*POINTRADIUS+2, 2*POINTRADIUS+2, 0, 360*64);

      last = i;
    }
  return;
}

void
ResizeHandle()
{
  return;
}

void quit()
{
  exit(0);
}
