/*
 * xmail - X window system interface to the mail program
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
 * Author:  Michael C. Wagnitz - National Semiconductor Corporation
 *
 */


#include "global.h"
#include <sys/stat.h>

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
** @(#)SetDirectory() - Create popup list of directory folder names
*/
/* ARGSUSED */
XtActionProc
SetDirectory(w, event, params, num_params)
Widget		w;
XEvent		*event;
String		*params;
Cardinal	*num_params;
{
 Arg		args[7];
 Cardinal	label_width, path_length, n, depth, x;
 int		total;
 DIR		*dirp;
 String		name, path;
 Widget		menu, layout, bw, above, to_left;
 char		*s, *List, **ftbl, trans[BUFSIZ], tmp[BUFSIZ];
 struct	stat	st_buf;

#ifdef USE_DIRENT
 struct dirent	*dp;
#else
 struct direct	*dp;
#endif

 static String dir_Trans =
	"<Btn1Down>: SetDirectory(%s, %s, %d)";

 static String m_Trans = "<LeaveWindow>:	MenuPopdown(%s)";

 static String b_Trans = "<EnterWindow>:	set() \n\
			  <LeaveWindow>:	unset() \n\
			  <Btn3Up>:		MyNotify(%d) MenuPopdown(%s)";

 static XtCallbackRec callbacks[] = {
        { (XtCallbackProc) GetFolderName, NULL },
        { NULL,          NULL }
       };


 name = params[0];
 path = params[1];
 path_length = strlen(path);
 (void) sscanf(params[2], "%d", &depth);
 depth++;
 label_width = 0;

 for (s = name; *s; s++)
     /*
     ** To avoid problems later on with XtNameToWidget, don't create a
     ** menu if the name contains either a dot (.) or an asterisk (*).
     */
     if (strchr(".*", *s)) break;

 if (! (*s && XtNameToWidget(w, name))) {
    SetCursor(WATCH);
    (void) sprintf(trans, m_Trans, name);
    XtSetArg(args[0], XtNtranslations, XtParseTranslationTable(trans));
    menu = XtCreatePopupShell(name, overrideShellWidgetClass, w, args, ONE);

    XtSetArg(args[0], XtNdefaultDistance, 1);
    layout = XtCreateManagedWidget("menu", formWidgetClass, menu, args, ONE);

    if ((dirp = opendir(path)) == NULL)
       XtError("xmail cannot access passed directory name in SetDirectory()");
/*
** Copy folder names into a list
*/
    n = BUFSIZ;
    List = XtMalloc((unsigned) n);		/* start with a BUFSIZ block */
    List[0] = '\0';
    for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp))
        if (dp->d_name[0] != '.') {		/* skip parent and dot files */
           if (strlen(List) + strlen(dp->d_name) + 2 > n) {
              n += BUFSIZ;
              List = XtRealloc(List, (unsigned) n);
             }
           if (*List) (void) strcat(List, " ");
           (void) strcat(List, dp->d_name);
          }
    List = XtRealloc(List, (unsigned) strlen(List) + 1);
    (void) closedir(dirp);
/*
** Store pointers to folder names in a table array for sorting
*/
    if (List[0]) {			/* if directory isn't empty... */
       for (total=1, s=List; *s; s++)	/* count number of names in the List */
           if (*s == ' ') total++;	/* (increase the folder name count) */
       ftbl = (char **) XtMalloc((unsigned) total * sizeof(char *));
       x = n = 0;
       for (s = List; *s; s++) {	/* stuff word pointers into a table */
           while (*(s+n) && *(s+n) != ' ') n++;
           if (*(s+n)) *(s+n) = '\0';	/* mark off the end of this name */
           ftbl[x++] = s;		/* save this pointer in our table */
           s += n;			/* find start of next folder name */
           n  = 0;
          }
/*
** (quick) sort our table into ascending alphabetical order
*/
       qsort((char *)ftbl, total, sizeof(char *), str_compare);
/*
** Next, determine proper label width by finding longest word in our table
*/
       for (x = n = 0; n < total; n++) {
           if (strlen(ftbl[n]) > strlen(ftbl[x])) x = n;
          }

       label_width = XTextWidth(XMail.buttonFont, ftbl[x], strlen(ftbl[x]));
      }

    if (label_width) {
       (void) sprintf(trans, b_Trans, depth, name);

       XtSetArg(args[0], XtNwidth, label_width + 12);
       XtSetArg(args[1], XtNfont, XMail.buttonFont);
       XtSetArg(args[2], XtNcallback, callbacks);
       XtSetArg(args[3], XtNtranslations, XtParseTranslationTable(trans));
/*
** create the menu buttons
*/
       bw = above = to_left = NULL;
       for (x = 0; x < total; x++) {
           s = XtMalloc((unsigned) path_length + strlen(ftbl[x]) + 2);
           (void) sprintf(s, "%s/%s", path, ftbl[x]);
           (void) strcpy(tmp, ftbl[x]);
/*
** If this folder file is also a directory, add a trailing slash '/'
*/
           if (access(s, R_OK) == 0 &&		/* IF exists and is readable */
              stat(s, &st_buf) == 0 &&		/* got the status okay, and  */
              S_ISDIR(st_buf.st_mode))		/* it IS a directory... then */
              (void) strcat(tmp, "/");
           XtSetArg(args[4], XtNlabel, tmp);
           XtSetArg(args[5], XtNfromHoriz, to_left);			n = 6;
           if (! to_left) XtSetArg(args[n], XtNfromVert, above);	n++;
           bw = XtCreateManagedWidget("menubutton",commandWidgetClass,layout,args,n);
           if (to_left == NULL) above = bw;
           to_left = bw;
           if ((x+1) % 3 == 0)			/* make box three items wide */
              to_left = NULL;
/*
** If this 'folder' is a directory, add a button popup menu of its files.
*/
           if (LASTCH(tmp) == '/') {
              (void) sprintf(trans, dir_Trans, tmp, s, depth);
              XtOverrideTranslations(bw, XtParseTranslationTable(trans));
              AddInfoHandler(bw, Folder_Info[2]);
             } else
              AddInfoHandler(bw, Folder_Info[1]);
           XtFree(s);
          } /* end - for each name in the folder table */
      } /* end - if there exists at least one folder name */
/*
** If no buttons were created for this menu, destroy the widget.
*/
    if (! label_width)
       XtDestroyWidget(menu);
    SetCursor(NORMAL);
   } /* end - if menu had not yet been created */
/*
** If menu exists, pop it up, after setting x,y coordinates
*/
 menu = XtNameToWidget(w, name);

 if (! menu || menu->core.being_destroyed)
    XBell(XtDisplay(toplevel), 33);
 else {
    SetPopup(w, event, params, num_params);

    XtSetArg(args[0], XtNy, NULL);
    XtGetValues(menu, args, ONE);
    args[0].value -= XMail.menuY;	/* don't offset menu below cursor */
    XtSetValues(menu, args, ONE);

    XtPopup(menu, XtGrabNone);
   }
} /* SetDirectory */
