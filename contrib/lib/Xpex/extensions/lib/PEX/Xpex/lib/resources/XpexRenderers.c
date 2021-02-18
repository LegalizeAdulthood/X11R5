/* $Header: XpexRenderers.c,v 2.11 91/09/11 16:07:25 sinyaw Exp $ */


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

static int sizeof_rdr_attrs();
static void pack_rdr_attrs();
static void unpack_rdr_attrs();

struct s {
	INT16 val1;
	INT16 val2;
};

XpexRenderer
XpexCreateRenderer(dpy, drawable, valuemask, values)
	Display  *dpy;
	Drawable  drawable;
	XpexBitmask valuemask;
	XpexRendererAttributes  *values;
{
	register 	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register	pexCreateRendererReq	*req;

	pexRenderer rdr_id;
    int	extra = 0;

	XpexCheckExtension(dpy,i,0);

	if (valuemask) {
		extra = sizeof_rdr_attrs(valuemask, values);
	}
	LockDisplay(dpy);
	XpexGetReqExtra(CreateRenderer, dpy, i, extra, req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->drawable = drawable;
	req->rdr = rdr_id = XAllocID(dpy);
	req->itemMask = valuemask;
	if (extra) {
    	pack_rdr_attrs(valuemask, values, (CARD32 *)(req + 1));
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return rdr_id;
}

void
XpexFreeRenderer(dpy, rdr_id)
	Display *dpy;
	XpexRenderer rdr_id;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexFreeRendererReq *req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetResReq(FreeRenderer, dpy, i, rdr_id, req);
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexChangeRenderer(dpy, rdr_id, valueMask, values)
	Display *dpy;
	XpexRenderer rdr_id;
	XpexBitmask valueMask;
	XpexRendererAttributes *values;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexChangeRendererReq *req;
    int extra = 0;

	XpexSimpleCheckExtension(dpy,i);

	if (valueMask) {
    	extra = sizeof_rdr_attrs(valueMask, values);
	}
	LockDisplay(dpy);
	XpexGetReqExtra(ChangeRenderer, dpy, i, extra, req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->rdr = rdr_id;
	req->itemMask = valueMask;
	if (extra) {
    	pack_rdr_attrs(valueMask, values, (CARD32 *)(req + 1));
	}
	UnlockDisplay(dpy);
	SyncHandle();
}


Status
XpexGetRendererAttributes(dpy, rdr_id, valueMask, values)
	register Display *dpy;
	XpexRenderer rdr_id;
	XpexBitmask valueMask;
	XpexRendererAttributes *values; /* RETURN */
{
	static Status _XpexGetRendererAttributes();

	CARD32 *list;

	if (!_XpexGetRendererAttributes(dpy,rdr_id,valueMask,&list)) {
		return 0;
	}
	unpack_rdr_attrs(list, valueMask, values);

	if (list) {
		XFree((char *)list);
	}
	return 1;
}

static Status
_XpexGetRendererAttributes(dpy, rdr_id, valueMask, list)
	register Display *dpy;
	XpexRenderer rdr_id;
	XpexBitmask valueMask;
	CARD32 **list; /* RETURN */
{
	register	XExtDisplayInfo	*i = XpexFindDisplay(dpy);
	register	pexGetRendererAttributesReq		*req;
				pexGetRendererAttributesReply	reply;
	Status		status = 1;
	unsigned	size;

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReq(GetRendererAttributes, dpy, i, req);
	req->rdr = rdr_id;
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->itemMask = valueMask;
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*list = (CARD32 *) Xmalloc(size)) {
			_XRead(dpy, (char *)*list, (long) size);
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
XpexGetRendererDynamics(dpy, rdr_id, dynamics)
	Display *dpy;
	XpexRenderer rdr_id;
	XpexRendererDynamics *dynamics; /* RETURN */
{
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register	pexGetRendererDynamicsReq	*req;
				pexGetRendererDynamicsReply	reply;


	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReq(GetRendererDynamics, dpy, i, req);
	req->id = rdr_id;
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy)
		SyncHandle();
		return 0;
	}
	dynamics->tables = (unsigned long) reply.tables;
	dynamics->namesets = (unsigned long) reply.namesets;
	dynamics->attributes = (unsigned long) reply.attributes;
	UnlockDisplay(dpy);
	SyncHandle();

	return 1;
}

void
XpexBeginRendering(dpy, rdr_id, drawable_id)
	Display *dpy;
	XpexRenderer rdr_id;
	Drawable drawable_id;
{
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register	pexBeginRenderingReq	*req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(BeginRendering, dpy, i, req);
	req->rdr = rdr_id;
	req->drawable = drawable_id;
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexEndRendering(dpy, rdr_id, flush_flag)
	Display  *dpy;
	XpexRenderer  rdr_id;
	XpexSwitch  flush_flag;
{
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register	pexEndRenderingReq	*req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(EndRendering,dpy, i, req);
	req->rdr = rdr_id;
	req->flushFlag = (pexSwitch) flush_flag;
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexBeginStructure(dpy, rdr_id, s_id)
	Display *dpy;
	XpexRenderer rdr_id;
	XpexStructure s_id;
{
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register	pexBeginStructureReq	*req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(BeginStructure,dpy, i, req);
	req->rdr = rdr_id;
	req->sid = s_id;
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexEndStructure(dpy, rdr_id)
	Display *dpy;
	XpexRenderer  rdr_id;
{
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register	pexEndStructureReq	*req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetResReq(EndStructure,dpy, i, rdr_id, req);
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexRenderOutputCommands(dpy, rdr_id, num_ocs, length, list)
	register Display *dpy;
	XpexRenderer rdr_id;
	int num_ocs;
	int length;
	char *list;
{
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register	pexRenderOutputCommandsReq	*req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReqExtra(RenderOutputCommands,dpy,i,length,req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->rdr = rdr_id;
	req->numCommands = (CARD32) num_ocs;
	bcopy(list, (char *)(req + 1), length);
	UnlockDisplay(dpy);
	SyncHandle();
}

void
_XpexRenderOutputCommands(dpy, r_id)
	Display *dpy;
	XpexRenderer r_id;
{
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexRenderOutputCommandsReq *req;

	XpexSimpleCheckExtension(dpy,i); /* just in case */
	XpexGetReq(RenderOutputCommands,dpy,i,req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->rdr = r_id;
	req->numCommands = (CARD32) 0;
}

void 
XpexRenderNetwork(dpy, rdr_id, drawable_id, s_id)
	register Display *dpy;
	XpexRenderer rdr_id;
	Drawable drawable_id;
	XpexStructure s_id;
{
	register	XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register	pexRenderNetworkReq		*req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(RenderNetwork, dpy, i, req);
	req->rdr = rdr_id;
	req->drawable = drawable_id;
	req->sid = s_id;
	UnlockDisplay(dpy);
	SyncHandle();
}

static int
sizeof_rdr_attrs(valueMask, values)
	XpexBitmask valueMask;
	XpexRendererAttributes *values;
{
    int size = 0;

	if (valueMask & PEXRDPipelineContext) {
    	size += sizeof(pexPC);
	}
	if (valueMask & PEXRDCurrentPath) {
    	size += sizeof(CARD32) +
		(values->current_path.num_refs * sizeof(pexElementRef));
	}
    if (valueMask & PEXRDMarkerBundle) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDTextBundle) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDLineBundle) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDInteriorBundle) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDEdgeBundle) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDViewTable) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDColourTable) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDDepthCueTable) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDLightTable) {
    	size += sizeof(CARD32);
	}
	if (valueMask & PEXRDColourApproxTable) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDPatternTable) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDTextFontTable) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDHighlightIncl) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDHighlightExcl) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDInvisibilityIncl) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDInvisibilityExcl) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDRendererState) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDHlhsrMode) {
    	size += sizeof(CARD32);
	}
    if (valueMask & PEXRDNpcSubvolume) {
    	size += sizeof(pexNpcSubvolume);
	}
    if (valueMask & PEXRDViewport) {
    	size += sizeof(pexViewport);
	}
    if (valueMask & PEXRDClipList) {
    	size += sizeof(CARD32) +
		values->clip_list.num_rects * sizeof(pexDeviceRect);
	}
    return size;
}

static void
pack_rdr_attrs(valueMask, values, list)
	XpexBitmask valueMask;
	XpexRendererAttributes *values; 
	CARD32 *list; /* RETURN */
{
	CARD32 *d;

	if (valueMask & XpexRD_PipelineContext) {
		*list++ = values->pc_id;
	}
	if (valueMask & XpexRD_CurrentPath) {
		int n = values->current_path.num_refs;
		int size = n * sizeof(pexElementRef);
		*list++ = n;

		if (size) {
			bcopy((char *)(values->current_path.elem_refs),list, size);
			list += size >> 2;
		}
	}
	if (valueMask & XpexRD_MarkerBundle) {
		*list++ = values->marker_bundle;
	}
	if (valueMask & XpexRD_TextBundle) {
		*list++ = values->text_bundle;
	}
	if (valueMask & XpexRD_LineBundle) {
		*list++ = values->line_bundle;
	}
	if (valueMask & XpexRD_InteriorBundle) {
		*list++ = values->interior_bundle;
	}
	if (valueMask & XpexRD_EdgeBundle) {
		*list++ = values->edge_bundle;
	}
	if (valueMask & XpexRD_ViewTable) {
		*list++ = values->view_table;
	}
	if (valueMask & XpexRD_ColorTable) {
		*list++ = values->color_table;
	}
	if (valueMask & XpexRD_DepthCueTable) {
		*list++ = values->depth_cue_table;
	}
	if (valueMask & XpexRD_LightTable) {
		*list++ = values->light_table;
	}
	if (valueMask & XpexRD_ColorApproxTable) {
		*list++ = values->color_approx_table;
	}
	if (valueMask & XpexRD_PatternTable) {
		*list++ = values->pattern_table;
	}
	if (valueMask & XpexRD_TextFontTable) {
		*list++ = values->text_font_table;
	}
	if (valueMask & XpexRD_HighlightIncl) {
		*list++ = values->highlight_incl;
	}
	if (valueMask & XpexRD_HighlightExcl) {
		*list++ = values->highlight_excl;
	}
	if (valueMask & XpexRD_InvisibilityIncl) {
		*list++ = values->invis_incl;
	}
	if (valueMask & XpexRD_InvisibilityExcl) {
		*list++ = values->invis_excl;
	}
	if (valueMask & XpexRD_RendererState) {
		CARD16 *v = (CARD16 *) list;
		*v = (CARD16) (values->renderer_state);
		list++;
	}
	if (valueMask & XpexRD_HlhsrMode) {
		INT16 *v = (INT16 *) list;	
		*v = (INT16) values->hlhsr_mode;
		list++;
	}
	if (valueMask & XpexRD_NpcSubvolume) {
		bcopy((char *) &(values->npc_subvolume),(char *) list,
		sizeof(pexNpcSubvolume));
		list += sizeof(pexNpcSubvolume) >> 2;
	}
	if (valueMask & XpexRD_Viewport) {
		bcopy((char *) &(values->viewport),(char *)list, 
		sizeof(pexViewport));
		list += sizeof(pexViewport) >> 2;
	}
	if (valueMask & XpexRD_ClipList) {
		int n = values->clip_list.num_rects;
		int size = n * sizeof(pexDeviceRect);
		*list++ = n;
		if (size) {
			bcopy((char *)(values->clip_list.device_rect),
			(char *)list, size);
		}
		list += size >> 2;
	}
}

static void
unpack_rdr_attrs(list, valueMask, values)
	CARD32 *list;
	XpexBitmask valueMask;
	XpexRendererAttributes *values; /* RETURN */
{
	if (valueMask & XpexRD_PipelineContext) {
		values->pc_id = *list++;
	}
	if (valueMask & XpexRD_CurrentPath) {

		int count =
		values->current_path.num_refs = (int) *list++;
		int size = count * sizeof(pexElementRef);

		if (count) {
			size = count * sizeof(pexElementRef);
			if(values->current_path.elem_refs = 
			(XpexElementRef *) Xmalloc((unsigned) size)) {
				bcopy((char *)list, 
				(char *)(values->current_path.elem_refs), 
				size);
			}
			list += size >> 2;
		}
	}
	if (valueMask & XpexRD_MarkerBundle) {
		values->marker_bundle = *list++;
	}
	if (valueMask & XpexRD_TextBundle) {
		values->text_bundle = *list++;
	}
	if (valueMask & XpexRD_LineBundle) {
		values->line_bundle = *list++;
	}
	if (valueMask & XpexRD_InteriorBundle) {
		values->interior_bundle = *list++;
	}
	if (valueMask & XpexRD_EdgeBundle) {
		values->edge_bundle = *list++;
	}
	if (valueMask & XpexRD_ViewTable) {
		values->view_table = *list++;
	}
	if (valueMask & XpexRD_ColorTable) {
		values->color_table = *list++;
	}
	if (valueMask & XpexRD_DepthCueTable) {
		values->depth_cue_table = *list++;
	}
	if (valueMask & XpexRD_LightTable) {
		values->light_table = *list++; 
	}
	if (valueMask & XpexRD_ColorApproxTable) {
		values->color_approx_table = *list++;
	}
	if (valueMask & XpexRD_PatternTable) {
		values->pattern_table = *list++;
	}
	if (valueMask & XpexRD_TextFontTable) {
		values->text_font_table = *list++;
	}
	if (valueMask & XpexRD_HighlightIncl) {
		values->highlight_incl = *list++;
	}
	if (valueMask & XpexRD_HighlightExcl) {
		values->highlight_excl = *list++;
	}
	if (valueMask & XpexRD_InvisibilityIncl) {
		values->invis_incl = *list++;
	}
	if (valueMask & XpexRD_InvisibilityExcl) {
		values->invis_excl = *list++;
	}
	if (valueMask & XpexRD_RendererState) {
		values->renderer_state = *((CARD16 *) list++);
	}
	if (valueMask & XpexRD_HlhsrMode) {
		values->hlhsr_mode = ((struct s *) list)->val2;
		list++;
	}
	if (valueMask & XpexRD_NpcSubvolume) {
		pexNpcSubvolume *npc = (pexNpcSubvolume *) list;
		values->npc_subvolume.minval.x = npc->minval.x;
		values->npc_subvolume.minval.y = npc->minval.y;
		values->npc_subvolume.minval.z = npc->minval.z;
		values->npc_subvolume.maxval.x = npc->maxval.x;
		values->npc_subvolume.maxval.y = npc->maxval.y;
		values->npc_subvolume.maxval.z = npc->maxval.z;
		list = (CARD32 *) ++npc;
	}
	if (valueMask & XpexRD_Viewport) {
		pexViewport *vp = (pexViewport *)list;
		values->viewport.minval.x = vp->minval.x;
		values->viewport.minval.y = vp->minval.y;
		values->viewport.minval.z = vp->minval.z;
		values->viewport.maxval.x = vp->maxval.x;
		values->viewport.maxval.y = vp->maxval.y;
		values->viewport.maxval.z = vp->maxval.z;
		values->viewport.useDrawable = vp->useDrawable;
		list = (CARD32 *) ++vp;
	}
	if (valueMask & XpexRD_ClipList) {
		int count = 
		values->clip_list.num_rects = (int) *((CARD32 *) list++);
		if (count) {
			int size = count * sizeof(pexDeviceRect);
			if (values->clip_list.device_rect = 
			(XpexDeviceRect *) Xmalloc(size)) {
				bcopy((char *)list,
				(char *)(values->clip_list.device_rect),
				size);
			}
			list += size >> 2;
		}
	}
}
