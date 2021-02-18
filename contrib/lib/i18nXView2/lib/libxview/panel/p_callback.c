#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_callback.c 70.11 91/08/29";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifdef	OW_I18N

#include <xview/xv_i18n.h>
#include <xview/base.h>
#include <xview_private/panel_impl.h>
#include <X11/Xlib.h>
#include <X11/XSunExt.h>


extern ml_panel_display_interm();
extern ml_panel_saved_caret();

/* get the private handle, allocate buffer for intermediate text and
 * attach it to the panel.
 */

int
panel_text_start (ic, client_data, callback_data)
XIC		ic;
XPointer	client_data;
XPointer	callback_data;
{
	Panel		panel_public;
	Panel_info	*panel;

	/* Get the panel handle from xim */
	panel_public = (Panel)client_data;
	panel = PANEL_PRIVATE(panel_public);

	/* Ignore all request if there's nothing with keyboard
	 * focus, the if the item with the focus is 
	 * not panel text item
	 */

	if ((!panel->kbd_focus_item) || 
	    (panel->kbd_focus_item->item_type != PANEL_TEXT_ITEM))
		return(0);

	/* store the current_caret_offset */
	ml_panel_saved_caret(panel->kbd_focus_item);
        if (!panel->interm_text) {
            panel->interm_text = (CHAR *) MALLOC(ITERM_BUFSIZE);
            panel->interm_text[0] = NULL;
        }
        if (!panel->interm_attr) {
            panel->interm_attr =  (XIMFeedback *) malloc(ITERM_BUFSIZE* sizeof (XIMFeedback));
        }   
	
	return(ITERM_BUFSIZE);

}

		

void
panel_text_draw(ic, client_data, callback_data)
XIC	ic;
XPointer	client_data;
XPointer	callback_data;

{
	Panel	panel_public;
	Panel_info *panel;
	XIMPreeditDrawCallbackStruct *pre_edit;
	int 	i;


	
	/* Get the panel handle from xim */
	panel_public = (Panel)client_data;
	panel = PANEL_PRIVATE(panel_public);

	/* Get the pre_edit text from xim */
	pre_edit = (XIMPreeditDrawCallbackStruct *)callback_data;
		
	/* Ignore all request if panel's intermediate buffer is
	 * not being assigned yet, or if there's nothing with
	 * keyboard focus, and if the item with the focus is
	 * not a panel text item
	 */

	if (!panel->kbd_focus_item)
		return;

	if (!panel->interm_text || 
		(panel->kbd_focus_item->item_type != PANEL_TEXT_ITEM))
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
                    mbstowcs(insert_text_ptr, pre_edit->text->string.multi_byte, 
                             pre_edit->text->length);
                else
                    STRNCPY(insert_text_ptr, pre_edit->text->string.wide_char, 
                            pre_edit->text->length);
                if (pre_edit->text->feedback)           
                    bcopy(pre_edit->text->feedback, insert_attr_ptr, 
                          sizeof(XIMFeedback) * pre_edit->text->length );
                else
                    bzero(insert_attr_ptr, 
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
	        if (pre_edit->text->feedback)
	            bcopy(pre_edit->text->feedback, panel->interm_attr + pre_edit->chg_first,
                        sizeof(XIMFeedback) * pre_edit->text->length);
                else
                     bzero(panel->interm_attr + pre_edit->chg_first,
                        sizeof(XIMFeedback) * pre_edit->text->length);

	    }  
	}

#ifdef VISIBLE_POS
	/* For debugging XIMVisiblePosEnd */
	/*
	panel->visible_type = XIMVisiblePosEnd;
	panel->visible_pos = pre_edit->text.length;
	*/
	panel->visible_type = pre_edit->text.visible_type;
	panel->visible_pos = pre_edit->text.visible_pos;
#endif /* VISIBLE_POS */

	ml_panel_display_interm(panel->kbd_focus_item);


}

void
panel_text_done(ic, client_data, callback_data)
XIC	ic;
XPointer	client_data;
XPointer	callback_data;

{
	Panel		panel_public;
	Panel_info	*panel;
	
	/* Get the panel handle from xim */
	panel_public = (Panel)client_data;
	panel = PANEL_PRIVATE(panel_public);

	/* Ignore all request if there's no item with keyboard
	 * focus
	 */

	if (!panel->kbd_focus_item)
		return;

	/* Ignore all request if panel's intermediate buffer is */
	/* not being assigned yet, or if this is not a panel    */
	/* text item.						*/
	if ((!panel->interm_text) ||
		(panel->kbd_focus_item->item_type != PANEL_TEXT_ITEM))
		return; 
	/* saved the caret offset */
	ml_panel_saved_caret(panel->kbd_focus_item);
}	

/*
void
panel_text_caret(ic, direction, udata)
XIC	ic;
IMTextDirection direction;
caddr_t		udata;
{
	interprete direction and calculate row and col;
	set cursor position
}
	
*/

#endif /*OW_I18N*/
