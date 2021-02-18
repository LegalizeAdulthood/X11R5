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
 * Author:  Michael C. Wagnitz - National Semiconductor Corporation
 *
 */


#include "defs.h"

/* HelpText.c */

extern String		Autograph_Info[];
extern String		Copy_Info[];
extern String		Default_Status_Info;
extern String		Delete_Info[];
extern String		Deliver_Info[];
extern String		Folder_Info[];
extern String		Hold_Info[];
extern String		NewMail_Info[];
extern String		Print_Info[];
extern String		Quit_Info[];
extern String		Read_Info[];
extern String		Reply_Info[];
extern String		Save_Info[];
extern String		Send_Info[];
extern String		SendMail_Info[];

extern String		Abort_Help;
extern String		Bcc_Help;
extern String		Cancel_Help;
extern String		Cc_Help;
extern String		Copy_Help;
extern String		Delete_Help;
extern String		Deliver_Help;
extern String		File_Help;
extern String		Folder_Help;
extern String		Hold_Help;
extern String		Index_Help;
extern String		NewMail_Help;
extern String		Print_Help;
extern String		Quit_Help;
extern String		Read_Help;
extern String		ReEdit_Help;
extern String		Reply_Help;
extern String		Save_Help;
extern String		Send_Help;
extern String		Sign_Help;
extern String		sign_Help;
extern String		Status_Help;
extern String		Subject_Help;
extern String		Text_Help;
extern String		Text2_Help;
extern String		Title_Help;
extern String		To_Help;

/* actions.c */

extern int		str_compare();		/* for qsort of folder names */
extern XtActionProc	CheckInsert();		/* for the file window */
extern XtActionProc	DeleteChar();		/* for deletes (file window) */
extern XtActionProc	DeleteLine();		/* in the file window */
extern XtActionProc	DeleteWord();		/* also in the file window */
extern XtActionProc	DoCmd();		/* effect specified command */
extern XtActionProc	DoNothing();		/* dummy action routine */
extern XtActionProc	DoReply();		/* invoke reply callback */
extern XtActionProc	DoSave();		/* invoke the save callback */
extern XtActionProc	DoSelected();		/* DoCmd w/ selected message */
extern XtActionProc	Folder();		/* execute folder command */
extern XtActionProc	Iconify();		/* iconification request */
extern XtActionProc	MyNotify();		/* invokes callbacks w/ param */
extern XtActionProc	NextField();		/* warps pointer to data fld */
extern XtActionProc	PrintMsg();		/* send mail to the printer */
extern XtActionProc	Quit();			/* termination procedure */
extern XtActionProc	SetAliases();		/* menu list of alias names */
extern XtActionProc	SetDirectory();		/* create folder files list */
extern XtActionProc	SetFolders();		/* create a list of folders */
extern XtActionProc	SetMenu();		/* Position set_menu popup */
extern XtActionProc	SetPopup();		/* Position relative to owner */
extern XtActionProc	SetSelect();		/* reposition select mark */
extern XtActionProc	ShowHelp();		/* Position help & popup text */

/* callbacks.c */

extern XtCallbackProc	Autograph();		/* adds Signature to message */
extern int		endDelivery();		/* catch delivery completion */
extern XtCallbackProc	Done();			/* completes mail delivery */
extern XtCallbackProc	DoIt();			/* callback do specified cmd */
extern XtCallbackProc	DoPrint();		/* callback printer execution */
extern XtCallbackProc	DoQuit();		/* callback termination func */
extern XtCallbackProc	DoSet();		/* callback to toggle options */
extern XtCallbackProc	DoWith();		/* do cmd with arguments */
extern XtCallbackProc	DropIt();		/* destroy Folder popup list */
extern XtCallbackProc	GetAliasName();		/* retrieve alias name */
extern XtCallbackProc	GetFolderName();	/* retrieve folder name */
extern XtCallbackProc	ReEdit();		/* re-edit the mail message */
extern XtCallbackProc	Reply();		/* use msg author and subject */
extern XtCallbackProc	Save();			/* callback save routine */
extern XtCallbackProc	SetNewmail();		/* highlight when new mail */
extern XtCallbackProc	UnsetNewmail();		/* remove newmail highlight */

/* callMail.c */

extern void		callMail();		/* fork child, exec mail */

/* confirm.c */

extern Bool		Confirm();		/* Confirm destructive acts */

/* environs.c */

extern char		*GetMailEnv();		/* get value from mail env */
extern char		*GetMailrc();		/* get mailrc item value */
extern char		*alias();		/* get mailrc alias value */

/* handler.c */

extern int 		In_System_Folder();	/* boolean test of folder */
extern XtEventHandler	icon_handler();		/* handle iconify changes */
extern int 		index_handler();	/* highlight the msg number */
extern void 		info_handler();		/* change content of status */
extern int 		file_handler();		/* handle change of folder */

/* mail.c */

extern int 		endEdits();		/* catch exit of edit child */
extern void 		editMail();		/* create/compose a message */
extern XtInputCallbackProc readMail();		/* get data from mail */
extern void 		sendMail();		/* used by Send/Reply cmds */
extern void 		writeMail();		/* send data to mail */

/* parser.c */

extern void		parser_init();		/* compile patterns */
extern int		match();		/* eval command or output */
extern void		parse();		/* parse mail output */
extern char		*QueryMail();		/* ask mail for info */

/* utils.c */

extern void 		Bell();			/* disp. error and sound bell */
extern int		figureWidth();		/* get figure width for font */
extern void 		markIndex();		/* add/replace index marker */
extern int 		SelectionNumber();	/* get number of selected msg */
extern void		SetCursor();		/* change between wait & norm */
extern void 		SetXY();		/* set widget x/y coordinates */
extern XawTextPosition	TextGetLastPos();	/* get last pos of text */
extern void		UpdateTitleBar();	/* file name and msg count */
extern void 		writeText();		/* write text to buffer */
extern void 		writeTo();		/* send text data to window */

/* windows.c */

extern void		AddHelpText();		/* Add help popup to window */
extern void		AddMenuWindow();	/* Add menu popup to window */
extern void		AddButton();		/* Add command to window */
extern void		CreateButtons();	/* make the command buttons */
extern void		CreateCommandPanel();	/* form for command buttons */
extern void		CreateFileWindow();	/* a text input window */
extern void		CreateFolderButton();	/* a special command button */
extern void		CreateHelpWindow();	/* the help window base */
extern void		CreateIndexWindow();	/* a text input window */
extern void		CreateStatusWindow();	/* a text output window */
extern void 		CreateSubWindows();	/* all subwindows of xmail */
extern void		CreateTextWindow();	/* another text window */
extern void		CreateTitleBar();	/* a text output window */
extern Widget		CreateInputWindow();	/* To, Subject, Cc, and Bcc */
extern void		ParseIconGeometry();	/* Parse icon geometry */

/* xmail.c */

extern void		initialize();		/* main initialization code */
extern int		mailoptions();		/* sets up call to mail pgm */

/* extern functions */

extern char	*strchr(), *strrchr();

/* extern variables */

extern char	Command[BUFSIZ];	/* xmail command string		*/
extern char 	InReply[BUFSIZ];	/* reply reference string	*/
extern char 	tmpName[BUFSIZ];	/* message temporary filename	*/
extern char	*mailargv[7];		/* array passed to mail child	*/

extern Pixmap	hatch;			/* cross_weave, used for Newmail */
extern Boolean	In_Bogus_Mail_File;	/* in bogus folder when we're iconic */
extern Widget	toplevel; 		/* top level shell widget	*/
extern Atom	wmDeleteWindow;		/* for WM_DELETE_WINDOW property */

extern AliasRecPtr	*aliases;	/* array of mail alias strings	*/
extern helpList		HelpList;
extern XmailResources	XMail;		/* XMail application resources */

extern int	mail_fd;		/* descriptor to mail process i/o */

extern int	mailargc;		/* counter passed to mail child	*/
extern int	RootWidth, RootHeight;
extern int	mailpid;		/* mail process id */
extern int	Highlighted;		/* state of 'Newmail' highlighting */
extern int	mailInputId;		/* Identity of the XtAddInput call */

/* global.h */
