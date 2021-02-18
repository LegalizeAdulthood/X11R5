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
 *  1. CheckInsert
 *  2. DeleteChar
 *  3. EraseIt:
 *  4. DoCmd:
 *  5. DoNothing:
 *  6. DoReply:
 *  7. DoSave:
 *  8. DoSelected:
 *  9. Folder:
 * 10. Iconify:
 * 11. MyNotify:
 * 12. NextField:
 * 13. PrintMsg:
 * 14. SetAliases:
 * 15. SetFolders:
 * 16. SetMenu:
 * 17. SetPopup:
 * 18. SetSelect: and
 * 19. ShowHelp.
 *
 * Author:  Michael C. Wagnitz - National Semiconductor Corporation
 *
 */


#include "global.h"
#include <ctype.h>
#include <sys/stat.h>
#include "xmailregexp.h"

#ifndef	S_ISDIR
#define	S_ISDIR(m)	(((m)&S_IFMT) == S_IFDIR)
#endif

#ifdef mips
#include <stdlib.h>
#endif

#ifdef USE_DIRENT
#include <dirent.h>
#else
#include <sys/dir.h>
#endif


/*
** @(#)str_compare() - compare function for the qsort of folder names
*/
str_compare(s1, s2)
char **s1, **s2;
{
 return(strcmp(*s1, *s2));
}


/*
** @(#)CheckInsert() - prevents the user from munging up the File: prompt.
** If the current insertion point is less than the minimum StartPos, move
** the insertion point to the StartPos.
*/
/* ARGSUSED */
XtActionProc
CheckInsert(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
 if (XawTextGetInsertionPoint(w) < StartPos)
     XawTextSetInsertionPoint(w, (XawTextPosition) StartPos);
} /* CheckInsert */


/*
** @(#)EraseIt() - Delete the specified portion of text.
*/
void
EraseIt(w, i, pos)
Widget	w;
XawTextPosition	i, pos;
{
 XawTextBlock	textblock;

 textblock.firstPos = 0;
 textblock.length   = 0;
 textblock.ptr      = "";

 XawTextReplace(w, i, pos, &textblock);

 XawTextSetInsertionPoint(w, (XawTextPosition) i);
} /* EraseIt */


/*
** @(#)DeleteChar() - prevents the user from deleting past the File: prompt.
** If the current insertion point is greater than the minimum StartPos, then
** delete the previous character.
*/
/* ARGSUSED */
XtActionProc
DeleteChar(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
 XawTextPosition pos, i;

 pos = XawTextGetInsertionPoint(w);
 if (pos > StartPos) {
    i = pos - 1;
    EraseIt(w, i, pos);
   }
} /* DeleteChar */


/*
** @(#)DeleteLine() - Deletes the entire current line from the file window.
**                    Simulates the action of the KILL character (ctrl-U).
*/
/* ARGSUSED */
XtActionProc
DeleteLine(w, event, params, num_params)
Widget			w;
XEvent			*event;
String			*params;
Cardinal		*num_params;
{
 Arg			args[1];
 String			FBuf;
 XawTextPosition	pos, i;


 pos = XawTextGetInsertionPoint(w);
 if (pos > StartPos) {
    XtSetArg(args[0], XtNstring, &FBuf);
    XtGetValues(w, args, ONE);

    for (i = pos; i > StartPos && FBuf[i - 1] != '\n'; i--);

    EraseIt(w, i, pos);
   }
} /* DeleteLine */


/*
** @(#)DeleteWord() - Erases the preceding word in the fileWindow buffer.
** Simulates the action of the WERASE character (ctrl-W).
*/
/* ARGSUSED */
XtActionProc
DeleteWord(w, event, params, num_params)
Widget			w;
XEvent			*event;
String			*params;
Cardinal		*num_params;
{
 Arg			args[1];
 String			FBuf;
 XawTextPosition	pos, i;

 pos = XawTextGetInsertionPoint(w);
 if (pos > StartPos) {
    XtSetArg(args[0], XtNstring, &FBuf);
    XtGetValues(w, args, ONE);

    for (i = pos; i > StartPos && FBuf[i - 1] == ' '; i--);
    for (; i > StartPos && FBuf[i - 1] != ' '; i--);

    EraseIt(w, i, pos);
   }
} /* DeleteWord */


/* ARGSUSED */
/*
** @(#)DoCmd() - send (multi-word) command to mail
*/
XtActionProc
DoCmd(w, event, params, num_params)
Widget		w;			/* unused */
XEvent		*event;			/* unused */
String		*params;
Cardinal	*num_params;
{
 char		buf[BUFSIZ];
 int		i;


 if (strcmp(params[0], "drop") == 0) {
    SetCursor(NORMAL);
    DropIt(w, *params, NULL);
   } else {
    buf[0] = '\0';
    for (i = 0; i < *num_params; i++) {
        if (i > 0)
           (void) strcat(buf, " ");
        (void) strcat(buf, params[i]);
       }
    DoIt(w, buf, NULL);			/* Let DoIt (CallbackProc) do work */
   }
} /* DoCmd */


/* ARGSUSED */
/*
** @(#)DoNothing() - dummy action for unwanted button(s)
*/
XtActionProc
DoNothing(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
 return;
} /* DoNothing */


/*
** @(#)DoReply() - call Reply() CallbackProc from an ActionProc
*/
/* ARGSUSED */
XtActionProc
DoReply(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
 Reply(w, *params, NULL);
} /* DoReply */


/*
** @(#)DoSave() - call Save() CallbackProc from an ActionProc
*/
/* ARGSUSED */
XtActionProc
DoSave(w, event, params, num_params)
Widget		w;
XEvent		*event;
String		*params;
Cardinal	*num_params;
{
 Save(w, *params, NULL);
} /* DoSave */


/* ARGSUSED */
/*
** @(#)DoSelected() - execute specified command using selected message number
*/
XtActionProc
DoSelected(w, event, params, num_params)
Widget		w;
XEvent		*event;
String		*params;
Cardinal	*num_params;
{
 int		num = 0;


 if (! mailpid) Bell("No current mail connection\n");
 else if (num_params) {
         SetCursor(WATCH);
         if (*params[0] != 'n' && *params[0] != '-')
            num = SelectionNumber(*params[0] == 'u');

         if (num) (void) sprintf(Command, "%s %d\n", params[0], num);
         else (void) sprintf(Command, "%s\n", params[0]);

         writeMail(Command);
        }
} /* DoSelected */


/*
** @(#)Folder() - change folders - must have specified folder name or error
*/
/* ARGSUSED */
XtActionProc
Folder(w, event, params, num_params)
Widget		w;
XEvent		*event;
String		*params;
Cardinal	*num_params;
{
 Arg		args[1];
 String		p, buf;
 Widget		fw = XtNameToWidget(toplevel, "topBox.commandPanel.fileWindow");
 XawTextPosition pos;


 pos = TextGetLastPos(fw);
 if (pos - StartPos <= 0) {
    Bell("Specify a folder name (in the [File: ] box) first\n");
   } else {
    XtSetArg(args[0], XtNstring, &buf);
    XtGetValues(fw, args, ONE);

    if (buf[pos] != '\0') buf[pos] = '\0';
    p = XtNewString(&buf[StartPos]);
    if (mailpid) {			/* check for commit of any changes */
       XtSetArg(args[0], XtNlabel, &buf);
       XtGetValues(XtNameToWidget(toplevel, "topBox.titleBar.title"), args, 1);

       if (strcmp(&buf[strlen(buf) - 7], "deleted") ||
           Confirm("COMMIT all changes to this folder")) {
          (void) sprintf(Command, "file %s\n", p);
          writeMail(Command);
          XtFree(p);
          SetCursor(WATCH);		/* will be reset by parser routine */
         }
      } else {
       /*
       ** We must first re-establish contact with the mail utility.
       ** This time, we indicate a specific mail folder to process.
       */
       XMail.MFileName = p;
       if (mailargc > 2 && strcmp(mailargv[mailargc - 2], "-f") == 0) {
          mailargv[mailargc - 1] = XMail.MFileName;
         } else {
          mailargv[mailargc++] = "-f";
          mailargv[mailargc++] = XMail.MFileName;
          mailargv[mailargc] = NULL;	/* list MUST be NULL terminated */
         }
       callMail(mailargv);
       (void) strcpy(Command, "Start");	/* Let em know we've re-started */
       In_Bogus_Mail_File = False;	/* reset this so titleBar will chg */
       SetCursor(WATCH);		/* will be reset by parser routine */
      }
   }
} /* Folder */


/* ARGSUSED */
/*
** @(#)Iconify() - request window iconification
*/
XtActionProc
Iconify(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
 Display		*disp = XtDisplay(toplevel);


 if (! XIconifyWindow(disp, XtWindow(toplevel), DefaultScreen(disp)))
    XBell(disp, 33);
}


/* ARGSUSED */
/*
** @(#)MyNotify() - call widget callbacks with passed parameter
*/
XtActionProc
MyNotify(w, event, params, num_params)
Widget		w;
XEvent		*event;
String		*params;
Cardinal	*num_params;
{
 XtCallCallbacks(w, XtNcallback, *params);
} /* MyNotify */


/*
** @(#)NextField() - warps pointer to next field in the Send command window.
** This allows carriage return to focus attention on the next data requirement.
*/
/* ARGSUSED */
XtActionProc
NextField(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
 String	name;
 Widget	shell;


 if (strcmp(w->core.name, "Cc") == 0)
    name = "Bcc"; else
 if (strcmp(w->core.name, "Bcc") == 0)
    name = "To"; else
 if (strcmp(w->core.name, "To") == 0)
    name = "Subject"; else
    name = "Cc";

 if ((shell = XtNameToWidget(XtParent(w), name)) != NULL)
    XWarpPointer(XtDisplay(toplevel), None, XtWindow(shell), 0,0,0,0, 15, 10);

} /* NextField */


/*
** @(#)PrintMsg() - sends the selected mail message to the system printer
*/
/* ARGSUSED */
XtActionProc
PrintMsg(w, event, params, num_params)
Widget		w;
XEvent		*event;
String		*params;
Cardinal	*num_params;
{
 char		*cp, *sp, buf[BUFSIZ];
 int		k, num;


 if (! mailpid) Bell("No current mail connection\n");
 else {
    num = SelectionNumber(False);	/* no current message returns zero */
    if (! num) Bell("No messages to print.\n");
    else {
       SetCursor(WATCH);		/* will be reset by parser routine */
       cp = QueryMail("|");
       if (strncmp(cp, "Unknown", 7) == 0) {	/* Then this is NOT Sun mail */
          XtFree(cp);
          (void) sprintf(buf, "copy %d %s", num, tmpName);
          cp = QueryMail(buf);			/* write message to a file */
          sp = strrchr(cp, '/');		/* get the size of the file */
          (void) sscanf(sp, "/%d", &k);
          XtFree(cp);
          cp = GetMailEnv("printmail");
          if (! cp) {
             (void) sprintf(buf, "lpr -p -r %s", tmpName);
            } else {
             (void) sprintf(buf, cp, tmpName);
             XtFree(cp);
            }
          (void) system(buf);
          (void) sprintf(buf, "Message %d sent to printer -- %d bytes\n",num,k);
          Bell(buf);				/* Notify user of action */
          Command[0] = '\0';			/* Toss any previous value */
         } else {
          XtFree(cp);
          cp = GetMailEnv("printmail");
          if (! cp) {
             (void) sprintf(Command, "| %d \"lpr -p\"\n", num);
            } else {
             (void) sprintf(Command, "| %d \"%s\"\n", num, cp);
             XtFree(cp);
            }
          writeMail(Command);
         }
      }
   }
} /* PrintMsg */


/*
** @(#)Quit() - call DoQuit() CallbackProc from the Quit ActionProc
*/
/* ARGSUSED */
XtActionProc
Quit(w, event, params, num_params)
Widget		w;	/* unused */
XEvent		*event;	/* unused */
String		*params;
Cardinal	*num_params;
{
 if (event->type == ClientMessage &&
    event->xclient.data.l[0] != wmDeleteWindow) {
    XBell (XtDisplay(w), 0);
    return;
   }

 DoQuit(w, *params, NULL);
} /* Quit */


/*
** @(#)SetAliases() - create a menu list of alias names
*/
/* ARGSUSED */
XtActionProc
SetAliases(w, event, params, num_params)
Widget		w;
XEvent		*event;
String		*params;
Cardinal	*num_params;
{
 int		k;
 Arg		args[7];
 Cardinal	columns, j, n;
 Widget		bw, lw, popup, above, left;

 static String l_Trans = "<Btn3Up>:	MenuPopdown(aliasList)";

 static String fl_trans = "<Enter>: set() \n\
			   <Leave>: unset() \n\
			   <Btn3Up>: notify()";

 static XtCallbackRec fl_callbacks[] = {
        { (XtCallbackProc) GetAliasName, NULL },
        { NULL,          NULL }
       };


 popup = XtNameToWidget(w, "aliasList");

 if (! popup) {
    (void) alias(NULL);			/* ensure our aliases list is set */
/*
** determine proper label width by finding longest name length
*/
    j = k = columns = 0;
    for (n = 0; aliases[n]; n++)
        if ((columns = strlen(aliases[n]->name)) > k) {
           k = columns;
           j = n;
          }

    if (k > 0) {				/* some alias names exist */
       /*
       ** Make equal width command buttons which contain the alias names
       */
       XtSetArg(args[0], XtNtranslations, XtParseTranslationTable(l_Trans));
       popup = XtCreatePopupShell("aliasList",overrideShellWidgetClass,w,args,1);

       XtSetArg(args[0], XtNdefaultDistance, (XtArgVal) 1);
       lw = XtCreateManagedWidget("table", formWidgetClass, popup, args, 1);

       XtSetArg(args[0], XtNwidth, XTextWidth(XMail.buttonFont, aliases[j]->name, k) + 14);
       XtSetArg(args[1], XtNfont, XMail.buttonFont);
       XtSetArg(args[2], XtNtranslations, XtParseTranslationTable(fl_trans));
       XtSetArg(args[3], XtNcallback, fl_callbacks);

       columns = XMail.shellWidth / args[0].value;	/* no wider than this */
       while (columns * columns > n) columns--;	/* try to shape into a square */

       k = 0;
       above = left = NULL;
       for (n = 0; aliases[n]; n++) {
           XtSetArg(args[4], XtNlabel, aliases[n]->name);
           XtSetArg(args[5], XtNfromHoriz, left);			j = 6;
           if (! left)			/* also anchor to the button above */
              XtSetArg(args[j], XtNfromVert, above);			j++;

           bw = XtCreateManagedWidget("entry", commandWidgetClass, lw, args, j);
           AddInfoHandler(bw, "Copy this alias to the current header field");

           if (! left) above = bw;
           left = bw;

           if (++k % columns == 0) {	/* make a box # columns wide */
              k = 0;
              left = NULL;		/* start next row at left edge of box */
             }
          } /* end - for all the aliases in the list */
      }	/* end - if some alias names exist */
   } /* end - if popup did not previously exist */
/*
** If the menu exists, set its x,y coordinates and pop it up
*/
 if (! popup)
    XBell(XtDisplay(toplevel), 33);
 else {
    SetXY(popup, w, XMail.menuX, XMail.buttonHeight / 2);

    XtPopupSpringLoaded(popup);
   }
} /* SetAliases */


/*
** @(#)SetFolders() - create a menu list of folder names
*/
/* ARGSUSED */
XtActionProc
SetFolders(w, event, params, num_params)
Widget		w;
XEvent		*event;
String		*params;
Cardinal	*num_params;
{
 Arg		args[8];
 DIR		*dirp = NULL;
 Widget		lw, above, this_one, to_left, popup;
 struct	stat	st_buf;
 int		m, n, foldername_length, total;
 char		trans[BUFSIZ], tmp[BUFSIZ], foldir[BUFSIZ];
 char		*GetMailEnv(), *getenv(), *cp, *List = NULL;
 char		**ftbl = NULL;


#ifdef USE_DIRENT
 struct dirent	*dp;
#else
 struct direct	*dp;
#endif

 static String dir_Trans = "\
			 <Btn1Down>: SetDirectory(%s,%s,%s)";

 static String  l_Trans = "<Btn3Up>: MenuPopdown(popupList)";

 static String fl_trans = "<Enter>: set() \n\
        		   <Leave>: unset() \n\
        		   <Btn3Up>: notify() MenuPopdown(popupList)";

 static XtCallbackRec fl_callbacks[] = {
        { (XtCallbackProc) GetFolderName, NULL },
        { NULL,          NULL }
       };


 popup = XtNameToWidget(w, "popupList");

 if (! popup) {
    XtSetSensitive(w, False);
    XFlush(XtDisplay(toplevel));
    cp = GetMailEnv("folder");			/* returns NULL if none */
    if (cp && strlen(cp)) {
       /*
	* Don't prepend HOME if it starts with a slash or a .
	*/
       if (strchr("/.", *cp))
	   (void) strcpy(foldir, cp);
       else
	   (void) sprintf(foldir, "%s/%s", getenv("HOME"), cp);
       XtFree((char *)cp);
       /*
	* Make sure it ends with (only one) slash
	*/
       if (LASTCH(foldir) != '/')
          (void) strcat(foldir, "/");
      } else
       foldir[0] = '\0';	/* If no folder variable, then no folders */
    foldername_length = strlen(foldir);

    if (*foldir) {				/* if folder variable exists */
       if (mailpid) {				/* try to get list from mail */
          List = QueryMail("folders");		/* returns a string of names */
         } else {
          if (dirp = opendir((char *)foldir)) {	/* and folder is readable... */
             n = BUFSIZ;
             List = XtMalloc((unsigned) n);	/* start with a BUFSIZ block */
             List[0] = '\0';
             for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp))
                 if (dp->d_name[0] != '.') {	/* skip parent and dot files */
                    if (strlen(List) + strlen(dp->d_name) + 2 > n) {
                       n += BUFSIZ;
                       List = XtRealloc(List, n);
                      }
                    if (List[0]) (void) strcat(List, " ");
                    (void) strcat(List, dp->d_name);
                   }
             List = XtRealloc(List, strlen(List) + 1);
             (void) closedir(dirp);
            } /* end - if folder directory is readable */
         } /* end - if mail process is running */
      } /* end - if a folder value exists */

    if (List)				/* could be null if no current mail */
       if (O_BELL == match(output_pattern, List)) {
          (void) strcat(List, "\n");
          Bell(List);
          XtFree((char *)List);
          List = NULL;
         }

    if (List && *List) {			/* if some folder names exist */
       for (total = 1, cp = List; *cp; cp++)	/* count number of names */
           if (strchr(" \t\n", *cp)) {
              total++;			/* (increase the folder name count) */
              while (*(cp+1) && strchr(" \t\n", *(cp+1))) cp++;
             }
       ftbl = (char **) XtMalloc((unsigned) total * sizeof(char *));
       ftbl[0] = "";
       m = n = 0;
       for (cp = List; *cp; cp++) {	/* stuff word pointers into a table */
           while (*(cp+n) && ! strchr(" \t\n", *(cp+n))) n++;
           if (*(cp+n)) *(cp+n) = '\0';	/* mark off the end of this name */
           ftbl[m++] = cp;		/* save this pointer in our table */
           cp += n;			/* find start of next folder name */
           n = 0;
           while (*(cp+1) && strchr(" \t\n", *(cp+1))) cp++;
          }
       /*
       ** (quick) sort our table into ascending alphabetical order
       */
       if (total > 1)
          qsort((char *)ftbl, total, sizeof(char *), str_compare);
       /*
       ** Next, determine label width of longest word in our table
       */
       for (m = n = 0; n < total; n++) {
           if (strlen(ftbl[n]) > strlen(ftbl[m])) m = n;
          }
       /*
       ** Now, make equal width command buttons which contain the folder names
       */
       XtSetArg(args[0], XtNtranslations, XtParseTranslationTable(l_Trans));
       popup = XtCreatePopupShell("popupList",overrideShellWidgetClass,w,args,1);

       XtSetArg(args[0], XtNdefaultDistance, 1);
       lw = XtCreateManagedWidget("list", formWidgetClass, popup, args, 1);

       n = XTextWidth(XMail.buttonFont, ftbl[m], strlen(ftbl[m]));

       XtSetArg(args[0], XtNwidth, n + 20);
       XtSetArg(args[1], XtNfont, XMail.buttonFont);
       XtSetArg(args[2], XtNtranslations, XtParseTranslationTable(fl_trans));
       XtSetArg(args[3], XtNcallback, fl_callbacks);

       above = this_one = to_left = NULL;
       for (m = 0; m < total; m++) {
           /*
           ** Watch out for special case of only one folder which isn't
           ** really a directory... (i.e. foldir and folder are same name)
           */
           if (strncmp(ftbl[m], foldir, foldername_length - 1))
              (void) strcpy(tmp, "+");	/* true folders start with a 'plus' */
           (void) strcat(tmp, ftbl[m]);
/*
** If this 'folder' is really a directory, mark it with a trailing slash '/'
*/
           foldir[foldername_length] = '\0';
           (void) strcat(foldir, ftbl[m]);
           if (access(foldir, R_OK) == 0 &&	/* IF exists and is readable */
              stat(foldir, &st_buf) == 0 &&	/* got the status okay, and  */
              S_ISDIR(st_buf.st_mode))		/* it IS a directory... then */
              (void) strcat(tmp, "/");
           XtSetArg(args[4], XtNlabel, tmp);
           XtSetArg(args[5], XtNfromHoriz, to_left);			n = 6;
           if (! to_left) XtSetArg(args[n], XtNfromVert, above);	n++;
   
           this_one = XtCreateManagedWidget("listbutton", commandWidgetClass, lw, args, n);
           if (to_left == NULL) above = this_one;
           to_left = this_one;

           if ((m+1) % 5 == 0)
              to_left = NULL;
/*
** If this 'folder' is a directory, add a button to popup a menu of filenames.
*/
           if (LASTCH(tmp) == '/') {
              (void) sprintf(trans, dir_Trans, &tmp[1], foldir, "0");
              XtOverrideTranslations(this_one, XtParseTranslationTable(trans));
              AddInfoHandler(this_one, Folder_Info[2]);
             } else
              AddInfoHandler(this_one, Folder_Info[1]);
          } /* end - for each folder name in the table of folders */
      }	/* end - if some folder names exist */
    XtSetSensitive(w, True);
   } /* end - if no popup previously existed */
/*
** If no folders to display, tell the user why
*/
 if (! popup) {
    if (! *foldir)
       Bell("No value set for \"folder\"\n");
    else if (dirp || List) {
            if (! mailpid || ! *List)
               Bell("No mail folders exist\n");
            else {
               foldir[foldername_length - 1] = '\0';
               (void) sprintf(tmp, "%s not found\n", foldir);
               Bell(tmp);
              }
           }
   } else {
    /*
    ** If folders menu exists, pop it up, after setting x,y coordinates
    */
    if (! XtIsRealized(popup))
       XtRealizeWidget(popup);
    /*
    ** If folder list is small enough, anchor it to
    ** the folder button instead of the commandPanel
    */
    if (popup->core.width + (3 * (XMail.buttonWidth + 12)) <= XMail.shellWidth)
       SetXY(popup, w, XMail.menuX, XMail.buttonHeight / 2);
    else
       SetXY(popup, XtNameToWidget(toplevel, "topBox.commandPanel"),
             XMail.menuX, XMail.commandHeight / 2);

    XtPopupSpringLoaded(popup);
   }

 if (List)
    XtFree((char *)List);

 if (ftbl)
    XtFree((char *)ftbl);
} /* SetFolders */



/* 
** @(#)SetMenu() - create a menu for toggling selected mail options
*/
/* ARGSUSED */
XtActionProc
SetMenu(parent, event, params, num_params)
Widget		parent;
XEvent		*event;		/* unused */
String		*params;	/* unused */
Cardinal	*num_params;	/* unused */
{
 Arg	 	args[6];
 Widget		menu, layout, previous, next;
 char		*c, *info, label[BUFSIZ], name[BUFSIZ];		
 int		indx;

 static String b_Trans =
			"<EnterWindow>:	set() \n\
	 		 <LeaveWindow>: reset() \n\
	 		      <Btn3Up>:	notify() unset()";

 static String m_Trans =
			"<Btn3Up>:	MenuPopdown(set_menu)";

 static String list[] = { "alwaysignore", "autoprint", "hold", "expert", NULL };

 static String set_info[] = {
 "Skip 'ignore'd header fields everywhere, not just during a print or read",
 "Enable automatic printing of messages after delete and undelete commands",
 "Preserve messages in the system mailbox after they have been read",
 "Don't ask for confirmation when commiting changes or aborting a new letter",
 NULL
 };

 static String unset_info[] = {
 "Skip 'ignore'd header fields only when doing a print or read command",
 "Disable automatic printing of messages after delete and undelete commands",
 "Move system mailbox messages to the mbox save file after you read them",
 "Ask for confirmation before commiting any changes or aborting a new letter",
 NULL
 };


 menu = XtNameToWidget(parent, "set_menu");

 if (! menu || menu->core.being_destroyed) {
    XtSetArg(args[0], XtNtranslations, XtParseTranslationTable(m_Trans));
    menu = XtCreatePopupShell("set_menu",overrideShellWidgetClass,parent,args,1);

    XtSetArg(args[0], XtNdefaultDistance, (XtArgVal) 1);
    layout = XtCreateManagedWidget("menu", formWidgetClass, menu, args, 1);
/*
** create the menu buttons
*/
    previous = NULL;
    XtSetArg(args[0], XtNwidth, figureWidth(XMail.buttonFont) * 18 + 12);
    XtSetArg(args[1], XtNfont, XMail.buttonFont);
    XtSetArg(args[2], XtNjustify, XtJustifyLeft);
    XtSetArg(args[3], XtNtranslations, XtParseTranslationTable(b_Trans));
    for (indx = 0; list[indx] != NULL; indx++) {
        info = set_info[indx];
        (void) strcpy(label, "set ");
        if (strcmp(list[indx], "expert") == 0) {
           if (XMail.expert) {
              info = unset_info[indx];
              (void) strcat(label, "no");
             }
          } else {
           if ((c = GetMailEnv(list[indx])) != NULL) {
              info = unset_info[indx];
              (void) strcat(label, "no");
              XtFree(c);
             }
          }
        (void) strcat(label, list[indx]);	/* set window name by label */
        (void) strcpy(name, &label[4]);
        XtSetArg(args[4], XtNlabel, label);
        XtSetArg(args[5], XtNfromVert, previous);
        next = XtCreateManagedWidget(name, commandWidgetClass, layout, args, 6);
        XtAddCallback(next, XtNcallback, (XtCallbackProc) DoSet, NULL);
        AddInfoHandler(next, info);
        previous = next;
       }
   } /* end - menu creation */
 SetXY(menu, parent, XMail.menuX, XMail.buttonHeight / 2);
} /* SetMenu */


/*
** @(#)SetPopup() - place named popup at menuX, menuY relative to Widget w.
*/
/* ARGSUSED */
XtActionProc
SetPopup(w, event, params, num_params)
Widget w;
XEvent *event; /* unused */
String *params;
Cardinal *num_params;
{
 Widget		shell;
 String		p;


 if (*num_params == 0)
    XtError("xmail had no name parameter passed to SetPopup()");

 p = params[0];

 if ((shell = XtNameToWidget(w, p)) == NULL)
    XtError("xmail shell name passed to SetPopup() not found in list");

 SetXY(shell, w, XMail.menuX, XMail.menuY);
} /* SetPopup */


/* ARGSUSED */
/*
** @(#)SetSelect() - flag the index number of the selected message
*/
XtActionProc
SetSelect(w, event, params, num_params)
Widget w;		/* unused */
XEvent *event;		/* unused */
String *params;		/* unused */
Cardinal *num_params;	/* unused */
{
 markIndex(">");
} /* SetSelect */


/*
** @(#)ShowHelp() - set named string source as text for and popup help window.
*/
/* ARGSUSED */
XtActionProc
ShowHelp(w, event, params, num_params)
Widget		w;
XEvent		*event;
String		*params;			/* unused */
Cardinal	*num_params;			/* unused */
{
 String		name;
 Widget		tb;
 int		x;


 name = w->core.name;
 if (strcmp(name, "text") == 0 && event->type == KeyPress)
    name = "text2";

 for (x = 0; x < HelpList.indx; x++)
     if (strcmp(name, HelpList.name[x]) == 0) {
        tb = XtNameToWidget(toplevel, "topBox");
        XawTextSetSource(XtNameToWidget(tb, "help.helpWindow"),
                         HelpList.text[x], (XawTextPosition) 0);

        SetXY(XtNameToWidget(tb, "help"),
              XtNameToWidget(tb, "textWindow.text"),
              XMail.helpX, XMail.helpY);

        XtPopup(XtNameToWidget(tb, "help"), XtGrabNone);
        break;
       }
} /* ShowHelp */
