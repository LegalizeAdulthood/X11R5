/* $Header: XpexSC.c,v 2.4 91/09/11 16:07:27 sinyaw Exp $ */
/*
 * 	XpexSC.c
 */

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

#include <stdio.h>
#include "Xpexlibint.h"
#include "Xpexlib.h"
#include <X11/extensions/Xext.h>
#include "extutil.h"
#include "Xpextutil.h"

static int sizeof_sc_values();
static void pack_sc_values();
static void unpack_sc_values();

XpexSC
XpexCreateSC(dpy, valuemask, values)
	Display *dpy;
	XpexBitmask valuemask;
	XpexSCValues *values;
{
	int size = 0;
	pexSC sc_id;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	{
		int bytes_packed;
		register	pexCreateSearchContextReq	*req;

		if (valuemask) {
			size = sizeof_sc_values(valuemask, values);
		}
		XpexGetReqExtra(CreateSearchContext, dpy, i, size, req);
		req->fpFormat = XpexGetFloatingPointFormat(dpy);
		sc_id = req->sc = XAllocID(dpy);
		req->itemMask = valuemask;
		if (size) {
			pack_sc_values(valuemask, values, 
			(char *)(req + 1), &bytes_packed);
		}
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return sc_id;
}

void
XpexCopySC(dpy, src, dest, valuemask)
	Display *dpy;
	XpexSC src, dest;
	XpexBitmask valuemask;
{
	register pexCopySearchContextReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(CopySearchContext, dpy, i, req);
	req->src = src;
	req->dst = dest;
	req->itemMask = valuemask;
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexFreeSC(dpy, sc)
	Display  *dpy;
	XpexSC  sc;
{
	register	pexFreeSearchContextReq		*req;
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetResReq(FreeSearchContext, dpy, i, sc, req);
	UnlockDisplay(dpy);
	SyncHandle();
}

Status
XpexGetSC(dpy, sc, valueMask, values)
    Display *dpy;
    XpexSC sc;
    XpexBitmask valueMask; 
    XpexSCValues *values; /* RETURN */
{
	register 	pexGetSearchContextReq		*req;
				pexGetSearchContextReply	reply;
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);

	CARD32 *list;
	Status status = 1;
	unsigned size;
	int bytes_unpacked;

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReq(GetSearchContext, dpy, i, req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->sc = sc;
	req->itemMask = (pexBitmask) valueMask;;
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (list = (CARD32 *) Xmalloc(size)) {
			_XRead(dpy, (CARD32 *)list, (long)size);
			unpack_sc_values(list, valueMask, 
			values, &bytes_unpacked);
			XFree(list);
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
XpexChangeSC(dpy, sc_id, valuemask, values)
	Display  *dpy;
	XpexSC  sc_id;
	XpexBitmask valuemask; 
	XpexSCValues  *values;
{
	char *list;
	register pexChangeSearchContextReq *req;
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	int size = sizeof_sc_values(valuemask, values); 
	int bytes_packed;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);

	XpexGetReqExtra(ChangeSearchContext, dpy, i, size, req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->sc = sc_id;
	req->itemMask = valuemask;
	list = (char *) (req + 1);
	pack_sc_values(valuemask, values, list, &bytes_packed);

	UnlockDisplay(dpy);
	SyncHandle();
}

Status
XpexSearchNetwork(dpy, sc_id, elem_refs, count)
	Display  *dpy;
	XpexSC  sc_id;
	XpexElementRef  **elem_refs; /* RETURN */
	int  *count; /* RETURN */
{
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexSearchNetworkReq *req;
	pexSearchNetworkReply reply;
	unsigned size;
	Status status = 1;

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetResReq(SearchNetwork, dpy, i, sc_id, req);
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	if (*elem_refs = (XpexElementRef *)Xmalloc(size)) {
		_XRead(dpy, (char *)*elem_refs, (long)size);
	} else {
		status = 0;
		_XpexClearReply(dpy, reply.length);
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return status;
}

static int
sizeof_sc_values(valueMask, values)
	XpexBitmask valueMask;
	XpexSCValues *values;
{
	int length = 0;

	valueMask &= XpexSC_All;

	if (valueMask & XpexSC_Position) {
		length += sizeof(pexCoord3D);			
	}
	if (valueMask & XpexSC_Distance) {
		length += sizeof(XpexFloat);
	}
	if (valueMask & XpexSC_Ceiling) {
		length += sizeof(CARD32);
	}
	if (valueMask & XpexSC_ModelClipFlag) {
		length += sizeof(CARD32);
	}
	if (valueMask & XpexSC_StartPath) {
		length += sizeof(CARD32) + 
		values->start_path.num_refs * sizeof(pexElementRef);
	}
	if (valueMask & XpexSC_NormalList) {
		length += sizeof(CARD32) +
		values->normal_list.num_pairs * sizeof(pexNameSetPair);
	}
	if (valueMask & XpexSC_InvertedList) {
		length += sizeof(CARD32) +
		values->inverted_list.num_pairs * sizeof(pexNameSetPair);
	}
	return length;
}

static void
pack_sc_values(valueMask, values, list, length)
	XpexBitmask valueMask;
	XpexSCValues *values;
	char *list; /* RETURN */
	int *length; /* RETURN */
{
	*length = 0;

	valueMask &= XpexSC_All; /* important bits only */

	if (valueMask & XpexSC_Position) {
		pexCoord3D *dest = (pexCoord3D *) list;
		dest->x = (XpexFloat) values->search_pos.x;
		dest->y = (XpexFloat) values->search_pos.y;
		dest->z = (XpexFloat) values->search_pos.z;
		list += sizeof(pexCoord3D);
		*length += sizeof(pexCoord3D);			
	}
	if (valueMask & XpexSC_Distance) {
		XpexFloat *dest = (XpexFloat *) list;
		*dest = (XpexFloat) values->search_dist;
		list += sizeof(XpexFloat);
		*length += sizeof(XpexFloat);
	}
	if (valueMask & XpexSC_Ceiling) {
		CARD16 *dest = (CARD16 *) list;
		*dest = (CARD16) values->search_ceiling;
		list += sizeof(CARD32);
		*length += sizeof(CARD32);
	}
	if (valueMask & XpexSC_ModelClipFlag) {
		CARD8 *dest = (CARD8 *) list;
		*dest = (CARD8) values->model_clip_flag;
		list += sizeof(CARD32);
		*length += sizeof(CARD32);
	}
	if (valueMask & XpexSC_StartPath) {
		int size = 
		values->start_path.num_refs * sizeof(pexElementRef);
		CARD32 *count = (CARD32 *) list;
		*count = (CARD32) values->start_path.num_refs;
		list += sizeof(CARD32); /* move pointer */
		if (size) {
			bcopy((char *)(values->start_path.elem_refs),
			(char *)list, size);
		}
		list += size;
		*length += sizeof(CARD32) + size;
	}
	if (valueMask & XpexSC_NormalList) {
		int size =
		values->normal_list.num_pairs * sizeof(pexNameSetPair);
		CARD32 *count = (CARD32 *) list;
		*count = (CARD32) values->normal_list.num_pairs;
		list += sizeof(CARD32); /* move pointer */
		if (size) {
			bcopy((char *)values->normal_list.pairs,
			(char *)list, size);
			list += size; /* move pointer */
		}
		*length += sizeof(CARD32) + size;
	}
	if (valueMask & XpexSC_InvertedList) {
		int size = 
			values->inverted_list.num_pairs * 
			sizeof(pexNameSetPair);
		CARD32 *count = (CARD32 *) list;
		*count = (CARD32) values->inverted_list.num_pairs;
		list += sizeof(CARD32); /* move pointer */
		if (size) {
			bcopy((char *)values->inverted_list.pairs,
			(char *)list, size);
			list += size; /* move pointer */
		}
		*length += sizeof(CARD32) + size;
	}
}

static void
unpack_sc_values(list, valueMask, values, length)
	CARD32 *list;
	XpexBitmask valueMask;
	XpexSCValues *values; /* RETURN */
	int *length; /* RETURN */
{
	*length = 0;

	if (valueMask & XpexSC_Position) {
		pexCoord3D *s = (pexCoord3D *) list;

		values->search_pos.x = s->x;
		values->search_pos.y = s->y;
		values->search_pos.z = s->z; 
		list = (CARD32 *) ++s;
		*length += sizeof(pexCoord3D);			
	}
	if (valueMask & XpexSC_Distance) {
		values->search_dist = *((XpexFloat *)list); list++;
		*length += sizeof(XpexFloat);
	}
	if (valueMask & XpexSC_Ceiling) {
		CARD16 *s = (CARD16 *) list;
		values->search_ceiling = *s;
		list++;
		*length += sizeof(CARD32);
	}
	if (valueMask & XpexSC_ModelClipFlag) {
		CARD8 *s = (CARD8 *) list;
		values->model_clip_flag = *s;
		list++;
		*length += sizeof(CARD32);
	}
	if (valueMask & XpexSC_StartPath) {
		int size, count; 

		count = values->start_path.num_refs = *((CARD32 *) list);
		list += sizeof(CARD32); /* move pointer */

		size = count * sizeof(pexElementRef);

		if (size) {
			bcopy((char *)list, 
			(char *)(values->start_path.elem_refs), size);

			list += size; /* move pointer */
		}
		*length += sizeof(CARD32) + size;
	}
	if (valueMask & XpexSC_NormalList) {
		int size, count;

		count = values->normal_list.num_pairs = *((CARD32 *) list);
		list += sizeof(CARD32); /* move pointer */

		size = count * sizeof(pexNameSetPair);

		if (size) {
			bcopy((char *)list,
			(char *)(values->normal_list.pairs), size);
			list += size; /* move pointer */
		}
		*length += sizeof(CARD32) + size;
	}
	if (valueMask & XpexSC_InvertedList) {
		int size, count;

		count = values->inverted_list.num_pairs = *((CARD32 *) list);
		list += sizeof(CARD32); /* move pointer */

		size = count * sizeof(pexNameSetPair);

		if (size) {
			bcopy((char *)list,
			(char *)(values->inverted_list.pairs), size);
			list += size; /* move pointer */
		}
		*length += sizeof(CARD32) + size;
	}
}
