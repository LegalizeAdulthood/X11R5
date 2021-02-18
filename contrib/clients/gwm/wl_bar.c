/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*********************\
* 		      *
*  WOOL_OBJECT: Bar  *
*  BODY		      *
* 		      *
\*********************/

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_atom.h"
#include "wl_number.h"
#include "wl_string.h"
#include "gwm.h"
#include "wl_event.h"
#include "wl_fsm.h"
#include "wl_plug.h"
#include "INTERN.h"
#include "wl_bar.h"

/*
 * Constructor: wool_bar_make callable from wool
 * (BAR min-width max-width plugs...)
 */

WOOL_Bar
wool_bar_make(argc, argv)
int    argc;
WOOL_Plug *argv;
{
    int             i;
    WOOL_Bar        bar;

    bar = (WOOL_Bar) Malloc(sizeof(struct _WOOL_Bar) +
			    sizeof(WOOL_OBJECT) * Max(0, argc - 1));
    zrt_put(bar);
    bar -> type = WLBar;
    bar -> min_width = DefaultBarMinWidth;
    bar -> max_width = DefaultBarMaxWidth;
    bar -> borderwidth = DefaultBorderWidth;
    bar -> borderpixel = Context -> pixel.Border;
    bar -> background = Context -> pixel.Back;
    bar -> plug_separator = DefaultPlugSeparator;
    get_val_from_context(bar -> bordertile, WA_bordertile);
    get_val_from_context(bar -> fsm, WA_fsm);
    get_val_from_context(bar -> menu, WA_menu);
    get_val_from_context(bar -> cursor, WA_cursor);
    get_val_from_context(bar -> tile, WA_tile);
    get_val_from_context(bar -> property, WA_property);

    fix_fsm(&(bar -> fsm));
    bar -> plugs_size = argc;
    for (i = 0; i < argc; i++) {
	increase_reference(bar -> plugs[i] = argv[i]);
    }
    return bar;
}

/*
 * WLBar_print:
 * BAR(text)
 */

WOOL_OBJECT
WLBar_print(obj)
WOOL_Bar        obj;
{
    int             i;

    wool_printf("{BAR 0x%x: ", obj);
    for (i = 0; i < obj -> plugs_size; i++) {
	wool_print((obj -> plugs)[i]);
	wool_puts(" ");
    }
    wool_puts("}");
    return (WOOL_OBJECT) obj;
}

/*
 * WLBar_free:
 * recursivly free fsm
 */

WOOL_OBJECT
WLBar_free(obj)
WOOL_Bar        obj;
{
    int    i;

    decrease_reference(obj -> bordertile);
    decrease_reference(obj -> fsm);
    decrease_reference(obj -> menu);
    decrease_reference(obj -> cursor);
    decrease_reference(obj -> tile);
    decrease_reference(obj -> property);
    for (i = 0; i < obj -> plugs_size; i++)
	decrease_reference(obj -> plugs[i]);
    Free(obj);
    return NULL;
}
