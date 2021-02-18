#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)stat_imcb.c 70.5 91/08/29";
#endif
#endif

/*
 *	(c) Copyright 1990 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifdef OW_I18N
/*
 * "stat_imcb.c" XIM Status callback interface routines.  Actual
 * routine is beside the libxvol/frame code.
 */

#include <xview/xv_i18n.h>

#include <X11/Xlib.h>
#include <X11/XSunExt.h>
#include <xview/xview.h>
#include <xview/frame.h>


void
status_start(ic, client_data, cb_data)
XIC		*ic;
XPointer	client_data;
XIMStatusDrawCallbackStruct *cb_data;
{
	/*
	 * Nothing I can do here. If the status region actually handle
	 * by the XView, I might able to dynamicaly create the new sub
	 * window, but currently not.  See the frame code to how
	 * actually initiated the status region (as window decoration).
	 */
}

#define	CB_STRING	cb_data->data.text->string


void
status_draw(ic, client_data, cb_data)
XIC		*ic;
XPointer	client_data;
XIMStatusDrawCallbackStruct *cb_data;
{
	Frame		frame;

	frame = (Frame) client_data;

	switch (cb_data->type)
	{
	case XIMTextType: {
		CHAR	*ws_status_str = NULL;

		if (cb_data->data.text->encoding_is_wchar) {
		     ws_status_str = (CHAR *)CB_STRING.wide_char;
		} else {
		    ws_status_str = (CHAR *)MALLOC(cb_data->data.text->length + 1);
		    BZERO(ws_status_str, (cb_data->data.text->length + 1));
		    mbstowcs(ws_status_str, CB_STRING.multi_byte, cb_data->data.text->length);
		}
		xv_set(frame,
		       FRAME_LEFT_IMSTATUS_WCS, ws_status_str,
		       NULL);
		if ((!cb_data->data.text->encoding_is_wchar) &&
		     ws_status_str)
		     free(ws_status_str);      
	}
	break;
	case XIMBitmapType:
		/*
		 * Ok, I do nothing for now.
		 */
		break;

	default:
		/*
		 * What is way to generate the error?
		 */
		break;
	}
}


void
status_done(ic, client_data, cb_data)
XIC		*ic;
XPointer	client_data;
XIMStatusDrawCallbackStruct *cb_data;
{
	Frame		frame;
	static wchar_t	*wcs_null = { '\0' };

	frame = (Frame) client_data;
	/*
	 * Nothing I can/need to do with this, but I may better to
	 * clear it.
	 */
	xv_set(frame,
	       FRAME_RIGHT_IMSTATUS_WCS,	wcs_null,
	       NULL);
}

#endif OW_I18N
