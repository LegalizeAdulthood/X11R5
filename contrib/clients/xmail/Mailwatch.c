/*
 * @(#)Mailwatch.c - MODIFIED for use as an active icon in the xmail program.
 *
 * Author:  Dan Heller <island!argv@sun.com>
 * This code was stolen from Mailbox.c --the widget which supports "xbiff"
 * written by Jim Fulton which was apparently stolen from Clock.c, the widget
 * which supports "xclock."  Note, you are witnessing the big bang theory of
 * software development (everything is a subclass of universeWidgetClass).
 *
 * Major changes:
 * XtRemoveTimeOut() is called before calling XtAddTimeOut().  The original
 * xbiff would eventually timeout all the time rather than every 30 seconds
 * because the old timer was never removed.
 *
 * User can specify any icon he chooses for either the up flag or the down
 * flag.  Icons don't need to be the same size (defaults to flagup/flagdown).
 *
 * When new mail comes in, a user supplied callback function is invoked.
 *
 * The mailbox flag goes up when there is new mail _and_ the user hasn't
 * read it yet.  As soon as the user updates the access time on the mailbox,
 * the flag goes down.  This removes the incredibly annoying habit xbiff
 * had where you read some mail but not delete it from the mailbox and the
 * flag would remain up.
 *
 * Destroy() will now destroy the flagup and flagdown pixmaps.
 *
** July 1991 - Michael C. Wagnitz - National Semiconductor Corporation
** The following modifications were made for use in xmail.
**
** Added support for colored icons via the XPixMap format library libXpm.a
**
** November 1990 - Michael C. Wagnitz - National Semiconductor Corporation
** The following modifications were made for use in xmail.
**
** Added options to display username or host name in icon window.
**
** November 1989 - Michael C. Wagnitz - National Semiconductor Corporation
**
** Removed button handler, custom cursor, and 'from()' reader functions.
** Added reset_mailbox() function, my own icons (56x56 bits), and also
** added timer initialization to Initialize() routine (we might not ever be
** Realized, if the user never iconifies my parent).  This also fixes a Sun4
** bug for trying to remove an initial interval_id with a garbage address.
** Also changed check_mailbox() to test access time vs. modified and zero size
** of file, rather than trying to track our last access or increase in size.
**
** Copyright 1990,1991 by National Semiconductor Corporation
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose is hereby granted without fee, provided that
** the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation, and that the name of National Semiconductor Corporation not
** be used in advertising or publicity pertaining to distribution of the
** software without specific, written prior permission.
**
** NATIONAL SEMICONDUCTOR CORPORATION MAKES NO REPRESENTATIONS ABOUT THE
** SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS"
** WITHOUT EXPRESS OR IMPLIED WARRANTY.  NATIONAL SEMICONDUCTOR CORPORATION
** DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO
** EVENT SHALL NATIONAL SEMICONDUCTOR CORPORATION BE LIABLE FOR ANY SPECIAL,
** INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
** LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
** OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
** PERFORMANCE OF THIS SOFTWARE.
**
** The following software modules were created and are Copyrighted by National
** Semiconductor Corporation:
**
** 1. reset_mailbox,
** 2. set_host,
** 3. set_user,
** 4. set_none,
** 5. GetHostName, and
** 6. XMyLocateXPixmapFile.
**
** Author:  Michael C. Wagnitz - National Semiconductor Corporation
*/


#include <X11/Xos.h>
#include <X11/IntrinsicP.h>		/* for toolkit stuff */
#include <X11/cursorfont.h>		/* for cursor constants */
#include <X11/StringDefs.h>		/* for useful atom names */
#include <X11/Xaw/XawInit.h>
#include "MailwatchP.h"			/* for implementation mailbox stuff */
#include <stdio.h>			/* for printing error messages */
#include <sys/stat.h>			/* for stat() */
#include <sys/param.h>			/* for MAXHOSTNAMELEN */
#include <pwd.h>			/* for getting username */
#include <errno.h>

#ifndef	XPM
#include <X11/Xmu/Converters.h>		/* for XmuCvtStringToBitmap */
#include "icon.mail"			/* for flag up (mail present) bits */
#include "icon.nomail"			/* for flag down (mail not here) */
#else
#include <X11/xpm.h>			/* for XPixMap colored icons */
#include <X11/Xmu/CvtCache.h>
#include "mail.xpm"			/* for flag up (mail present) bits */
#include "nomail.xpm"			/* for flag down (mail not here) */

#define	mail_width	64
#define	mail_height	64
#define	nomail_width	64
#define	nomail_height	64
#endif

#ifndef	MAXHOSTNAMELEN
#define	MAXHOSTNAMELEN	64
#endif

#define between(x, i, y)   ((i) < (x) ? (x) : (i) > (y) ? (y) : (i))

#define REMOVE_TIMEOUT(id) if (!id) ; else XtRemoveTimeOut(id)

static struct stat	stbuf;
static Boolean		SetValues();
static Pixmap		make_pixmap();
static GC		GetNormalGC(), GetInvertGC();
static void		GetMailFile(), GetUserName(), GetHostName(),
			CloseDown(), check_mailbox(), redraw_mailbox(),
			ClassInitialize(), Initialize(), Destroy(),
			Realize(), Redisplay(), clock_tic(), set_title();
extern void		reset_mailbox(), set_host(), set_user(), set_none();
extern Widget		toplevel;

extern	int		RootWidth;
extern	int		RootHeight;

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

static char defaultTranslations[] =
    "<Key>h:		set-host()	\n\
     <Key>u:		set-user()	\n\
     <Key>space:	set-none()";

static XtActionsRec actionsList[] = {
    { "set-host",	set_host	},
    { "set-user",	set_user	},
    { "set-none",	set_none	},
};

/* Initialization of defaults */
#define offset(field) XtOffset(MailwatchWidget,mailbox.field)
#define goffset(field) XtOffset(Widget,core.field)

static XtResource resources[] = {
    {XtNupdate, XtCInterval, XtRInt, sizeof(int),
	offset(update), XtRImmediate, (caddr_t) 30 },
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(foreground_pixel), XtRString, XtDefaultForeground },
    {XtNbackground, XtCBackground, XtRPixel, sizeof(Pixel),
	goffset(background_pixel), XtRString, XtDefaultBackground },
    {XtNreverseVideo, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(reverseVideo), XtRImmediate, (caddr_t) False },
    {XtNbell, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(bell), XtRImmediate, (caddr_t) True },
    {XtNfile, XtCFile, XtRString, sizeof(String),
	offset(filename), XtRString, NULL},
#ifndef	XPM
    {XtNmailFlag, XtCMailFlag, XtRBitmap, sizeof(Pixmap),
	offset(mail.bitmap), XtRString, NULL},
    {XtNnoMailFlag, XtCNoMailFlag, XtRBitmap, sizeof(Pixmap),
	offset(nomail.bitmap), XtRString, NULL},
#else
    {XtNmailFlag, XtCMailFlag, XtRString, sizeof(String),
	offset(mail.bitmap), XtRString, NULL},
    {XtNnoMailFlag, XtCNoMailFlag, XtRString, sizeof(String),
  	offset(nomail.bitmap), XtRString, NULL},
#endif
    {XtNcallback, XtCCallback, XtRCallback, sizeof(caddr_t),
	offset(callback), XtRCallback, NULL},
    {"useName", XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(useName), XtRString, "FALSE" },
    {"useHost", XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(useHost), XtRString, "FALSE" },
    { XtNfont, XtCFont, XtRFontStruct, sizeof (XFontStruct *),
	offset(font), XtRString, XtDefaultFont},
};

#undef offset
#undef goffset

MailwatchClassRec mailwatchClassRec = {
    {	/* core fields */
	/* superclass		*/ &widgetClassRec,
	/* class_name		*/ "Mailwatch",
	/* widget_size		*/ sizeof(MailwatchRec),
	/* class_initialize	*/ ClassInitialize,
	/* class_part_initialize*/ NULL,
	/* class_inited 	*/ FALSE,
	/* initialize		*/ Initialize,
	/* initialize_hook	*/ NULL,
	/* realize		*/ Realize,
	/* actions		*/ actionsList,
	/* num_actions		*/ XtNumber(actionsList),
	/* resources		*/ resources,
	/* resource_count	*/ XtNumber(resources),
	/* xrm_class		*/ NULL,
	/* compress_motion	*/ TRUE,
	/* compress_exposure	*/ TRUE,
	/* compress_enterleave	*/ TRUE,
	/* visible_interest	*/ FALSE,
	/* destroy		*/ Destroy,
	/* resize		*/ NULL,
	/* expose		*/ Redisplay,
	/* set_values		*/ SetValues,
	/* set_values_hook	*/ NULL,
	/* set_values_almost	*/ XtInheritSetValuesAlmost,
	/* get_values_hook	*/ NULL,
	/* accept_focus		*/ NULL,
	/* version		*/ XtVersion,
	/* callback_private	*/ NULL,
	/* tm_table		*/ defaultTranslations,
	/* query_geometry	*/ NULL,
    }
};

WidgetClass mailwatchWidgetClass = (WidgetClass) & mailwatchClassRec;

/*
 * private procedures
 */
static Pixmap
make_pixmap (dpy, w, bitmap, depth, widthp, heightp)
Display		*dpy;
MailwatchWidget	w;
Pixmap		bitmap;
int		depth;
int		*widthp, *heightp;
{
 Window root;
 int x, y;
 unsigned int width, height, bw, dep;
 unsigned long fore, back;

 
 if (bitmap == 0 ||
     ! XGetGeometry(dpy, bitmap, &root, &x, &y, &width, &height, &bw, &dep))
    return None;

 *widthp = (int) width;
 *heightp = (int) height;
 fore = w->mailbox.foreground_pixel;
 back = w->core.background_pixel;
 return XmuCreatePixmapFromBitmap(dpy, w->core.window, bitmap, 
				      width, height, depth, fore, back);
}


static void
ClassInitialize()
{
#ifndef XPM
 static XtConvertArgRec screenConvertArg[] = {
    { XtWidgetBaseOffset, (caddr_t) XtOffset(Widget, core.screen), sizeof(Screen *) }
    };
#endif

 XawInitializeWidgetSet();

#ifndef XPM
 XtAddConverter (XtRString, XtRBitmap, XmuCvtStringToBitmap,
		    screenConvertArg, XtNumber(screenConvertArg));
#endif

 return;
} /* ClassInitialize */


/* ARGSUSED */
static void
Initialize(request, new)
Widget request, new;
{
    MailwatchWidget w = (MailwatchWidget) new;

    GetUserName(w);

    GetHostName(w);

    if (!w->mailbox.filename)
	GetMailFile(w);

    if (w->mailbox.reverseVideo) {
	Pixel tmp;

	tmp = w->mailbox.foreground_pixel;
	w->mailbox.foreground_pixel = w->core.background_pixel;
	w->core.background_pixel = tmp;
    }

    GetNormalGC(w);

    GetInvertGC(w);

    w->mailbox.flag_up = FALSE;		/* because it hasn't been shown yet */
    w->mailbox.mail.pixmap = None;
    w->mailbox.nomail.pixmap = None;

    w->mailbox.last_access = (stat(w->mailbox.filename, &stbuf) == 0) ?
                             stbuf.st_atime : 0 ;	/* last accessed */
    return;
}


/* ARGSUSED */
static void
clock_tic(client_data, id)
caddr_t client_data;
XtIntervalId *id;
{
    MailwatchWidget w = (MailwatchWidget) client_data;

    check_mailbox(w, FALSE);
}


static void
set_title(str)
String	str;
{
 Display	*dpy = XtDisplay(toplevel);
 Window		win  = XtWindow(toplevel);
 String		c, name, title;


 XFetchName(dpy, win, &name);
 if (! name)
    name = XtNewString("xmail");
 else
    if ((c = (char *)strrchr(name, '_')) != NULL)
        *c = '\0';

 if (! *str)
    title = XtMalloc((unsigned) strlen(name) + 1);
 else
    title = XtMalloc((unsigned) strlen(name) + strlen(str) + 2);

 if (! title)
    XtError("xmail: Insufficient memory to allocate title space");

 if (! *str)
    (void) sprintf(title, "%s", name);
 else
    (void) sprintf(title, "%s_%s", name, str);

 XStoreName(dpy, win, title);

 XtFree((char *)name);
 XtFree((char *)title);
} /* end - set_title */


void
set_host(m)
MailwatchWidget		m;
{
 register Display	*dpy = XtDisplay(m);
 register Window	win = XtWindow(m);
 register int		x, y, h, w;


 set_title(m->mailbox.mailhost);

 m->mailbox.useHost = True;
 m->mailbox.useName = False;
 h = m->mailbox.font->max_bounds.width - m->mailbox.font->max_bounds.lbearing;
 w = strlen(m->mailbox.mailhost);
 h = (h * w) - 2;
 x = max(m->core.width - h, 2);			/* if (x < 2) x = 2; */
 x /= 2;
 y = m->core.height - m->mailbox.font->descent;

 XFillRectangle(dpy, win, m->mailbox.invert_GC,
                      0, y - m->mailbox.font->ascent, m->core.width,
                      m->mailbox.font->ascent + m->mailbox.font->descent);

 XDrawString(dpy, win, m->mailbox.normal_GC, x, y, m->mailbox.mailhost, w);
} /* end - set_host */


void
set_user(m)
MailwatchWidget		m;
{
 register Display	*dpy = XtDisplay(m);
 register Window	win = XtWindow(m);
 register int		x, y, h, w;


 set_title(m->mailbox.username);

 m->mailbox.useHost = False;
 m->mailbox.useName = True;
 h = m->mailbox.font->max_bounds.width - m->mailbox.font->max_bounds.lbearing;
 w = strlen(m->mailbox.username);
 h = (h * w) - 2;
 x = max(m->core.width - h, 2);			/* if (x < 2) x = 2; */
 x /= 2;
 y = m->core.height - m->mailbox.font->descent;

 XFillRectangle(dpy, win, m->mailbox.invert_GC,
                      0, y - m->mailbox.font->ascent, m->core.width,
                      m->mailbox.font->ascent + m->mailbox.font->descent);

 XDrawString(dpy, win, m->mailbox.normal_GC, x, y, m->mailbox.username, w);
} /* end - set_user */


void
set_none(m)
MailwatchWidget		m;
{
 set_title("");
 m->mailbox.useHost = m->mailbox.useName = False;
 redraw_mailbox(m);
} /* end - set_none */


static GC
GetNormalGC(w)
MailwatchWidget w;
{
    XtGCMask valuemask;
    XGCValues xgcv;

    valuemask = GCForeground | GCBackground | GCFunction | GCGraphicsExposures;
    xgcv.foreground = w->mailbox.foreground_pixel;
    xgcv.background = w->core.background_pixel;
    xgcv.function = GXcopy;
    xgcv.graphics_exposures = False;
    w->mailbox.normal_GC = XtGetGC((Widget) w, valuemask, &xgcv);
}

static GC
GetInvertGC(w)
MailwatchWidget w;
{
    XtGCMask valuemask;
    XGCValues xgcv;

    valuemask = GCForeground | GCBackground | GCFunction | GCGraphicsExposures;
    xgcv.foreground = w->core.background_pixel;
    xgcv.background = w->mailbox.foreground_pixel;
    xgcv.function = GXcopy;
    xgcv.graphics_exposures = False;	/* this is Bool, not Boolean */
    w->mailbox.invert_GC = XtGetGC((Widget) w, valuemask, &xgcv);
}


#ifdef XPM
/*
 * split_path_string - split a colon-separated list into its constituent
 * parts; to release, free list[0] and list.  From libXmu LocBitmap.c
 */
static char **
split_path_string (src)
register char *src;
{
 int nelems = 1;
 register char *dst;
 char **elemlist, **elem;

 /* count the number of elements */
 for (dst = src; *dst; dst++) if (*dst == ':') nelems++;

 /* get memory for everything */
 dst = (char *) XtMalloc ((unsigned) (dst - src) + 1);
 if (!dst) return NULL;
 elemlist = (char **) XtCalloc ((unsigned) (nelems + 1), sizeof (char *));
 if (!elemlist) {
    XtFree (dst);
    return NULL;
   }

 /* copy to new list and walk up nulling colons and setting list pointers */
 (void) strcpy (dst, src);
 for (elem = elemlist, src = dst; *src; src++) {
     if (*src == ':') {
        *elem++ = dst;
        *src = '\0';
        dst = src + 1;
       }
    }
 *elem = dst;

 return elemlist;
}

char			**bitmap_file_paths = NULL;

#ifndef MAXPATHLEN
#define MAXPATHLEN	256
#endif

#ifndef BITMAPDIR
#define BITMAPDIR	"/usr/include/X11/bitmaps"
#endif


/*
** @(#)XMyLocateXPixmapFile() - Cloned from libXmu LocBitmap.c for use in XPM
*/
static Pixmap
XMyLocateXPixmapFile(w, depth, name, widthp, heightp)
Widget	w;
int	depth;
char	*name;
int	*widthp, *heightp;	/* RETURN */
{
 Bool			try_plain_name = True;
 Display		*dpy = XtDisplay(w);
 int			screen = DefaultScreen(dpy);
 Colormap		cmap = DefaultColormap(dpy, screen);
 Pixmap			pixmap;					/* RETURN */
 Visual			*visual	= DefaultVisual(dpy, screen);
 XrmName		xrm_name[2];
 XrmClass		xrm_class[2];
 XrmRepresentation	rep_type;
 XrmValue		value;
 int			i;
 char			filename[MAXPATHLEN];
 char			**file_paths = NULL;
/*
** look for bitmap path (only once, because we're going to call this twice)
*/
 if (bitmap_file_paths == NULL) {
    xrm_name[0] = XrmStringToName ("bitmapFilePath");
    xrm_name[1] = NULL;
    xrm_class[0] = XrmStringToClass ("BitmapFilePath");
    xrm_class[1] = NULL;
    /*
    ** XXX - warning, derefing Display * until XDisplayDatabase
    */
    if (!dpy->db)
       (void) XGetDefault (dpy, "", "");

    if (XrmQGetResource (dpy->db, xrm_name, xrm_class, &rep_type, &value) &&
        rep_type == XrmStringToQuark(XtRString)) {
       bitmap_file_paths = split_path_string(value.addr);
      }
   }
 file_paths = bitmap_file_paths;
/*
** Search order:
**    1.  name if it begins with / or ./
**    2.  "each prefix in file_paths"/name
**    3.  BITMAPDIR/name
**    4.  name if didn't begin with / or .
*/
 for (i = 1; i <= 4; i++) {
     char *fn = filename;

     switch (i) {
       case 1:
            if (!(name[0] == '/' || (name[0] == '.') && name[1] == '/'))
              continue;
            fn = name;
            try_plain_name = False;
            break;
       case 2:
            if (file_paths && *file_paths) {
                (void) sprintf (filename, "%s/%s", *file_paths, name);
                file_paths++;
                i--;
                break;
            }
            continue;
       case 3:
            (void) sprintf (filename, "%s/%s", BITMAPDIR, name);
            break;
       case 4:
            if (!try_plain_name) continue;
            fn = name;
            break;
       }

     if (PixmapSuccess == XReadPixmapFile(dpy, visual, w->core.window, cmap, fn,
                         depth, &pixmap, widthp, heightp, 0, 0, NULL, 0, NULL))
       {
        return pixmap;
       }
    }
 return None;
}
#endif



static void
Realize(gw, valuemaskp, attr)
Widget gw;
XtValueMask *valuemaskp;
XSetWindowAttributes *attr;
{
 Arg			args[2];
 MailwatchWidget	w = (MailwatchWidget) gw;
 register Display	*dpy = XtDisplay(w);
 int			depth = w->core.depth;
#ifdef XPM
 int			x, y, screen = DefaultScreen(dpy);
 Colormap		cmap = DefaultColormap(dpy, screen);
 Visual			*visual	= DefaultVisual(dpy, screen);
 Pixmap			bitmap, XmuLocateBitmapFile();
#endif


 XtCreateWindow(gw, InputOutput, (Visual *) CopyFromParent, *valuemaskp, attr);
/*
** build up the pixmaps that we'll put into the icon image
*/
 if (w->mailbox.mail.bitmap == None) {	/* if user failed to specify an icon */
#ifndef	XPM			/* Non-XPM uses [no]mail.bitmap as a pixmap */
    w->mailbox.mail.bitmap = XCreateBitmapFromData(dpy, w->core.window,
					mail_bits, mail_width, mail_height);

    w->mailbox.mail.pixmap = make_pixmap(dpy, w, w->mailbox.mail.bitmap, depth,
					 &w->mailbox.mail.width,
					 &w->mailbox.mail.height);
   } else {			/* non-XPM used XmuConvertStringToPixmap */
    w->mailbox.mail.pixmap = make_pixmap(dpy, w, w->mailbox.mail.bitmap, depth,
					 &w->mailbox.mail.width,
					 &w->mailbox.mail.height);
#else
    XCreatePixmapFromData(dpy, visual, w->core.window, cmap, mail_xpm, depth,
                          &w->mailbox.mail.pixmap, &w->mailbox.mail.width, 
                          &w->mailbox.mail.height, 0, 0, NULL, 0, NULL);
   } else {
    /*
    ** Because for XPM we define mailbox.[no]mail.bitmap as a string,
    ** we must do the file conversion ourselves.  First, see if it is
    ** a plain old bitmap file.  Failing that, try for an XPM format
    ** file.  Failing that, default to our internally defined xpm icon.
    */
    if (bitmap = XmuLocateBitmapFile(w->core.screen, w->mailbox.mail.bitmap,
                                     NULL, 0, &w->mailbox.mail.width,
                                              &w->mailbox.mail.height, &x, &y))
       w->mailbox.mail.pixmap = make_pixmap(dpy, w, bitmap, depth,
                                            &w->mailbox.mail.width,
					    &w->mailbox.mail.height);

    else if (! (w->mailbox.mail.pixmap = XMyLocateXPixmapFile((Widget) w, depth,
                                                     w->mailbox.mail.bitmap,
                                                    &w->mailbox.mail.width,
                                                    &w->mailbox.mail.height)))
            XCreatePixmapFromData(dpy, visual, w->core.window, cmap,
                                  mail_xpm, depth, &w->mailbox.mail.pixmap,
                                                   &w->mailbox.mail.width, 
                                                   &w->mailbox.mail.height,
                                  0, 0, NULL, 0, NULL);
#endif
   }

 if (w->mailbox.nomail.bitmap == None) {
#ifndef	XPM
    w->mailbox.nomail.bitmap = XCreateBitmapFromData(dpy, w->core.window,
                                 no_mail_bits, no_mail_width, no_mail_height);

    w->mailbox.nomail.pixmap = make_pixmap(dpy, w, w->mailbox.nomail.bitmap,
					 depth, &w->mailbox.nomail.width,
					 &w->mailbox.nomail.height);
   } else {
    w->mailbox.nomail.pixmap = make_pixmap(dpy, w, w->mailbox.nomail.bitmap,
					 depth, &w->mailbox.nomail.width,
					 &w->mailbox.nomail.height);
#else
    XCreatePixmapFromData(dpy, visual, w->core.window, cmap, nomail_xpm, depth,
                          &w->mailbox.nomail.pixmap, &w->mailbox.nomail.width, 
                          &w->mailbox.nomail.height, 0, 0, NULL, 0, NULL);
   } else {
    if (bitmap = XmuLocateBitmapFile(w->core.screen, w->mailbox.nomail.bitmap,
                                        NULL, 0, &w->mailbox.nomail.width,
                                        &w->mailbox.nomail.height, &x, &y))
       w->mailbox.nomail.pixmap = make_pixmap(dpy, w, bitmap, depth,
					      &w->mailbox.nomail.width,
					      &w->mailbox.nomail.height);

    else if (! (w->mailbox.nomail.pixmap = XMyLocateXPixmapFile((Widget) w, depth,
                                                   w->mailbox.nomail.bitmap,
                                                  &w->mailbox.nomail.width,
                                                  &w->mailbox.nomail.height)))
            XCreatePixmapFromData(dpy, visual, w->core.window, cmap, nomail_xpm,
                                  depth, &w->mailbox.nomail.pixmap,
                                         &w->mailbox.nomail.width, 
                                         &w->mailbox.nomail.height,
                                  0, 0, NULL, 0, NULL);
#endif
   }
/*
** the size of the icon should be the size of the larger icon image.
*/
 w->core.width  = max(w->mailbox.mail.width, w->mailbox.nomail.width);
 w->core.height = max(w->mailbox.mail.height, w->mailbox.nomail.height);

 XtSetArg(args[0], XtNwidth,  (XtArgVal) w->core.width);
 XtSetArg(args[1], XtNheight, (XtArgVal) w->core.height);
 XtSetValues(XtNameToWidget(toplevel, "icon"), args, 2);

/* set status check timer */
 w->mailbox.interval_id = XtAddTimeOut(w->mailbox.update * 1000,
	                                        clock_tic, (caddr_t) w);
#ifdef XPM
 if (bitmap_file_paths) {
    XtFree((char *)bitmap_file_paths[0]);
    XtFree((char *)bitmap_file_paths);
   }
#endif
}

static void
Destroy(gw)
Widget gw;
{
 MailwatchWidget w = (MailwatchWidget) gw;
 Display	*dpy = XtDisplay(gw);

 XtFree(w->mailbox.filename);
 XtFree(w->mailbox.username);
 XtFree(w->mailbox.mailhost);
 REMOVE_TIMEOUT(w->mailbox.interval_id);
 XtReleaseGC(w, w->mailbox.normal_GC);
#ifndef	XPM
 XFreePixmap(dpy, w->mailbox.mail.bitmap);
 XFreePixmap(dpy, w->mailbox.nomail.bitmap);
#endif
 XFreePixmap(dpy, w->mailbox.mail.pixmap);
 XFreePixmap(dpy, w->mailbox.nomail.pixmap);
}

static void
Redisplay(gw)
Widget gw;
{
    MailwatchWidget w = (MailwatchWidget) gw;

    REMOVE_TIMEOUT(w->mailbox.interval_id);
    check_mailbox(w, TRUE);
}


/*
** Modified to NOT update the mail file timestamp via utimes(), and to
** ignore state change where mail adds a status record to the message.
** The first eliminates collisions with mail during delivery, and the
** second eliminates false triggers for new mail.  The number of times
** the terminal bell is rung is controlled by the same .mailrc resource
** (bell) used by Sunview's mailtool.
*/
static void
check_mailbox(w, force_redraw)
MailwatchWidget w;
Boolean force_redraw;
{
 int		i, redraw = 0;
 char		*p = NULL;


 if (stat(w->mailbox.filename, &stbuf) < 0) {		/* no mail file */
    if (w->mailbox.flag_up == TRUE) {
       force_redraw = 0;
       UnsetNewmail(w, NULL, NULL);
      }
   } else {
    if (stbuf.st_atime > stbuf.st_mtime &&
        stbuf.st_atime >= w->mailbox.last_access) {	/* mail was seen */
       w->mailbox.last_access = stbuf.st_atime;
       if (w->mailbox.flag_up == TRUE) {
          force_redraw = 0;
          UnsetNewmail(w, NULL, NULL);
         }
      } else {
       if (stbuf.st_size != 0 && 
           stbuf.st_mtime > w->mailbox.last_access) {	/* got new mail */
          w->mailbox.last_access = stbuf.st_mtime;
          if (w->mailbox.flag_up == FALSE) {
             if (w->mailbox.bell) {
                i = 1;
                if (p = (char *)GetMailrc("bell"))
                   (void) sscanf(p, "%d", &i);
                i = between(1, i, 5);
                for (; i > 0; i--)
                    XBell(XtDisplay(w), MAILBOX_VOLUME);
               }
             w->mailbox.flag_up = TRUE;
             redraw = TRUE;
             XtCallCallbacks(w, XtNcallback, NULL);
            }
         }
      }
   }

 if (redraw || force_redraw)
    redraw_mailbox(w);

 /* reset timer */
 w->mailbox.interval_id = XtAddTimeOut(w->mailbox.update * 1000,
	clock_tic, (caddr_t) w);
} /* check_mailbox */

/*
** Retrieve the user's mailbox filename - use MAIL environment value, if set
*/
static void
GetMailFile(w)
MailwatchWidget w;
{
 if (! (w->mailbox.filename = (char *) GetMailEnv("MAIL"))) {
    if (! (w->mailbox.filename = XtMalloc((unsigned) strlen(MAILBOX_DIRECTORY)
	   + 1 + strlen(w->mailbox.username) + 1))) {
       (void) fprintf(stderr, "Mailbox widget: can't allocate enough memory.\n");
       CloseDown(w, 1);
      }
    (void) sprintf(w->mailbox.filename, "%s/%s\0", MAILBOX_DIRECTORY, w->mailbox.username);
   }
} /* GetMailFile */


/*
** Retrieve the mailbox user's name
*/
static void
GetUserName(w)
MailwatchWidget w;
{
 char *username = (char *)getlogin();

 if (! username) {
    struct passwd *pw = getpwuid(getuid());

    if (! pw) {
       (void) fprintf(stderr, "Mailbox widget: can't find your username.\n");
       CloseDown(w, 1);
      }
    username = pw->pw_name;
   }

 if (! (w->mailbox.username = XtNewString(username))) {
    XtWarning("Mailbox widget: can't allocate space for username.\n");
    CloseDown(w, 1);
   }
} /* GetUserName */


/*
** Retrieve the process host name
*/
static void
GetHostName(w)
MailwatchWidget w;
{
 char	hostname[MAXHOSTNAMELEN];


 (void) gethostname(hostname, MAXHOSTNAMELEN);

 if (! (w->mailbox.mailhost = XtNewString(hostname))) {
    XtWarning("Mailbox widget: can't allocate space for hostname.\n");
    CloseDown(w, 1);
   }
} /* GetHostName */


static void
CloseDown(w, status)
MailwatchWidget w;
int status;
{
    Display *dpy = XtDisplay(w);

    XtDestroyWidget(w);
    XCloseDisplay(dpy);
    _exit(status);
}

/* ARGSUSED */
static Boolean
SetValues(gcurrent, grequest, gnew)
Widget gcurrent, grequest, gnew;
{
    MailwatchWidget current = (MailwatchWidget) gcurrent;
    MailwatchWidget new = (MailwatchWidget) gnew;
    Boolean redisplay = FALSE;

    if (current->mailbox.update != new->mailbox.update) {
	REMOVE_TIMEOUT(current->mailbox.interval_id);
	new->mailbox.interval_id = XtAddTimeOut(new->mailbox.update * 1000,
	    clock_tic,
	    (caddr_t) gnew);
    }
    if (current->mailbox.foreground_pixel != new->mailbox.foreground_pixel ||
	current->core.background_pixel != new->core.background_pixel) {
	XtReleaseGC(current, current->mailbox.normal_GC);
	GetNormalGC(new);
	redisplay = TRUE;
    }
    return (redisplay);
}

/*
 * drawing code
 */
static void
redraw_mailbox(m)
MailwatchWidget m;
{
    register Display	*dpy = XtDisplay(m);
    register Window	win = XtWindow(m);
    register int	x, y;
    Pixel		back = m->core.background_pixel;
    GC			gc = m->mailbox.normal_GC;
    struct _mbimage	*im;


    if (m->mailbox.flag_up)			/* paint the "up" position */
	im = &m->mailbox.mail;
    else					/* paint the "down" position */
	im = &m->mailbox.nomail;

    /* center the picture in the window */
    x = ((int)m->core.width - im->width) / 2;
    y = ((int)m->core.height - im->height) / 2;

    XSetWindowBackground(dpy, win, back);
    XClearWindow(dpy, win);
    XCopyArea(dpy, im->pixmap, win, gc, 0, 0, im->width, im->height, x, y);

    if (m->mailbox.useHost || m->mailbox.useName) {
       if (m->mailbox.useHost) set_host(m);
       else set_user(m);
      }
}


void
reset_mailbox(gw, down)
Widget	gw;
int	down;
{
 MailwatchWidget w = (MailwatchWidget) gw;
 char	*p;
 int	i;


 if (down)
    w->mailbox.flag_up = FALSE;
 else {
    if (w->mailbox.flag_up == FALSE) {
       if (w->mailbox.bell) {
          i = 1;
          if (p = (char *)GetMailrc("bell"))
             (void) sscanf(p, "%d", &i);
          i = between(1, i, 5);
          for (; i > 0; i--)
              XBell(XtDisplay(w), MAILBOX_VOLUME);
         }
      }
    w->mailbox.flag_up = TRUE;
   }

 redraw_mailbox(w);
} /* reset_mailbox */
