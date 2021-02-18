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
static char *rcsid=  "$Header: /usr/local/src/x11r5.mvex/RCS/MVEXSelEvt.c,v 2.0 1991/07/23 02:05:35 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#define NEED_EVENTS
#include "Xlibint.h"
#include "MVEXlibint.h"
#include "Xutil.h"
#include "MVEXlib.h"
#include "MVEXproto.h"

/*
 *    NAME
 *        MVEXSelectEvents - select events for VEX
 *
 *    SYNOPSIS
 */
MVEXSelectEvents(dpy, id, mask)
    Display *dpy;		/* in: display pointer */
    XID id;			/* in: video id or drawable */
    unsigned long mask;		/* in: bit mask */
/*
 *    DESCRIPTION
 *        Format and send the request.
 *
 *    RETURNS
 *        void
 */
{
    register xSelectVideoEventsReq	*req;
    XExtCodes			*codes;

    if (!(codes = _MVEXCheckExtension (dpy)))
	return;
    LockDisplay (dpy);
    GetReq (SelectVideoEvents, req);
    req->reqType = codes->major_opcode;
    req->videoReqType = X_SelectVideoEvents;

    req->id = id;
    req->mask = mask;

    UnlockDisplay (dpy);
    SyncHandle ();
}
