/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*********************\
* 		      *
*  WOOL_OBJECT: Menu  *
*  BODY		      *
* 		      *
\*********************/

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_atom.h"
#include "wl_number.h"
#include "wl_string.h"
#include "wl_list.h"
#include "gwm.h"
#include "wl_event.h"
#include "wl_fsm.h"
#include "wl_bar.h"
#include "INTERN.h"
#include "wl_menu.h"

extern Menu SetUpMenu();

/*
 * Constructor: wool_menu_make callable from wool
 * with direction, bars
 */

WOOL_Menu
wool_menu_make(argc, argv)
int		argc;
WOOL_Bar	argv[];
{
    int             i,j;
    WOOL_Menu       menu;

    menu = (WOOL_Menu) Malloc(sizeof(struct _WOOL_Menu) +
			    Max(0, argc - 1) * sizeof(WOOL_OBJECT));
    zrt_put(menu);
    menu -> type = WLMenu;
    menu -> direction = DefaultMenuDirection;
    menu -> borderwidth = DefaultBorderWidth;
    menu -> bar_separator = DefaultBarSeparator;
    menu -> borderpixel = Context -> pixel.Border;
    menu -> background = Context -> pixel.Back;
    get_val_from_context(menu -> menu, WA_menu);
    get_val_from_context(menu -> bordertile, WA_bordertile);
    get_val_from_context(menu -> fsm, WA_fsm);
    get_val_from_context(menu -> cursor, WA_cursor);
    get_val_from_context(menu -> property, WA_property);

    fix_fsm(&(menu -> fsm));
    menu -> bars_size = argc;
    for (i = 0, j = 0; i < argc; i++, j++) {
	increase_reference(menu -> bars[j] = argv[i]);
	if (NIL == (WOOL_OBJECT) menu -> bars [j]) 
	    j--, menu -> bars_size--;
    }
    menu -> wob_menu = SetUpMenu(menu);
    return menu;
}

/*
 * WLMenu_print:
 * MENU(text)
 */

WOOL_OBJECT
WLMenu_print(obj)
WOOL_Menu       obj;
{
    int i;

    wool_printf("{MENU 0x%x: ", obj);
    for (i = 0; i < obj -> bars_size; i++) {
	wool_print((obj -> bars)[i]);
	wool_puts(" ");
    }
    wool_puts("}");
    return (WOOL_OBJECT) obj;
}

/*
 * WLMenu_free:
 * recursivly free string and fsm
 */

WOOL_OBJECT
WLMenu_free(obj)
WOOL_Menu       obj;
{
    int i;

    decrease_reference(obj -> menu);
    decrease_reference(obj -> bordertile);
    decrease_reference(obj -> fsm);
    decrease_reference(obj -> cursor);
    decrease_reference(obj -> property);
    for (i = 0; i < obj -> bars_size; i++)
        decrease_reference(obj -> bars[i]);
    Free(obj);
    return NULL;
}

WOOL_OBJECT
WLMenu_get_dimensions(obj, box)
WOOL_Menu obj;
Box     box;                    /* RETURN the dimensions */
{
    return (WOOL_OBJECT) WOOL_send(WOOL_get_dimensions,
		     obj -> wob_menu, (obj -> wob_menu, box));
}

WOOL_OBJECT
WLMenu_wob(wl_menu)
WOOL_Menu wl_menu;
{
    must_be_menu(wl_menu, 0);
    return (WOOL_OBJECT) WLNumber_make(wl_menu -> wob_menu);
}
