/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/***********************\
* 		        *
*  WOOL_OBJECT: Client  *
*  BODY		        *
* 		        *
\***********************/

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_atom.h"
#include "wl_number.h"
#include "wl_string.h"
#include "wl_list.h"
#include "gwm.h"
#include "wl_pixmap.h"
#include "wl_event.h"
#include "wl_fsm.h"
#include "wl_plug.h"
#include "wl_bar.h"
#include "wl_cursor.h"
#include "wl_menu.h"
#include "INTERN.h"
#include "wl_client.h"
#include "EXTERN.h"

/*
 *  wool_client_make:
 * here we put all the hints got from window (properties) and wool profile
 * into a WOOL_Client structure
 * 
 *         (window-make titlebar leftbar rightbar basebar plug)
 */

WOOL_Client
wool_client_make(argc, argv)
int		argc;
WOOL_OBJECT	argv[];
{
    WOOL_Client     wl_client;

    if (argc != 5)
	wool_error(BAD_NUMBER_OF_ARGS, argc);
    wl_client = (WOOL_Client) Malloc(sizeof(struct _WOOL_Client));
    wl_client -> type = WLClient;
    zrt_put(wl_client);

    increase_reference(wl_client -> titlebar = (WOOL_Bar) argv[0]);
    increase_reference(wl_client -> leftbar = (WOOL_Bar) argv[1]);
    increase_reference(wl_client -> rightbar = (WOOL_Bar) argv[2]);
    increase_reference(wl_client -> basebar = (WOOL_Bar) argv[3]);
    increase_reference(wl_client -> icon_plug = (WOOL_Plug) argv[4]);

    wl_client -> borderwidth = DefaultBorderWidth;
    wl_client -> borderpixel = Context -> pixel.Border;
    wl_client -> background = Context -> pixel.Back;
    wl_client -> inner_borderwidth = DefaultInnerBorderwidth;
    get_val_from_context(wl_client -> bordertile, WA_bordertile);
    get_val_from_context(wl_client -> tile, WA_tile);
    get_val_from_context(wl_client -> fsm, WA_fsm);
    get_val_from_context(wl_client -> menu, WA_menu);
    get_val_from_context(wl_client -> cursor, WA_cursor);
    get_val_from_context(wl_client -> property, WA_property);
    get_val_from_context(wl_client -> grabs, WA_grabs);
    get_val_from_context(wl_client -> opening, WA_opening);
    get_val_from_context(wl_client -> closing, WA_closing);

    fix_fsm(&(wl_client -> fsm));
    return wl_client;
}

/*
 * WLClient_print:
 * CLIENT(text)
 */

WOOL_OBJECT
WLClient_print(obj)
WOOL_Client        obj;
{
    wool_printf("{CLIENT 0x%x: ", obj);
    wool_print(obj -> titlebar);
    wool_puts(" ");
    wool_print(obj -> leftbar);
    wool_puts(" ");
    wool_print(obj -> rightbar);
    wool_puts(" ");
    wool_print(obj -> basebar);
    wool_puts(" ");
    wool_print(obj -> icon_plug);
    wool_puts("}");
    return (WOOL_OBJECT) obj;
}

/*
 * WLClient_free:
 * recursivly free fsm
 */

WOOL_OBJECT
WLClient_free(obj)
WOOL_Client        obj;
{
    decrease_reference(obj -> bordertile);
    decrease_reference(obj -> tile);
    decrease_reference(obj -> fsm);
    decrease_reference(obj -> menu);
    decrease_reference(obj -> cursor);
    decrease_reference(obj -> opening);
    decrease_reference(obj -> closing);
    decrease_reference(obj -> property);
    decrease_reference(obj -> grabs);
    decrease_reference(obj -> titlebar);
    decrease_reference(obj -> leftbar);
    decrease_reference(obj -> rightbar);
    decrease_reference(obj -> basebar);
    decrease_reference(obj -> icon_plug);
    Free(obj);
    return NULL;
}

/*
 * MatchWoolDescription Executes the user-defined wool function
 * "describe-window" which is supposed to return the wl_clients descriptors
 * for the current-window & icon as a list of 2 wl_clients for window & icon,
 * 
 * If the returned value is not correct, it takes the value of
 * window-description
 */

MatchWoolDescription(cw, cw_desc_ptr)
ClientWindow	cw;
WOOL_OBJECT	*cw_desc_ptr; /* RETURN: WOOL_Clients */
{
    WOOL_List       desc =
    (WOOL_List) wool_eval_and_catch_errors(WL_describe_window_call);

    if (!desc || (desc -> type != WLList) || (desc -> size != 2)) {
	wool_puts("GWM: the return value of \"describe-window\": ");
	wool_print(desc);
	wool_puts("\n    is not a pair of window descriptors,");
	wool_puts("using \"window-description\"\n");
	desc = (WOOL_List) wool_eval_and_catch_errors(WA_window_description);
	if (!desc || (desc -> type != WLList) || (desc -> size != 2)) {
	    wool_puts("\"window-description\": ");
	    wool_print(desc);
	    wool_error("\n    is not pair of window descriptors%s", "");
	}
    }
    if (desc -> list[0] -> type != WLClient) {
	decrease_reference(desc -> list[0]);
	increase_reference(desc -> list[0] = wool_eval(desc -> list[0]));
	if (desc -> list[0] -> type != WLClient) {
	    wool_print(desc -> list[0]);
	    wool_newline();
	    wool_error("bad %s description!", "window");
	}
    }
    *cw_desc_ptr = desc -> list[0];
    increase_reference(cw -> icon_description = desc -> list[1]);
    return;
}

/* grabbing/ungrabbing buttons and keys (passive grabs) on the main window
 * (C-callable funcs)
 */

WLClient_set_grabs(list, window)
WOOL_List	list;
Window		window;
{
    int             i;
    WOOL_Event      event;
    KeyCode	    keycode;

    if (list == (WOOL_List) NIL || !window)
	return;
    if (list -> type != WLList)
	wool_error("Grabs must be a list, not a %s",
		   ((char *) ((WOOL_Atom) list -> type[0]) -> p_name));
    for (i = 0; i < list -> size; i++) {
	event = (WOOL_Event) list -> list[i];
	if (event -> type != WLEvent)
	    goto not_event;
	if (event -> match == WLEvent_buttonpress) {
	    XGrabButton(dpy,
			(event -> detail == (unsigned int) ANY ?
			 AnyButton : event -> detail),
			(event -> state == (unsigned int) ANY ?
			 AnyModifier : event -> state),
			window, FALSE,
			ButtonPressMask | ButtonReleaseMask,
			((event -> flags & FREEZE_ON_GRABS) ?
			 GrabModeSync : GrabModeAsync)
			,GrabModeAsync, None, None);
	} else if (event -> match == WLEvent_keypress) {
	    /* A 0 keycode Means AnyKey ! */
	    if (event -> detail != (unsigned int) ANY) {
		if (event -> detail != (unsigned int) UNDEFINED_KEYCODE)
		  if (keycode = XKeysymToKeycode(dpy, event -> detail))
		    XGrabKey(dpy, keycode,
			     (event -> state == (unsigned int) ANY ?
			      AnyModifier : event -> state),
			     window, FALSE,
			GrabModeAsync, ((event -> flags & FREEZE_ON_GRABS) ?
					GrabModeSync : GrabModeAsync));
	    } else {
		XGrabKey(dpy, AnyKey,
			 (event -> state == (unsigned int) ANY ?
			  AnyModifier : event -> state),
			 window, FALSE,
			 GrabModeAsync, ((event -> flags & FREEZE_ON_GRABS) ?
					 GrabModeSync : GrabModeAsync));
	    }
	} else
	    goto not_event;
    }
    return;
not_event:
    wool_error("grabs only on %s", "buttons or keys");
}

WLClient_unset_grabs(list, window)
WOOL_List	list;
Window		window;
{
    int             i;
    WOOL_Event      event;
    KeyCode	    keycode;

    if (list == (WOOL_List) NIL || !window)
	return;
    if (list -> type != WLList) 
	wool_error("Ungrabs must be a list, not a %s",
                  ((char *) ((WOOL_Atom) list -> type[0])->p_name));
    for (i = 0; i < list -> size; i++) {
	event = (WOOL_Event) list -> list[i];
	if (event -> type == WLEvent) {
	    if (event -> match == WLEvent_buttonpress) {
		XUngrabButton(dpy,
			    (event -> detail == (unsigned int) ANY ?
			     AnyButton : event -> detail),
			    (event -> state == (unsigned int) ANY ?
			     AnyModifier : event -> state),
			    window);
	    } else if (event -> match == WLEvent_keypress) {
		if (event -> detail != (unsigned int) ANY) {
		    if (event -> detail != (unsigned int) UNDEFINED_KEYCODE)
			if (keycode = XKeysymToKeycode(dpy, event -> detail))
			XUngrabKey(dpy, keycode,
				 (event -> state == (unsigned int) ANY ?
				  AnyModifier : event -> state),
				 window);
		} else {
		    XUngrabKey(dpy, AnyKey,
			     (event -> state == (unsigned int) ANY ?
			      AnyModifier : event -> state),
			     window);
		}
	    } else
		goto not_event;
	} else
	    goto not_event;
    }
    return;
not_event:
    wool_error("ungrabs only on %s", "buttons or keys");
}

/* grabbing/ungrabbing buttons and keys (passive grabs) on the main window
 * (WOOL-callable funcs)
 */

WOOL_OBJECT
wool_set_grabs(argc, argv)
int argc;
WOOL_OBJECT	*argv;
{
    if (argc)
	WLClient_set_grabs(wool_list_make_from_evaluated_array(argc, argv),
			   TargetWindow -> hook);
    return NIL;
}

WOOL_OBJECT
wool_unset_grabs(argc, argv)
int argc;
WOOL_OBJECT	*argv;
{
    if (argc)
	WLClient_unset_grabs(wool_list_make_from_evaluated_array(argc, argv),
			   TargetWindow -> hook);
    return NIL;
}

WOOL_OBJECT
WLClient_get_property()
{
    return (TargetWindow -> property);
}

WOOL_OBJECT
WLClient_set_property(property)
WOOL_OBJECT property;
{
    WOOL_OBJECT     object = TargetWindow -> property;

    must_be_or_nil(WLList, property, 0);
    increase_reference(TargetWindow -> property = property);
    decrease_reference(object);
    return property;
}
