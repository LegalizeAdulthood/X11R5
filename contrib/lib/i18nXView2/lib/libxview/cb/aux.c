#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)aux.c 70.5 91/07/25";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */



#include <xview/xv_i18n.h>
#include <xview/base.h>
#include <xview/frame.h>
#include <xview/panel.h>
#include <xview_private/panel_impl.h>
#include <X11/Xlib.h>
#include <X11/XSunExt.h>
#include <xview_private/aux.h>
#include <xview_private/xv_i18n_impl.h>

#define POP_UP
#define GET_EVENT
#define	ITERM_BUFSIZE	1024

extern ml_panel_display_interm();
extern ml_panel_saved_caret();

static	Panel_setting	aux_post_event();
static	Bool		aux_start_called = FALSE;

/* get the private handle, allocate buffer for intermediate text and
 * attach it to the panel.
 */

/*#define	HACK	/* 馬鹿もーん　誰がひよわだと？ */

#ifdef  GET_EVENT
static    XKeyEvent           e ;
#endif
void
aux_start (ic, client_data, callback_data)
    XIC		ic;
    XPointer	client_data;
    XIMAuxStartCallbackStruct *callback_data;
{
    Panel_info	*panel;
    wchar_t	*aux_label = (wchar_t     *)NULL;
    wchar_t	*aux_text = (wchar_t     *)NULL;
    AuxInfo     *info;
    Item_info	*item_private;
    int		win_x,win_y;
    int		sc_x, sc_y ;
    int         is_subframe;
    XID		xid;
    XWindowAttributes   w_attr;
    XKeyEvent   *event = (XKeyEvent *)callback_data->event;

    if( !aux_info_create(client_data) ) {
	xv_error( NULL,
		ERROR_STRING,
		XV_I18N_MSG("xv_messages", "Aux callback: out of memory"),
		0 );
    }
    info = (AuxInfo *)AUX_UDATA_INFO(client_data);

    info->ic = ic;

    info->dpy = (Display *)XDisplayOfIM( (XIM)XIMOfIC(ic) );
#ifndef	HACK
    xid = event->window;
    info->p_obj = (Xv_object)win_data(info->dpy, xid);
    if( !XGetWindowAttributes(info->dpy, event->window, &w_attr) )
        return ;
    sc_x = DisplayWidth(info->dpy, DefaultScreen(info->dpy));
    sc_y = DisplayHeight(info->dpy, DefaultScreen(info->dpy));
    win_x = event->x + w_attr.x ;
    win_y = event->y + w_attr.y;
    if(win_x < sc_x/10 ) win_x = sc_x/10 ;
    if(win_x > (sc_x*9)/10) win_x = (sc_x*9)/10 ;
    if(win_y < sc_y/10 ) win_y = sc_y/10 ;
    if(win_y > (sc_y*9)/10) win_y = (sc_y*9)/10 ;
#else
    win_x = 100;
    win_y = 100;
#endif

#ifdef POP_UP

    info->p_frame = (Frame)xv_get(info->p_obj, WIN_FRAME);

    is_subframe = (int) xv_get(xv_get(info->p_frame,WIN_OWNER),
			XV_IS_SUBTYPE_OF, FRAME_CLASS);

    if (is_subframe)
        info->p_frame = (Frame)xv_get(info->p_frame, WIN_OWNER);

    info->frame = (Frame)xv_create( info->p_frame ,      FRAME_CMD ,
                        XV_X,                   win_x,
                        XV_Y,                   win_y,
                        XV_WIDTH,                 300,
                        XV_HEIGHT,                200,
                        FRAME_CLOSED,           FALSE,
                        FRAME_SHOW_IMSTATUS,    FALSE,
                        XV_SHOW,                FALSE,
			WIN_USE_IM,		FALSE,
                        FRAME_LABEL,
                                XV_I18N_MSG("xv_messages","Auxiliary Region"),
			FRAME_CMD_POINTER_WARP,       FALSE,
                        NULL );


    if( !info->frame )
        goto err_return;

    info->panel = (Panel)xv_get( info->frame ,FRAME_CMD_PANEL,NULL);

    if( !info->panel )
        goto err_return;

#else


    info->frame = (Frame)xv_create( NULL ,	FRAME , 
			XV_X,			win_x,
			XV_Y,			win_y,
			FRAME_CLOSED,           FALSE,
			FRAME_SHOW_IMSTATUS,    FALSE,
			XV_SHOW,		FALSE,
			FRAME_LABEL,
				XV_I18N_MSG("xv_messages","Auxiliary Region"),
			NULL );

    if( !info->frame )
	goto err_return;

    info->panel = (Panel)xv_create( info->frame, 	PANEL,
			XV_WIDTH,			300,
			XV_HEIGHT,			200,
			WIN_USE_IM,			FALSE,
			NULL ); 

    if( !info->panel )
	goto err_return;
#endif

    if( callback_data->label->length > 0 ) {
    	if( callback_data->label->encoding_is_wchar )
    		aux_label = callback_data->label->string.wide_char;
    	else {
       		int	len_plus_one;
		len_plus_one = 1 + callback_data->label->length;
		aux_label = (wchar_t *)malloc( len_plus_one * sizeof(wchar_t) );
		mbstowcs(aux_label, callback_data->label->string.multi_byte, 
				len_plus_one);
   	 }
    }

    if( callback_data->text->length > 0 ) {
    	if( callback_data->text->encoding_is_wchar )
    		aux_text = callback_data->text->string.wide_char;
    	else {
        	int	len_plus_one;
		len_plus_one = 1 + callback_data->text->length;
		aux_text = (wchar_t *)malloc( len_plus_one * sizeof(wchar_t) );
		mbstowcs(aux_text, callback_data->text->string.multi_byte, 
				len_plus_one);
    	}
    }

    if( aux_label  && aux_text ) {
    	info->item = (Panel_item)xv_create( info->panel,	PANEL_TEXT,
			XV_X,				10,
			XV_Y,				10,
			PANEL_LABEL_STRING_WCS,		aux_label,
			PANEL_VALUE_WCS,		aux_text,
			PANEL_NOTIFY_LEVEL,		PANEL_ALL,
			PANEL_NOTIFY_PROC,		aux_post_event,
			XV_KEY_DATA,	AUX_KEY_DATA,	info,
			NULL );
    }
    else if( aux_label ) {
	info->item = (Panel_item)xv_create( info->panel,        PANEL_TEXT,
                        XV_X,                           10,
                        XV_Y,                           10,
                        PANEL_LABEL_STRING_WCS,         aux_label,
                        PANEL_NOTIFY_LEVEL,             PANEL_ALL,
                        PANEL_NOTIFY_PROC,              aux_post_event,
                        XV_KEY_DATA,    AUX_KEY_DATA,   info,
                        NULL );
    }
    else if ( aux_text ) {
	info->item = (Panel_item)xv_create( info->panel,        PANEL_TEXT,
                        XV_X,                           10,
                        XV_Y,                           10,
                        PANEL_VALUE_WCS,                aux_text,
                        PANEL_NOTIFY_LEVEL,             PANEL_ALL,
                        PANEL_NOTIFY_PROC,              aux_post_event,
                        XV_KEY_DATA,    AUX_KEY_DATA,   info,
                        NULL );
    }
    else {
	info->item = (Panel_item)xv_create( info->panel,        PANEL_TEXT,
                        XV_X,                           10,
                        XV_Y,                           10,
                        PANEL_NOTIFY_LEVEL,             PANEL_ALL,
                        PANEL_NOTIFY_PROC,              aux_post_event,
                        XV_KEY_DATA,    AUX_KEY_DATA,   info,
                        NULL );
    }


    if( !callback_data->label->encoding_is_wchar
		&& callback_data->label->length > 0 )
	free(aux_label);
    if( !callback_data->text->encoding_is_wchar 
		&& callback_data->text->length > 0 )
	free(aux_text);

    if( !info->item )
	goto err_return; 

    window_fit( info->panel );
    window_fit( info->frame );

    xv_set( info->frame , XV_SHOW, TRUE, NULL );
    
    panel = PANEL_PRIVATE(info->panel);
    panel->interm_text = 
	(wchar_t *) malloc(ITERM_BUFSIZE*sizeof(wchar_t));
    panel->interm_attr = 
	(XIMFeedback *) malloc(ITERM_BUFSIZE* sizeof (XIMFeedback));

    /* store the current_caret_offset */
    item_private = ITEM_PRIVATE( info->item );
    ml_panel_saved_caret(item_private);

    /*
    return XIMCB_Success;
    */

    aux_start_called = TRUE;
#ifdef  GET_EVENT
    e = *(callback_data->event);
#endif
    return;

err_return:     /* xv-object creation failure */
    xv_error( NULL,
	ERROR_STRING,
	XV_I18N_MSG("xv_messages", "Aux callback: failed to create xv_object "),
	0 );

    /*
    return XIMCB_FatalError;
    */
}


aux_info_create( client_data )
XPointer        client_data;
{
    AuxInfo     *data;
    Xv_opaque   *dum;
	 
    dum = (Xv_opaque *)client_data;
    data = (AuxInfo *)malloc( sizeof( AuxInfo ) );
    if( !data )
     	return 0;
    *dum = (Xv_opaque)data;
     	return 1;
}


void
aux_draw(ic, client_data, callback_data)
    XIC		ic;
    XPointer	client_data;
    XIMAuxDrawCallbackStruct         *callback_data;
{
    Panel_info *panel;
    XIMPreeditDrawCallbackStruct *pre_edit;    
    int 	i;
    AuxInfo    *info;
    Item_info  *item_private;

#ifdef	HACK
    if( !aux_start_called ){
	aux_start(ic, client_data, callback_data);
	return;
    }
#endif

    info = (AuxInfo *)AUX_UDATA_INFO(client_data);
    panel = PANEL_PRIVATE(info->panel);

#ifdef POP_UP

    if( xv_get(info->frame,XV_SHOW) == FALSE){
         xv_set( info->frame , XV_SHOW, TRUE, NULL);
    }

#endif

    item_private = ITEM_PRIVATE( info->item );

    pre_edit = (XIMPreeditDrawCallbackStruct *)callback_data;
	
    if (!panel->interm_text)
		/*
		return  XIMCB_FatalError;
		*/
		return;
		
    if (!pre_edit->text) {
	    int	org_len = STRLEN(panel->interm_text);

	/* Delete case;
	 *   If chg_len is the org_len, then delete entire str
	 *   Otherwise, shift up text from chg_first plus chg_len to
	 *   chg_first.
	 */
	    if ((pre_edit->chg_first == 0) && (org_len == pre_edit->chg_length))
	        panel->interm_text[0] = (wchar_t) NULL;
	    else {
	        int	start_shift_index = (pre_edit->chg_first + pre_edit->chg_length);
	        if (start_shift_index == org_len) {
	        /* Backspace case: Delete the last character */
	            *(panel->interm_text + start_shift_index - 1) = NULL;
	        } else {
	            /* copy_len need plus one for NULL termination */
	            int	copy_len = (org_len - start_shift_index) + 1;
	            BCOPY(panel->interm_text + start_shift_index, 
	              panel->interm_text + pre_edit->chg_first, copy_len);
                    bcopy(panel->interm_attr + start_shift_index, 
                      panel->interm_attr + pre_edit->chg_first, 
                          sizeof(XIMFeedback) * copy_len);
                }
	    }
    } else {
        if (pre_edit->text->string.wide_char || pre_edit->text->string.multi_byte) {
	    int	org_len = STRLEN(panel->interm_text);
    	    int	new_len = org_len + (pre_edit->text->length - pre_edit->chg_length);
    	    CHAR    *insert_text_ptr, *new_text;
    	    XIMFeedback         *insert_attr_ptr, *new_attr;
    	    int		buf_overflow = (new_len > ITERM_BUFSIZE);
	    	
    	    insert_text_ptr = new_text = MALLOC(new_len + 1);
    	    insert_attr_ptr = new_attr =  (XIMFeedback *)calloc(new_len + 1, sizeof(XIMFeedback));
                
    	    if (pre_edit->chg_first > 0 ) {                                        
    	        BCOPY(panel->interm_text, new_text, pre_edit->chg_first);
    	        insert_text_ptr += pre_edit->chg_first;
    	        bcopy(panel->interm_attr, new_attr, 
                          sizeof(XIMFeedback)* pre_edit->chg_first);              
    	       insert_attr_ptr += pre_edit->chg_first;
    	    }
          
    	
	    if( !pre_edit->text->encoding_is_wchar ) 
	 	mbstowcs(pre_edit->text->string.multi_byte, insert_text_ptr, 
                             pre_edit->text->length);
	    else
		STRNCPY(insert_text_ptr, pre_edit->text->string.wide_char, 
                            pre_edit->text->length);
	    bcopy(pre_edit->text->feedback, insert_attr_ptr, 
                      sizeof(XIMFeedback) * pre_edit->text->length );
	    insert_text_ptr += pre_edit->text->length;
	    insert_attr_ptr += pre_edit->text->length;
                /* 
                 * This case is when we need to append the old pre-edit text back to the
                 * pre-edit region
                 */
	    if (org_len > (pre_edit->chg_first + pre_edit->chg_length)) {
	        int		org_ptr_adv = (pre_edit->chg_first + 
                         		 (pre_edit->text->length - pre_edit->chg_length));
		BCOPY(panel->interm_text + org_ptr_adv, insert_text_ptr, org_ptr_adv);
		insert_text_ptr += (org_len - org_ptr_adv);
                        
		bcopy(panel->interm_text + org_ptr_adv, insert_attr_ptr,
                        sizeof(XIMFeedback)*(org_len - org_ptr_adv));
		insert_attr_ptr += (org_len - org_ptr_adv);        
            }
	    *insert_text_ptr = (CHAR)'\0';
                
	    if (buf_overflow) {
		CHAR	*temp_text = new_text;
		XIMFeedback *temp_attr = new_attr;
                    
		new_text = panel->interm_text;
		new_attr = panel->interm_attr;
                    
		panel->interm_text = temp_text;
		panel->interm_attr = temp_attr;
 	    } else {
		STRCPY(panel->interm_text, new_text);
		bcopy(new_attr, panel->interm_attr,
                        sizeof(XIMFeedback)*(STRLEN(new_text)));
 	    }        
	    free(new_text);
	    free(new_attr);    	
	} else {
	     /* Should test 
	      * if pre_edit->text->length == pre_edit->chg_length
	      */
	        bcopy(pre_edit->text->feedback, panel->interm_attr + pre_edit->chg_first,
                        sizeof(XIMFeedback) * pre_edit->text->length);
	}
    }

#ifdef VISIBLE_POS    
    panel->visible_type = pre_edit->text.visible_type;
    panel->visible_pos = pre_edit->text.visible_pos;
#endif /* VISIBLE_POS */

    ml_panel_display_interm(item_private);

    /*
    return XIMCB_Success;
    */
    
    
    return;
}

void
aux_done(ic, client_data, callback_data)
    XIC		ic;
    XPointer	client_data;
    XPointer	*callback_data;
{
    Panel_info *panel;
    AuxInfo    *info;
    Item_info  *item_private;

    info = (AuxInfo *)AUX_UDATA_INFO(client_data);
    panel = PANEL_PRIVATE(info->panel);
    item_private = ITEM_PRIVATE( info->item );
	
    free ((char *)panel->interm_text);
    panel->interm_text = NULL;
    free ((char *)panel->interm_attr);
    panel->interm_attr = NULL;

    /* saved the caret offset */
    ml_panel_saved_caret(item_private);

    xv_destroy( info->frame );
    free( info );
    
    aux_start_called = FALSE;

#ifdef POP_UP
/*
 * Can not WarpPointer. JOWN201  1/15/91
 */
#else
    XWarpPointer(e.display, 0, e.window, 0,0,0,0,e.x, e.y);
#endif

}	

static Panel_setting	aux_post_event(item, event)
    Panel_item	item;
    Event	*event;
{
    AuxInfo    *info;
#ifdef POP_UP

/*
 * Can not use Event proc. JOWN201  1/15/91 
 */

#else
    info = (AuxInfo *)xv_get(item, XV_KEY_DATA, AUX_KEY_DATA);
    win_post_event(info->p_obj, event, NOTIFY_SAFE);
#endif
    
}
