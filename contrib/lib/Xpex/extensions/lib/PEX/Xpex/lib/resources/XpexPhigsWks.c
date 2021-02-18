/* $Header: XpexPhigsWks.c,v 2.6 91/09/11 15:50:59 sinyaw Exp $ */
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

XpexPhigsWks
XpexCreatePhigsWks(dpy, drawable, wks_res, buffer_mode)
	Display *dpy;
	Drawable drawable;
	XpexWksResources *wks_res;
	XpexBufferMode buffer_mode;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexCreatePhigsWksReq *req;

    pexPhigsWks wks_id;

	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	XpexGetReq(CreatePhigsWks,dpy,i,req);
	req->wks = wks_id = XAllocID(dpy);
	req->drawable = drawable;
	req->markerBundle = wks_res->marker_bundle;
	req->textBundle = wks_res->text_bundle;
	req->lineBundle = wks_res->line_bundle;
	req->interiorBundle = wks_res->interior_bundle;
	req->edgeBundle = wks_res->edge_bundle;
	req->colourTable = wks_res->color_table;
	req->depthCueTable = wks_res->depth_cue_table;
	req->lightTable = wks_res->light_table;
	req->colourApproxTable = wks_res->color_approx_table;
	req->patternTable = wks_res->pattern_table;
	req->textFontTable = wks_res->text_font_table;
	req->highlightIncl = wks_res->highlight_incl; 
	req->highlightExcl = wks_res->highlight_excl;
	req->invisIncl = wks_res->invis_incl;
	req->invisExcl = wks_res->invis_excl;
	req->bufferMode =  buffer_mode;
	
	UnlockDisplay(dpy);
	SyncHandle();

    return wks_id;
}

void
XpexFreePhigsWks(dpy, wks_id)
	Display *dpy;
	XpexPhigsWks wks_id;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexFreePhigsWksReq *req;

	XpexSimpleCheckExtension(dpy,i);

	LockDisplay(dpy);
	XpexGetResReq(FreePhigsWks, dpy, i, wks_id, req);
	UnlockDisplay(dpy);
	SyncHandle();
}

Status 
XpexGetWksInfo(dpy, wks_id, valueMask, values)
	Display *dpy;
	XpexPhigsWks wks_id;
	XpexBitmask valueMask[2];
	XpexWksInfo *values;
{
	/* static Status _XpexGetWksInfo(); */
	static void unpack_wks_info();

	CARD32 *list;

	if (!_XpexGetWksInfo(dpy, wks_id, valueMask, &list)) {
		return 0;
	}

	unpack_wks_info((CARD32 *)list, valueMask, values);

	if (list) {
		XFree(list);
	}
	return 1;
}

Status
XpexGetDynamics(dpy, drawable, dynamics)
	Display  *dpy;
	Drawable  drawable;
	XpexDynamics  *dynamics; /* RETURN */
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
    register pexGetDynamicsReq *req;
    pexGetDynamicsReply reply;

	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	XpexGetReq(GetDynamics, dpy, i, req);
	req->drawable = drawable;

	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
    dynamics->view_rep	= (int) reply.viewRep;
    dynamics->marker_bundle	= (int) reply.markerBundle;
    dynamics->text_bundle	= (int) reply.textBundle;
    dynamics->line_bundle	= (int) reply.lineBundle;
    dynamics->interior_bundle	= (int) reply.interiorBundle;
    dynamics->edge_bundle	= (int) reply.edgeBundle;
    dynamics->color_table	= (int) reply.colourTable;
    dynamics->pattern_table	= (int) reply.patternTable;
    dynamics->wks_transform	= (int) reply.wksTransform;
    dynamics->highlight_filter	= (int) reply.highlightFilter;
    dynamics->invisibility_filter	= (int) reply.invisibilityFilter;
    dynamics->hlhsr_mode	= (int) reply.HlhsrMode;
    dynamics->structure_modify	= (int) reply.structureModify;
    dynamics->post_structure	= (int) reply.postStructure;
    dynamics->unpost_structure	= (int) reply.unpostStructure;
    dynamics->delete_structure	= (int) reply.deleteStructure;
    dynamics->reference_modify	= (int) reply.referenceModify;
    dynamics->buffer_modify	= (int) reply.bufferModify;

	UnlockDisplay(dpy);
	SyncHandle();

	return 1;
}

Status
XpexGetViewRep(dpy, wks_id, index, view_update, requested, current)
	Display  *dpy;
	XpexPhigsWks  wks_id;
	XpexTableIndex  index;
	XpexUpdateState  *view_update;  /* RETURN */
	XpexViewRep *requested;  /* RETURN */
	XpexViewRep *current;  /* RETURN */
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexGetViewRepReq 	*req;
			 pexGetViewRepReply reply;
	pexViewRep *view_reps;
	unsigned size;
	Status status = 0;

	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	XpexGetReq(GetViewRep, dpy, i, req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->wks = wks_id;
	req->index = index;
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*view_update = (XpexUpdateState) reply.viewUpdate;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (view_reps = (pexViewRep *) Xmalloc(size)) {
			_XRead(dpy, (char *) view_reps, (long) size);
			bcopy((char *)&view_reps[0], 
				(char *)requested, sizeof(pexViewRep));
			bcopy((char *)&view_reps[1], 
				(char *)current, sizeof(pexViewRep));
			XFree((char *)view_reps);
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
XpexRedrawAllStructures(dpy, wks_id)
	Display  *dpy;
	XpexPhigsWks  wks_id;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
    register pexRedrawAllStructuresReq *req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetResReq(RedrawAllStructures,dpy,i,wks_id,req);
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexUpdateWorkstation(dpy, wks_id)
	Display  *dpy;
	XpexPhigsWks  wks_id;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
    register pexUpdateWorkstationReq *req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetResReq(UpdateWorkstation,dpy,i,wks_id,req);
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexRedrawClipRegion(dpy, wks_id, dev_rects, count)
	Display  *dpy;
	XpexPhigsWks  wks_id;
	XpexDeviceRect *dev_rects;
	int count;
{

	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexRedrawClipRegionReq *req;
	int extra = count * sizeof(pexDeviceRect);
	pexDeviceRect *dest;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReqExtra(RedrawClipRegion,dpy,i,extra,req);
	req->wks = wks_id;
	req->numRects = (CARD32) count;
	dest = (pexDeviceRect *) (req + 1);
	while (count--) {
		dest->xmin = (INT16) dev_rects->xmin;
		dest->ymin = (INT16) dev_rects->ymin;
		dest->xmax = (INT16) dev_rects->xmax;
		dest->ymax = (INT16) dev_rects->ymax;
		dest++;
		dev_rects++; /* src */
	}
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexExecuteDeferredActions(dpy, wks_id)
	Display  *dpy;
	XpexPhigsWks  wks_id;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexExecuteDeferredActionsReq *req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetResReq(ExecuteDeferredActions, dpy, i, wks_id, req);
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexSetViewPriority(dpy, wks_id, index1, index2, priority)
	Display  *dpy;
	XpexPhigsWks  wks_id;
	XpexTableIndex  index1;
	XpexTableIndex  index2;
	short  priority;
{
	register	pexSetViewPriorityReq *req;
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(SetViewPriority,dpy,i,req);
	req->wks = wks_id;
	req->index1 = index1;
	req->index2 = index2;
	req->priority = priority;
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexSetDisplayUpdateMode(dpy, wks_id, mode)
	Display  *dpy;
	XpexPhigsWks  wks_id;
	XpexEnumTypeIndex  mode;
{
	register	pexSetDisplayUpdateModeReq *req;
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(SetDisplayUpdateMode, dpy, i, req);
	req->wks = wks_id;
	req->displayUpdate = (pexEnumTypeIndex) mode;
	UnlockDisplay(dpy);
	SyncHandle();
}

Status
XpexMapDCtoWC(dpy, wks_id, dc_pts, num_dc_pts, view_index, 
	wc_pts, num_wc_pts)
	Display  *dpy;
	XpexPhigsWks  wks_id;
	XpexDeviceCoord  *dc_pts;
	int num_dc_pts;
	XpexTableIndex  *view_index; /* RETURN */
	XpexCoord3D  **wc_pts; /* RETURN */
	int  *num_wc_pts; /* RETURN */
{
	register 	pexCoord3D *pCoord3D;
	register	pexDeviceCoord *dest;
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register	pexMapDCtoWCReq *req;
				pexMapDCtoWCReply reply;

	Status		status = 1;
	int 		extra = num_dc_pts * sizeof(pexDeviceCoord);
	unsigned 	size;

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReqExtra(MapDCtoWC, dpy, i, extra, req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->wks = wks_id;
	req->numCoords = (CARD32) num_dc_pts;
	dest = (pexDeviceCoord *)(req + 1);
	bcopy((char *)dc_pts, (char *)dest, extra);
	if( ! _XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*view_index = (XpexTableIndex) reply.viewIndex;
	*num_wc_pts = (int) reply.numCoords;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (pCoord3D = (pexCoord3D *) Xmalloc(size)) {
			_XRead(dpy, (char *)pCoord3D, (long) size);
			bcopy((char *)pCoord3D, (char *)*wc_pts, size);
			XFree((char *)pCoord3D);
		} else {
			status = 0;
			_XpexClearReply(dpy, reply.length);
		}
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return status;
}

Status
XpexMapWCtoDC(dpy, wks_id, view_index, wc_pts, num_wc_pts,
    dc_pts, num_dc_pts)
	Display  *dpy;
	XpexPhigsWks  wks_id;
	XpexTableIndex  view_index;     
	XpexCoord3D  *wc_pts;
	int  num_wc_pts;
	XpexDeviceCoord  **dc_pts;  /* RETURN */
	int  *num_dc_pts;  /* RETURN */
{
	register pexCoord3D *dest;
	register pexDeviceCoord *pDC;
	register pexMapWCtoDCReq *req;
	pexMapWCtoDCReply reply;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	Status status = 1;

	int extra = num_wc_pts * sizeof(pexCoord3D);
	unsigned size;

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReqExtra(MapWCtoDC, dpy, i, extra, req);
	req->wks = wks_id;
	req->index = (pexTableIndex) view_index;
	req->numCoords = (CARD32) num_wc_pts;
	dest = (pexCoord3D *) (req + 1);
	bcopy((char *)wc_pts, (char *)dest, extra);
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*num_dc_pts = (int) reply.numCoords;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (pDC = (pexDeviceCoord *)Xmalloc(size)) {
			_XRead(dpy, (char *) pDC, (long) size);
			bcopy((char *)pDC, (char *)*dc_pts, size);
			XFree((char *)pDC);
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
XpexSetViewRep(dpy, wks_id, view_rep)
	Display *dpy;
	XpexPhigsWks wks_id;
	XpexViewRep *view_rep;
{
	register 	int 				i, j;
	register 	pexViewRep			*dest;
	register 	pexSetViewRepReq	*req;
	register	XExtDisplayInfo 	*info = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,info);
	
	LockDisplay(dpy);
	XpexGetReq(SetViewRep, dpy, info, req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->wks = wks_id;
	req->viewRep.index = view_rep->index;
	req->viewRep.view.clipFlags = view_rep->view.clipFlags;

	bcopy((char *)&(view_rep->view.clipLimits),
		(char *)&(req->viewRep.view.clipLimits), 
		sizeof(pexViewRep));

	for (j = 0; j <= 3 ; j++) {
		for (i = 0; i <= 3 ; i++) {
			req->viewRep.view.orientation[j][i] = 
				(XpexFloat) view_rep->view.orientation[j][i];
		}
	}

	for (j = 0; j <= 3 ; j++) {
		for (i = 0; i <= 3 ; i++) {
			req->viewRep.view.mapping[j][i] = 
				(XpexFloat) view_rep->view.mapping[j][i];
		}
	}
	UnlockDisplay(dpy);
	SyncHandle();
}

#define Xpex_ConvertNpcSubvolume(_dest,_src) \
_dest->minval.x = (XpexFloat) _src->minval.x; \
_dest->minval.y = (XpexFloat) _src->minval.y; \
_dest->minval.z = (XpexFloat) _src->minval.z; \
_dest->maxval.x = (XpexFloat) _src->maxval.x; \
_dest->maxval.y = (XpexFloat) _src->maxval.y; \
_dest->maxval.z = (XpexFloat) _src->maxval.z

void
XpexSetWksWindow(dpy, wks_id, npc_subvolume)
	Display  *dpy;
	XpexPhigsWks  wks_id;
	XpexNpcSubvolume  *npc_subvolume; 
{
	register pexSetWksWindowReq *req;
    register pexNpcSubvolume *pDest;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);

	LockDisplay(dpy);
	XpexGetReq(SetWksWindow, dpy, i, req);
	req->wks = wks_id;
	pDest = (pexNpcSubvolume *)(req + 1);
    Xpex_ConvertNpcSubvolume(pDest,npc_subvolume);
	UnlockDisplay(dpy);
	SyncHandle();
}

#undef Xpex_ConvertNpcSubvolume

#define Xpex_ConvertWksViewport(_dest,_src) \
_dest->minval.x = (INT16) _src->minval.x; \
_dest->minval.y = (INT16) _src->minval.y; \
_dest->minval.z = (XpexFloat) _src->minval.z; \
_dest->maxval.x = (INT16) _src->maxval.x; \
_dest->maxval.y = (INT16) _src->maxval.y; \
_dest->maxval.z = (XpexFloat) _src->maxval.z; \
_dest->useDrawable = (pexSwitch) _src->useDrawable

void
XpexSetWksViewport(dpy, wks_id, viewport)
	Display  *dpy;
	XpexPhigsWks  wks_id;
	XpexViewport  *viewport; 
{
	register pexViewport *dest;
	register pexSetWksViewportReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);

	LockDisplay(dpy);
	XpexGetReq(SetWksViewport, dpy, i, req);
	req->wks = wks_id;
	dest = (pexViewport *)(req + 1);
	Xpex_ConvertWksViewport(dest,viewport);
	UnlockDisplay(dpy);
	SyncHandle();
}

#undef Xpex_ConvertWksViewport

void
XpexSetHlhsrMode(dpy, wks_id, mode)
	Display  *dpy;
	XpexPhigsWks  wks_id;
	XpexHlhsrMode  mode;
{
	register pexSetHlhsrModeReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);

	LockDisplay(dpy);
	XpexGetReq(SetHlhsrMode, dpy, i, req);
	req->wks = wks_id;
	req->mode = mode;
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexSetWksBufferMode(dpy, wks_id, buffer_mode)
	Display  *dpy;
	XpexPhigsWks  wks_id;
	int  buffer_mode; 
{ 
	register pexSetWksBufferModeReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);

	LockDisplay(dpy);
	XpexGetReq(SetWksBufferMode,dpy,i,req);
	req->wks = wks_id;
	req->bufferMode = (CARD16) buffer_mode;
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexPostStructure(dpy, wks_id, s_id, priority)
	Display  *dpy;
	XpexPhigsWks  wks_id;
	XpexStructure  s_id;
	XpexFloat  priority;
{
	register pexPostStructureReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);

	LockDisplay(dpy);
	XpexGetReq(PostStructure, dpy, i, req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->wks = wks_id;
	req->sid = s_id;
	req->priority = (XpexFloat) priority;
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexUnpostStructure(dpy, wks_id, s_id)
	Display  *dpy;
	XpexPhigsWks  wks_id;
	XpexStructure  s_id;
{
	register pexUnpostStructureReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);

	LockDisplay(dpy);
	XpexGetReq(UnpostStructure, dpy, i, req);
	req->wks = wks_id;
	req->sid = s_id;
	UnlockDisplay(dpy);
	SyncHandle();
}

void 
XpexUnpostAllStructures(dpy, wks_id)
	Display  *dpy;
	XpexPhigsWks  wks_id;
{
	register pexUnpostAllStructuresReq *req;

	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetResReq(UnpostAllStructures, dpy, i, wks_id, req);
	UnlockDisplay(dpy);
	SyncHandle();
}

Status
XpexGetWksPostings(dpy, s_id, wks_ids, count)
	Display  *dpy;
	XpexStructure  s_id;
	XpexPhigsWks  **wks_ids;  /* RETURN */
	int  *count;  /* RETURN */
{
	register pexGetWksPostingsReq *req;
	pexGetWksPostingsReply reply;
	unsigned size;
	Status status = 1;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	XpexGetResReq(GetWksPostings, dpy, i, s_id, req);
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*count = (int) reply.length;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*wks_ids = (pexPhigsWks *) Xmalloc(size)) {
			_XRead(dpy, (char *)*wks_ids, (long)size);
		} else {
			status = 0;
			_XpexClearReply(dpy, reply.length);
		}
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return status;
}

/* static */ Status
_XpexGetWksInfo(dpy, wks_id, valuemask, list)
	Display	*dpy;
	XpexPhigsWks wks_id;
	XpexBitmask	valuemask[PEXMSGetWksInfo];
	CARD32 **list; /* RETURN */
{
	register	pexGetWksInfoReq *req;
				pexGetWksInfoReply reply;
	register 	XExtDisplayInfo *i = XpexFindDisplay(dpy);

	Status		status = 1;
	unsigned	size;

	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	XpexGetReq(GetWksInfo, dpy, i, req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->wks = wks_id;
	req->itemMask[0] = valuemask[0];
	req->itemMask[1] = valuemask[1];
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*list = (CARD32 *)Xmalloc(size)) {
			_XRead(dpy, (char *) *list, (long) size);
		} else {
			status = 0;
			_XpexClearReply(dpy, reply.length);
		}
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return status;
}

typedef struct {
	pexTableIndex index B16;
	CARD16 pad B16;
} pexTableIndexPadded;

static void
unpack_wks_info(list, itemMask, wksInfo)
	CARD32 *list;
	XpexBitmask itemMask[2];
	XpexWksInfo *wksInfo; /* RETURN */
{
	char *ptr;

	if (itemMask[0] & XpexPW_DisplayUpdate) {
		wksInfo->display_update = *((INT16 *) list);
		list++;
	}
	if (itemMask[0] & XpexPW_VisualState) {
		wksInfo->visual_state = *((CARD8 *) list);
		list++;
	}
	if (itemMask[0] & XpexPW_DisplaySurface) {
		wksInfo->display_surface = *((CARD8 *) list);
		list++;
	}
	if (itemMask[0] & XpexPW_ViewUpdate) {
		wksInfo->view_update = *((CARD8 *) list);
		list++;
	}
	if (itemMask[0] & XpexPW_DefinedViews) {
		register int i;
		pexTableIndexPadded *src;
		int count = wksInfo->defined_views.num_indices = (int) *list++;
		int size = count * sizeof(pexTableIndex);
		if (count) {
			if (wksInfo->defined_views.indices = 
			(pexTableIndex *) Xmalloc(size)) {
				src = (pexTableIndexPadded *) list;
				for (i = 0; i < count; i++) {
					wksInfo->defined_views.indices[i] = src[i].index;
				}
			}
			ptr = ((char *) list) + 
				(count * sizeof(pexTableIndexPadded));
			list = (CARD32 *) ptr;
		}
	} 
	if (itemMask[0] & XpexPW_WksUpdate) {
		wksInfo->wks_update = *((CARD8 *) list);
		list++;
	}
	if (itemMask[0] & XpexPW_ReqNpcSubvolume) {
		pexNpcSubvolume *p = (pexNpcSubvolume *) list;
		wksInfo->req_NPC_subvolume.minval.x = p->minval.x;
		wksInfo->req_NPC_subvolume.minval.y = p->minval.y;
		wksInfo->req_NPC_subvolume.minval.z = p->minval.z;
		wksInfo->req_NPC_subvolume.maxval.x = p->maxval.x;
		wksInfo->req_NPC_subvolume.maxval.y = p->maxval.y;
		wksInfo->req_NPC_subvolume.maxval.z = p->maxval.z;
		list = (CARD32 *) ++p;
	}
	if (itemMask[0] & XpexPW_CurNpcSubvolume) {
		pexNpcSubvolume *p = (pexNpcSubvolume *) list;
		wksInfo->cur_NPC_subvolume.minval.x = p->minval.x;
		wksInfo->cur_NPC_subvolume.minval.y = p->minval.y;
		wksInfo->cur_NPC_subvolume.minval.z = p->minval.z;
		wksInfo->cur_NPC_subvolume.maxval.x = p->maxval.x;
		wksInfo->cur_NPC_subvolume.maxval.y = p->maxval.y;
		wksInfo->cur_NPC_subvolume.maxval.z = p->maxval.z;
		list = (CARD32 *) ++p;
	}
	if (itemMask[0] & XpexPW_ReqWksViewport) {
		pexViewport *p = (pexViewport *) list;
		wksInfo->req_wks_viewpt.minval.x = p->minval.x;
		wksInfo->req_wks_viewpt.minval.y = p->minval.y;
		wksInfo->req_wks_viewpt.minval.z = p->minval.z;
		wksInfo->req_wks_viewpt.maxval.x = p->maxval.x;
		wksInfo->req_wks_viewpt.maxval.y = p->maxval.y;
		wksInfo->req_wks_viewpt.maxval.z = p->maxval.z;
		wksInfo->req_wks_viewpt.useDrawable = p->useDrawable;
		list = (CARD32 *) ++p;
	}
	if (itemMask[0] & XpexPW_CurWksViewport) {
		pexViewport *p = (pexViewport *) list;
		wksInfo->cur_wks_viewpt.minval.x = p->minval.x;
		wksInfo->cur_wks_viewpt.minval.y = p->minval.y;
		wksInfo->cur_wks_viewpt.minval.z = p->minval.z;
		wksInfo->cur_wks_viewpt.maxval.x = p->maxval.x;
		wksInfo->cur_wks_viewpt.maxval.y = p->maxval.y;
		wksInfo->cur_wks_viewpt.maxval.z = p->maxval.z;
		wksInfo->cur_wks_viewpt.useDrawable = p->useDrawable;
		list = (CARD32 *) ++p;
	}
	if (itemMask[0] & XpexPW_HlhsrUpdate) {
		wksInfo->hlhsr_update = *((CARD8 *) list); 
		list++;
	}
	if (itemMask[0] & XpexPW_ReqHlhsrMode) {
		wksInfo->req_hlhsr_mode = *((INT16 *) list); 
		list++;
	}
	if (itemMask[0] & XpexPW_CurHlhsrMode) {
		wksInfo->cur_hlhsr_mode = *((INT16 *) list);
		list++;
	}
	if (itemMask[0] & XpexPW_Drawable) {
		wksInfo->drawable_id = *list++;
	}
	if (itemMask[0] & XpexPW_MarkerBundle) {
		wksInfo->marker_bundle = *list++;
	}
	if (itemMask[0] & XpexPW_TextBundle) {
		wksInfo->text_bundle = *list++;
	}
	if (itemMask[0] & XpexPW_LineBundle) {
		wksInfo->line_bundle = *list++;
	}
	if (itemMask[0] & XpexPW_InteriorBundle) {
		wksInfo->interior_bundle = *list++;
	}
	if (itemMask[0] & XpexPW_EdgeBundle) {
		wksInfo->edge_bundle = *list++;
	}
	if (itemMask[0] & XpexPW_ColorTable) {
		wksInfo->color_table = *list++;
	}
	if (itemMask[0] & XpexPW_DepthCueTable) {
		wksInfo->depth_cue_table = *list++;
	}
	if (itemMask[0] & XpexPW_LightTable) {
		wksInfo->light_table = *list++;
	}
	if (itemMask[0] & XpexPW_ColorApproxTable) {
		wksInfo->color_approx_table = *list++;
	}
	if (itemMask[0] & XpexPW_PatternTable) {
		wksInfo->pattern_table = *list++;
	}
	if (itemMask[0] & XpexPW_TextFontTable) {
		wksInfo->text_font_table = *list++;
	}
	if (itemMask[0] & XpexPW_HighlightIncl) {
		wksInfo->highlight_incl = *list++;
	}
	if (itemMask[0] & XpexPW_HighlightExcl) {
		wksInfo->highlight_excl = *list++;
	}
	if (itemMask[0] & XpexPW_InvisibilityIncl) {
		wksInfo->invis_incl = *list++;
	}
	if (itemMask[0] & XpexPW_InvisibilityExcl) {
		wksInfo->invis_excl = *list++;
	}
	if (itemMask[0] & XpexPW_PostedStructures) {
		pexStructureInfo *src;
		XpexPostedStructureInfo *dest;
		int count =
		wksInfo->posted_structs.num_info = *((CARD32 *) list);
		list += sizeof(CARD32);
		if (count) {
			register int i;
			int sizeOfList = count * sizeof(pexStructureInfo);

			src = (pexStructureInfo *) list;
			if (wksInfo->posted_structs.info = 
			(XpexPostedStructureInfo *) Xmalloc(sizeOfList)) {
				dest = wksInfo->posted_structs.info;
				for (i = 0; i < count; i++) {
					dest[i].sid = src[i].sid;
					dest[i].priority = src[i].priority;
				}
				list += sizeOfList;
			}
		}
	}
	if (itemMask[0] & XpexPW_NumPriorities) {
		wksInfo->num_priorities = *list++;
	}
	if (itemMask[0] & XpexPW_BufferUpdate) {
		wksInfo->buffer_update = *((CARD8 *) list);
		list++;
	}
	if (itemMask[1] & XpexPW_ReqBufferMode) {
		wksInfo->req_buffer_mode = *((CARD16 *) list);
		list++;
	}
	if (itemMask[1] & XpexPW_CurBufferMode) {
		wksInfo->cur_buffer_mode = *((CARD16 *) list);
		list++; /* not really necessary */
	}
}
