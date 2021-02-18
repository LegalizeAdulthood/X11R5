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
*/


#include <stdio.h>
#if XtSpecificationRelease > 4
#else
#include <sys/types.h>
#endif
#include <sys/param.h>
#include <X11/Xos.h>

#if XtSpecificationRelease > 4
#ifndef	X_NOT_STDC_ENV
#include <stdlib.h>
#else
char	*malloc(), *realloc(), *calloc();
#endif
#if defined(macII) && !defined(__STDC__)   /* stdlib.h fails to define these */
char	*malloc(), *realloc(), *calloc();
#endif	/* macII */
#endif

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/TextP.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Label.h>

#if XtSpecificationRelease > 4
#include <X11/Xfuncs.h>			/* for bcopy, et.al. */
#else
#define	_XawTextActionsTable		textActionsTable
#define	_XawTextActionsTableCount	textActionsTableCount
#endif

#define	TITLE		"xmail 1."	/* program title and version string */
#define	StartPos	 6		/* size of 'File: ' (del stop point) */
#define	LASTCH(s)	(s[ *s ? strlen(s) - 1 : 0])
#define	CHARHEIGHT(X)	(X->max_bounds.descent + X->max_bounds.ascent)
#define	APPEND		True		/* used in writeTo function logic */
#define	REPLACE		False
#define	WATCH		True		/* used in SetCursor function logic */
#define	NORMAL		False

#ifndef	DEFAULT_MAILER
#define	DEFAULT_MAILER	"/usr/ucb/Mail"
#endif

#if defined(SYSV) && !defined(SIGCHLD)
#define	SIGCHLD		SIGCLD
#endif

typedef struct {
    XFontStruct	*textFont;		/* font to use when displaying text */
    XFontStruct	*helpFont;		/* font to use when displaying help */
    XFontStruct	*buttonFont;		/* font to use when making buttons */
    String	iconGeometry;		/* xmail icon geometry */
    String	MFileName;		/* mail option -f filename */
    String	editorCommand;		/* optional xmail send edit command */
    Dimension	shellWidth;		/* xmail window width */
    Dimension	fileBoxWidth;		/* file window box width */
    Dimension	indexHeight;		/* index window height */
    Dimension	textHeight;		/* text window height */
    Dimension	buttonWidth;		/* command button width */
    Dimension	buttonHeight;		/* command button height */
    Dimension	commandHSpace;		/* command horizontal spacing */
    Dimension	commandVSpace;		/* command vertical spacing */
    Dimension	commandHeight;		/* command window height */
    Dimension	helpWidth;		/* width of the help text popup */
    Dimension	helpHeight;		/* height of the help text popup */
    Dimension	helpX;			/* help x offset from textWindow */
    Dimension	helpY;			/* help y offset from textWindow */
    Dimension	menuX;			/* menu x offset from parent */
    Dimension	menuY;			/* menu y offset from parent */
    Boolean	bellRing;		/* xmail audible bell option */
    Boolean	expert;			/* do not confirm destructive acts */
    Boolean	iconic;			/* xmail starts in withdrawn state */
    Boolean	mailopt_n;		/* mail option -n */
    Boolean	mailopt_U;		/* mail option -U */
    Boolean	Show_Last;		/* xmail show latest option -ls */
    Boolean	Show_Info;		/* Do (or don't) show info messages */
    Boolean	No_X_Hdr;		/* don't show xface header option -nx */
    int		borderWidth;		/* default or specified border width */
} XmailResources;


/*
** structure sent to AddMenuButton()
*/
typedef struct	menuList_str
	{
	String		label;
	XtCallbackProc	func;
	String		data;
	} menuList, *menuList_p;

/*
** structure used by AddHelpText() and ShowHelp()
*/
typedef struct	helpList_str
	{
	int		indx;
	String		name[27];
	Widget		text[27];
	} helpList, *helpList_p;


typedef struct {
    char			*pat;		/* regular expression */
    char			*buf;		/* buffer for compiled regexp */
} PatternRec, *PatternRecPtr;


typedef struct {
    char			*name;		/* recipient name */
    char			*alias;		/* alias for name */
} AliasRec, *AliasRecPtr;
