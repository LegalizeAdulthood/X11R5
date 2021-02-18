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
static char *rcsid=  "$Header: /usr/local/src/x11r5.mvex/RCS/MVEXCrVid.c,v 2.1 1991/08/13 19:46:19 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#include "Xlibint.h"
#include "MVEXlibint.h"
#include "Xutil.h"
#include "MVEXlib.h"
#include "MVEXproto.h"

/*
 *    NAME
 *        MVEXAddVideoID - add a video id to the internal list
 *
 *    SYNOPSIS
 */
Status
MVEXAddVideoID(dpy, id, reference, type)
    Display *dpy;		/* in: display pointer */
    VideoIO id;			/* in: new id */
    VideoReference reference;	/* in: Vin or Vout vref */
    int type;			/* in: Input or Output */
/*
 *    DESCRIPTION
 *        Find the Vin or Vout, add the id to the right list.
 *
 *    RETURNS
 *        0 if it fails, 1 if it succeeds.
 */
{
    VideoInfo *vinfo;
    VidMapPtr map;
    MVEXVin *vin;
    MVEXVout *vout;
    int i, count, oldCount;

    if ((vinfo = _MVEXGetVideoInfo(dpy, -1, NULL)) == NULL)
      return(0);

    switch (type) {
    case Input:
	for (vin = vinfo->vin, i = vinfo->nvin; --i >= 0; vin++)
	    if (vin->vref == reference)
		break;
	if (i < 0)
	    return(0);
	oldCount = vinfo->vin_ids.num;
	count = oldCount + 1;
	map = (VidMapPtr) Xmalloc(count * sizeof(VidMap));
	if (map == NULL)
	    return (0);
	if (oldCount)
	    bcopy(vinfo->vin_ids.map, map, oldCount * sizeof(VidMap));
	XFree(vinfo->vin_ids.map);
	vinfo->vin_ids.map = map;
	vinfo->vin_ids.num = count;
	map[ count - 1 ].id = id;
	map[ count - 1 ].u.vin = vin;
	break;

    case Output:
	for (vout = vinfo->vout, i = vinfo->nvout; --i >= 0; vout++)
	    if (vout->vref == reference)
		break;
	if (i < 0)
	    return(0);
	oldCount = vinfo->vout_ids.num;
	count = oldCount + 1;
	map = (VidMapPtr) Xmalloc(count * sizeof(VidMap));
	if (map == NULL)
	    return (0);
	if (oldCount)
	    bcopy(vinfo->vout_ids.map, map, oldCount * sizeof(VidMap));
	XFree(vinfo->vout_ids.map);
	vinfo->vout_ids.map = map;
	vinfo->vout_ids.num = count;
	map[ count - 1 ].id = id;
	map[ count - 1 ].u.vout = vout;
	break;

    default:
	return (0);
    }

    return (1);
}

/*
 *    NAME
 *        MVEXCreateVideo - make a video id for a Vin or Vout
 *
 *    SYNOPSIS
 */
VideoIO
MVEXCreateVideo(dpy, reference)
    Display *dpy;		/* in: display pointer */
    VideoReference reference;	/* in: Vin or Vout vref */
/*
 *    DESCRIPTION
 *        Get a new id, format and send the request, add it to the internal
 *        list.
 *
 *    RETURNS
 *        A new video id.
 */
{
    register xCreateVideoReq	*req;
    XExtCodes			*codes;
    VideoIO			id;

    if (!(codes = _MVEXCheckExtension (dpy)))
	return None;
    LockDisplay (dpy);
    GetReq (CreateVideo, req);
    req->reqType = codes->major_opcode;
    req->videoReqType = X_CreateVideo;

    req->id = id = XAllocID(dpy);
    req->reference = reference;

    UnlockDisplay (dpy);
    SyncHandle ();

    /*
     * Maybe we should require the type (input or output) to
     * be passed in.
     */
    if (! MVEXAddVideoID(dpy, id, reference, Input))
	(void) MVEXAddVideoID(dpy, id, reference, Output);

    return (id);
}

