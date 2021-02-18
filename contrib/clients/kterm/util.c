/*
 *	$XConsortium: util.c,v 1.31 91/06/20 18:34:47 gildea Exp $
 *	$Id: util.c,v 5.0 1991/10/05 08:27:08 kagotani Rel $
 */

/*
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.
 *
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/* util.c */

#include "ptyx.h"
#include "data.h"
#include "error.h"
#include "menu.h"

#include <stdio.h>

static void horizontal_copy_area();
static void vertical_copy_area();

#ifdef KTERM
static int fnum = F_ISO8859_1; /* refered by GC's in many functions */
#endif /* KTERM */

/*
 * These routines are used for the jump scroll feature
 */
FlushScroll(screen)
register TScreen *screen;
{
	register int i;
	register int shift = -screen->topline;
	register int bot = screen->max_row - shift;
	register int refreshtop;
	register int refreshheight;
	register int scrolltop;
	register int scrollheight;

	if(screen->cursor_state)
		HideCursor();
	if(screen->scroll_amt > 0) {
		refreshheight = screen->refresh_amt;
		scrollheight = screen->bot_marg - screen->top_marg -
		 refreshheight + 1;
		if((refreshtop = screen->bot_marg - refreshheight + 1 + shift) >
		 (i = screen->max_row - screen->scroll_amt + 1))
			refreshtop = i;
		if(screen->scrollWidget && !screen->alternate
		 && screen->top_marg == 0) {
			scrolltop = 0;
			if((scrollheight += shift) > i)
				scrollheight = i;
			if((i = screen->bot_marg - bot) > 0 &&
			 (refreshheight -= i) < screen->scroll_amt)
				refreshheight = screen->scroll_amt;
			if((i = screen->savedlines) < screen->savelines) {
				if((i += screen->scroll_amt) >
				  screen->savelines)
					i = screen->savelines;
				screen->savedlines = i;
				ScrollBarDrawThumb(screen->scrollWidget);
			}
		} else {
			scrolltop = screen->top_marg + shift;
			if((i = bot - (screen->bot_marg - screen->refresh_amt +
			 screen->scroll_amt)) > 0) {
				if(bot < screen->bot_marg)
					refreshheight = screen->scroll_amt + i;
			} else {
				scrollheight += i;
				refreshheight = screen->scroll_amt;
				if((i = screen->top_marg + screen->scroll_amt -
				 1 - bot) > 0) {
					refreshtop += i;
					refreshheight -= i;
				}
			}
		}
	} else {
		refreshheight = -screen->refresh_amt;
		scrollheight = screen->bot_marg - screen->top_marg -
		 refreshheight + 1;
		refreshtop = screen->top_marg + shift;
		scrolltop = refreshtop + refreshheight;
		if((i = screen->bot_marg - bot) > 0)
			scrollheight -= i;
		if((i = screen->top_marg + refreshheight - 1 - bot) > 0)
			refreshheight -= i;
	}
	scrolling_copy_area(screen, scrolltop+screen->scroll_amt,
			    scrollheight, screen->scroll_amt);
	ScrollSelection(screen, -(screen->scroll_amt));
	screen->scroll_amt = 0;
	screen->refresh_amt = 0;
	if(refreshheight > 0) {
		XClearArea (
		    screen->display,
		    TextWindow(screen),
		    (int) screen->border + screen->scrollbar,
		    (int) refreshtop * FontHeight(screen) + screen->border,
		    (unsigned) Width(screen),
		    (unsigned) refreshheight * FontHeight(screen),
		    FALSE);
		ScrnRefresh(screen, refreshtop, 0, refreshheight,
		 screen->max_col + 1, False);
	}
}

AddToRefresh(screen)
register TScreen *screen;
{
	register int amount = screen->refresh_amt;
	register int row = screen->cur_row;

#ifdef STATUSLINE
	if(screen->instatus)
		return(0);
#endif /* STATUSLINE */
	if(amount == 0)
		return(0);
	if(amount > 0) {
		register int bottom;

		if(row == (bottom = screen->bot_marg) - amount) {
			screen->refresh_amt++;
			return(1);
		}
		return(row >= bottom - amount + 1 && row <= bottom);
	} else {
		register int top;

		amount = -amount;
		if(row == (top = screen->top_marg) + amount) {
			screen->refresh_amt--;
			return(1);
		}
		return(row <= top + amount - 1 && row >= top);
	}
}

/* 
 * scrolls the screen by amount lines, erases bottom, doesn't alter 
 * cursor position (i.e. cursor moves down amount relative to text).
 * All done within the scrolling region, of course. 
 * requires: amount > 0
 */
Scroll(screen, amount)
register TScreen *screen;
register int amount;
{
	register int i = screen->bot_marg - screen->top_marg + 1;
	register int shift;
	register int bot;
	register int refreshtop = 0;
	register int refreshheight;
	register int scrolltop;
	register int scrollheight;

	if(screen->cursor_state)
		HideCursor();
	if (amount > i)
		amount = i;
    if(screen->jumpscroll) {
	if(screen->scroll_amt > 0) {
		if(screen->refresh_amt + amount > i)
			FlushScroll(screen);
		screen->scroll_amt += amount;
		screen->refresh_amt += amount;
	} else {
		if(screen->scroll_amt < 0)
			FlushScroll(screen);
		screen->scroll_amt = amount;
		screen->refresh_amt = amount;
	}
	refreshheight = 0;
    } else {
	ScrollSelection(screen, -(amount));
	if (amount == i) {
		ClearScreen(screen);
		return;
	}
	shift = -screen->topline;
	bot = screen->max_row - shift;
	scrollheight = i - amount;
	refreshheight = amount;
	if((refreshtop = screen->bot_marg - refreshheight + 1 + shift) >
	 (i = screen->max_row - refreshheight + 1))
		refreshtop = i;
	if(screen->scrollWidget && !screen->alternate
	 && screen->top_marg == 0) {
		scrolltop = 0;
		if((scrollheight += shift) > i)
			scrollheight = i;
		if((i = screen->savedlines) < screen->savelines) {
			if((i += amount) > screen->savelines)
				i = screen->savelines;
			screen->savedlines = i;
			ScrollBarDrawThumb(screen->scrollWidget);
		}
	} else {
		scrolltop = screen->top_marg + shift;
		if((i = screen->bot_marg - bot) > 0) {
			scrollheight -= i;
			if((i = screen->top_marg + amount - 1 - bot) >= 0) {
				refreshtop += i;
				refreshheight -= i;
			}
		}
	}

	if (screen->multiscroll && amount == 1 &&
	    screen->topline == 0 && screen->top_marg == 0 &&
	    screen->bot_marg == screen->max_row) {
	    if (screen->incopy < 0 && screen->scrolls == 0)
		CopyWait(screen);
	    screen->scrolls++;
	}
	scrolling_copy_area(screen, scrolltop+amount, scrollheight, amount);
	if(refreshheight > 0) {
		XClearArea (
		   screen->display,
		   TextWindow(screen),
		   (int) screen->border + screen->scrollbar,
		   (int) refreshtop * FontHeight(screen) + screen->border,
		   (unsigned) Width(screen),
		   (unsigned) refreshheight * FontHeight(screen),
		   FALSE);
		if(refreshheight > shift)
			refreshheight = shift;
	}
    }
	if(screen->scrollWidget && !screen->alternate && screen->top_marg == 0)
		ScrnDeleteLine(screen->allbuf, screen->bot_marg +
		 screen->savelines, 0, amount, screen->max_col + 1);
	else
		ScrnDeleteLine(screen->buf, screen->bot_marg, screen->top_marg,
		 amount, screen->max_col + 1);
	if(refreshheight > 0)
		ScrnRefresh(screen, refreshtop, 0, refreshheight,
		 screen->max_col + 1, False);
}


/*
 * Reverse scrolls the screen by amount lines, erases top, doesn't alter
 * cursor position (i.e. cursor moves up amount relative to text).
 * All done within the scrolling region, of course.
 * Requires: amount > 0
 */
RevScroll(screen, amount)
register TScreen *screen;
register int amount;
{
	register int i = screen->bot_marg - screen->top_marg + 1;
	register int shift;
	register int bot;
	register int refreshtop;
	register int refreshheight;
	register int scrolltop;
	register int scrollheight;

	if(screen->cursor_state)
		HideCursor();
	if (amount > i)
		amount = i;
    if(screen->jumpscroll) {
	if(screen->scroll_amt < 0) {
		if(-screen->refresh_amt + amount > i)
			FlushScroll(screen);
		screen->scroll_amt -= amount;
		screen->refresh_amt -= amount;
	} else {
		if(screen->scroll_amt > 0)
			FlushScroll(screen);
		screen->scroll_amt = -amount;
		screen->refresh_amt = -amount;
	}
    } else {
	shift = -screen->topline;
	bot = screen->max_row - shift;
	refreshheight = amount;
	scrollheight = screen->bot_marg - screen->top_marg -
	 refreshheight + 1;
	refreshtop = screen->top_marg + shift;
	scrolltop = refreshtop + refreshheight;
	if((i = screen->bot_marg - bot) > 0)
		scrollheight -= i;
	if((i = screen->top_marg + refreshheight - 1 - bot) > 0)
		refreshheight -= i;

	if (screen->multiscroll && amount == 1 &&
	    screen->topline == 0 && screen->top_marg == 0 &&
	    screen->bot_marg == screen->max_row) {
	    if (screen->incopy < 0 && screen->scrolls == 0)
		CopyWait(screen);
	    screen->scrolls++;
	}
	scrolling_copy_area(screen, scrolltop-amount, scrollheight, -amount);
	if(refreshheight > 0)
		XClearArea (
		    screen->display,
		    TextWindow(screen),
		    (int) screen->border + screen->scrollbar,
		    (int) refreshtop * FontHeight(screen) + screen->border,
		    (unsigned) Width(screen),
		    (unsigned) refreshheight * FontHeight(screen),
		    FALSE);
    }
	ScrnInsertLine (screen->buf, screen->bot_marg, screen->top_marg,
			amount, screen->max_col + 1);
}

/*
 * If cursor not in scrolling region, returns.  Else,
 * inserts n blank lines at the cursor's position.  Lines above the
 * bottom margin are lost.
 */
InsertLine (screen, n)
register TScreen *screen;
register int n;
{
	register int i;
	register int shift;
	register int bot;
	register int refreshtop;
	register int refreshheight;
	register int scrolltop;
	register int scrollheight;

	if (screen->cur_row < screen->top_marg ||
	 screen->cur_row > screen->bot_marg)
		return;
	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
	if (n > (i = screen->bot_marg - screen->cur_row + 1))
		n = i;
    if(screen->jumpscroll) {
	if(screen->scroll_amt <= 0 &&
	 screen->cur_row <= -screen->refresh_amt) {
		if(-screen->refresh_amt + n > screen->max_row + 1)
			FlushScroll(screen);
		screen->scroll_amt -= n;
		screen->refresh_amt -= n;
	} else if(screen->scroll_amt)
		FlushScroll(screen);
    }
    if(!screen->scroll_amt) {
	shift = -screen->topline;
	bot = screen->max_row - shift;
	refreshheight = n;
	scrollheight = screen->bot_marg - screen->cur_row - refreshheight + 1;
	refreshtop = screen->cur_row + shift;
	scrolltop = refreshtop + refreshheight;
	if((i = screen->bot_marg - bot) > 0)
		scrollheight -= i;
	if((i = screen->cur_row + refreshheight - 1 - bot) > 0)
		refreshheight -= i;
	vertical_copy_area(screen, scrolltop-n, scrollheight, -n);
	if(refreshheight > 0)
		XClearArea (
		    screen->display,
		    TextWindow(screen),
		    (int) screen->border + screen->scrollbar,
		    (int) refreshtop * FontHeight(screen) + screen->border,
		    (unsigned) Width(screen),
		    (unsigned) refreshheight * FontHeight(screen),
		    FALSE);
    }
	/* adjust screen->buf */
	ScrnInsertLine(screen->buf, screen->bot_marg, screen->cur_row, n,
			screen->max_col + 1);
}

/*
 * If cursor not in scrolling region, returns.  Else, deletes n lines
 * at the cursor's position, lines added at bottom margin are blank.
 */
DeleteLine(screen, n)
register TScreen *screen;
register int n;
{
	register int i;
	register int shift;
	register int bot;
	register int refreshtop;
	register int refreshheight;
	register int scrolltop;
	register int scrollheight;

	if (screen->cur_row < screen->top_marg ||
	 screen->cur_row > screen->bot_marg)
		return;
	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
	if (n > (i = screen->bot_marg - screen->cur_row + 1))
		n = i;
    if(screen->jumpscroll) {
	if(screen->scroll_amt >= 0 && screen->cur_row == screen->top_marg) {
		if(screen->refresh_amt + n > screen->max_row + 1)
			FlushScroll(screen);
		screen->scroll_amt += n;
		screen->refresh_amt += n;
	} else if(screen->scroll_amt)
		FlushScroll(screen);
    }
    if(!screen->scroll_amt) {

	shift = -screen->topline;
	bot = screen->max_row - shift;
	scrollheight = i - n;
	refreshheight = n;
	if((refreshtop = screen->bot_marg - refreshheight + 1 + shift) >
	 (i = screen->max_row - refreshheight + 1))
		refreshtop = i;
	if(screen->scrollWidget && !screen->alternate && screen->cur_row == 0) {
		scrolltop = 0;
		if((scrollheight += shift) > i)
			scrollheight = i;
		if((i = screen->savedlines) < screen->savelines) {
			if((i += n) > screen->savelines)
				i = screen->savelines;
			screen->savedlines = i;
			ScrollBarDrawThumb(screen->scrollWidget);
		}
	} else {
		scrolltop = screen->cur_row + shift;
		if((i = screen->bot_marg - bot) > 0) {
			scrollheight -= i;
			if((i = screen->cur_row + n - 1 - bot) >= 0) {
				refreshheight -= i;
			}
		}
	}
	vertical_copy_area(screen, scrolltop+n, scrollheight, n);
	if(refreshheight > 0)
		XClearArea (
		    screen->display,
		    TextWindow(screen),
		    (int) screen->border + screen->scrollbar,
		    (int) refreshtop * FontHeight(screen) + screen->border,
		    (unsigned) Width(screen),
		    (unsigned) refreshheight * FontHeight(screen),
		    FALSE);
    }
	/* adjust screen->buf */
	if(screen->scrollWidget && !screen->alternate && screen->cur_row == 0)
		ScrnDeleteLine(screen->allbuf, screen->bot_marg +
		 screen->savelines, 0, n, screen->max_col + 1);
	else
		ScrnDeleteLine(screen->buf, screen->bot_marg, screen->cur_row,
		 n, screen->max_col + 1);
}

/*
 * Insert n blanks at the cursor's position, no wraparound
 */
InsertChar (screen, n)
    register TScreen *screen;
    register int n;
{
        register int cx, cy;

	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
#ifdef STATUSLINE
	if(screen->cur_row - screen->topline <= screen->max_row ||
	   screen->instatus) {
#else /* !STATUSLINE */
	if(screen->cur_row - screen->topline <= screen->max_row) {
#endif /* !STATUSLINE */
	    if(!AddToRefresh(screen)) {
		if(screen->scroll_amt)
			FlushScroll(screen);
#ifdef KTERM_MBCS
		BreakMBchar(screen);
#endif /* KTERM_MBCS */

		/*
		 * prevent InsertChar from shifting the end of a line over
		 * if it is being appended to
		 */
		if (non_blank_line (screen->buf, screen->cur_row, 
				    screen->cur_col, screen->max_col + 1))
		    horizontal_copy_area(screen, screen->cur_col,
					 screen->max_col+1 - (screen->cur_col+n),
					 n);
	
		cx = CursorX (screen, screen->cur_col);
		cy = CursorY (screen, screen->cur_row);

		XFillRectangle(
		    screen->display,
		    TextWindow(screen), 
#ifdef STATUSLINE
		    screen->instatus && screen->reversestatus ?
		    screen->normalGC :
#endif /* STATUSLINE */
		    screen->reverseGC,
		    cx, cy,
		    (unsigned) n * FontWidth(screen), (unsigned) FontHeight(screen));
	    }
	}
	/* adjust screen->buf */
	ScrnInsertChar(screen->buf, screen->cur_row, screen->cur_col, n,
			screen->max_col + 1);
}

/*
 * Deletes n chars at the cursor's position, no wraparound.
 */
DeleteChar (screen, n)
    register TScreen *screen;
    register int	n;
{
	register int width;

	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
	if (n > (width = screen->max_col + 1 - screen->cur_col))
	  	n = width;
		
#ifdef STATUSLINE
	if(screen->cur_row - screen->topline <= screen->max_row ||
	   screen->instatus) {
#else /* !STATUSLINE */
	if(screen->cur_row - screen->topline <= screen->max_row) {
#endif /* !STATUSLINE */
	    if(!AddToRefresh(screen)) {
		if(screen->scroll_amt)
			FlushScroll(screen);
#ifdef KTERM_MBCS
		BreakMBchar(screen);
		screen->cur_col += n;
		BreakMBchar(screen);
		screen->cur_col -= n;
#endif /* KTERM_MBCS */
	
		horizontal_copy_area(screen, screen->cur_col+n,
				     screen->max_col+1 - (screen->cur_col+n),
				     -n);
	
		XFillRectangle
		    (screen->display, TextWindow(screen),
#ifdef STATUSLINE
		     screen->instatus && screen->reversestatus ?
		     screen->normalGC :
#endif /* STATUSLINE */
		     screen->reverseGC,
		     screen->border + screen->scrollbar
		       + Width(screen) - n*FontWidth(screen),
		     CursorY (screen, screen->cur_row), n * FontWidth(screen),
		     FontHeight(screen));
	    }
	}
	/* adjust screen->buf */
	ScrnDeleteChar (screen->buf, screen->cur_row, screen->cur_col, n,
			screen->max_col + 1);

}

/*
 * Clear from cursor position to beginning of display, inclusive.
 */
ClearAbove (screen)
register TScreen *screen;
{
	register top, height;

	if(screen->cursor_state)
		HideCursor();
	if((top = -screen->topline) <= screen->max_row) {
		if(screen->scroll_amt)
			FlushScroll(screen);
		if((height = screen->cur_row + top) > screen->max_row)
			height = screen->max_row;
		if((height -= top) > 0)
			XClearArea(screen->display, TextWindow(screen),
			 screen->border + screen->scrollbar, top *
			 FontHeight(screen) + screen->border,
			 Width(screen), height * FontHeight(screen), FALSE);

		if(screen->cur_row - screen->topline <= screen->max_row)
			ClearLeft(screen);
	}
	ClearBufRows(screen, 0, screen->cur_row - 1);
}

/*
 * Clear from cursor position to end of display, inclusive.
 */
ClearBelow (screen)
register TScreen *screen;
{
	register top;

	ClearRight(screen);
	if((top = screen->cur_row - screen->topline) <= screen->max_row) {
		if(screen->scroll_amt)
			FlushScroll(screen);
		if(++top <= screen->max_row)
			XClearArea(screen->display, TextWindow(screen),
			 screen->border + screen->scrollbar, top *
			 FontHeight(screen) + screen->border,
			 Width(screen), (screen->max_row - top + 1) *
			 FontHeight(screen), FALSE);
	}
	ClearBufRows(screen, screen->cur_row + 1, screen->max_row);
}

/* 
 * Clear last part of cursor's line, inclusive.
 */
ClearRight (screen)
register TScreen *screen;
{
	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
#ifdef STATUSLINE
	if(screen->cur_row - screen->topline <= screen->max_row ||
	   screen->instatus) {
#else /* !STATUSLINE */
	if(screen->cur_row - screen->topline <= screen->max_row) {
#endif /* !STATUSLINE */
	    if(!AddToRefresh(screen)) {
	if(screen->scroll_amt)
		FlushScroll(screen);
#ifdef KTERM_MBCS
		BreakMBchar(screen);
#endif /* KTERM_MBCS */
		XFillRectangle(screen->display, TextWindow(screen),
#ifdef STATUSLINE
		  screen->instatus && screen->reversestatus ?
		  screen->normalGC :
#endif /* STATUSLINE */
		  screen->reverseGC,
		 CursorX(screen, screen->cur_col),
		 CursorY(screen, screen->cur_row),
		 Width(screen) - screen->cur_col * FontWidth(screen),
		 FontHeight(screen));
	    }
	}
#ifdef KTERM
	bzero(screen->buf[screen->cur_row] + screen->cur_col,
	       (screen->max_col - screen->cur_col + 1) * sizeof(Bchr));
	/* with the right part cleared, we can't be wrapping */
	screen->buf [screen->cur_row] [0].attr &= ~LINEWRAPPED;
#else /* !KTERM */
	bzero(screen->buf [2 * screen->cur_row] + screen->cur_col,
	       (screen->max_col - screen->cur_col + 1));
	bzero(screen->buf [2 * screen->cur_row + 1] + screen->cur_col,
	       (screen->max_col - screen->cur_col + 1));
	/* with the right part cleared, we can't be wrapping */
	screen->buf [2 * screen->cur_row + 1] [0] &= ~LINEWRAPPED;
#endif /* !KTERM */
}

/*
 * Clear first part of cursor's line, inclusive.
 */
ClearLeft (screen)
    register TScreen *screen;
{
        int i;
#ifdef KTERM
	Bchr *cp;
#else /* !TERM */
	Char *cp;
#endif /* !TERM */

	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
#ifdef STATUSLINE
	if(screen->cur_row - screen->topline <= screen->max_row ||
	   screen->instatus) {
#else /* !STATUSLINE */
	if(screen->cur_row - screen->topline <= screen->max_row) {
#endif /* !STATUSLINE */
	    if(!AddToRefresh(screen)) {
		if(screen->scroll_amt)
			FlushScroll(screen);
#ifdef KTERM_MBCS
		BreakMBchar(screen);
#endif /* KTERM_MBCS */
		XFillRectangle (screen->display, TextWindow(screen),
#ifdef STATUSLINE
		     screen->instatus && screen->reversestatus ?
		     screen->normalGC :
#endif /* STATUSLINE */
		     screen->reverseGC,
		     screen->border + screen->scrollbar,
		      CursorY (screen, screen->cur_row),
		     (screen->cur_col + 1) * FontWidth(screen),
		     FontHeight(screen));
	    }
	}
	
#ifdef KTERM
	for ( i=0, cp=screen->buf[screen->cur_row];
	      i < screen->cur_col + 1;
	      i++, cp++) {
	    cp->gset = GSET_ASCII;
	    cp->code = ' ';
	    cp->attr = CHARDRAWN;
	}
#else /* !KTERM */
	for ( i=0, cp=screen->buf[2 * screen->cur_row];
	      i < screen->cur_col + 1;
	      i++, cp++)
	    *cp = ' ';
	for ( i=0, cp=screen->buf[2 * screen->cur_row + 1];
	      i < screen->cur_col + 1;
	      i++, cp++)
	    *cp = CHARDRAWN;
#endif /* !KTERM */
}

/* 
 * Erase the cursor's line.
 */
ClearLine(screen)
register TScreen *screen;
{
	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
#ifdef STATUSLINE
	if(screen->cur_row - screen->topline <= screen->max_row ||
	   screen->instatus) {
#else /* !STATUSLINE */
	if(screen->cur_row - screen->topline <= screen->max_row) {
#endif /* !STATUSLINE */
	    if(!AddToRefresh(screen)) {
		if(screen->scroll_amt)
			FlushScroll(screen);
		XFillRectangle (screen->display, TextWindow(screen), 
#ifdef STATUSLINE
		     screen->instatus && screen->reversestatus ?
		     screen->normalGC :
#endif /* STATUSLINE */
		     screen->reverseGC,
		     screen->border + screen->scrollbar,
		      CursorY (screen, screen->cur_row),
		     Width(screen), FontHeight(screen));
	    }
	}
#ifdef KTERM
	bzero (screen->buf[screen->cur_row], (screen->max_col + 1) * sizeof(Bchr));
#else /* !KTERM */
	bzero (screen->buf [2 * screen->cur_row], (screen->max_col + 1));
	bzero (screen->buf [2 * screen->cur_row + 1], (screen->max_col + 1));
#endif /* !KTERM */
}

ClearScreen(screen)
register TScreen *screen;
{
	register int top;

	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
	if((top = -screen->topline) <= screen->max_row) {
		if(screen->scroll_amt)
			FlushScroll(screen);
#ifdef STATUSLINE
		if(top == 0 && !screen->statusline)
#else /* !STATUSLINE */
		if(top == 0)
#endif /* !STATUSLINE */
			XClearWindow(screen->display, TextWindow(screen));
		else
			XClearArea(screen->display, TextWindow(screen),
			 screen->border + screen->scrollbar, 
			 top * FontHeight(screen) + screen->border,	
		 	 Width(screen), (screen->max_row - top + 1) *
			 FontHeight(screen), FALSE);
	}
	ClearBufRows (screen, 0, screen->max_row);
}

CopyWait(screen)
register TScreen *screen;
{
	XEvent reply;
	XEvent *rep = &reply;

	while (1) {
		XWindowEvent (screen->display, VWindow(screen), 
		  ExposureMask, &reply);
		switch (reply.type) {
		case Expose:
			HandleExposure (screen, &reply);
			break;
		case NoExpose:
		case GraphicsExpose:
			if (screen->incopy <= 0) {
				screen->incopy = 1;
				if (screen->scrolls > 0)
					screen->scrolls--;
			}
			if (reply.type == GraphicsExpose)
			    HandleExposure (screen, &reply);

			if ((reply.type == NoExpose) ||
			    ((XExposeEvent *)rep)->count == 0) {
			    if (screen->incopy <= 0 && screen->scrolls > 0)
				screen->scrolls--;
			    if (screen->scrolls == 0) {
				screen->incopy = 0;
				return;
			    }
			    screen->incopy = -1;
			}
			break;
		}
	}
}

/*
 * used by vertical_copy_area and and horizontal_copy_area
 */
static void
copy_area(screen, src_x, src_y, width, height, dest_x, dest_y)
    TScreen *screen;
    int src_x, src_y;
    unsigned int width, height;
    int dest_x, dest_y;
{
    /* wait for previous CopyArea to complete unless
       multiscroll is enabled and active */
    if (screen->incopy  &&  screen->scrolls == 0)
	CopyWait(screen);
    screen->incopy = -1;

    /* save for translating Expose events */
    screen->copy_src_x = src_x;
    screen->copy_src_y = src_y;
    screen->copy_width = width;
    screen->copy_height = height;
    screen->copy_dest_x = dest_x;
    screen->copy_dest_y = dest_y;

    XCopyArea(screen->display, 
	      TextWindow(screen), TextWindow(screen),
	      screen->normalGC,
	      src_x, src_y, width, height, dest_x, dest_y);
}

/*
 * use when inserting or deleting characters on the current line
 */
static void
horizontal_copy_area(screen, firstchar, nchars, amount)
    TScreen *screen;
    int firstchar;		/* char pos on screen to start copying at */
    int nchars;
    int amount;			/* number of characters to move right */
{
    int src_x = CursorX(screen, firstchar);
    int src_y = CursorY(screen, screen->cur_row);

    copy_area(screen, src_x, src_y,
	      (unsigned)nchars*FontWidth(screen), FontHeight(screen),
	      src_x + amount*FontWidth(screen), src_y);
}

/*
 * use when inserting or deleting lines from the screen
 */
static void
vertical_copy_area(screen, firstline, nlines, amount)
    TScreen *screen;
    int firstline;		/* line on screen to start copying at */
    int nlines;
    int amount;			/* number of lines to move up (neg=down) */
{
    if(nlines > 0) {
	int src_x = screen->border + screen->scrollbar;
	int src_y = firstline * FontHeight(screen) + screen->border;

	copy_area(screen, src_x, src_y,
		  (unsigned)Width(screen), nlines*FontHeight(screen),
		  src_x, src_y - amount*FontHeight(screen));
    }
}

/*
 * use when scrolling the entire screen
 */
scrolling_copy_area(screen, firstline, nlines, amount)
    TScreen *screen;
    int firstline;		/* line on screen to start copying at */
    int nlines;
    int amount;			/* number of lines to move up (neg=down) */
{

    if(nlines > 0) {
	vertical_copy_area(screen, firstline, nlines, amount);
    }
}

/*
 * Handler for Expose events on the VT widget.
 * Returns 1 iff the area where the cursor was got refreshed.
 */
HandleExposure (screen, event)
    register TScreen *screen;
    register XEvent *event;
{
    register XExposeEvent *reply = (XExposeEvent *)event;

    /* if not doing CopyArea or if this is a GraphicsExpose, don't translate */
    if(!screen->incopy  ||  event->type != Expose)
	return handle_translated_exposure (screen, reply->x, reply->y,
					   reply->width, reply->height);
    else {
	/* compute intersection of area being copied with
	   area being exposed. */
	int both_x1 = Max(screen->copy_src_x, reply->x);
	int both_y1 = Max(screen->copy_src_y, reply->y);
	int both_x2 = Min(screen->copy_src_x+screen->copy_width,
			  reply->x+reply->width);
	int both_y2 = Min(screen->copy_src_y+screen->copy_height,
			  reply->y+reply->height);
	int value = 0;

	/* was anything copied affected? */
	if(both_x2 > both_x1  && both_y2 > both_y1) {
	    /* do the copied area */
	    value = handle_translated_exposure
		(screen, reply->x + screen->copy_dest_x - screen->copy_src_x,
		 reply->y + screen->copy_dest_y - screen->copy_src_y,
		 reply->width, reply->height);
	}
	/* was anything not copied affected? */
	if(reply->x < both_x1 || reply->y < both_y1
	   || reply->x+reply->width > both_x2
	   || reply->y+reply->height > both_y2)
	    value = handle_translated_exposure (screen, reply->x, reply->y,
						reply->width, reply->height);

	return value;
    }
}

/*
 * Called by the ExposeHandler to do the actual repaint after the coordinates
 * have been translated to allow for any CopyArea in progress.
 * The rectangle passed in is pixel coordinates.
 */
handle_translated_exposure (screen, rect_x, rect_y, rect_width, rect_height)
    register TScreen *screen;
    register int rect_x, rect_y;
    register unsigned int rect_width, rect_height;
{
	register int toprow, leftcol, nrows, ncols;
	extern Bool waiting_for_initial_map;

	toprow = (rect_y - screen->border) / FontHeight(screen);
	if(toprow < 0)
		toprow = 0;
	leftcol = (rect_x - screen->border - screen->scrollbar)
	    / FontWidth(screen);
	if(leftcol < 0)
		leftcol = 0;
	nrows = (rect_y + rect_height - 1 - screen->border) / 
		FontHeight(screen) - toprow + 1;
	ncols =
	 (rect_x + rect_width - 1 - screen->border - screen->scrollbar) /
			FontWidth(screen) - leftcol + 1;
	toprow -= screen->scrolls;
	if (toprow < 0) {
		nrows += toprow;
		toprow = 0;
	}
	if (toprow + nrows - 1 > screen->max_row)
#ifdef STATUSLINE
		nrows = screen->max_row - toprow + 1 + screen->statusline;
#else /* !STATUSLINE */
		nrows = screen->max_row - toprow + 1;
#endif /* !STATUSLINE */
	if (leftcol + ncols - 1 > screen->max_col)
		ncols = screen->max_col - leftcol + 1;

	if (nrows > 0 && ncols > 0) {
		ScrnRefresh (screen, toprow, leftcol, nrows, ncols, False);
		if (waiting_for_initial_map) {
		    first_map_occurred ();
		}
		if (screen->cur_row >= toprow &&
		    screen->cur_row < toprow + nrows &&
		    screen->cur_col >= leftcol &&
		    screen->cur_col < leftcol + ncols)
			return (1);

	}
	return (0);
}

ReverseVideo (termw)
	XtermWidget termw;
{
	register TScreen *screen = &termw->screen;
	GC tmpGC;
	Window tek = TWindow(screen);
	unsigned long tmp;
#ifdef KTERM
	int fnum;
#endif /* KTERM */
#ifdef KTERM_COLOR
	XGCValues xgcv;
#endif /* KTERM_COLOR */

	tmp = termw->core.background_pixel;
	if(screen->cursorcolor == screen->foreground)
		screen->cursorcolor = tmp;
	termw->core.background_pixel = screen->foreground;
	screen->foreground = tmp;

	tmp = screen->mousecolorback;
	screen->mousecolorback = screen->mousecolor;
	screen->mousecolor = tmp;

#ifdef KTERM
# ifdef KTERM_COLOR
      xgcv.foreground = screen->foreground;
      xgcv.background = termw->core.background_pixel;
# endif /* KTERM_COLOR */
	for (fnum=F_ISO8859_1; fnum<FCNT; fnum++) {
# ifdef KTERM_COLOR
	    /* reset color attribute */
	    xgcv.foreground = screen->foreground;
	    xgcv.background = termw->core.background_pixel;
	    if (screen->reverseGC)
		XChangeGC(screen->display, screen->reverseGC,
			  GCForeground|GCBackground, &xgcv);
	    if (screen->reverseboldGC)
		XChangeGC(screen->display, screen->reverseboldGC,
			  GCForeground|GCBackground, &xgcv);
	    xgcv.foreground = term->core.background_pixel;
	    xgcv.background = screen->foreground;
	    if (screen->normalGC)
		XChangeGC(screen->display, screen->normalGC,
			  GCForeground|GCBackground, &xgcv);
	    if (screen->normalboldGC)
		XChangeGC(screen->display, screen->normalboldGC,
			  GCForeground|GCBackground, &xgcv);
# endif /* KTERM_COLOR */
	    tmpGC = screen->normalGC;
	    screen->normalGC = screen->reverseGC;
	    screen->reverseGC = tmpGC;

	    tmpGC = screen->normalboldGC;
	    screen->normalboldGC = screen->reverseboldGC;
	    screen->reverseboldGC = tmpGC;

	    tmpGC = screen->cursorGC;
	    screen->cursorGC = screen->reversecursorGC;
	    screen->reversecursorGC = tmpGC;
	}
#else /* !KTERM */
	tmpGC = screen->normalGC;
	screen->normalGC = screen->reverseGC;
	screen->reverseGC = tmpGC;

	tmpGC = screen->normalboldGC;
	screen->normalboldGC = screen->reverseboldGC;
	screen->reverseboldGC = tmpGC;
# ifndef ENBUG
/*
 * Bug fix by michael
 * 3 non null lines are inserted.
 */
	tmpGC = screen->cursorGC;
	screen->cursorGC = screen->reversecursorGC;
	screen->reversecursorGC = tmpGC;
# endif /* !ENBUG */
#endif /* !KTERM */

	recolor_cursor (screen->pointer_cursor, 
			screen->mousecolor, screen->mousecolorback);
	recolor_cursor (screen->arrow,
			screen->mousecolor, screen->mousecolorback);

	termw->misc.re_verse = !termw->misc.re_verse;

	XDefineCursor(screen->display, TextWindow(screen), screen->pointer_cursor);
	if(tek)
		XDefineCursor(screen->display, tek, screen->arrow);

	
	if(screen->scrollWidget)
		ScrollBarReverseVideo(screen->scrollWidget);

	XSetWindowBackground(screen->display, TextWindow(screen), termw->core.background_pixel);
	if(tek) {
	    TekReverseVideo(screen);
	}
	XClearWindow(screen->display, TextWindow(screen));
#ifdef STATUSLINE
	ScrnRefresh (screen, 0, 0, screen->max_row + 1 + screen->statusline,
#else /* !STATUSLINE */
	ScrnRefresh (screen, 0, 0, screen->max_row + 1,
#endif /* !STATUSLINE */
	 screen->max_col + 1, False);
	if(screen->Tshow) {
	    XClearWindow(screen->display, tek);
	    TekExpose((Widget)NULL, (XEvent *)NULL, (Region)NULL);
	}
	update_reversevideo();
#ifdef KTERM_KCONV
	SendColor();
#endif /* KTERM_KCONV */
}


recolor_cursor (cursor, fg, bg)
    Cursor cursor;			/* X cursor ID to set */
    unsigned long fg, bg;		/* pixel indexes to look up */
{
    register TScreen *screen = &term->screen;
    register Display *dpy = screen->display;
    XColor colordefs[2];		/* 0 is foreground, 1 is background */

    colordefs[0].pixel = fg;
    colordefs[1].pixel = bg;
    XQueryColors (dpy, DefaultColormap (dpy, DefaultScreen (dpy)),
		  colordefs, 2);
    XRecolorCursor (dpy, cursor, colordefs, colordefs+1);
    return;
}

#ifdef KTERM_MBCS
/*
 * If the cursor points second byte of multi byte character,
 * replace this character into two blanks.
 */
BreakMBchar(screen)
register TScreen *screen;
{
	register Bchr *ptr;
	if (screen->cur_col >= 1 && screen->cur_col <= screen->max_col
	 && screen->buf[screen->cur_row][screen->cur_col].gset == MBC2) {
		XFillRectangle(screen->display, TextWindow(screen),
#ifdef STATUSLINE
		 screen->instatus && screen->reversestatus ?
		 screen->normalGC :
#endif /* STATUSLINE */
		 screen->reverseGC,
		 CursorX(screen, screen->cur_col - 1),
		 CursorY(screen, screen->cur_row),
		 2 * FontWidth(screen), FontHeight(screen));
		ptr = screen->buf[screen->cur_row] + screen->cur_col - 1;
		bzero((char*)ptr, 2 * sizeof(Bchr));
	}
}

#endif /* KTERM_MBCS */
#ifdef KTERM

Char
MapOnFont(gset, code)
register Char gset;
register Char code;
{
	if (code & NEEDMAP) {
		switch (gset) {
		case GSET_UK:
			return '\243';		/* UK pound sign on iso8859-1 */
			break;
		case GSET_GRAPH:
			code &= ~NEEDMAP;
			return (code == 0x5f) ? 0x7f : (code - 0x5f);
			break;
		case GSET_KANA:
			return code & ~NEEDMAP | 0x80;    /* kana on jisx0201 */
			break;
		}
	} else if (code == 0) {
		return ' ';
	} else {
		return code;
	}
}

#endif /* KTERM */
