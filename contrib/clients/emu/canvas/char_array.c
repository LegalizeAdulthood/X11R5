#ifndef lint
static char *rcsid = "$Header: /usr3/emu/canvas/RCS/char_array.c,v 1.13 90/11/20 17:39:47 jkh Exp Locker: me $";
#endif

/*
 * This file is part of the PCS emu system.
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
 * character array manipulation routines
 *
 * Author: Michael Elbel
 * Date: March 20th, 1990.
 * Description: here are all routines that manipulate the structure of
 * 		character arrays including creation, resize, deleting lines
 * 		and so on.
 *
 * Revision History:
 *
 * $Log:	char_array.c,v $
 * Revision 1.13  90/11/20  17:39:47  jkh
 * Alpha.
 * 
 * Revision 1.13  90/10/26  08:27:54  me
 * whatever Jordan did
 * 
 * Revision 1.12  90/10/12  12:40:40  me
 * checkmark
 * 
 * Revision 1.11  90/08/31  19:51:41  me
 * Checkpoint vor dem Urlaub
 * 
 * Revision 1.10  90/08/09  10:19:06  me
 * fixed shift_array_line's declaration to also contain the widget
 *  * Revision 1.9  90/07/26  02:38:26  jkh
 * Added new copyright.
 * 
 * Revision 1.8  90/07/18  16:32:35  me
 * checkpoint
 * 
 * Revision 1.7  90/06/13  18:15:46  me
 * Removed usage of cur_array and cur_widget
 * 
 * Revision 1.6  90/06/07  15:36:25  me
 * This and that
 * 
 * Revision 1.5  90/05/11  14:20:11  me
 * resize_char_array added
 * 
 * Revision 1.4  90/05/08  17:32:36  me
 * Support for the tab functions added.
 * 
 * Revision 1.3  90/05/08  14:32:20  me
 * Jordan's infamous changes
 * 
 * Revision 1.2  90/05/04  13:39:15  me
 * Fixed interpolation of delete and insert line requests.
 * 
 * Revision 1.1  90/04/12  14:29:47  me
 * Initial revision
 * 
 *
 */

#include "canvas.h"

#ifdef TRACK_MALLOC
static int tab_list_count = 0;
static int char_array_count = 0;
static int line_array_count = 0;
static int saved_area_count = 0;
static int saved_line_count = 0;
#endif

Export TabList
create_tab_list(columns)
int columns;
{
     TabList list;

     if ((list = (TabList)XtMalloc(columns * sizeof(int))) == NULL)
	  fatal("couldn't allocate Tab List");
#ifdef TRACK_MALLOC
     debug("allocated %d bytes for tab list %d", columns * sizeof(int),
	   tab_list_count++);
#endif
     return(list);
}

/* Clears a Tab List */
Export void
clear_tab_list(list, cols)
TabList list;
int cols;
{
     bzero(list, cols * sizeof(int));
}

/* Initializes a Tab List to fixed steps */
Export void
init_fixed_tabs(list, cols, step)
register TabList list;
register int cols, step;
{
     register int i;

     for (i = 0; i < cols; i += step)
	  *(list + i) = True;
}

/*
 * Creates a new TabList with the specified size and copies as many
 * old tabs over as will fit in the new list.
 * The old list is free'd.
 */
Export TabList
resize_tab_list(old_list, old_col, new_col)
TabList old_list;
int old_col, new_col;
{
     TabList new;

     new = create_tab_list(new_col);
     if (new_col < old_col) {
	  bcopy(old_list, new, new_col * sizeof(TabListElem));
     }
     else {
	  clear_tab_list(new, new_col);
	  bcopy(old_list, new, old_col * sizeof(TabListElem));
     }

     XtFree(old_list);
#ifdef TRACK_MALLOC
     debug("Freeing tab array %d", --tab_list_count);
#endif
     
     return(new);
}

/*
 * Creates a char_array of the specified size and clears it.
 *
 * If either Dimension is 0, set it to 1.
 */
Export CharArray
create_char_array(lin, col)
int lin,col;
{
     register int i,len;
     CharArray array;

     if (lin == 0) {
	  lin = 1;
     }
     if (col == 0) {
	  col = 1;
     }
     
     /* first create the Line Array */
     
     if ((array = (LinePtr)XtMalloc(lin * sizeof(Line))) == NULL)
	  fatal("couldn't allocate Line Pointers for char_array");
     len = col * sizeof(Char);
#ifdef TRACK_MALLOC
     debug("Allocated %d bytes for char array %d", lin * sizeof(Line),
	   char_array_count++);
#endif
     /* next create the lines */
     for (i = 0; i < lin; i++) {
	  if ((array[i] = (Line)XtMalloc(len)) == NULL)
	       fatal("couldn't allocate Line %d for char_array",i);
	  bzero(array[i], len);
#ifdef TRACK_MALLOC
	  debug("Allocated %d bytes for line %d", len, line_array_count++);
#endif
     }
     
     return(array);
}

/*
 * Frees a char_array of the given size, can also be used to free
 * the save_array, since it checks if lines are allocated.
 */
Export void
free_char_array(array, lines)
CharArray array;
int lines;
{
     register int i;
     register LinePtr ptr;
     
     /* first free all lines */
     for (ptr = array, i = 0; i < lines; i++, ptr++)
	  if (*ptr != NULL)
	       XtFree((caddr_t)*ptr);

     /* now free the array itself */
     XtFree((caddr_t)array);
}

/*
 * Creates a new CharArray with the specified size and copies the old
 * data into it. The old array gets erased.
 * Extra space will be added to the right and the bottom, it will
 * be initialized with zeroes.
 * Lines that have to be deleted get cut off below the cursor and then, if this
 * is not enough, on the top.
 * Columns are cut off at the right side.
 * Characters on the right will be lost, while top lines will be saved
 * in the save_area. This assumes, that the save_area has already been
 * resized.
 */
Export CharArray
resize_char_array(w, old_array, old_lin, old_col,
		  new_lin, new_col)
register TermCanvasWidget w;
CharArray old_array;
int old_lin, old_col, new_lin, new_col;
{
     register CharArray new;
     int i, len, dels, pos_dels;
     register LinePtr from, to;
     
     new = create_char_array(new_lin, new_col);
     len = (new_col < old_col ? new_col : old_col) * sizeof(Char);
     
     if (new_lin < old_lin) {
	  dels = old_lin - new_lin;
	  pos_dels = old_lin - w->term.array_cur.lin - 1;
	  
	  /* First delete lines under the cursor, don't save them */
	  for (from = old_array + (old_lin - 1);
	       (dels > 0) && (pos_dels > 0);
	       dels--, pos_dels--) {
	       XtFree((caddr_t)*from--);
#ifdef TRACK_MALLOC
	       debug("Freeing line %d", --line_array_count);
#endif
	  }

	  /* If there are any left over, delete them at the top */
	  if (dels > 0) {
	       save_array_lines(w, old_array, old_lin - new_lin);
	       from = old_array + dels;
	  }
	  else {
	       from = old_array;
	  }
     }
     else
	  from = old_array;
     to = new;

     /* copy the lines and free the old ones */
     for (i = old_lin < new_lin ? old_lin : new_lin; i; i--) {
	  bcopy(*from, *to++, len);
	  XtFree((caddr_t)*from++);
#ifdef TRACK_MALLOC
	  debug("Freeing line %d", --line_array_count);
#endif
     }

     /* free the old array */
     XtFree((caddr_t)old_array);
#ifdef TRACK_MALLOC
     debug("Freeing char array %d", --char_array_count);
#endif
     return((CharArray)new);
}

/*
 * Scrolls the area starting at start_line, which is "lines" long and
 * "scrolled_lines" up or down. Empty lines are inserted at the top or
 * bottom of the area.
 * Depending on saveflag the lines that scroll off are either saved via
 * 'save_array_lines' or reused for the new line.
 */

Export void
scroll_array(w, start_line, lines, scroll_lines, saveflag)
register TermCanvasWidget w;
int start_line, lines, scroll_lines;	
Boolean saveflag;	/* whether to save the deleted lines in the savearea */
{
     register int i,len;
     int abs_lines;
     Line *clear_start;
     register Line *tmp, *from, *to;
     
     Import int abs();

     abs_lines = abs(scroll_lines);
     /* number of bytes in a line */
     len = w->term.columns * sizeof(Char);
     
     if (scroll_lines < 0)
	  /* scroll down */
	  clear_start = w->term.char_array
	       + (start_line + lines + scroll_lines);
     else
	  /* scroll up */
	  clear_start = w->term.char_array + start_line;
     
     if (saveflag)
	  save_array_lines(w, clear_start, abs_lines);
     else
	  for (i = abs_lines, tmp = clear_start; i; i--, tmp++)
	       bzero(*tmp,len);
     
     /* save the cleared area */
#ifdef HAVE_ALLOCA
     tmp = (Line *)alloca(abs_lines * sizeof(Line));
#else
     tmp = (Line *)XtMalloc(abs_lines * sizeof(Line));
#ifdef TRACK_MALLOC
     debug("Allocated %d bytes for saved area %d", abs_lines * sizeof(Line),
	   ++saved_area_count);
#endif /* TRACK_MALLOC */
#endif /* HAVE_ALLOCA */
     
     for (i = abs_lines, from = clear_start, to = tmp; i; i--)
	  *(to++) = *(from++);

     
     /* move the lines */
     if (scroll_lines < 0) {
	  /* scroll down */
	  for (i = lines - abs_lines,
	       from = clear_start - 1, to = from + abs_lines; i; i--)
	       *(to--) = *(from--);
	  
	  /* put back the cleared area */
	  for (i = abs_lines, from = tmp,
	       to = w->term.char_array + start_line; i; i--)
	       *(to++) = *(from++);
     } else {
	  /* scroll up */
	  for (i = lines - abs_lines,
	       from = clear_start + abs_lines, to = clear_start; i; i--)
	       *(to++) = *(from++);
	  
	  /* put back the cleared area, to is already set right */
	  for (i = abs_lines, from = tmp ; i; i--)
	       *(to++) = *(from++);
     }
#ifndef HAVE_ALLOCA
     XtFree(tmp);
#ifdef TRACK_MALLOC
     debug("Freed saved area %d", --saved_area_count);
#endif /* TRACK_MALLOC */
#endif /* HAVE_ALLOCA */
}

/* Resizes the save_area's width */
Export void
resize_save_area(w, old_width, new_width)
register TermCanvasWidget w;
int old_width, new_width;
{
     register int i, len, cplen;
     register Line tmp, *ptr;
     
     /* If the width doesn't change, just get back */
     if (old_width == new_width)
	  return;

     len = new_width * sizeof(Char);
     cplen = (new_width < old_width ? new_width : old_width) * sizeof(Char);
     ptr = w->term.save_array;
     
     for (i = w->term.save_lines; i; i--) {
	  /* Is there a Line at all? */
	  if (*ptr != NULL) {
	       /* create a new line */
	       tmp = (Line)XtMalloc(len);
	       
	       /* clear it */
	       bzero(tmp, len);
	       
	       /* copy the stuff over */
	       bcopy(*ptr, tmp, cplen);

	       /* free the old one */
	       XtFree((caddr_t)*ptr);

	       *ptr = tmp;
	  }
	  ptr++;
     }
}

/* Appends lines to the save_area and creates new ones in the old place */
Export void
save_array_lines(w, start, lines)
register TermCanvasWidget w;
LinePtr start;
int lines;
{
     register int i;
     register LinePtr from, to, tmp;
     register int len = w->term.columns * sizeof(Char);

     /* Save the top of the save_array and create new lines if necessary */
#ifdef HAVE_ALLOCA
     tmp = (Line *)alloca(lines * sizeof(Line));
#else
     tmp = (Line *)XtMalloc(lines * sizeof(Line));
#ifdef TRACK_MALLOC
     debug("Allocated %d bytes for saved area %d", lines * sizeof(Line),
	   ++saved_area_count);
#endif /* TRACK_MALLOC */
#endif /* HAVE_ALLOCA */
     for (from = w->term.save_array, to = tmp, i = lines; i; i--) {
	  if (*from != NULL)
	       *to = *from;
	  else {
	       *to = (Line)XtMalloc(len);
#ifdef TRACK_MALLOC
	       debug("Allocated %d bytes for saved line %d", len,
		     ++saved_line_count);
#endif
	  }
	  bzero(*(to++), len);
	  from++;
     }
     
     /* Scroll the save_array up, from is already at the right position */
     to = w->term.save_array;

     for (i = w->term.save_lines - lines; i; i--)
	  *(to++) = *(from++);

     /* Now copy the lines to save into the save_array, to is already ok */
     from = start;
     for (i = lines; i; i--)
	  *(to++) = *(from++);

     /* put the saved or new lines in the old place */
     for (i = lines, to = start, from = tmp; i; i--)
	  *(to++) = *(from++);
     
     /* Adjust the save_size */
     if (w->term.save_size < w->term.save_lines) {
	  w->term.save_size += lines;
	  if (w->term.save_size > w->term.save_lines)
	       w->term.save_size = w->term.save_lines;
	  /*
	   * Tell the Parent to adjust the scrollbar
	   */
	  if (w->term.adjust_scroll_bar != NULL)
	       w->term.adjust_scroll_bar(XtParent(w),
					 (float)(w->term.save_size -
						 w->term.scroll_pos) /
					 (float)(w->term.lines +
						 w->term.save_size),
					 (float)w->term.lines /
					 (float)(w->term.lines +
						 w->term.save_size));
     }
}

/*
 * Deletes characters in the given line starting with the given column.
 * Characters to the right automatically get shifted left.
 */
Export void
delete_array_chars(w, line, start_col, no_chars, line_len)
register TermCanvasWidget w;
int line, start_col, no_chars, line_len;
{
     register CharPtr from, to;
     register int i;
     
     /* clip to the right border of the line */
     if ((start_col + no_chars) > line_len)
	  no_chars = line_len - start_col;
     
     /* set up copying pointers */
     to = w->term.char_array[line] + start_col;
     from = to + no_chars;
     
     /* copy the chars */
     for (i = line_len - start_col - no_chars; i; i--)
	  *to++ = *from++;

     /* clear the rest of the line */
     for (i = no_chars ; i; i--) {
	  to->value = 0;
	  to->attributes = 0;
	  to++;
     }
}

/*
 * Erases characters in the given line starting with the given column.
 * Characters to the right stay at their place
 */
Export void
erase_array_chars(w, line, start_col, no_chars, line_len)
register TermCanvasWidget w;
int line, start_col, no_chars, line_len;
{
     register CharPtr ptr;
     register int i;
     
     /* clip to the right border of the line */
     if ((start_col + no_chars) > line_len)
	  no_chars = line_len - start_col;
     
     /* set up pointer */
     ptr = w->term.char_array[line] + start_col;
     
     /* clear the chars */
     for (i = no_chars; i; i--) {
	  ptr->value = 0;
	  ptr->attributes = 0;
	  ptr++;
     }
}

/*
 * Clears a number of lines in the array (e.g. for a clear screen operation)
 */
Export void
erase_array_lines(w, start, no)
register TermCanvasWidget w;
int start;
register int no;
{
     register LinePtr linptr = w->term.char_array + start;
     register int len = w->term.columns * sizeof(Char);
     
     while (no--)
	  bzero(*linptr++, len);
}

/*
 * Puts the given string into the array with the given attribute.
 */
Export void
put_string_in_array(w, buffer, cnt, lin, col, attrib, color)
register TermCanvasWidget w;
register char *buffer;
register char cnt;
int lin, col;
register unsigned char attrib, color;
{
     register CharPtr to = w->term.char_array[lin] + col;

     for (; cnt; cnt--) {
	  to->value = *buffer++;
	  to->attributes = attrib;
	  to->color = color;
	  to++;
     }
}

/*
 * Shift the given line starting at 'col' 'cnt' positions to the right.
 * The created space is NOT cleared.
 */
Export void
shift_array_line(w, line, col, cnt, cols)
register TermCanvasWidget w;
Line line;
register int col, cnt, cols;
{
     if ((col + cnt) >= cols)
	  return;

     bcopy(line + col, line + col + cnt, (cols - col - cnt) * sizeof(Char));
}
