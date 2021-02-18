/* $Header: XpexPick.c,v 2.4 91/09/11 15:51:02 sinyaw Exp $ */
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

static int sizeof_pd_values();
static void pack_pd_values();
static void unpack_pd_values();
static void unpack_pm_values();

Status
XpexGetPickDevice(dpy, wks_id, dev_type, valueMask, values)
	register Display  *dpy;
	XpexPhigsWks  wks_id;
	XpexPickDeviceType  dev_type;
	XpexBitmask valueMask;
	XpexPickDevice  *values; /* RETURN */
{
	register	pexGetPickDeviceReq		*req;
				pexGetPickDeviceReply	reply;
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	Status		status = 1;
	char		*list;
	unsigned	size;

	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	XpexGetReq(GetPickDevice, dpy, i, req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->wks = wks_id;
	req->devType = (pexEnumTypeIndex) dev_type;
	req->itemMask = valueMask;
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (list = (char *) Xmalloc(size)) {
			_XRead(dpy, (char *)list, (long) size);
			unpack_pd_values(list, dev_type, valueMask, values);
			XFree((char *) list);
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
XpexChangePickDevice(dpy, wks, devType, valueMask, values)
	register Display  *dpy;
	XpexPhigsWks  wks;
	XpexPickDeviceType  devType;
	XpexBitmask valueMask;
	XpexPickDevice  *values;  /* RETURN */
{
	register pexChangePickDeviceReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	int	extra = sizeof_pd_values(valueMask, values);

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReqExtra(ChangePickDevice, dpy, i, extra, req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->wks = wks;
	req->devType = devType;
	req->itemMask = (pexBitmask) valueMask;
	pack_pd_values(valueMask, values, (char *)(req+1));
	UnlockDisplay(dpy);
	SyncHandle();
}

XpexPickMeasure
XpexCreatePickMeasure(dpy, wks_id, dev_type)
	register Display  *dpy;
	XpexPhigsWks  wks_id;
	XpexEnumTypeIndex  dev_type;
{
	register	pexCreatePickMeasureReq		*req;
	register 	XExtDisplayInfo *i = XpexFindDisplay(dpy);

	pexPickMeasure	pm_id;
	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReq(CreatePickMeasure, dpy, i, req);
	req->wks = wks_id;
	req->devType = dev_type;
	req->pm = pm_id = XAllocID(dpy);
	UnlockDisplay(dpy);
	SyncHandle();

	return pm_id;
}

void
XpexFreePickMeasure(dpy, pm_id)
	register Display *dpy;
	XpexPickMeasure pm_id;
{
	register pexFreePickMeasureReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);

	LockDisplay(dpy);
	XpexGetResReq(FreePickMeasure, dpy, i, pm_id, req);
	UnlockDisplay(dpy);
	SyncHandle();
}

Status
XpexGetPickMeasure(dpy, pm_id, valueMask, values)
	register Display *dpy;
	XpexPickMeasure pm_id;
	XpexBitmask	valueMask;
	XpexPickMeasureAttributes *values;
{
	register pexGetPickMeasureReq *req;
	pexGetPickMeasureReply reply;
	Status status = 1;
	unsigned size;
	char *list;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	XpexGetReq(GetPickMeasure, dpy, i, req);
	req->itemMask = (pexBitmask) valueMask;
	req->pm = (pexPickMeasure) pm_id;

	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (list = (char *) Xmalloc(size)) {
			_XRead(dpy, list, (long)size);
			unpack_pm_values(list, valueMask, values);
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
XpexUpdatePickMeasure(dpy, pm_id, input_record, length)
	register Display  *dpy;
	XpexPickMeasure  pm_id;
	char  *input_record;
	int length;
{
	register pexUpdatePickMeasureReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReqExtra(UpdatePickMeasure, dpy, i, length, req);
	req->pm = pm_id;
	bcopy((char *)input_record,(char *)(req + 1),length);
	UnlockDisplay(dpy);
	SyncHandle();
}

static void
unpack_pd_values(list, dev_type, valueMask, values)
	char *list;
	XpexEnumTypeIndex dev_type;
	XpexBitmask valueMask;
	XpexPickDevice *values; /* RETURN */
{
	if (valueMask & XpexPD_PickStatus) {
		values->pick_status = (int) *((CARD16 *)list);
		list += sizeof(CARD32);
	}
	if (valueMask & XpexPD_PickPath) {
		int count =
		values->pick_path.num_refs = (int) *((CARD32 *)list);
		int size  = count * sizeof(pexPickPath);

		list += sizeof(CARD32);

		if (values->pick_path.refs = 
		(XpexPickElementRef *) Xmalloc(size)) {
			bcopy((char *)list,
			(char *)values->pick_path.refs, size);
		}
		list += count * sizeof(pexPickPath);
	}
	if (valueMask & XpexPD_PickPathOrder) {
		values->pick_path_order = *((CARD16 *) list);
		list += sizeof(CARD32);
	}
	if (valueMask & XpexPD_PickIncl) {
		values->pick_incl = *((CARD32 *) list);
		list += sizeof(CARD32);
	}
	if (valueMask & XpexPD_PickExcl) {
		values->pick_excl = *((CARD32 *) list);
		list += sizeof(CARD32);
	}
	if (valueMask & XpexPD_PickDataRec) {
		if (dev_type == Xpex_PickDeviceDC_HitBox) {
			pexPD_DC_HitBox *p = (pexPD_DC_HitBox *) list;
			values->pick_data_rec.data_rec1.position.x = (int) p->position.x;
			values->pick_data_rec.data_rec1.position.y = (int) p->position.y;
			values->pick_data_rec.data_rec1.distance = 
			(XpexFloat) p->distance;
			list += sizeof(pexPD_DC_HitBox);
		} else if (dev_type == Xpex_PickDeviceNPC_HitVolume) {
			bcopy((char *)list,
			(char *)&(values->pick_data_rec.data_rec2),
			sizeof(pexPD_NPC_HitVolume));
			list += sizeof(pexPD_NPC_HitVolume);
		}
	}
	if (valueMask & XpexPD_PickPromptEchoType) {
		values->prompt_echo_type = *((INT16 *) list);
		list += sizeof(CARD32);
	}
	if (valueMask & XpexPD_PickEchoVolume) {
		pexViewport *s = (pexViewport *) list;
		values->echo_volume.minval.x = s->minval.x;
		values->echo_volume.minval.y = s->minval.y;
		values->echo_volume.minval.z = s->minval.z;
		values->echo_volume.maxval.x = s->maxval.x;
		values->echo_volume.maxval.y = s->maxval.y;
		values->echo_volume.maxval.z = s->maxval.z;
		values->echo_volume.useDrawable = s->useDrawable;
		list += sizeof(pexViewport);
	}
	if (valueMask & PEXPDPickEchoSwitch) {
		values->echo_switch = *((CARD16 *) list);
		list += sizeof(CARD32);
	}
}

static int data_rec_size[3] = {
	0,
	sizeof(pexPD_DC_HitBox),
	sizeof(pexPD_NPC_HitVolume)
};

static unsigned long pd_item_bit[9] = {
	XpexPD_PickStatus,
	XpexPD_PickPath,
	XpexPD_PickPathOrder,
	XpexPD_PickIncl,
	XpexPD_PickExcl,
	XpexPD_PickDataRec,
	XpexPD_PickPromptEchoType,
	XpexPD_PickEchoVolume,
	XpexPD_PickEchoSwitch
};

static int
sizeof_pd_values(devType,valueMask,values)
	XpexPickDeviceType devType;
	XpexBitmask valueMask;
	XpexPickDevice *values;
{
	int total_length = 0;

	if (valueMask & PEXPDPickStatus) {
		total_length += sizeof(CARD32);
	}
	if (valueMask & PEXPDPickPath) {
		total_length += sizeof(CARD32) + 
		(values->pick_path.num_refs * sizeof(pexPickPath));
	}
	if (valueMask & PEXPDPickPathOrder) {
		total_length += sizeof(CARD32);
	}
	if (valueMask & PEXPDPickIncl) {
		total_length += sizeof(CARD32);
	}
	if (valueMask & PEXPDPickExcl) {
		total_length += sizeof(CARD32);
	}
	if (valueMask & PEXPDPickDataRec) {
		total_length += sizeof(CARD32) +
		((devType == PEXPickDeviceDC_HitBox) ? sizeof(pexPD_DC_HitBox) :
		sizeof(pexPD_NPC_HitVolume));
	}
	if (valueMask & PEXPDPickPromptEchoType) {
		total_length += sizeof(CARD32);
	}
	if (valueMask & PEXPDPickEchoVolume) {
		total_length += sizeof(pexViewport);
	}
	if (valueMask & PEXPDPickEchoSwitch) {
		total_length += sizeof(CARD32);
	}
	return total_length;
}

static void
pack_pd_values(dev_type, valueMask, values, list)
	XpexEnumTypeIndex dev_type;
	XpexBitmask valueMask;
	XpexPickDevice *values;
	char *list;
{
	if (valueMask & PEXPDPickStatus) {
		CARD16 *d = (CARD16 *) list;
		*d = (CARD16) values->pick_status;
		list += sizeof(CARD32);
	}
	if (valueMask & PEXPDPickPath) {
		register int i;
		pexPickPath *d;
		int count = values->pick_path.num_refs;
		CARD32 *c = (CARD32 *) list;
		*c = (CARD32) count;
		list += sizeof(CARD32);
		bcopy((char *)&(values->pick_path.refs),
			(char *)list, count * sizeof(pexPickPath));
		list += count * sizeof(pexPickPath);
	}
	if (valueMask & PEXPDPickPathOrder) {
		CARD16 *d = (CARD16 *) list;
		*d = (CARD16) values->pick_path_order;
		list += sizeof(CARD32);
	}
	if (valueMask & PEXPDPickIncl) {
		CARD32 *d = (CARD32 *) list;
		*d = (CARD32) values->pick_incl;
		list += sizeof(CARD32);
	}
	if (valueMask & PEXPDPickExcl) {
		CARD32 *d = (CARD32 *) list;
		*d = (CARD32) values->pick_excl;
		list += sizeof(CARD32);
	}
	if (valueMask & PEXPDPickDataRec) {
		CARD32 *c = (CARD32 *) list;
		if (dev_type == PEXPickDeviceDC_HitBox) {
			pexPD_DC_HitBox *d;
			*c = sizeof(pexPD_DC_HitBox);
			list += sizeof(CARD32);
			d = (pexPD_DC_HitBox *) list;
			d->position.x = values->pick_data_rec.data_rec1.position.x;
			d->position.y = values->pick_data_rec.data_rec1.position.y;
			d->distance = values->pick_data_rec.data_rec1.distance;
			list += sizeof(pexPD_DC_HitBox);

		} else {
			pexPD_NPC_HitVolume *d;
			*c = sizeof(pexPD_NPC_HitVolume);
			list += sizeof(CARD32);
			d = (pexPD_NPC_HitVolume *) list;
			d->minval.x = (XpexFloat)
			values->pick_data_rec.data_rec2.minval.x;
			d->minval.y = (XpexFloat)
			values->pick_data_rec.data_rec2.minval.y;
			d->minval.z = (XpexFloat)
			values->pick_data_rec.data_rec2.minval.z;
			d->maxval.x = (XpexFloat)
			values->pick_data_rec.data_rec2.maxval.x;
			d->maxval.y = (XpexFloat)
			values->pick_data_rec.data_rec2.maxval.y;
			d->maxval.z = (XpexFloat)
			values->pick_data_rec.data_rec2.maxval.z;
			list += sizeof(pexPD_NPC_HitVolume);
		}
	}
	if (valueMask & PEXPDPickPromptEchoType) {
		INT16 *d = (INT16 *) list;
		*d = (INT16) values->prompt_echo_type;
		list += sizeof(CARD32);
	}
	if (valueMask & PEXPDPickEchoVolume) {
		pexViewport *d = (pexViewport *) list;

		d->minval.x = values->echo_volume.minval.x;
		d->minval.y = values->echo_volume.minval.y;
		d->minval.z = values->echo_volume.minval.z;

		d->maxval.x = values->echo_volume.maxval.x;
		d->maxval.y = values->echo_volume.maxval.y;
		d->maxval.z = values->echo_volume.maxval.z;
		list += sizeof(pexViewport);
	}
	if (valueMask & PEXPDPickEchoSwitch) {
		CARD16 *d = (CARD16 *) list;
		*d = (CARD16) values->echo_switch;
		list += sizeof(CARD32);
	}
}

static void
unpack_pm_values(list, valueMask, values)
	char *list;
	XpexBitmask valueMask;
	XpexPickMeasureAttributes *values; /* RETURN */
{
	if (valueMask & PEXPMStatus) {
		values->pick_status = (int) *((CARD16 *) list);
		list += sizeof(CARD32);
	}

	if (valueMask & PEXPMPath) {
		int count =
		values->picked_prim.num_refs = (int) *((CARD32 *) list);
		int size = count * sizeof(XpexPickElementRef);
		list += sizeof(CARD32);
		if (values->picked_prim.refs = 
		(XpexPickElementRef *) Xmalloc(size)) {
			bcopy((char *)list, 
			(char *)(values->picked_prim.refs), size);
		}
		/* not necessary to advance pointer */
	}
}
