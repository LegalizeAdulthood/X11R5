#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)om_compat.c 20.15 90/02/26";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * SunView1.X compatibility.
 */

/* ------------------------------------------------------------------- */

#include <xview_private/om_impl.h>

/* ------------------------------------------------------------------- */

/*VARARGS*/
Sv1_public      Menu
menu_create(va_alist)
va_dcl
{
    va_list         args;
    Attr_avlist     avlist[ATTR_STANDARD_SIZE];
    Menu            menu;

    va_start(args);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, args);
    va_end(args);
    menu = (Menu) xv_create_avlist(NULL, MENU, avlist);
    return menu;
}

/*VARARGS*/
Sv1_public      Menu_item
menu_create_item(va_alist)
va_dcl
{
    Attr_attribute  avlist[ATTR_STANDARD_SIZE];
    va_list         valist;

    va_start(valist);
    (void) attr_make((char **) avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);
    return (Menu_item) xv_create_avlist(NULL, MENUITEM, avlist);
}

/*VARARGS*/
Sv1_public      Xv_opaque
menu_set(menu_public, va_alist)
    Menu            menu_public;
va_dcl
{
    Attr_avlist     avlist[ATTR_STANDARD_SIZE];
    va_list         valist;

    va_start(valist);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);
    return xv_set_avlist(menu_public, avlist);
}

/*VARARGS*/ /*ARGSUSED*/
Sv1_public      Xv_opaque
menu_get(menu_public, attr, v1)
    Menu            menu_public;
    Xv_opaque       attr, v1;
{

    return xv_get(menu_public, attr, v1);

}

/*
 * for compatibility. BUG: note that this avoids the normal destroy
 * interposition chain, since there is no way to pass destroy_proc to the
 * low-level destroy routine.  Maybe the proc should be stashed in the menu
 * or item struct.
 */
Sv1_public void
menu_destroy_with_proc(m_public, destroy_proc)
    Menu            m_public;	/* menu or menu_item */
    void            (*destroy_proc) ();
{
    Xv_menu_info   *menu;
    Xv_menu_item_info *item;

    if (xv_get(m_public, XV_IS_SUBTYPE_OF, MENUITEM)) {
	item = MENU_ITEM_PRIVATE(m_public);
	item->extra_destroy_proc = destroy_proc;
    } else if (xv_get(m_public, XV_IS_SUBTYPE_OF, MENU)) {
	menu = MENU_PRIVATE(m_public);
	menu->extra_destroy_proc = destroy_proc;
    }
    xv_destroy(m_public);
}
