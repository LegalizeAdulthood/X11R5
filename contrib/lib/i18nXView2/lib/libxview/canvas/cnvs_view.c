#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)cnvs_view.c 70.8 91/08/28";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/cnvs_impl.h>
#include <xview_private/win_keymap.h>
#include <xview/scrollbar.h>
#include <xview/rect.h>
#include <xview/rectlist.h>

#ifdef  OW_I18N
#include <xview_private/draw_impl.h>
#include <xview/font.h>
#include <xview/frame.h>
#include <X11/Xlib.h>
#include <X11/XSunExt.h>
Xv_opaque canvas_create_mle();
#endif  OW_I18N

static          canvas_view_create_paint_window();

/* ARGSUSED */
Pkg_private int
canvas_view_init(parent, view_public, avlist)
    Canvas          parent;
    Canvas_view     view_public;
    Attr_attribute  avlist[];
{
    Xv_canvas_view *view_object = (Xv_canvas_view *) view_public;
    Canvas_view_info *view;
    int             ret;

    view = xv_alloc(Canvas_view_info);

    /* link to object */
    view_object->private_data = (Xv_opaque) view;
    view->public_self = view_public;
    view->private_canvas = CANVAS_PRIVATE(parent);

#ifdef OW_I18N
    if ((ret = canvas_view_create_paint_window(view, avlist)) != XV_OK) {
#else
    if ((ret = canvas_view_create_paint_window(view)) != XV_OK) {
#endif OW_I18N
	xv_free(view);
	return (ret);
    }
    xv_set(view_public,
	   WIN_NOTIFY_SAFE_EVENT_PROC, canvas_view_event,
	   WIN_NOTIFY_IMMEDIATE_EVENT_PROC, canvas_view_event,
	   WIN_CONSUME_PICK_EVENTS,
	   WIN_RESIZE, 0,
	   0);

    return XV_OK;
}

/*ARGSUSED*/ /*VARARGS*/
Pkg_private Xv_opaque
canvas_view_get(view_public, stat, attr, valist)
    Canvas_view     view_public;
    int            *stat;
    Attr_attribute  attr;
    va_list         valist;
{
    Canvas_view_info *view = CANVAS_VIEW_PRIVATE(view_public);

    *stat = XV_OK;

    switch (attr) {
      case CANVAS_VIEW_PAINT_WINDOW:
	return ((Xv_opaque) view->paint_window);

      case CANVAS_VIEW_CANVAS_WINDOW:
	return ((Xv_opaque) CANVAS_PUBLIC(view->private_canvas));

      default:
	xv_check_bad_attr(&xv_canvas_view_pkg, attr);
	*stat = XV_ERROR;
	return (Xv_opaque) 0;
    }
}


/*ARGSUSED*/ /*VARARGS*/
Pkg_private Xv_opaque
canvas_paint_get(paint_public, stat, attr, valist)
    Canvas_paint_window paint_public;
    int            *stat;
    Attr_attribute  attr;
    va_list         valist;
{
    Canvas_view_info *view;
    Canvas_info    *canvas;

    switch (attr) {
      case CANVAS_PAINT_CANVAS_WINDOW:
	canvas = (Canvas_info *) xv_get(paint_public,
					XV_KEY_DATA, canvas_context_key);
	return (Xv_opaque) CANVAS_PUBLIC(canvas);

      case CANVAS_PAINT_VIEW_WINDOW:
	view = (Canvas_view_info *) xv_get(paint_public,
					   XV_KEY_DATA, canvas_view_context_key);
	return (Xv_opaque) CANVAS_VIEW_PUBLIC(view);

      default:
	xv_check_bad_attr(&xv_canvas_pw_pkg, attr);
	*stat = XV_ERROR;
	return (Xv_opaque) 0;
    }
}

#ifdef OW_I18N
static XVaNestedList
canvas_make_pre_edit_cb_list(canvas, paint_public)
    Canvas_info         *canvas;
    Canvas_paint_window paint_public;
{
    Canvas            canvas_public = CANVAS_PUBLIC(canvas);
    int		dummy;
    extern void canvas_text_start();
    extern void canvas_text_draw();
    extern void canvas_text_done();

	if (!canvas->start_pecb_struct.callback) {
	    canvas->start_pecb_struct.callback = (XIMProc)canvas_text_start;
	    canvas->start_pecb_struct.client_data = (XPointer)canvas_public;
	}
	if (!canvas->draw_pecb_struct.callback) {
	    canvas->draw_pecb_struct.callback = (XIMProc)canvas_text_draw;
	    canvas->draw_pecb_struct.client_data = (XPointer)canvas_public;
	}
	if (!canvas->done_pecb_struct.callback) {
	    canvas->done_pecb_struct.callback = (XIMProc)canvas_text_done;
	    canvas->done_pecb_struct.client_data = (XPointer)canvas_public;
	}
	    /* Should do caret too */

        return(XVaCreateNestedList(dummy,
			XNPreeditStartCallback, &canvas->start_pecb_struct,
			XNPreeditDrawCallback, &canvas->draw_pecb_struct,
			XNPreeditDoneCallback, &canvas->done_pecb_struct,
			NULL));
}

static XVaNestedList
canvas_make_status_cb_list(canvas, paint_public, input_frame)
    Canvas_info		*canvas;
    Canvas_paint_window paint_public;
    Frame       	input_frame;
{
    Canvas              canvas_public = CANVAS_PUBLIC(canvas);
    Frame		frame_public = (Frame) xv_get(canvas_public, WIN_FRAME);
    int			dummy;
    Xv_private	void	canvas_status_draw();
    Xv_private	void	status_done();
    Xv_private	void	status_start();
    
    if (!canvas->start_stcb_struct.callback) {
	canvas->start_stcb_struct.callback = (XIMProc)status_start;
	canvas->start_stcb_struct.client_data = (XPointer)frame_public;
    }
    /*  Registering our own status_draw
     *  routine, because we need to check
     *  whether or not frame cmd is showing
     *  first
     */



    if (!canvas->draw_stcb_struct.callback) {
	canvas->draw_stcb_struct.callback = (XIMProc)canvas_status_draw;
	canvas->draw_stcb_struct.client_data = (XPointer)input_frame;
    }
    if (!canvas->done_stcb_struct.callback) {
	canvas->done_stcb_struct.callback = (XIMProc)status_done;
	canvas->done_stcb_struct.client_data = (XPointer)frame_public;
    }
    return(XVaCreateNestedList(dummy,
			XNStatusStartCallback, &canvas->start_stcb_struct,
			XNStatusDrawCallback, &canvas->draw_stcb_struct,
			XNStatusDoneCallback, &canvas->done_stcb_struct,
			NULL));
}

static XVaNestedList
canvas_make_lookup_cb_list(canvas, paint_public)
    Canvas_info		*canvas;
    Canvas_paint_window paint_public;
{
    Canvas              canvas_public = CANVAS_PUBLIC(canvas);
    int			dummy;
    extern Window   	lookup_choices_start();
    extern void	    	lookup_choices_draw(),lookup_choices_done();
    extern int	    	lookup_choices_process();
    XPointer	luc_clientdata = (XPointer)malloc( sizeof(int *));


	if (!canvas->start_luc_struct.callback) {
	    canvas->start_luc_struct.callback =
		    (XIMProc)lookup_choices_start;
	    canvas->start_luc_struct.client_data =
		    (XPointer)luc_clientdata;
	}
	if (!canvas->draw_luc_struct.callback) {
	    canvas->draw_luc_struct.callback = (XIMProc)lookup_choices_draw;
	    canvas->draw_luc_struct.client_data = (XPointer)luc_clientdata;
	}
	if (!canvas->process_luc_struct.callback) {
	    canvas->process_luc_struct.callback =
		    (XIMProc)lookup_choices_process;
	    canvas->process_luc_struct.client_data =
		    (XPointer)luc_clientdata;
	}
	if (!canvas->done_luc_struct.callback) {
	    canvas->done_luc_struct.callback = (XIMProc)lookup_choices_done;
	    canvas->done_luc_struct.client_data = (XPointer)luc_clientdata;
	}
        return(XVaCreateNestedList(dummy,
			XNExtXimp_LookupStartCallback, &canvas->start_luc_struct,
			XNExtXimp_LookupDrawCallback,  &canvas->draw_luc_struct,
			XNExtXimp_LookupProcessCallback, &canvas->process_luc_struct,
			XNExtXimp_LookupDoneCallback,  &canvas->done_luc_struct,			NULL));

}

static XVaNestedList
canvas_make_aux_cb_list(canvas, paint_public)
    Canvas_info		*canvas;
    Canvas_paint_window paint_public;
{
    Canvas              canvas_public = CANVAS_PUBLIC(canvas);
    XPointer	aux_clientdata = (XPointer)malloc( sizeof(int *));
    int		dummy;
    extern void		aux_start(),aux_draw(),aux_done();
    
	 if (!canvas->start_aux_struct.callback) {
	    canvas->start_aux_struct.callback = (XIMProc)aux_start;
	    canvas->start_aux_struct.client_data = (XPointer)aux_clientdata;
	}
	if (!canvas->draw_aux_struct.callback) {
	    canvas->draw_aux_struct.callback = (XIMProc)aux_draw;
	    canvas->draw_aux_struct.client_data = (XPointer)aux_clientdata;
	}
	if (!canvas->done_aux_struct.callback) {
	    canvas->done_aux_struct.callback = (XIMProc)aux_done;
	    canvas->done_aux_struct.client_data = (XPointer)aux_clientdata;
	}

        return(XVaCreateNestedList(dummy,
			XNExtXimp_AuxStartCallback, &canvas->start_aux_struct,
			XNExtXimp_AuxDrawCallback, &canvas->draw_aux_struct,
			XNExtXimp_AuxDoneCallback, &canvas->done_aux_struct,
			NULL));

}

#endif /* OW_I18N */

/*ARGSUSED*/ /*VARARGS*/
Pkg_private Xv_opaque
canvas_paint_set(paint_public, avlist)
    Canvas_paint_window paint_public;
    Attr_avlist		avlist;
{
    Attr_attribute	attr;
    Xv_opaque           result = XV_OK;

#ifdef OW_I18N
    Frame               frame_public;
    Xv_pkg              *frame_type;
    Canvas_info 	*canvas;
    Canvas		canvas_public;
    int			ascii_events;
    Xv_opaque		use_im;
    Attr_avlist		attrs = avlist;

    canvas = (Canvas_info *) xv_get(paint_public,
			XV_KEY_DATA, canvas_context_key);
    if (!canvas) {
	for (; attrs[0];
	    attrs = attr_next(attrs)) {
	    switch (attrs[0]) {
		case XV_KEY_DATA:
		    if (attrs[1] == canvas_context_key) {
			canvas = (Canvas_info *) attrs[2];
			canvas_public = CANVAS_PUBLIC(canvas);
		    }
		    break;

		default:
		    break;
	    }
	}
	attrs = avlist;
    }
#else
    Canvas_info    	*canvas;
#endif OW_I18N

    for (attr = avlist[0]; attr;
         avlist = attr_next(avlist), attr = avlist[0]) {
        switch (attr) {
	    case WIN_CMS_CHANGE: 
#ifndef OW_I18N
		canvas = (Canvas_info *) xv_get(paint_public,
                                            XV_KEY_DATA, canvas_context_key);
#endif ~OW_I18N
                if (status(canvas, cms_repaint)) {
                    Rect                rect;
                    Rectlist    rl;
                    canvas = (Canvas_info *) xv_get(paint_public,
                                                XV_KEY_DATA, canvas_context_key);
                    rect.r_left = 0;
                    rect.r_top = 0;
                    rect.r_width = (short)xv_get(paint_public, WIN_WIDTH);
                    rect.r_height = (short)xv_get(paint_public, WIN_HEIGHT);
                    rl = rl_null;
                    rl_rectunion(&rect, &rl, &rl);

                    win_set_damage(paint_public, &rl);
                    canvas_inform_repaint(canvas, paint_public);
                    win_clear_damage(paint_public);
                }
                break;

	    default:
		xv_check_bad_attr(&xv_canvas_pw_pkg, attr);
		break;
	}
    }

#ifdef OW_I18N
    use_im = (Xv_opaque)xv_get(paint_public, WIN_USE_IM);
/*
    if (!ascii_events) {
	ascii_events = xv_get(paint_public, WIN_CONSUME_EVENT,
			WIN_ASCII_EVENTS);
    }
    if (use_im && ascii_events && !canvas->ic) {
*/

    if (canvas && use_im && !canvas->ic) {
        canvas_public = CANVAS_PUBLIC(canvas);
	frame_public = (Frame)xv_get(canvas_public, WIN_FRAME);
	frame_type = (Xv_pkg*)xv_get(frame_public, XV_TYPE);
		
	if (strcmp(frame_type->name, "Frame_base") == 0) {
	    Xv_opaque           input_window;
	    XIM                 im;
	    XIC                 ic;
	    Xv_object           serverobj;

	    Attr_avlist     defaults;
	    extern void     status_start(), canvas_status_draw(), status_done();
	    extern Window   lookup_choices_start();
	    extern void     lookup_choices_draw(),lookup_choices_done();
	    extern int      lookup_choices_process();
	    extern void     aux_start(), aux_draw(), aux_done();
	    int		    ic_refcnt = 0;


	    for (; attrs[0];
		 attrs = attr_next(attrs)) {
		switch (attrs[0]) {
    /*
		    case WIN_IGNORE_EVENT:
			if (attrs[1] == WIN_ASCII_EVENTS) {
			    ascii_events = FALSE;
			}
			break;

		    case WIN_IGNORE_EVENTS: {
			int i;
			for (i=1; attrs[i] != NULL; i++) {
			    if (attrs[i] == WIN_ASCII_EVENTS) {
				ascii_events = FALSE;
			    }
			}
			break;
		    }

		    case WIN_CONSUME_EVENT:
			if (attrs[1] == WIN_ASCII_EVENTS) {
			    ascii_events = TRUE;
			}
			break;

		    case WIN_CONSUME_EVENTS: {
			int i;
			for (i=1; attrs[i] != NULL; i++) {
			    if (defaults[i] == WIN_ASCII_EVENTS) {
				ascii_events = TRUE;
			    }
			}
			break;
		    }
    */

		    case WIN_IM_PREEDIT_START:
			canvas->start_pecb_struct.callback = (XIMProc)attrs[1];
			canvas->start_pecb_struct.client_data = (XPointer)attrs[2];
			break;

		    case WIN_IM_PREEDIT_DRAW:
			canvas->draw_pecb_struct.callback = (XIMProc)attrs[1];
			canvas->draw_pecb_struct.client_data = (XPointer)attrs[2];
			break;

		    case WIN_IM_PREEDIT_DONE:
			canvas->done_pecb_struct.callback = (XIMProc)attrs[1];
			canvas->done_pecb_struct.client_data = (XPointer)attrs[2];
			break;

		    case WIN_IM_STATUS_START:
			canvas->start_stcb_struct.callback = (XIMProc)attrs[1];
			canvas->start_stcb_struct.client_data = (XPointer)attrs[2];
			break;

		    case WIN_IM_STATUS_DRAW:
			canvas->draw_stcb_struct.callback = (XIMProc)attrs[1];
			canvas->draw_stcb_struct.client_data = (XPointer)attrs[2];
			break;
	 
		    case WIN_IM_STATUS_DONE:
			canvas->done_stcb_struct.callback = (XIMProc)attrs[1];
			canvas->done_stcb_struct.client_data = (XPointer)attrs[2];
			break;

		    case WIN_IM_LUC_START:
			canvas->start_luc_struct.callback = (XIMProc)attrs[1];
			canvas->start_luc_struct.client_data = (XPointer)attrs[2];
			break;

		    case WIN_IM_LUC_DRAW:
			canvas->draw_luc_struct.callback = (XIMProc)attrs[1];
			canvas->draw_luc_struct.client_data = (XPointer)attrs[2];
			break;

		    case WIN_IM_LUC_DONE:
			canvas->done_luc_struct.callback = (XIMProc)attrs[1];
			canvas->done_luc_struct.client_data = (XPointer)attrs[2];
			break;

		    case WIN_IM_LUC_PROCESS:
			canvas->process_luc_struct.callback = (XIMProc)attrs[1];
			canvas->process_luc_struct.client_data  = (XPointer)attrs[2];
			break;
 
/*
		    case WIN_IM_AUX_START:
			canvas->start_aux_struct.callback = (XIMProc)attrs[1];
			canvas->start_aux_struct.client_data = (XPointer)attrs[2];
			break;
	 
		    case WIN_IM_AUX_DRAW:
			canvas->draw_aux_struct.callback = (XIMProc)attrs[1];
			canvas->draw_aux_struct.client_data = (XPointer)attrs[2];
			break;
	 
		    case WIN_IM_AUX_DONE:
			canvas->done_aux_struct.callback = (XIMProc)attrs[1];
			canvas->done_aux_struct.client_data = (XPointer)attrs[2];
			break;
*/
     
		}
	    }

	    input_window = (Xv_opaque)xv_get(frame_public, FRAME_INPUT_WINDOW);
	    
	    if (input_window == NULL) {
		XIMStyles   *style = NULL;
#define	 XV_SUPPORTED_STYLE_COUNT		7		    	    
				    
		XIMStyle	    supported_styles[XV_SUPPORTED_STYLE_COUNT];
		int		    i, j;    	    
		Xv_Drawable_info *info;
		Window      window;
		Frame       input_frame;
		XVaNestedList   pre_edit_list, status_list,
		    		    lookup_list, aux_list;
	    
		/* create the input window */
		input_frame = canvas_create_mle(frame_public, NULL);
     
		/* create input context for the window */
	    
		serverobj = XV_SERVER_FROM_WINDOW(canvas_public);
		/* Get the im form serverobj in order to pass to XCreateIC */
		im = (XIM)xv_get(serverobj, XV_IM);
		if (!im)
		    goto Return;
		XGetIMValues(im, XNQueryInputStyle, &style, NULL);
		
	        if (!style) 
	            goto Return;
	    
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

    	    DRAWABLE_INFO_MACRO(canvas_public, info);  
	if ((supported_styles[j] & XIMPreeditCallbacks) &&
	    (supported_styles[j] & XIMStatusCallbacks) &&
	    (supported_styles[j] & XIMLookupCallbacks) &&
	    (supported_styles[j] & XIMAuxCallbacks)) {
	    pre_edit_list = canvas_make_pre_edit_cb_list(canvas, paint_public);
	    status_list = canvas_make_status_cb_list(canvas, paint_public, input_frame);
	    lookup_list = canvas_make_lookup_cb_list(canvas, paint_public);
	    aux_list = canvas_make_aux_cb_list(canvas, paint_public);
	    canvas->ic = (XIC)XCreateIC(im, 
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
	    pre_edit_list = canvas_make_pre_edit_cb_list(canvas, paint_public);
	    status_list = canvas_make_status_cb_list(canvas, paint_public, input_frame);
	    lookup_list = canvas_make_lookup_cb_list(canvas, paint_public);
	    
	    canvas->ic = (XIC)XCreateIC(im, 
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
	    pre_edit_list = canvas_make_pre_edit_cb_list(canvas, paint_public);
	    status_list = canvas_make_status_cb_list(canvas, paint_public, input_frame);
	    aux_list = canvas_make_aux_cb_list(canvas, paint_public);
	    
	    canvas->ic = (XIC)XCreateIC(im, 
		XNClientWindow,	xv_xid(info),
		XNInputStyle, supported_styles[j],            		
		XNPreeditAttributes,	pre_edit_list,
		XNStatusAttributes,	status_list,
		XNExtXimp_AuxAttributes,	aux_list,
		XNExtXimp_Backfront, IMServBackend,
		NULL);		
	 } else if ((supported_styles[j] & XIMPreeditCallbacks) &&
	    (supported_styles[j] & XIMStatusCallbacks)) {
	    pre_edit_list = canvas_make_pre_edit_cb_list(canvas, paint_public);
	    status_list = canvas_make_status_cb_list(canvas, paint_public, input_frame);
	    
	    canvas->ic = (XIC)XCreateIC(im, 
		XNClientWindow,	xv_xid(info),
		XNInputStyle, supported_styles[j],            		
		XNPreeditAttributes,	pre_edit_list,
		XNStatusAttributes,	status_list,
		XNExtXimp_Backfront, IMServBackend,
		NULL);           	    
	}  else if (supported_styles[j] & XIMPreeditCallbacks) {
		pre_edit_list = canvas_make_pre_edit_cb_list(canvas, paint_public);
		
		canvas->ic = (XIC)XCreateIC(im, 
		XNClientWindow,	xv_xid(info),
		XNInputStyle, supported_styles[j],            		
		XNPreeditAttributes,	pre_edit_list,
		XNExtXimp_Backfront, IMServBackend,
		NULL);
	}  else if ((supported_styles[j] & XIMPreeditNothing) &&
			  (supported_styles[j] & XIMStatusCallbacks)) {
	    status_list = canvas_make_status_cb_list(canvas, paint_public, input_frame);
		
		canvas->ic = (XIC)XCreateIC(im, 
		XNClientWindow,	xv_xid(info),
		XNInputStyle, supported_styles[j],            		
		XNStatusAttributes,	status_list,
		XNExtXimp_Backfront, IMServBackend,
		NULL);
			  
	}  else if ((supported_styles[j] & XIMPreeditNothing) &&
			  (supported_styles[j] & XIMStatusNothing)) {
	     canvas->ic = (XIC)XCreateIC(im, 
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
		    

	    /* Hang the ic on the base frame
    	     * so that pre_edit_done can get
   	     * at the ic later.  Yes, this is
	     * a hack
	     */
	     if(canvas->ic) {
		xv_set(frame_public, WIN_IC, canvas->ic, 0);
	      }    
	    } else {
		/* hang ic on each paint window of each
		 * canvas
		 */
		canvas->ic = (XIC) xv_get(frame_public, WIN_IC);
	    }
	    ic_refcnt = (int) xv_get(frame_public,XV_KEY_DATA, frame_ic_refcnt);
	    ic_refcnt++;
	    xv_set(frame_public, XV_KEY_DATA, frame_ic_refcnt, ic_refcnt, 0);
	}
    }
Return:
#endif OW_I18N

    return(result);
}
		
Pkg_private int
canvas_view_destroy(view_public, stat)
    Canvas_view     view_public;
    Destroy_status  stat;
{
    Canvas_view_info *view = CANVAS_VIEW_PRIVATE(view_public);

    if ((stat == DESTROY_CLEANUP) || (stat == DESTROY_PROCESS_DEATH)) {
	if (xv_destroy_status(view->paint_window, stat) != XV_OK) {
	    return XV_ERROR;
	}
	if (stat == DESTROY_CLEANUP)
	    free((char *) view);
    }
    return XV_OK;
}

#ifdef OW_I18N
static int
canvas_view_create_paint_window(view, avlist)
    Canvas_view_info	*view;
    Attr_avlist		avlist;
#else
static int
canvas_view_create_paint_window(view)
    Canvas_view_info	*view;
#endif OW_I18N
{
    Canvas_view     view_public = CANVAS_VIEW_PUBLIC(view);
    Canvas_info    *canvas = view->private_canvas;
    Canvas          canvas_public = CANVAS_PUBLIC(canvas);
    Xv_Window       split_paint;
    Scrollbar       sb;

#ifdef OW_I18N
    Xv_opaque	    use_im = FALSE;
    Xv_opaque	    win_ic = NULL;
    Attr_avlist	    attrs;
#endif OW_I18N

    if (canvas->width == 0) {
	canvas->width = (int) xv_get(view_public, WIN_WIDTH);
    }
    if (canvas->height == 0) {
	canvas->height = (int) xv_get(view_public, WIN_HEIGHT);
    }
#ifdef OW_I18N
/*
 * BUG ALERT !!! The paint window being split, split_paint, 
 * is *NOT* always the 0th paint window, this is a v2 bug from the
 * code below, I've propogated it here for consistency,
 * this should be changed when the mainline XView code is fixed ...
 */
    if (status(canvas, created)) {
	split_paint =
	    (Xv_Window) xv_get(canvas_public, CANVAS_NTH_PAINT_WINDOW, 0);
	if (use_im = (Xv_opaque) xv_get(split_paint, WIN_USE_IM)) {
	    win_ic = (Xv_opaque) canvas->ic;
	}
    } else if (use_im = (Xv_opaque) xv_get(canvas_public, WIN_USE_IM)) {
	win_ic = (Xv_opaque) canvas->ic;
    }

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
        switch (attrs[0]) {

	    case WIN_USE_IM:
		use_im = (int) attrs[1];
		break;

            case WIN_IM_PREEDIT_START:
                canvas->start_pecb_struct.callback = (XIMProc)attrs[1];
                canvas->start_pecb_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_PREEDIT_DRAW:
                canvas->draw_pecb_struct.callback = (XIMProc)attrs[1];
                canvas->draw_pecb_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_PREEDIT_DONE:
                canvas->done_pecb_struct.callback = (XIMProc)attrs[1];
                canvas->done_pecb_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_STATUS_START:
                canvas->start_stcb_struct.callback = (XIMProc)attrs[1];
                canvas->start_stcb_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_STATUS_DRAW:
                canvas->draw_stcb_struct.callback = (XIMProc)attrs[1];
                canvas->draw_stcb_struct.client_data = (XPointer)attrs[2];
                break;
     
            case WIN_IM_STATUS_DONE:
                canvas->done_stcb_struct.callback = (XIMProc)attrs[1];
                canvas->done_stcb_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_LUC_START:
                canvas->start_luc_struct.callback = (XIMProc)attrs[1];
                canvas->start_luc_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_LUC_DRAW:
                canvas->draw_luc_struct.callback = (XIMProc)attrs[1];
                canvas->draw_luc_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_LUC_DONE:
                canvas->done_luc_struct.callback = (XIMProc)attrs[1];
                canvas->done_luc_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_LUC_PROCESS:
                canvas->process_luc_struct.callback = (XIMProc)attrs[1];
                canvas->process_luc_struct.client_data  = (XPointer)attrs[2];
                break;
 
/*
            case WIN_IM_AUX_START:
                canvas->start_aux_struct.callback = (XIMProc)attrs[1];
                canvas->start_aux_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_AUX_DRAW:
                canvas->draw_aux_struct.callback = (XIMProc)attrs[1];
                canvas->draw_aux_struct.client_data = (XPointer)attrs[2];
                break;
 
            case WIN_IM_AUX_DONE:
                canvas->done_aux_struct.callback = (XIMProc)attrs[1];
                canvas->done_aux_struct.client_data = (XPointer)attrs[2];
                break;
*/
 
            default:
                break;
 
        }
    }
#endif OW_I18N

    if (canvas->paint_avlist == NULL) {
	view->paint_window = xv_create(view_public, CANVAS_PAINT_WINDOW,
#ifdef OW_I18N
				       WIN_USE_IM, use_im,
#endif OW_I18N
				       WIN_WIDTH, canvas->width,
				       WIN_HEIGHT, canvas->height,
			     WIN_NOTIFY_SAFE_EVENT_PROC, canvas_paint_event,
			WIN_NOTIFY_IMMEDIATE_EVENT_PROC, canvas_paint_event,
				     WIN_RETAINED, status(canvas, retained),
			       WIN_X_PAINT_WINDOW, status(canvas, x_canvas),
				    XV_KEY_DATA, canvas_context_key, canvas,
				    XV_KEY_DATA, canvas_view_context_key, view,
				       0);
    } else {
	view->paint_window = xv_create(view_public, CANVAS_PAINT_WINDOW,
				       ATTR_LIST, canvas->paint_avlist,
#ifdef OW_I18N
					WIN_USE_IM, use_im,
#endif OW_I18N
				       WIN_WIDTH, canvas->width,
				       WIN_HEIGHT, canvas->height,
			     WIN_NOTIFY_SAFE_EVENT_PROC, canvas_paint_event,
			WIN_NOTIFY_IMMEDIATE_EVENT_PROC, canvas_paint_event,
				     WIN_RETAINED, status(canvas, retained),
			       WIN_X_PAINT_WINDOW, status(canvas, x_canvas),
				    XV_KEY_DATA, canvas_context_key, canvas,
				    XV_KEY_DATA, canvas_view_context_key, view,
				       0);
	xv_free(canvas->paint_avlist);
	canvas->paint_avlist = canvas->paint_end_avlist = NULL;
    }

    if (view->paint_window == NULL) {
	return ((int) XV_ERROR);
    }

#ifdef OW_I18N
    if (canvas->ic)
        xv_set(view->paint_window, WIN_IC, canvas->ic, 0);
#endif
    
    if (status(canvas, created)) {
	split_paint = (Xv_Window) xv_get(canvas_public, CANVAS_NTH_PAINT_WINDOW, 0);
	if (split_paint != NULL) {
	    Xv_opaque   defaults_array[ATTR_STANDARD_SIZE];
	    Attr_avlist defaults = defaults_array;
	    Xv_opaque   value;
	    
	    /* inherit certain attributes from the split window */
	    value = xv_get(split_paint, WIN_BACKGROUND_PIXMAP, 0);
	    if (value) {
		*defaults++ = (Xv_opaque)WIN_BACKGROUND_PIXMAP;
		*defaults++ = xv_get(split_paint, WIN_BACKGROUND_PIXMAP, 0);
	    }		
	    
	    *defaults++ = (Xv_opaque)WIN_BIT_GRAVITY;
	    *defaults++ = xv_get(split_paint, WIN_BIT_GRAVITY, 0);

	    *defaults++ = (Xv_opaque)WIN_COLOR_INFO;
	    *defaults++ = xv_get(split_paint, WIN_COLOR_INFO, 0);
	    
            *defaults++ = (Xv_opaque)WIN_COLUMN_GAP;
	    *defaults++ = xv_get(split_paint, WIN_COLUMN_GAP, 0);

            *defaults++ = (Xv_opaque)WIN_COLUMN_WIDTH;
	    *defaults++ = xv_get(split_paint, WIN_COLUMN_WIDTH, 0);

            *defaults++ = (Xv_opaque)WIN_CURSOR;
	    *defaults++ = xv_get(split_paint, WIN_CURSOR, 0);

	    *defaults++ = (Xv_opaque)WIN_EVENT_PROC;
	    *defaults++ = xv_get(split_paint, WIN_EVENT_PROC, 0);

	    *defaults++ = (Xv_opaque)WIN_ROW_GAP;
	    *defaults++ = xv_get(split_paint, WIN_ROW_GAP, 0);

	    *defaults++ = (Xv_opaque)WIN_ROW_HEIGHT;
	    *defaults++ = xv_get(split_paint, WIN_ROW_HEIGHT, 0);

	    *defaults++ = (Xv_opaque)WIN_WINDOW_GRAVITY;
	    *defaults++ = xv_get(split_paint, WIN_WINDOW_GRAVITY, 0);

	    *defaults++ = (Xv_opaque)WIN_X_EVENT_MASK;
	    *defaults++ = xv_get(split_paint, WIN_X_EVENT_MASK, 0);

	    /* null terminate the list */
	    *defaults   = (Xv_opaque)0;
	    
	    /* propagate the attrs to the new paint window */
	    (void)xv_set_avlist(view->paint_window, defaults_array);
	    
	    /* Deal with possible scrollbars */
	    sb = (Scrollbar)xv_get(canvas_public, OPENWIN_VERTICAL_SCROLLBAR,
				   view_public);
	    if (sb != NULL) {
		canvas_scroll(view->paint_window, sb);
	    }
	    sb = (Scrollbar)xv_get(canvas_public, OPENWIN_HORIZONTAL_SCROLLBAR,
				   view_public);
	    if (sb != NULL) {
		canvas_scroll(view->paint_window, sb);
	    }
	}
    } else {
	xv_set(view->paint_window,
	       WIN_BIT_GRAVITY,
	            status(canvas, fixed_image) ? NorthWestGravity : ForgetGravity,
	       WIN_CONSUME_EVENTS,
	           KBD_USE,
	           KBD_DONE,
	           WIN_ASCII_EVENTS,
	           ACTION_HELP,
	           WIN_MOUSE_BUTTONS,
	           0,
	       0);
	status_set(canvas, created);
    }

    return (XV_OK);
}
