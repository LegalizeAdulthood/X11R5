/* $Header: XpexNameSets.c,v 2.2 91/09/11 16:07:18 sinyaw Exp $ */
/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

#include "Xpexlibint.h"
#include "Xpexlib.h"
#include <X11/extensions/Xext.h>
#include "extutil.h"
#include "Xpextutil.h"

XpexNameSet
XpexCreateNameSet(dpy)
	Display *dpy;
{
	register pexCreateNameSetReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
    pexNameSet ns_id = XAllocID(dpy);

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetResReq(CreateNameSet, dpy, i, ns_id, req);
	UnlockDisplay(dpy);
	SyncHandle();

    return ns_id;
}

void
XpexCopyNameSet(dpy, src_ns_id, dest_ns_id)
	Display *dpy;
	XpexNameSet src_ns_id;
	XpexNameSet dest_ns_id;
{
	register pexCopyNameSetReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(CopyNameSet, dpy, i, req);
	req->src = src_ns_id;
	req->dst = dest_ns_id;
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexFreeNameSet(dpy, ns_id)
	Display *dpy;
	XpexNameSet ns_id;
{
	register pexFreeNameSetReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetResReq(FreeNameSet, dpy, i, ns_id, req);
	UnlockDisplay(dpy);
	SyncHandle();
}

Status
XpexGetNameSet(dpy, ns_id, names, count)
	Display *dpy;
	XpexNameSet  ns_id;
	XpexName  **names;  /* RETURN */
	int  *count;  /* RETURN */
{
	register pexGetNameSetReq *req;
	pexGetNameSetReply reply;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	unsigned size;
	Status status = 1;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetResReq(GetNameSet, dpy, i, ns_id, req);
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*count = (int) reply.numNames;
	if ((size == reply.length * sizeof(CARD32)) > 0) {
		if (*names = (pexName *)Xmalloc(size)) {
			_XRead(dpy, (char *)*names, (long)size);
		} else {
			status = 0;
			_XpexClearReply(dpy, reply.length);
		}
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return status;
}

void
XpexChangeNameSet(dpy, ns_id, action, names, count)
	Display  *dpy;
	XpexNameSet  ns_id;
	int  action; 
	XpexName  *names;
	int  count;
{
	register pexChangeNameSetReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	int extra = count * sizeof(pexName);

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReqExtra(ChangeNameSet, dpy, i, extra, req);
	bcopy((char *)(names), (char *)(req + 1), extra);
	UnlockDisplay(dpy);
	SyncHandle();
}
