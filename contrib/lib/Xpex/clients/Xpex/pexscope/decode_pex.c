/* $Header: decode_pex.c,v 2.5 91/09/11 17:45:34 erwin Exp $ */
#ifndef lint
static char sccsid[] = "@(#)decode_pex.c 1.5 90/04/05 Copyright 1990 SMI/MIT";
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
#include "PEX.h"
#include "PEXprotost.h"
#include "PEXproto.h"
#include "PEX_names.h"
#include "scope.h"
#include "pexscope.h"

#if defined(__STDC__) && !defined(UNIXCPP)
#define SETUP_OUTPUT_CMD(name,oc,buf) \
pex/##/name/##/ *oc = (pex/##/name/##/ *) buf
#else
#define SETUP_OUTPUT_CMD(name,oc,buf) \
pex/**/name/**/ *oc = (pex/**/name/**/ *) buf
#endif

#define PRINT_STRING_VALUE(FORMAT,STRING,VALUE) \
(void) fprintf(stderr,FORMAT,STRING,VALUE)

unsigned char pex_last_req;

BOOL LookForPEXFlag;

CARD8 PEXCode;
CARD8 PEX_BEC;

#define VALID_PEX_REQ(_REQ) \
(_REQ >= PEX_GetExtensionInfo) && (_REQ <= PEXMaxRequest)

DecodePexRequest(fd, buf)
	FD fd;
	unsigned char *buf;
{
	short req = IByte (&buf[0]);

#ifdef DONT_USE
(void) fprintf( stderr, 
"In pex_decode_req(): req: %d\n", buf[1]); 
#endif /* DONT_USE */

	if ( VALID_PEX_REQ(buf[1])) {
	   pex_last_req = buf[1];
	} else {
	   return;
	}

	switch (buf[1]) {
	case PEX_GetExtensionInfo:
		GetExtensionInfo( buf);
		ReplyExpected( fd, req);
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
		SetElementPointerAtLabel( buf);
		break;
	case PEX_ElementSearch:
		ElementSearch( buf);
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
	case PEX_RedrawClipRegion:
		RedrawClipRegion( buf);
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
		MapDCtoWC( buf);
		ReplyExpected(fd, req);
		break;
	case PEX_MapWCtoDC:
		MapWCtoDC( buf);
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
	case PEX_SetWksBufferMode:
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
		OpenPexFont(buf);
		break;
	case PEX_CloseFont:
		ClosePexFont(buf);
		break;
	case PEX_QueryFont:
		QueryPexFont( buf);
		ReplyExpected( fd, req);
		break;
	case PEX_ListFonts:
		ListPexFonts( buf);
		ReplyExpected( fd, req);
		break;
	case PEX_ListFontsWithInfo:
		ListPexFontsWithInfo( buf);
		ReplyExpected( fd, req);
		break;
	case PEX_QueryTextExtents:
		QueryPexTextExtents( buf);
		ReplyExpected( fd, req);
		break;
	default:
		break;
	}
}
#undef VALID_PEX_REQ

/*
 *
 *
 *
 */

pex_decode_elem( num_cmds, buf)
	CARD32 num_cmds;
	register CARD32 *buf;
{
	register long i;
	register pexElementInfo *el_info;

	for (i = 0; i < num_cmds; i++) {

		el_info = (pexElementInfo *) buf;

		pexpr_element_info( buf);

		switch (el_info->elementType) {
		case PEXOCMarkerType:
		{
			SETUP_OUTPUT_CMD(MarkerType,oc,buf);

			PRINT_STRING_VALUE("markerType : %s (%d)\n",
				NameOfMarkerType(oc->markerType), 
				oc->markerType);

			break;
		}
		case PEXOCAtextStyle:	
		{
			SETUP_OUTPUT_CMD(AtextStyle,oc,buf);

			PRINT_STRING_VALUE("atextStyle : %s (%d)\n",
				NameOfAtextStyle(oc->style), 
				oc->style);

			break;
		}
		case PEXOCLineType:
		{
			SETUP_OUTPUT_CMD(LineType,oc,buf);
			PRINT_STRING_VALUE("lineType : %s (%d)\n",
				NameOfLineType(oc->lineType), 
				oc->lineType);
			break;
		}
		case PEXOCInteriorStyle:
		{
			SETUP_OUTPUT_CMD(InteriorStyle,oc,buf); 

			PRINT_STRING_VALUE("interiorStyle : %s (%d)\n", 
				NameOfInteriorStyle(oc->interiorStyle),  
				oc->interiorStyle); 

			break;
		}
		case PEXOCBfInteriorStyle:		
		{
			SETUP_OUTPUT_CMD(BfInteriorStyle,oc,buf);
			PRINT_STRING_VALUE("BfInteriorStyle : %s\n",
				NameOfInteriorStyle(oc->interiorStyle),
				oc->interiorStyle);

			break;
		}
		case PEXOCSurfaceReflModel:
		case PEXOCBfSurfaceReflModel:
		{
			SETUP_OUTPUT_CMD(SurfaceReflModel,oc,buf);
			PRINT_STRING_VALUE("ReflectionModel : %s (%d)\n",
			NameOfReflectionModel(oc->reflectionModel),
			oc->reflectionModel);
			break;
		}
		case PEXOCSurfaceInterp:
		case PEXOCBfSurfaceInterp:
		{
			SETUP_OUTPUT_CMD(SurfaceInterp,oc,buf);
			PRINT_STRING_VALUE("SurfaceInterpMethod : %s (%d)\n",
				NameOfSurfaceInterpMethod(oc->surfaceInterp),
				oc->surfaceInterp);
			break;
		}
		case PEXOCSurfaceEdgeType:
		{
			SETUP_OUTPUT_CMD(SurfaceEdgeType,oc,buf);
			PRINT_STRING_VALUE("SurfaceEdgeType : %s (%d)\n",
				NameOfSurfaceEdgeType(oc->edgeType),
				oc->edgeType);
			break;
		}
		case PEXOCRenderingColourModel:
		{
			SETUP_OUTPUT_CMD(RenderingColourModel,oc,buf);
			PRINT_STRING_VALUE("RenderingColourModel : %s (%d)\n",
				NameOfRenderingColourModel(oc->model),
				oc->model);
			break;
		}
		case PEXOCMarkerScale:
		case PEXOCCharExpansion:
		case PEXOCCharSpacing:
		case PEXOCCharHeight:
		case PEXOCAtextHeight:
		case PEXOCLineWidth:
		case PEXOCSurfaceEdgeWidth:	
			pexpr_float( "value",
			((pexMarkerScale *)buf)->scale );
			break;
		case PEXOCMarkerColourIndex:
		case PEXOCMarkerBundleIndex:
		case PEXOCTextFontIndex:
		case PEXOCTextColourIndex:
		case PEXOCTextBundleIndex:
		case PEXOCLineColourIndex:		
		case PEXOCLineBundleIndex:
		case PEXOCInteriorStyleIndex:	
		case PEXOCSurfaceColourIndex:
		case PEXOCBfInteriorStyleIndex:
		case PEXOCBfSurfaceColourIndex:
		case PEXOCInteriorBundleIndex:
		case PEXOCSurfaceEdgeColourIndex:
		case PEXOCEdgeBundleIndex:
		case PEXOCViewIndex:
		case PEXOCDepthCueIndex:
		case PEXOCColourApproxIndex:
		{
			SETUP_OUTPUT_CMD(MarkerBundleIndex,oc,buf);

			(void) fprintf(stderr,"index: %u\n", oc->index);

			break;
		}
		case PEXOCMarkerColour:
		case PEXOCTextColour:
		case PEXOCLineColour:	
		case PEXOCSurfaceColour:
		case PEXOCBfSurfaceColour:
		case PEXOCSurfaceEdgeColour:
		{
			SETUP_OUTPUT_CMD(MarkerColour,oc,buf);
			pexpr_colour_specifier( &(oc->colourSpec));
			break;
		}
		case PEXOCTextPrecision:
		case PEXOCTextPath:
		case PEXOCAtextPath:
		case PEXOCCullingMode:
			pexpr_card( "value", 
			((pexTextPrecision *) buf)->precision);
			break;
		case PEXOCCharUpVector:	
		case PEXOCAtextUpVector:
			pexpr_vector_2d( 
			buf + sizeof(pexElementInfo));
			break;
		case PEXOCTextAlignment:
		case PEXOCAtextAlignment:
			pexpr_text_alignment_data( 
			buf + sizeof(pexElementInfo));
			break;
		case PEXOCCurveApproximation:
		{
			pexCurveApproximation *p = (pexCurveApproximation *) buf;

			pexpr_curve_approx_method( "approx.approxMethod", 
			p->approx.approxMethod);
			pexpr_float( "approx.tolerance", p->approx.tolerance);
			break;
		}
		case PEXOCPolylineInterp:
			break;
		case PEXOCSurfaceReflAttr:
		case PEXOCBfSurfaceReflAttr:
		{
			pexSurfaceReflAttr *p = (pexSurfaceReflAttr *) buf;

			pexpr_float( "reflectionAttr.ambient",
				p->reflectionAttr.ambient);
			pexpr_float( "reflectionAttr.diffuse",
				p->reflectionAttr.diffuse);
			pexpr_float( "reflectionAttr.specular",
				p->reflectionAttr.specular);
			pexpr_float( "reflectionAttr.specularConc",
				p->reflectionAttr.specularConc);
			pexpr_float( "reflectionAttr.transmission",
				p->reflectionAttr.transmission);
			pexpr_colour_specifier( &(p->reflectionAttr.specularColour));
			break;
		}
		case PEXOCSurfaceApproximation:
			break;
		case PEXOCDistinguishFlag:
		case PEXOCSurfaceEdgeFlag:
		case PEXOCModelClip:
			pexpr_switch( "flag", 
			((pexDistinguishFlag *)buf)->distinguish);
			break;
		case PEXOCPatternSize:
			pexpr_coord_2d( "size", 
			&(((pexPatternSize *)buf)->size));
			break;
		case PEXOCPatternRefPt:
			pexpr_coord_2d( "point", 
			&(((pexPatternRefPt *)buf)->point));
			break;
		case PEXOCPatternAttr:
		{
			pexPatternAttr *p = (pexPatternAttr *) buf;
			pexpr_coord_3d( "refPt", &(p->refPt));
			pexpr_vector_3d( "vector1", &(p->vector1));
			pexpr_vector_3d( "vector2", &(p->vector2));
			break;
		}
		case PEXOCSetAsfValues:
		{
			break;
		}
		case PEXOCLocalTransform:
			LocalTransform(buf);
			break;
		case PEXOCLocalTransform2D:
		{
		    pexLocalTransform2D *p = (pexLocalTransform2D *) buf;
			pexpr_comp_type( p->compType);
			pexpr_matrix_3x3( p->matrix3X3);
			break;
		}
		case PEXOCGlobalTransform:
		{
			pexGlobalTransform *p = (pexGlobalTransform *) buf;
			pexpr_matrix( p->matrix);
			break;
		}
		case PEXOCGlobalTransform2D:
		{
			pexGlobalTransform2D *p = (pexGlobalTransform2D *) buf;
			pexpr_matrix_3x3( p->matrix3X3);
			break;
		}
		case PEXOCModelClipVolume:	
			break;
		case PEXOCModelClipVolume2D:
			break;
		case PEXOCRestoreModelClip:
			/* Nothing else is needed */
			break;
		case PEXOCLightState:
		{
#ifdef NOT_READY
			pexLightState *pOC = (pexLightState *) buf;
			int bytes_read;
			CARD16 *enable;
			CARD16 *disable;
			int offset = sizeof(pexLightState);

			pexpr_card( "numEnable", numEnable);
			pexpr_card( "numDisable", numDisable);
			enable = (CARD16 *) (buf + offset);
			pexpr_array_of_card16( "enable", 
			(int) pOC->numEnable, enable, &bytes_read);
			offset = bytes_read + PADDING(bytes_read);
			disable = (CARD16 *) (buf + offset);
			pexpr_array_of_card16( "disable", 
			(int) pOC->numDisable, disable, &bytes_read);
#endif /* NOT_READY */
			break;
		}
		case PEXOCPickId:
		case PEXOCHlhsrIdentifier:
		case PEXOCExecuteStructure:
		{	
			CAPTURE_OUTPUT_CMD(ExecuteStructure,oc,buf);
			pexpr_card( "id", 
			((pexExecuteStructure *)buf)->id);
			break;
		}
		case PEXOCParaSurfCharacteristics:
			break;
		case PEXOCAddToNameSet:
		case PEXOCRemoveFromNameSet:
		{
			pexAddToNameSet *p = (pexAddToNameSet *) buf;
			int count = ( ( p->head.length * 4) - 
				sizeof(pexAddToNameSet)) / sizeof(pexName);
			pexpr_card( "numNames", count);
			break;
		}
		case PEXOCLabel:
		{
			CAPTURE_OUTPUT_CMD(Label,oc,buf);

			(void) fprintf(stderr, "label: %d\n", oc->label);

			break;
		}
		case PEXOCApplicationData:
			break;
		case PEXOCGse:
			break;
		case PEXOCMarker:
		case PEXOCPolyline:
		{
			CAPTURE_OUTPUT_CMD(Marker,p,buf);
			
			int count = ((p->head.length * 4) - 
				sizeof(pexMarker)) / sizeof(pexCoord3D);
			pexpr_card( "numPoints", count);
			pexpr_array_of_coord_3d( count, (pexCoord3D *) ( p + 1));
			break;
		}
		case PEXOCMarker2D:
		case PEXOCPolyline2D:
		{
			CAPTURE_OUTPUT_CMD(Marker2D,p,buf);
			
			int count = ((p->head.length * 4) - 
				sizeof(pexMarker2D)) / sizeof(pexCoord2D);
			pexpr_card( "numPoints", count);
			pexpr_array_of_coord_2d( count, (pexCoord2D *) ( p + 1));
			break;
		}
		case PEXOCText:
		{
			CAPTURE_OUTPUT_CMD(Text,p,buf);
			
			pexpr_coord_3d( "origin", &(p->origin));
			pexpr_vector_3d( "vector1", &(p->vector1));
			pexpr_vector_3d( "vector2", &(p->vector2));
			pexpr_card( "numEncodings", p->numEncodings);
			break;
		}
		case PEXOCText2D:
		{
			CAPTURE_OUTPUT_CMD(Text2D,p,buf);
			
			pexpr_coord_2d( "origin", &(p->origin));
			pexpr_card( "numEncodings", p->numEncodings);
			break;
		}
		case PEXOCAnnotationText:
		{
			CAPTURE_OUTPUT_CMD(AnnotationText,p,buf);
			
			pexpr_coord_3d( "origin", &(p->origin));
			pexpr_coord_3d( "offset", &(p->offset));
			pexpr_card( "numEncodings", p->numEncodings);
			break;
		}
		case PEXOCAnnotationText2D:
		{
			CAPTURE_OUTPUT_CMD(AnnotationText2D,p,buf);
			
			pexpr_coord_2d( "origin", &(p->origin));
			pexpr_coord_2d( "offset", &(p->offset));
			pexpr_card( "numEncodings", p->numEncodings);
			break;
		}
		case PEXOCPolylineSet:
		{
			CAPTURE_OUTPUT_CMD(PolylineSet,p,buf);
			
			pexpr_colour_type( "colourType", p->colourType);
			pexpr_geo_attrs( "vertexAttribs", p->vertexAttribs);
			pexpr_card( "numLists", p->numLists);
			break;
		}
		case PEXOCNurbCurve:		
		{
			CAPTURE_OUTPUT_CMD(NurbCurve,p,buf);
			
			pexpr_card( "curveOrder", p->curveOrder);
			pexpr_coord_type( "coordType", p->coordType);
			pexpr_card( "numKnots", p->numKnots);
			pexpr_card( "numPoints", p->numPoints);
			break;
		}
		case PEXOCFillArea:	
			FillArea(buf);
			break;
		case PEXOCFillArea2D:
		{
			CAPTURE_OUTPUT_CMD(FillArea2D,p,buf);
			
			int count = ((p->head.length * 4) - 
				sizeof(pexFillArea2D)) / sizeof(pexCoord2D);

			pexpr_shape_hint( "shape", p->shape);
			pexpr_switch( "ignoreEdges", p->ignoreEdges);

			pexpr_card( "numPoints", (unsigned) count);

			pexpr_array_of_coord_2d( count,
			(pexCoord2D *)( buf + sizeof(pexFillArea2D)));
			break;
		}
		case PEXOCExtFillArea:
			break;
		case PEXOCFillAreaSet:
			FillAreaSet(buf);
			break;
		case PEXOCFillAreaSet2D:	
		{
			CAPTURE_OUTPUT_CMD(FillAreaSet2D,p,buf);
			
			pexpr_shape_hint( "shape", p->shape);
			pexpr_switch( "ignoreEdges", p->ignoreEdges);
			pexpr_contour_hint( "contourHint", p->contourHint);
			pexpr_card( "numLists", p->numLists);
			break;
		}
		case PEXOCExtFillAreaSet:
		{
			CAPTURE_OUTPUT_CMD(ExtFillAreaSet,p,buf);
			
			pexpr_shape_hint( "shape", p->shape);
			pexpr_switch( "ignoreEdges", p->ignoreEdges);
			pexpr_contour_hint( "contourHint", p->contourHint);
			pexpr_geo_attrs( "facetAttribs", p->facetAttribs);
			pexpr_geo_attrs( "vertexAttribs", p->vertexAttribs);
			pexpr_card( "numLists", p->numLists);
			break;
		}
		case PEXOCTriangleStrip:
		{
			CAPTURE_OUTPUT_CMD(TriangleStrip,p,buf);
			
			pexpr_colour_type( p->colourType);
			pexpr_geo_attrs( "facetAttribs", p->facetAttribs);
			pexpr_geo_attrs( "vertexAttribs", p->vertexAttribs);
			pexpr_card( "numVertices", p->numVertices);
			break;
		}
		case PEXOCQuadrilateralMesh:	
		{
			CAPTURE_OUTPUT_CMD(QuadrilateralMesh,p,buf);
			
			pexpr_colour_type( p->colourType);
			pexpr_card( "mPts", p->mPts);
			pexpr_card( "nPts", p->nPts);
			pexpr_geo_attrs( "facetAttribs", p->facetAttribs);
			pexpr_geo_attrs( "vertexAttribs", p->vertexAttribs);
			pexpr_shape_hint( "shape", p->shape); 
			break;
		}
		case PEXOCSOFAS:	
		case PEXOCNurbSurface:
			break;
		case PEXOCCellArray:
		{
			CAPTURE_OUTPUT_CMD(CellArray,p,buf);

			pexpr_coord_3d( "point1", &(p->point1));
			pexpr_coord_3d( "point2", &(p->point2));
			pexpr_coord_3d( "point3", &(p->point3));
			pexpr_card( "dx", p->dx);
			pexpr_card( "dy", p->dy);
			break;
		}
		case PEXOCCellArray2D:
		{
			CAPTURE_OUTPUT_CMD(CellArray2D,p,buf);
			
			pexpr_coord_2d( "point1", &(p->point1));
			pexpr_coord_2d( "point2", &(p->point2));
			pexpr_card( "dx", p->dx);
			pexpr_card( "dy", p->dy);
			break;
		}
		case PEXOCExtCellArray:
		{
			CAPTURE_OUTPUT_CMD(ExtCellArray,p,buf);

			pexpr_colour_type( p->colourType);
			pexpr_coord_3d( "point1", &(p->point1));
			pexpr_coord_3d( "point2", &(p->point2));
			pexpr_coord_3d( "point3", &(p->point3));
			pexpr_card( "dx", p->dx);
			pexpr_card( "dy", p->dy);
			break;
		}
		case PEXOCGdp:
		case PEXOCGdp2D:
			break;
		default:
			break;
		}
		buf += (((pexElementInfo *) buf)->length);
		(void) fprintf(stderr, "\n");
		
	} /* end-for */
}

/*
 *
 *
 *
 */
DecodePexReply(fd, buf)
	FD fd;
	unsigned char *buf;
{
#ifdef PEXSCOPE_DEBUG
(void) fprintf( stderr,"pex_decode_reply(): enter\n");
(void) fprintf( stderr,"pex_last_req: %d\n", pex_last_req);
#endif /* PEXSCOPE_DEBUG */

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
		QueryPexFontReply(buf);
		break;
	case PEX_ListFonts:
		ListPexFontsReply(buf);
		break;
	case PEX_ListFontsWithInfo:
		ListPexFontsWithInfoReply(buf);
		break;
	case PEX_QueryTextExtents:
		QueryPexTextExtentsReply(buf);
		break;
	default:
		break;
	}
}

/**
 **
 **
 **/

DecodePexError(fd, buf)
FD fd;
unsigned char *buf;
{
    short error = IByte(&buf[1]) - PEX_BEC;

#ifdef PEXSCOPE_DEBUG
(void) fprintf( stderr, "DecodePexError(): error: %d\n", error);
#endif /* PEXSCOPE_DEBUG */

    switch (error) {
    case PEXColourTypeError:
        ColorTypeError(buf);
        break;
    case PEXRendererStateError:
        RendererStateError(buf);
        break;
    case PEXFloatingPointFormatError:
        FloatingPointFormatError(buf);
        break;
    case PEXLabelError:
        LabelError(buf);
        break;
    case PEXLookupTableError:
        LookupTableError(buf);
        break;
    case PEXNameSetError:
        NamesetError(buf);
        break;
    case PEXPathError:
        PathError(buf);
        break;
    case PEXFontError:
        PexFontError(buf);
        break;
    case PEXPhigsWksError:
        PhigsWksError(buf);
        break;
    case PEXPickMeasureError:
        PickMeasureError(buf);
        break;
    case PEXPipelineContextError:
        PipelineContextError(buf);
        break;
    case PEXRendererError:
        RendererError(buf);
        break;
    case PEXSearchContextError:
        SearchContextError(buf);
        break;
    case PEXStructureError:
        StructureError(buf);
        break;
    case PEXOutputCommandError:
        OutputCommandError(buf);
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
