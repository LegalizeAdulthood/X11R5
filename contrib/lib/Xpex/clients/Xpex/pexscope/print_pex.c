/* $Header: print_pex.c,v 2.5 91/09/11 15:52:31 sinyaw Exp $ */
#ifndef lint
static char sccsid[] = "@(#)print_pex.c 1.5 90/04/05 Copyright SMI/MIT";
#endif

/***********************************************************
Copyright 1989 by Sun Microsystems, Inc. and the X Consortium.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include <stdio.h>
#include <X11/X.h>
#include "Xpexlib.h"
#include "pexscope.h"
#include "PEX_names.h"

#define PrintUval(_STR,_UVAL) \
(void) fprintf( stderr, "_STR: %u\n",_UVAL)

#define PrintVal(_STR,_VAL) \
(void) fprintf( stderr, "_STR: %d\n",_VAL)

#define PrintBitmask(_STR,_VALMASK) \
(void) fprintf( stderr, "_STR: 0x%x\n",_VALMASK)

#define PrintString(_STR,_DATA) \
(void) fprintf( stderr, "_STR: %s\n",_DATA)

#define PEX_DECODE(_REQNAME) \
_REQNAME(buf) \
register unsigned char *buf;

#if defined(__STDC__) && !defined(UNIXCPP)
#define CAPTURE_REQUEST(NAME,_pRequest,_pStream) \
pex##NAME##Req *_pRequest = (pex##NAME##Req *) _pStream
#else
#define CAPTURE_REQUEST(NAME,_pRequest,_pStream) \
pex/**/NAME/**/Req *_pRequest = (pex/**/NAME/**/Req *) _pStream
#endif

#if defined(__STDC__) && !defined(UNIXCPP)
#define CAPTURE_REPLY(NAME,_pReply,_pStream) \
pex##NAME##Reply *_pReply = (pex##NAME##Reply *) _pStream
#else
#define CAPTURE_REPLY(NAME,_pReply,_pStream) \
pex/**/NAME/**/Reply *_pReply = (pex/**/NAME/**/Reply *) _pStream
#endif

#if defined(__STDC__) && !defined(UNIXCPP)
#define CAPTURE_ERROR(NAME,_pError,_pStream) \
pex##NAME##Error *_pError = (pex##NAME##Error *) _pStream
#else
#define CAPTURE_ERROR(NAME,_pError,_pStream) \
pex/**/NAME/**/Error *_pError = (pex/**/NAME/**/Error *) _pStream
#endif

extern char pex_last_req;

PEX_DECODE(Nothing)
{
}

PEX_DECODE(GetExtensionInfo)
{
	CAPTURE_REQUEST(GetExtensionInfo,req,buf);

	PRINT("PEX request: %s\n", NameOfRequest(req->opcode));
	PRINT("request length : %u\n", req->length);
	PRINT("clientProtocolMajor: %u\n", req->clientProtocolMajor);
	PRINT("clientProtocolMinor: %u\n", req->clientProtocolMinor);
}

PEX_DECODE(GetExtensionInfoReply)
{
	CAPTURE_REPLY(GetExtensionInfo,reply,buf);

	PRINT("PEX reply: %s\n", NameOfRequest(pex_last_req));
	PRINT("reply length : %u\n", reply->length);
	PRINT("majorVersion: %u\n", reply->majorVersion);
	PRINT("minorVersion: %u\n", reply->minorVersion);
	PRINT("release: %u\n", reply->release);
	PRINT("lengthName: %u\n", reply->lengthName);
	PRINT("subsetInfo: %u\n", reply->subsetInfo);
}

PEX_DECODE(GetEnumeratedTypeInfo)
{
	CAPTURE_REQUEST(GetEnumeratedTypeInfo,req,buf);

	PEX_DECODE_REQUEST(GetEnumeratedTypeInfo,buf);

	pexpr_card("drawable",req->drawable);
	pexpr_card("itemMask",req->itemMask);
	pexpr_card("numEnums",req->numEnums);
}

PEX_DECODE(GetEnumeratedTypeInfoReply)
{
	CAPTURE_REPLY(GetEnumeratedTypeInfo,reply,buf);

    PEX_DECODE_REPLY(GetEnumeratedTypeInfo,buf);

	pexpr_card("numLists",reply->numLists);
}

PEX_DECODE(GetImpDepConstants)
{
	CAPTURE_REQUEST(GetImpDepConstants,req,buf);

	PEX_DECODE_REQUEST(GetImpDepConstants,buf);

	PRINT("drawable: %u\n",req->drawable);
	PRINT("fpFormat: %d\n",req->fpFormat);
	PRINT("numNames: %u\n",req->numNames);
}

PEX_DECODE(GetImpDepConstantsReply)
{
	CAPTURE_REPLY(GetImpDepConstants,reply,buf);

	PEX_DECODE_REPLY(GetImpDepConstants,buf);
}

PEX_DECODE(CreateLookupTable)
{
	CAPTURE_REQUEST(CreateLookupTable,req,buf);
    
    PEX_DECODE_REQUEST(CreateLookupTable,buf);

	PRINT("drawableExample: %u\n",req->drawableExample);
	PRINT("lut: %u\n",req->lut);
	PRINT("tableType: %s\n", NameOfTableType(req->tableType));	
}

PEX_DECODE(CopyLookupTable)
{
	CAPTURE_REQUEST(CopyLookupTable,req,buf); 

    PEX_DECODE_REQUEST(CopyLookupTable,buf);

	PRINT("src: %u\n",req->src);
	PRINT("dst: %u\n",req->dst);
}

PEX_DECODE(FreeLookupTable)
{
	CAPTURE_REQUEST(FreeLookupTable,req,buf);

    PEX_DECODE_REQUEST(FreeLookupTable,buf);

	PRINT("id: %u\n",req->id);
}

PEX_DECODE(GetTableInfo)
{
	CAPTURE_REQUEST(GetTableInfo,req,buf);
    
	PRINT("drawableExample: %u\n",req->drawableExample);
	PRINT("tableType: %s\n",NameOfTableType(req->tableType));
}

PEX_DECODE(GetTableInfoReply)
{
	CAPTURE_REPLY(GetTableInfo,reply,buf);

    PEX_DECODE_REPLY(GetTableInfo,buf);

	PRINT("definableEntries: %u\n",reply->definableEntries);
	PRINT("numPredefined: %u\n",reply->numPredefined);
	PRINT("predefinedMin: %u\n",reply->predefinedMin);
	PRINT("predefinedMax: %u\n",reply->predefinedMax);
}

PEX_DECODE(GetPredefinedEntries)
{	
	CAPTURE_REQUEST(GetPredefinedEntries,req,buf);

    PEX_DECODE_REQUEST(GetPredefinedEntries,buf);

	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(req->fpFormat));
	PRINT("drawableExample: %u\n",req->drawableExample);
	PRINT("tableType: %s\n",NameOfTableType(req->tableType)); 
	PRINT("start: %u\n",req->start);
	PRINT("count: %u\n",req->count);
}

PEX_DECODE(GetPredefinedEntriesReply)
{
	CAPTURE_REPLY(GetPredefinedEntries,p,buf);
		
    PEX_DECODE_REPLY(GetPredefinedEntries,buf);

	PRINT("numEntries: %u\n", p->numEntries); 
}

PEX_DECODE(GetDefinedIndices)
{
	CAPTURE_REQUEST(GetDefinedIndices,req,buf);

    PEX_DECODE_REQUEST(GetDefinedIndices,buf);

	PRINT("id: %u\n",req->id);
}

PEX_DECODE(GetDefinedIndicesReply)
{
	CAPTURE_REPLY(GetDefinedIndices,p,buf);

    PEX_DECODE_REPLY(GetDefinedIndices,buf);

	PRINT("numIndices: %u\n", p->numIndices);
}

PEX_DECODE(GetTableEntry)
{
	CAPTURE_REQUEST(GetTableEntry,req,buf);
	
    PEX_DECODE_REQUEST(GetTableEntry,buf);

	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(req->fpFormat));
	PRINT("valueType: %u\n",req->valueType);
	PRINT("lut: %u\n",req->lut);
	PRINT("index: %u\n",req->index);
}

PEX_DECODE(GetTableEntryReply)
{
	CAPTURE_REPLY(GetTableEntry,req,buf);

    PEX_DECODE_REPLY(GetTableEntry,buf);

	PRINT("status: %u\n",req->status);
	
	PRINT("tableType: %s\n",NameOfTableType(req->tableType)); 
}

PEX_DECODE(GetTableEntries)
{
	CAPTURE_REQUEST(GetTableEntries,p,buf);
	
	PEX_DECODE_REQUEST(GetTableEntries,buf);
}

PEX_DECODE(GetTableEntriesReply)
{
	CAPTURE_REPLY(GetTableEntries,p,buf);
	
	PEX_DECODE_REPLY(GetTableEntries,buf);
	
	PRINT("tableType: %s\n",NameOfTableType(p->tableType));

	PRINT("numEntries: %u\n",p->numEntries);
}

PEX_DECODE(SetTableEntries)
{
	CAPTURE_REQUEST(SetTableEntries,req,buf);
    
    PEX_DECODE_REQUEST(SetTableEntries,buf);

	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(req->fpFormat));
	PRINT("lut: %u\n",req->lut);
	PRINT("start: %u\n",req->start);
	PRINT("count: %u\n",req->count);
}

PEX_DECODE(DeleteTableEntries)
{
	CAPTURE_REQUEST(DeleteTableEntries,req,buf);

	PEX_DECODE_REQUEST(DeleteTableEntries,buf);

	PRINT("lut: %u\n",req->lut);
	PRINT("start: %u\n",req->start);
	PRINT("count: %u\n",req->count);
}

PEX_DECODE(CreatePipelineContext)
{
	CAPTURE_REQUEST(CreatePipelineContext,req,buf);
		
	PRINT("PEX request: %s\n",NameOfRequest(req->opcode));
	PRINT("request length: %u\n",req->length);
	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(req->fpFormat));
	PRINT("pc: %u\n",req->pc);
	PRINT("itemMask[0]: 0x%x\n",req->itemMask[0]);
	PRINT("itemMask[1]: 0x%x\n",req->itemMask[1]);
	PRINT("itemMask[2]: 0x%x\n",req->itemMask[2]);
}

PEX_DECODE(CopyPipelineContext)
{
	CAPTURE_REQUEST(CopyPipelineContext,req,buf);

	PRINT("PEX request: %s\n",NameOfRequest(req->opcode));
	PRINT("request length: %u\n",req->length);
	PRINT("src: %u\n",req->src);
	PRINT("dst: %u\n",req->dst);
	PRINT("itemMask[0]: 0x%x\n",req->itemMask[0]);
	PRINT("itemMask[1]: 0x%x\n",req->itemMask[1]);
	PRINT("itemMask[2]: 0x%x\n",req->itemMask[2]);
}

PEX_DECODE(FreePipelineContext)
{
	CAPTURE_REQUEST(FreePipelineContext,req,buf);

	PRINT("PEX request: %s\n",NameOfRequest(req->opcode));
	PRINT("request length: %u\n",req->length);
	PRINT("id: %u\n",req->id);
}

PEX_DECODE(GetPipelineContext)
{
	CAPTURE_REQUEST(GetPipelineContext,req,buf);
	
	PRINT("PEX request: %s\n",NameOfRequest(req->opcode));
	PRINT("request length: %u\n",req->length);
	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(req->fpFormat));
	PRINT("pc: %u\n",req->pc);
	PRINT("itemMask[0]: 0x%x\n",req->itemMask[0]);
	PRINT("itemMask[1]: 0x%x\n",req->itemMask[1]);
	PRINT("itemMask[2]: 0x%x\n",req->itemMask[2]);
}

PEX_DECODE(GetPipelineContextReply)
{
	PEX_DECODE_REPLY(GetPipelineContext,buf);
}

PEX_DECODE(ChangePipelineContext)
{
	CAPTURE_REQUEST(ChangePipelineContext,p,buf);

    PEX_DECODE_REQUEST(ChangePipelineContext,buf);

	PRINT("fpFormat: %d\n",p->fpFormat);
	PRINT("pc: %u\n",p->pc);
	PRINT("itemMask[0]: 0x%x\n",p->itemMask[0]);
	PRINT("itemMask[1]: 0x%x\n",p->itemMask[1]);
	PRINT("itemMask[2]: 0x%x\n",p->itemMask[2]);
}

PEX_DECODE(CreateRenderer)
{
	CAPTURE_REQUEST(CreateRenderer,req,buf);
	
    PEX_DECODE_REQUEST(CreateRenderer,buf);

	PRINT("fpFormat: %s\n", NameOfFloatingPointFormat(req->fpFormat));
	PRINT("rdr: %u\n",req->rdr);
	PRINT("drawable: %u\n",req->drawable);
	PRINT("itemMask: 0x%x\n",req->itemMask);
}

PEX_DECODE(FreeRenderer)
{
	CAPTURE_REQUEST(FreeRenderer,req,buf);

    PEX_DECODE_REQUEST(FreeRenderer,buf);

	PRINT("id: %u\n",req->id);
}

PEX_DECODE(ChangeRenderer)
{
	CAPTURE_REQUEST(ChangeRenderer,p,buf);
	
    PEX_DECODE_REQUEST(ChangeRenderer,buf);

	PrintVal(fpFormat,p->fpFormat);
	PrintUval(rdr,p->rdr);
	PrintBitmask(itemMask,p->itemMask);
}

PEX_DECODE(GetRendererAttributes)
{
	CAPTURE_REQUEST(GetRendererAttributes,p,buf);
		
    PEX_DECODE_REQUEST(GetRendererAttributes,buf);

	PrintVal(fpFormat,p->fpFormat);
	PrintUval(rdr,p->rdr);
	PrintBitmask(itemMask,p->itemMask);
}

PEX_DECODE(GetRendererAttributesReply)
{
    PEX_DECODE_REPLY(GetRendererAttributes,buf);
}

PEX_DECODE(GetRendererDynamics)
{
    PEX_DECODE_REQUEST(GetRendererDynamics,buf);
	PrintUval(id,((pexResourceReq *) buf)->id);
}

PEX_DECODE(GetRendererDynamicsReply)
{
	CAPTURE_REPLY(GetRendererDynamics,p,buf);
		
    PEX_DECODE_REPLY(GetRendererDynamics,buf);

	PrintBitmask(table,p->tables);
	PrintBitmask(namesets,p->namesets);
	PrintBitmask(attributes,p->attributes);
}

PEX_DECODE(BeginRendering)
{
	CAPTURE_REQUEST(BeginRendering,p,buf);
	
    PEX_DECODE_REQUEST(BeginRendering,buf);

	PrintUval(rdr,p->rdr);
	PrintUval(drawable,p->drawable);
}

PEX_DECODE(EndRendering)
{
	CAPTURE_REQUEST(EndRendering,p,buf);
	
    PEX_DECODE_REQUEST(EndRendering,buf);

	PRINT("rdr: %u\n", p->rdr);
	PrintUval(flushFlag,p->flushFlag);
}

PEX_DECODE(BeginStructure)
{
	CAPTURE_REQUEST(BeginStructure,req,buf);
	
	PEX_DECODE_REQUEST(BeginStructure,buf);

	PRINT("rdr: %u\n",req->rdr);
	PRINT("sid: %u\n",req->sid);
}

PEX_DECODE(EndStructure)
{
	CAPTURE_REQUEST(EndStructure,req,buf);

	PEX_DECODE_REQUEST(EndStructure,buf);

	PRINT("id: %u\n",req->id);
}

PEX_DECODE(RenderOutputCommands)
{
	CAPTURE_REQUEST(RenderOutputCommands,p,buf);
    
    CARD32 num_cmds = p->numCommands;

    PEX_DECODE_REQUEST(RenderOutputCommands,buf);
	PrintVal(fpFormat,p->fpFormat);
	PrintUval(rdr,p->rdr);
	PrintUval(numCommands,p->numCommands);

    buf += sizeof(pexRenderOutputCommandsReq);
    pex_decode_elem(num_cmds, (CARD32 *)(buf));
}

PEX_DECODE(RenderNetwork)
{
	CAPTURE_REQUEST(RenderNetwork,p,buf);

    PEX_DECODE_REQUEST(RenderNetwork,buf);

	PRINT("rdr: %u\n",p->rdr);
	PRINT("drawable: %u\n",p->drawable);
	PRINT("sid: %u\n",p->sid);
}

PEX_DECODE(CreateStructure)
{
	CAPTURE_REQUEST(CreateStructure,p,buf);

    PEX_DECODE_REQUEST(CreateStructure,buf);

	PRINT("id: %u\n",p->id);
}

PEX_DECODE(CopyStructure)
{
	CAPTURE_REQUEST(CopyStructure,req,buf);

    PEX_DECODE_REQUEST(CopyStructure,buf);

	PRINT("src: %u\n",req->src);
	PRINT("dst: %u\n",req->dst);
}


PEX_DECODE(DestroyStructures)
{
	CAPTURE_REQUEST(DestroyStructures,req,buf);

	PEX_DECODE_REQUEST(DestroyStructures,buf);

	PRINT("numStructures: %u\n",req->numStructures);
}

PEX_DECODE(GetStructureInfo)
{
	CAPTURE_REQUEST(GetStructureInfo,p,buf);
	
	PEX_DECODE_REQUEST(GetStructureInfo,buf);

	PrintVal(fpFormat,p->fpFormat);
	PrintBitmask(itemMask,p->itemMask);
	PrintUval(sid,p->sid);
}

PEX_DECODE(GetStructureInfoReply)
{
	CAPTURE_REPLY(GetStructureInfo,p,buf);

    PEX_DECODE_REPLY(GetStructureInfo,buf);

	pexpr_editing_mode(p->editMode);
	/* PrintUval(editMode,p->editMode); */
	PrintUval(elementPtr,p->elementPtr);
	PrintUval(numElements,p->numElements);
	PrintUval(lengthStructure,p->lengthStructure);
	PrintUval(hasRefs,p->hasRefs);
}

PEX_DECODE(GetElementInfo)
{
	CAPTURE_REQUEST(GetElementInfo,p,buf);
	
    PEX_DECODE_REQUEST(GetElementInfo,buf);

	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(p->fpFormat));
	PrintUval(sid,p->sid);
	pexpr_whence("range.position1.whence",p->range.position1.whence);
	PrintVal(range.position1.offset,p->range.position1.offset);
	pexpr_whence("range.position2.whence",p->range.position2.whence);
	PrintVal(range.position2.offset,p->range.position2.offset);
}

PEX_DECODE(GetElementInfoReply)
{
	CAPTURE_REPLY(GetElementInfo,p,buf);
		
	PEX_DECODE_REPLY(GetElementInfo,buf);
	PrintUval(numInfo,p->numInfo);
}

PEX_DECODE(GetStructuresInNetwork)
{
	CAPTURE_REQUEST(GetStructuresInNetwork,p,buf);
		
    PEX_DECODE_REQUEST(GetStructuresInNetwork,buf);

	PRINT("sid: %u\n", p->sid);

	PrintUval(which,p->which);
}

PEX_DECODE(GetStructuresInNetworkReply)
{
	CAPTURE_REPLY(GetStructuresInNetwork,reply,buf);
		
    PEX_DECODE_REPLY(GetStructuresInNetwork,buf);

	PRINT("numStructures: %u\n",reply->numStructures);
}

PEX_DECODE(GetAncestors)
{
	CAPTURE_REQUEST(GetAncestors,req,buf);
	
    PEX_DECODE_REQUEST(GetAncestors,buf);

	PRINT("sid: %u\n",req->sid);
	PRINT("pathOrder: %u\n",req->pathOrder);
	PRINT("pathDepth: %u\n",req->pathDepth);
}

PEX_DECODE(GetAncestorsReply)
{
	CAPTURE_REPLY(GetAncestors,reply,buf);

    PEX_DECODE_REPLY(GetAncestors,buf);

	PRINT("numPaths: %u\n",reply->numPaths);
}

PEX_DECODE(GetDescendants)
{
	CAPTURE_REQUEST(GetDescendants,req,buf); 

    PEX_DECODE_REQUEST(GetDescendants,buf);

	PRINT("sid: %u\n",req->sid);
	PRINT("pathOrder: %u\n",req->pathOrder);
	PRINT("pathDepth: %u\n",req->pathDepth);
}

PEX_DECODE(GetDescendantsReply)
{
	CAPTURE_REPLY(GetDescendants,reply,buf); 
	
    PEX_DECODE_REPLY(GetDescendants,buf);

	PRINT("numPaths: %u\n",reply->numPaths);
}

PEX_DECODE(FetchElements)
{
	CAPTURE_REQUEST(FetchElements,req,buf);
	
	PEX_DECODE_REQUEST(FetchElements,buf);

	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(req->fpFormat));
	PRINT("sid: %u\n",req->sid);
	pexpr_whence("position1.whence",req->range.position1.whence);
	PrintVal(postion1.offset,req->range.position1.offset);
	pexpr_whence("position2.whence",req->range.position2.whence);
	PrintVal(position2.offset,req->range.position2.offset);
}

PEX_DECODE(FetchElementsReply)
{
	CAPTURE_REPLY(FetchElements,reply,buf);
	
    PEX_DECODE_REPLY(FetchElements,buf);

	PRINT("numElements: %u\n",reply->numElements);
}

PEX_DECODE(SetEditingMode)
{
	CAPTURE_REQUEST(SetEditingMode,req,buf);
    
    PEX_DECODE_REQUEST(SetEditingMode,buf);
	PRINT("sid: %u\n",req->sid);
	pexpr_editing_mode(req->mode);
}
	
PEX_DECODE(SetElementPointer)
{
	CAPTURE_REQUEST(SetElementPointer,req,buf);
    
    PEX_DECODE_REQUEST(SetElementPointer,buf);
	PRINT("sid: %u\n",req->sid);
	pexpr_whence("position.whence",req->position.whence);
	PRINT("position.offset: %ld\n",req->position.offset);
}

PEX_DECODE(SetElementPointerAtLabel)
{
	CAPTURE_REQUEST(SetElementPointerAtLabel,req,buf);
		
    PEX_DECODE_REQUEST(SetElementPointerAtLabel,buf);

	PRINT("sid: %u\n",req->sid);
	PRINT("label: %d\n",req->label);
	PRINT("offset: %d\n",req->offset);
}

PEX_DECODE(ElementSearch)
{
	CAPTURE_REQUEST(ElementSearch,req,buf);
	
    PEX_DECODE_REQUEST(ElementSearch,buf);

	PRINT("sid: %u\n",req->sid);
	pexpr_whence("position.whence",req->position.whence);
	PrintVal(position.offset,req->position.offset);
	PrintUval(direction,req->direction);
	PrintUval(numIncls,req->numIncls);
	PrintUval(numExcls,req->numExcls);
}

PEX_DECODE(ElementSearchReply)
{
	CAPTURE_REPLY(ElementSearch,reply,buf);

    PEX_DECODE_REPLY(ElementSearch,buf);

	PRINT("status: %u\n",reply->status);
	PRINT("foundOffset: %u\n",reply->foundOffset);
}

PEX_DECODE(StoreElements)
{
	CAPTURE_REQUEST(StoreElements,req,buf);
    
    CARD32 num_cmds = req->numCommands;
    PEX_DECODE_REQUEST(StoreElements,buf);
	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(req->fpFormat));
	PRINT("sid: %u\n",req->sid);
	PRINT("numCommands: %u\n",req->numCommands);
	req++;
    pex_decode_elem(num_cmds, (CARD32 *)(req));
}

PEX_DECODE(DeleteElements)
{
	CAPTURE_REQUEST(DeleteElements,req,buf);

    PEX_DECODE_REQUEST(DeleteElements,buf);
	PRINT("sid: %u\n",req->sid);
	pexpr_whence("range.position1.whence",req->range.position1.whence);
	PrintVal(range.position1.offset,req->range.position1.offset);
	pexpr_whence("range.position2.whence",req->range.position2.whence);
	PrintVal(range.position2.offset,req->range.position2.offset);
}

PEX_DECODE(DeleteElementsToLabel)
{
	CAPTURE_REQUEST(DeleteElementsToLabel,req,buf);
		
    PEX_DECODE_REQUEST(DeleteElementsToLabel,buf);

	PRINT("sid: %u\n",req->sid);
	PRINT("position.whence: %u\n",req->position.whence);
	PRINT("position.offset: %ld\n",req->position.offset);
	PRINT("label: %ld\n",req->label);
}

PEX_DECODE(DeleteBetweenLabels)
{
	CAPTURE_REQUEST(DeleteBetweenLabels,req,buf);
		
    PEX_DECODE_REQUEST(DeleteBetweenLabels,buf);

	PRINT("sid: %u\n",req->sid);
	PRINT("label1: %ld\n",req->label1);
	PRINT("label2: %ld\n",req->label2);
}

PEX_DECODE(CopyElements)
{
	CAPTURE_REQUEST(CopyElements,req,buf);
	
    PEX_DECODE_REQUEST(CopyElements,buf);

	PRINT("src: %u\n",req->src);
	pexpr_whence("srcRange.position1.whence", 
		req->srcRange.position1.whence);
	PRINT("srcRange.position1.offset: %ld\n",req->srcRange.position1.offset);
	pexpr_whence("srcRange.position2.whence", 
		req->srcRange.position2.whence);
	PRINT("srcRange.position2.offset: %ld\n",req->srcRange.position2.offset);
	PRINT("dst: %u\n",req->dst);
	pexpr_whence("dstPosition.whence", 
		req->dstPosition.whence);
	PRINT("dstPosition.offset: %ld\n",req->dstPosition.offset);
}

PEX_DECODE(ChangeStructureRefs)
{
	CAPTURE_REQUEST(ChangeStructureRefs,req,buf);

    PEX_DECODE_REQUEST(ChangeStructureRefs,buf);

	PRINT("old: %u\n",req->old_id);
	PRINT("new: %u\n",req->new_id);
}

PEX_DECODE(CreateNameSet)
{
	CAPTURE_REQUEST(CreateNameSet,req,buf);

    PEX_DECODE_REQUEST(CreateNameSet,buf);

	PRINT("id: %u\n",req->id);
}

PEX_DECODE(CopyNameSet)
{
	CAPTURE_REQUEST(CopyNameSet,req,buf);

    PEX_DECODE_REQUEST(CopyNameSet,buf);

	PRINT("src: %u\n",req->src);
	PRINT("dst: %u\n",req->dst);
}

PEX_DECODE(FreeNameSet)
{
	CAPTURE_REQUEST(FreeNameSet,req,buf);

    PEX_DECODE_REQUEST(FreeNameSet,buf);

	PRINT("id: %u\n",req->id);
}

PEX_DECODE(GetNameSet)
{
	CAPTURE_REQUEST(GetNameSet,req,buf);

    PEX_DECODE_REQUEST(GetNameSet,buf);

	PRINT("id: %u\n",req->id);
}

PEX_DECODE(GetNameSetReply)
{
	CAPTURE_REPLY(GetNameSet,req,buf);

    PEX_DECODE_REPLY(GetNameSet,buf);

	PRINT("numNames: %u\n",req->numNames);
}

PEX_DECODE(ChangeNameSet)
{
	CAPTURE_REQUEST(ChangeNameSet,req,buf);

    PEX_DECODE_REQUEST(ChangeNameSet,buf);

	PRINT("ns: %u\n",req->ns);
	PRINT("action: %u\n",req->action);
}

PEX_DECODE(CreateSearchContext)
{
	CAPTURE_REQUEST(CreateSearchContext,req,buf);
	
    PEX_DECODE_REQUEST(CreateSearchContext,buf);

	PRINT("fpFormat: %d\n",req->fpFormat);
	PRINT("sc: %u\n",req->sc);
	PRINT("itemMask: 0x%x\n",req->itemMask);
}

PEX_DECODE(CopySearchContext)
{
	CAPTURE_REQUEST(CopySearchContext,req,buf);
	
    PEX_DECODE_REQUEST(CopySearchContext,buf);

	PRINT("src: %u\n",req->src);
	PRINT("dst: %u\n",req->dst);
	PRINT("itemMask: 0x%x\n",req->itemMask);
}

PEX_DECODE(FreeSearchContext)
{
	CAPTURE_REQUEST(FreeSearchContext,req,buf);
    
	PRINT("id: %u\n",req->id);
}

PEX_DECODE(GetSearchContext)
{
	CAPTURE_REQUEST(GetSearchContext,req,buf);
	
    PEX_DECODE_REQUEST(GetSearchContext,buf);

	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(req->fpFormat));
	PRINT("sc: %u\n",req->sc);
	PRINT("itemMask: 0x%x\n",req->itemMask);
}

PEX_DECODE(GetSearchContextReply)
{
	CAPTURE_REPLY(GetSearchContext,reply,buf);

    PEX_DECODE_REPLY(GetSearchContext,buf);
}

PEX_DECODE(ChangeSearchContext)
{
	CAPTURE_REQUEST(ChangeSearchContext,req,buf);
	
    PEX_DECODE_REQUEST(ChangeSearchContext,buf);

	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(req->fpFormat));
	PRINT("sc: %u\n",req->sc);
	PRINT("itemMask: 0x%x\n",req->itemMask);
}

PEX_DECODE(SearchNetwork)
{
	CAPTURE_REQUEST(SearchNetwork,req,buf);

    PEX_DECODE_REQUEST(SearchNetwork,buf);

	PRINT("id: %u\n",req->id);
}

PEX_DECODE(SearchNetworkReply)
{
	CAPTURE_REPLY(SearchNetwork,reply,buf);

	PEX_DECODE_REPLY(SearchNetwork,buf);

	PRINT("numItems: %u\n",reply->numItems);
}

PEX_DECODE(CreatePhigsWks)
{
	CAPTURE_REQUEST(CreatePhigsWks,req,buf);
    
    PEX_DECODE_REQUEST(CreatePhigsWks,buf);

	PRINT("wks: %u\n",req->wks);
	PRINT("drawable: %u\n",req->drawable);
	PRINT("markerBundle: %u\n",req->markerBundle);
	PRINT("textBundle: %u\n",req->textBundle);
	PRINT("lineBundle: %u\n",req->lineBundle);
	PRINT("interiorBundle: %u\n",req->interiorBundle);
	PRINT("edgeBundle: %u\n",req->edgeBundle);
	PRINT("colourTable: %u\n",req->colourTable);
	PRINT("depthCueTable: %u\n",req->depthCueTable);
	PRINT("lightTable: %u\n",req->lightTable);
	PRINT("colourApproxTable: %u\n",req->colourApproxTable);
	PRINT("patternTable: %u\n",req->patternTable);
	PRINT("textFontTable: %u\n",req->textFontTable);
	PRINT("highlightIncl: %u\n",req->highlightIncl);
	PRINT("highlightExcl: %u\n",req->highlightExcl);
	PRINT("invisIncl: %u\n",req->invisIncl);
	PRINT("invisExcl: %u\n",req->invisExcl);
	pexpr_buffer_mode(req->bufferMode);
}

PEX_DECODE(FreePhigsWks)
{
	CAPTURE_REQUEST(FreePhigsWks,req,buf);

    PEX_DECODE_REQUEST(FreePhigsWks,buf);

	PRINT("id: %u\n",req->id);
}

PEX_DECODE(GetWksInfo)
{
	CAPTURE_REQUEST(GetWksInfo,req,buf);
	
    PEX_DECODE_REQUEST(GetWksInfo,buf);

	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(req->fpFormat));
	PRINT("wks: %u\n",req->wks);
	PRINT("itemMask[0]: 0x%x\n",req->itemMask[0]);
	PRINT("itemMask[1]: 0x%x\n",req->itemMask[1]);
}

PEX_DECODE(GetWksInfoReply)
{
	CAPTURE_REPLY(GetWksInfo,reply,buf);

    PEX_DECODE_REPLY(GetWksInfo,buf);
}

PEX_DECODE(GetDynamics)
{
	CAPTURE_REQUEST(GetDynamics,req,buf);

    PEX_DECODE_REQUEST(GetDynamics,buf);

	PRINT("drawable: %u\n",req->drawable);
}

PEX_DECODE(GetDynamicsReply)
{
	pexGetDynamicsReply *p = (pexGetDynamicsReply *) buf;
    PEX_DECODE_REPLY(GetDynamics,buf);
	PrintUval(viewRep,p->viewRep);
	PrintUval(markerBundle,p->markerBundle);
	PrintUval(textBundle,p->textBundle);
	PrintUval(lineBundle,p->lineBundle);
	PrintUval(interiorBundle,p->interiorBundle);
	PrintUval(edgeBundle,p->edgeBundle);
	PrintUval(colourTable,p->colourTable);
	PrintUval(patternTable,p->patternTable);
	PrintUval(wksTransform,p->wksTransform);
	PrintUval(highlightFilter,p->highlightFilter);
	PrintUval(invisibilityFilter,p->invisibilityFilter);
	PrintUval(HlhsrMode,p->HlhsrMode);
	PrintUval(structureModify,p->structureModify);
	PrintUval(postStructure,p->postStructure);
	PrintUval(unpostStructure,p->unpostStructure);
	PrintUval(deleteStructure,p->deleteStructure);
	PrintUval(referenceModify,p->referenceModify);
	PrintUval(bufferModify,p->bufferModify);
}

PEX_DECODE(GetViewRep)
{
	CAPTURE_REQUEST(GetViewRep,req,buf);
	
    PEX_DECODE_REQUEST(GetViewRep,buf);
	PrintVal(fpFormat,req->fpFormat);
	PrintUval(index,req->index);
	PrintUval(wks,req->wks);
}

PEX_DECODE(GetViewRepReply)
{
	CAPTURE_REPLY(GetViewRep,reply,buf);
	
	pexViewRep *vr;
    PEX_DECODE_REPLY(GetViewRep,buf);
	PRINT("viewUpdate: %u\n",reply->viewUpdate);
	reply += sizeof(pexGetViewRepReply);
	vr = (pexViewRep *) reply;
	PrintUval(vr->index,vr->index);
}

PEX_DECODE(RedrawAllStructures)
{
	CAPTURE_REQUEST(RedrawAllStructures,req,buf);

    PEX_DECODE_REQUEST(RedrawAllStructures,buf);

	PRINT("id: %u\n",req->id);
}

PEX_DECODE(UpdateWorkstation)
{
	CAPTURE_REQUEST(UpdateWorkstation,req,buf);

    PEX_DECODE_REQUEST(UpdateWorkstation,buf);

	PRINT("id: %u\n",req->id);
}

PEX_DECODE(RedrawClipRegion)
{
	CAPTURE_REQUEST(RedrawClipRegion,req,buf);

	PEX_DECODE_REQUEST(RedrawClipRegion,buf);

	PRINT("wks: %u\n",req->wks);
	PRINT("numRects: %u\n",req->numRects);
}

PEX_DECODE(ExecuteDeferredActions)
{
	CAPTURE_REQUEST(ExecuteDeferredActions,req,buf);

    PEX_DECODE_REQUEST(ExecuteDeferredActions,buf);
    
	PRINT("id: %u\n",req->id);
}

PEX_DECODE(SetViewPriority)
{
	CAPTURE_REQUEST(SetViewPriority,req,buf);

	PEX_DECODE_REQUEST(SetViewPriority,buf);

	PRINT("wks: %u\n",req->wks);
	PRINT("index1: %u\n",req->index1);
	PRINT("index2: %u\n",req->index2);
	PRINT("priority: %u\n",req->priority);
}


PEX_DECODE(SetDisplayUpdateMode)
{
	CAPTURE_REQUEST(SetDisplayUpdateMode,req,buf);

    PEX_DECODE_REQUEST(SetDisplayUpdateMode,buf);

	PRINT("wks: %u\n",req->wks);
	PRINT("mode: %d\n",req->displayUpdate);
}

PEX_DECODE(MapDCtoWC)
{
	CAPTURE_REQUEST(MapDCtoWC,req,buf);

    PEX_DECODE_REQUEST(MapDCtoWC,buf);

	PRINT("fpFormat: %s\n", NameOfFloatingPointFormat(req->fpFormat));
	PRINT("wks: %u\n",req->wks);
	PRINT("numCoords: %u\n",req->numCoords);
}

PEX_DECODE(MapDCtoWCReply)
{
	CAPTURE_REPLY(MapDCtoWC,reply,buf);

	PEX_DECODE_REPLY(MapDCtoWC,buf);

	PRINT("viewIndex: %u\n",reply->viewIndex);
	PRINT("numCoords: %u\n",reply->numCoords);
}

PEX_DECODE(MapWCtoDC)
{
	CAPTURE_REQUEST(MapWCtoDC,req,buf);

    PEX_DECODE_REQUEST(MapWCtoDC,buf);

	PRINT("fpFormat: %s\n",req->fpFormat);
	PRINT("index: %u\n",req->index);
	PRINT("wks: %u\n",req->wks);
	PRINT("numCoords: %u\n",req->numCoords);
}

PEX_DECODE(MapWCtoDCReply)
{
	CAPTURE_REPLY(MapWCtoDC,reply,buf);

    PEX_DECODE_REPLY(MapWCtoDC,buf);

	PRINT("numCoords: %u\n",reply->numCoords);
}

#define PrintNpcSubvolume(_NPC) \
(void) fprintf( stderr, \
"( xmin: %g ymin: %g zmin: %g)\n(xmax: %g ymax: %g zmax: %g\n", \
_NPC.minval.x, \
_NPC.minval.y, \
_NPC.minval.z, \
_NPC.maxval.x, \
_NPC.maxval.y, \
_NPC.maxval.z )

#define PrintMatrix(_M) \
{ \
register long i = 4; \
\
while ( i--) { \
(void) fprintf( stderr, \
"(%g, %g, %g, %g)\n", \
_M[0][i], _M[1][i], _M[2][i], _M[3][i]); \
} \
}

#define PrintCR \
(void) fprintf( stderr, "\n");

PEX_DECODE(SetViewRep)
{
	CAPTURE_REQUEST(SetViewRep,req,buf);
    
    PEX_DECODE_REQUEST(SetViewRep,buf);

	PRINT("wks: %u\n",req->wks);
	PRINT("viewRep.index: %u\n",req->viewRep.index);
	PRINT("viewRep.view.clipFlags: 0x%x\n",req->viewRep.view.clipFlags);
	PrintNpcSubvolume(req->viewRep.view.clipLimits);
	PrintMatrix(req->viewRep.view.orientation);
	PrintCR;
	PrintMatrix(req->viewRep.view.mapping);
}

PEX_DECODE(SetWksWindow)
{
	CAPTURE_REQUEST(SetWksWindow,req,buf);
	
	PEX_DECODE_REQUEST(SetWksWindow,buf);

	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(req->fpFormat));
	PRINT("wks: %u\n",req->wks);
	PrintNpcSubvolume(req->npcSubvolume);
}

#define PrintDeviceCoord(_STR,_DC) \
(void) fprintf( stderr, "_STR: %d, %d, %g\n", \
_DC.x, _DC.y, _DC.z)

PEX_DECODE(SetWksViewport)
{
	CAPTURE_REQUEST(SetWksViewport,req,buf);
	
	PEX_DECODE_REQUEST(SetWksViewport,buf);

	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(req->fpFormat));
	PRINT("wks: %u\n",req->wks);
	PrintDeviceCoord(minval(x,y,z),req->viewport.minval);
	PrintDeviceCoord(maxval(x,y,z),req->viewport.maxval);
	PrintUval(useDrawable,req->viewport.useDrawable);
}

PEX_DECODE(SetHlhsrMode)
{
	CAPTURE_REQUEST(SetHlhsrMode,req,buf);

	PEX_DECODE_REQUEST(SetHlhsrMode,buf);

	PRINT("wks: %u\n",req->wks);
	PRINT("mode: %d\n",req->mode);
}

PEX_DECODE(PostStructure)
{
	CAPTURE_REQUEST(PostStructure,req,buf);
    
    PEX_DECODE_REQUEST(PostStructure,buf);

	PRINT("wks: %u\n",req->wks);
	PRINT("sid: %u\n",req->sid);
	PRINT("priority: %.4f\n",req->priority);
}

PEX_DECODE(UnpostStructure)
{
	CAPTURE_REQUEST(UnpostStructure,req,buf);

    PEX_DECODE_REQUEST(UnpostStructure,buf);

	PRINT("wks: %u\n",req->wks);
	PRINT("sid: %u\n",req->sid);
}

PEX_DECODE(UnpostAllStructures)
{
	CAPTURE_REQUEST(UnpostAllStructures,req,buf);

    PEX_DECODE_REQUEST(UnpostAllStructures,buf);

	PRINT("id: %u\n",req->id);
}

PEX_DECODE(GetWksPostings)
{
	CAPTURE_REQUEST(GetWksPostings,req,buf);

    PEX_DECODE_REQUEST(GetWksPostings,buf);

	PRINT("id: %u\n",req->id);
}

PEX_DECODE(GetWksPostingsReply)
{
    PEX_DECODE_REPLY(GetWksPostings,buf);
}

PEX_DECODE(GetPickDevice)
{
	CAPTURE_REQUEST(GetPickDevice,req,buf);

	PEX_DECODE_REQUEST(GetPickDevice,buf);

	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(req->fpFormat));
	PRINT("devType: %d\n",req->devType);
	PRINT("wks: %u\n",req->wks);
	PRINT("itemMask: 0x%x\n",req->itemMask);
}

PEX_DECODE(GetPickDeviceReply)
{
    PEX_DECODE_REPLY(GetPickDevice,buf);
}

PEX_DECODE(ChangePickDevice)
{
	CAPTURE_REQUEST(ChangePickDevice,req,buf);

    PEX_DECODE_REQUEST(ChangePickDevice,buf);

	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(req->fpFormat));
	PRINT("wks: %u\n",req->wks);
	PRINT("devType: %d\n",req->devType);
	PRINT("itemMask: 0x%x\n",req->itemMask);
}

PEX_DECODE(CreatePickMeasure)
{
	CAPTURE_REQUEST(CreatePickMeasure,req,buf);

    PEX_DECODE_REQUEST(CreatePickMeasure,buf);

	PRINT("wks: %u\n",req->wks);
	PRINT("pm: %u\n",req->pm);
	PRINT("devType: %d\n",req->devType);
}

PEX_DECODE(FreePickMeasure)
{
	CAPTURE_REQUEST(FreePickMeasure,req,buf);

    PEX_DECODE_REQUEST(FreePickMeasure,buf);

	PRINT("id: %u\n",req->id);
}

PEX_DECODE(GetPickMeasure)
{
	CAPTURE_REQUEST(GetPickMeasure,req,buf);

    PEX_DECODE_REQUEST(GetPickMeasure,buf);

	PRINT("pm: %u\n",req->pm);
	PRINT("itemMask: 0x%x\n",req->itemMask);
}

PEX_DECODE(GetPickMeasureReply)
{
    PEX_DECODE_REPLY(GetPickMeasure,buf);
}

PEX_DECODE(UpdatePickMeasure)
{
	CAPTURE_REQUEST(UpdatePickMeasure,req,buf);

    PEX_DECODE_REQUEST(UpdatePickMeasure,buf);

	PRINT("pm: %u\n",req->pm);
	PRINT("numBytes: %u\n",req->numBytes);
}

PEX_DECODE(OpenPexFont)
{
	CAPTURE_REQUEST(OpenFont,req,buf);

	PEX_DECODE_REQUEST(OpenFont,buf);

	PRINT("font: %u\n",req->font);
	PRINT("numBytes: %u\n",req->numBytes);
}

PEX_DECODE(ClosePexFont)
{
	CAPTURE_REQUEST(CloseFont,req,buf);

	PEX_DECODE_REQUEST(CloseFont,buf);

	PRINT("id: %u\n",req->id);
}

PEX_DECODE(QueryPexFont)
{
	CAPTURE_REQUEST(QueryFont,req,buf);

    PEX_DECODE_REQUEST(QueryFont,buf);

	PRINT("font: %u\n",req->font);
}

PEX_DECODE(QueryPexFontReply)
{
	CAPTURE_REPLY(QueryFont,reply,buf);

    PEX_DECODE_REPLY(QueryFont,buf);

	PRINT("lengthFontInfo: %u\n",reply->lengthFontInfo);
}

PEX_DECODE(ListPexFonts)
{
	CAPTURE_REQUEST(ListFonts,req,buf);

	PEX_DECODE_REQUEST(ListFonts,buf);

	PRINT("maxNames: %u\n",req->maxNames);
	PRINT("numChars: %u\n",req->numChars);
}

PEX_DECODE(ListPexFontsReply)
{
	CAPTURE_REPLY(ListFonts,reply,buf);

    PEX_DECODE_REPLY(ListFonts,buf);

	PRINT("numStrings: %u\n",reply->numStrings);
}

PEX_DECODE(ListPexFontsWithInfo)
{
	CAPTURE_REQUEST(ListFontsWithInfo,req,buf);

	PEX_DECODE_REQUEST(ListFontsWithInfo,buf);

	PRINT("maxNames: %u\n",req->maxNames);
	PRINT("numChars: %u\n",req->numChars);
}

PEX_DECODE(ListPexFontsWithInfoReply)
{
	CAPTURE_REPLY(ListFontsWithInfo,reply,buf);

    PEX_DECODE_REPLY(ListFontsWithInfo,buf);

	PRINT("numStrings: %u\n",reply->numStrings);
}

PEX_DECODE(QueryPexTextExtents)
{
	CAPTURE_REQUEST(QueryTextExtents,req,buf);
	
    PEX_DECODE_REQUEST(QueryTextExtents,buf);

	PRINT("fpFormat: %s\n",NameOfFloatingPointFormat(req->fpFormat));
	PRINT("textPath: %u\n",req->textPath);
	PRINT("fontGroupIndex: %u\n",req->fontGroupIndex);
	PRINT("id: %u\n",req->id);
	PRINT("charExpansion: %g\n",req->charExpansion);
	PRINT("charSpacing: %g\n",req->charSpacing);
	PRINT("charHeight: %g\n",req->charHeight);
	PRINT("vertical: %u\n",req->textAlignment.vertical);
	PRINT("horizontal: %u\n",req->textAlignment.horizontal);
	PRINT("numStrings: %u\n",req->numStrings);
}

PEX_DECODE(QueryPexTextExtentsReply)
{
    PEX_DECODE_REPLY(QueryTextExtents,buf);
}


/**
 **
 **	PEX Error Printing procedures
 **
 **/

PEX_DECODE(ColorTypeError)
{
	CAPTURE_ERROR(ColourType,err,buf);

	PRINT("sequence number: %u\n",err->sequenceNumber);
	PRINT("bad color type: %u\n",err->badColourType);
	PRINT("minor opcode: %u\n",err->minorOpcode);
	PRINT("major opcode: %u\n",err->majorOpcode);
}

PEX_DECODE(RendererStateError)
{
    pexRendererStateError *err = (pexRendererStateError *) buf;

	PRINT("sequence number: %u\n",err->sequenceNumber);
	PRINT("bad renderer ID: %u\n",err->badRendererId);
	PRINT("minor opcode: %u\n",err->minorOpcode);
	PRINT("major opcode: %u\n",err->majorOpcode);
}

PEX_DECODE(FloatingPointFormatError)
{
	CAPTURE_ERROR(FloatingPointFormat,err,buf);

	PRINT("sequence number: %u\n",err->sequenceNumber);
	PRINT("bad format: %u\n",err->badFormat);
	PRINT("minor opcode: %u\n",err->minorOpcode);
	PRINT("major opcode: %u\n",err->majorOpcode);
}

PEX_DECODE(LabelError)
{
	CAPTURE_ERROR(Label,err,buf);

	PRINT("sequence number: %u\n",err->sequenceNumber);
	PRINT("bad label: %u\n",err->badLabel);
	PRINT("minor opcode: %u\n",err->minorOpcode);
	PRINT("major opcode: %u\n",err->majorOpcode);
}

PEX_DECODE(LookupTableError)
{
	CAPTURE_ERROR(LookupTable,err,buf);

	PRINT("sequence number: %u\n",err->sequenceNumber);
	PRINT("bad table ID: %u\n",err->badId);
	PRINT("bad pipeline context ID: %u\n",err->badId);
	PRINT("minor opcode: %u\n",err->minorOpcode);
	PRINT("major opcode: %u\n",err->majorOpcode);
}

PEX_DECODE(NamesetError)
{
	CAPTURE_ERROR(Nameset,err,buf);
    
	PRINT("sequence number: %u\n",err->sequenceNumber);
	PRINT("bad nameset ID: %u\n",err->badId);
	PRINT("minor opcode: %u\n",err->minorOpcode);
	PRINT("major opcode: %u\n",err->majorOpcode);
}

PEX_DECODE(PathError)
{
	CAPTURE_ERROR(Path,err,buf);
    
	PRINT("sequence number: %u\n",err->sequenceNumber);
	PRINT("bad path ID: %u\n",err->badId);
	PRINT("minor_opcode: %u\n",err->minorOpcode);
	PRINT("major_opcode: %u\n",err->majorOpcode);
}

PEX_DECODE(PexFontError)
{
	CAPTURE_ERROR(PexFont,err,buf);
    
	PRINT("sequence number: %u\n",err->sequenceNumber);
	PRINT("bad_font_ID: %u\n",err->badId);
	PRINT("minor_opcode: %u\n",err->minorOpcode);
	PRINT("major_opcode: %u\n",err->majorOpcode);
}

PEX_DECODE(PhigsWksError)
{
	CAPTURE_ERROR(PhigsWks,err,buf);
    
	PRINT("sequence number: %u\n",err->sequenceNumber);
	PRINT("bad PHIGS workstation ID: %u\n",err->badId);
	PRINT("minor opcode: %u\n",err->minorOpcode);
	PRINT("major opcode: %u\n",err->majorOpcode);
}

PEX_DECODE(PickMeasureError)
{
	CAPTURE_ERROR(PickMeasure,err,buf);
    
	PRINT("sequence number: %u\n",err->sequenceNumber);
	PRINT("bad device ID: %u\n",err->badId);
	PRINT("minor opcode: %u\n",err->minorOpcode);
	PRINT("major opcode: %u\n",err->majorOpcode);
}

PEX_DECODE(PipelineContextError)
{
	CAPTURE_ERROR(PipelineContext,err,buf);
    
	PRINT("sequence number: %u\n",err->sequenceNumber);
	PRINT("bad pipeline context ID: %u\n",err->badId);
	PRINT("minor opcode: %u\n",err->minorOpcode);
	PRINT("major opcode: %u\n",err->majorOpcode);
}

PEX_DECODE(RendererError)
{
	CAPTURE_ERROR(Renderer,err,buf);

	PRINT("sequence number: %u\n",err->sequenceNumber);
	PRINT("bad renderer ID: %u\n",err->badId);
	PRINT("minor opcode: %u\n",err->minorOpcode);
	PRINT("major opcode: %u\n",err->majorOpcode);
}

PEX_DECODE(SearchContextError)
{
	CAPTURE_ERROR(SearchContext,err,buf);

	PRINT("sequence number: %u\n",err->sequenceNumber);
	PRINT("bad search context ID: %u\n",err->badId);
	PRINT("minor opcode: %u\n",err->minorOpcode);
	PRINT("major opcode: %u\n",err->majorOpcode);
}

PEX_DECODE(StructureError)
{
	CAPTURE_ERROR(Structure,err,buf);

	PRINT("sequence number: %u\n",err->sequenceNumber);
	PRINT("bad structure ID: %u\n",err->badId);
	PRINT("minor opcode: %u\n",err->minorOpcode);
	PRINT("major opcode: %u\n",err->majorOpcode);
}

PEX_DECODE(OutputCommandError)
{
	CAPTURE_ERROR(OutputCmd,err,buf);

	PRINT("sequence number: %u\n",err->sequenceNumber);
	PRINT("bad resource ID: %u\n",err->badId);
	PRINT("minor opcode: %u\n",err->minorOpcode);
	PRINT("major opcode: %u\n",err->majorOpcode);
}

FillArea(buf)
	register CARD32 *buf;
{	
	int i, num_points;
	pexCoord3D *p;
	CAPTURE_OUTPUT_CMD(FillArea,oc,buf);
	PRINT("shape: %u\n",oc->shape);
	PRINT("ignoreEdges: %u\n",oc->ignoreEdges);
	num_points = ((oc->head.length * 4) - 
		sizeof(pexFillArea)) / sizeof(pexCoord3D);

	PRINT("numPoints: %d\n",num_points);

	p = (pexCoord3D *)(oc + 1);

	for (i = 0; i < num_points; i++, p++) {
		PRINT("(%.4f, ", p->x); 
		PRINT("%.4f, ", p->y); 
		PRINT("%.4f)\n",p->z); 
	}
}

FillAreaSet(buf)
	register CARD32 *buf;
{
	register int i, j, numPoints;
	register CARD32 *p;

	CAPTURE_OUTPUT_CMD(FillAreaSet,oc,buf);
	PRINT("shape: %u\n",oc->shape);
	PRINT("ignoreEdges: %u\n",oc->ignoreEdges);
	PRINT("contourHint: %u\n",oc->contourHint);
	PRINT("numLists: %u\n",oc->numLists);

	p = (CARD32 *)(oc + 1);

	for (i = 0; i < oc->numLists; i++) {
		numPoints = (int) *p++;
		PRINT("list #%d\t",i);
		PRINT("numPoints: %d\n",numPoints);
		for (j = 0; j < numPoints; j++) {
			PRINT("(%.4f, ",(XpexFloat) *p); p++;
			PRINT("%.4f, ",(XpexFloat) *p); p++;
			PRINT("%.4f)\n",(XpexFloat) *p); p++;
		}
	}
}
