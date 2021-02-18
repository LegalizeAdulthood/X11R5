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
**   xmvex2.c --- MVEX test program 2
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

static char *ViolationAction();
static char *ViolationState();
static char *OverrideState();
static char *MvexEventName();

int Priority = 100;


main(argc, argv)
int	    argc;
char	    *argv[];
{
    char chr;
    int ii, jj, kk, ll, grabbed;
    int xx,yy;

    Display *dpy;
    unsigned long screen;
    Visual *vis;
    XVisualInfo *p_vis_info, vis_info_tmpl;
    unsigned long vis_id;
    unsigned long depth;
    XEvent event;
    Colormap cmap;
    XColor scolor,ecolor;
    XSetWindowAttributes win_attr;
    Window still_wins[3][4];
    Window root,main_win,video_win;
    GC gc;
    XGCValues gc_vals;
    Bool video = False;

    Atom port;
    MVEXVin *pXvins;
    VideoIn vid;
    int nvins;
    MVEXOwner owner;
    MVEXVideoValues vv;
    int mvexBase;

    printf("\n	Welcome to MVEX test program #2\n\n");
    printf("  This program is just a little something I whipped up.  It\n");
    printf("  plays live video into the top main window and allows the\n");
    printf("  user to use the mouse to capture a template of scaled stills\n");

    printf("\n> Press return to continue...");
    chr = getc(stdin);

    dpy = XOpenDisplay(0);
    if (!dpy)
    {
	printf("\n  Couldn't open display\n");
	printf("\n  Xv test program #2 terminated\n");
	exit();
    }

    if (MVEXMajorVersion(dpy) < 1) {
	printf("\n  Your display has no MVEX extension.\n");
	exit();
    }

    screen = XDefaultScreen(dpy);
    root = XDefaultRootWindow(dpy);
    pXvins = MVEXGetVins( dpy, screen, &nvins);
    mvexBase = MVEXBaseEvent(dpy);

    printf("MVEX version %d.%d\n",
	MVEXMajorVersion(dpy),
        MVEXMinorVersion(dpy));

    if (!nvins)
    {
	printf("\n  Your display has no video inputs.\n");
	printf("\n  MVEX test program #2 terminated.\n");
	exit();
    }

    if (!Setup(argc, argv, dpy, nvins, pXvins, &vid, &port, &depth, &vis_id))
    {
	printf("\n  Setup failed.\n");
	printf("\n  MVEX test program #2 terminated\n");
	exit();
    }

    vis_info_tmpl.visualid = vis_id;
    p_vis_info = XGetVisualInfo(dpy, VisualIDMask, &vis_info_tmpl, &ii);
    if (!p_vis_info)
    {
	printf("Error: Couldn't find visual to use.\n");
	return;
    }
    vis = p_vis_info->visual;

    cmap = XCreateColormap(dpy, root, vis, AllocNone);

    XAllocNamedColor(dpy, cmap, "midnight blue", &scolor, &ecolor);

    win_attr.background_pixel = scolor.pixel;
    win_attr.event_mask = ExposureMask | ButtonPressMask | 
	KeyPressMask | VisibilityChangeMask;
    win_attr.colormap = cmap;
    win_attr.border_pixel = scolor.pixel;

    main_win = XCreateWindow(dpy, root, 0, 0, 720, 980, 0, 
	depth, InputOutput, vis, 
	CWBackPixel | CWEventMask | 
	CWBorderPixel | CWColormap, &win_attr);

    XAllocNamedColor(dpy, cmap, "black", &scolor, &ecolor);

    win_attr.background_pixel = scolor.pixel;

    video_win = XCreateWindow(dpy, main_win, 40, 4, 640, 480, 0, 
	depth, InputOutput, vis, 
	CWBackPixel | CWEventMask | CWColormap, &win_attr);
    XMapWindow(dpy, video_win);

    yy = 488;
    for (ii=0; ii<3; ii++)
    {
	xx = 16;
	for (jj=0; jj<4; jj++)
	{
	    still_wins[ii][jj] = XCreateWindow(dpy, main_win, xx, yy, 160, 160, 
		0, depth, 
		InputOutput, vis, 
		CWBackPixel | CWEventMask | 
		CWColormap, 
		&win_attr);
	    XMapWindow(dpy, still_wins[ii][jj]);
	    xx += 160 + 16;
	}
	yy += 160 + 4;
    }

    XMapWindow(dpy, main_win);

    printf("\n	Waiting for main window to become visible...\n");
    while (1)
    {
	XNextEvent(dpy, &event);
	if (event.type == VisibilityNotify) break;
    }

    printf("\n  MVEXSelectEvents on window 0x%x %s\n",
	video_win, "(Override and Violation)");
    MVEXSelectEvents(dpy, video_win, MVEXOverrideMask | MVEXViolationMask);
    printf("  MVEXSelectEvents on vid 0x%x (Sync)\n", vid);
    MVEXSelectEvents(dpy, vid, MVEXSyncMask);

    printf("\n");
    printf("  Try pressing other mouse buttons in video window.\n");
    printf("  Hit any key to toggle ownership of video input\n");
    printf("  (this is *really* a video manager task)\n");
    printf("  Hit ^c to exit\n");
    printf("\n");

    gc_vals.foreground = scolor.pixel;
    gc = XCreateGC(dpy, video_win, GCForeground, &gc_vals);

    ii = 0;
    jj = 0;
    grabbed = 0;

    vv.priority = Priority;
    MVEXRenderVideo(dpy, vid, video_win, gc, port,
		    0,0,640,480,
		    0,0,640,480,
		    VRPriority, &vv);
    video = True;

    while (1)
    {
	XNextEvent(dpy, &event);

	if (event.type == KeyPress)
	{
	    if ((XLookupKeysym(&event.xkey,0) == 'c') && 
		(event.xkey.state & ControlMask))
	    {
		break;
	    }
	    if (!grabbed)
	    {
		owner.window = video_win;
		owner.vid = vid;
		MVEXChangeOwnership(dpy, &owner, 1);
		printf("  Ownership taken\n");
		grabbed = 1;
	    }
	    else 
	    {
		owner.window = root;
		owner.vid = vid;
		MVEXChangeOwnership(dpy, &owner, 1);
		printf("  Ownership relinquished\n");
		grabbed = 0;
	    }
	}
	else if (event.type == ButtonPress)
	{
	    if (! video) {
		vv.priority = Priority;
		MVEXRenderVideo(dpy, vid, video_win, gc, port,
				0,0,640,480,
				0,0,640,480,
				VRPriority, &vv);
		video = True;
	    }

	    if (event.xbutton.button == Button3)
	    {
		if (jj==0)
		{
		    jj=3;
		    if (ii==0) ii=2;
		    else ii--;
		}
		else
		    jj--;
	    }
	    if (event.xbutton.button == Button2)
	    {
		if (jj==3)
		{
		    jj=0;
		    if (ii==2) ii=0;
		    else ii++;
		}
		else
		    jj++;
	    }
	    if ((event.xbutton.window == video_win) ||
		(event.xbutton.button != Button1))
	    {
		vv.full_motion = False;
		vv.priority = Priority;
		MVEXRenderVideo(dpy, vid, still_wins[ii][jj], gc, port,
				0,0,640,480,
				0,0,160,160,
				VRPriority | VRFullMotion, &vv);

	    }
	    else
	    {
		for (kk=0; kk<3; kk++)
		{
		    for (ll=0; ll<4; ll++)
		    {
			if (still_wins[kk][ll] == event.xbutton.window)
			{
			    ii = kk;
			    jj = ll;
			    vv.full_motion = False;
			    vv.priority = Priority;
			    MVEXRenderVideo(dpy, vid, event.xbutton.window,
					    gc, port,
					    0,0,640,480,
					    0,0,160,160,
					    VRPriority | VRFullMotion, &vv);
			    break;
			}
		    }
		    if (ll < 4) break;
		}
	    }
	}
	else if (event.type >= mvexBase)
	{
	    union {
		MVEXViolationEvent *violate;
		MVEXSetupEvent *setup;
		MVEXSyncEvent *sync;
		MVEXOverrideEvent *override;
		MVEXRequestEvent *request;
		XEvent *core;
	    } u;

	    u.core = &event;
	    printf("**Mvex %s Event, ", MvexEventName(event.type - mvexBase));
	    switch(event.type - mvexBase) {
	    case MVEXViolation:
		printf("drawable=0x%x,vid=0x%x,time=0x%x\n",
		    u.violate->drawable,
		    u.violate->vid,
		    u.violate->time);
		printf("  action=%s,state=%s\n",
		    ViolationAction(u.violate->action_mask),
		    ViolationState(u.violate->state));
		break;

	    case MVEXSetup:
		printf("vid=0x%x,port=0x%x,time=0x%x,\n",
		    u.setup->vid,
		    u.setup->port,
		    u.setup->time);
		printf("  frames %savailable\n",
		    u.setup->frames_available ? "" : "not ");
		printf("  timecodes %savailable\n",
		    u.setup->timecodes_available ? "" : "not ");
		printf("  %sframe accurate\n",
		    u.setup->frame_accurate ? "" : "not ");
		break;

	    case MVEXSync:
		printf("drawable=0x%x,vid=0x%x,time=0x%x\n",
		    u.sync->drawable,
		    u.sync->vid,
		    u.sync->time);
		printf("  state=%s\n",
		    u.sync->state == MVEXSyncAcquired
		      ? "acquired" : "lost");
		break;

	    case MVEXOverride:
		printf("drawable=0x%x,vid=0x%x,time=0x%x\n",
		    u.override->drawable,
		    u.override->vid,
		    u.override->time);
		printf("  state=%s", OverrideState(u.override->state));
		if (u.override->state == MVEXRequestOverride)
		    printf(",override id=0x%x", u.override->override_id);
		printf("\n");
		if (u.override->state == MVEXRequestOverride
		 && u.override->override_id != vid) {
		    printf("Override by another client!\n");
		    printf("Waiting for input before starting again\n");
		    video = False;
		} else {
		    vv.priority = Priority;
		    MVEXRenderVideo(dpy, vid, video_win, gc, port,
				    0,0,640,480,
				    0,0,640,480,
				    VRPriority, &vv);
		    video = True;
		}
		break;

	    case MVEXRequest:
		printf("drawable=0x%x,vid=0x%x,time=0x%x\n",
		    u.override->drawable,
		    u.override->vid,
		    u.override->time);
		break;

	    default:
		printf("\n");
		break;
	    }
	}
    }
}




Setup(argc, argv, dpy, nvins, pXvins, p_vid, p_port, p_depth, p_vis_id)
    int	    argc;
    char    *argv[];
    Display *dpy;
    int nvins;
    MVEXVin *pXvins;
    VideoIn *p_vid;
    Atom *p_port;
    unsigned long *p_depth;
    unsigned long *p_vis_id;
{
    int ii;
    int depth, visual_id, vref, portnum;
    Atom port;
    VideoIn vid;
    MVEXVin *pXvin;
    MVEXRenderModel *pModel;

    vid = vref = portnum = depth = visual_id = -1;

    /* LOOK THROUGH COMMAND LINE ARGUMENTS */

    for ( ii = 1; ii < argc; ii++ )
    {
	if (strcmp( argv[ii], "-vref") == 0)
	{
	    if(++ii < argc)
		vref = atoi(argv[ii]);
	    else
		UseMsg(argv);
	}
	else if (strcmp( argv[ii], "-port") == 0)
	{
	    if(++ii < argc)
		portnum = atoi(argv[ii]);
	    else
		UseMsg(argv);
	}
	else if (strcmp( argv[ii], "-depth") == 0)
	{
	    if(++ii < argc)
		depth = atoi(argv[ii]);
	    else
		UseMsg(argv);
	}
	else if (strcmp( argv[ii], "-visual") == 0)
	{
	    if(++ii < argc)
		visual_id = atoi(argv[ii]);
	    else
		UseMsg(argv);
	}
	else if (strcmp( argv[ii], "-priority") == 0)
	{
	    if(++ii < argc)
		Priority = atoi(argv[ii]);
	    else
		UseMsg(argv);
	}
	else if (strcmp( argv[ii], "-help") == 0)
	{
	    UseMsg(argv);
	}
    }

    if (vref < 0) {
	vref = 1;
	pXvin = pXvins;
    }
    else
    {
	if (vref > nvins)
	{
	    printf("\n	vref #%d doesn't exist (there's only %d).\n",
		vref, nvins);
	    return False;
	}
	pXvin = pXvins+(vref-1);
    }
    vid = MVEXCreateVideo(dpy, pXvins->vref);

    if (portnum < 0)
    {
	port = pXvin->ports[ 0 ];
    }
    else
    {
	if (portnum > pXvin->n_ports)
	{
	    printf("\n	Port #%d doesn't exist for video input #%d.\n", 
		portnum, vref);
	    return False;
	}
	port = pXvin->ports[ portnum-1 ];
    }

    pModel = &pXvin->model_list[ 0 ];
    if (depth < 0)
    {
	for (ii=0; ii<pXvin->n_models; ii++, pModel++)
	    if (pModel->model_mask & MVEXWindowModelMask)
	    {
		depth = pModel->depth;
		break;
	    }
	if (depth < 0) {
	    printf("\n  no window depths available for video input #%d\n",
		vref);
	    return False;
	}
    }
    else
    {
	for (ii=0; ii<pXvin->n_models; ii++, pModel++)
	    if (pModel->depth == depth)
		break;

	if (ii >= pXvin->n_models)
	{
	    printf("\n	Depth %d not supported by video input #%d.\n",
		depth, vref);
	    return False;
	}
    }

    if (visual_id < 0)
    {
	visual_id = pModel->visualid;
    }
    else
    {
	pModel = &pXvin->model_list[ 0 ];
	for (ii=0; ii<pXvin->n_models; ii++, pModel++)
	    if ((pModel->visualid == visual_id) && pModel->depth == depth)
		break;

	if (ii >= pXvin->n_models)
	{
	    printf("\n	Visual-id %d at depth %d not supported ",
		depth, visual_id);
	    printf("by video input #%d.\n", vref);
	    return False;
	}
    }

    *p_vid = vid;
    *p_port = port;
    *p_depth = depth;
    *p_vis_id = visual_id;

    return True;
}

UseMsg(argv)
char	    *argv[];
{
    printf("use: %s [option]\n", argv[0]);
    printf("\t-vref #		       vref number (i.e., 1, 2, 3...)\n");
    printf("\t-port #		       port (i.e., 1, 2, 3...)\n");
    printf("\t-depth #		       drawable depth (planes)\n");
    printf("\t-visual #		       drawable visual (id)\n");
    printf("\t-priority #	       priority level 1-100\n");
    exit();
}

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

