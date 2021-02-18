/*  @(#)p_lst_impl.h 50.6 90/12/12 SMI */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef __panel_list_impl_h
#define __panel_list_impl_h

#include <xview/panel.h>
#include <xview/font.h>
#include <xview/openmenu.h>
#include <xview/scrollbar.h>

typedef struct panel_list_struct		Panel_list_info;
typedef struct panel_list_row_struct		Row_info;

typedef enum {
    OP_NONE,
    OP_CHANGE,
    OP_INSERT,
    OP_DELETE
} Edit_op;

#define PRIMARY_CHOICE	0
#define SHELF_CHOICE	1
#define PANEL_LIST_DEFAULT_ROW	5

struct panel_list_row_struct {
	Xv_opaque	client_data;	/* Client data with each row */
	int		display_str_len; /* length of displayed string */
	Xv_Font		font;		/* NULL => use WIN_FONT */
	Pixrect		*glyph;
	int		row;            /* Row number */
	u_char		*string;
	int		string_y;

	unsigned edit_selected : 1;	/* selected in edit mode */
	unsigned free_string : 1;	/* free malloc'ed string */
	unsigned selected : 1;          /* selected in read mode (= current) */
	unsigned show : 1;		/* row is to be painted */

	/* Chaining */
	struct panel_list_row_struct *next;
	struct panel_list_row_struct *prev;
#ifdef OW_I18N
	wchar_t		*string_wc;
#endif OW_I18N
};
 
struct panel_list_struct {
	Panel_item	public_self;
        Panel		parent_panel;   /* Panel we're in */
	Rect		list_box;	/* Box enclosing list of rows */
	Scrollbar	list_sb;	/* Scrollbar for list_box */
	Menu		edit_menu;	/* Row panel edit mode menu */
	Edit_op		edit_op;	/* current edit operation, if any */
	Pixfont		*font;		/* font of parent panel */
	XFontStruct     *font_struct;	/* font structure of parent panel */
	Menu		read_menu;	/* Row panel read mode menu */
	Panel_item	text_item;	/* Text item used during editing */
	Row_info	*text_item_row;	/* Text item is editing this row */
	int		text_item_view_start; /* first row in view when text
					       * item was made visible. */
	void		(*event_proc)();
					/* Client panel event proc for list */

	/* control */
	unsigned choose_none: 1;	/* no current row is okay */
	unsigned choose_one : 1;    /* TRUE: exclusive, FALSE: non-exclusive */
	unsigned edit_mode : 1;		/* TRUE: read-write, FALSE: read */
	unsigned has_kbd_focus : 1;	/* panel list has keyboard focus */
	unsigned initialized : 1;	/* set of XV_END_CREATE has occurred */
	unsigned left_hand_sb: 1; /* list_box Scrollbar is on left hand side */
	unsigned read_only : 1;		/* TRUE: read, FALSE: read/write */
	unsigned setting_current_row:1;
#ifdef OW_I18N
	unsigned wchar_notify : 1;	/* TRUE: use PANEL_NOTIFY_PROC_WCS
					   FALSE: use PANEL_NOTIFY_PROC */
#endif /* OW_I18N */

	/* sizes and positions */
	unsigned short	nlevels;	/* Number of levels */
	unsigned short	height;		/* Height of the scroll list */
	unsigned short	width;		/* Width of the scroll list */
	unsigned short	nrows;		/* Number of rows */
	unsigned short	rows_displayed;	/* Number of rows displayed */
	unsigned short	row_height;  /* Height of each row. 0 => font height */
	unsigned short  string_x;	/* left margin of each row's string */

	/* Current data */
	Row_info	*rows;
        char	 	*selection[2];  /* Seln string cache */
        int	 	have_seln[2];   /* Seln ownership info cache */
	char		*seln_client;   /* Selection client */
	Row_info	*current_row;	/* last row selected */
	Row_info	*last_edit_row; /* last row selected for editing */
	Row_info	*char_match_row; /* last row character-matched row */
#ifdef OW_I18N
	wchar_t		*selection_wc[2];
	XFontSet	font_set;	/*font set of parent panel */
#endif OW_I18N
};

#endif __panel_list_impl_h



