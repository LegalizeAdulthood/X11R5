#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)tty.c 70.10 91/08/28";
#endif
#endif

/*****************************************************************/
/* tty.c                           */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/*****************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <xview/sun.h>
#include <xview/frame.h>
#include <xview/tty.h>
#include <xview/ttysw.h>
#include <xview/textsw.h>
#include <xview/termsw.h>
#include <xview/defaults.h>
#include <xview_private/term_impl.h>
#include <xview/scrollbar.h>
#include <xview_private/charscreen.h>
#define	_NOTIFY_MIN_SYMBOLS
#include <xview/notify.h>
#undef	_NOTIFY_MIN_SYMBOLS

#ifdef	OW_I18N	
#include <xview_private/xv_i18n_impl.h>
#include <xview_private/draw_impl.h>
#include <xview/xv_i18n.h>
#include <X11/Xlib.h>
#include <X11/XSunExt.h>
#endif

#define HELP_INFO(s) XV_HELP_DATA, s,

extern char    *getenv();
extern caddr_t  textsw_checkpoint_undo();
extern caddr_t  ts_create();
extern Attr_avlist attr_find();

static int      tty_quit_on_death(), tty_handle_death();

Pkg_private Xv_Window csr_pixwin;
Pkg_private Notify_value ttysw_event();
Pkg_private void ttysw_interpose();

Pkg_private Menu ttysw_walkmenu();
Pkg_private int ttytlsw_escape();
Pkg_private int ttytlsw_string();
Pkg_private Ttysw *ttysw_init_internal();

Pkg_private int tty_folio_init();
Pkg_private Xv_opaque ttysw_folio_set();
Pkg_private Xv_opaque ttysw_folio_get();
Pkg_private int ttysw_folio_destroy();

Pkg_private int tty_view_init();
Pkg_private Xv_opaque ttysw_view_set();
Pkg_private Xv_opaque ttysw_view_get();
Pkg_private int ttysw_view_destroy();
extern int      ttysw_textsw_changed();


/*****************************************************************************/
/* Ttysw init routines for folio and  view	                             */
/*****************************************************************************/
#ifdef OW_I18N
static XVaNestedList
ttysw_make_pre_edit_cb_list(tty_public) 
     	Tty             tty_public;
{
	int		dummy;
	extern void		tty_text_start(),tty_text_draw(),tty_text_done();
	Ttysw_folio	ttysw = TTY_PRIVATE(tty_public);
	
	if (!ttysw->start_pecb_struct.callback) {
	    ttysw->start_pecb_struct.client_data =
		(XPointer)tty_public;
	    ttysw->start_pecb_struct.callback =
		(XIMProc)tty_text_start;
	}
	if (!ttysw->draw_pecb_struct.callback) {
	    ttysw->draw_pecb_struct.client_data =
		(XPointer)tty_public;
	    ttysw->draw_pecb_struct.callback =
		(XIMProc)tty_text_draw;
	}
	if (!ttysw->done_pecb_struct.callback) {
	    ttysw->done_pecb_struct.client_data =
		(XPointer)tty_public;
	    ttysw->done_pecb_struct.callback =
		(XIMProc)tty_text_done;
	}
	return(XVaCreateNestedList(dummy,
			XNPreeditStartCallback,	&ttysw->start_pecb_struct,
			XNPreeditDrawCallback,	&ttysw->draw_pecb_struct,
			XNPreeditDoneCallback,	&ttysw->done_pecb_struct,
			NULL));

}

static XVaNestedList
ttysw_make_status_cb_list(tty_public) 
     	Tty             tty_public;
{	
	Ttysw_folio	ttysw = TTY_PRIVATE(tty_public);
	int		dummy;
	Frame		frame_public = (Frame) xv_get(tty_public, WIN_FRAME);
	extern void     status_start(), status_draw(), status_done();
	
	if (!ttysw->start_stcb_struct.callback) {
	    ttysw->start_stcb_struct.client_data =
		(XPointer)frame_public;
	    ttysw->start_stcb_struct.callback =
		(XIMProc)status_start;
	}
	
	if (!ttysw->draw_stcb_struct.callback) {
	    ttysw->draw_stcb_struct.client_data =
		(XPointer)frame_public;
	    ttysw->draw_stcb_struct.callback =
		(XIMProc)status_draw;
	}
	if (!ttysw->done_stcb_struct.callback) {
	    ttysw->done_stcb_struct.client_data =
		(XPointer)frame_public;
	    ttysw->done_stcb_struct.callback =
		(XIMProc)status_done;
	}
	return(XVaCreateNestedList(dummy,
			XNStatusStartCallback, &ttysw->start_stcb_struct,
			XNStatusDrawCallback, &ttysw->draw_stcb_struct,
			XNStatusDoneCallback, &ttysw->done_stcb_struct,
			NULL));
	
}

static XVaNestedList
ttysw_make_lookup_cb_list(tty_public) 
     	Tty             tty_public;
{	
	Ttysw_folio	ttysw = TTY_PRIVATE(tty_public);
	int		dummy;
	Frame		frame_public = (Frame) xv_get(tty_public, WIN_FRAME);
	XPointer	luc_clientdata = (XPointer)malloc( sizeof(int *));
	extern Window   	lookup_choices_start();
        extern void     	lookup_choices_draw(),lookup_choices_done();
        extern int      	lookup_choices_process();
	
	if (!ttysw->start_luc_struct.callback) {
	ttysw->start_luc_struct.client_data =
	    (XPointer)luc_clientdata;
	ttysw->start_luc_struct.callback =
	    (XIMProc)lookup_choices_start;
    	}
	if (!ttysw->draw_luc_struct.callback) {
	    ttysw->draw_luc_struct.client_data =
		(XPointer)luc_clientdata;
	    ttysw->draw_luc_struct.callback =
		(XIMProc)lookup_choices_draw;
	}
	if (!ttysw->process_luc_struct.callback) {
	    ttysw->process_luc_struct.client_data =
		(XPointer)luc_clientdata;
	    ttysw->process_luc_struct.callback =
		(XIMProc)lookup_choices_process;
	}
	if (!ttysw->done_luc_struct.callback) {
	    ttysw->done_luc_struct.client_data =
		(XPointer)luc_clientdata;
	    ttysw->done_luc_struct.callback =
		(XIMProc)lookup_choices_done;
	}
	return(XVaCreateNestedList(dummy,
			XNExtXimp_LookupStartCallback, &ttysw->start_luc_struct,
			XNExtXimp_LookupDrawCallback,  &ttysw->draw_luc_struct,
			XNExtXimp_LookupProcessCallback, &ttysw->process_luc_struct,
			XNExtXimp_LookupDoneCallback,  &ttysw->done_luc_struct,
			NULL));

}
static XVaNestedList
ttysw_make_aux_cb_list(tty_public) 
     	Tty             tty_public;
{	
	Ttysw_folio	ttysw = TTY_PRIVATE(tty_public);
	int		dummy;
	Frame		frame_public = (Frame) xv_get(tty_public, WIN_FRAME);
	XPointer	aux_clientdata = (XPointer)malloc( sizeof(int *));
	extern void	aux_start(),aux_draw(),aux_done();
	
	if (!ttysw->start_aux_struct.callback) {
	    ttysw->start_aux_struct.client_data =
		(XPointer)aux_clientdata;
	    ttysw->start_aux_struct.callback =
		(XIMProc)aux_start;
	}
	if (!ttysw->draw_aux_struct.callback) {
	    ttysw->draw_aux_struct.client_data =
		(XPointer)aux_clientdata;
	    ttysw->draw_aux_struct.callback =
		(XIMProc)aux_draw;
	}
	if (!ttysw->done_aux_struct.callback) {
	    ttysw->done_aux_struct.client_data =
		(XPointer)aux_clientdata;
	    ttysw->done_aux_struct.callback =
		(XIMProc)aux_done;
	}
	return(XVaCreateNestedList(dummy,
			XNExtXimp_AuxStartCallback, &ttysw->start_aux_struct,
                        XNExtXimp_AuxDrawCallback, &ttysw->draw_aux_struct,
                        XNExtXimp_AuxDoneCallback, &ttysw->done_aux_struct,
			NULL));

}
#endif /* OW_I18N */
Pkg_private int
tty_folio_init(parent, tty_public, avlist)
    Xv_Window       parent;
    Tty             tty_public;
    Attr_avlist     avlist;
{
    Attr_avlist     attrs;
    Xv_tty         *tty_object = (Xv_tty *) tty_public;
    Ttysw_folio     ttysw;	/* Private object data */
#ifdef	OW_I18N    
    XIM			im;
    Xv_object		serverobj;
    Window		window;
#endif  OW_I18N

    ttysw = (Ttysw_folio) (ttysw_init_folio_internal(tty_public));
    if (!ttysw)
	return (XV_ERROR);

#ifdef OW_I18N
    ttysw->start_pecb_struct.callback = NULL;
    ttysw->start_pecb_struct.client_data = NULL;
    ttysw->draw_pecb_struct.callback = NULL;
    ttysw->draw_pecb_struct.client_data = NULL;
    ttysw->done_pecb_struct.callback = NULL;
    ttysw->done_pecb_struct.client_data = NULL;
 
    ttysw->start_stcb_struct.callback = NULL;
    ttysw->start_stcb_struct.client_data = NULL;
    ttysw->draw_stcb_struct.callback = NULL;
    ttysw->draw_stcb_struct.client_data = NULL;
    ttysw->done_stcb_struct.callback = NULL;
    ttysw->done_stcb_struct.client_data = NULL;
 
    ttysw->start_luc_struct.callback = NULL;
    ttysw->start_luc_struct.client_data = NULL;
    ttysw->draw_luc_struct.callback = NULL;
    ttysw->draw_luc_struct.client_data = NULL;
    ttysw->done_luc_struct.callback = NULL;
    ttysw->done_luc_struct.client_data = NULL;
    ttysw->process_luc_struct.callback = NULL;
    ttysw->process_luc_struct.client_data  = NULL;
 
    ttysw->start_aux_struct.callback = NULL;
    ttysw->start_aux_struct.client_data = NULL;
    ttysw->draw_aux_struct.callback = NULL;
    ttysw->draw_aux_struct.client_data = NULL;
    ttysw->done_aux_struct.callback = NULL;
    ttysw->done_aux_struct.client_data = NULL;
/*
    ttysw->process_aux_struct.callback = NULL;
    ttysw->process_aux_struct.client_data  = NULL;
*/

   for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
        switch (attrs[0]) {
 
            case WIN_IM_PREEDIT_START:
                ttysw->start_pecb_struct.callback =
                        (XIMProc)attrs[1];
                ttysw->start_pecb_struct.client_data =
                        (XPointer)attrs[2];
                break;
 
            case WIN_IM_PREEDIT_DRAW:
                ttysw->draw_pecb_struct.callback =
                        (XIMProc)attrs[1];
                ttysw->draw_pecb_struct.client_data =
                        (XPointer)attrs[2];
                break;
 
            case WIN_IM_PREEDIT_DONE:
                ttysw->done_pecb_struct.callback =
                        (XIMProc)attrs[1];
                ttysw->done_pecb_struct.client_data =
                        (XPointer)attrs[2];
                break;
 
            case WIN_IM_STATUS_START:
                ttysw->start_stcb_struct.callback =
                        (XIMProc)attrs[1];
                ttysw->start_stcb_struct.client_data =
                        (XPointer)attrs[2];
                break;
 
            case WIN_IM_STATUS_DRAW:
                ttysw->draw_stcb_struct.callback =
                        (XIMProc)attrs[1];
                ttysw->draw_stcb_struct.client_data =
                        (XPointer)attrs[2];
                break;
 
            case WIN_IM_STATUS_DONE:
                ttysw->done_stcb_struct.callback =
                        (XIMProc)attrs[1];
                ttysw->done_stcb_struct.client_data =
                        (XPointer)attrs[2];
                break;

            case WIN_IM_LUC_START:
                ttysw->start_luc_struct.callback =
                        (XIMProc)attrs[1];
                ttysw->start_luc_struct.client_data =
                        (XPointer)attrs[2];
                break;

            case WIN_IM_LUC_DRAW:
                ttysw->draw_luc_struct.callback =
                        (XIMProc)attrs[1];
                ttysw->draw_luc_struct.client_data =
                        (XPointer)attrs[2];
                break;

            case WIN_IM_LUC_DONE:
                ttysw->done_luc_struct.callback =
                        (XIMProc)attrs[1];
                ttysw->done_luc_struct.client_data =
                        (XPointer)attrs[2];
                break;

            case WIN_IM_LUC_PROCESS:
                ttysw->process_luc_struct.callback =
                        (XIMProc)attrs[1];
                ttysw->process_luc_struct.client_data =
                        (XPointer)attrs[2];
                break;

/*
	    case WIN_IM_AUX_START:
                ttysw->done_aux_struct.callback =
                        (XIMProc)attrs[1];
                ttysw->done_aux_struct.client_data =
                        (XPointer)attrs[2];
                break;

	    case WIN_IM_AUX_DRAW:
                ttysw->done_aux_struct.callback =
                        (XIMProc)attrs[1];
                ttysw->done_aux_struct.client_data =
                        (XPointer)attrs[2];
                break;

	    case WIN_IM_AUX_DONE:
                ttysw->done_aux_struct.callback =
                        (XIMProc)attrs[1];
                ttysw->done_aux_struct.client_data =
                        (XPointer)attrs[2];
                break;
*/

            default:
                break;

        }              
    }
    ttysw->ic = NULL;
	if( xv_get(tty_public, WIN_USE_IM)){
	    XIMStyles   *style = NULL;
    	    XVaNestedList   pre_edit_list, status_list,
		    		lookup_list, aux_list;
#define	 XV_SUPPORTED_STYLE_COUNT		7		    	    		    	
	    XIMStyle	    supported_styles[XV_SUPPORTED_STYLE_COUNT];
	    int		    i, j;    	    
    	    Xv_Drawable_info 	*info;


	    /* create input context for the window */

	    serverobj = XV_SERVER_FROM_WINDOW(tty_public);
	    /* Get the im form serverobj in order to pass to XCreateIC */
	    im = (XIM)xv_get(serverobj, XV_IM);
	    if (!im)
	        goto NoIM;
    	    XGetIMValues(im, XNQueryInputStyle, &style, NULL);
		
	    if (!style) 
	        goto NoIM;
	    
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

    	    DRAWABLE_INFO_MACRO(tty_public, info);

	    if ((supported_styles[j] & XIMPreeditCallbacks) &&
		(supported_styles[j] & XIMStatusCallbacks) &&
		(supported_styles[j] & XIMLookupCallbacks) &&
		(supported_styles[j] & XIMAuxCallbacks)) {
		pre_edit_list = ttysw_make_pre_edit_cb_list(tty_public);
	        status_list = ttysw_make_status_cb_list(tty_public);
	        lookup_list = ttysw_make_lookup_cb_list(tty_public);
	        aux_list = ttysw_make_aux_cb_list(tty_public);
		ttysw->ic = (XIC)XCreateIC(im, 
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
		pre_edit_list = ttysw_make_pre_edit_cb_list(tty_public);
	        status_list = ttysw_make_status_cb_list(tty_public);
	        lookup_list = ttysw_make_lookup_cb_list(tty_public);
		
		ttysw->ic = (XIC)XCreateIC(im, 
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
		pre_edit_list = ttysw_make_pre_edit_cb_list(tty_public);
	        status_list = ttysw_make_status_cb_list(tty_public);
	        aux_list = ttysw_make_aux_cb_list(tty_public);
	        
		ttysw->ic = (XIC)XCreateIC(im, 
		    XNClientWindow,	xv_xid(info),
		    XNInputStyle, supported_styles[j],            		
		    XNPreeditAttributes,	pre_edit_list,
		    XNStatusAttributes,	status_list,
		    XNExtXimp_AuxAttributes,	aux_list,
		    XNExtXimp_Backfront, IMServBackend,
		    NULL);		
	     } else if ((supported_styles[j] & XIMPreeditCallbacks) &&
		(supported_styles[j] & XIMStatusCallbacks)) {
		pre_edit_list = ttysw_make_pre_edit_cb_list(tty_public);
	        status_list = ttysw_make_status_cb_list(tty_public);
		
		ttysw->ic = (XIC)XCreateIC(im, 
		    XNClientWindow,	xv_xid(info),
		    XNInputStyle, supported_styles[j],            		
		    XNPreeditAttributes,	pre_edit_list,
		    XNStatusAttributes,	status_list,
		    XNExtXimp_Backfront, IMServBackend,
		    NULL);           	    
	    }  else if (supported_styles[j] & XIMPreeditCallbacks) {
	    	pre_edit_list = ttysw_make_pre_edit_cb_list(tty_public);
	    	
	    	ttysw->ic = (XIC)XCreateIC(im, 
		    XNClientWindow,	xv_xid(info),
		    XNInputStyle, supported_styles[j],            		
		    XNPreeditAttributes,	pre_edit_list,
		    XNExtXimp_Backfront, IMServBackend,
		    NULL);
	    }  else if ((supported_styles[j] & XIMPreeditNothing) &&
	      		(supported_styles[j] & XIMStatusCallbacks)) {
	        status_list = ttysw_make_status_cb_list(tty_public);
	    	
	    	ttysw->ic = (XIC)XCreateIC(im, 
		    XNClientWindow,	xv_xid(info),
		    XNInputStyle, supported_styles[j],            		
		    XNStatusAttributes,	status_list,
		    XNExtXimp_Backfront, IMServBackend,
		    NULL);
	      		
	    }  else if ((supported_styles[j] & XIMPreeditNothing) &&
	      		(supported_styles[j] & XIMStatusNothing)) {
		 ttysw->ic = (XIC)XCreateIC(im, 
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
	   
	    if (ttysw->ic) {			
    	        if( !ttysw->im_store ) {
	 	    ttysw->im_store = (CHAR *)malloc( ITERM_BUFSIZE * sizeof(CHAR) );
		    ttysw->im_store[0] = NULL;
	         }

    	         if( !ttysw->im_attr )
        	      ttysw->im_attr  = (XIMFeedback *)malloc( ITERM_BUFSIZE * sizeof( XIMFeedback) );
             }					
	}
NoIM:
#endif OW_I18N

    ttysw->hdrstate = HS_BEGIN;
    ttysw->ttysw_stringop = ttytlsw_string;
    ttysw->ttysw_escapeop = ttytlsw_escape;
    (void) xv_set(tty_public,
		  WIN_MENU, ttysw_walkmenu(tty_public),
		  0);
    ttysw_interpose(ttysw);
    return (XV_OK);
}

Pkg_private int
tty_view_init(parent, tty_view_public, avlist)
    Xv_Window       parent;	/* Tty public folio */
    Tty_view        tty_view_public;
    Tty_attribute   avlist[];
{
    Xv_tty_view    *tty_object = (Xv_tty_view *) tty_view_public;
    Ttysw_view_handle ttysw_view;	/* Private object data */

    /*
     * BUG ALERT!  Re-arrange code to pass this pixwin into the appropriate
     * layer instead of just smashing it set from here!
     */
    csr_pixwin = tty_view_public;


    ttysw_view = (Ttysw_view_handle) (ttysw_init_view_internal(parent, tty_view_public));

    if (!ttysw_view)
	return (XV_ERROR);


    /* ttysw_walkmenu() can only be called after public self linked to */
    (void) xv_set(tty_view_public,
		  WIN_NOTIFY_SAFE_EVENT_PROC, ttysw_event,
		  WIN_NOTIFY_IMMEDIATE_EVENT_PROC, ttysw_event,
#ifdef OW_I18N		  	/* Associate IC with the view window */
		  WIN_IC,	ttysw_view->folio->ic,
#endif		  
		  0);

    /* ttysw_interpose(ttysw_view); */

    /* Draw cursor on the screen and retained portion */
    (void) ttysw_drawCursor(0, 0);
    return (XV_OK);
}




/***************************************************************************
ttysw_set_internal
*****************************************************************************/
static          Xv_opaque
ttysw_set_internal(tty_public, avlist)
    Tty             tty_public;
    Attr_attribute  avlist[];
{
    Ttysw_folio     ttysw = TTY_PRIVATE_FROM_ANY_PUBLIC(tty_public);
    register Attr_avlist attrs;
    static int      quit_tool;
    int             pid = -1, bold_style = -1, argv_set = 0;
    char          **argv = 0;
    int             do_fork = FALSE;
    char           *buf;
    int            *buf_used;
    int             buf_len;

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch (attrs[0]) {

	  case TTY_ARGV:
	    do_fork = TRUE;
	    argv_set = 1;
	    argv = (char **) attrs[1];
	    break;

	  case TTY_CONSOLE:
	    if (attrs[1]) {
#ifdef sun			/* Vaxen do not support the TIOCCONS ioctl */
		if ((ioctl(ttysw->ttysw_tty, TIOCCONS, 0)) == -1)
		    perror("ttysw-TIOCCONS");
#endif
	    };
	    break;

	  case TTY_INPUT:
	    buf = (char *) attrs[1];
	    buf_len = (int) attrs[2];
	    buf_used = (int *) attrs[3];
	    *buf_used = ttysw_input_it(ttysw, buf, buf_len);
	    break;

	  case TTY_PAGE_MODE:
	    (void) ttysw_setopt(TTY_VIEW_HANDLE_FROM_TTY_FOLIO(ttysw), TTYOPT_PAGEMODE, (int)
				(attrs[1]));
	    break;

	  case TTY_QUIT_ON_CHILD_DEATH:
	    quit_tool = (int) attrs[1];
	    break;

	  case TTY_BOLDSTYLE:
	    (void) ttysw_setboldstyle((int) attrs[1]);
	    break;

	  case TTY_BOLDSTYLE_NAME:
	    bold_style = ttysw_lookup_boldstyle((char *) attrs[1]);
	    if (bold_style == -1)
		(void) ttysw_print_bold_options();
	    else
		(void) ttysw_setboldstyle(bold_style);
	    break;

	  case TTY_INVERSE_MODE:
	    (void) ttysw_set_inverse_mode((int) attrs[1]);
	    break;

	  case TTY_PID:
	    do_fork = TRUE;
	    /* TEXTSW_INFINITY ==> no child process, 0 ==> we want one */
	    /* BUG ALERT: need validity check on (int)attrs[1]. */
	    ttysw->ttysw_pidchild = (int) attrs[1];
	    break;

	  case TTY_UNDERLINE_MODE:
	    (void) ttysw_set_underline_mode((int) attrs[1]);
	    break;

	  case WIN_FONT:
	    {

#ifdef OW_I18N
		if (attrs[1] && csr_pixwin) {
#else
		if (attrs[1]) {
#endif
		    /*
		     * Cursor for the original font has been drawn, so take
		     * down
		     */
		    ttysw_removeCursor();
		    (void) xv_new_tty_chr_font(attrs[1]);
		    /* after changing font size, cursor needs to be re-drawn */
		    (void) ttysw_drawCursor(0, 0);
		}
		break;
	    }

	  case XV_END_CREATE:
	    /*
	     * xv_create(0, TTY, 0) should fork a default shell, but
	     * xv_create(0, TTY, TTY_ARGV, TTY_ARGV_DO_NOT_FORK, 0) should
	     * not fork anything (ttysw_pidchild will == TEXTSW_INFINITY >
	     * 0).
	     */
	    if (!do_fork && ttysw->ttysw_pidchild <= 0)
		do_fork = TRUE;
	    if (ttysw->view)
		ttysw_resize(ttysw->view);
	break;
	  default:
	    (void) xv_check_bad_attr(TTY, attrs[0]);
	    break;
	}
    }

    /*
     * WARNING. For certain sequences of calls, the following code loses
     * track of the process id of the current child, and could be tricked
     * into having multiple children executing at once.
     */
    if ((int) argv == TTY_ARGV_DO_NOT_FORK) {
	ttysw->ttysw_pidchild = TEXTSW_INFINITY;
    } else {
	if (argv_set && ttysw->ttysw_pidchild == TEXTSW_INFINITY) {
	    ttysw->ttysw_pidchild = 0;
	}
	if (ttysw->ttysw_pidchild <= 0 && do_fork) {
	    pid = ttysw_fork_it((char *) (ttysw), argv ? argv : (char **) &argv,
				0);
	    if (pid > 0) {
		(void) notify_set_wait3_func((Notify_client) ttysw,
				(Notify_func) (quit_tool ? tty_quit_on_death
					       : tty_handle_death),
					     pid);
	    }
	}
    }

    return (XV_OK);
}

Pkg_private     Xv_opaque
ttysw_folio_set(ttysw_folio_public, avlist)
    Tty             ttysw_folio_public;
    Tty_attribute   avlist[];
{
    return (ttysw_set_internal(ttysw_folio_public, avlist));

}

Pkg_private     Xv_opaque
ttysw_view_set(ttysw_view_public, avlist)
    Tty_view        ttysw_view_public;
    Tty_attribute   avlist[];
{
    return (ttysw_set_internal(ttysw_view_public, avlist));

}




/*****************************************************************************/
/* ttysw_get_internal        				                     */
/*****************************************************************************/
static          Xv_opaque
ttysw_get_internal(tty_public, status, attr, args)
    Tty             tty_public;
    int            *status;
    Tty_attribute   attr;
    va_list         args;
{
    Ttysw_folio     ttysw = TTY_PRIVATE_FROM_ANY_PUBLIC(tty_public);

    switch (attr) {
      case OPENWIN_VIEW_CLASS:
	return ((Xv_opaque) TTY_VIEW);

      case TTY_PAGE_MODE:
	return (Xv_opaque) ttysw_getopt((char *) (ttysw), TTYOPT_PAGEMODE);

      case TTY_QUIT_ON_CHILD_DEATH:
	return (Xv_opaque) 0;

      case TTY_PID:
	return (Xv_opaque) ttysw->ttysw_pidchild;

      case TTY_PTY_FD:
	return (Xv_opaque) ttysw->ttysw_pty;

      case TTY_TTY_FD:
	return (Xv_opaque) ttysw->ttysw_tty;

      case WIN_TYPE:		/* SunView1.X compatibility */
	return (Xv_opaque) TTY_TYPE;

      default:
	if (xv_check_bad_attr(TTY, attr) == XV_ERROR) {
	    *status = XV_ERROR;
	}
	return ((Xv_opaque) 0);
    }
}


Pkg_private     Xv_opaque
ttysw_folio_get(ttysw_folio_public, status, attr, args)
    Tty             ttysw_folio_public;
    int            *status;
    Tty_attribute   attr;
    va_list         args;
{
    return (ttysw_get_internal(ttysw_folio_public, status, attr, args));

}

Pkg_private     Xv_opaque
ttysw_view_get(ttysw_view_public, status, attr, args)
    Tty_view        ttysw_view_public;
    int            *status;
    Tty_attribute   attr;
    va_list         args;
{
    return (ttysw_get_internal(ttysw_view_public, status, attr, args));

}


/* ARGSUSED */
static
tty_quit_on_death(client, pid, status, rusage)
    caddr_t         client;
    int             pid;
#ifndef SVR4
    union wait     *status;
#else SVR4
    int     *status;
#endif SVR4
    struct rusage  *rusage;
{
    Ttysw_folio     ttysw = (Ttysw_folio) client;
    Tty             tty_public = TTY_PUBLIC(ttysw);
    Xv_object       frame;

    if (!(WIFSTOPPED(*status))) {
	if (WTERMSIG(*status) || WEXITSTATUS(*status) || WCOREDUMP(*status)) {
#ifdef OW_I18N
	    (void) fprintf(stderr,
		       XV_I18N_MSG("xv_messages", "A %s window has exited because its child exited.\n"),
			   TTY_IS_TERMSW(ttysw) ? "command" : "tty");
	    (void) fprintf(stderr, XV_I18N_MSG("xv_messages", "Its child's process id was %d and it"), pid);
	    if (WTERMSIG(*status)) {
		(void) fprintf(stderr, XV_I18N_MSG("xv_messages", " died due to signal %d"),
			       WTERMSIG(*status));
	    } else if (WEXITSTATUS(*status)) {
		(void) fprintf(stderr, XV_I18N_MSG("xv_messages", " exited with return code %d"),
			       WEXITSTATUS(*status));
	    }
	    if (WCOREDUMP(*status)) {
		(void) fprintf(stderr, XV_I18N_MSG("xv_messages", " and left a core dump.\n"));
#else
	    (void) fprintf(stderr,
		       "A %s window has exited because its child exited.\n",
			   TTY_IS_TERMSW(ttysw) ? "command" : "tty");
	    (void) fprintf(stderr, "Its child's process id was %d and it", pid);
	    if (WTERMSIG(*status)) {
		(void) fprintf(stderr, " died due to signal %d",
			       WTERMSIG(*status));
	    } else if (WEXITSTATUS(*status)) {
		(void) fprintf(stderr, " exited with return code %d",
			       WEXITSTATUS(*status));
	    }
	    if (WCOREDUMP(*status)) {
		(void) fprintf(stderr, " and left a core dump.\n");
#endif OW_I18N
	    } else {
		(void) fprintf(stderr, ".\n");
	    }
	}
	frame = xv_get(tty_public, WIN_FRAME);
	(void) xv_set(frame, FRAME_NO_CONFIRM, TRUE, 0);
	xv_destroy(frame);

    }
}

/* ARGSUSED */
static
tty_handle_death(tty_folio_private, pid, status, rusage)
    Ttysw_folio     tty_folio_private;
    int             pid;
#ifndef SVR4
    union wait     *status;
#else SVR4
    int     *status;
#endif SVR4
    struct rusage  *rusage;
{
    if (!(WIFSTOPPED(*status))) {
	tty_folio_private->ttysw_pidchild = 0;
    }
}


Pkg_private int
ttysw_view_destroy(ttysw_view_public, status)
    Tty_view        ttysw_view_public;
    Destroy_status  status;
{
    Ttysw_view_handle ttysw_view_private =
    TTY_VIEW_PRIVATE_FROM_ANY_VIEW(ttysw_view_public);


    if ((status != DESTROY_CHECKING) && (status != DESTROY_SAVE_YOURSELF)) {
	free((char *) ttysw_view_private);
    }
    return (XV_OK);
}

Pkg_private int
ttysw_folio_destroy(ttysw_folio_public, status)
    Ttysw_folio     ttysw_folio_public;
    Destroy_status  status;
{
    return (ttysw_destroy(ttysw_folio_public, status));
}

