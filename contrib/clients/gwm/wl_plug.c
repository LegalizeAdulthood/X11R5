/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/*********************\
* 		      *
*  WOOL_OBJECT: Plug  *
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
#include "wl_label.h"
#include "INTERN.h"
#include "wl_plug.h"

/*
 * Constructor: wool_plug_make callable from wool
 */

WOOL_Plug
wool_plug_make(label)
WOOL_OBJECT label;
{
    WOOL_Plug       plug = (WOOL_Plug) Malloc(sizeof(struct _WOOL_Plug));

    zrt_put(plug);
    plug -> type = WLPlug;
    plug -> borderwidth = DefaultBorderWidth;
    plug -> borderpixel = Context -> pixel.Border;
    plug -> background = Context -> pixel.Back;
    get_val_from_context(plug -> bordertile, WA_bordertile);
    get_val_from_context(plug -> fsm, WA_fsm);
    get_val_from_context(plug -> menu, WA_menu);
    get_val_from_context(plug -> cursor, WA_cursor);
    get_val_from_context(plug -> property, WA_property);

    fix_fsm(&(plug -> fsm));
    increase_reference(plug -> graphic = label);
    return plug;
}

/*
 * WLPlug_print:
 * PLUG(text)
 */

WOOL_OBJECT
WLPlug_print(obj)
WOOL_Plug       obj;
{
    wool_printf("{PLUG 0x%x: ", obj);
    wool_print(obj -> graphic);
    wool_puts("}");
    return (WOOL_OBJECT) obj;
}

/*
 * WLPlug_free:
 * recursivly free string and fsm
 */

WOOL_OBJECT
WLPlug_free(obj)
WOOL_Plug       obj;
{
    decrease_reference(obj -> bordertile);
    decrease_reference(obj -> menu);
    decrease_reference(obj -> graphic);
    decrease_reference(obj -> fsm);
    decrease_reference(obj -> cursor);
    decrease_reference(obj -> property);
    Free(obj);
    return NULL;
}
