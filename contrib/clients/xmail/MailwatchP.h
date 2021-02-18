/*
 * Author:  Dan Heller (see Mailwatch.c for detailed info)
 */
#ifndef _XtMailwatchP_h
#define _XtMailwatchP_h

#include "Mailwatch.h"
#include <X11/CoreP.h>

#ifndef MAILBOX_DIRECTORY
#define MAILBOX_DIRECTORY "/usr/spool/mail"
#endif /* MAILBOX_DIRECTORY */

#define MAILBOX_VOLUME 33		/* percentage */

typedef struct {			/* new fields for mailwatch widget */
    Pixel	foreground_pixel;	/* color index of normal state fg */
    GC		normal_GC;		/* normal GC to use */
    GC		invert_GC;		/* invert GC for FillRectangle call */
    int		update;			/* seconds between updates */
    String	filename;		/* filename to watch */
    long	last_size;		/* size in bytes of mailboxname */
    time_t	last_access;		/* last time user accessed mailbox */
    Boolean	reverseVideo;		/* do reverse video? */
    Boolean	flag_up;		/* is the flag up? */
    Boolean	bell;			/* ring the bell on new mail? */
    Boolean	useName;		/* add username to icon image? */
    Boolean	useHost;		/* add mailhost name to icon image? */
    String	username;		/* username of mailbox */
    String	mailhost;		/* host name of mail server */
    XFontStruct	*font;			/* font to use when displaying name */
    XtCallbackList	callback;	/* Call func(s) when mail arrives */
    XtIntervalId	interval_id;	/* time between checks */
    struct _mbimage {
	int	width, height;
	Pixmap	pixmap;			/* full depth pixmap */
#ifndef	XPM
	Pixmap  bitmap;
#else
	String  bitmap;
#endif
	} mail, nomail;
} MailwatchPart;

typedef struct _MailwatchRec {		/* full instance record */
    CorePart core;
    MailwatchPart mailbox;
} MailwatchRec;

typedef struct {			/* new fields for mailwatch class */
    int dummy;				/* stupid C compiler */
} MailwatchClassPart;

typedef struct _MailwatchClassRec {	/* full class record declaration */
    CoreClassPart core_class;
    MailwatchClassPart mailwatch_class;
} MailwatchClassRec;

extern MailwatchClassRec mailwatchClassRec;	 /* class pointer */

#endif _XtMailwatchP_h
