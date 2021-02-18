/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/annot/RCS/dpstextv.c,v 1.3 1991/09/25 23:26:19 mcinerny Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/annot/RCS/dpstextv.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	char *sketch_ch_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mit/annot/RCS/dpstextv.c,v 1.3 1991/09/25 23:26:19 mcinerny Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#include <dpstextv.eh>
#include <andrewos.h>		/* for DPS_ENV */

#ifdef DPS_ENV
#include <X11/X.h>
#include <X11/Xlib.h>
#include <DPS/XDPS.h>
#include <DPS/XDPSlib.h>
#include <DPS/dpsXclient.h>
#include <DPS/dpsclient.h>

#include <xgraphic.ih>
#endif /* DPS_ENV */

#include <class.h>
#include <view.ih>
#include <text.ih>
#include <im.ih>
#include <graphic.ih>
#include <rect.h>
#include <message.ih>
#include <stdio.h>
#include <signal.h>


/* #define DEBUG 1 */
#ifdef DEBUG
#define DBG(x) fprintf(stderr, "%s\n", (x))
#else
#define DBG(x) 
#endif

/****************************************************************/
/*		private functions				*/
/****************************************************************/

static int computeDPSscaling(self)
     struct dpstextview *self;
{
    long pw, ph, dw, dh, sw, sh;
    long x_offset, y_offset;
    double scale_factor;

    /* what do we want to be */
    pw = self->desired_width;
    ph = self->desired_height;

    /* what have we got to work with */
    dw = self->cached_width;
    dh = self->cached_height;

    /* try to be as tall as possible */
    sh = dh;
    scale_factor = (double)sh / (double)ph;
    sw = pw * scale_factor;
    y_offset = 0;
    x_offset = (dw - sw)/2;

    if (sw > dw) {
	/* try to be as wide as possible */
	sw = dw;
	scale_factor = (double)sw / (double)pw;
	sh = ph * scale_factor;
	y_offset = (dh - sh)/2;
	x_offset = 0;
    }
    if ((x_offset != self->offset_x)
	|| (y_offset != self->offset_y)
	|| (scale_factor != self->scale_width)
	|| (scale_factor != self->scale_height)) {
	dpstextview_SetScaling(self, x_offset, y_offset, scale_factor, scale_factor);
	return(-1);
    }
    return(0);
    
} /* computeDPSscaling */

void DoInterpret(self)
     struct dpstextview *self;
{
#ifdef DPS_ENV
    char *displayname = "";
    Display *dpy;
    Screen *scr;
    GC gc;
    Window win;
    XWindowAttributes wa;
    DPSContext ctxt;
    struct text *txtobj;
    struct xgraphic *xgr;
    char *buf;
    long length;
    struct rectangle r1, r2;
    long x_off, y_off;
    double dpi_x, dpi_y;

    txtobj = (struct text *)dpstextview_GetDataObject(self);

    im_GetLoc(dpstextview_GetIM(self), dpstextview_GetIM(self), &r1);
    im_GetLoc(dpstextview_GetIM(self), self, &r2);
    xgr = (struct xgraphic *)dpstextview_GetDrawable(self);
    dpy = xgraphic_XDisplay(xgr);
    if (dpy == NULL) {
 	fprintf (stderr, "dpstextview: NULL display.\n");
 	fprintf (stderr, "dpstextview: maybe I'm not visible?\n");
	return;
    }
    gc = xgraphic_XGC(xgr);
    win = (Window)xgraphic_XWindow(xgr);
    XGetWindowAttributes(dpy, win, &wa);
    scr = wa.screen;
    dpi_x = scr->width * 25.4 / scr->mwidth;
    dpi_y = scr->height * 25.4 / scr->mheight;

    x_off = r2.left - r1.left + self->offset_x;
    y_off = r2.top - r1.top + r2.height - self->offset_y;
#ifdef DEBUG
    ctxt = DPSCreateTextContext(DPSDefaultTextBackstop, DPSDefaultErrorProc); 
#else
    ctxt = XDPSCreateSimpleContext(dpy, win, gc, x_off, y_off,
				   DPSDefaultTextBackstop,
				   DPSDefaultErrorProc, NULL);
#endif
    
    if (ctxt == NULL) {
 	fprintf (stderr, "dpstextview: XDPSCreateSimpleContext returns NULL.\n");
 	fprintf (stderr, "dpstextview: Perhaps this server doesn't have the Display PostScript extension?\n");
	return;
    }

    DPSPrintf(ctxt, "gsave %g %g scale\n", 72.0/dpi_x, 72.0/dpi_y);
    DPSPrintf(ctxt, "0.9 setgray %d %d %d %d rectfill\n",
	      -(self->offset_x), -(self->offset_y), r2.width, r2.height);
    DPSPrintf(ctxt, "1.0 setgray 0 0 %g %g rectfill\n",
	      self->desired_width * self->scale_width,
	      self->desired_height * self->scale_height);
    DPSPrintf(ctxt, "grestore\n");
    DPSPrintf(ctxt, "%g %g scale\n", self->scale_width, self->scale_height);
    DPSPrintf(ctxt, "/showpage {} def\n");

    /* Make sure that gap is at end of buffer so that GetBuf
       gives us the whole buffer, not just the part before the gap.  */
    text_GetGap(txtobj, text_GetLength(txtobj), 0L);
    buf = text_GetBuf(txtobj, 0L, text_GetLength(txtobj), &length);
    DPSWritePostScript(ctxt, buf, length);
    DPSWaitContext(ctxt);
    
    DPSDestroySpace(DPSSpaceFromContext(ctxt));
    XFlush(dpy);
#endif /* DPS_ENV */
} /* DoInterpret */

/****************************************************************/
/*		class procedures				*/
/****************************************************************/

boolean
dpstextview__InitializeClass(classID)
    struct classheader * classID;
{
    return TRUE;
}


boolean
dpstextview__InitializeObject(classID,self)
struct classheader * classID;
struct dpstextview * self;
{
    dpstextview_SetScaling(self, 0L, 0L, 1.0, 1.0);
    self->drawn_at_least_once = 0;
    self->cached_width = 0;
    self->cached_height = 0;
    self->offset_x = 0;
    self->offset_y = 0;
    return TRUE;
}

/****************************************************************/
/*		instance methods				*/
/****************************************************************/

void dpstextview__SetDesired(self, w, h)
     struct dpstextview *self;
     long w,h;
{  
    self->desired_height = h;
    self->desired_width = w;

    if (computeDPSscaling(self) && self->drawn_at_least_once) {
	dpstextview_WantUpdate(self, self);
    }
} /* dpstextview__SetDesired */

void dpstextview__Update(self)
     struct dpstextview * self;
{
    struct rectangle r;

    dpstextview_SetTransferMode(self, graphic_COPY);
    dpstextview_EraseVisualRect(self);
    dpstextview_GetLogicalBounds(self, &r);
    dpstextview_FullUpdate(self, view_FullRedraw, r.left, r.top, r.width, r.height);
} /* dpstextview_Update */

void
dpstextview__FullUpdate(self, type, x, y, w, h)
     struct dpstextview *self;
     enum view_UpdateType type;
     long x,y,w,h;
{  
    if (type == view_FullRedraw
	|| type == view_LastPartialRedraw) {
	struct rectangle r;

	dpstextview_GetLogicalBounds(self, &r);
	if ((r.width != self->cached_width) || (r.height != self->cached_height)) {
	    self->cached_width = r.width;
	    self->cached_height = r.height;
	    computeDPSscaling(self);
	}

	DoInterpret(self);
	self->drawn_at_least_once = -1;
    }
} /* dpstextview__FullUpdate */

