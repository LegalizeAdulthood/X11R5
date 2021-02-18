/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) RubberWin.c 50.4 90/12/12 Crucible";
#ifdef OW_I18N
#include "i18n.h"
#endif
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#ifdef OW_I18N
#include "ollocale.h"
#endif
#include "olwm.h"
#include "win.h"
#include "events.h"
#include "list.h"
#include "globals.h"

extern int	DefScreen;
extern GC	RootGC;
extern unsigned int FindModifierMask();
extern int Resize_width, Resize_height;

typedef struct {
    Display *dpy;
    int initX, initY;
    int offX, offY;
    int rounder, divider;
    List *winlist;
} movestuff_t;

static Bool movewinInterposer();
static void *moveOneWindow();
static void *configOneWindow();
static void *drawOneBox();
static void moveDone();

static Bool invertMoveWindow = False;


/*
 * drawDouble
 *
 * Draw a thick box on the given window, using the given GC.  The box is drawn
 * using four rectangles.  This technique is used instead of wide lines
 * because this routine is used during animation, and the wide line code of
 * some servers is too slow.
 */

#define defrect(r, X, Y, W, H) \
	(r).x = X, (r).y = Y, (r).width = W, (r).height = H

static void
drawDouble(dpy, win, gc, x, y, w, h)
    Display    *dpy;
    Window      win;
    GC          gc;
    int         x, y, w, h;
{
    register int thick = GRV.RubberBandThickness;
    XRectangle  rects[4];

    defrect(rects[0], x, y, w, thick);
    defrect(rects[1], x, y + h - thick, w, thick);
    defrect(rects[2], x, y + thick, thick, h - 2 * thick);
    defrect(rects[3], x + w - thick, y + thick, thick, h - 2 * thick);
    XFillRectangles(dpy, win, gc, rects, 4);
}

#undef defrect


/*
 * UserMoveWindows
 *
 * Allow the user to move a window or the set of selected windows.  The
 * "first" parameter must be the button event that initiated the interaction.  
 * The "winInfo" parameter must be the frame or icon on which the action was 
 * initiated.  The external boolean DragWindow controls whether the whole 
 * window is moved or whether the outline is moved.
 */
void
UserMoveWindows(dpy, first, winInfo)
    Display *dpy;
    XEvent *first;
    WinGenericFrame *winInfo;
{
    static movestuff_t mstuff;
    List *winlist = NULL_LIST;
    Client *cli = winInfo->core.client;
    unsigned int modmask;

    mstuff.dpy = dpy;
    mstuff.offX = 0;
    mstuff.offY = 0;
    mstuff.initX = first->xbutton.x_root;
    mstuff.initY = first->xbutton.y_root;

    if (first->xbutton.state & ShiftMask) {
	invertMoveWindow = True;
	GRV.DragWindow = !GRV.DragWindow;
    }

    /*
     * If a meta key is held down, slow down the dragging by a factor of ten.
     */

    modmask = FindModifierMask(XKeysymToKeycode(dpy,XK_Meta_L)) |
	      FindModifierMask(XKeysymToKeycode(dpy,XK_Meta_R));

    if (first->xbutton.state & modmask) {
	mstuff.rounder = 5;
	mstuff.divider = 10;
    } else {
	mstuff.rounder = 0;
	mstuff.divider = 1;
    }

    /*
     * Generate the list of windows to be moved.  If the initial window is 
     * selected, we're moving the selection; otherwise, we're moving just this 
     * window.
     */
    if (IsSelected(cli)) {
	Client *c = (Client *) 0;
	while (c = EnumSelections(c)) {
	    if (c->wmState == IconicState)
		winlist = ListCons(c->iconwin, winlist);
	    else
		winlist = ListCons(c->framewin, winlist);
	}
    } else {
	winlist = ListCons(winInfo, NULL_LIST);
    }
    mstuff.winlist = winlist;

    XGrabPointer(dpy, DefaultRootWindow(dpy), False, 
		 (ButtonReleaseMask | PointerMotionMask),
		 GrabModeAsync, GrabModeAsync,
		 None, GRV.MovePointer, CurrentTime);
				    /* REMIND - use first.xbutton.time? */

    InstallInterposer( movewinInterposer, &mstuff );

    /*
     * If we're dragging the outlines, we must also grab the server and draw 
     * the initial set of bounding boxes.
     */
    if (!GRV.DragWindow) {
	XGrabServer(dpy);
	(void) ListApply(mstuff.winlist, drawOneBox, &mstuff);
    }
}


/*
 * movewinInterposer
 *
 * Interposer function for moving windows.  Moves the list of windows on each 
 * MotionNotify; releases interposition on ButtonRelease.
 */
static int
movewinInterposer(dpy, event, w, mstuff )
    Display *dpy;
    XEvent *event;
    WinGeneric *w;
    movestuff_t *mstuff;
{
    XEvent nextevent;

    switch (event->type) {
    case ButtonRelease:
	moveDone(dpy, mstuff);
	return DISPOSE_USED;
	break;

    case MotionNotify:
	/* if the event is off the screen, ignore it */
        if (!event->xmotion.same_screen)
	    return DISPOSE_USED;

	/*
	 * Motion compression.  If the next event is a MotionNotify,
	 * ignore this one.
	 */
	if (XEventsQueued(dpy, QueuedAfterReading) > 0 &&
	    (XPeekEvent(dpy,&nextevent), nextevent.type == MotionNotify))
	    return DISPOSE_USED;

	if (!GRV.DragWindow)
	    (void) ListApply(mstuff->winlist, drawOneBox, mstuff);

	mstuff->offX = (event->xmotion.x_root - mstuff->initX
			+ mstuff->rounder) / mstuff->divider;
	mstuff->offY = (event->xmotion.y_root - mstuff->initY
			+ mstuff->rounder) / mstuff->divider;

	if ((event->xmotion.state & ControlMask) != 0) {
	    if (ABS(mstuff->offX) > ABS(mstuff->offY))
		mstuff->offY = 0;
	    else
		mstuff->offX = 0;
	}

	if (GRV.DragWindow)
	    (void) ListApply(mstuff->winlist, moveOneWindow, mstuff);
	else
	    (void) ListApply(mstuff->winlist, drawOneBox, mstuff);

	return DISPOSE_USED;

    case LeaveNotify:
	return DISPOSE_DEFER;

    default:
	return DISPOSE_DISPATCH;
    }
}

/* windowOff -- function to determine how far a window should be offset
 * given a pointer offset.  Returns both X and Y, by reference.
 */
static void
windowOff(win, mstuff, pox, poy)
WinGenericFrame *win;
movestuff_t *mstuff;
int *pox, *poy;
{
    int pixw = DisplayWidth(win->core.client->dpy, win->core.client->screen);
    int hpoint;
    int pixy = DisplayHeight(win->core.client->dpy, win->core.client->screen);

    if (mstuff->offX == 0)
    {
	*pox = 0;
    }
    else if (mstuff->offX > 0)
    {
        hpoint = pixw - win->core.x - win->core.width;
    	if ((mstuff->offX >= hpoint) && (mstuff->offX <= hpoint + GRV.EdgeThreshold))
	    *pox = hpoint;
	else if (mstuff->offX >= hpoint+win->core.width-Resize_width)
	    *pox = hpoint+win->core.width-Resize_width;
	else
	    *pox = mstuff->offX;
    }
    else if (mstuff->offX < 0)
    {
    	if ((mstuff->offX <= -win->core.x) && (mstuff->offX >= (-win->core.x - GRV.EdgeThreshold)))
	    *pox = -win->core.x;
	else if (mstuff->offX <= (-win->core.x-win->core.width+Resize_width))
	    *pox = (-win->core.x-win->core.width+Resize_width);
	else
	    *pox = mstuff->offX;
    }

    if (mstuff->offY == 0)
    {
	*poy = 0;
    }
    else if (mstuff->offY > 0)
    {
        hpoint = pixy - win->core.y - win->core.height;
    	if ((mstuff->offY >= hpoint) && (mstuff->offY <= hpoint + GRV.EdgeThreshold))
	    *poy = hpoint;
	else if (mstuff->offY >= hpoint+win->core.height-Resize_height)
	    *poy = hpoint+win->core.height-Resize_height;
	else
	    *poy = mstuff->offY;
    }
    else if (mstuff->offY < 0)
    {
    	if ((mstuff->offY <= -win->core.y) && (mstuff->offY >= (-win->core.y - GRV.EdgeThreshold)))
	    *poy = -win->core.y;
	else if (mstuff->offY <= (-win->core.y-win->core.height+Resize_height))
	    *poy = (-win->core.y-win->core.height+Resize_height);
	else
	    *poy = mstuff->offY;
    }
}

/*
 * moveOneWindow
 *
 * Apply function for window moving animation.  Draws a window outline or 
 * actually moves the window, depending on DragWindow.
 */
static void *
moveOneWindow(win, mstuff)
    WinGenericFrame *win;
    movestuff_t *mstuff;
{
    int offX, offY;

    windowOff(win, mstuff, &offX, &offY);
    XMoveWindow(mstuff->dpy, win->core.self,
		win->core.x + offX,
		win->core.y + offY);
    return (void *) 0;
}


/*
 * drawOneBox
 *
 * Apply function for drawing XOR boxes.  Draws a double-width rectangle 
 * around the outline of a single window.
 */
static void *
drawOneBox(w, mstuff)
    WinGenericFrame *w;
    movestuff_t *mstuff;
{
    int offX, offY;

    windowOff(w, mstuff, &offX, &offY);
    drawDouble(mstuff->dpy, DefaultRootWindow(mstuff->dpy), RootGC,
	       w->core.x + offX, w->core.y + offY,
	       w->core.width, w->core.height);

    return (void *) 0;
}    


/*
 * configOneWindow
 *
 * Apply function for calling a moved window's configfunc.
 */
static void *
configOneWindow(win, mstuff)
    WinGenericFrame *win;
    movestuff_t *mstuff;
{
    int offX, offY;

    windowOff(win, mstuff, &offX, &offY);
    GFrameSetConfig(win, win->core.x + offX, win->core.y + offY,
	win->core.width, win->core.height);
    return (void *) 0;
}


/*
 * moveDone
 *
 * Cleanup function for window moving.  Releases grabs, uninstalls 
 * interposition, cleans up.
 */
static void
moveDone(dpy, mstuff)
    Display *dpy;
    movestuff_t *mstuff;
{
    /*
     * If we're dragging the outlines, we must ungrab the server and undraw 
     * the last set of boxes.
     */
    if (!GRV.DragWindow) {
	XUngrabServer(dpy);
	(void) ListApply(mstuff->winlist, drawOneBox, mstuff);
    }
    UninstallInterposer();
    XUngrabPointer(dpy, CurrentTime);

    (void) ListApply(mstuff->winlist, configOneWindow, mstuff);
    ListDestroy(mstuff->winlist);

    if (invertMoveWindow) {
	invertMoveWindow = False;
	GRV.DragWindow = !GRV.DragWindow;
    }
}


void
UserResizeWin(cli, corner, press)
Client		*cli;
WhichResize	corner;
XEvent		*press;
{
	Display			*dpy = cli->dpy;
	int			screen = cli->screen;
	WinPaneFrame		*winFrame = cli->framewin;
	int			mouseX, mouseY;
	int			startx, starty;
	Window			mRoot, mChild;
	int			relX, relY;
	int			curMask;
	XEvent			event;
	Bool			windowPlaced;
	int			newx, newy;
	unsigned int		neww, newh;
	NewXSizeHints		*sizeHints;
	int			minw, minh, maxw, maxh;
	int			incW, incH;
	int			incW2, incH2;
	int			basew, baseh;
	int			anchorX, anchorY;
	Bool			boxDrawn = False;
	int			borderHeight;
	int			borderWidth;
	int			diff;
	int			pressX = press->xbutton.x_root;
	int			pressY = press->xbutton.y_root;
	int			adjustX, adjustY;

	/*
	 * Local macro for adjusting the size of a window to its resize
	 * increment.  Uses scratch variable "diff".  First, assigns diff to
	 * be the difference between the new size and the next lesser
	 * incremental size.  If this is greater than or equal to half the
	 * incremental size, return the adjustment to round up to the next
	 * greater incremental size.  Otherwise, return the adjustment to
	 * round down the next lesser incremental size.  The resulting
	 * adjustment should always be added to the size.
	 *
	 * new is new size, to be adjusted
	 * base is base size, to be subtracted off before modulo is done
	 * i is the incremental size
	 * i2 is half the incremental size
	 */
#define INCADJ(new, base, i, i2)					\
	( (diff = ((new) - (base)) % (i)),				\
	  (diff > (i2) ? ((i)-diff) : (-diff))				\
	)

	/* set initial position and size */
	startx = newx = winFrame->core.x;
	starty = newy = winFrame->core.y;
	neww = winFrame->core.width;
	newh = winFrame->core.height;
	borderHeight = FrameHeightTop(winFrame)+FrameHeightBottom(winFrame);
	borderWidth = FrameWidthLeft(winFrame)+FrameWidthRight(winFrame);

	/*
	 * Set up default parameters here, in terms of the size of the pane.  
	 * Any client-specified size hints will also be in terms of the size 
	 * of the pane.
	 */
	minw  = 1;
	minh  = 1;
	maxw  = 32767;		/* max signed short value */
	maxh  = 32767;
	incW  = 1;
	incH  = 1;
	basew = 0;
	baseh = 0;

	sizeHints = cli->normHints;
	if (sizeHints) {
	    if (sizeHints->flags & PMinSize) {
		minw = sizeHints->min_width;
		minh = sizeHints->min_height;
	    }

	    if (sizeHints->flags & PMaxSize) {
		maxw = sizeHints->max_width;
		maxh = sizeHints->max_height;
	    }

	    if (sizeHints->flags & PResizeInc) {
		incW = sizeHints->width_inc;
		incH = sizeHints->height_inc;
	    }

	    if (sizeHints->flags & PBaseSize) {
		basew = sizeHints->base_width;
		baseh = sizeHints->base_height;
	    }
	}

	/*
	 * Add our border sizes to the size parameters.  We do this so all our 
	 * calculations can be in terms of the size of the frame, not the pane.
	 */
	minw  += borderWidth;
	minh  += borderHeight;
	maxw  += borderWidth;
	maxh  += borderHeight;
	basew += borderWidth;
	baseh += borderHeight;

	incW2 = incW / 2;
	incH2 = incH / 2;

	/* find out corner point */
	switch(corner)
	{
		case lowright:	/* mouse defines LR corner */
			anchorX = newx;
			anchorY = newy;
			adjustX = newx+neww-pressX;
			adjustY = newy+newh-pressY;
			break;

		case lowleft:	/* mouse defines LL corner */
			anchorX = (int)(newx + neww);
			anchorY = newy;
			adjustX = newx-pressX;
			adjustY = newy+newh-pressY;
			break;

		case upright:	/* mouse defines UR corner */
			anchorX = newx;
			anchorY = (int)(newy + newh);
			adjustX = newx+neww-pressX;
			adjustY = newy-pressY;
			break;

		case upleft:	/* mouse defines UL corner */
			anchorX = (int)(newx + neww);
			anchorY = (int)(newy + newh);
			adjustX = newx-pressX;
			adjustY = newy-pressY;
			break;
	}
		

	/*
	 * Change the cursor.
	 */
	XGrabPointer(dpy, RootWindow(dpy, screen),
		     True, (ButtonReleaseMask | PointerMotionMask),
		     GrabModeAsync, GrabModeAsync,
		     None,
		     GRV.ResizePointer,
		     CurrentTime);

	/*
	 * Grab the server, so that the outline doesnt
	 * corrupt or get corrupted.
	 */
	XGrabServer(dpy);

	/* Get initial mouse pointer location */
	XQueryPointer(dpy, RootWindow(dpy, screen),
		      &mRoot, &mChild,
		      &mouseX, &mouseY,
		      &relX, &relY, &curMask);
	

	for (windowPlaced = False; windowPlaced != True; )
	{
		XMaskEvent(dpy, ButtonReleaseMask|PointerMotionMask, &event);
		switch (event.type)
		{
		case ButtonRelease:
			/* set new window position */
			windowPlaced = True;
			break;
		
		case MotionNotify:
			/* if the event is off the screen, ignore it */
			if (!event.xmotion.same_screen)
			    break;

			/* erase old box, if any */
			if (boxDrawn)
			    drawDouble(dpy, DefaultRootWindow(dpy), RootGC,
				       newx, newy, neww, newh);
		
			/*
			 * there may be multiple motion events queued up,
			 * consume them and just use the last one.
			 */
			while (XCheckTypedEvent(dpy, MotionNotify, &event))
				;
			XQueryPointer(dpy, RootWindow(dpy, screen),
				      &mRoot, &mChild,
				      &mouseX, &mouseY,
				      &relX, &relY, &curMask);

			/* set new position */
			mouseX = event.xmotion.x_root + adjustX;
			mouseY = event.xmotion.y_root + adjustY;

			if ((event.xmotion.state & ControlMask) != 0) {
			    if (ABS(pressX-mouseX) > ABS(pressY-mouseY))
				mouseY = pressY+adjustY;
			    else
				mouseX = pressX+adjustX;
			}

			/* Window sizing using the window's XSizeHints
			 * is done so the following formula is met:
			 *
			 *	width = base_width + (i * width_inc)
			 *	height= base_height+ (j * height_inc)
			 *
			 * Where i and j are the number of size increments
			 * the window should be.  For example, an xterm
			 * might have the following values:
			 *
			 *	j = 24
			 *	min_height = 33
			 *	height_inc = 13 (for a font 13 pixels high)
			 */
			switch (corner)
			{
				case lowright:	/* mouse defines LR corner */
					/* make sure we don't go negative */
					if (mouseX < anchorX + minw)	
						mouseX = anchorX + minw;

					/* make sure we don't go negative */
					if (mouseY < anchorY + minh)	
						mouseY = anchorY + minh;

					neww = (unsigned)(mouseX - startx);
					newh = (unsigned)(mouseY - starty);

					neww +=
					    INCADJ(neww, basew, incW, incW2);
					newh +=
					    INCADJ(newh, baseh, incH, incH2);
					break;

				case lowleft:	/* mouse defines LL corner */
					/* make sure we don't go negative */
					if (mouseX > anchorX - minw)	
						mouseX = anchorX - minw;

					/* make sure we don't go negative */
					if (mouseY < anchorY + minh)	
						mouseY = anchorY + minh;
					
					neww = (unsigned)(anchorX - mouseX);
					newh = (unsigned)(mouseY - starty);

					neww +=
					    INCADJ(neww, basew, incW, incW2);
					newh +=
					    INCADJ(newh, baseh, incH, incH2);

					/* calculate new origin */
					newx = (int)(anchorX - neww);
					break;

				case upright:	/* mouse defines UR corner */
					/* make sure we don't go negative */
					if (mouseX < anchorX + minw)	
						mouseX = anchorX + minw;

					/* make sure we don't go negative */
					if (mouseY > anchorY - minh)	
						mouseY = anchorY - minh;

				      	neww = (unsigned)(mouseX - startx);
				      	newh = (unsigned)(anchorY - mouseY);

					neww +=
					    INCADJ(neww, basew, incW, incW2);
					newh +=
					    INCADJ(newh, baseh, incH, incH2);

					/* calculate new origin */
					newy = anchorY - newh;
					break;

				case upleft:	/* mouse defines UL corner */
					/* make sure we don't go negative */
					if (mouseX > anchorX - minw)	
						mouseX = anchorX - minw;

					/* make sure we don't go negative */
					if (mouseY > anchorY - minh)	
						mouseY = anchorY - minh;

				      	neww = (unsigned)(anchorX - mouseX);
				      	newh = (unsigned)(anchorY - mouseY);

					neww +=
					    INCADJ(neww, basew, incW, incW2);
					newh +=
					    INCADJ(newh, baseh, incH, incH2);

					/* calculate new origin */
					newx = (int)(anchorX - neww);
					newy = (int)(anchorY - newh);
					break;
			}

			/* REMIND not doing anything with aspect ratios */

			/* make sure width does not exceed limits */
			if (neww < minw)
				neww = (unsigned)minw;
			if (neww > maxw)
				neww = (unsigned)maxw;

			/* make sure height does not exceed limits */
			if (newh < minh)
				newh = (unsigned)minh;
			if (newh > maxh)
				newh = (unsigned)maxh;

			/*
			 * If we have exceeded the motion threshold, or if we 
			 * have already drawn a box, draw the new box.
			 */
			if (ABS(mouseX-pressX) > GRV.MoveThreshold ||
			    ABS(mouseY-pressY) > GRV.MoveThreshold ||
			    boxDrawn) {
			    drawDouble(dpy, DefaultRootWindow(dpy), RootGC,
				       newx, newy, neww, newh);
			    boxDrawn = True;
			}
			break;
		
		default:
			if (GRV.PrintOrphans)
				DebugEvent(&event, "UserResizeWin");
			break;
		}
	}

	/*
	 * Erase the last box, if any, and then let go of the server.
	 */
	if (boxDrawn)
	    drawDouble(dpy, DefaultRootWindow(dpy), RootGC,
		       newx, newy, neww, newh);
	XUngrabServer(dpy);
	XUngrabPointer(dpy, CurrentTime);

	/* 
	 * resize window to match new size/position 
	 */
	GFrameSetConfig(winFrame,newx,newy,neww,newh);
}


/*
 * TraceBoundingBox -- trace an XOR box with the initial point specified
 *	by pEvent, which is assumed to be a MotionNotify event.  Return the 
 *	dimensions of the box in boxX, boxY, boxW, and boxH.
 */
Bool
TraceBoundingBox(dpy, pEvent, pboxX, pboxY, pboxW, pboxH, timestamp)
Display	*dpy;
XEvent	*pEvent;
short	*pboxX, *pboxY, *pboxW, *pboxH;
Time *timestamp;
{
#define ex event.xmotion.x_root
#define ey event.xmotion.y_root

	XEvent	event;
	Bool	boxFinished;
	int	x0, y0, x, y, w, h;
	int	grabstatus;

	/*
	 * Grab the pointer to change the cursor.  Return immediately if we
	 * failed to grab.
	 */
	if (GrabSuccess != XGrabPointer(dpy, RootWindow(dpy, DefScreen), True, 
		     (ButtonReleaseMask | ButtonPressMask | PointerMotionMask),
		     GrabModeAsync, GrabModeAsync,
		     None, GRV.BasicPointer, CurrentTime))
		return False;

	/*
	 * Grab the server, so that the outline(s) don't
	 * corrupt or get corrupted.
	 */
	XGrabServer(dpy);
	XAllowEvents(dpy, SyncPointer, pEvent->xmotion.time);

	x = x0 = pEvent->xbutton.x_root;
	y = y0 = pEvent->xbutton.y_root;
	w = h = 0;

	/*
	 * Draw initial outline
	 */
	drawDouble(dpy, DefaultRootWindow(dpy), RootGC, x, y, w, h);

	for (boxFinished = False; boxFinished != True; )
	    {
		XMaskEvent(dpy, 
			ButtonReleaseMask|PointerMotionMask|ButtonPressMask, 
			&event);
		switch (event.type)
		{
		case ButtonPress:
			/*
			 * We're in a synchronous grab, so the protocol has
			 * frozen the pointer.  Unfreeze it.
			 */
			XAllowEvents(dpy, SyncPointer, pEvent->xbutton.time);
			break;

		case ButtonRelease:
			/*
			 * REMIND
			 * We should really check to see which button went up.
			 */
			boxFinished = True;
			break;
		
		case MotionNotify:
			/* if the event is off the screen, ignore it */
			if (!event.xmotion.same_screen)
			    break;

			/* erase old box */
			drawDouble(dpy, DefaultRootWindow(dpy), RootGC,
				   x, y, w, h);

			/*
			 * there may be multiple motion events queued up,
			 * consume them and just use the last one.
			 */
			while (XCheckTypedEvent(dpy, MotionNotify, &event))
				;

			/* set new position */
			if (ex > x0) {
			    x = x0;
			    w = ex - x;
			} else {
			    x = ex;
			    w = x0 - x;
			}
			if (ey > y0) {
			    y = y0;
			    h = ey - y;
			} else {
			    y = ey;
			    h = y0 - y;
			}
			/* draw new box */
			drawDouble(dpy, DefaultRootWindow(dpy), RootGC,
				   x, y, w, h);
			break;
		
		default:
			if (GRV.PrintOrphans)
				DebugEvent(&event, "TraceBounding");
			break;
		}
	}

	/*
	 * erase outline
	 */
        drawDouble(dpy, DefaultRootWindow(dpy), RootGC, x, y, w, h);

	/*
	 * let go of the server
	 */
	XUngrabServer(dpy);
	XUngrabPointer(dpy, CurrentTime);
	*pboxX = x;
	*pboxW = w;
	*pboxY = y;
	*pboxH = h;
	XFlush(dpy);
	*timestamp = event.xbutton.time;
	return True;
}
