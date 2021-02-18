/*	@(#)panel_impl.h 70.4 91/08/01	*/

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef panel_impl_defined
#define panel_impl_defined

/* turn on ecd code */
#define	ecd_panel
#define	ecd_slider

#ifdef OW_I18N
#include <xview_private/db_impl.h>
#include <xview/xv_i18n.h>
#include <X11/Xlib.h>
#include <X11/XSunExt.h>
#endif

#ifndef FILE
#ifndef SVR4
#undef NULL
#endif SVR4
#include <stdio.h>
#endif FILE
#include <sys/types.h>
#include <X11/Xlib.h>
#include <xview/pkg.h>
#include <xview/attrol.h>
#include <xview/frame.h>
#include <xview/openwin.h>
#include <xview/sel_svc.h>
#include <xview/svrimage.h>
#include <xview/window_hs.h>
#include <xview/panel.h>
#include <xview_private/item_impl.h>
#include <olgx/olgx.h>

/* panels and panel_items are both of type Xv_panel_or_item so that we
 * can pass them to common routines.
 */
#define PANEL_PRIVATE(p)	XV_PRIVATE(Panel_info, Xv_panel, p)
#define PANEL_PUBLIC(panel)     XV_PUBLIC(panel)

/*			per panel status flags  		         */

#define BLINKING		0x00000001
#define TIMING			0x00000002
#define SHOW_MENU_SET	        0x00000010
#ifdef ecd_panel
#define ADJUST_IS_PENDING_DELETE 0x00000020
#endif ecd_panel
#define PANEL_PAINTED	0x00000040
#define PANEL_HAS_INPUT_FOCUS	0x00000080
#define QUICK_MOVE		0x00000100	/* quick move pending */
#define NO_REDISPLAY_ITEM	0x00000200  /* don't call panel_redisplay_item
					     * from item_set_generic */
#define SELECT_DISPLAYS_MENU	0x00000400

#define blinking(panel)			((panel)->status & BLINKING)
#define show_menu_set(panel)		((panel)->status & SHOW_MENU_SET)
#ifdef ecd_panel
#define adjust_is_pending_delete(panel) ((panel)->status & ADJUST_IS_PENDING_DELETE)
#endif ecd_panel
#define panel_painted(panel)	((panel)->status & PANEL_PAINTED)
#define panel_has_input_focus(panel) ((panel)->status & PANEL_HAS_INPUT_FOCUS)
#define quick_move(panel)	((panel)->status & QUICK_MOVE)
#define no_redisplay_item(panel) ((panel)->status & NO_REDISPLAY_ITEM)
#define select_displays_menu(panel) ((panel)->status & SELECT_DISPLAYS_MENU)

#define rect_copy(to, from)		to.r_left = from.r_left; \
                                to.r_top = from.r_top;   \
                                to.r_width = from.r_width; \
                                to.r_height = from.r_height; 
                                
#define	set(value)	((value) != -1) 


#define PANEL_MORE_TEXT_WIDTH 16
#define PANEL_MORE_TEXT_HEIGHT 14
#define PANEL_PULLDOWN_MENU_WIDTH 16
#define PANEL_PULLDOWN_MENU_HEIGHT 8

#ifdef OW_I18N 
#define	ITERM_BUFSIZE	1024		/* Max size of preedit size */
#endif /* OW_I18N */


/* 			structures                                      */

/******************* selection info ****************************************/

typedef struct panel_selection {
   Seln_rank		rank;	/* SELN_{PRIMARY, SECONDARY, CARET} */
   unsigned int		is_null;/* TRUE if the selection has no chars */
   Item_info		*ip;	/* item with the selection */
} Panel_selection;


#define	panel_seln(panel, rank)	(&((panel)->selections[(unsigned int)(rank)]))


/***************************** panel **************************************/
/* *** NOTE: The first three fields of the
 * panel struct must match those of the panel_item
 * struct, since these are used interchangably in 
 * some cases.
 */
typedef struct panel_pw_struct {
	Xv_Window		    pw;
	Xv_Window			view;
	struct panel_pw_struct	*next;
} Panel_paint_window;

typedef struct panel_info {
    /****  DO NOT CHANGE THE ORDER OR PLACEMENT OF THESE THREE FIELDS ****/
    Panel_ops		*ops;		/* panel specific operations */
    unsigned int	flags;		/* default item flags */
    Panel		public_self;	/* back pointer to object */
    /**** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ****/

    int			active_caret_ascent;
    int			active_caret_height;
    int			active_caret_width;
    Pixfont		*bold_font;
    Font		bold_font_xid;
    Item_info		*kbd_focus_item;/* panel item with the keyboard focus */
    int			caret;		/* current caret character index */
    int			caret_ascent;	/* # of pixels above baseline */
    Pixmap		caret_bg_pixmap; /* used to restore the pixels
					  * underneath the caret */
    int			caret_height;
    int			caret_on;	/* caret is painted */
    int			caret_width;
    Xv_opaque		client_data;	/* for client use */
    Item_info		*current;
    int			current_col_x;	/* left position of current column */
    Panel_item		default_item;
    int			destroying;
    int			(*event_proc)();
    int			extra_height;
    int			extra_width;
    Graphics_info	*ginfo;		/* olgx graphics information */
    int			h_margin;	/* horizontal margin */
    int			inactive_caret_ascent;
    int			inactive_caret_height;
    int			inactive_caret_width;
    int			item_x;
    int			item_x_offset;
    int			item_y;
    int			item_y_offset;
    Item_info		*items;
    Panel_setting	layout;		/* HORIZONTAL, VERTICAL */
    int			(*layout_proc)(); /* interposed window layout proc */
    int			lowest_bottom;	/* lowest bottom of any item */
    int			max_item_y;	/* lowest item on row ??? */
    Panel_paint_window	*paint_window;
    Panel_setting	repaint;	/* default repaint behavior  */
    int			(*repaint_proc)();
    int			rightmost_right; /* rightmost right of any item */
    Panel_selection	selections[(int) SELN_UNSPECIFIED];
    Xv_opaque		seln_client;	/* selection svc client id */
    char		*shelf;		/* contents of shelf seln */
    int                 SliderActive;  /* TRUE if slider is previewing */
    unsigned int	status;		/* current event state */
    Pixfont		*std_font;	/* standard font */
    Font		std_font_xid;
    int			three_d;	/* TRUE: 3D, FALSE: 2D */
    struct itimerval	timer_full;	/* initial secs & usecs */
    int			v_margin;	/* vertical margin */
#ifdef OW_I18N
    wchar_t		*interm_text;	/* intermediate text */
    XIMFeedback      *interm_attr;   /* attribute for intermediate text */
    XIC			ic;
#ifdef VISIBLE_POS
    XIMTextVisiblePosType	visible_type;
    int			visible_pos;
#endif /* VISIBLE_POS */
    Xv_opaque		instance_qlist;

    XIMCallback     start_pecb_struct; 
    XIMCallback     draw_pecb_struct; 
    XIMCallback     done_pecb_struct;
    XIMCallback     start_stcb_struct; 
    XIMCallback     draw_stcb_struct; 
    XIMCallback     done_stcb_struct; 
    XIMCallback     start_luc_struct;
    XIMCallback     draw_luc_struct;
    XIMCallback     process_luc_struct;
    XIMCallback     done_luc_struct;
    XIMCallback     start_aux_struct; 
    XIMCallback     draw_aux_struct; 
    XIMCallback     done_aux_struct; 

#endif
} Panel_info;


#define	PANEL_EACH_PAINT_WINDOW(panel, pw)	\
   {Panel_paint_window *_next; \
    for (_next = (panel)->paint_window; _next != NULL; _next = _next->next) { \
    	(pw) = _next->pw;

#define	PANEL_END_EACH_PAINT_WINDOW	}}


Xv_private GC openwin_gc_list[OPENWIN_NBR_GCS];
Xv_private void openwin_check_gc_color();

/***********************************************************************/
/* external declarations of private variables                          */
/***********************************************************************/

Pkg_private Attr_attribute	panel_context_key;
Pkg_private Attr_attribute	panel_pw_context_key;


/***********************************************************************/
/* Pkg_private declarations of private functions                          */
/***********************************************************************/

Pkg_private void		panel_accept_kbd_focus();
Pkg_private void              	panel_adjust_label_size();
Pkg_private void		panel_append();
Pkg_private void		panel_btn_accepted();
Pkg_private void		panel_caret_invert();
Pkg_private void		panel_check_item_layout();
Pkg_private void          	panel_clear_item();
Pkg_private void		panel_clear_pw_rect();
Pkg_private void		panel_clear_rect();
Pkg_private int		 	panel_col_to_x();
Pkg_private int			panel_default_event();
Pkg_private void		panel_display();
Pkg_private Rect		panel_enclosing_rect();
Pkg_private int			panel_find_default_xy();
Pkg_private int 		panel_fonthome();
Pkg_private void		panel_free_image();
Pkg_private Xv_opaque	 	panel_get_attr();
Pkg_private int			panel_height();
Pkg_private void                panel_image_set_font();
Pkg_private void		panel_invert();
Pkg_private void		panel_invert_polygon();
Pkg_private void		panel_item_layout();
Pkg_private void		panel_itimer_set();
Pkg_private struct pr_size 	panel_make_image();
Pkg_private Item_info	       *panel_next_kbd_focus();
Pkg_private int		        panel_normalize_scroll();
Pkg_private Notify_value	panel_notify_event();
Pkg_private Notify_value	panel_notify_panel_event();
Pkg_private Notify_value	panel_notify_view_event();
Pkg_private int 		panel_nullproc();
Pkg_private void		panel_paint_svrim();
Pkg_private void		panel_paint_text();
Pkg_private Item_info	       *panel_previous_kbd_focus();
Pkg_private void		panel_pw_invert();
Pkg_private void		panel_redisplay();
Pkg_private void		panel_redisplay_item();
Pkg_private void		panel_refont();
Pkg_private void		panel_register_view();
Pkg_private int			panel_resize();
Pkg_private int		 	panel_row_to_y();
Pkg_private void		panel_seln_acquire();
Pkg_private void		panel_seln_cancel();
Pkg_private void		panel_seln_dehilite();
Pkg_private void		panel_seln_delete();
Pkg_private void		panel_seln_destroy();
Pkg_private void		panel_seln_hilite();
Pkg_private void		panel_seln_inform();
Pkg_private void		panel_seln_init();
Pkg_private Seln_holder		panel_seln_inquire();
Pkg_private Xv_opaque 		panel_set_avlist();
Pkg_private void		panel_set_bold_label_font();
Pkg_private Panel_item		panel_set_kbd_focus();
Pkg_private u_char             *panel_strsave();
Pkg_private Item_info 	       *panel_successor();
Pkg_private void		panel_text_caret_on();
Pkg_private void		panel_text_paint_label();
Pkg_private void		panel_unlink();
Pkg_private void		panel_update_extent();
Pkg_private void		panel_update_scrollbars();
Pkg_private Notify_value	panel_use_event();
Pkg_private int			panel_viewable_height();
Pkg_private Rect	       *panel_viewable_rect();
Pkg_private int			panel_viewable_width();
Pkg_private int			panel_width();
Pkg_private void		panel_yield_kbd_focus();


#endif
