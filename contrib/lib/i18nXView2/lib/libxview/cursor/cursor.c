#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)cursor.c 70.1 91/05/31";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * cursor.c: Routines for creating & modifying a cursor.
 * 
 */

#include <xview_private/curs_impl.h>
#include <xview/notify.h>
#include <xview/svrimage.h>
#include <xview/window.h>
#include <xview/screen.h>

#ifdef OW_I18N
#include <xview_private/xv_i18n_impl.h>
#endif

Xv_private Attr_avlist attr_find();
Pkg_private unsigned long cursor_make_x();
Pkg_private unsigned long cursor_make_x_font();
Pkg_private void cursor_free_x();
Pkg_private void cursor_set_cursor_internal();


int
cursor_create_internal(parent, object, avlist)
    Xv_Screen       parent;
    Xv_Cursor       object;
    Attr_avlist     avlist;
/*
 * Parent should be either a window or a screen, or any object that will
 * return the root window in response to xv_get(parent, XV_ROOT).
 */
{
    register Cursor_info *cursor;
    register Pixrect *pr;
    Cursor_info    *other_cursor;
    Attr_avlist     copy_attr;

    ((Xv_cursor_struct *) (object))->private_data =
	(Xv_opaque) xv_alloc(Cursor_info);
    if (!(cursor = CURSOR_PRIVATE(object))) {
	return XV_ERROR;
    }
    cursor->public_self = object;
    cursor->cur_src_char = NOFONTCURSOR;

    copy_attr = attr_find(avlist, XV_COPY_OF);
    if (*copy_attr) {
	other_cursor = CURSOR_PRIVATE(copy_attr[1]);
	*cursor = *other_cursor;

	/* Allocate new shape, copy old, and flag need to free new shape. */
	pr = other_cursor->cur_shape;
	cursor->cur_shape =
	    (Pixrect *) xv_create(xv_get(other_cursor->root, XV_SCREEN),
				  SERVER_IMAGE,
				  XV_WIDTH, pr->pr_width,
				  XV_HEIGHT, pr->pr_height,
				  SERVER_IMAGE_DEPTH, pr->pr_depth,
				  NULL);
	if (!cursor->cur_shape)
	    return XV_ERROR;
	xv_rop(cursor->cur_shape, 0, 0, pr->pr_width, pr->pr_height, PIX_SRC,
	       pr, 0, 0);
	cursor->flags |= FREE_SHAPE;
    } else {
	/* Use default screen if none given (xv_create ensures default ok) */
	cursor->root = xv_get((parent ? parent : xv_default_screen), XV_ROOT);
	cursor->cur_function = PIX_SRC | PIX_DST;
	cursor->cur_shape = 
	    (Pixrect *) xv_create(xv_get(cursor->root, XV_SCREEN),
				  SERVER_IMAGE,
				  XV_WIDTH, CURSOR_MAX_IMAGE_WORDS,
				  XV_HEIGHT, CURSOR_MAX_IMAGE_WORDS,
				  SERVER_IMAGE_DEPTH, 1,
				  NULL);
	cursor->flags = FREE_SHAPE;
    }
    /* the id will be set the first time through cursor_set() */
    cursor->cursor_id = 0;

    /* default foreground/background color */
    cursor->fg.red = 0;
    cursor->fg.green = 0;
    cursor->fg.blue = 0;
    cursor->bg.red = 255;
    cursor->bg.green = 255;
    cursor->bg.blue = 255;

    (void) xv_set(object, XV_RESET_REF_COUNT, 0);	/* Mark as ref counted. */

    return XV_OK;
}


int
cursor_destroy_internal(cursor_public, status)
    Xv_Cursor       cursor_public;
    Destroy_status  status;
{
    Cursor_info    *cursor = CURSOR_PRIVATE(cursor_public);

    if (status == DESTROY_CLEANUP) {
	if (free_shape(cursor))
	    xv_destroy(cursor->cur_shape);
	free((char *) cursor);
    }
    return XV_OK;
}

/*ARGSUSED*/ /*VARARGS3*/
Pkg_private Xv_opaque
cursor_get_internal(cursor_public, status, which_attr, args)
    Xv_Cursor       cursor_public;
    int            *status;
    Cursor_attribute which_attr;
    va_list         args;
{
    register Cursor_info     *cursor = CURSOR_PRIVATE(cursor_public);

    switch (which_attr) {
      case XV_XID:
	return (Xv_opaque) cursor->cursor_id;

      case XV_SHOW:
	return (Xv_opaque) show_cursor(cursor);

      case CURSOR_SRC_CHAR:
	return (Xv_opaque) cursor->cur_src_char;

      case CURSOR_MASK_CHAR:
	return (Xv_opaque) cursor->cur_mask_char;

      case CURSOR_XHOT:
	return (Xv_opaque) cursor->cur_xhot;

      case CURSOR_YHOT:
	return (Xv_opaque) cursor->cur_yhot;

      case CURSOR_OP:
	return (Xv_opaque) cursor->cur_function;

      case CURSOR_IMAGE:
	return (Xv_opaque) cursor->cur_shape;

      case CURSOR_FOREGROUND_COLOR: 
	 return ((Xv_opaque) &cursor->fg);

      case CURSOR_BACKGROUND_COLOR:
	 return ((Xv_opaque)&cursor->bg);

      default:
	if (xv_check_bad_attr(CURSOR, which_attr) == XV_ERROR) {
	    *status = XV_ERROR;
	}
	return XV_NULL;
    }

}


/* cursor_set_attr sets the attributes mentioned in avlist. */
Xv_opaque
cursor_set_internal(cursor_public, avlist)
    Xv_Cursor       cursor_public;
    register Attr_avlist avlist;
{
    register Cursor_info 	*cursor = CURSOR_PRIVATE(cursor_public);
    register Pixrect 		*pr;
    register int    		dirty = FALSE;
    Xv_Drawable_info 		*root_info;
    register Xv_opaque 		arg1;
    Xv_singlecolor		*fg = NULL, *bg = NULL;	
    XColor			xfg, xbg;

    for (; *avlist; avlist = attr_next(avlist)) {
	arg1 = avlist[1];
	switch ((Cursor_attribute) (*avlist)) {
	  case XV_SHOW:
	    /* BUG: is this used anywhere? */
	    if (arg1)
		cursor->flags &= ~DONT_SHOW_CURSOR;
	    else
		cursor->flags |= DONT_SHOW_CURSOR;
	    dirty = TRUE;
	    break;

	  case CURSOR_SRC_CHAR:
	    cursor->cur_src_char = (unsigned int) arg1;
	    dirty = TRUE;
	    break;

	  case CURSOR_MASK_CHAR:
	    cursor->cur_mask_char = (unsigned int) arg1;
	    dirty = TRUE;
	    break;

	  case CURSOR_XHOT:
	    cursor->cur_xhot = (int) arg1;
	    dirty = TRUE;
	    break;

	  case CURSOR_YHOT:
	    cursor->cur_yhot = (int) arg1;
	    dirty = TRUE;
	    break;

	  case CURSOR_OP:
	    cursor->cur_function = (int) arg1;
	    dirty = TRUE;
	    break;

	  case CURSOR_IMAGE:
	    if (free_shape(cursor)) {
		xv_destroy(cursor->cur_shape);	/* destroy the remote image */
		cursor->flags &= ~FREE_SHAPE;
	    }
	    cursor->cur_shape = (Pixrect *) arg1;
	    dirty = TRUE;
	    break;

	  case CURSOR_FOREGROUND_COLOR:
	     fg = (Xv_singlecolor *)arg1;
	     cursor->fg.red = fg->red;
	     cursor->fg.green = fg->green;
	     cursor->fg.blue = fg->blue;
	     break;

	  case CURSOR_BACKGROUND_COLOR:
	     bg = (Xv_singlecolor *)arg1;
	     cursor->bg.red = bg->red;
	     cursor->bg.green = bg->green;
	     cursor->bg.blue = bg->blue;
	     break;

	  default:
	    (void) xv_check_bad_attr(CURSOR, *avlist);
	    break;
	}
    }

    xfg.red = cursor->fg.red << 8;
    xfg.green = cursor->fg.green << 8;
    xfg.blue = cursor->fg.blue << 8;
    xfg.flags = DoRed | DoGreen | DoBlue;
    xbg.red = cursor->bg.red << 8; 
    xbg.green = cursor->bg.green << 8;
    xbg.blue = cursor->bg.blue << 8;
    xbg.flags = DoRed | DoGreen | DoBlue;

    DRAWABLE_INFO_MACRO(cursor->root, root_info);

    if (!dirty) {
	if (fg || bg) {
	    XRecolorCursor(xv_display(root_info), cursor->cursor_id, &xfg, &xbg);
	}
	return XV_OK;
    }

    /* make the cursor now */
    if (cursor->cursor_id) {
	cursor_free_x(root_info, cursor->cursor_id);
    }
    if (cursor->cur_src_char != NOFONTCURSOR) {
	cursor->cursor_id = cursor_make_x_font(root_info,
				       (unsigned int) cursor->cur_src_char,
				       (unsigned int) cursor->cur_mask_char,
				       &xfg, &xbg);
    } else {
	pr = cursor->cur_shape;
	cursor->cursor_id = cursor_make_x(root_info,
	   				  pr->pr_size.x, pr->pr_size.y,
					  pr->pr_depth, cursor->cur_function,
				          cursor->cur_xhot, cursor->cur_yhot, 
					  &xfg, &xbg, pr); 
    }

    /* BUG: ok to abort? */
    if (!cursor->cursor_id) {
#ifdef OW_I18N
	xv_error(cursor,
		 ERROR_STRING, XV_I18N_MSG("xv_messages","cursor: can't create cursor"),
		 ERROR_PKG, CURSOR,
		 0);
	return (Xv_opaque) XV_ERROR;
#else
	xv_error(cursor,
		 ERROR_STRING, "cursor: can't create cursor",
		 ERROR_PKG, CURSOR,
		 0);
#endif OW_I18N
    }
    return (Xv_opaque) XV_OK;
}

Xv_private void
cursor_set_cursor(window, cursor_public)
    Xv_object       window;
    Xv_Cursor       cursor_public;
{
    Cursor_info    *cursor = CURSOR_PRIVATE(cursor_public);
    Xv_Drawable_info *window_info;

    if (xv_get(window, XV_ROOT) != cursor->root) {
#ifdef OW_I18N
	xv_error(cursor,
		 ERROR_STRING,
		   XV_I18N_MSG("xv_messages","Window and cursor have different roots! Can't set cursor"),
		 ERROR_PKG, CURSOR,
		 0);
#else
	xv_error(cursor,
		 ERROR_STRING,
		   "Window and cursor have different roots! Can't set cursor",
		 ERROR_PKG, CURSOR,
		 0);
#endif OW_I18N
    } else {
	DRAWABLE_INFO_MACRO(window, window_info);
	cursor_set_cursor_internal(window_info, cursor->cursor_id);
    }
}
