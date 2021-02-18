/*
 *	$XConsortium: screen.c,v 1.29 91/05/10 16:57:37 gildea Exp $
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

/* screen.c */

#include "ptyx.h"
#include "error.h"
#include "data.h"

#include <stdio.h>
#include <signal.h>
#ifdef SVR4
#include <termios.h>
#else
#include <sys/ioctl.h>
#endif

#ifdef att
#include <sys/termio.h>
#include <sys/stream.h>			/* get typedef used in ptem.h */
#include <sys/ptem.h>
#endif

extern Char *calloc(), *malloc(), *realloc();
extern void free();

#ifdef I18N
Char	*draw_work = NULL;
#endif /* I18N */

ScrnBuf Allocate (nrow, ncol, addr)
/*
   allocates memory for a 2-dimensional array of chars and returns a pointer
   thereto
   each line is formed from a pair of char arrays.  The first (even) one is
   the actual character array and the second (odd) one is the attributes.
 */
register int nrow, ncol;
Char **addr;
{
	register ScrnBuf base;
	register Char *tmp;
	register int i;

#ifdef STATUSLINE
	++nrow;
#endif /* STATUSLINE */
	if ((base = (ScrnBuf) calloc ((unsigned)(nrow *= 2), sizeof (Char *))) == 0)
		SysError (ERROR_SCALLOC);

	if ((tmp = calloc ((unsigned) (nrow * ncol), sizeof(Char))) == 0)
		SysError (ERROR_SCALLOC2);

	*addr = tmp;
	for (i = 0; i < nrow; i++, tmp += ncol)
		base[i] = tmp;

#ifdef I18N
	if (!draw_work)
		if ((draw_work =
		    (Char *) calloc ((unsigned) ncol, sizeof(Char))) == 0)
			SysError (ERROR_SCALLOC2);
#endif /* I18N */
	return (base);
}

/*
 *  This is called when the screen is resized.
 *  Returns the number of lines the text was moved down (neg for up).
 *  (Return value only necessary with SouthWestGravity.)
 */
static
#ifdef STATUSLINE
Reallocate(sbuf, sbufaddr, nrow, ncol, oldrow, oldcol, havest)
#else /* STATUSLINE */
Reallocate(sbuf, sbufaddr, nrow, ncol, oldrow, oldcol)
#endif /* STATUSLINE */
    ScrnBuf *sbuf;
    Char **sbufaddr;
    int nrow, ncol, oldrow, oldcol;
#ifdef STATUSLINE
    Boolean havest;
#endif /* STATUSLINE */
{
	register ScrnBuf base;
	register Char *tmp;
	register int i, minrows, mincols;
	Char *oldbuf;
	int move_down = 0, move_up = 0;
#ifdef STATUSLINE
	Char *s0, *s1;
#endif /* STATUSLINE */

	if (sbuf == NULL || *sbuf == NULL)
		return 0;

	oldrow *= 2;
	oldbuf = *sbufaddr;
#ifdef STATUSLINE
	if (havest) {
		s0 = (*sbuf)[oldrow + 0];
		s1 = (*sbuf)[oldrow + 1];
		oldrow += 2;
	}
#endif /* STATUSLINE */

	/*
	 * Special case if oldcol == ncol - straight forward realloc and
	 * update of the additional lines in sbuf
	 */

	/* this is a good idea, but doesn't seem to be implemented.  -gildea */

	/* 
	 * realloc sbuf, the pointers to all the lines.
	 * If the screen shrinks, remove lines off the top of the buffer
	 * if resizeGravity resource says to do so.
	 */
	nrow *= 2;
#ifdef STATUSLINE
	if (havest)
		nrow += 2;
#endif /* STATUSLINE */
	if (nrow < oldrow  &&  term->misc.resizeGravity == SouthWestGravity) {
	    /* Remove lines off the top of the buffer if necessary. */
	    move_up = oldrow-nrow 
		        - 2*(term->screen.max_row - term->screen.cur_row);
	    if (move_up < 0)
		move_up = 0;
	    /* Overlapping bcopy here! */
	    bcopy(*sbuf+move_up, *sbuf,
		  (oldrow-move_up)*sizeof((*sbuf)[0]) );
	}
	*sbuf = (ScrnBuf) realloc((char *) (*sbuf),
				  (unsigned) (nrow * sizeof(char *)));
	if (*sbuf == 0)
	    SysError(ERROR_RESIZE);
	base = *sbuf;
#ifdef I18N
	draw_work = (Char *) realloc ((char *) draw_work,
			(unsigned) ncol * sizeof(Char));
	if (draw_work == 0)
	    SysError(ERROR_RESIZE);
#endif /* I18N */

	/* 
	 *  create the new buffer space and copy old buffer contents there
	 *  line by line.
	 */
	if ((tmp = calloc((unsigned) (nrow * ncol), sizeof(Char))) == 0)
		SysError(ERROR_SREALLOC);
	*sbufaddr = tmp;
	minrows = (oldrow < nrow) ? oldrow : nrow;
	mincols = (oldcol < ncol) ? oldcol : ncol;
#ifdef STATUSLINE
	if (havest) {
		nrow -= 2;
		oldrow -= 2;
		minrows -= 2;
	}
#endif /* STATUSLINE */
	if (nrow > oldrow  &&  term->misc.resizeGravity == SouthWestGravity) {
	    /* move data down to bottom of expanded screen */
	    move_down = Min(nrow-oldrow, 2*term->screen.savedlines);
	    tmp += ncol*move_down;
	}
	for (i = 0; i < minrows; i++, tmp += ncol) {
		bcopy(base[i], tmp, mincols);
	}
	/*
	 * update the pointers in sbuf
	 */
	for (i = 0, tmp = *sbufaddr; i < nrow; i++, tmp += ncol)
	    base[i] = tmp;

#ifdef STATUSLINE
	if (havest) {
		bcopy(s0, tmp, mincols);
		base[i++] = tmp;
		tmp += ncol;
		bcopy(s1, tmp, mincols);
		base[i++] = tmp;
		tmp += ncol;
		base[i] = tmp;
	}
#endif /* STATUSLINE */
        /* Now free the old buffer */
	free(oldbuf);

	return move_down ? move_down/2 : -move_up/2; /* convert to rows */
}

ScreenWrite (screen, str, flags, length)
/*
   Writes str into buf at row row and column col.  Characters are set to match
   flags.
 */
TScreen *screen;
register Char *str;
register unsigned flags;
register int length;		/* length of string */
{
	register Char *attrs;
	register int avail  = screen->max_col - screen->cur_col + 1;
	register Char *col;
	int collen;

	collen = ComputeCollen (screen, str, length);
	if (collen > avail) {
	    collen = avail;
	    length = ComputeWchlen (screen, str, collen);
	}
	if (collen <= 0 || length <= 0)
		return;

	col = screen->buf[avail = 2 * screen->cur_row] + screen->cur_col;
	attrs = screen->buf[avail + 1] + screen->cur_col;
	flags &= ATTRIBUTES;
	flags |= CHARDRAWN;
	while (length-- > 0) {
		*col++ = *str;
		*attrs++ = flags;
		if (ComputeCollen (screen, str++, 1) > 1)
			*col++ = DATA_PREV, *attrs++ = flags;
	}
}

ScrnInsertLine (sb, last, where, n, size)
/*
   Inserts n blank lines at sb + where, treating last as a bottom margin.
   Size is the size of each entry in sb.
   Requires: 0 <= where < where + n <= last
   	     n <= MAX_ROWS
 */
register ScrnBuf sb;
int last;
register int where, n, size;
{
	register int i;
	Char *save [2 * MAX_ROWS];


	/* save n lines at bottom */
	bcopy ((char *) &sb [2 * (last -= n - 1)], (char *) save,
		2 * sizeof (Char *) * n);
	
	/* clear contents of old rows */
	for (i = 2 * n - 1; i >= 0; i--)
		bzero ((char *) save [i], size * sizeof(Char *));

	/*
	 * WARNING, overlapping copy operation.  Move down lines (pointers).
	 *
	 *   +----|---------|--------+
	 *
	 * is copied in the array to:
	 *
	 *   +--------|---------|----+
	 */
	bcopy ((char *) &sb [2 * where], (char *) &sb [2 * (where + n)],
		2 * sizeof (Char *) * (last - where));

	/* reuse storage for new lines at where */
	bcopy ((char *)save, (char *) &sb[2 * where], 2 * sizeof(Char *) * n);
}


ScrnDeleteLine (sb, last, where, n, size)
/*
   Deletes n lines at sb + where, treating last as a bottom margin.
   Size is the size of each entry in sb.
   Requires 0 <= where < where + n < = last
   	    n <= MAX_ROWS
 */
register ScrnBuf sb;
register int n, last, size;
int where;
{
	register int i;
	Char *save [2 * MAX_ROWS];

	/* save n lines at where */
	bcopy ((char *) &sb[2 * where], (char *)save, 2 * sizeof(Char *) * n);

	/* clear contents of old rows */
	for (i = 2 * n - 1 ; i >= 0 ; i--)
		bzero ((char *) save [i], size * sizeof(Char *));

	/* move up lines */
	bcopy ((char *) &sb[2 * (where + n)], (char *) &sb[2 * where],
		2 * sizeof (Char *) * ((last -= n - 1) - where));

	/* reuse storage for new bottom lines */
	bcopy ((char *)save, (char *) &sb[2 * last],
		2 * sizeof(Char *) * n);
}


ScrnInsertChar (sb, row, col, n, size)
    /*
      Inserts n blanks in sb at row, col.  Size is the size of each row.
      */
    ScrnBuf sb;
    int row, size;
    register int col, n;
{
	register int i, j;
	register Char *ptr = sb [2 * row];
	register Char *attrs = sb [2 * row + 1];
	int wrappedbit = attrs[0]&LINEWRAPPED;

	attrs[0] &= ~LINEWRAPPED; /* make sure the bit isn't moved */
	for (i = size - 1; i >= col + n; i--) {
		ptr[i] = ptr[j = i - n];
		attrs[i] = attrs[j];
	}

	for (i=col; i<col+n; i++)
	    ptr[i] = ' ';
	for (i=col; i<col+n; i++)
	    attrs[i] = CHARDRAWN;

	if (wrappedbit)
	    attrs[0] |= LINEWRAPPED;
}


ScrnDeleteChar (sb, row, col, n, size)
    /*
      Deletes n characters in sb at row, col. Size is the size of each row.
      */
    ScrnBuf sb;
    register int row, size;
    register int n, col;
{
	register Char *ptr = sb[2 * row];
	register Char *attrs = sb[2 * row + 1];
	register nbytes = (size - n - col);
	int wrappedbit = attrs[0]&LINEWRAPPED;

	bcopy (ptr + col + n, ptr + col, nbytes * sizeof(Char));
	bcopy (attrs + col + n, attrs + col, nbytes * sizeof(Char));
	bzero (ptr + size - n, n * sizeof(Char));
	bzero (attrs + size - n, n * sizeof(Char));
	if (wrappedbit)
	    attrs[0] |= LINEWRAPPED;
}

#ifdef I18N
sXwcDrawImageString (screen, w, f, g, x, y, s, n)
	TScreen		*screen;
	Window		w;
	XFontSet	f;
	GC		g;
	int		x, y;
	Char		*s;
	int		n;
{
	register int	i;
	register int	len;
	register Char	*t;

	if (*s == DATA_PREV) {
		--s;
		++n;
		x -= FontWidth (screen);
	}
	len = 0;
	t = draw_work;
	for (i = 0 ; i < n ; i ++)
		if (s[i] != DATA_PREV)
			t[len++] = s[i];
	XwcDrawImageString(screen->display, w, f, g, x, y, t, len);
}

sXwcDrawString (screen, w, f, g, x, y, s, n)
	TScreen		*screen;
	Window		w;
	XFontSet	f;
	GC		g;
	int		x, y;
	Char		*s;
	int		n;
{
	register int	i;
	register int	len;
	register Char	*t;

	if (*s == DATA_PREV) {
		--s;
		++n;
		x -= FontWidth (screen);
	}
	len = 0;
	t = draw_work;
	for (i = 0 ; i < n ; i ++)
		if (s[i] != DATA_PREV)
			t[len++] = s[i];
	XwcDrawString(screen->display, w, f, g, x, y, t, len);
}
#endif /* I18N */

ScrnRefresh (screen, toprow, leftcol, nrows, ncols, force)
/*
   Repaints the area enclosed by the parameters.
   Requires: (toprow, leftcol), (toprow + nrows, leftcol + ncols) are
   	     coordinates of characters in screen;
	     nrows and ncols positive.
 */
register TScreen *screen;
int toprow, leftcol, nrows, ncols;
Boolean force;			/* ... leading/trailing spaces */
{
	int y = toprow * FontHeight(screen) + screen->border +
		screen->fnt_norm->ascent;
	register int row;
	register int topline = screen->topline;
	int maxrow = toprow + nrows - 1;
	int scrollamt = screen->scroll_amt;
	int max = screen->max_row;
#ifdef STATUSLINE
	int dostatus = 0, left, width;
#endif /* STATUSLINE */

#ifdef STATUSLINE
	if (screen->statusline && maxrow == screen->max_row + 1) {
		dostatus++;
		maxrow--;
	}
#endif	/* STATUSLINE */
	if(screen->cursor_col >= leftcol && screen->cursor_col <=
	 (leftcol + ncols - 1) && screen->cursor_row >= toprow + topline &&
	 screen->cursor_row <= maxrow + topline)
		screen->cursor_state = OFF;
#ifdef STATUSLINE
    for (;;) {
#endif /* STATUSLINE */
	for (row = toprow; row <= maxrow; y += FontHeight(screen), row++) {
	   register Char *chars;
	   register Char *attrs;
	   register int col = leftcol;
	   int maxcol = leftcol + ncols - 1;
	   int lastind;
	   int flags;
	   int x, n;
	   GC gc;
	   Boolean hilite;	
#ifdef I18N
	   XFontSet fs;
#endif /* I18N */

	   if (row < screen->top_marg || row > screen->bot_marg)
		lastind = row;
	   else
		lastind = row - scrollamt;

	   if (lastind < 0 || lastind > max)
	   	continue;

	   chars = screen->buf [2 * (lastind + topline)];
	   attrs = screen->buf [2 * (lastind + topline) + 1];

	   if (row < screen->startHRow || row > screen->endHRow ||
	       (row == screen->startHRow && maxcol < screen->startHCol) ||
	       (row == screen->endHRow && col >= screen->endHCol))
	       {
	       /* row does not intersect selection; don't hilite */
	       if (!force) {
		   while (col <= maxcol && (attrs[col] & ~BOLD) == 0 &&
			  (chars[col] & ~040) == 0)
		       col++;

		   while (col <= maxcol && (attrs[maxcol] & ~BOLD) == 0 &&
			  (chars[maxcol] & ~040) == 0)
		       maxcol--;
	       }
	       hilite = False;
	   }
	   else {
	       /* row intersects selection; split into pieces of single type */
	       if (row == screen->startHRow && col < screen->startHCol) {
		   ScrnRefresh(screen, row, col, 1, screen->startHCol - col,
			       force);
		   col = screen->startHCol;
	       }
	       if (row == screen->endHRow && maxcol >= screen->endHCol) {
		   ScrnRefresh(screen, row, screen->endHCol, 1,
			       maxcol - screen->endHCol + 1, force);
		   maxcol = screen->endHCol - 1;
	       }
	       /* remaining piece should be hilited */
	       hilite = True;
	   }

	   if (col > maxcol) continue;

	   flags = attrs[col];

	   if ( (!hilite && (flags & INVERSE) != 0) ||
	        (hilite && (flags & INVERSE) == 0) )
	       if (flags & BOLD) gc = screen->reverseboldGC;
	       else gc = screen->reverseGC;
	   else 
	       if (flags & BOLD) gc = screen->normalboldGC;
	       else gc = screen->normalGC;

	   x = CursorX(screen, col);
	   lastind = col;
#ifdef I18N
	   if (flags & BOLD)
		fs = screen->fs_bold;
	   else
		fs = screen->fs_norm;
#endif /* I18N */

	   for (; col <= maxcol; col++) {
		if (attrs[col] != flags) {
#ifdef I18N
		   sXwcDrawImageString(screen, TextWindow(screen), fs,
			gc, x, y, (Char *) &chars[lastind], n = col - lastind);
#else /* I18N */
		   XDrawImageString(screen->display, TextWindow(screen), 
			gc, x, y, (char *) &chars[lastind], n = col - lastind);
#endif /* I18N */
		   if((flags & BOLD) && screen->enbolden)
#ifdef I18N
		 	sXwcDrawString(screen, TextWindow(screen), fs,
			 gc, x + 1, y, (Char *) &chars[lastind], n);
#else /* I18N */
		 	XDrawString(screen->display, TextWindow(screen), 
			 gc, x + 1, y, (char *) &chars[lastind], n);
#endif /* I18N */
		   if(flags & UNDERLINE) 
			XDrawLine(screen->display, TextWindow(screen), 
			 gc, x, y+1, x+n*FontWidth(screen), y+1);

		   x += n * FontWidth(screen);

		   lastind = col;

		   flags = attrs[col];

	   	   if ((!hilite && (flags & INVERSE) != 0) ||
		       (hilite && (flags & INVERSE) == 0) )
	       		if (flags & BOLD) gc = screen->reverseboldGC;
	       		else gc = screen->reverseGC;
	  	    else 
	      		 if (flags & BOLD) gc = screen->normalboldGC;
	      		 else gc = screen->normalGC;
		}

		if(chars[col] == 0)
			chars[col] = ' ';
	   }


	   if ( (!hilite && (flags & INVERSE) != 0) ||
	        (hilite && (flags & INVERSE) == 0) )
	       if (flags & BOLD) gc = screen->reverseboldGC;
	       else gc = screen->reverseGC;
	   else 
	       if (flags & BOLD) gc = screen->normalboldGC;
	       else gc = screen->normalGC;
#ifdef I18N
	   if (flags & BOLD)
		fs = screen->fs_bold;
	   else
		fs = screen->fs_norm;
	   sXwcDrawImageString(screen, TextWindow(screen),
		fs, gc, 
		x, y, (Char *) &chars[lastind], n = col - lastind);
#else /* I18N */
	   XDrawImageString(screen->display, TextWindow(screen), gc, 
	         x, y, (char *) &chars[lastind], n = col - lastind);
#endif /* I18N */
	   if((flags & BOLD) && screen->enbolden)
#ifdef I18N
		sXwcDrawString(screen, TextWindow(screen), fs, gc,
		x + 1, y, (Char *) &chars[lastind], n);
#else /* I18N */
		XDrawString(screen->display, TextWindow(screen), gc,
		x + 1, y, (char *) &chars[lastind], n);
#endif /* I18N */
	   if(flags & UNDERLINE) 
		XDrawLine(screen->display, TextWindow(screen), gc, 
		 x, y+1, x + n * FontWidth(screen), y+1);
	}
#ifdef STATUSLINE
	if (dostatus <= 0)
		break;
	dostatus = -1;
	topline = 0;
	scrollamt = 0;
	toprow = maxrow = max = screen->max_row + 1;
	left = CursorX(screen, leftcol);
	width = ncols * FontWidth(screen);
	if (leftcol == 0) {
		left = screen->scrollbar;
		width += screen->border;
	}
	if (leftcol + ncols - 1 >= screen->max_col)
		width += screen->border;
	y += screen->border + 1;
	XFillRectangle(screen->display, TextWindow(screen),
	       screen->reversestatus ? screen->normalGC : screen->reverseGC,
		left, y - 1,
	       width, screen->statusheight);
	if (!screen->reversestatus) {
		StatusBox(screen);
	}
    }
#endif /* STATUSLINE */
}

ClearBufRows (screen, first, last)
/*
   Sets the rows first though last of the buffer of screen to spaces.
   Requires first <= last; first, last are rows of screen->buf.
 */
register TScreen *screen;
register int first, last;
{
	first *= 2;
	last = 2 * last + 1;
	while (first <= last)
		bzero (screen->buf [first++],
			(screen->max_col + 1) * sizeof(Char));
}

/*
  Resizes screen:
  1. If new window would have fractional characters, sets window size so as to
  discard fractional characters and returns -1.
  Minimum screen size is 1 X 1.
  Note that this causes another ExposeWindow event.
  2. Enlarges screen->buf if necessary.  New space is appended to the bottom
  and to the right
  3. Reduces  screen->buf if necessary.  Old space is removed from the bottom
  and from the right
  4. Cursor is positioned as closely to its former position as possible
  5. Sets screen->max_row and screen->max_col to reflect new size
  6. Maintains the inner border (and clears the border on the screen).
  7. Clears origin mode and sets scrolling region to be entire screen.
  8. Returns 0
  */
ScreenResize (screen, width, height, flags)
    register TScreen *screen;
    int width, height;
    unsigned *flags;
{
	int rows, cols;
	int border = 2 * screen->border;
	int move_down_by;
#ifdef sun
#ifdef TIOCSSIZE
	struct ttysize ts;
#endif	/* TIOCSSIZE */
#else	/* sun */
#ifdef TIOCSWINSZ
	struct winsize ws;
#endif	/* TIOCSWINSZ */
#endif	/* sun */
	Window tw = TextWindow (screen);

	/* clear the right and bottom internal border because of NorthWest
	   gravity might have left junk on the right and bottom edges */
	XClearArea (screen->display, tw,
		    width - screen->border, 0,                /* right edge */
#ifdef STATUSLINE
		    screen->border, height - screen->statusheight,
#else /* STATUSLINE */
		    screen->border, height,           /* from top to bottom */
#endif /* STATUSLINE */
		    False);
	XClearArea (screen->display, tw, 
#ifdef STATUSLINE
		    0, height - screen->border - screen->statusheight,
#else /* STATUSLINE */
		    0, height - screen->border,	                  /* bottom */
#endif /* STATUSLINE */
		    width, screen->border,         /* all across the bottom */
		    False);

	/* round so that it is unlikely the screen will change size on  */
	/* small mouse movements.					*/
#ifdef STATUSLINE
	rows = (height + FontHeight(screen) / 2
		- border - screen->statusheight) / FontHeight(screen);
#else /* STATUSLINE */
	rows = (height + FontHeight(screen) / 2 - border) /
	 FontHeight(screen);
#endif /* STATUSLINE */
	cols = (width + FontWidth(screen) / 2 - border - screen->scrollbar) /
	 FontWidth(screen);
	if (rows < 1) rows = 1;
	if (cols < 1) cols = 1;
#ifdef STATUSLINE
	if (screen->max_row < rows - 1) {
		XClearArea (screen->display, tw,
			    screen->scrollbar,
			    (screen->max_row + 1) * FontHeight(screen) +
			    screen->border * 2,
			    (screen->max_col + 1) * FontWidth(screen) + 
			    screen->border * 2,
			    screen->statusheight,
			    False);
	} else if (screen->max_row > rows - 1) {
		XClearArea (screen->display, tw,
			    screen->scrollbar,
			    rows * FontHeight(screen) + screen->border * 2,
			    cols * FontWidth(screen) + screen->border * 2,
			    screen->statusheight,
			    True);
	} else if (screen->max_col < cols - 1) {
		XClearArea (screen->display, tw,
			    screen->scrollbar +
			    (screen->max_col + 1) * FontWidth(screen) + 
			    screen->border,
			    (screen->max_row + 1) * FontHeight(screen) +
			    screen->border * 2,
			    screen->border,
			    screen->statusheight,
			    False);
	} else if (screen->max_col > cols - 1) {
		XClearArea (screen->display, tw,
			    screen->scrollbar +
			    cols * FontWidth(screen) + screen->border - 1,
			    (screen->max_row + 1) * FontHeight(screen) +
			    screen->border * 2,
			    screen->border + 1,
			    screen->statusheight,
			    True);
	}
#endif	/* STATUSLINE */

	/* update buffers if the screen has changed size */
	if (screen->max_row != rows - 1 || screen->max_col != cols - 1) {
		register int savelines = screen->scrollWidget ?
		 screen->savelines : 0;
		int delta_rows = rows - (screen->max_row + 1);
		
		if(screen->cursor_state)
			HideCursor();
		if ( screen->alternate
		     && term->misc.resizeGravity == SouthWestGravity )
		    /* swap buffer pointers back to make all this hair work */
		    SwitchBufPtrs(screen);
		if (screen->altbuf) 
		    (void) Reallocate (&screen->altbuf,
				(Char **)&screen->abuf_address,
				rows, cols, screen->max_row + 1,
				screen->max_col + 1
#ifdef STATUSLINE
				,False
#endif /* STATUSLINE */
			);
		move_down_by = Reallocate (&screen->allbuf,
					(Char **)&screen->sbuf_address,
					rows + savelines, cols,
					screen->max_row + 1 + savelines,
					screen->max_col + 1
#ifdef STATUSLINE
				,True
#endif /* STATUSLINE */
				);
		screen->buf = &screen->allbuf[2 * savelines];

		screen->max_row += delta_rows;
		screen->max_col = cols - 1;

		if (term->misc.resizeGravity == SouthWestGravity) {
		    screen->savedlines -= move_down_by;
		    if (screen->savedlines < 0)
			screen->savedlines = 0;
		    if (screen->savedlines > screen->savelines)
			screen->savedlines = screen->savelines;
		    if (screen->topline < -screen->savedlines)
			screen->topline = -screen->savedlines;
		    screen->cur_row += move_down_by;
		    screen->cursor_row += move_down_by;
		    ScrollSelection(screen, move_down_by);

		    if (screen->alternate)
			SwitchBufPtrs(screen); /* put the pointers back */
		}
	
		/* adjust scrolling region */
		screen->top_marg = 0;
		screen->bot_marg = screen->max_row;
		*flags &= ~ORIGIN;

#ifdef STATUSLINE
		if (screen->instatus)
			screen->cur_row = screen->max_row + 1;
		else
#endif /* STATUSLINE */
		if (screen->cur_row > screen->max_row)
			screen->cur_row = screen->max_row;
		if (screen->cur_col > screen->max_col)
			screen->cur_col = screen->max_col;
	
#ifdef STATUSLINE
		screen->fullVwin.height = height - border
			- screen->statusheight;
#else /* STATUSLINE */
		screen->fullVwin.height = height - border;
#endif /* STATUSLINE */
		screen->fullVwin.width = width - border - screen->scrollbar;

	} else if(FullHeight(screen) == height && FullWidth(screen) == width)
	 	return(0);	/* nothing has changed at all */

	if(screen->scrollWidget)
		ResizeScrollBar(screen->scrollWidget, -1, -1, height);
	
	screen->fullVwin.fullheight = height;
	screen->fullVwin.fullwidth = width;
	ResizeSelection (screen, rows, cols);
#ifdef sun
#ifdef TIOCSSIZE
	/* Set tty's idea of window size */
	ts.ts_lines = rows;
	ts.ts_cols = cols;
	ioctl (screen->respond, TIOCSSIZE, &ts);
#ifdef SIGWINCH
	if(screen->pid > 1) {
		int	pgrp;
		
		if (ioctl (screen->respond, TIOCGPGRP, &pgrp) != -1)
			kill_process_group(pgrp, SIGWINCH);
	}
#endif	/* SIGWINCH */
#endif	/* TIOCSSIZE */
#else	/* sun */
#ifdef TIOCSWINSZ
	/* Set tty's idea of window size */
	ws.ws_row = rows;
	ws.ws_col = cols;
	ws.ws_xpixel = width;
	ws.ws_ypixel = height;
	ioctl (screen->respond, TIOCSWINSZ, (char *)&ws);
#ifdef notdef	/* change to SIGWINCH if this doesn't work for you */
	if(screen->pid > 1) {
		int	pgrp;
		
		if (ioctl (screen->respond, TIOCGPGRP, &pgrp) != -1)
		    kill_process_group(pgrp, SIGWINCH);
	}
#endif	/* SIGWINCH */
#endif	/* TIOCSWINSZ */
#endif	/* sun */
	return (0);
}

/*
 * Sets the attributes from the row, col, to row, col + length according to
 * mask and value. The bits in the attribute byte specified by the mask are
 * set to the corresponding bits in the value byte. If length would carry us
 * over the end of the line, it stops at the end of the line.
 */
void
ScrnSetAttributes(screen, row, col, mask, value, length)
TScreen *screen;
int row, col;
unsigned mask, value;
register int length;		/* length of string */
{
	register Char *attrs;
	register int avail  = screen->max_col - col + 1;

	if (length > avail)
	    length = avail;
	if (length <= 0)
		return;
	attrs = screen->buf[2 * row + 1] + col;
	value &= mask;	/* make sure we only change the bits allowed by mask*/
	while(length-- > 0) {
		*attrs &= mask;		/* clear the bits */
		*attrs |= value;	/* copy in the new values */
		attrs++;
	}
}

/*
 * Gets the attributes from the row, col, to row, col + length into the
 * supplied array, which is assumed to be big enough.  If length would carry us
 * over the end of the line, it stops at the end of the line. Returns
 * the number of bytes of attributes (<= length)
 */
int
ScrnGetAttributes(screen, row, col, str, length)
TScreen *screen;
int row, col;
Char *str;
register int length;		/* length of string */
{
	register Char *attrs;
	register int avail  = screen->max_col - col + 1;
	int ret;

	if (length > avail)
	    length = avail;
	if (length <= 0)
		return 0;
	ret = length;
	attrs = screen->buf[2 * row + 1] + col;
	while(length-- > 0) {
		*str++ = *attrs++;
	}
	return ret;
}
Bool
non_blank_line(sb, row, col, len)
ScrnBuf sb;
register int row, col, len;
{
	register int	i;
	register Char *ptr = sb [2 * row];

	for (i = col; i < len; i++)	{
		if (ptr[i])
			return True;
	}
	return False;
}
