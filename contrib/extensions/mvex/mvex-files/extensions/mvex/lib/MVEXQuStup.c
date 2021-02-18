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
static char *rcsid=  "$Header: /n/lacey/usr/local/src/video/MVEX/lib/RCS/MVEXQuStup.c,v 1.2 1991/09/26 04:56:34 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#define NEED_REPLIES
#include "Xlibint.h"
#include "MVEXlibint.h"
#include "Xutil.h"

/*
 *    NAME
 *        MVEXQuerySetup - ask about a particular vio/port pair
 *
 *    SYNOPSIS
 */
Status
MVEXQuerySetup(dpy, setup)
    Display *dpy;		/* in: display pointer */
    MVEXSetupState *setup;	/* in: vio, port */
				/* out: {frames,timecodes}_available */
				/*      and frame_accurate. */
/*
 *    DESCRIPTION
 *        Ask the server about the capabilities of the specified setup
 *
 *    RETURNS
 *        0 for failure, 1 for success.
 */
{
    register xQuerySetupReq *req;
    xQuerySetupReply rep;
    XExtCodes	*codes;

    if (!(codes = _MVEXCheckExtension (dpy)))
	return 0;

    LockDisplay (dpy);
    GetReq (QuerySetup, req);
    req->reqType = codes->major_opcode;
    req->videoReqType = X_QuerySetup;
    req->id = setup->vio;
    req->atom = setup->port;

    if (!_XReply (dpy, (xReply *) &rep, 0, xFalse)) {
	UnlockDisplay (dpy);
	SyncHandle ();
	return 0;
    }

    UnlockDisplay (dpy);
    SyncHandle ();

    setup->frames_available = rep.framesAvailable;
    setup->timecodes_available = rep.timecodesAvailable;
    setup->frame_accurate = rep.frameAccurate;

    return 1;
}
