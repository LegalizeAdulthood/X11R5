/*
 *	$Id: HZinArea.c,v 1.2 1991/10/05 02:56:31 ygz Exp $
 */

/***********************************************************
Copyright 1990, 1991 by Yongguang Zhang and Man-Chi Pong.
All Rights Reserved.

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of the authors not
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifdef HANZI

#include "ptyx.h"		/* gets Xt headers, too */

#include <X11/Xos.h>
#include "data.h"
#include "error.h"
#include "menu.h"

#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#else
extern char *getenv();
extern char *malloc();
#endif
#if defined(macII) && !defined(__STDC__)  /* stdlib.h fails to define these */
char *malloc();
#endif /* macII */

#include "HZinput.h"
extern HZinputTable *cHZtbl;

static void HZi_ShowPrompt();
static void HZi_ClearArea();
static void HZ_XDrawImageStringMix();

static int	save2ndLineLen = 0;	/* length of 2nd line last drawn */

CreateHZInputArea(xw)
    XtermWidget xw;
{
  register TScreen *screen = &xw->screen;
  register char *cp = getenv ("HZINPUTDIR");

	screen->hzIwin.rows = ROWSINPUTAREA;	/* num of rows in Input Area */
	if (xw->misc.hz_encoding)
		screen->hzIwin.hzEncoding = HZencode (xw->misc.hz_encoding);
	if (!screen->hzIwin.it_dirs && cp) {
		screen->hzIwin.it_dirs = malloc(strlen(cp) + 1);
		if (screen->hzIwin.it_dirs)
			strcpy (screen->hzIwin.it_dirs, cp);
	}
	HZInitInputTable (screen);
}

RealizeHZInputArea(xw)
    XtermWidget xw;
{
	if (xw->misc.hz_mode)
		SetHZMode (&xw->screen, xw->misc.hz_mode, False);
	/* don't draw it now, leave it to the Expose event */
}

ResizeHZInputArea(screen)
    register TScreen *screen;
{
	screen->hzIwin.rule_x = screen->scrollbar;
	screen->hzIwin.rule_y = Height(screen) + screen->border;
	screen->hzIwin.rule_length = Width(screen) + 2 * screen->border;
	screen->hzIwin.x = screen->scrollbar + screen->border;
	screen->hzIwin.y = screen->hzIwin.rule_y + 1;
	screen->hzIwin.width = Width(screen);
	screen->hzIwin.height = FontHeight(screen) * screen->hzIwin.rows;

	RemakeHZInput (screen);
	/* don't redraw, leave it to the Expose event */
}

RefreshHZInputArea(screen)
    register TScreen *screen;
{
	HZi_ClearArea (screen);
	HZi_ShowPrompt (screen);
	HZi_ReshowUserInput (screen);
	HZi_ShowInputChoices (screen);
}

ResetHZInputArea(xw)
    XtermWidget xw;
{
  register TScreen *screen = &xw->screen;

	HZClearInputTable(screen);
	HZInitInputTable(screen);
	RefreshHZInputArea(screen);
}

/**************************************************************
 * The following for drawing in Hanzi Input area,
 * i.e. HZInputWindow:
 */

HZi_ClearHzInput( screen )
	TScreen * screen ;
/*
 * REQUIRE: dpyInbufLen properly set
 *
 * Clear from the end of current HZ input display buffer till
 * the Right Hand Side of window.
 * Also Clear the input choices.
 */
{
 extern int dpyInbufLen;
 int x_offset = (cHZtbl->lenPrompt + dpyInbufLen) * FontWidth(screen) ;

	XClearArea (screen->display, TextWindow(screen),
		screen->hzIwin.x + x_offset,
		screen->hzIwin.y + 1,		/* line top, not line bottom */
		screen->hzIwin.width - x_offset,
		FontHeight(screen),
		False);				/* no Expose event */
	HZi_ClearInputChoice (screen);
}

HZi_ReshowUserInput (screen)
	TScreen * screen ;
/*
 * (Re-)Show the current string of user HZ input.
 *
 * No need XFlush() as must be some procedure(s)
 * which has XFlush() follows this procedure.
 */
{
  extern int dpyInbufLen;
  extern Char dpyInbuf[];

	if (dpyInbufLen == 0)
		return;
	HZ_XDrawImageStringMix (screen->display, TextWindow(screen),
		screen->normalGC, screen->hz_normalGC,
		screen->hzIwin.x + FontWidth(screen) * cHZtbl->lenPrompt,
		screen->hzIwin.y + FontAscent(screen),
		(char *)dpyInbuf, dpyInbufLen,
		FontWidth(screen));
}

HZi_ClearInputChoice (screen)
	TScreen * screen ;
{
	if (save2ndLineLen == 0)
		return;

	/* ClearArea at top of line of char (not bottom) :
	 * 	y + height_of_the_prompt_line
	 * height: number of rows in input area minus the prompt line.
	 */
	XClearArea (screen->display, TextWindow(screen),
		screen->hzIwin.x,
		screen->hzIwin.y + FontHeight(screen),
		FontWidth(screen) * save2ndLineLen,
		FontHeight(screen),
		FALSE);			/* no Expose event */
	save2ndLineLen = 0;
}


HZi_ShowInputChoices (screen)
	TScreen * screen ;
{
  extern Char	dpyChoices[];
  extern int	dpyChoicesLen;

	if (dpyChoicesLen > 0)
		/* input choices are shown at 2nd text line. */
		HZ_XDrawImageStringMix (screen->display, TextWindow(screen),
			screen->normalGC, screen->hz_normalGC,
			screen->hzIwin.x,
			screen->hzIwin.y + FontHeight(screen) + FontAscent(screen),
			(char *)dpyChoices, dpyChoicesLen,
			FontWidth (screen));

	/* In case there is some gabage at the right */
	if (save2ndLineLen > dpyChoicesLen)
		XClearArea (screen->display, TextWindow(screen),
			screen->hzIwin.x + FontWidth(screen) * dpyChoicesLen,
			screen->hzIwin.y + FontHeight(screen),
			FontWidth(screen) * (save2ndLineLen - dpyChoicesLen),
			FontHeight(screen),
			FALSE);

	save2ndLineLen = dpyChoicesLen;
}

/* Temporally display one line of message on the 2nd row of the input area */ 
HZi_ShowMesg (screen, mesgstr)
    TScreen *screen ;
    char *mesgstr;
{
  int len = strlen (mesgstr);

	HZ_XDrawImageStringMix (screen->display, TextWindow(screen),
		screen->normalGC, screen->hz_normalGC,
		screen->hzIwin.x,
		screen->hzIwin.y + FontHeight(screen) + FontAscent(screen),
		mesgstr, strlen (mesgstr),
		FontWidth (screen));
	if (save2ndLineLen > len)
		XClearArea (screen->display, TextWindow(screen),
			screen->hzIwin.x + FontWidth(screen) * len,
			screen->hzIwin.y + FontHeight(screen),
			FontWidth(screen) * (save2ndLineLen - len),
			FontHeight(screen),
			FALSE);
	save2ndLineLen = len;
}

static void
HZi_ShowPrompt( screen )
     TScreen *screen ;
{
	XDrawLine (screen->display, TextWindow(screen),
		screen->hz_normalGC,
		screen->hzIwin.rule_x, screen->hzIwin.rule_y,
		screen->hzIwin.rule_x + screen->hzIwin.rule_length,
		screen->hzIwin.rule_y); 
	HZ_XDrawImageStringMix (screen->display, TextWindow(screen),
		screen->normalGC, screen->hz_normalGC,
		screen->hzIwin.x,
		screen->hzIwin.y + FontAscent(screen),
		(char *)cHZtbl->prompt, cHZtbl->lenPrompt,
		FontWidth(screen));
}

static void
HZi_ClearArea( screen )
    TScreen *screen ;
/*
 * Clear whole HZ input area
 */
{
	XClearArea (screen->display, TextWindow(screen),
		screen->hzIwin.x, screen->hzIwin.y,
		screen->hzIwin.width, screen->hzIwin.height,
		FALSE);			/* no Expose event */
	save2ndLineLen = 0;
}

static void
HZ_XDrawImageStringMix (display, d, gc, hzgc, x, y, string, length, fontwidth)
    Display *display;
    Drawable d;
    GC gc, hzgc;
    int x, y;
    char *string;
    register int length;
    int fontwidth;
{
    register char *ptr = string;
    register int i;

    /* fontwidth is used to compute the width of the string.
     * A more orthogonal way than this is to XQueryTextExtend using gc/hzgc.
     */
    while (length) {
	char *sptr;

	/*
	 * Draw substrings as long as possible to avoid overhead.
	 * But if HZ fontwidth is not 2 * (normal fontwidth), we have to
	 * draw them one by one. Suck. I am not going to do this. -YGZ
	 */
	sptr = ptr;
	i = 0;
	while (!(*ptr & 0x80) && length) {
	    ptr++; i++; length --;
	}
	if (ptr != sptr) {
	    XDrawImageString (display, d, gc, x, y, (char *)sptr, i);
	    x += fontwidth * i;
	}

	sptr = ptr;
	i = 0;
	while ((*ptr & 0x80) && length) {
	    if (--length) {
		ptr++; ptr++; i++; length--;
		/* should check 2nd byte according to GB or BIG5 */
	    } else
		break;
	}
	if (ptr != sptr) {
	    XDrawImageString16 (display, d, hzgc, x, y, (XChar2b *)sptr, i);
	    x += (fontwidth * 2) * i;
	}
    }
}

#endif /* HANZI */
