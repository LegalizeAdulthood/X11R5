#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)panel.c 70.4 91/08/01";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <X11/X.h>
#ifdef OW_I18N
#include <X11/Xlib.h>
#include <X11/XSunExt.h>
#endif
#include <xview_private/panel_impl.h>
#include <xview_private/draw_impl.h>
#include <xview/font.h>
#include <xview/defaults.h>
#include <xview/notify.h>


Xv_private void win_set_no_focus();
Pkg_private int panel_init();
Pkg_private int panel_view_init();
Pkg_private int panel_destroy();
static int      panel_layout();
static void     free_panel();

/* default timer value */
static struct itimerval PANEL_TIMER = {0, 500000, 0, 500000};

Attr_attribute  panel_context_key;


/* Note that this must be modified when the ops structure is modified. */
static Panel_ops default_panel_ops = {
    panel_default_handle_event,		/* handle_event() */
    (void (*) ()) panel_nullproc,	/* begin_preview() */
    (void (*) ()) panel_nullproc,	/* update_preview() */
    (void (*) ()) panel_nullproc,	/* cancel_preview() */
    (void (*) ()) panel_nullproc,	/* accept_preview() */
    (void (*) ()) panel_nullproc,	/* accept_menu() */
    (void (*) ()) panel_nullproc,	/* accept_key() */
    (void (*) ()) panel_display,	/* paint() */
    (void (*) ()) panel_nullproc,	/* remove() */
    (void (*) ()) panel_nullproc,	/* restore() */
    (void (*) ()) panel_nullproc,	/* layout() */
    (void (*) ()) panel_nullproc,	/* accept_kbd_focus() */
    (void (*) ()) panel_nullproc	/* yield_kbd_focus() */
};



/*ARGSUSED*/
Pkg_private int
panel_init(parent, panel_public, avlist)
    Xv_Window       parent, panel_public;
    Attr_avlist     avlist;
{
    register Panel_info *panel;
    Xv_panel       *panel_object = (Xv_panel *) panel_public;
    Xv_Drawable_info *info;

#ifdef OW_I18N
    Attr_avlist	    attrs;
#endif OW_I18N

    DRAWABLE_INFO_MACRO(panel_public, info);

    /* initial context key if necessary */
    if (panel_context_key == (Attr_attribute) 0) {
	panel_context_key = xv_unique_key();
    }
    panel = xv_alloc(Panel_info);

    /* link to object */
    panel_object->private_data = (Xv_opaque) panel;
    panel->public_self = panel_public;

    panel->caret = NULL;
    panel->caret_on = FALSE;
    panel->current_col_x = PANEL_ITEM_X_START;
    panel->extra_height = 1;
    panel->extra_width = 1;
    panel->event_proc = NULL;
    panel->h_margin = 4;
    panel->item_x = PANEL_ITEM_X_START;
    panel->item_x_offset = ITEM_X_GAP;
    panel->item_y = PANEL_ITEM_Y_START;
    panel->item_y_offset = ITEM_Y_GAP;
    panel->flags = IS_PANEL;
    panel->layout = PANEL_HORIZONTAL;
    panel->ops = &default_panel_ops;
    panel->repaint = PANEL_CLEAR;
    panel->status = ADJUST_IS_PENDING_DELETE;
    if (defaults_get_boolean("OpenWindows.SelectDisplaysMenu",
	"OpenWindows.SelectDisplaysMenu", FALSE))
	/* SELECT => Display menu default */
	panel->status |= SELECT_DISPLAYS_MENU;
    if (xv_depth(info) > 1)
	panel->three_d = defaults_get_boolean("OpenWindows.3DLook.Color",
	    "OpenWindows.3DLook.Color", TRUE);
    else
#ifdef MONO3D
	panel->three_d = defaults_get_boolean("OpenWindows.3DLook.Monochrome",
	    "OpenWindows.3DLook.Monochrome", FALSE);
#else
	panel->three_d = FALSE;
#endif
    panel->timer_full = PANEL_TIMER;
    panel->v_margin = 4;

    panel->layout_proc = (int (*) ()) xv_get(panel_public, WIN_LAYOUT_PROC);

    (void) xv_set(panel_public,
		  WIN_TOP_MARGIN, PANEL_ITEM_Y_START,
		  WIN_LEFT_MARGIN, PANEL_ITEM_X_START,
		  WIN_ROW_GAP, ITEM_Y_GAP,
		  WIN_LAYOUT_PROC, panel_layout,
		  XV_HELP_DATA, "xview:panel",
		  0);

    if (xv_get(panel_public, XV_IS_SUBTYPE_OF, CANVAS)) {
	(void) xv_set(panel_public,
		      WIN_NOTIFY_SAFE_EVENT_PROC, panel_notify_panel_event,
		      WIN_NOTIFY_IMMEDIATE_EVENT_PROC, panel_notify_panel_event,
		      CANVAS_AUTO_EXPAND, TRUE,
		      CANVAS_AUTO_SHRINK, TRUE,
		      CANVAS_FIXED_IMAGE, FALSE,
		      CANVAS_REPAINT_PROC, panel_redisplay,
		      CANVAS_RETAINED, FALSE,
		      OPENWIN_SHOW_BORDERS, FALSE,
		      0);
	win_set_no_focus(panel_public, TRUE);   /* panel sets own focus */
    } else
	panel_view_init(panel_public, XV_NULL, 0);

#ifdef OW_I18N
     /* Cache the instance quark list of the panel for fast access */
     panel->instance_qlist = xv_get(panel_public, XV_INSTANCE_QLIST);

    /* Initialize the panel callback structs			   */

    panel->start_pecb_struct.callback = NULL;
    panel->start_pecb_struct.client_data = NULL;
    panel->draw_pecb_struct.callback = NULL;
    panel->draw_pecb_struct.client_data = NULL;
    panel->done_pecb_struct.callback = NULL;
    panel->done_pecb_struct.client_data = NULL;

    panel->start_stcb_struct.callback = NULL;
    panel->start_stcb_struct.client_data = NULL;
    panel->draw_stcb_struct.callback = NULL;
    panel->draw_stcb_struct.client_data = NULL;
    panel->done_stcb_struct.callback = NULL;
    panel->done_stcb_struct.client_data = NULL;
 
    panel->start_luc_struct.callback = NULL;
    panel->start_luc_struct.client_data = NULL;
    panel->draw_luc_struct.callback = NULL;
    panel->draw_luc_struct.client_data = NULL;
    panel->done_luc_struct.callback = NULL;
    panel->done_luc_struct.client_data = NULL;
    panel->process_luc_struct.callback = NULL;
    panel->process_luc_struct.client_data  = NULL;
    
    panel->start_aux_struct.callback = NULL;
    panel->start_aux_struct.client_data = NULL;
    panel->draw_aux_struct.callback = NULL;
    panel->draw_aux_struct.client_data = NULL;
    panel->done_aux_struct.callback = NULL;
    panel->done_aux_struct.client_data = NULL;


    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
        switch (attrs[0]) {
            
            case WIN_IM_PREEDIT_START:
                panel->start_pecb_struct.callback = (XIMProc)attrs[1];
                panel->start_pecb_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_PREEDIT_DRAW:
                panel->draw_pecb_struct.callback = (XIMProc)attrs[1];
                panel->draw_pecb_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_PREEDIT_DONE:
                panel->done_pecb_struct.callback = (XIMProc)attrs[1];
                panel->done_pecb_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_STATUS_START:
                panel->start_stcb_struct.callback = (XIMProc)attrs[1];
                panel->start_stcb_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_STATUS_DRAW:
                panel->draw_stcb_struct.callback = (XIMProc)attrs[1];
                panel->draw_stcb_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_STATUS_DONE:
                panel->done_stcb_struct.callback = (XIMProc)attrs[1];
                panel->done_stcb_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_LUC_START:
                panel->start_luc_struct.callback = (XIMProc)attrs[1];
                panel->start_luc_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_LUC_DRAW:
                panel->draw_luc_struct.callback = (XIMProc)attrs[1];
                panel->draw_luc_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_LUC_DONE:
                panel->done_luc_struct.callback = (XIMProc)attrs[1];
                panel->done_luc_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_LUC_PROCESS:
                panel->process_luc_struct.callback = (XIMProc)attrs[1];
                panel->process_luc_struct.client_data  = (XPointer)attrs[2];
                break;

	    default:
		break;
 
        }
    }
  
#endif OW_I18N

    return XV_OK;
}

Pkg_private int
panel_destroy(panel_public, status)
    Panel           panel_public;
    Destroy_status  status;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);
    Panel_item      item;

    if (status == DESTROY_PROCESS_DEATH)
	panel->destroying = TRUE;
    else if (status == DESTROY_CLEANUP) {
	/* unlink layout procs */
	xv_set(panel_public,
	       WIN_LAYOUT_PROC, panel->layout_proc,
	       0);
	panel_itimer_set(panel_public, NOTIFY_NO_ITIMER);
	panel->destroying = TRUE;
    }
#ifdef OW_I18N
    if (panel->ic) {
        XDestroyIC(panel->ic);
        panel->ic = NULL;
    }
    if (panel->interm_text) {
        free(panel->interm_text);
        panel->interm_text = NULL;
    }
    if (panel->interm_attr) {
        free(panel->interm_attr);
        panel->interm_attr = NULL;
    }   
#endif /* OW_I18N */ 
   
    PANEL_EACH_ITEM(panel_public, item)
	if (xv_destroy_status(item, status) != XV_OK)
	    return XV_ERROR;
    PANEL_END_EACH

    if (status == DESTROY_CLEANUP) {
	free_panel(panel);
    }
    return XV_OK;
}

static
panel_layout(panel_public, child, op, d1, d2, d3, d4, d5)
    Panel           panel_public;
    Xv_Window       child;
    Window_layout_op op;
    int             d1, d2, d3, d4, d5;
{
    Panel_info     *panel = PANEL_PRIVATE(panel_public);

    switch (op) {
      case WIN_DESTROY:
	panel_unregister_view(panel, child);
	break;
      case WIN_CREATE:
	if ((int) xv_get(child, XV_IS_SUBTYPE_OF, PANEL_VIEW) == TRUE) {
	    panel_register_view(panel, child);
	}
      default:
	break;
    }

    if (panel->layout_proc != NULL) {
	return (panel->layout_proc(panel_public, child, op, d1, d2, d3, d4, d5));
    } else {
	return TRUE;
    }
}

static void
free_panel(panel)
    register Panel_info *panel;
{
    Panel_paint_window *pw;

    /* clean up with selection service */
    panel_seln_destroy(panel);

    /* free the ops vector storage */
    if (ops_set(panel)) {
	free((char *) panel->ops);
    }
    /* free storage for each paint window */
    while (panel->paint_window != NULL) {
	pw = panel->paint_window->next;
	free((char *) panel->paint_window);
	panel->paint_window = pw;
    }

    free((char *) panel);
}

Pkg_private void
panel_register_view(panel, view)
    Panel_info     *panel;
    Xv_Window       view;
{
    Panel_paint_window *paint_window_data;
    Xv_Window       pw;

    pw = view ? xv_get(view, CANVAS_VIEW_PAINT_WINDOW) : panel->public_self;
    paint_window_data = xv_alloc(Panel_paint_window);
    paint_window_data->pw = pw;
    paint_window_data->view = view;
    paint_window_data->next = panel->paint_window;
    panel->paint_window = paint_window_data;

    (void) xv_set(pw,
		  XV_KEY_DATA, panel_context_key, panel,
		  0);
    win_set_no_focus(pw, TRUE);	/* panel sets own focus */
}

static
panel_unregister_view(panel, view)
    Panel_info     *panel;
    Xv_Window       view;
{
    Panel_paint_window *pw_data, *lpw_data = NULL;

    for (pw_data = panel->paint_window; pw_data != NULL; pw_data = pw_data->next) {
	if (pw_data->view == view) {
	    if (lpw_data != NULL) {
		lpw_data->next = pw_data->next;
	    } else {
		panel->paint_window = pw_data->next;
	    }
	    xv_free(pw_data);
	}
    }

    return (XV_OK);
}
