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
static char *rcsid=  "$Header: /n/lacey/usr/local/src/video/MVEX/server/include/RCS/videostr.h,v 2.3 1991/09/26 21:28:32 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#ifndef _VIDEOSTR_H_
#define _VIDEOSTR_H_

/*
 * These should probably move out to another header file??
 */
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "dixstruct.h"
#include "gcstruct.h"
#include "video.h"

#ifdef XDEBUG
/*
 * This define appears as a single statement and can be used anywhere, e.g.
 *	assert(foo < 0);
 * doassert() avoids complaints from lint.
 */
#define assert(ex) 							\
	((ex) ? 0 :							\
	    (FatalError("Assertion in \"%s\", line %d failed:\n\t%s\n",	\
		__FILE__, __LINE__, "ex"), 1))
#define doassert() 							\
	    (FatalError("Assertion in \"%s\", line %d failed:\n\t%s\n",	\
		__FILE__, __LINE__, "ex"), 1)
#else
#define assert(ex)
#define doassert()
#endif

/* XXX
 * Replace the global "ScreenVideoInfo" array someday with a devPriv entry on
 * each screen.
 */
#define VSCREEN_OF_PDRAW(pDraw)  \
			(ScreenVideoInfo[ pDraw->pScreen->myNum ])
#define VDRAW_OF_PDRAW(pDraw) \
  ((VideoDrawPtr) LookupIDByType(pDraw->id, VSCREEN_OF_PDRAW(pDraw)->vDrawType))

#define VRES_OF_VID(vid) \
    ((VideoResourcePtr) LookupIDByType(vid, ScreenVideoInfo[0]->vIdType))


extern int VideoErrorBase;

/*
 * typedefs that are used below
 */
typedef struct _videoDrawRec	  *VideoDrawPtr;
typedef struct _VideoEventElement *VideoEventElementPtr;
typedef struct _VideoElement	  *VideoElementPtr;
typedef struct _screenIndexRec	  *screenIndexPtr;

typedef struct _VideoEventElement {
    ClientPtr client;
    VideoEventElementPtr next;
} VideoEventElement /* VideoEventElementPtr defined above */;

typedef struct {
    xVideoAbility xva;		/* the protocol xVideoAbility */
    int nAdjustments;		/* as a helper, the total # of FractionRanges */
    xFractionRange *pAdjustments;
} VideoAbility;

typedef struct {
    xVideoGeometry xvg;
    xPlacement *pPlacements;
} VideoGeometry;

typedef struct _videoVisual {
    xRenderModel xrm;
    unsigned char model;	/* WindowModel|PixmapModel|CompositeModel */
} VideoVisual;

typedef struct {
    Bool framesAvailable;	/* protocol value */
    Bool timecodesAvailable;	/* protocol value */
    Bool frameAccurate;		/* protocol value */
} Setup;

typedef struct _portList {
    int nPorts;			/* count of ports for a particular vin or vout */
    Atom *ports;		/* list of atoms that name the ports */
    char **portNames;		/* ddx: strings used to make atoms */
    int *devices;		/* ddx: device private data for identifying
				 * corresponding ports */
    Setup *setups;		/* values for QuerySetup and SetupEvent */
} VioPortList;

typedef struct _VioDataRec {
    unsigned char nAllowedDepths;
    VideoVisual *pAllowedDepths; /* List of which depths and visual ids are
				  * supported for use with this video input or
				  * output. Selected from MVEX list and
				  * core list.  In no particular order */
    VideoGeometry *pattr;
    VideoAbility *pAbility;
    xRectangle *pClipSize;	/* Used only for VideoIn */
    VioPortList *pPortList;
    unsigned long timeout;	/* milliseconds between calls to (*PollVideo) */
    unsigned long lastCall;	/* time of last call (maintained by ddx) */

    Bool (* SetupVideo)();	/* get the hardware ready for video in/out */
    Bool (* StartVideo)();	/* start the hardware if its not already */
    void (* StopVideo)();	/* stop the hardware */
    void (* PollVideo)();	/* poll the hardware for changes in sync */
    void (* QuerySetup)();	/* for protocol QuerySetup */
    void (* RealizeWindow)();	/* hook for window map */
    void (* UnrealizeWindow)();	/* hook for window unmap/destroy */
    void (* ClipNotify)();	/* hook for window clip changes */
    void (* GetCompositeClip)();/* ddx method for getting clip out of gc */
    Bool (* ValidateRequest)();	/* VideoScreenPtr, VideoActionPtr */
} VioDataRec, *VioDataPtr;

typedef struct _VideoElement {
    char type;				/* Input or Output */
    short nControl;
    unsigned long id;			/* VReference */
    VioDataPtr pvio;			/* Data for VideoInput or VideoOutput 
					 * (will be NULL for VideoDevice) */
    void (* Reset)();			/* Set this to NoOp if not needed */
    pointer devPrivate;
    VideoElementPtr next;
} VideoElement /* VideoElementPtr defined above */;

typedef struct _ActiveVideo {
    int nEntries;
    int maxEntries;
    VideoDrawPtr *vDrawArray;
} ActiveVideo, *ActiveVideoPtr;

typedef struct _VideoScreen {
    /*
     * dix stuff
     */
    XID id;			/* XID for Resource mgt, e.g. event selection */
    RESTYPE vDrawType;		/* Associates a VideoDrawRec with a 
				 * Drawable ID */
    RESTYPE vResType;		/* Associates a VideoResourceRec with a 
				 * VideoReference ID */
    RESTYPE vIdType;		/* Associates a VideoResourceRec with a 
				 * VideoIn or VideoOut */
    RESTYPE vClientOwnerType;
    RESTYPE vWinOwnerType;
    TimeStamp lastVideoChangeTime;

    ScreenPtr pScreen;
    short numVisuals;
    VisualPtr visuals;		/* Array of visuals provided by MVEX on this
				 * screen which are NOT supplied by the core
				 * visuals list (in pScreen->visuals) */
    int nDepths;
    DepthPtr pDepths;		/* Array of DepthRec structures for depths
				 * provided by MVEX on this screen which are NOT
				 * supplied by the core "pScreen->allowedDepths"
				 * list */
    unsigned char inputOverlap;   /* FALSE if VIRs may not overlap */
    unsigned char captureOverlap; /* FALSE if VORs may not overlap */
    unsigned char ioOverlap;	  /* FALSE if a VIR and a VOR may not overlap */

    ActiveVideo activeVDraws;	/* record of active video render/captures */

    VideoElementPtr vElements;

    pointer		devPrivate;

    /*
     * protocol vectors.
     */
    Bool (* QueryVideo)();		/* pScreen, pVideoInfo->deviceInfo */

    /*
     * utility vectors
     */
    void (* Reset)();
    void (* BlockHandler)();	/* for starting delayed renders/captures */
    void (* WakeupHandler)();	/* for polling devices for change in sync */

    /*
     * Saved vectors from the screen.  Replaced by dix vectors.
     */
    Bool (* CreateWindow)();
    void (* CopyWindow)();
    Bool (* RealizeWindow)();		/* pWin */
    Bool (* UnrealizeWindow)();		/* pWin */
    void (* ClipNotify)();		/* pWin, dx, dy */

    /*
     * also saved vectors, but are replaced by ddx vectors
     */
    void (* GetImage)();		/* pDrawable, sx, sy, w, h, format,
					 * planemask, pdestbits */
    Bool (* CreateGC)();                /* pGC */
} VideoScreenRec, *VideoScreenPtr;

typedef struct _ownership {
    ClientPtr		pClient;	/* The owning client */
    unsigned long	clientID;	/* A fake client XID */
    unsigned long	window;		/* A window XID */
    long		inUse;		/* TRUE if this ownership is applied
					 * to a current use */
} Ownership;

typedef struct _resourceUsage {
    ClientPtr		pClient;	/* client issuing the Render/Capture */
    DrawablePtr		pDraw;		/* Drawable being Rendered/Captured */
    VideoIO		videoIO;	/* VideoIO used in Render/Capture */
    long		owned;		/* TRUE is this use has an ownership */
} ResourceUsage;

typedef struct _screenIndexRec {
    ScreenPtr		pScreen;
    screenIndexPtr	next;
} screenIndexRec /* screenIndexPtr defined above */;

/*
 * holds references that identify a particular video resource
 */
typedef struct _videoResourceRec {
    short		ownerCount;	/* Count of current owners */
    short		useCount;	/* Count of current uses */
    short		maxUse;		/* Maximum of concurrent ownerships
					 * or uses */
    VideoElementPtr	vElement;	/* Video element data */
    screenIndexPtr	screens;	/* List of screens this resource is 
				 	 * attached to */
    Ownership		*owners;	/* array of ownership info */
    ResourceUsage	*usage;		/* array of usage info */
    VideoScreenPtr	vp;		/* for quick reference */
} VideoResourceRec, *VideoResourcePtr;

typedef struct _actionGeometry {
    int	srcX, srcY;
    int dstX, dstY;
    int srcWidth, srcHeight;
    int dstWidth, dstHeight;
} ActionGeometry;

/*
 * Holds the pertinent information for a RenderVideo or CaptureGraphics.
 */
typedef struct _videoActionRec {
    int request;		/* MVEXRender or MVEXCapture */
    ActionGeometry dix;		/* that specified in the request */
    ActionGeometry ddx;		/* private to ddx */
    Bool fullMotion;
    int priority;
    int markerType;		/* whether frame or timecode */
    int markerSpecified;	/* which of {in,out}{Frame,Timecode} */
				/* was specified */
    int port;			/* index of in/out port */
    xFraction saturation;
    xFraction contrast;
    xFraction hue;
    xFraction bright;
    xFrame inFrame;
    xFrame outFrame;
    xTimecode inTimecode;
    xTimecode outTimecode;
    Bool freeCompClip;		/* true if pClip was created by mvex */
    RegionPtr pClip;		/* pointer to window/gc clip */
    RegionPtr pGcClip;		/* a copy of the original gc clip */
    int subWindowMode;
    VideoDrawPtr vDraw;		/* mvex info about the drawable */
    VideoResourcePtr vRes;	/* mvex info about the video IO */
    DrawablePtr drawable;	/* source or destination */
    VideoIO videoIO;		/* VideoIn or VideoOut */
    ColormapPtr pcmap;		/* Used for pixmap or window Capture */
    Bool running;		/* true if ddx says video is running */
    Bool hasSync;		/* maintained by ddx */
} VideoActionRec, *VideoActionPtr;

/*
 * holds mvex-specific stuff for video windows and pixmaps.
 */
typedef struct _videoDrawRec {
    short	renderActive;	/* true if dix thinks so */
    short	captureActive;	/* true if dix thinks so */
    DrawablePtr	pDraw;
    VideoIn	videoIn;
    VideoOut	videoOut;
    /*
     * drawableWidth and drawableHeight are only used to determine if a
     * window has changed size.  We could have made a small change in
     * core dix instead.
     */
    unsigned short drawableWidth;	/* width of drawable the last time
					 * video was (re)started in it */
    unsigned short drawableHeight;	/* height of drawable the last time 
					 * video was (re)started in it */
    VideoActionRec captureAction;	/* Contains info from the original
					 * CaptureGraphics request */
    VideoActionRec renderAction;	/* Contains info from the original
					 * RenderVideo request */
} VideoDrawRec /* VideoDrawPtr defined above */;

extern VideoScreenPtr	*ScreenVideoInfo;

extern VideoDrawPtr GetVideoDrawable();	/* VideoScreenPtr, id */
extern VideoScreenRec *CreateVideoScreenRec();

#endif /* _VIDEOSTR_H_ */
