#ifndef lint
static char *rcsid = "$Header: /usr3/emu/canvas/RCS/functions.c,v 1.20 91/07/29 13:40:13 me Exp Locker: me $";
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
 * General interface Functions between the Canvas and the rest of emu.
 *
 * Author: Michael Elbel
 * Date: March 20th, 1990.
 * Description: Here we have many of the functions the canvas makes available
 *
 * Revision History:
 *
 * $Log:	functions.c,v $
 * Revision 1.20  91/07/29  13:40:13  me
 * Added color stuff
 * 
 * Revision 1.19  91/03/01  16:44:31  jkh
 * Changed some include convetions.
 * 
 * Revision 1.18  91/03/01  16:39:09  jkh
 * Formatted function declarations differently so that TAGS will work.
 * 
 * Revision 1.17  90/11/20  17:39:53  jkh
 * Alpha.
 * 
 * Revision 1.16  90/10/12  12:40:44  me
 * checkmark
 * 
 * Revision 1.15  90/10/09  14:43:24  me
 * The cursor isn't xored any more
 * 
 * Revision 1.14  90/08/31  19:51:44  me
 * Checkpoint vor dem Urlaub
 * 
 * Revision 1.13  90/08/16  10:32:02  me
 * removed insert_blanks
 * 
 * Revision 1.12  90/08/09  10:16:45  me
 * using index instead of strchr now
 * 
 * Revision 1.11  90/07/26  02:39:16  jkh
 * Added new copyright.
 * 
 * Revision 1.10  90/07/18  16:32:43  me
 * checkpoint
 * 
 * Revision 1.9  90/07/18  09:24:51  me
 * checkpoint
 * 
 * Revision 1.8  90/06/13  10:10:06  me
 * Removed access of global widget, char_array and comblock,
 * now passing arguments instead
 * 
 * Revision 1.7  90/06/08  16:50:52  me
 * Checkpoint
 * 
 * Revision 1.6  90/06/07  15:35:16  me
 * Translation and FLUT handling routines added,
 * type checking in the Com Block now provided
 * 
 * Revision 1.4  90/05/08  17:31:35  me
 * Tab handling functions added.
 * 
 * Revision 1.3  90/05/08  14:29:11  me
 * Jordans infamous changes
 * 
 * Added Bell support
 * 
 * Revision 1.2  90/05/04  13:39:43  me
 * Fixed interpolation of delete and insert line requests.
 * 
 * Revision 1.1  90/04/12  14:29:03  me
 * Initial revision
 *
 */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include "canvas.h"
#include <X11/Shell.h>

Export IntFuncPtr jumptable[] = {
     insert,			/*  0 */
     move_abs,			/*  1 */
     move_rel,			/*  2 */
     move_abs_column,		/*  3 */
     move_abs_row,		/*  4 */
     move_rel_column,		/*  5 */
     move_rel_row,		/*  6 */
     move_rel_row_scrolled,	/*  7 */
     insert_mode,		/*  8 */
     overwrite_mode,		/*  9 */
     delete_chars,		/* 10 */
     delete_to_eol,		/* 11 */
     insert_lines,		/* 12 */
     delete_to_eoscr,		/* 13 */
     erase_chars,		/* 14 */
     erase_line_left,		/* 15 */
     erase_lines,		/* 16 */
     erase_from_toscr,		/* 17 */
     clear_screen,		/* 18 */
     dummy,			/* 19 */
     delete_lines,		/* 20 */
     set_scroll_region,		/* 21 */
     ring_bell,			/* 22 */
     hor_tab,			/* 23 */
     set_tab_cur_col,		/* 24 */
     set_tab_col,		/* 25 */
     set_tabs_eq_width,		/* 26 */
     clear_tab_cur_col,		/* 27 */
     clear_tab_col,		/* 28 */
     clear_all_tabs,		/* 29 */
     set_attribute,		/* 30 */
     clear_attribute,		/* 31 */
     override_translations,	/* 32 */
     change_flut,		/* 33 */
     canvas_size,		/* 34 */
     canvas_cursor_pos,		/* 35 */
     canvas_attribs,		/* 36 */
     canvas_scroll_region,	/* 37 */
     canvas_wrap_mode,		/* 38 */
     canvas_reverse_mode,	/* 39 */
     canvas_cursor_on,		/* 40 */
     canvas_cursor_blinking,	/* 41 */
     canvas_cursor_size,	/* 42 */
     redraw_screen,		/* 43 */
     change_fonts,		/* 44 */
     scroll_screen_absolute,	/* 45 */
     scroll_screen_relative,	/* 46 */
     cursor_off,		/* 47 */
     cursor_on,			/* 48 */
     set_screen_size,		/* 49 */
     wrap_around,		/* 50 */
     dont_wrap,			/* 51 */
     cursor_pos_rel_to_scr_reg,	/* 52 */
     cursor_pos_absolute,	/* 53 */
     reverse_video,		/* 54 */
     normal_video,		/* 55 */
     save_flut,			/* 56 */
     restore_flut,		/* 57 */
     set_cursor_blink,		/* 58 */
     set_cursor_size,		/* 59 */
     change_fg_color,		/* 60 */
     change_bg_color,		/* 61 */
     set_cit_cell,		/* 62 */
     canvas_display_cells,	/* 63 */
};

#ifdef TRACK_MALLOC
static int flut_malloc_count = 0;
#endif

Export int
dummy(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     warn("This opcode (%d) is not currently supported!",
	  cb_opcode(com_block));
     return(99);
}

/*
 * Move the cursor to lin and col.
 * If lin or col exceed the borders of the canvas, put the cursor to the
 * nearest border.
 */
Export int
move_cursor(w, lin, col)
register TermCanvasWidget w;
int lin, col;
{
     int retval = 0;
     
     if (w->term.rel_cursor_pos) {
	  if (lin < 0) {
	       lin = 0;
	       retval = 1;
	  } else if (lin > (w->term.scroll_bottom - w->term.scroll_top)) {
	       lin = w->term.scroll_bottom - w->term.scroll_top;
	       retval = 2;
	  }
     }
     else {
	  if (lin < 0) {
	       lin = 0;
	       retval = 1;
	  } else if (lin >= w->term.lines) {
	       lin = w->term.lines - 1;
	       retval = 2;
	  }
     }
     
     if (col < 0) {
	  col = 0;
	  retval = 3;
     } else if (col >= w->term.columns) {
	  col = w->term.columns - 1;
	  retval = 4;
     }
	  
     if (w->term.rel_cursor_pos)
	  lin += w->term.scroll_top;
     
     flip_cursor_off(w);
     
     set_cursor_pos(w, lin, col);
     
     return(retval);
}

/*
 * Move the cursor relative to the current position.
 * If the resulting lin or column would exceed the borders of the canvas,
 * put the cursor to the nearest border.
 */
Export int
move_cursor_rel(w, lin, col)
register TermCanvasWidget w;
register int lin,col;
{
     register int cl;
     int retval = 0;

     flip_cursor_off(w);
     
     cl = w->term.array_cur.lin;
     col += w->term.array_cur.col;
     lin += cl;

     /* restrain the line movement to the scrolling region */
     if ((cl >= w->term.scroll_top) && (lin < w->term.scroll_top)) {
	  lin = w->term.scroll_top;
	  retval = 1;
     }
     else if ((cl <= w->term.scroll_bottom) && (lin > w->term.scroll_bottom)) {
	  lin = w->term.scroll_bottom;
	  retval = 2;
     }
     
     /* check the columns */
     if (col < 0) {
	  col = 0;
	  retval = 3;
     } else if (col >= w->term.columns) {
	  col = w->term.columns - 1;
	  retval = 4;
     }
     
     set_cursor_pos(w, lin, col);
     
     return(retval);
}

/*
 * Parse the flut description and change the given flut accordingly
 */
Export void
parse_and_change_flut(flut_desc, flut, flut_offset)
String flut_desc;
unsigned char flut[];
int flut_offset;
{
     Import int atoi();
     
     register char *str, *ptr, *ptr1, *ptr2;
     int start, end, offset;
     
#ifdef HAVE_ALLOCA
     str = alloca(strlen(flut_desc) + 1);
#else
     char *savstr;
     savstr = str = XtMalloc(strlen(flut_desc) + 1);
#ifdef TRACK_MALLOC
     debug("Allocated %d bytes for flut %d", strlen(flut_desc) + 1,
	   ++flut_malloc_count);
#endif /* TRACK_MALLOC */
#endif /* HAVE_ALLOCA */

     strcpy(str, flut_desc);
     
     /*
      * parse the resource, format is:
      *
      * <position | range> : <offset>
      */
     while ((ptr = index(str, '\n')) != NULL) {
	  /* split it up */
	  *ptr++ = '\0';

	  /* search for the : */
	  if ((ptr1 = index(str, ':')) == NULL) {
	       /* unusable line */
	       str = ptr;
	       continue;
	  }

	  *ptr1++ = '\0';
	  
	  if ((ptr2 = index(str, '-')) == NULL) {
	       /* no range, just a single position */
	       end = start = atoi(str);
	  }
	  else {
	       /* start and end */
	       *ptr2++ = '\0';
	       start = atoi(str);
	       end = atoi(ptr2);
	  }

	  /* get the offset */
	  offset = atoi(ptr1);

	  /* do the actual changing */
	  if ((start < 0) || (start > 255) || (end < 0) || (end > 255)) {
	       warn("ChangeFlut: specified range %d - %d out of bounds",
		    start, end);
	       continue;
	  }
	  else {
	       set_flut(flut, start + flut_offset,
			offset, end - start + 1);
	  }
	       
	  str = ptr;
     }
#ifndef HAVE_ALLOCA
     XtFree(savstr);
#ifdef TRACK_MALLOC
     debug("Freed flut %d", --flut_malloc_count);
#endif /* TRACK_MALLOC */
#endif /* HAVE_ALLOCA */
}

/*
 * Redraw the given rectangle of the given char array.
 * If the scroll position is != 0, take that into account
 */
 
Export void
redraw_rect(widget, array, start_lin, start_col, end_lin, end_col)
TermCanvasWidget widget;
CharArray array;
int start_lin, start_col, end_lin, end_col;
{
     register int i, j, cnt;
     int draw_x, draw_y;
     char buffer[MAX_CHARS_IN_LINE];
     register CharPtr c;
     register TermCanvasWidget w = (TermCanvasWidget)widget;
     register unsigned char cur_attr = 0;
     register unsigned char cur_color = 0;
     unsigned char save_attr, save_color;
     int fh = w->term.cell_height;
     int fw = w->term.cell_width;
     GC gc = w->term.norm_gc;
     int spos = w->term.scroll_pos;
     int slen = w->term.save_lines;
     CharArray sarray = w->term.save_array;
     
     /* save the current attributes and color */
     save_attr = w->term.act_attribs;
     save_color = w->term.act_color;
     
     for (i = start_lin; i <= end_lin; i++) {
	  if (i < spos)
	       c = sarray[slen - spos + i] + start_col;
	  else
	       c = array[i - spos] + start_col;
	  
	  draw_y = (i +1) * fh - w->term.font_descent;
	  
	  for (j = start_col; j <= end_col;) {
	       cnt = 0;
	       
	       /* skip over null characters */
	       for (; (j <= end_col) && (c->value == 0); c++)
		    j++;
	       draw_x = j * fw;
	       
	       /* collect characters */
	       for (; (j <= end_col) && (c->value != 0)
		       && ((c->attributes & ~ATT_CONTINUE) == cur_attr)
		       && (c->color == cur_color); c++) {
		    buffer[cnt++] = c->value;
		    j++;
	       }
	       
	       /* draw the string if any */
	       if (cnt != 0) {
		    buffer[cnt] = 0;
		    draw_string(w, gc, cur_attr, draw_x, draw_y, buffer, cnt);
	       }
	       
	       /* do we have to change the current gc */
	       if ((j <= end_col) && (c->value != 0)) {
		    cur_attr = c->attributes & ~ATT_CONTINUE;
		    cur_color = c->color;
		    gc = validate_gc(w, cur_attr, cur_color);
	       }
	  }
     }
     
     /*
      * restore the current attributes
      */
     (void) validate_gc(w, save_attr, save_color);
     /*
      * If the cursor was in the specified area and on, flip it on
      */
     if (w->term.array_cur.lin >= start_lin &&
	 w->term.array_cur.lin <= end_lin &&
	 w->term.array_cur.col >= start_col &&
	 w->term.array_cur.col <= end_col &&
	 w->term.cursor_on) {
	      w->term.cursor_on = False;
	      flip_cursor_on(w);
     }
}

Export int
font_lookup_string(buffer, flut, cnt)
unsigned char *buffer;
register int cnt;
register unsigned char flut[];
{
     register int no = 0;
     register unsigned char *from = buffer;
     register unsigned char *to = buffer;

     for (; cnt; cnt--) {
	  if ((*to = flut[*from]) != 0) {
	       to++;
	       no++;
	  }
	  from++;
     }
     return(no);
}

Export void
set_flut(flut, start, offset, no)
unsigned char flut[];
register int start, offset, no;
{
     register unsigned char *ptr = flut + start;

     while (no--) {
	  *ptr++ = offset++;
     }
}

/*
 * Insert text at the current cursor postion. Depending on the value of
 * w->term.wrap_around the cursor will move to the next line.
 */
Export int
insert(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register unsigned char *bufptr = cb_buffer(com_block);
     register int cnt = cb_nbytes(com_block);
     register int col = w->term.array_cur.col;
     int cols = w->term.columns;
     int lin = w->term.array_cur.lin;
     register unsigned char attrib = w->term.act_attribs;
     register unsigned char color = w->term.act_color;
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     flip_cursor_off(w);

     /*
      * Translate the incoming characters into ISO ones depending
      * on the current FLUTS. This replaces the original characters
      * in the buffer.
      */
     cnt = font_lookup_string(bufptr, w->term.flut, cnt);

     while (cnt > 0) {
	  if ((col + cnt) > cols) {
	       /* check the selection */
	       if (w->term.select_on &&
		   in_selected_area(w, lin, col, cols - 1))
		    unmark_selection(w);
		   
	       /*
		* No need to check for inserting since we're writing
		* till the end of the line anyways
		*/
	       put_string_in_array(w, bufptr, cols - col, lin, col,
				   attrib, color);
	       
	       /*
		* set the continuation attribute on the last character
		* so we can reconstruct the string on cutting
		*/
	       (w->term.char_array)[lin][w->term.columns - 1].attributes
		    |= ATT_CONTINUE;
	       
	       draw_string(w, w->term.gc, attrib, col * w->term.cell_width,
			   (lin + 1) * w->term.cell_height
			   - w->term.font_descent, bufptr, cols - col);
	       if (w->term.wrap_around) {
		    bufptr += cols - col;
		    cnt -= cols - col;
		    col = 0;
		    
		    /*
		     * do we have to scroll the screen 'cause we were
		     * at the last line of the scrolling area?
		     */
		    if (lin == w->term.scroll_bottom) {
			 scroll_selection(w, w->term.scroll_top,
				       lin - w->term.scroll_top + 1, 1);
			 scroll_screen(w, w->term.scroll_top,
				       lin - w->term.scroll_top + 1, 1);
			 /*
			  * delete the first line in the scrolling region,
			  * but save it only if the scrolling region starts
			  * at the first line
			  */
			 scroll_array(w, w->term.scroll_top, lin -
				      w->term.scroll_top + 1,
				      1, w->term.scroll_top == 0);
		    } else {
			 if (lin < (w->term.lines - 1))
			      lin++;
		    }
	       }
	       else {
		    /*
		     * just set the counter to 0 ,
		     * the col to the right border so we exit
		     */
		    cnt = 0;
	       }
	  }
	  else {
	       /* If we are inserting we gotta shift the rest of the line */
	       if (w->term.insert_mode) {
		    /*
		     * check the selection (the full line, cause I don't
		     * want to deal with moving the selection start if it
		     * would get shifted
		     */
		    if (w->term.select_on &&
			in_selected_area(w, lin, col, cols - 1))
			 unmark_selection(w);
		    
		    shift_array_line(w, w->term.char_array[lin],
				     col, cnt, cols);
		    shift_screen_line(w, lin, col, cnt, cols);
	       }
	       else
		    /* check the selection */
		    if (w->term.select_on &&
			in_selected_area(w, lin, col, col + cnt - 1))
			unmark_selection(w);
	       
	       put_string_in_array(w, bufptr, cnt, lin, col, attrib, color);
	       draw_string(w, w->term.gc, attrib, col * w->term.cell_width,
			   (lin + 1) * w->term.cell_height
				- w->term.font_descent, bufptr, cnt);
	       
	       col += cnt;
	       cnt = 0;
	  }
     }
     w->term.array_cur.lin = lin;
     w->term.array_cur.col = col;
     w->term.screen_cur.y = (lin + 1) * w->term.cell_height;
     
     /* keep the screen_cursor onscreen if sitting on the rightmost char */
     if (col == cols)
	  col--;
     
     w->term.screen_cur.x = col * w->term.cell_width;
     
     validate_cursor_gc(w, w->term.char_array[lin][col]);
     
     if (w->term.no_block_cursor)
	  XSetClipOrigin(XtDisplay(w), w->term.cursor_text_gc,
			 w->term.screen_cur.x,
			 w->term.screen_cur.y - w->term.cursor_height);
     
     return(0);
}

Export int
move_abs(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     check_arg_type(com_block, 'y', CB_INT_TYPE, "move_abs", 9);
     check_arg_type(com_block, 'x', CB_INT_TYPE, "move_abs", 9);
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     return(move_cursor(w, (int)cb_reg_data(com_block, 'y'),
			(int)cb_reg_data(com_block, 'x')));
}

Export int
move_rel(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     check_arg_type(com_block, 'y', CB_INT_TYPE, "move_rel", 9);
     check_arg_type(com_block, 'x', CB_INT_TYPE, "move_rel", 9);
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     return(move_cursor_rel(w, (int)cb_reg_data(com_block, 'y'),
			    (int)cb_reg_data(com_block, 'x')));
}

Export int
move_abs_column(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     check_arg_type(com_block, 'x', CB_INT_TYPE, "move_abs_column", 9);
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     return(move_cursor(w, w->term.array_cur.lin,
			(int)cb_reg_data(com_block, 'x')));
}

Export int
move_abs_row(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     check_arg_type(com_block, 'y', CB_INT_TYPE, "move_abs_row", 9);
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     return(move_cursor(w, (int)cb_reg_data(com_block, 'y'),
			w->term.array_cur.col));
}

Export int
move_rel_column(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     check_arg_type(com_block, 'x', CB_INT_TYPE, "move_rel_column", 9);
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     return(move_cursor_rel(w, 0, (int)cb_reg_data(com_block, 'x')));
}

Export int
move_rel_row(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     check_arg_type(com_block, 'y', CB_INT_TYPE, "move_rel_row", 9);
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     return(move_cursor_rel(w, (int)cb_reg_data(com_block, 'y'), 0));
}

/*
 * Move the cursor relative to its current position and scroll the screen
 * if necessary.
 */
Export int
move_rel_row_scrolled(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register int line = w->term.array_cur.lin;
     register int delta = (int)cb_reg_data(com_block, 'y');
     register int st = w->term.scroll_top;
     register int sb = w->term.scroll_bottom;
     
     int retval = 0;

     check_arg_type(com_block, 'y', CB_INT_TYPE, "move_rel_row_scrolled", 9);
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     if ((line <= sb) && (delta > 0)) {
	  /* move down within the scrolling area */
	  if ((line + delta) > sb) {
	       register int col = w->term.array_cur.col;
	       if (col == w->term.columns)
		    col--;
		    
	       flip_cursor_off(w);
	       
	       scroll_selection(w, st, sb - st + 1, line + delta - sb);
	       scroll_screen(w, st, sb - st + 1, line + delta - sb);
	       scroll_array(w, st, sb - st + 1, line + delta - sb, st == 0);
	       w->term.array_cur.lin = sb;
	       w->term.screen_cur.y = (sb + 1) * w->term.cell_height;
	       
	       validate_cursor_gc(w, w->term.char_array[sb][col]);
	       if (w->term.no_block_cursor)
		    XSetClipOrigin(XtDisplay(w), w->term.cursor_text_gc,
				   w->term.screen_cur.x,
				   w->term.screen_cur.y -
				   w->term.cursor_height);
	  } else
	       retval = move_cursor(w, line + delta, w->term.array_cur.col);
     }
     else if ((line >= st) && (delta < 0)) {
	  /* move up within the scrolling area */
	  if ((line + delta) < st) {
	       register int col = w->term.array_cur.col;
	       if (col == w->term.columns)
		    col--;
	       
	       flip_cursor_off(w);
	       
	       scroll_selection(w, st, sb - st + 1, delta + line - st);
	       scroll_screen(w, st, sb - st + 1, delta + line - st);
	       scroll_array(w, st, sb -st + 1, delta + line - st, False);
	       w->term.array_cur.lin = st;
	       w->term.screen_cur.y = (st + 1) * w->term.cell_height;
	       validate_cursor_gc(w, w->term.char_array[st][col]);
	       
	       if (w->term.no_block_cursor)
		    XSetClipOrigin(XtDisplay(w), w->term.cursor_text_gc,
				   w->term.screen_cur.x,
				   w->term.screen_cur.y -
				   w->term.cursor_height);
	  } else
	       retval = move_cursor(w, line + delta, w->term.array_cur.col);
     }
     else
	  /* outside scrolling area */
	  retval = move_cursor_rel(w, delta, 0);

     return(retval);
}

Export int
insert_mode(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     w->term.insert_mode = True;
     return(0);
}

Export int
overwrite_mode(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     w->term.insert_mode = False;
     return(0);
}

/*
 * Delete Characters in the current line
 * starting with the one under the cursor.
 */
Export int
delete_chars(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register int line = w->term.array_cur.lin;
     register int col = w->term.array_cur.col;
     register int no_dels = (int)cb_reg_data(com_block, 'a');
     register int line_len = w->term.columns;

     check_arg_type(com_block, 'a', CB_INT_TYPE, "delete_chars", 9);
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     flip_cursor_off(w);
     
     delete_array_chars(w, line, col, no_dels, line_len);
     delete_screen_chars(w, line, col, no_dels, line_len);
     
     validate_cursor_gc(w, w->term.char_array[line][col]);
     
     return(0);
}

Export int
delete_to_eol(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register int line = w->term.array_cur.lin;
     register int col = w->term.array_cur.col;
     register int line_len = w->term.columns;
     register int no_era = line_len - col;
     
     Char ch;
     ch.value = 0;
     ch.attributes = 0;
     
     flip_cursor_off(w);
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     erase_array_chars(w, line, col, no_era, line_len);
     erase_screen_chars(w, line, col, no_era, line_len);
     
     validate_cursor_gc(w, ch);
     
     return(0);
}

/*
 * Deletes Lines starting with the one where the cursor is, the cursor
 * stays in the same line but is set to the first column.
 *
 * Only lines whithin the scrolling region are deleted.
 *
 * A retval of 1 means the current line is outside the scrolling area,
 * A retval of 2 means the number of lines to delete had to be clipped
 * to the scrolling area.
 */
Export int
delete_lines(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register int line = w->term.array_cur.lin;
     register int no = (int)cb_reg_data(com_block, 'a');
     register int st = w->term.scroll_top;
     register int sb = w->term.scroll_bottom;
     
     int retval = 0;

     check_arg_type(com_block, 'a', CB_INT_TYPE, "delete_lines", 9);
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     if ((line > sb) || (line < st))
	  /* outside the scrolling area */
	  return(1);
     
     /* within the scrolling area */
     /* clip to the bottom of the scrolling area */
     if ((line + no) > sb) {
	  no = sb - line;
	  retval = 2;
     }
     
     flip_cursor_off(w);
	  
     /* do the actual scrolling */
     scroll_selection(w, line, sb - line + 1, - no);
     scroll_screen(w, line, sb - line + 1, - no);
     scroll_array(w, line, sb - line + 1, - no, False);

     /* This takes care of the cursor GCs */
     set_cursor_pos(w, line, 0);
     
     return(retval);
}

Export int
delete_to_eoscr(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register int line = w->term.array_cur.lin;
     register int col = w->term.array_cur.col;
     register int line_len = w->term.columns;
     register int no_era = line_len - col;
     
     Char ch;
     ch.value = 0;
     ch.attributes = 0;
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     flip_cursor_off(w);
     
     erase_array_chars(w, line, col, no_era, line_len);
     erase_screen_chars(w, line, col, no_era, line_len);
     if (line < (w->term.lines - 1)) {
	  erase_array_lines(w, line + 1, w->term.lines - line - 1);
	  erase_screen_lines(w, line + 1, w->term.lines - line - 1);
     }
     
     validate_cursor_gc(w, ch);
     
     return(0);
}

/* Erase Characters on the current line starting with the cursor position */
Export int
erase_chars(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register int line = w->term.array_cur.lin;
     register int col = w->term.array_cur.col;
     register int no_era = (int)cb_reg_data(com_block, 'a');
     register int line_len = w->term.columns;
     
     Char ch;
     ch.value = 0;
     ch.attributes = 0;
     
     check_arg_type(com_block, 'a', CB_INT_TYPE, "erase_chars", 9);
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     flip_cursor_off(w);
     
     erase_array_chars(w, line, col, no_era, line_len);
     erase_screen_chars(w, line, col, no_era, line_len);
     
     validate_cursor_gc(w, ch);
     
     return(0);
}

Export int
erase_line_left(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register int line = w->term.array_cur.lin;
     register int col = w->term.array_cur.col;
     register int line_len = w->term.columns;
     
     Char ch;
     ch.value = 0;
     ch.attributes = 0;
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     flip_cursor_off(w);
     
     erase_array_chars(w, line, 0, col + 1, line_len);
     erase_screen_chars(w, line, 0, col + 1, line_len);
     
     validate_cursor_gc(w, ch);
     
     return(0);
}

Export int
erase_lines(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register int line = w->term.array_cur.lin;
     register int no_era = (int)cb_reg_data(com_block, 'a');

     Char ch;
     ch.value = 0;
     ch.attributes = 0;
     
     check_arg_type(com_block, 'a', CB_INT_TYPE, "erase_lines", 9);

     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     if ((line + no_era) > w->term.lines)
	  no_era = w->term.lines - line;
     
     flip_cursor_off(w);
     
     erase_array_lines(w, line, no_era);
     erase_screen_lines(w, line, no_era);
     
     validate_cursor_gc(w, ch);
     
     return(0);
}

Export int
erase_from_toscr(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register int line = w->term.array_cur.lin;
     register int col = w->term.array_cur.col;
     register int line_len = w->term.columns;
     
     Char ch;
     ch.value = 0;
     ch.attributes = 0;
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     flip_cursor_off(w);
     
     if (line > 0) {
	  erase_array_lines(w, 0, line);
	  erase_screen_lines(w, 0, line);
     }
     
     erase_array_chars(w, line, 0, col + 1, line_len);
     erase_screen_chars(w, line, 0, col + 1, line_len);
     
     validate_cursor_gc(w, ch);
     
     return(0);
}

/*
 * Insert Lines at the cursor. Lines within the scroll region at and
 * below the cursor move down. The cursor is reset to the first column.
 *
 * Only lines whithin the scrolling region are moved down.
 *
 * A retval of 1 means the current line is outside the scrolling area,
 * A retval of 2 means the number of lines to insert had to be clipped
 * to the scrolling area.
 */
Export int
insert_lines(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register int line = w->term.array_cur.lin;
     register int no = (int)cb_reg_data(com_block, 'a');
     register int st = w->term.scroll_top;
     register int sb = w->term.scroll_bottom;
     
     int retval = 0;

     check_arg_type(com_block, 'a', CB_INT_TYPE, "insert_lines", 9);
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     if ((line > sb) || (line < st))
	  /* outside the scrolling area */
	  return(1);
     
     /* within the scrolling area */
     /* clip to the bottom of the scrolling area */
     if ((line + no) > sb) {
	  no = sb - line;
	  retval = 2;
     }
     
     flip_cursor_off(w);
	  
     /* do the actual scrolling */
     scroll_selection(w, line, sb - line + 1, no);
     scroll_screen(w, line, sb - line + 1, no);
     scroll_array(w, line, sb - line + 1, no, False);
     w->term.screen_cur.x = w->term.array_cur.col = 0;
     
     set_cursor_pos(w, line, 0);
     
     return(retval);
}

Export int
clear_screen(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     flip_cursor_off(w);
     
     erase_array_lines(w, 0, w->term.lines);
     erase_screen_lines(w, 0, w->term.lines);
     
     set_cursor_pos(w, 0, 0);
     
     return(0);
}

Export int
set_scroll_region(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register int nt = (int)cb_reg_data(com_block, 'a');
     register int nb = (int)cb_reg_data(com_block, 'b');
     register int tmp;

     check_arg_type(com_block, 'a', CB_INT_TYPE, "set_scroll_region", 9);
     check_arg_type(com_block, 'b', CB_INT_TYPE, "set_scroll_region", 9);

     if (nt > nb) {
	  tmp = nt;
	  nt = nb;
	  nb = tmp;
     }
     
     w->term.scroll_top = nt < 0 ? 0 : nt;
     w->term.scroll_bottom = nt > w->term.lines ? w->term.lines : nb;

     return(0);
}

Export int
ring_bell(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     XBell(XtDisplay(w), w->term.bell_volume);
     return(0);
}

Export int
hor_tab(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register int col = w->term.array_cur.col;
     register TabList list = w->term.tabs + ++col;
     register int no_cols = w->term.columns;
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     if (col >= no_cols)
	  return(4);
     
     while ((col < no_cols) && !*list) {
	  list++;
	  col++;
     }
	    
     return(move_cursor(w, w->term.array_cur.lin,col));
}

Export int
set_tab_cur_col(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     *(w->term.tabs + w->term.array_cur.col) = True;
     return(0);
}

Export int
set_tab_col(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register int col = (int)cb_reg_data(com_block, 'x');
     
     check_arg_type(com_block, 'x', CB_INT_TYPE, "set_tab_col", 9);
     
     if ((col < 0) || (col >= w->term.columns))
	  return(1);
     
     *(w->term.tabs + col) = True;
     return(0);
}

Export int
set_tabs_eq_width(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     check_arg_type(com_block, 'a', CB_INT_TYPE, "set_tabs_eq_width", 9);
     
     init_fixed_tabs(w->term.tabs, w->term.columns,
		     cb_reg_data(com_block, 'a'));
     return(0);
}


Export int
clear_tab_cur_col(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     *(w->term.tabs + w->term.array_cur.col) = False;
     return(0);
}

Export int
clear_tab_col(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register int col = (int)cb_reg_data(com_block, 'x');
     
     check_arg_type(com_block, 'x', CB_INT_TYPE, "clear_tab_col", 9);
     
     if ((col < 0) || (col >= w->term.columns))
	  return(1);
     
     *(w->term.tabs + col) = False;
     return(0);
}

Export int
clear_all_tabs(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     clear_tab_list(w->term.tabs, w->term.columns);
     return(0);
}

Export int
set_attribute(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     check_arg_type(com_block, 'a', CB_INT_TYPE, "set_attribute", 9);
     
     (void)validate_gc(w, (short)cb_reg_data(com_block, 'a')
		       | w->term.act_attribs, w->term.act_color);

     /*
      * if we've turned blinking on and the blink timer isn't running yet,
      * start it.
      */
     if (((w->term.act_attribs & ATT_BLINK) != 0)
	 && (w->term.blink_id == 0)) {
	  w->term.blink_id = XtAddTimeOut(w->term.tblink_interval,
					  blink_text, w);
     }
     
     return(0);
}

Export int
clear_attribute(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     check_arg_type(com_block, 'b', CB_INT_TYPE, "clear_attribute", 9);
     
     (void)validate_gc(w, ~(short)cb_reg_data(com_block, 'b')
		 & w->term.act_attribs, w->term.act_color);
     return(0);
}

/*
 * Override the current keyboard translations according to the resource
 * specified in register A.
 */
Export int
override_translations(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     String s = (String)cb_reg_data(com_block, 'a' );
     char name[255], class[255];
     String translations;

     check_arg_type(com_block, 'a', CB_STR_TYPE, "override_translations", 9);
     
     if (!s) {
	  warn("Resource name is NULL in override_translations");
	  return(2);
     }
     
     /* Set up the resource name */
     sprintf(name, "auxTrans-%s", s);
     sprintf(class, "AuxTrans-%s", s);
     
     translations = get_sub_resource_string(w, w->term.term_type, name, class);
     
     if (translations != NULL) {
	  XtOverrideTranslations((Widget)w, XtParseTranslationTable(translations));
	  return(0);
     }
     else
	  return(1);
}

/*
 * Change the current flut according to the resource specified in register A
 * with offset in register B
 */
Export int
change_flut(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     String s = (String)cb_reg_data(com_block, 'a');
     char name[255];
     char class[255];
     String flut_desc;
     
     check_arg_type(com_block, 'a', CB_STR_TYPE, "change_flut", 9);
     check_arg_type(com_block, 'b', CB_INT_TYPE, "change_flut", 9);
     
     if (!s) {
	  warn("FLUT name is NULL in change_flut.");
	  return(2);
     }
     
     /* Set up the resource name */
     sprintf(name, "flut-%s", s);
     sprintf(class, "Flut-%s", s);
     
     flut_desc = get_sub_resource_string(w, w->term.term_type, name, class);
     
     if (flut_desc == NULL)
	  return(1);

     parse_and_change_flut(flut_desc, w->term.flut,
			   (int)cb_reg_data(com_block, 'b'));
     
     return(0);
}

/*
 * Returns the size of the current char_array in register X and Y
 */
Export int
canvas_size(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     cb_reg_type(com_block, 'x') = CB_INT_TYPE;
     cb_reg_data(com_block, 'x') = (Generic) w->term.columns;
     
     cb_reg_type(com_block, 'y') = CB_INT_TYPE;
     cb_reg_data(com_block, 'y') = (Generic) w->term.lines;

     return(0);
}
   
/*
 * Returns the current cursor position in register X and Y
 */
Export int
canvas_cursor_pos(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     cb_reg_type(com_block, 'x') = CB_INT_TYPE;
     cb_reg_data(com_block, 'x') = (Generic) w->term.array_cur.col;
     
     cb_reg_type(com_block, 'y') = CB_INT_TYPE;
     cb_reg_data(com_block, 'y') = (Generic) w->term.array_cur.lin;

     return(0);
}

/*
 * Returns the current attributes in register A
 */
Export int
canvas_attribs(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     cb_reg_type(com_block, 'a') = CB_INT_TYPE;
     cb_reg_data(com_block, 'a') = (Generic) w->term.act_attribs;

     return(0);
}

/*
 * Returns the start and end lines of the current scrolling region
 * in registers A and B
 */
Export int
canvas_scroll_region(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     cb_reg_type(com_block, 'a') = CB_INT_TYPE;
     cb_reg_data(com_block, 'a') = (Generic) w->term.scroll_top;
     
     cb_reg_type(com_block, 'b') = CB_INT_TYPE;
     cb_reg_data(com_block, 'b') = (Generic) w->term.scroll_bottom;

     return(0);
}

/*
 * Returns the current wrap around mode in register A
 */
Export int
canvas_wrap_mode(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     cb_reg_type(com_block, 'a') = CB_INT_TYPE;
     cb_reg_data(com_block, 'a') = (Generic) w->term.wrap_around;

     return(0);
}

/*
 * Returns in register A whether the screen is reverse video
 */
Export int
canvas_reverse_mode(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     cb_reg_type(com_block, 'a') = CB_INT_TYPE;
     cb_reg_data(com_block, 'a') = (Generic) w->term.reverse_mode;

     return(0);
}

/*
 * Returns in register A wheter the cursor is switched on or off
 */
Export int
canvas_cursor_on(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     cb_reg_type(com_block, 'a') = CB_INT_TYPE;
     cb_reg_data(com_block, 'a') = (Generic)
	  (w->term.cursor_visible || (w->term.timeout_id != 0));

     return(0);
}

/*
 * Returns in register A wheter the cursor is blinking
 */
Export int
canvas_cursor_blinking(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     cb_reg_type(com_block, 'a') = CB_INT_TYPE;
     cb_reg_data(com_block, 'a') = (Generic) w->term.cursor_blinking;

     return(0);
}

/*
 * Returns the size of the cursor in register X and Y
 */
Export int
canvas_cursor_size(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     cb_reg_type(com_block, 'x') = CB_INT_TYPE;
     cb_reg_data(com_block, 'x') = (Generic) w->term.cursor_width;
     
     cb_reg_type(com_block, 'y') = CB_INT_TYPE;
     cb_reg_data(com_block, 'y') = (Generic) w->term.cursor_height;

     return(0);
}

/*
 * Clear the screen and redraw everything
 */
Export int
redraw_screen(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     flip_cursor_off(w);
     
     erase_screen_lines(w, 0, w->term.lines);
     redraw_rect(w, w->term.char_array,
		 0, 0, w->term.lines - 1, w->term.columns - 1);
     
     return(0);
}

/*
 * Change the fonts in use and redraw the window
 */
Export int
change_fonts(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register XFontStruct *fn, *fb;
     Arg args[5];
     int n, col;
     char *str;
     int old_width, old_height;
     
     check_arg_type(com_block, 'a', CB_STR_TYPE, "change_fonts", 9);
     check_arg_type(com_block, 'b', CB_STR_TYPE, "change_fonts", 9);

     /* If the caller supplies no font name, fall back to the default */
     if (*(str = cb_reg_data(com_block, 'a')) == '\0')
	  str = w->term.normal_font_n;
     if ((fn = XLoadQueryFont(XtDisplay(w), str)) == NULL) {
	  warn("change_fonts: Couldn't load normal font \"%s\"",
	  (char *)cb_reg_data(com_block, 'a'));
	  return(1);
     }
     
     /* If the caller supplies no font name, fall back to the default */
     if (*(str = cb_reg_data(com_block, 'b')) == '\0')
	  str = w->term.bold_font_n;
     if ((fb = XLoadQueryFont(XtDisplay(w), str)) == NULL) {
	 warn("change_fonts: Couldn't load bold font \"%s\", using normal one",
	  (char *)cb_reg_data(com_block, 'b'));
	  fb = fn;
     }

     flip_cursor_off(w);
     
     /* Save the old cell_size */
     old_width = w->term.cell_width;
     old_height = w->term.cell_height;

     /* now do the actual changing */
     /* first free the fonts */
     
     XFreeFont(XtDisplay(w), w->term.normal_font);
     /* Free the bold font only if it's not the same as the normal one */
     if (w->term.bold_font != w->term.normal_font)
	  XFreeFont(XtDisplay(w), w->term.bold_font);
     
     
     /* assign the new ones */
     w->term.normal_font = fn;
     w->term.bold_font = fb;

     /* set the character cell parameters */
     if (fn->max_bounds.width != fn->min_bounds.width)
	  warn("change_fonts: Normal font is not of fixed width!");
     
     w->term.cell_width = fn->max_bounds.width;
     w->term.cell_height = fn->ascent + fn->descent;
     w->term.font_descent = fn->descent;
     
     if (fb->max_bounds.width != fb->min_bounds.width)
	  warn("change_fonts: Bold font is not of fixed width!");

     if (fn->max_bounds.width != fb->max_bounds.width)
	  warn("change_fonts: Normal and bold fonts are of different width (%d vs. %d)",
	       fn->max_bounds.width, fb->max_bounds.width);
     if ((fn->ascent + fn->descent) != (fb->ascent + fb->descent))
	  warn("change_fonts: Normal and bold fonts are of different height (%d vs. %d)",
	       fn->ascent + fn->descent, fb->ascent + fb->descent);
     if (fn->descent != fb->descent)
	  warn("change_fonts: Normal and bold fonts have different descent (%d vs. %d)",
	       fn->descent, fb->descent);


     /* change the gcs */
     XSetFont(XtDisplay(w), w->term.norm_gc, fn->fid);
     if ((w->term.old_attribs & ATT_BOLD) != 0)
	  XSetFont(XtDisplay(w), w->term.spec_gc, fb->fid);
     else
	  XSetFont(XtDisplay(w), w->term.spec_gc, fn->fid);
     
     /* force the cursor fonts to be changed */
     w->term.curs_char.attributes ^= ATT_BOLD;
     
     col = w->term.array_cur.col;
     if (col == w->term.columns)
	  col--;
     
     validate_cursor_gc(w, w->term.char_array[w->term.array_cur.lin][col]);
     validate_cursor_size(w);
	  
     w->term.screen_cur.y = (w->term.array_cur.lin + 1) * w->term.cell_height;
     w->term.screen_cur.x = w->term.array_cur.col * w->term.cell_width;
     
     /*
      * If the cell size changes, we have to change the
      * resize increments accordingly
      */
     if ((old_width != w->term.cell_width) ||
	 (old_height != w->term.cell_height)) {
     
	  n = 0;
	  
	  XtSetArg(args[n], XtNheightInc,
		   w->term.cell_height);
	  n++;
	  XtSetArg(args[n], XtNwidthInc,
		   w->term.cell_width);
	  n++;
	  
	  XtSetValues(XtParent(w), args, n);
	  
	  /* change the canvas size accordingly */
	  XtMakeResizeRequest((Widget)w, (Dimension)(w->term.cell_width *
					     w->term.columns),
			      (Dimension)(w->term.cell_height * w->term.lines),
			      NULL, NULL);
     }
     /* otherwise redraw the canvas in the new font */
     else {
	  erase_screen_lines(w, 0, w->term.lines);
	  redraw_rect(w, w->term.char_array,
		      0, 0, w->term.lines - 1, w->term.columns - 1);
	  
     }
     
	  return(0);
}

/*
 * Scroll the screen the percentage specified in register a
 * The percentage is given as int from 0 to 999.
 * Register b holds a flag that tells whether the scrollbar should be 
 * adjusted or not.
 */
Export int
scroll_screen_relative(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     register float f;
     register int no;
     
     check_arg_type(com_block, 'a', CB_INT_TYPE, "scroll_screen_relative", 9);
     check_arg_type(com_block, 'b', CB_INT_TYPE, "scroll_screen_relative", 9);

     f = (float)((int)cb_reg_data(com_block, 'a')) / 1000.0;
     
     /* We always move at least one line */
     no = (int)((float)w->term.lines * f);
     if (no == 0)
	  if (f < 0.0)
	       no--;
	  else
	       no++;
	       
     scroll_save_rel(w, no, (int)cb_reg_data(com_block, 'b'));

     return(0);
}

/*
 * Scroll the screen to the percentage specified in register a
 * The percentage is given as int from 0 to 999.
 * Register b holds a flag that tells whether the scrollbar should be 
 * adjusted or not.
 */
Export int
scroll_screen_absolute(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     float f;
     
     check_arg_type(com_block, 'a', CB_INT_TYPE, "scroll_screen_absolute", 9);
     check_arg_type(com_block, 'b', CB_INT_TYPE, "scroll_screen_absolute", 9);

     f = (float)((int)cb_reg_data(com_block, 'a')) / 1000.0;
     
     scroll_save_abs(w, w->term.save_size -
		     (int)((float)(w->term.lines + w->term.save_size) * f),
		     (int)cb_reg_data(com_block, 'b'));

     return(0);
}
     
/*
 * Switch cursor off
 */
Export int
cursor_off(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     switch_cursor_off(w);
     return(0);
}

/*
 * Switch cursor on
 */
Export int
cursor_on(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     switch_cursor_on(w);
     return(0);
}

/*
 * set screen size
 */
Export int
set_screen_size(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     check_arg_type(com_block, 'x', CB_INT_TYPE, "set_screen_size", 9);
     check_arg_type(com_block, 'y', CB_INT_TYPE, "set_screen_size", 9);
     
     if (w->term.scroll_pos != 0) scroll_save_abs(w, 0, True);
     
     XtMakeResizeRequest((Widget)w, (Dimension)(w->term.cell_width *
					(int)cb_reg_data(com_block, 'x')),
			 (Dimension)(w->term.cell_height *
				     (int)cb_reg_data(com_block, 'y')),
			 NULL, NULL);
     return(0);
}

/*
 * wrap around at end of the line
 */
Export int
wrap_around(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     w->term.wrap_around = True;
     return(0);
}

/*
 * don't wrap around at line end (keep writing last character)
 */
Export int
dont_wrap(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     w->term.wrap_around = False;
     return(0);
}

/*
 * Direct cursor positioning is relative to scrolling region
 */
Export int
cursor_pos_rel_to_scr_reg(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     w->term.rel_cursor_pos = True;
     return(0);
}

/*
 * Direct cursor positioning is absolute
 */
Export int
cursor_pos_absolute(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     w->term.rel_cursor_pos = False;
     return(0);
}

/*
 * Routine to do the actual switching between fg and bg colors,
 * it's the same for switching to reverse as for switching back
 */
Local void
flip_reverse_video(w, flag)
register TermCanvasWidget w;
Boolean flag;
{
     Arg args[1];
     int n = 0;
     XGCValues val;
     register int col;
     unsigned short color, attribs;
     Pixel fg, bg;
     
     flip_cursor_off(w);
     
     fg = w->term.cit[0].bg.pix;
     bg = w->term.cit[0].fg.pix;
     w->term.cit[0].fg.pix = fg;
     w->term.cit[0].bg.pix = bg;
     
     /* modify the normal text GC */
     val.foreground = fg;
     val.background = bg;
     
     XChangeGC(XtDisplay(w), w->term.norm_gc,
	       GCForeground | GCBackground, &val);

     /* modify the special text GC by forcing validate_gc */
     attribs = w->term.act_attribs;
     color = w->term.act_color;

     w->term.act_attribs ^= ATT_REVERSE;
     w->term.old_color ^=0xff;

     (void)validate_gc(w, attribs, color);
     
     /* the clearing GC */
     val.foreground = bg;
     val.background = fg;
     
     XChangeGC(XtDisplay(w), w->term.clear_gc,
	       GCForeground | GCBackground, &val);
     
     w->term.reverse_mode = flag;

     /* force the cursor validation routine to change the colors */
     w->term.curs_char.attributes ^= ATT_REVERSE;
     
     col = w->term.array_cur.col;
     if (col == w->term.columns)
	  col--;
     validate_cursor_gc(w, w->term.char_array[w->term.array_cur.lin][col]);
     
     /*
      * Change the mouse pointer color
      */
     if ((w->term.color_cells > 2) && !w->term.pt_fg_eq_wd_fg)
	  XRecolorCursor(XtDisplay(w), w->term.pointer,
			 &(w->term.pointer_fg), &(w->term.window_fg));
     else
	  XRecolorCursor(XtDisplay(w), w->term.pointer,
			 &(w->term.window_bg), &(w->term.window_fg));
     
     /*
      * Set the widget's background. This will result in a redraw
      * by the toolkit.
      */
     n = 0;
     XtSetArg(args[n], XtNbackground, bg);
     n++;
     XtSetValues((Widget)w, args, n);
}
     
/*
 * use reverse video (switch back- and foreround colors of color 0)
 */
Export int
reverse_video(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     /* already reverse ?? */
     if (w->term.reverse_mode)
	  return(1);

     flip_reverse_video(w, True);
     
     return(0);
}

/*
 * use normal video
 */
Export int
normal_video(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     /* already normal ?? */
     if (!w->term.reverse_mode)
	  return(1);
     
     flip_reverse_video(w, False);
     
     return(0);
}

/*
 * save the current flut
 */
Export int
save_flut(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     bcopy(w->term.flut, w->term.save_flut, 256 * sizeof(unsigned char));
     return(0);
}

/*
 * restore the saved flut
 */
Export int
restore_flut(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     bcopy(w->term.save_flut, w->term.flut, 256 * sizeof(unsigned char));
     return(0);
}

/*
 * sets the cursor blink from register A
 */
Export int
set_cursor_blink(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     Import void blink_cursor();
     
     check_arg_type(com_block, 'a', CB_INT_TYPE, "set_cursor_blink", 9);
     
     w->term.cursor_blinking = (Boolean) cb_reg_data(com_block, 'a');

     /* If it wasn't blinking switch it on */
     if (w->term.cursor_blinking && w->term.cursor_visible &&
	 ((w->term.timeout_id == 0) &&
	  (w->term.focused || w->term.blink_wo_focus)))
	  w->term.timeout_id = XtAddTimeOut(w->term.blink_interval,
					    blink_cursor, w);
     /* If it was blinking switch it off */
     else if (!w->term.cursor_blinking && ((w->term.timeout_id != 0))) {
	  XtRemoveTimeOut(w->term.timeout_id);
	  w->term.timeout_id = 0;
	  if (!w->term.cursor_visible) {
	       flip_cursor_on(w);
	       w->term.cursor_visible = True;
	  }
     }
	  
     return(0);
}

/*
 * sets the cursor size from register X and Y
 */
Export int
set_cursor_size(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     check_arg_type(com_block, 'y', CB_INT_TYPE, "set_cursor_size", 9);
     check_arg_type(com_block, 'x', CB_INT_TYPE, "set_cursor_size", 9);
     
     flip_cursor_off(w);
     
     w->term.cursor_width = (int)cb_reg_data(com_block, 'x');
     w->term.cursor_height = (int)cb_reg_data(com_block, 'y');
     
     validate_cursor_size(w);
     
     return(0);
}

/*
 * Changes the foreground color to draw with from register A
 */
Export int
change_fg_color(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     unsigned char color;
     
     check_arg_type(com_block, 'a', CB_INT_TYPE, "change_fg_color", 9);

     color = (unsigned char)cb_reg_data(com_block, 'a');

     /*
      * Check if the color is within 0 - 16
      */
     if ((color < 0) || (color > 15)) {
	  warn("change_fg_color: color %d out of bounds", (int)color);
	  return(1);
     }
     
     /*
      * Check if the color was correctly loaded in the cit.
      * Color 0 will always be set by  the initialization code,
      * but bitch nevertheless if somebody tries to set this color
      * and it hasn't been loaded, because monochrome Emulators that
      * don't initialize the cit (and don't have to) shouldn't change
      * colors then
      */
     if (!w->term.cit[color].fg.set) {
	  warn("change_fg_color: Trying to switch to uninitialized foreground color %d", color);
	  return(2);
     }
     
     /*
      * Now finally set the color
      */
     (void)validate_gc(w, w->term.act_attribs,
		       SET_COLOR_FG(w->term.act_color, color));
     
     return(0);
}

/*
 * Changes the background color to draw with from register B
 */
Export int
change_bg_color(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     unsigned char color;
     
     check_arg_type(com_block, 'b', CB_INT_TYPE, "change_bg_color", 9);

     color = (unsigned char)cb_reg_data(com_block, 'b');

     /*
      * Check if the color is within 0 - 16
      */
     if ((color < 0) || (color > 15)) {
	  warn("change_bg_color: color %d out of bounds", (int)color);
	  return(1);
     }
     
     /*
      * Check if the color was correctly loaded in the cit.
      * Color 0 will always be set by  the initialization code,
      * but bitch nevertheless if somebody tries to set this color
      * and it hasn't been loaded, because monochrome Emulators that
      * don't initialize the cit (and don't have to) shouldn't change
      * colors then
      */
     if (!w->term.cit[color].bg.set) {
	  warn("change_bg_color: Trying to switch to uninitialized background color %d", color);
	  return(2);
     }
     
     /*
      * Now finally set the color
      */
     (void)validate_gc(w, w->term.act_attribs,
		       SET_COLOR_BG(w->term.act_color, color));
     
     return(0);
}

/*
 * Sets a cit cell. The cell number is passed in register A,
 * the names of foreground and background colors in registers B and C.
 * set_cit_cell allocates the named colors from the default color
 * map. If it doesn't succeed to get the exact values it prints a
 * warning.
 *
 * Changing the background of cell 0 will result in the window background
 * being changed, which causes the toolkit to redraw the window.
 */
Export int
set_cit_cell(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     Arg args[5];
     int n = 0;
     
     unsigned char color;
     XColor screen_color, exact_color;
     
     check_arg_type(com_block, 'a', CB_INT_TYPE, "set_cit_cell", 9);
     check_arg_type(com_block, 'b', CB_STR_TYPE, "set_cit_cell", 9);
     check_arg_type(com_block, 'c', CB_STR_TYPE, "set_cit_cell", 9);

     color = (unsigned char)cb_reg_data(com_block, 'a');
     
     /*
      * Check if the color is within 0 - 16
      */
     if ((color < 0) || (color > 15)) {
	  warn("set_cit_cell: color %d out of bounds", (int)color);
	  return(1);
     }

     /*
      * If the cell has been set yet, free the colors
      */
     if (w->term.cit[color].fg.set)
	  XFreeColors(XtDisplay(w), w->core.colormap,
		      &(w->term.cit[color].fg.pix), 1, 0L);
     
     if (w->term.cit[color].bg.set)
	  XFreeColors(XtDisplay(w), w->core.colormap,
		      &(w->term.cit[color].bg.pix), 1, 0L);
     
     /*
      * Now look up the colors
      */
     (void) XAllocNamedColor(XtDisplay(w), w->core.colormap,
			     (char *)cb_reg_data(com_block, 'b'),
			     &screen_color, &exact_color);
     
     /*
      * Mask out the lower byte of the color for the check,
      * I haven't encountered a server yet that uses more
      * than 256 levels for r,g,b. Some servers however
      * seem to produce garbage in the low byte of the intensity
      */
     if ((screen_color.red & 0xff00 != exact_color.red & 0xff0) ||
	 (screen_color.green & 0xff00 != exact_color.green & 0xff0) ||
	 (screen_color.blue & 0xff00 != exact_color.blue & 0xff0)) {
	  warn("set_cit_cell:\ncouldn't allocate exact color value for \"%s\"\nexpected (%x, %x, %x), got (%x, %x, %x)",
	       (char *)cb_reg_data(com_block, 'b'),
	       exact_color.red, exact_color.green, exact_color.blue,
	       screen_color.red, screen_color.green, screen_color.blue);
     }

     w->term.cit[color].fg.pix = screen_color.pixel;
     
     (void) XAllocNamedColor(XtDisplay(w), w->core.colormap,
			     (char *)cb_reg_data(com_block, 'c'),
			     &screen_color, &exact_color);
     
     if ((screen_color.red & 0xff00 != exact_color.red & 0xff0) ||
	 (screen_color.green & 0xff00 != exact_color.green & 0xff0) ||
	 (screen_color.blue & 0xff00 != exact_color.blue & 0xff0)) {
	  warn("set_cit_cell:\ncouldn't allocate exact color value for \"%s\"\nexpected (%x, %x, %x), got (%x, %x, %x)",
	       (char *)cb_reg_data(com_block, 'b'),
	       exact_color.red, exact_color.green, exact_color.blue,
	       screen_color.red, screen_color.green, screen_color.blue);
     }

     w->term.cit[color].bg.pix = screen_color.pixel;
     
     /* Have we just changed color 0 ? */
     if (color == 0) {
	  /*
	   * Set the widget's background. This will result in a redraw
	   * by the toolkit.
	   */
	  n = 0;
	  if (w->term.reverse_mode) {
	       XtSetArg(args[n], XtNbackground, w->term.cit[0].fg.pix);
	  }
	  else {
	       XtSetArg(args[n], XtNbackground, w->term.cit[0].bg.pix);
	  }
	  n++;
	  XtSetValues((Widget)w, args, n);
     }

     /* The color is set now */
     w->term.cit[color].fg.set = True;
     w->term.cit[color].bg.set = True;
     
     return(0);
}

/*
 * Returns the number of display cells (colors in the colormap) in register A
 */
Export int
canvas_display_cells(w, com_block)
register TermCanvasWidget w;
ComBlockPtr com_block;
{
     cb_reg_type(com_block, 'a') = CB_INT_TYPE;
     cb_reg_data(com_block, 'a') = (Generic) w->term.color_cells;

     return(0);
}
