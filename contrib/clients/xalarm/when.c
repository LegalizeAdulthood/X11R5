/*
	 Copyright (c) 1991 by Simon Marshall, University of Hull, UK

		   If you still end up late, don't blame me!
				       
  Permission to use, copy, modify, distribute, and sell this software and its
       documentation for any purpose and without fee is hereby granted,
    provided that the above copyright notice appear in all copies and that
	both that copyright notice and this permission notice appear in
			   supporting documentation.
				       
  This software is provided AS IS with no warranties of any kind.  The author
    shall have no liability with respect to the infringement of copyrights,
     trade secrets or any patents by this file or any part thereof.  In no
      event will the author be liable for any lost revenue or profits or
	      other special, indirect and consequential damages.
*/

/* 
 * Stuff for getting the time the alarm is due to go off, and 
 * confirmation if needed.
 */



#include "xalarm.h"

#include <X11/Xaw/Dialog.h>


#define		TIMEINSTRUCTIONS	"Enter time (as [+]time[am/pm]):"
#define		WARNINGINSTRUCTIONS	"Enter warnings (as time[,time...]):"
#define		CONFIRMFORMAT		"%s, %s (in %d:%02d), "



void		PopupAndAsk(), EnteredTime(), EnteredWarnings(), Confirmed();
static void	PopupAndConfirm(), Popup(), ToggleWhen(), Cancel();
extern void	Quit(), AddTimeOuts(), SetWarningTimes();
extern long	TimeToMilliSeconds();



extern AlarmData	xalarm;



/* 
 * just pop up a dialog widget to get an at/in time.  won't pop down 
 * until a valid time is given.  EnteredTime() does this.
 */

void PopupAndAsk ()
{
    Widget    popup;
    Display  *display = XtDisplay (xalarm.toplevel);
    Arg       warg[1];

    popup = XtCreatePopupShell ("When?", topLevelShellWidgetClass, xalarm.toplevel,
				NULL, 0);

    xalarm.gettimewidget = XtCreateManagedWidget ("when", dialogWidgetClass, popup,
						  (ArgList) NULL, 0);
    XawDialogAddButton (xalarm.gettimewidget, "ok", EnteredTime, (XtPointer) NULL);
    XawDialogAddButton (xalarm.gettimewidget, "toggle", ToggleWhen, (XtPointer) False);
    XawDialogAddButton (xalarm.gettimewidget, "quit", Quit, (XtPointer) NULL);

    xalarm.getwtimewidget = XtCreateManagedWidget ("warnings", dialogWidgetClass,
						   popup, (ArgList) NULL, 0);
    XawDialogAddButton (xalarm.getwtimewidget, "ok", EnteredWarnings, (XtPointer) NULL);
    XawDialogAddButton (xalarm.getwtimewidget, "toggle", ToggleWhen, (XtPointer) True);
    XawDialogAddButton (xalarm.getwtimewidget, "quit", Quit, (XtPointer) NULL);

    xalarm.confirmwidget = XtCreateManagedWidget ("confirm", dialogWidgetClass, popup,
						  (ArgList) NULL, 0);
    XawDialogAddButton (xalarm.confirmwidget, "ok", Confirmed, (XtPointer) NULL);
    XawDialogAddButton (xalarm.confirmwidget, "cancel", Cancel, (XtPointer) NULL);
    XawDialogAddButton (xalarm.confirmwidget, "quit", Quit, (XtPointer) NULL);

    if (xalarm.geometry != NULL) {
	XtSetArg (warg[0], XtNgeometry, xalarm.geometry);
	XtSetValues (popup, warg, 1);
    }
    XtPopup (popup, XtGrabExclusive);
    XFlush (display);

    if (ISINVALID (xalarm.timeout))
	Popup (xalarm.gettimewidget, TIMEINSTRUCTIONS, xalarm.timestr,
	       (Widget) NULL, (String *) NULL);
    else
	if (ISINVALID (xalarm.numwarnings))
	    Popup (xalarm.getwtimewidget, WARNINGINSTRUCTIONS, xalarm.warningsstr,
		   (Widget) NULL, (String *) NULL);
	else
	    PopupAndConfirm ();
}



/* 
 * If invalid, stay.  Otherwise, if the warnings are invalid, popup 
 * and get them.  If confirm, do that, otherwise fake acceptance.
 */

void EnteredTime (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    Arg    warg[1];
    char   message[TEXT];

    xalarm.timeout = TimeToMilliSeconds
	(xalarm.timestr = XawDialogGetValueString (xalarm.gettimewidget));

    if (ISINVALID (xalarm.timeout)) {
	(void) sprintf (message, "Invalid.  %s", TIMEINSTRUCTIONS);
	XtSetArg (warg[0], XtNlabel, message);
	XtSetValues (xalarm.gettimewidget, warg, 1);
    }
    else
	if (ISINVALID (xalarm.numwarnings))
	    ToggleWhen ((Widget) NULL, (XtPointer) NULL, (XtPointer) False);
	else
	    if (xalarm.confirm)
		PopupAndConfirm ();
	    else
		Confirmed ((Widget) NULL, (XtPointer) NULL, (XtPointer) NULL);
}



/* 
 * If invalid, stay.  Otherwise, if the alarm time is invalid, popup 
 * and get it.  If confirm, do that, otherwise fake acceptance.
 */

void EnteredWarnings (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    Arg    warg[1];
    char   message[TEXT];

    SetWarningTimes (xalarm.warningsstr =
		     XawDialogGetValueString (xalarm.getwtimewidget));

    if (ISINVALID (xalarm.numwarnings)) {
	(void) sprintf (message, "Invalid.  %s", WARNINGINSTRUCTIONS);
	XtSetArg (warg[0], XtNlabel, message);
	XtSetValues (xalarm.getwtimewidget, warg, 1);
    }
    else
	if (ISINVALID (xalarm.timeout))
	    ToggleWhen ((Widget) NULL, (XtPointer) True, (XtPointer) NULL);
	else
	    if (xalarm.confirm)
		PopupAndConfirm ();
	    else
		Confirmed ((Widget) NULL, (XtPointer) NULL, (XtPointer) NULL);
}



/* 
 * Just toggle whether i/t concerns alarm time/warning time(s).
 */

static void ToggleWhen (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    if ((Boolean) clientdata == True)
	Popup (xalarm.gettimewidget, TIMEINSTRUCTIONS, xalarm.timestr,
	       xalarm.getwtimewidget, &xalarm.warningsstr);
    else
	Popup (xalarm.getwtimewidget, WARNINGINSTRUCTIONS, xalarm.warningsstr,
	       xalarm.gettimewidget, &xalarm.timestr);
}



/* 
 * Switch the widget in the popup from gettimewidget to confirmwidget,
 * reset the focus too, otherwise the kbd input still goes to gettimewidget.
 */

static void PopupAndConfirm ()
{
    String   timestr, ctime();
    char     message[TEXT];
    time_t   now, time();
    int      count = 0, i;

    (void) time (&now);
    /* 
     * We may as well recalculate here too.
     */
    now += (TimeToMilliSeconds (xalarm.timestr) / 1000);

    timestr = ctime (&now);
    timestr[3] = timestr[16] = '\0';

    (void) sprintf (message, CONFIRMFORMAT, timestr, timestr+11,
		    (xalarm.timeout/60000) / 60, (xalarm.timeout/60000) % 60);
    for (i=0; i<xalarm.numwarnings; i++)
	if (xalarm.timeout > xalarm.warnings[i])
	    count++;
    if (count == 0)
	(void) sprintf (message + strlen (message), "no warnings:");
    else 
	(void) sprintf (message + strlen (message), "warning%s: ", PLURAL (count));
    count = 0;
    for (i=0; i<xalarm.numwarnings; i++)
	if (xalarm.timeout > xalarm.warnings[i]) {
	    count++;
	    if (count != 1)
		(void) sprintf (message + strlen (message), ",");
	    MAKETIME (message + strlen (message), xalarm.warnings[i]/(1000*60));
	}

    Popup (xalarm.confirmwidget, message, xalarm.messagestr,
	   (Widget) NULL, (String *) NULL);
}



/* 
 * Yow!  Let's go!
 */

void Confirmed (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    Arg      warg[1];
    String   message = XawDialogGetValueString (xalarm.confirmwidget);

    if (*message) {
	XtSetArg (warg[0], XtNlabel, xalarm.messagestr = message);
	XtSetValues (xalarm.messagewidget, warg, 1);
    }

    /* 
     * Reset the timeout, since the gap between entering 
     * and confirming may be significant.
     */

    xalarm.timeout = TimeToMilliSeconds (xalarm.timestr);
    AddTimeOuts ();

    XtPopdown (XtParent (xalarm.gettimewidget));
}



/* 
 * Swap back to gettime widget & remove kbd focus.
 */

static void Cancel (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    Popup (xalarm.gettimewidget, TIMEINSTRUCTIONS, xalarm.timestr,
	   xalarm.confirmwidget, &xalarm.messagestr);
}



/* 
 * Let's get this widget to the top and set the kbd focus to it.
 * While we're at it, save the value into oldvalue for the oldwidget,
 * and stick the new label and value for the new widget.
 */

static void Popup (widget, label, value, oldwidget, oldvalue)
  Widget   widget, oldwidget;
  String   label, value, *oldvalue;
{
    Arg   warg[2];

    if (oldwidget != (Widget) NULL)
	*oldvalue = XawDialogGetValueString (oldwidget);

    XtSetArg (warg[0], XtNlabel, label);
    XtSetArg (warg[1], XtNvalue, value);
    XtSetValues (widget, warg, 2);

    XtSetMappedWhenManaged (xalarm.gettimewidget, (widget == xalarm.gettimewidget));
    XtSetMappedWhenManaged (xalarm.getwtimewidget, (widget == xalarm.getwtimewidget));
    XtSetMappedWhenManaged (xalarm.confirmwidget, (widget == xalarm.confirmwidget));

    if (widget == xalarm.gettimewidget)
	XtSetKeyboardFocus (xalarm.gettimewidget, (Widget) NULL);
    else
	XtSetKeyboardFocus (xalarm.gettimewidget, widget);
}
