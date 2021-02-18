/*
 * Author:  Dan heller
 */

#ifndef _XtMailwatch_h
#define _XtMailwatch_h

/*
 * Mailwatch widget; looks a lot like the Mailbox widget, don't it...
 */

#ifndef XtNupdate
#define XtNupdate	"update"	/* Int: how often to check mail */
#endif /* XtNupdate */

#ifndef XtNfile
#define XtNfile		"file"		/* String:  file name to check */
#endif /* Xtfile */

#define XtNbell		"bell"		/* boolean to ring bell on new mail */
#define XtNmailFlag	"mailFlag"	/* path to icon file for mail */
#define XtNnoMailFlag	"noMailFlag"	/* path to icon file for no mail */
#define XtNmail		"mail"		/* For XtGetValues to get mail info */

#define XtCPixmapMask	"PixmapMask"
#define XtCMailFlag	"MailFlag"	/* Class name of icon file for mail */
#define XtCNoMailFlag	"NoMailFlag"	/* Class name of icon file for nomail */

/* structures */
typedef struct _MailwatchRec *MailwatchWidget;  /* see MailwatchP.h */
typedef struct _MailwatchClassRec *MailwatchWidgetClass;  /* see MailwatchP.h */

extern WidgetClass mailwatchWidgetClass;

#endif /* _XtMailwatch_h */
/* DON'T ADD STUFF AFTER THIS #endif */
