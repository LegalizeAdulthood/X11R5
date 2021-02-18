#ifndef XP_TERMCANVASP_H
#define XP_TERMCANVASP_H

/* $Header: /usr3/emu/canvas/RCS/TermCanvasP.h,v 1.11 91/07/30 09:24:17 me Exp Locker: me $ */

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
 * Private Header file for the TermCanvas widget
 *
 * Author: Michael Elbel
 * Date: March 20th, 1990.
 *
 * Revision History:
 *
 * $Log:	TermCanvasP.h,v $
 * Revision 1.11  91/07/30  09:24:17  me
 * added motionHead
 * 
 * Revision 1.10  91/07/29  17:32:30  me
 * Added color stuff
 * 
 * Revision 1.9  91/04/11  17:37:11  me
 * whatever Jordan did
 * 
 * Revision 1.8  90/10/12  12:41:00  me
 * checkmark
 * 
 * Revision 1.7  90/08/31  19:51:34  me
 * Checkpoint vor dem Urlaub
 * 
 * Revision 1.6  90/08/09  10:17:36  me
 * added notify_first_map and mapped_yet to the private structure
 * 
 * Revision 1.5  90/07/26  02:30:01  jkh
 * Added new copyright.
 * 
 * Revision 1.4  90/06/08  16:51:50  me
 * checkpoint
 * 
 * Revision 1.3  90/05/08  17:33:03  me
 * Tab List and default tab width added
 * 
 * Revision 1.2  90/05/08  14:10:49  me
 * Jordan's infamous changes
 * 
 *
 */

#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>

#include "common.h"
#include "TermCanvas.h"

typedef struct {
     int nothing;
} TermCanvasClassPart;

typedef struct _TermCanvasClassRec {
     CoreClassPart		core_class;
     TermCanvasClassPart	term_canvas_class;
} TermCanvasClassRec;

/*
 * Don't change this structure without absolutely
 * knowing what you do. Right now it fits in 32
 * Bits. If you add only a single byte, your compiler
 * will most likely add another 32 Bits to its size.
 */
typedef struct _Char {
     unsigned short	value;
     unsigned char	attributes;
     unsigned char	color;
} Char, *CharPtr;
#define GET_COLOR_FG(col) ((int)(col & 0xf))
#define GET_COLOR_BG(col) ((int)(col >> 4))
#define SET_COLOR_FG(col, fg) ((col & 0xf0) | (unsigned char)(fg & 0xf))
#define SET_COLOR_BG(col, bg) ((col & 0x0f) | (unsigned char)(bg << 4))

typedef Char *Line, **LinePtr;

typedef Line *CharArray;

typedef Boolean TabListElem, *TabList;

typedef struct _ArrayCursor {
     int	lin;
     int	col;
} ArrayCursor, *ArrayCursorPtr;

typedef struct _ScreenCursor {
     int	x;
     int	y;
} ScreenCursor, *ScreenCursorPtr;

/*
 * I hardcode the number of Colors 'cuz it depends on
 * the structure of Char - you'd have to do major changes
 * in the CharArray structure if you wanted to increase
 * the number.
 */
typedef struct _ColorIndex {
     Boolean	set;
     Pixel	pix;
} ColorIndex;

typedef struct _ColorIndexTable {
     ColorIndex	fg;
     ColorIndex bg;
} ColorIndexTable[16];

typedef struct {
     /*
      * Public Resources
      */
     Dimension		lines;		/* Lines in current array	     */
     Dimension		columns;	/* Columns in array		     */
     Dimension		cell_width;	/* Width of a character cell	     */
     Dimension		cell_height;	/* Heigth of a character cell	     */
     String		normal_font_n;	/* Name of Font	for ordinary Text    */
     String		bold_font_n;	/* Name of Font	for bold Text	     */
     String		default_font_n;	/* Name of the default Font	     */
     Dimension		ul_width;	/* Width of underline		     */
     Pixel		cursor_fg;	/* color of the cursor foreground    */
     Pixel		cursor_bg;	/* color of the cursor background    */
     Pixel		text_color;	/* color of the text		     */
     Pixel		background_color;/*backup of the background color    */
     Dimension		cursor_width;	/* Cursor width in pixels	     */
     Dimension		cursor_height;	/* Cursor height in pixels	     */
     Boolean		cursor_blinking;/* Does the cursor blink ?	     */
     int		blink_interval;	/* How fast does it blink?	     */
     Boolean		blink_wo_focus; /* blink cursor w/o input focus	     */
     int		tblink_interval;/* How fast does text blink?	     */
     Boolean		wrap_around;	/* Wrap at the end of the line?	     */
     Boolean		insert_mode;	/* Insert or overwrite characters    */
     Dimension		bell_volume;	/* How loud to toot the horn	     */
     Dimension		def_tab_width;	/* Default tab width		     */
     String		term_type;	/* The emulation's name		     */
     Boolean		selection_inv;	/* Mark selections by inverting	     */
     Boolean		reverse_mode;	/* Screen in reverse mode ?	     */
     int		save_lines;	/* How many lines to save	     */
     int		multi_click_time;/* Maximum time between two	     */
					/* clicks in milliseconds	     */
     /* Communication Functions */
     VoidFuncPtr	output;		/* Who takes the output		     */
     VoidFuncPtr	set_size;	/* To set the size		     */
     VoidFuncPtr	notify_first_map;/*Whom to call on the first map     */
     ComBlockPtr	com_block;	/* Used to call 'output' with	     */
     Boolean		com_block_alloc;/* Have we allocated it ourselves?   */
     /*
      * Private Resources, to be kept by every widget instance
      */
     /* X specific stuff */
     /* Text output stuff */
     GC			norm_gc;	/* GC for ordinary text		     */
     GC			spec_gc;	/* GC for text with attributes	     */
     GC			gc;		/* GC we're currently writing with   */
     int		font_descent;	/* Descent below the baseline	     */
     XFontStruct	*normal_font;	/* Font for ordinary text	     */
     XFontStruct	*bold_font;	/* Font for bold text		     */
     /* other X stuff */
     GC			cursor_graph_gc;/* GC for graphic cursor drawing    */
     GC			cursor_text_gc;	/* GC for textual cursor drawing    */
     GC			cursor_rem_gc;  /* GC for textual cursor removing    */
     GC			clear_gc;	/* GC for clearing the screen	     */
     XtIntervalId	timeout_id;	/* Timer ID for cursor blinking	     */
     XtIntervalId	blink_id;	/* Timer ID for text blinking	     */
     Boolean		focused;	/* True if widget has the focus	     */
     Boolean		mapped_yet;	/* Have we ever been mapped ?	     */
     Cursor		pointer;	/* The X pointer to use		     */
     String		pointer_color;	/* The pointer's foreground color    */
     XColor		pointer_fg;	/* The actual X color		     */
     XColor		window_fg;	/* The actual X color		     */
     XColor		window_bg;	/* The actual X color		     */
     int		color_cells;	/* The number of colors on the screen*/
     Boolean		pt_fg_eq_wd_fg; /* Is the pointer foreground the same*/
     					/* color as the window foreground?   */
     ColorIndexTable	cit;		/* The Color Index Table	     */
     /* Selection stuff */
     Position		sel_start_x;	/* Start of the selection X	     */
     Position		sel_start_y;	/* Start of the selection y	     */
     Position		sel_end_x;	/* End of the selection X	     */
     Position		sel_end_y;	/* End of the selection y	     */
     Position		sel_first_x;	/* first X point		     */
     Position		sel_first_y;	/* first Y point		     */
     Atom	        *sel_atoms;	/* list of selections we own	     */
     Cardinal		n_sel_atoms;	/* how many selections we own	     */
     Cardinal		n_sel_size;	/* how much room for selections	     */
     Boolean		select_on;	/* Have we marked the selection ?    */
     String		selection_text; /* The text the selection contains   */
     Dimension		selection_len;	/* The length of the selected text   */
     /* fixed  Layer specific */
     unsigned char	flut[256];	/* Font lookup table 		     */
     unsigned char	save_flut[256];	/* Saved font lookup table 	     */
     CharArray		char_array;	/* Points to the current array	     */
     CharArray		save_array;	/* Points to the save array	     */
     Dimension		save_size;	/* How many lines in save array	     */
     Dimension		scroll_pos;	/* How much we are scrolled back     */
     VoidFuncPtr	adjust_scroll_bar;/* To adjust the scroll bar	     */
     TabList		tabs;		/* Points to the current tab list    */
     ArrayCursor	array_cur;	/* Cursor coords in the array	     */
     ScreenCursor	screen_cur;	/* Ditto in the screen		     */
     Boolean		rel_cursor_pos; /* Cursor positioning relative	     */
     Boolean		cursor_visible;	/* Is the cursor on the screen?	     */
     Boolean		cursor_on;	/* Has the cursor been switched on?  */
     unsigned char	act_attribs;	/* Attributes we are writing with    */
     unsigned char	old_attribs;	/* The last special attributes	     */
     unsigned char	act_color;	/* The Colors we're using	     */
     unsigned char	old_color;	/* The last Colors		     */
     Char 		curs_char;	/* Attributes under the cursor	     */
     Boolean		blink_text_on;	/* Are blinking chars visible ?	     */
     Boolean		outline_cursor;	/* Just draw a outlined cursor	     */
     Boolean		no_block_cursor;/* The cursor is smaller than the
					 * character cell		     */
     Dimension		scroll_top;	/* Scrolling region top		     */
     Dimension		scroll_bottom;	/* Scrolling region bottom	     */
     Generic		motionHead;	/* motion description list	     */
} TermCanvasPart;

typedef struct _TermCanvasRec {
     CorePart		core;
     TermCanvasPart	term;
} TermCanvasRec;

extern TermCanvasClassRec termCanvasClassRec;

#endif /* XP_TERMCANVASP_H */
