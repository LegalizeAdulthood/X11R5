#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_list.c 70.2 91/07/08";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <ctype.h>
#include <strings.h>
#include <X11/X.h>
#include <xview_private/panel_impl.h>
#include <xview/defaults.h>
#include <xview_private/p_lst_impl.h>
#include <xview_private/draw_impl.h>
#include <xview_private/pw_impl.h>
#include <xview/font.h>
#include <xview/scrollbar.h>
#include <xview/sel_attrs.h>
#include <xview/server.h>
#include <xview/screen.h>
#include <xview/openmenu.h>
#ifdef OW_I18N
#include <stdlib.h>
#include <xview_private/xv_i18n_impl.h>
#endif /* OW_I18N */

#define PANEL_LIST_PRIVATE(item)        \
        XV_PRIVATE(Panel_list_info, Xv_panel_list, item)
#define PANEL_LIST_PUBLIC(item)         XV_PUBLIC(item)
#define PANEL_LIST_FROM_ITEM(ip)	PANEL_LIST_PRIVATE(ITEM_PUBLIC(ip))
#define ITEM_FROM_PANEL_LIST(plist)	ITEM_PRIVATE(PANEL_LIST_PUBLIC(plist))
#define PANEL_LIST_ROW_GAP	5
#define PANEL_LIST_COL_GAP	4	/* space between glyph and string */
#define ROW_MARGIN	9 /* space between vertical borders and box */
#define LIST_BOX_BORDER_WIDTH 1
#define SHOW_ROW TRUE
#define HIDE_ROW FALSE
#define REPAINT_LIST TRUE
#define DO_NOT_REPAINT_LIST FALSE
#define RETURN '\r'
#define TAB '\t'

Pkg_private int panel_list_init();
Pkg_private int panel_list_destroy();
Pkg_private Xv_opaque panel_list_set_avlist();
Pkg_private Xv_opaque panel_list_get_attr();

extern int OW_GC_KEY;
#ifdef OW_I18N
extern struct pr_size	xv_pf_textwidth_wc();
extern wchar_t	null_string_wc[];
#endif OW_I18N

static char    *selection_string();
static int      fit_list_box_to_rows();
static int      sizeof_selection();
static Panel_setting change_done();
static Row_info *next_row();
static Row_info *find_or_create_nth_row();
static Row_info *panel_list_insert_row();
static Seln_result get_selection();
static Seln_result panel_list_reply_proc();
static void	accept_change();
static void	accept_insert();
static void	check_row_in_view();
static void	handle_menu_event();
static void	paint_list_box();
static void	paint_list_box_border();
static void	paint_row();
static void	panel_list_accept_kbd_focus();
static void     panel_list_delete_row();
static void     panel_list_handle_event();
static void	panel_list_layout();
static void	panel_list_paint();
static void	panel_list_remove();
static void     panel_list_seln_function();
static void	panel_list_yield_kbd_focus();
static void	set_current_row();
static void	set_edit_row();
static void	set_row_display_str_length();
static void	set_row_glyph();
static void	show_feedback();
static Xv_opaque change_proc();
static Xv_opaque clear_all_choices();
static Xv_opaque delete_proc();
static Xv_opaque enter_edit_mode();
static Xv_opaque enter_read_mode();
static Xv_opaque insert_proc();
static Xv_opaque locate_next_choice();

extern void     panel_default_handle_event();
extern struct pr_size xv_pf_textwidth();

static Panel_ops ops = {
    (void (*) ()) panel_list_handle_event,/* handle_event() */
    (void (*) ()) panel_nullproc,	/* begin_preview() */
    (void (*) ()) panel_nullproc,	/* update_preview() */
    (void (*) ()) panel_nullproc,	/* cancel_preview() */
    (void (*) ()) panel_nullproc,	/* accept_preview() */
    (void (*) ()) panel_nullproc,	/* accept_menu() */
    (void (*) ()) panel_nullproc,	/* accept_key() */
    (void (*) ()) panel_list_paint,	/* paint() */
    (void (*) ()) panel_list_remove,	/* remove() */
    (void (*) ()) panel_nullproc,	/* restore() */
    (void (*) ()) panel_list_layout,	/* layout() */
    (void (*) ()) panel_list_accept_kbd_focus,	/* accept_kbd_focus() */
    (void (*) ()) panel_list_yield_kbd_focus	/* yield_kbd_focus() */
};


static Defaults_pairs sb_placement_pairs[] = {
	"Right", FALSE,
	"right", FALSE,
	"Left", TRUE,
	"left", TRUE,
	NULL, FALSE,
};


typedef enum {
    INSERT_BEFORE,
    INSERT_AFTER
} Insert_pos_t;

/*ARGSUSED*/
Pkg_private int
panel_list_init(parent, panel_list_public, avlist)
    Xv_Window       parent;
    Panel_item      panel_list_public;
    Attr_avlist     avlist;
{
    Panel_list_info *dp;
    Xv_Drawable_info *info;
    Xv_panel_list  *panel_list = (Xv_panel_list *) panel_list_public;
    Xv_object       server = xv_get(xv_get(parent, XV_SCREEN), SCREEN_SERVER);
    Item_info      *ip = ITEM_PRIVATE(panel_list_public);
    int			chrht;

    ip->item_type = PANEL_LIST_ITEM;
    if (ops_set(ip))
	*ip->ops = ops;		/* copy the panel list ops vector */
    else
	ip->ops = &ops;		/* use the static panel list ops vector */
    panel_set_bold_label_font(ip);
    dp = xv_alloc(Panel_list_info);

    /* link to object */
    panel_list->private_data = (Xv_opaque) dp;
    dp->public_self = panel_list_public;

    /* Initialize the object */
    dp->nlevels = 1;
    dp->parent_panel = parent;
    dp->choose_one = TRUE; /* exclusive scrolling list */
    dp->edit_op = OP_NONE;
    dp->font = (Pixfont *) xv_get(parent,
	XV_FONT);
    DRAWABLE_INFO_MACRO(ip->panel->paint_window->pw, info);
#ifdef OW_I18N
    dp->font_set = (XFontSet)xv_get(dp->font, FONT_SET_ID);
    dp->wchar_notify = TRUE;
#else
    dp->font_struct = XQueryFont(xv_display(info), ip->panel->std_font_xid);
#endif OW_I18N
    dp->left_hand_sb = defaults_get_enum("OpenWindows.ScrollbarPlacement",
	"OpenWindows.ScrollbarPlacement", sb_placement_pairs);
    dp->list_box.r_left = ip->panel->item_x;
    dp->list_box.r_top = ip->panel->item_y;
    chrht = xv_get(dp->font, FONT_DEFAULT_CHAR_HEIGHT);
    dp->row_height = chrht + PANEL_LIST_ROW_GAP;

    dp->seln_client = selection_create(server,
			    panel_list_seln_function, panel_list_reply_proc,
						  (char *) dp);
    panel_append(ip);

    /* If this is the first kbd_focus item, then set current
     * keyboard focus item.
     */
    ip->flags |= WANTS_KEY;
    if (!hidden(ip) && ip->panel->kbd_focus_item == NULL)
        ip->panel->kbd_focus_item = ip;

    return XV_OK;
}


Pkg_private int
panel_list_destroy(panel_list_public, status)
    Panel_item      panel_list_public;
    Destroy_status  status;
{
    Panel_list_info *dp = PANEL_LIST_PRIVATE(panel_list_public);
    Item_info	   *ip = ITEM_PRIVATE(panel_list_public);
    Row_info       *row = dp->rows;
    Row_info       *next;

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF)) {
	return XV_OK;
    }
    panel_list_remove(ip);
    while (row) {
	next = row->next;
	if (row->free_string)
	    xv_free(row->string);
	xv_free(row);
	row = next;
    }
    if (dp->selection[0])
	xv_free(dp->selection[0]);
    if (dp->selection[1])
	xv_free(dp->selection[1]);
    if (dp->text_item)
	xv_destroy(dp->text_item);
    xv_destroy(dp->list_sb);
    xv_free(dp);

    return XV_OK;
}


static void
panel_list_create_displayarea(dp)
    Panel_list_info *dp;
{
    Frame	    base_frame;
    Item_info      *ip = ITEM_PRIVATE(dp->public_self);
    int             number_rows;
    Xv_Window	    parent_pw;
    int		    sb_x;	/* x coordinate of PANEL_LIST Scrollbar */
    
    if (dp->rows_displayed == 0) {
	if (dp->nrows < PANEL_LIST_DEFAULT_ROW)
	    dp->rows_displayed = dp->nrows;
	else
	    dp->rows_displayed = PANEL_LIST_DEFAULT_ROW;
    }
    number_rows = dp->rows_displayed;

    /*
     * By this time all the rows have been created. Calculate the size of the
     * list box (depends on the number of rows to be displayed and the size of
     * the fonts).
     */
    dp->list_box.r_height = 2*LIST_BOX_BORDER_WIDTH +
	2*ROW_MARGIN + number_rows*dp->row_height;
    base_frame = xv_get(dp->parent_panel, WIN_FRAME);
    parent_pw = xv_get(dp->parent_panel, CANVAS_NTH_PAINT_WINDOW, 0);
    if (dp->left_hand_sb)
	sb_x = dp->list_box.r_left;
    else
	sb_x = (int) xv_get(dp->parent_panel, XV_X) +
	    rect_right(&dp->list_box) + 1;
    dp->list_sb = xv_create(base_frame, SCROLLBAR,
    	WIN_CMS, xv_get(parent_pw, WIN_CMS),
	XV_X, sb_x,
	XV_Y, xv_get(dp->parent_panel, XV_Y) + dp->list_box.r_top,
	XV_HEIGHT, dp->list_box.r_height,
	XV_KEY_DATA, PANEL_LIST, ip,
	SCROLLBAR_NOTIFY_CLIENT, parent_pw,
	SCROLLBAR_VIEW_LENGTH,	dp->rows_displayed,  /* in rows */
	SCROLLBAR_OBJECT_LENGTH, dp->nrows,  /* in rows */
	SCROLLBAR_PIXELS_PER_UNIT, dp->row_height,
	SCROLLBAR_INACTIVE, inactive(ip),
	XV_SHOW, !hidden(ip),
	0);
    if (dp->left_hand_sb)
	dp->list_box.r_left += (int) xv_get(dp->list_sb, XV_WIDTH);
    if (ip->color_index >= 0)
	xv_set(dp->list_sb,
	       WIN_FOREGROUND_COLOR, ip->color_index,
	       0);
}


/*
 * fit the scrolling list
 */
static int	/* returns TRUE if dp->list_box.r_width changed */
fit_list_box_to_rows(dp)
    Panel_list_info *dp;
{
    int		    width_changed;
    int		    max_str_size;
    int		    new_width;
    Row_info       *row;
    struct pr_size  str_size;	/* width & height of row string */

    if (!dp->width) {
	max_str_size = 0;
	for (row=dp->rows; row; row=row->next) {
#ifdef OW_I18N
	    if (row->string_wc) {
		str_size = xv_pf_textwidth_wc(wslen(row->string_wc), 
			dp->font, row->string_wc);
		max_str_size = MAX(max_str_size, str_size.x);
	    }
#else
	    if (row->string) {
		str_size = xv_pf_textwidth(strlen(row->string), dp->font,
		    row->string);
		max_str_size = MAX(max_str_size, str_size.x);
	    }
#endif OW_I18N
	}
	new_width = dp->string_x + max_str_size + ROW_MARGIN +
	    LIST_BOX_BORDER_WIDTH;
    } else
	new_width = dp->width;
    width_changed = new_width != dp->list_box.r_width;
    dp->list_box.r_width = new_width;
    return (width_changed);
}


static void
set_row_display_str_length(dp, row)
    Panel_list_info	*dp;
    Row_info		*row;
{
#ifdef OW_I18N
    XFontSet		font_set;
#else
    XFontStruct		*font_struct;
#endif OW_I18N
    Xv_Drawable_info	*info;
    Item_info		*ip = ITEM_FROM_PANEL_LIST(dp);
    int			max_string_width;
   

    DRAWABLE_INFO_MACRO(ip->panel->paint_window->pw, info);
#ifdef OW_I18N
    if (row->font)
	font_set = (XFontSet)xv_get(row->font, FONT_SET_ID);
    else
	font_set = dp->font_set;
    row->display_str_len = row->string_wc ? wslen(row->string_wc) : 0;
    max_string_width = dp->list_box.r_width - LIST_BOX_BORDER_WIDTH -
	ROW_MARGIN - dp->string_x;
    while (XwcTextEscapement(font_set, row->string_wc, row->display_str_len) >
	   max_string_width) {
	row->display_str_len--;
    }
#else
    if (row->font)
	font_struct = XQueryFont(xv_display(info), xv_get(row->font, XV_XID));
    else
	font_struct = dp->font_struct;
    row->display_str_len = row->string ? strlen(row->string) : 0;
    max_string_width = dp->list_box.r_width - LIST_BOX_BORDER_WIDTH -
	ROW_MARGIN - dp->string_x;
    while (XTextWidth(font_struct, row->string, row->display_str_len) >
	   max_string_width) {
	row->display_str_len--;
    }
#endif OW_I18N
}


static void
set_row_font(dp, row, font)
    Panel_list_info *dp;
    Row_info	   *row;
    Xv_Font	    font;
{
    Xv_Font	    old_font = row->font;

    if (!font)
	row->font = font;
    else if ((int) xv_get(font, FONT_DEFAULT_CHAR_HEIGHT) <= dp->row_height)
	row->font = font;
    else {
	xv_error(font,
		 ERROR_STRING,
#ifdef OW_I18N
		   XV_I18N_MSG("xv_messages", "Font height exceeds row height; font ignored"),
#else
		   "Font height exceeds row height; font ignored",
#endif OW_I18N
		 ERROR_PKG, PANEL,
		 0);
	row->font = NULL;
    }
    if (row->font != old_font)
	row->display_str_len = 0;   /* force recomputation */
}


static void
set_row_glyph(dp, row, glyph_pr)
    Panel_list_info *dp;
    Row_info *row;
    Pixrect *glyph_pr;
{
    if (glyph_pr->pr_height <= dp->row_height) {
	row->glyph = glyph_pr;
    } else {
	xv_error(glyph_pr,
		 ERROR_STRING,
#ifdef OW_I18N
		   XV_I18N_MSG("xv_messages", "Panel List glyph height exceeds row height; glyph ignored"),
#else
		   "Panel List glyph height exceeds row height; glyph ignored",
#endif OW_I18N
		 ERROR_PKG, PANEL,
		 0);
	row->glyph = NULL;
    }
}


Pkg_private     Xv_opaque
panel_list_set_avlist(panel_list_public, avlist)
    Panel_item      panel_list_public;
    Attr_avlist     avlist;
{
    register Panel_list_info *dp = PANEL_LIST_PRIVATE(panel_list_public);
    register int i;
    Item_info      *ip = ITEM_PRIVATE(panel_list_public);
    int		    list_box_width_changed;
    int             max_glyph_width;
    Attr_avlist     orig_avlist = avlist;
    Rect	    parent_panel_rect;
    int             reset_rows = FALSE;
    Xv_opaque	    result;
    Row_info       *row;
    int		    sb_x;	/* x coordinate of PANEL_LIST Scrollbar */
    Xv_Server	    server;
    int             xv_end_create = FALSE;
	
#ifdef OW_I18N
    XID             db;
    int		    db_int;
    int		    db_count = 0;

    server = XV_SERVER_FROM_WINDOW(PANEL_PUBLIC(ip->panel));
    db = xv_get(server, SERVER_RESOURCE_DB);
#endif  OW_I18N


    /* Call generic item set code to handle layout attributes */
    if (*avlist != XV_END_CREATE) {
	/* Prevent panel_redisplay_item from being called in item_set_avlist.
	 * Otherwise, the ip->painted_rect will be cleared prematurely, and
	 * we'll be unable to clear out the old painted rect (especially
	 * important in case the item rect is being moved).
	 */
	ip->panel->status |= NO_REDISPLAY_ITEM;
	result = xv_super_set_avlist(panel_list_public, &xv_panel_list_pkg,
				     avlist);
	ip->panel->status &= ~NO_REDISPLAY_ITEM;
    	if (result != XV_OK)
	    return result;
    }


#ifdef OW_I18N
    while (*avlist || db_count) {
	switch ((int) avlist[0]) {

	  case XV_USE_DB:
		db_count++;
		break;

	  case PANEL_LIST_ROW_HEIGHT:
		if (!dp->initialized) {
		    if (db_count) {
			dp->row_height = 
			    (unsigned short)db_get_data(db,
					    ip->instance_qlist,
					    "panel_list_row_height", XV_INT,
					    avlist[1]);	
		   }
		   else
			dp->row_height = (unsigned short) avlist[1];
	 	}
		break;
	  default:
		break;
	}

	if (avlist[0] == XV_USE_DB)
	    avlist++;
        else if (db_count && !avlist[0]) {
            db_count--;
            avlist++;  
        } else
            avlist = attr_next(avlist);
    }
#else OW_I18N
    for (; *avlist; avlist = attr_next(avlist)) {
	switch ((int) avlist[0]) {
	  case PANEL_LIST_ROW_HEIGHT:
	        if (!dp->initialized)
		    dp->row_height = (unsigned short) avlist[1];
	        break;
	  default:
	        break;
	}
    }
#endif OW_I18N

    /*
     * Process the attributes that rely on the already processed attributes.
     */
#ifdef OW_I18N
    avlist = orig_avlist;
    while (*avlist || db_count) {
	switch ((int) avlist[0]) {
	  case XV_USE_DB:
		db_count++;
		break;
	  case PANEL_CHOOSE_NONE:
		dp->choose_none = avlist[1] ? TRUE : FALSE;
		break;
	  case PANEL_CHOOSE_ONE:
		dp->choose_one = avlist[1] ? TRUE : FALSE;
		break;
	  case PANEL_EVENT_PROC:
		dp->event_proc = (void (*)()) avlist[1];
		break;
	  case PANEL_LIST_WIDTH:
		if (db_count) {
		    db_int = db_get_data(db, ip->instance_qlist,
				"panel_list_width", XV_INT, avlist[1]);
	            dp->width = MAX((int) db_int, 2*LIST_BOX_BORDER_WIDTH +
				2*ROW_MARGIN + PANEL_LIST_COL_GAP);
		} else {
	            dp->width = MAX((int) avlist[1], 2*LIST_BOX_BORDER_WIDTH +
				2*ROW_MARGIN + PANEL_LIST_COL_GAP);
		}
	        break;

	  case PANEL_LIST_DISPLAY_ROWS:
	    if (db_count) {
		dp->rows_displayed = db_get_data(db, ip->instance_qlist,
					    "panel_list_display_rows", 
					    XV_INT, avlist[1]);
	    }
	    else
		dp->rows_displayed = (int) avlist[1];
	    reset_rows = TRUE;
	    break;
	  case PANEL_LIST_FONTS:
		row = dp->rows;
		for (i = 1; avlist[i]; i++) {
		    if (row)
			set_row_font(dp, row, (Xv_Font) avlist[i]);
		    row = row->next;
		}
		break;

	  case PANEL_LIST_FONT:
		row = find_or_create_nth_row(dp, (int) avlist[1], FALSE);
		if (row)
		    set_row_font(dp, row, (Xv_Font) avlist[2]);
		break;

	  case PANEL_ITEM_COLOR:
		if (dp->list_sb)
		    xv_set(dp->list_sb,
			   WIN_FOREGROUND_COLOR, ip->color_index,
			   0);
		break;

	  case PANEL_ITEM_MENU:
		if (dp->edit_mode)
		    dp->edit_menu = (Menu) avlist[1];
		else
		    dp->read_menu = (Menu) avlist[1];
	        ATTR_CONSUME(*((Attr_avlist) & avlist[0]));
	        break;

	  case PANEL_LIST_SELECT:{
		int             which_row = (int) avlist[1];
		int             selected = (int) avlist[2];

		row = find_or_create_nth_row(dp, which_row, FALSE);
		if (row) {
		    if (row->selected != selected)
			set_current_row(dp, row, NULL);
		    if (!hidden(ip))
			check_row_in_view(dp, row->row);
		}
		break;
	    }

	  case PANEL_LIST_STRINGS:{
		int             n = 0;

		row = dp->rows;
		while (avlist[n+1]) {
		    row = next_row(dp, row, n++);
                    /*
		     *  Panel will only process wchar_t strings.  Free
		     *  up all the old spaces first, then allocate
		     *  space for the list strings.
		     */
		    if (*row->string_wc)
			free ((char *) row->string_wc);
		    if (row->string) {
			if (*row->string)
			    free ((char *) row->string);
		    }

		    row->string_wc = mbstowcsdup ((u_char *) avlist[n]);
		    row->display_str_len = 0;  /* force recomputation */
		    row->free_string = TRUE;
		}
		break;
	    }

	  case PANEL_LIST_STRINGS_WCS:{
		int             n = 0;

		row = dp->rows;

		while (avlist[n+1]) {
		    row = next_row(dp, row, n++);
		    if (*row->string_wc)
			free ((char *) row->string_wc);
		    if (row->string) {
			if (*row->string)
			   free ((char *) row->string);
		    }
		    row->string_wc = (wchar_t *) panel_strsave_wc((wchar_t *)avlist[n]);
		    row->display_str_len = 0;  /* force recomputation */
		    row->free_string = TRUE;
		}
		break;
	    }
	  case PANEL_LIST_STRING:{
		int             which_row = (int) avlist[1];
		row = find_or_create_nth_row(dp, which_row,
					     TRUE);
		if (*row->string_wc)
		    free ((char *) row->string_wc);
		if (row->string) {
		    if (*row->string)
		       free ((char *) row->string);
		}

		if (avlist[2]) {
		    row->string_wc = mbstowcsdup((u_char *) avlist[2]);
		    row->display_str_len = 0;  /* force recomputation */
		    row->free_string = TRUE;
		} else {
		    row->string_wc = mbstowcsdup((u_char *) avlist[2]);
		    row->free_string = FALSE;
		}
		break;
	    }

	   case PANEL_LIST_STRING_WCS:{
		  int             which_row = (int) avlist[1];
		  row = find_or_create_nth_row(dp, which_row,
					       TRUE);
		  if (*row->string_wc)
		      free ((char *) row->string_wc);
		  if (row->string) {
		      if (*row->string)
		         free ((char *) row->string);
		  }
		  if (avlist[2]) {
		      row->string_wc = (wchar_t *) panel_strsave_wc((wchar_t *) avlist[2]);
		      row->display_str_len = 0;  /* force recomputation */
		      row->free_string = TRUE;
		  } else {
		      row->string_wc = (wchar_t *) avlist[2];
		      row->free_string = FALSE;
		  }
		  break;
	     }

	  case PANEL_NOTIFY_PROC: {
		 dp->wchar_notify = FALSE;
		 break;
	     }

	  case PANEL_NOTIFY_PROC_WCS: {
		 dp->wchar_notify = TRUE;
		 break;
	     }
	  case PANEL_LIST_GLYPHS:{
		int             n = 0;
		
		row = dp->rows;
		while (avlist[n+1]) {
		    row = next_row(dp, row, n++);
		    set_row_glyph(dp, row, (Pixrect *) avlist[n]);
		}
		break;
	    }

	  case PANEL_LIST_GLYPH:{
		int             which_row = (int) avlist[1];
		row = find_or_create_nth_row(dp,
					     which_row, TRUE);
		set_row_glyph(dp, row, (Pixrect *) avlist[2]);
		break;
	    }


	  case PANEL_LIST_CLIENT_DATAS:{
	  	int n=1;
		row = dp->rows;
		while (avlist[n]) {
		    if (row) {
			row->client_data = (Xv_opaque) avlist[n];
		    }
		    row = row->next;
		    n++;
		}
		break;
	    }

	  case PANEL_LIST_CLIENT_DATA:{
		int             which_row = (int) avlist[1];
		row = find_or_create_nth_row(dp,
					     which_row, FALSE);
		if (row)
		    row->client_data = (Xv_opaque) avlist[2];
		break;
	    }

	  case PANEL_LIST_INSERT:{
		int             which_row = (int) avlist[1];
		panel_list_insert_row(dp, which_row, SHOW_ROW,
				      DO_NOT_REPAINT_LIST);
		break;
	    }

	  case PANEL_LIST_DELETE:{
		int             which_row = (int) avlist[1];
		Row_info       *node = dp->rows;

		while (node && (node->row != which_row))
		    node = node->next;
		if (node)
		    panel_list_delete_row(dp, node, DO_NOT_REPAINT_LIST);
		break;
	    }

	  case PANEL_INACTIVE:
	    if (avlist[1] && ip->panel->kbd_focus_item == ip) {
		dp->has_kbd_focus = FALSE;
		if (dp->initialized)
		    paint_list_box(dp);
		ip->panel->kbd_focus_item = panel_next_kbd_focus(ip->panel);
		panel_accept_kbd_focus(ip->panel);
	    }
	    if (dp->list_sb)
		xv_set(dp->list_sb, SCROLLBAR_INACTIVE, avlist[1], 0);
	    break;

	  case PANEL_READ_ONLY:
	    dp->read_only = avlist[1] ? TRUE : FALSE;
	    break;

	  case XV_SHOW:
	    if (dp->text_item_row)
		xv_set(dp->text_item, XV_SHOW, avlist[1], 0);
	    if (dp->list_sb)
		xv_set(dp->list_sb, XV_SHOW, avlist[1], 0);
	    break;

	  case XV_END_CREATE:
	    panel_list_create_displayarea(dp);
	    dp->initialized = TRUE;
	    xv_end_create = TRUE;
	    break;

	  default:
	    break;
	}

	if (avlist[0] == XV_USE_DB)
	    avlist++;
        else if (db_count && !avlist[0]) {
            db_count--;
            avlist++;  
        } else
            avlist = attr_next(avlist);
    }
#else OW_I18N
    for (avlist = orig_avlist; *avlist; avlist = attr_next(avlist)) {
	switch ((int) avlist[0]) {
	  case PANEL_CHOOSE_NONE:
		dp->choose_none = avlist[1] ? TRUE : FALSE;
		break;
	  case PANEL_CHOOSE_ONE:
		dp->choose_one = avlist[1] ? TRUE : FALSE;
		break;
	  case PANEL_EVENT_PROC:
		dp->event_proc = (void (*)()) avlist[1];
		break;

	  case PANEL_LIST_WIDTH:
	        dp->width = MAX((int) avlist[1], 2*LIST_BOX_BORDER_WIDTH +
				2*ROW_MARGIN + PANEL_LIST_COL_GAP);
	        break;

	  case PANEL_LIST_DISPLAY_ROWS:
                dp->rows_displayed = (int) avlist[1];
	        reset_rows = TRUE;
	        break;

	  case PANEL_LIST_FONTS:
		row = dp->rows;
		for (i = 1; avlist[i]; i++) {
		    if (row)
			set_row_font(dp, row, (Xv_Font) avlist[i]);
		    row = row->next;
		}
		break;

	  case PANEL_LIST_FONT:
		row = find_or_create_nth_row(dp, (int) avlist[1], FALSE);
		if (row)
		    set_row_font(dp, row, (Xv_Font) avlist[2]);
		break;

	  case PANEL_ITEM_COLOR:
		if (dp->list_sb)
		    xv_set(dp->list_sb,
			   WIN_FOREGROUND_COLOR, ip->color_index,
			   0);
		break;

	  case PANEL_ITEM_MENU:
		if (dp->edit_mode)
		    dp->edit_menu = (Menu) avlist[1];
		else
		    dp->read_menu = (Menu) avlist[1];
	        ATTR_CONSUME(*((Attr_avlist) & avlist[0]));
	        break;

	  case PANEL_LIST_SELECT:{
		int             which_row = (int) avlist[1];
		int             selected = (int) avlist[2];

		row = find_or_create_nth_row(dp, which_row, FALSE);
		if (row) {
		    if (row->selected != selected)
			set_current_row(dp, row, NULL);
		    if (!hidden(ip))
			check_row_in_view(dp, row->row);
		}
		break;
	    }

            case PANEL_LIST_STRINGS:{
                int             n = 0;
 
                row = dp->rows;
                while (avlist[n+1]) {
                    row = next_row(dp, row, n++);
                    row->string = panel_strsave((u_char *) avlist[n]);
                    row->display_str_len = 0;  /* force recomputation */
                    row->free_string = TRUE;
                }
                break;
             }
  
            case PANEL_LIST_STRING:{
                int             which_row = (int) avlist[1];
                row = find_or_create_nth_row(dp, which_row,
                                             TRUE);
                if (avlist[2]) {
                    row->string = panel_strsave((u_char *) avlist[2]);
                    row->display_str_len = 0;  /* force recomputation */
                    row->free_string = TRUE;
                } else {
                    row->string = (u_char *) avlist[2];
                    row->free_string = FALSE;
                }
                break;
            }
	  case PANEL_LIST_GLYPHS:{
		int             n = 0;
		
		row = dp->rows;
		while (avlist[n+1]) {
		    row = next_row(dp, row, n++);
		    set_row_glyph(dp, row, (Pixrect *) avlist[n]);
		}
		break;
	    }

	  case PANEL_LIST_GLYPH:{
		int             which_row = (int) avlist[1];
		row = find_or_create_nth_row(dp,
					     which_row, TRUE);
		set_row_glyph(dp, row, (Pixrect *) avlist[2]);
		break;
	    }


	  case PANEL_LIST_CLIENT_DATAS:{
	  	int n=1;
		row = dp->rows;
		while (avlist[n]) {
		    if (row) {
			row->client_data = (Xv_opaque) avlist[n];
		    }
		    row = row->next;
		    n++;
		}
		break;
	    }

	  case PANEL_LIST_CLIENT_DATA:{
		int             which_row = (int) avlist[1];
		row = find_or_create_nth_row(dp,
					     which_row, FALSE);
		if (row)
		    row->client_data = (Xv_opaque) avlist[2];
		break;
	    }

	  case PANEL_LIST_INSERT:{
		int             which_row = (int) avlist[1];
		panel_list_insert_row(dp, which_row, SHOW_ROW,
				      DO_NOT_REPAINT_LIST);
		break;
	    }

	  case PANEL_LIST_DELETE:{
		int             which_row = (int) avlist[1];
		Row_info       *node = dp->rows;

		while (node && (node->row != which_row))
		    node = node->next;
		if (node)
		    panel_list_delete_row(dp, node, DO_NOT_REPAINT_LIST);
		break;
	    }

	  case PANEL_INACTIVE:
	    if (avlist[1] && ip->panel->kbd_focus_item == ip) {
		dp->has_kbd_focus = FALSE;
		if (dp->initialized)
		    paint_list_box(dp);
		ip->panel->kbd_focus_item = panel_next_kbd_focus(ip->panel);
		panel_accept_kbd_focus(ip->panel);
	    }
	    if (dp->list_sb)
		xv_set(dp->list_sb, SCROLLBAR_INACTIVE, avlist[1], 0);
	    break;

	  case PANEL_READ_ONLY:
	    dp->read_only = avlist[1] ? TRUE : FALSE;
	    break;

	  case XV_SHOW:
	    if (dp->text_item_row)
		xv_set(dp->text_item, XV_SHOW, avlist[1], 0);
	    if (dp->list_sb)
		xv_set(dp->list_sb, XV_SHOW, avlist[1], 0);
	    break;

	  case XV_END_CREATE:
	    panel_list_create_displayarea(dp);
	    dp->initialized = TRUE;
	    xv_end_create = TRUE;
	    break;

	  default:
	    break;
	}
    }
#endif OW_I18N

    /* Non-exclusive lists can have no current row */
    if (!dp->choose_one)
	dp->choose_none = TRUE;

    if (!dp->choose_none && dp->rows && !dp->setting_current_row) {
	/* If no row is selected, then select the first row */
	for (row = dp->rows; row; row = row->next)
	    if (row->selected)
		break;
	if (!row) {
	    dp->current_row = dp->rows;
	    dp->current_row->selected = TRUE;
	}
    }

    if (reset_rows && dp->initialized) {
	dp->list_box.r_height = 2*LIST_BOX_BORDER_WIDTH +
	    2*ROW_MARGIN + dp->rows_displayed*dp->row_height;
	xv_set(dp->list_sb,
	    SCROLLBAR_VIEW_LENGTH, dp->rows_displayed,
	    XV_HEIGHT, dp->list_box.r_height,
	    0);
    }

    /* Horizontally align strings in row panel */
    max_glyph_width = 0;
    for (row = dp->rows; row; row = row->next)
	if (row->glyph) {
	    if (row->glyph->pr_width > max_glyph_width)
		max_glyph_width = row->glyph->pr_width;
	}
    dp->string_x = LIST_BOX_BORDER_WIDTH + ROW_MARGIN + PANEL_LIST_COL_GAP;
    if (max_glyph_width) {
	dp->string_x += max_glyph_width;
    }

    if (dp->initialized) {
	list_box_width_changed = fit_list_box_to_rows(dp);

	/*
	 * Calculate the string y-coordinate and displayed string length
	 * of each row.
	 */
	for (row=dp->rows; row; row=row->next) {
	    row->string_y = LIST_BOX_BORDER_WIDTH + ROW_MARGIN +
		row->row*dp->row_height;
	    if (!row->display_str_len || list_box_width_changed)
		set_row_display_str_length(dp, row);
	}

	/*
	 * Note: item_set_generic sets ip->rect to the enclosing rect of
	 * ip->label_rect and ip->value_rect. However, on XV_END_CREATE,
	 * item_set_generic gets called before panel_list_set_avlist.  So, if
	 * we're processing the XV_END_CREATE, we must also set ip->rect.
	 */
	ip->value_rect.r_width = dp->list_box.r_width +
	    (int) xv_get(dp->list_sb, XV_WIDTH);
	ip->value_rect.r_height = dp->list_box.r_height;
	switch (ip->layout) {
	  case PANEL_HORIZONTAL:
	    ip->value_rect.r_left = rect_right(&ip->label_rect) + 1 +
		(ip->label_rect.r_width ? LABEL_X_GAP : 0);
	    ip->value_rect.r_top = ip->label_rect.r_top;
	    break;
	  case PANEL_VERTICAL:
	    ip->value_rect.r_left = ip->label_rect.r_left;
	    ip->value_rect.r_top = rect_bottom(&ip->label_rect) + 1 +
		(ip->label_rect.r_height ? LABEL_Y_GAP : 0);
	    break;
	}
	dp->list_box.r_left = ip->value_rect.r_left;
	dp->list_box.r_top = ip->value_rect.r_top;
	parent_panel_rect = *(Rect *) xv_get(dp->parent_panel, XV_RECT);
	sb_x = parent_panel_rect.r_left;
	if (dp->left_hand_sb)
	    sb_x += dp->list_box.r_left;
	else
	    sb_x += rect_right(&dp->list_box) + 1;
	xv_set(dp->list_sb,
	    XV_X, sb_x,
	    XV_Y, parent_panel_rect.r_top + dp->list_box.r_top,
	    0);
	if (dp->left_hand_sb)
	    dp->list_box.r_left += (int) xv_get(dp->list_sb, XV_WIDTH);
	if (xv_end_create) {
	    ip->rect = panel_enclosing_rect(&ip->label_rect, &ip->value_rect);
	    panel_check_item_layout(ip);

	    /* Create Row Panel menus, if not user defined.  */
	    if (!dp->read_menu) {
		server = XV_SERVER_FROM_WINDOW(PANEL_PUBLIC(ip->panel));
		dp->read_menu = xv_create(server, MENU,
		   XV_KEY_DATA, PANEL_LIST, dp,
#ifdef OW_I18N
		   MENU_TITLE_ITEM, XV_I18N_MSG("xv_messages", "Scrolling List"),
#else
		   MENU_TITLE_ITEM, "Scrolling List",
#endif OW_I18N
		   MENU_ITEM,
			MENU_STRING, dp->choose_one ?
#ifdef OW_I18N
			    XV_I18N_MSG("xv_messages", "Locate Choice") :
			    XV_I18N_MSG("xv_messages", "Locate Next Choice"),
#else
			    "Locate Choice" : "Locate Next Choice",
#endif OW_I18N
			MENU_ACTION,	locate_next_choice,
			0,
		    0);
		if (!dp->choose_one)
		    xv_set(dp->read_menu,
			MENU_APPEND_ITEM,
			    xv_create(NULL, MENUITEM,
#ifdef OW_I18N
				MENU_STRING, XV_I18N_MSG("xv_messages", "Clear All Choices"),
#else
				MENU_STRING, "Clear All Choices",
#endif OW_I18N
				MENU_ACTION, clear_all_choices,
				0),
			0);
		if (!dp->read_only) {
		    xv_set(dp->read_menu,
			MENU_APPEND_ITEM,
			    xv_create(NULL, MENUITEM,
#ifdef OW_I18N
				MENU_STRING, XV_I18N_MSG("xv_messages", "Edit List"),
#else
				MENU_STRING, "Edit List",
#endif OW_I18N
				MENU_ACTION, enter_edit_mode,
				0),
		        0);
		    if (!dp->edit_menu) {
			dp->edit_menu = xv_create(server, MENU,
			    XV_KEY_DATA, PANEL_LIST, dp,
#ifdef OW_I18N
			    MENU_TITLE_ITEM, XV_I18N_MSG("xv_messages", "Scrolling List"),
#else
			    MENU_TITLE_ITEM, "Scrolling List",
#endif OW_I18N
			    MENU_ITEM,
#ifdef OW_I18N
				MENU_STRING, XV_I18N_MSG("xv_messages", "Change"),
#else
				MENU_STRING, "Change",
#endif OW_I18N
				MENU_ACTION, change_proc,
				0,
			    MENU_ITEM,
#ifdef OW_I18N
				MENU_STRING, XV_I18N_MSG("xv_messages", "Insert"),
#else
				MENU_STRING, "Insert",
#endif OW_I18N
				MENU_PULLRIGHT,
				    xv_create(server, MENU,
					XV_KEY_DATA, PANEL_LIST, dp,
					MENU_ITEM,
#ifdef OW_I18N
					    MENU_STRING, XV_I18N_MSG("xv_messages", "Before"),
#else
					    MENU_STRING, "Before",
#endif OW_I18N
					    MENU_ACTION, insert_proc,
					    XV_KEY_DATA, PANEL_INSERT,
						INSERT_BEFORE,
					    0,
					MENU_ITEM,
#ifdef OW_I18N
					    MENU_STRING, XV_I18N_MSG("xv_messages", "After"),
#else
					    MENU_STRING, "After",
#endif OW_I18N
					    MENU_ACTION, insert_proc,
					    XV_KEY_DATA, PANEL_INSERT,
						INSERT_AFTER,
					    0,
					0),
				0,
			    MENU_ITEM,
#ifdef OW_I18N
				MENU_STRING, XV_I18N_MSG("xv_messages", "Delete"),
#else
				MENU_STRING, "Delete",
#endif OW_I18N
				MENU_ACTION, delete_proc,
				0,
			    MENU_ITEM,
				MENU_STRING,	"",
				MENU_FEEDBACK, FALSE,
				0,
			    MENU_ITEM,
#ifdef OW_I18N
				MENU_STRING, XV_I18N_MSG("xv_messages", "End Editing"),
#else
				MENU_STRING, "End Editing",
#endif OW_I18N
				MENU_ACTION, enter_read_mode,
				0,
			    0);
		    }   /* if (!dp->edit_menu) */
		}   /* if (!dp->read_only) */
	    }   /* if (!dp->read_menu) */
	}   /* if (xv_end_create) */
	else {
	    xv_set(dp->list_sb,
		   SCROLLBAR_OBJECT_LENGTH, dp->nrows,
		   0);
	}
    }	/* if (dp->initialized) */

    return XV_OK;
}


Pkg_private     Xv_opaque
panel_list_get_attr(panel_list_public, status, which_attr, valist)
    Panel_item      panel_list_public;
    int            *status;
    Attr_attribute  which_attr;
    va_list         valist;
{
    Panel_list_info *dp = PANEL_LIST_PRIVATE(panel_list_public);

    switch (which_attr) {

      case PANEL_CHOOSE_NONE:
	return ((Xv_opaque) dp->choose_none);

      case PANEL_CHOOSE_ONE:
	return ((Xv_opaque) dp->choose_one);

      case PANEL_EVENT_PROC:
	return ((Xv_opaque) dp->event_proc);

      case PANEL_LIST_WIDTH:
	return ((Xv_opaque) dp->width);

      case PANEL_LIST_DISPLAY_ROWS:
	return ((Xv_opaque) dp->rows_displayed);

      case PANEL_LIST_ROW_HEIGHT:
	return (Xv_opaque) dp->row_height;

      case PANEL_LIST_SCROLLBAR:
	return (Xv_opaque) dp->list_sb;

      case PANEL_LIST_SELECTED:{
	    int             row = *(int *) valist;
	    Row_info       *node = find_or_create_nth_row(dp, row, FALSE);
	    return (node ? (Xv_opaque) node->selected : (Xv_opaque) XV_ERROR);
	}

      case PANEL_LIST_CLIENT_DATA:{
	    int             row = *(int *) valist;
	    Row_info       *node = find_or_create_nth_row(dp, row, FALSE);
	    return (node ? (Xv_opaque) node->client_data : (Xv_opaque) XV_ERROR);
	}

#ifdef OW_I18N
      case PANEL_LIST_STRING_WCS:{
	    int             row = *(int *) valist;
	    Row_info       *node = find_or_create_nth_row(dp, row, FALSE);
	    return (node ? (Xv_opaque) node->string_wc : (Xv_opaque) XV_ERROR);
	}
      case PANEL_LIST_STRING:{
	    int             row = *(int *) valist;
	    Row_info       *node = find_or_create_nth_row(dp, row, FALSE);

	    node->string = (u_char * ) 
			wcstombsdup ((wchar_t *)node->string_wc);
	    return (node ? (Xv_opaque) node->string : (Xv_opaque) XV_ERROR);
	}
#else
      case PANEL_LIST_STRING:{
	    int             row = *(int *) valist;
	    Row_info       *node = find_or_create_nth_row(dp, row, FALSE);
	    return (node ? (Xv_opaque) node->string : (Xv_opaque) XV_ERROR);
	}
#endif /* OW_I18N */

      case PANEL_LIST_GLYPH:{
	    int             row = *(int *) valist;
	    Row_info       *node = find_or_create_nth_row(dp, row, FALSE);
	    return (node ? (Xv_opaque) node->glyph : (Xv_opaque) XV_ERROR);
	}

      case PANEL_LIST_FONT:{
	    int             row = *(int *) valist;
	    Row_info       *node = find_or_create_nth_row(dp, row, FALSE);
	    return (node ? (Xv_opaque) node->font : (Xv_opaque) XV_ERROR);
	}

      case PANEL_LIST_NROWS:
	return ((Xv_opaque) dp->nrows);

      case PANEL_ITEM_MENU:
	return ((Xv_opaque) dp->edit_mode ? dp->edit_menu :
	    dp->read_menu);

      case PANEL_READ_ONLY:
	return ((Xv_opaque) dp->read_only);

      default:
	*status = XV_ERROR;
	return ((Xv_opaque) 0);
    }
}


static void
panel_list_paint(ip)
    register Item_info *ip;
{
    Panel_list_info *dp = PANEL_LIST_FROM_ITEM(ip);

    /* Paint the label.  The value is the scrolling list panel.  Since
     * the panel (which is a window) will get its own repaint event,
     * it is unnecessary to paint it here.
     */
    panel_paint_image(ip->panel, &ip->label, &ip->label_rect, inactive(ip),
		      ip->color_index);
    if (dp->initialized)
	paint_list_box(dp);
}


static void
paint_list_box(dp)
    Panel_list_info *dp;
{
    Xv_Drawable_info *info;
    Item_info	   *ip = ITEM_FROM_PANEL_LIST(dp);
    Row_info	   *row;
    Xv_Window	    pw;
    Xv_Screen      screen;
    GC             *openwin_gc_list;

    /* Paint list box border.
     *   Dashed = Scrolling List does not have keyboard focus
     *   Solid = Scrolling List has keyboard focus
     */
    paint_list_box_border(dp);

    /* Paint (visible) rows */
    for (row=dp->rows; row; row=row->next)
	paint_row(dp, row);

    if (inactive(ip)) {
	PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	    DRAWABLE_INFO_MACRO(pw, info);
	    screen = xv_screen(info);
	    openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY );
	    openwin_check_gc_color(info, OPENWIN_INACTIVE_GC);
	    XFillRectangle(xv_display(info), xv_xid(info),
			   openwin_gc_list[OPENWIN_INACTIVE_GC],
			   dp->list_box.r_left, dp->list_box.r_top,
			   dp->list_box.r_width, dp->list_box.r_height);
	PANEL_END_EACH_PAINT_WINDOW
    }
}


static void
paint_list_box_border(dp)
    Panel_list_info *dp;
{
    Display	   *display;
    GC		    gc;
    int		    gc_mask;
    XGCValues	    gc_values;
    Xv_Drawable_info *info;
    Item_info	   *ip = ITEM_FROM_PANEL_LIST(dp);
    Xv_window	    pw;
    Drawable	    xid;
    Xv_Screen      screen;
    GC             *openwin_gc_list;

    /* Paint list box border */
    PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	DRAWABLE_INFO_MACRO(pw, info);
        screen = xv_screen(info);
        openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY );
	display = xv_display(info);
	xid = xv_xid(info);
	if (ip->panel->three_d) {
	    olgx_draw_box(ip->panel->ginfo, xid,
			  dp->list_box.r_left,
			  dp->list_box.r_top,
			  dp->list_box.r_width,
			  dp->list_box.r_height,
			  dp->has_kbd_focus ? OLGX_INVOKED : OLGX_NORMAL,
			  FALSE);
	} else {
	    if (ip->color_index >= 0) {
		gc = openwin_gc_list[OPENWIN_NONSTD_GC];
		XSetForeground(display, openwin_gc_list[OPENWIN_NONSTD_GC],
		    xv_get(xv_cms(info), CMS_PIXEL, ip->color_index));
		if (dp->has_kbd_focus) {
		    gc_values.line_style = LineSolid;
		    gc_mask = GCLineStyle;
		} else {
		    gc_values.line_style = LineDoubleDash;
		    gc_values.dashes = 1;
		    gc_mask = GCLineStyle | GCDashList;
		}
		XChangeGC(display, gc, gc_mask, &gc_values);
	    } else {
		if (dp->has_kbd_focus)
		    gc = openwin_gc_list[OPENWIN_SET_GC];
		else
		    gc = openwin_gc_list[OPENWIN_DIM_GC];
	    }
	    XDrawRectangle(display, xid, gc,
			   dp->list_box.r_left,
			   dp->list_box.r_top,
			   dp->list_box.r_width - 1,
			   dp->list_box.r_height - 1);
	}
    PANEL_END_EACH_PAINT_WINDOW
}


static void
paint_row(dp, row)
    Panel_list_info *dp;
    Row_info *row;
{
    Display	   *display;
    int		    fg_pixval;	/* foreground pixel value */
    Pixfont	   *font;
#ifdef OW_I18N
    XFontSet	    font_set;
#endif OW_I18N
    int		    gc_mask;
    XGCValues	    gc_values;
    Xv_Drawable_info *info;
    Item_info	   *ip = ITEM_FROM_PANEL_LIST(dp);
    int		    first_row_in_view;   /* row number */
    GC		    gc;
    Xv_window	    pw;
    Rect	    row_rect;
    int		    save_black;
    Rect	    string_rect;
    Item_info	   *text_item_private;
    int		    view_start;  /* in pixels */
    Drawable	    xid;
    Xv_Screen      screen;
    GC             *openwin_gc_list;

    /* Insure that the row is completely visible within the list box.  */
    first_row_in_view = (int) xv_get(dp->list_sb, SCROLLBAR_VIEW_START);
    if (row->row < first_row_in_view ||
	row->row >= first_row_in_view + dp->rows_displayed)
	return;   /* row not within list box view */

    /* Get row rect. */
    view_start = dp->row_height * first_row_in_view;
    row_rect.r_top = row->string_y - view_start;
    row_rect.r_top += dp->list_box.r_top;
    row_rect.r_left = dp->list_box.r_left + LIST_BOX_BORDER_WIDTH +
	ROW_MARGIN;
    row_rect.r_width = dp->list_box.r_width - 2*LIST_BOX_BORDER_WIDTH -
	2*ROW_MARGIN;
    row_rect.r_height = dp->row_height;
    if (rect_bottom(&row_rect) > rect_bottom(&dp->list_box))
	row_rect.r_height = dp->list_box.r_height - row_rect.r_top;
#ifdef OW_I18N
    if (row->string_wc) {
#else
    if (row->string) {
#endif OW_I18N
	string_rect.r_left = dp->list_box.r_left + dp->string_x;
	string_rect.r_top = row_rect.r_top;
	string_rect.r_width = row_rect.r_width - dp->string_x +
	    LIST_BOX_BORDER_WIDTH + ROW_MARGIN;
	string_rect.r_height = row_rect.r_height;
    } else
	string_rect = row_rect;

    /* Clear row */
    PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	DRAWABLE_INFO_MACRO(pw, info);
	display = xv_display(info);
	xid = xv_xid(info);
	XClearArea(display, xid, row_rect.r_left, row_rect.r_top,
		   row_rect.r_width, row_rect.r_height, False);
	if (!row->show && row == dp->text_item_row) {
	    text_item_private = ITEM_PRIVATE(dp->text_item);
	    (*text_item_private->ops->paint) (text_item_private);
	}
    PANEL_END_EACH_PAINT_WINDOW

    if (!row->show)
	return;

    PANEL_EACH_PAINT_WINDOW(ip->panel, pw)
	DRAWABLE_INFO_MACRO(pw, info);
	screen = xv_screen(info);
	openwin_gc_list = (GC *) xv_get( screen, XV_KEY_DATA, OW_GC_KEY );
	display = xv_display(info);
	xid = xv_xid(info);
	if (ip->color_index >= 0)
	    fg_pixval = xv_get(xv_cms(info), CMS_PIXEL, ip->color_index);

	/* If 3D, read mode, and row is selected, then draw a recessed box */
	if (ip->panel->three_d && !dp->edit_mode && row->selected) {
	    if (ip->color_index >= 0) {
		save_black = olgx_get_single_color(ip->panel->ginfo,
						   OLGX_BLACK);
		olgx_set_single_color(ip->panel->ginfo, OLGX_BLACK, fg_pixval,
				      OLGX_SPECIAL);
	    }
	    olgx_draw_box(ip->panel->ginfo, xid,
		string_rect.r_left, string_rect.r_top,
		string_rect.r_width, string_rect.r_height,
		OLGX_INVOKED, TRUE);
	    if (ip->color_index >= 0)
		olgx_set_single_color(ip->panel->ginfo, OLGX_BLACK, save_black,
				      OLGX_SPECIAL);
	}

#ifdef OW_I18N
	/* Paint text */
	if (row->string_wc) {
	    if (ip->color_index >= 0) {
		XSetForeground(display, openwin_gc_list[OPENWIN_TEXT_GC],
		    fg_pixval);
	    }
	    if (row->font) {
		font = (Pixfont *) row->font;
	    } else {
		font = dp->font;
	    }
	    font_set = (XFontSet) xv_get(font, FONT_SET_ID);
	    XwcDrawString(display,  xid, font_set, 
			openwin_gc_list[OPENWIN_TEXT_GC],
			string_rect.r_left,
			string_rect.r_top + PANEL_LIST_ROW_GAP/2 +
			    panel_fonthome(font),
			row->string_wc, row->display_str_len);
	    if (ip->color_index >= 0) {
		XSetForeground(display, openwin_gc_list[OPENWIN_TEXT_GC],
			       xv_fg(info));
	    }
	}
#else
	/* Paint text */
	if (row->string) {
	    if (ip->color_index >= 0) {
		XSetForeground(display, openwin_gc_list[OPENWIN_TEXT_GC],
		    fg_pixval);
	    }
	    if (row->font) {
		font = (Pixfont *) row->font;
		XSetFont(display, openwin_gc_list[OPENWIN_TEXT_GC],
			 xv_get(font, XV_XID));
	    } else
		font = dp->font;
	    XDrawString(display, xid, openwin_gc_list[OPENWIN_TEXT_GC],
			string_rect.r_left,
			string_rect.r_top + PANEL_LIST_ROW_GAP/2 +
			    panel_fonthome(font),
			row->string, row->display_str_len);
	    if (row->font) {
		/* Set the text gc back to the standard font */
		XSetFont(display, openwin_gc_list[OPENWIN_TEXT_GC],
			 ip->panel->std_font_xid);
	    }
	    if (ip->color_index >= 0) {
		XSetForeground(display, openwin_gc_list[OPENWIN_TEXT_GC],
			       xv_fg(info));
	    }
	}
#endif OW_I18N

	if (dp->edit_mode && row->edit_selected) {
	    /* Invert text */
	    panel_pw_invert(pw, &string_rect, ip->color_index);
	}

	/* Paint glyph */
	if (row->glyph)
	    panel_paint_svrim(pw, row->glyph, row_rect.r_left, row_rect.r_top,
			      ip->color_index);

	if (row->selected &&
	    (!ip->panel->three_d || dp->edit_mode)) {
	    /* Box text */
	    if (ip->color_index < 0) {
		if (!dp->edit_mode)
		    gc = openwin_gc_list[OPENWIN_SET_GC];
		else
		    gc = openwin_gc_list[OPENWIN_DIM_GC];
	    } else {
		gc = openwin_gc_list[OPENWIN_NONSTD_GC];
		XSetForeground(display, gc, fg_pixval);
		if (!dp->edit_mode) {
		    gc_values.line_style = LineSolid;
		    gc_mask = GCLineStyle;
		} else {
		    gc_values.line_style = LineDoubleDash;
		    gc_values.dashes = 1;
		    gc_mask = GCLineStyle | GCDashList;
		}
		XChangeGC(display, gc, gc_mask, &gc_values);
	    }
	    XDrawRectangle(display, xid, gc,
		       string_rect.r_left, string_rect.r_top,
		       string_rect.r_width - 1, string_rect.r_height - 1);
	}
    PANEL_END_EACH_PAINT_WINDOW
}


static void
panel_list_remove(ip)
    register Item_info *ip;
{
    register Panel_info *panel = ip->panel;
    register Panel_list_info *dp = PANEL_LIST_FROM_ITEM(ip);

    if (panel_seln(panel, SELN_CARET)->ip == ip)
	panel_seln_cancel(panel, SELN_CARET);

    /*
     * Only reassign the keyboard focus to another item if the panel isn't
     * being destroyed.
     */
    if (!panel->destroying && panel->kbd_focus_item == ip) {
	dp->has_kbd_focus = FALSE;
	if (panel->kbd_focus_item = panel_next_kbd_focus(panel))
	    (*panel->kbd_focus_item->ops->accept_kbd_focus) (
		panel->kbd_focus_item);
    }

    return;
}


static void
panel_list_layout(ip, deltas)
    Item_info      *ip;
    Rect           *deltas;
{
    Panel_list_info *dp = PANEL_LIST_FROM_ITEM(ip);

    if (!created(ip))
	return;
    dp->list_box.r_left += deltas->r_left;
    dp->list_box.r_top += deltas->r_top;
    if (dp->list_sb)
	xv_set(dp->list_sb,
	       XV_X, xv_get(dp->list_sb, XV_X) + deltas->r_left,
	       XV_Y, xv_get(dp->list_sb, XV_Y) + deltas->r_top,
	       0);
    if (dp->text_item)
	xv_set(dp->text_item,
	       XV_X, xv_get(dp->text_item, XV_X) + deltas->r_left,
	       XV_Y, xv_get(dp->text_item, XV_Y) + deltas->r_top,
	       0);
}


static void
panel_list_accept_kbd_focus(ip)
    Item_info	*ip;
{
    Panel_list_info *dp = PANEL_LIST_FROM_ITEM(ip);

    dp->has_kbd_focus = TRUE;
    paint_list_box_border(dp);
}


static void
panel_list_yield_kbd_focus(ip)
    Item_info	*ip;
{
    Panel_list_info *dp = PANEL_LIST_FROM_ITEM(ip);
    dp->has_kbd_focus = FALSE;
    paint_list_box_border(dp);
}


/*
 * Find/Create the row
 */
static Row_info *
find_or_create_nth_row(dp, which_row, create)
    Panel_list_info *dp;
    int             which_row;
    int             create;
{
    Row_info       *prev = NULL;
    Row_info       *node = dp->rows;

    while (node && (node->row != which_row)) {
	prev = node;
	node = node->next;
    }
    if (!node && create) {
	node = xv_alloc(Row_info);
	if (prev) {
	    node->row = prev->row + 1;
	    prev->next = node;
	} else {
	    dp->rows = node;
	    node->row = 0;
	}
	node->next = NULL;
	node->prev = prev;
	/*
	 * It's possible that prev->row + 1 is not equal to which_row ie. the
	 * row is created at the end of the list
	 */
	node->string = NULL;
#ifdef OW_I18N
	node->string_wc = null_string_wc;
#endif OW_I18N
	node->free_string = FALSE;
	node->glyph = 0;
	node->show = TRUE;
	dp->nrows++;
    }
    return (node);
}


static Row_info *
next_row(dp, row, n)
    Panel_list_info *dp;
    Row_info       *row;
    int             n;
{
    Row_info       *node = NULL;

    if (!row) {
	dp->rows = row = xv_alloc(Row_info);
	dp->nrows = 1;
	row->prev = NULL;
    } else {
	if (n == 0) {		/* If it is the first row, return the row
				 * itself */
	    return (row);
	} else if (row->next)	/* Already created */
	    return (row->next);
	else {
	    node = xv_alloc(Row_info);
	    node->prev = row;
	    row->next = node;
	    row = node;
	    dp->nrows++;
	}
    }
    row->next = NULL;
    row->selected = FALSE;
    row->show = TRUE;
    row->row = n;
    row->string = NULL;
#ifdef OW_I18N
    row->string_wc = null_string_wc;
#endif OW_I18N
    row->free_string = FALSE;
    row->glyph = NULL;
    return (row);
}


static Row_info *
panel_list_insert_row(dp, which_row, show, repaint)
    Panel_list_info *dp;
    int             which_row;
    int		    show;
    int		    repaint;
{
    Row_info       *node = dp->rows;
    Row_info       *prev = NULL;
    Row_info       *row = xv_alloc(Row_info);

    while (node && (node->row != which_row)) {
	prev = node;
	node = node->next;
    }
    row->selected = FALSE;
    row->show = show;
    row->next = node;
    row->prev = prev;
    row->glyph = NULL;
    row->string = NULL;
#ifdef OW_I18N
    row->string_wc = null_string_wc;
#endif OW_I18N
    row->free_string = FALSE;

    /*
     * It's possible that prev->row + 1 is not equal to which_row ie. the row
     * is created at the end of the list
     */
    if (prev) {
	prev->next = row;
	row->row = prev->row + 1;
    } else {			/* Insert at the begining of the list */
	dp->rows = row;
	row->row = 0;
    }
    row->string_y = LIST_BOX_BORDER_WIDTH + ROW_MARGIN +
	row->row*dp->row_height;
    if (node) {
	node->prev = row;
    }
    while (node) {
	node->row++;
	node->string_y += dp->row_height;
	node = node->next;
    }
    dp->nrows++;
    if (dp->list_sb)
	xv_set(dp->list_sb,
	       SCROLLBAR_OBJECT_LENGTH, dp->nrows,  /* in rows */
	       0);

    if (repaint)
	paint_list_box(dp);

    return(row);
}


static void
panel_list_delete_row(dp, node, repaint)
    Panel_list_info *dp;
    Row_info        *node;
    int		     repaint;
{
    Row_info       *prev = node->prev;

    if (prev) {
	prev->next = node->next;
    } else {
	dp->rows = node->next;
    }

    if (node->next) {
	node->next->prev = prev;
    }
    /* Adjust the row numbers */
    prev = node;
    node = node->next;
    if (prev->free_string)
#ifdef OW_I18N
	xv_free((char *) prev->string_wc);
#else
	xv_free(prev->string);
#endif /* OW_I18N */
    xv_free(prev);
    while (node) {
	node->row--;
	node->string_y -= dp->row_height;
	node = node->next;
    }
    dp->nrows--;
    if (dp->list_sb)
	xv_set(dp->list_sb,
	       SCROLLBAR_OBJECT_LENGTH, dp->nrows,  /* in rows */
	       0);

    if (repaint) {
	/* Erase old rows */
	panel_clear_rect(PANEL_PRIVATE(dp->parent_panel), dp->list_box);
	/* Repaint list box and currently visible rows */
	paint_list_box(dp);
    }
}


static void
handle_menu_event(dp, event)
    Panel_list_info	*dp;
    Event		*event;
{
    int		    edit_cnt;
    Menu	    menu;
    Panel_item	    change_item;
    Panel_item	    delete_item;
    Panel_item	    insert_item;
    Item_info	   *ip = ITEM_FROM_PANEL_LIST(dp);
    Row_info	    *edit_row;
    Row_info	    *row;

    if (dp->edit_mode) {
	menu = dp->edit_menu;
	if (menu) {
	    /* Note: The client can change the edit menu */
	    change_item = xv_find(menu, MENUITEM,
		    XV_AUTO_CREATE, FALSE,
#ifdef OW_I18N
		    MENU_STRING, XV_I18N_MSG("xv_messages", "Change"),
#else
		    MENU_STRING, "Change",
#endif OW_I18N
		    0);
	    delete_item = xv_find(menu, MENUITEM,
		    XV_AUTO_CREATE, FALSE,
#ifdef OW_I18N
		    MENU_STRING, XV_I18N_MSG("xv_messages", "Delete"),
#else
		    MENU_STRING, "Delete",
#endif OW_I18N
		    0);
	    insert_item = xv_find(menu, MENUITEM,
		    XV_AUTO_CREATE, FALSE,
#ifdef OW_I18N
		    MENU_STRING, XV_I18N_MSG("xv_messages", "Insert"),
#else
		    MENU_STRING, "Insert",
#endif OW_I18N
		    0);
	    edit_row = dp->text_item_row;
	    if (change_item || delete_item) {
		edit_cnt = 0;
		for (row=dp->rows; row; row=row->next)
		    if (row->edit_selected && row != edit_row)
			edit_cnt++;
		if (change_item)
		    xv_set(change_item,
			MENU_INACTIVE, edit_row || edit_cnt != 1,
			0);
		if (delete_item)
		    xv_set(delete_item,
			MENU_INACTIVE, edit_cnt == 0,
			0);
	    }
	    if (insert_item)
		xv_set(insert_item,
		    MENU_INACTIVE, edit_row ? TRUE : FALSE,
		    0);
	}
    } else
	menu = dp->read_menu;
    if (menu) {
	xv_set(menu, MENU_COLOR, ip->color_index, 0);
	menu_show(menu, event_window(event), event, 0);
    }
}


static void
panel_list_handle_event(panel_list_public, event)
    Panel_item      panel_list_public;
    Event          *event;
{
    Panel_list_info *dp = PANEL_LIST_PRIVATE(panel_list_public);
    Item_info	   *ip = ITEM_FROM_PANEL_LIST(dp);
    Panel_info	   *panel = PANEL_PRIVATE(dp->parent_panel);
    Row_info	   *row;
    int		    row_nbr;
    struct timeval  wait;
    int		    y_offset;
#ifdef OW_I18N
    char	   action_string[2];
    static wchar_t ascii_char_wc[4]={'a', 'z', 'A', 'Z'};
    wchar_t 	   *tmp_str_wc;
    wchar_t	   *tmp_char_wc;
#endif /* OW_I18N */

    if (dp->event_proc) {
	(*dp->event_proc)(panel_list_public, event);
	return;
    }

    if (inactive(ip))
	return;

    if  (event_action(event) == SCROLLBAR_REQUEST) {
	/* Scroll request received from scrollbar */
	panel_clear_rect(panel, dp->list_box);
	paint_list_box(dp);
	return;
    } else if (event_is_iso(event)) {
        if (event_is_up(event))
            return;
	switch (event_action(event)) {
	  case RETURN:
	  case TAB:
	    if (event_shift_is_down(event))
		panel_set_kbd_focus(panel, panel_previous_kbd_focus(panel));
	    else
		panel_set_kbd_focus(panel, panel_next_kbd_focus(panel));
	    return;
	}
	/* Select the row starting with the character typed that is after
	 * the last character-matched row, if any.  Match is case-insensitive.
	 */
	if (dp->char_match_row)
	    row = dp->char_match_row->next;
	else
	    row = dp->rows;
#ifdef OW_I18N
	if (event_is_string(event)) {
	    tmp_str_wc = (wchar_t *) mbstowcsdup(event_string(event));
	    for (; row && row->string_wc; row=row->next) {
		if (row->string_wc[0] == *tmp_str_wc)
		    break;
		else if (*tmp_str_wc >= ascii_char_wc[0] &&
			 *tmp_str_wc <= ascii_char_wc[1] &&
			 row->string_wc[0] == *tmp_str_wc - 0x20)
		    break;
		else if (*tmp_str_wc >= ascii_char_wc[2] &&
			 *tmp_str_wc <= ascii_char_wc[3] &&
			 row->string_wc[0] == *tmp_str_wc + 0x20)
		    break;
	    }
	    free ((char *) tmp_str_wc);
	}
	else {
	    action_string[0] = event_action(event);
	    action_string[1] = '\0';
	    tmp_char_wc = mbstowcsdup(action_string);
	    for (; row && row->string_wc; row=row->next) {
		if (row->string_wc[0] == *tmp_char_wc)
		    break;
		else if (*tmp_char_wc >= ascii_char_wc[0] &&
		    *tmp_char_wc <= ascii_char_wc[1] &&
		    row->string_wc[0] == *tmp_char_wc - 0x20)
		    break;
		else if (*tmp_char_wc >= ascii_char_wc[2] &&
		    *tmp_char_wc <= ascii_char_wc[3] &&
		    row->string_wc[0] == *tmp_char_wc + 0x20)
		    break;
	    }
	}
#else
	for (; row && row->string; row=row->next) {
	    if (row->string[0] == event_action(event))
		break;
	    else if (event_action(event) >= 'a' &&
		event_action(event) <= 'z' &&
		row->string[0] == event_action(event) - 0x20)
		break;
	    else if (event_action(event) >= 'A' &&
		event_action(event) <= 'Z' &&
		row->string[0] == event_action(event) + 0x20)
		break;
	}
#endif /* OW_I18N */

	if (row) {
	    /* Character match: select row */
	    dp->char_match_row = row;
	} else {
	    /* No character match: beep user and deselect last char match */
	    wait.tv_sec = 0;
	    wait.tv_usec = 0;
	    win_bell(event_window(event), wait, 0);
	    if (dp->char_match_row) {
		row = dp->char_match_row;
		dp->char_match_row = NULL;
	    } else
		/* No previous character match */
		return;
	}
	check_row_in_view(dp, row->row);
	event_set_action(event, ACTION_SELECT);
    } else {
	/* Non-ascii event: find row */
	y_offset = event_y(event) - dp->list_box.r_top -
	    LIST_BOX_BORDER_WIDTH - ROW_MARGIN;
	if (y_offset < 0)
	    /* event above first row => first row */
	    y_offset = 0;
	if (y_offset >= dp->rows_displayed*dp->row_height)
	    /* event below last row => last row */
	    y_offset = dp->rows_displayed*dp->row_height - 1;
	row_nbr = (int) xv_get(dp->list_sb, SCROLLBAR_VIEW_START) +
	    y_offset / dp->row_height;
	for (row=dp->rows; row; row=row->next)
	    if (row->row == row_nbr)
		break;
	if (event_action(event) != ACTION_MENU) {
	    if (!row)
		/* Non-menu event not over a row: ignore */
		return;
	    if (!row->show) {
		/* Non-menu event is over text item in edit mode */
		panel_handle_event(dp->text_item, event);
		return;
	    }
	}
    }

    switch (event_action(event)) {

      case ACTION_SELECT:
	if (event_is_down(event)) {
	    if (dp->edit_mode)
		set_edit_row(dp, row, FALSE, event);
	    else
		set_current_row(dp, row, event);
	} else if (dp->edit_mode)
	    /* Possible end of SELECT-drag */
	    dp->last_edit_row = NULL;
	break;

      case ACTION_ADJUST:
	if (dp->edit_mode) {
	    if (event_is_down(event))
		set_edit_row(dp, row, TRUE, event);
	    else
		/* Possible end of ADJUST-drag */
		dp->last_edit_row = NULL;
	}
#ifdef QUESTION_MARK_CURSOR
	else {
	    /* BUG ALERT: display the question mark while ADJUST is down */
	}
#endif /* QUESTION_MARK_CURSOR */
	break;

      case ACTION_MENU:
	if (event_is_down(event))
	    handle_menu_event(dp, event);
	break;

      case LOC_DRAG:
	if (event_left_is_down(event)) {
	    if (dp->edit_mode) {
		if (dp->last_edit_row != row) {
		    set_edit_row(dp, row, TRUE, event);
		}
	    } else if (dp->current_row != row)
		set_current_row(dp, row, event);
	} else if (event_middle_is_down(event) && dp->edit_mode) {
	    if (dp->last_edit_row != row) {
		set_edit_row(dp, row, TRUE, event);
	    }
	}
	break;
    }
}


static void
set_current_row(dp, event_row, event)
    Panel_list_info	*dp;
    Row_info		*event_row;
    Event		*event;	/* NULL => not event generated */
{
    int		new_state = TRUE;
    int		toggle = FALSE;
    
    if (dp->choose_one) {
    	if (dp->current_row == event_row) {
    	    if (dp->choose_none)
    	    	toggle = TRUE;
    	} else if (dp->current_row) {
	    dp->setting_current_row = TRUE;
	    dp->current_row->selected = FALSE;
    	    show_feedback(dp, dp->current_row, event);
    	}
	if (toggle)
	    new_state = event_row->selected ? FALSE : TRUE;
	event_row->selected = new_state;
	show_feedback(dp, event_row, event);
	dp->setting_current_row = FALSE;
    } else {
    	new_state = event_row->selected ? FALSE : TRUE;
    	event_row->selected = new_state;
    	show_feedback(dp, event_row, event);
    }
    dp->current_row = event_row;
}


static void
set_edit_row(dp, event_row, toggle, event)
    Panel_list_info	*dp;
    Row_info		*event_row;	/* the row the event occurred in */
    int			toggle;	/* TRUE or FALSE */
    Event		*event;
{
    Row_info	*row;
    int		new_state = TRUE;
    
    if (!toggle) {
	event_row->edit_selected = TRUE;
	show_feedback(dp, event_row, event);
    	for (row=dp->rows; row; row=row->next) {
    	    if (row != event_row && row->edit_selected) {
    	    	row->edit_selected = FALSE;
    	        show_feedback(dp, row, event);
    	    }
    	}
    } else {
    	new_state = event_row->edit_selected ? FALSE : TRUE;
    	event_row->edit_selected = new_state;
    	show_feedback(dp, event_row, event);
    }
    dp->last_edit_row = event_row;
}


static void
unset_all_selected_items(dp)
    Panel_list_info *dp;
{
    Row_info       *row = dp->rows;

    while (row) {
	if (row->selected) {
	    row->selected = FALSE;
	    show_feedback(dp, row, NULL);
	}
	row = row->next;
    }
}


static void
panel_list_seln_function(dp, buffer)
    Panel_list_info *dp;
    Seln_function_buffer *buffer;
{
    Xv_object       server = xv_get(xv_get(dp->parent_panel, XV_SCREEN),
				    SCREEN_SERVER);
    Seln_holder    *holder;

    switch (selection_figure_response(server, buffer, &holder)) {
      case SELN_IGNORE:
      case SELN_REQUEST:
      case SELN_FIND:
      case SELN_DELETE:
	break;

      case SELN_SHELVE:
	if (selection_acquire(server, dp->seln_client, SELN_SHELF)
	    == SELN_SHELF) {
	    dp->have_seln[SHELF_CHOICE] = TRUE;
	    (void) selection_string(dp, SHELF_CHOICE);
	}
	break;
      default:
	break;
    }
}


static          Seln_result
panel_list_reply_proc(attr, context, max_length)
    Seln_attribute  attr;
    Seln_replier_data *context;
    int             max_length;
{
    Panel_list_info *dp = (Panel_list_info *) context->client_data;
    Bool            seln;

    switch (context->rank) {
      case SELN_PRIMARY:
	seln = dp->have_seln[PRIMARY_CHOICE];
	break;
      case SELN_SHELF:
	seln = dp->have_seln[SHELF_CHOICE];
	break;
      default:
	seln = FALSE;
	break;
    }

    if (!seln)
	return (SELN_DIDNT_HAVE);

    switch (attr) {
      case SELN_REQ_BYTESIZE:
	if (context->rank == SELN_PRIMARY)
	    *context->response_pointer++ =
		(char *) sizeof_selection(dp, seln);
	else
	    *context->response_pointer++ =
		(char *) strlen(dp->selection[SHELF_CHOICE]);
	return (SELN_SUCCESS);

      case SELN_REQ_CONTENTS_ASCII:
	return (get_selection(dp, context, max_length));

      case SELN_REQ_YIELD:
	unset_all_selected_items(dp);
	*context->response_pointer++ = (char *) SELN_SUCCESS;
	return (SELN_SUCCESS);

      case SELN_REQ_END_REQUEST:
	return (SELN_SUCCESS);

      default:
	return (SELN_UNRECOGNIZED);
    }
}


static int
sizeof_selection(dp)
    Panel_list_info *dp;
{
    Row_info       *row = dp->rows;
    int             nbytes = 0;
    int             how_many = 0;
    while (row) {
	if (row->selected && row->string) {
	    how_many++;
	    nbytes += strlen(row->string);
	}
	row = row->next;
    }
    return (nbytes + how_many);
}


static          Seln_result
get_selection(dp, context, length)
    Panel_list_info *dp;
    Seln_replier_data *context;
    int             length;
{
    int             size;
    char           *destp;
    int             needed;

    if (context->context == NULL) {
	/*
	 * if this was called the first time, get selection from the selected
	 * items. If the rank is SELN_SHELF, selection_string already exists
	 */
	if (context->rank == SELN_PRIMARY)
	    context->context = selection_string(dp, PRIMARY_CHOICE);
	else
	    context->context = dp->selection[SHELF_CHOICE];
    }
    size = strlen(context->context);
    destp = (char *) context->response_pointer;

    needed = size + 4;
    if (size % 4 != 0)
	needed += 4 - size % 4;
    if (needed <= length) {
	/* Entire selection fits */
	strcpy(destp, context->context);
	destp += size;
	while ((int) destp % 4 != 0)
	    *destp++ = '\0';
	context->response_pointer = (char **) destp;
	*context->response_pointer++ = 0;
	return (SELN_SUCCESS);
    } else {
	strncpy(destp, context->context, length);
	destp += length;
	context->response_pointer = (char **) destp;
	context->context += length;
	return (SELN_CONTINUED);
    }
}


static char *
selection_string(dp, choice)
    Panel_list_info *dp;
    int             choice;
{
    char           *temp;
    int             size = sizeof_selection(dp);
    Row_info       *row = dp->rows;
    int             how_many = 0;
    u_char         *str;

    if (dp->selection[choice] != NULL)
	xv_free(dp->selection[choice]);
    dp->selection[choice] = temp = xv_alloc_n(char *, size);
    while (row) {
	if (row->selected && row->string) {
	    if (how_many)
		*temp++ = '\n';
	    how_many++;
	    str = row->string;
	    bcopy(str, temp, strlen(str));
	    temp += strlen(str);
	}
	row = row->next;
    }
    return (dp->selection[choice]);
}


static void
show_feedback(dp, row, event)
    Panel_list_info *dp;
    Row_info *row;
    Event *event;	/* NULL => feedback was not event generated */
{
    int		(*notify_proc)();
    Item_info	*panel_list_ip = ITEM_PRIVATE(dp->public_self);

    if (!hidden(panel_list_ip))
	paint_row(dp, row);
#ifdef OW_I18N
    if (!dp->edit_mode) {
	if (dp->wchar_notify) {
	    notify_proc = panel_list_ip->notify_wc;
	    if (notify_proc && event) {
		(*notify_proc) (dp->public_self, row->string_wc, 
				row->client_data, row->selected ? 
				PANEL_LIST_OP_SELECT : 
				PANEL_LIST_OP_DESELECT, event);
	    }
	}
	else {
	    notify_proc = panel_list_ip->notify;
	    if (notify_proc && event) {
		row->string = (u_char *) wcstombsdup ((wchar_t *)
					row->string_wc);
		(*notify_proc) (dp->public_self, row->string, 
				row->client_data, row->selected ? 
				PANEL_LIST_OP_SELECT : 
				PANEL_LIST_OP_DESELECT, event);
	    }
	}
    }
#else
    if (!dp->edit_mode) {
	notify_proc = panel_list_ip->notify;
	if (notify_proc && event) {
	    (*notify_proc) (dp->public_self, row->string, row->client_data,
		row->selected ? PANEL_LIST_OP_SELECT : PANEL_LIST_OP_DESELECT,
		event);
	}
    }
#endif /* OW_I18N */
}


static void
check_row_in_view(dp, desired_row_nbr)
    Panel_list_info *dp;
    int desired_row_nbr;
{
    int			first_row_nbr_in_view;

    /* If desired row is out of the view window, then scroll desired row
     * to the top of the view window.
     */
    first_row_nbr_in_view = (int) xv_get(dp->list_sb,
	SCROLLBAR_VIEW_START);
    if (desired_row_nbr < first_row_nbr_in_view ||
	desired_row_nbr >= first_row_nbr_in_view + dp->rows_displayed) {
	desired_row_nbr = MIN(desired_row_nbr,
	    dp->nrows - dp->rows_displayed);
	xv_set(dp->list_sb, SCROLLBAR_VIEW_START, desired_row_nbr, 0);
    }
}


static Xv_opaque
locate_next_choice(menu, menu_item)	/*ARGSUSED*/
    Menu menu;
    Menu_item menu_item;
{
    register Panel_list_info *dp;
    Row_info *first_row;
    Row_info *row;
    int view_end;
    int view_start;  /* in pixels */
    
    dp = (Panel_list_info *) xv_get(menu, XV_KEY_DATA, PANEL_LIST);

    if (!dp->rows)
	return(XV_OK);  /* no rows */

    /* Determine the range of the view */
    view_start = dp->row_height *
	(int) xv_get(dp->list_sb, SCROLLBAR_VIEW_START);
    view_end = view_start +
	dp->rows_displayed * dp->row_height - 1;

    /* Find the first row beyond the view (if any) */
    for (first_row=dp->rows; first_row; first_row=first_row->next) {
	if (first_row->string_y >= view_end)
	    break;
    }
    if (!first_row)
	first_row = dp->rows;

    /* Find the next selected row (if any) */
    row = first_row;
    do {
	if (row->selected)
	    break;
	row = row->next;
	if (!row)
	    row = dp->rows;
    } while (row!=first_row);

    /* Scroll the display to this row */
    check_row_in_view(dp, row->row);
    return(XV_OK);
}


static Xv_opaque
clear_all_choices(menu, menu_item)	/*ARGSUSED*/
    Menu menu;
    Menu_item menu_item;
{
    register Panel_list_info *dp;
    Event	*event = (Event *) xv_get(menu, MENU_LAST_EVENT);
    Row_info	*row;
    
    dp = (Panel_list_info *) xv_get(menu, XV_KEY_DATA, PANEL_LIST);
    for (row=dp->rows; row; row=row->next) {
	if (row->selected) {
	    row->selected = FALSE;
	    show_feedback(dp, row, event);
	}
    }
    return(XV_OK);
}


static Xv_opaque
enter_edit_mode(menu, menu_item)	/*ARGSUSED*/
    Menu menu;
    Menu_item menu_item;
{
    register Panel_list_info *dp;
    int			display_length;
    Item_info		*panel_list_ip;
    Row_info		*row;
    int			text_width, chrwth;
    
    dp = (Panel_list_info *) xv_get(menu, XV_KEY_DATA, PANEL_LIST);
    dp->edit_mode = TRUE;

    /*
     *** Create a text field for editing ***
     */
    /* Determine the panel text field's display length, in characters. */
    text_width = dp->list_box.r_width - LIST_BOX_BORDER_WIDTH -
	ROW_MARGIN - dp->string_x;
    chrwth = xv_get(dp->font, FONT_DEFAULT_CHAR_WIDTH);
    display_length = text_width / chrwth;
    
    /* Create the text field */
    panel_list_ip = ITEM_PRIVATE(dp->public_self);
#ifdef OW_I18N
    dp->text_item = xv_create(dp->parent_panel, PANEL_TEXT,
	PANEL_ITEM_OWNER, PANEL_LIST_PUBLIC(dp),
	PANEL_ITEM_COLOR, panel_list_ip->color_index,
    	PANEL_ITEM_X,	dp->list_box.r_left + dp->string_x,
    	PANEL_NOTIFY_STRING,	"\r",   /* RETURN only */
    	PANEL_VALUE_DISPLAY_LENGTH,	display_length,
	XV_KEY_DATA, PANEL_LIST, dp,
    	XV_KEY_DATA, PANEL_NOTIFY_PROC_WCS, panel_list_ip->notify_wc,
	XV_SHOW, FALSE,
    	0);
#else
    dp->text_item = xv_create(dp->parent_panel, PANEL_TEXT,
	PANEL_ITEM_OWNER, PANEL_LIST_PUBLIC(dp),
	PANEL_ITEM_COLOR, panel_list_ip->color_index,
    	PANEL_ITEM_X,	dp->list_box.r_left + dp->string_x,
    	PANEL_NOTIFY_STRING,	"\r",   /* RETURN only */
    	PANEL_VALUE_DISPLAY_LENGTH,	display_length,
	XV_KEY_DATA, PANEL_LIST, dp,
    	XV_KEY_DATA, PANEL_NOTIFY_PROC, panel_list_ip->notify,
	XV_SHOW, FALSE,
    	0);
#endif /* OW_I18N */
    dp->text_item_row = NULL;   /* no row being edited */

    /* Repaint boxes around selected rows */
    for (row=dp->rows; row; row=row->next)
	if (row->selected)
	    paint_row(dp, row);

    return(XV_OK);
}


static void
accept_change(text_item, edit_row)
    Panel_item	text_item;
    Row_info	*edit_row;
{
    Panel_list_info *dp;

    dp = (Panel_list_info *) xv_get(text_item, XV_KEY_DATA, PANEL_LIST);
    panel_set_kbd_focus(PANEL_PRIVATE(dp->parent_panel),
	ITEM_FROM_PANEL_LIST(dp));
    xv_set(text_item, XV_SHOW, FALSE, 0);
    xv_set(dp->list_sb, SCROLLBAR_INACTIVE, FALSE, 0);
    dp->text_item_row = NULL;   /* no row being edited */
#ifdef OW_I18N
    edit_row->string_wc = (wchar_t *) panel_strsave_wc((wchar_t *) xv_get(text_item, PANEL_VALUE_WCS));
#else
    edit_row->string = panel_strsave((u_char *) xv_get(text_item, PANEL_VALUE));
#endif OW_I18N
    edit_row->free_string = TRUE;
    edit_row->show = TRUE;
    set_row_display_str_length(dp, edit_row);
    paint_row(dp, edit_row);
}


static Panel_setting
change_done(text_item, event)
    Panel_item	text_item;
    Event	*event;
{
    int		(*notify_proc)();
    Panel_list_info *dp;
    
    /* Validate entry */
    dp = (Panel_list_info *) xv_get(text_item, XV_KEY_DATA, PANEL_LIST);
#ifdef OW_I18N
    if (dp->wchar_notify) {
        notify_proc = (int (*)()) xv_get(text_item,
    			XV_KEY_DATA, PANEL_NOTIFY_PROC_WCS);
	if (notify_proc &&
	    (*notify_proc) (dp->public_self,
			    (wchar_t *) xv_get(text_item, PANEL_VALUE_WCS),
			    dp->text_item_row->client_data,
			    PANEL_LIST_OP_VALIDATE,
			    event)
	    == XV_ERROR)
		return(PANEL_NONE);
    }
    else {
        notify_proc = (int (*)()) xv_get(text_item,
			XV_KEY_DATA, PANEL_NOTIFY_PROC);
	if (notify_proc &&
	    (*notify_proc) (dp->public_self,
			    (char *) xv_get(text_item, PANEL_VALUE),
			    dp->text_item_row->client_data,
			    PANEL_LIST_OP_VALIDATE,
			    event)
	    == XV_ERROR)
		return(PANEL_NONE);
    }
#else
    notify_proc = (int (*)()) xv_get(text_item,
        XV_KEY_DATA, PANEL_NOTIFY_PROC);
    if (notify_proc &&
        (*notify_proc) (dp->public_self,
                        (char *) xv_get(text_item, PANEL_VALUE),
                        dp->text_item_row->client_data,
                        PANEL_LIST_OP_VALIDATE,
                        event)
        == XV_ERROR)
        return(PANEL_NONE);
#endif /* OW_I18N */

    /* Entry validated.  Update edit row. */
    accept_change(text_item, dp->text_item_row);

    dp->edit_op = OP_NONE;
    return(PANEL_NONE);
}


static Xv_opaque
change_proc(menu, menu_item)	/*ARGSUSED*/
    Menu menu;
    Menu_item menu_item;
{
    register Panel_list_info *dp;
    Row_info	*edit_row = NULL;
    int		first_row_in_view;
    int		item_y;

    dp = (Panel_list_info *) xv_get(menu, XV_KEY_DATA, PANEL_LIST);
    for (edit_row=dp->rows; edit_row; edit_row=edit_row->next) {
    	if (edit_row->edit_selected)
	    break;
    }
    if (!edit_row || !dp->text_item)
    	return(XV_OK);	/* shouldn't occur, but let's be safe */

    edit_row->show = FALSE;
    paint_row(dp, edit_row);	/* clear glyph and string */

    /* Overlay text item over blank row */
    first_row_in_view = (int) xv_get(dp->list_sb, SCROLLBAR_VIEW_START);
    xv_set(dp->list_sb, SCROLLBAR_INACTIVE, TRUE, 0);
    item_y = dp->list_box.r_top + edit_row->string_y -
	first_row_in_view * dp->row_height;
    xv_set(dp->text_item,
    	PANEL_ITEM_Y,	item_y,
    	PANEL_NOTIFY_PROC,	change_done,
#ifdef OW_I18N
    	PANEL_VALUE_WCS,	edit_row->string_wc,
#else
    	PANEL_VALUE,	edit_row->string,
#endif OW_I18N
	PANEL_TEXT_SELECT_LINE,
	XV_SHOW,	TRUE,
	0);
    dp->text_item_row = edit_row;

    /* Warp the pointer to the bottom right corner of the text item */
    xv_set(dp->parent_panel,
	   WIN_MOUSE_XY,
	        rect_right(&dp->list_box) - LIST_BOX_BORDER_WIDTH -
		   ROW_MARGIN,
		item_y + dp->row_height - 1,
	    0);

    /* Transfer keyboard focus to the text item */
    panel_set_kbd_focus(PANEL_PRIVATE(dp->parent_panel),
	ITEM_PRIVATE(dp->text_item));
    
    dp->edit_op = OP_CHANGE;
    return(XV_OK);
}


static void
accept_insert(dp, row)
    Panel_list_info *dp;
    Row_info	    *row;
{

    PANEL_PRIVATE(dp->parent_panel)->kbd_focus_item = NULL;
    xv_set(dp->text_item, XV_SHOW, FALSE, 0);
    xv_set(dp->list_sb, SCROLLBAR_INACTIVE, FALSE, 0);
    dp->text_item_row = NULL;   /* no row being edited */
#ifdef OW_I18N
    row->string_wc = (wchar_t *) panel_strsave_wc((wchar_t *) xv_get(dp->text_item, PANEL_VALUE_WCS));
#else
    row->string = panel_strsave((u_char *) xv_get(dp->text_item, PANEL_VALUE));
#endif OW_I18N
    row->free_string = TRUE;
    row->show = TRUE;
    set_row_display_str_length(dp, row);
    paint_row(dp, row);
}

	
static Panel_setting
insert_done(text_item, event)
    Panel_item	text_item;
    Event	*event;
{
    int		(*notify_proc)();
    int		first_row_in_view;
    int		item_y;
    Panel_list_info *dp;
    Row_info	*insert_row;
    
    dp = (Panel_list_info *) xv_get(text_item, XV_KEY_DATA, PANEL_LIST);
    insert_row = dp->text_item_row;

    /* If value is empty, then exit insert mode. */
#ifdef OW_I18N
    if (wslen(xv_get(text_item, PANEL_VALUE_WCS)) == 0) {
#else
    if (strlen(xv_get(text_item, PANEL_VALUE)) == 0) {
#endif OW_I18N
	panel_set_kbd_focus(PANEL_PRIVATE(dp->parent_panel),
	    ITEM_FROM_PANEL_LIST(dp));
	xv_set(text_item, XV_SHOW, FALSE, 0);
	xv_set(dp->list_sb, SCROLLBAR_INACTIVE, FALSE, 0);
	dp->text_item_row = NULL;   /* no row being edited */
	panel_list_delete_row(dp, insert_row, REPAINT_LIST);
	dp->edit_op = OP_NONE;
	return(PANEL_NONE);
    }

    /* Validate entry */

#ifdef OW_I18N
    if (dp->wchar_notify) {
	notify_proc = (int (*)()) xv_get(text_item,
	    XV_KEY_DATA, PANEL_NOTIFY_PROC_WCS);
	if (notify_proc &&
	    (*notify_proc) (dp->public_self,
		(wchar_t *) xv_get(text_item, PANEL_VALUE_WCS),
		(Xv_opaque) insert_row->row, /* pass row # as client data */
		PANEL_LIST_OP_VALIDATE,
		event)
	    == XV_ERROR)
	    return(PANEL_NONE);
    }
    else {
	notify_proc = (int (*)()) xv_get(text_item,
	    XV_KEY_DATA, PANEL_NOTIFY_PROC);
	if (notify_proc &&
	    (*notify_proc) (dp->public_self,
		(char *) xv_get(text_item, PANEL_VALUE),
		(Xv_opaque) insert_row->row, /* pass row # as client data */
		PANEL_LIST_OP_VALIDATE,
		event)
	    == XV_ERROR)
	    return(PANEL_NONE);
   }
#else
    if (notify_proc &&
	(*notify_proc) (dp->public_self,
	    (char *) xv_get(text_item, PANEL_VALUE),
	    (Xv_opaque) insert_row->row, /* pass row # as client data */
	    PANEL_LIST_OP_VALIDATE,
	    event)
	== XV_ERROR)
	return(PANEL_NONE);
#endif /* OW_I18N */

    /* Entry validated.  Insert into Scrolling List */
    accept_insert(dp, insert_row);

    /* Create a new row before the next row.  If no next row, then append. */
    insert_row = panel_list_insert_row(dp,
	insert_row->next ? insert_row->next->row : -1, HIDE_ROW, REPAINT_LIST);

    /* Continue insert mode. insert_row now points to empty
     * row just inserted.
     *
     * If new row is out of the view window, then scroll new row
     * to top of view window.
     */
    check_row_in_view(dp, insert_row->row);

    /* Overlay text item over blank new row */
    first_row_in_view = (int) xv_get(dp->list_sb, SCROLLBAR_VIEW_START);
    xv_set(dp->list_sb, SCROLLBAR_INACTIVE, TRUE, 0);
    item_y = dp->list_box.r_top + insert_row->string_y -
	first_row_in_view * dp->row_height;
#ifdef OW_I18N
    xv_set(dp->text_item,
    	   PANEL_ITEM_Y,	item_y,
    	   PANEL_NOTIFY_PROC,	insert_done,
	   PANEL_VALUE_WCS,	null_string_wc,
	   XV_SHOW,	TRUE,
	   0);
#else
    xv_set(dp->text_item,
    	   PANEL_ITEM_Y,	item_y,
    	   PANEL_NOTIFY_PROC,	insert_done,
	   PANEL_VALUE,	"",
	   XV_SHOW,	TRUE,
	   0);
#endif OW_I18N
    dp->text_item_row = insert_row;

    
    /* Warp the pointer to the bottom right corner of the text item */
    xv_set(dp->parent_panel,
	   WIN_MOUSE_XY,
	        rect_right(&dp->list_box) - LIST_BOX_BORDER_WIDTH -
		   ROW_MARGIN,
		item_y + dp->row_height - 1,
	    0);

    /* Transfer keyboard focus to the text item */
    panel_set_kbd_focus(PANEL_PRIVATE(dp->parent_panel),
	ITEM_PRIVATE(dp->text_item));
    
    return(PANEL_NONE);
}


static Xv_opaque
insert_proc(menu, menu_item)
    Menu menu;
    Menu_item menu_item;
{
    register Panel_list_info *dp;
    int			first_row_in_view;
    int			item_y;
    int			which_row;
    Insert_pos_t	insert_position;
    Row_info		*insert_row = NULL;
    
    dp = (Panel_list_info *) xv_get(menu, XV_KEY_DATA, PANEL_LIST);
    insert_position = (Insert_pos_t) xv_get(menu_item,
	XV_KEY_DATA, PANEL_INSERT);

    /* Find the first row selected for editing */
    for (insert_row=dp->rows; insert_row;
	 insert_row=insert_row->next) {
	if (insert_row->edit_selected)
	    break;
    }
    if (!insert_row)
	insert_row = dp->rows;

    /* Determine where to insert new row */
    if (insert_position == INSERT_AFTER) {
	if (insert_row)
	    insert_row = insert_row->next;
	if (insert_row)
	    which_row = insert_row->row;
	else
	    which_row = -1;   /* append to end of list */
    } else {
	if (insert_row)
	    which_row = insert_row->row;
	else
	    which_row = 0;   /* insert at beginning of list */
    }

    /* Create insert row */
    insert_row = panel_list_insert_row(dp, which_row, HIDE_ROW,
				       REPAINT_LIST);

    /* If new row is out of the view window, then scroll new row
     * to top of view window.
     */
    check_row_in_view(dp, insert_row->row);
    
    /* Overlay text item over blank new row */
    first_row_in_view = (int) xv_get(dp->list_sb, SCROLLBAR_VIEW_START);
    xv_set(dp->list_sb, SCROLLBAR_INACTIVE, TRUE, 0);
    item_y = dp->list_box.r_top + insert_row->string_y -
	first_row_in_view * dp->row_height;
#ifdef OW_I18N
    xv_set(dp->text_item,
    	PANEL_ITEM_Y,	item_y,
    	PANEL_NOTIFY_PROC,	insert_done,
	PANEL_VALUE_WCS,	null_string_wc,
	XV_SHOW,	TRUE,
	0);
#else
    xv_set(dp->text_item,
    	PANEL_ITEM_Y,	item_y,
    	PANEL_NOTIFY_PROC,	insert_done,
	PANEL_VALUE,	"",
	XV_SHOW,	TRUE,
	0);
#endif OW_I18N
    dp->text_item_row = insert_row;

    /* Warp the pointer to the bottom right corner of the text item */
    xv_set(dp->parent_panel,
	   WIN_MOUSE_XY,
	        rect_right(&dp->list_box) - LIST_BOX_BORDER_WIDTH -
		   ROW_MARGIN,
		item_y + dp->row_height - 1,
	    0);

    /* Transfer keyboard focus to the text item */
    panel_set_kbd_focus(PANEL_PRIVATE(dp->parent_panel),
	ITEM_PRIVATE(dp->text_item));
    
    dp->edit_op = OP_INSERT;
    return(XV_OK);
}


static Xv_opaque
delete_proc(menu, menu_item)	/*ARGSUSED*/
    Menu menu;
    Menu_item menu_item;
{
    register Panel_list_info *dp;
    int		(*notify_proc)();
    int		row_deleted;
    Item_info	*panel_list_ip;
    Row_info	*edit_row;   /* Row being edited with a text field */
    Row_info	*row;
    Event	*event;
    
    dp = (Panel_list_info *) xv_get(menu, XV_KEY_DATA, PANEL_LIST);
    panel_list_ip = ITEM_PRIVATE(dp->public_self);
#ifdef OW_I18N
    if (dp->wchar_notify) {
	notify_proc = panel_list_ip->notify_wc;
	if (notify_proc)
	    event = (Event *) xv_get(menu, MENU_LAST_EVENT);
	edit_row = dp->text_item_row;
	do {
	    row_deleted = FALSE;
	    for (row=dp->rows; row; row=row->next)
		if (row->edit_selected && row != edit_row) {
		    if (notify_proc)
			(*notify_proc) (dp->public_self,
					row->string_wc, row->client_data,
					PANEL_LIST_OP_DELETE,
					event);
		    panel_list_delete_row(dp, row, REPAINT_LIST);
		    row_deleted = TRUE;
		    break;
		}
	} while (row_deleted);
	return(XV_OK);
    }
    else {
	notify_proc = panel_list_ip->notify;
	if (notify_proc)
	    event = (Event *) xv_get(menu, MENU_LAST_EVENT);
	edit_row = dp->text_item_row;
	do {
	    row_deleted = FALSE;
	    for (row=dp->rows; row; row=row->next)
		if (row->edit_selected && row != edit_row) {
		    if (notify_proc) { 
	    		row->string = (u_char *) wcstombsdup 
					((wchar_t *)row->string_wc);
			(*notify_proc) (dp->public_self,
					row->string, row->client_data,
					PANEL_LIST_OP_DELETE,
					event);
		    }
		    panel_list_delete_row(dp, row, REPAINT_LIST);
		    row_deleted = TRUE;
		    break;
		}
	} while (row_deleted);
	return(XV_OK);
    }
#else
    notify_proc = panel_list_ip->notify;
    if (notify_proc)
	event = (Event *) xv_get(menu, MENU_LAST_EVENT);
    edit_row = dp->text_item_row;
    do {
	row_deleted = FALSE;
	for (row=dp->rows; row; row=row->next)
	    if (row->edit_selected && row != edit_row) {
		if (notify_proc)
		    (*notify_proc) (dp->public_self,
				    row->string, row->client_data,
				    PANEL_LIST_OP_DELETE,
				    event);
		panel_list_delete_row(dp, row, REPAINT_LIST);
		row_deleted = TRUE;
		break;
	    }
    } while (row_deleted);
    return(XV_OK);
#endif /* OW_I18N */
}


static Xv_opaque
enter_read_mode(menu, menu_item)	/*ARGSUSED*/
    Menu menu;
    Menu_item menu_item;
{
    register Panel_list_info *dp;
    Item_info	*panel_list_ip;
    int		(*notify_proc)();
    Event	*event = (Event *) xv_get(menu, MENU_LAST_EVENT);
    Row_info	*row;

    dp = (Panel_list_info *) xv_get(menu, XV_KEY_DATA, PANEL_LIST);

    row = dp->text_item_row;
    if (row) {
	panel_list_ip = ITEM_FROM_PANEL_LIST(dp);
#ifdef OW_I18N
	if (wslen(xv_get(dp->text_item, PANEL_VALUE_WCS)) == 0) {
#else
	if (strlen(xv_get(dp->text_item, PANEL_VALUE)) == 0) {
#endif OW_I18N
	    panel_set_kbd_focus(PANEL_PRIVATE(dp->parent_panel),
		panel_list_ip);
	    xv_set(dp->text_item, XV_SHOW, FALSE, 0);
	    xv_set(dp->list_sb, SCROLLBAR_INACTIVE, FALSE, 0);
	    panel_list_delete_row(dp, row, REPAINT_LIST);
	} else {
#ifdef OW_I18N
	    if (dp->wchar_notify) {
		notify_proc = panel_list_ip->notify_wc;
		if (!notify_proc ||
		    (*notify_proc) (dp->public_self,
				    (wchar_t *) xv_get(dp->text_item,
						    PANEL_VALUE_WCS),
				    row->row,
				    PANEL_LIST_OP_VALIDATE,
				    event)
		    == XV_OK) {
		    /* Text was validated */
		    if (dp->edit_op == OP_CHANGE)
			accept_change(dp->text_item, row);
		    else {
			accept_insert(dp, row);
			panel_set_kbd_focus(PANEL_PRIVATE(dp->parent_panel),
			    panel_list_ip);
		    }
		} else {
		    /* Text was not validated */
		    panel_set_kbd_focus(PANEL_PRIVATE(dp->parent_panel),
			panel_list_ip);
		    xv_set(dp->text_item, XV_SHOW, FALSE, 0);
		    xv_set(dp->list_sb, SCROLLBAR_INACTIVE, FALSE, 0);
		    if (!row->string_wc || wslen(row->string_wc) == 0)
			panel_list_delete_row(dp, row, DO_NOT_REPAINT_LIST);
		    else {
			row->show = TRUE;
			paint_row(dp, row);
		    }
		}
	    }
	    else {
		notify_proc = panel_list_ip->notify;
		if (!notify_proc ||
		    (*notify_proc) (dp->public_self,
				    (char *) xv_get(dp->text_item,
						    PANEL_VALUE),
				    row->row,
				    PANEL_LIST_OP_VALIDATE,
				    event)
		    == XV_OK) {
		    /* Text was validated */
		    if (dp->edit_op == OP_CHANGE)
			accept_change(dp->text_item, row);
		    else {
			accept_insert(dp, row);
			panel_set_kbd_focus(PANEL_PRIVATE(dp->parent_panel),
			    panel_list_ip);
		    }
		} else {
		    /* Text was not validated */
		    panel_set_kbd_focus(PANEL_PRIVATE(dp->parent_panel),
			panel_list_ip);
		    xv_set(dp->text_item, XV_SHOW, FALSE, 0);
		    xv_set(dp->list_sb, SCROLLBAR_INACTIVE, FALSE, 0);
		    if (!row->string || strlen(row->string) == 0)
			panel_list_delete_row(dp, row, DO_NOT_REPAINT_LIST);
		    else {
			row->show = TRUE;
			paint_row(dp, row);
		    }
		}
	    }
#else
	    notify_proc = panel_list_ip->notify;
	    if (!notify_proc ||
		(*notify_proc) (dp->public_self,
				(char *) xv_get(dp->text_item,
						PANEL_VALUE),
				row->row,
				PANEL_LIST_OP_VALIDATE,
				event)
		== XV_OK) {
		/* Text was validated */
		if (dp->edit_op == OP_CHANGE)
		    accept_change(dp->text_item, row);
		else {
		    accept_insert(dp, row);
		    panel_set_kbd_focus(PANEL_PRIVATE(dp->parent_panel),
			panel_list_ip);
		}
	    } else {
		/* Text was not validated */
		panel_set_kbd_focus(PANEL_PRIVATE(dp->parent_panel),
		    panel_list_ip);
		xv_set(dp->text_item, XV_SHOW, FALSE, 0);
		xv_set(dp->list_sb, SCROLLBAR_INACTIVE, FALSE, 0);
		if (!row->string || strlen(row->string) == 0)
		    panel_list_delete_row(dp, row, DO_NOT_REPAINT_LIST);
		else {
		    row->show = TRUE;
		    paint_row(dp, row);
		}
	    }
#endif /* OW_I18N */
	}
    }
    dp->edit_op = OP_NONE;
    xv_destroy_safe(dp->text_item);
    dp->text_item = NULL;
    dp->text_item_row = NULL;   /* no row being edited */
    dp->edit_mode = FALSE;

    /* Remove highlighting and repaint boxes around selected rows */
    for (row=dp->rows; row; row=row->next) {
    	if (row->edit_selected || row->selected) {
    	    row->edit_selected = FALSE;
	    paint_row(dp, row);
    	}
    }

    return(XV_OK);
}
