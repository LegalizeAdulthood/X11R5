/* $Header: /usr3/Src/emu/canvas/RCS/selection.c,v 1.8 91/08/02 10:38:59 me Exp Locker: me $ */

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
 * Selection Handling Stuff
 *
 * Author: Michael Elbel
 * Date: 28. 06. 1990
 * Description:	Contains the routines to deal with selections.
 * 		As there are:
 * 			Marking the area to select,
 * 			Getting the selection,
 * 			Transforming the screen data to a string,
 * 			Pasting to the output stream.
 *
 * Revision History:
 *
 * $Log:	selection.c,v $
 * Revision 1.8  91/08/02  10:38:59  me
 * Fixed behaviour when no selection is available, and we want to paste
 * 
 * Revision 1.7  91/08/01  18:09:28  jkh
 * Whatever the hell mikey last did to this poor code.
 * 
 * Revision 1.6  90/11/20  17:40:02  jkh
 * Alpha.
 * 
 * Revision 1.5  90/10/09  14:38:02  me
 * Added support for the changed cursor drawing
 * 
 * Revision 1.4  90/08/31  19:51:57  me
 * Checkpoint vor dem Urlaub
 * 
 * Revision 1.3  90/08/09  10:21:35  me
 * changed function names to match the actions, fixed bug in code that
 * handles the case you try to select beyond the bottom of the window
 * 
 * Revision 1.2  90/07/26  02:37:22  jkh
 * Added new copyright.
 * 
 * Revision 1.1  90/07/18  16:32:57  me
 * Initial revision
 * 
 *
 */

#include "canvas.h"
#include <X11/Xatom.h>

#ifdef TRACK_MALLOC
static int selection_malloc_count = 0;
static int atom_malloc_count = 0;
#endif

/*
 * Two macros for calculating the x and y positions in character coordinates
 * screen coordinates
 */
#define char_x(x_pos) (x_pos / w->term.cell_width)
#define char_y(y_pos) (y_pos / w->term.cell_height)

/*
 * Global Variables
 */
Local Time release_time = 0;	/* Time when the last key release occured */
Local int num_clicks = 0;	/* Number of clicks so far */
Local int max_num_clicks = 2;	/* Maximum supported number of clicks */

/* For selections */
typedef struct _selectionList {
    String *params;
    Cardinal count;
    Time time;
} SelectionList;

Local void get_selection();

/*
 * flip a rectangle's ATT_SELECTED status and redraw it
 */
Local void
rect(w, x1, y1, x2, y2)
register TermCanvasWidget w;
int x1, y1, x2, y2;
{
     register CharPtr ptr;
     register CharArray array = w->term.char_array;
     register CharArray sarray = w->term.save_array;
     int spos = w->term.scroll_pos;
     int slen = w->term.save_lines;
     
     register int i, j;
     for (i = y1; i <= y2; i++) {
	  if (i < spos)
	       ptr = sarray[slen - spos + i] + x1;
	  else
	       ptr = array[i - spos] + x1;
	  
	  for (j = x1; j <= x2; j++, ptr++) {
	       ptr->attributes ^= ATT_SELECTED;
	  }
     }

     i = w->term.array_cur.lin;
     j = w->term.array_cur.col;

     /* Is the cursor inside the rectangle ? */
     if ((j >= x1) && (j <= x2) && (i >= y1) && (i <=y2)) {
	  /* Validate the cursor GCs */
	  Char ch;
	  ch = w->term.curs_char;
	  ch.attributes ^= ATT_SELECTED;

	  validate_cursor_gc(w, ch);
     }
     redraw_rect(w, array, y1, x1, y2, x2);
}

#define sx w->term.sel_start_x
#define sy w->term.sel_start_y
#define ex w->term.sel_end_x
#define ey w->term.sel_end_y
     
Local int
atom_to_buffer(a)
Atom a;
{
     int buffer = -1;

     switch (a) {
     case XA_CUT_BUFFER0: buffer = 0; break;
     case XA_CUT_BUFFER1: buffer = 1; break;
     case XA_CUT_BUFFER2: buffer = 2; break;
     case XA_CUT_BUFFER3: buffer = 3; break;
     case XA_CUT_BUFFER4: buffer = 4; break;
     case XA_CUT_BUFFER5: buffer = 5; break;
     case XA_CUT_BUFFER6: buffer = 6; break;
     case XA_CUT_BUFFER7: buffer = 7; break;
     }
     return buffer;
}

Local Boolean
convert_selection(w, selection, target, type, value, length, format)
register TermCanvasWidget w;
Atom *selection, *target, *type;
caddr_t *value;
unsigned long *length;
int *format;
{
     if (*target == XA_STRING) {
	  *value = w->term.selection_text;
	  *type = XA_STRING;
	  *length = w->term.selection_len;
	  *format = sizeof(unsigned char) * 8;
	  return(True);
     }
     else
	  return(False);
}

Local void
lose_selection(w, selection)
register TermCanvasWidget w;
Atom *selection;
{
     register Atom *atomP = w->term.sel_atoms;
     register int i = 0;

     for (i = 0, atomP = w->term.sel_atoms;
	  i < w->term.n_sel_atoms; i++, atomP++)
	  if (*selection == *atomP || atom_to_buffer(*atomP) != -1)
	       *atomP = (Atom)0;

     for (i = w->term.n_sel_atoms; i; i--)
	  if (w->term.sel_atoms[i - 1] != 0)
	       break;

     w->term.n_sel_atoms = i;

     for (i = 0, atomP = w->term.sel_atoms;
	  i < w->term.n_sel_atoms; i++, atomP++)
	  if (*atomP == (Atom)0)
	       *atomP = w->term.sel_atoms[--w->term.n_sel_atoms];

     if (w->term.n_sel_atoms == 0)
	  unmark_selection(w);
}

static void
selection_done(w, selection, target)
Widget w;
Atom *selection, *target;
{
     /*
      * This function is just needed to prevent the selection data
      * from being freed by the Intrinsics
      */
}

Local void
receive_selection(w, client_data, selection, type, value, length, format)
register TermCanvasWidget w; 
caddr_t client_data;
Atom *selection, *type;
caddr_t value;
unsigned long *length;
int *format;
{
     register ComBlockPtr block = w->term.com_block;
     int i = *length;
     unsigned char *buf = (unsigned char *)value;
     
     /* Nothing is selected, bag it */
     if (!value && !i)
	  return;

     cb_opcode(block) = OP_INSERT;

     /* XT_CONVERT_FAIL case */ 
     if (*type == 0 || *length == 0) {
	  SelectionList *list = (SelectionList *)client_data;
	  if (list != NULL) {
	       get_selection(w, list->time, list->params, list->count);
	       XtFree(client_data);
	  }
	  return;
     }
     /* if necessary, split up the request */
     while (i > BUF_SZ) {
	  cb_nbytes(block) = BUF_SZ;
	  strncpy(cb_buffer(block), buf, BUF_SZ);
	  
	  if (w->term.output != NULL) {
	       w->term.output(XtParent(w), block);
	  }
	  buf += BUF_SZ;
	  i -= BUF_SZ;
     }
     
     cb_nbytes(block) = i;
     strncpy(cb_buffer(block), buf, i);
     
     if (w->term.output != NULL) {
	  w->term.output(XtParent(w), block);
     }
     
     if (client_data != NULL)
	  XtFree(client_data);
     XtFree(value);
}

/* action routine to request the selection */

Export void
paste_selection(w, event, params, num_params)
Widget w;
XButtonEvent *event;
String *params;
Cardinal *num_params;
{
     get_selection(w, event->time, params, *num_params);
}

/*
 * Grot to get selection / cut buffer contents. Mostly stolen from xterm.
 * To be honest, I'm not all that sure I understand all of it, but it seems
 * to work, so what the heck -jkh.
 */
Local void
get_selection(w, time, params, num_params)
register TermCanvasWidget w;
Time time;
String *params;
Cardinal num_params;
{
     int inbytes, buffer, fmt8 = 8;
     Atom selection, type = XA_STRING;

     XmuInternStrings(XtDisplay(w), params, (Cardinal)1, &selection);
     buffer = atom_to_buffer(selection);

     /*
      * Stay moderately compatible with old clients by trying the cut buffers
      * first.
      */
     if (buffer >= 0) {
	  char *line;
	  unsigned long nbytes;
	  
	  line = XFetchBuffer(XtDisplay(w), &inbytes, buffer);
	  nbytes = (unsigned long)inbytes;

	  if (nbytes > 0)
	       receive_selection(w, NULL, &selection, &type, (caddr_t)line,
				 &nbytes, &fmt8);
	  else if (num_params > 0)
	       get_selection(w, time, params + 1, num_params - 1);
     }
     else {
	  SelectionList *list;
	  if (--num_params) {
	       list = XtNew(SelectionList);
	       list->params = params + 1;
	       list->count = num_params;
	       list->time = time;
	  }
	  else
	       list = NULL;
	  XtGetSelectionValue((Widget)w, selection, XA_STRING, receive_selection,
			      (caddr_t)list, time);
     }
}

/*
 * Invert the given text region.
 * flip_mark relies on start values being <= end values
 */
Local void
flip_mark(w, start_x, start_y, end_x, end_y)
register TermCanvasWidget w;
register int start_x, start_y, end_x, end_y;
{
     register int tmp;
     
     /* if necessary flip the margins so start is always smaller than end */
     if (start_y > end_y) {
	  tmp = start_y;
	  start_y = end_y;
	  end_y = tmp;
	  tmp = start_x;
	  start_x = end_x;
	  end_x = tmp;
     }
     else if ((start_y == end_y) && (start_x > end_x)) {
	  tmp = start_x;
	  start_x = end_x;
	  end_x = tmp;
     }
     
     if (start_y == end_y)
	  /* it's just on one line */
	  rect(w, start_x, start_y, end_x - 1, start_y);
     else {
	  /* draw the first line till the end */
	  if (start_x < w->term.columns)
	       rect(w, start_x, start_y, w->term.columns - 1, start_y);
	  
	  /* are there more than two lines ? */
	  if ((end_y - start_y) > 1)
	       /* Draw fill lines */
	       rect(w, 0, start_y + 1, w->term.columns - 1, end_y  - 1);
	  /* draw the last line from the beginning */
	  if (end_x > 0)
	       rect(w, 0, end_y, end_x - 1, end_y);
     }
}

/*
 * Unmark the selected area
 */
Export void
unmark_selection(w)
TermCanvasWidget w;
{
     if ((w->term.select_on) && ((sx != ex) || (sy != ey)))
	  flip_mark(w, sx, sy, ex, ey);
     w->term.select_on = False;
}

/*
 * Action routine to start highlighting. Will be called via a Translation
 */
Export void
start_selection(w, event)
TermCanvasWidget w;
XButtonEvent *event;
{
     /* Check if the last key release was less than the multi-click time ago */
     if ((event->time - release_time) <= w->term.multi_click_time) {
	  /*
	   * Go get the next type of extend action
	   */
	  if (++num_clicks > max_num_clicks)
	       num_clicks = 0;
	  extend_selection(w, event);
	  return;
     }
     else {
	  /*
	   * Reset the num_clicks. We'll fall into this case with the
	   * first click, so it's not necessary to unmark the selection
	   * in the code above.
	   */
	  num_clicks = 0;
     }
     
     /* If we already have the selection on unmark the area first */
     unmark_selection(w);
     
     /* Set the start point */
     sx = ex = w->term.sel_first_x = char_x(event->x);
     sy = ey = w->term.sel_first_y = char_y(event->y);
}

Local int
next_char_back(w, scp, attrp)
register TermCanvasWidget w;
ScreenCursorPtr scp;
int *attrp;
{
     CharPtr cpt;
     int val;
     
     /* save region ? */
     if (scp->y < w->term.scroll_pos) {
	  cpt = w->term.save_array[w->term.save_lines -
				  w->term.scroll_pos + scp->y] + scp->x;
     }
     else {
	  cpt = w->term.char_array[scp->y - w->term.scroll_pos] + scp->x;
     }
     
     val = cpt->value;
     *attrp = (int)cpt->attributes;
     
     if (scp->x == 0) {
	  *attrp |= ATT_BOL;
     }
     else if (scp->x >= w->term.columns) {
	  *attrp |= ATT_EOL;
     }
     if (scp->y == 0) {
	  *attrp |= ATT_BOS;
     }
     else if (scp->y >= w->term.lines) {
	  *attrp |= ATT_EOS;
     }

     if (scp->x == 0) {
	  if (scp->y != 0) {
	       scp->y--;
	       scp->x = w->term.columns - 1;
	  }
	  else {
	       return EOF;
	  }
     }
     else {
	  scp->x--;
     }
     
     return val;
}


Local int
next_char_forw(w, scp, attrp)
register TermCanvasWidget w;
ScreenCursorPtr scp;
int *attrp;
{
     CharPtr cpt;
     int val;
     
     /* save region ? */
     if (scp->y < w->term.scroll_pos) {
	  cpt = w->term.save_array[w->term.save_lines -
				  w->term.scroll_pos + scp->y] + scp->x;
     }
     else {
	  cpt = w->term.char_array[scp->y - w->term.scroll_pos] + scp->x;
     }
     
     *attrp = (int)cpt->attributes;
     
     if (scp->x == 0) {
	  *attrp |= ATT_BOL;
     }
     else if (scp->x >= w->term.columns) {
	  *attrp |= ATT_EOL;
     }
     if (scp->y == 0) {
	  *attrp |= ATT_BOS;
     }
     else if (scp->y >= w->term.lines) {
	  *attrp |= ATT_EOS;
     }

     val = cpt->value;
     
     if (scp->x == w->term.columns) {
	  if (scp->y < w->term.lines) {
	       scp->y++;
	       scp->x = 0;
	  }
	  else {
	       return EOF;
	  }
     }
     else {
	  scp->x++;
     }

     return val;
}

Export void
extend_selection(w, event)
register TermCanvasWidget w;
XButtonEvent *event;
{
     int x, y;
     ScreenCursor start, end;


     x = char_x(event->x);
     if (x < 0)
	  x = 0;
     if (x > w->term.columns)
	  x = w->term.columns;
     
     y = char_y(event->y);
     if (y < 0)
	  y = 0;
     if (y >= w->term.lines)
	  y = w->term.lines - 1;
     
     /*
      * set the new margins
      *
      * Are we before or after the first point?
      */
     if (y < w->term.sel_first_y ||
	 (y == w->term.sel_first_y && x <= w->term.sel_first_x)) {
	  /* before, so change the start point */
	  start.x = x;
	  start.y = y;
	  end.x = w->term.sel_first_x;
	  end.y = w->term.sel_first_y;
     }
     else {
	  /* after, so change the end point */
	  start.x = w->term.sel_first_x;
	  start.y = w->term.sel_first_y;
	  end.x = x;
	  end.y = y;
     }

     /*
      * Now expand the beginning and the end of the selection
      * depending on num_clicks.
      */
     
     switch (num_clicks) {
	  /* 0 Is the default case, do nothing*/
	case 0:
	  break;
	  
	  /* Hop words */
	case 1:
	  start = canvasDoMotion(w, "word", next_char_back, start);
	  end = canvasDoMotion(w, "word", next_char_forw, end);
	  /* correct the end */
	  end.x++;
	  break;

	  /* Whole line, just set sx to 0 and ex to columns */
	case 2:
	  start.x = 0;
	  end.x = w->term.columns;
	  break;
	
	default:
	  warn("extend_selection: Unsupported multi_clicks: %d\n", num_clicks);
     }

     /*
      * Redraw the newly (un)marked parts
      */
     if (start.x != sx || start.y != sy) {
	  flip_mark(w, sx, sy, start.x, start.y);
	  w->term.select_on = True;
	  sx = start.x;
	  sy = start.y;
     }
     if (end.x != ex || end.y != ey) {
	  flip_mark(w, ex, ey, end.x, end.y);
	  w->term.select_on = True;
	  ex = end.x;
	  ey = end.y;
     }
}

Export void
end_selection(w, event, params, num_params)
register TermCanvasWidget w;
XButtonEvent *event;
String *params;
Cardinal *num_params;
{
     unsigned char buffer[BUF_SZ * 10];
     int i, x, y, e_x, e_y, len, ecol;
     unsigned char *bp;
     register CharPtr ptr;
     
     register int spos = w->term.scroll_pos;
     register int slen = w->term.save_lines;
     Atom *atoms = w->term.sel_atoms;
     Cardinal count = *num_params;
     Boolean have_selection = FALSE;

     /*
      * Set the release time
      */
     release_time = event->time;
     
     extend_selection(w, event);

     /* accumulate the selected Text */
     e_x = ex;
     e_y = ey;
     x = sx;
     y = sy;

     if ((e_x == x) && (e_y == y)) {
	  /* empty selection */
	  w->term.select_on = False;
	  return;
     }
     
     ecol = w->term.columns - 1;
     
     len = 0;
     bp = buffer;
     
     /* Usual check for save_region */
     if (y < spos)
	  ptr = w->term.save_array[slen - spos + y] + x;
     else
	  ptr = w->term.char_array[y - spos] + x;
     
     while ((y < e_y) || ((y == e_y) && (x < e_x))) {
	  if (ptr->value != 0) {
	       *bp++ = ptr->value;
	       len++;
	  }
	  if ((ptr->attributes & ATT_CONTINUE) != 0) {
	       /* just go to a new line and don't insert a \n */
	       y++;
	       x = 0;
	       if (y < spos)
		    ptr = w->term.save_array[slen - spos + y];
	       else
		    ptr = w->term.char_array[y - spos];
	  } else if (++x > ecol) {
	       /* insert a newline */
	       *bp++ = '\n';
	       len++;
	       
	       /* advance to next line */
	       y++;
	       x = 0;
	       if (y < spos)
		    ptr = w->term.save_array[slen - spos + y];
	       else
		    ptr = w->term.char_array[y - spos];
	  }
	  else
	       ptr++;
     }

     /*
      * If we haven't got any text to select,
      * just exit here
      */
     if (len == 0) {
	  return;
     }
     
     /* if there was an old buffer in the widget, free it */
     if (w->term.selection_text != NULL) {
	  XtFree(w->term.selection_text);
#ifdef TRACK_MALLOC
	  debug("Freed selection %d", --selection_malloc_count);
#endif
     }
     /* now copy the data to a freshly allocated buffer in the widget */
     if ((w->term.selection_text =
	  XtMalloc(len * sizeof(unsigned char))) == NULL) {
	  warn("EndHighlight: couldn't allocate %d chars for selection buffer",
	       len);
	  return;
     }
#ifdef TRACK_MALLOC
     debug("Allocated %d bytes for selection %d", len * sizeof(unsigned char),
	   ++selection_malloc_count);
#endif
     strncpy(w->term.selection_text, buffer, len);
     w->term.selection_len = len;

     if (count > w->term.n_sel_size) {
	  if (atoms) {
	       XtFree((char *)atoms);
#ifdef TRACK_MALLOC
	       debug("Freed atom list %d", --atom_malloc_count);
#endif
	  }
	  atoms = (Atom *)XtMalloc(count * sizeof(Atom));
#ifdef TRACK_MALLOC
	  debug("Allocated %d bytes for atom list %d", count * sizeof(Atom),
		++atom_malloc_count);
#endif
	  w->term.sel_atoms = atoms;
	  w->term.n_sel_size = count;
     }
     XmuInternStrings(XtDisplay(w), params, count, atoms);
     for (i = 0; i < count; i++) {
	  int buffer = atom_to_buffer(atoms[i]);

	  if (buffer >= 0)
	       XStoreBuffer(XtDisplay(w), w->term.selection_text,
			    w->term.selection_len, buffer);
	  else
	       have_selection |= XtOwnSelection((Widget)w, atoms[i], event->time,
						convert_selection,
						lose_selection,
						selection_done);
     }
     w->term.n_sel_atoms = count;
     if (!have_selection) {
	  warn("couldn't get a selection");
	  unmark_selection(w);
     }
}

/*
 * Checks if the text to draw would overwrite part of the selected area
 */
Export Boolean
in_selected_area(w, lin, scol, ecol)
register TermCanvasWidget w;
int lin, scol, ecol;
{
     if (!w->term.select_on)
	  return(False);
     
     /*
      * in-selected area is true if
      *   the selection beginning is less-equal the end of the checked area
      * AND
      *   the selection end is greater-equal the beginning of the checked area
      */
     return(((lin > sy) || ((lin == sy) && (ecol >= sx))) &&
	    ((lin < ey) || ((lin == ey) && (scol <= (ex + 1)))));
}

/*
 * This routine MUST be called before doing any actual scrolling !!!
 * 
 * If the selection as a whole gets scrolled, adjust its lines.
 * If only a part of it would be scrolled just deselect it
 */
Export void
scroll_selection(w, slin, lines, no)
register TermCanvasWidget w;
int slin, lines, no;
{
     int elin;
     
     if (!w->term.select_on)
	  return;

     elin = slin + lines - 1;
     
     /* If the selection is outside the scroll region get out */
     if ((slin > ey) || (elin < sy))
	  return;
     
     /* If the scroll region is part of the selection, unmark it */
     if ((slin > sy) || (elin < ey))
	  unmark_selection(w);

     /*
      * Now we're sure that the selection is fully inside the scroll region
      * so adjust the lines
      */

     /*
      * If we'd get outside the scroll region now, unmark the selection
      */
     if (((sy - no) < slin) || ((ey - no) > elin))
	  unmark_selection(w);
     
     sy -= no;
     ey -= no;
}
