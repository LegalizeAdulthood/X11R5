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
 * The following software modules were created and are Copyrighted by National
 * Semiconductor Corporation:
 *
 * 1. In_System_Folder:
 * 2. icon_handler:
 * 3. index_handler:
 * 4. info_handler: and
 * 5. file_handler.
 *
 * Author:  Michael C. Wagnitz - National Semiconductor Corporation
 *
 */


#include <ctype.h>
#include "global.h"
#include "xmailregexp.h"
#include "MailwatchP.h"
#include <X11/Xaw/LabelP.h>

extern Boolean		Waiting;	/* TRUE when we set the busy cursor */
Boolean	In_Bogus_Mail_File = False;	/* TRUE when we go iconic */

/*
** Examine the current value of the titleBar title and return TRUE/FALSE
** to the question "Does it match the system mail folder name?"  The folder
** name in the title bar label should be surrounded by a pair of quote marks.
*/
int
In_System_Folder()
{
 int		len, result;
 char		*fq, *lq, *buf;
 Arg		args[1];
 MailwatchWidget mb = (MailwatchWidget) XtNameToWidget(toplevel, "icon.mailbox");
 Widget		tb = XtNameToWidget(toplevel, "topBox.titleBar.title");


 XtSetArg(args[0], XtNlabel, &buf);
 XtGetValues(tb, args, 1);

 if ((lq = strrchr(buf, '"')) == NULL) result = FALSE;
 else {					/* if a current folder name exists */
    if ((fq = strchr(buf, '"')) == NULL) result = FALSE;
    else {
       ++fq;
       len = lq - fq;
       result = (strncmp(mb->mailbox.filename, fq, len) == 0) ? TRUE : FALSE ;
      }
   }
 return(result);
} /* In_System_Folder */


/*
** Process map and unmap events for the icon widget to manage our iconify and
** deiconify actions and switch to/from a bogus mail folder.  This allows users
** to access mail from another process without colliding with an open folder.
** Once the bogus mail folder is accessed, we unlink it, to avoid having to
** explicitly remove it later.
*/
/* ARGSUSED */
XtEventHandler
icon_handler(w, client_data, event)
Widget	w;
caddr_t	client_data;
XEvent	*event;
{
 FILE	*fp;
 char	*cp, cmd[BUFSIZ];
 static Boolean	Was_System_Folder = False;	/* TRUE if we had been in */


 if (mailpid) {
    switch (event->type) {
           case MapNotify :
                if (! In_Bogus_Mail_File) {
                   Was_System_Folder = In_System_Folder();
                   (void) sprintf(cmd, "%s+", tmpName);
                   if (fp = fopen(cmd, "w")) {	/* create a bogus mail file */
                      (void) fprintf(fp, "\n");
                      (void) fclose(fp);
                      (void) sprintf(cmd, "file %s+", tmpName);
                      cp = QueryMail(cmd);	/* focus our attention there */
                      XtFree(cp);
		      (void) UpdateTitleBar("No current folder");
                      (void) sprintf(cmd, "%s+", tmpName);
                      (void) unlink(cmd);	/* toss it now that we're in */
                      In_Bogus_Mail_File = True;
                     }
                  }
                break;
           case UnmapNotify :
                if (In_Bogus_Mail_File) {
                   if (strcmp(Command, "Start") != 0) {	/* if not starting up */
                      if (Was_System_Folder) {		/* go to system file */
                         (void) strcpy(Command, "file %\n");
                        } else {			/* go to prev. file */
                         (void) strcpy(Command, "file #\n");
                        }
                     } else {
                      if (XMail.MFileName) {	/* start in specified folder */
                         (void) sprintf(Command, "file %s\n", XMail.MFileName);
                        } else {		/* or else use system folder */
                         (void) strcpy(Command, "file %\n");
                        }
                     }
                   writeMail(Command);		/* let parser do the updating */
                   In_Bogus_Mail_File = False;
                   Was_System_Folder = False;
                  } else if (In_System_Folder())
                            UnsetNewmail(NULL, NULL, NULL);
                break;
           default:
                break;
          } /* end - switch on event type */
   } /* end - if mail is active */
} /* icon_handler */


/*
** @(#)index_handler() - find value of specified or next appropriate message
*/
int
index_handler(msg_num, undeleting)
int	msg_num;
int	undeleting;
{
 Arg			args[1];
 Cardinal		j, k;
 String			c, s, IBuf;
 Widget			iw;
 XawTextPosition	pos;


 Bell("");					/* reset bell worthyness flag */
 Bell(Default_Status_Info);

 k = msg_num;					/* Try to use specified msg. */
 if (k == 0) {					/* But, if not specified... */
    c = QueryMail("=");
    if (*c)
       (void) sscanf(c, "%d", &k);		/* get msg num from mail. */
    XtFree(c);
   }

 iw = XtNameToWidget(toplevel, "topBox.indexWindow");
 IBuf = NULL;
 XtSetArg(args[0], XtNstring, &IBuf);
 XtGetValues(iw, args, 1);

 if (k <= 50) {					/* do short index from front */
    s = IBuf;
    for (j = k - 1; j > 0 && (c = strchr(s, '\n')) != NULL; j--) s = c + 1;
   } else {					/* if index is fairly large */
    for (s = &IBuf[strlen(IBuf) - 2]; s > IBuf && *s != '\n'; s--);
    for (c = s + 1; *c && !isdigit(*c); c++);	/* get number of last entry */
    j = 0;
    if (*c)
       (void) sscanf(c, "%d", &j);		/* look backwards from there */
    for (; j > k; j--) {
        if (s > IBuf) s--;
        for (; s > IBuf && *s != '\n'; s--);
       }
    if (*s == '\n') s++;
   } 

 j = k;
 for (;;) {					/* allow for deleted msgs */
     for (c = s; *c && !isdigit(*c); c++);
     if (! *c || k == 0 ||
	(sscanf(c, "%d", &j) && j <= k))	/* total may be less than us */
        break;

     if (s - 2 >= IBuf)
        for (s -= 2; s > IBuf && *s != '\n'; s--);
     if (*s == '\n') s++;
    }

 if (! undeleting && *(s + 1) == 'D') {		/* If this is a deleted msg */
    j = 0;					/* then num is inappropriate */
    for (;*s && *(s + 1) == 'D';) {		/* try looking above deletes */
        if (! (c = strchr(s,'\n'))) break;	/* don't go past end of buf */
        s = c + 1;
       }
    for (;s > IBuf && *(s + 1) == 'D';) {
        for (s -= 2; s > IBuf && *s != '\n'; s--);
        if (*s == '\n') s++;
       }
    if (*(s + 1) != 'D') {			/* If we found a readable msg */
       for (c = s; *c && !isdigit(*c); c++);
       if (*c)
          (void) sscanf(c, "%d", &j);
      }
   }

 if (j) {					/* if we have a valid msg num */
    pos = s - IBuf;				/* resolve our position */
    XawTextSetInsertionPoint(iw, pos);		/* point at selected line */
    markIndex("> ");				/* flag it as current */
   }

 return(j);					/* return msg actually found */
} /* index_handler */


/*
** @(#)info_handler - use enter/leave_notify events to alter status window info
*/
/*ARGSUSED*/
void
info_handler(w, client_data, event)
Widget	w;				/* unused */
caddr_t	client_data;
XEvent	*event;
{
 if      (event->type == LeaveNotify && ! Waiting) Bell(Default_Status_Info);
 else if (event->type == EnterNotify) {
         if (! Waiting) Bell(client_data);
         Waiting = FALSE;
        }
} /* end - info_handler */


/*
** @(#)file_handler() - Handle output of Start and file commands.
**                      If *Show_Last: resource is TRUE show last
**                      message in list, if none are marked as new.
*/
file_handler()
{
 Arg			args[1];
 Widget			iw = XtNameToWidget(toplevel, "topBox.indexWindow");
 XawTextPosition	pos;
 char			*c, *s, *p, msgnum_str[10];
 int			more_than_one, msg_num = 0;


 c = QueryMail("=");				/* Get current mail msg num. */
 while (! isdigit(*c)) {
    XtFree(c);
    if (! mailpid) return (msg_num);
    c = QueryMail("=");
   }
 (void) sprintf(msgnum_str, c);
 XtFree(c);

 c = QueryMail("file");				/* Forces current msg to one */
 if (! mailpid) return (msg_num);

 if (! In_Bogus_Mail_File)
    (void) UpdateTitleBar(c);
 XtFree(c);

 if (strcmp(Command, "Start") == 0) {		/* unless we're at Start, in */
    c = QueryMail("=");				/* which case we get it now  */
    (void) sprintf(msgnum_str, c);		/* (due to -N mailargv use). */
    XtFree(c);
   }
 if (! mailpid) return (msg_num);
 
 s = QueryMail("h");
 if (strncmp(s, "No applicable messages", 22) == 0) {
    Bell(s);
    writeTo(iw, " ", REPLACE);
    writeText(" ");
   } else {
/*
** find mail's idea of this folder's current message number
*/
    for (c = s; *c; c++)
        if (*c == '>' && (c == s || *(c - 1) == '\n'))
           break;
/*
** If the current message is marked new or unread, ensure as many of the new
** message headers as possible are displayed in the index window (especially
** the current one), else if the Show_Last resource is not false, select the
** last message in the header.
*/
   p = c;					/* save position of current */
   more_than_one = FALSE;			/* set a flag for new/unread */
   if (*(c+1) == 'N' || *(c+1) == 'U') more_than_one = TRUE;
   else if (XMail.Show_Last) {			/* find the last message */
           if (*p == '>') *p = ' ';		/* erase the original marker */
           if (c == s) {			/* if would've started @ One */
              for (p = &s[strlen(s)-2];
                   p > s && *p != '\n'; p--);
              if (*p == '\n') p++;		/* go to last line of buffer */
             } else {
              for (;*++c && *(c+1) != 'N' && *(c+1) != 'U';) {
                  if (! (c = strchr(p,'\n')))
                     break;			/* don't go past end of buf */
                  p = c + 1;
                 }
             }
           for (c = p; *c && !isdigit(*c); c++);
           if (*c)
              (void) sscanf(c, "%d", &msg_num);
           (void) sprintf(msgnum_str, "%d", msg_num);
          }
/*
** mark the current message's index line
*/
   *p = '>'; *(p+1) = ' ';
/*
** Return the current message number.
*/
    msg_num = atoi(msgnum_str);
/*
** Replacing existing header info in the index window with this buffer.
*/
    XtSetArg(args[0], XtNstring, s);
    XtSetValues(iw, args, ONE);
/*
** Ensure if we are seeing new or unread messages that as many as possible are
** displayed in the index window, to reduce the amount of scrolling required
** when reading(/deleting) them.
*/
    pos = p - s;
    if (more_than_one)
       XawTextSetInsertionPoint(iw, TextGetLastPos(iw));
    XawTextSetInsertionPoint(iw, pos);
   }
 XtFree(s);
 return(msg_num);
} /* file_handler */
