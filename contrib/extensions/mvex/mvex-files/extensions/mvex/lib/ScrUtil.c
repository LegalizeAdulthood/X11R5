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
static char *rcsid=  "$Header: /usr/local/src/x11r5.mvex/RCS/ScrUtil.c,v 2.1 1991/08/13 19:46:19 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#include "Xlibint.h"
#include "MVEXlibint.h"
#include "Xutil.h"

/*
 *    NAME
 *        _MVEXGetVideoInfo - return the video elements for the screen
 *
 *    SYNOPSIS
 */
VideoInfo *
_MVEXGetVideoInfo(dpy, screen_number, codes)
    Display *dpy;		/* in: display pointer */
    int screen_number;		/* in: screen */
    XExtCodes	*codes;		/* in: extensions codes */
/*
 *    DESCRIPTION
 *        Find the screen with the video on it (assume only one per display).
 *
 *    RETURNS
 *        The pointer to the video element linked list.
 */
{
    XEDataObject object;
    XExtData **pScreenXExtData;
    XExtData *videoXExtData;
    int last_screen;

    if (codes == NULL) {
	if (!(codes = _MVEXCheckExtension (dpy))) {
	    return (NULL);
	}
    }

    /*
     * If we are not given a screen number (because one is not available,
     * and could only be picked from thin air), then we must search all
     * the screens looking for information.
     */
    if (screen_number < 0) {
	last_screen = dpy->nscreens - 1;
	screen_number = 0;
    } else
	last_screen = screen_number;

    while (screen_number <= last_screen) {
	object.screen = ScreenOfDisplay(dpy, screen_number);
	pScreenXExtData = XEHeadOfExtensionList(object);
	videoXExtData = XFindOnExtensionList(pScreenXExtData, codes->extension);
	if (videoXExtData->private_data) {
	    return (VideoInfo *)videoXExtData->private_data;
	}

	screen_number++;
    }

    return (VideoInfo *) NULL;
}

/*
 *    NAME
 *        GetExtVisualInfo - return MVEX visual info per a mask and template
 *
 *    SYNOPSIS
 */
static XVisualInfo *
GetExtVisualInfo( dpy, mask, template, nitems)
    Display *dpy;			/* in: display pointer */
    register long mask; 		/* in: item mask */
    register XVisualInfo *template;	/* in: visual template */
    int *nitems;			/* out: length of returned array */
/*
 *    DESCRIPTION
 *	The choices for mask are:
 *
 *		VisualNoMask
 *		VisualIDMask
 *		VisualDepthMask
 *		VisualClassMask
 *		VisualRedMaskMask
 *		VisualGreenMaskMask
 *		VisualBlueMaskMask
 *		VisualColormapSizeMask
 *		VisualBitsPerRGBMask
 *		VisualAllMask
 *		VisualScreenMask
 *
 *    RETURNS
 *	This procedure returns a list of visual information structures
 *	that match the specified attributes given in the visual information 
 *	template, and that are unique to MVEX.
 *
 *	If no visuals exist that match the specified attributes, a NULL is
 *	returned.
 */
{
    Visual *vp;
    Depth *dp;
    VideoInfo *vinfo;
    int total,count;
    XVisualInfo *vip,*vip_base;
    int min_screen, max_screen;
    int i;
    XExtCodes	*codes;

    if (!(codes = _MVEXCheckExtension (dpy))) {
	*nitems = 0;
	return (XVisualInfo *) NULL;
    }


    /* NOTE: NO HIGH PERFORMING CODE TO BE FOUND HERE */

    LockDisplay(dpy);

    /* ALLOCATE THE ORIGINAL BUFFER; REALLOCED LATER OF OVERFLOW OCCURS;
     FREED AT END IF NO VISUALS ARE FOUND */

    count = 0;
    total = 10;
    vip_base = vip = (XVisualInfo *)Xmalloc(sizeof(XVisualInfo)*total);

    if (mask & VisualScreenMask) {
	min_screen = template->screen;
	max_screen = min_screen + 1;
    } else {
	min_screen = 0;
	max_screen = ScreenCount(dpy);
    }

    /* LOOP THROUGH SCREENS */

    for (i = min_screen; i < max_screen; i++) {

	if ((vinfo = _MVEXGetVideoInfo(dpy, i, codes)) == NULL)
	  break;

	/* LOOP THROUGH DEPTHS */

	for (dp=vinfo->depths; dp < (vinfo->depths + vinfo->ndepths); dp++)
	{
	    if ((mask & VisualDepthMask) &&
	      (dp->depth != template->depth)) continue;

	  /* LOOP THROUGH VISUALS */

	    if (dp->visuals) {
		for (vp=dp->visuals; vp<(dp->visuals + dp->nvisuals); vp++) {
		    if ((mask & VisualIDMask) &&
		        (vp->visualid != template->visualid)) continue;
		    if ((mask & VisualClassMask) &&
		        (vp->class != template->class)) continue;
		    if ((mask & VisualRedMaskMask) &&
			(vp->red_mask != template->red_mask)) continue;
		    if ((mask & VisualGreenMaskMask) &&
			(vp->green_mask != template->green_mask)) continue;
		    if ((mask & VisualBlueMaskMask) &&
			(vp->blue_mask != template->blue_mask)) continue;
		    if ((mask & VisualColormapSizeMask) &&
			(vp->map_entries != template->colormap_size)) continue;
		    if ((mask & VisualBitsPerRGBMask) &&
			(vp->bits_per_rgb != template->bits_per_rgb)) continue;

		  /* YEA!!! WE FOUND A GOOD ONE */

		    if (count+1 > total)
		    {
		        total += 10;
		        vip_base = (XVisualInfo *)
				Xrealloc(vip_base, sizeof(XVisualInfo)*total);
		        vip = &vip_base[count];
		    }

		    count++;

		    vip->visual = vp;
		    vip->visualid = vp->visualid;
		    vip->screen = i;
		    vip->depth = dp->depth;
		    vip->class = vp->class;
		    vip->red_mask = vp->red_mask;
		    vip->green_mask = vp->green_mask;
		    vip->blue_mask = vp->blue_mask;
		    vip->colormap_size = vp->map_entries;
		    vip->bits_per_rgb = vp->bits_per_rgb;
  
		    vip++;

		} /* END OF LOOP ON VISUALS */
	    } /* END OF IF THERE ARE ANY VISUALS AT THIS DEPTH */
	} /* END OF LOOP ON DEPTHS */
    }

    UnlockDisplay(dpy);

    if (count)
    {
	*nitems = count;
	return vip_base;
    }


    XFree(vip_base);

    *nitems = 0;

    return NULL;
}

/*
 *    NAME
 *        MVEXGetVisualInfo - return visual info per a mask and template
 *
 *    SYNOPSIS
 */
XVisualInfo *
MVEXGetVisualInfo(dpy, mask, template, nitems, include_core_visuals)
    Display *dpy;		/* in: display pointer */
    long mask; 			/* in: item mask */
    XVisualInfo *template;	/* in: visual template */
    int *nitems;		/* out: length of returned array */
    Bool include_core_visuals;	/* in: True for core & MVEX, False = only MVEX */
/*
 *    DESCRIPTION
 *        Get core X and MVEX unique visual info, combine if necessary.
 *
 *    RETURNS
 *	This procedure returns a list of visual information structures
 *	that match the specified attributes given in the visual information 
 *	template, and that are either unique to MVEX, to core X or both.
 *
 *	If no visuals exist that match the specified attributes, a NULL is
 *	returned.
 */
{
    XVisualInfo *vip;
    XVisualInfo *pvisCore, *pvisMVEX;
    int nvisCore, nvisMVEX;

    LockDisplay(dpy);

    /*
     * Check both lists if necessary.
     */
    pvisMVEX = GetExtVisualInfo( dpy, mask, template, &nvisMVEX);
    if (include_core_visuals)
	pvisCore = XGetVisualInfo( dpy, mask, 
		template, &nvisCore);
    if (!include_core_visuals || !pvisCore) {
	*nitems = nvisMVEX;
	return pvisMVEX;
    } else if (!pvisMVEX){
	*nitems = nvisCore;
	return pvisCore;
    }

    /*
     * If we get here, we have visuals returned from both the Core
     * list and the MVEX list.  Merge them together and return the
     * merged list
     */
    *nitems = nvisCore + nvisMVEX;
    vip = (XVisualInfo *)Xmalloc(sizeof(XVisualInfo) * *nitems);
    bcopy(pvisCore, vip, sizeof(XVisualInfo) * nvisCore);
    bcopy(pvisMVEX, vip + nvisCore, sizeof(XVisualInfo) * nvisMVEX);

    XFree(pvisCore);
    XFree(pvisMVEX);

    UnlockDisplay(dpy);

    return vip;
}

/*
 *    NAME
 *        MVEXMinorVersion - return the MVEX major version
 *
 *    SYNOPSIS
 */
int
MVEXMinorVersion(dpy)
    Display *dpy;		/* in: display pointer */
/*
 *    DESCRIPTION
 *        Get the value out of a structure.
 *
 *    RETURNS
 *        0 if MVEX is not installed, not 0 if it is.
 */
{
    VideoInfo *vinfo;

    if ((vinfo = _MVEXGetVideoInfo(dpy, -1, (XExtCodes *) NULL)) == NULL)
	return -1;

    return(vinfo->minor_version);
}

/*
 *    NAME
 *        MVEXMajorVersion - return the MVEX major version
 *
 *    SYNOPSIS
 */
int
MVEXMajorVersion(dpy)
    Display *dpy;		/* in: display pointer */
/*
 *    DESCRIPTION
 *        Get the value out of a structure.
 *
 *    RETURNS
 *        0 if MVEX is not installed, not 0 if it is.
 */
{
    VideoInfo *vinfo;

    if ((vinfo = _MVEXGetVideoInfo(dpy, -1, (XExtCodes *) NULL)) == NULL)
	return -1;

    return(vinfo->major_version);
}

/*
 *    NAME
 *        MVEXBaseOpcode - return the MVEX base opcode
 *
 *    SYNOPSIS
 */
int
MVEXBaseOpcode(dpy)
    Display *dpy;		/* in: display pointer */
/*
 *    DESCRIPTION
 *        Get the value out of a structure.
 *
 *    RETURNS
 *        0 if MVEX is not installed, not 0 if it is.
 */
{
    XExtCodes	*codes;

    codes = _MVEXCheckExtension (dpy);
    if (!codes)
	return (0);

    return(codes->major_opcode);
}

/*
 *    NAME
 *        MVEXBaseError - return the MVEX base error
 *
 *    SYNOPSIS
 */
int
MVEXBaseError(dpy)
    Display *dpy;		/* in: display pointer */
/*
 *    DESCRIPTION
 *        Get the value out of a structure.
 *
 *    RETURNS
 *        0 if MVEX is not installed, not 0 if it is.
 */
{
    XExtCodes	*codes;

    codes = _MVEXCheckExtension (dpy);
    if (!codes)
	return (0);

    return(codes->first_error);
}

/*
 *    NAME
 *        MVEXBaseEvent - return the MVEX base event
 *
 *    SYNOPSIS
 */
int
MVEXBaseEvent(dpy)
    Display *dpy;		/* in: display pointer */
/*
 *    DESCRIPTION
 *        Get the value out of a structure.
 *
 *    RETURNS
 *        0 if MVEX is not installed, not 0 if it is.
 */
{
    XExtCodes	*codes;

    codes = _MVEXCheckExtension (dpy);
    if (!codes)
	return (0);

    return(codes->first_event);
}

#define VINS	1
#define VOUTS	2
#define NextAddr(type, ptr, length_previous) \
    ((type *) (((char *)ptr) + length_previous))

/*
 *    NAME
 *        GetVidInfo - common function for MVEXGetVins, etc.
 *
 *    SYNOPSIS
 */
static char *
GetVidInfo(dpy, screen_number, nitems_return, type)
    Display *dpy;		/* in: display pointer */
    int screen_number;		/* in: screen */
    int *nitems_return;		/* out: length of returned array */
    int type;			/* in: VINS, VOUTS */
/*
 *    DESCRIPTION
 *        Allocate sufficient memory, copy stored info into it.
 *
 *    RETURNS
 *        A pointer to the resulting structure(s).
 */
{
    VideoInfo *vinfo;
    char *pdata;
    int size;
    MVEXVout *pVoutSrc, *pVoutDst;
    char *pDataReturn;
    int i,j;
    long nPlace, nModels;
    long descLen;
    char *pDescrDst;
    MVEXPlacement *pPlaceSrc, *pPlaceDst;
    MVEXRenderModel *pRenderModelSrc, *pRenderModelDst;
    int nitems;
    int nbytesTotal, nbytesDescr, nbytesPlaecment,
	nbytesRenderModel;

    *nitems_return = 0;

    if ((vinfo = _MVEXGetVideoInfo(dpy, screen_number, NULL)) == NULL)
      return(NULL);

    switch (type) {
    case VINS:
	nitems = vinfo->nvin;
	pdata = (char *)vinfo->vin;
	size = sizeof(MVEXVin);
	break;
    case VOUTS:
	nitems = vinfo->nvout;
	pdata = (char *)vinfo->vout;
	size = sizeof(MVEXVout);
	break;
    }
    if (nitems == 0)
	return (NULL);
    /*
     * These data structures are nestable.  Start by predending that we have
     * a Video Output.
     *
     * First figure the total space needed.  We will do a single malloc
     * and manage space within the large block ourselves.  The client
     * will be able to free this structure with a call to XFree.
     * We are assuming no alignment problems!
     */
    pVoutSrc = (MVEXVout *)pdata;
    nbytesPlaecment = nbytesRenderModel = 0;
    nbytesDescr = size * nitems;
    for (i=0; i < nitems; i++, pVoutSrc = NextAddr(MVEXVout, pVoutSrc, size)) {
	nPlace = pVoutSrc->geometry.nplacement;
	nbytesPlaecment += nPlace * sizeof(MVEXPlacement);
	nModels = pVoutSrc->n_models;
	nbytesRenderModel += nModels * sizeof(MVEXRenderModel);
    }
    /* XXX
     * If you have alignment problems, you could adjust the nbytes*
     * variables here to force any alignment you desire between the
     * various sections of our pDataReturn memory
     */
    nbytesTotal = nbytesDescr + nbytesPlaecment + nbytesRenderModel;
    pDataReturn = (char *)Xmalloc (nbytesTotal);
    if (!pDataReturn)
	return (char *) NULL;
    pVoutDst        = NextAddr(MVEXVout,        pDataReturn,  0);
    pPlaceDst       = NextAddr(MVEXPlacement,   pDataReturn,  nbytesDescr);
    pRenderModelDst = NextAddr(MVEXRenderModel, pPlaceDst,    nbytesPlaecment);

    /*
     * Copy MVEXV{in/out} array first.
     */
    pVoutSrc = (MVEXVout *)pdata;
    bcopy(pdata, pDataReturn, size * nitems);

    for (i=0; i < nitems; i++) {
	/*
	 * pretend we have
	 * a MVEXVout struct and copy MVEXPlacements and MVEXRenderModel lists
	 * (models and pixmaps).
	 */
	nPlace = pVoutSrc->geometry.nplacement;
	if (nPlace) {
	    pPlaceSrc = pVoutSrc->geometry.placement;
	    pVoutDst->geometry.placement = pPlaceDst;
	    bcopy(pPlaceSrc, pPlaceDst, nPlace * sizeof(MVEXPlacement));
	    pPlaceDst += nPlace;
	}
	nModels = pVoutSrc->n_models;
	if (nModels) {
	    pRenderModelSrc = pVoutSrc->model_list;
	    pVoutDst->model_list = pRenderModelDst;
	    bcopy(pRenderModelSrc,
		  pRenderModelDst, 
		  nModels * sizeof(MVEXRenderModel));
	    pRenderModelDst += nModels;
	}

	pVoutSrc = NextAddr(MVEXVout, pVoutSrc, size);
	pVoutDst = NextAddr(MVEXVout, pVoutDst, size);
    }
    *nitems_return = nitems;
    return pDataReturn;
}


/*
 *    NAME
 *        MVEXGetVins - return a list of VideoIns
 *
 *    SYNOPSIS
 */
MVEXVin *
MVEXGetVins( dpy, screen_number, nitems)
    Display *dpy;		/* in: display pointer */
    int screen_number;		/* in: screen */
    int *nitems;		/* out: length of returned array */
/*
 *    DESCRIPTION
 *        Use the common routine to get the list
 *
 *    RETURNS
 *        An array of Vins.
 */
{
    return (MVEXVin *) GetVidInfo(dpy, screen_number, nitems, VINS);
}

/*
 *    NAME
 *        MVEXGetVouts - return a list of VideoOuts
 *
 *    SYNOPSIS
 */
MVEXVout *
MVEXGetVouts( dpy, screen_number, nitems)
    Display *dpy;		/* in: display pointer */
    int screen_number;		/* in: screen */
    int *nitems;		/* out: length of returned array */
/*
 *    DESCRIPTION
 *        Use the common routine to get the list.
 *
 *    RETURNS
 *        An array of Vouts.
 */
{
    return (MVEXVout *) GetVidInfo(dpy, screen_number, nitems, VOUTS);
}

#undef VINS
#undef VOUTS
#undef NextAddr

/*
 *    NAME
 *        MVEXOverlapsOfScreen - return screenoverlap booleans
 *
 *    SYNOPSIS
 */
Status
MVEXOverlapsOfScreen( dpy, screen_number, input_overlap, capture_overlap,
    io_overlap)
    Display *dpy;		/* in: display pointer */
    int screen_number;		/* in: screen */
    Bool *input_overlap;	/* out: can VIRs overlap? */
    Bool *capture_overlap;	/* out: can VORs overlap? */
    Bool *io_overlap;		/* out: can VIRs overlap VORs? */
/*
 *    DESCRIPTION
 *        Fetch the data out of the screens video info.
 *
 *    RETURNS
 *        0 if failure, not 0 if success
 */
{
    VideoInfo *vinfo;

    if ((vinfo = _MVEXGetVideoInfo(dpy, screen_number, NULL)) == NULL)
	return (0);

    *input_overlap = vinfo->input_overlap;
    *capture_overlap = vinfo->capture_overlap;
    *io_overlap = vinfo->io_overlap;
    return (1);
}
