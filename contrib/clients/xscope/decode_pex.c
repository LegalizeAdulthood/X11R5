/* $XConsortium: decode_pex.c,v 5.3 91/02/17 12:34:53 rws Exp $ */

/***********************************************************
Copyright 1989, 1990, 1991 by Sun Microsystems, Inc. and the X Consortium.

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
#include "scope.h"
#include "pexRNames.h"
#include "pexOCNames.h"
#include "pexscope.h"

unsigned char pex_last_req;

BOOL LookForPEXFlag;

CARD8 PEXCode;

pex_decode_req(fd, buf)
FD fd;
unsigned char *buf;
{
	short req = IByte (&buf[0]);

	if ((buf[1] < PEX_GetExtensionInfo) ||
	    (buf[1] > PEXMaxRequest)) {
	   return;
	} else
	   pex_last_req =buf[1];

	switch (buf[1]) {
	case PEX_GetExtensionInfo:
		GetExtensionInfo(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_GetEnumeratedTypeInfo:
		GetEnumeratedTypeInfo(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_GetImpDepConstants:
		GetImpDepConstants(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_CreateLookupTable:
		CreateLookupTable(buf);
		break;
	case PEX_CopyLookupTable:
		CopyLookupTable(buf);
		break;
	case PEX_FreeLookupTable:
		FreeLookupTable(buf);
		break;
	case PEX_GetTableInfo:
		GetTableInfo(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_GetPredefinedEntries:
		GetPredefinedEntries(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_GetDefinedIndices:
		GetDefinedIndices(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_GetTableEntry:
		GetTableEntry(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_GetTableEntries:
		GetTableEntries(buf);
		ReplyExpected(fd, buf);
		break;
	case PEX_SetTableEntries:
		SetTableEntries(buf);
		break;
	case PEX_DeleteTableEntries:
		DeleteTableEntries(buf);
		break;
	case PEX_CreatePipelineContext:
		CreatePipelineContext(buf);
		break;
	case PEX_CopyPipelineContext:
		CopyPipelineContext(buf);
		break;
	case PEX_FreePipelineContext:
		FreePipelineContext(buf);
		break;
	case PEX_GetPipelineContext:
		GetPipelineContext(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_ChangePipelineContext:
		ChangePipelineContext(buf);
		break;
	case PEX_CreateRenderer:
		CreateRenderer(buf);
		break;
	case PEX_FreeRenderer:
		FreeRenderer(buf);
		break;
	case PEX_ChangeRenderer:
		ChangeRenderer(buf);
		break;
	case PEX_GetRendererAttributes:
		GetRendererAttributes(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_BeginRendering:
		BeginRendering(buf);
		break;
	case PEX_EndRendering:
		EndRendering(buf);
		break;
	case PEX_BeginStructure:
		BeginStructure(buf);
		break;
	case PEX_EndStructure:
		EndStructure(buf);
		break;
	case PEX_RenderOutputCommands:
		RenderOutputCommands(buf);
		break;
	case PEX_RenderNetwork:
		RenderNetwork(buf);
		break;
	case PEX_CreateStructure:
		CreateStructure(buf);
		break;
	case PEX_CopyStructure:
		CopyStructure(buf);
		break;
	case PEX_GetRendererDynamics:
		GetRendererDynamics(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_DestroyStructures:
		DestroyStructures(buf);
		break;
	case PEX_GetStructureInfo:
		GetStructureInfo(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_GetElementInfo:
		GetElementInfo(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_GetStructuresInNetwork:
		GetStructuresInNetwork(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_GetAncestors:
		GetAncestors(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_GetDescendants:
		GetDescendants(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_FetchElements:
		FetchElements(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_SetEditingMode:
		SetEditingMode(buf);
		break;
	case PEX_SetElementPointer:
		SetElementPointer(buf);
		break;
	case PEX_SetElementPointerAtLabel:
		SetElementPointerAtLabel(buf);
		break;
	case PEX_ElementSearch:
		ElementSearch(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_StoreElements:
		StoreElements(buf);
		break;
	case PEX_DeleteElements:
		DeleteElements(buf);
		break;
	case PEX_DeleteElementsToLabel:
		DeleteElementsToLabel(buf);
		break;
	case PEX_DeleteBetweenLabels:
		DeleteBetweenLabels(buf);
		break;
	case PEX_CopyElements:
		CopyElements(buf);
		break;
	case PEX_ChangeStructureRefs:
		ChangeStructureRefs(buf);
		break;
	case PEX_CreateNameSet:
		CreateNameSet(buf);
		break;
	case PEX_CopyNameSet:
		CopyNameSet(buf);
		break;
	case PEX_FreeNameSet:
		FreeNameSet(buf);
		break;
	case PEX_GetNameSet:
		GetNameSet(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_ChangeNameSet:
		ChangeNameSet(buf);
		break;
	case PEX_CreateSearchContext:
		CreateSearchContext(buf);
		break;
	case PEX_CopySearchContext:
		CopySearchContext(buf);
		break;
	case PEX_FreeSearchContext:
		FreeSearchContext(buf);
		break;
	case PEX_GetSearchContext:
		GetSearchContext(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_ChangeSearchContext:
		ChangeSearchContext(buf);
		break;
	case PEX_SearchNetwork:
		SearchNetwork(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_CreatePhigsWks:
		CreatePhigsWks(buf);
		break;
	case PEX_FreePhigsWks:
		FreePhigsWks(buf);
		break;
	case PEX_GetWksInfo:
		GetWksInfo(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_GetDynamics:
		GetDynamics(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_GetViewRep:
		GetViewRep(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_RedrawAllStructures:
		RedrawAllStructures(buf);
		break;
	case PEX_UpdateWorkstation:
		UpdateWorkstation(buf);
		break;
	case PEX_ExecuteDeferredActions:
		ExecuteDeferredActions(buf);
		break;
	case PEX_SetViewPriority:
		SetViewPriority(buf);
		break;
	case PEX_SetDisplayUpdateMode:
		SetDisplayUpdateMode(buf);
		break;
	case PEX_MapDCtoWC:
		MapDCtoWC(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_MapWCtoDC:
		MapWCtoDC(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_SetViewRep:
		SetViewRep(buf);
		break;
	case PEX_SetWksWindow:
		SetWksWindow(buf);
		break;
	case PEX_SetWksViewport:
		SetWksViewport(buf);
		break;
	case PEX_SetHlhsrMode:
		SetHlhsrMode(buf);
		break;
	case PEX_PostStructure:
		PostStructure(buf);
		break;
	case PEX_UnpostStructure:
		UnpostStructure(buf);
		break;
	case PEX_UnpostAllStructures:
		UnpostAllStructures(buf);
		break;
	case PEX_GetWksPostings:
		GetWksPostings(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_GetPickDevice:
		GetPickDevice(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_ChangePickDevice:
		ChangePickDevice(buf);
		break;
	case PEX_CreatePickMeasure:
		CreatePickMeasure(buf);
		break;
	case PEX_FreePickMeasure:
		FreePickMeasure(buf);
		break;
	case PEX_GetPickMeasure:
		GetPickMeasure(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_UpdatePickMeasure:
		UpdatePickMeasure(buf);
		break;
	case PEX_OpenFont:
		pexopenfont(buf);
		break;
	case PEX_CloseFont:
		pexclosefont(buf);
		break;
	case PEX_QueryFont:
		pexqueryfont(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_ListFonts:
		pexlistfonts(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_ListFontsWithInfo:
		pexlistfontswithinfo(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_QueryTextExtents:
		pexquerytextextents(buf);
		ReplyExpected(fd, req);
		break;
	case PEX_RedrawClipRegion:
		RedrawClipRegion(buf);
		break;
	default:
		break;
	}
}

/*
 *
 *
 *
 */
pex_decode_elem(num_cmds, buf)
CARD32 num_cmds;
unsigned char *buf;
{
	register long i;
	register pexElementInfo *el_info;

	for (i = 0; i < num_cmds; i++) {
		el_info = (pexElementInfo *) buf;
		if ((el_info->elementType <= PEXOCAll) ||
		    (el_info->elementType >= PEXMaxOC) )
			return;
		fprintf(stdout, "\t\t\t     %s\n",
		pex_oc_name[el_info->elementType]);

		fprintf(stdout, "\t\t\t     length : %d\n",el_info->length);
		buf += el_info->length * 4;

#ifdef IGNORE
		switch (el_info->elementType) {
		case PEXOCMarkerType:
			break;
		case PEXOCMarkerScale:
			break;
		case PEXOCMarkerColourIndex:
			break;
		case PEXOCMarkerColour:
			break;
		case PEXOCMarkerBundleIndex:
			break;
		case PEXOCTextFontIndex:
			break;
		case PEXOCTextPrecision:
			break;
		case PEXOCCharExpansion:
			break;
		case PEXOCCharSpacing:
			break;
		case PEXOCTextColourIndex:
			break;
		case PEXOCTextColour:
			break;
		case PEXOCCharHeight:
			break;
		case PEXOCCharUpVector:	
			break;
		case PEXOCTextPath:
			break;
		case PEXOCTextAlignment:
			break;
		case PEXOCAtextHeight:
			break;
		case PEXOCAtextUpVector:
			break;
		case PEXOCAtextPath:
			break;
		case PEXOCAtextAlignment:
			break;
		case PEXOCAtextStyle:	
			break;
		case PEXOCTextBundleIndex:
			break;
		case PEXOCLineType:
			break;
		case PEXOCLineWidth:
			break;
		case PEXOCLineColourIndex:		
			break;
		case PEXOCLineColour:	
			break;
		case PEXOCCurveApproximation:
			break;
		case PEXOCPolylineInterp:
			break;
		case PEXOCLineBundleIndex:
			break;
		case PEXOCInteriorStyle:
			break;
		case PEXOCInteriorStyleIndex:	
			break;
		case PEXOCSurfaceColourIndex:
			break;
		case PEXOCSurfaceColour:
			break;
		case PEXOCSurfaceReflAttr:
			break;
		case PEXOCSurfaceReflModel:
			break;
		case PEXOCSurfaceInterp:
			break;
		case PEXOCBfInteriorStyle:		
			break;
		case PEXOCBfInteriorStyleIndex:
			break;
		case PEXOCBfSurfaceColourIndex:
			break;
		case PEXOCBfSurfaceColour:
			break;
		case PEXOCBfSurfaceReflAttr:
			break;
		case PEXOCBfSurfaceReflModel:
			break;
		case PEXOCBfSurfaceInterp:
			break;
		case PEXOCSurfaceApproximation:
			break;
		case PEXOCTCApproximation:
			break;
		case PEXOCCullingMode:
			break;
		case PEXOCDistinguishFlag:
			break;
		case PEXOCNormalFlip:
			break;
		case PEXOCPatternSize:
			break;
		case PEXOCPatternRefPt:
			break;
		case PEXOCPatternAttr:
			break;
		case PEXOCInteriorBundleIndex:
			break;
		case PEXOCSurfaceEdgeFlag:
			break;
		case PEXOCSurfaceEdgeType:
			break;
		case PEXOCSurfaceEdgeWidth:		
			break;
		case PEXOCSurfaceEdgeColourIndex:
			break;
		case PEXOCSurfaceEdgeColour:
			break;
		case PEXOCEdgeBundleIndex:
			break;
		case PEXOCSetAsfValues:
			break;
		case PEXOCLocalTransform:
			break;
		case PEXOCLocalTransform2D:
			break;
		case PEXOCGlobalTransform:
			break;
		case PEXOCGlobalTransform2D:
			break;
		case PEXOCModelClip:
			break;
		case PEXOCModelClipVolume:	
			break;
		case PEXOCModelClipVolume2D	:
			break;
		case PEXOCRestoreModelClip:
			break;
		case PEXOCViewIndex:
			break;
		case PEXOCLightState:
			break;
		case PEXOCDepthCueIndex:
			break;
		case PEXOCPickId:
			break;
		case PEXOCHlhsrIdentifier:
			break;
		case PEXOCAddToNameSet:
			break;
		case PEXOCRemoveFromNameSet:
			break;
		case PEXOCExecuteStructure:
			break;
		case PEXOCLabel:
			break;
		case PEXOCApplicationData:
			break;
		case PEXOCGse:
			break;
		case PEXOCMarker:
			break;
		case PEXOCMarker2D:
			break;
		case PEXOCText:
			break;
		case PEXOCText2D:
			break;
		case PEXOCAnnotationText:
			break;
		case PEXOCAnnotationText2D:
			break;
		case PEXOCPolyline:
			break;
		case PEXOCPolyline2D:
			break;
		case PEXOCPolylineSet:
			break;
		case PEXOCNurbCurve:		
			break;
		case PEXOCFillArea:	
			break;
		case PEXOCFillArea2D:
			break;
		case PEXOCExtFillArea:
			break;
		case PEXOCFillAreaSet:	
			break;
		case PEXOCFillAreaSet2D:	
			break;
		case PEXOCExtFillAreaSet:		
			break;
		case PEXOCTriangleStrip:	
			break;
		case PEXOCQuadrilateralMesh:	
			break;
		case PEXOCIndexedPolygons:
			break;
		case PEXOCNurbSurface:
			break;
		case PEXOCCellArray:	
			break;
		case PEXOCCellArray2D:
			break;
		case PEXOCExtCellArray:		
			break;
		case PEXOCGdp:
			break;
		case PEXOCGdp2D:
			break;
		case PEXOCColourApproxIndex:
			break;
		default:
			break;
		}
#endif /* IGNORE */
	}
}

/*
 *
 *
 *
 */
pex_decode_reply(fd, buf)
FD fd;
unsigned char *buf;
{
	switch (pex_last_req) {
	case PEX_GetExtensionInfo:
		GetExtensionInfoReply(buf);
		break;
	case PEX_GetEnumeratedTypeInfo:
		GetEnumeratedTypeInfoReply(buf);
		break;
	case PEX_GetImpDepConstants:
		GetImpDepConstantsReply(buf);
		break;
	case PEX_GetTableInfo:
		GetTableInfoReply(buf);
		break;
	case PEX_GetPredefinedEntries:
		GetPredefinedEntriesReply(buf);
		break;
	case PEX_GetDefinedIndices:
		GetDefinedIndicesReply(buf);
		break;
	case PEX_GetTableEntry:
		GetTableEntryReply(buf);
		break;
	case PEX_GetTableEntries:
		GetTableEntriesReply(buf);
		break;
	case PEX_GetPipelineContext:
		GetPipelineContextReply(buf);
		break;
	case PEX_GetRendererAttributes:
		GetRendererAttributesReply(buf);
		break;
	case PEX_GetRendererDynamics:
		GetRendererDynamicsReply(buf);
		break;
	case PEX_GetStructureInfo:
		GetStructureInfoReply(buf);
		break;
	case PEX_GetElementInfo:
		GetElementInfoReply(buf);
		break;
	case PEX_GetStructuresInNetwork:
		GetStructuresInNetworkReply(buf);
		break;
	case PEX_GetAncestors:
		GetAncestorsReply(buf);
		break;
	case PEX_GetDescendants:
		GetDescendantsReply(buf);
		break;
	case PEX_FetchElements:
		FetchElementsReply(buf);
		break;
	case PEX_ElementSearch:
		ElementSearchReply(buf);
		break;
	case PEX_GetNameSet:
		GetNameSetReply(buf);
		break;
	case PEX_GetSearchContext:
		GetSearchContextReply(buf);
		break;
	case PEX_SearchNetwork:
		SearchNetworkReply(buf);
		break;
	case PEX_GetWksInfo:
		GetWksInfoReply(buf);
		break;
	case PEX_GetDynamics:
		GetDynamicsReply(buf);
		break;
	case PEX_GetViewRep:
		GetViewRepReply(buf);
		break;
	case PEX_MapDCtoWC:
		MapDCtoWCReply(buf);
		break;
	case PEX_MapWCtoDC:
		MapWCtoDCReply(buf);
		break;
	case PEX_GetWksPostings:
		GetWksPostingsReply(buf);
		break;
	case PEX_GetPickDevice:
		GetPickDeviceReply(buf);
		break;
	case PEX_GetPickMeasure:
		GetPickMeasureReply(buf);
		break;
	case PEX_QueryFont:
		pexqueryfontreply(buf);
		break;
	case PEX_ListFonts:
		pexlistfontsreply(buf);
		break;
	case PEX_ListFontsWithInfo:
		pexlistfontswithinforeply(buf);
		break;
	case PEX_QueryTextExtents:
		pexquerytextextentsreply(buf);
		break;
	default:
		break;
	}
}

/*
 *
 *
 */

pex_decode_error(fd, buf)
FD fd;
unsigned char *buf;
{
	short error = IByte(&buf[1]);
	switch (error) {
	case PEXColourTypeError:
		pexerror_colortype(buf);
		break;
	case PEXRendererStateError:
		pexerror_rendererstate(buf);
		break;
	case PEXFloatingPointFormatError:
		pexerror_floatingpointformat(buf);
		break;
	case PEXLabelError:
		pexerror_label(buf);
		break;
	case PEXLookupTableError:
		pexerror_lookuptable(buf);
		break;
	case PEXNameSetError:
		pexerror_nameset(buf);
		break;
	case PEXPathError:
		pexerror_path(buf);
		break;
	case PEXFontError:
		pexerror_font(buf);
		break;
	case PEXPhigsWksError:
		pexerror_phigswks(buf);
		break;
	case PEXPickMeasureError:
		pexerror_pickmeasure(buf);
		break;
	case PEXPipelineContextError:
		pexerror_pipelinecontext(buf);
		break;
	case PEXRendererError:
		pexerror_renderer(buf);
		break;
	case PEXSearchContextError:
		pexerror_searchcontext(buf);
		break;
	case PEXStructureError:
		pexerror_structure(buf);
		break;
	case PEXOutputCommandError:
		pexerror_outputcommand(buf);
		break;
	default:
		break;
	}
}

InitializePEX()
{
    LookForPEXFlag = 0;
    PEXCode = 0;
}
