/*
 * xmail - X window system interface to the mail program
 *
 * Copyright 1989 The University of Texas at Austin
 *
 * Author:	Po Cheung
 * Date:	March 10, 1989
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.  The University of Texas at Austin makes no 
 * representations about the suitability of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
 *
 * Copyright 1990,1991 by National Semiconductor Corporation
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of National Semiconductor Corporation not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * NATIONAL SEMICONDUCTOR CORPORATION MAKES NO REPRESENTATIONS ABOUT THE
 * SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS"
 * WITHOUT EXPRESS OR IMPLIED WARRANTY.  NATIONAL SEMICONDUCTOR CORPORATION
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO
 * EVENT SHALL NATIONAL SEMICONDUCTOR CORPORATION BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * The following software modules were created and are Copyrighted by
 * National Semiconductor Corporation:
 *
 * 1. markIndex:
 * 2. SelectionNumber:
 * 3. SetCursor: and
 * 4. SetXY.
 *
 * Author:  Michael C. Wagnitz - National Semiconductor Corporation
 *
 */


#include <ctype.h>
#include "global.h"
#include "patchlevel.h"
#include "revtable.h"


/*
** @(#)Bell() - write a status message and (optionally) ring the terminal bell
*/
void
Bell(msg)
String		msg;
{
 Arg		args[1];
 char		*p;
 char		buf[BUFSIZ];
 int		size;
 static int	worthy = 0;		/* only msgs with ending newline are */


 bzero(buf, (int) BUFSIZ);
 size = strlen(msg);
 if (size >= BUFSIZ) size = BUFSIZ - 1;

 (void) strncpy(buf, msg, size);

 p = strchr(buf, '\n');			/* messages with newline get a bell */
/*
** Because we have now added enter/leave window event information messages,
** we must play a slight game with incoming messages, to prevent any error
** messages from a menu selection failure or status messages from a command
** button being overwritten by the subsequent event info messages produced
** when the menu popup is dismissed or the command completes.  To do this,
** we make the bell worthy-ness flag static, and only replace a message after
** first seeing a non-default status message, which will reset the worthy-ness
** flag, but otherwise be ignored.  When the first non-default, non-error
** message comes in, simply reset our bell worthy-ness flag and return, leaving
** the previous error message still intact.  This means, to ensure seeing the
** default message, first send a blank line to reset the worthy-ness flag.
*/
 if (! XMail.Show_Info)			/* if NOT showing info flags always */
    worthy = 0;				/* reset the bell worthy-ness flag */
 else {
    if (worthy && p == NULL) {		/* if last was but this ain't urgent */
       if (strcmp(buf, Default_Status_Info))	/* and not the default info */
          worthy = 0;			/* reset the bell worthy-ness flag */
       return;				/* ignore event info if menu errored */
      }					/* by leaving previous error message */
   }

 if (size == 0)				/* if intent was just to reset flag */
    return;

 if (p != NULL) {			/* Only display first line of output */
    *p = '\0';				/* (no new line at end of label) */
    worthy = 1;				/* this message is worthy of a bell */
   }

 if (worthy && XMail.bellRing)		/* ring bell if not silenced by user */
    XBell(XtDisplay (toplevel), 33);

 XtSetArg(args[0], XtNlabel, (XtArgVal) buf);	/* show this message text */
 XtSetValues(XtNameToWidget(toplevel, "topBox.statusWindow"), args, 1);
 XFlush(XtDisplay(toplevel));
} /* Bell */


/*
** @(#)figureWidth() - determine the figure width of the specified font
*/
int
figureWidth(font)
XFontStruct *font;
{
 Atom		_XA_FIGURE_WIDTH;
 unsigned long	width = 0;


 _XA_FIGURE_WIDTH = XInternAtom(XtDisplay(toplevel), "FIGURE_WIDTH", FALSE);
 if ((_XA_FIGURE_WIDTH != NULL) &&
    ((! XGetFontProperty(font, _XA_FIGURE_WIDTH, &width)) || (width == 0)))
    if (font->per_char && font->min_char_or_byte2 <= '$' &&
                          font->max_char_or_byte2 >= '$')
       width = font->per_char['$' - font->min_char_or_byte2].width;
    else
       width = font->max_bounds.width;

 return(width);
} /* end - figureWidth */


/*
** @(#)markIndex() - add or remove a tag from the start of a mail header line
*/
void
markIndex(s)
char	*s;
{
 int			size;
 char			buf[3];
 Arg			args[1];
 String			c, p;
 Widget			iw;
 XawTextBlock		text;
 XawTextPosition	pos, old;


 iw = XtNameToWidget(toplevel, "topBox.indexWindow");
 pos = XawTextGetInsertionPoint(iw);	/* save the current insertion point */
 (void) strcpy(buf, s);			/* make a copy of our marking string */
 if (pos != 0) {			/* if not already at front of buffer */
    text.firstPos = 0;			/* find the start of this index line */
    text.length   = 1;
    text.ptr      = "\n";
    text.format   = FMT8BIT;
    if ((old = XawTextSearch(iw, XawsdLeft, &text)) != XawTextSearchError)
       pos = old + 1;
    else pos = 0;
   }

 if (buf[0] == '>') {			/* if mark is for 'current' pointer */
    XtSetArg(args[0], XtNstring, &p);	/* retrieve the current index buffer */
    XtGetValues(iw, args, 1);

    if (strlen(p) > pos + 1)
       if (p[pos + 1] == 'S')		/* keep the Save marker if it exists */
          if (strlen(buf) > 1)
             buf[1] = '\0';

    for (c = p; *c; c++)
        if (*c == '>' && (c == p || *(c - 1) == '\n')) {
           old = c - p;			/* if found, remove the old '>' mark */
           text.firstPos = 0;
           text.length   = 1;
           text.ptr      = " ";
           text.format   = FMT8BIT;
           XawTextReplace(iw, (XawTextPosition) old, (XawTextPosition) old + 1, &text);
           break;
          }
   }

 size = strlen(buf);			/* now write the specified marker */
 old = (buf[0] == '>') ? pos : pos + 1;
 text.firstPos = 0;
 text.length   = size;
 text.ptr      = buf;
 text.format   = FMT8BIT;
 XawTextReplace(iw, (XawTextPosition) old, (XawTextPosition) old + size, &text);

 XawTextSetInsertionPoint(iw, pos);	/* reset our actual insertion point */
} /* markIndex */


/*
** @(#)SelectionNumber() - Get mail index number from line position
*/
int
SelectionNumber(undeleting)
int	undeleting;
{
 Arg			args[1];
 Widget			iw = XtNameToWidget(toplevel, "topBox.indexWindow");
 String			c, str = NULL;
 int			selection_number = 0;
 

 XtSetArg(args[0], XtNstring, &str);
 XtGetValues(iw, args, 1);

 for (c = str; *c; c++)
     if (*c == '>' && (c == str || *(c - 1) == '\n'))
        break;

 if (*c && (! undeleting || *(c + 1) == 'D')) {
    for (; *c && !isdigit(*c); c++);
    (void) sscanf(c, "%d", &selection_number);
   }

 return(selection_number);
} /* SelectionNumber */

extern Window	WaitCursorWindow;
Boolean		Waiting;

/* ARGSUSED */
/*
** @(#)SetCursor() - sets the wait cursor or restores the default
*/
void
SetCursor(waiting)
int	waiting;		/* a non-zero value sets the busy cursor */
{
 if (! waiting)
    XUnmapWindow(XtDisplay(toplevel), WaitCursorWindow);
 else {
      XMapWindow(XtDisplay(toplevel), WaitCursorWindow);
    Waiting = TRUE;		/* to prevent overwrite of important info */
   }

 XFlush(XtDisplay(toplevel));
} /* SetCursor */


/*
** @(#)SetXY() - Set relative window coordinates including specified offset
*/
void
SetXY(target, reference, X_offset, Y_offset)
Widget	target, reference;
int	X_offset, Y_offset;
{
 Arg		args[2];
 Display	*dpy = XtDisplay(reference);
 
 Window		dumy;
 int		x, y;


 XTranslateCoordinates(dpy, XtWindow(reference),
                       RootWindow(dpy, DefaultScreen(dpy)),
                       X_offset, Y_offset, &x, &y, &dumy);
 /*
 ** Keep window within root window borders (don't place it off-screen)
 */
 if (! XtIsRealized(target))
    XtRealizeWidget(target);		/* to get width and height values */

 if (x + target->core.width > RootWidth)
    x = RootWidth - target->core.width - 2;

 if (y + target->core.height > RootHeight)
    y = RootHeight - target->core.height - 2;

 XtSetArg(args[0], XtNx, x);
 XtSetArg(args[1], XtNy, y);
 XtSetValues(target, args, 2);
} /* end - SetXY */


/*
** @(#)TextGetLastPos() - return position of last text character
*/
XawTextPosition
TextGetLastPos(w)
Widget w;
{
 TextWidget ctx = (TextWidget) w;
 return (XawTextSourceScan(ctx->text.source,0,XawstAll,XawsdRight,1,TRUE));
}


/*
** @(#)UpdateTitleBar() - replace information in the title bar title
*/
void
UpdateTitleBar(msg)
char *msg;
{
 char		message[BUFSIZ];
 Arg		args[1];
 Widget		w;


 (void) sprintf(message, "%s%d - %s", TITLE, PATCHLEVEL, msg);

 w = XtNameToWidget(toplevel, "topBox.titleBar.title");

 XtSetArg(args[0], XtNlabel, (XtArgVal) message);
 XtSetValues(w, args, 1);
 
 w = XtNameToWidget(toplevel, "topBox.commandPanel.Newmail.Newmail_menu.menu.inc");
 if (w)
    XtSetSensitive(w, In_System_Folder());
} /* UpdateTitleBar */


/*
** @(#)writeTo() - write text to the specified text widget
*/
void
writeTo(w, s, do_append)
Widget	w;
char   *s;
int	do_append;
{
 XawTextBlock		text;
 XawTextPosition	startPos, endPos;


 text.firstPos = 0;
 text.length   = strlen(s);
 text.ptr      = s;
 text.format   = FMT8BIT;

 endPos = TextGetLastPos(w) + (do_append ? 0 : 1);
 startPos = (do_append ? endPos : 0);

 XawTextReplace(w, startPos, endPos, &text);

 endPos = TextGetLastPos(w);

 XawTextSetInsertionPoint(w, endPos);

 XawTextInvalidate(w, 0, endPos);
} /* writeTo */


/*
** @(#)writeText() - replace the current text string in the text window.
**                 Also look for an X-Face: header and if found, display.
*/
void
writeText(buf)
char    *buf;
{
 Arg            args[2];
 Widget		w = XtNameToWidget(toplevel, "topBox.textWindow.text");
#ifdef X_FACE
 Display        *dpy = XtDisplay(w);
 Widget         fw;
 Window         rw;
 char           cb[1024], fb[2048], *ptr, *xface;
 int            i, n, x, y;
#endif

 if (buf && *buf) {
#ifndef X_FACE
    XtSetArg(args[0], XtNstring, buf);
    XtSetValues(w, args, 1);
#else
    if (! (fw = XtNameToWidget(XtParent(w), "face"))) {
       XtSetArg(args[0], XtNstring, buf);
       XtSetValues(w, args, 1);
       return;
      }
    /*
    ** First, unmap any current picture.
    **
    ** Look for a line containing an 'X-Face:' header, followed by 72
    ** characters of compressed data.  The second and any subsequentlines
    ** will contain an initial space (which is ignored), followed by79
    ** characters of compressed data.  The last line may contain fewer than 79
    ** characters.
    **
    ** The X-Face: header and any immediate whitespace (tabs or spaces) will be
    ** removed, and the remaining line placed in the internal buffer(minus
    ** any trailing newline).  On subsequent lines, initial whitespace will be
    ** removed, and the remainder of the data appended to the buffer(minus any
    ** trailing newline).
    **
    ** A blank line, a line without an initial whitespace character,or the
    ** end of the input buffer will signify the end of the X-Face data.  That
    ** buffer will then be uncompressed, and if the data was valid, displayed.
    */

    if (XtIsManaged(fw))
       XtUnmanageChild(fw);

    for (ptr = buf; *ptr; ptr++) {
        if (*ptr == '\n'                    ||
           strncmp(ptr, "Status:", 7) == 0  ||
           strncmp(ptr, "X-Face:", 7) == 0) break;

        for (; *ptr && *ptr != '\n'; ptr++);
       }

    if (! *ptr || strncmp(ptr, "X-Face:", 7) != 0) {
       XtSetArg(args[0], XtNstring, buf);
       XtSetValues(w, args, 1);
       return;
      }

    xface = ptr;	/* keep track of the start position of X-Face header */

    bzero(fb, 2048);
    for (i = 0, ptr += 7; *ptr; ptr++) {
        if (*ptr != ' ' && *ptr != '\t' && *ptr != '\n')
           fb[i++] = *ptr;
        if (*ptr == '\n' && *(ptr+1) && *(ptr+1) != ' ' && *(ptr+1) != '\t')
           break;
       }

    if (XMail.No_X_Hdr) {
       if (*ptr) ptr++;
       bcopy(ptr, xface, strlen(ptr) + 1);	/* suppress the X-Face header */
      }

    XtSetArg(args[0], XtNstring, buf);
    XtSetValues(w, args, 1);

    if (uncompface(fb) >= 0) {
       bzero(cb, 1024);
       for (i = n = 0;i < 1024;) {
           if (! sscanf(&fb[n], "%i%n", &x, &y)) break;
           cb[i++] = revtable[(x >> 8) & 0xFF];
           cb[i++] = revtable[x & 0xFF];
           n += y;
           while (fb[n] && (fb[n]==',' || fb[n]=='\n')) n++;
          }

       XtSetArg(args[0], XtNwidth, NULL);
       XtGetValues(XtParent(w), args, 1);
       n = args[0].value - 48;
       if (n < 0) n = 0;

       XtSetArg(args[0], XtNbitmap, NULL);
       XtGetValues(fw, args, 1);
       if (args[0].value != None)
          XFreePixmap(dpy, args[0].value);

       rw = RootWindow(dpy, DefaultScreen(dpy));
       XtSetArg(args[0], XtNbitmap, XCreateBitmapFromData(dpy, rw, cb, 48, 48));
       XtSetArg(args[1], XtNhorizDistance, n);
       XtSetValues(fw, args, 2);

       XtManageChild(fw);
      }
#endif
   }
} /* writeText */
