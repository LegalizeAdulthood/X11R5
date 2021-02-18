#ifndef lint
     static char *rcsid = "$Header: /usr3/emu/client/RCS/main.c,v 1.12 91/08/17 10:17:09 jkh Exp Locker: me $";
#endif

/*
 * This file is part of the PCS emu program.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * This is the new "xterm" program. It uses the new term widget to do most
 * of its work. One file; not bad, eh? :-)
 *
 * Author: Jordan K. Hubbard
 * Date: July 26th, 1990.
 * Description: The canonical term widget client program. If nothing else,
 *		a good example of how to use the term and termCanvas widgets
 *		in a complete application.
 *
 * Revision History:
 *
 * $Log:	main.c,v $
 * Revision 1.12  91/08/17  10:17:09  jkh
 * Added a lot more command line flags.
 * 
 * Revision 1.11  91/04/22  11:26:35  me
 * Whatever Jordan did
 * 
 * Revision 1.10  90/11/20  17:36:45  jkh
 * Alpha.
 * 
 * Revision 1.1  90/07/26  02:57:12  jkh
 * Initial revision
 * 
 */

#include "client.h"
#include <signal.h>

/* We want these global */
Export Widget Term, SBar, MBar, Canvas;

typedef struct _AppResources {
     Boolean menu_bar, scroll_bar, show_usage, is_console;
     String command, title, icon_name, term_type;
} AppResourcesRec, *AppResources;

static AppResourcesRec appResources;

static XrmOptionDescRec optionList[] = {
     {"-mbar",	"*menuBar",		XrmoptionNoArg,  "True"		},
     {"+mbar",	"*menuBar",		XrmoptionNoArg,  "False"	},
     {"-sbar",	"*scrollBar",		XrmoptionNoArg,  "True"		},
     {"+sbar",	"*scrollBar",		XrmoptionNoArg,	 "False"	},
     {"-C",	"*console",		XrmoptionNoArg,  "True"		},
     {"+C",	"*console",		XrmoptionNoArg,  "False"	},
     {"-help",	"*help",		XrmoptionNoArg,  "True"		},
     {"-e",	"*command",		XrmoptionSepArg, NULL		},
     {"-term",	"*termType",		XrmoptionSepArg, NULL		},
     {"-rs",	"*readSize",		XrmoptionSepArg, (caddr_t)0	},
     {"-ut",	"*utmpInhibit",		XrmoptionNoArg,	 "True"		},
     {"+ut",	"*utmpInhibit",		XrmoptionNoArg,	 "False"	},
     {"-ls",	"*loginShell",		XrmoptionNoArg,	 "True"		},
     {"+ls",	"*loginShell",		XrmoptionNoArg,	 "False"	},
     {"-T",	"*title",		XrmoptionSepArg, NULL		},
     {"-n",	"*iconName",		XrmoptionSepArg, NULL		},
     /* Screen specific resources */
     {"-ul",	"*screen.underlineWidth", XrmoptionSepArg, (caddr_t)1	},
     {"-cfg",	"*screen.cursorFg",	XrmoptionSepArg, XtDefaultForeground },
     {"-cbg",	"*screen.cursorBg",	XrmoptionSepArg, XtDefaultBackground },
     {"-ch",	"*screen.cursorHeight",	XrmoptionSepArg, (caddr_t)0	},
     {"-cw",	"*screen.cursorWidth",	XrmoptionSepArg, (caddr_t)0	},
     {"-cbl",	"*screen.cursorBlinking", XrmoptionSepArg, "True"	},
     {"+cbl",	"*screen.cursorBlinking", XrmoptionSepArg, "False"	},
     {"-cbival","*screen.blinkInterval", XrmoptionSepArg, (caddr_t)500	},
     {"-mclick","*screen.multiClickTime", XrmoptionSepArg, (caddr_t)300	},
     {"-tbival","*screen.textBlinkInterval", XrmoptionSepArg, (caddr_t)500, },
     {"-bfocus","*screen.blinkWOFocus",	XrmoptionSepArg, "False",	},
     {"+bfocus","*screen.blinkWOFocus",	XrmoptionSepArg, "True",	},
     {"-pcol",	"*screen.pointerColor",	XrmoptionSepArg, XtDefaultForeground },
     {"-pcur",	"*screen.pointerShape",	XrmoptionSepArg, NULL		},
     {"-wrap",	"*screen.wrapAround",	XrmoptionSepArg, "True",	},
     {"+wrap",	"*screen.wrapAround",	XrmoptionSepArg, "False",	},
     {"-insert","*screen.insertMode",	XrmoptionSepArg, "False",	},
     {"+insert","*screen.insertMode",	XrmoptionSepArg, "True",	},
     {"-bell",	"*screen.bellVolume",	XrmoptionSepArg, (caddr_t)100	},
     {"-tab",	"*screen.defTabWidth",	XrmoptionSepArg, (caddr_t)8	},
     {"-sl",	"*screen.saveLines",	XrmoptionSepArg, (caddr_t)0	},
     {"-fn",	"*screen.font",		XrmoptionSepArg,  NULL		},
     {"-fb",	"*screen.boldFont",	XrmoptionSepArg,  NULL		},
};

#define offset(field) XtOffset(struct _AppResources *, field)

/* Just the resources we're directly interested in */
static XtResource appResourceList[] = {
     { "menuBar", XtCBoolean, XtRBoolean, sizeof(Boolean),
	    offset(menu_bar), XtRString, "False",
     },
     { "scrollBar", XtCBoolean, XtRBoolean, sizeof(Boolean),
	    offset(scroll_bar), XtRString, "False",
     },
     { "console", XtCBoolean, XtRBoolean, sizeof(Boolean),
	    offset(is_console), XtRString, "False",
     },
     { "title",	"Title", XtRString, sizeof(String),
	    offset(title), XtRImmediate, NULL,
     },
     { "iconName", "IconName", XtRString, sizeof(String),
	    offset(icon_name), XtRImmediate, NULL,
     },
     { XpNcommand, XpCCommand, XtRString, sizeof(String),
	    offset(command), XtRImmediate, NULL,
     },
     { XpNtermType, XpCTermType, XtRString, sizeof(String),
	    offset(term_type), XtRImmediate, NULL,
     },
     { "help", XtCBoolean, XtRBoolean, sizeof(Boolean),
	    offset(show_usage), XtRImmediate, FALSE,
     },
};

/* Our process death handler. Just exits */
void
do_exit(w, pid, fail)
Widget w;
int pid, fail;
{
     exit(0);
}

Local void
usage(argc, argv)
char**argv;
int argc;
{
     int i;
     
     static char *syntax[] = {
	  "-display dispname",	"Selects X server to use",
	  "-geometry geom",	"Specify window geometry in characters",
	  "-sl nlines",		"Indicates how many scroll lines to save",
	  "-rs nbytes",		"Sets the term widget's read size.",
	  "-e command",		"Command to execute on startup",
	  "-term termtype",	"Terminal type to emulate",
	  "-fn fontname",	"Normal font",
	  "-fb fontname",	"Bold font",
	  "-T title",		"Set window title string",
	  "-n icon_name",	"Set icon name for window",
	  "-name string",	"Set application name, icon and title strings",
	  "-/+rv",		"Turn on (off) reverse video",
	  "-help",		"Prints this output",
	  "-/+mbar",		"Selects (unselects) menu bar option",
	  "-/+sbar",		"Adds (removes) a vertical scrollbar",
	  "-/+C",		"Turns on (off) console mode",
	  "-/+ut",		"Add (don't add) utmp entry for session",
	  "-/+ls",		"Turn on (off) login shell handling",
	  "-ul ulwidth",	"Width of an underline",
	  "-cfg color",		"Cursor foreground color",
	  "-cbg color",		"Cursor background color",
	  "-cbival interval",	"Set cursor blink interval in milliseconds",
	  "-cw width",		"Set cursor width in pixels",
	  "-ch height",		"Set cursor height in pixels",
	  "-/+cbl",		"Turn off (on) cursor blink",
	  "-/+bfocus",		"Turn off (on) blinking cursor without focus",
	  "-mclick interval",	"Set multi-click interval in milliseconds",
	  "-tbival interval",	"Set text blink interval in milliseconds",
	  "-pcol color",	"Set pointer color",
	  "-pcur cursor",	"Set pointer cursor",
	  "-/+wrap",		"Enable (disable) text wrap",
	  "-/+insert",		"Disable (enable) insert mode",
	  "-bell volume",	"Set bell volume as a percentage",
	  "-tab n",		"Set tabstops to be every <n> characters",
	  NULL,			NULL,
     };
     static char **usage = syntax;
     
     if (argc > 1) {
	  printf("%s: unknown arguments: ", argv[0]);
	  for (i = 1; i < argc ; i++)
	       printf("%s ", argv[i]);
     }
     printf("\n%s takes the following arguments:\n", argv[0]);
     while (*usage != NULL) {
	  printf("%-30s - %s\n", usage[0], usage[1]);
	  usage += 2;
     }
     exit (0);
}

Export int
main(argc, argv)
int argc;
char **argv;
{
     int i;
     Dimension w_inc, h_inc;
     Arg args[15];
     char *cp;
     Widget top;
     Import char *getenv();
     Import void rparse(), tty_set_size();
     Dimension base_width = 0, base_height = 0;

     top = XtInitialize(argv[0], "Emu", optionList, XtNumber(optionList),
			&argc, argv);
     
     XtGetApplicationResources(top, (XtPointer)&appResources,
			       appResourceList, XtNumber(appResourceList),
			       NULL, (Cardinal)0);
     
     if ((argc != 1 && !appResources.command) || (appResources.show_usage))
	  usage(argc, argv);

     i = 0;
     XtSetArg(args[i], XpNlayoutProc, do_layout);		i++;
     if (!appResources.command) {
	  /* If no command, select shell by default */
	  if ((cp = getenv("SHELL")) == NULL)
	       cp = XpNdefaultCommand;
	  XtSetArg(args[i], XpNcommand, cp);			i++;
     }
     else if (argc != 1) {
	  /* We have some arguments to the command */
	  XtSetArg(args[i], XpNcommandArgs, argv);		i++;
     }
     if (!appResources.term_type) {
	  /* If no term type, use argv[0] */
	  XtSetArg(args[i], XpNtermType, argv[0]);		i++;
     }
     XtSetArg(args[i], XtNborderWidth, 0);			i++;
     Term = XtCreateManagedWidget("term", termWidgetClass,
				   top, args, i);
     XtAddCallback(Term, XpNprocessDeath, do_exit, NULL);

     i = 0;
     XtSetArg(args[i], XtNborderWidth, 0);			i++;
     XtSetArg(args[i], XpNoutput, rparse);			i++;
     XtSetArg(args[i], XpNsetSize, tty_set_size);		i++;
     XtSetArg(args[i], XpNcomBlock, XpTermComBlock(Term));	i++;
     if (appResources.scroll_bar) {
	  XtSetArg(args[i], XpNadjustScrollBar, ScrollbarAdjust); i++;
     }
     Canvas = XtCreateManagedWidget("screen", termCanvasWidgetClass,
				    Term, args, i);

     /* If we want a scrollbar, now's the time to whap it up */
     if (appResources.scroll_bar)
	  SBar = ScrollbarCreate(Term, &base_width);

     /* Wake up the menus */
     MBar = XpEmuInitializeMenus(Term, appResources.menu_bar, &base_height);

     /*
      * Get the cell width and height from the canvas and pass them
      * to the term widget, which will in turn automatically pass them
      * up to the shell.
      */
     i = 0;
     XtSetArg(args[i], XpNcellWidth, &w_inc);			i++;
     XtSetArg(args[i], XpNcellHeight, &h_inc);			i++;
     XtGetValues(Canvas, args, i);

     i = 0;
     XtSetArg(args[i], XtNwidthInc, w_inc);			i++;
     XtSetArg(args[i], XtNheightInc, h_inc);			i++;
     XtSetValues(Term, args, i);

     /* Set base_width and base_height, let the shell resize as needed */
     i = 0;
     XtSetArg(args[i], XtNbaseWidth,  base_width);		i++;
     XtSetArg(args[i], XtNbaseHeight, base_height);		i++;
     XtSetArg(args[i], XtNallowShellResize, TRUE);		i++;
     XtSetArg(args[i], XtNinput, TRUE);				i++;
     if (appResources.title) {
	  XtSetArg(args[i], XtNtitle, appResources.title);	i++;
     }
     if (appResources.icon_name) {
	  XtSetArg(args[i], XtNiconName, appResources.icon_name); i++;
     }
     XtSetValues(top, args, i);

     /* Zark it up there */
     XtRealizeWidget(top);

     XtMainLoop();
     
     /* Keep GCC happy */
     return 0;
}
