#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fmbs_set.c 70.2 91/07/03";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <X11/Xlib.h>
#include <xview_private/fm_impl.h>
#include <xview_private/frame_base.h>
#include <xview_private/draw_impl.h>
#include <xview_private/wmgr_decor.h>
#include <xview/server.h>

Pkg_private     Xv_opaque
frame_base_set_avlist(frame_public, avlist)
    Frame           frame_public;
    Attr_attribute  avlist[];
{
    Attr_avlist	    attrs;
    Frame_base_info *frame = FRAME_BASE_PRIVATE(frame_public);
    Xv_Drawable_info *info;
    Xv_opaque       server_public;
    int             result = XV_OK;
    int             add_decor, delete_decor;
    Atom            add_decor_list[WM_MAX_DECOR], delete_decor_list[WM_MAX_DECOR];

    DRAWABLE_INFO_MACRO(frame_public, info);
    server_public = xv_server(info);
    add_decor = delete_decor = 0;

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch (attrs[0]) {
	  case FRAME_SHOW_LABEL:	/* same as FRAME_SHOW_HEADER */
	    attrs[0] = (Frame_attribute) ATTR_NOP(attrs[0]);
	    if (status_get(frame, show_label) == (int) attrs[1])
		break;

	    status_set(frame, show_label, (int) attrs[1]);

	    if ((int) attrs[1])
		add_decor_list[add_decor++] =
		    (Atom) xv_get(server_public, SERVER_WM_DECOR_HEADER);
	    else
		delete_decor_list[delete_decor++] =
		    (Atom) xv_get(server_public, SERVER_WM_DECOR_HEADER);
	    break;

	  case FRAME_SHOW_FOOTER:
	    attrs[0] = (Frame_attribute) ATTR_NOP(attrs[0]);
	    if (status_get(frame, show_footer) == (int) attrs[1])
		break;

	    status_set(frame, show_footer, (int) attrs[1]);

	    if ((int) attrs[1])
		add_decor_list[add_decor++] =
		    (Atom) xv_get(server_public, SERVER_WM_DECOR_FOOTER);
	    else
		delete_decor_list[delete_decor++] =
		    (Atom) xv_get(server_public, SERVER_WM_DECOR_FOOTER);
	    break;

          case FRAME_SHOW_RESIZE_CORNER:
#ifdef OW_I18N
            status_set(frame, show_resize_corner_init, TRUE);
#endif
            attrs[0] = (Frame_attribute) ATTR_NOP(attrs[0]);
            if (status_get(frame, show_resize_corner) == (int) attrs[1])
                break;

            status_set(frame, show_resize_corner, (int) attrs[1]);

            if ((int) attrs[1])
                add_decor_list[add_decor++] =
                    (Atom) xv_get(server_public, SERVER_WM_DECOR_RESIZE);
            else
                delete_decor_list[delete_decor++] =
                    (Atom) xv_get(server_public, SERVER_WM_DECOR_RESIZE);
            break;

	  case FRAME_PROPERTIES_PROC:
	    attrs[0] = (Frame_attribute) ATTR_NOP(attrs[0]);
	    frame->props_proc = (void (*) ()) attrs[1];
	    break;

	  case FRAME_SCALE_STATE:
	    attrs[0] = (Frame_attribute) ATTR_NOP(attrs[0]);
	    /*
	     * set the local rescale state bit, then tell the WM the current
	     * state, and then set the scale of our subwindows
	     */
	    /*
	     * WAIT FOR NAYEEM window_set_rescale_state(frame_public,
	     * attrs[1]);
	     */
	    wmgr_set_rescale_state(frame_public, attrs[1]);
	    frame_rescale_subwindows(frame_public, attrs[1]);
	    break;

	  case XV_LABEL:
	    {
		Frame_class_info *frame_class = FRAME_CLASS_FROM_BASE(frame);

		*attrs = (Frame_attribute) ATTR_NOP(*attrs);
#ifdef OW_I18N
		if (frame_class->label)
		{
		    free(frame_class->label);
		    frame_class->label = NULL;
		}
		if (frame_class->label_wcs) {
                    free(frame_class->label_wcs);
		}
		if ((char *) attrs[1]) {
		    frame_class->label_wcs = mbstowcsdup((char *)attrs[1]);
		} else {
		    frame_class->label_wcs = NULL;
		}
#else OW_I18N
		if (frame_class->label)
		    free(frame_class->label);
		if ((char *) attrs[1]) {
		    frame_class->label = (char *)
			calloc(1, strlen((char *) attrs[1]) + 1);
		    strcpy(frame_class->label, (char *) attrs[1]);
		} else {
		    frame_class->label = NULL;
		}
#endif OW_I18N
		(void) frame_display_label(frame_class);
	    }
	    break;

#ifdef OW_I18N
	  case XV_LABEL_WCS:
	    {
		Frame_class_info *frame_class = FRAME_CLASS_FROM_BASE(frame);

		*attrs = (Frame_attribute) ATTR_NOP(*attrs);
		if (frame_class->label)
		{
		    free(frame_class->label);
		    frame_class->label = NULL;
		}
		if (frame_class->label_wcs)
		    free(frame_class->label_wcs);
                if ((char *) attrs[1]) {
		    frame_class->label_wcs = wsdup((wchar_t *)attrs[1]);
		} else {
		    frame_class->label_wcs = NULL;
		}
		(void) frame_display_label(frame_class);
	    }
	    break;

	  case FRAME_INPUT_WINDOW:
	    {
		Frame_class_info *frame_class = FRAME_CLASS_FROM_BASE(frame);

		attrs[0] = (Frame_attribute) ATTR_NOP(attrs[0]);
		frame_class->input_window = (Xv_opaque) attrs[1];
	    }
	    break;

	  case FRAME_SHOW_IMSTATUS:
	    status_set(frame, show_IMstatus_init, TRUE);
	    attrs[0] = (Frame_attribute) ATTR_NOP(attrs[0]);
	    if (status_get(frame, show_IMstatus) == (int) attrs[1])
		break;

	    status_set(frame, show_IMstatus, (int) attrs[1]);

	    if ((int) attrs[1])
		add_decor_list[add_decor++] =
		    (Atom) xv_get(server_public, SERVER_WM_DECOR_IMSTATUS);
	    else
		delete_decor_list[delete_decor++] =
		    (Atom) xv_get(server_public, SERVER_WM_DECOR_IMSTATUS);
	    break;
#endif OW_I18N

	  case XV_END_CREATE:
	    (void) wmgr_set_win_attr(frame_public, &(frame->win_attr));
	    break;

	  default:
	    break;

	}
    }

#ifdef OW_I18N
    if (status_get(frame, show_IMstatus_init) != TRUE
     && status_get(frame, show_IMstatus) != TRUE
     && xv_get(frame_public, WIN_USE_IM) == TRUE
     && (XIM *)xv_get(server_public, XV_IM) != 0)
    {
	status_set(frame, show_IMstatus, TRUE);
	add_decor_list[add_decor++] =
		    (Atom) xv_get(server_public, SERVER_WM_DECOR_IMSTATUS);
    }
    status_set(frame, show_IMstatus_init, TRUE);

    if (status_get(frame, show_resize_corner_init) != TRUE
     && status_get(frame, show_resize_corner) == FALSE)
    {
	status_set(frame, show_resize_corner, TRUE);
	add_decor_list[add_decor++] =
		    (Atom) xv_get(server_public, SERVER_WM_DECOR_RESIZE);
    }
    status_set(frame, show_resize_corner_init, TRUE);
#endif

    if (add_decor)
	wmgr_add_decor(frame_public, add_decor_list, add_decor);

    if (delete_decor)
	wmgr_delete_decor(frame_public, delete_decor_list, delete_decor);

    return (Xv_opaque) result;
}
