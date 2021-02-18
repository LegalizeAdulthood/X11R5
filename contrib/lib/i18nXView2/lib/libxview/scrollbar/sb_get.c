#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)sb_get.c 1.26 90/04/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	sb_get.c
 * 
 * Description:
 * 
 * returns values for scrollbar attributes
 * 
 */


/*
 * Include files:
 */

#include <xview_private/sb_impl.h>



/*
 * Declaration of Functions Defined in This File (in order):
 */

Pkg_private Xv_opaque scrollbar_get_internal();

/******************************************************************/

/*ARGSUSED*/ /*VARARGS3*/
Pkg_private     Xv_opaque
scrollbar_get_internal(scroll_public, status, attr, valist)
    Scrollbar       scroll_public;
    int            *status;
    Attr_attribute  attr;
    va_list         valist;
{
    Xv_scrollbar_info *sb = SCROLLBAR_PRIVATE(scroll_public);

    switch (attr) {
      case SCROLLBAR_INACTIVE:
	return ((Xv_opaque) sb->inactive);
      case SCROLLBAR_PIXELS_PER_UNIT:
	return ((Xv_opaque) sb->pixels_per_unit);
      case SCROLLBAR_OBJECT_LENGTH:
	return ((Xv_opaque) sb->object_length);
      case SCROLLBAR_OVERSCROLL:
	return ((Xv_opaque) sb->overscroll);
      case SCROLLBAR_VIEW_START:
	return ((Xv_opaque) sb->view_start);
      case SCROLLBAR_VIEW_LENGTH:
	return ((Xv_opaque) sb->view_length);
      case SCROLLBAR_PAGE_LENGTH:
	return ((Xv_opaque) sb->page_length);
      case SCROLLBAR_SPLITTABLE:
	return ((Xv_opaque) sb->can_split);
      case SCROLLBAR_NORMALIZE_PROC:
	return ((Xv_opaque) sb->normalize_proc);
      case SCROLLBAR_COMPUTE_SCROLL_PROC:
	return ((Xv_opaque) sb->compute_scroll_proc);
      case SCROLLBAR_DIRECTION:
	return ((Xv_opaque) sb->direction);
      case SCROLLBAR_MENU:
	return ((Xv_opaque) sb->menu);
      case SCROLLBAR_LAST_VIEW_START:
	return ((Xv_opaque) sb->last_view_start);
      case SCROLLBAR_NOTIFY_CLIENT:
	return ((Xv_opaque) sb->managee);
      default:
	xv_check_bad_attr(SCROLLBAR, attr);
	*status = XV_ERROR;
	return (Xv_opaque) 0;
    }
}
