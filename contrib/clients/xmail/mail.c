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
 * 1. warp_handler: 
 * 2. endEdits: 
 * 3. editMail: 
 * 4. sendMail: 
 *
 * Author:  Michael C. Wagnitz - National Semiconductor Corporation
 *
 */
#include "global.h"
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>

#ifdef vms
extern int	noshare errno;
#else
extern int	errno;
#endif

#ifndef	DEFAULT_VISUAL
#define	DEFAULT_VISUAL	"/usr/ucb/vi"
#endif

static int		editMail_pid;


/* ARGSUSED */
XtEventHandler
warp_handler(w, client_data, event)
Widget	w;
caddr_t	client_data;
XEvent	*event;
{
 Widget Popup = XtNameToWidget(toplevel,"topBox.commandPanel.Send.popup");
 Widget    To = XtNameToWidget(Popup, "SubjCc.To");

 if (event->type == MapNotify) {
    XWarpPointer(XtDisplay(toplevel), None, XtWindow(To), 0,0,0,0, 15, 10);
    XtRemoveEventHandler(Popup, StructureNotifyMask, False, warp_handler, NULL);
   }
}


/*
** @(#)endEdits() - catch the signal when the edit child finishes
**                  (The idea for this was ``borrowed'' from xrn.)
*/
int
endEdits(signum)
int	signum;
{
 Widget		Popup;
 int		status;


 if (signum != SIGCHLD)
    return 1;

 if (editMail_pid != wait(&status))
    return 1;				/* the mail child could die too */

 (void) signal(SIGCHLD, SIG_DFL);	/* turn off the endEdits hook */

 Popup = XtNameToWidget(toplevel, "topBox.commandPanel.Send.popup");

 SetXY(Popup, XtNameToWidget(toplevel, "topBox.commandPanel"), 0, 0);

 XtPopup(Popup, XtGrabNone);

 XSync(XtDisplay(toplevel), False);

 XtAddEventHandler(Popup, StructureNotifyMask, False, warp_handler, NULL);

 return 1;
}


/*
** @(#)editMail() - edit a mail message using the preferred editor
**
** Support is now provided for declaring the editor command as an xmail
** resource, ala xrn.  If the resource ``xmail.editorCommand'' is defined,
** it must contain an `sprintf'-able format string that provides for the
** inclusion of both a display name and the name of the file to be edited.
** If the resource declaration is not included, or does not contain the two
** required percent-s (%s) formatting strings, xmail will use the VISUAL
** resource.  If VISUAL is used, try to accommodate those editors (emacs,
** xedit...) which start their own window in X11.  We know for a fact that
** vi and ed variants do not invoke windows.  We assume any other editor
** specification does.
*/
void
editMail()
{
 Display	*ad;
 char		*edit, *cp;
 char		cmd[BUFSIZ];


 bzero(cmd, BUFSIZ);

 ad  = XtDisplay(XtNameToWidget(toplevel, "topBox.statusWindow"));
/*
** If editorCommand resource exists, use it (format validated during initialize)
*/
 if (XMail.editorCommand)
    (void) sprintf(cmd, XMail.editorCommand, ad->display_name, tmpName);
 else {
    /*
    ** Otherwise, default to the VISUAL method of starting things
    */
    if ((edit = GetMailEnv("VISUAL")) == NULL)
         edit = XtNewString(DEFAULT_VISUAL);

    if ((cp = strrchr(edit, '/')) == NULL) cp = edit;
    else cp++;

    if (strcmp(cp, "ed") == 0 ||
        strcmp(cp,"red") == 0 ||
        strcmp(cp, "ex") == 0 ||
        strcmp(cp, "vi") == 0) {
       (void) sprintf(cmd, "xterm -display %s -name XMail -title 'Message entry' -e %s %s",
                     ad->display_name, edit, tmpName);
      } else (void) sprintf(cmd, "%s -display %s %s", edit, ad->display_name, tmpName);
    XtFree(edit);
   }

 editMail_pid = fork();

 switch (editMail_pid) {
    case -1:			/* fork failed ... */
         if (errno == ENOMEM)
            Bell("Not enough core for edits\n");
         else
            Bell("No more processes - no edits\n");
         break;

    case 0:			/* child starts the message entry session */
         (void) close(ConnectionNumber(XtDisplay(toplevel)));
         (void) execl("/bin/csh", "csh", "-f", "-c", cmd, 0);
         perror("editMail: Failed to start the text editor");
         (void) _exit(127);
         break;

    default:			/* parent waits for editing to conclude */
         (void) signal(SIGCHLD, endEdits);
         break;
   }
} /* editMail */


/*
** @(#)readMail() - callback invoked every time input is pending on mail fd
**
** Calls QueryMail() to read all available data from mail file descriptor,
** and passes output to parse() for analysis and appropriate action.
*/
/* ARGSUSED */
XtInputCallbackProc
readMail(client_data, source, id)
caddr_t   client_data;	/* unused */
int 	  *source;	/* unused */
XtInputId *id;		/* unused */
{
 parse(QueryMail(""));
} /* readMail */


/*
** @(#)sendMail() - Create the send window popup for message headers
*/
void
sendMail(parent)
Widget	parent;
{
 Arg		args[11];
 Widget		Popup, Layout, Box;
 Widget		lab1, lab2, lab3, lab4;
 Widget		To, Subject, Cclist, Bcc, Last;


 Popup = XtNameToWidget(parent, "popup");

 if (! Popup) {
    XtSetArg(args[0], XtNinput, True);
    XtSetArg(args[1], XtNwidth, XMail.shellWidth - 2);
    XtSetArg(args[2], XtNheight,
           XMail.borderWidth*3 + XMail.buttonHeight*5 + 44);
    Popup = XtCreatePopupShell("popup",transientShellWidgetClass,parent,args,3);

    XtSetArg(args[0], XtNdefaultDistance, 2);
    Layout = XtCreateManagedWidget("SubjCc", formWidgetClass, Popup, args, 1);

    XtSetArg(args[0], XtNfromVert, NULL);
    XtSetArg(args[1], XtNfromHoriz, NULL);
    XtSetArg(args[2], XtNlabel, "To:");
    XtSetArg(args[3], XtNborderWidth, 0);
    XtSetArg(args[4], XtNfont, XMail.buttonFont);
    XtSetArg(args[5], XtNheight, XMail.buttonHeight + XMail.borderWidth + 7);
    XtSetArg(args[6], XtNwidth, XMail.buttonWidth);
    XtSetArg(args[7], XtNjustify, XtJustifyLeft);
    XtSetArg(args[8], XtNinternalHeight, 0);
    XtSetArg(args[9], XtNinternalWidth, 1);
    lab1 = XtCreateManagedWidget("SubjCc", labelWidgetClass, Layout, args, 10);

    To = CreateInputWindow(Layout, "To");

    AddInfoHandler(To, SendMail_Info[0]);

    XtSetArg(args[0], XtNfromVert, NULL);
    XtSetArg(args[1], XtNfromHoriz, lab1);
    XtSetValues(To, args, 2);

    AddHelpText(To, To_Help);

    XtSetArg(args[0], XtNfromVert, lab1);
    XtSetArg(args[1], XtNfromHoriz, NULL);
    XtSetArg(args[2], XtNlabel, "Subject:");
    lab2 = XtCreateManagedWidget("SubjCc", labelWidgetClass, Layout, args, 10);

    Subject = CreateInputWindow(Layout, "Subject");

    AddInfoHandler(Subject, SendMail_Info[1]);

    XtSetArg(args[0], XtNfromVert, To);
    XtSetArg(args[1], XtNfromHoriz, lab2);
    XtSetValues(Subject, args, 2);

    AddHelpText(Subject, Subject_Help);

    XtSetArg(args[0], XtNfromVert, lab2);
    XtSetArg(args[1], XtNfromHoriz, NULL);
    XtSetArg(args[2], XtNlabel, "Cc:");
    lab3 = XtCreateManagedWidget("SubjCc", labelWidgetClass, Layout, args, 10);

    Cclist = CreateInputWindow(Layout, "Cc");

    AddInfoHandler(Cclist, SendMail_Info[2]);

    XtSetArg(args[0], XtNfromVert, Subject);
    XtSetArg(args[1], XtNfromHoriz, lab3);
    XtSetValues(Cclist, args, 2);

    AddHelpText(Cclist, Cc_Help);

    XtSetArg(args[0], XtNfromVert, lab3);
    XtSetArg(args[1], XtNfromHoriz, NULL);
    XtSetArg(args[2], XtNlabel, "Bcc:");
    lab4 = XtCreateManagedWidget("SubjCc", labelWidgetClass, Layout, args, 10);

    Bcc = CreateInputWindow(Layout, "Bcc");

    AddInfoHandler(Bcc, SendMail_Info[3]);

    XtSetArg(args[0], XtNfromVert, Cclist);
    XtSetArg(args[1], XtNfromHoriz, lab4);
    XtSetValues(Bcc, args, 2);

    AddHelpText(Bcc, Bcc_Help);

    XtRegisterGrabAction(SetAliases, True, ButtonPressMask | ButtonReleaseMask,
                         GrabModeAsync, GrabModeAsync);

    XtSetArg(args[0], XtNfont, XMail.buttonFont);
    XtSetArg(args[1], XtNheight, XMail.buttonHeight + XMail.borderWidth*2 + 4);
    XtSetArg(args[2], XtNwidth, XMail.shellWidth - 2);
    XtSetArg(args[3], XtNfromVert, lab4);
    XtSetArg(args[4], XtNfromHoriz, NULL);
    XtSetArg(args[5], XtNborderWidth, 0);
    XtSetArg(args[6], XtNresize, False);
    XtSetArg(args[7], XtNmin, args[1].value);
    XtSetArg(args[8], XtNmax, args[1].value);
    XtSetArg(args[9], XtNhSpace, 2);
    XtSetArg(args[10],XtNvSpace, 2);
    Box = XtCreateManagedWidget("Box", boxWidgetClass, Layout, args, 11);

    XtSetArg(args[1], XtNheight, XMail.buttonHeight);
    XtSetArg(args[2], XtNwidth,
    (((XMail.shellWidth - 2) / 6) - XMail.borderWidth * 2) - 4);

    lab1 = XtCreateManagedWidget("Autograph", commandWidgetClass, Box, args, 3);
    XtAddCallback(lab1, XtNcallback, (XtCallbackProc) Autograph, (caddr_t) "A");
    AddInfoHandler(lab1, Autograph_Info[0]);
    AddHelpText(lab1, Sign_Help);

    lab2 = XtCreateManagedWidget("autograph", commandWidgetClass, Box, args, 3);
    XtAddCallback(lab2, XtNcallback, (XtCallbackProc) Autograph, (caddr_t) "a");
    AddInfoHandler(lab2, Autograph_Info[1]);
    AddHelpText(lab2, sign_Help);

    lab1 = XtCreateManagedWidget("ReEdit", commandWidgetClass, Box, args, 3);
    XtAddCallback(lab1,XtNcallback,(XtCallbackProc) ReEdit, (caddr_t)"ReEdit");
    AddInfoHandler(lab1, Deliver_Info[1]);
    AddHelpText(lab1, ReEdit_Help);

    lab2 = XtCreateManagedWidget("Cancel", commandWidgetClass, Box, args, 3);
    XtAddCallback(lab2, XtNcallback, (XtCallbackProc) Done, (caddr_t) "cancel");
    AddInfoHandler(lab2, Deliver_Info[2]);
    AddHelpText(lab2, Cancel_Help);

    lab3 = XtCreateManagedWidget("Abort", commandWidgetClass, Box, args, 3);
    XtAddCallback(lab3, XtNcallback, (XtCallbackProc) Done, (caddr_t) "Cancel");
    AddInfoHandler(lab3, Deliver_Info[3]);
    AddHelpText(lab3, Abort_Help);

    Last = XtCreateManagedWidget("Deliver", commandWidgetClass, Box, args, 3);
    XtAddCallback(Last, XtNcallback, (XtCallbackProc) Done, (caddr_t)"Deliver");
    AddInfoHandler(Last, Deliver_Info[0]);
    AddHelpText(Last, Deliver_Help);
   }
} /* sendMail */


/*
** @(#)writeMail() - Write command s to the mail process.
*/
void
writeMail(s) 
char *s;
{
 (void) write(mail_fd, s, strlen(s));
} /* writeMail */
