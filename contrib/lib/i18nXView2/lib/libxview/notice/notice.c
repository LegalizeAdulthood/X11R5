#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)notice.c 70.2 91/07/03";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */


#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <xview/sun.h>
#include <xview/base.h>
#include <xview/defaults.h>
#include <xview/window_hs.h>
#include <xview/font.h>
#include <xview/fullscreen.h>
#include <xview/openmenu.h>
#include <xview/pixwin.h>
#include <xview/server.h>
#include <xview/frame.h>
#include <xview/panel.h>
#include <xview/screen.h>
#include <xview/win_input.h>
#include <X11/Xlib.h>
#include <xview_private/draw_impl.h>
#include <xview/notice.h>
#include <xview/cms.h>
#include <xview_private/noticeimpl.h>
#include <xview_private/pw_impl.h>
#include <xview/svrimage.h>

#ifdef	OW_I18N
#include <xview/xv_i18n.h>
#include <xview_private/xv_i18n_impl.h>
#endif

#include <olgx/olgx.h>

/*
 * --------------------------- Cursor Stuff -------------------------
 */


#ifndef SVR4
static short    notice_gray17_data[] = {
#else SVR4
static unsigned short    notice_gray17_data[] = {
#endif SVR4
#include <images/square_17.pr>
};

mpr_static(notice_gray17_patch, 16, 16, 1, notice_gray17_data);

#ifndef SVR4
static short    notice_gray25_data[] = {
#else SVR4
static unsigned short    notice_gray25_data[] = {
#endif SVR4
#include <images/square_25.pr>
};

mpr_static(notice_gray25_patch, 16, 16, 1, notice_gray25_data);

#ifndef SVR4
static short    notice_gray75_data[] = {
#else SVR4
static unsigned short    notice_gray75_data[] = {
#endif SVR4
#include <images/square_75.pr>
};

mpr_static(notice_gray75_patch, 16, 16, 1, notice_gray75_data);

extern void     xv_help_save_image();

#ifdef	OW_I18N
extern struct pr_size xv_pf_textwidth_wc();
#else
extern struct pr_size xv_pf_textwidth();
#endif
Xv_private void win_beep();

#ifdef	OW_I18N
static wchar_t notice_default_button_str[8] = {
			(wchar_t)'C' ,
			(wchar_t)'o' ,
			(wchar_t)'n' ,
			(wchar_t)'f' ,
			(wchar_t)'i' ,
			(wchar_t)'r' ,
			(wchar_t)'m' ,
			(wchar_t)'0' };

#else
static char *notice_default_button_str = "Confirm";
#endif

/*
 * --------------------------- DISPLAY PROCS ------------------------
 */

static int      notice_show();
static int      notice_determine_font();
static void	notice_add_default_button();
static void     notice_get_notice_size();
static void     notice_drawbox();
static void     notice_layout();
static void     notice_build_button();
static void     notice_preview_notice_font();
static void     notice_set_avlist();
static void     notice_defaults();
static void     notice_add_button_to_list();
static int      notice_text_width();
static void     notice_paint_button();
static int      notice_button_width();
static void     notice_get_button_pin_points();
static notice_buttons_handle notice_button_for_event();
static int      notice_offset_from_baseline();
static notice_buttons_handle create_button_struct();
static void     free_button_structs();
static void     notice_copy_event();
static void     notice_do_bell();
#ifdef	OW_I18N
static CHAR	**notice_string_set();
static CHAR	*notice_button_string_set();
#endif

/*
 * --------------------------- STATICS ------------------------------
 */

static Defaults_pairs bell_types[] = {
	"never",   0,
	"notices", 1, 
	"always",  2,
	NULL,      2,
};

static int      left_marg = 5;
static int      right_marg = 5;
static int      top_marg = 5;
static int      bottom_marg = 10;
static int      row_gap = 4;
static int      min_button_gap = 5;

#define BORDER			3	/* white space surounding text */
#define SHADOW			6	/* thickness of shadow         */
#define NOTICE_INVERT_BUTTON	1
#define NOTICE_NORMAL_BUTTON	0
#define NOTICE_BUTTON_GAP      10	/* same as ITEM_X_GAP in item_impl.h */

/*
 * --------------------------- Globals ------------------------------
 */

static notice_handle saved_notice;
static int      default_beeps;
static int      notice_use_audible_bell;
static int      notice_jump_cursor;
static Rect     notice_screen_rect;
static Display *notice_display;
static Xv_Window notice_window;
static int      leftoff, topoff;
static int      max_msg_width;
static Fullscreen fs;
static int      button_gap, left_indent;
static Xv_Font  defpf;

/* box to which cursor is contrained */
static int             bottombox;

/*
 * --------------------------- Externals ----------------------------
 */

Xv_private Graphics_info *xv_init_olgx();


/*
 * ----------------------- Public Interface -------------------------
 */

/*VARARGS*/
Xv_public int
notice_prompt(client_window, event, va_alist)
    Xv_Window       client_window;
    Event          *event;
va_dcl
{
    va_list         valist;
    int             result;
    register Xv_Drawable_info *info;

    if (!client_window) {
	xv_error(NULL,
		 ERROR_STRING,
#ifdef OW_I18N
	     XV_I18N_MSG("xv_messages","NULL parent window passed to notice_show(). Not allowed. (Notice package)"),
#else
	     "NULL parent window passed to notice_show(). Not allowed. (Notice package)",
#endif
		 0);
	return (NOTICE_FAILED);
    }
    DRAWABLE_INFO_MACRO(client_window, info);
    notice_display = xv_display(info);
#ifndef DISABLE_NOTICES
    /*
     * No need for loop to keep popping up notice, as help has been
     * disabled for notices for now
     */
    va_start(valist);
    result = notice_show(client_window, event, valist);
    va_end(valist);

    return (result);
#else
    return (NOTICE_YES);
#endif
}

/*
 * ----------------------PRIVATE PROCS-------------------------------
 */

static int
notice_show(client_window, event, valist)
    Xv_Window       client_window;
    Event          *event;
    va_list         valist;
{
    register notice_handle  notice;
    register Xv_Drawable_info *info;
    Attr_attribute 	    avlist[ATTR_STANDARD_SIZE];
    notice_buttons_handle   button;
    Cms			    cms = NULL;
    notice_buttons_handle   current_button = NULL;
    Graphics_info  	   *ginfo;
    Event           	    ie;
    Inputmask       	    im;
    int             	    is_highlighted = FALSE;
    int			    left_placement;
    int             	    ok_to_toggle_buttons = FALSE;
    int             	    old_mousex;
    int			    old_mousey;
    Rect           	   *old_mouse_position;
    int			    quadrant;
    struct rect     	    rect;
    int             	    result;
    Xv_opaque       	    root_window;
    Xv_Screen		    screen;
    unsigned short  	    this_event;
    unsigned short	    this_id;
    int             	    three_d;
    int			    top_placement;
    unsigned short  	    trigger;
    int             	    x;
    int			    y;
    int                     visual;

    DRAWABLE_INFO_MACRO(client_window, info);
    notice_display = xv_display(info);
    screen = xv_screen(info);

    input_imnull(&im);
    /*
     * Set im to be used in xv_input_readevent
     */
    win_setinputcodebit(&im, MS_LEFT);
    win_setinputcodebit(&im, MS_MIDDLE);
    win_setinputcodebit(&im, MS_RIGHT);
    win_setinputcodebit(&im, LOC_WINENTER);
    win_setinputcodebit(&im, LOC_WINEXIT);
    win_setinputcodebit(&im, LOC_DRAG);
    win_setinputcodebit(&im, LOC_MOVE);
    win_setinputcodebit(&im, WIN_VISIBILITY_NOTIFY);
    win_setinputcodebit(&im, WIN_REPAINT);
    im.im_flags = IM_ASCII | IM_NEGEVENT;

    if (!saved_notice) {
	saved_notice = (struct notice *) calloc(1, sizeof(struct notice));
	if (!saved_notice) {
	    xv_error(NULL,
#ifdef	OW_I18N
		     ERROR_STRING, XV_I18N_MSG("xv_messages","Malloc failed. (Notice package)"),
#else
		     ERROR_STRING, "Malloc failed. (Notice package)",
#endif
		     0);
	    return (NOTICE_FAILED);
	}
    }

    notice_use_audible_bell = defaults_get_enum("openWindows.beep",
                                "OpenWindows.Beep", bell_types);
    notice_jump_cursor = (int) defaults_get_boolean(
                "openWindows.popupJumpCursor", "OpenWindows.PopupJumpCursor", 
		(Bool) TRUE);
    default_beeps = defaults_get_integer(
                "notice.beepCount", "Notice.BeepCount", 1);

    notice = saved_notice;
    root_window = (Xv_object) xv_get(client_window, XV_ROOT);

    (void) notice_defaults(notice);
    (void) win_getrect(root_window, &notice_screen_rect);

    (void)notice_determine_font(client_window, notice);

    notice->event = (Event *) event;
    notice->client_window = client_window;

    attr_make(avlist, ATTR_STANDARD_SIZE, valist);
    (void) notice_set_avlist(notice, avlist);
    if (notice->number_of_buttons == 0)
        notice_add_default_button(notice);

    if (xv_depth(info) > 1) {
	char		    cms_name[25];
	Xv_singlecolor      color;
	
	color.red = color.green = color.blue = 0;
	visual = xv_get(client_window, XV_VISUAL_CLASS);
	CMS_CONTROL_CMS_NAME(cms_name, visual, color);
        cms = (Cms)xv_find(screen, CMS,
			CMS_NAME, cms_name,
			CMS_CONTROL_CMS, TRUE,
			CMS_SIZE, CMS_CONTROL_COLORS + 1,
			CMS_COLORS, &color,
			XV_VISUAL_CLASS, visual,
			NULL);
	three_d = defaults_get_boolean("OpenWindows.3DLook.Color",
	    "OpenWindows.3DLook.Color", TRUE);
    } else
#ifdef MONO3D
	three_d = defaults_get_boolean("OpenWindows.3DLook.Monochrome",
	    "OpenWindows.3DLook.Monochrome", FALSE);
#else
	three_d = FALSE;
#endif
    if (cms == NULL) {
        cms = xv_cms(info);
    }


    notice_window = (Xv_Window) xv_create(root_window, WINDOW,
	WIN_TRANSPARENT,
	WIN_TOP_LEVEL_NO_DECOR, TRUE,	/* no wmgr decoration */
	WIN_SAVE_UNDER, TRUE,		/* no damage caused */
	WIN_CMS, cms,
        XV_VISUAL_CLASS, xv_get(cms, XV_VISUAL_CLASS, 0),
	XV_SHOW, FALSE,
#ifdef	OW_I18N
	XV_FONT,	notice->notice_font,
#endif
	0);


    ginfo = xv_init_olgx(notice_window, &three_d);

    /*
     * Get mouse positioning info
     */
    if (notice->focus_specified) {
        int new_x, new_y;
        win_translate_xy(client_window, root_window, 
			 notice->focus_x,
			 notice->focus_y,
			 &new_x,
			 &new_y);
	x = old_mousex = new_x;
	y = old_mousey = new_y;
    } else {
	old_mouse_position = (Rect *) xv_get(root_window, WIN_MOUSE_XY);
	x = old_mousex = notice->focus_x = old_mouse_position->r_left;
	y = old_mousey = notice->focus_y = old_mouse_position->r_top;
    }

    /* Get size of rectangle */
    (void) notice_get_notice_size(notice, ginfo, &rect);

    (void) notice_get_button_pin_points(notice, ginfo);

    /*
     * Now offset for shadow
     */
    leftoff = MIN(50, rect.r_width / 3);
    topoff = MIN(50, rect.r_height / 2);
    quadrant = notice_quadrant(x, y);

    switch (quadrant) {
      case 0:
	left_placement = old_mousex;
	top_placement = old_mousey;
	rect.r_left = leftoff;
	rect.r_top = topoff;
	break;
      case 1:
	left_placement = old_mousex - (rect.r_width + leftoff + 2 * BORDER);
	top_placement = old_mousey;
	rect.r_left = 0;
	rect.r_top = topoff;
	break;
      case 2:
	left_placement = old_mousex - (rect.r_width + leftoff + 2 * BORDER);
	top_placement = old_mousey - (rect.r_height + topoff + 2 * BORDER);
	rect.r_left = 0;
	rect.r_top = 0;
	break;
      case 3:
	left_placement = old_mousex;
	top_placement = old_mousey - (rect.r_height + topoff + 2 * BORDER);
	rect.r_left = leftoff;
	rect.r_top = 0;
	break;
    }

    xv_set(notice_window,
	   XV_X, left_placement,
	   XV_Y, top_placement,
	   XV_WIDTH, rect.r_width + leftoff + 6 * BORDER + 1,
	   XV_HEIGHT, rect.r_height + topoff + 6 * BORDER + 1,
	   XV_SHOW, TRUE,
	   0);

    fs = (Fullscreen)
	xv_create(root_window, FULLSCREEN,
	    FULLSCREEN_INPUT_WINDOW, notice_window,
	    WIN_CONSUME_EVENTS,
		WIN_VISIBILITY_NOTIFY,
		WIN_REPAINT,
		WIN_MOUSE_BUTTONS,
		WIN_ASCII_EVENTS,
		WIN_UP_ASCII_EVENTS,
		LOC_WINENTER, LOC_WINEXIT,
		LOC_DRAG, LOC_MOVE,
		0,
	    0);
    if (fs) {
	notice->fullscreen = fs;
	notice->fullscreen_window = (Xv_opaque) notice_window;
    } else {
	(void) xv_destroy(notice_window);
	return (NOTICE_FAILED);	/* out of memory?? */
    }

    if (xv_get(xv_server(info), SERVER_JOURNALLING))
	(void) xv_set(xv_server(info), SERVER_JOURNAL_SYNC_EVENT, 1, 0);

    (void) notice_do_bell(notice);

    /*
     * then draw empty box and shadow
     */
    (void) notice_drawbox(notice->fullscreen_window, &rect, quadrant);
    /*
     * now fill in the box with the text AND buttons
     */
    (void) notice_layout(notice, &rect, ginfo, three_d);

    if (notice_jump_cursor && notice->yes_button_exists) {
	notice_buttons_handle curr;
	for (curr = (notice_buttons_handle) notice->button_info;
	     curr != (notice_buttons_handle) NULL;
	     curr = (notice_buttons_handle) curr->next)
	    if (curr->is_yes) {
		x = left_placement + curr->button_rect.r_left +
		    (curr->button_rect.r_width / 2);
		y = top_placement + bottombox + ((quadrant < 2) ? topoff : 0);
		(void) xv_set(root_window, WIN_MOUSE_XY, x, y, 0);
		break;
	    }
    }
    /*
     * Stay in fullscreen until a button is pressed, or trigger used
     */
    trigger = notice->default_input_code;

    if (xv_get(xv_server(info), SERVER_JOURNALLING))
	(void) xv_set(xv_server(info), SERVER_JOURNAL_SYNC_EVENT, 1, 0);

    for (;;) {
	int             is_select_action, is_stop_key;

	if (xv_input_readevent(notice_window, &ie, TRUE, TRUE, &im) 
		== (Xv_object)-1) {
	    break;
	}
	x = event_x(&ie);
	y = event_y(&ie);
	this_event = event_action(&ie);	/* get encoded event */
	this_id = event_id(&ie);/* get unencoded event */

	if (this_event == ACTION_HELP)  {
	    continue;
	}

	is_select_action = ((this_event == (int) ACTION_SELECT) ||
			    (this_id == (int) MS_LEFT))
	    ? 1 : 0;
	is_stop_key = ((this_event == (int) ACTION_STOP) ||
		       (this_id == (int) WIN_STOP))
	    ? 1 : 0;

	button = notice_button_for_event(notice, x, y);

	/*
	 * Check if notice is obscured
	 */
	if (this_event == WIN_VISIBILITY_NOTIFY)  {
	    XVisibilityEvent	*xVisEv;
	    xVisEv = (XVisibilityEvent *)event_xevent(&ie);

	    if ((xVisEv->state == VisibilityPartiallyObscured) || 
		(xVisEv->state == VisibilityFullyObscured))  {
		Xv_Drawable_info	*notice_window_info;

		DRAWABLE_INFO_MACRO(notice_window, notice_window_info);
		/*
		 * If notice is obscured, raise it
		 */
		XRaiseWindow(xv_display(notice_window_info), 
				xv_xid(notice_window_info));
	    }

	    continue;
	}

	/*
	 * Check if notice needs to be repainted
	 */
	if (this_event == WIN_REPAINT)  {
            /*
             * draw empty box and shadow
             */
            (void) notice_drawbox(notice->fullscreen_window, &rect, 
                                quadrant);
            /*
             * now fill in the box with the text AND buttons
             */
            (void) notice_layout(notice, &rect, ginfo, three_d);

	    continue;
	}

	if (((this_event == trigger) || (this_id == trigger))
	    && (((trigger == (int) ACTION_SELECT) ||
		 (trigger == (int) MS_LEFT)) ?
		(event_is_up(&ie) && (current_button == NULL))
		: 0)) {
	    /*
	     * catch UP mouse left if missed down below for trigger
	     */
	    notice->result = NOTICE_TRIGGERED;
	    (void) notice_copy_event(notice, &ie);
	    goto Done;
	} else if (((this_event == trigger) || (this_id == trigger))
		   && (((trigger == (int) ACTION_SELECT) ||
			(trigger == (int) MS_LEFT)) ?
		       (event_is_down(&ie) && (button == NULL))
		       : 0)) {
	    /*
	     * catch down mouse left for trigger, check above against
	     * button rather than current_button since current_button
	     * is NULL on SELECT down, but button may be a real button
	     */
	    notice->result = NOTICE_TRIGGERED;
	    (void) notice_copy_event(notice, &ie);
	    goto Done;
	} else if (is_stop_key
		   && saved_notice->no_button_exists) {
	    notice->result = NOTICE_NO;
	    (void) notice_copy_event(notice, &ie);
	    goto Done;
	} else if ((this_event == ACTION_DO_IT
		    || this_event == NOTICE_ACTION_DO_IT)
		   && saved_notice->yes_button_exists) {
	    notice->result = NOTICE_YES;
	    (void) notice_copy_event(notice, &ie);
	    goto Done;
	    /*
	     * NOTE: handle button event processing starting here
	     */
	} else if (is_select_action && notice->button_info) {
	    if (event_is_down(&ie)) {
		if (current_button &&
		    (current_button != button) &&
		    is_highlighted) {
		    notice_paint_button(notice->fullscreen_window,
			current_button, NOTICE_NORMAL_BUTTON, ginfo, three_d);
		    current_button = NULL;
		    is_highlighted = FALSE;
		    ok_to_toggle_buttons = FALSE;
		}
		if (button &&
		    !is_highlighted &&
		    current_button != button) {
		    current_button = button;
		    notice_paint_button(notice->fullscreen_window,
			current_button, NOTICE_INVERT_BUTTON, ginfo, three_d);
		    current_button = button;
		    is_highlighted = TRUE;
		    ok_to_toggle_buttons = TRUE;
		}
	    } else {		/* event_is_up */
		if (button) {
		    if (current_button &&
			(current_button != button) &&
			is_highlighted) {
			notice_paint_button(notice->fullscreen_window,
			    current_button, NOTICE_NORMAL_BUTTON, ginfo, three_d);
			current_button = NULL;
			is_highlighted = FALSE;
			ok_to_toggle_buttons = FALSE;
		    }
		    notice->result = button->value;
		    (void) notice_copy_event(notice, &ie);
		    goto Done;
		} else {
		    ok_to_toggle_buttons = FALSE;
		}
	    }
	} else if (this_event == LOC_DRAG) {
	    if (current_button && (current_button != button)) {
		notice_paint_button(notice->fullscreen_window,
		    current_button, NOTICE_NORMAL_BUTTON, ginfo, three_d);
		is_highlighted = FALSE;
		current_button = NULL;
		continue;
	    }
	    if (button) {
		if (current_button == button) {
		    continue;	/* already there */
		} else if ((current_button == NULL) && ok_to_toggle_buttons) {
		    notice_paint_button(notice->fullscreen_window,
			button, NOTICE_INVERT_BUTTON, ginfo, three_d);
		    current_button = button;
		    is_highlighted = TRUE;
		    continue;
		}
	    } else if (!button && current_button) {
		notice_paint_button(notice->fullscreen_window,
		    current_button, NOTICE_NORMAL_BUTTON, ginfo, three_d);
		current_button = NULL;
		is_highlighted = FALSE;
		continue;
	    }
	} else if (((this_event==trigger)||(this_id==trigger))
		&& (!is_select_action)) {
	    /*
	     * catch trigger as a last case, trigger can't be select button
	     * here as that case is dealt with above
	     */
	    notice->result = NOTICE_TRIGGERED;
	    (void) notice_copy_event(notice, &ie);
	    goto Done;
	}
    }

Done:
    if (xv_get(xv_server(info), SERVER_JOURNALLING))
	(void) xv_set(xv_server(info), SERVER_JOURNAL_SYNC_EVENT, 1, 0);
    (void) xv_destroy(fs);
    (void) free_button_structs(notice->button_info);
    result = notice->result;
    if (client_window && (event != (Event *)0)) {
    	int new_x, new_y;
        win_translate_xy(notice_window, client_window, 
			 event_x(notice->event),
			 event_y(notice->event),
			 &new_x,
			 &new_y);
	event_set_x(event, new_x);
	event_set_y(event, new_y);
	event_set_window(event, client_window);
    }
    /* warp mouse back */
    if (notice_jump_cursor && notice->yes_button_exists) {
	if (notice->focus_specified) {
	    (void) xv_set(root_window, WIN_MOUSE_XY,
			  old_mousex, old_mousey,
			  0);
	} else {
	    (void) xv_set(root_window, WIN_MOUSE_XY, old_mousex, old_mousey, 0);
	}
    }
    (void) xv_destroy(notice_window);
    return (result);
}

static void
notice_copy_event(notice, event)
    register notice_handle notice;
    Event          *event;
{
    if (notice->event == (Event *) 0) {
	return;
    } else
	*notice->event = *event;
}

/*
 * --------------------------- Statics ------------------------------
 */

static void
notice_preview_notice_font(notice, avlist)
    register notice_handle notice;
    register Attr_attribute *avlist;
{
    caddr_t         value;

    for (; *avlist; avlist = attr_next(avlist)) {
	switch (avlist[0]) {
	  case NOTICE_FONT:
	    value = (caddr_t) avlist[1];
	    notice->notice_font = (Xv_Font) value;
	    break;
	  default:
	    break;
	}
    }
}
static void
notice_add_default_button(notice)
    register notice_handle notice;
{
    notice_buttons_handle button;

    button = (notice_buttons_handle) create_button_struct();
    button->string = (CHAR *) notice_default_button_str;
    button->is_yes = TRUE;
    button->value = NOTICE_YES;
    notice->yes_button_exists = TRUE;
    (void) notice_add_button_to_list(notice, button);

}

static void
notice_set_avlist(notice, avlist)
    register notice_handle notice;
    register Attr_attribute *avlist;
{
    int             yes_button_seen = FALSE;
    int             no_button_seen = FALSE;
    int             number_of_buttons_seen = 0;
    int             trigger_set = 0;
    caddr_t         value;

    (void) notice_preview_notice_font(notice, avlist);

    for (; *avlist; avlist = attr_next(avlist)) {
	value = (caddr_t) avlist[1];
	switch (avlist[0]) {

	  case NOTICE_NO_BEEPING:
	    if ((int) value == 1)
		notice->dont_beep = 1;
	    break;

	  case NOTICE_TRIGGER:
	    notice->default_input_code = (int) avlist[1];
	    trigger_set = 1;
	    break;

#ifdef	OW_I18N
	  case NOTICE_MESSAGE_STRINGS_ARRAY_PTR:
	    notice->message_items = (CHAR **) notice_string_set( notice,value , avlist[0] );
	    break;

	  case NOTICE_MESSAGE_STRINGS_ARRAY_PTR_WCS:
	    notice->message_items = (CHAR **) value;
	    break;

	  case NOTICE_MESSAGE_STRINGS:
	    notice->message_items = (CHAR **) notice_string_set( notice , &avlist[1],avlist[0] );
	    break;

	  case NOTICE_MESSAGE_STRINGS_WCS:
	    notice->message_items = (CHAR **) &avlist[1];
	    break;

	  case NOTICE_MESSAGE_STRING:
	  case NOTICE_MESSAGE_STRING_WCS:
	    notice->message_items = (CHAR **) notice_string_set( notice,value ,avlist[0]);
	    break;

	  case NOTICE_BUTTON_YES:
	  case NOTICE_BUTTON_YES_WCS:{
		notice_buttons_handle button;

		if (!yes_button_seen) {
		    yes_button_seen = TRUE;
		} else {
		    (void) xv_error(NULL,
				    ERROR_STRING,
#ifdef	OW_I18N
		    XV_I18N_MSG("xv_messages","Only one NOTICE_BUTTON_YES attr allowed. Attr ignored. (Notice package)"),
#else
		    "Only one NOTICE_BUTTON_YES attr allowed. Attr ignored. (Notice package)",
#endif
				    0);
		    break;
		}
		button = (notice_buttons_handle) create_button_struct();
		if( avlist[0] == NOTICE_BUTTON_YES )
		  button->string = notice_button_string_set(button,avlist[1]);
		else
		  button->string = (CHAR *) avlist[1];
		button->is_yes = TRUE;
		button->value = NOTICE_YES;
		notice->yes_button_exists = TRUE;
		number_of_buttons_seen++;
		(void) notice_add_button_to_list(notice, button);
		break;
	    }

	  case NOTICE_BUTTON_NO:
	  case NOTICE_BUTTON_NO_WCS:{
		notice_buttons_handle button;

		if (!no_button_seen) {
		    no_button_seen = TRUE;
		} else {
		    xv_error(NULL,
			     ERROR_STRING,
#ifdef	OW_I18N
		    XV_I18N_MSG("xv_messages","Only one NOTICE_BUTTON_NO attr allowed. Attr ignored. (Notice package)"),
#else
		    "Only one NOTICE_BUTTON_NO attr allowed. Attr ignored. (Notice package)",
#endif
			     0);
		    break;
		}
		button = (notice_buttons_handle) create_button_struct();
		if( avlist[0] == NOTICE_BUTTON_NO )
		  button->string = notice_button_string_set(button,avlist[1]);
		else
		  button->string = (CHAR *) avlist[1];
		button->is_no = TRUE;
		button->value = NOTICE_NO;
		notice->no_button_exists = TRUE;
		number_of_buttons_seen++;
		(void) notice_add_button_to_list(notice, button);
		break;
	    }

	  case NOTICE_BUTTON:
	  case NOTICE_BUTTON_WCS:{
		notice_buttons_handle button;

		button = (notice_buttons_handle) create_button_struct();
		if( avlist[0] == NOTICE_BUTTON )
		  button->string = notice_button_string_set(button,avlist[1]);
		else
		  button->string = (CHAR *) avlist[1];
		button->value = (int) avlist[2];
		(void) notice_add_button_to_list(notice, button);
		number_of_buttons_seen++;
		break;
	    }
#else
	  case NOTICE_MESSAGE_STRINGS_ARRAY_PTR:
	    notice->message_items = (char **) value;
	    break;

	  case NOTICE_MESSAGE_STRINGS:
	    notice->message_items = (char **) &avlist[1];
	    break;

	  case NOTICE_BUTTON_YES:{
		notice_buttons_handle button;

		if (!yes_button_seen) {
		    yes_button_seen = TRUE;
		} else {
		    (void) xv_error(NULL,
				    ERROR_STRING,
#ifdef	OW_I18N
		    XV_I18N_MSG("xv_messages","Only one NOTICE_BUTTON_YES attr allowed. Attr ignored. (Notice package)"),
#else
		    "Only one NOTICE_BUTTON_YES attr allowed. Attr ignored. (Notice package)",
#endif
				    0);
		    break;
		}
		button = (notice_buttons_handle) create_button_struct();
		button->string = (char *) avlist[1];
		button->is_yes = TRUE;
		button->value = NOTICE_YES;
		notice->yes_button_exists = TRUE;
		number_of_buttons_seen++;
		(void) notice_add_button_to_list(notice, button);
		break;
	    }

	  case NOTICE_BUTTON_NO:{
		notice_buttons_handle button;

		if (!no_button_seen) {
		    no_button_seen = TRUE;
		} else {
		    xv_error(NULL,
			     ERROR_STRING,
#ifdef	OW_I18N
		    XV_I18N_MSG("xv_messages","Only one NOTICE_BUTTON_NO attr allowed. Attr ignored. (Notice package)"),
#else
		    "Only one NOTICE_BUTTON_NO attr allowed. Attr ignored. (Notice package)",
#endif
			     0);
		    break;
		}
		button = (notice_buttons_handle) create_button_struct();
		button->string = (char *) avlist[1];
		button->is_no = TRUE;
		button->value = NOTICE_NO;
		notice->no_button_exists = TRUE;
		number_of_buttons_seen++;
		(void) notice_add_button_to_list(notice, button);
		break;
	    }

	  case NOTICE_BUTTON:{
		notice_buttons_handle button;

		button = (notice_buttons_handle) create_button_struct();
		button->string = (char *) avlist[1];
		button->value = (int) avlist[2];
		(void) notice_add_button_to_list(notice, button);
		number_of_buttons_seen++;
		break;
	    }
#endif
	  case NOTICE_FONT:
	    /*
	     * already previewed above
	     */
	    break;

	  case NOTICE_FOCUS_XY:
	    /*
	     * needs to be implemented
	     */
	    notice->focus_x = (int) avlist[1];
	    notice->focus_y = (int) avlist[2];
	    notice->focus_specified = TRUE;
	    break;

	  case XV_KEY_DATA:
	    if (avlist[1] == XV_HELP) {
		notice->help_data = (char *) avlist[2];
	    } else
		(void) xv_error(NULL,
				ERROR_BAD_VALUE, avlist[0], avlist[1],
#ifdef	OW_I18N
				ERROR_STRING, XV_I18N_MSG("xv_messages","(Notice package)"),
#else
				ERROR_STRING, "(Notice package)",
#endif
				0);
	    break;

	  default:
	    (void) xv_error(NULL,
			    ERROR_BAD_ATTR, avlist[0],
#ifdef	OW_I18N
			    ERROR_STRING, XV_I18N_MSG("xv_messages","(Notice package)"),
#else
			    ERROR_STRING, "(Notice package)",
#endif
			    0);
	    break;
	}
    }

    if ((number_of_buttons_seen == 0) && (trigger_set == 0))
	notice->default_input_code = (int) ACTION_STOP;

}

/*
 * ------------------------------------------------------------------
 */

static void
notice_get_notice_size(notice, ginfo, rect)
    register notice_handle notice;
    Graphics_info  *ginfo;
    struct rect    *rect;
{
    int             x = 0, y = 0, i;
    Pixfont        *this_font = (Pixfont *) ((notice->notice_font) ?
					     notice->notice_font : defpf);
    max_msg_width = 0;
    if (notice->message_items) {
	CHAR          **strs;
	int             num_lines = 0, str_width = 0, chrht, chrwth;

	chrwth = xv_get(this_font, FONT_DEFAULT_CHAR_WIDTH);
	chrht = xv_get(this_font, FONT_DEFAULT_CHAR_HEIGHT);
	strs = (CHAR **) notice->message_items;
	while (*strs) {
	    str_width = notice_text_width(this_font, (CHAR *) *strs)
		+ 2 * (chrwth);	/* fudge factor */
	    max_msg_width = MAX(str_width, max_msg_width);	/* keep track of largest */
	    num_lines++;	/* keep line count */
	    strs++;
	}
	x = max_msg_width;
	y += ((num_lines + 2) * chrht);
	y += chrht / 2;
    }
    y = MAX(y + row_gap, top_marg);	/* top_marg in case no text */

    i = 0;			/* init button count */
    if (notice->button_info) {
	int             max_buttons_width = 0;
	int             this_buttons_width = 0;
	int             total = 0;
	notice_buttons_handle curr;

	for (curr = (notice_buttons_handle) notice->button_info;
	     curr != (notice_buttons_handle) NULL;
	     curr = (notice_buttons_handle) curr->next) {
	    this_buttons_width = notice_button_width(this_font, ginfo,
					      (notice_buttons_handle) curr);
	    max_buttons_width += this_buttons_width;
	    i++;		/* increment button count */
	}
	y += notice->button_info->button_rect.r_height;
	x = MAX(x, max_buttons_width);
	total = x + left_marg + right_marg + (6 * BORDER) + (4 * SHADOW);
	if (total >= notice_screen_rect.r_width) {
	    /*
	     * if buttons too wide, then they clip just like the text will.
	     * this is not very nice, but TOO BAD!! it's a rough job
	     * attempting to handle their wraping.
	     */
	    button_gap = 0;
	} else {
	    if (i == 1) {	/* center button */
		button_gap = MAX(0, (x - max_buttons_width) / 2);
	    } else {		/* >1 */
		int             leftover = x - max_buttons_width;
		int             desired_gap = (i - 1) * NOTICE_BUTTON_GAP;

		left_indent = MAX(0, (leftover - desired_gap) / 2);
		button_gap = NOTICE_BUTTON_GAP;
	    }
	    if ((i > 1) && (left_indent == 0) && (((i - 1) * min_button_gap) < total)) {
		left_indent = min_button_gap;
		x += (i - 1) * min_button_gap;
	    }
	}
    }
    /*
     * Sum up positions and add in OL FUDGE factors
     */
    x += left_marg + right_marg + (6 * BORDER) + (4 * SHADOW);
    y += top_marg + bottom_marg + (2 * BORDER);

    rect->r_top = 0;
    rect->r_left = 0;
    rect->r_width = x;
    rect->r_height = y;
}

static void
notice_layout(notice, rect, ginfo, three_d)
    register notice_handle notice;
    struct rect    *rect;
    Graphics_info  *ginfo;
    int		   three_d;
{
    int             x, y;
    notice_buttons_handle curr;
    int             x1, y1, wd, ht;
    Xv_Window       pw = notice->fullscreen_window;
    Pixfont        *this_font = (Pixfont *) ((notice->notice_font) ?
					     notice->notice_font : defpf);
    /* Additional vars for replacing pw_ calls */
    Xv_Drawable_info *info;
    Display        *display;
    Drawable        d;
    GC              gc;
    int		    i = 0;
    int		    chrht, chrwth;
    XSegment	    segments[5];
#ifdef	OW_I18N
    XFontSet	    font_set;
    XFontSetExtents	*font_set_extents; 
#endif

    /*
     * Set information neede by Xlib calls
     */
    DRAWABLE_INFO_MACRO(pw, info);
    display = xv_display(info);
    d = xv_xid(info);

    /*
     * later, consider centering here
     */
    chrht = xv_get(this_font, FONT_DEFAULT_CHAR_HEIGHT);
    chrwth = xv_get(this_font, FONT_DEFAULT_CHAR_WIDTH);
    x = rect->r_left + left_marg + BORDER + chrwth;
    y = rect->r_top + top_marg + BORDER + chrht;

    if (notice->message_items) {
	CHAR          **strs;
	int             offset = notice_offset_from_baseline(this_font);
	/* Additional vars for replacing pw_text call */
        XID             font;
        int             len;

	/*
	 * Set GC neede by XDrawImageString()
	 */
        gc = (GC)xv_find_proper_gc(display, info, PW_TEXT);

        /* SunView1.X incompatibility: NULL pixfont meant use system_font. */
        if (this_font == 0) {
	    this_font = (Pixfont *)xv_get(pw, WIN_FONT);
        }

	/*
	 * op == PIX_SRC, so use XV_USE_CMS_FG instead of XV_USE_OP_FG
	 * since there isn't any color encoded in the op
	 */
	xv_set_gc_op(display, info, gc, PIX_SRC, XV_USE_CMS_FG, XV_DEFAULT_FG_BG);

#ifdef	OW_I18N
	font_set = (XFontSet)xv_get( this_font , FONT_SET_ID );
#else
	font = (XID) xv_get(this_font, XV_XID);
	XSetFont(display, gc, font);
#endif	/*OW_I18N*/

	for (strs = (CHAR **) notice->message_items;
	     *strs;
	     strs++) {
	    int str_width = notice_text_width(this_font, (CHAR *) *strs);
	    /*
	     * Replace pw_text with Xlib calls
	     */
	    /***********************************************/
            if ((len = STRLEN(*strs)) == 0) {
	        y += chrht;
	        continue;
            }

#ifdef	OW_I18N
	    XwcDrawImageString(display, d, font_set , gc, 
		x + (max_msg_width - str_width) / 2, 
		(offset >= 0) ? y + offset : y - offset, *strs, len);
#else
	    XDrawImageString(display, d, gc, 
		x + (max_msg_width - str_width) / 2, 
		(offset >= 0) ? y + offset : y - offset, *strs, len);
#endif
	    /***********************************************/
	    y += chrht;
	}
    }
    y += chrht;
    y = MAX(y + row_gap, 0);

    if (notice->button_info) {
	if (notice->number_of_buttons == 1) {
	    x += button_gap;
	    notice_build_button(pw, x, y, notice->button_info, ginfo, three_d);
	} else if (notice->number_of_buttons > 1) {
	    x += left_indent;
	    for (curr = (notice_buttons_handle) notice->button_info;
		 curr != (notice_buttons_handle) NULL;
		 curr = (notice_buttons_handle) curr->next) {
		notice_build_button(pw, x, y, (notice_buttons_handle)curr, 
			ginfo, three_d);
		x += curr->button_rect.r_width + button_gap;
	    }
	}
    }
    x1 = rect->r_left + 4;
    y1 = rect->r_top + 4;
    ht = rect->r_height - 8;
    wd = rect->r_width - 8;

    /*
     * Set GC for XDrawLine call
     */
    gc = (GC)xv_find_proper_gc(display, info, PW_VECTOR);
    /* The 1 in PIX_COLOR(1) is the cms_index */
    xv_set_gc_op(display, info, gc, 
    			(PIX_SET | PIX_COLOR(1)), 
			XV_USE_OP_FG, XV_DEFAULT_FG_BG);

    /*
     * Replace pw_vector with Xlib calls
     */
    /*****************************************/
    segments[i].x1 = x1;
    segments[i].y1 = y1;
    segments[i].x2 = x1 + wd;
    segments[i++].y2 = y1;

    segments[i].x1 = x1;
    segments[i].y1 = y1 + ht;
    segments[i].x2 = x1 + wd;
    segments[i++].y2 = y1 + ht;

    segments[i].x1 = x1;
    segments[i].y1 = y1;
    segments[i].x2 = x1;
    segments[i++].y2 = y1 + ht;

    segments[i].x1 = x1 + wd;
    segments[i].y1 = y1;
    segments[i].x2 = x1 + wd;
    segments[i++].y2 = y1 + ht;

    XDrawSegments(display, d, gc, segments, i);
    /*****************************************/
}

static void
notice_get_button_pin_points(notice, ginfo)
    register notice_handle notice;
    Graphics_info  *ginfo;
{
    int             x, y, chrht, chrwth;
    notice_buttons_handle curr;
    Pixfont        *this_font = (Pixfont *) ((notice->notice_font) ?
					     notice->notice_font : defpf);

#ifdef	OW_I18N
    XFontSet            font_set;
    XFontSetExtents	*font_set_extents;
    Xv_Drawable_info *info;
    
    font_set = (XFontSet)xv_get(notice->notice_font ? notice->notice_font :
                                defpf, FONT_SET_ID);
    DRAWABLE_INFO_MACRO(notice->fullscreen_window, info);
    font_set_extents = XExtentsOfFontSet(font_set);
    chrwth = font_set_extents->max_ink_extent.width;
    chrht  = font_set_extents->max_ink_extent.height;                                  
#else
    chrht = xv_get(this_font, FONT_DEFAULT_CHAR_HEIGHT);
    chrwth = xv_get(this_font, FONT_DEFAULT_CHAR_WIDTH);
#endif
    x = left_marg + BORDER + chrwth;
    y = top_marg + BORDER + chrht;

    if (notice->message_items) {
	CHAR          **strs;

	strs = (CHAR **) notice->message_items;
	while (*strs) {
	    y += chrht;
	    strs++;
	}
    }
    y += chrht;
    y = MAX(y + row_gap, 0);

    if (notice->button_info) {
	if (notice->number_of_buttons == 1) {
	    x += button_gap;
	    x += notice_button_width(this_font, ginfo, notice->button_info);
	} else if (notice->number_of_buttons > 1) {
	    for (curr = notice->button_info; curr != NULL; curr = curr->next) {
		x += notice_button_width(this_font, ginfo, curr) + button_gap;
	    }
	    x -= button_gap;
	}
	y += notice->button_info->button_rect.r_height - (2 * BORDER);
	bottombox = y;
    }
}


/*
 * ----------------------   Misc Utilities   ------------------------
 */

static void
notice_defaults(notice)
    notice_handle   notice;
{
    notice->beeps = default_beeps;
    notice->default_input_code = '\0';	/* ASCII NULL */
    notice->notice_font = (Xv_Font) defpf;
    notice->dont_beep = FALSE;
    notice->yes_button_exists = FALSE;
    notice->no_button_exists = FALSE;
    notice->focus_specified = FALSE;
    notice->event = (Event *) 0;
    notice->button_info = (notice_buttons_handle) 0;
    notice->number_of_buttons = 0;
    notice->help_data = "xview:notice";
    button_gap = 0;
}

/*
 * ------------------------------------------------------------------
 */

static void
notice_add_button_to_list(notice, button)
    register notice_handle notice;
    notice_buttons_handle button;
{
    notice_buttons_handle curr;

    if (notice->button_info) {
	for (curr = notice->button_info; curr; curr = curr->next)
	    if (curr->next == NULL) {
		curr->next = button;
		break;
	    }
    } else
	notice->button_info = button;
    notice->number_of_buttons++;
}

/*
 * ------------------------------------------------------------------
 */

static          notice_buttons_handle
create_button_struct()
{
    notice_buttons_handle pi = NULL;

    pi = (notice_buttons_handle) calloc(1, sizeof(struct notice_buttons));
    if (!pi) {
	xv_error(NULL,
	         ERROR_STRING,
#ifdef	OW_I18N
	     XV_I18N_MSG("xv_messages","calloc failed in notice[create_button_struct]. (Notice package)"),
#else
	     "calloc failed in notice[create_button_struct]. (Notice package)",
#endif
		 0);
    } else {
	pi->is_yes = (int)FALSE;
    }
    return pi;
}


/*
 * ------------------------------------------------------------------
 */

static void
free_button_structs(first)
    notice_buttons_handle first;
{
    notice_buttons_handle current;
    notice_buttons_handle next;

    if (!first)
	return;
    for (current = first; current != NULL; current = next) {
	next = current->next;
	free(current);
    }
}

/*
 * font char/pixel conversion routines
 */

static int
notice_offset_from_baseline(font)
    Pixfont        *font;
{
#ifdef	OW_I18N
    XFontSet		font_set;
    XFontSetExtents	*font_set_extents;
#else
    XFontStruct		*x_font_info;
#endif

    if (font == NULL)
	return (0);

#ifdef	OW_I18N
    font_set = (XFontSet)xv_get(font, FONT_SET_ID);
    font_set_extents = XExtentsOfFontSet(font_set);
    return(font_set_extents->max_ink_extent.y);
#else
    x_font_info = (XFontStruct *)xv_get(font, FONT_INFO);
    return (-(x_font_info->ascent));
#endif
}

static int
notice_text_width(font, str)
    Pixfont        *font;
    CHAR           *str;
{
    struct pr_size  size;

#ifdef	OW_I18N
    size = xv_pf_textwidth_wc(STRLEN(str), font, str);
#else
    size = xv_pf_textwidth(STRLEN(str), font, str);
#endif
    return (size.x);
}

static int
notice_button_width(font, ginfo, button)
    Pixfont        *font;
    Graphics_info  		*ginfo;
    notice_buttons_handle button;
{
    button->button_rect.r_width = notice_text_width(font, button->string) +
	2*ButtonEndcap_Width(ginfo);
    button->button_rect.r_height = Button_Height(ginfo);
    return (button->button_rect.r_width);
}

static int
notice_quadrant(x, y)
    int             x, y;
{
    int             quadrant;

    if ((x <= notice_screen_rect.r_width / 2) && (y <= notice_screen_rect.r_height / 2))
	quadrant = 0;
    else if ((x > notice_screen_rect.r_width / 2) && (y <= notice_screen_rect.r_height / 2))
	quadrant = 1;
    else if ((x > notice_screen_rect.r_width / 2) && (y > notice_screen_rect.r_height / 2))
	quadrant = 2;
    else
	quadrant = 3;

    return (quadrant);
}

static void
notice_drawbox(pw, rectp, quadrant)
    Xv_Window       pw;
    struct rect    *rectp;
    int             quadrant;
{
    struct rect     rect;
    struct pr_pos   vlist[6];
    int             nptarr[1];
    /* Additional vars for replacing pw_ calls */
    Xv_Drawable_info *info;
    Display        *display;
    Drawable        d;
    GC              gc;
    int		    i = 0;
    XSegment	    segments[9];
    GC		fill_gc;
    XGCValues	gc_val;
    short	    depth;
    Server_image	tile1;
    Server_image	tile2;
    Pixmap	    tile_id1;
    Pixmap	    tile_id2;
    XPoint	    points[4];
    struct pixrect	*spr1;
    struct pixrect	*spr2;

    /*
     * Set information neede by Xlib calls
     */
    DRAWABLE_INFO_MACRO(pw, info);
    display = xv_display(info);
    d = xv_xid(info);

    rect = *rectp;
    nptarr[0] = 3;

    /*
     * draw 3D shadow
     * Isa:
     * Replace pw_polygon_2 calls with Xlib calls:
     * Each case (for each quadrant) involves 
     * - getting 2 different bitmaps/pixrects(1 bit deep) for the shadows
     * - create server images from the above
     * - getting/setting GC's
     * - filling two triangle(polygon) using the 2 server images.
     * To reduce code duplication, the bitmaps to be used
     * are determined separately in another switch.
     * Most of the code here is straight out from libxvin/pw/pw_plygon2.c
     */

    /*
     * Determine which bitmaps to use
     */
    switch (quadrant) {
      case 0:			/* break down and to right */
	spr1 = &notice_gray17_patch;
	spr2 = &notice_gray25_patch;
	break;
      case 1:			/* break down and to left */
	spr1 = &notice_gray25_patch;
	spr2 = &notice_gray17_patch;
	break;
      case 2:			/* break up and to left */
	spr1 = &notice_gray25_patch;
	spr2 = &notice_gray75_patch;
	break;
      case 3:			/* break up and to right */
	spr1 = &notice_gray75_patch;
	spr2 = &notice_gray25_patch;
	break;
    }

    /*
     * Since at this point, we know which bitmaps/pixrects are to be used for the
     * shadows, create the server images now - OUTSIDE the following switch 
     * statement.
     */
    if (PR_NOT_SERVER_IMAGE(spr1)) {
	tile1 = xv_create(xv_screen(info), SERVER_IMAGE,
			     XV_HEIGHT, spr1->pr_height,
			     XV_WIDTH, spr1->pr_width,
			     SERVER_IMAGE_DEPTH, spr1->pr_depth,
			     SERVER_IMAGE_BITS, (mpr_d(spr1))->md_image,
			     0);
	tile_id1 = (unsigned long) xv_get(tile1, XV_XID);
    } else {
	tile_id1 = (unsigned long) xv_get((Server_image) spr1, XV_XID);
    }

    if (PR_NOT_SERVER_IMAGE(spr2)) {
	tile2 = xv_create(xv_screen(info), SERVER_IMAGE,
			     XV_HEIGHT, spr2->pr_height,
			     XV_WIDTH, spr2->pr_width,
			     SERVER_IMAGE_DEPTH, spr2->pr_depth,
			     SERVER_IMAGE_BITS, (mpr_d(spr2))->md_image,
			     0);
	tile_id2 = (unsigned long) xv_get(tile2, XV_XID);
    } else {
	tile_id2 = (unsigned long) xv_get((Server_image) spr2, XV_XID);
    }

#ifdef VAX_8PLANE_ONLY
    depth = 8;
#else
    /*
     * All the shadow pixrects have the same depth
     */
    depth = spr1->pr_depth;
#endif

    /*
     * Most of the code here is straight out from libxvin/pw/pw_plygon2.c
     * Using the server images created, get/set the gc to be used by XFillPolygon
     * The reason, the GCs are NOT get/set separately, as are the server images,
     * is because xv_find_proper_gc may return the same GC when called the 2nd
     * time. 
     */
    switch (quadrant) {
      case 0:			/* break down and to right */
	/*
	 * 1st triangle of the shadow
	 */
	points[0].x = 0;
	points[0].y = 0;
	points[1].x = rect.r_width + leftoff;
	points[1].y = topoff;
	points[2].x = leftoff;
	points[2].y = topoff;
        fill_gc = (GC)xv_find_proper_gc(display, info, PW_POLYGON2);
	if (depth == xv_depth(info)) {
	    gc_val.tile = tile_id1;
	    gc_val.fill_style = FillTiled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCTile, &gc_val);
	} else {
	    /*
	     * BUG ALERT! This only works in a 1->8 bit case, but won't it
	     * get called for the 8->1 bit case as well? e.g. you can't
	     * stipple an 8-bit pixmap into a 1-bit deep window. There should
	     * be a third case for this. -DMC
	     */
	    gc_val.stipple = tile_id1;
	    gc_val.fill_style = FillOpaqueStippled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCStipple, &gc_val);
	}
	/*
	 * use XV_USE_CMS_FG because the op in this case is PIX_SRC - nothing in the 
	 * color field
	 */
	xv_set_gc_op(display, info, fill_gc, PIX_SRC, XV_USE_CMS_FG, XV_DEFAULT_FG_BG);
	/*
	 * BUG ALERT! Wouldn't this be smaller using XSetTSOrigin? -DMC
	 */
	gc_val.ts_x_origin = gc_val.ts_y_origin = 0;
	XChangeGC(display, fill_gc, GCTileStipXOrigin | GCTileStipYOrigin, &gc_val);
	XFillPolygon(display, d, fill_gc, points, 3, Complex, CoordModeOrigin);

	/*
	 * 2nd triangle of the shadow
	 */
	points[1].x = leftoff;
	points[1].y = topoff;
	points[2].x = leftoff;
	points[2].y = rect.r_height + topoff;
        fill_gc = (GC)xv_find_proper_gc(display, info, PW_POLYGON2);
	if (depth == xv_depth(info)) {
	    gc_val.tile = tile_id2;
	    gc_val.fill_style = FillTiled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCTile, &gc_val);
	} else {
	    gc_val.stipple = tile_id2;
	    gc_val.fill_style = FillOpaqueStippled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCStipple, &gc_val);
	}
	xv_set_gc_op(display, info, fill_gc, PIX_SRC, XV_USE_CMS_FG, XV_DEFAULT_FG_BG);
	gc_val.ts_x_origin = gc_val.ts_y_origin = 0;
	XChangeGC(display, fill_gc, GCTileStipXOrigin | GCTileStipYOrigin, &gc_val);
	XFillPolygon(display, d, fill_gc, points, 3, Complex, CoordModeOrigin);
	break;
      case 1:			/* break down and to left */
	points[0].x = rect.r_width + leftoff;
	points[0].y = 0;
	points[1].x = rect.r_width;
	points[1].y = rect.r_height + topoff;
	points[2].x = rect.r_width;
	points[2].y = topoff;
        fill_gc = (GC)xv_find_proper_gc(display, info, PW_POLYGON2);
	if (depth == xv_depth(info)) {
	    gc_val.tile = tile_id1;
	    gc_val.fill_style = FillTiled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCTile, &gc_val);
	} else {
	    gc_val.stipple = tile_id1;
	    gc_val.fill_style = FillOpaqueStippled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCStipple, &gc_val);
	}
	xv_set_gc_op(display, info, fill_gc, PIX_SRC, XV_USE_CMS_FG, XV_DEFAULT_FG_BG);
	gc_val.ts_x_origin = gc_val.ts_y_origin = 0;
	XChangeGC(display, fill_gc, GCTileStipXOrigin | GCTileStipYOrigin, &gc_val);
	XFillPolygon(display, d, fill_gc, points, 3, Complex, CoordModeOrigin);

	points[1].x = 0;
	points[1].y = topoff;
        fill_gc = (GC)xv_find_proper_gc(display, info, PW_POLYGON2);
	if (depth == xv_depth(info)) {
	    gc_val.tile = tile_id2;
	    gc_val.fill_style = FillTiled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCTile, &gc_val);
	} else {
	    gc_val.stipple = tile_id2;
	    gc_val.fill_style = FillOpaqueStippled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCStipple, &gc_val);
	}
	xv_set_gc_op(display, info, fill_gc, PIX_SRC, XV_USE_CMS_FG, XV_DEFAULT_FG_BG);
	gc_val.ts_x_origin = gc_val.ts_y_origin = 0;
	XChangeGC(display, fill_gc, GCTileStipXOrigin | GCTileStipYOrigin, &gc_val);
	XFillPolygon(display, d, fill_gc, points, 3, Complex, CoordModeOrigin);
	break;
      case 2:			/* break up and to left */
	points[0].x = rect.r_width + leftoff;
	points[0].y = rect.r_height + topoff;
	points[1].x = rect.r_width;
	points[1].y = 0;
	points[2].x = rect.r_width;
	points[2].y = rect.r_height;
        fill_gc = (GC)xv_find_proper_gc(display, info, PW_POLYGON2);
	if (depth == xv_depth(info)) {
	    gc_val.tile = tile_id1;
	    gc_val.fill_style = FillTiled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCTile, &gc_val);
	} else {
	    gc_val.stipple = tile_id1;
	    gc_val.fill_style = FillOpaqueStippled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCStipple, &gc_val);
	}
	xv_set_gc_op(display, info, fill_gc, PIX_SRC, XV_USE_CMS_FG, XV_DEFAULT_FG_BG);
	gc_val.ts_x_origin = gc_val.ts_y_origin = 0;
	XChangeGC(display, fill_gc, GCTileStipXOrigin | GCTileStipYOrigin, &gc_val);
	XFillPolygon(display, d, fill_gc, points, 3, Complex, CoordModeOrigin);

	points[1].x = 0;
	points[1].y = rect.r_height;
        fill_gc = (GC)xv_find_proper_gc(display, info, PW_POLYGON2);
	if (depth == xv_depth(info)) {
	    gc_val.tile = tile_id2;
	    gc_val.fill_style = FillTiled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCTile, &gc_val);
	} else {
	    gc_val.stipple = tile_id2;
	    gc_val.fill_style = FillOpaqueStippled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCStipple, &gc_val);
	}
	xv_set_gc_op(display, info, fill_gc, PIX_SRC, XV_USE_CMS_FG, XV_DEFAULT_FG_BG);
	gc_val.ts_x_origin = gc_val.ts_y_origin = 0;
	XChangeGC(display, fill_gc, GCTileStipXOrigin | GCTileStipYOrigin, &gc_val);
	XFillPolygon(display, d, fill_gc, points, 3, Complex, CoordModeOrigin);
	break;
      case 3:			/* break up and to right */
	points[0].x = 0;
	points[0].y = rect.r_height + topoff;
	points[1].x = rect.r_width + leftoff;
	points[1].y = rect.r_height;
	points[2].x = leftoff;
	points[2].y = rect.r_height;
        fill_gc = (GC)xv_find_proper_gc(display, info, PW_POLYGON2);
	if (depth == xv_depth(info)) {
	    gc_val.tile = tile_id1;
	    gc_val.fill_style = FillTiled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCTile, &gc_val);
	} else {
	    gc_val.stipple = tile_id1;
	    gc_val.fill_style = FillOpaqueStippled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCStipple, &gc_val);
	}
	xv_set_gc_op(display, info, fill_gc, PIX_SRC, XV_USE_CMS_FG, XV_DEFAULT_FG_BG);
	gc_val.ts_x_origin = gc_val.ts_y_origin = 0;
	XChangeGC(display, fill_gc, GCTileStipXOrigin | GCTileStipYOrigin, &gc_val);
	XFillPolygon(display, d, fill_gc, points, 3, Complex, CoordModeOrigin);

	points[1].x = leftoff;
	points[1].y = 0;
        fill_gc = (GC)xv_find_proper_gc(display, info, PW_POLYGON2);
	if (depth == xv_depth(info)) {
	    gc_val.tile = tile_id2;
	    gc_val.fill_style = FillTiled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCTile, &gc_val);
	} else {
	    gc_val.stipple = tile_id2;
	    gc_val.fill_style = FillOpaqueStippled;
	    XChangeGC(display, fill_gc, GCFillStyle | GCStipple, &gc_val);
	}
	xv_set_gc_op(display, info, fill_gc, PIX_SRC, XV_USE_CMS_FG, XV_DEFAULT_FG_BG);
	gc_val.ts_x_origin = gc_val.ts_y_origin = 0;
	XChangeGC(display, fill_gc, GCTileStipXOrigin | GCTileStipYOrigin, &gc_val);
	XFillPolygon(display, d, fill_gc, points, 3, Complex, CoordModeOrigin);
	break;
    }

    if (spr1 && PR_NOT_SERVER_IMAGE(spr1)) {
	xv_destroy(tile1);
    }

    if (spr2 && PR_NOT_SERVER_IMAGE(spr2)) {
	xv_destroy(tile2);
    }

    /*
     * draw box
     */

    /*
     * Clear the box
     * Replace pw_ call with Xlib call
     * Get/set GC for XFillRectangle call
     */
    gc = xv_find_proper_gc(display, info, PW_ROP_NULL_SRC);
    xv_set_gc_op(display, info, gc, PIX_CLR, XV_USE_OP_FG, XV_DEFAULT_FG_BG);
    XFillRectangle(display, d, gc, rect.r_left, rect.r_top, rect.r_width, 
			rect.r_height);

    /*
     * Vector list for box
     */
    vlist[0].x = rect.r_left;
    vlist[0].y = rect.r_top;
    vlist[1].x = rect.r_left + rect.r_width;
    vlist[1].y = rect.r_top;
    vlist[2].x = rect.r_left + rect.r_width;
    vlist[2].y = rect.r_top + rect.r_height;
    vlist[3].x = rect.r_left;
    vlist[3].y = rect.r_top + rect.r_height;

    /*
     * Set GC for XDrawLine call
     */
    gc = (GC)xv_find_proper_gc(display, info, PW_VECTOR);
    /* The 1 in PIX_COLOR(1) is the cms_index */
    xv_set_gc_op(display, info, gc, 
    			(PIX_SET | PIX_COLOR(1)), 
			XV_USE_OP_FG, XV_DEFAULT_FG_BG);

    segments[i].x1 = vlist[0].x;
    segments[i].y1 = vlist[0].y;
    segments[i].x2 = vlist[1].x;
    segments[i++].y2 = vlist[1].y;

    segments[i].x1 = vlist[0].x;
    segments[i].y1 = vlist[0].y + 1;
    segments[i].x2 = vlist[1].x;
    segments[i++].y2 = vlist[1].y + 1;

    segments[i].x1 = vlist[1].x;
    segments[i].y1 = vlist[1].y;
    segments[i].x2 = vlist[2].x;
    segments[i++].y2 = vlist[2].y;

    segments[i].x1 = vlist[1].x - 1;
    segments[i].y1 = vlist[1].y;
    segments[i].x2 = vlist[2].x - 1;
    segments[i++].y2 = vlist[2].y;

    segments[i].x1 = vlist[2].x;
    segments[i].y1 = vlist[2].y;
    segments[i].x2 = vlist[3].x;
    segments[i++].y2 = vlist[3].y;

    segments[i].x1 = vlist[2].x;
    segments[i].y1 = vlist[2].y - 1;
    segments[i].x2 = vlist[3].x;
    segments[i++].y2 = vlist[3].y - 1;

    segments[i].x1 = vlist[3].x;
    segments[i].y1 = vlist[3].y;
    segments[i].x2 = vlist[0].x;
    segments[i++].y2 = vlist[0].y;

    segments[i].x1 = vlist[3].x + 1;
    segments[i].y1 = vlist[3].y;
    segments[i].x2 = vlist[0].x + 1;
    segments[i++].y2 = vlist[0].y;

    /*
     * Draw box borders
     */
    XDrawSegments(display, d, gc, segments, i);
}

static void
notice_build_button(pw, x, y, button, ginfo, three_d)
    Xv_Window       pw;
    int             x, y;
    notice_buttons_handle button;
    Graphics_info  *ginfo;
    int		   three_d;
{
    button->button_rect.r_top = y;
    button->button_rect.r_left = x;
    notice_paint_button(pw, button, NOTICE_NORMAL_BUTTON, ginfo, three_d);
}

static void
notice_paint_button(pw, button, invert, ginfo, three_d)
    Xv_Window	    pw;
    notice_buttons_handle button;
    int             invert;
    Graphics_info  *ginfo;
    int		    three_d;
{
    Xv_Drawable_info *info;
    int		    state;

    DRAWABLE_INFO_MACRO(pw, info);
    if (invert)
	state = OLGX_INVOKED;
    else if (three_d)
	state = OLGX_NORMAL;
    else
	state = OLGX_NORMAL | OLGX_ERASE;
    if (button->is_yes)
	state |= OLGX_DEFAULT;
#ifdef	DBG
	fprintf(stderr , "button string = %ws \n" , button->string );
#endif
    olgx_draw_button(ginfo, xv_xid(info), button->button_rect.r_left,
	button->button_rect.r_top, button->button_rect.r_width, 0,
	button->string, state);
}

static          notice_buttons_handle
notice_button_for_event(notice, x, y)
    register notice_handle notice;
{
    register notice_buttons_handle curr;

    if (notice->button_info == NULL)
	return (NULL);
    for (curr = notice->button_info; curr; curr = curr->next) {
	if ((x >= curr->button_rect.r_left - min_button_gap / 2)
	    && (x <= (curr->button_rect.r_left + min_button_gap / 2
		      + curr->button_rect.r_width))
	    && (y >= curr->button_rect.r_top)
	    && (y <= (curr->button_rect.r_top
		      + curr->button_rect.r_height))) {
	    return (curr);
	}
    }
    return ((notice_buttons_handle) 0);
}

static void
notice_do_bell(notice)
    notice_handle   notice;
{
    Xv_Drawable_info *info;
    struct timeval  wait;

    if (!notice_use_audible_bell)
	return;

    DRAWABLE_INFO_MACRO(notice->fullscreen_window, info);
    wait.tv_sec = 0;
    wait.tv_usec = 100000;
    if (!notice->dont_beep && (notice->beeps > 0)) {
	int             i = notice->beeps;
	while (i--)
	    win_beep(xv_display(info), wait);
    }
}

/*
 * Determines which font to use for notice window.
 * OPEN LOOK specifies it to be one scale larger than
 * the client window font.
 * 
 * The NOTICE pkg however tries to use the font of the client 
 * window. If that is not available, a default font is used.
 * Rescaling of fonts is not done because we dont want to depend
 * on the ability of the server to rescale fonts, or on the 
 * presence of fonts in the sizes/scales we want.
 */

#ifdef	OW_I18N

static int
notice_determine_font(client_window, notice)
Xv_Window       client_window;
notice_handle 	notice;
{
    Xv_Font	font = NULL;

    if (client_window)
	font = xv_get(client_window, XV_FONT);

    if (font == NULL) 
	defpf = font = (Xv_Font) xv_find(NULL, FONT, 
				FONT_FAMILY, FONT_FAMILY_DEFAULT,
				FONT_STYLE, FONT_STYLE_DEFAULT,
				FONT_SCALE, FONT_SCALE_DEFAULT,
				NULL);

    if (font == NULL) {
	xv_error(NULL,
	    ERROR_STRING,
		XV_I18N_MSG("xv_messages","Unable to find \"fixed\" font. (Notice package)"),
	NULL);
	return(XV_ERROR);
    }

    notice->notice_font = font;
    return(XV_OK);
}

#else /*OW_I18N*/

static int
notice_determine_font(client_window, notice)
Xv_Window       client_window;
notice_handle 	notice;
{
    Xv_Font         		client_font;
    Xv_Font			default_font;

    /*
     * Get client window font
     */
    if (client_window) {
	client_font = xv_get(client_window, XV_FONT);

	/*
	 * If cannot find client window font, try to find
	 * default font
	 */
	if (!client_font)  {
            default_font = (Xv_Font) xv_find(NULL, FONT,
			      FONT_FAMILY, FONT_FAMILY_DEFAULT,
			      FONT_STYLE, FONT_STYLE_DEFAULT,
			      FONT_SCALE, FONT_SCALE_DEFAULT,
			      0);

	    if (!default_font)  {
		/*
		 * If cannot find default font, find fixed font
		 */
	        default_font = (Xv_Font) xv_find(client_window, FONT,
				      FONT_NAME, "fixed",
				      0);

		/*
		 * If all the above fails, return error code
		 */
	        if (!default_font) {
		    xv_error(NULL,
			 ERROR_STRING,
			     "Unable to find \"fixed\" font. (Notice package)",
			 0);
		    return (XV_ERROR);
	        }
	    }

	    /*
	     * set notice font to default_font
	     */
            notice->notice_font = default_font;
	}
	else  {
	    /*
	     * set notice font to client_font
	     */
            notice->notice_font = client_font;
	}

    } else {
	/*
	 * If no client window, we need to find default font, as above
	 */
        default_font = (Xv_Font) xv_find(NULL, FONT,
			      FONT_FAMILY, FONT_FAMILY_DEFAULT,
			      FONT_STYLE, FONT_STYLE_DEFAULT,
			      FONT_SCALE, FONT_SCALE_DEFAULT,
			      0);
	if (!default_font)  {
	    /*
	     * If cannot find default font, find fixed font
	     */
	    default_font = (Xv_Font) xv_find(client_window, FONT,
	                                    FONT_NAME, "fixed",
	                                    0);

	    /*
	     * If all the above fails, return error code
	     */
	    if (!default_font) {
	        xv_error(NULL,
	                ERROR_STRING,
	                "Unable to find \"fixed\" font. (Notice package)",
	                0);
	        return (XV_ERROR);
	    }
	}

	/*
	 * set notice font to default_font
	 */
        notice->notice_font = default_font;
    }

    if (default_font)  {
	defpf = default_font;
    }

    return(XV_OK);

}

#endif OW_I18N


#ifdef	OW_I18N
static CHAR	**
notice_string_set( notice, value ,attr)
register	notice_handle	notice;
CHAR		*value;
Attr_attribute	attr;
{
    register	wchar_t	*wc; 	  /* pointer which moves in a string */
    register	char	*c;
    register	int	char_num = 0;  /* length of the string            */
    int		line_num  = 0;     /* # of lines 		     */
    register	wchar_t	w_ret = (wchar_t)'\n';
    register	char	ret = '\n';
    char	**pptr,*ptr;
    int		len;
    int		sum_len;
    int		n;

    if(!value)
	return NULL;

/** count # of chars and # of lines in each case **/
    switch(attr) {
      case  NOTICE_MESSAGE_STRINGS:
      case  NOTICE_MESSAGE_STRINGS_ARRAY_PTR:
	pptr = (char **)value;
	for( ; *pptr ; pptr++ , line_num ++ ) {
		char_num += (1+strlen( *pptr ));
		for (c = *pptr; *c; c++)
			if (*c == ret)
				line_num++;
	}
	break;
      case  NOTICE_MESSAGE_STRING:
	c = (char *)value;
    	for(  ; *c ; c++ , char_num++ ) {
		if( *c == ret ) {
			line_num++;
		}
    	}
	line_num +=1;
	break;
      case  NOTICE_MESSAGE_STRING_WCS:
	wc = value;
    	for(  ; *wc ; wc++ , char_num++ ) {
		if( *wc == w_ret ) {
			line_num++;
		}
    	}
	line_num +=1;
	break;
    }
	
/**  allocate space  **/
    if( !notice->text_string ) {
        if( (notice->text_string = (CHAR *)malloc(sizeof(wchar_t)*(char_num+1))) == NULL ) {
	   goto	MALLOC_FAILED;
	}
	notice->text_nchars = char_num;
    }
    else if( notice->text_nchars < char_num ) {
	if( (notice->text_string = 
	      (CHAR *)realloc(notice->text_string,
				sizeof(wchar_t *) * (char_num+1))) == NULL ) {
	   goto	MALLOC_FAILED;
	}
	notice->text_nchars = char_num;
    }

    if( !notice->text_ptrs ) {
        if( (notice->text_ptrs = 
    	      (CHAR **)malloc(sizeof(wchar_t *) *(line_num+1) )) == NULL ){
	   goto	MALLOC_FAILED;
	}
	notice->text_nstrings = line_num;
    }
    else if( notice->text_nstrings < line_num ) {
        if( (notice->text_ptrs = 
    	        (CHAR **)realloc( notice->text_ptrs , 
				sizeof(wchar_t *) *(line_num+1) )) == NULL ){
	   goto	MALLOC_FAILED;
	}
	notice->text_nstrings = line_num;
    }
	
    switch( attr ) {
      case  NOTICE_MESSAGE_STRINGS_ARRAY_PTR:
      case  NOTICE_MESSAGE_STRINGS:
	pptr = (char **)value;
	sum_len = 0;
	for( n = 0 ; n < line_num ; n++ ){
	    len = strlen( *pptr );
	    wc = &(notice->text_string[sum_len]);
	    mbstowcs( wc , *pptr , len );
	    while (1) {
		*notice->text_ptrs++ = wc;
		if ((wc = wschr(wc, w_ret)) == NULL)
		    break;
		*wc++ = 0;
		n++;
	    }
	    sum_len += len;
	    notice->text_string[sum_len] = 0;
	    sum_len++;
	    pptr++;
	}
	*(notice->text_ptrs) = 0;
	notice->text_ptrs -= line_num;
	return( notice->text_ptrs );

      case  NOTICE_MESSAGE_STRING:
      case  NOTICE_MESSAGE_STRING_WCS:
	if( attr == NOTICE_MESSAGE_STRING )
    	 	mbstowcs( notice->text_string , (char *)value , char_num );
	else
		wscpy( notice->text_string , value );
    	*notice->text_ptrs = notice->text_string;
  	notice->text_ptrs += line_num;
   	*notice->text_ptrs = NULL;
        wc = notice->text_string + char_num - 1;
        for( ; char_num > 0 ; char_num-- ) {
	    if( *wc == w_ret ) {
		*(--notice->text_ptrs) = wc + 1;
		*wc = 0;
    	    }
	    --wc;
        }
        return( --notice->text_ptrs );
    }


MALLOC_FAILED:
    xv_error(NULL,
	     ERROR_STRING, XV_I18N_MSG("xv_messages","Malloc failed. (Notice package)"),
	     0);
    return (NULL);
}

static CHAR	*
notice_button_string_set(button,value)
notice_buttons_handle	button;
char			*value;
{
    if ((button->string=
		(CHAR *)mbstowcsdup(value)) == NULL ) {
	   goto	MALLOC_FAILED;
    }

    return( button->string );

MALLOC_FAILED:
    xv_error(NULL,
	     ERROR_STRING, XV_I18N_MSG("xv_messages","Malloc failed. (Notice package)"),
	     0);
    return (NULL);

}

#endif
