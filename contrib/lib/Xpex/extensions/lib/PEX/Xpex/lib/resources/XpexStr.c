/* $Header: XpexStr.c,v 2.4 91/09/11 17:46:21 sinyaw Exp $ */
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

XpexStructure
XpexCreateStructure(dpy)
	register Display  *dpy;
{
	register pexCreateStructureReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	pexStructure s_id;

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	s_id = XAllocID(dpy);
	XpexGetResReq(CreateStructure, dpy, i, s_id, req);
	UnlockDisplay(dpy);
	SyncHandle();
	return  s_id;
}

void
XpexCopyStructure(dpy, src_s_id, dest_s_id)
	Display *dpy;
	XpexStructure  src_s_id;
	XpexStructure  dest_s_id;
{
	register pexCopyStructureReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(CopyStructure, dpy, i, req);
	req->src = src_s_id;
	req->dst = dest_s_id;
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexDestroyStructures(dpy, s_ids, count)
    Display  *dpy;
	XpexStructure  *s_ids;
	int  count;
{
	register pexDestroyStructuresReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	int extra = count * sizeof(pexStructure);

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReqExtra(DestroyStructures, dpy, i, extra, req);
	req->numStructures = (CARD32) count;
	bcopy((char *)(s_ids),(char *)(req + 1), extra);
	UnlockDisplay(dpy);
	SyncHandle();
}

Status
XpexGetStructureInfo(dpy, s_id, info)
	register Display *dpy;
	XpexStructure  s_id;
	XpexStructureInfo  *info; /* RETURN */
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexGetStructureInfoReq *req;
	pexGetStructureInfoReply reply;

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReq(GetStructureInfo, dpy, i, req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->sid = s_id;
	req->itemMask = PEXEditMode | PEXElementPtr | 
		PEXNumElements | PEXLengthStructure | PEXHasRefs;
	if (!_XReply(dpy, (xReply *)&reply, 0, xTrue)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	info->editing_mode = (int) reply.editMode;
	info->element_ptr = (int) reply.elementPtr;
	info->num_elements = (int) reply.numElements;
	info->total_length = (int) reply.lengthStructure;
	info->has_refs = (Bool) reply.hasRefs;
	UnlockDisplay(dpy);
	SyncHandle();

	return 1;
}

Status
XpexGetElementInfo(dpy, s_id, range, elem_info, count)
	register Display *dpy;
	XpexStructure s_id;
	XpexElementRange *range;
	XpexElementInfo **elem_info;  /* RETURN */
	int *count;  /* RETURN */
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexGetElementInfoReq *req;
	pexGetElementInfoReply reply;
	Status status = 1;
	unsigned size;
	char *pStream;

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReq(GetElementInfo, dpy, i, req);
	req->fpFormat = XpexGetFloatingPointFormat(req);
	req->sid = s_id;
	req->range.position1.whence = range->position1.whence;
	req->range.position1.offset = range->position1.offset;
	req->range.position2.whence = range->position2.whence;
	req->range.position2.offset = range->position2.offset;
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*count = (int) reply.numInfo;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*elem_info = (XpexElementInfo*) Xmalloc(size)) {
			_XRead(dpy, (char *)*elem_info, (long)size);
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
XpexGetStructuresInNetwork(dpy, s_id, which, s_ids, count)
	Display *dpy;
	XpexStructure s_id;
	int which;
	XpexStructure **s_ids; /* RETURN */
	int *count; /* RETURN */
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexGetStructuresInNetworkReq *req;
	pexGetStructuresInNetworkReply reply;
	Status status = 1;
	unsigned size;

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReq(GetStructuresInNetwork, dpy, i, req);
	req->sid = s_id;
	req->which = (CARD16) which;

	if(!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*count = (int) reply.numStructures;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*s_ids = (XpexStructure *) Xmalloc(size)) {
			_XRead(dpy, (char *)*s_ids, (long)size);
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
XpexGetAncestors(dpy, s_id, path_order, path_depth, paths, num_paths) 
	Display  *dpy; 
	XpexStructure  s_id;
	int  path_order;
	int  path_depth;
	XpexListOfElementRef  *paths[];	/* RETURN */
	int  *num_paths;	/* RETURN */
{
	register XExtDisplayInfo *info = XpexFindDisplay(dpy);
	register i, j;
	register pexGetAncestorsReq *req;
	pexGetAncestorsReply reply;
	Status status = 1;
	unsigned size;
	char *list;

	XpexCheckExtension(dpy,info,0);
	LockDisplay(dpy);
	XpexGetReq(GetAncestors, dpy, info, req);
	req->sid = s_id;
	req->pathOrder = (CARD16) path_order;
	req->pathDepth = (CARD16) path_depth;
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*num_paths = (int) reply.numPaths;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (list = Xmalloc(size)) {
			_XRead(dpy, (char *) list, (long)size);
		} else {
			status = 0;
			_XpexClearReply(dpy, reply.length);
		}
	}	
	UnlockDisplay(dpy);
	SyncHandle();

	/* convert stream to paths */ 
	for (i = 0; i < *num_paths; i++) {
		CARD32 *c32 = (CARD32 *) list;
		paths[i]->num_refs = (int) *c32;
		list += sizeof(CARD32);
		paths[i]->elem_refs = (XpexElementRef *) 
		Xmalloc((unsigned) paths[i]->num_refs * sizeof(XpexElementRef));
		for (j = 0; j < paths[i]->num_refs; j++) {
			pexElementRef *ref = (pexElementRef *) list;
			paths[i]->elem_refs[j].structure = (XID) ref->structure;
			paths[i]->elem_refs[j].offset = ref->offset;
			list += sizeof(pexElementRef);
		}
	}
	return status;
}

Status
XpexGetDescendants(dpy, s_id, path_order, path_depth, paths, num_paths) 
	Display *dpy; 
	XpexStructure  s_id;
	int  path_order;
	int  path_depth;
	XpexListOfElementRef  *paths[]; 	/* RETURN */
	int  *num_paths; /* RETURN */
{
	register XExtDisplayInfo *info = XpexFindDisplay(dpy);
	register i, j;
	register pexGetDescendantsReq *req;
	pexGetDescendantsReply reply;
	Status status = 0;
	unsigned size;
	char *list;

	XpexCheckExtension(dpy,info,0);
	LockDisplay(dpy);
	XpexGetReq(GetDescendants, dpy, info, req);
	req->sid = s_id;
	req->pathOrder = (CARD16) path_order;
	req->pathDepth = (CARD16) path_depth;
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*num_paths = (int) reply.numPaths;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (list = Xmalloc(size)) {
			_XRead(dpy, (char *) list, (long)size);
		} else {
			status = 0;
			_XpexClearReply(dpy, reply.length);
		}
	}	
	UnlockDisplay(dpy);
	SyncHandle();

	/* convert stream to paths */ 
	for (i = 0; i < *num_paths; i++) {
		CARD32 *c32 = (CARD32 *) list;
		paths[i]->num_refs = (int) *c32;
		list += sizeof(CARD32);
		paths[i]->elem_refs = (XpexElementRef *) 
		Xmalloc((unsigned) paths[i]->num_refs * sizeof(XpexElementRef));
		for (j = 0; j < paths[i]->num_refs; j++) {
			pexElementRef *ref = (pexElementRef *) list;
			paths[i]->elem_refs[j].structure = (XID) ref->structure;
			paths[i]->elem_refs[j].offset = ref->offset;
			list += sizeof(pexElementRef);
		}
	}
	return status;
}

Status
XpexFetchElements( dpy, s_id, range, elements, count)
	Display  *dpy;
	XpexStructure  s_id;
	XpexElementRange  *range;
	char **elements;  /* RETURN */
	int  *count;  /* RETURN */
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexFetchElementsReq *req;
	pexFetchElementsReply reply;

	Status status = 1;
	unsigned size;

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReq(FetchElements, dpy, i, req);
	req->sid = s_id;
	req->range.position1.whence = range->position1.whence;
	req->range.position1.offset = range->position1.offset;
	req->range.position2.whence = range->position2.whence;
	req->range.position2.offset = range->position2.offset;
	if (!_XReply(dpy, (xReply *) &reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*elements = NULL;
	*count = (int) reply.numElements;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*elements = Xmalloc(size)) {
			_XRead(dpy, (char *)*elements, (long)size);
		} else {
			status = 0;
			_XpexClearReply(dpy, reply.length);
		}
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return status;
}

/* XpexFetchAllElements is a convenience function */

Status
XpexFetchAllElements(dpy, s_id, elements, count)
	Display  *dpy;
	XpexStructure  s_id;
	char **elements;  /* RETURN */
	int  *count;  /* RETURN */
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexFetchElementsReq *req;
	pexFetchElementsReply reply;

	Status status = 1;
	unsigned size;

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReq(FetchElements, dpy, i, req);
	req->sid = s_id;
	req->range.position1.whence = (CARD16) PEXBeginning;
	req->range.position1.offset = ( INT32) 0;
	req->range.position2.whence = (CARD16) PEXEnd;
	req->range.position2.offset = ( INT32) 0;

	if (!_XReply(dpy, (xReply *) &reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*elements = NULL;
	*count = (int) reply.numElements;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*elements = Xmalloc(size)) {
			_XRead(dpy, (char *)*elements, (long)size);
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
XpexSetEditingMode(dpy, s_id, editing_mode)
    Display  *dpy;
    XpexStructure  s_id;
    int editing_mode;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexSetEditingModeReq *req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(SetEditingMode, dpy, i, req);
	req->sid = s_id;
	req->mode = (CARD16) editing_mode;
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexSetElementPointer(dpy, s_id, position)
	Display *dpy;
	XpexStructure s_id;
	XpexElementPos *position;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexSetElementPointerReq *req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(SetElementPointer, dpy, i, req);
	req->sid = s_id;
	req->position.whence = (CARD16) position->whence;
	req->position.offset = (INT32) position->offset;
	UnlockDisplay(dpy);
    SyncHandle();
}

void
XpexSetElementPointerAtLabel(dpy, s_id, label, offset)
	Display *dpy;
	XpexStructure s_id;
	long label, offset;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexSetElementPointerAtLabelReq *req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(SetElementPointerAtLabel, dpy, i, req);
	req->sid = s_id;
	req->label = (INT32) label;
	req->offset = (INT32) offset;
	UnlockDisplay(dpy);
    SyncHandle();
}

Status
XpexElementSearch(dpy, s_id, position, direction, 
    incl_list, excl_list, srch_status, found_offset)
    Display *dpy;
    XpexStructure s_id;
    XpexElementPos *position;
    int direction;
    XpexListOfElementType  *incl_list; 
    XpexListOfElementType  *excl_list;
    int *srch_status;  /* RETURN */
    int *found_offset; /* RETURN */
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexElementSearchReq *req;
	pexElementSearchReply reply;
	char	*list;
	int incl_list_size = incl_list->num_types * sizeof(CARD16);
	int excl_list_size = excl_list->num_types * sizeof(CARD16);
	int extra = incl_list_size + PADDING(incl_list_size) +
		excl_list_size + PADDING(excl_list_size);

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReqExtra(ElementSearch, dpy, i, extra, req);
	req->sid = s_id;
	req->position.whence = (CARD16) position->whence;
	req->position.offset = (INT32) position->offset;
	req->direction = (CARD32) direction;
	req->numIncls = (CARD32) incl_list->num_types;
	req->numExcls = (CARD32) excl_list->num_types;
	list = (char *)(req + 1);
	bcopy((char *)(incl_list->elem_type), list, incl_list_size);
	list += (incl_list_size + PADDING(incl_list_size));
	bcopy((char *)(excl_list->elem_type), list, excl_list_size);

	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*srch_status = (int) reply.status;
	*found_offset = (int) reply.foundOffset;

	UnlockDisplay(dpy);
	SyncHandle();
	return 1;
}

void
XpexStoreElements(dpy, s_id, num_ocs, length, oc_list)
	Display *dpy;
	XpexStructure s_id;
	int num_ocs;
	int length;
	char *oc_list;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexStoreElementsReq *req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReqExtra(StoreElements,dpy,i,length,req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->sid = (pexStructure) s_id;
	req->numCommands = (CARD32) num_ocs;
	bcopy(oc_list,(char *)(req + 1),length);
	UnlockDisplay(dpy);
	SyncHandle(); 
}
/*
 * Assumption:  LockDisplay(dpy) has been called right before.
 */
void
_XpexStoreElements(dpy, s_id)
	Display *dpy;
	XpexStructure s_id;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexStoreElementsReq *req;

	XpexSimpleCheckExtension(dpy,i); /* just in case */

	XpexGetReq(StoreElements,dpy,i,req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->sid = (pexStructure) s_id;
	req->numCommands = (CARD32) 0;
}

void
XpexDeleteElements(dpy, s_id, range)
	Display *dpy;
	XpexStructure s_id;
	XpexElementRange *range;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexDeleteElementsReq *req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(DeleteElements, dpy, i, req);
	req->sid = s_id;
	req->range.position1.whence = range->position1.whence;
	req->range.position1.offset = range->position1.offset;
	req->range.position2.whence = range->position2.whence;
	req->range.position2.offset = range->position2.offset;
	UnlockDisplay(dpy);
	SyncHandle();
}

/* XpexDeleteAllElements is a convenience function */

void
XpexDeleteAllElements(dpy, s_id)
	Display *dpy;
	XpexStructure s_id;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexDeleteElementsReq *req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(DeleteElements, dpy, i, req);
	req->sid = s_id;
	req->range.position1.whence = (CARD16) PEXBeginning;
	req->range.position1.offset = ( INT32) 0;
	req->range.position2.whence = (CARD16) PEXEnd;
	req->range.position2.offset = ( INT32) 0;
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexDeleteElementsToLabel(dpy, s_id, position, label)
	Display *dpy;
	XpexStructure s_id;
	XpexElementPos *position;
	long label;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexDeleteElementsToLabelReq *req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(DeleteElementsToLabel, dpy, i, req);
	req->sid = s_id;
	req->position.whence = (CARD16) position->whence;
	req->position.offset = (INT32) position->offset;
	req->label = (INT32) label;
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexDeleteBetweenLabels(dpy, s_id, label1, label2)
	Display *dpy;
	XpexStructure s_id;
	long  label1, label2; 
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexDeleteBetweenLabelsReq *req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(DeleteBetweenLabels, dpy, i, req);
	req->sid = s_id;
	req->label1 = (INT32) label1;
	req->label2 = (INT32) label2;
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexCopyElements(dpy, src_s_id, src_range, dest_s_id, dest_pos)
	Display *dpy;
	XpexStructure  src_s_id;
	XpexElementRange  *src_range;
	XpexStructure  dest_s_id;
	XpexElementPos  *dest_pos;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexCopyElementsReq *req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(CopyElements, dpy, i, req);
	req->src = src_s_id;
	req->srcRange.position1.whence = src_range->position1.whence;
	req->srcRange.position1.offset = src_range->position1.offset;
	req->srcRange.position2.whence = src_range->position2.whence;
	req->srcRange.position2.offset = src_range->position2.offset;
	req->dst = dest_s_id;
	req->dstPosition.whence = (CARD16) dest_pos->whence;
	req->dstPosition.offset = ( INT32) dest_pos->offset;
	UnlockDisplay(dpy);
	SyncHandle();
}

/*
 * XpexCopyAllElements * is a convenience function.
 */
void
XpexCopyAllElements(dpy, src_s_id, dest_s_id, dest_pos)
	Display *dpy;
	XpexStructure  src_s_id;
	XpexStructure  dest_s_id;
	XpexElementPos  *dest_pos;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
	register pexCopyElementsReq *req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(CopyElements, dpy, i, req);
	req->src = src_s_id;
	req->srcRange.position1.whence = (CARD16) PEXBeginning;
	req->srcRange.position1.offset = ( INT32) 0;
	req->srcRange.position2.whence = (CARD16) PEXEnd;
	req->srcRange.position2.offset = ( INT32) 0;
	req->dst = dest_s_id;
	req->dstPosition.whence = (CARD16) dest_pos->whence;
	req->dstPosition.offset = ( INT32) dest_pos->offset;
	UnlockDisplay(dpy);
	SyncHandle();
}

void 
XpexChangeStructureRefs(dpy, old_s_id, new_s_id)
	Display *dpy;
	XpexStructure old_s_id;
	XpexStructure new_s_id;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);
    register pexChangeStructureRefsReq *req;

	XpexSimpleCheckExtension(dpy,i);
	LockDisplay(dpy);
	XpexGetReq(ChangeStructureRefs, dpy, i, req);
	req->old_id = old_s_id;
	req->new_id = new_s_id;
	UnlockDisplay(dpy);
	SyncHandle();
}
