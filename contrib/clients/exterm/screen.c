/*
 * $Id: screen.c,v 1.4 91/08/26 08:48:17 proj Exp $
 */
/*
 *	$XConsortium: screen.c,v 1.30 91/08/22 16:27:13 gildea Exp $
 */
/*
 * Copyright 1990, 1991 by OMRON Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of OMRON not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * OMRON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL OMRON BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 *	Author:	LU Qunyun	OMRON Corporation
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

#ifdef XLOCALE
#include <setjmp.h>
extern XtermWidget term;
extern XlcInfoTab       *CurXlcInfo;
#ifdef XML
extern XIC xic;
#endif /* XML */
#endif  /* XLOCALE */

extern Char *calloc(), *malloc(), *realloc();
extern void free();

ScrnBuf Allocate (nrow, ncol, addr)
/*
   allocates memory for a 2-dimensional array of chars and returns a pointer
   thereto
   each line is formed from a pair of char arrays.  The first (even) one is
   the actual character array and the second (odd) one is the attributes.
 */
register int nrow, ncol;
#ifdef XLOCALE
wchar **addr;
#else
Char **addr;
#endif /* XLOCALE */
{
	register ScrnBuf base;
#ifdef XLOCALE
        register wchar *tmp;
#else
	register Char *tmp;
#endif /* XLOCALE */
	register int i;

#ifdef XLOCALE
	if ((base = (ScrnBuf) calloc ((unsigned)(nrow *= 2), sizeof (wchar *))) == 0)
#else
	if ((base = (ScrnBuf) calloc ((unsigned)(nrow *= 2), sizeof (char *))) == 0)
#endif /* XLOCALE */
		SysError (ERROR_SCALLOC);

#ifdef XLOCALE
        if ((tmp=(wchar *)calloc ((unsigned) (nrow * ncol),sizeof(wchar))) == 0)
#else
	if ((tmp = calloc ((unsigned) (nrow * ncol), sizeof(char))) == 0)
#endif /* XLOCALE */
		SysError (ERROR_SCALLOC2);

	*addr = tmp;
	for (i = 0; i < nrow; i++, tmp += ncol)
		base[i] = tmp;

	return (base);
}

/*
 *  This is called when the screen is resized.
 *  Returns the number of lines the text was moved down (neg for up).
 *  (Return value only necessary with SouthWestGravity.)
 */
static
Reallocate(sbuf, sbufaddr, nrow, ncol, oldrow, oldcol)
    ScrnBuf *sbuf;
#ifdef XLOCALE
    wchar **sbufaddr;
#else
    Char **sbufaddr;
#endif /* XLOCALE */
    int nrow, ncol, oldrow, oldcol;
{
	register ScrnBuf base;
#ifdef XLOCALE
        register wchar *tmp;
#else
	register Char *tmp;
#endif /* XLOCALE */
	register int i, minrows, mincols;
#ifdef XLOCALE
        wchar *oldbuf;
#else
	Char *oldbuf;
#endif /* XLOCALE */
	int move_down = 0, move_up = 0;
	
	if (sbuf == NULL || *sbuf == NULL)
		return 0;

	oldrow *= 2;
	oldbuf = *sbufaddr;

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
	if (nrow < oldrow  &&  term->misc.resizeGravity == SouthWestGravity) {
	    /* Remove lines off the top of the buffer if necessary. */
	    move_up = oldrow-nrow 
		        - 2*(term->screen.max_row - term->screen.cur_row);
	    if (move_up < 0)
		move_up = 0;
#ifdef XLOCALE
	    /* Overlapping bcopy here! */
	    bcopy((char *)(*sbuf+move_up), (char *)(*sbuf),
		  (oldrow-move_up)*sizeof((*sbuf)[0]) );
#else
	    /* Overlapping bcopy here! */
	    bcopy(*sbuf+move_up, *sbuf,
		  (oldrow-move_up)*sizeof((*sbuf)[0]) );
#endif /* XLOCALE */
	}
#ifdef XLOCALE
	*sbuf = (ScrnBuf) realloc((wchar *) (*sbuf),
				  (unsigned) (nrow * sizeof(wchar *)));
#else
	*sbuf = (ScrnBuf) realloc((char *) (*sbuf),
				  (unsigned) (nrow * sizeof(char *)));
#endif /* XLOCALE */
	if (*sbuf == 0)
	    SysError(ERROR_RESIZE);
	base = *sbuf;

	/* 
	 *  create the new buffer space and copy old buffer contents there
	 *  line by line.
	 */
#ifdef XLOCALE
	if ((tmp = (wchar *)calloc((unsigned) (nrow * ncol), sizeof(wchar))) == 0)
#else
	if ((tmp = calloc((unsigned) (nrow * ncol), sizeof(char))) == 0)
#endif /* XLOCALE */
		SysError(ERROR_SREALLOC);
	*sbufaddr = tmp;
	minrows = (oldrow < nrow) ? oldrow : nrow;
	mincols = (oldcol < ncol) ? oldcol : ncol;
	if (nrow > oldrow  &&  term->misc.resizeGravity == SouthWestGravity) {
	    /* move data down to bottom of expanded screen */
	    move_down = Min(nrow-oldrow, 2*term->screen.savedlines);
	    tmp += ncol*move_down;
	}
	for (i = 0; i < minrows; i++, tmp += ncol) {
#ifdef XLOCALE
                wcopy(base[i], tmp, mincols);
#else
		bcopy(base[i], tmp, mincols);
#endif /* XLOCALE */
	}
	/*
	 * update the pointers in sbuf
	 */
	for (i = 0, tmp = *sbufaddr; i < nrow; i++, tmp += ncol)
	    base[i] = tmp;

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
#ifdef XLOCALE
wchar *str;
#else
char *str;
#endif /* XLOCALE */
register unsigned flags;
register int length;		/* length of string */
{
#ifdef XLOCALE
        register wchar *attrs;
#else
	register Char *attrs;
#endif /* XLOCALE */
	register int avail  = screen->max_col - screen->cur_col + 1;
#ifdef XLOCALE
        register wchar *col;
#else
	register Char *col;
#endif /* XLOCALE */

	if (length > avail)
	    length = avail;
	if (length <= 0)
		return;

	col = screen->buf[avail = 2 * screen->cur_row] + screen->cur_col;
	attrs = screen->buf[avail + 1] + screen->cur_col;
	flags &= ATTRIBUTES;
	flags |= CHARDRAWN;
#ifdef XLOCALE
        while(length-- > 0) {
                if (XwcGetColumn(*str) > 1)
                        *col++ = WPENDCHAR, *attrs++ = flags|WPENDCOLUMN;
                *col++ = *str++, *attrs++ = flags;
        }
#else
	bcopy(str, col, length);
	while(length-- > 0)
		*attrs++ = flags;
#endif /* XLOCALE */
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
#ifdef XLOCALE
        wchar *save [2 * MAX_ROWS];
#else
	char *save [2 * MAX_ROWS];
#endif /* XLOCALE */


	/* save n lines at bottom */
	bcopy ((char *) &sb [2 * (last -= n - 1)], (char *) save,
		2 * sizeof (char *) * n);
	
	/* clear contents of old rows */
	for (i = 2 * n - 1; i >= 0; i--)
#ifdef XLOCALE
                wbzero ((wchar *) save [i], size);
#else
		bzero ((char *) save [i], size);
#endif /* XLOCALE */

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
		2 * sizeof (char *) * (last - where));

	/* reuse storage for new lines at where */
	bcopy ((char *)save, (char *) &sb[2 * where], 2 * sizeof(char *) * n);
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
#ifdef XLOCALE
        wchar *save [2 * MAX_ROWS];
#else
	char *save [2 * MAX_ROWS];
#endif /* XLOCALE */

	/* save n lines at where */
	bcopy ((char *) &sb[2 * where], (char *)save, 2 * sizeof(char *) * n);

	/* clear contents of old rows */
	for (i = 2 * n - 1 ; i >= 0 ; i--)
#ifdef XLOCALE
                wbzero ((wchar *) save [i], size);
#else
		bzero ((char *) save [i], size);
#endif /* XLOCALE */

	/* move up lines */
	bcopy ((char *) &sb[2 * (where + n)], (char *) &sb[2 * where],
		2 * sizeof (char *) * ((last -= n - 1) - where));

	/* reuse storage for new bottom lines */
	bcopy ((char *)save, (char *) &sb[2 * last],
		2 * sizeof(char *) * n);
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
#ifdef XLOCALE
        register wchar *ptr = sb [2 * row];
        register wchar *attrs = sb [2 * row + 1];
#else
	register Char *ptr = sb [2 * row];
	register Char *attrs = sb [2 * row + 1];
#endif /* XLOCALE */
	int wrappedbit = attrs[0]&LINEWRAPPED;

	attrs[0] &= ~LINEWRAPPED; /* make sure the bit isn't moved */
	for (i = size - 1; i >= col + n; i--) {
		ptr[i] = ptr[j = i - n];
		attrs[i] = attrs[j];
	}

#ifdef XLOCALE
        for (i=col; i<col+n; i++)
            ptr[i] = atowc(' ');
#else
	for (i=col; i<col+n; i++)
	    ptr[i] = ' ';
#endif /* XLOCALE */
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
#ifdef XLOCALE
        register wchar *ptr = sb[2 * row];
        register wchar *attrs = sb[2 * row + 1];
#else
	register Char *ptr = sb[2 * row];
	register Char *attrs = sb[2 * row + 1];
#endif /* XLOCALE */
	register nbytes = (size - n - col);
	int wrappedbit = attrs[0]&LINEWRAPPED;

#ifdef XLOCALE
        wcopy (ptr + col + n, ptr + col, nbytes);
        wcopy (attrs + col + n, attrs + col, nbytes);
        wbzero (ptr + size - n, n);
        wbzero (attrs + size - n, n);
#else
	bcopy (ptr + col + n, ptr + col, nbytes);
	bcopy (attrs + col + n, attrs + col, nbytes);
	bzero (ptr + size - n, n);
	bzero (attrs + size - n, n);
#endif /* XLOCALE */
	if (wrappedbit)
	    attrs[0] |= LINEWRAPPED;
}


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
#ifdef XLOCALE
                GetAscent(screen->fset_norm);
#else
		screen->fnt_norm->ascent;
#endif /* XLOCALE */
	register int row;
	register int topline = screen->topline;
	int maxrow = toprow + nrows - 1;
	int scrollamt = screen->scroll_amt;
	int max = screen->max_row;
#ifdef XLOCALE
        int     npc;
        wchar   wstr[MAX_COLS];
        XFontSet        screen_fontset;
#endif /* XLOCALE */

	if(screen->cursor_col >= leftcol && screen->cursor_col <=
	 (leftcol + ncols - 1) && screen->cursor_row >= toprow + topline &&
	 screen->cursor_row <= maxrow + topline)
		screen->cursor_state = OFF;
	for (row = toprow; row <= maxrow; y += FontHeight(screen), row++) {
#ifdef XLOCALE
           register wchar *chars;
           register wchar *attrs;
#else
	   register Char *chars;
	   register Char *attrs;
#endif /* XLOCALE */
	   register int col = leftcol;
	   int maxcol = leftcol + ncols - 1;
	   int lastind;
	   int flags;
	   int x, n;
	   GC gc;
	   Boolean hilite;	

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
#ifdef XLOCALE
                        /* true, if zero or blank */
                          (chars[col] & ~(atowc(040))) == 0)
#else
			  (chars[col] & ~040) == 0)
#endif /* XLOCALE */
		       col++;

		   while (col <= maxcol && (attrs[maxcol] & ~BOLD) == 0 &&
#ifdef XLOCALE
                        /* true, if zero or blank */
                          (chars[maxcol] & ~(atowc(040))) == 0)
#else
			  (chars[maxcol] & ~040) == 0)
#endif /* XLOCALE */
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

#ifdef XLOCALE
           if ((attrs[col] & WPENDCOLUMN) == 0 &&
                        (XwcGetColumn(chars[col]) > 1)) col--;
           if (attrs[maxcol] & WPENDCOLUMN)
                maxcol++;
#endif /* XLOCALE */

	   if (col > maxcol) continue;

	   flags = attrs[col];

	   if ( (!hilite && (flags & INVERSE) != 0) ||
	        (hilite && (flags & INVERSE) == 0) )
#ifdef XLOCALE
               if (flags & BOLD) {
                        screen_fontset = screen->fset_bold;
                        gc = screen->reverseboldGC;
                }
                else {
                        screen_fontset = screen->fset_norm;
                        gc = screen->reverseGC;
                }
#else
	       if (flags & BOLD) gc = screen->reverseboldGC;
	       else gc = screen->reverseGC;
#endif /* XLOCALE */
	   else 
#ifdef XLOCALE
               if (flags & BOLD) {
                        screen_fontset = screen->fset_bold;
                        gc = screen->normalboldGC;
                }
               else {
                        screen_fontset = screen->fset_norm;
                        gc = screen->normalGC;
                }
#else
	       if (flags & BOLD) gc = screen->normalboldGC;
	       else gc = screen->normalGC;
#endif /* XLOCALE */

	   x = CursorX(screen, col);
	   lastind = col;

	   for (; col <= maxcol; col++) {
#ifdef XLOCALE
      /*determine if attrs[col] is equal to flags, except WPENDCOLUMN*/
                if ((attrs[col] & ~WPENDCOLUMN) != (flags & ~WPENDCOLUMN)) {
                   npc = wFilterWPEND(wstr, &chars[lastind], n = col - lastind);
                   XwcDrawImageString(screen->display, TextWindow(screen),
                                screen_fontset, gc, x, y, wstr, n - npc);
                   if((flags & BOLD) && screen->enbolden)
                        XwcDrawString(screen->display, TextWindow(screen),
                                screen->fset_norm, gc,
                                x + 1, y, wstr, n - npc);
                   if(flags & UNDERLINE)
                        XDrawLine(screen->display, TextWindow(screen),
                         gc, x, y+1, x+n*FontWidth(screen), y+1);
#else
		if (attrs[col] != flags) {
		   XDrawImageString(screen->display, TextWindow(screen), 
		        	gc, x, y, (char *) &chars[lastind], n = col - lastind);
		   if((flags & BOLD) && screen->enbolden)
		 	XDrawString(screen->display, TextWindow(screen), 
			 gc, x + 1, y, (char *) &chars[lastind], n);
		   if(flags & UNDERLINE) 
			XDrawLine(screen->display, TextWindow(screen), 
			 gc, x, y+1, x+n*FontWidth(screen), y+1);
#endif /* XLOCALE */

		   x += (col - lastind) * FontWidth(screen);

		   lastind = col;

		   flags = attrs[col];

	   	   if ((!hilite && (flags & INVERSE) != 0) ||
		       (hilite && (flags & INVERSE) == 0) )
#ifdef XLOCALE
                        if (flags & BOLD) {
                                screen_fontset = screen->fset_bold;
                                gc = screen->reverseboldGC;
                        }
                        else {
                                screen_fontset = screen->fset_norm;
                                gc = screen->reverseGC;
                        }
#else
	       		if (flags & BOLD) gc = screen->reverseboldGC;
	       		else gc = screen->reverseGC;
#endif /* XLOCALE */
	  	    else 
#ifdef XLOCALE
                         if (flags & BOLD) {
                                screen_fontset = screen->fset_bold;
                                gc = screen->normalboldGC;
                         }
                         else {
                                screen_fontset = screen->fset_norm;
                                gc = screen->normalGC;
                         }
#else
	      		 if (flags & BOLD) gc = screen->normalboldGC;
	      		 else gc = screen->normalGC;
#endif /* XLOCALE */
		}

		if(chars[col] == 0)
#ifdef XLOCALE
                        chars[col] = atowc(' ');
#else
			chars[col] = ' ';
#endif /* XLOCALE */
	   }


	   if ( (!hilite && (flags & INVERSE) != 0) ||
	        (hilite && (flags & INVERSE) == 0) )
#ifdef XLOCALE
               if (flags & BOLD) {
                        screen_fontset = screen->fset_bold;
                        gc = screen->reverseboldGC;
                }
               else {
                        screen_fontset = screen->fset_norm;
                        gc = screen->reverseGC;
                }
#else
	       if (flags & BOLD) gc = screen->reverseboldGC;
	       else gc = screen->reverseGC;
#endif /* XLOCALE */
	   else 
#ifdef XLOCALE
               if (flags & BOLD) {
                        screen_fontset = screen->fset_bold;
                        gc = screen->normalboldGC;
                }
               else {
                        screen_fontset = screen->fset_norm;
                        gc = screen->normalGC;
                }
#else
	       if (flags & BOLD) gc = screen->normalboldGC;
	       else gc = screen->normalGC;
#endif /* XLOCALE */

#ifdef XLOCALE
           npc = wFilterWPEND(wstr, &chars[lastind], n = col - lastind);
           XwcDrawImageString(screen->display, TextWindow(screen),
                 screen_fontset, gc, x, y, wstr, n - npc);
           if((flags & BOLD) && screen->enbolden)
                XwcDrawString(screen->display, TextWindow(screen),
                screen->fset_bold, gc, x + 1, y, wstr, n - npc);
           if(flags & UNDERLINE)
                XDrawLine(screen->display, TextWindow(screen), gc,
                 x, y+1, x + n * FontWidth(screen), y+1);
#else
	   XDrawImageString(screen->display, TextWindow(screen), gc, 
	         x, y, (char *) &chars[lastind], n = col - lastind);
	   if((flags & BOLD) && screen->enbolden)
		XDrawString(screen->display, TextWindow(screen), gc,
		x + 1, y, (char *) &chars[lastind], n);
	   if(flags & UNDERLINE) 
		XDrawLine(screen->display, TextWindow(screen), gc, 
		 x, y+1, x + n * FontWidth(screen), y+1);
#endif /* XLOCALE */
	}
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
#ifdef XLOCALE
                wbzero (screen->buf [first++], (screen->max_col + 1));
#else
		bzero (screen->buf [first++], (screen->max_col + 1));
#endif /* XLOCALE */
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
		    screen->border, height,           /* from top to bottom */
		    False);
	XClearArea (screen->display, tw, 
		    0, height - screen->border,	                  /* bottom */
		    width, screen->border,         /* all across the bottom */
		    False);

	/* round so that it is unlikely the screen will change size on  */
	/* small mouse movements.					*/
	rows = (height + FontHeight(screen) / 2 - border) /
	 FontHeight(screen);
	cols = (width + FontWidth(screen) / 2 - border - screen->scrollbar) /
	 FontWidth(screen);
	if (rows < 1) rows = 1;
	if (cols < 1) cols = 1;

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
#ifdef XLOCALE
		    (void) Reallocate(&screen->altbuf, (wchar **)&screen->abuf_address,
			 rows, cols, screen->max_row + 1, screen->max_col + 1);
		move_down_by = Reallocate(&screen->allbuf,
					  (wchar **)&screen->sbuf_address,
					  rows + savelines, cols,
					  screen->max_row + 1 + savelines,
					  screen->max_col + 1);
#else
		    (void) Reallocate(&screen->altbuf, (Char **)&screen->abuf_address,
			 rows, cols, screen->max_row + 1, screen->max_col + 1);
		move_down_by = Reallocate(&screen->allbuf,
					  (Char **)&screen->sbuf_address,
					  rows + savelines, cols,
					  screen->max_row + 1 + savelines,
					  screen->max_col + 1);
#endif /* XLOCALE */
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

		if (screen->cur_row > screen->max_row)
			screen->cur_row = screen->max_row;
		if (screen->cur_col > screen->max_col)
			screen->cur_col = screen->max_col;
	
		screen->fullVwin.height = height - border;
		screen->fullVwin.width = width - border - screen->scrollbar;

#ifdef XLOCALE
        }
#else
	} else if(FullHeight(screen) == height && FullWidth(screen) == width)
	 	return(0);	/* nothing has changed at all */
#endif /* XLOCALE */

	if(screen->scrollWidget)
		ResizeScrollBar(screen->scrollWidget, -1, -1, height);
	
	screen->fullVwin.fullheight = height;
	screen->fullVwin.fullwidth = width;
	ResizeSelection (screen, rows, cols);
#ifdef XLOCALE
        if(screen->ximon &&
		((term->misc.immode == OVERSPOT) || 
			(term->misc.immode == OFFSPOT))){
        	if(screen->cur_row == screen->max_row) {
        		Scroll(screen, 1);
			screen->cur_row--;
                	XClearArea (screen->display, TextWindow (screen),
                            (int)screen->border,
                            (int)(FullHeight(screen) - FontHeight(screen)
                                   -  screen->border),
                            (unsigned)Width(screen),
                            (unsigned)FontHeight(screen),
                            False);
        	}
                screen->max_row--;
                screen->bot_marg = screen->max_row;
#ifdef sun
#ifdef TIOCSSIZE
       		/* Set tty's idea of window size */
       		ts.ts_lines = rows - 1;
       		ts.ts_cols = cols;
       		ioctl (screen->respond, TIOCSSIZE, &ts);
#ifdef SIGWINCH
       		if(screen->pid > 1) {
               		int     pgrp;

               		if (ioctl (screen->respond, TIOCGPGRP, &pgrp) != -1)
                        	killpg(pgrp, SIGWINCH);
        	}
#endif  /* SIGWINCH */
#endif  /* TIOCSSIZE */
#else   /* sun */
#ifdef TIOCSWINSZ
        	/* Set tty's idea of window size */
                ws.ws_row = rows -1;
                ws.ws_col = cols;
                ws.ws_xpixel = width;
                ws.ws_ypixel = height;
                ioctl (screen->respond, TIOCSWINSZ, (char *)&ws);
#ifdef notdef   /* change to SIGWINCH if this doesn't work for you */
        	if(screen->pid > 1) {
                	int     pgrp;

                	if (ioctl (screen->respond, TIOCGPGRP, &pgrp) != -1)
                        	killpg(pgrp, SIGWINCH);
        	}
#endif  /* SIGWINCH */
#endif  /* TIOCSWINSZ */
#endif  /* sun */
#ifdef XML
		ModifyICValue(xic);
#else /* XML */
		ModifyICValue(CurXlcInfo->ic);
#endif /* XML */
	}
        else {
		/* root window, window size is unchanged. */
#ifdef sun
#ifdef TIOCSSIZE
       		/* Set tty's idea of window size */
       		ts.ts_lines = rows;
        	ts.ts_cols = cols;
        	ioctl (screen->respond, TIOCSSIZE, &ts);
#ifdef SIGWINCH
        	if(screen->pid > 1) {
                	int     pgrp;

                	if (ioctl (screen->respond, TIOCGPGRP, &pgrp) != -1)
                        	killpg(pgrp, SIGWINCH);
        	}
#endif  /* SIGWINCH */
#endif  /* TIOCSSIZE */
#else   /* sun */
#ifdef TIOCSWINSZ
        	/* Set tty's idea of window size */
                ws.ws_row = rows;
               	ws.ws_col = cols;
                ws.ws_xpixel = width;
               	ws.ws_ypixel = height;
                ioctl (screen->respond, TIOCSWINSZ, (char *)&ws);
#ifdef notdef   /* change to SIGWINCH if this doesn't work for you */
        	if(screen->pid > 1) {
                	int     pgrp;

                	if (ioctl (screen->respond, TIOCGPGRP, &pgrp) != -1)
                        	killpg(pgrp, SIGWINCH);
        	}
#endif  /* SIGWINCH */
#endif  /* TIOCSWINSZ */
#endif  /* sun */
	}
#else	/* XLOCALE */
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
#endif  /* XLOCALE */
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
#ifdef XLOCALE
        register wchar *attrs;
#else
	register Char *attrs;
#endif /* XLOCALE */
	register int avail  = screen->max_col - col + 1;

	if (length > avail)
	    length = avail;
	if (length <= 0)
		return;
	attrs = screen->buf[2 * row + 1] + col;
	value &= mask;	/* make sure we only change the bits allowed by mask*/
	while(length-- > 0) {
		*attrs &= ~mask;	/* clear the bits */
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
#ifdef XLOCALE
wchar *str;
#else
Char *str;
#endif /* XLOCALE */
register int length;		/* length of string */
{
#ifdef XLOCALE
        register wchar *attrs;
#else
	register Char *attrs;
#endif /* XLOCALE */
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
#ifdef XLOCALE
        register wchar *ptr = sb [2 * row];
#else
	register Char *ptr = sb [2 * row];
#endif /* XLOCALE */

	for (i = col; i < len; i++)	{
		if (ptr[i])
			return True;
	}
	return False;
}
