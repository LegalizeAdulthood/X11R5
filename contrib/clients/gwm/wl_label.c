/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/***********************\
* 		        *
*  WOOL_OBJECT: Label  *
*  BODY		        *
* 		        *
\***********************/

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_string.h"
#include "wl_atom.h"
#include "wl_number.h"
#include "gwm.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "INTERN.h"
#include "wl_label.h"



WOOL_OBJECT
wool_label_make(argc, argv)
int		argc;
WOOL_Number	argv[];
{
    WOOL_Label      object;

    if (argc == 0 || argc > 2)
	return wool_error(BAD_NUMBER_OF_ARGS, argc);
    must_be_string(argv[0], 0);
    object = (WOOL_Label) Malloc(sizeof(struct _WOOL_Label));
    zrt_put(object);
    object -> type = WLLabel;
    object -> color = Context -> pixel.Fore;
    increase_reference(object -> label = (WOOL_String) argv[0]);
    if (argc == 2) {
	must_be_number(argv[1], 1);
	object -> font = argv[1] -> number;
    } else
	object -> font = DefaultFont;
    return (WOOL_OBJECT) object;
}

/*
 * WLLabel_print:
 * a label prints as [fontnum]"string"
 */

WOOL_OBJECT 
WLLabel_print(obj)
WOOL_Label     obj;
{
    wool_printf("{LABEL %ld,", obj -> font);
    wool_printf(" %s}", obj -> label);
    return (WOOL_OBJECT) obj;
}

/*
 * WLLabel_free:
 * The structure is just freed, and string released.
 */

WOOL_OBJECT 
WLLabel_free(obj)
WOOL_Label     obj;
{
    decrease_reference(obj -> label);
    Free(obj);
    return NULL;
}

/*
 * handles its label as an C string.
 */

char *
WLLabel_get_C_value(obj)
WOOL_Label obj;
{
    return (char *) obj->label->string;
}

/*
 * returns the dimensions of the strings in the font + margins
 * x,y is the start of the string (baseline) in the box.
 */

WOOL_OBJECT
WLLabel_get_dimensions(label, box)
WOOL_Label label;
Box	box;	     		/* RETURN the dimensions */
{
    int             up, down, dir;
    XCharStruct     extent;

    XQueryTextExtents(dpy, label -> font,
		 label -> label -> string, strlen(label -> label -> string),
		      &dir, &up, &down, &extent);
    label -> x = DefaultLabelHorizontalMargin - extent.lbearing;
    label -> y = DefaultLabelVerticalMargin + up;
    box -> x = box -> y = 0;
    box -> width = Max(1, extent.width + 2 * DefaultLabelHorizontalMargin);
    box -> height = Max(1, up + down + 2 * DefaultLabelVerticalMargin);
    return NULL;
}

/*
 * drawing a string: we don't need to refresh the background since
 * the font occupies all the space
 */

WOOL_OBJECT
WLLabel_draw(label, wob)
WOOL_Label	label;
Wob		wob;
{
    XSetFont(dpy, Context->gc.Work, label -> font);
    XSetForeground(dpy, Context->gc.Work, label -> color);
    XClearWindow(dpy, wob -> hook);
    XDrawString(dpy, wob -> hook, Context->gc.Work,
		label -> x, label -> y, label -> label -> string,
		strlen(label -> label -> string));
    return NULL;
}

WOOL_OBJECT
WLLabel_open(label, wob)
WOOL_Label label;
Wob wob;
{
    XSetWindowBackground(dpy, wob -> hook, wob ->box.background);
    XClearWindow(dpy,  wob -> hook);
    return (WOOL_OBJECT) ExposureMask;
}

/* since this module has the "know-how" (and the good includes), we define
 * this method for strings here
 */

WOOL_OBJECT
WLString_get_dimensions(string, box)
WOOL_String string;
Box     box;                    /* RETURN the dimensions */
{
    int             up, down, dir;
    XCharStruct     extent;

    XQueryTextExtents(dpy, DefaultFont,
		      string -> string, strlen(string -> string),
		      &dir, &up, &down, &extent);
    box -> x = box -> y = 0;
    box -> width = Max(1, extent.width + 2 * DefaultLabelHorizontalMargin);
    box -> height = Max(1, up + down + 2 * DefaultLabelVerticalMargin);
    return NULL;
}
