/* $XConsortium: Vendor.c,v 1.21 91/07/30 15:29:56 rws Exp $ */

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 * This is a copy of Xt/Vendor.c with an additional ClassInitialize
 * procedure to register Xmu resource type converters.
 *
 */

/* Make sure all wm properties can make it out of the resource manager */

#include <stdio.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/ShellP.h>
#include <X11/Vendor.h>
#include <X11/VendorP.h>
#include <X11/Xmu/Converters.h>
#include <X11/Xmu/Atoms.h>
#include <X11/Xmu/Editres.h>
#if defined(IM)
#include <X11/Xaw/VendorEP.h>
#include <X11/Xaw/XawIm.h>
#include <X11/Xaw/XawImP.h>
#endif /* defined(IM) */

static XtResource resources[] = {
  {XtNinput, XtCInput, XtRBool, sizeof(Bool),
		XtOffsetOf(VendorShellRec, wm.wm_hints.input),
		XtRImmediate, (XtPointer)True}
};

/***************************************************************************
 *
 * Vendor shell class record
 *
 ***************************************************************************/

static void XawVendorShellClassInitialize();
static void XawVendorShellInitialize();
static Boolean XawVendorShellSetValues();
static void Realize(), ChangeManaged();
#if defined(IM)
void XawVendorShellExtResize();
#endif	/* defined(IM) */

#define SuperClass (&wmShellClassRec)
externaldef(vendorshellclassrec) VendorShellClassRec vendorShellClassRec = {
  {
    /* superclass	  */	(WidgetClass)SuperClass,
    /* class_name	  */	"VendorShell",
    /* size		  */	sizeof(VendorShellRec),
    /* class_initialize	  */	XawVendorShellClassInitialize,
    /* class_part_initialize*/	NULL,
    /* Class init'ed ?	  */	FALSE,
    /* initialize	  */	XawVendorShellInitialize,
    /* initialize_hook	  */	NULL,		
    /* realize		  */	Realize,
    /* actions		  */	NULL,
    /* num_actions	  */	0,
    /* resources	  */	resources,
    /* resource_count	  */	XtNumber(resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	FALSE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/	FALSE,
    /* visible_interest	  */	FALSE,
    /* destroy		  */	NULL,
#if defined(IM)
    /* resize		  */	XawVendorShellExtResize,
#else
    /* resize		  */	XtInheritResize,
#endif
    /* expose		  */	NULL,
    /* set_values	  */	XawVendorShellSetValues,
    /* set_values_hook	  */	NULL,			
    /* set_values_almost  */	XtInheritSetValuesAlmost,  
    /* get_values_hook	  */	NULL,
    /* accept_focus	  */	NULL,
    /* intrinsics version */	XtVersion,
    /* callback offsets	  */	NULL,
    /* tm_table		  */	NULL,
    /* query_geometry	  */	NULL,
    /* display_accelerator*/	NULL,
    /* extension	  */	NULL
  },{
    /* geometry_manager	  */	XtInheritGeometryManager,
    /* change_managed	  */	ChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension	  */	NULL
  },{
    /* extension	  */	NULL
  },{
    /* extension	  */	NULL
  },{
    /* extension	  */	NULL
  }
};

externaldef(vendorshellwidgetclass) WidgetClass vendorShellWidgetClass =
	(WidgetClass) (&vendorShellClassRec);

#if defined(IM)

static XtResource ext_resources[] = {
  {XtNinputMethod, XtCInputMethod, XtRString, sizeof(String),
		XtOffsetOf(XawVendorShellExtRec, vendor_ext.im.input_method),
		XtRString, (XtPointer)NULL},
  {XtNpreeditType, XtCPreeditType, XtRString, sizeof(String),
		XtOffsetOf(XawVendorShellExtRec, vendor_ext.im.preedit_type),
		XtRString, (XtPointer)NULL}
};

/***************************************************************************
 *
 * Vendor shell Extension class record
 *
 ***************************************************************************/

static void XawVendorShellExtClassInitialize();
static void XawVendorShellExtInitialize();
static Boolean XawVendorShellExtSetValues();
static void ExtRealize();

externaldef(vendorshellextclassrec) XawVendorShellExtClassRec
       xawvendorShellExtClassRec = {
  {
    /* superclass	  */	(WidgetClass)&coreClassRec,
    /* class_name	  */	"VendorShellExt",
    /* size		  */	sizeof(XawVendorShellExtRec),
    /* class_initialize	  */	XawVendorShellExtClassInitialize,
    /* class_part_initialize*/	NULL,
    /* Class init'ed ?	  */	FALSE,
    /* initialize	  */	XawVendorShellExtInitialize,
    /* initialize_hook	  */	NULL,		
    /* realize		  */	ExtRealize,
    /* actions		  */	NULL,
    /* num_actions	  */	0,
    /* resources	  */	ext_resources,
    /* resource_count	  */	XtNumber(ext_resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	FALSE,
    /* compress_exposure  */	FALSE,
    /* compress_enterleave*/	FALSE,
    /* visible_interest	  */	FALSE,
    /* destroy		  */	NULL,
    /* resize		  */	XtInheritResize,
    /* expose		  */	NULL,
    /* set_values	  */	XawVendorShellExtSetValues,
    /* set_values_hook	  */	NULL,			
    /* set_values_almost  */	XtInheritSetValuesAlmost,  
    /* get_values_hook	  */	NULL,
    /* accept_focus	  */	NULL,
    /* intrinsics version */	XtVersion,
    /* callback offsets	  */	NULL,
    /* tm_table		  */	NULL,
    /* query_geometry	  */	NULL,
    /* display_accelerator*/	NULL,
    /* extension	  */	NULL
  },{
    /* extension	  */	NULL
  }
};

externaldef(xawvendorshellwidgetclass) WidgetClass
     xawvendorShellExtWidgetClass = (WidgetClass) (&xawvendorShellExtClassRec);
#endif	/* defined(IM) */

/*ARGSUSED*/
static Boolean
XawCvtCompoundTextToString(dpy, args, num_args, fromVal, toVal, cvt_data)
Display *dpy;
XrmValuePtr args;
Cardinal    *num_args;
XrmValue *fromVal;
XrmValue *toVal;
XtPointer *cvt_data;
{
    XTextProperty prop;
    char **list;
    int count;
    static char *mbs = NULL;
    int len;

    prop.value = (unsigned char *)fromVal->addr;
    prop.encoding = XA_COMPOUND_TEXT(dpy);
    prop.format = 8;
    prop.nitems = fromVal->size;

    if(XmbTextPropertyToTextList(dpy, &prop, &list, &count) != Success) {
	XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
	"converter", "XmbTextPropertyToTextList", "XawError",
	"conversion from CT to MB failed.", NULL, 0);
	return False;
    }
    toVal->size = len;
    len = strlen(*list);
    mbs = XtRealloc(mbs, len + 1); /* keep buffer because no one call free :-( */
    strcpy(mbs, *list);
    XFreeStringList(list);
    toVal->addr = (XtPointer)mbs;
    return True;
}

static void XawVendorShellClassInitialize()
{
    static XtConvertArgRec screenConvertArg[] = {
        {XtWidgetBaseOffset, (XtPointer) XtOffsetOf(WidgetRec, core.screen),
	     sizeof(Screen *)}
    };

    XtAddConverter(XtRString, XtRCursor, XmuCvtStringToCursor,      
		   screenConvertArg, XtNumber(screenConvertArg));

    XtAddConverter(XtRString, XtRBitmap, XmuCvtStringToBitmap,
		   screenConvertArg, XtNumber(screenConvertArg));

    XtSetTypeConverter("CompoundText", XtRString, XawCvtCompoundTextToString,
			NULL, 0, XtCacheNone, NULL);
}

/* ARGSUSED */
#if defined(IM)
static void XawVendorShellInitialize(req, new, args, num_args)
	Widget req, new;
	ArgList     args;
	Cardinal    *num_args;
#else
static void XawVendorShellInitialize(req, new)
	Widget req, new;
#endif
{
    XtAddEventHandler(new, (EventMask) 0, TRUE, _XEditResCheckMessages, NULL);
#if defined(IM)
    XtCreateWidget("shellext", xawvendorShellExtWidgetClass,
		   new, args, *num_args);
#endif
}

/* ARGSUSED */
static Boolean XawVendorShellSetValues(old, ref, new)
	Widget old, ref, new;
{
	return FALSE;
}

static void Realize(wid, vmask, attr)
	Widget wid;
	Mask *vmask;
	XSetWindowAttributes *attr;
{
	WidgetClass super = wmShellWidgetClass;

	/* Make my superclass do all the dirty work */

	(*super->core_class.realize) (wid, vmask, attr);
#if defined(IM)
	_XawImRealize(wid);
#endif	/* defined(IM) */
}


#if defined(IM)
static void XawVendorShellExtClassInitialize()
{
}

/* ARGSUSED */
static void XawVendorShellExtInitialize(req, new)
        Widget req, new;
{
    _XawImInitialize(new->core.parent, new);
}

/* ARGSUSED */
static Boolean XawVendorShellExtSetValues(old, ref, new)
	Widget old, ref, new;
{
	return FALSE;
}

static void ExtRealize(wid, vmask, attr)
	Widget wid;
	Mask *vmask;
	XSetWindowAttributes *attr;
{
}

void XawVendorShellExtResize(wid)
    Widget wid;
{
	ShellWidget sw = (ShellWidget) wid;
	Widget childwid;
	int i;
	int core_height;

	_XawImResizeVendorShell(sw);
	core_height = _XawGetImShellHeight(sw);
	for(i = 0; i < sw->composite.num_children; i++) {
	    if(XtIsManaged(sw->composite.children[i])) {
		childwid = sw->composite.children[i];
		XtResizeWidget(childwid, sw->core.width, core_height,
			       childwid->core.border_width);
	    }
	}
}

#endif	/* defined(IM) */

static void ChangeManaged(wid)
	Widget wid;
{
	ShellWidget w = (ShellWidget) wid;
	Widget* childP;
	int i;

	(*SuperClass->composite_class.change_managed)(wid);
	for (i = w->composite.num_children, childP = w->composite.children;
	     i; i--, childP++) {
	    if (XtIsManaged(*childP)) {
		XtSetKeyboardFocus(wid, *childP);
		break;
	    }
	}
}
