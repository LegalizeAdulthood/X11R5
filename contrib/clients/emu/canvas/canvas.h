#ifndef XP_CANVAS_H
#define XP_CANVAS_H

/* $Header: /usr3/emu/canvas/RCS/canvas.h,v 1.11 91/03/01 16:44:11 jkh Exp Locker: me $ */

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
 * Declarations and Definitions local to the TermCanvas Widget
 *
 * Author: Michael W. Elbel
 * Date: April 24th, 1990.
 * Description: Siehe oben.
 *
 * Revision History:
 *
 * $Log:	canvas.h,v $
 * Revision 1.11  91/03/01  16:44:11  jkh
 * Changed some include convetions.
 * 
 * Revision 1.10  90/11/20  17:39:41  jkh
 * Alpha.
 * 
 * Revision 1.9  90/10/12  13:53:06  jkh
 * Renaming changes; removed direct malloc/free references.
 * 
 * Revision 1.8  90/10/12  12:41:03  me
 * checkmark
 * 
 * Revision 1.7  90/08/31  19:51:38  me
 * Checkpoint vor dem Urlaub
 * 
 * Revision 1.6  90/08/09  10:22:36  me
 * changed the selction functions' names
 * 
 * Revision 1.5  90/07/26  02:34:19  jkh
 * Added new copyright.
 * 
 * Revision 1.4  90/06/08  16:51:14  me
 * checkpoint
 * 
 * Revision 1.3  90/06/07  15:34:04  me
 * Added various declarations
 * 
 * Revision 1.2  90/05/08  14:43:19  me
 * Changed the declaration of the interface functions from Boolean to int
 * 
 * Revision 1.1  90/05/04  13:39:04  me
 * Initial revision
 * 
 *
 */

#include "TermCanvasP.h"

/* The bits that result in a 'non-normal' GC */
#define FONT_ATTS	(ATT_BOLD | ATT_REVERSE | ATT_SELECTED)

/*
 * Macro to make checking the type of an argument in a comblock easier
 */
#define check_arg_type(block, reg, type, mesg, retcode)			\
{									\
     if (cb_reg_type(block, reg) != type) {				\
          warn("%s - Expected '%s' type in register '%c', got '%s'",	\
	       mesg, reg_type_name(type), reg,				\
	       reg_type_name(cb_reg_type(block, reg)));			\
	  return(retcode);						\
     }									\
}

/* Jump vector for the canvas_handler */
Import IntFuncPtr jumptable[];

/* Function declarations from TermCanvas.c */
Import int XpTermCanvasDispatch();

/* Function declarations from char_array.c */
Import CharArray create_char_array();
Import CharArray resize_char_array();
Import void save_array_lines();
Import void resize_save_area();
Import void scroll_array();
Import void delete_array_chars();
Import void erase_array_chars();
Import void erase_array_lines();
Import TabList create_tab_list();
Import void clear_tab_list();
Import void init_fixed_tabs();
Import TabList resize_tab_list();
Import void free_char_array();
Import void shift_array_line();
Import void put_string_in_array();

/* Function declarations from screen.c */
Import void flip_cursor_off();
Import void flip_cursor_on();
Import void switch_cursor_off();
Import void switch_cursor_on();
Import void do_flip();
Import void blink_cursor();
Import void scroll_screen();
Import void delete_screen_chars();
Import void erase_screen_chars();
Import void erase_screen_lines();
Import GC validate_gc();
Import void validate_cursor_gc();
Import void draw_string();
Import void shift_screen_line();
Import void blink_text();
Import void set_cursor_pos();
Import void validate_cursor_size();

/* Function declarations from selection.c */
Import void start_selection();
Import void extend_selection();
Import void end_selection();
Import void paste_selection();
Import void unmark_selection();
Import Boolean in_selected_area();
Import void scroll_selection();

/* Function declarations from motion.c */
Import ScreenCursor canvasDoMotion();
Import void canvasFreeMotion();

/* Functions from functions.c */
Import int move_cursor();
Import int move_cursor_rel();
Import int insert();
Import int move_abs();
Import int move_rel();
Import int move_abs_column();
Import int move_abs_row();
Import int move_rel_column();
Import int move_rel_row();
Import int move_rel_row_scrolled();
Import int insert_mode();
Import int overwrite_mode();
Import int delete_chars();
Import int delete_to_eol();
Import int delete_lines();
Import int delete_to_eoscr();
Import int erase_chars();
Import erase_line_left();
Import erase_lines();
Import erase_from_toscr();
Import int clear_screen();
Import int insert_blanks();
Import int insert_lines();
Import int set_scroll_region();
Import int ring_bell();
Import int hor_tab();
Import int set_tab_cur_col();
Import int set_tab_col();
Import int set_tabs_eq_width();
Import int clear_tab_cur_col();
Import int clear_tab_col();
Import int clear_all_tabs();
Import int set_attribute();
Import int clear_attribute();
Import int override_translations();
Import int change_flut();
Import int canvas_size();
Import int canvas_cursor_pos();
Import int canvas_attribs();
Import int canvas_scroll_region();
Import int canvas_wrap_mode();
Import int canvas_reverse_mode();
Import int canvas_cursor_on();
Import int canvas_cursor_blinking();
Import int canvas_cursor_size();
Import int redraw_screen();
Import int change_fonts();
Import int scroll_screen_absolute();
Import int scroll_screen_relative();
Import int cursor_off();
Import int cursor_on();
Import int set_screen_size();
Import int wrap_around();
Import int dont_wrap();
Import int cursor_pos_rel_to_scr_reg();
Import int cursor_pos_absolute();
Import int reverse_video();
Import int normal_video();
Import int save_flut();
Import int restore_flut();
Import int set_cursor_blink();
Import int set_cursor_size();
Import int change_fg_color();
Import int change_bg_color();
Import int set_cit_cell();
Import int canvas_display_cells();
Import int dummy();

Import void parse_and_change_flut();
Import void redraw_rect();
Import int font_lookup_string();
Import void set_flut();
Import void scroll_save_abs();
Import void scroll_save_rel();

#endif /* XP_CANVAS_H */
