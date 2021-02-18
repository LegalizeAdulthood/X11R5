/*
 * Copyright 1990 by Tektronix, Inc., Beaverton, Oregon, and
 * the Massachusetts Institute of Technology, Cambridge, Massachusetts.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Tektronix or M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Tektronix and M.I.T. make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * TEKTRONIX AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL TEKTRONIX OR M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: /n/lacey/usr/local/src/video/MVEX/include/RCS/MVEXlib.h,v 2.4 1991/09/26 03:59:20 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#ifndef _MVEXLIB_H_
#define _MVEXLIB_H_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/MVEX/MVEX.h>


/*
 * New types for video extension client library
 */

/*
 * Base QueryVideo types
 */
typedef struct {
    VideoIO	vio;
    Atom	port;
    Bool	frames_available;
    Bool	timecodes_available;
    Bool	frame_accurate;
} MVEXSetupState;

typedef struct {
    Bool	negative;
    int		hour;
    int		minute;
    int		second;
    int		frame;
    int		field;
} MVEXTimecode;

typedef struct {
    Bool	negative;
    int		frame;
    int		field;
} MVEXFrame;

typedef struct {
    long	numerator;
    long	denominator;
} MVEXFraction;

typedef struct {
    long	num_base;
    long	num_increment;
    long	num_limit;
    int		num_type;	/* MVEXGeometricRange or MVEXLinearRange */
    long	denom_base;
    long	denom_increment;
    long	denom_limit;
    int		denom_type;	/* MVEXGeometricRange or MVEXLinearRange */
} MVEXFractionRange;

typedef struct {
    XRectangle	base;
    XRectangle	limit;
    int		x_inc;
    int		y_inc;
    int		width_inc;
    int		height_inc;
    int		type;	/* MVEXGeometricRange or MVEXLinearRange */
} MVEXRectangleRange;

typedef struct {
    MVEXFraction	frame_rate;	/* max nominal rate (frames/sec) */
    MVEXRectangleRange	src;	/* set of possible source rectangles */
    MVEXRectangleRange	dest;	/* set of possible dest rectangles */
    MVEXFractionRange	x_scale; /* set of dst/src width ratios */
    MVEXFractionRange	y_scale; /* set of dst/src height ratios */
    Bool		identity_aspect; /* true if x and y scale must be = */
} MVEXPlacement;

typedef struct {
    MVEXFraction	frame_rate;	/* frames/sec in signal format */
    MVEXFraction	field_rate;	/* fields/sec in signal format */
    int			width;		/* signal width (in pixels) */
    int			height;		/* signal height (in pixels) */
    int			priority_steps;	/* priority resolution */
    int			concurrent_use;	/* max simultaneous use */
    int			nplacement;	/* number of placements */
    MVEXPlacement	*placement;	/* placement list */
} MVEXGeometry;

typedef struct {
    Window		window;	/* identifies client requesting ownership */
    VideoIO		vid;	/* resource to be owned */
} MVEXOwner;

/*
 * Aggregate QueryVideo types
 */
typedef struct {
    int depth;				/* depth */
    VisualID visualid;			/* visual ID used for colormaps in */
					/* CaptureGraphics, window in */
					/* RenderVideo */
    Bool opaque;			/* True if video input pixels cannot */
					/* be interpreted by a Core type */
    unsigned long	model_mask;	/* Window, Pixmap and/or Composite */
    unsigned long	red_mask;
    unsigned long	green_mask;
    unsigned long	blue_mask;
} MVEXRenderModel;

typedef struct {
    MVEXFraction	normal_saturation;
    MVEXFraction	normal_contrast;
    MVEXFraction	normal_hue;
    MVEXFraction	normal_bright;
    int			n_saturation;
    int			n_contrast;
    int			n_hue;
    int			n_bright;
    MVEXFractionRange	*saturation;
    MVEXFractionRange	*contrast;
    MVEXFractionRange	*hue;
    MVEXFractionRange	*bright;
} MVEXVideoAbility;

typedef struct {
    Bool		is_input;	/* helper: true if this if video in */
    VideoReference	vref;		/* VideoIn reference ID */
    MVEXGeometry	geometry;
    int			n_models;
    MVEXRenderModel	*model_list;
    int			n_ports;
    Atom		*ports;
    MVEXVideoAbility	ability;
    XRectangle		clip_size;
} MVEXVin;

typedef struct _mvexvout {
    Bool		is_input;	/* helper: false if this if video out */
    VideoReference	vref;		/* VideoOut reference ID */
    MVEXGeometry	geometry;
    int			n_models;
    MVEXRenderModel	*model_list;
    int			n_ports;
    Atom		*ports;
    MVEXVideoAbility	ability;
} MVEXVout;

typedef struct _mvexvout MVEXVio;

/*
 * New events for video extension client library
 * A "XEvent" structure always  has type as the first entry.  This
 * uniquely identifies what  kind of event it is.  The second entry
 * is always a pointer to the display the event was read from.
 * The third entry is always a window of one type or another,
 */

typedef struct {
	int type;		/* of event */
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Drawable drawable;	/* window on which Violation was selected */
	VideoIO	vid;
	Time	time;		/* current server time */
	unsigned long	action_mask;  /* Scale, Placement, Clip, Overlap */
	int	state;		/* Success, Fail, Subset */
} MVEXViolationEvent;

typedef struct {
	int type;		/* of event */
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Drawable window;	/* unused */
	VideoIO	vid;		/* vid on which Render/Capture was requested */
	Atom	port;		/* port setup changed */
	Time	time;		/* current server time */
	Bool	frames_available; /* true if frames can be read */
	Bool	timecodes_available; /* true if timecodes can be read */
	Bool	frame_accurate; /* true if requests are frame accurate */
} MVEXSetupEvent;

typedef struct {
	int type;		/* of event */
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Drawable drawable;	/* dest drawable for Render/Capture request */
	VideoIO	vid;		/* video id on which Sync was selected */
	Time	time;		/* current server time */
	int	state;		/* Acquired or Lost */
} MVEXSyncEvent;

typedef struct {
	int type;		/* of event */
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Drawable drawable;	/* drawable on which Override was selected */
	VideoIO	vid;		/* vid on which Render/Capture was requested */
	VideoIO	override_id;	/* vid in overriding request */
	Time	time;		/* current server time */
	int	state;		/* Reason for override */
} MVEXOverrideEvent;

typedef struct {
	int type;		/* of event */
	unsigned long serial;	/* # of last request processed by server */
	Bool send_event;	/* true if this came from a SendEvent request */
	Display *display;	/* Display the event was read from */
	Window	window;		/* unused */
	XID src;
	XID dst;
	Bool	redirect;
	int	request;	/* MVEXRender or MVEXCapture */
	Time	time;		/* current server time */
} MVEXRequestEvent;

typedef struct {
	int full_motion;
	int priority;
	int marker_type;
	MVEXFrame in_frame;
	MVEXFrame out_frame;
	MVEXTimecode in_timecode;
	MVEXTimecode out_timecode;
	MVEXFraction hue;
	MVEXFraction saturation;
	MVEXFraction bright;
	MVEXFraction contrast;
} MVEXVideoValues;

#define FracToFloat(frac) (((float)(frac)->numerator) \
			 / ((float)(frac)->denominator))

/*
 * MVEX function declarations.
 */
extern Status MVEXQueryVideo();
extern VideoIO MVEXCreateVideo();
extern Status MVEXAddVideoID();
extern XVisualInfo *MVEXGetVisualInfo();
extern MVEXVin *MVEXGetVins();
extern MVEXVout *MVEXGetVouts();
extern MVEXCaptureGraphics();
extern MVEXChangeOwnership();
extern MVEXRenderVideo();
extern MVEXSelectEvents();
extern MVEXStopVideo();
extern Status MVEXOverlapsOfScreen();
extern Status MVEXTimestamps();
extern Bool MVEXMatchFraction();
extern Bool MVEXMatchFractionInList();
extern MVEXFraction *MVEXFractionList();
extern int MVEXFractionCount();
extern Bool MVEXMatchRectangle();
extern int MVEXBaseOpCode();
extern int MVEXBaseError();
extern int MVEXBaseEvent();

#endif /* _MVEXLIB_H_ */
