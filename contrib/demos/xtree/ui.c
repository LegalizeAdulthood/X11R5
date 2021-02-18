/*
 * $XConsortium: ui.c,v 1.3 90/05/08 14:06:50 jim Exp $
 *
 * Copyright 1990 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Jim Fulton, MIT X Consortium
 */

#include "xtree.h"
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Porthole.h>
#include <X11/Xaw/Panner.h>
#include <X11/Xaw/Reports.h>
#include <X11/Xaw/Tree.h>
#include <ctype.h>

static Widget formWidget, portholeWidget, pannerWidget, treeWidget;


/*
 * panner/porthole controls - called when the other changes
 */
/* ARGSUSED */
static void panner_callback (gw, closure, data)
    Widget gw;                          /* panner widget */
    caddr_t closure;                    /* porthole widget */
    caddr_t data;                       /* report */
{
    XawPannerReport *rep = (XawPannerReport *) data;
    Arg args[2];

    if (!treeWidget) return;

    XtSetArg (args[0], XtNx, -rep->slider_x);
    XtSetArg (args[1], XtNy, -rep->slider_y);
    XtSetValues (treeWidget, args, TWO);        /* just assume... */
}

/* ARGSUSED */
static void porthole_callback (gw, closure, data)
    Widget gw;                          /* porthole widget */
    caddr_t closure;                    /* panner widget */
    caddr_t data;                       /* report */
{
    Widget panner = (Widget) closure;
    XawPannerReport *rep = (XawPannerReport *) data;
    Arg args[6];
    Cardinal n = TWO;
    Window raise = None, unmap = None;
    static Boolean unmapped = TRUE;
    Window pwin = XtWindow(panner);

    XtSetArg (args[0], XtNsliderX, rep->slider_x);
    XtSetArg (args[1], XtNsliderY, rep->slider_y);
    if (rep->changed != (XawPRSliderX | XawPRSliderY)) {
        XtSetArg (args[2], XtNsliderWidth, rep->slider_width);
        XtSetArg (args[3], XtNsliderHeight, rep->slider_height);
        XtSetArg (args[4], XtNcanvasWidth, rep->canvas_width);
        XtSetArg (args[5], XtNcanvasHeight, rep->canvas_height);
        n = SIX;
	raise = pwin;
	if (rep->slider_width >= rep->canvas_width &&
	    rep->slider_height >= rep->canvas_height)
	  unmap = pwin;
    }
    XtSetValues (panner, args, n);
    if (unmap) {
	XUnmapWindow (XtDisplay(panner), unmap);
	unmapped = TRUE;
    } else {
	if (unmapped && pwin) {
	    XMapWindow (XtDisplay(panner), pwin);
	    unmapped = FALSE;
	}
	if (raise) 
	  XRaiseWindow (XtDisplay(panner), raise);
    }
}


void create_ui ()
{
    Arg args[2];
    static XtCallbackRec callback_rec[2] = {{ NULL, NULL }, { NULL, NULL }};
    Widget pane, box;

    pane = XtCreateManagedWidget ("pane", panedWidgetClass, toplevel,
				  (ArgList) NULL, ZERO);
    box = XtCreateManagedWidget ("buttonbox", boxWidgetClass, pane,
				  (ArgList) NULL, ZERO);
    (void) XtCreateManagedWidget ("quit", commandWidgetClass, box,
				  (ArgList) NULL, ZERO);
    formWidget = XtCreateManagedWidget ("form", formWidgetClass, pane,
					(ArgList) NULL, ZERO);

    XtSetArg (args[0], XtNbackgroundPixmap, None);  /* faster updates */
    portholeWidget = XtCreateManagedWidget ("porthole", portholeWidgetClass, 
					    formWidget, args, ONE);
    pannerWidget = XtCreateManagedWidget ("panner", pannerWidgetClass,
					  formWidget,
                                    (ArgList) NULL, ZERO);

    XtSetArg (args[0], XtNreportCallback, callback_rec);
    callback_rec[0].callback = (XtCallbackProc) panner_callback;
    callback_rec[0].closure = (XtPointer) portholeWidget;
    XtSetArg (args[1], XtNmappedWhenManaged, FALSE);
    XtSetValues (pannerWidget, args, TWO);

    callback_rec[0].callback = (XtCallbackProc) porthole_callback;
    callback_rec[0].closure = (XtPointer) pannerWidget;
    XtSetValues (portholeWidget, args, ONE);

    treeWidget = XtCreateManagedWidget ("tree", treeWidgetClass,
                                        portholeWidget, (ArgList) NULL, ZERO);
}


void adjust_panner ()
{
    Arg args[4];
    Dimension canvasWidth, canvasHeight, sliderWidth, sliderHeight;

    /*
     * Realize the tree, but do not map it (we set mappedWhenManaged to
     * false up above).  Get the initial size of the tree so that we can
     * size the panner appropriately.
     */
    XtRealizeWidget (toplevel);

    XtSetArg (args[0], XtNwidth, &canvasWidth);
    XtSetArg (args[1], XtNheight, &canvasHeight);
    XtGetValues (treeWidget, args, TWO);

    XtSetArg (args[0], XtNwidth, &sliderWidth);
    XtSetArg (args[1], XtNheight, &sliderHeight);
    XtGetValues (portholeWidget, args, TWO);

    XtSetArg (args[0], XtNcanvasWidth, canvasWidth);
    XtSetArg (args[1], XtNcanvasHeight, canvasHeight);
    XtSetArg (args[2], XtNsliderWidth, sliderWidth);
    XtSetArg (args[3], XtNsliderHeight, sliderHeight);
    XtSetValues (pannerWidget, args, FOUR);

    XRaiseWindow (XtDisplay(pannerWidget), XtWindow(pannerWidget));
}


static void print_resource_name (node, height)
    Node *node;
    int height;
{
    if (!node || node == &rootnode) return;
    print_resource_name (node->parent, height + 1);
    fputs (node->name, stdout);
    if (height > 0) putchar ('.');
}

/* ARGSUSED */
static void button_callback (gw, closure, data)
    Widget gw;
    caddr_t closure;			/* Node */
    caddr_t data;
{
    Node *node = (Node *) closure, *cur;
    int depth;

    switch (GlobalResources.output_format) {
      case FORMAT_NAME:
	puts (node->name);
	break;

      case FORMAT_RESOURCE:
	print_resource_name (node, 0);
	putchar ('\n');
	break;

      case FORMAT_NUMBER:
	for (cur = node, depth = 0; cur != &rootnode;
	     cur = cur->parent, depth++) ;
	printf ("%d: %s\n", depth - 1 + GlobalResources.number_origin,
		node->name);
	break;

      case FORMAT_SEPARATOR:
	for (cur = node; cur != &rootnode; cur = cur->parent) {
	    fputs (GlobalResources.separator, stdout);
	}
	puts (node->name);
	break;
    }
}

void build_tree (node, parent, show)
    Node *node;
    Widget parent;
    Boolean show;
{
    Arg args[3];
    Widget w;
    char nodename[256], *src, *dst;
    int i;
    static XtCallbackRec callback_rec[2] = {{ button_callback, NULL },
					    { NULL, NULL }};


    if (!node) return;

    if (show) {
	/*
	 * generate the widget name by removing all non-alphanumerics
	 */
	nodename[0] = '\0';
	for (i = 0, src = node->name, dst = nodename;
	     *src && i < sizeof nodename; i++, src++) {
	    if (isascii(*src) && isalnum(*src)) *dst++ = *src;
	}
	nodename[(sizeof nodename) - 1] = '\0';
	XtSetArg (args[0], XtNtreeParent, parent);
	XtSetArg (args[1], XtNlabel, node->name);
	XtSetArg (args[2], XtNcallback, callback_rec);
	callback_rec[0].closure = (caddr_t) node;
	w = XtCreateManagedWidget (nodename, commandWidgetClass, treeWidget, 
				   args, THREE);
    } else {
	w = (Widget) NULL;
    }

    for (node = node->children; node; node = node->siblings) {
	build_tree (node, w, TRUE);
    }
}


void circulate_panner_tree ()
{
    XCirculateSubwindowsDown (XtDisplay(formWidget), XtWindow(formWidget));
}
