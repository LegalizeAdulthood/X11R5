/***********************************************************
Copyright 1991 by Digital Equipment Corporation, Maynard, Massachusetts,
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
** File: 
**
**   xmvex1.c --- MVEX test program 1
**   
** Author: 
**
**   David Carver (Digital Workstation Engineering/Project Athena)
**   Ported to MVEX by Todd Brunhoff (Tektronix Research Labs)
**
** Revisions:
**
**   01.08.91 Brunhoff
**     - ported to MVEX
**       
*/
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/MVEX/MVEXlib.h>

int IndentLevel = 0;

char *PortName();

main()
{
    int screen;
    Display *dpy;
    MVEXVin *pXvin;
    MVEXVout *pXvout;
    int nvins, nvouts;

    dpy = XOpenDisplay(0);
    if (!dpy) {
	Output("Couldn't open display\n");
	Output("MVEX test program #1 terminated\n");
	exit();
    }
    screen = XDefaultScreen(dpy);
    XSynchronize(dpy, True);

    Output("Welcome to MVEX test program #1.\n\n");
    Output("This program invokes all the MVEX functions that are\n");
    Output("supported by your display for video input and output and their\n");
    Output("corresponding information.  It will create a window for the\n");
    Output("display of video and still images.  Then for each video\n");
    Output("input it will get/put a still image into the bottom window\n");
    Output("and a video image into the top.  For each video output it\n");
    Output("will capture the upper left portion of the screen (or as close\n");
    Output("as the hardware will allow).\n");

    Prompt(dpy, "Press return to continue");

    Output("Try MVEXQueryVideo() Request\n");
    if (! MVEXQueryVideo(dpy, screen)) {
	Output("MVEX video extension not available on this screen\n");
	Output("MVEX test program #1 terminated\n");
	exit();
    }

    /*
     *  Use mvexinfo to show all the info about the screens... it does it well.
     */
    system("mvexinfo");

    pXvin = MVEXGetVins( dpy, screen, &nvins);
    pXvout = MVEXGetVouts( dpy, screen, &nvouts);

    Output("\n");

    IndentLevel++;
    TestVio(dpy, (MVEXVio *)pXvin, nvins);
    TestVio(dpy, (MVEXVio *)pXvout, nvouts);
    IndentLevel--;

    Output("MVEX test program #1 finished\n");

    exit (0);
    /*NOTREACHED*/
}

TestVio(dpy, vio, nvio)
    Display *dpy;
    MVEXVio *vio;
    int nvio;
{
    int ii, jj, kk;
    MVEXRenderModel *pModel;
    VideoIO vioId;
    MVEXVideoValues vv;
    MVEXOwner owner;
    char chr;
    GC gc;
    Window win;

    for (ii = 0; ii < nvio; ii++) {
	Output("Testing video %s #%d\n",
	    vio->is_input ? "input" : "output", ii);
	IndentLevel++;
	for (jj = 0; jj < vio[ ii ].n_models; jj++) {
	    Output("\n");
	    Output("Testing model #%d\n", jj+1);

	    pModel = &vio[ ii ].model_list[ jj ];
	    if ((pModel->model_mask & MVEXWindowModelMask) == 0) {
		Output("Skipping model #%d (windows not supported)\n", jj+1);
		continue;
	    }

	    vioId = MVEXCreateVideo(dpy, vio->vref);
	    Output("Created vio 0x%x from video reference 0x%x\n",
		vioId,
		vio->vref);

	    MakeWindowAndGc(dpy, vioId, pModel, &gc, &win);

	    /*
	     * This should really be done by a video manager... but for
	     * testing, why not.
	     */
	    Output("taking ownership of vio %s...\n",
		"(this is *really* a video manager task)");
	    owner.window = win;
	    owner.vid = vioId;
	    MVEXChangeOwnership(dpy, &owner, 1);

	    Output("\n");
	    IndentLevel++;
	    for (kk = 0; kk < vio->n_ports; kk++)  {
		Output("Testing port #%d (0x%x) \"%s\"\n",
		    kk + 1, vio->ports[ kk ], PortName(dpy, vio->ports[ kk ]));

		if (vio->is_input) {
		    Output("RenderVideo (full motion)\n");
		    MVEXRenderVideo(dpy, vioId, win, gc, vio->ports[ kk ],
				    0,0,640,480,
				    16,5,640,480,
				    0, (MVEXVideoValues *) NULL);

		    Prompt(dpy, "Press <ret> to Render a single frame");

		    Output("RenderVideo (single frame)\n");

		    /* This does single frame */
		    vv.full_motion = False;
		    MVEXRenderVideo(dpy, vioId, win, gc, vio->ports[ kk ],
				    0,0,640,480,
				    16,485,640,480,
				    VRFullMotion, &vv);

		    /* This restarts full-motion */
		    MVEXRenderVideo(dpy, vioId, win, gc, vio->ports[ kk ],
				    0,0,640,480,
				    16,5,640,480,
				    0, (MVEXVideoValues *) NULL);

		    Prompt(dpy, "Press <ret> to StopVideo");

		    Output("StopVideo\n");
		    MVEXStopVideo(dpy,
				  MVEXStopRenderVideo,	/* what to stop */
				  (Drawable *)&win,	/* list of drawables */
				  1);			/* # of drawables */

		    Output("Waiting for Override event\n");
		    ShowEvents(dpy, MVEXBaseEvent(dpy) + MVEXOverride);
		}

		else /* vio->is_input if false */
		{
		    Output("CaptureGraphics (full motion)\n");
		    MVEXCaptureGraphics(dpy, vioId, win, None, vio->ports[ kk ],
				    0,0,640,480,
				    16,5,640,480,
				    0, (MVEXVideoValues *) NULL);

		    Prompt(dpy, "Press <ret> to Capture a single frame");
		    Output("CaptureGraphics (single frame)\n");

		    /* This does single frame */
		    vv.full_motion = False;
		    MVEXCaptureGraphics(dpy, vioId, win, None, vio->ports[ kk ],
				    0,0,640,480,
				    16,485,640,480,
				    VRFullMotion, &vv);

		    /* This restarts full-motion */
		    MVEXRenderVideo(dpy, vioId, win, None, vio->ports[ kk ],
				    0,0,640,480,
				    16,5,640,480,
				    0, (MVEXVideoValues *) NULL);

		    Prompt(dpy, "Press <ret> to StopVideo");

		    Output("StopVideo\n");
		    MVEXStopVideo(dpy,
				  MVEXStopCaptureGraphics,/* what to stop */
				  (Drawable *)&win,	/* list of drawables */
				  1);			/* # of drawables */
		}

		if (kk + 1 < vio->n_ports)  {
		    if (SkipRemaining(dpy, "ports"))
			break;
		    Output("\n");
		}
	    }
	    IndentLevel--;

	    XDestroyWindow(dpy, win);
	    XFreeGC(dpy, gc);

	    if (jj + 1  < vio[ ii ].n_models) {
		if (SkipRemaining(dpy, "models"))
		    break;
	    }
	}
	IndentLevel--;

	if (ii + 1 < nvio) {
	    if (SkipRemaining(dpy, vio->is_input ? "inputs" : "outputs"))
		break;
	}
    }
}

MakeWindowAndGc(dpy, vid, pModel, pgc, pwin)
    Display *dpy;
    VideoIO vid;
    MVEXRenderModel *pModel;
    GC *pgc;
    Window *pwin;
{
    XVisualInfo *p_vis_info, vis_info_temp;
    XSetWindowAttributes win_attr;
    Window root = XDefaultRootWindow(dpy);
    XGCValues gc_attr;
    XEvent event;
    int screen = XDefaultScreen(dpy);
    Colormap cmap;
    XColor scolor,ecolor;
    Visual *vis,*def_vis;
    int nitems;

    vis_info_temp.visualid = pModel->visualid;
    p_vis_info = XGetVisualInfo(dpy,
				VisualIDMask, 
				&vis_info_temp,
				&nitems);
    if (!p_vis_info) {
	Output("Error: Couldn't find visual ");
	Output("#%x listed for adaptor.\n", pModel->visualid);
	return;
    }
    vis = p_vis_info->visual;

    def_vis = XDefaultVisual(dpy, screen);
    if (vis->visualid == def_vis->visualid)
	cmap = XDefaultColormap(dpy, screen);
    else
	cmap = XCreateColormap(dpy, root, vis, AllocNone);

    XAllocNamedColor(dpy, cmap, "midnight blue", &scolor, &ecolor);

    win_attr.colormap = cmap;
    win_attr.background_pixel = scolor.pixel;
    win_attr.event_mask = ExposureMask | StructureNotifyMask;
    win_attr.backing_store = Always;
    win_attr.border_pixel = scolor.pixel;

    *pwin = XCreateWindow(dpy, root, 0, 0, 672, 980, 0, 
	pModel->depth, InputOutput, vis,
	CWColormap | CWBackPixel | CWEventMask |
	CWBackingStore | CWBorderPixel,
	&win_attr);

    XMapWindow(dpy, *pwin);

    Output("Waiting for window to become visible...\n");
    ShowEvents(dpy, MapNotify);

    Output("MVEXSelectEvents on window 0x%x %s\n",
	*pwin, "(Override and Violation)");
    MVEXSelectEvents(dpy, *pwin, MVEXOverrideMask | MVEXViolationMask);
    Output("MVEXSelectEvents on vid 0x%x (Sync)\n", vid);
    MVEXSelectEvents(dpy, vid, MVEXSyncMask);

    gc_attr.foreground = scolor.pixel;
    *pgc = XCreateGC(dpy, *pwin, GCForeground, &gc_attr);
}

char *CoreEventNames[] = {
    "type 0?",
    "type 1?",
    "KeyPress",
    "KeyRelease",
    "ButtonPress",
    "ButtonRelease",
    "MotionNotify",
    "EnterNotify",
    "LeaveNotify",
    "FocusIn",
    "FocusOut",
    "KeymapNotify",
    "Expose",
    "GraphicsExpose",
    "NoExpose",
    "VisibilityNotify",
    "CreateNotify",
    "DestroyNotify",
    "UnmapNotify",
    "MapNotify",
    "MapRequest",
    "ReparentNotify",
    "ConfigureNotify",
    "ConfigureRequest",
    "GravityNotify",
    "ResizeRequest",
    "CirculateNotify",
    "CirculateRequest",
    "PropertyNotify",
    "SelectionClear",
    "SelectionRequest",
    "SelectionNotify",
    "ColormapNotify",
    "ClientMessage"
};

char *MvexEventNames[] = {
    "Violation",
    "Setup",
    "Sync",
    "Override",
    "Request"
};

char *ViolationActions[] = {
    "scale",
    "placement",
    "clip",
    "overlap",
    "hue",
    "saturation",
    "contrast",
    "brightness",
};

char *ViolationStates[] = {
    "success",
    "fail",
    "subset"
};

char *OverrideStates[] = {
    "stopped",
    "drawable change",
    "marker reached",
    "request override",
    "hardware error",
    "reused",
};

#define AllViolationBits       (MVEXScaleViolation	\
			      | MVEXPlacementViolation	\
			      | MVEXClipViolation	\
			      | MVEXOverlapViolation	\
			      | MVEXHueViolation	\
			      | MVEXSaturationViolation	\
			      | MVEXContrastViolation	\
			      | MVEXBrightnessViolation)

static char *ViolationAction(action)
    int action;
{
    static char buf[ BUFSIZ ];
    int i, bits;

    buf[0] = '\0';
    bits = 0;
    for (i = 0; (1<<i) < AllViolationBits; i++) {
	if ((1<<i) & action) {
	    sprintf(buf + strlen(buf), "%s%s",
		bits ? "," : "", ViolationActions[ i ]);
	    bits++;
	}
    }
    if (action & (~AllViolationBits))
	    sprintf(buf + strlen(buf), "%s ?? bits", bits ? "and" : "");
    return (buf);
}

static char *ViolationState(state)
    int state;
{
    if (state >= sizeof(ViolationStates) / sizeof(ViolationStates[0]))
	return ("??");
    return (ViolationStates[ state ]);
}

static char *OverrideState(state)
    int state;
{
    if (state >= sizeof(OverrideStates) / sizeof(OverrideStates[0]))
	return ("??");
    return (OverrideStates[ state ]);
}

static char *MvexEventName(mvexType)
    int mvexType;
{
    if (mvexType >= sizeof(MvexEventNames) / sizeof(MvexEventNames[0]))
	return("unknown");
    else
	return(MvexEventNames[ mvexType ]);
}

/*
 * Since XSynchronize is in effect, we can assume that all the events
 * we are going to get have arrived.
 */
ShowEvents(dpy, waitFor)
    Display *dpy;
    int waitFor;
{
    XEvent event;
    int mvexBase = MVEXBaseEvent(dpy);
    int mvexType;
    union {
	MVEXViolationEvent *violate;
	MVEXSetupEvent *setup;
	MVEXSyncEvent *sync;
	MVEXOverrideEvent *override;
	MVEXRequestEvent *request;
	XEvent *core;
    } u;
    Bool foundTarget = (waitFor == 0);

    u.core = &event;
    while (XEventsQueued(dpy, QueuedAlready) || ! foundTarget) {
	XNextEvent(dpy, u.core);
	mvexType = event.type - mvexBase;
	if (event.type == waitFor)
	    foundTarget = True;

	if (mvexType >= 0) {
	    Output("**Mvex %s Event, ", MvexEventName(mvexType));
	    switch(mvexType) {
	    case MVEXViolation:
		printf("drawable=0x%x,vid=0x%x,time=0x%x\n",
		    u.violate->drawable,
		    u.violate->vid,
		    u.violate->time);
		Output("  action=%s,state=%s\n",
		    ViolationAction(u.violate->action_mask),
		    ViolationState(u.violate->state));
		break;

	    case MVEXSetup:
		printf("vid=0x%x,port=0x%x,time=0x%x,\n",
		    u.setup->vid,
		    u.setup->port,
		    u.setup->time);
		Output("  frames %savailable\n",
		    u.setup->frames_available ? "" : "not ");
		Output("  timecodes %savailable\n",
		    u.setup->timecodes_available ? "" : "not ");
		Output("  %sframe accurate\n",
		    u.setup->frame_accurate ? "" : "not ");
		break;

	    case MVEXSync:
		printf("drawable=0x%x,vid=0x%x,time=0x%x\n",
		    u.sync->drawable,
		    u.sync->vid,
		    u.sync->time);
		Output("  state=%s\n",
		    u.sync->state == MVEXSyncAcquired
		      ? "acquired" : "lost");
		break;

	    case MVEXOverride:
		printf("drawable=0x%x,vid=0x%x,time=0x%x\n",
		    u.override->drawable,
		    u.override->vid,
		    u.override->time);
		Output("  state=%s", OverrideState(u.override->state));
		if (u.override->state == MVEXRequestOverride)
		    printf(",override id=0x%x", u.override->override_id);
		break;

	    case MVEXRequest:
		printf("drawable=0x%x,vid=0x%x,time=0x%x\n",
		    u.override->drawable,
		    u.override->vid,
		    u.override->time);
		break;
	    }
	} else
	    Output("**Core %s Event\n", CoreEventNames[ event.type ]);
    }
}

SkipRemaining(dpy, skip)
    Display *dpy;
    char *skip;
{
    char prompt[ BUFSIZ ];

    sprintf(prompt,
	    "Press <ret> to continue or s<ret> to skip remaining %s",
	    skip);
    return (Prompt(dpy, prompt) == 's');
}

Prompt(dpy, prompt)
    Display *dpy;
    char *prompt;
{
    char buf[ BUFSIZ ];
    long files;
    int nFiles;

    nFiles = fileno(stdin) + 1;
    if (nFiles < ConnectionNumber(dpy) + 1)
	nFiles = ConnectionNumber(dpy) + 1;

    if (XEventsQueued(dpy, QueuedAlready))
	ShowEvents(dpy, 0);

    for (;;) {
	Output("%s... ", prompt);

	files = ((1 << fileno(stdin)) | (1 << ConnectionNumber(dpy)));
	if (select(nFiles, &files, 0, 0, 0) < 0) {
	    perror("can't select");
	    exit();
	}

	if (files & (1 << ConnectionNumber(dpy))) {
	    if (! XEventsQueued(dpy, QueuedAfterReading)) {
		printf("X server died\n");
		exit(1);
	    }
	    ShowEvents(dpy, 0);
	}

	if (files & (1 << fileno(stdin))) {
	    fgets(buf, BUFSIZ, stdin);
	    break;
	}
    }

    return (buf[0]);
}

Output(fmt, x0,x1,x2,x3,x4,x5,x6,x7,x8,x9)
    char *fmt;
{
    printf("%*s", IndentLevel * 2, "");
    printf(fmt, x0,x1,x2,x3,x4,x5,x6,x7,x8,x9);
    fflush(stdout);
}

char *PortName(dpy, port)
    Display *dpy;
    Atom port;
{
    char *portName = XGetAtomName(dpy, port);

    if (portName == NULL)
	portName = "unknown??";

    return (portName);
}
