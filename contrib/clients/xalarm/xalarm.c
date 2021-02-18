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
 * The marvelous xalarm proggie, or, how to write an X prog in 2 wks.
 * 
 * Main bit of prog, just deal with parsing command line options etc 
 * (well, letting XtAppInitialize() do it) & setting up stuff by 
 * calling out to other fns.
 */



#include "xalarm.h"


void		DoAlarm();
static void	SetAlarm();
extern void	Initialise(), CreateAlarmWidget(), PopupAndAsk(), DoAppointments();
extern SIGRET	ResetAlarm();
extern void	AddTimeOuts(), EnteredTime(), EnteredWarnings(), Confirmed();
extern void	SetWarningTimes(), ShowSnoozeValue(), ShowClickToZero(), Quit();
extern long	TimeToMilliSeconds();
extern String   Concat();
extern String  *PreParseArgList();



static XtResource resources[] = {
    {XtNtime,		XtCTime,	XtRString,	sizeof (String),
     XtOffset (ApplDataPtr, time),	XtRString,	""},
    {XtNwarnings,	XtCWarnings,	XtRString,	sizeof (String),
     XtOffset (ApplDataPtr, warnings),	XtRString,	NULL},
    {XtNsnooze,		XtCSnooze,	XtRString,	sizeof (String),
     XtOffset (ApplDataPtr, snooze),	XtRString,	NULL},
    {XtNconfirm,	XtCConfirm,	XtRBoolean,	sizeof (Boolean),
     XtOffset (ApplDataPtr, confirm),	XtRString,	NULL},
    {XtNreset,		XtCReset,	XtRInt,		sizeof (int),
     XtOffset (ApplDataPtr, reset),	XtRString,	NULL},
    {XtNbell,		XtCBell,	XtRBoolean,	sizeof (Boolean),
     XtOffset (ApplDataPtr, bell),	XtRString,	NULL},
    {XtNlist,		XtCList,	XtRBoolean,	sizeof (Boolean),
     XtOffset (ApplDataPtr, list),	XtRString,	"False"},
    {XtNversion,	XtCVersion,	XtRBoolean,	sizeof (Boolean),
     XtOffset (ApplDataPtr, version),	XtRString,	"False"},
    {XtNhelp,		XtCHelp,	XtRBoolean,	sizeof (Boolean),
     XtOffset (ApplDataPtr, help),	XtRString,	"False"},
    {XtNname,		XtCName,	XtRString,	sizeof (String),
     XtOffset (ApplDataPtr, proggie),	XtRString,	NULL},
    {XtNgeometry,	XtCGeometry,	XtRString,	sizeof (String),
     XtOffset (ApplDataPtr, geometry),	XtRString,	NULL},
};
	    


static XrmOptionDescRec options[] = {
    {"-time",		".time",	XrmoptionSepArg,	""},
    {"-t",		".time",	XrmoptionSepArg,	""},
    {"-warn",		".warnings",	XrmoptionSepArg,	NULL},
    {"-w",		".warnings",	XrmoptionSepArg,	NULL},
    {"-nowarn",		".warnings",	XrmoptionNoArg,		""},
    {"-nw",		".warnings",	XrmoptionNoArg,		""},
    {"-confirm",	".confirm",	XrmoptionNoArg,		"True"},
    {"-c",		".confirm",	XrmoptionNoArg,		"True"},
    {"-noconfirm",	".confirm",	XrmoptionNoArg,		"False"},
    {"-nc",		".confirm",	XrmoptionNoArg,		"False"},
    {"-snooze",		".snooze",	XrmoptionSepArg,	NULL},
    {"-s",		".snooze",	XrmoptionSepArg,	NULL},
    {"-reset",		".reset",	XrmoptionSepArg,	NULL},
    {"-r",		".reset",	XrmoptionSepArg,	NULL},
    {"-bell",		".bell",	XrmoptionNoArg,		"True"},
    {"-beep",		".bell",	XrmoptionNoArg,		"True"},
    {"-b",		".bell",	XrmoptionNoArg,		"True"},
    {"-nobell",		".bell",	XrmoptionNoArg,		"False"},
    {"-nobeep",		".bell",	XrmoptionNoArg,		"False"},
    {"-nb",		".bell",	XrmoptionNoArg,		"False"},
    {"-list",		".list",	XrmoptionNoArg,		"True"},
    {"-l",		".list",	XrmoptionNoArg,		"True"},
    {"-version",	".version",	XrmoptionNoArg,		"True"},
    {"-v",		".version",	XrmoptionNoArg,		"True"},
    {"-help",		".help",	XrmoptionNoArg,		"True"},
    {"-h",		".help",	XrmoptionNoArg,		"True"},
    {"-geometry",	".geometry",	XrmoptionSepArg,	NULL},
    {"-g",		".geometry",	XrmoptionSepArg,	NULL},
};



static XtActionsRec actions[] = {
    {"Quit",		Quit},
    {"EnteredTime",	EnteredTime},
    {"EnteredWarnings",	EnteredWarnings},
    {"Confirmed",	Confirmed},
    {"ShowClickToZero",	ShowClickToZero},
    {"ShowSnoozeValue",	ShowSnoozeValue}
};



static char alarmtranslations [] =
    "<Message> WM_PROTOCOLS:	Quit()";



AlarmData       xalarm;



void main (argc, argv)
  int 	   argc;
  String   argv[];
{
    String   *args;
    Boolean   appointments;

    args = PreParseArgList (&appointments, argv, argc);

    if (appointments)
	DoAppointments (args, argc);
    else
	DoAlarm (args, argc);
}



void DoAlarm (argv, argc)
  int 	   argc;
  String   argv[];
{
    ApplData   data;

    xalarm.toplevel = XtAppInitialize (&xalarm.appcon, "XAlarm",
				       options, XtNumber (options),
				       &argc, argv, NULL, NULL, 0);
    
    XtGetApplicationResources (xalarm.toplevel, &data,
			       resources, XtNumber (resources), NULL, 0);

    XtAppAddActions (xalarm.appcon, actions, XtNumber (actions));
    XtOverrideTranslations (xalarm.toplevel,
			    XtParseTranslationTable (alarmtranslations));

    /* 
     * First initialise; exit or fork.
     * We have to make the alarm widget first, as SetAlarm() may popup 
     * a dialog box which may need to know the alarm widget's label.
     */

    Initialise (argv[0], &data);
    CreateAlarmWidget (Concat (argv, argc));
    SetAlarm (&data);

    XtAppMainLoop (xalarm.appcon);
}       
	       
	       

/* 
 * If the time is not given, pop up and ask for it, otherwise add the 
 * time outs.  Get the warnings first, tho.
 */

static void SetAlarm (data)
  ApplDataPtr 	data;
{
    char   snoozetime[TEXT];

    snoozetime[0] = '+';
    (void) strcpy (snoozetime+1, data->snooze);
    xalarm.snooze = TimeToMilliSeconds (snoozetime) / (60*1000);

    xalarm.bell = data->bell;
    xalarm.confirm = data->confirm;
    xalarm.proggie = data->proggie;
    xalarm.geometry = data->geometry;

    /* 
     * Get the time from the time string, and the warnings from the 
     * warnings string...
     */
    xalarm.timeout = TimeToMilliSeconds (xalarm.timestr = data->time);
    SetWarningTimes (xalarm.warningsstr = data->warnings);

    /* 
     * If ``invalid'' (may not have been given anyway), popup.
     * If ok, but we want confirmation, popup to check.  (this is cheating!)
     * There is no waiting for confirmation if the timeout is 0 - the user
     * will soon realise that there was something wrong...
     */
    if ((ISINVALID (xalarm.timeout)) or
	(ISINVALID (xalarm.numwarnings)) or
	(xalarm.confirm and (xalarm.timeout != 0)))
	PopupAndAsk ();
    else
	AddTimeOuts ();

    (void) signal (SIGUSR1, ResetAlarm);
}
