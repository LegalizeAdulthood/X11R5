#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)windowcmpt.c 50.1 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/windowimpl.h>


/* VARARGS2 */
Xv_Window
window_create(parent_public, create_proc, va_alist)
    Xv_Window       parent_public;
caddr_t(*create_proc) ();
va_dcl
{
    caddr_t         avlist[ATTR_STANDARD_SIZE];
    va_list         valist;

    va_start(valist);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);

    return xv_create_avlist(parent_public, create_proc, avlist);
}


/* VARARGS1 */
int
window_set(win_public, va_alist)
    Xv_Window       win_public;
va_dcl
{
    va_list         valist;
    caddr_t         avlist[ATTR_STANDARD_SIZE];

    va_start(valist);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);

    /*
     * convert row/column units to pixels now. This is provided for
     * compatibility with ATTR_ROW/COL().
     */
    window_scan_and_convert_to_pixels(win_public, avlist);

    /* BUG: right return value */
    return ((XV_OK == (int) xv_set_avlist(win_public, avlist)) ? TRUE : FALSE);
}

/*
 * convert any row or column unit attribute values in avlist to pixel units,
 * using the window's row, column parameters.
 */
Xv_private void
window_rc_units_to_pixels(win_public, avlist)
    Window_attribute avlist[];
{
    register Window_info *win = WIN_PRIVATE(win_public);

    if (!win)
	return;

    attr_rc_units_to_pixels((char **) (avlist),
	 actual_column_width(win), actual_row_height(win), win->left_margin,
			    win->top_margin, win->column_gap, win->row_gap);
}


Xv_private void
window_scan_and_convert_to_pixels(win_public, avlist)
    Xv_Window       win_public;
    Attr_avlist     avlist;
{
    register Window_info *win = WIN_PRIVATE(win_public);
    register Attr_avlist attrs;

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch ((Window_attribute) attrs[0]) {
	  case WIN_FONT:
	    if (attrs[1]) {
		(void) xv_set(win->font, XV_DECREMENT_REF_COUNT, 0);
		win->font = (Pixfont *) attrs[1];
		(void) xv_set(win->font, XV_INCREMENT_REF_COUNT, 0);
	    }
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_TOP_MARGIN:
	    win->top_margin = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_BOTTOM_MARGIN:
	    win->bottom_margin = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_LEFT_MARGIN:
	    win->left_margin = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_RIGHT_MARGIN:
	    win->right_margin = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_ROW_HEIGHT:
	    win->row_height = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_COLUMN_WIDTH:
	    win->column_width = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_ROW_GAP:
	    win->row_gap = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;

	  case WIN_COLUMN_GAP:
	    win->column_gap = (int) attrs[1];
	    ATTR_CONSUME(attrs[0]);
	    break;
	}
    }
    window_rc_units_to_pixels(win_public, avlist);
}

int				/* bool */
window_destroy(window)
    Xv_Window       window;
{
    xv_destroy_check(window);
    return TRUE;
}
