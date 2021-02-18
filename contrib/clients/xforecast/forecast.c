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

Widget
  Message;

char *
GetForecast(city)
char *city;

{

  FILE *whatever;
  char *forecast;
  char command[100];
  char c;
  int  size  = 1;
  int  index = 0;
  
  forecast = (char *) malloc(1000);
  (void) memset (forecast, '\0', 1000);
  (void) sprintf (command, "%s %s", WEATHER, city);

  if ((whatever = popen (command, "r")) == NULL)
    {
      (void) fprintf (stderr,"popen of [%s] failed\n", command);
      exit (1);
    }

  while ((forecast[index++] = getc(whatever)) != EOF)
    {
      if (index % 1000 == 0)
	{
	  size++;
	  forecast = (char *) realloc (forecast, 1000*size);
	}
    }
  forecast[index - 1] = '\0';
  pclose(whatever);
  return (forecast);
}

void
PopItUp (city, string)

char *city;
char *string;

{
  Arg
    args[15];

  int
    cnt;

  Widget
    popup,
    frame, 
    text,
    thing;

  char title[100];

  (void) sprintf (title, "Forecast for %s", city);

  cnt = 0;
  popup = XtAppCreateShell (title, CLASS, topLevelShellWidgetClass,
			    XtDisplay(Toplevel), args, cnt);


  /* create a frame widget to hold things */
  cnt = 0;
  frame = XtCreateManagedWidget ("Frame", formWidgetClass, 
				 popup, args, cnt);

  cnt = 0;
  XtSetArg (args[cnt], XtNborderWidth, 2);                   cnt++;
  XtSetArg (args[cnt], XtNright, XtChainLeft);               cnt++;
  XtSetArg (args[cnt], XtNlabel, "Press to Destroy Window"); cnt++;
  XtSetArg (args[cnt], XtNwidth, 550);                       cnt++;
  thing = XtCreateManagedWidget ("donebutton", 
				 commandWidgetClass,
				 frame, args, cnt);
  
  XtAddCallback (thing, XtNcallback, DestroyPopup, NULL);

  cnt = 0;
  XtSetArg (args[cnt], XtNfromVert,        thing);                cnt++;
  XtSetArg (args[cnt], XtNeditType,   XawtextRead);               cnt++;
  XtSetArg (args[cnt], XtNscrollHorizontal, XawtextScrollWhenNeeded); cnt++;
  XtSetArg (args[cnt], XtNscrollVertical,   XawtextScrollAlways);  cnt++;
  XtSetArg (args[cnt], XtNtype, XawAsciiString);                  cnt++;
  XtSetArg (args[cnt], XtNstring, string);                        cnt++;
  XtSetArg (args[cnt], XtNwidth, 550);                            cnt++;
  XtSetArg (args[cnt], XtNheight, 250);                           cnt++;

  text = XtCreateManagedWidget ("text", asciiTextWidgetClass,
				frame, args, cnt);

  XtRealizeWidget (popup);

  return;
}

void
DestroyPopup(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;

{

  XtDestroyWidget(XtParent(XtParent(w)));
  Redraw (XtNameToWidget (Toplevel, "*drawingarea"));
  return;
}

void
PopupMessage (string)

char *string;
{
  Arg
    args[15];

  int
    cnt;

  Dimension
    width, 
    height;

  Position
    x,
    y;

  unsigned long
    foreground;

  Widget
    popup,
    frame, 
    label;


  cnt = 0;
  XtSetArg (args[cnt], XtNx,            &x);                cnt++;
  XtSetArg (args[cnt], XtNy,            &y);                cnt++;
  XtSetArg (args[cnt], XtNwidth,    &width);                cnt++;
  XtSetArg (args[cnt], XtNheight,  &height);                cnt++;
  XtGetValues (Toplevel, args, cnt);

  cnt = 0;
  XtSetArg (args[cnt], XtNoverrideRedirect, True);          cnt++;
  XtSetArg (args[cnt], XtNx,  x + width/2);                 cnt++;
  XtSetArg (args[cnt], XtNy,  y + height/2);                cnt++;
  Message = XtAppCreateShell ("message", CLASS, transientShellWidgetClass,
			    XtDisplay(Toplevel), args, cnt);


  /* create a frame widget to hold things */
  cnt = 0;
  frame = XtCreateManagedWidget ("Frame", formWidgetClass, 
				 Message, args, cnt);

  cnt = 0;
  XtSetArg (args[cnt], XtNborder, 2);                  cnt++; 
  XtSetArg (args[cnt], XtNlabel,string);               cnt++;
  
  label = XtCreateManagedWidget ("messagelabel", 
			  labelWidgetClass,
			  frame, args, cnt);
  

  XtRealizeWidget (Message);

  do 
    {
      XtAppProcessEvent (Appcon, XtIMAll);
    } while (XtAppPending(Appcon));
  return;
}

void
PopdownMessage()

{
  XtDestroyWidget (Message);
}
