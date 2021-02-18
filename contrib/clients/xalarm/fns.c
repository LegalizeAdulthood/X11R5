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
 * Just a few bits and bobs.
 */


#include "xalarm.h"
#include "fns.h"



/* 
 * This is cos of a bug of some sort spotted on a Sparc 2 (SunOS 4.1.1), X11r4,
 * whereby X crashes if given a zero timeout.  Just woof it up a bit.
 */

#if defined(SparcArchitecture)
#define		TIMEOUT(t)	((t) < 100 ? 100 : (t))
#else
#define		TIMEOUT(t)	(t)
#endif

#define		APPOINTMENTS	"-appointments"
#define		COMMAND		\
	"echo xalarms: `%s -cx | %s xalarm | %s 7 | %s -v %d`"



String		Concat(), *PreParseArgList();
void		DoAppointments(), Initialise(), AddTimeOuts(), Quit();
SIGRET		ResetAlarm();
extern void	DoAlarm(), Warning(), WakeUp(), PopupAndAsk(), SetAlarmIfToday();
extern int	fork();
extern long	TimeToMilliSeconds();
extern Boolean	AppointmentIsToday();
extern String	getenv();
extern time_t	time();



extern AlarmData   xalarm;



/* 
 * Just give a bit of help etc.
 * Fork off a child otherwise.
 */

void Initialise (proggie, data)
  String 	proggie;
  ApplDataPtr 	data;
{
    char   command[TEXT];

    if (data->proggie == NULL)
	data->proggie = proggie;

    if (data->version)
	(void) printf ("%s: using xalarm version %d.%02d\n",
		       data->proggie, VERSION, PATCHLEVEL);

    if (data->help) {
	(void) printf ("Usage: %s [options] [text]\n\n", data->proggie);
	(void) printf ("Default values for these resources/options are used by %s,\n\t",
		       data->proggie);
	(void) printf ("but may be over-ruled.\n");
	(void) printf ("Usual X options, plus:\n\t");
	(void) printf ("-appointments\t\tSet alarms for today from xalarm file.\n\t");
	(void) printf ("-time time\t\tTime at which to trigger %s.\n\t", data->proggie+1);
	(void) printf ("-warn time[,time...]\tTimes before %s for warnings.\n\t",
		       data->proggie+1);
	(void) printf ("-reset pid\t\tReset alarm for %s process number pid.\n\t",
		       data->proggie);
	(void) printf ("-snooze time\t\tSet initial alarm snooze time value.\n\t");
	(void) printf ("-nowarn\t\t\tNo warnings.\n\t");
	(void) printf ("-[no]confirm\t\t[Don't] ask for confirmation.\n\t");
	(void) printf ("-[no]bell\t\t[Don't] beep on warnings and %s.\n", data->proggie+1);
    }

    if (data->list) {
	(void) sprintf (command, COMMAND, PS, FGREP, COLRM, FGREP, getpid());
	(void) system (command);
    }

    if (data->reset > 0)
	if (kill (data->reset, SIGUSR1) == -1)
	    perror (data->proggie);

    if ((data->list) or (data->version) or (data->help) or (data->reset > 0))
	exit (0);

    /* 
     * Fork & exit the parent.  If the fork fails, carry on anyway.
     * Is this OK?  Should we sleep a bit first?
     */
    switch (fork ()) {
     case -1:
	perror (data->proggie);
     case 0:
	break;
     default:
	exit (0);
    }
}



/* 
 * Parse each line of the alarm file, if it's around.  If the 
 * appointment is today, fork and do the alarm.
 */

void DoAppointments (argv, argc)
  String   argv[];
  int 	   argc;
{
    FILE    *file;
    String   alarm, timestr;
    char     filename[TEXT], line[TEXT];
    int      newargc, endofdate;

    if ((alarm = getenv ("XALARM")) == NULL)
	alarm = XtNewString (".xalarm");
    if (alarm[0] == '/')
	(void) sprintf (filename, "%s", alarm);
    else
	(void) sprintf (filename, "%s/%s", getenv ("HOME"), alarm);

    if ((file = fopen (filename, "r")) == NULL) {
	perror (filename);
	exit (1);
    }

    while (fgets (line, TEXT, file) != NULL) {
	endofdate = 0;
	if (AppointmentIsToday (line, &timestr, &endofdate)) {	  
	    while (isspace (line[endofdate]))
		endofdate++;

	    argv[argc-1] = "-time";
	    argv[argc] = timestr;

	    if (*(line+endofdate) == NULL)
		newargc = argc + 1;
	    else {
		argv[argc+1] = line+endofdate;
		newargc = argc + 2;
	    }
	    switch (fork ()) {
	     case -1:
		perror ("xalarm");
		exit (-1);
	     case 0:
		DoAlarm (argv, newargc);
	    }
	}
    }
    exit (0);
}



/* 
 * Returns a string concated from the given array of strings.
 * Separates the strings with a newline.
 *
 * Hacked from various books on X & Xt.
 */

String Concat (strings, n)
  String  *strings;
  int 	   n;
{
    String 	   buffer;
    unsigned int   i, len = 0;

    if (n <= 1)
	return (NULL);

    for (i=1; i<n; i++)
	len += strlen (strings[i]);
    len += (n-1);

    buffer = XtMalloc (len+1);
    buffer[0] = '\0';
    for (i=1; i<n; i++) {
	if (i > 1)
	    (void) strcat (buffer, "\n");
	(void) strcat (buffer, strings[i]);
    }

    return (buffer);
}



/* 
 * Check to see if we're to do the appointments thing, and copy out 
 * the arg list, then clear it all.
 */

String *PreParseArgList (app, argv, argc)
  Boolean  *app;
  String   *argv;
  int 	    argc;
{
    String  *args = (String *) XtCalloc (argc+2, sizeof (String *));
    String   ch, end;
    int      i, first, offset;

    *app = (argc > 1) ? (strcmp (argv[1], APPOINTMENTS) == 0) : False;

    first = (*app) ? 2 : 1;
    offset = (*app) ? 1 : 0;
    args[0] = XtNewString (argv[0]);
    for (i=first; i<argc; i++) {
	args[i-offset] = XtNewString (argv[i]);
    }

    for (i=1; i<argc; i++) {
	end = argv[i] + strlen (argv[i]);
	for (ch=argv[i]; ch<end; ch++)
	    *ch = '\0';
    }

    return (args);
}



/* 
 * We add the time outs for the alarm & warnings.
 * The warnings are added only if there is time enuf to go.
 * ie. if "-in 10 -warn 15" you will get nowt.
 * "-in 10 -warn 9" will, however, give you a warning in 1 min.
 */

void AddTimeOuts ()
{
    int   i;
    
    xalarm.settime = time ((time_t *) NULL);

    for (i=0; i<xalarm.numwarnings; i++)
	if (xalarm.timeout > xalarm.warnings[i])
	    xalarm.timeouts[i] =
		XtAppAddTimeOut (xalarm.appcon,
				 TIMEOUT (xalarm.timeout - xalarm.warnings[i]),
				 Warning, (XtPointer) (xalarm.warnings[i]/(1000*60)));
    xalarm.timeouts[xalarm.numwarnings] =
	XtAppAddTimeOut (xalarm.appcon, TIMEOUT (xalarm.timeout),
			 WakeUp, (XtPointer) NULL);
}
  
   

/* 
 * Let's popup to reset our alarm.
 */

SIGRET ResetAlarm (sig, code, scp, addr)
  int 		      sig, code;
  struct sigcontext  *scp;
  char 		     *addr;
{
    time_t   now;
    int      i;

    now = time ((time_t *) NULL);
    for (i=0; i<xalarm.numwarnings; i++)
	if ((xalarm.timeout - (now - xalarm.settime)) > xalarm.warnings[i])
	    XtRemoveTimeOut (xalarm.timeouts[i]);
    XtRemoveTimeOut (xalarm.timeouts[xalarm.numwarnings]);

    xalarm.timeout = TimeToMilliSeconds (xalarm.timestr);

    PopupAndAsk ();
}



/* 
 * This function generates a random number and stuffs it down the pipe.
 */

void Quit (widget, clientdata, calldata)
  Widget      widget;
  XtPointer   clientdata, calldata;
{
    XtDestroyApplicationContext (xalarm.appcon);
    exit (0);
}
