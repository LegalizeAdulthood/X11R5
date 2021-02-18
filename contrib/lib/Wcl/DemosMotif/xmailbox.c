/***************************************************************************
 *
 * FILE NAME		: xmailbox.c
 *
 * AUTHOR		: Andrew Peebles
 *
 * DESCRIPTION		: Simple utility to monitor mail box
 *
 * VERSIONS		: %W%
 *
 ***************************************************************************/

/*
 * standard includes
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <Xm/Xm.h>

#include <X11/Shell.h>
#include <Wc/WcCreate.h>

static void ClearTextACT();
static void HideMailACT();
static void ReScanACT();

static XtActionsRec	MYactions[] = {
  {"ClearTextACT",	ClearTextACT},
  {"HideMailACT",	HideMailACT},
  {"ReScanACT",		ReScanACT},
};

/*
 * resources
 */
typedef struct _defs {
  char		*mailbox;
  Boolean	hide_nomail;
  Boolean	include_subject;
  Boolean	flash;
  int		flash_timeout;
  int		flash_number;
  Boolean	beep;
  char		*mri_file;
  int		timeout;
  char		*from_cmd;
  char		*subject_cmd;
  Boolean	nomailboxWarningMessage;
} Defs, *DefsPtr;
static Defs defaults;

static	int 	timeout;
static	Boolean	hide_nomail, include_subject, flash, beep;
static	char	*mailbox, *mri_file;
static	int	flash_timeout, flash_number;
static	char	*from_cmd, *subject_cmd;

static XtResource resources[] = {
  {"mailbox", "Mailbox",
     XmRString, sizeof (char *),
     XtOffset (DefsPtr, mailbox),
     XmRString,
     NULL},

  {"hideNoMail", "HideNoMail",
     XmRBoolean, sizeof (Boolean),
     XtOffset (DefsPtr, hide_nomail),
     XmRString,
     "False"},

  {"nomailboxWarningMessage", "NomailboxWarningMessage",
     XmRBoolean, sizeof (Boolean),
     XtOffset (DefsPtr, nomailboxWarningMessage),
     XmRString,
     "True"},

  {"flash", "Flash",
     XmRBoolean, sizeof (Boolean),
     XtOffset (DefsPtr, flash),
     XmRString,
     "False"},

  {"beep", "Beep",
     XmRBoolean, sizeof (Boolean),
     XtOffset (DefsPtr, beep),
     XmRString,
     "False"},

  {"includeSubject", "IncludeSubject",
     XmRBoolean, sizeof (Boolean),
     XtOffset (DefsPtr, include_subject),
     XmRString,
     "False"},

  {"timeout", "Timeout",
     XmRInt, sizeof (int),
     XtOffset (DefsPtr, timeout),
     XmRString,
     "60"},

  {"flashTimeout", "FlashTimeout",
     XmRInt, sizeof (int),
     XtOffset (DefsPtr, flash_timeout),
     XmRString,
     "100"},

  {"flashNumber", "FlashNumber",
     XmRInt, sizeof (int),
     XtOffset (DefsPtr, flash_number),
     XmRString,
     "10"},

  {"wcResources", "WcResources",
     XmRString, sizeof (char *),
     XtOffset (DefsPtr, mri_file),
     XmRString,
     "/user/cadroot/lib/Mri/XMailbox.res"},

  {"fromCmd", "FromCmd",
     XmRString, sizeof (char *),
     XtOffset (DefsPtr, from_cmd),
     XmRString,
     "grep \"From:\""},

  {"subjectCmd", "SubjectCmd",
     XmRString, sizeof (char *),
     XtOffset (DefsPtr, subject_cmd),
     XmRString,
     "grep \"Subject:\""},

};

/*
 * Command line equivelents
 */
static XrmOptionDescRec cmd_options[] =
{
  {"-mailbox",		".mailbox",	XrmoptionSepArg,	NULL},
  {"-resources",	".wcResources",	XrmoptionSepArg,	"/user/cadroot/lib/Mri/XMailbox.res"},
  {"-hide",		".hideNoMail",	XrmoptionNoArg,		"True"},
  {"-subject",		".includeSubject", XrmoptionNoArg,	"True"},
  {"-flash",		".flash",	XrmoptionNoArg,		"True"},
  {"-beep",		".beep",	XrmoptionNoArg,		"True"},
  {"-timeout",		".timeout",	XrmoptionSepArg,	"60"},
  {"-ftimeout",		".flashTimeout", XrmoptionSepArg,	"100"},
  {"-fnumber",		".flashNumber",	XrmoptionSepArg,	"10"},
  {"-nowarning",	".nomailboxWarningMessage", XrmoptionSepArg, "False"},
  {"-fromCmd",		".fromCmd",	XrmoptionSepArg,	NULL},
  {"-subjectCmd",	".subjectCmd",	XrmoptionSepArg,	NULL},
};


/*
 * global vars
 */
static	XtAppContext 	app;
static	Widget		text, shell;
static	Boolean		UP = False;
static  Boolean		DoAutoRaise = False;
static  Widget		topshell;
static	int		last_size;
static	int		flash_count = 0;
static	int		box_size = 0;

#define FLASH_MAX	6

char	*mystrdup();
static  void ClearText();
static	void FlashEvent();
static	Boolean ScanMBox();

main (argc, argv)
int	argc;
char	*argv;
{
  Widget	appshell;
  void		TimeOutEvent();
  char		*resfile;
  Display	*display;
  int		n;
  Arg		args[5];
  int		mfp;

  topshell = XtInitialize ("xmailbox", "XMailbox",
			   cmd_options, XtNumber(cmd_options),
			   &argc, argv);
#if 0
  appshell = XtAppCreateShell ("xmailbox", "XMailbox",
			       applicationShellWidgetClass,
                               XtDisplay(topshell), NULL, 0);
#endif
  n=0;
  XtSetArg (args[n], XmNtransient, True); n++;
  shell = XtCreatePopupShell ("xmailbox",
			      topLevelShellWidgetClass,	
                              topshell, args, n);

  app = XtWidgetToApplicationContext (topshell);

  XtAppAddActions (app, MYactions, XtNumber(MYactions));

  XtGetApplicationResources (topshell,
                             &defaults,
                             resources, XtNumber(resources),
                             NULL, 0);

  timeout = defaults.timeout;
  hide_nomail = defaults.hide_nomail;
  include_subject = defaults.include_subject;
  flash = defaults.flash;
  beep = defaults.beep;
  flash_timeout = defaults.flash_timeout;
  flash_number = defaults.flash_number;
  if (flash_number % 2) ++flash_number;
  if (defaults.mailbox == NULL) {
    fprintf (stderr,"MUST specify a mailbox resource!\n");
    exit (1);
  }
  mailbox = mystrdup (defaults.mailbox);

  if ((mfp = open (mailbox, O_RDONLY)) == -1) {
    if (defaults.nomailboxWarningMessage) {
      fprintf (stderr, "Can't open mailbox: %s\n",mailbox);
    }
  }

#ifdef DEBUG
  printf ("timeout = %d seconds\n", timeout);
  printf ("hide_nomail = %s\n",((hide_nomail) ? "True" : "False"));
  printf ("include_subject = %s\n", ((include_subject) ? "True" : "False"));
  printf ("mailbox = %s\n",mailbox);
#endif

  RegisterApplicationCallbacks (app);
  MriRegisterMotif(app);

  resfile = (char *) malloc (strlen(defaults.mri_file) + 1);
  strcpy (resfile, defaults.mri_file);
  WcLoadResourceFileCB (shell, resfile, NULL);

  if( WcWidgetCreation(shell) != 0 )
    return(1);

  XtRealizeWidget (shell);

#ifdef DEBUG
  ListWidgetNames (appshell);
#endif

  XtAppAddTimeOut (app,
                   1000 * 1,	/* get to it fast for the first time */
                   TimeOutEvent,
                   NULL);

  XtPopup (shell, XtGrabNone);
  UP = True;

  XtMainLoop();
}

static Boolean first_up = True;

void
TimeOutEvent (type, id)
int             type;
XtIntervalId    *id;
{
  int FLASH = False;

#ifdef DEBUG
  printf ("timeout ...\n");
#endif

  FLASH = ScanMBox();

  XtAppAddTimeOut (app,
                   1000 * timeout,
                   TimeOutEvent,
                   NULL);

  if (FLASH) {
    FLASH = False;
    flash_count = 0;
    XtAppAddTimeOut (app,
		     flash_timeout,
		     FlashEvent,
		     NULL);
  }

  if (first_up) first_up = False;

}

static Boolean
ScanMBox()
{
  struct stat stats;
  int	FLASH = False;

  /*
   * Try to stat mailbox, see if any mail
   */
  stats.st_size = 0;
  if ((stat (mailbox, &stats) == -1) &&
      (defaults.nomailboxWarningMessage)) {
    Error ("Can't stat (open) mailbox");
    return (True);
  }
  else if ((stats.st_size != 0) && (stats.st_size > last_size)) {

    last_size = stats.st_size;

    if (DealWithText(True)) {
      if (UP) {
	if (DoAutoRaise) {
	  XRaiseWindow (XtDisplay (shell), XtWindow (shell));
	}
      } else {
	XtPopup (shell, XtGrabNone);
	UP = True;
      }
    
      if (beep) {
	XBell (XtDisplay(shell), 99);
      }

      if (flash) {
	FLASH = True;
      }
    }

  }
  else if (stats.st_size == 0) {
    if (hide_nomail && UP && !first_up) {
      XtPopdown (shell);
      UP = False;
    } else {
      if (DoAutoRaise)
	XLowerWindow (XtDisplay (shell), XtWindow (shell));
    }
    box_size = 0;
    last_size = 0;
    ClearText (NULL, NULL, NULL);
  }
  else if (stats.st_size < last_size) {
    last_size = stats.st_size;
    box_size = 0;
    DealWithText (False);	/* recalcualtes box_size */
  }

  return (FLASH);
}

static void
FlashEvent (type, id)
int             type;
XtIntervalId    *id;
{
  Arg	args[5];
  int	n;
  Pixel	fg, bg;
  int	pos;

  if (flash_count == flash_number)
    return;

  pos = XmTextGetLastPosition (text);

  n=0;
  XtSetArg (args[n], XmNbackground, &bg); n++;
  XtSetArg (args[n], XmNforeground, &fg); n++;
  XtGetValues (text, args, n);

  n=0;
  XtSetArg (args[n], XmNbackground, fg); n++;
  XtSetArg (args[n], XmNforeground, bg); n++;
  XtSetValues (text, args, n);

  XmTextSetInsertionPosition (text, pos);

  ++flash_count;
  XtAppAddTimeOut (app,
		   flash_timeout,
		   FlashEvent,
		   NULL);
}

/*
 * User accessabible Callback routines
 */

static void
RegisterText (widget, widgetName, unused)
Widget  widget;
char*   widgetName;
caddr_t unused;
{
  Widget        cp;
  char          clean[128];

  WcCleanName (widgetName, clean);
  text = WcFullNameToWidget (widget, clean);
}

static void
ClearText (widget, widgetName, unused)
Widget  widget;
char*   widgetName;
caddr_t unused;
{
  XmTextSetString (text, NULL);
}

static void
HideMail (widget, widgetName, unused)
Widget  widget;
char*   widgetName;
caddr_t unused;
{
  XtPopdown (shell);
  UP = False;
}

static void
AutoRaise (widget, widgetName, unused)
{
  DoAutoRaise = !DoAutoRaise;
}

static void
ReScan (widget, widgetName, unused)
Widget  widget;
char*   widgetName;
caddr_t unused;
{
  box_size = 0;
  last_size = 0;
  ClearText (NULL, NULL, NULL);
  ScanMBox();
}

RegisterApplicationCallbacks (app)
XtAppContext app;
{
#define RCALL( name, func ) WcRegisterCallback ( app, name, func, NULL );

  RCALL ( "RegisterText",  	RegisterText       );
  RCALL ( "ClearTextCB",	ClearText	   );
  RCALL ( "HideMailCB",		HideMail	   );
  RCALL ( "ReScanCB",		ReScan		   );
  /*
   * and for backward compatability
   */
  RCALL ( "ClearText",		ClearText	   );
  RCALL ( "HideMail",		HideMail	   );
  RCALL ( "ReScan",		ReScan		   );
  RCALL ( "AutoRaise",		AutoRaise	   );
}

DealWithText(displayit)
Boolean displayit;
{
  int pos, THIS_POS;
  char	cmd[128];
  FILE	*popen(), *fp, *sp;
  char	from[180], subject[180];
  int	index;
  char	*tok;
  int	ret = False;

  sprintf (cmd,"%s %s",defaults.from_cmd,mailbox);

  if ((fp = popen (cmd, "r")) == NULL) {
    Error ("Problems reading From from mailbox");
    return (0);
  }

  if (include_subject) {

    sprintf (cmd,"%s %s",defaults.subject_cmd,mailbox);

    if ((sp = popen (cmd, "r")) == NULL) {
      Error ("Problems reading Subject from mailbox");
      pclose (fp);
      return (0);
    }

  }

  index = 0;
  while (fgets (from, 180, fp) != NULL) {
    if (include_subject) {
      fgets (subject, 180, sp);
    }

    if (index++ < box_size) {
      continue;
    }

#ifdef DEBUG
    printf (">> %s ...\n", from);
#endif

    ++box_size;

    if (!displayit)
      continue;

    ret = True;

    if (!include_subject) {
      if (from[strlen(from)-1] != '\n') {
	from[strlen(from)-1] = '\n';
      }
      tok = from;
      THIS_POS = 
      pos = XmTextGetLastPosition (text);
      XmTextReplace (text, pos, pos, tok);
    }
    else {
      if (from[strlen(from)-1] == '\n') {
	from[strlen(from)-1] = '\0';
      }
      if (subject[strlen(subject)-1] != '\n') {
	subject[strlen(subject)-1] = '\n';
      }
      tok = from;
      THIS_POS = 
      pos = XmTextGetLastPosition (text);
      XmTextReplace (text, pos, pos, tok);

      tok = subject;
      pos = XmTextGetLastPosition (text);
      XmTextReplace (text, pos, pos, tok);
    }
    bzero (from, 80);
    bzero (subject, 80);
  }

  XmTextSetInsertionPosition (text, THIS_POS);

  pclose(fp);
  if (include_subject)
    pclose(sp);

  return (ret);
}

Error (msg) 
char *msg;
{
  fprintf (stderr, "xmailbox Warning: %s\n", msg);
}

char *
mystrdup (str)
char	*str;
{
  char	*ret = (char *) malloc (strlen(str) + 1);
  strcpy (ret, str);
  return (ret);
}

/*
 * action functions - they mearly call thier callback equivelents
 */
static void
ClearTextACT()
{
  ClearText();
}

static void
HideMailACT()
{
  HideMail();
}

static void
ReScanACT()
{
  ReScan();
}

