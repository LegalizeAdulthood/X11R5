#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_txt.c 70.5 91/07/10";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <malloc.h>
#include <strings.h>
#include <X11/X.h>
#include <xview_private/panel_impl.h>
#include <xview/defaults.h>
#include <xview_private/draw_impl.h>
#include <xview/screen.h>
#include <xview/pixwin.h>
#include <xview/font.h>
#ifdef	OW_I18N
#include <xview/server.h>
#include <xview/xv_i18n.h>
#include <widec.h>
#include <wctype.h>
#include <stdlib.h>
#endif	OW_I18N

#define TEXT_PRIVATE(item)	\
	XV_PRIVATE(Text_info, Xv_panel_text, item)
#define TEXT_PUBLIC(item)	XV_PUBLIC(item)

#define	TEXT_FROM_ITEM(ip)	TEXT_PRIVATE(ITEM_PUBLIC(ip))

extern int OW_GC_KEY;
#ifdef OW_I18N
extern wchar_t	null_string_wc[];
#endif /* OW_I18N */

Pkg_private int text_init();
Pkg_private Xv_opaque text_set_avlist();
Pkg_private Xv_opaque text_get_attr();
Pkg_private int text_destroy();


Pkg_private void panel_invert();
Pkg_private Xv_opaque panel_set_kbd_focus();

char           *malloc();

/*
 * laf (look and feel) begin -- this is the same as the one defined in
 * panel_slider.c
 */


/***********************************************************************/
/* field-overflow pointer                                    		 */
/***********************************************************************/

#define PRIMARY_SELN 0
#define SECONDARY_SELN 1
static int      select_click_cnt[2] = {0, 0};
	/* nbr of select mouse clicks pending (primary, secondary) */

static struct timeval last_click_time;
static int      multi_click_timeout = -1;
static u_char   delete_pending = FALSE;	/* primary selection is
					 * pending-delete */
static Panel_info *primary_seln_panel, *secondary_seln_panel;
static Rect     primary_seln_rect, secondary_seln_rect;
static int      primary_seln_first, primary_seln_last;
static int      secondary_seln_first, secondary_seln_last;
static short	delim_init = FALSE; /* delim_table initialized */
static char	delim_table[256];   /* TRUE= character is a word delimiter */

typedef enum {
    HL_NONE,
    HL_UNDERLINE,
    HL_STRIKE_THRU,
    HL_INVERT
} highlight_t;


static int erase_go_actions[] = {
    ACTION_ERASE_CHAR_BACKWARD,
    ACTION_ERASE_CHAR_FORWARD,
    ACTION_ERASE_WORD_BACKWARD,
    ACTION_ERASE_WORD_FORWARD,
    ACTION_ERASE_LINE_BACKWARD,
    ACTION_ERASE_LINE_END,
    ACTION_GO_CHAR_BACKWARD,
    ACTION_GO_CHAR_FORWARD,
    ACTION_GO_WORD_BACKWARD,
    ACTION_GO_WORD_END,
    ACTION_GO_WORD_FORWARD,
    ACTION_GO_LINE_BACKWARD,
    ACTION_GO_LINE_END,
    ACTION_GO_LINE_FORWARD,
    ACTION_GO_COLUMN_BACKWARD,
    ACTION_GO_COLUMN_FORWARD,
    0	/* list terminator */
};

static highlight_t seln_highlight = HL_NONE;

#define PV_HIGHLIGHT TRUE
#define PV_NO_HIGHLIGHT FALSE

static void	draw_scroll_btn();
static void     text_accept_kbd_focus();
static void     text_accept_key();
static void     text_accept_preview();
static void     text_begin_preview();
static void     blink_value();
static void     text_cancel_preview();
static void     horizontal_scroll();
static void     text_paint();
static void     paint_caret();
static void     paint_text();
static void     paint_value();
static void	panel_add_selection();
static void     panel_find_word();
static void     panel_multiclick_handler();
static void     panel_select_line();
static void     panel_text_handle_event();
static void     text_remove();
static void     text_restore();
static void     text_caret_invert();
static void     text_seln_hilite();
static void     update_caret_offset();
static void     update_text_rect();
static void     update_value();
static void     update_value_offset();
static void     text_yield_kbd_focus();

extern void     (*panel_caret_invert_proc) ();
extern void     (*panel_seln_hilite_proc) ();
extern struct pr_size xv_pf_textwidth();
#ifdef	OW_I18N
extern struct pr_size xv_pf_textwidth_wc();
static void 	ml_panel_moded_interm();
static void 	paint_value_and_interm();
#endif	OW_I18N

extern struct pixrect textedit_cursor_pr;


static Panel_ops ops = {
    panel_text_handle_event,		/* handle_event() */
    text_begin_preview,			/* begin_preview() */
    text_begin_preview,			/* update_preview() */
    text_cancel_preview,		/* cancel_preview() */
    text_accept_preview,		/* accept_preview() */
    (void (*) ()) panel_nullproc,	/* accept_menu() */
    text_accept_key,			/* accept_key() */
    text_paint,				/* paint() */
    text_remove,			/* remove() */
    text_restore,			/* restore() */
    (void (*) ()) panel_nullproc,	/* layout() */
    (void (*) ()) text_accept_kbd_focus,/* accept_kbd_focus() */
    (void (*) ()) text_yield_kbd_focus	/* yield_kbd_focus() */
};


/***********************************************************************/
/* data area		                       */
/***********************************************************************/

typedef struct {
    Panel_item      public_self;/* back pointer to object */
    int             caret_offset;	/* caret's x offset from right margin
					 * of left arrow (which may be blank).
					 * -1 = invalid. */
    int             caret_position;	/* caret's character position */
#ifdef	OW_I18N
    int		    saved_caret_offset; /* caret's x offset, saved when
			   		 * conv mode on and commit. */
    int             saved_caret_position; /* caret's character position */
					  /* saved when conv mode on and commit */
#endif	OW_I18N
    int             display_length;	/* in characters */
    int             ext_first;	/* first char of extended word */
    int             ext_last;	/* last char of extended word */
    int             first_char;	/* first displayed character */
    int             flags;
    int             font_home;
    int             last_char;	/* last displayed character */
    u_char          mask;
    Panel_setting   notify_level;	/* NONE, SPECIFIED, NON_PRINTABLE,
					 * ALL */
    Item_info      *orig_caret;	/* original item with the caret */
    int		    scroll_btn_height;	/* Abbrev_MenuButton_Height() */
    int		    scroll_btn_width;	/* Abbrev_MenuButton_Width() + space */
    int             seln_first[2];	/* index of first char selected
					 * (primary, secondary) */
    int             seln_last[2];	/* index of last char selected
					 * (primary, secondary) */
    int             stored_length;
    u_char         *terminators;
    Rect            text_rect;	/* rect containing text (i.e., not arrows) */
#ifdef	OW_I18N
    wchar_t        *value_wc;
    wchar_t        *terminators_wc;
#endif	OW_I18N
    u_char         *value;
    int             value_offset;	/* right margin of last displayed
					 * char (x offset from right margin
					 * of left arrow) */
}               Text_info;


#define SELECTING_ITEM	0x00000001
#define HASCARET		0x00000002
#define UNDERLINED		0x00000008
#define PTXT_READ_ONLY		0x00000010
#define SELECTING_TEXT	0x00000020
#define LEFT_SCROLL_BTN_SELECTED  0x00000040
#define RIGHT_SCROLL_BTN_SELECTED 0x00000080
#define SELECTING_SCROLL_BTN	0x00000100

#define BOX_X   4		/* laf */
#define BOX_Y   2		/* laf */

#define LINE_Y   1		/* laf */
#define SCROLL_BTN_GAP 3	/* space between Scrolling Button and text */

#define UP_CURSOR_KEY           (KEY_RIGHT(8))
#define DOWN_CURSOR_KEY         (KEY_RIGHT(14))
#define RIGHT_CURSOR_KEY        (KEY_RIGHT(12))
#define LEFT_CURSOR_KEY         (KEY_RIGHT(10))


/*ARGSUSED*/
Pkg_private int
text_init(panel_public, item_public, avlist)
    Panel           panel_public;
    Panel_item      item_public;
    Attr_avlist     avlist;
{
    register Item_info *ip = ITEM_PRIVATE(item_public);
    register Text_info *dp;
    int		    chrht;
    u_char	   *delims;   /* printf formatted text.delimChars default */
    u_char	    delim_chars[256];	/* delimiter characters */
    int		    i;
    Xv_Drawable_info *info;
    Xv_panel_text  *item_object = (Xv_panel_text *) item_public;
    Panel_info	   *panel = PANEL_PRIVATE(panel_public);
    int		    pc_home_y;
#ifdef OW_I18N
    XFontSet        font_set;
    XFontSetExtents	*font_set_extents;
    wchar_t	    *tmp_str_wc;
#else
    XFontStruct	   *x_font_info;
#endif /*OW_I18N*/

    dp = xv_alloc(Text_info);

    /* link to object */
    item_object->private_data = (Xv_opaque) dp;
    dp->public_self = item_public;

    /* set the caret functions for panel_public.c and panel_select.c */
    panel_caret_invert_proc = text_caret_invert;
    panel_seln_hilite_proc = text_seln_hilite;

    if (!panel->seln_client)
	panel_seln_init(panel);

    if (ops_set(ip))
	*ip->ops = ops;		/* copy the button ops vector */
    else
	ip->ops = &ops;		/* use the static text ops vector */
    if (panel->event_proc)
	ip->ops->handle_event = (void (*) ()) panel->event_proc;
    ip->item_type = PANEL_TEXT_ITEM;
    if (ip->notify == panel_nullproc)
	ip->notify = (int (*) ()) panel_text_notify;
    panel_set_bold_label_font(ip);

#ifdef OW_I18N
    font_set = (XFontSet)xv_get(panel->std_font, FONT_SET_ID);
    font_set_extents = XExtentsOfFontSet(font_set);
    pc_home_y = font_set_extents->max_ink_extent.y;
#else
    x_font_info = (XFontStruct *)xv_get(panel->std_font, FONT_INFO);
    pc_home_y = -x_font_info->ascent;
#endif /*OW_I18N*/
    if (pc_home_y < dp->font_home)
        dp->font_home = pc_home_y;

    dp->font_home = -dp->font_home;

    dp->display_length = 80;
    dp->flags |= UNDERLINED;
    dp->mask = '\0';
    dp->notify_level = PANEL_SPECIFIED;
    dp->scroll_btn_height = Abbrev_MenuButton_Height(panel->ginfo);
    dp->scroll_btn_width = Abbrev_MenuButton_Width(panel->ginfo) +
	SCROLL_BTN_GAP;
    dp->stored_length = 80;
#ifdef OW_I18N
    tmp_str_wc = mbstowcsdup ("\n\r\t");
    dp->terminators_wc = (wchar_t *)panel_strsave_wc(tmp_str_wc);
    free ((char *) tmp_str_wc);
#else
    dp->terminators = panel_strsave((u_char *) "\n\r\t");
#endif /*  OW_I18N  */

    if (multi_click_timeout == -1) {
	multi_click_timeout = 100 *
	    defaults_get_integer_check("openWindows.multiClickTimeout",
				 "OpenWindows.MultiClickTimeout", 4, 2, 10);
    }
#ifdef	OW_I18N
    dp->value_wc = (wchar_t *) calloc(1, (u_int) ((dp->stored_length + 1) * 
					     sizeof (wchar_t)));
    dp->value = (u_char *) calloc(1, (u_int) (dp->stored_length + 1));
    if (!dp->value_wc)
	return (NULL);
#else	OW_I18N
    dp->value = (u_char *) calloc(1, (u_int) (dp->stored_length + 1));
    if (!dp->value)
	return (NULL);
#endif	OW_I18N

    ip->value_rect.r_width = panel_col_to_x(panel->std_font,
	dp->display_length);
    chrht = xv_get(panel->std_font, FONT_DEFAULT_CHAR_HEIGHT);
    ip->value_rect.r_height = chrht + BOX_Y;
    if (ip->value_rect.r_height < dp->scroll_btn_height)
	ip->value_rect.r_height = dp->scroll_btn_height;

    /*
     * BUG: since set is not called yet, append will not have the right size
     * for the item.
     */
    panel_append(ip);

    /* If this is the first kbd_focus item, then set current
     * keyboard focus item.
     */
    ip->flags |= WANTS_KEY;
    if (!hidden(ip) && panel->kbd_focus_item == NULL) {
	panel->kbd_focus_item = ip;
	dp->flags |= HASCARET;
    }

    /* If the pixmap used to save and restore the pixels underneath the
     * caret hasn't been created yet, then do so now.
     */
    if (panel->caret_bg_pixmap == NULL) {
	DRAWABLE_INFO_MACRO(panel_public, info);
	panel->caret_bg_pixmap = XCreatePixmap(xv_display(info),
	    xv_get(xv_get(xv_screen(info), XV_ROOT), XV_XID),
	    MAX(panel->active_caret_width, panel->inactive_caret_width),
	    MAX(panel->active_caret_height, panel->inactive_caret_height),
	    xv_depth(info));
    }

    /*
     * Initialize the word delimiter table
     */
    if (!delim_init) {
	delims = (u_char *) defaults_get_string("text.delimiterChars",
	    "Text.DelimiterChars", " \t,.:;?!\'\"`*/-+=(){}[]<>\\\|\~@#$%^&");
	/* Print the string into an array to parse the potential
	 * octal/special characters.
	 */
	sprintf(delim_chars, delims);
	/* Mark off the delimiters specified */
	for (i = 0; i < 256; i++)
	    delim_table[i] = FALSE;
	for (delims = delim_chars; *delims; delims++)
	    delim_table[*delims] = TRUE;
	delim_init = TRUE;
    }

    return XV_OK;
}


Pkg_private     Xv_opaque
text_set_avlist(item_public, avlist)
    Panel_item      item_public;
    register Attr_avlist avlist;
{
    register Item_info *ip = ITEM_PRIVATE(item_public);
    register Text_info *dp = TEXT_PRIVATE(item_public);
    register Attr_attribute attr;
    u_char         *new_value = NULL;
    short	    select_line = FALSE;
    short           value_rect_changed = FALSE;
    Panel_info     *panel = ip->panel;
    Xv_opaque       result;
#ifdef	OW_I18N
    wchar_t        *new_value_wc = NULL;
    wchar_t        *tmp_str_wc;
    XID		   db;
    Attr_avlist	   attrs;
    Xv_Server	   server;
    int		   db_count=0;

    server = XV_SERVER_FROM_WINDOW(PANEL_PUBLIC(ip->panel));
    db = xv_get(server, SERVER_RESOURCE_DB);
#endif	OW_I18N

    /* if a client has called panel_item_parent this item may not */
    /* have a parent -- do nothing in this case */
    if (panel == NULL) {
	return ((Xv_opaque) XV_ERROR);
    }

    /* XV_END_CREATE is not used here, so return. */
    if (*avlist == XV_END_CREATE)
	return XV_OK;

    /* Parse Panel Item Generic attributes before Text Field attributes.
     * Prevent panel_redisplay_item from being called in item_set_avlist.
     */
    ip->panel->status |= NO_REDISPLAY_ITEM;
    result = xv_super_set_avlist(item_public, &xv_panel_text_pkg, avlist);
    ip->panel->status &= ~NO_REDISPLAY_ITEM;
    if (result != XV_OK)
	return result;

    attrs = avlist;

#ifndef OW_I18N
    while (*(attrs = attr_next(attrs))) {
	switch (attrs[0]) {
#else
    while (*attrs || db_count) {
        switch (attrs[0]) {
 
          case XV_USE_DB :
           db_count++;
           break;
#endif OW_I18N

	  case PANEL_VALUE:
	    new_value = (u_char *) attrs[1];
	    break;

#ifdef	OW_I18N
	  case PANEL_VALUE_WCS:
	    new_value_wc = (wchar_t *) attrs[1];
	    break;
#endif	OW_I18N

	  case PANEL_VALUE_UNDERLINED:
	    if (attrs[1]) {
		dp->flags |= UNDERLINED;
	    } else {
		dp->flags &= ~UNDERLINED;
	    }
	    break;
	    /* laf end */

	  case PANEL_NOTIFY_LEVEL:
	    dp->notify_level = (Panel_setting) attrs[1];
	    break;

#ifdef OW_I18N
	  case PANEL_NOTIFY_STRING:
	    if (dp->terminators_wc)
		free(dp->terminators_wc);
	    tmp_str_wc = mbstowcsdup ((u_char *) attrs[1]);
	    dp->terminators_wc = (wchar_t *) panel_strsave_wc(tmp_str_wc);
	    free ((char *) tmp_str_wc);
	    break;

	  case PANEL_NOTIFY_STRING_WCS:
	    if (dp->terminators_wc)
		free(dp->terminators_wc);
	    dp->terminators_wc = (wchar_t *) panel_strsave_wc((wchar_t *) attrs[1]);
	    break;

	  case PANEL_VALUE_STORED_LENGTH:
	    if (db_count) {
		dp->stored_length =
		    (unsigned short)db_get_data(db,
				    ip->instance_qlist,
				    "panel_value_stored_length", XV_INT,
				    attrs[1]);
	    }
	    else
	       dp->stored_length = (int) attrs[1];
	    dp->value = (u_char *) realloc(dp->value,
		(u_int) (dp->stored_length + 1));
	    dp->value_wc = (wchar_t *) realloc(dp->value_wc,
		(u_int) ((dp->stored_length + 1) * sizeof(wchar_t)));
	    break;

	  case PANEL_VALUE_DISPLAY_LENGTH:
	    if (db_count) {
		dp->display_length =
		    (unsigned short)db_get_data(db,
				    ip->instance_qlist,
				    "panel_value_display_length", XV_INT,
				    attrs[1]);
	    }
	    else
		dp->display_length = (int) attrs[1];
	    value_rect_changed = TRUE;
	    break;
#else
	  case PANEL_NOTIFY_STRING:
	    if (dp->terminators)
		free(dp->terminators);
	    dp->terminators = panel_strsave((u_char *) attrs[1]);
	    break;

	  case PANEL_VALUE_STORED_LENGTH:
	    dp->stored_length = (int) attrs[1];
	    dp->value = (u_char *) realloc(dp->value,
		(u_int) (dp->stored_length + 1));
	    break;

	  case PANEL_VALUE_DISPLAY_LENGTH:
	    dp->display_length = (int) attrs[1];
	    value_rect_changed = TRUE;
	    break;
#endif /*  OW_I18N  */



	  case PANEL_MASK_CHAR:
	    dp->mask = (u_char) attrs[1];
	    break;


	  case PANEL_INACTIVE:
	    if (attrs[1] && panel->kbd_focus_item == ip) {
		/* Text item had keyboard focus:
		 * Move keyboard focus to next item that wants keystrokes,
		 * if any.
		 */
		paint_caret(panel->kbd_focus_item, FALSE);
		panel->kbd_focus_item = panel_next_kbd_focus(panel);
		if (panel->kbd_focus_item) {
		    if (panel->kbd_focus_item->item_type == PANEL_TEXT_ITEM) {
			TEXT_FROM_ITEM(panel->kbd_focus_item)->flags |=
			    HASCARET;
			paint_caret(panel->kbd_focus_item, TRUE);
		    } else {
			(*panel->kbd_focus_item->ops->accept_kbd_focus) (
			    panel->kbd_focus_item);
		    }
		}
		if (panel_seln(panel, SELN_CARET)->ip == ip) {
		    panel_seln_cancel(panel, SELN_CARET);
		    panel_seln(panel, SELN_CARET)->ip = panel->kbd_focus_item;
		}
		/* Remove primary selection from item, if any */
		if (panel_seln(panel, SELN_PRIMARY)->ip == ip)
		    panel_seln_cancel(panel, SELN_PRIMARY);
	    }
	    break;

	  case PANEL_READ_ONLY:
	    if (attrs[1]) {
		dp->flags |= PTXT_READ_ONLY;
		ip->flags &= ~WANTS_KEY;
		if (panel->kbd_focus_item == ip) {
		    /*
		     * Text item had caret: move caret to next text item, if
		     * any
		     */
		    paint_caret(panel->kbd_focus_item, FALSE);
		    panel->kbd_focus_item = panel_next_kbd_focus(panel);
		    if (panel->kbd_focus_item) {
			if (panel->kbd_focus_item->item_type ==
			    PANEL_TEXT_ITEM) {
			    TEXT_FROM_ITEM(panel->kbd_focus_item)->flags |=
				HASCARET;
			    paint_caret(panel->kbd_focus_item, TRUE);
			} else {
			    (*panel->kbd_focus_item->ops->accept_kbd_focus) (
				panel->kbd_focus_item);
			}
		    }
		    if (panel_seln(panel, SELN_CARET)->ip == ip) {
			panel_seln_cancel(panel, SELN_CARET);
			panel_seln(panel, SELN_CARET)->ip =
			    panel->kbd_focus_item;
		    }
		    /* Remove primary selection from item, if any */
		    if (panel_seln(panel, SELN_PRIMARY)->ip == ip)
			panel_seln_cancel(panel, SELN_PRIMARY);
		}
	    } else {
		dp->flags &= ~PTXT_READ_ONLY;
		ip->flags |= WANTS_KEY;
	    }
	    break;

	  case PANEL_TEXT_SELECT_LINE:
	    select_line = TRUE;
	    break;

	  default:
	    break;

	}
 
#ifdef OW_I18N
       if (attrs[0] == XV_USE_DB)
	   attrs++;
       else if (db_count && !attrs[0]) {
           db_count--;
           attrs++;
        } else 
            attrs = attr_next(attrs);
#endif OW_I18N
    }


#ifdef OW_I18N
    if ((new_value) || (new_value_wc)){

	wchar_t          *old_value_wc = dp->value_wc;
	u_char           *old_value = dp->value;
	dp->value_wc = (wchar_t *) calloc(1, (u_int) ((dp->stored_length + 1) *
						   sizeof(wchar_t)));
	dp->value = (u_char *) calloc(1, (u_int) (dp->stored_length + 1));

	if (new_value) {
		mbstowcs (dp->value_wc, new_value, dp->stored_length);
		(void) strncpy(dp->value, new_value, dp->stored_length);
	} else {	/* new_value_wc */
		wcstombs (dp->value, new_value_wc, dp->stored_length);
		(void) wsncpy(dp->value_wc, new_value_wc, dp->stored_length);
	}
	
	free(old_value_wc);
	free(old_value);
	
#else
    if (new_value) {
	u_char           *old_value = dp->value;
	dp->value = (u_char *) calloc(1, (u_int) (dp->stored_length + 1));
	(void) strncpy(dp->value, new_value, dp->stored_length);
	free(old_value);
#endif	OW_I18N
	if (created(ip) && !hidden(ip) && panel->kbd_focus_item == ip)
	    paint_caret(ip, FALSE);
	update_value_offset(ip, 0, 0);
	update_value(ip, ACTION_GO_LINE_END, FALSE);
	if (created(ip) && !hidden(ip) && panel->kbd_focus_item == ip)
	    paint_caret(ip, TRUE);
    }

    /*
     * update the value & items rect if the width or height of the value has
     * changed.
     */
    if (value_rect_changed) {
	int		chrht;
	ip->value_rect.r_width = panel_col_to_x(ip->panel->std_font,
	    dp->display_length);
	if (ip->value_rect.r_width < 2*dp->scroll_btn_width)
	    ip->value_rect.r_width = 2*dp->scroll_btn_width;
	chrht = xv_get(ip->panel->std_font, FONT_DEFAULT_CHAR_HEIGHT);
	ip->value_rect.r_height = chrht + BOX_Y;
	ip->rect = panel_enclosing_rect(&ip->label_rect, &ip->value_rect);
    }

#ifdef	OW_I18N
    if (select_line && wslen(dp->value_wc)) {
#else
    if (select_line && strlen(dp->value)) {
#endif	OW_I18N
	/* Select line and position caret at the end of the line */
	select_click_cnt[PRIMARY_SELN] = 3;  /* fake a triple-click */
	update_text_rect(ip);
	panel_select_line(ip, NULL, PRIMARY_SELN);
	delete_pending = TRUE;
	panel_seln_acquire(panel, ip, SELN_PRIMARY, FALSE);
	panel_seln_acquire(panel, ip, SELN_CARET, TRUE);
	dp->orig_caret = NULL;
	update_caret_offset(ip, 0);
	panel_set_kbd_focus(panel, ip);
    }

    return XV_OK;
}


/***********************************************************************/
/* get_attr                                                            */
/* returns the current value of an attribute for the text item.        */
/***********************************************************************/
/*ARGSUSED*/
Pkg_private     Xv_opaque
text_get_attr(item_public, status, which_attr, avlist)
    Panel_item      item_public;
    int            *status;
    register Attr_attribute which_attr;
    va_list         avlist;
{
    register Text_info *dp = TEXT_PRIVATE(item_public);

    switch (which_attr) {
      case PANEL_VALUE:
#ifdef	OW_I18N
	dp->value = (u_char *) wcstombsdup (dp->value_wc);
#endif	OW_I18N
	return (Xv_opaque) dp->value;

#ifdef	OW_I18N
      case PANEL_VALUE_WCS:
	return (Xv_opaque) dp->value_wc;
#endif	OW_I18N

      case PANEL_VALUE_UNDERLINED:
	return (Xv_opaque) (dp->flags & UNDERLINED) ? TRUE : FALSE;

      case PANEL_VALUE_STORED_LENGTH:
	return (Xv_opaque) dp->stored_length;

      case PANEL_VALUE_DISPLAY_LENGTH:
	return (Xv_opaque) dp->display_length;

      case PANEL_MASK_CHAR:
	return (Xv_opaque) dp->mask;

      case PANEL_NOTIFY_LEVEL:
	return (Xv_opaque) dp->notify_level;

#ifdef OW_I18N
      case PANEL_NOTIFY_STRING:
	dp->terminators = (u_char *) wcstombsdup (dp->terminators_wc);
	return (Xv_opaque) dp->terminators;

      case PANEL_NOTIFY_STRING_WCS:
	return (Xv_opaque) dp->terminators_wc;
#else
      case PANEL_NOTIFY_STRING:
	return (Xv_opaque) dp->terminators;
#endif /*  OW_I18N  */

      case PANEL_READ_ONLY:
	if (dp->flags & PTXT_READ_ONLY)
	    return (Xv_opaque) TRUE;
	else
	    return (Xv_opaque) FALSE;

      default:
	*status = XV_ERROR;
	return (Xv_opaque) 0;
    }
}

/***********************************************************************/
/* text_remove                                                              */
/***********************************************************************/

static void
text_remove(ip)
    register Item_info *ip;
{
    register Panel_info *panel = ip->panel;
    Text_info	   *dp = TEXT_FROM_ITEM(ip);
    short           had_caret_seln = FALSE;
    Panel_selection	*selection;

    if (dp->flags & PTXT_READ_ONLY)
	return;

    /*
     * cancel the selection if this item owns it.
     */
    if (panel->seln_client) {
	selection = panel_seln(panel, SELN_PRIMARY);
	if (selection->ip == ip) {
	    selection->ip = (Item_info *) 0;
	    seln_done(panel->seln_client, SELN_PRIMARY);
	}

	selection = panel_seln(panel, SELN_SECONDARY);
	if (selection->ip == ip) {
	    selection->ip = (Item_info *) 0;
	    seln_done(panel->seln_client, SELN_SECONDARY);
	}
    }
    if (panel_seln(panel, SELN_CARET)->ip == ip) {
	had_caret_seln = TRUE;
	panel_seln_cancel(panel, SELN_CARET);
    }

    /*
     * Only reassign the keyboard focus to another item if the panel isn't
     * being destroyed.
     */
    if (!panel->destroying && panel->kbd_focus_item == ip) {
	/*
	 * The caret is cleared, so don't paint the caret off. However, mark
	 * that no caret is on.
	 */
	panel->caret_on = FALSE;
	dp->flags &= ~HASCARET;
	panel->kbd_focus_item = panel_next_kbd_focus(panel);
	if (panel->kbd_focus_item) {
	    if (panel->kbd_focus_item->item_type == PANEL_TEXT_ITEM) {
		TEXT_FROM_ITEM(panel->kbd_focus_item)->flags |= HASCARET;
		if (had_caret_seln)
		    panel_seln(panel, SELN_CARET)->ip = panel->kbd_focus_item;
		paint_caret(panel->kbd_focus_item, TRUE);
	    } else {
		(*panel->kbd_focus_item->ops->accept_kbd_focus) (
		    panel->kbd_focus_item);
	    }
	}
    }
    return;
}


/*************************************************************************/
/* text_restore                                                               */
/* this code assumes that the caller has already set ip to be not hidden */
/*************************************************************************/

static void
text_restore(ip)
    register Item_info *ip;
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);

    if (dp->flags & PTXT_READ_ONLY || hidden(ip))
	return;

    /* see whether selection client is initialized */
    /* may not be if restore is called because of reparent */
    if (!ip->panel->seln_client)
	panel_seln_init(ip->panel);

    /* If this is the only item, give the caret to this item. */
    if (ip->panel->items == ip && !ip->next) {
	ip->panel->kbd_focus_item = ip;
	dp->flags |= HASCARET;
	/* If we have the caret selection, change the item.
	 * Note: the caret will be drawn when the item is painted.
	 */
	if (panel_seln(ip->panel, SELN_CARET)->ip)
	    panel_seln(ip->panel, SELN_CARET)->ip = ip;
    }
    return;
}

/***********************************************************************/
/* destroy                                                             */
/***********************************************************************/

Pkg_private int
text_destroy(item_public, status)
    Panel_item      item_public;
    Destroy_status  status;
{
    register Text_info *dp = TEXT_PRIVATE(item_public);

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF))
	return XV_OK;

    text_remove(ITEM_PRIVATE(item_public));

    free(dp->value);
#ifdef	OW_I18N
    free(dp->value_wc);
    free(dp->terminators_wc);
#endif	OW_I18N
    free(dp->terminators);
    free((char *) dp);

    return XV_OK;
}


static void
draw_scroll_btn(ip, state)
    register Item_info *ip;
    int		state;
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    Xv_Drawable_info *info;
    Xv_Window	    pw;

    PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	DRAWABLE_INFO_MACRO(pw, info);
	if (state & OLGX_SCROLL_BACKWARD) {
	    /* Draw left scrolling button */
	    olgx_draw_textscroll_button(ip->panel->ginfo, xv_xid(info),
		ip->value_rect.r_left,
		ip->value_rect.r_top +
		    (ip->value_rect.r_height - dp->scroll_btn_height) / 2,
		state);
	} else {
	    /* Draw right scrolling button */
	    olgx_draw_textscroll_button(ip->panel->ginfo, xv_xid(info),
		ip->value_rect.r_left + ip->value_rect.r_width -
		    (dp->scroll_btn_width - SCROLL_BTN_GAP),
		ip->value_rect.r_top +
		    (ip->value_rect.r_height - dp->scroll_btn_height) / 2,
		state);
	}
    PANEL_END_EACH_PAINT_WINDOW
}

	
Pkg_private void
panel_text_paint_label(ip)
    register Item_info *ip;
{
    Rect            text_label_rect;
    struct pr_size  image_size;
    int             image_width;

    text_label_rect = ip->label_rect;
    if (ip->label_width) {
	if (is_string(&ip->label)) {
#ifdef	OW_I18N
	    image_size = xv_pf_textwidth_wc(wslen(image_string_wc(&ip->label)),
			  image_font(&ip->label), image_string_wc(&ip->label));
#else
	    image_size = xv_pf_textwidth(strlen(image_string(&ip->label)),
			  image_font(&ip->label), image_string(&ip->label));
#endif	OW_I18N
	    image_width = image_size.x;
	} else
	    image_width = ((Pixrect *)image_svrim(&ip->label))->pr_width;
	text_label_rect.r_left += ip->label_rect.r_width - image_width;
    }
    panel_paint_image(ip->panel, &ip->label, &text_label_rect, inactive(ip),
		      ip->color_index);
}


/***********************************************************************/
/* text_paint                                                               */
/***********************************************************************/

static void
text_paint(ip)
    register Item_info *ip;
{
#ifdef	OW_I18N
    int		has_caret = ip->panel->kbd_focus_item == ip;
#endif	OW_I18N
    panel_text_paint_label(ip);
#ifdef  OW_I18N
    if (has_caret && ip->panel->interm_text && ip->panel->interm_text[0])
        ml_panel_display_interm(ip);
    else
#endif  OW_I18N
    paint_text(ip);
}


/***********************************************************************/
/* paint_text                                                          */
/***********************************************************************/

static void
paint_text(ip)
    Item_info      *ip;
{
    /* compute the caret position */
    update_value_offset(ip, 0, 0);
    update_caret_offset(ip, 0);

    paint_value(ip, PV_HIGHLIGHT);
}


/*
 * paint_value clears the value rect for ip and paints the string value
 * clipped to the left of the rect.
 */
static void
paint_value(ip, highlight)
    register Item_info *ip;
    int          highlight;
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    register Panel_info *panel = ip->panel;
    register int    x = ip->value_rect.r_left;
    register int    y = ip->value_rect.r_top;
    int             i, j, len;
    Xv_Drawable_info *info;
#ifdef	OW_I18N
    wchar_t        *str_wc;
#else
    u_char         *str;
#endif	OW_I18N
    Xv_Window       pw;

    /* Clear the caret and value rect.
     * Set the colors for the Clear and Set GC's.
     */
    PANEL_EACH_PAINT_WINDOW(panel, pw)
	DRAWABLE_INFO_MACRO(pw, info);
	openwin_check_gc_color(info, OPENWIN_SET_GC);
	panel_clear_pw_rect(pw, ip->value_rect);
    PANEL_END_EACH_PAINT_WINDOW
    if (panel->kbd_focus_item == ip)
	/* Since part of the caret may lie outside the value rect,
	 * we need to call paint_caret to make sure all of the
	 * caret has been removed.
	 */
	paint_caret(ip, FALSE);

    /* Get the actual characters which will be displayed */
    len = dp->last_char - dp->first_char + 2;
    if (len) {
#ifdef	OW_I18N
	str_wc = (wchar_t *) malloc(len * sizeof(wchar_t));
	for (j = 0, i = dp->first_char; i <= dp->last_char; i++, j++)
	    str_wc[j] = dp->value_wc[i];
	str_wc[len - 1] = (wchar_t)'\0';
#else
	str = (u_char *) malloc(len);
	for (j = 0, i = dp->first_char; i <= dp->last_char; i++, j++)
	    str[j] = dp->value[i];
	str[len - 1] = '\0';
#endif	OW_I18N

	/* Draw the left scrolling button if needed */
	if (dp->first_char)
	    draw_scroll_btn(ip, OLGX_SCROLL_BACKWARD);

	/* Draw the text */
	if (dp->first_char)
	    x += dp->scroll_btn_width;
	if (dp->mask == '\0') {	/* not masked */
	    PANEL_EACH_PAINT_WINDOW(panel, pw)
#ifdef	OW_I18N
		panel_paint_text(pw, panel->std_font_xid, ip->color_index,
				 x, y+dp->font_home, str_wc);
#else
		panel_paint_text(pw, panel->std_font_xid, ip->color_index,
				 x, y+dp->font_home, str);
#endif	OW_I18N
	    PANEL_END_EACH_PAINT_WINDOW
	} else {		/* masked */
	    wchar_t         *buf;
	    int             length, i;
	    length = dp->last_char - dp->first_char + 2;
	    buf = (wchar_t *) malloc(length * sizeof(wchar_t));
	    for (j = 0, i = dp->first_char; i <= dp->last_char; i++, j++)
		buf[j] = (wchar_t) dp->mask; buf[length - 1] = (wchar_t) '\0';

	    PANEL_EACH_PAINT_WINDOW(panel, pw)
		panel_paint_text(pw, panel->std_font_xid, ip->color_index,
				 x, y+dp->font_home, buf);
	    PANEL_END_EACH_PAINT_WINDOW
	    free(buf);
	}

	/* Draw the right scrolling button if needed */
#ifdef	OW_I18N
	if (dp->last_char < (wslen(dp->value_wc) - 1))
	    draw_scroll_btn(ip, OLGX_SCROLL_FORWARD);

	free(str_wc);
#else
	if (dp->last_char < (strlen(dp->value) - 1))
	    draw_scroll_btn(ip, OLGX_SCROLL_FORWARD);

	free(str);
#endif	OW_I18N

    }
    /* Underline the text (optional) */
    if (dp->flags & UNDERLINED) {
	y = rect_bottom(&ip->value_rect);
	if (ip->panel->three_d) {
	    /* 3D text ledge is 2 pixels high.  (2D is 1 pixel high.) */
	    y--;
	}
	PANEL_EACH_PAINT_WINDOW(panel, pw)
	    DRAWABLE_INFO_MACRO(pw, info);
	    olgx_draw_text_ledge(panel->ginfo, xv_xid(info),
	        ip->value_rect.r_left, y,
		ip->value_rect.r_width);
	PANEL_END_EACH_PAINT_WINDOW
    }
    if (highlight) {
	/* re-hilite if this is a selection item */
	if (ip == panel_seln(panel, SELN_PRIMARY)->ip &&
	    !panel_seln(panel, SELN_PRIMARY)->is_null)
	    panel_seln_hilite(panel_seln(panel, SELN_PRIMARY));
	if (ip == panel_seln(panel, SELN_SECONDARY)->ip &&
	    !panel_seln(panel, SELN_SECONDARY)->is_null)
	    panel_seln_hilite(panel_seln(panel, SELN_SECONDARY));
    }
#ifdef PAINT_BOX
    /*
     * The following is used to debug font positioning problems. The painted
     * characters should not go outside of the value rectangle.
     */
    xv_draw_rectangle(pw, ip->value_rect.r_left, ip->value_rect.r_top,
		      ip->value_rect.r_width, ip->value_rect.r_height,
		      LineSolid, color_op);
#endif				/* PAINT_BOX */

    if (panel->kbd_focus_item == ip)
	paint_caret(ip, TRUE);

    if (inactive(ip)) {
	Xv_Screen      screen;
	GC             *openwin_gc_list;
	DRAWABLE_INFO_MACRO(pw, info);
	screen = xv_screen(info);
	openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY );
	openwin_check_gc_color(info, OPENWIN_INACTIVE_GC);
	XFillRectangle(xv_display(info), xv_xid(info),
		       openwin_gc_list[OPENWIN_INACTIVE_GC],
		       ip->value_rect.r_left, ip->value_rect.r_top,
		       ip->value_rect.r_width, ip->value_rect.r_height);
    }
}


/***********************************************************************/
/* paint_caret                                                         */
/***********************************************************************/

static void
paint_caret(ip, on)
    Item_info      *ip;
    int             on;
{
    register Panel_info *panel = ip->panel;
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    Display	   *display;
    XGCValues	    gc_values;
    Xv_Drawable_info *info;
    int		    max_x;
    Rect           *r;
    int             painted_caret_offset;
    Xv_Window       pw;
    XID		    pw_xid;
    u_char	    str[2];
    int    	    x;
    int		    y;
    Xv_Screen      screen;
    GC             *openwin_gc_list;

    if ((on && (panel->caret_on || (dp->flags & SELECTING_TEXT))) ||
	(!on && !panel->caret_on) ||
	panel->caret == NULL)
	return;
    panel->caret_on = on;

    /* paint the caret after the offset & above descender */
    painted_caret_offset = dp->mask ? 0 : dp->caret_offset;
    x = ip->value_rect.r_left + painted_caret_offset - panel->caret_width/2;
    if (dp->first_char)
	x += dp->scroll_btn_width;
    y = ip->value_rect.r_top + dp->font_home;

    PANEL_EACH_PAINT_WINDOW(panel, pw)
	r = panel_viewable_rect(panel, pw);
	max_x = r->r_left + r->r_width;
	if (x <= max_x - panel->caret_width/2) {
	    DRAWABLE_INFO_MACRO(pw, info);
	    screen = xv_screen(info);
	    openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY );
	    display = xv_display(info);
	    pw_xid = xv_xid(info);
	    if (panel->caret_on) {
		/* Save pixels that will be overwritten by caret */
		XCopyArea(display, pw_xid, panel->caret_bg_pixmap,
			  openwin_gc_list[OPENWIN_SET_GC],
			  x, y - panel->caret_ascent,
			  panel->caret_width, panel->caret_height,
			  0, 0);
		/* Write caret in foreground color */
		gc_values.foreground = xv_fg(info);
		XChangeGC(display, openwin_gc_list[OPENWIN_GLYPH_GC],
			  GCForeground, &gc_values);
		str[0] = panel->caret;
		str[1] = NULL;
		XDrawString(display, pw_xid, openwin_gc_list[OPENWIN_GLYPH_GC],
			    x, y, str, 1);
	    } else {
		/* Restore pixels that were overwritten by caret */
		XCopyArea(display, panel->caret_bg_pixmap, pw_xid,
			  openwin_gc_list[OPENWIN_SET_GC],
			  0, 0, panel->caret_width, panel->caret_height,
			  x, y - panel->caret_ascent);
	    }
	}
    PANEL_END_EACH_PAINT_WINDOW
}

/*
 * This is to fix the flashing problem during dragging in primary selection
 * of panel text item. Instead of dehiliting the old selection and then
 * hiliting the new one (which is a continuation of the old one), just invert
 * the difference in selection.  That is either hiliting more if selection
 * has grown, or dehiliting if selection has shrinked.
 */
static void
panel_add_selection(panel, ip, rank)
    register Panel_info *panel;
    Seln_rank       rank;
    Item_info      *ip;
{
    register Panel_selection *selection;
    Text_info      *dp = TEXT_FROM_ITEM(ip);
    Rect            rect;
    int             seln_rank;	/* PRIMARY_SELN or SECONDARY_SELN */
    struct pr_size  size;
    /* left and right pos of the text to invert */
    int             diff_first, diff_last;

    if (!panel->seln_client)
	return;

    selection = panel_seln(panel, rank);
    /*
     * if we already own the selection, don't ask the service for it.
     */
    if (!(ip && selection->ip == ip)) {
	rank = seln_acquire(panel->seln_client, rank);
    }
    selection = panel_seln(panel, rank);

    /* record the selection */
    selection->ip = ip;

    rect = ip->value_rect;
    seln_rank = (selection->rank == SELN_PRIMARY) ? PRIMARY_SELN : SECONDARY_SELN;

    if (select_click_cnt[seln_rank] == 0)
	return;
    if (selection->rank != SELN_PRIMARY)
	return;
    if ((dp->seln_first[seln_rank] == primary_seln_first) &&
	(dp->seln_last[seln_rank] == primary_seln_last))
	return;			/* no change */

    if (dp->seln_first[seln_rank] == primary_seln_first) {
	if (dp->seln_last[seln_rank] > primary_seln_last) {
	    /* more at the end */
	    diff_first = primary_seln_last + 1;
	    diff_last = dp->seln_last[seln_rank];
	} else if (dp->seln_last[seln_rank] < primary_seln_last) {
	    /* less at the end */
	    diff_first = dp->seln_last[seln_rank] + 1;
	    diff_last = primary_seln_last;
	}
    } else if (dp->seln_last[seln_rank] == primary_seln_last) {
	if (dp->seln_first[seln_rank] > primary_seln_first) {
	    /* less at the beg */
	    diff_first = primary_seln_first;
	    diff_last = dp->seln_first[seln_rank] - 1;
	} else if (dp->seln_first[seln_rank] < primary_seln_first) {
	    /* more at the beg */
	    diff_first = dp->seln_first[seln_rank];
	    diff_last = primary_seln_first - 1;
	}
    }
    /* Highlight characters bounded by diff_first and diff_last */
    if (diff_first >= dp->first_char) {
#ifdef	OW_I18N
	size = xv_pf_textwidth((diff_first - dp->first_char),
			       ip->panel->std_font, &dp->value_wc[dp->first_char]);
#else
	size = xv_pf_textwidth((diff_first - dp->first_char),
			       ip->panel->std_font, &dp->value[dp->first_char]);
#endif	OW_I18N
	rect.r_left += size.x;
    }
    if (dp->first_char)
	rect.r_left += dp->scroll_btn_width;
#ifdef	OW_I18N
    size = xv_pf_textwidth_wc(diff_last - diff_first + 1,
			   ip->panel->std_font, &dp->value_wc[diff_first]);
#else
    size = xv_pf_textwidth(diff_last - diff_first + 1,
			   ip->panel->std_font, &dp->value[diff_first]);
#endif	OW_I18N
    rect.r_width = size.x;
    if (rect.r_width > dp->text_rect.r_width)
	rect.r_width = dp->text_rect.r_width;
    rect.r_height--;		/* don't disturb underlining */

    seln_highlight = HL_INVERT;
    panel_invert(ip->panel, &rect, ip->color_index);

    /* restore the rect */
    rect = ip->value_rect;
    /*
     * Update rect to be bounded by seln_first and seln_last this is
     * necessary so that primary_seln_rect is always the entire selection.
     */
    if (dp->seln_first[seln_rank] >= dp->first_char) {
#ifdef	OW_I18N
	size = xv_pf_textwidth_wc((dp->seln_first[seln_rank] - dp->first_char),
			   ip->panel->std_font, &dp->value_wc[dp->first_char]);
#else
	size = xv_pf_textwidth((dp->seln_first[seln_rank] - dp->first_char),
			       ip->panel->std_font, &dp->value[dp->first_char]);
#endif	OW_I18N
	rect.r_left += size.x;
    }
    if (dp->first_char)
	rect.r_left += dp->scroll_btn_width;
#ifdef	OW_I18N
    size = xv_pf_textwidth_wc(
	dp->seln_last[seln_rank] - dp->seln_first[seln_rank] + 1,
	ip->panel->std_font, &dp->value_wc[dp->seln_first[seln_rank]]);
#else
    size = xv_pf_textwidth(
	dp->seln_last[seln_rank] - dp->seln_first[seln_rank] + 1,
	ip->panel->std_font, &dp->value[dp->seln_first[seln_rank]]);
#endif	OW_I18N
    rect.r_width = size.x;
    if (rect.r_width > dp->text_rect.r_width)
	rect.r_width = dp->text_rect.r_width;
    rect.r_height--;		/* don't disturb underlining */

    /* update globals */
    primary_seln_panel = ip->panel;	/* save panel */
    primary_seln_rect = rect;	/* save rectangle coordinates */
    primary_seln_first = dp->seln_first[seln_rank];
    primary_seln_last = dp->seln_last[seln_rank];

}

/***********************************************************************/
/* ops vector routines                                                 */
/***********************************************************************/

/* ARGSUSED */
static void
text_begin_preview(ip, event)
    register Item_info *ip;
    Event          *event;
{
    Text_info      *dp = TEXT_FROM_ITEM(ip);
    register Panel_info *panel = ip->panel;
    Seln_holder     holder;
    int             is_null;
    int             ext_caret_offset;	/* new caret offset when adjusting to
					 * the right */
    int             save_caret_offset;	/* caret doesn't move in secondary
					 * selections */
    u_char          adjust_right;
    int		    event_offset;
    u_char          save_delete_pending;
#ifdef	OW_I18N
    wchar_t         save_char_wc;
#else
    u_char          save_char;
#endif	OW_I18N
    int             seln_rank;	/* selection rank: PRIMARY_SELN or
				 * SECONDARY_SELN */
    struct pr_size  size;
    int             dragging = 0;	/* to fix flashing bug */

    holder = panel_seln_inquire(SELN_UNSPECIFIED);
    seln_rank = (holder.rank == SELN_PRIMARY) ? PRIMARY_SELN : SECONDARY_SELN;

    /*
     * Kludge to fix flashing bug when wiping thru. If SELN_PRIMARY and
     * wiping thru text item, call panel_add_selection() to invert the
     * difference in the growing or shrinking selection.
     */
#ifdef	OW_I18N
    if (wslen(dp->value_wc) && (select_click_cnt[seln_rank] > 0) &&
	(holder.rank == SELN_PRIMARY) &&
	(event_action(event) == LOC_DRAG) &&
	(event_left_is_down(event)) &&
	(dp->flags & SELECTING_TEXT))
#else
    if (strlen(dp->value) && (select_click_cnt[seln_rank] > 0) &&
	(holder.rank == SELN_PRIMARY) &&
	(event_action(event) == LOC_DRAG) &&
	(event_left_is_down(event)) &&
	(dp->flags & SELECTING_TEXT))
#endif	OW_I18N
	/*
	 * dragging SELECT: select text, do panel_add_selection()
	 */
	dragging = 1;


    /* Ask for kbd focus if this is a primary selection */
    holder = panel_seln_inquire(SELN_UNSPECIFIED);
    if (holder.rank == SELN_PRIMARY)
	win_set_kbd_focus(PANEL_PUBLIC(panel),
			  (XID) xv_get(event_window(event), XV_XID));

    /* Check if one of the horizontal scrolling buttons was selected */
    event_offset = event->ie_locx - ip->value_rect.r_left;
    if (dp->first_char &&
	event_offset >= 0 &&
	event_offset < dp->scroll_btn_width) {
	if ((event_action(event) == ACTION_SELECT ||
	     (event_action(event) == LOC_DRAG && action_select_is_down(event)))
	    && !(dp->flags & LEFT_SCROLL_BTN_SELECTED)) {
	    /* SELECT-down on left scrolling button: invoke left button */
	    text_cancel_preview(ip, event);
	    draw_scroll_btn(ip,
			    OLGX_SCROLL_BACKWARD | OLGX_INVOKED | OLGX_ERASE);
	    dp->flags |= SELECTING_SCROLL_BTN | LEFT_SCROLL_BTN_SELECTED;
	} /* else ignore event */
	return;
#ifdef	OW_I18N
    } else if (event_offset > ip->value_rect.r_width -
		    dp->scroll_btn_width &&
	       event_offset < ip->value_rect.r_width &&
	       dp->last_char < wslen(dp->value_wc) - 1) {
#else
    } else if (event_offset > ip->value_rect.r_width -
		    dp->scroll_btn_width &&
	       event_offset < ip->value_rect.r_width &&
	       dp->last_char < strlen(dp->value) - 1) {
#endif	OW_I18N
	if ((event_action(event) == ACTION_SELECT ||
	     (event_action(event) == LOC_DRAG && action_select_is_down(event)))
	    && !(dp->flags & RIGHT_SCROLL_BTN_SELECTED)) {
	    /* SELECT-down on right scrolling button: invoke right button */
	    text_cancel_preview(ip, event);
	    draw_scroll_btn(ip,
			    OLGX_SCROLL_FORWARD | OLGX_INVOKED | OLGX_ERASE);
	    dp->flags |= SELECTING_SCROLL_BTN | RIGHT_SCROLL_BTN_SELECTED;
	} /* else ignore event */
	return;
    }

    /*
     * If we started out selecting a scrolling button, then don't
     * start selecting text.  Unhiglight any highlighted scrolling
     * button and return.
     */
    if (dp->flags & SELECTING_SCROLL_BTN) {
	if (dp->flags & LEFT_SCROLL_BTN_SELECTED) {
	    dp->flags &= ~LEFT_SCROLL_BTN_SELECTED;
	    draw_scroll_btn(ip, OLGX_SCROLL_BACKWARD | OLGX_ERASE);
	} else if (dp->flags & RIGHT_SCROLL_BTN_SELECTED) {
	    dp->flags &= ~RIGHT_SCROLL_BTN_SELECTED;
	    draw_scroll_btn(ip, OLGX_SCROLL_FORWARD | OLGX_ERASE);
	}
	return;
    }

    /*
     * If nothing is selected on the line being pointed to, then allow the
     * middle button to pick (i.e., make it look like the left button was
     * clicked).
     */
    save_delete_pending = delete_pending;
    if (((select_click_cnt[seln_rank] == 0) &&
	 (event_action(event) == ACTION_ADJUST))
	|| ((panel_seln(panel, holder.rank)->ip != ip)
	    && (event_action(event) == ACTION_ADJUST))) {
	event_set_action(event, ACTION_SELECT);
    }
    /*
     * Ignore the middle mouse button when the mouse is dragged to a line not
     * containing the caret.
     */
    if ((panel->kbd_focus_item != ip) &&
	((event_action(event) == LOC_DRAG) && event_middle_is_down(event))) {
	return;
    }
    /* Caret position can be set by mouse */
    save_caret_offset = dp->caret_offset;	/* in case of secondary
						 * select */

    /*
     * Define rectangle containing the text (i.e., value rect less arrows),
     * and caret offset within that rectangle.
     */
    update_text_rect(ip);

    /* Define caret_offset as offset within text rectangle */
    dp->caret_offset = event->ie_locx - dp->text_rect.r_left;
    if (dp->caret_offset < 0)
	dp->caret_offset = 0;	/* => leftmost pixel */
    if (dp->caret_offset > dp->text_rect.r_width)
	dp->caret_offset = dp->text_rect.r_width;	/* => rightmost pixel +
							 * 1 */

    if (event_action(event) == ACTION_SELECT) {
	/*
	 * This is a mouse left click event. Set seln_first and seln_last to
	 * the character pointed to by the caret.
	 */
#ifdef	OW_I18N
	if (wslen(dp->value_wc) == 0) {
#else
	if (strlen(dp->value) == 0) {
#endif	OW_I18N
	    dp->seln_first[seln_rank] = 0;
	    dp->seln_last[seln_rank] = -1;
	} else {
#ifdef	OW_I18N
	    save_char_wc = dp->value_wc[dp->last_char + 1];
	    dp->value_wc[dp->last_char + 1] = 0;
	    dp->seln_first[seln_rank] = dp->first_char;
	    dp->seln_first[seln_rank] += char_position(dp->caret_offset,
				      ip->panel->std_font,
				      &dp->value_wc[dp->first_char], TRUE);
	    dp->value_wc[dp->last_char + 1] = save_char_wc;
	    if (dp->seln_first[seln_rank] >= wslen(dp->value_wc))
		if ((dp->seln_first[seln_rank] = wslen(dp->value_wc) - 1) < 0)
		    dp->seln_first[seln_rank] = 0;
	    dp->seln_last[seln_rank] = dp->seln_first[seln_rank];
#else
	    save_char = dp->value[dp->last_char + 1];
	    dp->value[dp->last_char + 1] = 0;
	    dp->seln_first[seln_rank] = dp->first_char;
	    dp->seln_first[seln_rank] += char_position(dp->caret_offset,
				      ip->panel->std_font,
				      &dp->value[dp->first_char], TRUE);
	    dp->value[dp->last_char + 1] = save_char;
	    if (dp->seln_first[seln_rank] >= strlen(dp->value))
		if ((dp->seln_first[seln_rank] = strlen(dp->value) - 1) < 0)
		    dp->seln_first[seln_rank] = 0;
	    dp->seln_last[seln_rank] = dp->seln_first[seln_rank];
#endif	OW_I18N
	}
	if (event_action(event) == ACTION_SELECT) {
	    select_click_cnt[seln_rank]++;
	    /*
	     * If this is not a double click, or we've moved to another panel
	     * item, then reset mouse left click count to 1.
	     */
	    if ((!panel_dclick_is(&event_time(event))) ||
		(panel_seln(panel, holder.rank)->ip &&
		 panel_seln(panel, holder.rank)->ip != ip))
		select_click_cnt[seln_rank] = 1;
	}
	dp->flags &= ~SELECTING_TEXT;	/* assume we're not selecting text */
	if (select_click_cnt[seln_rank] > 1) {
	    /* Double or triple click */
	    if (!event_ctrl_is_down(event))
		dp->flags |= SELECTING_TEXT;
	    (void) panel_multiclick_handler(ip, event, seln_rank);
	} else if (event_ctrl_is_down(event) || event_action(event) == LOC_DRAG)
	    /* Single click with ctrl or drag */
	    dp->flags |= SELECTING_TEXT;

#ifdef	OW_I18N
    } else if (wslen(dp->value_wc) && (select_click_cnt[seln_rank] > 0) &&
	       ((event_action(event) == ACTION_ADJUST) ||
		((event_action(event) == LOC_DRAG) &&
	     (event_left_is_down(event) || event_middle_is_down(event))))) {
#else
    } else if (strlen(dp->value) && (select_click_cnt[seln_rank] > 0) &&
	       ((event_action(event) == ACTION_ADJUST) ||
		((event_action(event) == LOC_DRAG) &&
	     (event_left_is_down(event) || event_middle_is_down(event))))) {
#endif	OW_I18N

	/* Define extended selection point.
	 * Get the index of the character that the caret_offset is
	 * within.  Do not use the "balance beam" method.
	 */
#ifdef	OW_I18N
	dp->ext_first = char_position(dp->caret_offset,
				      ip->panel->std_font,
				      &dp->value_wc[dp->first_char], FALSE);
#else
	dp->ext_first = char_position(dp->caret_offset,
				      ip->panel->std_font,
				      &dp->value[dp->first_char], FALSE);
#endif	OW_I18N
	if (dp->first_char)
	    dp->ext_first += dp->first_char;
#ifdef	OW_I18N
	if (dp->ext_first >= wslen(dp->value_wc))
	    if ((dp->ext_first = wslen(dp->value_wc) - 1) < 0)
#else
	if (dp->ext_first >= strlen(dp->value))
	    if ((dp->ext_first = strlen(dp->value) - 1) < 0)
#endif	OW_I18N
		dp->ext_first = 0;
	dp->ext_last = dp->ext_first;

	if (select_click_cnt[seln_rank] >= 3) {
	    dp->ext_first = dp->seln_first[seln_rank];
	    dp->ext_last = dp->seln_last[seln_rank];
	    panel_select_line(ip, event, seln_rank); /* update caret offset */
	} else {
	    if (select_click_cnt[seln_rank] == 2) {
		panel_find_word(dp, &dp->ext_first, &dp->ext_last);
	    }
#ifdef	OW_I18N
	    size = xv_pf_textwidth_wc(dp->ext_last - dp->first_char + 1,
				   ip->panel->std_font,
				   &dp->value_wc[dp->first_char]);
#else
	    size = xv_pf_textwidth(dp->ext_last - dp->first_char + 1,
				   ip->panel->std_font,
				   &dp->value[dp->first_char]);
#endif	OW_I18N
	    ext_caret_offset = size.x;

	    /* Adjust first or last character selected */
	    if (dp->ext_last > dp->seln_last[seln_rank])
		adjust_right = TRUE;
	    else if (dp->ext_first < dp->seln_first[seln_rank])
		adjust_right = FALSE;	/* adjust left */
	    else if ((dp->ext_first - dp->seln_first[seln_rank]) <
		     (dp->seln_last[seln_rank] - dp->ext_first))
		adjust_right = FALSE;	/* adjust left */
	    else
		adjust_right = TRUE;
	    if (adjust_right) {	/* Note: caret must be to the right of the
				 * last selected character, due to a check
				 * made on this assumption in
				 * panel_seln_delete(). */
		dp->seln_last[seln_rank] = dp->ext_last;
		dp->caret_offset = ext_caret_offset;
	    } else {		/* adjust left */
		dp->seln_first[seln_rank] = dp->ext_first;
		if ((dp->flags & SELECTING_TEXT) == 0) {
		    /* First drag after SELECT-down: don't include character
		     * to the right of the caret.
		     */
		    dp->seln_last[seln_rank] = dp->seln_first[seln_rank];
		}
#ifdef	OW_I18N
		size = xv_pf_textwidth_wc(dp->ext_first - dp->first_char,
				       ip->panel->std_font,
				       &dp->value_wc[dp->first_char]);
#else
		size = xv_pf_textwidth(dp->ext_first - dp->first_char,
				       ip->panel->std_font,
				       &dp->value[dp->first_char]);
#endif	OW_I18N
		dp->caret_offset = size.x;
	    }
	}
	/*
	 * ADJUST or dragging SELECT: select text
	 */
	dp->flags |= SELECTING_TEXT;
    }

    delete_pending = (seln_rank == PRIMARY_SELN) &&
	(dp->flags & SELECTING_TEXT);
    update_caret_offset(ip, 0);

    if (dragging)
	panel_add_selection(panel, ip, holder.rank);
    else {

	/*
	 * Make this item the selection. Dehilite any current selection and
	 * acquire the {PRIMARY,SECONDARY} selection for this ip.  If we're
	 * selecting text, and there is text to be selected, then highlight
	 * the selected text.
	 */
#ifdef	OW_I18N
	is_null = !(dp->flags & SELECTING_TEXT) || wslen(dp->value_wc) == 0;
#else
	is_null = !(dp->flags & SELECTING_TEXT) || strlen(dp->value) == 0;
#endif	OW_I18N
	panel_seln_acquire(panel, ip, holder.rank, is_null);
    }

    /*
     * If we are getting the PRIMARY selection, make sure we have the CARET
     * selection also. If we are getting the SECONDARY selection, restore
     * dp->caret_offset since the insertion point (represented by the caret)
     * doesn't move during secondary selections, and restore delete_pending,
     * since it only applies to primary selections.
     */
    if (holder.rank == SELN_PRIMARY) {
	if (!(dp->flags & PTXT_READ_ONLY))
	    panel_seln_acquire(panel, ip, SELN_CARET, TRUE);
    } else if (holder.rank == SELN_SECONDARY) {
	dp->caret_offset = save_caret_offset;
	delete_pending = save_delete_pending;
    }
    dp->flags |= SELECTING_ITEM;
    dp->orig_caret = ip->panel->kbd_focus_item;
    if (!(dp->flags & PTXT_READ_ONLY) && holder.rank == SELN_PRIMARY)
	panel_set_kbd_focus(panel, ip);
}


static void
horizontal_scroll(ip, shift)
    register Item_info *ip;
    int             shift;	/* number of characters to shift value and
				 * caret: <0 for shift left, >0 for shift
				 * right. */
{
    update_value_offset(ip, 0, shift);
    update_caret_offset(ip, -shift);
    paint_value(ip, PV_HIGHLIGHT);
}



/***********************************************************************/
/* text_cancel_preview                                                      */
/***********************************************************************/

/* ARGSUSED */
static void
text_cancel_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    register Panel_info *panel = ip->panel;

    if (dp->flags & SELECTING_SCROLL_BTN) {
	dp->flags &= ~SELECTING_SCROLL_BTN;
	if (dp->flags & LEFT_SCROLL_BTN_SELECTED) {
	    dp->flags &= ~LEFT_SCROLL_BTN_SELECTED;
	    draw_scroll_btn(ip, OLGX_SCROLL_BACKWARD | OLGX_ERASE);
	} else if (dp->flags & RIGHT_SCROLL_BTN_SELECTED) {
	    dp->flags &= ~RIGHT_SCROLL_BTN_SELECTED;
	    draw_scroll_btn(ip, OLGX_SCROLL_FORWARD | OLGX_ERASE);
	}
    }
    if (dp->flags & SELECTING_ITEM) {
	panel_yield_kbd_focus(panel);
	if (panel->kbd_focus_item = dp->orig_caret) {
	    TEXT_FROM_ITEM(dp->orig_caret)->flags |= HASCARET;
	    dp->orig_caret = (Item_info *) NULL;
	}
	dp->flags &= ~SELECTING_ITEM;
    }
    if (ip == panel_seln(panel, SELN_PRIMARY)->ip)
	panel_seln_cancel(panel, SELN_PRIMARY);
    if (ip == panel_seln(panel, SELN_SECONDARY)->ip)
	panel_seln_cancel(panel, SELN_SECONDARY);
}


/***********************************************************************/
/* text_accept_preview                                                      */
/***********************************************************************/

/* ARGSUSED */
static void
text_accept_preview(ip, event)
    Item_info      *ip;
    Event          *event;
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    int		    event_offset;

    /* Check if one of the horizontal scrolling buttons was selected */
    dp->flags &= ~SELECTING_SCROLL_BTN;
    event_offset = event->ie_locx - ip->value_rect.r_left;
    if (event_action(event) == ACTION_SELECT) {
	if (dp->first_char &&
	    event_offset >= 0 &&
	    event_offset < dp->scroll_btn_width) {
	    /* User clicked on left scrolling button */
	    dp->flags &= ~LEFT_SCROLL_BTN_SELECTED;
	    horizontal_scroll(ip, -1);	/* scroll left */
#ifdef	OW_I18N
	} else if (event_offset > ip->value_rect.r_width -
			dp->scroll_btn_width &&
		   event_offset < ip->value_rect.r_width &&
		   dp->last_char < wslen(dp->value_wc) - 1) {
#else
	} else if (event_offset > ip->value_rect.r_width -
			dp->scroll_btn_width &&
		   event_offset < ip->value_rect.r_width &&
		   dp->last_char < strlen(dp->value) - 1) {
#endif	OW_I18N
	    /* User clicked on right scrolling button */
	    dp->flags &= ~RIGHT_SCROLL_BTN_SELECTED;
	    horizontal_scroll(ip, 1);	/* scroll right */
	}
    }

    if (!(dp->flags & SELECTING_ITEM))
	return;

    dp->orig_caret = (Item_info *) NULL;
    dp->flags &= ~SELECTING_ITEM;
    dp->flags &= ~SELECTING_TEXT;
    update_caret_offset(ip, 0);

}


#define CTRL_D_KEY	'\004'
#define CTRL_G_KEY	'\007'

/***********************************************************************/
/* text_accept_key                                                          */
/***********************************************************************/

static void
text_accept_key(ip, event)
    register Item_info *ip;
    register Event *event;
{
    Panel_info     *panel = ip->panel;
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    short           action = event_action(event);
    int             has_caret;
    int		    i;
    int             notify_desired = FALSE;
    Panel_setting   notify_rtn_code;
    int             ok_to_insert;
    Panel_selection *selection = panel_seln(panel, SELN_CARET);
#ifdef	OW_I18N
    wchar_t         *selection_string_wc;
    wchar_t         save_char_wc;
    char 	    *ie_string_mbs;
    wchar_t	    ie_string_wc;
    char	    tmp_char;
    wchar_t	    *tmp_char_wc;
    wchar_t	    *tmp_str_wc;
#endif	OW_I18N
    u_char         *selection_string;
    int             selection_length;
    u_char          save_char;

    if (event_is_iso(event) ||
	panel_event_is_panel_semantic(event) ||	/* erase or go char, word or
						 * line */
	(event_is_up(event) &&
	 ((action == ACTION_PASTE && select_click_cnt[SECONDARY_SELN]) ||
	  action == ACTION_CUT))) {
	if (delete_pending) {
	    for (i=0; erase_go_actions[i]; i++)
		if (erase_go_actions[i] == action)
		    break;
	    if (erase_go_actions[i]) {
		panel_seln_dehilite(ip, SELN_PRIMARY);
		delete_pending = FALSE;
	    } else {
		/* Copy the selection to the shelf */
		if (panel->shelf) {
		    free(panel->shelf);
		    panel->shelf = 0;
		}
#ifdef OW_I18N
		panel_get_text_selection(ip, &selection_string_wc,
					 &selection_length, SELN_PRIMARY);
		if (selection_string_wc) {
		    save_char_wc = *(selection_string_wc + selection_length);
		    *(selection_string_wc + selection_length) = 0;
		    panel->shelf = (char *) wcstombsdup (panel_strsave_wc(selection_string_wc));
		    *(selection_string_wc + selection_length) = save_char_wc;
#else
		panel_get_text_selection(ip, &selection_string,
					 &selection_length, SELN_PRIMARY);
		if (selection_string) {
		    save_char = *(selection_string + selection_length);
		    *(selection_string + selection_length) = 0;
		    panel->shelf = (char *) panel_strsave(selection_string);
		    *(selection_string + selection_length) = save_char;
#endif /* OW_I18N */
		    panel_seln_acquire(panel, (Item_info *) 0,
				       SELN_SHELF, TRUE);

		    /* Delete the selected characters */
		    panel_seln_delete(panel_seln(panel, SELN_PRIMARY));
		}
	    }
	}
	select_click_cnt[PRIMARY_SELN] = 0;
    }
    if (dp->flags & PTXT_READ_ONLY)
	return;

    dp->flags &= ~SELECTING_TEXT;

    /* get the caret selection */
    if (panel->seln_client && !selection->ip)
	if (event_is_down(event) &&
	    (event_is_iso(event) || action == ACTION_PASTE))
	    panel_seln_acquire(panel, ip, SELN_CARET, TRUE);

    /*
     * not interested in function keys, except for acquiring the caret
     * selection.
     */
    /*
     * also, never pass ctrl-D or ctrl-G, which have been mapped to the
     * Delete and Get function keys, to the notify proc
     */
#ifdef OBSOLETE
    if (panel_event_is_xview_semantic(event) ||
	event_id(event) == CTRL_D_KEY ||
	event_id(event) == CTRL_G_KEY)
	return;
#else
    if (panel_event_is_xview_semantic(event))
	return;
#endif				/* OBSOLETE */


    /* de-hilite */
    if (ip == panel_seln(panel, SELN_PRIMARY)->ip)
	panel_seln_dehilite(ip, SELN_PRIMARY);
    else if (ip == panel_seln(panel, SELN_SECONDARY)->ip)
	panel_seln_dehilite(ip, SELN_SECONDARY);

    switch (dp->notify_level) {
      case PANEL_ALL:
	notify_desired = TRUE;
	break;
#ifdef OW_I18N
      case PANEL_SPECIFIED:
	if (event_is_string(event)) {
	    tmp_str_wc = (wchar_t *) mbstowcsdup (event_string(event));
	    notify_desired = (((wchar_t *)wschr(dp->terminators_wc, 
				tmp_str_wc[0]) != 0) && 
				event_is_down(event));
	    free ((char *) tmp_str_wc);
	}
	else {
	   if (iswascii(action)) {
	      tmp_char = action;
	      tmp_char_wc = (wchar_t *) malloc(sizeof(wchar_t));
	      mbtowc(tmp_char_wc, &tmp_char, MB_CUR_MAX);
	      notify_desired = (((wchar_t *) wschr(dp->terminators_wc,
				      tmp_char_wc[0]) != 0) && 
				      event_is_down(event));
	      free ((char *) tmp_char_wc);
	   }
	}
	break;
#else
      case PANEL_SPECIFIED:
	notify_desired = (action <= ASCII_LAST &&
			  (index(dp->terminators, action) != 0) &&
			  event_is_down(event));
	break;
#endif /*  OW_I18N  */
      case PANEL_NON_PRINTABLE:
	notify_desired = !panel_printable_char(action);
	break;
      case PANEL_NONE:
	notify_desired = FALSE;
	break;
    }
    if (notify_desired) {
	notify_rtn_code = (Panel_setting)
	    (*ip->notify) (ITEM_PUBLIC(ip), event);
	ok_to_insert = notify_rtn_code == PANEL_INSERT;
    } else {
	/* editting characters are always ok to insert */
	ok_to_insert = (panel_printable_char(action) ||
			(action == ACTION_ERASE_CHAR_BACKWARD) ||
			(action == ACTION_ERASE_CHAR_FORWARD) ||
			(action == ACTION_ERASE_WORD_BACKWARD) ||
			(action == ACTION_ERASE_WORD_FORWARD) ||
			(action == ACTION_ERASE_LINE_BACKWARD) ||
			(action == ACTION_ERASE_LINE_END));
    }

    /* Process movement actions that could change lines */
    if (event_is_down(event)) {
	switch (action) {
	  case ACTION_GO_LINE_BACKWARD:
	    /* Go to the start of this or the previous line */
	    if (dp->first_char == 0 && dp->caret_offset == 0) {
		notify_rtn_code = PANEL_PREVIOUS;
		notify_desired = TRUE;
		ok_to_insert = FALSE;
	    }
	    break;
	  case ACTION_GO_LINE_END:
	    /* Go to the end of this or the next line */
	    if (dp->caret_offset < dp->value_offset)
		break;
	    /* else fall through to ACTION_GO_LINE_FORWARD */
	  case ACTION_GO_LINE_FORWARD:  /* Go to the start of the next line */
	  case ACTION_GO_COLUMN_FORWARD: /* down arrow */
	    notify_rtn_code = PANEL_NEXT;
	    notify_desired = TRUE;
	    ok_to_insert = FALSE;
	    break;
	  case ACTION_GO_COLUMN_BACKWARD: /* up arrow */
	    notify_rtn_code = PANEL_PREVIOUS;
	    notify_desired = TRUE;
	    ok_to_insert = FALSE;
	    break;
	  case ACTION_GO_CHAR_FORWARD:   /* right arrow */
	  case ACTION_GO_CHAR_BACKWARD:  /* left arrow */
	    notify_desired = FALSE;
	    ok_to_insert = FALSE;
	    break;
	  default:
	    break;
	}
    }

    /* If this item has the caret, then turn off the caret.
     * Note: has_caret is calculated after the text item's notify proc is
     *       called since the notify proc may change who has the caret.
     */
    has_caret = panel_has_input_focus(panel) && panel->kbd_focus_item == ip;
    if (has_caret)
	paint_caret(ip, FALSE);

    /*
     * Do something with the character.
     */
    if (event_is_down(event))
#ifdef	OW_I18N
	/* if the event is caused by committed string from cm, this
	 * ie_string is always non NULL.
	 */
	if (event_string(event)) {
		ie_string_mbs = event->ie_string;
	 	while(*ie_string_mbs) {
		    ie_string_mbs += mbtowc(&ie_string_wc, ie_string_mbs, MB_LEN_MAX);
		    update_value (ip, ie_string_wc, ok_to_insert);
		}
	}
	else
		update_value(ip, action, ok_to_insert);
#else
	update_value(ip, action, ok_to_insert);
#endif	OW_I18N

    if (has_caret)
	paint_caret(ip, TRUE);

    if (notify_desired && has_caret && event_is_down(event))
	switch (notify_rtn_code) {
	  case PANEL_NEXT:
	    (void) panel_advance_caret(PANEL_PUBLIC(panel));
	    break;

	  case PANEL_PREVIOUS:
	    (void) panel_backup_caret(PANEL_PUBLIC(panel));
	    break;

	  default:
	    break;
	}
}

/*
 * update_value updates the value of ip according to code.  If code is an
 * edit character, the appropriate edit action is taken. Otherwise, if
 * ok_to_insert is true, code is appended to ip->value.
 */
/* ???? OW_I18N: Did not take care of word movement yet ??? */
static void
update_value(ip, action, ok_to_insert)
    Item_info      *ip;
    register int    action;	/* event action code */
    int             ok_to_insert;
{
    register int    i;		/* counter */
    register Text_info *dp = TEXT_FROM_ITEM(ip);
#ifdef	OW_I18N
    register wchar_t *sp_wc;		/* string value */
#endif	OW_I18N
/* ??? OW_I18N: When we take care of word delim_table, delete sp. ??? */
    register u_char *sp;		/* string value */
    int		    ascent;
    int             caret_shift = 0;	/* number of positions to move caret */
    int             char_code;
    int		    descent;
    int             direction;
    Xv_Drawable_info *info;
    int             insert_pos;	/* position for character add/delete */
    int             j;
    int		    new_len;	/* new string length */
    int             orig_len;	/* original string length */
    int             orig_offset;/* before caret offset */
    int             orig_text_rect_width;  /* original text rectangle width */
    XCharStruct     overall;
    int		    pc_adv_x;
    Xv_Window       pw;
    Rect	    rect;
    struct pr_size  size;
    int             was_clipped;	/* TRUE if value was clipped */
    int             val_change = 0;	/* number of characters added (+) or
					 * deleted (-) */
    int             val_shift = 0;	/* number of characters to shift
					 * value display */
    int		    x;		/* left of insert/del point */
#ifdef	OW_I18N
    XFontSet        font_set;
    XRectangle	   	    overall_ink_extents, overall_logical_extents;
#else
    XFontStruct	   *x_font_info;
#endif	OW_I18N

#ifdef	OW_I18N
    wchar_t 	temp_char;
    char	space_char[2];

    strcpy(space_char," ");
    mbtowc(&temp_char, space_char, 1);
#endif	OW_I18N

    /* Define rectangle containing the text (i.e., value rect less arrows) */
    update_text_rect(ip);

    /* Get the insert position for character add/delete */
    if (dp->caret_offset == 0)
	insert_pos = dp->first_char;
    else
	insert_pos = dp->caret_position;

#ifdef	OW_I18N
    sp_wc = dp->value_wc;
    orig_len = wslen(sp_wc);
#else
    sp = dp->value;
    orig_len = strlen(sp);
#endif	OW_I18N

    if (action == ACTION_ERASE_CHAR_BACKWARD) {
	/* Allow notify_proc to override editting characters. */
	/* Supports read-only text fields. */
	if (!ok_to_insert) {
	    blink_value(ip);
	    return;
	}
	/* Nothing to backspace if caret is at left boundary. */
	if (dp->caret_offset == 0)
	    return;

	/* Can't show result of backspace if display length exceeded and */
	/* caret is to the right of the panel left arrow.  The moral here */
	/* is that you can't delete what you can't see. */
	if ((orig_len > dp->display_length) && (dp->first_char) && (dp->caret_offset == 0))
	    return;

#ifdef	OW_I18N
	if ((*sp_wc) && (insert_pos > 0)) {
	    for (i = insert_pos; i < orig_len; i++)
		sp_wc[i - 1] = sp_wc[i];
	    sp_wc[orig_len - 1] = (wchar_t)'\0';
#else
	if ((*sp) && (insert_pos > 0)) {
	    for (i = insert_pos; i < orig_len; i++)
		sp[i - 1] = sp[i];
	    sp[orig_len - 1] = '\0';
#endif	OW_I18N
	    insert_pos--;
	    caret_shift = -1;
	    val_change = -1;

	    /*
	     * If clipped at left boundary, leave caret alone. Characters
	     * will shift in from the left.
	     */
	    if (dp->first_char) {
		caret_shift = 0;
		dp->caret_position--;
	    }
	}
    } else if (action == ACTION_ERASE_CHAR_FORWARD) {

	/* Allow notify_proc to override editting characters. */
	/* Supports read-only text fields. */
	if (!ok_to_insert) {
	    blink_value(ip);
	    return;
	}
	/* Can't show result of forespace if display length exceeded and */
	/* caret is to the left of the panel right arrow.  The moral here */
	/* is that you can't delete what you can't see. */
	if (dp->caret_offset == dp->text_rect.r_width)
	    return;
#ifdef	OW_I18N
	size = xv_pf_textwidth_wc(dp->last_char - dp->first_char,
			       ip->panel->std_font,
			       &dp->value_wc[dp->first_char]);
#else
	size = xv_pf_textwidth(dp->last_char - dp->first_char,
			       ip->panel->std_font,
			       &dp->value[dp->first_char]);
#endif	OW_I18N
	if (dp->caret_offset > size.x)
	    return;

#ifdef	OW_I18N
	if ((*sp_wc) && (insert_pos >= 0)) {
	    for (i = insert_pos; i < orig_len; i++)
		sp_wc[i] = sp_wc[i + 1];
	    sp_wc[orig_len - 1] = (wchar_t)'\0';
#else
	if ((*sp) && (insert_pos >= 0)) {
	    for (i = insert_pos; i < orig_len; i++)
		sp[i] = sp[i + 1];
	    sp[orig_len - 1] = '\0';
#endif	OW_I18N
	    caret_shift = 0;
	    val_change = 0;
#ifdef	OW_I18N
	    if ((dp->last_char >= (wslen(sp_wc) - 1)) && (dp->last_char > 1)) {
#else
	    if ((dp->last_char >= (strlen(sp) - 1)) && (dp->last_char > 1)) {
#endif	OW_I18N
		val_change = -1;
		/*
		 * ???  Why was the following line put in here?  This causes
		 * a bug in deleting the next character when the last char is
		 * displayed and the first character is not.  ???
		 */
		/* if (dp->first_char > 2) caret_shift = 1; */
	    }
	}
    } else if (action == ACTION_ERASE_WORD_BACKWARD) {

	/*
	 * for(i = orig_len - 1; i >= 0 && sp[i] == ' '; i--); for(; i >= 0
	 * && sp[i] != ' '; i--); sp[i + 1] = '\0';
	 */
	/* Allow notify_proc to override editting characters. */
	/* Supports read-only text fields. */
	if (!ok_to_insert) {
	    blink_value(ip);
	    return;
	}
	/* skip back past blanks */
	if (insert_pos > orig_len)
	    insert_pos -= (dp->first_char - 1);
#ifdef	OW_I18N
	for (i = insert_pos - 1; (i >= 0) && (sp_wc[i] == temp_char); i--);
	for (; (i >= 0) && (sp_wc[i] != temp_char); i--);
#else
	for (i = insert_pos - 1; (i >= 0) && (sp[i] == ' '); i--);
	for (; (i >= 0) && (sp[i] != ' '); i--);
#endif	OW_I18N
	if (i < 0)
	    i = 0;
	if (i > 0)
	    i++;
	caret_shift = i - insert_pos;
	val_change = i - insert_pos;
	for (j = insert_pos; j <= orig_len; j++, i++)
#ifdef	OW_I18N
	    sp_wc[i] = sp_wc[j];
#else
	    sp[i] = sp[j];
#endif	OW_I18N
	insert_pos += caret_shift;
    } else if (action == ACTION_ERASE_WORD_FORWARD) {

	/* Allow notify_proc to override editting characters. */
	/* Supports read-only text fields. */
	if (!ok_to_insert) {
	    blink_value(ip);
	    return;
	}
	/* skip back past blanks */
#ifdef	OW_I18N
	for (i = insert_pos; (i < orig_len) && (sp_wc[i] == temp_char); i++);
	for (; (i < orig_len) && (sp_wc[i] != temp_char); i++);
#else
	for (i = insert_pos; (i < orig_len) && (sp[i] == ' '); i++);
	for (; (i < orig_len) && (sp[i] != ' '); i++);
#endif	OW_I18N
	if (i >= orig_len)
	    i = orig_len - 1;
	if (i < (orig_len - 1))
	    i--;
	caret_shift = 0;
	val_change = 0;
	for (j = insert_pos; i < orig_len; j++, i++)
#ifdef	OW_I18N
	    sp_wc[j] = sp_wc[i + 1];
	if ((dp->last_char >= (wslen(sp_wc) - 1)) && (dp->first_char > 1)) {
	    val_change = wslen(sp_wc) - 1 - dp->last_char;
#else
	    sp[j] = sp[i + 1];
	if ((dp->last_char >= (strlen(sp) - 1)) && (dp->first_char > 1)) {
	    val_change = strlen(sp) - 1 - dp->last_char;
#endif	OW_I18N
	    if (dp->last_char < (orig_len - 1))
		val_change--;
	    caret_shift = -val_change;
	}
    } else if (action == ACTION_ERASE_LINE_BACKWARD) {
	/* Allow notify_proc to override editting characters. */
	/* Supports read-only text fields. */
	if (!ok_to_insert) {
	    blink_value(ip);
	    return;
	}
	/* sp[0] = '\0'; */
	caret_shift = -insert_pos;
	val_change = -insert_pos;
	for (i = 0, j = insert_pos; j <= orig_len; i++, j++)
#ifdef	OW_I18N
	    sp_wc[i] = sp_wc[j];
#else
	    sp[i] = sp[j];
#endif	OW_I18N
	insert_pos = 0;
    } else if (action == ACTION_ERASE_LINE_END) {

	/* Allow notify_proc to override editting characters. */
	/* Supports read-only text fields. */
	if (!ok_to_insert) {
	    blink_value(ip);
	    return;
	}
	caret_shift = 0;
	val_change = 0;
#ifdef	OW_I18N
	sp_wc[insert_pos] = (wchar_t) '\0';
#else
	sp[insert_pos] = '\0';
#endif	OW_I18N
	if (dp->first_char > 1) {
#ifdef	OW_I18N
	    val_change = wslen(sp_wc) - 1 - dp->last_char;
#else
	    val_change = strlen(sp) - 1 - dp->last_char;
#endif	OW_I18N
	    if (dp->last_char < (orig_len - 1))
		val_change--;
	    caret_shift = -val_change;
	}
    } else if (action == ACTION_GO_CHAR_FORWARD) {
	caret_shift = 1;
	if (dp->last_char < orig_len - 1 &&
	    dp->caret_offset == dp->value_offset)
	    val_shift = 1;	/* display will include next char to right */
    } else if (action == ACTION_GO_CHAR_BACKWARD) {
	caret_shift = -1;
	if ((dp->first_char) && (dp->caret_offset == 0))
	    val_shift = -1;	/* display will include next char to left */
    } else if (action == ACTION_GO_WORD_BACKWARD) {
	/* Skip back to start of current or previous word */
	if (insert_pos > orig_len)
	    insert_pos -= (dp->first_char - 1);
	for (i = insert_pos - 1; i >= 0 && delim_table[sp[i]]; i--);
	for (; i >= 0 && !delim_table[sp_wc[i]]; i--);
	if (i < 0)
	    i = 0;
	if (i > 0)
	    i++;
	caret_shift = i - insert_pos;
	if (i < dp->first_char)
	    /* Shift value so that caret is at correct character */
	    val_shift = caret_shift;
    } else if (action == ACTION_GO_WORD_END && insert_pos < orig_len) {
	/* Skip forward to end of current or next word */
	for (i = insert_pos; i < orig_len && delim_table[sp[i]]; i++);
	for (; i < orig_len && !delim_table[sp[i]]; i++);
	caret_shift = i - insert_pos;
	if (i > dp->last_char)
	    /* Shift value so that caret is visible */
	    val_shift = i - dp->last_char;
    } else if (action == ACTION_GO_WORD_FORWARD && insert_pos < orig_len) {
	/* Skip forward to start of next word */
	for (i = insert_pos; i < orig_len && !delim_table[sp[i]]; i++);
	for (; i < orig_len && delim_table[sp[i]]; i++);
	caret_shift = i - insert_pos;
	if (i > dp->last_char)
	    /* Shift value so that caret is visible */
	    val_shift = i - dp->last_char;
    } else if (action == ACTION_GO_LINE_BACKWARD ||
	       action == ACTION_GO_LINE_FORWARD) {
	/* Note: For ACTION_GO_LINE_FORWARD, the caret has already been
	 * advanced to the next line.  We now want to position it at
	 * the beginning of this line.
	 */
	caret_shift = -insert_pos;
	if (dp->first_char)
	    val_shift = -dp->first_char;
    } else if (action == ACTION_GO_LINE_END) {
	caret_shift = orig_len - insert_pos;
	if (dp->last_char < orig_len - 1)
	    val_shift = orig_len - dp->last_char;
    }
	
    /* Since all events are now allowed to pass thru, need to explicitly */
    /* check for printable characters.  */
    else if (panel_printable_char(action)) {
	if (ok_to_insert) {	/* insert */
	    if (orig_len < dp->stored_length) {	/* there is room */
		char_code = action;
		for (i = orig_len; i > insert_pos; i--)
#ifdef	OW_I18N
		    sp_wc[i] = sp_wc[i - 1];
		sp_wc[insert_pos] = (wchar_t) char_code;
#else
		    sp[i] = sp[i - 1];
		sp[insert_pos] = (u_char) char_code;
#endif	OW_I18N
		caret_shift = 1;
		val_change = 1;
#ifdef	OW_I18N
		sp_wc[orig_len + 1] = (wchar_t)'\0';
#else
		sp[orig_len + 1] = '\0';
#endif	OW_I18N

	    } else		/* no more room */
		blink_value(ip);

	} else			/* must be read-only */
	    blink_value(ip);
    }
    /* determine the new caret offset */
    orig_offset = dp->value_offset;
    orig_text_rect_width = dp->text_rect.r_width;
    update_value_offset(ip, val_change, val_shift);
    dp->caret_offset = -1;	/* caret offset is no longer valid */
    update_caret_offset(ip, caret_shift);
    update_text_rect(ip);

    /* update the display */
    if (dp->mask == ' ')
	return;
    else if (dp->mask)
	paint_value(ip, PV_HIGHLIGHT);
    else {
	/* compute the position of the caret */
	x = dp->text_rect.r_left + dp->caret_offset;
#ifdef	OW_I18N
	new_len = wslen(sp_wc);
#else
	new_len = strlen(sp);
#endif	OW_I18N
	was_clipped = dp->first_char || dp->last_char < new_len - 1;
	/* erase deleted characters that were displayed */
	if (new_len < orig_len) {
	    /* repaint the whole value if needed */
	    if (was_clipped || dp->text_rect.r_width != orig_text_rect_width)
		paint_value(ip, PV_HIGHLIGHT);
	    else {
		/* clear the deleted characters and everything to the right */
		rect.r_left = x;
		rect.r_top = ip->value_rect.r_top;
		rect.r_width = orig_offset - dp->caret_offset;
		rect.r_height = ip->value_rect.r_height - 2*LINE_Y;
		panel_clear_rect(ip->panel, rect);
		PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
#ifdef	OW_I18N
		    panel_paint_text(pw, ip->panel->std_font_xid,
			ip->color_index, x,
			ip->value_rect.r_top + dp->font_home,
			&sp_wc[insert_pos]);
#else
		    panel_paint_text(pw, ip->panel->std_font_xid,
			ip->color_index, x,
			ip->value_rect.r_top + dp->font_home,
			&sp[insert_pos]);
#endif	OW_I18N
		PANEL_END_EACH_PAINT_WINDOW
	    }

	} else if (new_len > orig_len) {
#ifdef	OW_I18N
	    font_set = (XFontSet)xv_get(ip->panel->std_font, FONT_SET_ID);
#else
	    x_font_info = (XFontStruct *)xv_get(ip->panel->std_font, FONT_INFO);
#endif	OW_I18N
	    /* repaint the whole value if it doesn't fit */
	    if (was_clipped)
		paint_value(ip, PV_HIGHLIGHT);
	    else {
		/* write the new character to the left of the caret */
#ifdef	OW_I18N
		pc_adv_x = XwcTextEscapement(font_set, &sp_wc[insert_pos], 1);
		XwcTextExtents(font_set, &sp_wc[insert_pos],
			     wslen(&sp_wc[insert_pos]),
                             &overall_ink_extents, &overall_logical_extents);
#else
		if (x_font_info->per_char)  {
		    pc_adv_x = x_font_info->per_char[sp[insert_pos] -
			x_font_info->min_char_or_byte2].width;
		} else
		    pc_adv_x = x_font_info->min_bounds.width;
		XTextExtents(x_font_info, &sp[insert_pos],
			     strlen(&sp[insert_pos]),
			     &direction, &ascent, &descent, &overall);
#endif	OW_I18N
		PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
		    DRAWABLE_INFO_MACRO(pw, info);
#ifdef OW_I18N	    	
		    XClearArea(xv_display(info), xv_xid(info),
			x - pc_adv_x, ip->value_rect.r_top,
			overall_logical_extents.width, overall_logical_extents.height, False);	    
#else /* OW_I18N */
		    
		    XClearArea(xv_display(info), xv_xid(info),
			x - pc_adv_x, ip->value_rect.r_top,
			overall.width, ascent + descent, False);

#endif  /* OW_I18N */
#ifdef	OW_I18N
		    panel_paint_text(pw, ip->panel->std_font_xid,
			ip->color_index, x - pc_adv_x,
			ip->value_rect.r_top + dp->font_home,
			&sp_wc[insert_pos]);
#else
		    panel_paint_text(pw, ip->panel->std_font_xid,
			ip->color_index, x - pc_adv_x,
			ip->value_rect.r_top + dp->font_home,
			&sp[insert_pos]);
#endif	OW_I18N
		PANEL_END_EACH_PAINT_WINDOW
	    }
	} else
	    /* Cursor key causes display shift */
	if (val_shift)
	    paint_value(ip, PV_HIGHLIGHT);
    }
}


static void
update_value_offset(ip, val_change, val_shift)
    Item_info      *ip;
    int             val_change;	/* number of characters added (+) or deleted
				 * (-) */
    int             val_shift;	/* number of characters to shift value
				 * display */
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
#ifdef	OW_I18N
    XFontSet            font_set;
#else
    XFontStruct		*x_font_info;
#endif	OW_I18N
    int             full_len;
    struct pr_size  size;
    int             max_caret = ip->value_rect.r_width;
    int             i, x;
    int             max_width;

#ifdef	OW_I18N
    full_len = wslen(dp->value_wc);
    size = xv_pf_textwidth_wc(full_len, ip->panel->std_font, dp->value_wc);
    font_set = (XFontSet)xv_get(ip->panel->std_font, FONT_SET_ID);

    if (size.x <= max_caret) {
	size = xv_pf_textwidth_wc(full_len, ip->panel->std_font, dp->value_wc);
#else
    full_len = strlen(dp->value);
    size = xv_pf_textwidth(full_len, ip->panel->std_font, dp->value);
    x_font_info = (XFontStruct *)xv_get(ip->panel->std_font, FONT_INFO);

    if (size.x <= max_caret) {
	size = xv_pf_textwidth(full_len, ip->panel->std_font, dp->value);
#endif	OW_I18N

	dp->first_char = 0;
	dp->last_char = full_len - 1;
	dp->value_offset = size.x;

    } else {			/* there are more characters than can be
				 * displayed */

	if (val_change > 0) {
	    /* Add a character */

	    /*
	     * Inserted characters will always be visible and the caret is
	     * always positioned after the inserted character, unless the
	     * caret is already positioned after the last displayable
	     * character, in which case all the characters to the left of the
	     * inserted character are shifted to the left on the display.
	     */
	    if (dp->caret_position > dp->last_char) {
		/*
		 * We are appending characters to the end of the string.
		 * Compute first_char = first character that can be fully
		 * displayed when the current (just-typed) character is the
		 * last character displayed.
		 */
		x = 0;
		max_width = max_caret - dp->scroll_btn_width;
#ifdef	OW_I18N
		if (dp->caret_position < wslen(dp->value_wc) - 1)
#else
		if (dp->caret_position < strlen(dp->value) - 1)
#endif	OW_I18N
		    max_width -= dp->scroll_btn_width;
		for (i = dp->caret_position; x < max_width; i--)  {
#ifdef	OW_I18N
			x += XwcTextEscapement(font_set, &dp->value_wc[i], 1);
#else
		    if (x_font_info->per_char)  {
		        x += x_font_info->per_char[dp->value[i] -
			    x_font_info->min_char_or_byte2].width;
		    } else
		        x += x_font_info->min_bounds.width;
#endif	OW_I18N
                }
		dp->first_char = i + 2;
	    }
	} else if (val_change < 0) {
	    /* Delete 1 or more characters */
	    dp->first_char += val_change;
	    if (dp->first_char < 0)
		dp->first_char = 0;	/* no more clip at left */
	} else {
	    /* Shift the display */
	    dp->first_char += val_shift;
	}

	/*
	 * dp->last_char = the last character in the string, starting from
	 * first_char, that can be fully displayed within the rectangle
	 * reserved for the text value string.  If the last character in the
	 * string cannot be displayed, then recompute dp->last_char to
	 * accomodate the right arrow scroll button.
	 */
	if (dp->first_char)
	    max_caret -= dp->scroll_btn_width;
#ifdef	OW_I18N
	dp->last_char = char_position(max_caret, ip->panel->std_font,
	    &dp->value_wc[dp->first_char], TRUE) - 1 + dp->first_char;
	if (dp->last_char < wslen(dp->value_wc) - 1)
#else
	dp->last_char = char_position(max_caret, ip->panel->std_font,
	    &dp->value[dp->first_char], TRUE) - 1 + dp->first_char;
	if (dp->last_char < strlen(dp->value) - 1)
#endif	OW_I18N
	    /*
	     * Decrement dp->last_char until enough space has been made to
	     * draw the right arrow scroll button.
	     */
	    for (x = 0; x < dp->scroll_btn_width;)  {
#ifdef	OW_I18N
		x += XwcTextEscapement(font_set, &dp->value_wc[dp->last_char--], 1);
#else
		if (x_font_info->per_char)  {
		    x += x_font_info->per_char[dp->value[dp->last_char--] -
		        x_font_info->min_char_or_byte2].width;
		} else
		    x += x_font_info->min_bounds.width;
#endif	OW_I18N
            }

	/* Compute value offset */
#ifdef	OW_I18N
	size = xv_pf_textwidth_wc(dp->last_char - dp->first_char + 1,
			       ip->panel->std_font,
			       &dp->value_wc[dp->first_char]);
#else
	size = xv_pf_textwidth(dp->last_char - dp->first_char + 1,
			       ip->panel->std_font,
			       &dp->value[dp->first_char]);
#endif	SUNDAe
	dp->value_offset = size.x;
    }
}


/*
 * update_caret_offset computes the caret x offset for ip.
 */
static void
update_caret_offset(ip, caret_shift)
    Item_info      *ip;
    int             caret_shift;  /* char position delta from caret_position */
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    int             max_caret_pos;
    struct pr_size  size;

    if (caret_shift) {
	dp->caret_position += caret_shift;
	if (dp->caret_position < dp->first_char)
	    dp->caret_position = dp->first_char;
    } else if (dp->caret_offset >= 0)
#ifdef	OW_I18N
	dp->caret_position = char_position(dp->caret_offset,
	    ip->panel->std_font, &dp->value_wc[dp->first_char], TRUE) +
	    dp->first_char;
    max_caret_pos = wslen(dp->value_wc);
#else
	dp->caret_position = char_position(dp->caret_offset,
	    ip->panel->std_font, &dp->value[dp->first_char], TRUE) +
	    dp->first_char;
    max_caret_pos = strlen(dp->value);
#endif	OW_I18N
    if (dp->caret_position > max_caret_pos)
	dp->caret_position = max_caret_pos;
#ifdef	OW_I18N
    size = xv_pf_textwidth_wc(dp->caret_position - dp->first_char,
			   ip->panel->std_font, &dp->value_wc[dp->first_char]);
#else
    size = xv_pf_textwidth(dp->caret_position - dp->first_char,
			   ip->panel->std_font, &dp->value[dp->first_char]);
#endif	OW_I18N
    dp->caret_offset = size.x;

    /* Caret cannot exceed last character of value */
    if (dp->caret_offset > dp->value_offset)
	dp->caret_offset = dp->value_offset;
}


/*
 * blink_value blinks the value rect of ip.
 */
static void
blink_value(ip)
    Item_info      *ip;
{
    int             i;		/* counter */
    Rect            r;		/* laf */

    /* laf begin */
    r = ip->rect;
    r.r_height += BOX_Y;

    /* invert the value rect */
    (void) panel_invert(ip->panel, &ip->value_rect, ip->color_index);

    /* wait a while */
    for (i = 1; i < 5000; i++);

    /* un-invert the value rect */
    (void) panel_invert(ip->panel, &ip->value_rect, ip->color_index);
}


/***********************************************************************/
/* caret routines                              */
/***********************************************************************/

static void
text_accept_kbd_focus(ip)
    Item_info *ip;	/* text item with new keyboard focus */
{
    Text_info	*dp = TEXT_FROM_ITEM(ip);

    if (panel_seln(ip->panel, SELN_CARET)->ip)
	panel_seln(ip->panel, SELN_CARET)->ip = ip;
    dp->flags |= HASCARET;
    if (panel_painted(ip->panel))
	paint_caret(ip,
	    (dp->flags & SELECTING_ITEM && delete_pending) ? FALSE : TRUE);
}


static void
text_yield_kbd_focus(ip)
    Item_info *ip;	/* text item with old keyboard focus */
{
    TEXT_FROM_ITEM(ip)->flags &= ~HASCARET;
    if (panel_painted(ip->panel))
	paint_caret(ip, FALSE);
}


Pkg_private Panel_item
panel_set_kbd_focus(panel, ip)
    Panel_info     *panel;
    Item_info      *ip;
{
    if (ip == NULL || hidden(ip))
	return NULL;

    panel_yield_kbd_focus(panel);
    panel->kbd_focus_item = ip;
    panel_accept_kbd_focus(panel);
    return ITEM_PUBLIC(ip);
}


Xv_public       Panel_item
panel_advance_caret(panel_public)
    Panel           panel_public;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);

    if (!panel->kbd_focus_item)
	return (NULL);

    (void) panel_set_kbd_focus(panel, panel_next_kbd_focus(panel));

    return ITEM_PUBLIC(panel->kbd_focus_item);
}


Xv_public       Panel_item
panel_backup_caret(panel_public)
    Panel           panel_public;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);

    if (!panel->kbd_focus_item)
	return (NULL);

    (void) panel_set_kbd_focus(panel, panel_previous_kbd_focus(panel));

    return ITEM_PUBLIC(panel->kbd_focus_item);
}


/*
 * text_caret_invert inverts the type-in caret.
 */
static void
text_caret_invert(panel)
    register Panel_info *panel;
{
    if (!panel->kbd_focus_item)
	return;

    paint_caret(panel->kbd_focus_item, panel->caret_on ? FALSE : TRUE);
}


/*
 * panel_text_caret_on paints the type-in caret if on is true;
 * otherwise it restores the pixels underneath the caret.
 */
Pkg_private void
panel_text_caret_on(panel, on)
    Panel_info     *panel;
    int             on;
{
    if (!panel->kbd_focus_item)
	return;

    paint_caret(panel->kbd_focus_item, on);
}


/***********************************************************************/
/* panel_text_notify                                                   */
/***********************************************************************/

/* ARGSUSED */
Xv_public       Panel_setting
panel_text_notify(client_item, event)
    Panel_item      client_item;
    register Event *event;
{
    switch (event_action(event)) {
      case '\n':
      case '\r':
      case '\t':
	return (event_shift_is_down(event) ? PANEL_PREVIOUS : PANEL_NEXT);

	/* always insert editting characters */
      case ACTION_ERASE_CHAR_BACKWARD:
      case ACTION_ERASE_CHAR_FORWARD:
      case ACTION_ERASE_WORD_BACKWARD:
      case ACTION_ERASE_WORD_FORWARD:
      case ACTION_ERASE_LINE_BACKWARD:
      case ACTION_ERASE_LINE_END:
	return PANEL_INSERT;

      default:
	if (panel_printable_char(event_action(event)))
	    return PANEL_INSERT;
	else
	    return PANEL_NONE;
    }
}


/*
 * Hilite selection according to its rank.
 */
static void
text_seln_hilite(selection)
    register Panel_selection *selection;
{
    register Item_info *ip = selection->ip;
    Text_info      *dp = TEXT_FROM_ITEM(ip);
    Rect            rect;
    int             seln_rank;	/* selection rank: PRIMARY_SELN or
				 * SECONDARY_SELN */
    int             y;
    struct pr_size  size;
    Xv_Window       pw;
    Xv_Drawable_info *info;
    Xv_Screen      screen;
    GC             *openwin_gc_list;

    rect = ip->value_rect;
    seln_rank = (selection->rank == SELN_PRIMARY) ? PRIMARY_SELN :
	SECONDARY_SELN;

    if (select_click_cnt[seln_rank] == 0)
	return;

    /* Highlight characters bounded by seln_first and seln_last */
    if (dp->seln_first[seln_rank] > dp->first_char) {
#ifdef	OW_I18N
	size = xv_pf_textwidth_wc((dp->seln_first[seln_rank] - dp->first_char),
			       ip->panel->std_font, &dp->value_wc[dp->first_char]);
#else
	size = xv_pf_textwidth((dp->seln_first[seln_rank] - dp->first_char),
			       ip->panel->std_font, &dp->value[dp->first_char]);
#endif	OW_I18N
	rect.r_left += size.x;
    }
    if (dp->first_char)
	rect.r_left += dp->scroll_btn_width;
#ifdef	OW_I18N
    size = xv_pf_textwidth_wc(
	dp->seln_last[seln_rank] - dp->seln_first[seln_rank] + 1,
	ip->panel->std_font, &dp->value_wc[dp->seln_first[seln_rank]]);
#else
    size = xv_pf_textwidth(
	dp->seln_last[seln_rank] - dp->seln_first[seln_rank] + 1,
	ip->panel->std_font, &dp->value[dp->seln_first[seln_rank]]);
#endif	OW_I18N
    rect.r_width = size.x;
    if (rect.r_width > dp->text_rect.r_width)
	rect.r_width = dp->text_rect.r_width;
    rect.r_height--;		/* don't disturb underlining */

    switch (selection->rank) {
      case SELN_PRIMARY:
	primary_seln_panel = ip->panel;	/* save panel */
	primary_seln_rect = rect;	/* save rectangle coordinates */
	primary_seln_first = dp->seln_first[seln_rank];
	primary_seln_last = dp->seln_last[seln_rank];
	seln_highlight = HL_INVERT;
	panel_invert(ip->panel, &rect, ip->color_index);
	break;

      case SELN_SECONDARY:
	secondary_seln_panel = ip->panel;	/* save panel */
	secondary_seln_rect = rect;	/* save rectangle coordinates */
	secondary_seln_first = dp->seln_first[seln_rank];
	secondary_seln_last = dp->seln_last[seln_rank];
	if (quick_move(ip->panel)) {
	    seln_highlight = HL_STRIKE_THRU;
	    y = rect.r_top + (rect.r_height / 2);
	} else {
	    seln_highlight = HL_UNDERLINE;
	    y = rect_bottom(&rect);
	}
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    if (ip->color_index >= 0) {
		xv_vector(pw, rect.r_left, y, rect.r_left + rect.r_width - 1, y,
			  ip->color_index < 0 ? PIX_SET :
			      PIX_SRC | PIX_COLOR(ip->color_index),
			  0);
	    } else {
		DRAWABLE_INFO_MACRO(pw, info);
		screen = xv_screen(info);
		openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY );
		XDrawLine(xv_display(info), xv_xid(info),
			  openwin_gc_list[OPENWIN_SET_GC],
			  rect.r_left, y, rect.r_left + rect.r_width - 1, y);
	    }
	PANEL_END_EACH_PAINT_WINDOW
	break;
    }
}


/* Dehighlight whatever was last highlighted */
Pkg_private void
panel_seln_dehilite(ip, rank)
    Item_info      *ip;
    Seln_rank       rank;
{
    Panel_info     *seln_panel = (Panel_info *) 0;
    Rect           *seln_rect_handle;
    Text_info      *dp = TEXT_FROM_ITEM(ip);
    int             seln_first, seln_last;
#ifdef	OW_I18N
    wchar_t         save_seln_last_char;
#else
    u_char          save_seln_last_char;
#endif	OW_I18N
    Xv_Window       pw;

    switch (rank) {
      case SELN_PRIMARY:
	seln_panel = primary_seln_panel;
	seln_rect_handle = &primary_seln_rect;
	seln_first = primary_seln_first;
	seln_last = primary_seln_last;
	primary_seln_panel = 0;	/* no longer valid */
	break;
      case SELN_SECONDARY:
	seln_panel = secondary_seln_panel;
	seln_rect_handle = &secondary_seln_rect;
	seln_first = secondary_seln_first;
	seln_last = secondary_seln_last;
	secondary_seln_panel = 0;	/* no longer valid */
	break;
    }
    if (seln_panel) {
	if (seln_highlight == HL_INVERT)
	    panel_invert(seln_panel, seln_rect_handle, ip->color_index);
	else if (ip && (seln_highlight == HL_UNDERLINE ||
			seln_highlight == HL_STRIKE_THRU)) {
	    if ((seln_first >= dp->first_char) &&
		(seln_last - seln_first + 1 <= dp->display_length)) {
		/* ??? variable-width * ??? */
		/* Entire selection is visible */
		panel_clear_rect(ip->panel, *seln_rect_handle);
#ifdef	OW_I18N
		save_seln_last_char = dp->value_wc[seln_last + 1];
		dp->value_wc[seln_last + 1] = 0;	/* terminate substring */
		PANEL_EACH_PAINT_WINDOW(seln_panel, pw)
		    panel_paint_text(pw, ip->panel->std_font_xid,
			ip->color_index,
			seln_rect_handle->r_left,
			seln_rect_handle->r_top + dp->font_home,
			dp->value_wc + seln_first);
		PANEL_END_EACH_PAINT_WINDOW
		    dp->value_wc[seln_last + 1] = save_seln_last_char;
#else
		save_seln_last_char = dp->value[seln_last + 1];
		dp->value[seln_last + 1] = 0;	/* terminate substring */
		PANEL_EACH_PAINT_WINDOW(seln_panel, pw)
		    panel_paint_text(pw, ip->panel->std_font_xid,
			ip->color_index,
			seln_rect_handle->r_left,
			seln_rect_handle->r_top + dp->font_home,
			dp->value + seln_first);
		PANEL_END_EACH_PAINT_WINDOW
		    dp->value[seln_last + 1] = save_seln_last_char;
#endif	OW_I18N
	    } else
		paint_value(ip, PV_NO_HIGHLIGHT);
	}
	if (seln_highlight != HL_NONE)
	    seln_highlight = HL_NONE;
    }
}


static void
panel_multiclick_handler(ip, event, seln_rank)
    Item_info      *ip;
    Event          *event;
    int             seln_rank;	/* selection rank: PRIMARY_SELN or
				 * SECONDARY_SELN */
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    int             left, right;/* left and right pixel coordinates of
				 * selection */
    struct pr_size  size;

    if (select_click_cnt[seln_rank] == 2) {
	/* Double click received: select displayed word containing caret */
	panel_find_word(dp, &dp->seln_first[seln_rank],
			&dp->seln_last[seln_rank]);
	/*
	 * Set caret offset to end of selected word or line closest to mouse
	 * position.
	 */
#ifdef	OW_I18N
	size = xv_pf_textwidth_wc(dp->seln_first[seln_rank] - dp->first_char,
			       ip->panel->std_font, &dp->value_wc[dp->first_char]);
#else
	size = xv_pf_textwidth(dp->seln_first[seln_rank] - dp->first_char,
			       ip->panel->std_font, &dp->value[dp->first_char]);
#endif	OW_I18N
	left = ip->value_rect.r_left + size.x;
	if (dp->first_char)
	    left += dp->scroll_btn_width;
#ifdef	OW_I18N
	size = xv_pf_textwidth_wc(dp->seln_last[seln_rank] -
			       dp->seln_first[seln_rank] + 1,
			       ip->panel->std_font,
			       &dp->value_wc[dp->seln_first[seln_rank]]);
#else
	size = xv_pf_textwidth(dp->seln_last[seln_rank] -
			       dp->seln_first[seln_rank] + 1,
			       ip->panel->std_font,
			       &dp->value[dp->seln_first[seln_rank]]);
#endif	OW_I18N
	right = left + size.x;
	if ((event->ie_locx - left) < (right - event->ie_locx))
	    event->ie_locx = left;	/* caret will move to left side */
	else
	    event->ie_locx = right;	/* caret will move to right side */
	dp->caret_offset = event->ie_locx - ip->value_rect.r_left;
	if (dp->first_char)
	    dp->caret_offset -= dp->scroll_btn_width;
    } else {
	/*
	 * Triple click received: select entire line. Repaint the value, with
	 * the first or last character displayed, if the mouse is closer to
	 * the left or right margin, respectively.
	 */
	panel_select_line(ip, event, seln_rank);
	update_text_rect(ip);
	paint_value(ip, PV_HIGHLIGHT);
    }

}


static void
panel_select_line(ip, event, seln_rank)
    Item_info      *ip;
    Event          *event;	/* NULL => position caret at end of line */
    int             seln_rank;	/* selection rank: PRIMARY_SELN or
				 * SECONDARY_SELN */
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    int             max_caret = dp->text_rect.r_width;
    int             right;	/* horizontal offset of right margin of
				 * selection */
    int             left_offset;/* horizontal offset of cursor from left
				 * margin of text value */
    int             right_offset;	/* horizontal offset of cursor from
					 * right margin of text value */
    int             x, i;
    struct pr_size  size;

    dp->first_char = dp->seln_first[seln_rank] = 0;
#ifdef	OW_I18N
    dp->last_char = dp->seln_last[seln_rank] = wslen(dp->value_wc) - 1;
    right = dp->text_rect.r_width;
    size = xv_pf_textwidth_wc(wslen(dp->value_wc), ip->panel->std_font, dp->value_wc);
#else
    dp->last_char = dp->seln_last[seln_rank] = strlen(dp->value) - 1;
    right = dp->text_rect.r_width;
    size = xv_pf_textwidth(strlen(dp->value), ip->panel->std_font, dp->value);
#endif	OW_I18N
    if (size.x < right)
	right = size.x;
    if (event) {
	left_offset = event_x(event) - dp->text_rect.r_left;
	right_offset = right + dp->text_rect.r_left - event_x(event);
    }
    if (event && left_offset < right_offset) {
	/*
	 * Repaint (later) with first character displayed. dp->last_char =
	 * the last character in the string, starting from first_char, that
	 * can be fully displayed within the rectangle reserved for the text
	 * value string.
	 */
	dp->caret_offset = 0;
#ifdef	OW_I18N
	dp->last_char = char_position(right, ip->panel->std_font, dp->value_wc,
	    TRUE) - 1;
#else
	dp->last_char = char_position(right, ip->panel->std_font, dp->value,
	    TRUE) - 1;
#endif	OW_I18N
	update_value_offset(ip, 0, 0);  /* fix dp->last_char to account for
					   scrolling button */
    } else {
	XFontStruct	*x_font_info;

	x_font_info = (XFontStruct *)xv_get(ip->panel->std_font, FONT_INFO);
	/* Repaint (later) with last character displayed */
	x = 0;
	for (i = dp->last_char; (i >= 0) && (x < max_caret); i--)  {
		if (x_font_info->per_char)  {
#ifdef	OW_I18N
		    /* ??? OW_I18N: For the time, use max_bounds 
		    x += x_font_info->per_char[dp->value_wc[i] -
		        x_font_info->min_char_or_byte2].width;
		    */
		    x += x_font_info->max_bounds.width;
#else
		    x += x_font_info->per_char[dp->value[i] -
		        x_font_info->min_char_or_byte2].width;
#endif	OW_I18N
		} else
		    x += x_font_info->min_bounds.width;
        }
	    if (i >= 0)
		dp->first_char = i + 2;
	    dp->caret_offset = ip->value_rect.r_width;
	    if (dp->first_char)
		dp->caret_offset -= dp->scroll_btn_width;
	    /* Caret cannot exceed last character of value */
	    if (dp->caret_offset > dp->value_offset)
		dp->caret_offset = dp->value_offset;
	}
    }

#ifdef OW_I18N
    static void
    panel_find_word(dp, first, last)
	register Text_info *dp;
	int            *first, *last;
    {
	register int    index;
	int		wc_type;

	/*
	 * When the selected character is in the only ASCII set, then
	 * go through the original algorithm
	 */
	wc_type = wchar_type(&dp->value_wc[*first]);
	if (wc_type == 1) {
	    /* Find beginning of word */
	    index = *first;
	    while ((index > dp->first_char) && 
		(wchar_type(&dp->value_wc[index]) == wc_type )) {
		if (!delim_table[dp->value_wc[index]])
		    index--;
	    }
	    if ((index != *first) &&
		(wchar_type(&dp->value_wc[index]) != wc_type ))
		index++;/* don't include characters from another codeset */
	    else if ((index != *first) && 
		wchar_type(&dp->value_wc[index]) == wc_type && 
		delim_table[dp->value_wc[index]])
		index++;
	    *first = index;

	    /*
	     * Find end of word.  Note that on a single click, seln_last is set equal
	     * to seln_first.
	     */
	    index = *last;
	    while ((index < dp->last_char) && 
		(wchar_type(&dp->value_wc[index]) == wc_type )) {
		if (!delim_table[dp->value_wc[index]])
		    index++;
	    }
	    if ((index != *last) && 
			(wchar_type(&dp->value_wc[index]) != wc_type ))
		index--; /* don't include characters from another codeset */
	    else if ((index != *last) &&
		wchar_type(&dp->value_wc[index]) == wc_type &&
		delim_table[dp->value_wc[index]])
		index--;	/* don't include word delimiter */
	    *last = index;
	}
	else {
	    /* Find beginning of word */
	    index = *first;
	    while ((index > dp->first_char) && 
			(wchar_type(&dp->value_wc[index])) == wc_type)
		index--;
	    if ((index != *first) && 
			(wchar_type(&dp->value_wc[index])) != wc_type)
		index++;
	    *first = index;

	    /*
	     * Find end of word. 
	     */
	    index = *last;
	    while ((index < dp->last_char) && wchar_type(&dp->value_wc[index]) == wc_type)
		index++;
	    if ((index != *last) && wchar_type(&dp->value_wc[index]) != wc_type)
		index--;
	    *last = index;
	}
    }
#else
    static void
    panel_find_word(dp, first, last)
	register Text_info *dp;
	int            *first, *last;
    {
	register int    index;

	/* Find beginning of word */
	index = *first;
	while ((index > dp->first_char) && !delim_table[dp->value[index]])
	    index--;
	if ((index != *first) && delim_table[dp->value[index]])
	    index++;		/* don't include word delimiter */
	*first = index;

	/*
	 * Find end of word.  Note that on a single click, seln_last is set equal
	 * to seln_first.
	 */
	index = *last;
	while ((index < dp->last_char) && !delim_table[dp->value[index]])
	    index++;
	if ((index != *last) && delim_table[dp->value[index]])
	    index--;		/* don't include word delimiter */
	*last = index;
    }
#endif /* OW_I18N */


    Pkg_private
    panel_get_text_selection(ip, selection_string_wc, selection_length, rank)
	Item_info      *ip;
#ifdef OW_I18N
	wchar_t	      **selection_string_wc;
#else
	u_char        **selection_string;
#endif /* OW_I18N */
	int            *selection_length;
	Seln_rank       rank;
    {
	register Text_info *dp = TEXT_FROM_ITEM(ip);
	int             seln_rank;	/* selection rank: PRIMARY_SELN or
				     * SECONDARY_SELN */

	seln_rank = (rank == SELN_PRIMARY) ? PRIMARY_SELN : SECONDARY_SELN;
#ifdef OW_I18N
	if ((wslen(dp->value_wc) == 0) || (select_click_cnt[seln_rank] == 0)) {
	    *selection_string_wc = null_string_wc;
#else
	if ((strlen(dp->value) == 0) || (select_click_cnt[seln_rank] == 0)) {
	    *selection_string = 0;
#endif /* OW_I18N */
	    *selection_length = 0;
	    return;
	}
	*selection_length = dp->seln_last[seln_rank] -
	    dp->seln_first[seln_rank] + 1;
#ifdef OW_I18N
	*selection_string_wc = dp->value_wc + dp->seln_first[seln_rank];
#else
	*selection_string = dp->value + dp->seln_first[seln_rank];
#endif /* OW_I18N */
    }


    Pkg_private void
    panel_seln_delete(selection)
	Panel_selection *selection;
    {
	Item_info      *ip = selection->ip;
	register Text_info *dp;
	register int    new;	/* new position of char to be moved */
	register int    old;	/* old position of char to be moved */
	int             last;	/* position of last valid char in value */
	int             val_change = 0;
	int             val_shift = 0;
	int             caret_shift = 0;
	int             seln_rank;	/* selection rank: PRIMARY_SELN or
				     * SECONDARY_SELN */
	struct pr_size  size;

	if (!ip)
	    return;
	dp = TEXT_FROM_ITEM(ip);

	seln_rank = (selection->rank == SELN_PRIMARY) ?
	    PRIMARY_SELN : SECONDARY_SELN;
	if (seln_rank == PRIMARY_SELN)
	    delete_pending = FALSE;

	if (dp->flags & PTXT_READ_ONLY)
	    return;

	/*
	 * Calculate number of character positions to move displayed value
	 * (val_change) and number of character positions to move caret
	 * (caret_shift).
	 */
	val_change = dp->seln_first[seln_rank] - dp->seln_last[seln_rank] - 1;
#ifdef	OW_I18N
	size = xv_pf_textwidth_wc(dp->seln_last[seln_rank] - dp->first_char + 1,
			   ip->panel->std_font, &dp->value_wc[dp->first_char]);
#else
	size = xv_pf_textwidth(dp->seln_last[seln_rank] - dp->first_char + 1,
			       ip->panel->std_font, &dp->value[dp->first_char]);
#endif	OW_I18N
	if (dp->caret_offset == size.x)	/* Is caret at right margin of
					     * selection? */
	    caret_shift = val_change;	/* Yes: shift caret to left margin of
					     * selection */
	if (dp->first_char)
	    caret_shift += ((dp->first_char - 1) > -val_change) ?
		(-val_change) : (dp->first_char - 1);

	/* Delete the selected characters from the value buffer */
	new = dp->seln_first[seln_rank];
	old = dp->seln_last[seln_rank] + 1;
#ifdef	OW_I18N
	last = wslen(dp->value_wc);
	for (; new <= dp->stored_length - 1; new++, old++) {
	    if (old > last)
		dp->value_wc[new] = 0;
	    else
		dp->value_wc[new] = dp->value_wc[old];
	}
#else
	last = strlen(dp->value);
	for (; new <= dp->stored_length - 1; new++, old++) {
	    if (old > last)
		dp->value[new] = 0;
	    else
		dp->value[new] = dp->value[old];
	}
#endif	OW_I18N

	/*
	 * Selection has been "used up": no mouse-left clicks or primary
	 * selection pending.
	 */
	select_click_cnt[seln_rank] = 0;
	if (selection->rank == SELN_PRIMARY)
	    primary_seln_panel = 0;
	else
	    secondary_seln_panel = 0;

	/* Repaint the value */
	update_value_offset(ip, val_change, val_shift);
	paint_value(ip, PV_HIGHLIGHT);

	if (ip->panel->kbd_focus_item == ip) {
	    /* Repaint the caret */
	    paint_caret(ip, FALSE);
	    update_caret_offset(ip, caret_shift);
	    paint_caret(ip, TRUE);
	}
    }
    /*
     * OW_I18N: for the time just return true for codeset 1 
     *	       and regular printable ascii code.
     */
    static int
    panel_printable_char(code)
	int             code;	/* event action code */
    {
	return((code >= ' ' && code <= '~')
		|| (code >= 0xA0 && code <= 0xFF)
#ifdef	OW_I18N
		|| (code & 0x8080)
		|| ((code & 0x60000000) == 0x60000000)
		|| ((code & 0x20000000) == 0x20000000)
		|| ((code & 0x40000000) == 0x40000000)

#endif	OW_I18N
	      );
    }


    /*
     * return TRUE if tv is within multi-click timeout from global
     * last_click_time.
     */
    static int
    panel_dclick_is(tv)
	struct timeval *tv;
    {
	int             delta;

	delta = (tv->tv_sec - last_click_time.tv_sec) * 1000;
	delta += tv->tv_usec / 1000;
	delta -= last_click_time.tv_usec / 1000;
	last_click_time = *tv;
	return (delta <= multi_click_timeout);
    }


    /* Find the position of the character to the left of caret_offset */
    static int
#ifdef	OW_I18N
    char_position(caret_offset, font, str_wc, balance_beam)
#else
    char_position(caret_offset, font, str, balance_beam)
#endif	OW_I18N
	int             caret_offset;
	Pixfont        *font;
#ifdef	OW_I18N
	wchar_t        *str_wc;
#else
	u_char         *str;
#endif	OW_I18N
	int		    balance_beam;  /* TRUE = return index of char to the
					* right if caret_offset is in the
					* right half of the character.
					* FALSE = return index of char the
					* caret offset is within.
					*/
    {
#ifdef	OW_I18N
	XFontSet                font_set;
#else
	XFontStruct		*x_font_info;
#endif	OW_I18N
	int             i;		/* character string index */
	int             x = 0;	/* desired horizontal position */
	int             x_new = 0;	/* next horizontal position */

#ifdef	OW_I18N
	font_set = (XFontSet)xv_get(font, FONT_SET_ID);
	for (i = 0; x_new <= caret_offset && str_wc[i]; i++) {
#else
	x_font_info = (XFontStruct *)xv_get(font, FONT_INFO);
	for (i = 0; x_new <= caret_offset && str[i]; i++) {
#endif	OW_I18N
	    x = x_new;
#ifdef	OW_I18N
	    x_new += XwcTextEscapement(font_set, &str_wc[i], 1);
#else
	    if (x_font_info->per_char)  {
	        x_new += x_font_info->per_char[str[i] -
	        x_font_info->min_char_or_byte2].width;
	    } else
	        x_new += x_font_info->min_bounds.width;
#endif	OW_I18N
    }

    /* Return character string index */
    if (x_new <= caret_offset)
	return (i);		/* cursor points to the right of the last
				 * character */
    else if (--i < 0)
	return (0);		/* cursor points to the left of the first
				 * character */
    else if (balance_beam && (caret_offset - x) > (x_new - x) / 2)
	return (i + 1);		/* cursor is in right half of char; point to
				 * next char */
    else
	return (i);		/* cursor is in left half of char, or not
				 * using "balance beam" method; point to
				 * this char */
}


static void
panel_text_handle_event(client_object, event)
    Panel_item      client_object;
    register Event *event;
{
    Item_info      *ip = ITEM_PRIVATE(client_object);
    Xv_Drawable_info *info;
    int		    take_down_caret;

    DRAWABLE_INFO_MACRO(ip->panel->public_self, info);
    take_down_caret = event_action(event) != ACTION_MENU &&
	!server_get_seln_function_pending(xv_server(info));
    if (take_down_caret && ip->panel->kbd_focus_item) {
	/* turn caret off */
	paint_caret(ip->panel->kbd_focus_item, FALSE);
    }

    update_text_rect(ip);
    panel_default_handle_event(client_object, event);

    /* Note: The panel item with the keyboard focus may have changed. */
    if (take_down_caret && ip->panel->kbd_focus_item &&
	ip->panel->kbd_focus_item->item_type == PANEL_TEXT_ITEM) {
	/* turn text caret back on */
	paint_caret(ip->panel->kbd_focus_item, TRUE);
    }
}

/*
 * Define rectangle containing the text (i.e., value rect less arrows)
 */
static void
update_text_rect(ip)
    Item_info      *ip;
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);

    dp->text_rect = ip->value_rect;
    if (dp->first_char) {
	dp->text_rect.r_left += dp->scroll_btn_width;
	dp->text_rect.r_width -= dp->scroll_btn_width;
    }
#ifdef	OW_I18N
    if (dp->last_char < wslen(dp->value_wc) - 1)
#else
    if (dp->last_char < strlen(dp->value) - 1)
#endif	OW_I18N
	dp->text_rect.r_width -= dp->scroll_btn_width;
}


#ifdef	OW_I18N
/*
 * ml_panel_display_interm(ip, visible_type, visible_pos)
 * displays the intermediate text str at the end of the panel text item.
 * The method to display the interm region is to display it as plain text
 * first, then reverse it, write underline to it, etc.
 * visible_type and visible_pos tells how to display it if the interm text
 * is longer than the panel text display length.  
 */
ml_panel_display_interm(ip)
    Item_info      *ip;
{
    Text_info *dp;
    int		interm_len;		/* strlen of interm text */
    int		interm_display_len;	/* intem display len in x axis */
    int		real_display_len;	/* actualy display len in x asix */
					/* for the text item. 		 */
#ifdef VISIBLE_POS
    XIMTextVisiblePosType   visible_type;
    int		   visible_pos;
#endif /* VISIBLE_POS */
    register int 	len;
    register int	i,j;		/* counters */
    int saved_value_rect_width;
    Xv_Window pw;
    wchar_t *str;
    XIMFeedback  *attr;
    int 	interm_bytes;
    int		start_interm_offset;
    int		has_caret = ip->panel->kbd_focus_item == ip;
    XFontSet            font_set;


    /* set up */
    dp = TEXT_FROM_ITEM(ip);
    font_set = (XFontSet)xv_get(ip->panel->std_font, FONT_SET_ID);
    interm_len = wslen(ip->panel->interm_text);
    interm_display_len = XwcTextEscapement(font_set, ip->panel->interm_text ,interm_len);
    str = ip->panel->interm_text;
    attr = ip->panel->interm_attr;
#ifdef VISIBLE_POS
    visible_type = ip->panel->visible_type;
    visible_pos = ip->panel->visible_pos;
#endif /* VISIBLE_POS */
					
    /* calculate real display length */
    real_display_len = ip->value_rect.r_width;
    if (dp->first_char)
   	real_display_len-=dp->scroll_btn_width;
    if (dp->last_char < wslen(dp->value_wc) - 1)
   	real_display_len-=dp->scroll_btn_width;

    if (interm_len == 0)
	paint_value(ip, 0);

    if (has_caret)
	paint_caret(ip, FALSE);
	
    if (interm_display_len >= real_display_len) {
	/*
	 * Interm columns is longer than the display length, we check
	 * visible_pos and visible_type to decide how to display.
	 */
	wchar_t		saved_wc;
	int		prev_pos;
	int		display_length;

#ifdef VISIBLE_POS
	switch (visible_type) {
	case XIMVisiblePosMid:
		/*
		 * Put visible_pos to the middle of the display.
		 * -------++++++X++++++-----
		 * Count back from visible_pos, then look from
		 * the beginning of interm text to find where
		 * we should start to display.
		 */
		for (i = visible_pos, len = real_display_len / 2;
		    i >= 0 && len >= 0; i--)
			len -=XwcTextEscapement(font_set, &str[i], 1);
		ml_panel_simple_display(ip, str + i, attr + i);
		break;

	default:
	case XIMVisiblePosEnd:
		/*
		 * Display the string before visible_pos.
		 * ++++++++++X--------------, including X-1
		 */
		saved_wc = str[visible_pos];
		str[visible_pos] = (wchar_t)NULL;
    		display_length = XwcTextEscapement(font_set, str ,wslen(str));
		if (display_length > real_display_len) {
			/* starting from str won't show X - 1 */
	    		prev_pos = visible_pos - 1;
	    		j = 0;
	    		i = visible_pos - 1;
 	    		while ((j < real_display_len) && (i >= 0)) {
				prev_pos = i;
    				j += XwcTextEscapement(font_set, &str[i] ,1);
				i--;
	    		}
	    		/* Sometimes when boundary is right in the middle */
	    		/* of a character, j will then go beyond display_legth. */
	    		/* In that case, we need to increment pre_pos by 1 */
	    		if (j > real_display_len)
				prev_pos += 1;

			str[visible_pos] = saved_wc;
			ml_panel_simple_display(ip,
			    str + prev_pos,
			    attr + prev_pos);
		} else {
			/* starting from str will show X */
			str[visible_pos] = saved_wc;
			ml_panel_simple_display(ip, str, attr);
		}
		break;

	case XIMVisiblePosStart:
		/* ----------X++++++++++++++, including X */
    		display_length = XwcTextEscapement(font_set, str + visible_pos,
					      wslen(str + visible_pos));
		if (display_length > real_display_len) {
			/*
			 * Clipping from right end won't show X.
			 * Start display at X, (not right-justified)
			 */
			ml_panel_simple_display(ip, 
			    str + visible_pos, attr + visible_pos);
			break;
		} else {
			/* Clipping from right end will show X */
	    		prev_pos = wslen(str) - 1;
	    		j = 0;
	    		i = wslen(str) - 1;
 	    		while ((j < real_display_len) && (i >= 0)) {
				prev_pos = i;
    				j += XwcTextEscapement(font_set, &str[i] ,1);
				i--;
	    		}
	    		/* Sometimes when boundary is right in the middle */
	    		/* of a character, j will then go beyond display_legth. */
	    		/* In that case, we need to increment pre_pos by 1 */
	    		if (j > real_display_len)
				prev_pos += 1;

			paint_value_and_interm(ip,
					str + prev_pos,
					attr + prev_pos);

			break;
		}
		break;
	}  /* case statement */
#endif /* VISIBLE_POS */
    } else {
	 /*
	 * The interm text is displayed to the right of the panel text
	 * with right attributes.
	 */
	    paint_value_and_interm(ip, str, attr);
    }
#ifdef	FUKU
	/* JEN ??? Why do we need to display the caret again??? */
	/* Shouldn't pain_value_and_interm will do it after */
	/* drawing intermediate text??			    */
	/* display the caret */
	if (has_caret)
		paint_caret(ip, PIX_SET);
#endif	FUKU
}

/*
 * ml_panel_moded_interm(ip, left, str, attr) paints invert, underline,
 * bold, or shaded to the displayed interm region according the attributes.
 * left is the start of the interm region in pixels.
 * str is expected to be null terminated.
 */
static void
ml_panel_moded_interm(ip, left, str, attr)
Item_info	*ip;
coord		left;
wchar_t		*str; 
XIMFeedback  *attr;

{
	Rect	interm_rect;
    	int	count;
	int	orig_count, attr_count, y;
    	int	interm_len;
        XFontSet            font_set;
	int	adv_x;
    	Xv_Window pw;
    	Xv_Drawable_info *info;
    	Xv_Screen      screen;
	GC	 *openwin_gc_list;
	XIMFeedback  bad_attr;

	font_set = (XFontSet)xv_get(ip->panel->std_font, FONT_SET_ID);
    	interm_len = wslen(str);
	/* now paint special effects according to attributes */
	interm_rect = ip->value_rect; 
	interm_rect.r_left = left;
	y = rect_bottom(&ip->value_rect);
        count = 0;
	while (count < interm_len) {
    		switch(attr[count]) {
		case 0:
			orig_count = count;
			/* skip and do nothig */
			while ((count < interm_len) && (attr[count] == 0))
				count++;
			
			attr_count = count - orig_count;
			adv_x = XwcTextEscapement(font_set, &str[orig_count],
                                        attr_count);
			interm_rect.r_left += adv_x;
			break;
		case XIMReverse:
			orig_count = count;
			while ((count < interm_len) && (attr[count] == XIMReverse))
				count++;
			attr_count = count - orig_count;
			adv_x = XwcTextEscapement(font_set, &str[orig_count],
                                        attr_count);
			interm_rect.r_width = adv_x;
			panel_invert(ip->panel, &interm_rect, ip->color_index);
			interm_rect.r_left += adv_x;
			break;
		case XIMUnderline:
			orig_count = count;
			while ((count < interm_len) && (attr[count] == XIMUnderline))
				count++;
			
			attr_count = count - orig_count;
			adv_x = XwcTextEscapement(font_set, &str[orig_count],
                                        attr_count);
			interm_rect.r_width = adv_x;
			PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	   		if (ip->color_index >= 0) {
				xv_vector(pw, interm_rect.r_left, y-1, interm_rect.r_left + interm_rect.r_width - 1, y-1,
					  ip->color_index < 0 ? PIX_SET :
					      PIX_SRC | PIX_COLOR(ip->color_index),
					  0);
	   		 } else {
				DRAWABLE_INFO_MACRO(pw, info);
				screen = xv_screen(info);
				openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY );
				XDrawLine(xv_display(info), xv_xid(info),
					  openwin_gc_list[OPENWIN_SET_GC],
					  interm_rect.r_left, y-1, interm_rect.r_left + interm_rect.r_width - 1, y-1);
	    		}
			PANEL_END_EACH_PAINT_WINDOW
			interm_rect.r_left += adv_x;
			break;
		
		default:
			/* Bad attr, trea it like XIMPlain */
			bad_attr = attr[count];
			orig_count = count;
			/* skip and do nothig */
			while ((count < interm_len) && (attr[count] == bad_attr))
				count++;
			
			attr_count = count - orig_count;
			adv_x = XwcTextEscapement(font_set, &str[orig_count],
                                        attr_count);
			interm_rect.r_left += adv_x;
			break;
		}
	}
}

/*
 * ml_panel_simply_display(ip, str, attr) displays str and attr
 * according to textdp(ip)->display_length.  Here, str cannot
 * be displayed in full and we start to display from str
 * and see how far we can go.
 */
ml_panel_simple_display(ip, str, attr)
	register Item_info		*ip;
	register wchar_t		*str;
	register XIMFeedback  	*attr;
{
	register Text_info		*dp = TEXT_FROM_ITEM(ip);
	register int			j; 	/* counting display in x axis */
	register int			last_pos; /* possible last index */
        XFontSet            		font_set;
	wchar_t				saved_wc;
	int				real_display_len;
/* ???? JEN why do we need this. Wait for Fuku's reply */
/*
	int				len;
*/


	/* compute real displayable length */
    	real_display_len = ip->value_rect.r_width;
    	if (dp->first_char)
   		real_display_len-=dp->scroll_btn_width;
    	if (dp->last_char < wslen(dp->value_wc) - 1)
   		real_display_len-=dp->scroll_btn_width;

	/*
	 * Find the last char we can display, then put a NULL
	 * there temporarily and display.
	 */
    	font_set = (XFontSet)xv_get(ip->panel->std_font, FONT_SET_ID);
	j = 0;
	last_pos = 0;
	while (j < real_display_len) {
	    j += XwcTextEscapement(font_set, &str[last_pos], 1);
	    last_pos++;
	}
	/* Sometimes when boundary is right in the middle */
	/* of a character, j will then go beyond real_display_legth. */
	/* In that case, we need to decrement last_pos by 1 */
	if (j > real_display_len)
		last_pos -= 1;


 	/* Save last char displayable */
	saved_wc = str[last_pos];

/* ??? JEN why do we need this? Wait for Fuku's reply */
/* calculate length of interm text before assing NULL temporary */
/*
	len = strlen(str);
*/

	/* set dummy data to run procedure correctly */
	str[last_pos] = (wchar_t)NULL;

	paint_value_and_interm(ip, str, attr);

 	/* Restore last char displayable */
	str[last_pos] = saved_wc;
}








#define	has_right_arrow(x) ((x)->last_char < (strlen((x)->value) - 1) ? 1 : 0)

/* paint_value_and_interm() does
 *	1. clear value rect
 *	2. draw left arrow if needed
 *	3. draw value of left hand side of caret
 *	4. draw intermediate text
 *	5. draw value of right hand side of caret
 *	6. draw right arrow if needed
 */
static void
paint_value_and_interm(ip, interm_str, interm_attr)
    register Item_info		*ip;
    register wchar_t		*interm_str;
    register XIMFeedback  	*interm_attr;
{
    register Text_info *dp = TEXT_FROM_ITEM(ip);
    register int    x = ip->value_rect.r_left;
    register int    y = ip->value_rect.r_top;
    register Panel_info *panel = ip->panel;
    int		caret_offset;
    int		insert_pos;
    int		interm_display_len;
    int		interm_len;
    XFontSet            font_set;
    int		real_display_len;
    wchar_t	*str_left;
    wchar_t	*str_right;
    int             i, j, len;
    Xv_Drawable_info *info;
    Xv_Window       pw;
    int		temp_first_char = 0;
    int		temp_last_char = 0;
#ifdef	INTERM_SCROLL
    int	value_right_len;
    int value_right_display_len;
#endif	INTERM_SCROLL




    register Rect  *r;
    struct	pr_size	size;

    /* Get the column position from current caret position */
    caret_offset = dp->saved_caret_offset;
    if (caret_offset < 0) {
	caret_offset = 0;
	insert_pos = 0;
    }
    else {
	/* Assuming saved_caret_offset and saved_caret_position is correct */
	insert_pos =  dp->saved_caret_position;
    }


    /***************************************************/
    /* store panel_value to str_left[] and str_right[] */
    /***************************************************/

    font_set = (XFontSet)xv_get(ip->panel->std_font, FONT_SET_ID);
    interm_len = wslen(interm_str);
    interm_display_len = XwcTextEscapement(font_set, interm_str ,interm_len);


    /* compute real displayable length */
    real_display_len = ip->value_rect.r_width;
    if (dp->first_char)
	real_display_len-=dp->scroll_btn_width;
    if (dp->last_char < wslen(dp->value_wc) - 1)
   	real_display_len-=dp->scroll_btn_width;

#ifdef	INTERM_SCROLL
    /* if there is no right scroll button, but real_display_len */
    /* is not enough to accomodate left str + interm + right str */
    /* Then we have to temporarily add scroll button on right side */ 
	
    value_right_len = wslen(&dp->value_wc[insert_pos]);
    value_right_display_len = XwcTextEscapement(font_set, 
		  	&dp->value_wc[insert_pos], value_right_len);
        if ((dp->last_char == (wslen(dp->value_wc) - 1)) && 
	((interm_display_len + dp->saved_caret_offset + value_right_display_len) > real_display_len)) {
   	real_display_len-=dp->scroll_btn_width;
	temp_last_char = 1;
    }
#endif	INTERM_SCROLL


    if (interm_display_len + dp->saved_caret_offset < real_display_len) {
	/*
	 * store left hand side of caret into str_left[]
	 */
	if (insert_pos > dp->first_char) {
	    len = insert_pos - dp->first_char + 1;
	    str_left = (wchar_t *) malloc(len * sizeof(wchar_t));
	    for (j = 0, i = dp->first_char; i < insert_pos; i++, j++)
		str_left[j] = dp->value_wc[i];
	    str_left[j] = (wchar_t) 0;
	}
	else
	    str_left = (wchar_t *)0;

	/*
	 * store right hand side of caret into str_right[]
	 */
	if (dp->last_char + 1 > insert_pos) {
	    int	display_length;
	    int	prev_pos;

	    len = dp->last_char - insert_pos + 2; /* allocate maximum size */
	    str_right = (wchar_t *) malloc(len * sizeof(wchar_t));

	   
	    display_length = real_display_len - interm_display_len - dp->saved_caret_offset;
	    if (display_length != 0) {
/* ???? JEN check with fuku why we need prev_pos ?? */
	 	    prev_pos = insert_pos;
		    i = insert_pos;
		    j = 0;
		    while ((j < display_length) && (i < dp->last_char + 1)) {
			prev_pos = i;
    			j += XwcTextEscapement(font_set, &dp->value_wc[i] ,1);
			i++;
		    }
		    /* Sometimes when boundary is right in the middle */
		    /* of a character, j will then go beyond display_legth. */
		    /* In that case, we need to decrement pre_pos by 1 */
		    if (j > display_length)
			prev_pos -= 1;
		    (void) wsncpy(str_right,
					&dp->value_wc[insert_pos],
					prev_pos - insert_pos + 1);
		    str_right[prev_pos - insert_pos +1] = (wchar_t) 0;
	    } 
	    else {
		/* No need to store str_right anymore. Free memory and */
		/* set str_right to NULL.			       */
		if (str_right)
			free(str_right);
	    	str_right = (wchar_t *)0;
	    }
	}
	else
	    str_right = (wchar_t *)0;
    }
    else {
	/*
	 * store left hand side of caret into str_left[]
	 */
	if (insert_pos > dp->first_char) {
	    int	display_length;
	    int	prev_pos;

	    len = insert_pos - dp->first_char + 1;
	    str_left = (wchar_t *) malloc(len);

#ifdef	INTERM_SCROLL
	    /* since left hand side + interm > display, then we must */
	    /* temporarily add scroll arrow to the left side if it   */
            /* is not there yet. */
	    if (dp->first_char == 0) {
		real_display_len-=dp->scroll_btn_width;
		temp_first_char = 1;
	     }
#endif	INTERM_SCROLL
	
	    display_length = real_display_len - interm_display_len;
				

	    if (display_length != 0) {
	 	prev_pos = insert_pos -1;
	    	j = 0;
	    	i = insert_pos - 1;
 	    	while ((j < display_length) && (i >= dp->first_char)) {
			prev_pos = i;
    			j += XwcTextEscapement(font_set, &dp->value_wc[i] ,1);
			i--;
	    	}
	    	/* Sometimes when boundary is right in the middle */
	    	/* of a character, j will then go beyond display_legth. */
	    	/* In that case, we need to increment pre_pos by 1 */
	    	if (j > display_length)
			prev_pos += 1;
	    	(void) wsncpy(str_left,
				&dp->value_wc[prev_pos],
				insert_pos - prev_pos);
	    	str_left[insert_pos - prev_pos] = (wchar_t) 0;
	    }
	    else {
		/* No need to store str_right anymore. Free memory and */
		/* set str_right to NULL.			       */
		if (str_left)
			free(str_left);
	    	str_left = (wchar_t *)0;
	    }
	}
	else
	    str_left = (wchar_t *)0;

	/*
	 * not store into str_right[] because interm text
	 * fill remaining space
	 */
	str_right = (wchar_t *)0;
    }




    /************************/
    /* clear the value rect */ 
    /************************/

    /*
     * Set the colors for the Clear and Set GC's.
     */
    PANEL_EACH_PAINT_WINDOW(panel, pw)
        DRAWABLE_INFO_MACRO(pw, info);
        openwin_check_gc_color(info, OPENWIN_SET_GC);
        panel_clear_pw_rect(pw, ip->value_rect);
    PANEL_END_EACH_PAINT_WINDOW

    /**************************************/
    /* draw the left clip arrow if needed */
    /**************************************/

#ifdef	INTERM_SCROLL
    if ((dp->first_char) || (temp_first_char))
#else
    if (dp->first_char)
#endif	INTERM_SCROLL
            draw_scroll_btn(ip, OLGX_SCROLL_BACKWARD);


    /***********************/
    /* Draw Left Hand Side */
    /***********************/


#ifdef	INTERM_SCROLL
    if ((dp->first_char) || (temp_first_char))
#else
    if (dp->first_char)
#endif	INTERM_SCROLL
	x += dp->scroll_btn_width;
    if (dp->mask == '\0') { /* not masked */
	if (str_left) {
	    PANEL_EACH_PAINT_WINDOW(panel, pw)
		panel_paint_text(pw, panel->std_font_xid, ip->color_index,
                                 x, y+dp->font_home, &str_left[0]);
            PANEL_END_EACH_PAINT_WINDOW
   	    x += XwcTextEscapement(font_set, str_left ,wslen(str_left));
	}
    } else {                /* masked */
        wchar_t         *buf;
        int             length, i;
        length = dp->last_char - dp->first_char + 2;
        buf = (wchar_t *) malloc(length * sizeof(wchar_t));
        for (j = 0, i = dp->first_char; i <= dp->last_char; i++, j++)
                buf[j] = (wchar_t)dp->mask; buf[length - 1] = (wchar_t) 0;
 
        PANEL_EACH_PAINT_WINDOW(panel, pw)
                panel_paint_text(pw, panel->std_font_xid, ip->color_index,
                                 x, y+dp->font_home, buf);
        PANEL_END_EACH_PAINT_WINDOW
   	x += XwcTextEscapement(font_set, buf ,wslen(buf));
        free(buf);
    }

#ifdef	FUKU	/* fuku's code for panel_find_columns */
		/* JEN : do this later		      */


    else {			/* masked */
	char           *buf;
	int             length, i;
	/* length is the column # of string, which is different from original */
	length = panel_find_columns(str_left, dp->first_char, insert_pos - 1);
	buf = (char *) sv_malloc(length + 1);
	for (j = 0; j < length; j++)
	    buf[j] = dp->mask;
	buf[length] = '\0';
	(void) panel_pw_text(panel,
			     x,
			     y + panel_fonthome(dp->font),
			     PIX_SRC | PIX_COLOR(ip->color_index),
			     dp->font,
			     buf);
	x += (strcollen(buf) * dp->font->pf_defaultsize.x);
	free(buf);
    }

#endif	FUKU


    /**************************/
    /* Draw Intermediate text */
    /**************************/

    PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	panel_paint_text(pw, ip->panel->std_font_xid, ip->color_index,
		x, ip->value_rect.r_top +dp->font_home,
		 interm_str);
    PANEL_END_EACH_PAINT_WINDOW
	
    /* now paint special effects according to attributes */
    ml_panel_moded_interm(ip, x, interm_str,interm_attr);
    x += interm_display_len;

    /* update caret end of interm text */
    /* caret_offset is  caret's x offset from right margin */
    /* of left arrow (which may be blank). */
#ifdef	INTERM_SCROLL
    if ((dp->first_char) || (temp_first_char))
#else
    if (dp->first_char)
#endif	INTERM_SCROLL
	dp->caret_offset = x - ip->value_rect.r_left - dp->scroll_btn_width;
    else
   	dp->caret_offset = x - ip->value_rect.r_left;;
    /* display caret */
    paint_caret(ip, TRUE);





    /************************/
    /* Draw Right Hand Side */
    /************************/

    if (dp->mask == '\0') {	/* not masked */
	if (str_right) {
	    PANEL_EACH_PAINT_WINDOW(panel, pw)
		panel_paint_text(pw, panel->std_font_xid, ip->color_index,
                                 x, y+dp->font_home, &str_right[0]);
            PANEL_END_EACH_PAINT_WINDOW
   	    x += XwcTextEscapement(font_set, str_right ,wslen(str_right));
	}
    }
    else {                /* masked */
        wchar_t         *buf;
        int             length, i;
        length = dp->last_char - dp->first_char + 2;
        buf = (wchar_t *) malloc(length * sizeof(wchar_t));
        for (j = 0, i = dp->first_char; i <= dp->last_char; i++, j++)
                buf[j] = (wchar_t)dp->mask; buf[length - 1] = (wchar_t) 0;
 
        PANEL_EACH_PAINT_WINDOW(panel, pw)
                panel_paint_text(pw, panel->std_font_xid, ip->color_index,
                                 x, y+dp->font_home, buf);
        PANEL_END_EACH_PAINT_WINDOW
   	x += XwcTextEscapement(font_set, buf ,wslen(buf));
        free(buf);
    }


#ifdef	FUKU	/* fuku's code for panel_find_columns */
		/* JEN : do this later		      */

    else {			/* masked */
	char           *buf;
	int             length, i;
	/* length is the column # of string, which is different from original */
	length = panel_find_columns(str_right, insert_pos, dp->last_char);
	buf = (char *) sv_malloc(length + 1);
	for (j = 0; j < length; j++)
	    buf[j] = dp->mask;
	buf[length] = '\0';
	(void) panel_pw_text(panel,
			     x,
			     y + panel_fonthome(dp->font),
			     PIX_SRC | PIX_COLOR(ip->color_index),
			     dp->font,
			     buf);
	x += (strcollen(buf) * dp->font->pf_defaultsize.x);
	free(buf);
    }
#endif	FUKU

    /***************************************/
    /* draw the right clip arrow if needed */
    /***************************************/

#ifdef	INTERM_SCROLL
    if ((dp->last_char < (wslen(dp->value_wc) - 1))  || (temp_last_char))
#else
    if (dp->last_char < (wslen(dp->value_wc) - 1))
#endif	INTERM_SCROLL
            draw_scroll_btn(ip, OLGX_SCROLL_FORWARD);


    /* Underline the text (optional) */
    if (dp->flags & UNDERLINED) {
	y = rect_bottom(&ip->value_rect);
	if (ip->panel->three_d) {
	    /* 3D text ledge is 2 pixels high.  (2D is 1 pixel high.) */
	    y--;
	}
	PANEL_EACH_PAINT_WINDOW(panel, pw)
	    DRAWABLE_INFO_MACRO(pw, info);
	    olgx_draw_text_ledge(panel->ginfo, xv_xid(info),
	        ip->value_rect.r_left, y,
		ip->value_rect.r_width);
	PANEL_END_EACH_PAINT_WINDOW
    }

    if (str_left)
	free(str_left);
    if (str_right)
	free(str_right);

    /*
     * paint_value_and_interm() does not hilite selection, Because
     * selection while conversion mode does not happen.
     */
}



ml_panel_saved_caret(ip)
Item_info	*ip;
{
    	Text_info *dp;


	/* store the current_caret_offset */
    	dp = TEXT_FROM_ITEM(ip);
	dp->saved_caret_offset = dp->caret_offset;
	dp->saved_caret_position = dp->caret_position;
}
#endif	OW_I18N



