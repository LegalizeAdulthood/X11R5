#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_compat.c 50.1 90/12/12 Copyr 1985 Sun Micro";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview/pkg.h>
#include <xview_private/panel_impl.h>

Xv_private void window_rc_units_to_pixels();

/*
 * routines for compatibility with SunView 1.n
 */


/* VARARGS2 */ /*VARARGS*/
Sv1_public      Panel_item
panel_create_item(client_panel, create_proc, va_alist)
    Panel           client_panel;
Panel_item(*create_proc) ();
va_dcl
{
    Attr_avlist     avlist[ATTR_STANDARD_SIZE];
    va_list         valist;

    va_start(valist);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);

    /*
     * convert row/column units to pixels now. This is provided for
     * compatibility with ATTR_ROW/COL().
     */
    window_rc_units_to_pixels(client_panel, (Window_attribute *) avlist);

    return xv_create_avlist(client_panel, create_proc, avlist);
}


/* VARARGS2 */ /*VARARGS*/
Sv1_public      Panel_attribute_value
panel_get(client_object, attr, arg1)
    Panel           client_object;
    Attr_attribute  attr;
    Xv_opaque       arg1;
{
    return (Panel_attribute_value) xv_get(client_object, attr, arg1);
}


/* VARARGS */ /*VARARGS*/
Sv1_public
panel_set(client_object, va_alist)
    Panel           client_object;
va_dcl
{
    Item_info      *object = ITEM_PRIVATE(client_object);
    Panel_info     *panel;
    Attr_avlist     avlist[ATTR_STANDARD_SIZE];
    va_list         valist;

    va_start(valist);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);

    if (is_panel(object)) {
	return window_set(client_object, ATTR_LIST, avlist, 0);
    }
    /*
     * convert row/column units to pixels now. This is provided for
     * compatibility with ATTR_ROW/COL().
     */
    panel = ((Item_info *) object)->panel;

    window_rc_units_to_pixels(PANEL_PUBLIC(panel), (Window_attribute *) avlist);

    (void) xv_set_avlist(client_object, avlist);
    return 1;
}

Sv1_public
panel_destroy_item(client_item)
    Panel_item      client_item;
{
    xv_destroy(client_item);
}


Sv1_public
panel_free(client_object)
    Panel           client_object;
{
    /* Must be immediate destroy for SunView 1.X compatibility. */
    xv_destroy_immediate(client_object);
}
