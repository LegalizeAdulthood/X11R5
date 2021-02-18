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
 * 1. initialize: and
 * 2. SetHints.
 *
 * Author:  Michael C. Wagnitz - National Semiconductor Corporation
 *
 */

#include "global.h"
#include "Mailwatch.h"			/* use as icon and watch for newmail */
#include <X11/Xaw/CommandP.h>
#include <X11/Xaw/TextP.h>
#include <X11/cursorfont.h>		/* use watch cursor for busy notify */
#include <X11/bitmaps/cross_weave>	/* background use in Newmail notify */

#ifndef	lint
static char what[] =
 "@(#)xmail.c 1.4 91/09/20 Copyright 1989,1990,1991 National Semiconductor Corp.";
#endif

#ifndef	DEFAULT_PROMPT
#define	DEFAULT_PROMPT	"& "
#endif

#define Offset(field) (XtOffset(XmailResources *, field))

static XtResource resrcs[] = {
    {"textFont", XtCFont, XtRFontStruct, sizeof (XFontStruct *),
	Offset(textFont), XtRString, XtDefaultFont},
    {"helpFont", XtCFont, XtRFontStruct, sizeof (XFontStruct *),
	Offset(helpFont), XtRString, XtDefaultFont},
    {"buttonFont", XtCFont, XtRFontStruct, sizeof (XFontStruct *),
	Offset(buttonFont), XtRString, XtDefaultFont},
    {"iconGeometry", "IconGeometry", XtRString, sizeof(char *),
        Offset(iconGeometry), XtRString, (caddr_t) NULL},
    {"mFileName", "MFileName", XtRString, sizeof(char *), 
	Offset(MFileName), XtRString, (caddr_t) NULL},
    {"editorCommand", "EditorCommand", XtRString, sizeof(char *), 
	Offset(editorCommand), XtRString, (caddr_t) NULL},
    {"bellRing", "BellRing", XtRBoolean, sizeof(Boolean), 
	Offset(bellRing), XtRImmediate, (caddr_t) True},
    {"expert", "Expert", XtRBoolean, sizeof(Boolean), 
	Offset(expert), XtRImmediate, (caddr_t) False},
    {"iconic", "Iconic", XtRBoolean, sizeof(Boolean), 
	Offset(iconic), XtRImmediate, (caddr_t) False},
    {"mailopt_n", "Mailopt_n", XtRBoolean, sizeof(Boolean), 
	Offset(mailopt_n), XtRImmediate, (caddr_t) False},
    {"mailopt_U", "Mailopt_U", XtRBoolean, sizeof(Boolean), 
	Offset(mailopt_U), XtRImmediate, (caddr_t) False},
    {"show_Last", "Show_Last", XtRBoolean, sizeof(Boolean), 
	Offset(Show_Last), XtRImmediate, (caddr_t) True},
    {"show_Info", "Show_Info", XtRBoolean, sizeof(Boolean), 
	Offset(Show_Info), XtRImmediate, (caddr_t) True},
    {"no_X_Hdr", "No_X_Hdr", XtRBoolean, sizeof(Boolean), 
	Offset(No_X_Hdr), XtRImmediate, (caddr_t) False},
    { XtNborderWidth, XtCBorderWidth, XtRInt, sizeof (int),
	Offset(borderWidth), XtRString, "1"},
};

#undef Offset

static XrmOptionDescRec Opts[] = {
    {"-borderwidth",	".TopLevelShell.borderWidth",	XrmoptionSepArg,	(caddr_t) NULL},
    {"-buttonfont",	"*buttonFont",			XrmoptionSepArg,	(caddr_t) NULL},
    {"-buttonFont",	"*buttonFont",			XrmoptionSepArg,	(caddr_t) NULL},
    {"-bw",		".TopLevelShell.borderWidth",	XrmoptionSepArg,	(caddr_t) NULL},
    {"-editorcommand",	"*editorCommand",		XrmoptionSepArg,	(caddr_t) NULL},
    {"-editorCommand",	"*editorCommand",		XrmoptionSepArg,	(caddr_t) NULL},
    {"-e",		"*expert",			XrmoptionNoArg,		(caddr_t) "True"},
    {"-f",		"*MFileName",			XrmoptionSepArg,	(caddr_t) NULL},
    {"-fn",		"*Font",			XrmoptionSepArg,	(caddr_t) NULL},
    {"-font",		"*Font",			XrmoptionSepArg,	(caddr_t) NULL},
    {"-geometry",	".geometry",			XrmoptionSepArg,	(caddr_t) NULL},
    {"-helpfont",	"*helpFont",			XrmoptionSepArg,	(caddr_t) NULL},
    {"-helpFont",	"*helpFont",			XrmoptionSepArg,	(caddr_t) NULL},
    {"-iconGeometry",	"*iconGeometry",		XrmoptionSepArg,	(caddr_t) NULL},
    {"-iconic",		"*iconic",			XrmoptionNoArg,		(caddr_t) "True"},
    {"-ls",		"*show_Last",			XrmoptionNoArg,		(caddr_t) "False"},
    {"-m",		"*icon*useHost",		XrmoptionNoArg,		(caddr_t) "True"},
    {"-n",		"*mailopt_n",			XrmoptionNoArg,		(caddr_t) "True"},
    {"-nb",		"*bellRing",			XrmoptionNoArg,		(caddr_t) "False"},
    {"-noinfo",		"*show_Info",			XrmoptionNoArg,		(caddr_t) "False"},
    {"-nx",		"*no_X_Hdr",			XrmoptionNoArg,		(caddr_t) "True"},
    {"-rv",		"*icon*reverseVideo",		XrmoptionNoArg,		(caddr_t) "True"},
    {"-textfont",	"*textFont",			XrmoptionSepArg,	(caddr_t) NULL},
    {"-textFont",	"*textFont",			XrmoptionSepArg,	(caddr_t) NULL},
    {"-U",		"*mailopt_U",			XrmoptionNoArg,		(caddr_t) "True"},
    {"-u",		"*icon*useName",		XrmoptionNoArg,		(caddr_t) "True"},
    {"-xrm",		NULL,				XrmoptionResArg,	(caddr_t) NULL},
};

Atom		wmDeleteWindow;

int		mailargc;		/* counter passed to mail child	*/
int		RootWidth, RootHeight;
int		Highlighted;		/* state of 'Newmail' highlighting */

char		Command[BUFSIZ];	/* xmail command string */
char	 	InReply[BUFSIZ];	/* reply reference string */
char	 	tmpName[BUFSIZ];	/* message temporary filename */
char		*mailargv[7];		/* array passed to mail child */

Pixmap		hatch;			/* cross_weave used to note Newmail */
Widget		toplevel; 		/* top level shell widget */
Window		WaitCursorWindow;
helpList	HelpList;

XmailResources 	XMail;			/* application resources of xmail */

/*
** @(#)mailoptions() - construct command line arguments for calling mail.
**                     Return the argument list for mail and new value of argc.
*/
int
mailoptions()
{
 XWMHints	*wm_hints;
 int		i;
 char		*Mailpgm;		/* name of executable Mailpgm */


 if (! (Mailpgm = (char *)getenv("XMAILER")))	/* first looks up env var */
    Mailpgm = DEFAULT_MAILER;


 wm_hints = XGetWMHints(XtDisplay(toplevel), XtWindow(toplevel));

 i = 0;
 mailargv[i++] = Mailpgm;	/* Argv[0] is the name of the program */
 mailargv[i++] = "-N";		/* no version or header info at start */
 if (XMail.mailopt_n)
    mailargv[i++] = "-n";	/* don't initialize from Mail.rc file */
 if (XMail.mailopt_U)
    mailargv[i++] = "-U";	/* Change uucp to Internet addresses */

 if (wm_hints->initial_state == IconicState) {	/* start iconic in bogus */
    FILE	*fp;
    char	bogus[BUFSIZ];

    (void) sprintf(bogus, "%s+", tmpName);
    if (fp = fopen(bogus, "w")) {	/* create our bogus mail file */
       (void) fprintf(fp, "\n");
       (void) fclose(fp);
       mailargv[i++] = "-f";		/* start in our bogus mail folder */
       mailargv[i++] = bogus;
       In_Bogus_Mail_File = True;
      }
   } else if (XMail.MFileName) {
	     mailargv[i++] = "-f";	/* start from mail folder MFileName */
	     mailargv[i++] = XMail.MFileName;
            }
 mailargv[i] = NULL;
 XFree(wm_hints);
 return (i);
} /* mailoptions */


/*
** @(#)initialize() - establish xmail program defaults and setups
*/
void
initialize()
{
 String		disp, file;
 long		buttonSpace, i;
 Display	*dpy = XtDisplay(toplevel);
 char		*str_type[20];
 XrmValue	value;


 if (XrmGetResource(dpy->db, "command.borderWidth", "Command.BorderWidth",
    str_type, &value)) {
    if ((i = atoi(value.addr)) > XMail.borderWidth)
       XMail.borderWidth = i;
   }

 if (XrmGetResource(dpy->db, "send.borderWidth", "Send.BorderWidth",
    str_type, &value)) {
    if ((i = atoi(value.addr)) > XMail.borderWidth)
       XMail.borderWidth = i;
   }

 HelpList.indx = 0;

 (void) strcpy(Command, "Start");	/* let parser know we've started */

 (void) sprintf(tmpName, "/tmp/xmail%d", getpid());
/*
** TRAP any stupid attempt to set the border width of this application to less
** than the allowed minimum (0) or greater than an acceptable maximum.  This
** to prevent a really wierd reaction to a negative number for border width.
*/
 if (XMail.borderWidth < 0)
     XMail.borderWidth = 1;
 if (XMail.borderWidth > 6)
     XMail.borderWidth = 6;
/*
** Compute all window dimensions based on extents of the specified fonts.
** Make shell wide enough to hold eight buttons on a row.  Total button width
** includes width of text area plus width of separation plus width of border.
*/
 XMail.commandHSpace	= 10;
 XMail.commandVSpace	= 10;
 XMail.buttonHeight	= CHARHEIGHT(XMail.buttonFont) + (XMail.buttonFont->ascent / 2);
 XMail.buttonWidth	= figureWidth(XMail.buttonFont) * 9; /* ' NewMail ' */
       buttonSpace	= XMail.borderWidth * 2;
       buttonSpace	+= XMail.buttonWidth + XMail.commandHSpace;
 XMail.shellWidth	= (8 * buttonSpace) + 26;    /*  8 buttons per row */
 XMail.fileBoxWidth	= (4 * buttonSpace) + XMail.buttonWidth;
 XMail.helpHeight	= CHARHEIGHT(XMail.helpFont) * 14 + 5;
 XMail.helpWidth	= figureWidth(XMail.helpFont) * 60 + 5;
 XMail.helpX		= (XMail.shellWidth - XMail.helpWidth) / 2;
 XMail.helpY		= 70;
       buttonSpace	= CHARHEIGHT(XMail.textFont) + (XMail.textFont->ascent / 2);
 XMail.indexHeight	= 12 * buttonSpace;
 XMail.textHeight	= 23 * buttonSpace;
 XMail.commandHeight	= (XMail.buttonHeight*2) + (XMail.commandVSpace*3) +
			  (4 * XMail.borderWidth);
 XMail.menuX		= 15;
 XMail.menuY		= 7;
       RootWidth	= XDisplayWidth(XtDisplay(toplevel),
			  DefaultScreen(XtDisplay(toplevel))) - 1;
       RootHeight	= XDisplayHeight(XtDisplay(toplevel),
			   DefaultScreen(XtDisplay(toplevel))) - 1;
/*
** Ensure xmail shell height does not exceed the height of the root window.
*/
      buttonSpace	= (2 * XMail.buttonHeight) + XMail.indexHeight +
			  XMail.commandHeight + XMail.textHeight + 
			  (6 * XMail.borderWidth);

 for (i = 0, buttonSpace -= RootHeight; buttonSpace > 0; i++) {
     buttonSpace -= CHARHEIGHT(XMail.textFont);
     if (i % 2)
        XMail.indexHeight -= CHARHEIGHT(XMail.textFont);
     else
        XMail.textHeight -= CHARHEIGHT(XMail.textFont);
    }
/*
** If editorCommand resource exists, warn if it doesn't have two %s entries.
*/
 if (XMail.editorCommand)
    if (! (disp =  strchr(XMail.editorCommand, '%')) || *(disp + 1) != 's' ||
        ! (file = strrchr(XMail.editorCommand, '%')) || *(file + 1) != 's' ||
           disp == file) {
    XtWarning("xmail editorCommand resource improperly defined... ignoring");
    XtFree((char *) XMail.editorCommand);
    XMail.editorCommand = NULL;
   }

 parser_init();
} /* initialize */


/*
** @(#)SetHints() - Sets hatching, cursor, icon and size hints, and wm protocol
*/
SetHints()
{
 int			x, y;
 unsigned long		valuemask;
 CommandWidget		nm = (CommandWidget)XtNameToWidget(toplevel, "topBox.commandPanel.Newmail");
 Display		*dpy = XtDisplay(toplevel);
 Screen			*scn = XtScreen(toplevel);
 Window			win = XtWindow(toplevel);
 XSetWindowAttributes	attributes;
 XSizeHints		size_hints;
 XWMHints		wm_hints;
/*
** Hatch will be used to indicate new mail arrival on the command button
*/
 hatch	= XCreatePixmapFromBitmapData(dpy, XtScreen(toplevel)->root,
			cross_weave_bits, cross_weave_width, cross_weave_height,
			nm->label.foreground, nm->core.background_pixel,
			DefaultDepth(dpy, DefaultScreen(dpy)));
/*
** Notify the window manager about our icon window
*/
 wm_hints.input		= True;
 wm_hints.initial_state	= (XMail.iconic) ? IconicState : NormalState;
 wm_hints.flags		= InputHint | StateHint;

 if (XMail.iconGeometry) {
    ParseIconGeometry(XMail.iconGeometry, &x, &y);
    wm_hints.icon_x	= x;
    wm_hints.icon_y	= y;
    wm_hints.flags	|= IconPositionHint;
   }
 wm_hints.flags		|= IconWindowHint;
 wm_hints.icon_window	= XtWindow(XtNameToWidget(toplevel, "icon"));

 XSetWMHints(dpy, win, &wm_hints);
/*
** Set starting position and default geometry
*/
 if (! XGetNormalHints(dpy, win, &size_hints)) {
    size_hints.x = 56;
    size_hints.y = 56;			/* slightly off from top left corner */
   }

 size_hints.width	= XMail.shellWidth;
 size_hints.height	= (2 * XMail.buttonHeight) + XMail.indexHeight +
			  XMail.commandHeight + XMail.textHeight +
			  (6 * XMail.borderWidth);

 size_hints.min_width = size_hints.max_width = size_hints.width;
 size_hints.min_height = size_hints.max_height = size_hints.height;
 size_hints.flags = USPosition | USSize | PMinSize;

 XSetNormalHints(dpy, win, &size_hints);
/*
** Add a protocol flag indicating we wish to handle WM_DELETE_WINDOW requests
*/
 wmDeleteWindow = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
 XSetWMProtocols(dpy, win, &wmDeleteWindow, 1);
/*
** Create a WaitCursor window, ala FAQ to Ignore device events while
** the busy cursor is displayed.  The window will be as big as the
** display screen, and clipped by its own parent window, so we never
** have to worry about resizing.
*/
 valuemask = CWDontPropagate | CWCursor;

 attributes.do_not_propagate_mask =  (KeyPressMask | KeyReleaseMask |
                                   ButtonPressMask | ButtonReleaseMask |
                                   PointerMotionMask);

 attributes.cursor = XCreateFontCursor(dpy, XC_watch);

 WaitCursorWindow = XCreateWindow(dpy, win, 0, 0, WidthOfScreen(scn),
                          HeightOfScreen(scn), (unsigned int) 0, CopyFromParent,
                          InputOnly, CopyFromParent, valuemask, &attributes);
} /* SetHints */


/*
** @(#)main() - main routine for the x11 interface to the mail utility
*/
main(argc, argv)
int	argc;
char	**argv;
{
 char	*cp, **aV;
 int	indx, value, aC;

 static XtActionsRec actionTable[] = {
	{"CheckInsert",		(XtActionProc) CheckInsert},
	{"DeleteChar",		(XtActionProc) DeleteChar},
	{"DeleteLine",		(XtActionProc) DeleteLine},
	{"DeleteWord",		(XtActionProc) DeleteWord},
	{"DoCmd",		(XtActionProc) DoCmd},
	{"DoNothing",		(XtActionProc) DoNothing},
	{"DoReply",		(XtActionProc) DoReply},
	{"DoSave",		(XtActionProc) DoSave},
	{"DoSelected",		(XtActionProc) DoSelected},
	{"Iconify",		(XtActionProc) Iconify},
	{"Folder",		(XtActionProc) Folder},
	{"MyNotify",		(XtActionProc) MyNotify},
	{"NextField",		(XtActionProc) NextField},
	{"PrintMsg",		(XtActionProc) PrintMsg},
	{"Quit",		(XtActionProc) Quit},
	{"SetAliases",		(XtActionProc) SetAliases},
	{"SetDirectory",	(XtActionProc) SetDirectory},
	{"SetFolders",		(XtActionProc) SetFolders},
	{"SetMenu",		(XtActionProc) SetMenu},
	{"SetPopup",		(XtActionProc) SetPopup},
	{"SetSelect",		(XtActionProc) SetSelect},
	{"ShowHelp",		(XtActionProc) ShowHelp},
        {NULL, NULL}
      };
/*
** First, examine our command line arguments and Class resources for bogus
** values which would cause the application to operate incorrectly.  Drop
** command lines arguments and/or change the resource values before we
** initialize our widgets.
*/
 aV = (char **) XtMalloc((unsigned) (argc + 1) * sizeof(char *));
 aC = 0;

 for (value = indx = 0; indx < argc; indx++)
      value += strlen(argv[indx]) + 1;
 aV[aC] = (char *) XtMalloc((unsigned) value);

 (void) strcpy(aV[aC++], *argv);
 aV[aC] = aV[aC - 1] + strlen(aV[aC - 1]) + 1;
 for (indx = 1; indx < argc; indx++)
     switch (argv[indx][0]) {		/* look for, -xrm *[Bb]orderWidth or */
         case '-':			/* -bw, w/ negative valued arguments */
              if (argv[indx][1] != 'b' && argv[indx][2] != 'w' &&
                  argv[indx][1] != 'x' && argv[indx][2] != 'r' &&
                  argv[indx][3] != 'm') {
                 (void) strcpy(aV[aC++], argv[indx]);
                 aV[aC] = aV[aC - 1] + strlen(aV[aC - 1]) + 1;
                }
              else if (sscanf(argv[indx+1], "%d", &value) == 1) {
                   if (value < 0)
                      indx++;		/* skip the value argument too */
                   else if (value < 7) {
                           (void) strcpy(aV[aC++], argv[indx]);
                           aV[aC] = aV[aC - 1] + strlen(aV[aC - 1]) + 1;
                          } else {
                           (void) strcpy(aV[aC++], argv[indx++]);
                           aV[aC] = aV[aC - 1] + strlen(aV[aC - 1]) + 1;
                           (void) strcpy(aV[aC++], "6");
                           aV[aC] = aV[aC - 1] + 2;
                          }
                }
              else if ((cp = strchr(argv[indx + 1], 'o')) == NULL ||
                       strncmp(cp, "orderWidth:", 11)     != 0    ||
                       sscanf(cp + 11, "%d", &value)      != 1) {
                      (void) strcpy(aV[aC++], argv[indx]);
                      aV[aC] = aV[aC - 1] + strlen(aV[aC - 1]) + 1;
                     }
              else if (value >= 0) {
                      if (value < 7) {
                         (void) strcpy(aV[aC++], argv[indx]);
                         aV[aC] = aV[aC - 1] + strlen(aV[aC - 1]) + 1;
                        } else {
                         (void) strcpy(aV[aC++], argv[indx++]);
                         aV[aC] = aV[aC - 1] + strlen(aV[aC - 1]) + 1;
                         (void) strcpy(aV[aC], argv[indx]);
                         cp = strchr(aV[aC], ':');
                         *++cp = '\0';
                         (void) strcat(aV[aC], " 6");	/* max allowed value */
                         aV[aC + 1] = aV[aC] + strlen(aV[aC]) + 1;
                         aC++;
                        }
                }
              else indx++;		/* skip the value argument too */
              break;			/* if negative, it is not included */
         default :
              (void) strcpy(aV[aC++], argv[indx]);
              aV[aC] = aV[aC - 1] + strlen(aV[aC - 1]) + 1;
              break;
        }
 aV[aC] = NULL;				/* array MUST end with a null */
/*
** Now, initialize our toplevel widget and parse the command line
*/
 toplevel = XtInitialize(*aV, "XMail", Opts, XtNumber(Opts), &aC, aV);
 if (aC > 1) {
    (void) fprintf(stderr,"Usage: %s [-toolkitoptions] [-xmailoptions]\n",*aV);
    exit(-1);
    /*NOTREACHED*/
   }

 XtFree((char *)aV[0]);				/* we're done we these now */
 XtFree((char *)aV);
/*
** Next, add xmail specific as well as Athena text widget actions
*/
 XtAddActions(actionTable, XtNumber(actionTable));
 XtAddActions(_XawTextActionsTable, _XawTextActionsTableCount);

 XtGetApplicationResources(toplevel, &XMail, resrcs, XtNumber(resrcs), NULL, 0);

 XtOverrideTranslations(toplevel,
                     XtParseTranslationTable("<Message>WM_PROTOCOLS: Quit(q)"));

 initialize();				/* set window sizes based on font */
 CreateSubWindows(toplevel);		/* and build our widget hierarchy */
 XSync(XtDisplay(toplevel), False);	/* ensure everyone's on the same page */
 XtRealizeWidget(toplevel);		/* initialize widgets for our hints */
 SetHints();				/* tell WM about our icon and size */

 mailargc = mailoptions();		/* set up the mail pgm argv and argc */

 callMail(mailargv);			/* attempt to start Mail connection */

 SetCursor(WATCH);

 XtMainLoop();
 /* NOTREACHED */
} /* main */
