/*

Copyright 1991 by the University of Edinburgh, Department of Computer Science

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of the University of Edinburgh not be used
in advertising or publicity pertaining to distribution of the software
without specific, written prior permission.  The University of Edinburgh
makes no representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.

*/

/*
	Author:	George Ross
		Department of Computer Science
		University of Edinburgh
		gdmr@dcs.ed.ac.uk
*/

/* Mail watcher.  Main window lists message senders, status and subjects.
 * Active icon indicates arrival of (new) mail.
 */

#include "box.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef sparc
#include <fcntl.h>
#endif

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Label.h>
#include <X11/Xmu/Converters.h>

#define POLL_DEFAULT 67
#define BUFFER_DEFAULT 12288

#include <stdio.h>

typedef struct _Resources {
	int	pollInterval;
	char	*mailbox;
	Pixel	iconForeground;
	Pixel	iconBackground;
	Pixel	iconMailForeground;
	Pixel	iconMailBackground;
	Pixmap	mailBitmap;
	Pixmap	nomailBitmap;
	Boolean	silent;
	int	bellVolume;
	char	*soundFile;
	int	bufferSize;
	Boolean	newMail;
	Boolean	fullNames;
	Boolean	debug;
	Boolean setTitle;
} Resources;

static Resources theResources;

static XtResource resourceSpec[] = {
	{ "pollInterval", "PollInterval", XtRInt, sizeof(int),
	  XtOffsetOf(Resources, pollInterval),
	  XtRImmediate, (XtPointer) POLL_DEFAULT },
	{ "mailbox", "Mailbox", XtRString, sizeof(char *),
	  XtOffsetOf(Resources, mailbox), XtRString, ".mail" },
	{ "iconForeground", XtCForeground, XtRPixel, sizeof(Pixel),
	  XtOffsetOf(Resources, iconForeground),
	  XtRString, XtDefaultForeground },
	{ "iconBackground", XtCBackground, XtRPixel, sizeof(Pixel),
	  XtOffsetOf(Resources, iconBackground),
	  XtRString, XtDefaultBackground },
	{ "iconMailForeground", "IconMailForeground", XtRPixel, sizeof(Pixel),
	  XtOffsetOf(Resources, iconMailForeground),
	  XtRPixel, (XtPointer) &theResources.iconBackground },
	{ "iconMailBackground", "IconMailBackground", XtRPixel, sizeof(Pixel),
	  XtOffsetOf(Resources, iconMailBackground),
	  XtRPixel, (XtPointer) &theResources.iconForeground },
	{ "mailBitmap", "MailBitmap", XtRBitmap, sizeof(Pixmap),
	  XtOffsetOf(Resources, mailBitmap), XtRString, "letters" },
	{ "nomailBitmap", "NomailBitmap", XtRBitmap, sizeof(Pixmap),
	  XtOffsetOf(Resources, nomailBitmap), XtRString, "noletters" },
	{ "silent", "Silent", XtRBoolean, sizeof(Boolean),
	  XtOffsetOf(Resources, silent), XtRImmediate, (XtPointer) False },
	{ "bellVolume", "BellVolume", XtRInt, sizeof(int),
	  XtOffsetOf(Resources, bellVolume), XtRImmediate, (XtPointer) 0 },
	{ "soundFile", "SoundFile", XtRString, sizeof(char *),
	  XtOffsetOf(Resources, soundFile), XtRImmediate, (XtPointer) NULL },
	{ "bufferSize", "BufferSize", XtRInt, sizeof(int),
	  XtOffsetOf(Resources, bufferSize),
	  XtRImmediate, (XtPointer) BUFFER_DEFAULT },
	{ "newMail", "NewMail", XtRBoolean, sizeof(Boolean),
	  XtOffsetOf(Resources, newMail), XtRImmediate, (XtPointer) True },
	{ "fullNames", "FullNames", XtRBoolean, sizeof(Boolean),
	  XtOffsetOf(Resources, fullNames), XtRImmediate, (XtPointer) False },
	{ "debug", "Debug", XtRBoolean, sizeof(Boolean),
	  XtOffsetOf(Resources, debug), XtRImmediate, (XtPointer) False },
	{ "setTitle", "SetTitle", XtRBoolean, sizeof(Boolean),
	  XtOffsetOf(Resources, setTitle), XtRImmediate, (XtPointer) True },
};

static XrmOptionDescRec options[] = {
	{ "-poll", ".pollInterval", XrmoptionSepArg, NULL },
	{ "-mailBox", ".mailbox", XrmoptionSepArg, NULL },
	{ "-mailbox", ".mailbox", XrmoptionSepArg, NULL },
	{ "-iconGeometry", ".iconShell.geometry", XrmoptionSepArg, NULL },
	{ "-icongeometry", ".iconShell.geometry", XrmoptionSepArg, NULL },
	{ "-newMail", ".newMail", XrmoptionNoArg, "True" },
	{ "-newmail", ".newMail", XrmoptionNoArg, "True" },
	{ "-allMail", ".newMail", XrmoptionNoArg, "False" },
	{ "-allmail", ".newMail", XrmoptionNoArg, "False" },
	{ "-silent", ".silent", XrmoptionNoArg, "True" },
	{ "-soundfile", ".soundFile", XrmoptionSepArg, NULL },
	{ "-soundFile", ".soundFile", XrmoptionSepArg, NULL },
	{ "-debug", ".debug", XrmoptionNoArg, "True" },
};

static XtConvertArgRec screenConvertArg[] = {
    { XtBaseOffset, (XtPointer) XtOffset(Widget, core.screen), sizeof(Screen *)}
};

void timedRescan();
void eventHandler();

static int mapped;

static char *buffer;
static int flags; 

static XtAppContext appContext;
static Widget shell;
static Widget viewport;
static Widget messages;
static Widget iconShell;
static Widget iconLabel;

main(argc, argv)
int argc;
char **argv;
{	Arg args[10];
	int n;
	Position iconX, iconY;

	shell = XtAppInitialize(&appContext, "MailWatcher", 
			options, XtNumber(options),
			&argc, argv, NULL, NULL, 0);

	XtAddConverter(XtRString, XtRBitmap, XmuCvtStringToBitmap,
		screenConvertArg, XtNumber(screenConvertArg));

	XtGetApplicationResources(shell, &theResources,
			resourceSpec, XtNumber(resourceSpec), NULL, 0);

	if (theResources.debug) {
		(void) printf("mailbox %s, pollInterval %d\n",
			theResources.mailbox, theResources.pollInterval);
		(void) printf("newMail %d, fullNames %d\n",
			theResources.newMail, theResources.fullNames);
		(void) printf("bellVolume %d\n", theResources.bellVolume);
		(void) printf("soundFile <%s>\n", theResources.soundFile);
	}

	if ((theResources.bellVolume < -100) ||
			(theResources.bellVolume > 100)) {
		(void) fprintf(stderr, "%s: bogus value %d for bellVolume\n",
			*argv, theResources.bellVolume);
		theResources.bellVolume = 0;
	}

	if (theResources.newMail)	flags  = BOX_NEWONLY;
	if (theResources.fullNames)	flags |= BOX_FULLNAMES;
	if (theResources.debug) 	flags |= BOX_DEBUG;

	buffer = (char *) malloc(theResources.bufferSize);

	n = 0;
	XtSetArg(args[n], XtNallowVert, True);	n++;
	viewport = XtCreateManagedWidget("viewport", viewportWidgetClass,
			shell, args, n);

	n = 0;
	XtSetArg(args[n], XtNjustify, XtJustifyLeft);	n++;
	XtSetArg(args[n], XtNlabel, "");		n++;
	messages = XtCreateManagedWidget("messages", labelWidgetClass,
			viewport, args, n);

	iconShell = XtCreatePopupShell("iconShell", transientShellWidgetClass,
			shell, NULL, 0);
	
	n = 0;
	XtSetArg(args[n], XtNbitmap, theResources.nomailBitmap);	n++;
	XtSetArg(args[n], XtNforeground, theResources.iconForeground);	n++;
	XtSetArg(args[n], XtNbackground, theResources.iconBackground);	n++;
	iconLabel = XtCreateManagedWidget("icon", labelWidgetClass,
			iconShell, args, n);

	XtRealizeWidget(iconShell);

	n = 0;
	XtSetArg(args[n], XtNx, &iconX);	n++;
	XtSetArg(args[n], XtNy, &iconY);	n++;
	XtGetValues(iconShell, args, n);
	n = 0;
	XtSetArg(args[n], XtNiconWindow, XtWindow(iconShell));	n++;
	if (iconX || iconY) {
		XtSetArg(args[n], XtNiconX, iconX);		n++;
		XtSetArg(args[n], XtNiconY, iconY);		n++;
	}
	XtSetValues(shell, args, n);

	XtRealizeWidget(shell);
	XtAddEventHandler(shell, StructureNotifyMask,
		False, eventHandler, NULL);

	timedRescan(NULL, NULL);

	XtAppMainLoop(appContext);
}

static void setTitle(time, have)
int time, have;
{	char titleBuffer[80];
	Arg args[4];
	int n;
	static int lastTime;

	if (time == lastTime) return;

	if (time) {
		(void) sprintf(titleBuffer,
			have ? "Mail at %s" : "No mail since %s",
			ctime(&time));
		titleBuffer[strlen(titleBuffer) - 1] = '\0';
		if (theResources.debug)
			(void) printf("Title -> <%s>\n",
					titleBuffer);
		n = 0;
		XtSetArg(args[n], XtNtitle, titleBuffer);	n++;
	}
	else {
		n = 0;
		XtSetArg(args[n], XtNtitle, "No mail");	 	n++;
	}
	XtSetValues(shell, args, n);
	lastTime = time;
}

static void beep()
{
#ifdef sparc
	int sound;
	int audio;
	int n;
	char buffer[512];

	if (theResources.soundFile) {
		if ((audio = open("/dev/audio", O_WRONLY)) > 0 &&
		    (sound = open(theResources.soundFile, O_RDONLY)) > 0) {
			while ((n = read(sound, buffer, 512)) > 0)
				(void) write(audio, buffer, n);
		}
		if (sound) (void) close(sound);
		if (audio) (void) close(audio);
	}
	else
#endif
		XBell(XtDisplay(shell), theResources.bellVolume);
}

static void rescanMailbox(timedScan)
int timedScan;
{	Arg args[10];
	int n;
	static int haveMail;
	static int hadMail;
	static int pendingClear;
	static int forceRescan;
	struct stat sb;
	static time_t lastTime;
	Widget scrollbar;
	Dimension scrollWidth, scrollHeight;
	XButtonEvent be;
	static char *cont = "Continuous";

	if (theResources.debug)
		(void) printf("rescanMailbox %d\n", timedScan);

	if (stat(theResources.mailbox, &sb) < 0) {
		/* No mail file, so no mail */
		if (theResources.debug)
			(void) printf("No mail file, %d %d\n",
					timedScan, hadMail);
		if (timedScan && !hadMail) return;  /* No change */
		*buffer = '\0';
		haveMail = 0;
		lastTime = 0;
		pendingClear = 1;
	}
	else if (sb.st_mtime == lastTime && !(forceRescan && mapped)) {
		/* No change */
		if (theResources.debug)
			(void) printf("No change, %d %s",
					timedScan, ctime(&lastTime));
		if (timedScan) return;
	}
	else if (sb.st_size == 0) {
		/* Empty mail file */
		if (theResources.debug)
			(void) printf("Empty, %d %s",
					timedScan, ctime(&sb.st_mtime));
		*buffer = '\0';
		haveMail = 0;
		lastTime = sb.st_mtime;
		pendingClear = 1;
	}
	else if (mapped || theResources.newMail) {
		/* Something in the mail file.  We only need to go to the
		 * trouble of reading it if we're mapped or we're only
		 * wanting to report on new mail */
		haveMail = parseMailbox(theResources.mailbox,
			buffer, theResources.bufferSize, flags);
		forceRescan = 0;
		lastTime = sb.st_mtime;
		if (theResources.debug)
			(void) printf("Scanned, %d %d %s",
					timedScan, haveMail, ctime(&lastTime));
	}
	else { /* Changed, not empty, not mapped */
		if (theResources.debug)
			(void) printf("Changed but not scanned, %d %x %x %d %s",
					timedScan, lastTime, sb.st_mtime,
					mapped, ctime(&sb.st_mtime));
		haveMail = 1;
		lastTime = sb.st_mtime;
		forceRescan = 1;
	}

	if (timedScan && haveMail && !theResources.silent) beep();

	if ((haveMail || hadMail || pendingClear) && mapped) {
		pendingClear = 0;
		n = 0;
		XtSetArg(args[n], XtNlabel, buffer);	n++;
		XtSetValues(messages, args, n);

		/* This is revolting, but there doesn't seem to be any
		 * other way to get the thing to scroll right to the bottom */
		scrollbar = XtNameToWidget(viewport, "vertical");
		if (scrollbar) {
			n = 0;
			XtSetArg(args[n], XtNwidth, &scrollWidth);	n++;
			XtSetArg(args[n], XtNheight, &scrollHeight);	n++;
			XtGetValues(scrollbar, args, n);
			/* Fake up a button event -- enough to pacify the
			 * Scrollbar widget, at any rate... */
			be.type = ButtonPress;
			be.x = scrollWidth / 2;
			be.y = scrollHeight - 1;
			XtCallActionProc(scrollbar, "StartScroll", &be, &cont, 1);
			XtCallActionProc(scrollbar, "MoveThumb", &be, NULL, 0);
			XtCallActionProc(scrollbar, "NotifyThumb", &be, NULL, 0);
			XtCallActionProc(scrollbar, "EndScroll", &be, NULL, 0);
		}
		/* YUK! */

		if (theResources.setTitle)
			setTitle(lastTime, haveMail);
	}

	if (haveMail) {
		if (hadMail) return;	/* No change */
		n = 0;
		XtSetArg(args[n], XtNforeground,
			theResources.iconMailForeground);		n++;
		XtSetArg(args[n], XtNbackground,
			theResources.iconMailBackground);		n++;
		XtSetArg(args[n], XtNbitmap, theResources.mailBitmap);	n++;
	}
	else {
		if (!hadMail) return;	/* No change */
		n = 0;
		XtSetArg(args[n], XtNforeground, theResources.iconForeground);
		n++;
		XtSetArg(args[n], XtNbackground, theResources.iconBackground);
		n++;
		XtSetArg(args[n], XtNbitmap, theResources.nomailBitmap);
		n++;
	}
	XtSetValues(iconLabel, args, n);
	hadMail = haveMail;
}

void timedRescan(d, t)
XtPointer d;
XtIntervalId *t;
{	rescanMailbox(1);
	(void) XtAppAddTimeOut(appContext, 1000 * theResources.pollInterval,
		timedRescan, NULL);
}

void eventHandler(w, cd, e, ctd)
Widget w;
XtPointer cd;
XEvent *e;
Boolean *ctd;
{	if (e->type == MapNotify) {
		if (theResources.debug) (void) printf("MapNotify\n");
		mapped = 1;
		rescanMailbox(0);
	}
	else if (e->type == UnmapNotify) {
		if (theResources.debug) (void) printf("UnmapNotify\n");
		mapped = 0;
		rescanMailbox(0);
	}
}
