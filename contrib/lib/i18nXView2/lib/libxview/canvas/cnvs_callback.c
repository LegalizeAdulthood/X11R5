#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)cnvs_callback.c 70.10 91/10/01";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifdef	OW_I18N

#include <xview/xv_i18n.h>
#include <xview_private/draw_impl.h>
#include <xview_private/cnvs_impl.h>
#include <xview_private/xv_i18n_impl.h>
#include <X11/Xlib.h>
#include <X11/XSunExt.h>
#include <xview/frame.h>
#include <xview/canvas.h>
#include <xview/panel.h>
#include <xview/font.h>
#include <xview/notice.h>

#define	MAX_INTERM_TEXT		1024


Xv_opaque ml_panel_create_canvas_mle();
void ml_canvas_mle_event_proc();
void pre_edit_done();
void canvas_status_draw();
extern void status_draw();

/*
 * allocate memory storage for interm text. First get the public handle
 * of the panel item and then get the public handle of the input window.
 * Simulate a panel_text_start call to allocate the interm text storage
 * space for this item.
 */

int
canvas_text_start(ic, client_data, callback_data)
XIC		ic;
XPointer	client_data;		
XPointer	callback_data;
{
	Canvas		canvas_public;
	Frame		frame_public; /* base frame for canvas */
	Panel_item	input_window; /* panel text item for input_frame */
	Panel		panel_public; /* panelsw for input_frame */
#ifdef POSTPUBLICREVIEW_BC	
	XID		*xid = NULL;
#else /* POSTPUBLICREVIEW_BC */
	XID		xid = NULL;	
#endif /* POSTPUBLICREVIEW_BC */	
	/*  Cannot trust client_data since the original canvas might not 
	 *  be valid anymore. 
	 *  If we could not get the XNFocusWindow, then give it a shot
	 *  with client_data.
	 */
#ifdef POSTPUBLICREVIEW_BC		 
	if ((!XGetICValues(ic, XNFocusWindow, &xid, NULL)) && xid) {
	   Xv_Window	paint_public = (Canvas) win_data(XDisplayOfIM(XIMOfIC(ic)), *xid);
	   Canvas_info    *canvas = (Canvas_info *) xv_get(paint_public, XV_KEY_DATA,
					    canvas_context_key);
	   canvas_public = CANVAS_PUBLIC(canvas);				     
	} else
	   canvas_public = (Canvas)client_data;
	   
	if (xid)
	    XFree(xid);
#else /* POSTPUBLICREVIEW_BC */
	if (!XGetICValues(ic, XNFocusWindow, &xid, NULL)) {
	   Xv_Window	paint_public = (Canvas) win_data(XDisplayOfIM(XIMOfIC(ic)), xid);
	   Canvas_info    *canvas = (Canvas_info *) xv_get(paint_public, XV_KEY_DATA,
					    canvas_context_key);
	   canvas_public = CANVAS_PUBLIC(canvas);				     
	} else
	   canvas_public = (Canvas)client_data;
#endif /* POSTPUBLICREVIEW_BC */	
	    
	      
	frame_public = (Frame)xv_get(canvas_public, WIN_FRAME);
	input_window = (Panel_item)xv_get(frame_public, FRAME_INPUT_WINDOW);
	/* get the public handle for the input panel window , input window for 
	   canvas is pointing to the input panel_text_item */
	panel_public = xv_get(input_window, PANEL_PARENT_PANEL);
	
	/* show cmd frame if in conversion mode */
	/* Should check for conversion on, but it is not working correctly
	 * in Htt.
	 */
	xv_set(xv_get(panel_public, WIN_FRAME), XV_SHOW, TRUE, NULL);

	panel_text_start(ic, panel_public, NULL);
	return(MAX_INTERM_TEXT); /* Should really use the return value of panel_text_start */
	
}
		
/*
 * get the handle to the input panel window, simulate the panel_text_draw
 * callback to do the drawing.
 *	committed string ==> pass to application through
 *			     window event proc
 *	Note: JLE is just following the regular ascii
 *	      handling.In the ascii world, when
 *      user type, an event is also being generated.
 *      It is up to the application to decide where
 *      to display the text. Also, one EUC char per event.
 */
void
canvas_text_draw(ic, client_data, callback_data)
XIC		ic;
XPointer	client_data;
XPointer	callback_data;

{
	/* call panel interm display routine */
	Canvas		canvas_public;
	Frame		frame_public;
	Panel_item	input_window;
	Panel		panel_public;
#ifdef POSTPUBLICREVIEW_BC	
	XID		*xid = NULL;
#else /* POSTPUBLICREVIEW_BC */
	XID		xid = NULL;	
#endif /* POSTPUBLICREVIEW_BC */	
	
	/*  Cannot trust client_data since the original canvas might not 
	 *  be valid anymore. 
	 *  If we could not get the XNFocusWindow, then give it a shot
	 *  with client_data.
	 */
#ifdef POSTPUBLICREVIEW_BC		 	 
	if ((!XGetICValues(ic, XNFocusWindow, &xid, NULL)) && xid) {
	   Xv_Window	paint_public = (Canvas) win_data(XDisplayOfIM(XIMOfIC(ic)), *xid);
	   Canvas_info    *canvas = (Canvas_info *) xv_get(paint_public, XV_KEY_DATA,
					    canvas_context_key);
	   canvas_public = CANVAS_PUBLIC(canvas);			
	} else
	   canvas_public = (Canvas)client_data;
	   
	if (xid)
	    XFree(xid);
#else /* POSTPUBLICREVIEW_BC */
	if (!XGetICValues(ic, XNFocusWindow, &xid, NULL)) {
	   Xv_Window	paint_public = (Canvas) win_data(XDisplayOfIM(XIMOfIC(ic)), xid);
	   Canvas_info    *canvas = (Canvas_info *) xv_get(paint_public, XV_KEY_DATA,
					    canvas_context_key);
	   canvas_public = CANVAS_PUBLIC(canvas);				     
	} else
	   canvas_public = (Canvas)client_data;
#endif /* POSTPUBLICREVIEW_BC */	
	    
	frame_public = (Frame)xv_get(canvas_public, WIN_FRAME);
	input_window = (Panel_item)xv_get(frame_public, FRAME_INPUT_WINDOW);
	/* get the public handle for the input panel window , input window for 
	   canvas is pointing to the input panel_text_item */
	panel_public = xv_get(input_window, PANEL_PARENT_PANEL);

	panel_text_draw(ic, panel_public, callback_data);
	
	
}

/*
 * free memory storage for interm text. First get the public handle
 * of the panel item and then get the public handle of the input window.
 * Simulate a panel_text_done call to free the interm text storage
 * space for this item.
 */
void
canvas_text_done(ic, client_data, callback_data)
XIC		ic;
XPointer	client_data;
XPointer	callback_data;

{
	Canvas		canvas_public;
	Frame		frame_public;
	Panel_item	input_window;
	Panel		panel_public;
#ifdef POSTPUBLICREVIEW_BC	
	XID		*xid = NULL;
#else /* POSTPUBLICREVIEW_BC */
	XID		xid = NULL;	
#endif /* POSTPUBLICREVIEW_BC */	
	
	/*  Cannot trust client_data since the original canvas might not 
	 *  be valid anymore. 
	 *  If we could not get the XNFocusWindow, then give it a shot
	 *  with client_data.
	 */
#ifdef POSTPUBLICREVIEW_BC		 	 
	if ((!XGetICValues(ic, XNFocusWindow, &xid, NULL)) && xid) {
	   Xv_Window	paint_public = (Canvas) win_data(XDisplayOfIM(XIMOfIC(ic)), *xid);
	   Canvas_info 	*canvas = (Canvas_info *) xv_get(paint_public, XV_KEY_DATA,
					    canvas_context_key);
	   canvas_public = CANVAS_PUBLIC(canvas);			
	} else
	   canvas_public = (Canvas)client_data;
	
	if (xid)
	    XFree(xid);
#else /* POSTPUBLICREVIEW_BC */
	if (!XGetICValues(ic, XNFocusWindow, &xid, NULL)) {
	   Xv_Window	paint_public = (Canvas) win_data(XDisplayOfIM(XIMOfIC(ic)), xid);
	   Canvas_info    *canvas = (Canvas_info *) xv_get(paint_public, XV_KEY_DATA,
					    canvas_context_key);
	   canvas_public = CANVAS_PUBLIC(canvas);				     
	} else
	   canvas_public = (Canvas)client_data;
#endif /* POSTPUBLICREVIEW_BC */		      
	frame_public = (Frame)xv_get(canvas_public, WIN_FRAME);
	input_window = (Panel_item)xv_get(frame_public, FRAME_INPUT_WINDOW);
	/* get the public handle for the input panel window , input window for 
	   canvas is pointing to the input panel_text_item */
	panel_public = xv_get(input_window, PANEL_PARENT_PANEL);

	panel_text_done(ic, panel_public, NULL);

}	

void
canvas_text_caret(ic, client_data, callback_data)
XIC		ic;
XPointer	client_data;
XPointer	callback_data;
{
	/*
	interprete direction and calculate row and col;
	set cursor position
	*/
}
		



/****************************************************************/
/* 			SUPPORT FOR CANVAS 			*/
/****************************************************************/



/*
 * ml_panel_create_canvas_mle(frame, font) is called from canvas_create_mle()
 * in canvas.c.  Panel_impl.h conflicts with canvas_impl.h, so we
 * let the real work of canvas_create_mle be done here.
 * ml_panel_create_canvas_mle(frame, font) creates a cmd frame, panel sw and
 * a panel text item for canvas intermediate text. (Xv_font *)font is
 * the font used to display the intermediate text.
 */
Xv_opaque
ml_panel_create_canvas_mle(frame, font)
	Frame		frame;
	Xv_font		font;
{
#define LEFT_RIGHT_CHAR_BORDER		4
#define	INPUT_WINDOW_WIDTH		300
	int			width, display_columns;
	int			chrwth;
	Frame			ml_cmdframe;
	Panel			ml_panel;
	Panel_item		ml_panel_item;

	width = INPUT_WINDOW_WIDTH;
	/* get a sample column width */
	chrwth = xv_get(font, FONT_DEFAULT_CHAR_WIDTH);
	display_columns = (width / chrwth) - LEFT_RIGHT_CHAR_BORDER;

	ml_cmdframe = (Frame) xv_create(frame, FRAME_CMD,
	    XV_LABEL,	XV_I18N_MSG("xv_messages", "Pre-Edit Display"),
	    WIN_USE_IM,			FALSE,
	    FRAME_CMD_POINTER_WARP,     FALSE,
	    FRAME_SHOW_RESIZE_CORNER,	TRUE,
	    FRAME_CMD_PUSHPIN_IN,	TRUE,
	    FRAME_DONE_PROC,		pre_edit_done,
	    NULL);

	ml_panel = xv_get(ml_cmdframe, FRAME_CMD_PANEL);

	xv_set(ml_panel,
	    WIN_ROWS,		1,
	    WIN_COLUMNS,	display_columns + LEFT_RIGHT_CHAR_BORDER,
	    WIN_IGNORE_X_EVENT_MASK, 	(KeyPress|KeyRelease|ButtonPress|ButtonRelease),
	    XV_FONT,	font,
	    0);

	notify_interpose_event_func(ml_panel, ml_canvas_mle_event_proc,
	    NOTIFY_SAFE);

	ml_panel_item = (Panel_item)xv_create(ml_panel,
	    PANEL_TEXT,
	    PANEL_VALUE_DISPLAY_LENGTH,	display_columns,
	    PANEL_VALUE_STORED_LENGTH, 	MAX_INTERM_TEXT,
	    NULL);

	xv_set(frame, FRAME_INPUT_WINDOW, ml_panel_item, NULL);

	window_fit(ml_panel);
	window_fit(ml_cmdframe);

	return(ml_cmdframe);
}

/*
 * ml_canvas_mle_event_proc((Window)nls_canvas_mle, (Event)event, arg)
 * handles the resizing of the canvas mls.  It changes the display
 * length of the text item.
 */
void
ml_canvas_mle_event_proc(ml_canvas_mle, event, arg, type)
	Window			ml_canvas_mle;
	Event			*event;
	Notify_arg		arg;
	Notify_event_type	type;
{
	Panel_item	ip;
	Xv_font		*font;
	int		width, display_columns;
	int		chrwth, chrheight;

	if ((event_action(event) == WIN_RESIZE) ||
	    (event_action(event) == WIN_REPAINT)) {
		(void)notify_next_event_func(ml_canvas_mle, event,
		    arg, type);
		ip = (Panel_item)xv_get(ml_canvas_mle, PANEL_CARET_ITEM);
		if ((font = (Xv_font *)xv_get(ip, XV_FONT)) == (Xv_font *)NULL)
			return;
		width = (int)xv_get(ml_canvas_mle, WIN_WIDTH);
		chrwth = xv_get(font, FONT_DEFAULT_CHAR_WIDTH);
		display_columns = (width / chrwth) - LEFT_RIGHT_CHAR_BORDER;
		xv_set(ip,
		    PANEL_VALUE_DISPLAY_LENGTH, display_columns,
		    0);
	}
}



/*
 * nls_panel_link_canvas_tscreen(ip, canvas_tscreen) links to the
 * canvas_tscreen to the panel, so panel package can repaint the
 * canvas intermediate text.
 */
/*
nls_panel_link_canvas_tscreen(ip, canvas_tscreen)
	Panel_item	ip;
	Tscreen			*canvas_tscreen;
{
	if (ip->panel)
		ip->panel->pa_tscreen = canvas_tscreen;
}

*/

/*
 * Only bring down the pre-edit popup if conversion mode is off
 * Jenny, I need the ic associated with the canvas paint window
 * not the ic associated with the panel of the cmd frame??
 */
void
pre_edit_done (input_frame)

Frame	input_frame; /* frame for pre-edit */

{
    Frame	frame_public;  /* base frame for pre_edit popup */
    XIC		ic;
#ifdef POSTPUBLICREVIEW_BC	
    int		*conv_on = NULL; /* status of conversion mode */
#else /* POSTPUBLICREVIEW_BC */
    int		conv_on; /* status of conversion mode */
#endif /* POSTPUBLICREVIEW_BC */	
    


    frame_public = (Frame) xv_get(input_frame, XV_OWNER);
    ic = (XIC) xv_get(frame_public, WIN_IC);
#ifdef POSTPUBLICREVIEW_BC	    
    if (!XGetICValues(ic, XNExtXimp_Conversion, &conv_on, NULL))
    {
	if (conv_on && *conv_on == XIMDisable)
	    xv_set(input_frame, XV_SHOW, FALSE, NULL);
	else 
	    notice_prompt(input_frame, NULL,
			  NOTICE_MESSAGE_STRING,
			  XV_I18N_MSG("xv_messages", "\
	You are in conversion mode.\n\
So the Pre-Edit Display window cannot be dismissed."),
			  NOTICE_BUTTON_YES, "Continue",
			  NULL);
			  
    }
    if (conv_on)
	XFree(conv_on);
#else /* POSTPUBLICREVIEW_BC */
    if (!XGetICValues(ic, XNExtXimp_Conversion, &conv_on, NULL))
    {
	if (conv_on == XIMDisable)
	    xv_set(input_frame, XV_SHOW, FALSE, NULL);
	else 
	    notice_prompt(input_frame, NULL,
			  NOTICE_MESSAGE_STRING,
			  XV_I18N_MSG("xv_messages", "\
	You are in conversion mode.\n\
So the Pre-Edit Display window cannot be dismissed."),
			  NOTICE_BUTTON_YES, "Continue",
			  NULL);
			  
    }
#endif /* POSTPUBLICREVIEW_BC */	
	
}
void
canvas_status_draw(ic, client_data, cb_data)
XIC		*ic;
XPointer	client_data;
XIMStatusDrawCallbackStruct *cb_data;
{
	Frame		input_frame;  /* cmd frame handle */
	Frame		base_frame;
	Panel_item	input_window; /* panel text item for echo back */
	Panel		panel_public; /* panelsw for input_window */
	int 	        conv_on; /* flag for checking conversion mode */

	/* obtain cmd frame handle from the panel text item */

	input_frame = (Frame) client_data;
	
#ifdef notdef		
	/* In the Ximp server, conversion on is send after
	 * the status draw. So this code is moved to pre-edit start
	 */ 
 
	/* show cmd frame if in conversion mode */

	if (!XGetICValues(ic, XNExtXimp_Conversion, conv_on, NULL))
	{
	    if (conv_on == XIMEnable)
		xv_set(input_frame, XV_SHOW, TRUE, NULL);
	}
#endif

	/* Call the real status_draw routine */
	base_frame = (Frame) xv_get(input_frame, XV_OWNER);
	status_draw(ic, (XPointer) base_frame, cb_data);
}
#endif	OW_I18N
