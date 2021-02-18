/* $XConsortium: print_pex.c,v 5.4 91/02/17 12:34:48 rws Exp $ */

/***********************************************************
Copyright (c) 1989, 1990, 1991 by Sun Microsystems, Inc. and the X Consortium.

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
#include <X11/extensions/PEX.h>
#include <X11/extensions/PEXproto.h>
#include "pexRNames.h"
#include "pexOCNames.h"
#include "pexscope.h"


#define PEX_CARD8       0
#define PEX_CARD16      1
#define PEX_CARD32      2
#define PEX_ENUM        3
#define PEX_FP_FORMAT   4
#define PEX_STRING      5
#define PEX_DRAWABLE	6

Nothing(buf)
register unsigned char *buf;
{
}

GetExtensionInfo(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetExtensionInfo,buf);
	pex_print(buf,4,2,PEX_CARD16,"     Protocol Major Version");
	pex_print(buf,6,2,PEX_CARD16,"     Protocol Minor Version");
}

GetExtensionInfoReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetExtensionInfo,buf);
	pex_print(buf,8,2,PEX_CARD16,"     Protocol Major Version");
	pex_print(buf,10,2,PEX_CARD16,"     Protocol Minor Version");
	pex_print(buf,12,4,PEX_CARD32,"\t     Release Number");
	pex_print(buf,16,4,PEX_CARD32,"      Length of Vendor Name");
	pex_print(buf,20,4,PEX_CARD32,"\t\tSubset info");
}

GetEnumeratedTypeInfo(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetEnumeratedTypeInfo,buf);
	pex_print(buf,4,4,PEX_DRAWABLE,"\t        Drawable Id");
}

GetEnumeratedTypeInfoReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetEnumeratedTypeInfo,buf);
}

GetImpDepConstants(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetImpDepConstants,buf);
}

GetImpDepConstantsReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetImpDepConstants,buf);
}

CreateLookupTable(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(CreateLookupTable,buf);
}

CopyLookupTable(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(CopyLookupTable,buf);
}

FreeLookupTable(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(FreeLookupTable,buf);
}

GetTableInfo(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetTableInfo,buf);
}

GetTableInfoReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetTableInfo,buf);
}

GetPredefinedEntries(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetPredefinedEntries,buf);
}

GetPredefinedEntriesReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetPredefinedEntries,buf);
}

GetDefinedIndices(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetDefinedIndices,buf);
}

GetDefinedIndicesReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetDefinedIndices,buf);
}

GetTableEntry(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetTableEntry,buf);
}

GetTableEntryReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetTableEntry,buf);
}

GetTableEntries(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetTableEntries,buf);
}

GetTableEntriesReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetTableEntries,buf);
}

SetTableEntries(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(SetTableEntries,buf);
}

DeleteTableEntries(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(DeleteTableEntries,buf);
}

CreatePipelineContext(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(CreatePipelineContext,buf);
}

CopyPipelineContext(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(CopyPipelineContext,buf);
}

FreePipelineContext(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(FreePipelineContext,buf);
}

GetPipelineContext(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetPipelineContext,buf);
}

GetPipelineContextReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetPipelineContext,buf);
}

ChangePipelineContext(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(ChangePipelineContext,buf);
}

CreateRenderer(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(CreateRenderer,buf);
}

FreeRenderer(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(FreeRenderer,buf);
}

ChangeRenderer(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(ChangeRenderer,buf);
}

GetRendererAttributes(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetRendererAttributes,buf);
}

GetRendererAttributesReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetRendererAttributes,buf);
}

BeginRendering(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(BeginRendering,buf);
}

EndRendering(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(EndRendering,buf);
}

BeginStructure(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(BeginStructure,buf);
}

EndStructure(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(EndStructure,buf);
}

RenderOutputCommands(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(RenderOutputCommands,buf);
}

RenderNetwork(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(RenderNetwork,buf);
}

CreateStructure(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(CreateStructure,buf);
}

CopyStructure(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(CopyStructure,buf);
}

GetRendererDynamics(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetRendererDynamics,buf);
}

GetRendererDynamicsReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetRendererDynamics,buf);
}

DestroyStructures(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(DestroyStructures,buf);
}

GetStructureInfo(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetStructureInfo,buf);
}

GetStructureInfoReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetStructureInfo,buf);
}

GetElementInfo(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetElementInfo,buf);
}

GetElementInfoReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetElementInfo,buf);
}

GetStructuresInNetwork(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetStructuresInNetwork,buf);
}

GetStructuresInNetworkReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetStructuresInNetwork,buf);
}

GetAncestors(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetAncestors,buf);
}

GetAncestorsReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetAncestors,buf);
}

GetDescendants(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetDescendants,buf);
}

GetDescendantsReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetDescendants,buf);
}

FetchElements(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(FetchElements,buf);
}

FetchElementsReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(FetchElements,buf);
}

SetEditingMode(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(SetEditingMode,buf);
}
	
SetElementPointer(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(SetElementPointer,buf);
}

SetElementPointerAtLabel(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(SetElementPointerAtLabel,buf);
}

ElementSearch(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(ElementSearch,buf);
}

ElementSearchReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(ElementSearch,buf);
}

StoreElements(buf)
unsigned char *buf;
{
	pexStoreElementsReq *req_ptr = (pexStoreElementsReq *)buf;
	CARD32 num_cmds = req_ptr->numCommands;
	PEX_DECODE_REQUEST(StoreElements,buf);
	buf += sizeof(pexStoreElementsReq);
	pex_decode_elem(num_cmds, (unsigned char *)(buf));
}

DeleteElements(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(DeleteElements,buf);
}

DeleteElementsToLabel(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(DeleteElementsToLabel,buf);
}

DeleteBetweenLabels(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(DeleteBetweenLabels,buf);
}

CopyElements(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(CopyElements,buf);
}

ChangeStructureRefs(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(ChangeStructureRefs,buf);
}

CreateNameSet(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(CreateNameSet,buf);
}

CopyNameSet(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(CopyNameSet,buf);
}

FreeNameSet(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(FreeNameSet,buf);
}

GetNameSet(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetNameSet,buf);
}

GetNameSetReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetNameSet,buf);
}

ChangeNameSet(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(ChangeNameSet,buf);
}

CreateSearchContext(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(CreateSearchContext,buf);
}

CopySearchContext(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(CopySearchContext,buf);
}

FreeSearchContext(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(FreeSearchContext,buf);
}

GetSearchContext(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetSearchContext,buf);
}

GetSearchContextReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetSearchContext,buf);
}

ChangeSearchContext(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(ChangeSearchContext,buf);
}

SearchNetwork(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(SearchNetwork,buf);
}

SearchNetworkReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(SearchNetwork,buf);
}

CreatePhigsWks(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(CreatePhigsWks,buf);
}

FreePhigsWks(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(FreePhigsWks,buf);
}

GetWksInfo(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetWksInfo,buf);
}

GetWksInfoReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetWksInfo,buf);
}

GetDynamics(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetDynamics,buf);
}

GetDynamicsReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetDynamics,buf);
}

GetViewRep(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetViewRep,buf);
}

GetViewRepReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetViewRep,buf);
}

RedrawAllStructures(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(RedrawAllStructures,buf);
}

UpdateWorkstation(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(UpdateWorkstation,buf);
}

ExecuteDeferredActions(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(ExecuteDeferredActions,buf);
}

SetViewPriority(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(SetViewPriority,buf);
}

SetDisplayUpdateMode(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(SetDisplayUpdateMode,buf);
}

MapDCtoWC(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(MapDCtoWC,buf);
}

MapDCtoWCReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(MapDCtoWC,buf);
}

MapWCtoDC(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(MapWCtoDC,buf);
}

MapWCtoDCReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(MapWCtoDC,buf);
}

SetViewRep(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(SetViewRep,buf);
}

SetWksWindow(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(SetWksWindow,buf);
}

SetWksViewport(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(SetWksViewport,buf);
}

SetHlhsrMode(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(SetHlhsrMode,buf);
}

PostStructure(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(PostStructure,buf);
}

UnpostStructure(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(UnpostStructure,buf);
}

UnpostAllStructures(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(UnpostAllStructures,buf);
}

GetWksPostings(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetWksPostings,buf);
}

GetWksPostingsReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetWksPostings,buf);
}

GetPickDevice(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetPickDevice,buf);
}

GetPickDeviceReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetPickDevice,buf);
}

ChangePickDevice(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(ChangePickDevice,buf);
}

CreatePickMeasure(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(CreatePickMeasure,buf);
}

FreePickMeasure(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(FreePickMeasure,buf);
}

GetPickMeasure(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(GetPickMeasure,buf);
}

GetPickMeasureReply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(GetPickMeasure,buf);
}

UpdatePickMeasure(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(UpdatePickMeasure,buf);
}

pexopenfont(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(OpenFont,buf);
}

pexclosefont(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(CloseFont,buf);
}

pexqueryfont(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(QueryFont,buf);
}

pexqueryfontreply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(QueryFont,buf);
}

pexlistfonts(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(ListFonts,buf);
}

pexlistfontsreply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(ListFonts,buf);
}

pexlistfontswithinfo(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(ListFontsWithInfo,buf);
}

pexlistfontswithinforeply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(ListFontsWithInfo,buf);
}

pexquerytextextents(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(QueryTextExtents,buf);
}

pexquerytextextentsreply(buf)
register unsigned char *buf;
{
	PEX_DECODE_REPLY(QueryTextExtents,buf);
}

RedrawClipRegion(buf)
register unsigned char *buf;
{
	PEX_DECODE_REQUEST(RedrawClipRegion,buf);
}

/*
 *	PEX Error Printing procedures
 */
pexerror_colortype(buf)
register unsigned char *buf;
{
}

pexerror_rendererstate(buf)
register unsigned char *buf;
{
}

pexerror_floatingpointformat(buf)
register unsigned char *buf;
{
}

pexerror_label(buf)
register unsigned char *buf;
{
}

pexerror_lookuptable(buf)
register unsigned char *buf;
{
}

pexerror_nameset(buf)
register unsigned char *buf;
{
}

pexerror_path(buf)
register unsigned char *buf;
{
}
pexerror_font(buf)
register unsigned char *buf;
{
}

pexerror_phigswks(buf)
register unsigned char *buf;
{
}

pexerror_pickmeasure(buf)
register unsigned char *buf;
{
}

pexerror_pipelinecontext(buf)
register unsigned char *buf;
{
}

pexerror_renderer(buf)
register unsigned char *buf;
{
}

pexerror_searchcontext(buf)
register unsigned char *buf;
{
}

pexerror_structure(buf)
register unsigned char *buf;
{
}

pexerror_outputcommand(buf)
register unsigned char *buf;
{
}


pex_print(buf, start, length, field_type, name)
    unsigned char *buf;
    short start;
    short length;
    short field_type;
    char *name;
{
    switch (field_type) {
    case PEX_CARD8:
    	break;
    case PEX_CARD16:
    	fprintf(stderr,"%s: %d\n",name,*((CARD16*)&buf[start]));
        break;
    case PEX_CARD32:
    	fprintf(stderr,"%s: %d\n",name,*((CARD32*)&buf[start]));
        break;
    case PEX_DRAWABLE:
    	fprintf(stderr,"%s: %d\n",name,*((Drawable*)&buf[start]));
        break;
    case PEX_ENUM:
    	break;
    case PEX_FP_FORMAT:
    	break;
    case PEX_STRING:
        break;
    default:
        break;
    }       
}
