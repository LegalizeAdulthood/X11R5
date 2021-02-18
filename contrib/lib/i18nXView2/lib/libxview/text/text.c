#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)text.c 70.3 91/07/08";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview/pkg.h>
#include <xview/attrol.h>
#include <xview_private/primal.h>
#include <xview_private/txt_impl.h>
#include <xview/textsw.h>
#include <xview/win_struct.h>
#include <xview/window.h>
#include <xview/text.h>
#include <X11/Xlib.h>
#include <X11/XSunExt.h>


Pkg_private Textsw_folio textsw_init_internal();

Pkg_private int textsw_init();
Pkg_private Xv_opaque textsw_get();
Pkg_private Xv_opaque textsw_set();
Pkg_private int textsw_folio_destroy();

Pkg_private int
textsw_init(parent, textsw_public, avlist)
    Xv_Window       parent;
    Textsw          textsw_public;
    Attr_attribute  avlist[];
{
    Attr_avlist     attrs;
    Textsw_status   dummy_status;
    Textsw_status  *status = &dummy_status;
    Xv_textsw      *textsw_object = (Xv_textsw *) textsw_public;
    Textsw_folio    folio = NEW(struct textsw_object);

    textsw_object->private_data = (Xv_opaque) folio;
    folio->public_self = textsw_public;

#ifdef OW_I18N
    folio->start_pecb_struct.callback = NULL;
    folio->start_pecb_struct.client_data = NULL;
    folio->draw_pecb_struct.callback = NULL;
    folio->draw_pecb_struct.client_data = NULL;
    folio->done_pecb_struct.callback = NULL;
    folio->done_pecb_struct.client_data = NULL;
 
    folio->start_stcb_struct.callback = NULL;
    folio->start_stcb_struct.client_data = NULL;
    folio->draw_stcb_struct.callback = NULL;
    folio->draw_stcb_struct.client_data = NULL;
    folio->done_stcb_struct.callback = NULL;
    folio->done_stcb_struct.client_data = NULL;

    folio->start_luc_struct.callback = NULL;    
    folio->start_luc_struct.client_data = NULL;
    folio->draw_luc_struct.callback = NULL;
    folio->draw_luc_struct.client_data = NULL;
    folio->done_luc_struct.callback = NULL;
    folio->done_luc_struct.client_data = NULL;
    folio->process_luc_struct.callback = NULL;
    folio->process_luc_struct.client_data  = NULL;
    
    folio->start_aux_struct.callback = NULL;
    folio->start_aux_struct.client_data = NULL;
    folio->draw_aux_struct.callback = NULL;
    folio->draw_aux_struct.client_data = NULL;
    folio->done_aux_struct.callback = NULL;
    folio->done_aux_struct.client_data = NULL;

#endif OW_I18N

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch (*attrs) {
	  case TEXTSW_STATUS:
	    status = (Textsw_status *) attrs[1];
	    break;

#ifdef OW_I18N
	  case WIN_IM_PREEDIT_START:
	    folio->start_pecb_struct.callback = (XIMProc)attrs[1];
	    folio->start_pecb_struct.client_data = (XPointer)attrs[2];
	    break;
 
	  case WIN_IM_PREEDIT_DRAW:
	    folio->draw_pecb_struct.callback = (XIMProc)attrs[1];
	    folio->draw_pecb_struct.client_data = (XPointer)attrs[2];
	    break;
 
	  case WIN_IM_PREEDIT_DONE:
	    folio->done_pecb_struct.callback = (XIMProc)attrs[1];
	    folio->done_pecb_struct.client_data = (XPointer)attrs[2];
	    break;
 
	  case WIN_IM_STATUS_START:
	    folio->start_stcb_struct.callback = (XIMProc)attrs[1];
	    folio->start_stcb_struct.client_data = (XPointer)attrs[2];
	    break;
 
	  case WIN_IM_STATUS_DRAW:
	    folio->draw_stcb_struct.callback = (XIMProc)attrs[1];
	    folio->draw_stcb_struct.client_data = (XPointer)attrs[2];
	    break;
 
	  case WIN_IM_STATUS_DONE:
	    folio->done_stcb_struct.callback = (XIMProc)attrs[1];
	    folio->done_stcb_struct.client_data = (XPointer)attrs[2];
	    break;
 
	  case WIN_IM_LUC_START:
	    folio->start_luc_struct.callback = (XIMProc)attrs[1];
	    folio->start_luc_struct.client_data = (XPointer)attrs[2];
	    break;
 
	  case WIN_IM_LUC_DRAW:
	    folio->draw_luc_struct.callback = (XIMProc)attrs[1];
	    folio->draw_luc_struct.client_data = (XPointer)attrs[2];
	    break;
 
	  case WIN_IM_LUC_DONE:
	    folio->done_luc_struct.callback = (XIMProc)attrs[1];
	    folio->done_luc_struct.client_data = (XPointer)attrs[2];
	    break;
 
	  case WIN_IM_LUC_PROCESS:
	    folio->process_luc_struct.callback = (XIMProc)attrs[1];
	    folio->process_luc_struct.client_data  = (XPointer)attrs[2];
	    break;
#endif OW_I18N

	  default:
	    break;
	}
    }

    if (!folio) {
	*status = TEXTSW_STATUS_CANNOT_ALLOCATE;
	return (XV_ERROR);
    }
    /* link to object */

    folio = textsw_init_internal(folio, status, textsw_default_notify, avlist);

    /*
     * BUG: Note the folio is not really initialized until the first view is
     * created.
     */
    return (folio ? XV_OK : XV_ERROR);
}

