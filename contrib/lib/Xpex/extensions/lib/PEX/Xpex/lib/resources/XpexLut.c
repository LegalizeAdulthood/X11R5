/* $Header: XpexLut.c,v 2.5 91/09/11 16:07:14 sinyaw Exp $ */
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

XpexLookupTable
XpexCreateLookupTable(dpy, drawable, table_type)
	Display  *dpy;
	Drawable  drawable;
	XpexTableType  table_type;
{
    pexLookupTable lut_id;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	{
		register pexCreateLookupTableReq *req;
		XpexGetReq(CreateLookupTable, dpy, i, req);
		req->drawableExample = drawable;
		lut_id = req->lut = XAllocID(dpy);
		req->tableType = (pexTableType) table_type;
	}
	UnlockDisplay(dpy);

	SyncHandle();

    return lut_id;
}

void
XpexCopyLookupTable(dpy, src_lut_id, dest_lut_id)
	Display  *dpy;
	XpexLookupTable  src_lut_id;
	XpexLookupTable  dest_lut_id;
{  
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);

	LockDisplay(dpy);
	{
		register pexCopyLookupTableReq *req;

		XpexGetReq(CopyLookupTable, dpy, i, req);
		req->src = (pexLookupTable) src_lut_id;
		req->dst = (pexLookupTable) dest_lut_id;
	}
	UnlockDisplay(dpy);

	SyncHandle();
}

void
XpexFreeLookupTable(dpy, lut_id)
	Display  *dpy;
	XpexLookupTable  lut_id;
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);

	LockDisplay(dpy);
	{
		register pexFreeLookupTableReq *req;

		XpexGetResReq(FreeLookupTable, dpy, i, lut_id, req);
	}
	UnlockDisplay(dpy);

	SyncHandle();
}

Status
XpexGetTableInfo(dpy, drawable, table_type, info)
	Display  *dpy;
	Drawable  drawable;
	XpexTableType  table_type;
	XpexTableInfo  *info; /* RETURN */
{
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	{
		register pexGetTableInfoReq *req;
		pexGetTableInfoReply reply;

		XpexGetReq(GetTableInfo, dpy, i, req);
		req->drawableExample = drawable;
		req->tableType = (pexTableType) table_type;

		if (!_XReply(dpy, (xReply *)&reply, 0, xTrue)) {
			UnlockDisplay(dpy);
			SyncHandle();
			return 0;
		}
		info->definableEntries = reply.definableEntries;
		info->numPredefined = reply.numPredefined;
		info->predefinedMin = reply.predefinedMin;
		info->predefinedMax = reply.predefinedMax;
	}	
	UnlockDisplay(dpy);

	SyncHandle();

	return 1;
}

Status 
XpexGetPredefinedEntries(dpy, drawable, table_type, start, count, 
	entries, actual_count)
	Display  *dpy;
	Drawable  drawable;
	XpexTableType  table_type;
	XpexTableIndex  start; 
	int  count;
	char **entries; /* RETURN */
	int  *actual_count;  /* RETURN */
{
	Status status = 1;
	unsigned size;
	register pexGetPredefinedEntriesReq *req;
	pexGetPredefinedEntriesReply reply;

	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReq(GetPredefinedEntries, dpy, i, req);
	req->drawableExample = drawable;
	req->fpFormat = (pexEnumTypeIndex) XpexGetFloatingPointFormat(dpy);
	req->tableType = (pexTableType) table_type;
	req->start = (pexTableIndex) start;
	req->count = (CARD16) count;
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*actual_count = (int) reply.numEntries;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*entries = Xmalloc(size)) {
			_XRead(dpy, *entries, (long)size);
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
XpexGetDefinedIndices(dpy, lut_id, indices, count)
	Display  *dpy;
	XpexLookupTable  lut_id;
	XpexTableIndex **indices; /* RETURN */
	int  *count; /* RETURN  */
{
	Status status = 1;
	unsigned size;
	register pexGetDefinedIndicesReq *req;
	pexGetDefinedIndicesReply reply; 

	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetResReq(GetDefinedIndices, dpy, i, lut_id, req);
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*count = (int) reply.numIndices;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*indices = (XpexTableIndex *) Xmalloc(size)) {
			_XRead(dpy, (char *)*indices, (long)size);
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
XpexGetTableEntry(dpy, lut_id, index, value_type, 
	table_type, entry_status, table_entry)
	Display  *dpy;
	XpexLookupTable  lut_id;
	XpexTableIndex  index;
	int  value_type;
	XpexTableType  *table_type; /* RETURN */
	int  *entry_status; /* RETURN */
	char **table_entry; /* RETURN */
{
	Status status = 1;
	unsigned size;
	register pexGetTableEntryReq *req;
	pexGetTableEntryReply reply;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexCheckExtension(dpy,i,0);

	LockDisplay(dpy);
	XpexGetReq(GetTableEntry, dpy, i, req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->lut = (pexLookupTable) lut_id;
	req->index = (pexTableIndex) index;
	req->valueType = (CARD16) value_type;
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*table_type = (XpexTableType) reply.tableType;
	*entry_status = reply.status;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*table_entry = Xmalloc(size)) {
			_XRead(dpy, (char *)*table_entry, (long) size);
		}
	} else {
		status = 0;
		_XpexClearReply(dpy, reply.length);
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return status;
}

Status
XpexGetTableEntries(dpy, lut_id, start, count, value_type, 
	table_type, entries, actual_count) 
	Display  *dpy;
	XpexLookupTable  lut_id;
	XpexTableIndex  start;
	int  count;
	int  value_type;
	XpexTableType  *table_type; /* RETURN */
	char **entries; /* RETURN */
	int  *actual_count; /* RETURN */
{
	Status status = 1;
	unsigned size;
	register pexGetTableEntriesReq *req;
	pexGetTableEntriesReply reply;

	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexCheckExtension(dpy,i,0);
	LockDisplay(dpy);
	XpexGetReq(GetTableEntries, dpy, i, req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->start = (pexTableIndex) start;
	req->count = (CARD16) count;
	req->valueType = (CARD16) value_type;
	if (!_XReply(dpy, (xReply *)&reply, 0, xFalse)) {
		UnlockDisplay(dpy);
		SyncHandle();
		return 0;
	}
	*table_type = (XpexTableType) reply.tableType;
	*actual_count = (int) reply.numEntries;
	if ((size = reply.length * sizeof(CARD32)) > 0) {
		if (*entries = Xmalloc(size)) {
			_XRead(dpy, (char *)*entries, (long)size);
		}
	} else {
		status = 0;
		_XpexClearReply(dpy, reply.length);
	}
	UnlockDisplay(dpy);
	SyncHandle();

	return status;
}

void
XpexSetTableEntries(dpy, lut_id, table_type, start, count, 
	entries, length)
	Display  *dpy;
	XpexLookupTable  lut_id;
	XpexTableType table_type;
	XpexTableIndex  start;
	int  count;
	char *entries;
	int length;
{
	register pexSetTableEntriesReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);

	LockDisplay(dpy);
	XpexGetReqExtra(SetTableEntries,dpy,i,length,req);
	req->fpFormat = XpexGetFloatingPointFormat(dpy);
	req->lut = (pexLookupTable) lut_id;
	req->start = (pexTableIndex) start;
	req->count = (CARD16) count;
	bcopy((char *) entries, (char *)(req + 1), (unsigned) length);
	UnlockDisplay(dpy);
	SyncHandle();
}

void
XpexDeleteTableEntries(dpy, lut_id, start, count)
	Display  *dpy;
	XpexLookupTable  lut_id;
	XpexTableIndex  start;
	int  count;
{
	register pexDeleteTableEntriesReq *req;
	register XExtDisplayInfo *i = XpexFindDisplay(dpy);

	XpexSimpleCheckExtension(dpy,i);

	LockDisplay(dpy);
	XpexGetReq(DeleteTableEntries, dpy, i, req);
	req->lut = (pexLookupTable) lut_id;
	req->start = (pexTableIndex) start;
	req->count = (CARD16) count;
	UnlockDisplay(dpy);
	SyncHandle();
}
