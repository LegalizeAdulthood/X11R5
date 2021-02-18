#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_set.c 70.9 91/08/29";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>
#include <xview/font.h>
#include <xview/scrollbar.h>
#include <xview/xv_xrect.h>
#include <xview/font.h>
#include <xview_private/draw_impl.h>
#ifdef	OW_I18N
#include <xview_private/xv_i18n_impl.h>
#include <xview_private/db_impl.h>
#include <xview/xv_i18n.h>
#include <xview/server.h>
#include <X11/Xlib.h>
#include <X11/XSunExt.h>
#endif	OW_I18N
#include <olgx/olgx.h>

Xv_private void	openwin_create_gcs();
Xv_private Graphics_info *xv_init_olgx();


static void panel_set_fonts();

#ifdef OW_I18N
#include <xview_private/xv_i18n_impl.h>
#endif

#ifdef OW_I18N
static XVaNestedList
panel_make_pre_edit_cb_list(panel_public)
    Panel           panel_public;
{
    Panel_info 	*panel = PANEL_PRIVATE(panel_public);
    int		dummy;
    extern	void	panel_text_start();
    extern	void	panel_text_draw();
    extern	void	panel_text_done();

    
	if (!panel->start_pecb_struct.callback) {
	    panel->start_pecb_struct.callback =
		(XIMProc)panel_text_start; 
	    panel->start_pecb_struct.client_data =
		(XPointer)panel_public; 
	}
	if (!panel->draw_pecb_struct.callback) {
	    panel->draw_pecb_struct.callback =
		(XIMProc)panel_text_draw; 
	    panel->draw_pecb_struct.client_data =
		(XPointer)panel_public; 
	}
	if (!panel->done_pecb_struct.callback) {
	    panel->done_pecb_struct.callback =
		(XIMProc)panel_text_done; 
	    panel->done_pecb_struct.client_data =
		(XPointer)panel_public; 
	}

	    /* Should do caret too */

        return(XVaCreateNestedList(dummy,
			XNPreeditStartCallback, &panel->start_pecb_struct,
			XNPreeditDrawCallback, &panel->draw_pecb_struct,
			XNPreeditDoneCallback, &panel->done_pecb_struct,
			NULL));
}

static XVaNestedList
panel_make_status_cb_list(panel_public)
    Panel           panel_public;
{
    Panel_info *panel = PANEL_PRIVATE(panel_public);

    int			dummy;
    Frame		frame_public = (Frame) xv_get(panel_public, WIN_FRAME);
    Xv_private	void	status_start();
    Xv_private	void	status_done();
    Xv_private	void	status_draw();
    
	if (!panel->start_stcb_struct.callback) {
	    panel->start_stcb_struct.callback =
		(XIMProc)status_start; 
	    panel->start_stcb_struct.client_data =
		(XPointer)frame_public; 
	}
	if (!panel->draw_stcb_struct.callback) {
	    panel->draw_stcb_struct.callback =
		(XIMProc)status_draw; 
	    panel->draw_stcb_struct.client_data =
		(XPointer)frame_public; 
	}
	if (!panel->done_stcb_struct.callback) {
	    panel->done_stcb_struct.callback =
		(XIMProc)status_done; 
	    panel->done_stcb_struct.client_data =
		(XPointer)frame_public; 
	}

        return(XVaCreateNestedList(dummy,
			XNStatusStartCallback, &panel->start_stcb_struct,
			XNStatusDrawCallback, &panel->draw_stcb_struct,
			XNStatusDoneCallback, &panel->done_stcb_struct,
			NULL));
}

static XVaNestedList
panel_make_lookup_cb_list(panel_public)
    Panel           panel_public;

{
    Panel_info 	*panel = PANEL_PRIVATE(panel_public);
    XPointer		luc_client_data = (XPointer)malloc( sizeof(int *));
    int			dummy;
    extern Window   	lookup_choices_start();
    extern void	    	lookup_choices_draw(),lookup_choices_done();
    extern int	    	lookup_choices_process();

	if (!panel->start_luc_struct.callback) {
	    panel->start_luc_struct.callback =
		(XIMProc)lookup_choices_start;
	    panel->start_luc_struct.client_data =
		(XPointer)luc_client_data;
	}
	if (!panel->draw_luc_struct.callback) {
	    panel->draw_luc_struct.callback =
		(XIMProc)lookup_choices_draw;
	    panel->draw_luc_struct.client_data =
		(XPointer)luc_client_data;
	}
	if (!panel->process_luc_struct.callback) {
	    panel->process_luc_struct.callback =
		(XIMProc)lookup_choices_process;
	    panel->process_luc_struct.client_data =
		(XPointer)luc_client_data;
	}
	if (!panel->done_luc_struct.callback) {
	    panel->done_luc_struct.callback =
		(XIMProc)lookup_choices_done;
	    panel->done_luc_struct.client_data =
		(XPointer)luc_client_data;
	}
        return(XVaCreateNestedList(dummy,
			XNExtXimp_LookupStartCallback, &panel->start_luc_struct,
			XNExtXimp_LookupDrawCallback,  &panel->draw_luc_struct,
			XNExtXimp_LookupProcessCallback, &panel->process_luc_struct,
			XNExtXimp_LookupDoneCallback,  &panel->done_luc_struct,
			NULL));

}

static XVaNestedList
panel_make_aux_cb_list(panel_public)
    Panel           panel_public;
{
    Panel_info *panel = PANEL_PRIVATE(panel_public);
    XPointer	aux_clientdata = (XPointer)malloc( sizeof(int *));
    int		dummy;
    extern void		aux_start(),aux_draw(),aux_done();
    
	if (!panel->start_aux_struct.callback) {
	    panel->start_aux_struct.client_data =
		(XPointer)aux_clientdata;
	    panel->start_aux_struct.callback =
		(XIMProc)aux_start;
	}
	if (!panel->draw_aux_struct.callback) {
		panel->draw_aux_struct.client_data =
		(XPointer)aux_clientdata;
	    panel->draw_aux_struct.callback =
		(XIMProc)aux_draw;
	}
	if (!panel->done_aux_struct.callback) {
	    panel->done_aux_struct.client_data =
		(XPointer)aux_clientdata;
	    panel->done_aux_struct.callback =
		(XIMProc)aux_done;
	}

        return(XVaCreateNestedList(dummy,
			XNExtXimp_AuxStartCallback, &panel->start_aux_struct,
			XNExtXimp_AuxDrawCallback, &panel->draw_aux_struct,
			XNExtXimp_AuxDoneCallback, &panel->done_aux_struct,
			NULL));

}


#endif /* OW_I18N */

Pkg_private     Xv_opaque
panel_set_avlist(panel_public, avlist)
    Panel           panel_public;
    register Attr_avlist avlist;
{
    register Attr_attribute attr;
    register Panel_info *panel = PANEL_PRIVATE(panel_public);
    int             extra_width = -1, extra_height = -1;
    Xv_Drawable_info *info;
    Scrollbar       new_h_scrollbar = 0;
    Panel_ops      *new_ops;
    Scrollbar       new_v_scrollbar = 0;
#ifdef	OW_I18N
    XID		db;
    int		db_count= 0;
    XIM		im;
    XIC		ic;
    Xv_object	server;
    Window	window;

    server = XV_SERVER_FROM_WINDOW(panel_public);
    db = xv_get(server, SERVER_RESOURCE_DB);
    attr = avlist[0];
    while (attr || db_count) {
	switch (attr) {

          case XV_USE_DB:
	    db_count++;
            break;

	  case PANEL_CLIENT_DATA:
	    panel->client_data = avlist[1];
	    break;

	  case PANEL_BOLD_FONT:
	    /* Sunview1 compatibility attribute: not used */
	    break;

	  case PANEL_BLINK_CARET:
	    if ((int) avlist[1])
		panel->status |= BLINKING;
	    else
		panel->status &= ~BLINKING;
	    if (panel->kbd_focus_item->item_type == PANEL_TEXT_ITEM)
		panel_text_caret_on(panel, TRUE);
	    break;

	  case PANEL_CARET_ITEM:
	    panel_yield_kbd_focus(panel);
	    panel->kbd_focus_item = ITEM_PRIVATE(avlist[1]);
	    panel_accept_kbd_focus(panel);
	    break;

	  case PANEL_EVENT_PROC:
	    panel->event_proc = (int (*) ()) avlist[1];
	    break;

	  case PANEL_REPAINT_PROC:
	    panel->repaint_proc = (int (*) ()) avlist[1];
	    break;

	  case PANEL_BACKGROUND_PROC:
	    if (!ops_set(panel)) {
		new_ops = xv_alloc(Panel_ops);
		*new_ops = *panel->ops;
		panel->ops = new_ops;
		panel->flags |= OPS_SET;
	    }
	    panel->ops->handle_event = (void (*) ()) avlist[1];
	    if (!panel->ops->handle_event)
		panel->ops->handle_event = (void (*) ()) panel_nullproc;
	    break;

	  case PANEL_ITEM_X_GAP:
            if (db_count) {
		panel->item_x_offset = (int)db_get_data(db,
						panel->instance_qlist,
						"panel_item_x_gap", XV_INT,
						(int) avlist[1]);
            } else
		panel->item_x_offset = (int) avlist[1];
	    if (panel->item_x_offset < 1)
		panel->item_x_offset = 1;
	    break;

	  case PANEL_ITEM_Y_GAP:
            if (db_count) {
                panel->item_y_offset = (int)db_get_data(db,
						panel->instance_qlist,
						"panel_item_y_gap", XV_INT,
						(int)avlist[1]);
            } else 
		panel->item_y_offset = (int) avlist[1];
	    if (panel->item_y_offset < 1)
		panel->item_y_offset = 1;
	    break;

	  case PANEL_EXTRA_PAINT_WIDTH:
            if (db_count) {
                extra_width = (int)db_get_data(db,
					panel->instance_qlist,
					"extra_width", XV_INT,
					(int)avlist[1]);
            } else 
		extra_width = (int) avlist[1];
	    break;

	  case PANEL_EXTRA_PAINT_HEIGHT:
            if (db_count) {
                extra_height = (int)db_get_data(db,
					panel->instance_qlist,
					"extra_height", XV_INT,
					(int)avlist[1]);
            } else 
		extra_height = (int) avlist[1];
	    break;

	  case PANEL_LABEL_INVERTED:
	    if (avlist[1])
		panel->flags |= LABEL_INVERTED;
	    else
		panel->flags &= ~LABEL_INVERTED;
	    break;

	  case PANEL_LAYOUT:
	    switch ((Panel_setting) avlist[1]) {
	      case PANEL_HORIZONTAL:
	      case PANEL_VERTICAL:
		panel->layout = (Panel_setting) avlist[1];
		break;
	      default:		/* invalid layout */
		break;
	    }
	    break;

	  case PANEL_PAINT:
	    panel->repaint = (Panel_setting) avlist[1];
	    break;

	  case WIN_VERTICAL_SCROLLBAR:
	  case OPENWIN_VERTICAL_SCROLLBAR:
	    new_v_scrollbar = (Scrollbar) avlist[1];
	    break;

	  case WIN_HORIZONTAL_SCROLLBAR:
	  case OPENWIN_HORIZONTAL_SCROLLBAR:
	    new_h_scrollbar = (Scrollbar) avlist[1];
	    break;

	  case PANEL_ACCEPT_KEYSTROKE:
	    if (avlist[1])
		panel->flags |= WANTS_KEY;
	    else
		panel->flags &= ~WANTS_KEY;
	    break;

	  case PANEL_DEFAULT_ITEM:
	    panel->default_item = (Panel_item) avlist[1];
	    break;

	  case WIN_REMOVE_CARET:
	    if (panel->kbd_focus_item &&
		panel->kbd_focus_item->item_type == PANEL_TEXT_ITEM) {
		/* Clear caret */
		panel_text_caret_on(panel, FALSE);
	    }
	    panel->caret = NULL;
	    break;

#ifdef VERSION_3
	  case WIN_FOREGROUND_COLOR:
	  case WIN_BACKGROUND_COLOR:
	    if (panel->three_d) {
		char error_string[64];
		sprintf(error_string, XV_I18N_MSG("xv_messages",
					"%s not valid on a 3D Panel"),
		    attr == WIN_FOREGROUND_COLOR ? "WIN_FOREGROUND_COLOR" :
		    "WIN_BACKGROUND_COLOR");
		xv_error(panel_public,
			 ERROR_STRING, error_string,
			 0);
		ATTR_CONSUME(avlist[0]);
	    }
	    break;
#endif /* VERSION_3 */

	  case WIN_SET_FOCUS:{
		Xv_Window       pw;

		ATTR_CONSUME(avlist[0]);

		if (!wants_key(panel) && !panel->kbd_focus_item)
		    return ((Xv_opaque) XV_ERROR);

		/*
		 * Find the first paint window that can accept kbd input and
		 * set the input focus to it.  Only do this if we have a
		 * caret/text item or the panel wants the key.  Since panels
		 * always assigns their own input focus, there is no need to
		 * check xv_no_focus().
		 */
		PANEL_EACH_PAINT_WINDOW(panel, pw)
		    DRAWABLE_INFO_MACRO(pw, info);
		    if (wants_key(panel) || 
			win_getinputcodebit(
			    (Inputmask *) xv_get(pw, WIN_INPUT_MASK),
			    KBD_USE)) {
			win_set_kbd_focus(panel_public, xv_xid(info));
			return (XV_OK);
		    }
		PANEL_END_EACH_PAINT_WINDOW
		return ((Xv_opaque) XV_ERROR);
	    }

	  case XV_FONT:
	    panel->std_font = (Pixfont *) avlist[1];	    	
	    panel_set_fonts(panel_public, panel);
	    break;

	  case XV_END_CREATE:
	    /* Set up the fonts */
	    panel->std_font = (Pixfont *) xv_get(panel_public, XV_FONT);
	    panel_set_fonts(panel_public, panel);

	    /* Set up the Colormap Segment and OLGX */
	    panel->ginfo = xv_init_olgx(panel_public, &panel->three_d);

	    if (!panel->paint_window) {
		/* PANEL instead of SCROLLABLE_PANEL:
		 *   set up paint_window structure, create Openwin GC list.
		 */
		panel_register_view(panel, XV_NULL);
		/* Create Openwin GC's, based on this Colormap Segment */
		openwin_create_gcs(panel_public);
	    } else {
		Pixmap bg_pixmap = (Pixmap) xv_get(panel_public,
		    WIN_BACKGROUND_PIXMAP);
		if (bg_pixmap)
		    xv_set(panel->paint_window->pw,
			   WIN_BACKGROUND_PIXMAP, bg_pixmap,
			   0);
	    }

	    xv_set(panel_public,
		   WIN_ROW_HEIGHT, panel->ginfo->button_height,
		   0);

	    if (xv_get(panel_public,WIN_USE_IM) == TRUE) {
	        XIMStyles   *style = NULL;
		/* create input context for the window */
		
		server = XV_SERVER_FROM_WINDOW(panel_public);
		/* Get the im form server in order to pass to XCreateIC */
		im = (XIM)xv_get(server, XV_IM);
		if (!im)
		    break;
		XGetIMValues(im, XNQueryInputStyle, &style, NULL);
    		
		if (style) {
		    XVaNestedList   pre_edit_list, status_list,
				    lookup_list, aux_list;
#define	 XV_SUPPORTED_STYLE_COUNT		7		    	    
				    
	    	    XIMStyle	    supported_styles[XV_SUPPORTED_STYLE_COUNT];
	    	    int		    i, j;    	    
				    
		    pre_edit_list = status_list = lookup_list = aux_list = NULL;
		    supported_styles[0] = (XIMPreeditCallbacks | XIMStatusCallbacks |
					   XIMLookupCallbacks | XIMAuxCallbacks);
		    supported_styles[1] = (XIMPreeditCallbacks | XIMStatusCallbacks |
					   XIMLookupCallbacks);
		    supported_styles[2] = (XIMPreeditCallbacks | XIMStatusCallbacks |
					   XIMAuxCallbacks);
		    supported_styles[3] = (XIMPreeditCallbacks | XIMStatusCallbacks);
		    supported_styles[4] = (XIMPreeditCallbacks);
		    supported_styles[5] = (XIMPreeditNothing | XIMStatusCallbacks);
		    supported_styles[6] = (XIMPreeditNothing | XIMStatusNothing);
					  
		    for (j = 0; j < XV_SUPPORTED_STYLE_COUNT; j++) {
			for (i = 0; i < style->count_styles; i++) 
			    if ((style->supported_styles[i] & supported_styles[j])
				 == supported_styles[j])
				goto Found_it;
		    }
Found_it:	    		    
		    XFree(style);
		    
		DRAWABLE_INFO_MACRO(panel_public, info);  
		
		if ((supported_styles[j] & XIMPreeditCallbacks) &&
		    (supported_styles[j] & XIMStatusCallbacks) &&
		    (supported_styles[j] & XIMLookupCallbacks) &&
		    (supported_styles[j] & XIMAuxCallbacks)) {
		    pre_edit_list = panel_make_pre_edit_cb_list(panel_public);
		    status_list = panel_make_status_cb_list(panel_public);
		    lookup_list = panel_make_lookup_cb_list(panel_public);
		    aux_list = panel_make_aux_cb_list(panel_public);
		    ic = (XIC)XCreateIC(im, 
			XNClientWindow,	xv_xid(info),
			XNInputStyle, supported_styles[j],            		
			XNPreeditAttributes,	pre_edit_list,
			XNStatusAttributes,	status_list,
			XNExtXimp_LookupAttributes,	lookup_list,
			XNExtXimp_AuxAttributes,	aux_list,
			XNExtXimp_Backfront, IMServBackend,
			NULL);
		 } else if ((supported_styles[j] & XIMPreeditCallbacks) &&
		    (supported_styles[j] & XIMStatusCallbacks) &&
		    (supported_styles[j] & XIMLookupCallbacks)) {
		    pre_edit_list = panel_make_pre_edit_cb_list(panel_public);
		    status_list = panel_make_status_cb_list(panel_public);
		    lookup_list = panel_make_lookup_cb_list(panel_public);
		    
		    ic = (XIC)XCreateIC(im, 
			XNClientWindow,	xv_xid(info),
			XNInputStyle, supported_styles[j],            		
			XNPreeditAttributes,	pre_edit_list,
			XNStatusAttributes,	status_list,
			XNExtXimp_LookupAttributes,	lookup_list,
			XNExtXimp_Backfront, IMServBackend,
			NULL);
		 } else if ((supported_styles[j] & XIMPreeditCallbacks) &&
		    (supported_styles[j] & XIMStatusCallbacks) &&
		    (supported_styles[j] & XIMAuxCallbacks)) {
		    pre_edit_list = panel_make_pre_edit_cb_list(panel_public);
		    status_list = panel_make_status_cb_list(panel_public);
		    aux_list = panel_make_aux_cb_list(panel_public);
		    
		    ic = (XIC)XCreateIC(im, 
			XNClientWindow,	xv_xid(info),
			XNInputStyle, supported_styles[j],            		
			XNPreeditAttributes,	pre_edit_list,
			XNStatusAttributes,	status_list,
			XNExtXimp_AuxAttributes,	aux_list,
			XNExtXimp_Backfront, IMServBackend,
			NULL);		
		 } else if ((supported_styles[j] & XIMPreeditCallbacks) &&
		    (supported_styles[j] & XIMStatusCallbacks)) {
		    pre_edit_list = panel_make_pre_edit_cb_list(panel_public);
		    status_list = panel_make_status_cb_list(panel_public);
		    
		    ic = (XIC)XCreateIC(im, 
			XNClientWindow,	xv_xid(info),
			XNInputStyle, supported_styles[j],            		
			XNPreeditAttributes,	pre_edit_list,
			XNStatusAttributes,	status_list,
			XNExtXimp_Backfront, IMServBackend,
			NULL);           	    
		}  else if (supported_styles[j] & XIMPreeditCallbacks) {
			pre_edit_list = panel_make_pre_edit_cb_list(panel_public);
			
			ic = (XIC)XCreateIC(im, 
			XNClientWindow,	xv_xid(info),
			XNInputStyle, supported_styles[j],            		
			XNPreeditAttributes,	pre_edit_list,
			XNExtXimp_Backfront, IMServBackend,
			NULL);
		}  else if ((supported_styles[j] & XIMPreeditNothing) &&
				  (supported_styles[j] & XIMStatusCallbacks)) {
		    status_list = panel_make_status_cb_list(panel_public);
			
			ic = (XIC)XCreateIC(im, 
			XNClientWindow,	xv_xid(info),
			XNInputStyle, supported_styles[j],            		
			XNStatusAttributes,	status_list,
			XNExtXimp_Backfront, IMServBackend,
			NULL);
				  
		}  else if ((supported_styles[j] & XIMPreeditNothing) &&
				  (supported_styles[j] & XIMStatusNothing)) {
		     ic = (XIC)XCreateIC(im, 
			XNClientWindow,	xv_xid(info),
			XNInputStyle, (XIMPreeditNothing | XIMStatusNothing),    
			XNExtXimp_Backfront, IMServBackend,
			NULL);
		}  	        
		  
		if (pre_edit_list)		
		    XFree(pre_edit_list);
		if (status_list)		
		    XFree(status_list);
		if (lookup_list)		
		    XFree(lookup_list);
		if (aux_list)		
		    XFree(aux_list);
			
		if(ic) {
		    /* set ic to be part of window pkg */
		    xv_set(panel_public, WIN_IC, ic, 0);
		    /* Store the ic in panel info   */
		    panel->ic = ic;
		    if (!panel->interm_text)
	    	        panel->interm_text = (CHAR *) MALLOC(ITERM_BUFSIZE);
	    	    panel->interm_text[0] = NULL;
	    	    if (!panel->interm_attr)
		        panel->interm_attr =  (XIMFeedback *) malloc(ITERM_BUFSIZE* sizeof (XIMFeedback));

		 }
		}
	    }
	    break;

	  default:
	    if (attr)
		xv_check_bad_attr(&xv_panel_pkg, attr);
	    break;
	}

	if (avlist[0] == XV_USE_DB)
	    avlist++;
	else if (db_count && !avlist[0]) {
	    db_count--;
	    avlist++;
	} else
	    avlist = attr_next(avlist);

	attr = avlist[0];
    }
#else OW_I18N

    for (attr = avlist[0]; attr;
	 avlist = attr_next(avlist), attr = avlist[0]) {
	switch (attr) {

	  case PANEL_CLIENT_DATA:
	    panel->client_data = avlist[1];
	    break;

	  case PANEL_BOLD_FONT:
	    /* Sunview1 compatibility attribute: not used */
	    break;

	  case PANEL_BLINK_CARET:
	    if ((int) avlist[1])
		panel->status |= BLINKING;
	    else
		panel->status &= ~BLINKING;
	    if (panel->kbd_focus_item->item_type == PANEL_TEXT_ITEM)
		panel_text_caret_on(panel, TRUE);
	    break;

	  case PANEL_CARET_ITEM:
	    panel_yield_kbd_focus(panel);
	    panel->kbd_focus_item = ITEM_PRIVATE(avlist[1]);
	    panel_accept_kbd_focus(panel);
	    break;

	  case PANEL_EVENT_PROC:
	    panel->event_proc = (int (*) ()) avlist[1];
	    break;

	  case PANEL_REPAINT_PROC:
	    panel->repaint_proc = (int (*) ()) avlist[1];
	    break;

	  case PANEL_BACKGROUND_PROC:
	    if (!ops_set(panel)) {
		new_ops = xv_alloc(Panel_ops);
		*new_ops = *panel->ops;
		panel->ops = new_ops;
		panel->flags |= OPS_SET;
	    }
	    panel->ops->handle_event = (void (*) ()) avlist[1];
	    if (!panel->ops->handle_event)
		panel->ops->handle_event = (void (*) ()) panel_nullproc;
	    break;

	  case PANEL_ITEM_X_GAP:
	    panel->item_x_offset = (int) avlist[1];
	    if (panel->item_x_offset < 1)
		panel->item_x_offset = 1;
	    break;

	  case PANEL_ITEM_Y_GAP:
	    panel->item_y_offset = (int) avlist[1]; 
	    if (panel->item_y_offset < 1)
		panel->item_y_offset = 1;
	    break;

	  case PANEL_EXTRA_PAINT_WIDTH:
	    extra_width = (int) avlist[1];
	    break;

	  case PANEL_EXTRA_PAINT_HEIGHT:
	    extra_height = (int) avlist[1];
	    break;

	  case PANEL_LABEL_INVERTED:
	    if (avlist[1])
		panel->flags |= LABEL_INVERTED;
	    else
		panel->flags &= ~LABEL_INVERTED;
	    break;

	  case PANEL_LAYOUT:
	    switch ((Panel_setting) avlist[1]) {
	      case PANEL_HORIZONTAL:
	      case PANEL_VERTICAL:
		panel->layout = (Panel_setting) avlist[1];
		break;
	      default:		/* invalid layout */
		break;
	    }
	    break;

	  case PANEL_PAINT:
	    panel->repaint = (Panel_setting) avlist[1];
	    break;

	  case WIN_VERTICAL_SCROLLBAR:
	  case OPENWIN_VERTICAL_SCROLLBAR:
	    new_v_scrollbar = (Scrollbar) avlist[1];
	    break;

	  case WIN_HORIZONTAL_SCROLLBAR:
	  case OPENWIN_HORIZONTAL_SCROLLBAR:
	    new_h_scrollbar = (Scrollbar) avlist[1];
	    break;

	  case PANEL_ACCEPT_KEYSTROKE:
	    if (avlist[1])
		panel->flags |= WANTS_KEY;
	    else
		panel->flags &= ~WANTS_KEY;
	    break;

	  case PANEL_DEFAULT_ITEM:
	    panel->default_item = (Panel_item) avlist[1];
	    break;

	  case WIN_REMOVE_CARET:
	    if (panel->kbd_focus_item &&
		panel->kbd_focus_item->item_type == PANEL_TEXT_ITEM) {
		/* Clear caret */
		panel_text_caret_on(panel, FALSE);
	    }
	    panel->caret = NULL;
	    break;

#ifdef VERSION_3
	  case WIN_FOREGROUND_COLOR:
	  case WIN_BACKGROUND_COLOR:
	    if (panel->three_d) {
		char error_string[64];

		sprintf(error_string, "%s not valid on a 3D Panel",
		    attr == WIN_FOREGROUND_COLOR ? "WIN_FOREGROUND_COLOR" :
		    "WIN_BACKGROUND_COLOR");
		xv_error(panel_public,
			 ERROR_STRING, error_string,
			 0);
		ATTR_CONSUME(avlist[0]);
	    }
	    break;
#endif /* VERSION_3 */

	  case WIN_SET_FOCUS:{
		Xv_Window       pw;

		ATTR_CONSUME(avlist[0]);

		if (!wants_key(panel) && !panel->kbd_focus_item)
		    return ((Xv_opaque) XV_ERROR);

		/*
		 * Find the first paint window that can accept kbd input and
		 * set the input focus to it.  Only do this if we have a
		 * caret/text item or the panel wants the key.  Since panels
		 * always assigns their own input focus, there is no need to
		 * check xv_no_focus().
		 */
		PANEL_EACH_PAINT_WINDOW(panel, pw)
		    DRAWABLE_INFO_MACRO(pw, info);
		    if (wants_key(panel) || 
			win_getinputcodebit(
			    (Inputmask *) xv_get(pw, WIN_INPUT_MASK),
			    KBD_USE)) {
			win_set_kbd_focus(panel_public, xv_xid(info));
			return (XV_OK);
		    }
		PANEL_END_EACH_PAINT_WINDOW
		return ((Xv_opaque) XV_ERROR);
	    }

	  case XV_FONT:
	    panel->std_font = (Pixfont *) avlist[1];	    	
	    panel_set_fonts(panel_public, panel);
	    break;

	  case XV_END_CREATE:
	    /* Set up the fonts */
	    panel->std_font = (Pixfont *) xv_get(panel_public, XV_FONT);
	    panel_set_fonts(panel_public, panel);

	    /* Set up the Colormap Segment and OLGX */
	    panel->ginfo = xv_init_olgx(panel_public, &panel->three_d);

	    if (!panel->paint_window) {
		/* PANEL instead of SCROLLABLE_PANEL:
		 *   set up paint_window structure, create Openwin GC list.
		 */
		panel_register_view(panel, XV_NULL);
		/* Create Openwin GC's, based on this Colormap Segment */
		openwin_create_gcs(panel_public);
	    } else {
		Pixmap bg_pixmap = (Pixmap) xv_get(panel_public,
		    WIN_BACKGROUND_PIXMAP);
		if (bg_pixmap)
		    xv_set(panel->paint_window->pw,
			   WIN_BACKGROUND_PIXMAP, bg_pixmap,
			   0);
	    }

	    xv_set(panel_public,
		   WIN_ROW_HEIGHT, panel->ginfo->button_height,
		   0);

	    break;

	  default:
	    xv_check_bad_attr(&xv_panel_pkg, attr);
	    break;
	}
    }
#endif OW_I18N

    /* set up any scrollbars */
    if (new_v_scrollbar != NULL &&
    (int (*) ()) xv_get(new_v_scrollbar, SCROLLBAR_NORMALIZE_PROC) == NULL) {
	xv_set(new_v_scrollbar,
	       SCROLLBAR_NORMALIZE_PROC, panel_normalize_scroll,
	       0);
    }
    if (new_h_scrollbar != NULL &&
    (int (*) ()) xv_get(new_h_scrollbar, SCROLLBAR_NORMALIZE_PROC) == NULL) {
	xv_set(new_h_scrollbar,
	       SCROLLBAR_NORMALIZE_PROC, panel_normalize_scroll,
	       0);
    }
    if (extra_width >= 0 && extra_width != panel->extra_width) {
	panel->extra_width = extra_width;
    }
    if (extra_height >= 0 && extra_height != panel->extra_height) {
	panel->extra_height = extra_height;
    }
    if (panel->extra_width == extra_width || panel->extra_height == extra_height) {
	panel_update_scrolling_size(panel_public);
    }
    return XV_OK;
}


static void
panel_set_fonts(panel_public, panel)
    Panel           panel_public;
    register Panel_info *panel;
{
    XCharStruct	    active_caret_info;
    XFontStruct	   *font_info;
    int		    font_size;
    XCharStruct	    inactive_caret_info;
    Font	    glyph_font;

#ifdef OW_I18N
    panel->std_font_xid = (Font) xv_get(panel->std_font, FONT_SET_ID);
#else
    panel->std_font_xid = (Font) xv_get(panel->std_font, XV_XID);
#endif /*OW_I18N*/

    font_size = (int) xv_get(panel->std_font, FONT_SIZE);
#ifdef OW_I18N
    if (font_size == FONT_NO_SIZE) {
        font_size = (int) xv_get(panel->std_font, FONT_DEFAULT_CHAR_HEIGHT);

    if (font_size <= 10)
   	font_size = 10;
    else if (font_size <= 12)
    	font_size = 12;
    else if (font_size <= 14)
    	font_size = 14;
    else
    	font_size = 19;
    }
#else
    if (font_size == FONT_NO_SIZE) {
	font_size = (int) xv_get(panel->std_font, FONT_DEFAULT_CHAR_HEIGHT);
	if (font_size < 10)
	    font_size = 10;
	else if (font_size == 11)
	    font_size = 12;
	else if (font_size == 13)
	    font_size = 14;
	else if (font_size > 14)
	    font_size = 19;
    }
#endif OW_I18N

    panel->bold_font = (Pixfont *) xv_find(panel_public, FONT,
        FONT_FAMILY, xv_get(panel->std_font, FONT_FAMILY),
        FONT_STYLE, FONT_STYLE_BOLD,
        FONT_SIZE, font_size,
        0);

#ifdef  OW_I18N
    if (panel->bold_font == NULL) 
	panel->bold_font = panel->std_font;
#endif  OW_I18N

    if (panel->bold_font == NULL) {
        xv_error(NULL,
                 ERROR_STRING,
#ifdef	OW_I18N
		     XV_I18N_MSG("xv_messages",
			      "Unable to find bold font; using standard font"),
#else
		     "Unable to find bold font; using standard font",
#endif	OW_I18N
                 ERROR_PKG, PANEL,
                 0);
        panel->bold_font = panel->std_font;
    }  

#ifdef OW_I18N
    panel->bold_font_xid = (Font) xv_get(panel->bold_font, FONT_SET_ID);
#else
    panel->bold_font_xid = (Font) xv_get(panel->bold_font, XV_XID);
#endif /*OW_I18N*/

    glyph_font = xv_find(panel_public, FONT,
			 FONT_FAMILY, FONT_FAMILY_OLGLYPH,
			 FONT_SIZE, font_size,
			 FONT_TYPE, FONT_TYPE_GLYPH,
			 0);
    if (!glyph_font)
	xv_error(NULL,
#ifdef	OW_I18N
		 ERROR_STRING, XV_I18N_MSG("xv_messages",
					"Unable to find OPEN LOOK glyph font"),
#else
		 ERROR_STRING, "Unable to find OPEN LOOK glyph font",
#endif	OW_I18N
		 ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
		 ERROR_PKG, PANEL,
		 0);
    xv_set(panel_public, WIN_GLYPH_FONT, glyph_font, 0);

    font_info = (XFontStruct *) xv_get(glyph_font, FONT_INFO);
    if (font_info->per_char) {
	active_caret_info = font_info->per_char[OLGX_ACTIVE_CARET];
	inactive_caret_info = font_info->per_char[OLGX_INACTIVE_CARET];
    } else {
	active_caret_info = font_info->min_bounds;
	inactive_caret_info = font_info->min_bounds;
    }
    panel->active_caret_ascent = active_caret_info.ascent;
    panel->active_caret_height = active_caret_info.ascent +
	active_caret_info.descent;
    panel->active_caret_width = active_caret_info.width;
    panel->inactive_caret_ascent = inactive_caret_info.ascent;
    panel->inactive_caret_height = inactive_caret_info.ascent +
	inactive_caret_info.descent;
    panel->inactive_caret_width = inactive_caret_info.width;
}


Pkg_private void
panel_refont(panel, arg)
    register Panel_info *panel;
    int             arg;
{
    register Panel_item item;
    register Panel  panel_public = PANEL_PUBLIC(panel);
    register Item_info *ip;
    register Panel_image *label;
    Pixfont        *panel_font, *old_win_font, *old_bold_font, *new_win_font,
                   *new_bold_font;
    int             label_bold, item_x, item_y, row_gap,
                    col_gap, left_margin, top_margin, item_row, item_col;

    old_win_font = (Pixfont *) xv_get(panel_public, XV_FONT);
    new_win_font = (old_win_font) ? (Pixfont *) xv_find(
							panel_public, FONT,
				   FONT_RESCALE_OF, old_win_font, (int) arg,
							0)
	: (Pixfont *) 0;
    if (new_win_font) {
	(void) xv_set(old_win_font, XV_INCREMENT_REF_COUNT, 0);
	(void) xv_set(panel_public, XV_FONT, new_win_font, 0);
	panel_font = new_win_font;
    } else
	panel_font = old_win_font;

    old_bold_font = panel->bold_font;
    new_bold_font = (old_bold_font) ? (Pixfont *) xv_find(
							  panel_public, FONT,
				  FONT_RESCALE_OF, old_bold_font, (int) arg,
							  0)
	: (Pixfont *) 0;
    if (new_bold_font) {
	(void) xv_set(panel_public, PANEL_BOLD_FONT, new_bold_font, 0);
    }

    if ((!new_win_font) && (!new_bold_font))
	return;

    row_gap = (int) xv_get(panel_public, WIN_ROW_GAP);
    col_gap = (int) xv_get(panel_public, WIN_COLUMN_GAP);
    left_margin = (int) xv_get(panel_public, WIN_LEFT_MARGIN);
    top_margin = (int) xv_get(panel_public, WIN_TOP_MARGIN);

    PANEL_EACH_ITEM(panel_public, item)
	ip = ITEM_PRIVATE(item);
    if (new_win_font) {
	item_x = (int) xv_get(ITEM_PUBLIC(ip), PANEL_ITEM_X);
	item_y = (int) xv_get(ITEM_PUBLIC(ip), PANEL_ITEM_Y);
	item_col = column_from_absolute_x(item_x, col_gap, top_margin,
					  new_win_font);
	item_row = row_from_absolute_y(item_y, row_gap, left_margin,
				       new_win_font);
	(void) xv_set(ITEM_PUBLIC(ip),
		      PANEL_ITEM_X, xv_col(panel_public, item_col),
		      PANEL_ITEM_Y, xv_row(panel_public, item_row),
		      PANEL_PAINT, PANEL_NONE,
		      0);
    }
    label = &ip->label;
    if (is_string(label)) {

	label_bold = (int) xv_get(
				  ITEM_PUBLIC(ip), PANEL_LABEL_BOLD);

#ifdef OW_I18N
	xv_set(ITEM_PUBLIC(ip),
	       PANEL_PAINT, PANEL_NONE,
	       PANEL_LABEL_FONT, panel_font,
	       PANEL_LABEL_STRING_WCS, image_string_wc(label),
	       0);
#else
	xv_set(ITEM_PUBLIC(ip),
	       PANEL_PAINT, PANEL_NONE,
	       PANEL_LABEL_FONT, panel_font,
	       PANEL_LABEL_STRING, image_string(label),
	       0);

/*  The following statement is in here because we are not using
 *  bold font for now, when bold Japanese font is available
 *  move it out of the the #ifdef statement
 */

	if (label_bold) {
	    xv_set(ITEM_PUBLIC(ip),
		   PANEL_PAINT, PANEL_NONE,
		   PANEL_LABEL_BOLD, label_bold,
		   0);
	}

#endif OW_I18N

    }
    xv_set(ITEM_PUBLIC(ip),
	   PANEL_PAINT, PANEL_NONE,
	   PANEL_LABEL_FONT, panel_font,
	   0);
    switch (ip->item_type) {

      case PANEL_MESSAGE_ITEM:
	break;

#ifdef OW_I18N
      case PANEL_BUTTON_ITEM:{
	    wchar_t           *label = (wchar_t *) xv_get(
				       ITEM_PUBLIC(ip), PANEL_LABEL_STRING_WCS);
	    if (label)		/* don't scale image buttons */
		xv_set(ITEM_PUBLIC(ip),
		       PANEL_PAINT, PANEL_NONE,
		       PANEL_LABEL_STRING_WCS, label,
		       0);
	    break;
	}
#else
      case PANEL_BUTTON_ITEM:{
	    char           *label = (char *) xv_get(
				       ITEM_PUBLIC(ip), PANEL_LABEL_STRING);
	    if (label)		/* don't scale image buttons */
		xv_set(ITEM_PUBLIC(ip),
		       PANEL_PAINT, PANEL_NONE,
		       PANEL_LABEL_STRING, label,
		       0);
	    break;
	}
#endif OW_I18N

      case PANEL_TOGGLE_ITEM:
	xv_set(ITEM_PUBLIC(ip),
	       PANEL_PAINT, PANEL_NONE,
	       PANEL_CHOICE_FONTS, panel_font, 0,
	       0);
	break;

      case PANEL_CHOICE_ITEM:
	xv_set(ITEM_PUBLIC(ip),
	       PANEL_PAINT, PANEL_NONE,
	       PANEL_CHOICE_FONTS, panel_font, 0,
	       0);
	break;

      case PANEL_TEXT_ITEM:
      case PANEL_SLIDER_ITEM:
	xv_set(ITEM_PUBLIC(ip),
	       PANEL_PAINT, PANEL_NONE,
	       PANEL_VALUE_FONT, panel_font,
	       0);
	break;


      default:
	break;
    }
    /*
     * undecided if we should paint it.  Damage will do it for free when it
     * is resized.
     */
    panel_paint(ITEM_PUBLIC(ip), PANEL_CLEAR);
    PANEL_END_EACH
	if (new_win_font) {
	(void) xv_set(panel_public, XV_FONT, old_win_font, 0);
	(void) xv_set(old_win_font, XV_DECREMENT_REF_COUNT, 0);
    }
}


static int
column_from_absolute_x(x_position, col_gap, left_margin, font)
    int             x_position, col_gap, left_margin;
    Pixfont        *font;
{
    int		chrwth;

    x_position -= left_margin;
    chrwth = xv_get(font, FONT_DEFAULT_CHAR_WIDTH);
    return (x_position / (chrwth + col_gap));
}


static int
row_from_absolute_y(y_position, row_gap, top_margin, font)
    int             y_position, row_gap, top_margin;
    Pixfont        *font;
{
    int		chrht;

    y_position -= top_margin;
    chrht = xv_get(font, FONT_DEFAULT_CHAR_HEIGHT);
    return (y_position / (chrht + row_gap));
}
