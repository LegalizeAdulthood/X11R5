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
static char *rcsid=  "$Header: /n/lacey/usr/local/src/video/MVEX/lib/RCS/MVEXQuVid.c,v 2.3 1991/09/26 04:01:16 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#define NEED_REPLIES
#include "Xlibint.h"
#include "MVEXlibint.h"
#include "Xutil.h"

/*
 *    NAME
 *        MVEXQueryVideo - update the internal structures
 *
 *    SYNOPSIS
 */
Status
MVEXQueryVideo(dpy, screen_number)
    Display *dpy;		/* in: display pointer */
    int screen_number;		/* in: screen */
/*
 *    DESCRIPTION
 *        Free current structure and get a new one.
 *
 *    RETURNS
 *        0 for failure, 1 for success.
 */
{
    XExtCodes	*codes;
    XEDataObject object;
    XExtData **pScreenXExtData;
    XExtData *videoXExtData;
    unsigned long videoConnectivityTime;
    unsigned long queryVideoTime;

    if (!(codes = _MVEXCheckExtension (dpy)))
	return 0;

    object.screen = ScreenOfDisplay(dpy, screen_number);
    pScreenXExtData = XEHeadOfExtensionList(object);
    videoXExtData = XFindOnExtensionList(pScreenXExtData, codes->extension);
    _MVEXFreeVideoInfo((VideoInfo *)videoXExtData->private_data);
    (void) _MVEXQueryVideo(dpy, RootWindow(dpy, screen_number),
	    (VideoInfo **)&videoXExtData->private_data, codes);
    /*
     * If saved videoConnectivityTime is more recent than queryVideoTime,
     * set videoConnectivityTime in VideoInfo structure to the saved time.
     * Otherwise set videoConnectivityTime to queryVideoTime.
     */
    if (videoXExtData->private_data) {
	videoConnectivityTime = 
		((VideoInfo *)videoXExtData->private_data)->video_connect_time;
	queryVideoTime = 
		((VideoInfo *)videoXExtData->private_data)->query_video_time;
	if (    ((videoConnectivityTime > queryVideoTime) &&
		    (videoConnectivityTime - queryVideoTime) < 
		    (unsigned long)(1 << 31)) ||
		((videoConnectivityTime < queryVideoTime) &&
		    (queryVideoTime - videoConnectivityTime) < 
		    (unsigned long)(1 << 31))) {
	    ((VideoInfo *)videoXExtData->private_data)->video_connect_time =
		    videoConnectivityTime;
	} else
	    ((VideoInfo *)videoXExtData->private_data)->video_connect_time =
		   ((VideoInfo *)videoXExtData->private_data)->query_video_time;
    }

    return 1;
}

#define CARD32		unsigned long
#define sz_CARD32	4
/*
 *    NAME
 *        _VEXQueryVideo - request and parse the QueryVideo reply
 *
 *    SYNOPSIS
 */
Status
_MVEXQueryVideo(dpy, w, vinfo, codes)
    Display *dpy;		/* in: display pointer */
    Window w;			/* in: window */
    VideoInfo **vinfo;		/* in: video info pointer */
    XExtCodes   *codes;		/* in: extension codes */
/*
 *    DESCRIPTION
 *        Format and send the request, parse the reply.
 *
 *    RETURNS
 *        0 for failure, 1 for success
 */
{
    register xQueryVideoReq *req;
    xQueryVideoReply	    rep;
    char *pReplyData;
    int i,j,k;
    int dataLength;
    unsigned int size;
    VideoInfo *pVideoInfo;
    int nBytes;
    int nPorts;
    Atom *ports;
    unsigned char *adjMats;
    int             nvio;
    int             nallowed_depths;
    xRenderModel     *allowed_depths;
    unsigned char model_mask;
    MVEXRenderModel *ioModels;
    MVEXVideoAbility *pAbility;
    MVEXFractionRange *pFR;
    int nModels;
    int nvid;
    VideoIO vid;
    int type;
    union {
	    xDepth *dp;
	    xVisualType *vp;
	    xRenderModel *rmp;
	    xVideoGeometry *vgp;
	    xVideoAbility *vap;
	    xPlacement *pp;
	    xFractionRange *pfr;
	    CARD32 *card32p;
	    unsigned char *iomdl;
	    unsigned char *nPorts;
	    char *charp;
	    xRectangle *rectp;
    } u;

    *vinfo = (VideoInfo *) NULL;

    LockDisplay (dpy);
    GetReq (QueryVideo, req);
    req->reqType = codes->major_opcode;
    req->videoReqType = X_QueryVideo;
    req->window = w;

    if (!_XReply (dpy, (xReply *) &rep, 0, xFalse)) {
	UnlockDisplay (dpy);
	SyncHandle ();
	return 0;
    }

    /*
     * Create a VideoInfo struct and fill in the "non-pointer" values
     */
    pVideoInfo = (VideoInfo *) Xmalloc(sizeof(VideoInfo));
    if (!pVideoInfo)
	return 1;
    pVideoInfo->screen = rep.screen;
    pVideoInfo->input_overlap = rep.inputOverlap;
    pVideoInfo->capture_overlap = rep.captureOverlap;
    pVideoInfo->io_overlap = rep.ioOverlap;
    pVideoInfo->query_video_time = rep.time;
    pVideoInfo->major_version = rep.majorVersion;
    pVideoInfo->minor_version = rep.minorVersion;
    pVideoInfo->ndepths = rep.nDepths;
    pVideoInfo->nvin = rep.nVin;
    pVideoInfo->nvout = rep.nVout;

    nallowed_depths = rep.nAllowedDepths;

    /*
     * Read all of the lists of data from the connection into the "u" structure
     * XXX Maybe if the reply is HUGE, this isn't such a good idea -- we should
     * just read one list at a time.
     */
    dataLength = (rep.length << 2);
    pReplyData = Xmalloc ((unsigned) dataLength);
    u.dp = (xDepth *) pReplyData;
    _XRead (dpy, pReplyData, dataLength);

    /*
     * Read in Depth structures
     */
    if (pVideoInfo->ndepths) {
	pVideoInfo->depths =
	    (Depth *)Xmalloc(pVideoInfo->ndepths*sizeof(Depth));
	for (j = 0; j < pVideoInfo->ndepths; j++) {
	    Depth *dp = &pVideoInfo->depths[j];
	    dp->depth = u.dp->depth;
	    dp->nvisuals = u.dp->nVisuals;
	    u.dp = (xDepth *) (((char *) u.dp) + sz_xDepth);
	    if (dp->nvisuals > 0) {
		dp->visuals = 
		  (Visual *)Xmalloc((unsigned)dp->nvisuals*sizeof(Visual));
		for (k = 0; k < dp->nvisuals; k++) {
		    register Visual *vp = &dp->visuals[k];
		    vp->visualid = u.vp->visualID;
		    vp->class	= u.vp->class;
		    vp->bits_per_rgb= u.vp->bitsPerRGB;
		    vp->map_entries	= u.vp->colormapEntries;
		    vp->red_mask	= u.vp->redMask;
		    vp->green_mask	= u.vp->greenMask;
		    vp->blue_mask	= u.vp->blueMask;
		    vp->ext_data	= NULL;
		    u.vp = (xVisualType *) (((char *) u.vp) +
					    sz_xVisualType);
		}
	    } else {
		dp->visuals = (Visual *) NULL;
	    }
	}
    } else 
	pVideoInfo->depths = (Depth *) NULL;

    /*
     * "Remember" allowedDepths
     */
    allowed_depths = u.rmp;
    u.rmp = (xRenderModel *) 
	    (((char *) u.rmp) + nallowed_depths*sz_xRenderModel);
    
    pVideoInfo->vin = (MVEXVin *)
	    Xmalloc(pVideoInfo->nvin*sizeof(MVEXVin));
    pVideoInfo->vout = (MVEXVout *)
	    Xmalloc(pVideoInfo->nvout*sizeof(MVEXVout));
    /*
     * Read in Input Attribute and Output Attribute structures
     */
    for (i=0; i<2; i++) {
	if (i==0)
	    nvio = pVideoInfo->nvin;
	else
	    nvio = pVideoInfo->nvout;
	for (j = 0; j < nvio; j++) {
	    MVEXGeometry *vgp;
	    if (i==0) {
		vgp = &pVideoInfo->vin[j].geometry;
		pVideoInfo->vin[j].vref = u.vgp->referenceId;
		pVideoInfo->vin[j].is_input = True;
	     } else {
		vgp = &pVideoInfo->vout[j].geometry;
		pVideoInfo->vout[j].vref = u.vgp->referenceId;
		pVideoInfo->vout[j].is_input = False;
	    }
	    vgp->frame_rate.numerator = u.vgp->frameRate.numerator;
	    vgp->frame_rate.denominator = u.vgp->frameRate.denominator;
	    vgp->field_rate.numerator = u.vgp->fieldRate.numerator;
	    vgp->field_rate.denominator = u.vgp->fieldRate.denominator;
	    vgp->width = u.vgp->width;
	    vgp->height = u.vgp->height;
	    vgp->concurrent_use = u.vgp->concurrentUse;
	    vgp->priority_steps = u.vgp->prioritySteps;
	    vgp->nplacement = u.vgp->nPlacement;
	    u.vgp = (xVideoGeometry *) (((char *) u.vgp) + sz_xVideoGeometry);
	    if (vgp->nplacement > 0) {
		vgp->placement = (MVEXPlacement *)
			Xmalloc((unsigned)vgp->nplacement*sizeof(MVEXPlacement));
		for (k = 0; k < vgp->nplacement; k++) {
		    /* XXX
		     * On machines where xPlacement == XPlacement,
		     * we could just do a bcopy here.
		     */
		    copyPlacement(&vgp->placement[k], u.pp);
		    u.pp = (xPlacement *) (((char *) u.pp) + sz_xPlacement);
		}
	    } else {
		vgp->placement = (MVEXPlacement *) NULL;
	    }
	}
    }

    /*
     * Read in VideoInputModels
     * Read in VideoOutputModels
     */
    for (i = 0; i < 2; i++) {
	if (i == 0)
	    nvio = pVideoInfo->nvin;
	else
	    nvio = pVideoInfo->nvout;
	for (j = 0; j < nvio; j++) {
	    ioModels = (MVEXRenderModel *) NULL;
	    nModels = 0;
	    size = 0;
	    for (k = 0; k < nallowed_depths; k++) {
		if ((model_mask = *u.iomdl++) == 0)
		    continue;
		size += sizeof(MVEXRenderModel);
		ioModels = (MVEXRenderModel *)
		    (ioModels ? Xrealloc(ioModels, size) : Xmalloc(size));
		ioModels[ nModels ].model_mask = model_mask;
		ioModels[ nModels ].depth      = allowed_depths[k].depth;
		ioModels[ nModels ].visualid   = allowed_depths[k].visualid;
		ioModels[ nModels ].opaque     = allowed_depths[k].opaque;
		ioModels[ nModels ].red_mask   = allowed_depths[k].redMask;
		ioModels[ nModels ].green_mask = allowed_depths[k].greenMask;
		ioModels[ nModels ].blue_mask  = allowed_depths[k].blueMask;
		nModels++;
	    }
	    if (i==0) {
		pVideoInfo->vin[j].model_list = ioModels;
		pVideoInfo->vin[j].n_models = nModels;
	    } else {
		pVideoInfo->vout[j].model_list = ioModels;
		pVideoInfo->vout[j].n_models = nModels;
	    }
	}
    }

    /*
     * Read in lengths for in-ports
     * Read in lengths for out-ports
     */
    for (i = 0; i < 2; i++) {
	if (i == 0)
	    nvio = pVideoInfo->nvin;
	else
	    nvio = pVideoInfo->nvout;
	for (j = 0; j < nvio; j++) {
	    nPorts = *u.nPorts++;
	    ports = nPorts ? (Atom *) Xmalloc(nPorts * sizeof(Atom))
			   : (Atom *) NULL;
	    if (i==0) {
		pVideoInfo->vin[j].ports = ports;
		pVideoInfo->vin[j].n_ports = nPorts;
	    } else {
		pVideoInfo->vout[j].ports = ports;
		pVideoInfo->vout[j].n_ports = nPorts;
	    }
	}
    }

    /*
     * apply the padding
     */
    u.iomdl += ((sz_CARD32 - ((u.charp - pReplyData) & 0x3)) & 0x3);

    /*
     * Read in in-ports
     * Read in out-ports
     */
    for (i = 0; i < 2; i++) {
	if (i == 0)
	    nvio = pVideoInfo->nvin;
	else
	    nvio = pVideoInfo->nvout;
	for (j = 0; j < nvio; j++) {
	    if (i==0) {
		ports = pVideoInfo->vin[j].ports;
		nPorts = pVideoInfo->vin[j].n_ports;
	    } else {
		ports = pVideoInfo->vout[j].ports;
		nPorts = pVideoInfo->vout[j].n_ports;
	    }
	    for (k = 0; k < nPorts; k++)
		ports[ k ] = *u.card32p++;
	}
    }

    /*
     * Read in clip-size Rectangle list
     */
    for (j = 0; j < pVideoInfo->nvin; j++) {
	XRectangle *rectp = &pVideoInfo->vin[j].clip_size;
	rectp->x = u.rectp->x;
	rectp->y = u.rectp->y;
	rectp->width = u.rectp->width;
	rectp->height = u.rectp->height;
	u.rectp = (xRectangle *) (((char *) u.rectp) + sz_xRectangle);
    }

    /*
     * Read in in-abilities
     * Read in out-abilities
     */
#define CopyFraction(from, to)				\
    to.numerator = from.numerator;			\
    to.denominator = from.denominator;

#define CopyFRanges(from, to, count)			\
{							\
    for (k = count-1; k>=0; k--) {			\
	to->num_base        = from->numBase;		\
	to->num_increment   = from->numIncrement;	\
	to->num_limit       = from->numLimit;		\
	to->denom_base      = from->denomBase;		\
	to->denom_increment = from->denomIncrement;	\
	to->denom_limit     = from->denomLimit;		\
	to->num_type        = from->numType;		\
	to->denom_type      = from->denomType;		\
	to++, from++;					\
    }							\
}
	

    for (i = 0; i < 2; i++) {
	if (i == 0)
	    nvio = pVideoInfo->nvin;
	else
	    nvio = pVideoInfo->nvout;
	for (j = 0; j < nvio; j++) {
	    pAbility = ((i == 0) ? &pVideoInfo->vin[j].ability
	    			 : &pVideoInfo->vout[j].ability);
	    /* XXX
	     * On machines where xVideoAbility == MVEXVideoAbility,
	     * we could just do a bcopy here.
	     */
	    CopyFraction(u.vap->normalSaturation, pAbility->normal_saturation);
	    CopyFraction(u.vap->normalContrast,   pAbility->normal_contrast);
	    CopyFraction(u.vap->normalHue,        pAbility->normal_hue);
	    CopyFraction(u.vap->normalBright,     pAbility->normal_bright);
	    k  = (pAbility->n_saturation = u.vap->nSaturation);
	    k += (pAbility->n_contrast   = u.vap->nContrast);
	    k += (pAbility->n_hue        = u.vap->nHue);
	    k += (pAbility->n_bright     = u.vap->nBright);
	    u.vap++;
	    pFR = (MVEXFractionRange *)
		(k ? Xmalloc(sizeof(MVEXFractionRange)*k) : NULL);
	    pAbility->saturation = pFR;
	    CopyFRanges(u.pfr, pFR, pAbility->n_saturation);
	    pAbility->contrast   = pFR;
	    CopyFRanges(u.pfr, pFR, pAbility->n_contrast);
	    pAbility->hue        = pFR;
	    CopyFRanges(u.pfr, pFR, pAbility->n_hue);
	    pAbility->bright     = pFR;
	    CopyFRanges(u.pfr, pFR, pAbility->n_bright);
	}
    }
#undef CopyFraction
#undef CopyFRanges

    /*
     * Initialize count of cached vid and vout ids.
     */
    pVideoInfo->vin_ids.num = pVideoInfo->vout_ids.num = 0;
    pVideoInfo->vin_ids.map = pVideoInfo->vout_ids.map = NULL;

    *vinfo = pVideoInfo;
    Xfree((char *)pReplyData);

    UnlockDisplay (dpy);
    SyncHandle ();

    return 1;
}
#undef CARD32
#undef sz_CARD32

/*
 *    NAME
 *        copyPlacement - internal function for moving data
 *
 *    SYNOPSIS
 */
static
copyPlacement(clientPlacement, protocolPlacement)
    MVEXPlacement *clientPlacement;
    xPlacement *protocolPlacement;
/*
 *    DESCRIPTION
 *        Move the data.
 *
 *    RETURNS
 *        void
 */
{
    clientPlacement->frame_rate.numerator = 
	    protocolPlacement->frameRate.numerator;
    clientPlacement->frame_rate.denominator = 
	    protocolPlacement->frameRate.denominator;
    copyRectangleRange(&clientPlacement->src, &protocolPlacement->src);
    copyRectangleRange(&clientPlacement->dest, &protocolPlacement->dest);
    /* XXX
     * On machines where xFractionRange == XFractionRange,
     * we could just do a bcopy here.
     */
    copyFractionRange(&clientPlacement->x_scale, &protocolPlacement->xScale);
    copyFractionRange(&clientPlacement->y_scale, &protocolPlacement->yScale);
    clientPlacement->identity_aspect = protocolPlacement->identityAspect;
}

/*
 *    NAME
 *        copyPlacement - internal function for moving data
 *
 *    SYNOPSIS
 */
static
copyRectangleRange(clientRectRange, protocolRectRange)
    MVEXRectangleRange *clientRectRange;
    xRectangleRange *protocolRectRange;
/*
 *    DESCRIPTION
 *        Move the data.
 *
 *    RETURNS
 *        void
 */
{
    clientRectRange->base.x = protocolRectRange->base.x;
    clientRectRange->base.y = protocolRectRange->base.y;
    clientRectRange->base.width = protocolRectRange->base.width;
    clientRectRange->base.height = protocolRectRange->base.height;
    clientRectRange->limit.x = protocolRectRange->limit.x;
    clientRectRange->limit.y = protocolRectRange->limit.y;
    clientRectRange->limit.width = protocolRectRange->limit.width;
    clientRectRange->limit.height = protocolRectRange->limit.height;
    clientRectRange->x_inc = protocolRectRange->xInc;
    clientRectRange->y_inc = protocolRectRange->yInc;
    clientRectRange->width_inc = protocolRectRange->widthInc;
    clientRectRange->height_inc = protocolRectRange->heightInc;
    clientRectRange->type = protocolRectRange->type;
}

/*
 *    NAME
 *        copyPlacement - internal function for moving data
 *
 *    SYNOPSIS
 */
static
copyFractionRange(clientFracRange, protocolFracRange)
    MVEXFractionRange *clientFracRange;
    xFractionRange *protocolFracRange;
/*
 *    DESCRIPTION
 *        Move the data.
 *
 *    RETURNS
 *        void
 */
{
    clientFracRange->num_base = protocolFracRange->numBase;
    clientFracRange->num_increment= protocolFracRange->numIncrement;
    clientFracRange->num_limit = protocolFracRange->numLimit;
    clientFracRange->num_type = protocolFracRange->numType;
    clientFracRange->denom_base = protocolFracRange->denomBase;
    clientFracRange->denom_increment= protocolFracRange->denomIncrement;
    clientFracRange->denom_limit = protocolFracRange->denomLimit;
    clientFracRange->denom_type = protocolFracRange->denomType;
}
