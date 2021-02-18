/* 
 * XDPSshare.c
 *
 * Copyright (C) 1990, 1991 by Adobe Systems Incorporated.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notices appear in all copies and that
 * both those copyright notices and this permission notice appear in
 * supporting documentation and that the name of Adobe Systems
 * Incorporated not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  If any portion of this software is changed, it cannot be
 * marketed under Adobe's trademarks and/or copyrights unless Adobe, in
 * its sole discretion, approves by a prior writing the quality of the
 * resulting implementation.
 * 
 * ADOBE MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THE SOFTWARE FOR
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
 * ADOBE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO EVENT SHALL ADOBE BE LIABLE
 * TO YOU OR ANY OTHER PARTY FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE, STRICT LIABILITY OR ANY OTHER ACTION ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  ADOBE WILL NOT
 * PROVIDE ANY TRAINING OR OTHER SUPPORT FOR THE SOFTWARE.
 * 
 * PostScript, Display PostScript, and Adobe are trademarks of Adobe Systems
 * Incorporated registered in the U.S.A. and other countries.
 *
 * Author: Adobe Systems Incorporated
 */

#include <X11/Xlib.h>
#include <DPS/dpsXclient.h>
#include <DPS/dpsops.h>
#include "DPS/dpsXshare.h"
#include "XDPSswraps.h"
#include "dpsXcommonI.h"
#include <stdio.h>

/* 
   Alloc...Info allocates an info entry and stores it at the head of the list.
   Find...Info looks for an info entry and returns NULL if not found.
   Lookup...Info looks for an info entry and creates one if not found.
*/

void _DPSSSetContextParameters(), _DPSSInstallDPSlibDict();

typedef struct _ContextInfoRec {
    DPSContext context;
    DPSContext text;
    Bool enableText;
    unsigned long initFlags;
    Display *display;
    struct _DisplayInfoRec *displayInfo;
    struct _ContextInfoRec *next;
} ContextInfoRec, *ContextInfo;
   
static ContextInfo contextList = NULL;

typedef enum {ext_yes, ext_no, ext_no_idea} ExtensionStatus;

typedef struct _DisplayInfoRec {
    Display *display;
    ExtensionStatus extensionPresent;
    ContextInfo contextInfo;
    int *depthsForScreen;
    int **validDepths;
    GC **gcForDepth;
    struct _DisplayInfoRec *next;
} DisplayInfoRec, *DisplayInfo;

/* If a display is in displayList, it means that we have looked to see if
   the extension exists on the display.  If contextInfo is not NULL, the
   display has a default context associated with it. */

static DisplayInfo displayList = NULL;

static DisplayInfo FindDisplayInfo(), LookupDisplayInfo();

static ContextInfo AllocContextInfo(context)
    DPSContext context;
{
    ContextInfo c = (ContextInfo) malloc(sizeof(ContextInfoRec));

    bzero((char *)c, sizeof(ContextInfoRec));
    c->context = context;
    c->next = contextList;
    contextList = c;
    return c;
}

static ContextInfo FindContextInfo(context)
    DPSContext context;
{
    ContextInfo c = contextList;

    while (c != NULL && c->context != context) c = c->next;
    return c;
}

/* If context == NULL, LookupContext must be followed immediately by filling
   in the context field of the returned context info record.  May only be
   called for a display in the display list. */

static ContextInfo LookupContext(display, context)
    Display *display;
    DPSContext context;
{
    ContextInfo c = FindContextInfo(context);

    if (c != NULL) {
        if (c->display != display) return NULL;		/* Something's wrong */
	else return c;
    }

    /* Create one */

    c = AllocContextInfo(context);
    c->displayInfo = LookupDisplayInfo(display);
    c->display = display;
    if (c->displayInfo == NULL) return NULL;		/* Internal error */
    return c;
}

static DisplayInfo AllocDisplayInfo(display, context)
    Display *display;
    DPSContext context;
{
    DisplayInfo d = (DisplayInfo) malloc(sizeof(DisplayInfoRec));
    register int i;

    if (d == NULL) return NULL;
    d->next = displayList;
    displayList = d;

    d->display = display;
    if (context == NULL) d->contextInfo = NULL;
    else d->contextInfo = LookupContext(display, context);
    d->extensionPresent = (context == NULL) ? ext_no_idea : ext_yes;

    d->depthsForScreen = (int *) calloc(ScreenCount(display), sizeof(int));
    d->validDepths = (int **) calloc(ScreenCount(display), sizeof(int *));
    d->gcForDepth = (GC **) calloc(ScreenCount(display), sizeof(GC *));

    for (i = 0; i < ScreenCount(display); i++) {
        d->validDepths[i] = XListDepths(display, i, &d->depthsForScreen[i]);
	d->gcForDepth[i] = (GC *) calloc(d->depthsForScreen[i], sizeof(GC));
    }

    return d;
}

static DisplayInfo FindDisplayInfo(display)
    Display *display;
{
    DisplayInfo d = displayList;

    while (d != NULL && d->display != display) d = d->next;
    return d;
}

static DisplayInfo LookupDisplayInfo(display)
    Display *display;
{
    DisplayInfo d = FindDisplayInfo(display);

    if (d == NULL) d = AllocDisplayInfo(display, (DPSContext) NULL);

    return d;
}

int _XDPSSetComponentInitialized(context, bit)
    DPSContext context;
    unsigned long bit;
{
    ContextInfo c = FindContextInfo(context);

    if (c == NULL) return dps_status_unregistered_context;
    c->initFlags |= bit;
    return dps_status_success;
}

int _XDPSTestComponentInitialized(context, bit, result)
    DPSContext context;
    unsigned long bit;
    Bool *result;
{
    ContextInfo c = FindContextInfo(context);

    if (c == NULL) {
	*result = False;
	return dps_status_unregistered_context;
    }
    *result = ((c->initFlags & bit) != 0);
    return dps_status_success;
}

int XDPSSetContextDepth(context, screen, depth)
    DPSContext context;
    Screen *screen;
    int depth;
{
    return XDPSSetContextParameters(context, screen, depth, None, 0,
				    (XDPSStandardColormap *) NULL,
				    (XDPSStandardColormap *) NULL,
				    XDPSContextScreenDepth);
}

int XDPSSetContextDrawable(context, drawable, height)
    DPSContext context;
    Drawable drawable;
    int height;
{
    return XDPSSetContextParameters(context, (Screen *) NULL, 0,
				    drawable, height,
				    (XDPSStandardColormap *) NULL,
				    (XDPSStandardColormap *) NULL,
				    XDPSContextDrawable);
}

int XDPSSetContextRGBMap(context, map)
    DPSContext context;
    XDPSStandardColormap *map;
{
    return XDPSSetContextParameters(context, (Screen *) NULL, 0, None, 0,
				    map, (XDPSStandardColormap *) NULL,
				    XDPSContextRGBMap);
}

int XDPSSetContextGrayMap(context, map)
    DPSContext context;
    XDPSStandardColormap *map;
{
    return XDPSSetContextParameters(context, (Screen *) NULL, 0, None, 0,
				    map, (XDPSStandardColormap *) NULL,
				    XDPSContextGrayMap);
}

static GC DisplayInfoSharedGC(d, screen, depth)
    DisplayInfo d;
    Screen *screen;
    int depth;
{
    int s = XScreenNumberOfScreen(screen);
    register int i;
    XGCValues v;
    Pixmap p;

    if (s >= ScreenCount(DisplayOfScreen(screen))) return NULL;
	  
    for (i = 0; i < d->depthsForScreen[s] &&
	       d->validDepths[s][i] != depth; i++) {}
	  
    if (i >= d->depthsForScreen[s]) return NULL;
	  
    if (d->gcForDepth[s][i] == 0) {	/* Not "None" -- used calloc */
	if (depth == DefaultDepthOfScreen(screen)) {
	    d->gcForDepth[s][i] = XCreateGC(d->display,
					    RootWindowOfScreen(screen), 0, &v);
	} else {
	    p = XCreatePixmap(d->display,
			      RootWindowOfScreen(screen),
			      1, 1, depth);
	    d->gcForDepth[s][i] = XCreateGC(d->display, p, 0, &v);
	    XFreePixmap(d->display, p);
	}
    }

    return d->gcForDepth[s][i];
}

int XDPSSetContextParameters(context, screen, depth, drawable, height,
			     rgbMap, grayMap, flags)
    DPSContext context;
    Screen *screen;
    int depth;
    Drawable drawable;
    int height;
    XDPSStandardColormap *rgbMap;
    XDPSStandardColormap *grayMap;
    unsigned int flags;
{
    ContextInfo c = FindContextInfo(context);
    Bool doDepth = False, doDrawable = False, doRGB = False, doGray = False;
    Colormap map = None;
    XDPSStandardColormap rgb, gray;
    GC gc;
    GContext gctx = None;
    DisplayInfo d;
    
    if (c == NULL) return dps_status_unregistered_context;
    d = c->displayInfo;

    if (flags & XDPSContextScreenDepth) {
	doDepth = True;

	if (DisplayOfScreen(screen) != c->display) {
	    return dps_status_illegal_value;
	}

	gc = DisplayInfoSharedGC(d, screen, depth);
	if (gc == NULL) return dps_status_illegal_value;

	gctx = XGContextFromGC(gc);
    }

    if (flags & XDPSContextDrawable) {
	doDrawable = True;
	if (drawable != None && height <= 0) return dps_status_illegal_value;
    }

    if (flags & XDPSContextRGBMap) {
	doRGB = True;
	if (rgbMap == NULL) {
	    XDPSGetDefaultColorMaps(DisplayOfScreen(screen), screen,
				    drawable,
				    (XStandardColormap *) &rgb,
				    (XStandardColormap *) NULL);
	    rgbMap = &rgb;
	}
	map = rgbMap->colormap;
    } else rgbMap = &rgb;	/* point to something */

    if (flags & XDPSContextGrayMap) {
	doGray = True;
	if (grayMap == NULL) {
	    XDPSGetDefaultColorMaps(DisplayOfScreen(screen), screen,
				    drawable,
				    (XStandardColormap *) NULL,
				    (XStandardColormap *) &gray);
	    grayMap = &gray;
	}

	if (doRGB && map != grayMap->colormap) {
	    return dps_status_illegal_value;
	} else map = grayMap->colormap;
    } else grayMap = &gray;	/* point to something */

    if (doDepth || doDrawable || doRGB || doGray) {
	_DPSSSetContextParameters(context, gctx, drawable, height, map,
				  rgbMap->base_pixel,
				  rgbMap->red_max, rgbMap->red_mult,
				  rgbMap->green_max, rgbMap->green_mult,
				  rgbMap->blue_max, rgbMap->blue_mult,
				  grayMap->base_pixel,
				  grayMap->red_max, grayMap->red_mult,
				  doDepth, doDrawable, doRGB, doGray);
    }
    return dps_status_success;
}

typedef struct {
    DPSContext context;
} PushCookieRec, *PushCookie;

int XDPSPushContextParameters(context, screen, depth, drawable, height,
			      rgbMap, grayMap, flags, pushCookieReturn)
    DPSContext context;
    Screen *screen;
    int depth;
    Drawable drawable;
    int height;
    XDPSStandardColormap *rgbMap;
    XDPSStandardColormap *grayMap;
    unsigned int flags;
    DPSPointer *pushCookieReturn;
{
    ContextInfo c = FindContextInfo(context);
    PushCookie p;
    int status;

    if (c == NULL) return dps_status_unregistered_context;

    p = (PushCookie) malloc(sizeof(PushCookieRec));
    p->context = c->context;
    DPSgsave(context);

    status = XDPSSetContextParameters(context, screen, depth, drawable, height,
				      rgbMap, grayMap, flags);

    *pushCookieReturn = (DPSPointer) p;
    return status;
}

int XDPSPopContextParameters(pushCookie)
    DPSPointer pushCookie;
{
    PushCookie p = (PushCookie) pushCookie;
    ContextInfo c = FindContextInfo(p->context);

    if (c == NULL) return dps_status_illegal_value;

    DPSgrestore(c->context);

    free((char *) pushCookie);
    return dps_status_success;
}
    
int XDPSCaptureContextGState(context, gsReturn)
    DPSContext context;
    DPSGState *gsReturn;
{
    ContextInfo c = FindContextInfo(context);
    unsigned int gs;

    if (c == NULL) return dps_status_unregistered_context;

    _DPSSCaptureGState(context, &gs);

    *gsReturn = gs;
    return dps_status_success;
}

int XDPSUpdateContextGState(context, gs)
    DPSContext context;
    DPSGState gs;
{
    ContextInfo c = FindContextInfo(context);

    if (c == NULL) return dps_status_unregistered_context;

    _DPSSUpdateGState(context, gs);

    return dps_status_success;
}

int XDPSFreeContextGState(context, gs)
    DPSContext context;
    DPSGState gs;
{
    ContextInfo c = FindContextInfo(context);

    if (c == NULL) return dps_status_unregistered_context;

    _DPSSUndefineUserObject(context, gs);

    return dps_status_success;
}

int XDPSSetContextGState(context, gs)
    DPSContext context;
    DPSGState gs;
{
    ContextInfo c = FindContextInfo(context);

    if (c == NULL) return dps_status_unregistered_context;

    _DPSSRestoreGState(context, gs);

    return dps_status_success;
}

int XDPSPushContextGState(context, gs, pushCookieReturn)
    DPSContext context;
    DPSGState gs;
    DPSPointer *pushCookieReturn;
{
    ContextInfo c = FindContextInfo(context);
    PushCookie p;
    int status;

    if (c == NULL) return dps_status_unregistered_context;

    p = (PushCookie) malloc(sizeof(PushCookieRec));
    p->context = c->context;
    DPSgsave(context);

    status = XDPSSetContextGState(context, gs);
    *pushCookieReturn = (DPSPointer) p;

    return status;
}

int XDPSPopContextGState(pushCookie)
    DPSPointer pushCookie;
{
    PushCookie p = (PushCookie) pushCookie;
    ContextInfo c = FindContextInfo(p->context);

    if (c == NULL) return dps_status_unregistered_context;

    DPSgrestore(c->context);

    c->context = p->context;

    free((char *) pushCookie);
    return dps_status_success;
}

void XDPSRegisterContext(context, makeSharedContext)
    DPSContext context;
    Bool makeSharedContext;
{
    DisplayInfo d;
    Display *display;
    Bool inited;
    ContextInfo c;
    
    /* Get the display */
    (void) XDPSXIDFromContext(&display, context);

    if (makeSharedContext) {	/* Install as shared ctxt for this display */
        d = LookupDisplayInfo(display);

	d->contextInfo = LookupContext(display, context);
	d->extensionPresent = ext_yes;

    } else {			/* Just add to the context list */
        c = LookupContext(display, context);

	c->displayInfo->extensionPresent = ext_yes;
    }

    (void) _XDPSTestComponentInitialized(context, dps_init_bit_share, &inited);
    if (!inited) {
	(void) _XDPSSetComponentInitialized(context, dps_init_bit_share);
	_DPSSInstallDPSlibDict(context);
    }
}

DPSContext XDPSGetSharedContext(display)
    Display *display;
{
    DisplayInfo d = LookupDisplayInfo(display);
    ContextInfo c;

    if (d->extensionPresent == ext_no) return NULL;

    if (d->contextInfo == NULL) {
	c = AllocContextInfo((DPSContext) NULL);
	d->contextInfo = c;
	c->displayInfo = d;
	c->display = display;
    } else c = d->contextInfo;

    if (c->context == NULL) {
	c->context = XDPSCreateSimpleContext(display,
					     None, None, 0, 0,
					     DPSDefaultTextBackstop,
					     DPSDefaultErrorProc, NULL);
	if (c->context != NULL) {
	    (void) _XDPSSetComponentInitialized(c->context, dps_init_bit_share);
	    _DPSSInstallDPSlibDict(c->context);
	    (void) XDPSSetContextDepth(c->context,
				       DefaultScreenOfDisplay(display),
				       DefaultDepth(display,
						    DefaultScreen(display)));
	}
    }

    if (c->context == NULL) {
	d->extensionPresent = ext_no;
	return NULL;
    } else d->extensionPresent = ext_yes;

    return c->context;
}

void XDPSDestroySharedContext(context)
    DPSContext context;
{
    ContextInfo *cp = &contextList;
    ContextInfo c;

    while (*cp != NULL && (*cp)->context != context) cp = &((*cp)->next);

    if (*cp == NULL) return;

    c = *cp;
    *cp = c->next;	/* remove from list */

    c->displayInfo->contextInfo = NULL;
    DPSDestroySpace(DPSSpaceFromContext(c->context));	/* Also gets context */
    if (c->text != NULL) DPSDestroySpace(DPSSpaceFromContext(c->text));
    free((char *) c);
}

void XDPSUnregisterContext(context)
    DPSContext context;
{
    ContextInfo *cp = &contextList;
    ContextInfo c;

    while (*cp != NULL && (*cp)->context != context) cp = &((*cp)->next);

    if (*cp == NULL) return;

    c = *cp;
    *cp = c->next;	/* remove from list */

    c->displayInfo->contextInfo = NULL;
    if (c->text != NULL) DPSDestroySpace(DPSSpaceFromContext(c->text));
    free((char *) c);
}

void XDPSFreeDisplayInfo(display)
    Display *display;
{
    DisplayInfo *dp = &displayList;
    DisplayInfo d;
    register int i, j;

    while (*dp != NULL && (*dp)->display != display) dp = &((*dp)->next);

    if (*dp == NULL) return;

    d = *dp;
    *dp = d->next;	/* remove from list */

    for (i = 0; i < ScreenCount(display); i++) {
#ifdef NO_XLISTDEPTHS
	free((char *) d->validDepths[i]);
#else
	XFree((char *) d->validDepths[i]);
#endif
	for (j = 0; j < d->depthsForScreen[i]; j++) {
	    if (d->gcForDepth[i][j] != 0) {
		XFreeGC(display, d->gcForDepth[i][j]);
	    }
	}
	free((char *) d->depthsForScreen[i]);
    }

    free((char *) d->depthsForScreen);
    free((char *) d->validDepths);
    free((char *) d->gcForDepth);
    free((char *) d);
}

int XDPSChainTextContext(context, enable)
    DPSContext context;
    Bool enable;
{
    ContextInfo c = FindContextInfo(context);

    if (c == NULL) return dps_status_unregistered_context;

    /* Check if already in desired state */

    if (c->enableText == enable) return dps_status_success;

    if (enable) {
	if (c->text == NULL) {
	    c->text = DPSCreateTextContext(DPSDefaultTextBackstop,
					   DPSDefaultErrorProc);
	    if (c->text == NULL) return dps_status_no_extension;
	}
	DPSChainContext(c->context, c->text);
	c->enableText = True;
	return dps_status_success;
    }

    /* disabling, currently enabled */

    DPSUnchainContext(c->text);
    c->enableText = False;
    return dps_status_success;
}

Bool XDPSExtensionPresent(display)
    Display *display;
{
    DisplayInfo d = LookupDisplayInfo(display);

    if (d->extensionPresent != ext_no_idea) {
	return (d->extensionPresent == ext_yes);
    }

    return d->extensionPresent;
}

int PSDefineAsUserObj()
{
    return DPSDefineAsUserObj(DPSGetCurrentContext());
}

void PSRedefineUserObj(uo)
    int uo;
{
    DPSRedefineUserObj(DPSGetCurrentContext(), uo);
}

void PSUndefineUserObj(uo)
    int uo;
{
    DPSUndefineUserObj(DPSGetCurrentContext(), uo);
}

int DPSDefineAsUserObj(ctxt)
    DPSContext ctxt;
{
    int out;

    _DPSSDefineUserObject(ctxt, 0, &out);
    return out;
}

void DPSRedefineUserObj(ctxt, uo)
    DPSContext ctxt;
    int uo;
{
    int out;

    _DPSSDefineUserObject(ctxt, uo, &out);
}

void DPSUndefineUserObj(ctxt, uo)
    DPSContext ctxt;
    int uo;
{
    _DPSSUndefineUserObject(ctxt, uo);
}

int PSReserveUserObjIndices(number)
    int number;
{
    return DPSReserveUserObjIndices(DPSGetCurrentContext(), number);
}

int DPSReserveUserObjIndices(ctxt, number)
    DPSContext ctxt;
    int number;
{
    int out;

    _DPSSReserveUserObjectIndices(ctxt, number, &out);
    return out;
}

void PSReturnUserObjIndices(start, number)
    int start, number;
{
    DPSReturnUserObjIndices(DPSGetCurrentContext(), start, number);
}

void DPSReturnUserObjIndices(ctxt, start, number)
    DPSContext ctxt;
    int start, number;
{
    _DPSSReturnUserObjectIndices(ctxt, start, number);
}

#ifdef NO_XLISTDEPTHS
/* This function copyright 1989 Massachusetts Institute of Technology */

/*
 * XListDepths - return info from connection setup
 */
int *XListDepths (dpy, scrnum, countp)
    Display *dpy;
    int scrnum;
    int *countp;
{
    Screen *scr;
    int count;
    int *depths;

    if (scrnum < 0 || scrnum >= dpy->nscreens) return NULL;

    scr = &dpy->screens[scrnum];
    if ((count = scr->ndepths) > 0) {
	register Depth *dp;
	register int i;

	depths = (int *) malloc (count * sizeof(int));
	if (!depths) return NULL;
	for (i = 0, dp = scr->depths; i < count; i++, dp++) 
	  depths[i] = dp->depth;
    } else {
	/* a screen must have a depth */
	return NULL;
    }
    *countp = count;
    return depths;
}
#endif /* NO_XLISTDEPTHS */
