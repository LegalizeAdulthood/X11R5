#ifndef lint
 static char *rcsid = "$Header: /usr3/emu/canvas/RCS/screen.c,v 1.11 90/10/12 12:40:53 me Exp $";
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
 * General screen manipulation routines
 *
 * Author: Michael Elbel
 * Date: March 28th, 1990.
 * Description: Here we have most of the routines that manipulate the canvas
 * 		window directly like scrolling and so on.
 *
 * Revision History:
 *
 * $Log:	screen.c,v $
 * Revision 1.11  90/10/12  12:40:53  me
 * checkmark
 * 
 * Revision 1.10  90/10/05  17:34:09  me
 * Changed the cursor drawing to not xor over.
 * 
 * Revision 1.9  90/09/17  18:26:19  me
 * check
 * 
 * Revision 1.8  90/08/31  19:51:54  me
 * Checkpoint vor dem Urlaub
 * 
 * Revision 1.7  90/08/30  13:16:18  me
 * Test checkpoint
 * 
 * Revision 1.6  90/07/26  02:36:56  jkh
 * Added new copyright.
 * 
 * Revision 1.4  90/06/07  15:35:52  me
 * FLUT and Translation stuff added
 * 
 * Revision 1.3  90/05/08  14:33:05  me
 * Jordan's infamous changes
 * 
 * Revision 1.2  90/05/04  13:39:51  me
 * Fixed interpolation of delete and insert line requests.
 * 
 * Revision 1.1  90/04/12  14:29:34  me
 * Initial revision
 * 
 *
 */

#include "canvas.h"

/*
 * Routine to turn the cursor off if necessary.
 */

Export void
flip_cursor_off(w)
register TermCanvasWidget w;
{
     char c;
     
     /* Should we really flip? */
     if (!w->term.cursor_on || !w->term.cursor_visible ||
	 (w->term.screen_cur.y < 0) || (w->term.screen_cur.x < 0))
	  return;
     
     if (w->term.curs_char.value == 0)
	  XFillRectangle(XtDisplay(w), XtWindow(w), w->term.clear_gc,
			 w->term.screen_cur.x,
			 w->term.screen_cur.y - w->term.cursor_height,
			 w->term.cursor_width, w->term.cursor_height);
     else {
	  c = w->term.curs_char.value;
	  draw_string(w, w->term.cursor_rem_gc, w->term.curs_char.attributes,
		      w->term.screen_cur.x,
		      w->term.screen_cur.y - w->term.font_descent, &c, 1);
     }

     w->term.cursor_on = False;
}

/*
 * Routine to turn the cursor on if desired. If the cursor is off but there
 * is a timer set to turn it on, turn it on immediately and reset the timer.
 */

Export void
flip_cursor_on(w)
register TermCanvasWidget w;
{
     char c;
     
     /* Should we really flip? */
     if (w->term.cursor_on ||
	 (!w->term.cursor_visible && (w->term.timeout_id == 0)) ||
	 (w->term.screen_cur.y < 0) || (w->term.screen_cur.x < 0))
	  return;
     
     if (w->term.outline_cursor)
	  XDrawRectangle(XtDisplay(w), XtWindow(w), w->term.cursor_graph_gc,
			 w->term.screen_cur.x,
			 w->term.screen_cur.y - w->term.cursor_height,
			 w->term.cursor_width - 1, w->term.cursor_height -1);
     else
	  if (w->term.curs_char.value == 0)
	       XFillRectangle(XtDisplay(w), XtWindow(w), w->term.cursor_graph_gc,
			      w->term.screen_cur.x,
			      w->term.screen_cur.y - w->term.cursor_height,
			      w->term.cursor_width, w->term.cursor_height);
	  else {
	       c = w->term.curs_char.value;
	       draw_string(w, w->term.cursor_text_gc,
			   w->term.curs_char.attributes,
			   w->term.screen_cur.x,
			   w->term.screen_cur.y - w->term.font_descent, &c, 1);
	  }
     
     if (!w->term.cursor_visible && (w->term.timeout_id != 0)) {
	  w->term.cursor_visible = True;
	  XtRemoveTimeOut(w->term.timeout_id);
	  w->term.timeout_id = XtAddTimeOut(w->term.blink_interval,
					    blink_cursor, w);
     }
     
     w->term.cursor_on = True;
}

/*
 * To switch the cursor off totally. Checks whether there is a timeout
 * set and turns that off if necessary
 */
Export void
switch_cursor_off(w)
register TermCanvasWidget w;
{
     if (w->term.timeout_id != 0) {
	      XtRemoveTimeOut(w->term.timeout_id);
	      w->term.timeout_id = 0;
     }

     if (w->term.cursor_visible) {
	  flip_cursor_off(w);
	  w->term.cursor_visible = False;
     }
}

/*
 * To switch the cursor on again. Checks whether the cursor should blink,
 * then activates the timeout again
 */
Export void
switch_cursor_on(w)
register TermCanvasWidget w;
{
     if (w->term.cursor_visible || (w->term.timeout_id != 0))
	  return;		/* cursor was already on */

     flip_cursor_on(w);
     w->term.cursor_visible = True;

     if (w->term.cursor_blinking &&
	 (w->term.focused || w->term.blink_wo_focus))
	  w->term.timeout_id = XtAddTimeOut(w->term.blink_interval,
					    blink_cursor, w);
}    

/*
 * Scroll the screen. The lines that were scrolled away get erased.
 */
Export void
scroll_screen(w, start_lin, lines, scroll_lines)
register TermCanvasWidget w;
register int start_lin, lines;
int scroll_lines;
{
     register int fh = w->term.cell_height;
     register int fw = w->term.cell_width;

     if (scroll_lines >0) {
	  /* scroll up */
	  XCopyArea(XtDisplay(w), XtWindow(w), XtWindow(w), w->term.gc,
		    0, (start_lin + scroll_lines) * fh,
		    w->term.columns * fw, (lines - scroll_lines) * fh,
		    0, start_lin * fh);
	  XFillRectangle(XtDisplay(w), XtWindow(w), w->term.clear_gc,
			 0, (start_lin + lines - scroll_lines)* fh,
			 w->term.columns * fw, fh * scroll_lines);
     } else {
	  /* scroll down */
	  XCopyArea(XtDisplay(w), XtWindow(w), XtWindow(w), w->term.gc,
		    0, start_lin * fh,
		    w->term.columns * fw, (lines - scroll_lines) * fh,
		    0, (start_lin - scroll_lines) * fh);
	  XFillRectangle(XtDisplay(w), XtWindow(w), w->term.clear_gc,
			 0, start_lin * fh,
			 w->term.columns * fw, fh * (-scroll_lines));
     }
}

/*
 * Deletes characters in the given line starting with start_col by shifting
 * everything right of that region left and blanking the resulting space.
 */
Export void
delete_screen_chars(w, line, start_col, no_chars, line_len)
register TermCanvasWidget w;
int line, start_col, no_chars, line_len;
{
     register int fh = w->term.cell_height;
     register int fw = w->term.cell_width;
     
     /* clip to the right border of the line */
     if ((start_col + no_chars) > line_len)
	  no_chars = line_len - start_col;
     
     XCopyArea(XtDisplay(w), XtWindow(w), XtWindow(w), w->term.gc,
	       (start_col + no_chars) * fw, line * fh,
	       (line_len - start_col - no_chars) * fw, fh,
	       start_col * fw, line * fh);
     XFillRectangle(XtDisplay(w), XtWindow(w), w->term.clear_gc,
		    (line_len - no_chars) * fw , line * fh,
		    no_chars * fw, fh);
}

/*
 * Erases characters in the given line starting with start_col by 
 * blanking the space.
 */
Export void
erase_screen_chars(w, line, start_col, no_chars, line_len)
register TermCanvasWidget w;
int line, start_col, no_chars, line_len;
{
     register int fh = w->term.cell_height;
     register int fw = w->term.cell_width;
     
     /* clip to the right border of the line */
     if ((start_col + no_chars) > line_len)
	  no_chars = line_len - start_col;
     
     XFillRectangle(XtDisplay(w), XtWindow(w), w->term.clear_gc,
		    start_col * fw , line * fh,
		    no_chars * fw, fh);
}

/*
 * Clears a number of lines on the screen (e.g. for a clear screen operation)
 */
Export void
erase_screen_lines(w, start, no)
register TermCanvasWidget w;
register int start, no;
{

     XFillRectangle(XtDisplay(w), XtWindow(w), w->term.clear_gc,
		    0, start * w->term.cell_height,
		    w->term.columns * w->term.cell_width,
		    no * w->term.cell_height);
}

/*
 * Vaildate the default GCs in the widget private structure.
 *
 * Normal Text without attributes and color 0/0 is always done via
 * 'term->norm_gc' so this doesnt need to be changed while 'term->spec_gc'
 * is used for text 'with' attributes. The attributes for the last
 * spec_gc are stored in 'term->old_attribs' and 'term->old_color'.
 *
 * validate_gc sets 'term->gc' to 'term->norm_gc' or 'term->spec_gc' and
 * returns the gc.
 */
GC validate_gc(w, attrs, color)
register TermCanvasWidget w;
register unsigned char attrs, color;
{
     XGCValues val;
     register unsigned char old, oldc;
     
     w->term.act_attribs = attrs;
     w->term.act_color = color;
     
     /* Switching to normal ?? */
     if (((attrs & FONT_ATTS) == 0) && (color == 0)) {
	  w->term.gc = w->term.norm_gc;
	  return(w->term.gc);
     }

     oldc = w->term.old_color;
     old = w->term.old_attribs;
     
     /*
      * Just return the old gc if we're changing to the old attributes
      * and the color hasn't changed.
      */
     if ((old == attrs) && (color == oldc)) {
	  w->term.gc = w->term.spec_gc;
	  return(w->term.spec_gc);
     }
     
     if (attrs & ATT_BOLD) {
	  if(!(old & ATT_BOLD))
	       XSetFont(XtDisplay(w), w->term.spec_gc, w->term.bold_font->fid);
     }
     else {
	  if(old & ATT_BOLD)
	       XSetFont(XtDisplay(w), w->term.spec_gc,
			w->term.normal_font->fid);
     }
     if ((attrs & ATT_REVERSE) != (attrs & ATT_SELECTED)) {
	  if (((old & ATT_REVERSE) == (old & ATT_SELECTED))
	      || (color != oldc)) {
	       val.background = w->term.cit[GET_COLOR_FG(color)].fg.pix;
	       val.foreground = w->term.cit[GET_COLOR_BG(color)].bg.pix;
	       XChangeGC(XtDisplay(w), w->term.spec_gc,
			 GCForeground | GCBackground, &val);
	  }
     }
     else {
	  if (((old & ATT_REVERSE ) != (old & ATT_SELECTED))
	      || (color != oldc)) {
	       val.foreground = w->term.cit[GET_COLOR_FG(color)].fg.pix;
	       val.background = w->term.cit[GET_COLOR_BG(color)].bg.pix;
	       XChangeGC(XtDisplay(w), w->term.spec_gc,
			 GCForeground | GCBackground, &val);
	  }
     }
     w->term.old_attribs = attrs;
     w->term.old_color = color;
     w->term.gc = w->term.spec_gc;
     return(w->term.spec_gc);
}

/*
 * Vaildate the cursor GCs in the widget private structure.
 */
void validate_cursor_gc(w, cur_char)
register TermCanvasWidget w;
Char cur_char;
{
     XGCValues val;
     register short value = cur_char.value;
     register unsigned char attrs = value == 0 ? 0 : cur_char.attributes;
     register unsigned char color = value == 0 ? 0 : cur_char.color;
     register unsigned char old_att = w->term.curs_char.attributes;
     register unsigned char old_color = w->term.curs_char.color;

     /* If the attributes haven't changed, just come back */
     if ((old_att == attrs) && (old_color == color)) {
	  w->term.curs_char = cur_char;
	  return;
     }
     
     /* Normal display */
     if ((attrs & ATT_REVERSE) == (attrs & ATT_SELECTED)) {
	  if (((old_att & ATT_REVERSE) != (old_att & ATT_SELECTED))
	      || (color != old_color)) {
	       /* was the display inverted ? */
	       if (w->term.cursor_bg != 
		   w->term.cit[GET_COLOR_BG(color)].bg.pix) {
		    val.foreground = w->term.cursor_bg;
		    val.background = w->term.cursor_fg;
		    XChangeGC(XtDisplay(w), w->term.cursor_graph_gc,
			      GCForeground | GCBackground, &val);
		    val.foreground = w->term.cursor_fg;
		    val.background = w->term.cursor_bg;
		    XChangeGC(XtDisplay(w), w->term.cursor_text_gc,
			      GCForeground | GCBackground, &val);
	       }
	       else {
		    val.foreground = w->term.cursor_fg;
		    val.background = w->term.cursor_bg;
		    XChangeGC(XtDisplay(w), w->term.cursor_graph_gc,
			      GCForeground | GCBackground, &val);
		    val.foreground = w->term.cursor_bg;
		    val.background = w->term.cursor_fg;
		    XChangeGC(XtDisplay(w), w->term.cursor_text_gc,
			      GCForeground | GCBackground, &val);
	       }
	       val.foreground = w->term.cit[GET_COLOR_FG(color)].fg.pix;
	       val.background = w->term.cit[GET_COLOR_BG(color)].bg.pix;
	       XChangeGC(XtDisplay(w), w->term.cursor_rem_gc,
			 GCForeground | GCBackground, &val);
	  }
     }
     /* Inverted display */
     else {
	  /* was the display not inverted ? */
	  if (((old_att & ATT_REVERSE) == (old_att & ATT_SELECTED))
	      || (color != old_color)) {
	       if (w->term.cursor_bg == 
		   w->term.cit[GET_COLOR_FG(color)].fg.pix) {
		    val.foreground = w->term.cursor_fg;
		    val.background = w->term.cursor_bg;
		    XChangeGC(XtDisplay(w), w->term.cursor_graph_gc,
			      GCForeground | GCBackground, &val);
		    val.foreground = w->term.cursor_bg;
		    val.background = w->term.cursor_fg;
		    XChangeGC(XtDisplay(w), w->term.cursor_text_gc,
			      GCForeground | GCBackground, &val);
	       }
	       else {
		    val.foreground = w->term.cursor_bg;
		    val.background = w->term.cursor_fg;
		    XChangeGC(XtDisplay(w), w->term.cursor_graph_gc,
			      GCForeground | GCBackground, &val);
		    val.foreground = w->term.cursor_fg;
		    val.background = w->term.cursor_bg;
		    XChangeGC(XtDisplay(w), w->term.cursor_text_gc,
			      GCForeground | GCBackground, &val);
	       }
	       val.background = w->term.cit[GET_COLOR_FG(color)].fg.pix;
	       val.foreground = w->term.cit[GET_COLOR_BG(color)].bg.pix;
	       XChangeGC(XtDisplay(w), w->term.cursor_rem_gc,
			 GCForeground | GCBackground, &val);
	  }
     }
     
     if (attrs & ATT_BOLD) {
	  if(!(old_att & ATT_BOLD)) {
	       XSetFont(XtDisplay(w), w->term.cursor_text_gc,
			w->term.bold_font->fid);
	       XSetFont(XtDisplay(w), w->term.cursor_rem_gc,
			w->term.bold_font->fid);
	  }
     }
     else {
	  if(old_att & ATT_BOLD) {
	       XSetFont(XtDisplay(w), w->term.cursor_text_gc,
			w->term.normal_font->fid);
	       XSetFont(XtDisplay(w), w->term.cursor_rem_gc,
			w->term.normal_font->fid);
	  }
     }
     
     w->term.curs_char = cur_char;
}

/*
 * Draw a string with the given attribs, using the given gc (validate it first
 * if necessary) at draw_x, draw_y.
 */
Export void
draw_string(w, gc, attr, x, y, str, len)
TermCanvasWidget w;
GC gc;
unsigned char attr;
int x, y;
char *str;
int len;
{
     XDrawImageString(XtDisplay(w), XtWindow(w), gc, x, y, str, len);
     
     /* underlining ? */
     if (attr & ATT_UNDERL)
	  XDrawLine(XtDisplay(w), XtWindow(w), gc, x, y + w->term.ul_width,
		    x + w->term.cell_width * len, y + w->term.ul_width);
}

/*
 * Shifts the given line to the right starting at the given column.
 * The resulting space isn't cleared !!
 */

Export void
shift_screen_line(w, line, col, cnt, cols)
register TermCanvasWidget w;
register int line, col, cnt, cols;
{
     XCopyArea(XtDisplay(w), XtWindow(w), XtWindow(w), w->term.gc,
	       col * w->term.cell_width, line * w->term.cell_height,
	       (cols - col - cnt) * w->term.cell_width, w->term.cell_height,
	       (col + cnt) * w->term.cell_width, line * w->term.cell_height);
}

/*
 * Routine to blink Text on the screen. It gets called via timer.
 * Turns itself off if there is no blinking text on the screen and the current
 * attribute is not blinking.
 */

Export void
blink_text(w, id)
register TermCanvasWidget w;
XtIntervalId id;
{
     register int i, j, cnt, no;
     int draw_x, draw_y;
     char buffer[MAX_CHARS_IN_LINE];
     register CharPtr c;
     register unsigned char cur_attr = 0;
     register unsigned char cur_color = 0;
     unsigned char save_attr, save_color;
     int fh = w->term.cell_height;
     int fw = w->term.cell_width;
     GC gc = w->term.norm_gc;
     
     no = 0;

     /* save the current attributes and color */
     save_attr = w->term.act_attribs;
     save_color = w->term.act_color;
     
     for (i = 0; i < w->term.lines; i++) {
	  c = (w->term.char_array[i]);
	  draw_y = (i +1) * fh - w->term.font_descent;
	  
	  for (j = 0; j < w->term.columns;) {
	       cnt = 0;
	       
	       /* skip over characters that aren't blinking */
	       for (; (j < w->term.columns) && ((c->value == 0)
		    || ((c->attributes & ATT_BLINK) == 0)) ; c++)
		    j++;
	       draw_x = j * fw;
	       
	       /* collect characters */
	       for (; (j < w->term.columns) && (c->value != 0)
		       && (c->attributes == cur_attr)
		       && (c->color == cur_color); c++) {
		    buffer[cnt++] = c->value;
		    no++;
		    j++;
	       }
	       
	       /* draw the string if any */
	       if (cnt != 0) {
		    buffer[cnt] = 0;
		    /* draw the string or remove it ?? */
		    if (w->term.blink_text_on)
			 draw_string(w, gc, cur_attr, draw_x, draw_y,
				     buffer, cnt);
		    else
			 XFillRectangle(XtDisplay(w), XtWindow(w),
					w->term.clear_gc,
					draw_x, i * fh, cnt * fw, fh);
	       }
	       
	       /* do we have to change the current gc */
	       if ((j < w->term.columns) && (c->value != 0)) {
		    cur_attr = c->attributes;
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
      * restart the timer if we've found any blinking characters or the
      * current attribute is set to blink
      */
     if ((no > 0) || ((w->term.act_attribs & ATT_BLINK) != 0))
	  w->term.blink_id = XtAddTimeOut(w->term.tblink_interval,
					  blink_text, w);
     else
	  w->term.blink_id = 0;
     
     /*
      * Flip the draw mode
      */
     w->term.blink_text_on = !w->term.blink_text_on;
}

/*
 * Scroll the Screen to start with the 'no'th Line of the save area from the
 * bottom.
 *
 * The normal area will be 'under' the saved.
 */
Export void
scroll_save_abs(w, no, flag)
register TermCanvasWidget w;
int no;
int flag;
{
     scroll_save_rel(w, no - w->term.scroll_pos, flag);
}

/*
 * Scroll the Screen plus the save area the specified number of lines
 */
Export void
scroll_save_rel(w, no, flag)
register TermCanvasWidget w;
register int no;
int flag;
{
     register signed int pos;
     
     /*
      * Check if we can really scroll that many lines
      */
     pos = w->term.scroll_pos + no;
     if (pos <= 0) {
	  no = - w->term.scroll_pos;
	  
	  /*
	   * At this point, adjust the scrollbar no matter what, since
	   * the thumb starts leaving the bottom of the bar now.
	   */
	  if (w->term.adjust_scroll_bar != NULL)
	       w->term.adjust_scroll_bar(XtParent(w),
			(float)(w->term.save_size) /
			(float)(w->term.lines + w->term.save_size),
			(float)w->term.lines /
			(float)(w->term.lines + w->term.save_size));
     }
	  
     if (pos > (signed) w->term.save_size)
	  no = w->term.save_size - w->term.scroll_pos;

     if (no == 0)
	  return;
     
     /*
      * Unmark the selection to avoid trouble with the coordinates.
      */
     if (w->term.select_on)
	  unmark_selection(w);
     
     /* Set the new scroll position */
     w->term.scroll_pos += no;
     
     /*
      * Scrolling the screen only makes sense if we scroll less than
      * a screen full.
      */
     if (abs(no) < w->term.lines) {
	  scroll_screen(w, 0, w->term.lines, -no);

	  /* let redraw_rect do the new lines */
	  if (no > 0)
	       redraw_rect(w, w->term.char_array, 0, 0,
			   no - 1, w->term.columns - 1);
	  else
	       redraw_rect(w, w->term.char_array, w->term.lines + no, 0,
			   w->term.lines - 1, w->term.columns - 1);
     }
     /*
      * Just redraw the whole screen elsewhise
      */
     else {
	  XClearWindow(XtDisplay(w), XtWindow(w));
	  redraw_rect(w, w->term.char_array, 0, 0,
		      w->term.lines - 1, w->term.columns - 1);
     }
     
     /* Adjust the cursor position */
     if(w->term.array_cur.lin + w->term.scroll_pos >= w->term.lines)
	  w->term.screen_cur.y = -1;
     else {
	  register int tmp = w->term.screen_cur.y;
	  
	  w->term.screen_cur.y = (w->term.array_cur.lin + w->term.scroll_pos
				  + 1) * w->term.cell_height;
	  
	  /* If the cursor reappears, put it back */
	  if ((tmp < 0) && (w->term.array_cur.lin + w->term.scroll_pos
			    < w->term.lines))
	       flip_cursor_on(w);
     }
     /*
      * Tell the Parent to adjust the scrollbar 
      */
     if ((flag != 0) && (w->term.adjust_scroll_bar != NULL))
	  w->term.adjust_scroll_bar(XtParent(w),
				 (float)(w->term.save_size - w->term.scroll_pos) /
				 (float)(w->term.lines + w->term.save_size),
				 (float)w->term.lines /
				 (float)(w->term.lines + w->term.save_size));
} 

/*
 * Set the current cursor positon to be lin and col.
 * Checks if the clip rectangle for the gc has to be changed and validates
 * the cursor gcs.
 */
Export void
set_cursor_pos(w, lin, col)
register TermCanvasWidget w;
register int lin, col;
{
     if (col >= w->term.columns)
	  col = w->term.columns;
     
     w->term.array_cur.lin = lin;
     w->term.array_cur.col = col;
     w->term.screen_cur.y = (lin + 1) * w->term.cell_height;
     w->term.screen_cur.x = col * w->term.cell_width;
     
     validate_cursor_gc(w, w->term.char_array[lin][col]);
     
     if (w->term.no_block_cursor)
	  XSetClipOrigin(XtDisplay(w), w->term.cursor_text_gc,
			 w->term.screen_cur.x,
			 w->term.screen_cur.y - w->term.cursor_height);
}

Export void
validate_cursor_size(w)
register TermCanvasWidget w;
{
     XRectangle rect;

     if ((w->term.cursor_width == 0) ||
	 (w->term.cursor_width > w->term.cell_width))
	  w->term.cursor_width = w->term.cell_width;
     if ((w->term.cursor_height == 0) ||
	 (w->term.cursor_height > w->term.cell_height))
	  w->term.cursor_height = w->term.cell_height;

     w->term.no_block_cursor = (w->term.cursor_height != w->term.cell_height)
	  || (w->term.cursor_width != w->term.cell_width);

     /* change the clip rectangle */
     if (w->term.no_block_cursor) {
	  rect.x = 0;
	  rect.y = 0;
	  rect.width = w->term.cursor_width;
	  rect.height = w->term.cell_height - w->term.cursor_height;
	  
	  XSetClipRectangles(XtDisplay(w), w->term.cursor_text_gc,
			     w->term.screen_cur.x,
			     w->term.screen_cur.y - w->term.cursor_height,
			     &rect, 1, YXBanded);
     }
     else
	  XSetClipMask(XtDisplay(w), w->term.cursor_text_gc, None);
}
