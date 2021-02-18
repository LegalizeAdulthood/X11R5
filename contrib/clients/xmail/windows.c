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
 * 1. AddHelpText:
 * 2. AddInfoHandler:
 * 3. AddMenuWindow:
 * 4. AddSetMenu:
 * 5. CreateFolderButton: and
 * 6. CreateInputWindow.
 *
 * Author:  Michael C. Wagnitz - National Semiconductor Corporation
 *
 */


#include "global.h"
#include "MailwatchP.h"
#include "patchlevel.h"
#include "national.bm"

#ifdef XPM
#define	mail_width	64
#define	mail_height	64
#else
#include "icon.mail"
#endif

/*
** @(#)AddHelpText() - add help text to string resources for help widget
*/
void
AddHelpText(parent, help_text)
Widget parent;
String	help_text;
{
 Arg		args[2];
 String		name;

 static String	p_Trans = "#override \
			!Shift<Btn2Down>: ShowHelp()\n\
				<Btn2Up>: MenuPopdown(help)";

 XtOverrideTranslations(parent, XtParseTranslationTable(p_Trans));

 HelpList.name[HelpList.indx] = name = parent->core.name;

 XtSetArg(args[0], XtNstring, (XtArgVal) help_text);
 XtSetArg(args[1], XtNlength, (XtArgVal) strlen(help_text));

 HelpList.text[HelpList.indx] = XtCreateWidget(name, asciiSrcObjectClass,
                           XtNameToWidget(toplevel, "topBox.help.helpWindow"),
                           args, 2);
 HelpList.indx += 1;
} /* AddHelpText */


void
AddInfoHandler(widget, message)
Widget widget;
char *message;
{
 if (XMail.Show_Info)
    XtAddEventHandler(widget,
		  (EventMask) (EnterWindowMask|LeaveWindowMask),
		  False,
		  info_handler,
		  (caddr_t) message);
}


/* 
** @(#)AddMenuWindow() - create menu popup for the specified widget
*/
void
AddMenuWindow(parent, action, list, info_text)
Widget		parent;
String		action;
menuList_p	*list;
String		*info_text;
{
 Arg	 	args[6];
 Widget		menu, layout, previous, next;
 int		j, k, n, indx;
 char		tag[BUFSIZ], trans[BUFSIZ], *c;		

 static String p_Trans =
	"<Btn3Down>:	SetPopup(%s) MenuPopup(%s)";

 static String b_Trans =
	"<EnterWindow>:	highlight() set() \n\
	 <LeaveWindow>: unset()  unhighlight()\n\
	 <Btn3Up>:	notify() reset()";

 static String m_Trans =
	"<Btn3Up>:	MenuPopdown(%s)";

 static String L_Trans[] = { NULL,
	"! Shift      <Btn1Down>: %s(%s)",
	"!       Ctrl <Btn1Down>: %s(%s)",
	"! Shift Ctrl <Btn1Down>: %s(%s)",
	NULL };

 static String Labels[] = { NULL,
	" [Shift]",
	"        [Ctrl]",
	" [Shift][Ctrl]",
	NULL };


 if (list[0]) {
    (void) sprintf(tag, "%s_menu", parent->core.name);
    (void) sprintf(trans, p_Trans, tag, tag);
    XtOverrideTranslations(parent, XtParseTranslationTable(trans));

    (void) sprintf(trans, m_Trans, tag);
    XtSetArg(args[0], XtNtranslations, XtParseTranslationTable(trans));
    menu = XtCreatePopupShell(tag, overrideShellWidgetClass, parent, args, 1);

    XtSetArg(args[0], XtNdefaultDistance, (XtArgVal) 1);
    layout = XtCreateManagedWidget("menu", formWidgetClass, menu, args, 1);
/*
** Find widest label for this menu
*/
    for (j = k = n = indx = 0; list[indx] != NULL; indx++)
        if ((k = strlen(list[indx]->label)) > j) {
           j = k;
           n = indx;
          }
    k = XTextWidth(XMail.buttonFont, list[n]->label, j) + 12;

    if (--indx > 0)	/* add length for accelerator keys menu notes */
       k += XTextWidth(XMail.buttonFont, Labels[indx], strlen(Labels[indx]));
/*
** create the menu buttons
*/
    previous = NULL;
    XtSetArg(args[0], XtNwidth, k);
    XtSetArg(args[1], XtNfont, XMail.buttonFont);
    XtSetArg(args[2], XtNjustify, XtJustifyLeft);
    XtSetArg(args[3], XtNtranslations, XtParseTranslationTable(b_Trans));
    for (indx = 0; list[indx] != NULL; indx++) {
        (void) strcpy(tag, list[indx]->label);	/* set window name by label */
        if ((c = strchr(tag, ' '))) *c = '\0';	/* first word only */
        (void) sprintf(trans, "%-*.*s", j, j, list[indx]->label);
        if (Labels[indx])			/* add accel. note if needed */
           (void) strcat(trans, Labels[indx]);

        XtSetArg(args[4], XtNlabel, trans);
        XtSetArg(args[5], XtNfromVert, previous);
        next = XtCreateManagedWidget(tag, commandWidgetClass, layout, args, 6);

        if (info_text)
           AddInfoHandler(next, info_text[indx]);

        XtAddCallback(next, XtNcallback, list[indx]->func, list[indx]->data);
        previous = next;

	if (L_Trans[indx]) {
	   (void) sprintf(trans, L_Trans[indx], action, list[indx]->data);
           XtOverrideTranslations(parent, XtParseTranslationTable(trans));
          }
       }
/*
** Dummy out any remaining combinations of accelerators to do nothing
*/
    for (; L_Trans[indx] != NULL; indx++) {
	(void) sprintf(trans, L_Trans[indx], "DoNothing", "");
        XtOverrideTranslations(parent, XtParseTranslationTable(trans));
       }
   }
} /* AddMenuWindow */


/*
** @(#)AddButton() - add a button to the command window
**                   include help and an auxilary commands popup menu
*/
void
AddButton(parent, name, action, command, list, info_text, help_text)
Widget		parent;			/* widget which holds this button */
String		name;			/* Label for button (and children) */
String		action;			/* name of the XtActionProc to call */
String		command;		/* passed as param(s) to ActionProc */
menuList_p	*list;			/* aux. menu buttons list w/ cbacks */
String		*info_text;		/* pointer to button specific info */
String		help_text;		/* pointer to button specific help */
{
 Arg		args[5];
 Widget		button;
 char		trans[BUFSIZ];
/*
** Insert name of XtActionProc (and parameter(s)) in the translation list
*/
 (void) sprintf(trans, "#override <Btn1Down>: %s(%s)", action, command);

 XtSetArg(args[0], XtNwidth, XMail.buttonWidth);
 XtSetArg(args[1], XtNheight, XMail.buttonHeight);
 XtSetArg(args[2], XtNresize, False);
 XtSetArg(args[3], XtNfont, XMail.buttonFont);
 XtSetArg(args[4], XtNtranslations, XtParseTranslationTable(trans));
 button = XtCreateManagedWidget(name, commandWidgetClass, parent, args, 5);

 if (info_text)
    AddInfoHandler(button, info_text[0]);

 if (help_text)
    AddHelpText(button, help_text);

 if (list)
    AddMenuWindow(button, action, list, info_text);
} /* AddButton */


/* 
** @(#)AddSetMenu() - add hook to create menu for toggling mail options
*/
void
AddSetMenu(w)
Widget		w;
{
 Widget		parent = XtNameToWidget(w, "preserve");
 static String p_Trans = "#override \
    <Btn3Down>: SetMenu() MenuPopup(set_menu)";

 XtOverrideTranslations(parent, XtParseTranslationTable(p_Trans));
} /* AddSetMenu */


void
CreateButtons(parent)
Widget parent;
{
 menuList_p	*list;
 menuList	b1, b2, b3, b4;


 list = (menuList **) XtMalloc(5 * sizeof(menuList *));

 b1.label = "read";        b1.func = (XtCallbackProc) DoWith;  b1.data = "p";
 b2.label = "next";        b2.func = (XtCallbackProc) DoIt;    b2.data = "n";
 b3.label = "previous";    b3.func = (XtCallbackProc) DoIt;    b3.data = "-";
 b4.label = "full header"; b4.func = (XtCallbackProc) DoWith;  b4.data = "P";
 list[0] = &b1; list[1] = &b2; list[2] = &b3; list[3] = &b4; list[4] = NULL;
 AddButton(parent, "read", "DoSelected", "p", list, Read_Info, Read_Help);

 b1.label = "save";	b1.func = (XtCallbackProc) Save;	b1.data = "s";
 b2.label = "Save to author"; b2.func = (XtCallbackProc) Save;	b2.data = "S";
 b3.label = "write";	b3.func = (XtCallbackProc) Save;	b3.data = "w";
 list[0] = &b1; list[1] = &b2; list[2] = &b3; list[3] = NULL;
 AddButton(parent, "save", "DoSave", "s", list, Save_Info, Save_Help);

 CreateFolderButton(parent);

 b1.label = "copy";	b1.func = (XtCallbackProc) Save;	b1.data = "c";
 b2.label = "Copy to author"; b2.func = (XtCallbackProc) Save;	b2.data = "C";
 list[0] = &b1; list[1] = &b2; list[2] = NULL;
 AddButton(parent, "copy", "DoSave", "c", list, Copy_Info, Copy_Help);

 AddButton(parent, "preserve", "DoSelected", "preserve", (menuList_p *)NULL, Hold_Info, Hold_Help);

 b1.label = "delete";	b1.func = (XtCallbackProc) DoWith;	b1.data = "d";
 b2.label = "undelete";	b2.func = (XtCallbackProc) DoWith;	b2.data = "u";
 list[0] = &b1; list[1] = &b2; list[2] = NULL;
 AddButton(parent, "delete", "DoSelected", "d", list, Delete_Info, Delete_Help);

 b1.label = "Newmail"; b1.func = (XtCallbackProc) DoIt; b1.data = "file %";
 b2.label = "inc (no commit)"; b2.func = (XtCallbackProc) DoIt; b2.data = "inc";
 b3.label = "Drop Folders"; b3.func = (XtCallbackProc) DropIt; b3.data = "drop";
 list[0] = &b1; list[1] = &b2; list[2] = &b3; list[3] = NULL;
 AddButton(parent, "Newmail", "DoCmd", "file %", list, NewMail_Info, NewMail_Help);

 b1.label = "quit";	b1.func = (XtCallbackProc) DoQuit;	b1.data = "q";
 b2.label = "exit";	b2.func = (XtCallbackProc) DoQuit;	b2.data = "x";
 list[0] = &b1; list[1] = &b2; list[2] = NULL;
 AddButton(parent, "quit", "Quit", "q", list, Quit_Info, Quit_Help);

 b1.label = "print"; b1.func = (XtCallbackProc) DoPrint; b1.data = "";
 list[0] = &b1; list[1] = NULL;
 AddButton(parent, "Print", "PrintMsg", "", list, Print_Info, Print_Help);

 b1.label="send a message";    b1.func = (XtCallbackProc) Reply; b1.data = "s";
 b2.label="forward message";   b2.func = (XtCallbackProc) Reply; b2.data = "S";
 list[0] = &b1; list[1] = &b2; list[2] = NULL;
 AddButton(parent, "Send", "DoReply", "s", list, Send_Info, Send_Help);

 b1.label="reply";             b1.func = (XtCallbackProc) Reply; b1.data = "r";
 b2.label="reply included";    b2.func = (XtCallbackProc) Reply; b2.data = "R";
 b3.label="replyall";          b3.func = (XtCallbackProc) Reply; b3.data = "a";
 b4.label="replyall included"; b4.func = (XtCallbackProc) Reply; b4.data = "A";
 list[0] = &b1; list[1] = &b2; list[2] = &b3;
 list[3] = &b4; list[4] = NULL;
 AddButton(parent, "reply", "DoReply", "r", list, Reply_Info, Reply_Help);

 XtFree((menuList *) list);

 CreateFileWindow(parent);

 AddSetMenu(parent);		/* add a menu for toggling mail options */

} /* CreateButtons */


/*
** @(#)CreateCommandPanel() - add buttons, and set up the file menu.
*/
void
CreateCommandPanel(parent)
Widget parent;
{
 Arg		args[8];
 Widget		cw;

 static String w_Trans = "\
	<Btn1Down>:	DoNothing()\n\
	<Btn2Down>:	DoNothing()\n\
	<Btn3Down>:	DoNothing()";

 XtSetArg(args[0], XtNhSpace, XMail.commandHSpace);
 XtSetArg(args[1], XtNvSpace, XMail.commandVSpace);
 XtSetArg(args[2], XtNallowResize, (XtArgVal) False);
 XtSetArg(args[3], XtNmax, XMail.commandHeight);
 XtSetArg(args[4], XtNmin, XMail.commandHeight);
 XtSetArg(args[5], XtNfont, XMail.buttonFont);
 XtSetArg(args[6], XtNtranslations, XtParseTranslationTable(w_Trans));
 XtSetArg(args[7], XtNshowGrip, (XtArgVal) False);

 cw = XtCreateManagedWidget("commandPanel", boxWidgetClass, parent, args, 8);

 CreateButtons(cw);
} /* CreateCommandPanel */


/*
** @(#)CreateFileWindow() - for user entry of file and folder names
** Includes a special set of action routines for the keys to do
** Delete/Backspace, Ctrl-U, Ctrl-W, and insert-selection().
*/
void
CreateFileWindow(parent)
Widget parent;
{
 Arg		args[9];
 Widget		w;

 static String	trans = "#override \n\
  Ctrl <Key>a:		beginning-of-file() \n\
  Ctrl <Key>b:		backward-character() \n\
  Ctrl <Key>d:		CheckInsert() delete-next-character() \n\
  Ctrl <Key>e:		end-of-file() \n\
  Ctrl <Key>f:		forward-character() \n\
  Ctrl <Key>h:		DeleteChar() \n\
  Ctrl <Key>j:		DoNothing() \n\
  Ctrl <Key>m:		DoNothing() \n\
  Ctrl <Key>u:		DeleteLine() \n\
  Ctrl <Key>w:		DeleteWord() \n\
  Meta <Key>b:		backward-word() \n\
  Meta <Key>d:		CheckInsert() delete-next-word() \n\
  Meta <Key>f:		forward-word() \n\
       <Key>Delete:	DeleteChar() \n\
       <Key>BackSpace:	DeleteChar() \n\
       <Key>Linefeed:	DoNothing() \n\
       <Key>Return:	DoNothing() \n\
       <Key>Right:	forward-character() \n\
       <Key>Left:	backward-character() \n\
       <Key>:		CheckInsert() insert-char()\n\
       <Btn1Down>:	select-start() \n\
       <Btn2Down>:	CheckInsert() insert-selection(PRIMARY, CUT_BUFFER0)";


 XtSetArg(args[0], XtNwidth, XMail.fileBoxWidth);
 XtSetArg(args[1], XtNheight, XMail.buttonHeight);
 XtSetArg(args[2], XtNborderWidth, 1);
 XtSetArg(args[3], XtNfont, XMail.buttonFont);
 XtSetArg(args[4], XtNeditType, XawtextEdit);
 XtSetArg(args[5], XtNstring, (XtArgVal) "");
 XtSetArg(args[6], XtNtranslations, XtParseTranslationTable(trans));

 w = XtCreateManagedWidget("fileWindow", asciiTextWidgetClass, parent, args, 7);

 writeTo(w, "File: ", REPLACE);		/* insert label and position cursor */

 AddInfoHandler(w, "Specify the name of a mail folder");

 AddHelpText(w, File_Help);
} /* CreateFileWindow */


/*
** @(#)CreateFolderButton() - attach special Btn3 hook to list folders
*/
void
CreateFolderButton(parent)
Widget	parent;
{
 Arg		args[5];
 Widget		button;

 static String	trans = "#override \n\
			<Btn1Down>: Folder() \n\
			<Btn3Down>: SetFolders()";

 XtSetArg(args[0], XtNwidth, XMail.buttonWidth);
 XtSetArg(args[1], XtNheight, XMail.buttonHeight);
 XtSetArg(args[2], XtNresize, False);
 XtSetArg(args[3], XtNfont, XMail.buttonFont);
 XtSetArg(args[4], XtNtranslations, XtParseTranslationTable(trans));
 button = XtCreateManagedWidget("Folder", commandWidgetClass, parent, args, 5);

 AddInfoHandler(button, Folder_Info[0]);

 AddHelpText(button, Folder_Help);

 XtRegisterGrabAction(SetFolders, True, ButtonPressMask | ButtonReleaseMask,
                       GrabModeAsync, GrabModeAsync);
} /* CreateFolderButton */


/* 
** @(#)CreateHelpWindow() - create popup window for the help text
*/
void
CreateHelpWindow(parent)
Widget parent;
{
 Arg	args[4];
 Widget	w;
 static String	trans = "#override \n\
  Ctrl <Key>J:		MenuPopdown(help) \n\
  <Key>Linefeed:	MenuPopdown(help)";

 XtSetArg(args[0], XtNborderWidth, 3);
 w = XtCreatePopupShell("help", overrideShellWidgetClass, parent, args, 1);

 XtSetArg(args[0], XtNwidth, XMail.helpWidth);
 XtSetArg(args[1], XtNheight, XMail.helpHeight);
 XtSetArg(args[2], XtNfont, XMail.helpFont);
 XtSetArg(args[3], XtNtranslations, XtParseTranslationTable(trans));

 XtCreateManagedWidget("helpWindow", asciiTextWidgetClass, w, args, 4);
} /* CreateHelpWindow */


/* 
** Index window is a AsciiTextWidget, with special key assignments
*/
void
CreateIndexWindow(parent)
Widget parent;
{
 Arg	args[7];
 Widget	w;

 XtSetArg(args[0], XtNheight, XMail.indexHeight);
 XtSetArg(args[1], XtNfont, XMail.textFont);
 XtSetArg(args[2], XtNdisplayCaret, (XtArgVal) False);
 XtSetArg(args[3], XtNeditType, XawtextEdit);
 XtSetArg(args[4], XtNscrollVertical, XawtextScrollAlways);
 XtSetArg(args[5], XtNwrap, XawtextWrapNever);
 XtSetArg(args[6], XtNstring, "");

 w = XtCreateManagedWidget("indexWindow", asciiTextWidgetClass,parent,args,7);

 AddHelpText(w, Index_Help);
} /* CreateIndexWindow */


/*
** @(#)CreateStatusWindow() - place for messages from mail
*/
void
CreateStatusWindow(parent)
Widget parent;
{
 Arg		args[10];
 Widget		w;

 static String w_Trans = "\
	<Btn1Down>:	DoNothing()\n\
	<Btn3Down>:	DoNothing()";

 XtSetArg(args[0], XtNallowResize, False);
 XtSetArg(args[1], XtNmin, XMail.buttonHeight);
 XtSetArg(args[2], XtNmax, XMail.buttonHeight);
 XtSetArg(args[3], XtNshowGrip, False);

 XtSetArg(args[4], XtNwidth, XMail.shellWidth);
 XtSetArg(args[5], XtNfont, XMail.textFont);
 XtSetArg(args[6], XtNjustify, XtJustifyCenter);
 XtSetArg(args[7], XtNlabel, "Establishing connections");
 XtSetArg(args[8], XtNresize, False);
 XtSetArg(args[9], XtNtranslations, XtParseTranslationTable(w_Trans));

 w = XtCreateManagedWidget("statusWindow", labelWidgetClass, parent, args, 10);

 AddHelpText(w, Status_Help);
} /* CreateStatusWindow */


/*
** @(#)CreateSubWindows() - create all the xmail subwindows.
*/
void
CreateSubWindows(parent)
Widget parent;
{
 Arg		args[4];
 Widget		icon, mb, topbox;
 int		n, x, y;


 XtSetArg(args[0], XtNinput,      True);
 XtSetArg(args[1], XtNwidth,      XMail.shellWidth);
 XtSetArg(args[2], XtNgripIndent, 0);
 XtSetArg(args[3], XtNskipAdjust, True);

 topbox = XtCreateManagedWidget("topBox", panedWidgetClass, parent, args, 4);

 CreateHelpWindow(topbox);
 CreateTitleBar(topbox);
 CreateIndexWindow(topbox);
 CreateStatusWindow(topbox);
 CreateCommandPanel(topbox);
 CreateTextWindow(topbox);
/*
** Create an icon window for the mailwatchWidget
*/
 XtSetArg(args[0], XtNwidth, (XtArgVal) mail_width);
 XtSetArg(args[1], XtNheight, (XtArgVal) mail_height);
 n = 2;
 if (XMail.iconGeometry) {
    ParseIconGeometry(XMail.iconGeometry, &x, &y);
    XtSetArg(args[n], XtNx, (XtArgVal) x);		n++;
    XtSetArg(args[n], XtNy, (XtArgVal) y);		n++;
   }
 icon = XtCreateWidget("icon", applicationShellWidgetClass, toplevel, args, n);
 mb = XtCreateManagedWidget("mailbox", mailwatchWidgetClass, icon, NULL, 0);
 XtAddCallback(mb, XtNcallback, SetNewmail, NULL);
} /* CreateSubWindows */ 


/* 
** @(#)CreateTextWindow() - AsciiTextWidget window, without special keys
*/
void
CreateTextWindow(parent)
Widget parent;
{
 Arg 		args[11];
 Widget		form, tw;


 XtSetArg(args[0], XtNmin, XMail.textHeight / 3);
 XtSetArg(args[1], XtNheight, XMail.textHeight);
 XtSetArg(args[2], XtNwidth, XMail.shellWidth);
 form = XtCreateManagedWidget("textWindow", formWidgetClass, parent, args, 3);

 XtSetArg(args[0], XtNleft, XtChainRight);
 XtSetArg(args[1], XtNright, XtChainRight);
 XtSetArg(args[2], XtNtop, XtChainTop);
 XtSetArg(args[3], XtNbottom, XtChainTop);
 XtSetArg(args[4], XtNvertDistance, 0);
 XtSetArg(args[5], XtNhorizDistance, XMail.shellWidth - 48);
 XtSetArg(args[6], XtNborderWidth, 0);
 XtSetArg(args[7], XtNheight, 48);
 XtSetArg(args[8], XtNwidth, 48);
 XtSetArg(args[9], XtNinternalWidth, 0);
 XtCreateWidget("face", labelWidgetClass, form, args, 10);

 XtSetArg(args[0], XtNleft, XtChainLeft);
 XtSetArg(args[1], XtNright, XtChainRight);
 XtSetArg(args[2], XtNtop, XtChainTop);
 XtSetArg(args[3], XtNbottom, XtChainBottom);
 /* see above... */
 XtSetArg(args[5], XtNhorizDistance, 0);
 /* see above... */
 XtSetArg(args[7], XtNheight, XMail.textHeight);
 XtSetArg(args[8], XtNfont, XMail.textFont);
 XtSetArg(args[9], XtNstring, "");
 XtSetArg(args[10], XtNscrollVertical, XawtextScrollAlways);
 tw = XtCreateManagedWidget("text", asciiTextWidgetClass, form, args, 11);

 AddHelpText(tw, Text_Help);

 HelpList.name[HelpList.indx] = XtNewString("text2");

 XtSetArg(args[0], XtNstring, (XtArgVal) Text2_Help);
 XtSetArg(args[1], XtNlength, (XtArgVal) strlen(Text2_Help));
 HelpList.text[HelpList.indx] = XtCreateWidget("text2", asciiSrcObjectClass,
                        XtNameToWidget(toplevel, "topBox.help.helpWindow"),
                        args, 2);
 HelpList.indx += 1;
} /* CreateTextWindow */


/*
** @(#)CreateTitleBar() - creates the title bar displayed at top of shell
**			  Include Nat. Semi. Corp. logo in top left corner.
*/
void
CreateTitleBar(parent)
Widget parent;
{
 Arg		args[8];
 Pixmap		logo;
 Widget		form, lg, tb;
 char		buf[20];

 static String w_Trans = "\
	<Btn1Down>:	DoNothing()\n\
	<Btn2Down>:	DoNothing()\n\
	<Btn3Down>:	DoNothing()";

 static String l_Trans = "\
	<Btn1Down>:	Iconify()\n\
	<Btn2Down>:	Iconify()\n\
	<Btn3Down>:	Iconify()";

 XtSetArg(args[0], XtNdefaultDistance, 1);
 XtSetArg(args[1], XtNallowResize, (XtArgVal) False);
 XtSetArg(args[2], XtNmax, (XtArgVal) XMail.buttonHeight);
 XtSetArg(args[3], XtNmin, (XtArgVal) XMail.buttonHeight);
 XtSetArg(args[4], XtNshowGrip, (XtArgVal) False);

 form = XtCreateManagedWidget("titleBar", formWidgetClass, parent, args, 5);

 logo = XCreateBitmapFromData(XtDisplay(toplevel), XtScreen(toplevel)->root,
                        national_bits, national_width, national_height);

 XtSetArg(args[0], XtNborderWidth, 0);
 XtSetArg(args[1], XtNheight, (XtArgVal) XMail.buttonHeight);
 XtSetArg(args[2], XtNwidth, national_width);
 XtSetArg(args[3], XtNinternalWidth, 0);
 XtSetArg(args[4], XtNbitmap, (XtArgVal) logo);
 XtSetArg(args[5], XtNtranslations, XtParseTranslationTable(l_Trans));
 lg = XtCreateManagedWidget("logo", commandWidgetClass, form, args, 6);

 (void) sprintf(buf, "%s%d", TITLE, PATCHLEVEL);

 XtSetArg(args[2], XtNwidth, XMail.shellWidth - national_width - 2);
 XtSetArg(args[3], XtNlabel, (XtArgVal) buf);
 XtSetArg(args[4], XtNfont, XMail.textFont);
 XtSetArg(args[5], XtNjustify, (XtArgVal) XtJustifyLeft);
 XtSetArg(args[6], XtNfromHoriz, lg);
 XtSetArg(args[7], XtNtranslations, XtParseTranslationTable(w_Trans));
 tb = XtCreateManagedWidget("title", labelWidgetClass, form, args, 8);

 AddHelpText(tb, Title_Help);
} /* CreateTitleBar */


/*
** @(#)CreateInputWindow() - for specifying recipient, subject, and Cc list
** Has its own set of translations for editing.
** Special actions for Delete/Backspace, Ctrl-U, and Ctrl-W.
*/
Widget
CreateInputWindow(parent, name)
Widget	parent;
String	name;
{
 Arg	args[7];
 int	n;
 Widget	w;

 n  = XMail.shellWidth - 26;	/* - (20 + 2 * (internal_width+def_dist)) */
 n -= figureWidth(XMail.buttonFont)*9-10; /* less label width+fudge factor */

 XtSetArg(args[0], XtNinput, True);
 XtSetArg(args[1], XtNwidth, n);
 XtSetArg(args[2], XtNheight, XMail.buttonHeight + XMail.borderWidth);
 XtSetArg(args[3], XtNborderWidth, 1);
 XtSetArg(args[4], XtNfont, XMail.buttonFont);
 XtSetArg(args[5], XtNstring, "");
 XtSetArg(args[6], XtNeditType, XawtextEdit);

 w = XtCreateManagedWidget(name, asciiTextWidgetClass, parent, args, 7);

 return( w );
} /* CreateInputWindow */

/* 
** @(#)ParseIconGeometry() - Parse the icon geometry
*/
void
ParseIconGeometry(str, x, y)
char	*str;
int	*x, *y;
{
 int		res;
 unsigned int	w, h;

 *x = 0;
 *y = 0;
 res = XParseGeometry(str, x, y, &w, &h);
 if ((res & (XNegative|XValue)) == (XNegative|XValue))
    *x = RootWidth + *x - mail_width;
 if ((res & (YNegative|YValue)) == (YNegative|YValue))
    *y = RootHeight + *y - mail_height;
} /* ParseIconGeometry */
