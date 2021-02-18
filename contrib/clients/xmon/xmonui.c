/*

Copyright 1991 by OTC Limited


Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of OTC Limited not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission. OTC Limited makes no representations about the
suitability of this software for any purpose. It is provided "as is"
without express or implied warranty.

Any person supplied this software by OTC Limited may make such use of it
including copying and modification as that person desires providing the
copyright notice above appears on all copies and modifications including
supporting documentation.

The only conditions and warranties which are binding on OTC Limited in
respect of the state, quality, condition or operation of this software
are those imposed and required to be binding by statute (including the
Trade Practices Act 1974). and to the extent permitted thereby the
liability, if any, of OTC Limited arising from the breach of such
conditions or warranties shall be limited to and completely discharged
by the replacement of this software and otherwise all other conditions
and warranties whether express or implied by law in respect of the
state, quality, condition or operation of this software which may
apart from this paragraph be binding on OTC Limited are hereby
expressly excluded and negatived.

Except to the extent provided in the paragraph immediately above OTC
Limited shall have no liability (including liability in negligence) to
any person for any loss or damage consequential or otherwise howsoever
suffered or incurred by any such person in relation to the software
and without limiting the generality thereof in particular any loss or
damage consequential or otherwise howsoever suffered or incurred by
any such person caused by or resulting directly or indirectly from any
failure, breakdown, defect or deficiency of whatsoever nature or kind
of or in the software.

*/

/*
 * Project: XMON - An X protocol monitor
 *
 * File: xmonui.c
 *
 * Description: Contains main() for xmondui
 *
 */

#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>

#include "common.h"
#include "widgeti.h"
#include "commands.h"

/* function prototypes: */
/* xmonui.c: */
static void quit_cb P((Widget w , caddr_t client_data , caddr_t call_data ));
static void toggle_cb P((Widget widget , caddr_t client_data , caddr_t
call_data ));
static void list_cb P((Widget widget , caddr_t client_data , caddr_t
call_data ));
static void count_cb P((Widget widget , caddr_t client_data , caddr_t
call_data ));
static Widget build_toggle P((Widget parent , int toggle_index , Widget
vert ));
static Widget build_list P((Widget parent , int list_index , Widget horiz ));
static Widget build_counting_buttons P((Widget parent , Widget vert ,
char *label , char **output_strings ));

/* end function prototypes */

#define NUM_REQUESTS	120

char			*request_monitor_list[NUM_REQUESTS + 1] =
{
#include "requests.h"
};

char			*request_block_list[NUM_REQUESTS + 1] =
{
#include "requests.h"
};

#define NUM_EVENTS  33

char			*event_monitor_list[NUM_EVENTS + 1] =
{
#include "events.h"
};

char			*event_block_list[NUM_EVENTS + 1] =
{
#include "events.h"
};

#define NUM_COUNT_BUTTONS	5

char			*count_names[NUM_COUNT_BUTTONS] =
{
    "start", "stop", "clear", "print", "print zero",
};

char			*request_output_strings[NUM_COUNT_BUTTONS] =
{
    START_REQUEST_COUNT_STR,
    STOP_REQUEST_COUNT_STR,
    CLEAR_REQUEST_COUNT_STR,
    PRINT_REQUEST_COUNT_STR,
    PRINT_ZERO_REQUEST_COUNT_STR,
};

char			*event_output_strings[NUM_COUNT_BUTTONS] =
{
    START_EVENT_COUNT_STR,
    STOP_EVENT_COUNT_STR,
    CLEAR_EVENT_COUNT_STR,
    PRINT_EVENT_COUNT_STR,
    PRINT_ZERO_EVENT_COUNT_STR,
};

char			*error_output_strings[NUM_COUNT_BUTTONS] =
{
    START_ERROR_COUNT_STR,
    STOP_ERROR_COUNT_STR,
    CLEAR_ERROR_COUNT_STR,
    PRINT_ERROR_COUNT_STR,
    PRINT_ZERO_ERROR_COUNT_STR,
};

typedef struct
{
    char		*label;
    char		*toggle_strings;
    char		*output_string;
    char		*default_string;
    Widget		widget;
}
    ToggleData;

#define DETAIL_STR	"off\nnames\nmain\nfull\nhex"

static ToggleData toggledata[] =
{
    {
	"output detail: request ",
	DETAIL_STR, REQUEST_VERBOSE_STR, "off"
    },
    {
	"               event   ",
	DETAIL_STR, EVENT_VERBOSE_STR, "off"
    },
    {
	"               error   ",
	DETAIL_STR, ERROR_VERBOSE_STR, "names"
    },
    {
	"               reply   ",
	DETAIL_STR, REPLY_VERBOSE_STR, "off"
    },
    {
	"show packet size       ",
	"off\non", SHOW_PACKET_SIZE_BASE_STR, "off"
    },
    {
	"detail ",
	DETAIL_STR, SELECTED_REQUEST_VERBOSE_STR, "full"
    },
    {
	"detail ",
	DETAIL_STR, SELECTED_EVENT_VERBOSE_STR, "full"
    },
    {
	" blocking ",
	"off\non", REQUEST_BLOCKING_BASE_STR, "on"
    },
    {
	" blocking ",
	"off\non", EVENT_BLOCKING_BASE_STR, "on"
    },
};

#define NUM_FIRST_TOGGLES	5
#define REQUEST_VERBOSE		5
#define EVENT_VERBOSE		6
#define REQUEST_BLOCKING	7
#define EVENT_BLOCKING		8

#define NUM_TOGGLES (sizeof(toggledata) / sizeof(ToggleData))

typedef struct
{
    char		**list;
    int			toggle_index;
    int			width;
    int			height;
    int			num_items;
    char		*command_base;
    Widget		widget;
}
    ListData;

static ListData listdata[] =
{
    {
	request_monitor_list, REQUEST_VERBOSE, 240, 197, NUM_REQUESTS,
	MONITOR_REQUEST_BASE_STR,
    },
    {
	request_block_list, REQUEST_BLOCKING, 240, 197, NUM_REQUESTS,
	BLOCK_REQUEST_BASE_STR,
    },
    {
	event_monitor_list, EVENT_VERBOSE, 240, 92, NUM_EVENTS,
	MONITOR_EVENT_BASE_STR,
    },
    {
	event_block_list, EVENT_BLOCKING, 240, 92, NUM_EVENTS,
	BLOCK_EVENT_BASE_STR,
    },
};
#define REQUEST_MONITOR_LIST	0
#define REQUEST_BLOCK_LIST	1
#define EVENT_MONITOR_LIST	2
#define EVENT_BLOCK_LIST	3

#define NUM_LISTS (sizeof(listdata) / sizeof(ListData))

static void
quit_cb(w, client_data, call_data)
    Widget		w;
    caddr_t		client_data;
    caddr_t		call_data;
{
    printf("%s\n", QUIT_STR);
    fflush(stdout);
    exit(1);
}

static void
toggle_cb(widget, client_data, call_data)
    Widget		widget;
    caddr_t		client_data;
    caddr_t		call_data;
{
    int			toggle_count;

    if (!(Bool)call_data)
	return;

    for (toggle_count = 0; toggle_count < NUM_TOGGLES; toggle_count++)
	if (toggledata[toggle_count].widget == XtParent(widget))
	{
	    if (toggle_count < 4 || toggle_count == 5 || toggle_count == 6)
	    {
		printf(toggledata[toggle_count].output_string);
		if (Streq((char*) client_data, "off"))
		    printf(" 0\n");
		else if (Streq((char*) client_data, "names"))
		    printf(" 1\n");
		else if (Streq((char*) client_data, "main"))
		    printf(" 2\n");
		else if (Streq((char*) client_data, "full"))
		    printf(" 3\n");
		else if (Streq((char*) client_data, "hex"))
		    printf(" 4\n");
	    }
	    else
		printf
		(
		    "%s%s\n", toggledata[toggle_count].output_string,
		    (char *)client_data
		);
	    fflush(stdout);
	    return;
	}
    printf("error in toggle_cb\n");
    fflush(stdout);
}

static void
list_cb(widget, client_data, call_data)
    Widget		widget;
    caddr_t		client_data;
    caddr_t		call_data;
{
    XawListReturnStruct *return_struct;
    Widget		list_widget;
    char		*selected_string;
    int			list_count;
    Bool		on;

    list_widget = XtParent(widget);
    return_struct = (XawListReturnStruct*) call_data;
    for (list_count = 0; list_count < NUM_LISTS; list_count++)
	if (listdata[list_count].widget == list_widget)
	{
	    selected_string =
		listdata[list_count].list[return_struct->list_index];
	    on = (selected_string[4] == ' ');
	    selected_string[4] = '\0';
	    printf
	    (
		"%s%s %s\n",
		listdata[list_count].command_base,
		(on) ? "on" : "off", selected_string
	    );
	    fflush(stdout);
	    selected_string[4] = (on) ? '*' : ' ';
	    set_viewlist
	    (
		list_widget, listdata[list_count].list,
		listdata[list_count].num_items, listdata[list_count].width
	    );
	    return;
	}
    printf("error in list_cb\n");
    fflush(stdout);
}

static void
count_cb(widget, client_data, call_data)
    Widget		widget;
    caddr_t		client_data;
    caddr_t		call_data;
{
    printf("%s\n", (char *)client_data);
    fflush(stdout);
}

static Widget
build_toggle(parent, toggle_index, vert)
Widget			parent;
int			toggle_index;
Widget			vert;
{
    Widget		widget;

    widget = create_one_of_many_toggle
    (
	toggledata[toggle_index].label, parent, NULL, vert,
	toggledata[toggle_index].toggle_strings, toggle_cb
    );
    toggledata[toggle_index].widget = widget;
    set_toggle_state
    (
	widget, toggledata[toggle_index].default_string
    );
    return widget;
}

static Widget
build_list(parent, list_index, horiz)
Widget			parent;
int			list_index;
Widget			horiz;
{
    Widget		widget;
    Widget		form;


    form = create_form(parent, horiz, NULL, 0);
    widget = build_toggle(form, listdata[list_index].toggle_index, NULL);
    widget = create_viewlist
    (
	form, NULL, widget, listdata[list_index].list,
	listdata[list_index].width, listdata[list_index].height,
	1, list_cb, (Pointer) 0
    );
    listdata[list_index].widget = widget;
    set_viewlist
    (
	widget, listdata[list_index].list,
	listdata[list_index].num_items, listdata[list_index].width
    );
    return form;
}

/*
static void
sel_cb(w, client_data, call_data)
    Widget		w;
    caddr_t		client_data;
    caddr_t		call_data;
{
    Widget		list_widget = (Widget)client_data;
    XawScrollListReturnStruct	*return_struct;

    return_struct = XawScrollListShowCurrent(list_widget);
    if (return_struct->list_index == XAW_SCROLL_LIST_NONE)
	printf("none selected\n");
    else
	printf("item %d selected\n", return_struct->list_index);
    fflush(stdout);
    XawScrollListUnhighlight(list_widget);
}

    widget = create_command_button
    (
	widget, NULL, list, "change", change_cb, list_widget
    );
    widget = create_command_button
    (
	widget, NULL, widget, "print selected", sel_cb, list_widget
    );
*/

static Widget
build_counting_buttons(parent, vert, label, output_strings)
Widget			parent;
Widget			vert;
char			*label;
char			**output_strings;
{
    Widget		widget;
    Widget		form;
    char		string[100];
    int			button;

    form = create_form(parent, NULL, vert, 0);
    widget = create_label(form, NULL, NULL, 0, 0, label);
    for (button = 0; button < NUM_COUNT_BUTTONS; button++)
	widget = create_command_button
	(
	    form, widget, NULL, count_names[button], count_cb,
	    output_strings[button]
	);
    return form;
}

Global int
main(argc, argv)
int argc;
char **argv;
{
    Widget		toplevel;
    Widget		form;
    Widget		inner_form;
    Widget		lists_form;
    Widget		widget;
    Widget		list_widget;
    int			toggle_count;

    toplevel = init_widgeti("XmonUI", "XmonUI", &argc, argv);
    form = create_form(toplevel, NULL, NULL, 0);

/* quit button */
    widget = create_command_button(form, NULL, NULL, "quit", quit_cb,NULL);

/* header */
    widget = create_label
    (
	form, widget, NULL, 0,0,"               Xmon - X protocol monitor"
    );


/* verbose level toggles */
    for (toggle_count = 0; toggle_count < NUM_FIRST_TOGGLES; toggle_count++)
	widget = build_toggle(form, toggle_count, widget);

/* counting buttons */
    widget = build_counting_buttons
    (
	form, widget,	"counting:    requests", request_output_strings
    );
    widget = build_counting_buttons
    (
	form, widget, "             events  ", event_output_strings
    );
    widget = build_counting_buttons
    (
	form, widget, "             errors  ", error_output_strings
    );


/* selected requests section */
    inner_form = create_form(form, NULL, widget, 1);
    widget = create_label(inner_form, NULL, NULL, 0, 0,"selected requests:");
    lists_form = create_form(inner_form, NULL, widget, 0);
    widget = build_list(lists_form, REQUEST_MONITOR_LIST, NULL);
    widget = build_list(lists_form, REQUEST_BLOCK_LIST, widget);

/* selected events section */
    inner_form = create_form(form, NULL, inner_form, 1);
    widget = create_label(inner_form, NULL, NULL, 0, 0, "selected events:");
    lists_form = create_form(inner_form, NULL, widget, 0);
    widget = build_list(lists_form, EVENT_MONITOR_LIST, NULL);
    widget = build_list(lists_form, EVENT_BLOCK_LIST, widget);

    XtRealizeWidget(toplevel);
    XtMainLoop();
}
