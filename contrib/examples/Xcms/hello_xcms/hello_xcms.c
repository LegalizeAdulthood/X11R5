/*
 *
 * Copyright 1991 Tektronix, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Tektronix, Inc. not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission. Tektronix makes no representations about
 * the suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Tektronix, Inc. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO 
 * EVENT SHALL Tektronix BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 *
 *	NAME
 *		 hello_xcms -- Another version of hello_color
 *
 *	DESCRIPTION
 *		This demonstrates some of the calls of Xcms and
 *		the InterClient Color Exchange Convention used
 *		in the TekColor Editor (xtici).
 *
 *	HISTORY
 *		
 *
 *	 HISTORY END
 *
 */
#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: $";
#endif /* RCS_ID */
#endif /* LINT */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Form.h>
#include "hello_xcms.h"

#define LABEL_TEXT	"Hello\nXcms"
#define LABEL_WIDTH	200
#define LABEL_TEXT_1	"1"
#define LABEL_WIDTH_1	20
#define LABEL_TEXT_2	"2"
#define LABEL_WIDTH_2	20
#define LABEL		0
#define LABEL_1		1
#define LABEL_2		2

#define ColorCt	2

Colormap	myMap;
Pixel		back, fore, back_1, fore_1, back_2, fore_2;
Atom		ColorExchangeProp, ColorChangeTargetProp;
Atom		ColorChangeMessageAtom, ColorChangeSelectionAtom;
Atom		ColorChangeTargetAtom;
Widget		label, label1, label2;
Widget		toplevel, primary;
int		whichLabel;
CmsChangeStruct	ColorChangeData;

/*
 *  Change the background and foreground colors to the values found in from
 *  Could also keep the same index and change the colormap to get the same
 *  effect.  However, I would need to be careful using the passed colormap,
 *  because it will continue to be used to edit this and other colors.
 */
/* ARGSUSED */
void ChangeColors(w, client_data, selection, type, value, length, format)
    Widget w;
    XtPointer client_data;
    Atom *selection;
    Atom *type;
    XtPointer *value;
    unsigned long *length;
    int *format;
{
    CmsChangeStruct *change_data = (CmsChangeStruct *)client_data;
    Display         *dpy = XtDisplay(w);
    Colormap        editors_cmap = (Colormap)*value;
    XColor          mine[ColorCt];
    Pixel           last[ColorCt], localback, localfore;
    Arg             args[ColorCt];
    int             n;

    if ((*selection != ColorChangeSelectionAtom) 
        || (*type != ColorChangeTargetAtom)) {
	    return;
    }

    /* assert((*length == 1) && (*format == 32)); */
    switch (whichLabel) {
	case LABEL:
	    localback = last[0] = mine[0].pixel = back;
	    localfore = last[1] = mine[1].pixel = fore;
	    break;
	case LABEL_1:
	    localback = last[0] = mine[0].pixel = back_1;
	    localfore = last[1] = mine[1].pixel = fore_1;
	    break;
	case LABEL_2:
	    localback = last[0] = mine[0].pixel = back_2;
	    localfore = last[1] = mine[1].pixel = fore_2;
	    break;
	default:
	    break;
    }        

    mine[0].flags = DoRed | DoGreen | DoBlue;    
    mine[1].flags = DoRed | DoGreen | DoBlue;
    XQueryColors(dpy, editors_cmap, mine, ColorCt);

    /* Could query existing colors and check for a difference */
    /* appears that colors can always be freed, even if different */
    if (ColorChangeData.min != ColorChangeData.max) {
    	if (XAllocColor(dpy, myMap, mine) != 0)
		localback = mine[0].pixel;
    	if (XAllocColor(dpy, myMap, mine+1) != 0)
		localfore = mine[1].pixel;

    	/* XFreeColors(dpy, myMap, &last[0], ColorCt, 0); */

    	XtSetArg(args[0], XtNbackground, localback);
    	XtSetArg(args[1], XtNforeground, localfore);
	n = 2;
    } else if ((Pixel)ColorChangeData.min == localback) {
    	if (XAllocColor(dpy, myMap, mine) != 0)
		localback = mine[0].pixel;

    	/* XFreeColors(dpy, myMap, &last[0], 1, 0); */

    	XtSetArg(args[0], XtNbackground, localback);
	n = 1;
    } else if ((Pixel)change_data->min == localfore) {
    	if (XAllocColor(dpy, myMap, mine+1) != 0)
		localfore = mine[1].pixel;

    	/* XFreeColors(dpy, myMap, &last[1], 1, 0); */

    	XtSetArg(args[0], XtNforeground, localfore);
	n = 1;
    } else {
	return;
    }
    
    XtSetValues(w, args, n);

    switch (whichLabel) {
	case LABEL:
	    back = localback;
	    fore = localfore;
#ifdef XDEBUG
	    printf ("background, foreground for LABEL = (%d, %d)\n",
		    back, fore);
#endif
	    break;
	case LABEL_1:
	    back_1 = localback;
	    fore_1 = localfore;
#ifdef XDEBUG
	    printf ("background, foreground for LABEL 1 = (%d, %d)\n",
		    back_1, fore_1);
#endif
	    break;
	case LABEL_2:
	    back_2 = localback;
	    fore_2 = localfore;
#ifdef XDEBUG
	    printf ("background, foreground for LABEL 2 = (%d, %d)\n",
		    back_2, fore_2);
#endif
	    break;
	default:
	    break;
    }        
}

/*
 *  Create the atoms we will be using to communicate with the 
 *  color editor.  The strings used to define atoms should be
 *  in a common header file somewhere.
 */
void CreateAtoms(w)
Widget w;
{
    Display *dpy = XtDisplay(w);

    ColorChangeMessageAtom   = XInternAtom(dpy, CMS_MessageName, False);
    ColorChangeSelectionAtom = XInternAtom(dpy, CMS_SelectionName, False);
    ColorExchangeProp        = XInternAtom(dpy, CMS_ExchangeName, False);
    ColorChangeTargetAtom    = XInternAtom(dpy, CMAP, False);
    /* Target property can be anything, so reuse an existing atom */
    ColorChangeTargetProp    = ColorChangeSelectionAtom;
}

/* 
 *  Get window geometry in terms of the root window for the widget passed.
 */
Status GetWinGeometry (pDpy, w, pRoot, pX, pY, pW, pH, pBW, pD)
    Display      *pDpy;
    Widget       w;
    Window       *pRoot;
    int          *pX, *pY;
    unsigned int *pW, *pH, *pBW, *pD;
{
    Window win = XtWindow (w);
    Window root, parent, children;
    int x, y;
    unsigned int _w, h, bw, d, nchild;

    if (!XGetGeometry (pDpy, win, pRoot, pX, pY, pW, pH, pBW, pD)) {
	return (0);
    }
    while (XQueryTree (pDpy, win, &root, &parent, &children, &nchild) &&
	   parent) {
	XFree ((char *)children);
	if (XGetGeometry (pDpy, parent, &root, &x, &y, &_w, &h, &bw, &d)) {
	    *pX += x;
	    *pY += y;
	    win = parent;
	} else {
	    return (0);
	}
    }
    return (1);
}
/*
 *  If event is user message with the correct type,
 *  request the CMS_SelectionName ColorChangeSelectionAtom value,
 *  query and reset colors of interest to those values.
 *  Refer to ICCCM section 2.4 for the description of how/why this is done.
 *  If not, return without action.
 */
/* ARGSUSED */
void UseMessage(w, child, e)
Widget w;
Widget child;
XClientMessageEvent *e;
{
    void XtTranslateCoords ();
    Display *dpy = XtDisplay(toplevel);
    Window root;
    long lx, ly, l1x, l1y, l2x, l2y;
    unsigned long lw, lh, lbw, ld, l1w, l1h, l1bw, l1d, l2w, l2h, l2bw, l2d;

    if (e->type != ClientMessage || e->message_type != ColorChangeMessageAtom)
	return;

    (void) GetWinGeometry (dpy, label, &root, 
			   (int *)&lx, (int *)&ly, 
			   (unsigned int *)&lw, (unsigned int *)&lh, 
			   (unsigned int *)&lbw, (unsigned int *)&ld);
    (void) GetWinGeometry (dpy, label1, &root, 
			   (int *)&l1x, (int *)&l1y, 
			   (unsigned int *)&l1w, (unsigned int *)&l1h, 
			   (unsigned int *)&l1bw, (unsigned int *)&l1d);
    (void) GetWinGeometry (dpy, label2, &root, 
			   (int *)&l2x, (int *)&l2y, 
			   (unsigned int *)&l2w, (unsigned int *)&l2h, 
			   (unsigned int *)&l2bw, (unsigned int *)&l2d);

    ColorChangeData.when = e->data.l[0];
    ColorChangeData.min = (Pixel)e->data.l[1];
    ColorChangeData.max = (Pixel)e->data.l[2];
    ColorChangeData.x_root = e->data.l[3];
    ColorChangeData.y_root = e->data.l[4];

    if (ColorChangeData.x_root >= lx && ColorChangeData.x_root <= lx + lw && 
	ColorChangeData.y_root >= ly && ColorChangeData.y_root <= ly + lh) {
	whichLabel = LABEL;
    } else if (ColorChangeData.x_root >= l1x && ColorChangeData.x_root <= l1x + l1w && 
	ColorChangeData.y_root >= l1y && ColorChangeData.y_root <= l1y + l1h) {
	whichLabel = LABEL_1;
    } else if (ColorChangeData.x_root >= l2x && ColorChangeData.x_root <= l2x + l2w && 
	ColorChangeData.y_root >= l2y && ColorChangeData.y_root <= l2y + l2h) {
	whichLabel = LABEL_2;
    } else {
	whichLabel = -1;
    }

    /* 
     * This code will determine which widget we are in based on a location
     * This could be done in a simpler manner but this uses the capabilities
     * of the ICCEC protocol.
     */
    
    switch (whichLabel) {
	case LABEL:
	    XtGetSelectionValue(label, ColorChangeSelectionAtom, 
			        ColorChangeTargetAtom, ChangeColors, 
				&ColorChangeData, ColorChangeData.when);
	    break;
	case LABEL_1:
	    XtGetSelectionValue(label1, ColorChangeSelectionAtom, 
				ColorChangeTargetAtom, ChangeColors, 
				&ColorChangeData, ColorChangeData.when);
	    break;
	case LABEL_2:
	    XtGetSelectionValue(label2, ColorChangeSelectionAtom, 
				ColorChangeTargetAtom, ChangeColors, 
				&ColorChangeData, ColorChangeData.when);
	    break;
	default:
	    break;
    }
}

main(argc, argv)
    int argc;
    char **argv;
{
    Arg	        wargs[10];
    int		argCt;

    /*
     * Create the Widget that represents the window.
     */
    toplevel = XtInitialize(argv[0], "Xhx", NULL, 0, &argc, argv);

    /*
     *  Create a form widget to hold the three label widgets.
     */
    argCt = 0;
    /* XtSetArg(wargs[argCt], XtNwidth, 
		LABEL_WIDTH+LABEL_WIDTH_1+LABEL_WIDTH_2);   argCt++*/
    primary = XtCreateManagedWidget("primary", formWidgetClass, toplevel, 
				    wargs, argCt);

    /*
     * Create a set of label widgets to display the string,  using wargs to set
     * the string as its value.
     */
    argCt = 0;
    XtSetArg(wargs[argCt], XtNlabel, LABEL_TEXT_1);	    argCt++;
    /* XtSetArg(wargs[argCt], XtNwidth, LABEL_WIDTH_1);	    argCt++; */
    label1 = XtCreateManagedWidget("label1", labelWidgetClass, primary,
				   wargs, argCt);

    argCt = 0;
    XtSetArg(wargs[argCt], XtNlabel, LABEL_TEXT);	    argCt++;
    /* XtSetArg(wargs[argCt], XtNwidth, LABEL_WIDTH);	    argCt++; */
    XtSetArg(wargs[argCt], XtNfromHoriz, (XtArgVal)label1); argCt++;
    label = XtCreateManagedWidget("label", labelWidgetClass, primary,
				  wargs, argCt);

    argCt = 0;
    XtSetArg(wargs[argCt], XtNlabel, LABEL_TEXT_2);	    argCt++;
    /* XtSetArg(wargs[argCt], XtNwidth, LABEL_WIDTH_2);	    argCt++; */
    XtSetArg(wargs[argCt], XtNfromHoriz, (XtArgVal)label);  argCt++;
    label2 = XtCreateManagedWidget("label2", labelWidgetClass, primary,
				   wargs, argCt);

    /*
     * Create the windows, and set their attributes according
     * to the Widget data.
     */
    XtRealizeWidget(toplevel);

    /*
     * Find out what colors each widget is using
     */
    XtSetArg(wargs[0], XtNbackground, &back);
    XtSetArg(wargs[1], XtNforeground, &fore);
    XtSetArg(wargs[2], XtNcolormap, &myMap);
    XtGetValues(label, wargs, 3);

    XtSetArg(wargs[0], XtNbackground, &back_1);
    XtSetArg(wargs[1], XtNforeground, &fore_1);
    XtGetValues(label1, wargs, 2);

    XtSetArg(wargs[0], XtNbackground, &back_2);
    XtSetArg(wargs[1], XtNforeground, &fore_2);
    XtGetValues(label2, wargs, 2);

    /*
     * Create atoms that are used to communicate with color editor.
     */
    CreateAtoms(label);
    /*
     * Mark window to indicate that client is prepared to receive color
     * change requests.  Type and content of property is undefined,
     * just use anything.  It is only important that the property exist.
     */
    XChangeProperty(XtDisplay(toplevel), XtWindow(toplevel), 
		    ColorExchangeProp, ColorChangeTargetAtom, 
		    32, PropModeReplace, NULL, 0);

    /* Add an event handler to look for user message from editor */
    XtAddEventHandler(toplevel, 0, True, UseMessage, primary);

    /*
     * Now process the events.
     */
   XtMainLoop();
}
