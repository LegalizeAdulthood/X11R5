#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)win_input.c 70.8 91/08/27";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Win_input.c: Implement the input functions of the win_struct.h interface.
 */

#include <stdio.h>
#include <xview/frame.h>
#include <xview/icon.h>
#include <xview/pkg.h>
#include <xview/rect.h>
#include <xview/rectlist.h>
#include <xview_private/draw_impl.h>
#include <xview_private/win_info.h>
#include <xview/win_input.h>
#include <xview_private/win_keymap.h>
#include <xview/fullscreen.h>
#include <xview/defaults.h>

#ifdef _XV_DEBUG
#include <xview/panel.h>
#include <xview/canvas.h>
#endif

#include <xview/win_struct.h>
#include <xview/win_screen.h>
#include <xview/window.h>
#include <xview/server.h>
#include <xview_private/svr_atom.h>
#define XV_ATTRIBUTES_ONLY
#include <xview/screen.h>
#undef XV_ATTRIBUTES_ONLY
#include <xview_private/ntfy.h>
#include <xview/notify.h>
#include <xview_private/ndis.h>
#include <xview_private/windowimpl.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#ifdef OW_I18N
#include <locale.h>
#endif


extern int      SELN_FNMASK;
extern struct rectlist *win_get_damage();
extern void     server_set_timestamp();
extern Xv_opaque server_get_timestamp();
extern Xv_object win_data();
extern void     window_update_cache_rect();
extern char    *xv_app_name;

Xv_private void window_release_selectbutton();

FILE           *fopen(), *fexp;

Xv_object       xview_x_input_readevent();
static void     process_selection_events();
extern          input_imnull();

static int      process_clientmessage_events();
static int      process_property_events();
static int      process_wm_pushpin_state();

#ifndef OW_I18N
#define 	BUFFERSIZE	256
#endif

/* the following should be defined in XLIB */
typedef struct {
    int             state;	/* IconicState or NormalState */
    XID             icon;	/* icon window XID */
}               WMState;


void
win_xmask_to_im(xevent_mask, im)
    register unsigned int xevent_mask;
    register Inputmask *im;

{
    register int    i;

    input_imnull(im);

    if (xevent_mask & ExposureMask)
	win_setinputcodebit(im, WIN_REPAINT);

    if (xevent_mask & PointerMotionMask)
	win_setinputcodebit(im, LOC_MOVE);

    if (xevent_mask & EnterWindowMask)
	win_setinputcodebit(im, LOC_WINENTER);

    if (xevent_mask & LeaveWindowMask)
	win_setinputcodebit(im, LOC_WINEXIT);

    if (xevent_mask & ButtonMotionMask)
	win_setinputcodebit(im, LOC_DRAG);

    if (xevent_mask & KeymapStateMask)
	win_setinputcodebit(im, KBD_MAP);

    if (xevent_mask & VisibilityChangeMask)
	win_setinputcodebit(im, WIN_VISIBILITY_NOTIFY);

    if (xevent_mask & StructureNotifyMask) 
	win_setinputcodebit(im, WIN_STRUCTURE_NOTIFY);
	
    if (xevent_mask & SubstructureNotifyMask) 
	win_setinputcodebit(im, WIN_SUBSTRUCTURE_NOTIFY);
	
    if (xevent_mask & ResizeRedirectMask) 
	win_setinputcodebit(im, WIN_RESIZE_REQUEST);

    if (xevent_mask & PropertyChangeMask) 
	win_setinputcodebit(im, WIN_PROPERTY_NOTIFY);

    if (xevent_mask & ColormapChangeMask) 
	win_setinputcodebit(im, WIN_COLORMAP_NOTIFY);

    if (xevent_mask & SubstructureRedirectMask) 
	win_setinputcodebit(im, WIN_SUBSTRUCTURE_REDIRECT);

    if (xevent_mask & KeyPressMask) {
	im->im_flags |= IM_ASCII;
	for (i = 1; i <= (KEY_LEFTLAST - KEY_LEFTFIRST); i++)
	    win_setinputcodebit(im, KEY_LEFT(i));

	for (i = 1; i <= (KEY_RIGHTLAST - KEY_RIGHTFIRST); i++)
	    win_setinputcodebit(im, KEY_RIGHT(i));

	for (i = 1; i <= (KEY_TOPLAST - KEY_TOPFIRST); i++)
	    win_setinputcodebit(im, KEY_TOP(i));
    }
    if (xevent_mask & KeyReleaseMask)
	im->im_flags |= (IM_NEGASCII | IM_NEGEVENT | IM_NEGMETA);

    if (xevent_mask & FocusChangeMask) {
	win_setinputcodebit(im, KBD_USE);
	win_setinputcodebit(im, KBD_DONE);
    }
    if ((xevent_mask & ButtonPressMask) || (xevent_mask & ButtonReleaseMask)) {
	for (i = 1; i <= (BUT_LAST - BUT_FIRST); i++)
	    win_setinputcodebit(im, BUT(i));
	if (xevent_mask & ButtonReleaseMask)
	    im->im_flags |= IM_NEGEVENT;
    }
}


/*
 * Convert Sunview events to xevents.
 */
unsigned int
win_im_to_xmask(window, im)
    Xv_object       window;
    register Inputmask *im;
{
    register unsigned int xevent_mask = 0;
    register int    i;

    /*
     * BUG The events that cannot be generated in X are: LOC_STILL,
     * LOC_RGN{EXIT,ENTER}, WIN_STOP, LOC_TRAJECTORY, KBD_REQUEST
     */
    if (win_getinputcodebit(im, LOC_MOVE))
	xevent_mask |= PointerMotionMask;
    if (win_getinputcodebit(im, LOC_WINENTER))
	xevent_mask |= EnterWindowMask;
    if (win_getinputcodebit(im, LOC_WINEXIT))
	xevent_mask |= LeaveWindowMask;
    if (win_getinputcodebit(im, KBD_MAP))
	xevent_mask |= KeymapStateMask;
    if (win_getinputcodebit(im, WIN_VISIBILITY_NOTIFY))
	xevent_mask |= VisibilityChangeMask;
    if ((win_getinputcodebit(im, WIN_CIRCULATE_NOTIFY)) ||
        (win_getinputcodebit(im, WIN_DESTROY_NOTIFY)) ||
	(win_getinputcodebit(im, WIN_GRAVITY_NOTIFY)) ||
	(win_getinputcodebit(im, WIN_MAP_NOTIFY)) ||
	(win_getinputcodebit(im, WIN_REPARENT_NOTIFY)) ||
/*
	(win_getinputcodebit(im, WIN_RESIZE)) ||
*/
	(win_getinputcodebit(im, WIN_UNMAP_NOTIFY)))
	xevent_mask |= StructureNotifyMask;
	
    if (win_getinputcodebit(im, WIN_SUBSTRUCTURE_NOTIFY))
	xevent_mask |= SubstructureNotifyMask;

    if (win_getinputcodebit(im, WIN_RESIZE_REQUEST))
	xevent_mask |= ResizeRedirectMask;

    if (win_getinputcodebit(im, WIN_PROPERTY_NOTIFY))
	xevent_mask |= PropertyChangeMask;

    if (win_getinputcodebit(im, WIN_COLORMAP_NOTIFY))
	xevent_mask |= ColormapChangeMask;
	
    if ((win_getinputcodebit(im, WIN_CIRCULATE_REQUEST)) ||
        (win_getinputcodebit(im, WIN_CONFIGURE_REQUEST)) ||
	(win_getinputcodebit(im, WIN_MAP_REQUEST)))
	xevent_mask |= SubstructureRedirectMask;

    if (win_getinputcodebit(im, LOC_DRAG)) {
	xevent_mask |= ButtonMotionMask;
    }
    /* BUT(1-3) are MS_{LEFT, MIDDLE, RIGHT} */
    for (i = 1; i <= (BUT_LAST - BUT_FIRST); i++) {
	if (win_getinputcodebit(im, BUT(i))) {
	    xevent_mask |= ButtonPressMask;
	    break;
	}
    }

    /* Set ButtonReleaseMask if some button consumed and IM_NEGEVENT */
    if (xevent_mask & ButtonPressMask && im->im_flags & IM_NEGEVENT)
	xevent_mask |= ButtonReleaseMask;

    if (win_getinputcodebit(im, WIN_REPAINT)) {
	xevent_mask |= ExposureMask;
    }
    /* Enable focus change events if consuming KBD_USE/KBD_DONE */
    if (win_getinputcodebit(im, KBD_USE) ||
	win_getinputcodebit(im, KBD_DONE)) {
	xevent_mask |= FocusChangeMask;
    }
    /*
     * if top level window AND it does not have no decor flag set then turn
     * on StructureNotify  and PropertyChangeMask
     */
    if ((window) && (Bool) xv_get(window, WIN_TOP_LEVEL) &&
	!(Bool) xv_get(window, WIN_TOP_LEVEL_NO_DECOR)) {
	xevent_mask |= StructureNotifyMask | PropertyChangeMask;
    }
    /*
     * NOTE:  If interested in any keyboard events, must set ButtonPressMask
     * and FocusChangeMask for Click to Type.
     * 
     * BUG ALERT: If you are interested in any keyboard events, you will see
     * button press, KBD_USE, and KBD_DONE events whether you want them or
     * not.
     */
    if ((im->im_flags & IM_NEGASCII) ||
	(im->im_flags & IM_NEGMETA))
	xevent_mask |= KeyReleaseMask | ButtonPressMask | FocusChangeMask;

    /*
     * NOTE:  Anything below this point only deal with KeyPressMask.
     */
    if (im->im_flags & IM_ASCII) {
	xevent_mask |= KeyPressMask | FocusChangeMask;
	goto Return;
    }
    for (i = 1; i <= KEY_LEFTLAST - KEY_LEFTFIRST; i++)
	if (win_getinputcodebit(im, KEY_LEFT(i))) {
	    xevent_mask |= KeyPressMask | FocusChangeMask;
	    goto Return;
	}
    for (i = 1; i <= KEY_RIGHTLAST - KEY_RIGHTFIRST; i++)
	if (win_getinputcodebit(im, KEY_RIGHT(i))) {
	    xevent_mask |= KeyPressMask | FocusChangeMask;
	    goto Return;
	}
    for (i = 1; i <= KEY_TOPLAST - KEY_TOPFIRST; i++)
	if (win_getinputcodebit(im, KEY_TOP(i))) {
	    xevent_mask |= KeyPressMask | FocusChangeMask;
	    goto Return;
	}
Return:
    /* Set KeyReleaseMask if consuming keyboard events and IM_NEGEVENT */
    if (im->im_flags & IM_NEGEVENT && xevent_mask & KeyPressMask)
	xevent_mask |= KeyReleaseMask;
    return (xevent_mask);
}

/*
 * Utilities
 */
input_imnull(im)
    struct inputmask *im;
{
    int             i;
    /* BUG:  Use bzero here */
    im->im_flags = 0;
    for (i = 0; i < IM_MASKSIZE; i++)
	im->im_keycode[i] = 0;
}

input_imall(im)
    struct inputmask *im;
{
    int             i;

    (void) input_imnull(im);
    im->im_flags = IM_ASCII | IM_META;
    for (i = 0; i < IM_MASKSIZE; i++)
	im->im_keycode[i] = 1;
}

/*
 * Find an event for req_window. Block till the event for this window is
 * found.
 */
Xv_object
input_readevent(window, event)
    Xv_object       window;
    Event          *event;
{
    register Xv_Drawable_info *info;
    XEvent	    xevent;

    DRAWABLE_INFO_MACRO(window, info);
    return (xview_x_input_readevent(xv_display(info), event, window, TRUE, FALSE, 0, &xevent));
}

/* BUG: implement or throw out all this focus stuff */

/*ARGSUSED*/
win_refuse_kbd_focus(window)
    Xv_object       window;
{
}

/*ARGSUSED*/
win_release_event_lock(window)
    Xv_object       window;
{
}

int
win_set_kbd_focus(window, xid)
    Xv_object       window;
    XID             xid;
{
    int             rtn = 0;
    register Xv_Drawable_info *info;
    Xv_opaque       server_public;


    DRAWABLE_INFO_MACRO(window, info);
    /* Get server info */
    server_public = xv_server(info);

    if (xid == (XID) WIN_NULLLINK)
	xid = None;
    if (!xv_has_focus(info)) {
	Display        *display = xv_display(info);

	rtn = XSetInputFocus(display, xid, RevertToParent,
			     server_get_timestamp(server_public));
    }
    /*
     * if(xv_get(xv_server(info),SERVER_JOURNALLING))
     * xv_set(xv_server(info),SERVER_JOURNAL_SYNC_EVENT,1,0);
     */
    return (rtn);
}

XID
win_get_kbd_focus(window)
    Xv_object       window;
{
    register Xv_Drawable_info *info;
    XID             xid;
    int             state;

    DRAWABLE_INFO_MACRO(window, info);
    /* PERFORMANCE: Round trip to the server!!! */
    XGetInputFocus(xv_display(info), &xid, &state);
    return (xid == None ? (XID) WIN_NULLLINK : xid);
}

/*
 * Set no-focus state for window.  If true, don't set input focus on click.
 */
void
win_set_no_focus(window, state)
    Xv_object       window;
    int             state;
{
    register Xv_Drawable_info *info;

    DRAWABLE_INFO_MACRO(window, info);
    xv_set_no_focus(info, state);
}

#define	SET_SHIFTS(e, state, metamask)	\
	event_set_shiftmask(e, \
	    (((state) & ShiftMask) ? SHIFTMASK : 0) | \
	    (((state) & ControlMask) ? CTRLMASK : 0) | \
	    (((state) & metamask) ? META_SHIFT_MASK : 0) | \
	    (((state) & Button1Mask) ? MS_LEFT_MASK : 0) | \
	    (((state) & Button2Mask) ? MS_MIDDLE_MASK : 0) | \
	    (((state) & Button3Mask) ? MS_RIGHT_MASK : 0))

Xv_private_data Event xv_last_event;
Xv_private_data XEvent xv_last_x_event;

typedef struct {
    Xv_object       window_requested;
    Event           event;
}               Event_info;


/*
 * Predicate for XCheckIfEvent or XIfEvent Checks to see if the xevent
 * belongs to req_window or not.
 */
static int
is_reqwindow(display, xevent, event_info)
    Display        *display;
    XEvent         *xevent;
    Event_info     *event_info;
{
    Xv_object       ie_window;
    XAnyEvent      *any = (XAnyEvent *) xevent;
    int             event_type = (xevent->type & 0177);

    /*
     * Check for proper window before calling xevent_to_event, so translation
     * only takes place if event is wanted.
     */
    if (event_type > 1 &&
	event_info->window_requested == win_data(display, any->window) &&
	!xevent_to_event(display, xevent, &event_info->event, &ie_window))
	return (TRUE);
    else
	return (FALSE);
}

/*
 * Find an event for req_window. Block till the event for this window is
 * found.
 */
Xv_object
xv_input_readevent(window, event, block, type, im)
    Xv_object       window;
    Event          *event;
    int             block, type;
    Inputmask      *im;
{
    register Xv_Drawable_info *info;
    unsigned int    	xevent_mask;
    XEvent		xevent;

    if (im) {
	xevent_mask = win_im_to_xmask(window, im);
	if (!win_getinputcodebit(im, WIN_REPAINT))
	    xevent_mask &= ~ExposureMask;
	if (((Bool) xv_get(window, WIN_TOP_LEVEL) == TRUE) &&
	    !((Bool) xv_get(window, WIN_TOP_LEVEL_NO_DECOR) == TRUE))
	    xevent_mask &= ~StructureNotifyMask & ~PropertyChangeMask;
    }
    DRAWABLE_INFO_MACRO(window, info);
    return (xview_x_input_readevent(xv_display(info), event, window, block, type,
			      xevent_mask, &xevent));
}


/*
 * Read an event from an This needs to be rewritten
 */
Xv_object
xview_x_input_readevent(display, event, req_window, block, type, xevent_mask, rep)
    Display        *display;
    register Event *event;
    Xv_object       req_window;
    int             block, type;
    unsigned int    xevent_mask;
    XEvent          *rep;

{
    Xv_object       window = 0;

    /*
     * Read an event for the req_window.
     */
    if (req_window) {
	Xv_Drawable_info *info;
	Event_info      event_info;

	DRAWABLE_INFO_MACRO(req_window, info);
	if (type) {
	    if (block) {
		XWindowEvent(display, xv_xid(info), xevent_mask, rep);
		(void) xevent_to_event(display, rep, event, &window);
	    } else {
		if (!XCheckWindowEvent(display, xv_xid(info), xevent_mask, rep))
		    return (0);
		(void) xevent_to_event(display, rep, event, &window);
	    }
	    if(!window)
	      return(NULL);
	    event_set_window(event, req_window);
	} else {
	    event_info.window_requested = req_window;
	    if (block)
		XIfEvent(display, rep, is_reqwindow, &event_info);
	    else if (!XCheckIfEvent(display, rep, is_reqwindow,
				    &event_info))
		return (0);	/* window. pending and last event not set */
	    window = event_info.window_requested;
	    *event = event_info.event;
	    /* set the window in the event */
	    event_set_window(event, window);
	}
    } else {
	XNextEvent(display, rep);
	(void) xevent_to_event(display, rep, event, &window);
    }

    /*
     * pending = QLength(display);
     */
    return (window);
}


/*
 * This converts a xevent to Sunview event. If the event was invalid i.e. no
 * window wanted it ; *pwindow = 0; return 1 else *pwindow = window to which
 * event is to be posted to ; return 0
 * 
 * NOTE: Code has been placed here for handling click to type.  This isn't a
 * terribly appropriate place for it, but it is convenient, since all X
 * events pass through here before becoming  SunView events.  A modification
 * has been made to is_reqwindow, so each X event should be seen here exactly
 * once.
 */

static int
xevent_to_event(display, xevent, event, pwindow)
    Display        	*display;
    XEvent         	*xevent;
    Event          	*event;
    Xv_object      	*pwindow;
{
    XAnyEvent      	*any = (XAnyEvent *) xevent;
    Xv_object       	 window = NULL;
    static XID      	 pointer_window_xid;
    Xv_Drawable_info 	*info;
    Xv_opaque       	 server_public;
    register int    	 event_type ;
    register unsigned 	 temp;
    static short     	 nbuttons = 0;
    static Xv_opaque 	 last_server_public = NULL;
    static unsigned int  but2_mod,
			 but3_mod,
			*key_map;
    static int      	 META_MODMASK = 0;
    static char 	*ascii_sem_map,
			*key_sem_map;
#ifdef OW_I18N
    XIC			ic=NULL;
    Window		client_window;


    if(XFilterEvent(xevent, NULL)==True){
	*pwindow = NULL ;
	return(NULL);
    }
#endif
    event_type = (xevent->type & 0177);
    /* BUG: assuming 0 => error, 1 => reply */
    if (event_type > 1) {
	Window_info	*win;

	window = win_data(display, any->window);

	if (!window) {
	    if (event_type == ClientMessage) {
		XClientMessageEvent *cme = (XClientMessageEvent *) xevent;

		if (cme->data.l[0] == XV_POINTER_WINDOW) {
		    window = win_data(display, pointer_window_xid);
		} else if (cme->data.l[0] == XV_FOCUS_WINDOW) {
		    XID             focus_window_id;
		    int             dummy;

		    (void) XGetInputFocus(display, &focus_window_id, &dummy);
		    window = win_data(display, focus_window_id);
		}
	    }
	    if (!window)
		goto Default;
	}
	
        win = WIN_PRIVATE(window);

        /*
         * Check if window is deaf and the event type is one that we would 
         * want it to ignore.
         * Also check if the select button is pressed so that XAllowEvents
         * can be called.
         */
        if (WIN_IS_DEAF(win) && 
            ((event_type == KeyPress) ||
            (event_type == KeyRelease) ||
            (event_type == ButtonPress) ||
            (event_type == ButtonRelease) ||
            (event_type == MotionNotify)))  {

            XButtonEvent	*buttonEvent = (XButtonEvent *)&xevent->xbutton;

            if ((event_type == ButtonPress) && (buttonEvent->button == Button1))  {
                XAllowEvents(display, AsyncBoth, CurrentTime);
            }

            goto Default;
        }
    } else {
	XErrorEvent    *er = (XErrorEvent *) & any;

	fprintf(stderr, "Error event: \n");
	fprintf(stderr, "   serial = %d, error code = %d\n", er->serial,
		er->error_code);
	fprintf(stderr, "   request code = %d, minor code = %d\n",
		er->request_code, er->minor_code);
	fprintf(stderr, "   resource id = %d\n", er->resourceid);
	goto Default;
    }

    /* clear out the event */
    event_init(event);

    /* set the window */
    event_set_window(event, window);

    /* make a reference to the XEvent */
    event_set_xevent(event, xevent);

    /* Get server info */
    DRAWABLE_INFO_MACRO(window, info);
    server_public = xv_server(info);

    /* The following code caches server specific information such as the keymap,
     * position of the modifier key in the modifier mask and the number of
     * mouse buttons associated with the mouse.
     */
    if (last_server_public != server_public) {
	last_server_public = server_public;
	key_map = (unsigned int *) xv_get(server_public, SERVER_XV_MAP);
	key_sem_map = (char *) xv_get(server_public, SERVER_SEMANTIC_MAP);
	ascii_sem_map = (char *) xv_get(server_public, SERVER_ASCII_MAP);

			        /* Cache the modifier mask for the META keys. */
	META_MODMASK = (int) xv_get(server_public, SERVER_META_MOD_MASK);

	    			/* Get the number of phyical mouse buttons and
				 * the button modifier masks.  This info is
				 * cached.
				 */
	nbuttons = (short) xv_get(server_public, SERVER_MOUSE_BUTTONS);
	but2_mod = (unsigned int) xv_get(server_public, SERVER_BUTTON2_MOD);
	but3_mod = (unsigned int) xv_get(server_public, SERVER_BUTTON3_MOD);
    }

    switch (event_type) {
      case KeyPress:
      case KeyRelease:{
        XKeyEvent      		*ek = (XKeyEvent *) xevent;
	static char 		 buffer[BUFFERSIZE];
	static int               buf_length = 0;
	static Xv_opaque 	 last_server_public;
	static XComposeStatus	 status;
	KeySym		 	 ksym,
				 sem_ksym;
	unsigned int		 key_value;
	int			 modifiers = 0,
				 sem_action,
				 keyboard_key;
#ifdef OW_I18N
	int			 ret_status = XLookupNone;
	static char		 *mb_lookup_buffer;
	static int		 free_mb_lookup_buffer = False;
#endif

				/* Clear buffer before we fill it again.  
				 * Only NULL out the number of chars that
				 * where actually used in the last pass.
				 */
#ifndef OW_I18N
	bzero(buffer, buf_length); 
#endif 

#ifdef OW_I18N
	if (mb_lookup_buffer == 0) { 	/* first time */
            mb_lookup_buffer = buffer;
	    bzero(mb_lookup_buffer, buf_length);	/* buf_length should be zero at this point, just in case */
	} else if (mb_lookup_buffer != buffer) {	/* last time was overflowed to backup */
	    bzero(buffer, BUFFERSIZE);		/* may have filled the whole array 
						   last time*/ 
	    free(mb_lookup_buffer);
	    mb_lookup_buffer = buffer;
	} else 
	    bzero(buffer, buf_length); 
	    
	ksym = NoSymbol;
		    
#ifndef INCOMPATIBLE_WITH_POST_PUBLIC_REVEW_DRAFT
        if (event_type == KeyPress) {
            ic = (XIC)xv_get(window, WIN_IC);
	    if(ic){
	        buf_length = XmbLookupString(ic, ek, mb_lookup_buffer, BUFFERSIZE, &ksym, &ret_status); 
	        if (ret_status == XBufferOverflow) {
		    mb_lookup_buffer = (char *)malloc(buf_length + 1);
		    buf_length = XmbLookupString(ic, ek, mb_lookup_buffer, buf_length, &ksym, &ret_status); 
	        }
	    } else {
                buf_length = XLookupString(ek, buffer, BUFFERSIZE, &ksym, &status);
                ret_status = (ksym != NoSymbol) ? XLookupKeySym : XLookupNone;
	    }
        } else {
            /* 
             * Error: Should check if conversion on, then return if it is a
             * key IM interested in
             */
            buf_length = XLookupString(ek, buffer, BUFFERSIZE, &ksym, &status);
            ret_status = (ksym != NoSymbol) ? XLookupKeySym : XLookupNone;
        }
        
#else /* INCOMPATIBLE_WITH_POST_PUBLIC_REVEW_DRAFT */
	ic = (XIC)xv_get(window, WIN_IC);
	if(ic){
	    buf_length = XmbLookupString(ic, ek, mb_lookup_buffer, BUFFERSIZE, &ksym, &ret_status); 
	    if (ret_status == XBufferOverflow) {
		mb_lookup_buffer = (char *)malloc(buf_length + 1);
		buf_length = XmbLookupString(ic, ek, mb_lookup_buffer, buf_length, &ksym, &ret_status); 
	    }
	} else {
	    buf_length = XLookupString(ek, buffer, BUFFERSIZE, &ksym, &status);
            ret_status = (ksym != NoSymbol) ? XLookupKeySym : XLookupNone;
	}
#endif /* INCOMPATIBLE_WITH_POST_PUBLIC_REVEW_DRAFT */	  
	
	if (ret_status == XLookupNone)
	  goto Default;
#else
  	buf_length = XLookupString(ek, buffer, BUFFERSIZE, &ksym, &status); 
#endif   

				/* If the event is a synthetic event (the event
				 * came from SendEvent request), and the
				 * key-pressed is not the help key (for wm);
				 * we should ignore it for security reasons.
				 */
	if ( ((xevent->xany.send_event) && 
#ifdef OW_I18N
			       (((XKeyEvent *)xevent)->keycode) &&
#endif
	     		       (ksym != XK_Help)) && !defeat_event_security )  {
	    goto Default;
	}	
				/* Determine if this ksym is in the Keyboard
				 * Keysym set.  These are basically your
				 * Function, Shift, Ctrl, Meta, Keypad, etc
				 * keys.
				 */
	keyboard_key = ((ksym & KEYBOARD_KYSM_MASK) == KEYBOARD_KYSM);

				/* Set the modifier states */
	SET_SHIFTS(event, ek->state, META_MODMASK);

				/* Determine offsets into the semantic
				 * mapping tables.
				 */
	if (event_ctrl_is_down(event))
	    modifiers += 0x100;
	if (event_meta_is_down(event))
	    modifiers += 0x200;
	if (event_shift_is_down(event) && keyboard_key)
	    modifiers += 0x400;

				/* The semantic table only wants to see a
				 * shifted key when the shift key is down,
				 * not when it is caused by the lock key.
				 * So if the lock key is down and not the
				 * shift key we unshift the ksym and then do
				 * our semantic lookup. 
				 */
	sem_ksym = ksym;
	if ((ek->state & LockMask) && !(ek->state & ShiftMask) &&
						   (ksym >= 'A' && ksym <= 'Z'))
	    sem_ksym = ksym | 0x20;


				/* Look up in a semantic table to see if
				 * the event has an XView semantic event
				 * mapping. (eg. ACTION_OPEN)
				 * There is one table for keyboard keysysm
				 * and one for ascii keysyms.
				 */
	if (keyboard_key)
		sem_action = key_sem_map[(sem_ksym & 0xFF) + modifiers] |
							        XVIEW_SEMANTIC;
	else
		sem_action = ascii_sem_map[(sem_ksym & 0xFF) + modifiers] |
							        XVIEW_SEMANTIC;

	if ((ksym == NoSymbol) && 
#ifdef OW_I18N
	    (((XKeyEvent *)xevent)->keycode) &&
#endif
	    (sem_action == ACTION_NULL_EVENT)) {
	        *pwindow = 0;
	        return (TRUE);
	}

	/*
	 * Make sure the keystroke is sent to the appropriate window.  In
	 * the X input model, keystrokes are sent to the outmost (leaf)
	 * window even it is not the focus window.  To correct this
	 * behavior, we search up the tree for a parent that has the
	 * focus and redirect the keystroke to it.
	 */
	if (!xv_has_focus(info)) {
	    int		found_focuswindow = FALSE;
	    Xv_object	dummy_window = window;

	    while (window = xv_get(window, WIN_PARENT)) {
	        DRAWABLE_INFO_MACRO(window, info);
		if (xv_has_focus(info)) {
		    found_focuswindow = TRUE;
		    event_set_window(event, window);
		    break;
		}
	    }
	    if ((!found_focuswindow)  && (sem_action == ACTION_HELP)) {
	        Inputmask	*im;

		window = dummy_window;

		for (;;) {
		    if (!window) {
		        *pwindow = 0;
		    	return (TRUE);
		    }
		    im = (Inputmask *)xv_get(window, WIN_INPUT_MASK);
		    if ((im) && (im->im_flags & ( IM_ASCII | IM_NEGASCII))) {
		        event_set_window(event, window);
		        break;
		    }
		    window = xv_get(window, WIN_PARENT);		
		}

            } else { /* to fix lost event bug */
	    	if ((!found_focuswindow)  && (ic)) {
/*
		    XGetICValue(ic, XNClientWindow, &client_window, NULL);
		    window = (Xv_object)win_data(display, client_window);
*/
		    window = dummy_window;
		}
	    }
	}
	server_set_timestamp(server_public, &event->ie_time, ek->time);
	server_set_seln_function_pending(server_public,ek->state & SELN_FNMASK);
	event_set_x(event, ek->x);
	event_set_y(event, ek->y);
	if (event_type == KeyPress)
	    event_set_down(event);
	else
	    event_set_up(event);


#ifdef OW_I18N
	key_value = (unsigned char) mb_lookup_buffer[0];
#else
	key_value = (unsigned char) buffer[0];
#endif

				/* If it's in the keyboard keysym set
				 * check to see if it maps into an XView
				 * ie_code.  (eg. KEY_LEFT(5)...)
				 */
	if (keyboard_key)
	    key_value = ((key_map[(int)ksym & 0xFF] == ksym) ||
		         (!key_map[(int)ksym & 0xFF])) ? key_value :
				                   key_map[(int)ksym & 0xFF];

				/* If more than one character was returned
				 * tell the client that there is a string
				 * in event_string().  Can use event_is_string()
				 * to check.
				 */
#ifdef OW_I18N
	/* only when buf_length>1 then put the string in ie_string, else
  	   ie_code suffices to store it */
	if ((buf_length > 1) && ((ret_status == XLookupBoth) || (ret_status == XLookupChars)))
		event->ie_string = mb_lookup_buffer;	
	else
		event->ie_string=NULL;
#else
	event_set_string(event, (buf_length > 1) ? buffer: NULL);
#endif
	event_set_id(event, key_value);
	event_set_action(event, sem_action);
	break;
      }

      case ButtonPress:
      case ButtonRelease:{
	    int              button,
	    		     action;
	    XButtonEvent     *e = (XButtonEvent *) xevent;
	    Window_info	     *win;

	    /*
	    * If the event is mouse-press and it is a synthetic
	    * event ( the event came from SendEvent request ), we should
	    * ignore it for security reasons.
	    */
	    if ( xevent->xany.send_event && !defeat_event_security ) 
		    goto Default;

	    server_set_timestamp(server_public, &event->ie_time, e->time);
	    temp = e->state;
	    server_set_seln_function_pending(server_public, e->state & SELN_FNMASK);
	    event_set_x(event, e->x);
	    event_set_y(event, e->y);

	    switch (e->button) {
	      case Button3:
		button = MS_RIGHT;
		action = ACTION_MENU;
		break;
	      case Button2:
				/* OL says on two button mice, the right button
				 * is the menu button.                        */
		if (nbuttons == 2) {  
			button = MS_RIGHT;
			action = ACTION_MENU;
			temp = (temp | Button3Mask) & ~Button2Mask;
		} else {
			button = MS_MIDDLE;
			action = ACTION_ADJUST;
		}
		break;
	      case Button1:
		if ((but2_mod & e->state)&&((nbuttons == 2)||(nbuttons == 1))) {
			button = MS_MIDDLE;
			action = ACTION_ADJUST;
			temp = (temp | Button2Mask) & ~Button1Mask;
		} else if ((but3_mod & e->state) && (nbuttons == 1)) {
			button = MS_RIGHT;
			action = ACTION_MENU;
			temp = (temp | Button3Mask) & ~Button1Mask;
		} else {
			button = MS_LEFT;
			action = ACTION_SELECT;
		}
		break;

	      default:
		goto Default;
	    }
	    SET_SHIFTS(event, temp, META_MODMASK);
	    event_set_id(event, button);
	    event_set_action(event, action);
	    if (event_type == ButtonPress) {
		event_set_down(event);
	    } else {
		event_set_up(event);
	    }

	    win = WIN_PRIVATE(event_window(event));
	    /*
	     * For click to type, if pressed SELECT, window does not appear
	     * to have input focus, window appears to be capable of accepting
	     * focus (i.e. accepts KBD_USE), and no selection function is
	     * pending, set input focus to this window.
	     */
	    if (action == ACTION_SELECT &&
		event_type == ButtonPress &&
		!xv_has_focus(info) &&
		!xv_no_focus(info) &&
		(win->xmask & FocusChangeMask) &&
		!server_get_seln_function_pending(server_public)) {
		(void) win_set_kbd_focus(window, xv_xid(info));
	    }
	    break;
	}

      case MotionNotify:{
	    XMotionEvent   *e = (XMotionEvent *) xevent;

	    server_set_timestamp(server_public, &event->ie_time, e->time);

	    event_set_x(event, e->x);
	    event_set_y(event, e->y);

	    temp = e->state;
	    SET_SHIFTS(event, temp, META_MODMASK);
	    if (event_button_is_down(event)) {
		if (nbuttons == 2) {
		    if ((but3_mod & temp) && (temp & Button1Mask))
			temp = (temp | Button2Mask) & ~Button1Mask;
		    else if (temp & Button2Mask)
			temp = (temp | Button3Mask) & ~Button2Mask;
		} else if (nbuttons == 1) {  /* assume it is button 1 */
		    if (but3_mod & temp)
			temp = (temp | Button3Mask) & ~Button1Mask;
		    else if (but2_mod & temp)
			temp = (temp | Button2Mask) & ~Button1Mask;
		}
		event_set_id(event, LOC_DRAG);
		event_set_down(event);
	    } else {
		event_set_id(event, LOC_MOVE);
		event_set_up(event);
	    }
	    SET_SHIFTS(event, temp, META_MODMASK);
	    break;
	}

      case EnterNotify:
      case LeaveNotify:{
	    XCrossingEvent *e = (XCrossingEvent *) xevent;
	    Xv_Drawable_info *info;

	    server_set_timestamp(server_public, &event->ie_time, e->time);

	    event_set_down(event);
	    temp = e->state;
	    SET_SHIFTS(event, temp, META_MODMASK);
	    server_set_seln_function_pending(server_public,
							e->state & SELN_FNMASK);
	    event_set_x(event, e->x);
	    event_set_y(event, e->y);
	    if (event_type == EnterNotify) {
		event_set_id(event, LOC_WINENTER);
		DRAWABLE_INFO_MACRO(event_window(event), info);
		pointer_window_xid = xv_xid(info);
	    } else {
		event_set_id(event, LOC_WINEXIT);
	    }
	    break;
	}

      case ConfigureNotify:{
	    Rect            temp_rect1;

	    temp_rect1.r_width = xevent->xconfigure.width;
	    temp_rect1.r_height = xevent->xconfigure.height;
	    temp_rect1.r_left = xevent->xconfigure.x;
	    temp_rect1.r_top = xevent->xconfigure.y;

	    if ((Bool) xv_get(window, WIN_TOP_LEVEL) == TRUE)
		window_update_cache_rect(window, &temp_rect1);

	    event_set_id(event, WIN_RESIZE);
	    break;
	}
	/*
	 * BUG ALERT:to this correctly need tos store XID of parent instead
	 * of Window handle (or both)
	 */
      case ReparentNotify:{
	    XReparentEvent *e = (XReparentEvent *) xevent;
	    Xv_Window       parent;

	    if (parent = win_data(display, e->parent))
		window_set_parent(window, parent);

	    event_set_id(event, WIN_REPARENT_NOTIFY);
	    event_set_x(event, e->x);
	    event_set_y(event, e->y);
	}
	break;

      case MapNotify:
	if ((Bool) xv_get(window, WIN_TOP_LEVEL) &&
	    !(Bool) xv_get(window, WIN_TOP_LEVEL_NO_DECOR))
	    event_set_action(event, ACTION_OPEN);
	else
	    event_set_id(event, WIN_MAP_NOTIFY);
	break;

      case UnmapNotify:
	if ((Bool) xv_get(window, WIN_TOP_LEVEL) &&
	    !(Bool) xv_get(window, WIN_TOP_LEVEL_NO_DECOR))
	    event_set_action(event, ACTION_CLOSE);
	else
	    event_set_id(event, WIN_UNMAP_NOTIFY);
	break;

	/*
	 * BUG ALERT: this is not exactly correct should really translate
	 * GraphicsExpose to Expose events to be exactly correct
	 */
      case GraphicsExpose:
      case Expose:{
	    XExposeEvent   *e = (XExposeEvent *) xevent;
    	    Window_info    *win = WIN_PRIVATE(window);

	    if (win_do_expose_event(display, event, e, &window,
						     win->collapse_exposures)) {
		*pwindow = 0;
		return (1);
	    }
	    if (event_type == Expose)
	    	event_set_id(event, WIN_REPAINT);
	    else
	    	event_set_id(event, WIN_GRAPHICS_EXPOSE);
	    break;
	}

      case NoExpose:
	 event_set_id(event, WIN_NO_EXPOSE);
	 break;

      case SelectionClear:
      case SelectionRequest:
      case SelectionNotify:
	(void) process_selection_events(server_public, xevent);
	*pwindow = 0;
	return (1);

      case ClientMessage:{
	    XClientMessageEvent *cme = (XClientMessageEvent *) xevent;

	    if (process_clientmessage_events(window, cme, event)) {
		*pwindow = 0;
		return (1);
	    }
	    break;
	}

      case PropertyNotify:{
	    XPropertyEvent *pne = (XPropertyEvent *) xevent;

	    if (process_property_events(window, pne, event)) {
		*pwindow = 0;
		return (1);
	    }
	    break;
	}

      case FocusIn:{
	    XFocusChangeEvent *fce = (XFocusChangeEvent *) xevent;

	    if (fce->detail == NotifyAncestor ||
		fce->detail == NotifyInferior ||
		fce->detail == NotifyNonlinear) {
		if (xv_get(xv_server(info), SERVER_JOURNALLING))
		    xv_set(xv_server(info), SERVER_JOURNAL_SYNC_EVENT, 1, 0);
		xv_set_has_focus(info, TRUE);
		event_set_id(event, KBD_USE);
	    } else {
	    /* BUG: We are dropping Notify{Virtual, NonlinearVirt, pntr, etc}
		    on the floor.					      */
		*pwindow = 0;
		return (1);
	    }
	    break;
	}

      case FocusOut:{
	    XFocusChangeEvent *fce = (XFocusChangeEvent *) xevent;

	    if (fce->detail == NotifyAncestor ||
		fce->detail == NotifyInferior ||
		fce->detail == NotifyNonlinear) {
		xv_set_has_focus(info, FALSE);
		event_set_id(event, KBD_DONE);
	    } else {
		*pwindow = 0;
		return (1);
	    }
	    break;
	}
      case KeymapNotify: 
	    event_set_id(event, KBD_MAP);
	    break;
      case VisibilityNotify:  {
	    XVisibilityEvent *e = (XVisibilityEvent *) xevent;
	    event_set_id(event, WIN_VISIBILITY_NOTIFY);
	    event_set_flags(event, e->state); /* VisibilityUnobscured,
						 VisibilityPartiallyObscured,
						 VisibilityObscured           */
	    break;
	}
      case GravityNotify:  {
	    XGravityEvent *e = (XGravityEvent *) xevent;
	    event_set_id(event, WIN_GRAVITY_NOTIFY);
	    event_set_x(event, e->x);
	    event_set_y(event, e->y);
	    break;
	}
      case CirculateNotify:  {
	    XCirculateEvent *e = (XCirculateEvent *) xevent;
	    event_set_id(event, WIN_CIRCULATE_NOTIFY);
	    event_set_flags(event, e->place);  /* PlaceOnTop or PlaceOnButton */
	    break;
	}
      case ColormapNotify:
	    event_set_id(event, WIN_COLORMAP_NOTIFY);
	    /* BUG: This needs support macros to allow user to get at the
		    colormap, new and state fields in the xevent.             */
	    break;
      case MappingNotify: {
	    XMappingEvent *e = (XMappingEvent *) xevent;
	    /* BUG: Really should ask client if he wants to update his
	     *      keymap.  Need a new attr.				      */
	    if (e->request == MappingKeyboard)
		XRefreshKeyboardMapping(e);
	    
	    *pwindow =0;
	    return(1);
	}
            /* Events a window manger (not toolkit) would be interested in.   */
      case CreateNotify:
	    event_set_id(event, WIN_CREATE_NOTIFY);
	    break;
      case DestroyNotify:
	    event_set_id(event, WIN_DESTROY_NOTIFY);
	    break;
      case MapRequest:
	    event_set_id(event, WIN_MAP_REQUEST);
	    break;
      case ResizeRequest:
	    event_set_id(event, WIN_RESIZE_REQUEST);
	    break;
      case ConfigureRequest:
	    event_set_id(event, WIN_CONFIGURE_REQUEST);
	    break;
      case CirculateRequest:
	    event_set_id(event, WIN_CIRCULATE_REQUEST);
	    break;

	/* If we get an event that is not defined in the X protocol, assume
	 * it is an event generated from a server extension.  If an
	 * extension proc has been set on this server, we will call it
	 * back with the display, extension event, and a window object that
	 * is possibly NULL.
	 */
      default: { 
	  /* I would like to cache the extensionProc, but then we run into the
	   * problem where the extensionProc is sporatically changed by the
	   * programmer. 
	   */
          void (*extensionProc)() = (void (*)()) xv_get(server_public,
							 SERVER_EXTENSION_PROC);
	  if (extensionProc)
	      (*extensionProc)(display, xevent, window); 
	}
Default:
	*pwindow = 0;
	return (1);
    }

    *pwindow = window;
    return (0);
}

/* read the input and post to the proper window */
/* ARGSUSED */
Xv_private      Notify_value
xv_input_pending(client, fd)
    Notify_client   client;
    int             fd;
{
    Event           event;
    XEvent	    xevent;
    Xv_object       window;
    int             events_handled = 0;

    do {
	window = xview_x_input_readevent((Display *) client, &event, NULL,
					 FALSE, FALSE, NULL, &xevent);
	if (window)
	    switch (event_id(&event)) {

	      case WIN_GRAPHICS_EXPOSE:
	      case WIN_REPAINT:{
		    if ((!xv_get(window, WIN_X_PAINT_WINDOW)) &&
			    (!xv_get(window, WIN_NO_CLIPPING))) {
			/*
			 * Force the clipping list for the window to be the
			 * damage list while the client is processing the
			 * WIN_REPAINT, then clear the clipping list back to
			 * its normal state.
			 */
			Rectlist       *rl;

			rl = win_get_damage(window);
			win_set_clip(window, rl);
			(void) win_post_event(window, &event, NOTIFY_SAFE);
			win_set_clip(window, RECTLIST_NULL);
		    } else {
			(void) win_post_event(window, &event, NOTIFY_SAFE);
		    }
		    win_clear_damage(window);
		    break;
		}
	      case MS_LEFT: {
	      	    Window_info    *win = WIN_PRIVATE(window);

		    /* Since we have a synchronous grab on the select button,
		     * after we set the focus, we need to release the events
		     * with XAllowEvents().
		     */
		    window_release_selectbutton(window, &event);

                    /* Do not post MS_LEFT events if the user hasn't asked for
		     * them.  XView needs them for click-to-type and we
		     * get them reguardless of whether they are in the
		     * event mask beacuse of the passive button grab. 
		     */
		    if((win->xmask & ButtonPressMask && event_is_down(&event))||
		       (win->xmask & ButtonReleaseMask && event_is_up(&event)))

		        (void) win_post_event(window, &event, NOTIFY_SAFE);

		    break;
		}
	      default:
		(void) win_post_event(window, &event, NOTIFY_SAFE);
		break;
	    }

	/* If we've handled all the events on the wire and we've not handled */
	/* more than 25 events on this go-round (as we don't want to deprive */
	/* other input src's [eg. another server]), see if any more events   */
	/* have arrived.					             */
	if ((!QLength((Display *) client)) && (events_handled < 25))
	    events_handled += XEventsQueued((Display *) client,
					    QueuedAfterReading);
    } while (QLength((Display *) client));
    return NOTIFY_DONE;
}

/*
 * This routine should be called just before going into the central
 * "select(2) and notify" loop, and also before returning to the select
 * within the central loop.  It ensures that any buffered requests are sent
 * to the server, and that any events buffered in the Xlib event queue are
 * processed before this process goes to sleep on the select.  If an event is
 * already queued up on the notifier, return without processing queued X
 * events, since the notifier will hopefully call us back to process the
 * event on its queue.
 */
Xv_private void
xv_do_enqueued_input(client, need_dispatch)
    Notify_client   client;
    int             need_dispatch;
{
    /* Process events in notifier queue first */
    if (ndis_flags & NDIS_EVENT_QUEUED)
	return;

    while (XPending((Display *) client)) {
	(void) xv_input_pending(client, win_server_fd((Display *) client));
	if (need_dispatch) {
	    (void) notify_dispatch();
	}
    }
}

#ifdef _XV_DEBUG
/*
 * this procedure prints out the contents of the xexpose event structure
 */

static
print_event(xevent, window, event_type)
    XEvent         *xevent;
    Xv_object       window;
    int             event_type;
{
    Xv_pkg         *type;

    if (event_num == 0) {
	fexp = fopen("events", "w+");
    }
    fprintf(fexp, "event number %d\n", event_num++);

    if ((type = (Xv_pkg *) xv_get(window, XV_TYPE)) == FRAME)
	fprintf(fexp, "FRAME\n");
    else if (type == WINDOW)
	fprintf(fexp, "WINDOW\n");
    else if (type == ICON)
	fprintf(fexp, "ICON\n");
    else if (type == PANEL)
	fprintf(fexp, "PANEL\n");
    else if (type == CANVAS)
	fprintf(fexp, "CANVAS\n");
    fprintf(fexp, "parent %d owner %d \n", xv_get(window, XV_OWNER), xv_get(window, WIN_PARENT));

    switch (event_type) {
      case Expose:
	fprintf(fexp, "Expose for window %d \n", window);
	fprintf(fexp, "x %d y %d width %d height %d \n ", xevent->xexpose.x, xevent->xexpose.y, xevent->xexpose.width, xevent->xexpose.height);
	fprintf(fexp, "count %d \n\n", xevent->xexpose.count);
	break;
      case ConfigureNotify:
	fprintf(fexp, "ConfigureNotify for window %d\n", window);
	fprintf(fexp, "x %d y %d width %d height %d \n\n ", xevent->xconfigure.x, xevent->xconfigure.y, xevent->xconfigure.width, xevent->xconfigure.height);
	break;
      case MotionNotify:
	fprintf(fexp, "MotionNotify for window %d\n\n", window);
	break;
      case EnterNotify:
      case LeaveNotify:
	fprintf(fexp, "Leave/EnterNOtify for window %d\n", window);
	break;
      case MapNotify:
	fprintf(fexp, "MapNotify on window %d \n\n", window);
	break;
      case UnmapNotify:
	fprintf(fexp, "UnMapNotify on window %d \n\n", window);
	break;
      case ReparentNotify:
	fprintf(fexp, "ReparentNotify on window %d \n", window);
	break;
      case FocusIn:
	fprintf(fexp, "FocusIn on window %d\n", window);
	break;
      case FocusOut:
	fprintf(fexp, "FocusOut on window %d\n", window);
	break;
      default:
	break;

    }
    fflush(fexp);
}


void
print_request(window, req_type, x, y, width, height)
    int             req_type;
    Xv_object       window;
    int             x, y, width, height;
{
    Xv_pkg         *type;

    if (event_num == 0) {
	fexp = fopen("events", "w+");
	event_num = 1;
    }
    if ((type = (Xv_pkg *) xv_get(window, XV_TYPE)) == FRAME)
	fprintf(fexp, "FRAME\n");
    else if (type == WINDOW)
	fprintf(fexp, "WINDOW\n");
    else if (type == ICON)
	fprintf(fexp, "ICON\n");
    else if (type == PANEL)
	fprintf(fexp, "PANEL\n");
    else if (type == CANVAS)
	fprintf(fexp, "CANVAS\n");
    fprintf(fexp, "parent %d owner %d \n", xv_get(window, XV_OWNER), xv_get(window, WIN_PARENT));

    switch (req_type) {
      case 1:
	fprintf(fexp, "XConfigureWindow for window %d\n", window);
	fprintf(fexp, "x %d y %d width %d height %d \n\n ", x, y, width, height);
	break;
      case 2:
	fprintf(fexp, "ROP in window %d\n", window);
	fprintf(fexp, "x %d y %d width %d height %d \n\n ", x, y, width, height);
	break;
      case 3:
	fprintf(fexp, "RELROP in window %d\n", window);
	fprintf(fexp, "x %d y %d width %d height %d \n\n ", x, y, width, height);
	break;
      default:
	break;

    }
    fflush(fexp);
}

#endif




static void
process_selection_events(server_public, xevent)
    Xv_Server       server_public;
    register XEvent *xevent;
{
    switch (xevent->type & 0177) {

      case SelectionClear:{
	    XSelectionClearEvent *clear_event = (XSelectionClearEvent *) xevent;

	    selection_agent_clear(server_public, clear_event);
	    break;
	}


      case SelectionRequest:{
	    XSelectionRequestEvent *req_event = (XSelectionRequestEvent *) xevent;

	    selection_agent_selectionrequest(server_public, req_event);
	    break;
	}

      case SelectionNotify:{
	    /*
	     * This happens only if the previous request timed out.
	     */
	    break;
	}

      default:
	break;
    }
}

get_cmdline_option(window, str, appl_cmdline)
    Xv_object		window;
    char	       *str;
    char	       *appl_cmdline;
{
    Xv_Drawable_info   *info;
    int        	        root = 0;
    int			parent, *children;
    int                *dummy, temp;
    XWindowAttributes   xwin_attr;
    Rect               *rect;
    char	        scale[30], icon_label[30], label[30], iconic[6];
    char	        font[256];
    char		foreground_color[256], background_color[256];
    char	        icon_image[256], icon_font[256];
    char	        disable_retain[8];
    char	       *tmp_str;
    Icon		icon;
    Xv_Drawable_info	*icon_info;
    int			icon_x, icon_y;

    DRAWABLE_INFO_MACRO(window, info);
    rect = (Rect *) xv_get(window, WIN_RECT);
    if (xv_get(window, XV_SHOW)) {
       temp = XQueryTree(xv_display(info), xv_xid(info), &root, &parent, 
     		      &children, &dummy);
       xv_free(children);
       if (temp) 
           XGetWindowAttributes(xv_display(info), parent, &xwin_attr);
    } else 
        XGetWindowAttributes(xv_display(info), xv_xid(info), &xwin_attr);

    rect->r_left = xwin_attr.x;
    rect->r_top = xwin_attr.y;
    
    icon = (Icon )xv_get(window, FRAME_ICON);
    DRAWABLE_INFO_MACRO(icon, icon_info);
    if (!root) 
        root = (int)xv_get(xv_root(icon_info), XV_XID);

    win_translate_xy_internal(xv_display(info), xv_xid(icon_info), root, 0, 0, 
			      &icon_x, &icon_y);

    scale[0] = '\0';
    if (defaults_exists("window.scale", "Window.Scale"))
      sprintf(scale, " -Wx %s", 
	      defaults_get_string("window.scale", "Window.Scale", (char *)NULL));
    
    label[0] = '\0';
    if (defaults_exists("window.header", "Window.Header"))
      sprintf(label, " -Wl \"%s\"",
 	      defaults_get_string("window.header", 
				  "Window.Header", (char *)NULL));
    
    iconic[0] = '\0';
    if (xv_get(window, FRAME_CLOSED))
      sprintf(iconic, " -Wi");
    else 
      sprintf(iconic, " +Wi");
    
    font[0] = '\0';
    tmp_str = defaults_get_string("font.name", "Font.Name", (char *) NULL);
    if (tmp_str)
      sprintf(font, " -Wt %s", tmp_str);

    icon_image[0] = '\0';
    tmp_str = defaults_get_string("icon.pixmap", "Icon.Pixmap", 
				  (char *) NULL);
    if (tmp_str)
      sprintf(icon_image, " -WI %s", tmp_str);

    icon_label[0] = '\0';
    if (defaults_exists("icon.footer", "Icon.Footer"))
      sprintf(icon_label, " -WL \"%s\"",
 	      defaults_get_string("icon.footer", 
				  "Icon.Footer", (char *) NULL));

    icon_font[0] = '\0';
    tmp_str = defaults_get_string("icon.font.name", 
				  "Icon.Font.Name", (char *) NULL);
    if (tmp_str)
      sprintf(icon_font, " -WT %s", tmp_str);
	
    background_color[0] = '\0';
    tmp_str = defaults_get_string("window.color.background", 
				  "Window.Color.Background",
				  (char *) NULL);
    if (tmp_str)
        sprintf(background_color, " -bg \"%s\"", tmp_str);

    foreground_color[0] = '\0';
    tmp_str = defaults_get_string("window.color.foreground", 
				  "Window.Color.Foreground",
				  (char *) NULL);
    if (tmp_str)
      sprintf(foreground_color, " -fg \"%s\"", tmp_str);

    disable_retain[0] = '\0';
    tmp_str = defaults_get_string("window.mono.disableRetained", 
				  "Window.Mono.DisabledRetained", 
				  (char *)NULL);
    if (tmp_str)
        sprintf(disable_retain, " -Wdr");
			
    /* 
     * Create the string from the current values
     */
    sprintf(str, 
	    "%s -Wp %d %d -Ws %d %d -WP %d %d%s%s%s%s%s%s%s%s%s%s ", 
	    xv_app_name, 
	    rect->r_left, rect->r_top, rect->r_width, rect->r_height,
	    icon_x, icon_y, scale,
	    label, iconic, font, icon_image, icon_label,
	    icon_font, foreground_color, background_color, 
	    disable_retain);

    /*
     * add any application specific cmdline args
     */
    if (appl_cmdline)
        strcat(str, appl_cmdline);
}

static int
process_clientmessage_events(window, clientmessage, event)
    Xv_object       window;
    XClientMessageEvent *clientmessage;
    Event          *event;
{
    Xv_Drawable_info *info;
    Xv_opaque       server_public;
    Server_atom_type atom_type;
    int             result = 0;

    DRAWABLE_INFO_MACRO(window, info);
    server_public = xv_server(info);
    atom_type = server_get_atom_type(server_public,clientmessage->message_type);

    switch (atom_type) {
      case SERVER_WM_DISMISS_TYPE:
	event_set_action(event, ACTION_DISMISS);
	break;
      case SERVER_DO_DRAG_MOVE_TYPE:
      case SERVER_DO_DRAG_COPY_TYPE:
      case SERVER_DO_DRAG_LOAD_TYPE:
	{
	    int             final_x, final_y;

	    /*
	     * the xy is in the sender's coordinate, need to translate it
	     */
	    win_translate_xy_internal(xv_display(info),
				              (XID) clientmessage->data.l[3],
				              xv_xid(info),
				              (int) clientmessage->data.l[1],
				              (int) clientmessage->data.l[2],
				                            &final_x, &final_y);
	    event_set_x(event, final_x);
	    event_set_y(event, final_y);

	    /*
	     * save off the clientmessage info into the window struct
	     */
	    window_set_client_message(window, clientmessage);

	    switch (atom_type) {
	      case SERVER_DO_DRAG_MOVE_TYPE:
		event_set_action(event, ACTION_DRAG_MOVE);
		break;
	      case SERVER_DO_DRAG_COPY_TYPE:
		event_set_action(event, ACTION_DRAG_COPY);
		break;
	      case SERVER_DO_DRAG_LOAD_TYPE:
		event_set_action(event, ACTION_DRAG_LOAD);
		break;
	    }
	}
	break;
      case SERVER_WM_PROTOCOLS_TYPE:{
	    Server_atom_type atom_type2 =
		  server_get_atom_type(server_public, clientmessage->data.l[0]);
	    switch (atom_type2) {
	      case SERVER_WM_SAVE_YOURSELF_TYPE:{
		char            str[200];
		    
		str[0] = '\0';

		xv_destroy_save_yourself(window);

 		if (xv_get(window, XV_OWNER) ==
		    xv_get(xv_screen(info), XV_ROOT)) {
		    char     *appl_cmdline;	
		    appl_cmdline = (char *) xv_get(window, WIN_CMD_LINE);

		    if ((int)appl_cmdline == -1)
    			win_change_property(window, SERVER_WM_COMMAND,
							  XA_STRING, 8, str, 0);
		    else if (xv_app_name) {
			get_cmdline_option(window, str, appl_cmdline);
			win_change_property(window, SERVER_WM_COMMAND,
					    XA_STRING, 8, str, strlen(str) + 1);
		    } else
					/* If we can't save the state, perform
					 * a zero length append on WM_COMMAND.
					 */
		        win_change_property(window, SERVER_WM_COMMAND,
							  XA_STRING, 8, str, 0);
		} else
		    win_change_property(window, SERVER_WM_COMMAND, XA_STRING,
								     8, str, 0);
		XFlush(xv_display(info));
		break;
		}
	      case SERVER_WM_DELETE_WINDOW_TYPE:
		if (xv_get(window, XV_OWNER) == xv_get(xv_screen(info),
								       XV_ROOT))
		    xv_destroy_safe(window);
		else
		    event_set_action(event, ACTION_DISMISS);
		break;
	      case SERVER_WM_TAKE_FOCUS_TYPE:
		server_set_timestamp(server_public, &event->ie_time,
						      clientmessage->data.l[1]);
		event_set_action(event, ACTION_TAKE_FOCUS);
		break;
	    }
	    break;
	}
      default:
	/*
	 * Set the id to WIN_CLIENT_MESSAGE and the content of the gets
	 * stuffed into window struct
	 */
	event_set_id(event, WIN_CLIENT_MESSAGE);
	window_set_client_message(window, clientmessage);
    }
    return (result);
}

static int
process_property_events(window, property, event)
    Xv_object       window;
    XPropertyEvent *property;
    Event          *event;
{
    Xv_Drawable_info *info;
    Xv_opaque       server_public;
    Server_atom_type atom_type;

    DRAWABLE_INFO_MACRO(window, info);
    server_public = xv_server(info);
    atom_type = server_get_atom_type(server_public, property->atom);

    switch (atom_type) {
      case SERVER_WM_PIN_STATE_TYPE:
	return (process_wm_pushpin_state(window, property->atom, event));
      default:
    	event_set_id(event, WIN_PROPERTY_NOTIFY);
    }
    return FALSE;
}


static int
process_wm_pushpin_state(window, atom, event)
    Xv_object       window;
    Atom           *atom;
    Event          *event;
{
    Xv_Drawable_info *info;
    int             status;
    Atom            type;
    int             format;
    long            nitems;
    long            bytes;
    char           *prop;
    long           *pinstate;

    DRAWABLE_INFO_MACRO(window, info);
    status = XGetWindowProperty(xv_display(info), xv_xid(info), atom,
				0, 1, False, XA_INTEGER,
				&type, &format, &nitems, &bytes, &prop);
    if (status != Success)
	return 1;

    if (!prop)
	return 1;

    if (format != 32) {
	XFree(prop);
	return 1;
    }
    pinstate = (long *) prop;
    switch (*pinstate) {
      case WMPushpinIsIn:
	event_set_action(event, ACTION_PININ);
	break;
      case WMPushpinIsOut:
	event_set_action(event, ACTION_PINOUT);
	break;
    }
    XFree(prop);
    return 0;
}


Xv_private void
win_event_to_proc_with_ptr(window_public, event_type, sender_id, x, y)
    Xv_opaque       window_public;
    Atom            event_type;
    XID             sender_id;
    int             x, y;
{
    Xv_Drawable_info *info;
    XClientMessageEvent event_struct;

    DRAWABLE_INFO_MACRO(window_public, info);
    event_struct.type = ClientMessage;
    event_struct.message_type = event_type;

    event_struct.window = XV_DUMMY_WINDOW;	/* Put anything in here, the
						 * server will not use this */
    event_struct.format = 32;
    event_struct.data.l[0] = x;
    event_struct.data.l[1] = y;
    event_struct.data.l[2] = sender_id;
    XSendEvent(xv_display(info), PointerWindow, False, NoEventMask,
	       &event_struct);
    XFlush(xv_display(info));
}
