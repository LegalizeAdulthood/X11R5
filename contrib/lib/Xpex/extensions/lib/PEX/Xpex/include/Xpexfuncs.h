/* -*-C-*- $Header: Xpexfuncs.h,v 2.5 91/09/11 16:05:55 sinyaw Exp $ */

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


#ifndef _Xpexfuncs_h
#define _Xpexfuncs_h

#if defined(__STDC__)
#define EXTERN_FUNC(type, name, args) extern type name args
#else
#if defined(__cplusplus)
#define EXTERN_FUNC(type, name, args) extern "C" type name args
#else
#define EXTERN_FUNC(type, name, args) extern type name ()
#endif /* __cplusplus */
#endif /* __STDC__ */

EXTERN_FUNC(XExtCodes *, XpexInitialize,
	(Display *dpy));


EXTERN_FUNC(Bool, XpexQueryExtension,
	(register Display  *dpy,
	int  *major_opcode,  
	int  *first_error,  
	int  *first_event));  


EXTERN_FUNC(Status, XpexElementSearch,
	(Display *dpy,
    XpexStructure s_id,
    XpexElementPos *position,
    int direction,
    XpexListOfElementType  *incl_list, 
    XpexListOfElementType  *excl_list,
    int *srch_status,  
    int *found_offset)); 


EXTERN_FUNC(Status, XpexFetchElements,
	(Display  *dpy,
	XpexStructure  s_id,
	XpexElementRange  *range,
	char **elements,  
	int  *count));  


EXTERN_FUNC(Status, XpexGetAllImpDepConstants,
	(Display *dpy,
	Drawable drawable,
	XpexAllImpDepConstants *idc)); 


EXTERN_FUNC(Status, XpexGetAncestors,
	(Display  *dpy, 
	XpexStructure  s_id,
	int  path_order,
	int  path_depth,
	XpexListOfElementRef  *paths[],	
	int  *num_paths));	


EXTERN_FUNC(Status, XpexGetDefinedIndices,
	(Display  *dpy,
	XpexLookupTable  lut_id,
	XpexTableIndex *indices,
	int  *count)); 


EXTERN_FUNC(Status, XpexGetDescendants,
	(Display *dpy, 
	XpexStructure  s_id,
	int  path_order,
	int  path_depth,
	XpexListOfElementRef  *paths[], 	
	int  *num_paths)); 


EXTERN_FUNC(Status, XpexGetDynamics,
	(Display  *dpy,
	Drawable  drawable,
	XpexDynamics  *dynamics)); 


EXTERN_FUNC(Status, XpexGetElementInfo,
	(register Display *dpy,
	XpexStructure s_id,
	XpexElementRange *range,
	XpexElementInfo **elem_info,  
	int *count));  


EXTERN_FUNC(Status, XpexGetEnumTypeInfo,
	(Display *dpy,
	Drawable drawable,
	short *enum_types,
	int count,
	XpexBitmask returnFormat,
	XpexListOfEnumTypeDesc *info_list[], 
	int *actual_count)); 


EXTERN_FUNC(Status, XpexGetExtensionInfo,
	(Display *dpy,
	int proto_major,
	int proto_minor,
	XpexExtensionInfo *info));  


EXTERN_FUNC(Status, XpexGetImpDepConstants,
	(register Display *dpy,
	Drawable drawable,
	XpexImpDepConstantName *names,
	int numNames,
	unsigned long **values)); 


EXTERN_FUNC(Status, XpexGetNameSet,
	(Display *dpy,
	XpexNameSet  ns_id,
	XpexName  **names,  
	int  *count));  


EXTERN_FUNC(Status , XpexGetPC,
	(Display	*dpy, 
	XpexPC	pc, 
	XpexPCBitmask valueMask,
	XpexPCValues *values));  


EXTERN_FUNC(Status, XpexGetPickDevice,
	(register Display  *dpy,
	XpexPhigsWks  wks_id,
	XpexEnumTypeIndex  dev_type,
	XpexBitmask valueMask,
	XpexPickDevice  *values)); 


EXTERN_FUNC(Status, XpexGetPickMeasure,
	(register Display *dpy,
	XpexPickMeasure pm_id,
	XpexBitmask	valueMask,
	XpexPickMeasureAttributes *values));


EXTERN_FUNC(Status, XpexGetPredefinedEntries,
	(Display  *dpy,
	Drawable  drawable,
	XpexTableType  table_type,
	XpexTableIndex  start, 
	int  count,
	char **entries,
	int  *actual_count));  


EXTERN_FUNC(Status, XpexGetRendererAttributes,
	(register Display *dpy,
	XpexRenderer rdr_id,
	XpexBitmask valueMask,
	XpexRendererAttributes *values)); 


EXTERN_FUNC(Status, XpexGetRendererDynamics,
	(Display *dpy,
	XpexRenderer rdr_id,
	XpexRendererDynamics *dynamics)); 


EXTERN_FUNC(Status, XpexGetSC,
	(Display *dpy,
    XpexSC sc,
    XpexBitmask valueMask, 
    XpexSCValues *values)); 


EXTERN_FUNC(Status, XpexGetStructureInfo,
	(register Display *dpy,
	XpexStructure  s_id,
	XpexStructureInfo  *info)); 


EXTERN_FUNC(Status, XpexGetStructuresInNetwork,
	(Display *dpy,
	XpexStructure s_id,
	int which,
	XpexStructure **s_ids, 
	int *count)); 


EXTERN_FUNC(Status, XpexGetTableEntries,
	(Display  *dpy,
	XpexLookupTable  lut_id,
	XpexTableIndex  start,
	int  count,
	int  value_type,
	XpexTableType  *table_type, 
	char **entries,
	int  *actual_count)); 


EXTERN_FUNC(Status, XpexGetTableEntry,
	(Display  *dpy,
	XpexLookupTable  lut_id,
	XpexTableIndex  index,
	int  value_type,
	XpexTableType  *table_type, 
	int  *entry_status, 
	char **table_entry)); 


EXTERN_FUNC(Status, XpexGetTableInfo,
	(Display  *dpy,
	Drawable  drawable,
	XpexTableType  table_type,
	XpexTableInfo  *info)); 


EXTERN_FUNC(Status, XpexGetViewRep,
	(Display  *dpy,
	XpexPhigsWks  wks_id,
	int  index,
	int  *view_update,  
	XpexViewRep *requested,  
	XpexViewRep *current));  


EXTERN_FUNC(Status , XpexGetWksInfo,
	(Display *dpy,
	XpexPhigsWks wks_id,
	XpexBitmask valueMask[2],
	XpexWksInfo *values));


EXTERN_FUNC(Status, XpexGetWksPostings,
	(Display  *dpy,
	XpexStructure  s_id,
	XpexPhigsWks  **wks_ids,  
	int  *count));  


EXTERN_FUNC(Status, XpexMapDCtoWC,
	(Display  *dpy,
	XpexPhigsWks  wks_id,
	XpexDeviceCoord  *dc_pts,
	int num_dc_pts,
	XpexTableIndex  *view_index, 
	XpexCoord3D  **wc_pts, 
	int  *num_wc_pts)); 


EXTERN_FUNC(Status, XpexMapWCtoDC,
	(Display  *dpy,
	XpexPhigsWks  wks_id,
	XpexTableIndex  view_index,     
	XpexCoord3D  *wc_pts,
	int  num_wc_pts,
	XpexDeviceCoord  **dc_pts,  
	int  *num_dc_pts));  


EXTERN_FUNC(Status, XpexSearchNetwork,
	(Display  *dpy,
	XpexSC  sc_id,
	XpexElementRef  **elem_refs, 
	int  *count)); 


EXTERN_FUNC(XpexColorType, XpexGetColorType,
	(Display *dpy));


EXTERN_FUNC(XpexEnumTypeIndex, XpexGetFloatingPointFormat,
	(Display *dpy));


EXTERN_FUNC(XpexExtentInfo *, XpexQueryTextExtents,
	(register Display *dpy,
	XID resource_id,
	XpexTableIndex	font_group,
	short		text_path,
	XpexFloat	char_expansion,
	XpexFloat	char_spacing,
	XpexFloat	char_height,
	short 		h_alignment,
	short 		v_alignment,
	char		*strings[],
	int			num_strings));


EXTERN_FUNC(XpexFont , XpexOpenFont,
	(register Display *dpy,
	char *name));


EXTERN_FUNC(XpexFontInfo *, XpexQueryFont,
	(register Display *dpy,
	XpexFont fid));


EXTERN_FUNC(XpexLookupTable, XpexCreateLookupTable,
	(Display  *dpy,
	Drawable  drawable,
	XpexTableType  table_type));


EXTERN_FUNC(XpexNameSet, XpexCreateNameSet,
	(Display *dpy));

EXTERN_FUNC(XpexPC, XpexCreatePC,
	(Display *dpy,
	XpexPCBitmask valuemask,
	XpexPCValues *values));


EXTERN_FUNC(XpexPhigsWks, XpexCreatePhigsWks,
	(Display *dpy,
	Drawable drawable,
	XpexWksResources *wks_res,
	int buffer_mode));


EXTERN_FUNC(XpexPickMeasure, XpexCreatePickMeasure,
	(register Display  *dpy,
	XpexPhigsWks  wks_id,
	XpexEnumTypeIndex  dev_type));


EXTERN_FUNC(XpexRenderer, XpexCreateRenderer,
	(Display  *dpy,
	Drawable  drawable,
	XpexBitmask valuemask,
	XpexRendererAttributes  *values));


EXTERN_FUNC(XpexSC, XpexCreateSC,
	(Display *dpy,
	XpexBitmask valuemask,
	XpexSCValues *values));


EXTERN_FUNC(XpexStructure, XpexCreateStructure,
	(register Display  *dpy));


EXTERN_FUNC(char **, XpexListFonts,
    (register Display  *dpy,
    char  *pattern,
    int  maxnames,
    int  *actual_count)); 


EXTERN_FUNC(char **, XpexListFontsWithInfo,
	(register Display *dpy,
	char *pattern,
	int maxnames,
	int *count,	
	XpexFontInfo	**info));	


EXTERN_FUNC(int, XpexComputeGeometricNormal,
	(XpexBitmaskShort vertAttr,
	XpexListOfVertex  *vertexLists,
	int  numLists,
	XpexVector3D  *geometricNormal)); 


EXTERN_FUNC(int, XpexEvaluateViewMappingMatrix,
	(XpexViewMap3D  *map,  
	register XpexMatrix  m));  


EXTERN_FUNC(int, XpexEvaluateViewMappingMatrix2D,
	(XpexViewMap2D  *map,  
	register XpexMatrix3X3  m));  


EXTERN_FUNC(int, XpexEvaluateViewOrientationMatrix,
	(XpexCoord3D  *vrp,  
	register XpexVector3D  *vpn,  
	XpexVector3D  *vup,  
	XpexMatrix  m));  


EXTERN_FUNC(int, XpexEvaluateViewOrientationMatrix2D,
	(XpexCoord2D   *vrp,  
	register XpexVector2D  *vup,  
	register XpexMatrix3X3  m));  


EXTERN_FUNC(int, XpexTransformPoint,
	(register XpexCoord3D  *point,
	register XpexMatrix  transformationMatrix,
	XpexCoord3D  *transformedPoint));   


EXTERN_FUNC(int, XpexTransformPoint2D,
	(register XpexCoord2D  *point,
	register XpexMatrix3X3  transformationMatrix, 
	XpexCoord2D  *transformedPoint));  


EXTERN_FUNC(void, XpexAddToNameSet,
	(Display *dpy,
	XpexName *names,
	int count));


EXTERN_FUNC(void, XpexApplicationData,
	(Display  *dpy,
	XpexData *data));


EXTERN_FUNC(void, XpexAtext,
	(Display *dpy,
	XpexCoord3D *origin,
	XpexCoord3D *offset,
	char *string,
	int length));


EXTERN_FUNC(void, XpexAtext2D,
	(Display *dpy,
	XpexCoord2D *origin,
	XpexCoord2D *offset,
	char *string,
	int length));


EXTERN_FUNC(void, XpexCellArray,
	(Display *dpy,
	XpexCoord3D *pt1,
	XpexCoord3D *pt2,
	XpexCoord3D *pt3,
	int dx,
	int dy,
	XpexTableIndex *color_indices));


EXTERN_FUNC(void, XpexCellArray2D,
	(Display *dpy,
	XpexCoord2D *pt1,
	XpexCoord2D *pt2,
	int dx,
	int dy,
	XpexTableIndex *color_indices));


EXTERN_FUNC(void, XpexExecuteStructure,
	(Display *dpy,
	XpexStructure id));


EXTERN_FUNC(void, XpexExtCellArray,
	(Display *dpy,
	XpexCoord3D *pt1,
	XpexCoord3D *pt2,
	XpexCoord3D *pt3,
	int dx,
	int dy,
	XpexArrayOfColor *colors));


EXTERN_FUNC(void, XpexExtFillArea,
	(Display *dpy,
	XpexShape shape,
	XpexSwitch ignoreEdges,
	XpexBitmaskShort facetAttr,
	XpexBitmaskShort vertAttr,
	XpexFacetOptData *facetData,
	XpexArrayOfVertex *vertices,
	int numVertices));


EXTERN_FUNC(void, XpexExtFillAreaSet,
	(Display *dpy,
	XpexShape shape,
	XpexSwitch ignoreEdges,
	XpexContour contourHint,
	XpexBitmaskShort facetAttr,
	XpexBitmaskShort vertAttr,
	XpexFacetOptData *facetData,
	XpexListOfVertex *vertexLists,
	int numLists));


EXTERN_FUNC(void, XpexFillArea,
	(Display *dpy,
	XpexShape shape,
	XpexSwitch ignore_edges,
	XpexCoord3D *points,
	int num_points));


EXTERN_FUNC(void, XpexFillArea2D,
	(Display *dpy,
	XpexShape shape,
	XpexSwitch ignore_edges,
	XpexCoord2D *points,
	int num_points));


EXTERN_FUNC(void, XpexFillAreaSet,
	(Display *dpy,
	XpexShape shape,
	XpexSwitch ignoreEdges,
	XpexContour contourHint,
	XpexListOfCoord3D *vertexLists,
	int numLists));


EXTERN_FUNC(void, XpexFillAreaSet2D,
	(Display *dpy,
	XpexShape shape,
	XpexSwitch ignoreEdges,
	XpexContour contourHint,
	XpexListOfCoord2D *vertexLists,
	int numLists));


EXTERN_FUNC(void, XpexGdp,
	(Display *dpy,
	int gdp_id,
	XpexCoord3D *points,
	int numPoints,
	XpexData *data));


EXTERN_FUNC(void, XpexGdp2D,
	(Display *dpy,
	int gdp_id,
	XpexCoord2D *points,
	int numPoints,
	XpexData *data));


EXTERN_FUNC(void, XpexGse,
	(Display *dpy,
	int gse_id,
	XpexData *gse_data));


EXTERN_FUNC(void, XpexLabel,
	(Display *dpy,
	long label));


EXTERN_FUNC(void, XpexMarkers,
	(Display *dpy,
	XpexCoord3D *points,
	int num_points));


EXTERN_FUNC(void, XpexMarkers2D,
	(Display *dpy,
	XpexCoord2D *points,
	int num_points));


EXTERN_FUNC(void, XpexNurbCurve,
	(Display *dpy,
	int curveOrder,
	XpexCoordType coordType,
	XpexFloat *knots,
	long numKnots,
	XpexArrayOfCoord *points,
	long numPoints,
	XpexFloat tMin,
	XpexFloat tMax));


EXTERN_FUNC(void, XpexNurbSurface,
	(Display *dpy,
	XpexCoordType type,
	int uOrder,
	int vOrder,
	XpexFloat *uKnots,
	int numUknots,
	XpexFloat *vKnots,
	int numVknots,
	int mPts,
	int nPts,
	XpexArrayOfCoord *points,
	XpexListOfTrimCurve *trimCurveLists,
	int numLists));


EXTERN_FUNC(void, XpexPolyline,
	(Display *dpy,
	XpexCoord3D *points,
	int num_points));


EXTERN_FUNC(void, XpexPolyline2D,
	(Display *dpy, 
	XpexCoord2D *points, 
	int num_points));


EXTERN_FUNC(void, XpexPolylineSet,
	(Display *dpy,
	XpexBitmaskShort vertexAttr,
	XpexListOfVertex *vertexLists,
	int numLists));


EXTERN_FUNC(void, XpexQuadMesh,
	(Display *dpy,
	int shape,
	XpexBitmaskShort facetAttr,
	XpexBitmaskShort vertAttr,
	XpexArrayOfFacetOptData *facetData,
	XpexArrayOfVertex *vertices,
	int numPointsM,
	int numPointsN));


EXTERN_FUNC(void, XpexRemoveFromNameSet,
	(Display *dpy,
	XpexName *names,
	int count));


EXTERN_FUNC(void, XpexRestoreModelClip,
	(Display *dpy));


EXTERN_FUNC(void, XpexSetAtextAlignment,
	(Display *dpy,
	XpexTextValignment vertical,
	XpexTextHalignment horizontal));


EXTERN_FUNC(void, XpexSetAtextHeight,
	(Display *dpy,
	XpexFloat height));


EXTERN_FUNC(void, XpexSetAtextPath,
	(Display *dpy,
	XpexTextPath path));


EXTERN_FUNC(void, XpexSetAtextStyle,
	(Display *dpy,
	XpexEnumTypeIndex style));


EXTERN_FUNC(void, XpexSetAtextUpVector,
	(Display *dpy,
	XpexVector3D *up));


EXTERN_FUNC(void, XpexSetBfInteriorStyle,
	(Display *dpy,
	XpexEnumTypeIndex style));


EXTERN_FUNC(void, XpexSetBfInteriorStyleIndex,
	(Display *dpy,
	XpexTableIndex index));


EXTERN_FUNC(void, XpexSetBfReflectionAttributes,
	(Display *dpy,
	XpexReflectionAttr *refl_attr));


EXTERN_FUNC(void, XpexSetBfReflectionModel,
	(Display *dpy,
	XpexEnumTypeIndex refl_model));


EXTERN_FUNC(void, XpexSetBfSurfaceColor,
	(Display *dpy,
	XpexColor *color));


EXTERN_FUNC(void, XpexSetBfSurfaceColorIndex,
	(Display *dpy,
	XpexTableIndex index));


EXTERN_FUNC(void, XpexSetBfSurfaceInterpMethod,
	(Display *dpy,
	XpexEnumTypeIndex method));


EXTERN_FUNC(void, XpexSetCharExpansion,
	(Display *dpy,
	XpexFloat expansion));


EXTERN_FUNC(void, XpexSetCharHeight,
	(Display *dpy,
	XpexFloat height));


EXTERN_FUNC(void, XpexSetCharSpacing,
	(Display *dpy,
	XpexFloat spacing));


EXTERN_FUNC(void, XpexSetCharUpVector,
	(Display *dpy,
	XpexVector3D *up));


EXTERN_FUNC(void, XpexSetColorApproxIndex,
	(Display *dpy,
	XpexTableIndex index));


EXTERN_FUNC(void, XpexSetCullingMode,
	(Display *dpy,
	XpexCullMode culling_mode));


EXTERN_FUNC(void, XpexSetCurveApproximation,
	(Display *dpy,
	XpexEnumTypeIndex approx_method,
	XpexFloat tolerance));


EXTERN_FUNC(void, XpexSetDepthCueIndex,
	(Display *dpy,
	XpexTableIndex index));


EXTERN_FUNC(void, XpexSetDistinguishFlag,
	(Display *dpy,
	XpexSwitch distinguish));


EXTERN_FUNC(void, XpexSetEdgeBundleIndex,
	(Display *dpy,
	XpexTableIndex index));


EXTERN_FUNC(void, XpexSetGlobalTransform,
	(Display *dpy,
	XpexMatrix matrix));


EXTERN_FUNC(void, XpexSetGlobalTransform2D,
	(Display *dpy,
	XpexMatrix3X3 matrix3X3));


EXTERN_FUNC(void, XpexSetHlhsrId,
	(Display *dpy,
	long hlhsrID));


EXTERN_FUNC(void, XpexSetIndividualAsf,
	(Display *dpy,
	XpexAsfAttribute attribute,
	XpexAsfValue source));


EXTERN_FUNC(void, XpexSetInteriorBundleIndex,
	(Display *dpy,
	XpexTableIndex index));


EXTERN_FUNC(void, XpexSetInteriorStyle,
	(Display *dpy,
	XpexEnumTypeIndex style));


EXTERN_FUNC(void, XpexSetInteriorStyleIndex,
	(Display *dpy,
	XpexTableIndex index));


EXTERN_FUNC(void, XpexSetLightState,
	(Display *dpy,
	XpexTableIndex *enable,
	int numEnable,
	XpexTableIndex *disable,
	int numDisable));


EXTERN_FUNC(void, XpexSetLineBundleIndex,
	(Display *dpy,
	XpexTableIndex index));


EXTERN_FUNC(void, XpexSetLineColor,
	(Display *dpy,
	XpexColor *color));


EXTERN_FUNC(void, XpexSetLineColorIndex,
	(Display *dpy,
	XpexTableIndex color_index));


EXTERN_FUNC(void, XpexSetLineType,
	(Display *dpy, 
	XpexEnumTypeIndex lineType));


EXTERN_FUNC(void, XpexSetLineWidth,
	(Display *dpy,
	XpexFloat width));


EXTERN_FUNC(void, XpexSetLocalTransform,
	(Display *dpy,
	XpexComposition compType,
	XpexMatrix matrix));


EXTERN_FUNC(void, XpexSetLocalTransform2D,
	(Display *dpy,
	XpexComposition compType,
	XpexMatrix3X3 matrix3X3));


EXTERN_FUNC(void, XpexSetMarkerBundleIndex,
	(Display *dpy,
	XpexTableIndex index));


EXTERN_FUNC(void, XpexSetMarkerColor,
	(Display *dpy,
	XpexColor *color));


EXTERN_FUNC(void, XpexSetMarkerColorIndex,
	(Display *dpy,
	XpexTableIndex color_index));


EXTERN_FUNC(void, XpexSetMarkerScale,
	(Display *dpy,
	XpexFloat scale));


EXTERN_FUNC(void, XpexSetMarkerType,
	(Display *dpy,
	XpexEnumTypeIndex type));


EXTERN_FUNC(void, XpexSetModelClip,
	(Display *dpy,
	XpexSwitch on_off));


EXTERN_FUNC(void, XpexSetModelClipVolume,
	(Display *dpy,
	XpexEnumTypeIndex modelClipOperator,
	XpexHalfSpace *halfSpaces,
	int numHalfSpaces));


EXTERN_FUNC(void, XpexSetModelClipVolume2D,
	(Display *dpy,
	XpexEnumTypeIndex modelClipOperator,
	XpexHalfSpace2D *halfSpaces,
	int numHalfSpaces));


EXTERN_FUNC(void, XpexSetOfFillAreaSets,
	(Display *dpy,
	XpexShape shape,
	XpexBitmaskShort fas_attributes,
	XpexBitmaskShort vert_attributes,
	XpexBitmaskShort edge_attributes,
	XpexContour contour_hint,
	XpexSwitch contours_all_1,
	XpexArrayOfOptData *per_fas_data,
	int num_fas,
	XpexArrayOfVertex *per_vert_data,
	int num_verts,
	XpexSwitch *per_edge_data,
	int num_edges,
	int num_contours));


EXTERN_FUNC(void, XpexSetParaSurfChar,
	(Display *dpy,
	XpexParaSurfChar *psc));


EXTERN_FUNC(void, XpexSetPatternAttr,
	(Display *dpy,
	XpexCoord3D *ref_pt,
	XpexVector3D *vector1,
	XpexVector3D *vector2));


EXTERN_FUNC(void, XpexSetPatternRefPt,
	(Display *dpy,
	XpexCoord2D *point));


EXTERN_FUNC(void, XpexSetPatternSize,
	(Display *dpy,
	XpexCoord2D *size));


EXTERN_FUNC(void, XpexSetPickId,
	(Display *dpy,
	long pickId));


EXTERN_FUNC(void, XpexSetPolylineInterpMethod,
	(Display *dpy,
	XpexEnumTypeIndex method));


EXTERN_FUNC(void, XpexSetReflectionAttributes,
	(Display *dpy,
	XpexReflectionAttr *refl_attr));


EXTERN_FUNC(void, XpexSetReflectionModel,
	(Display *dpy,
	XpexEnumTypeIndex refl_model));


EXTERN_FUNC(void, XpexSetRenderingColorModel,
	(Display *dpy,
	XpexEnumTypeIndex model));


EXTERN_FUNC(void, XpexSetSurfaceApproximation,
	(Display *dpy,
	XpexEnumTypeIndex approxMethod,
	XpexFloat uTolerance,
	XpexFloat vTolerance));


EXTERN_FUNC(void, XpexSetSurfaceColor,
	(Display *dpy,
	XpexColor *color));


EXTERN_FUNC(void, XpexSetSurfaceColorIndex,
	(Display *dpy,
	XpexTableIndex index));


EXTERN_FUNC(void, XpexSetSurfaceEdgeColor,
	(Display *dpy,
	XpexColor *color));


EXTERN_FUNC(void, XpexSetSurfaceEdgeColorIndex,
	(Display *dpy,
	XpexTableIndex index));


EXTERN_FUNC(void, XpexSetSurfaceEdgeFlag,
	(Display *dpy,
	XpexSwitch on_off));


EXTERN_FUNC(void, XpexSetSurfaceEdgeType,
	(Display *dpy,
	XpexEnumTypeIndex edgeType));


EXTERN_FUNC(void, XpexSetSurfaceEdgeWidth,
	(Display *dpy,
	XpexFloat width));


EXTERN_FUNC(void, XpexSetSurfaceInterpMethod,
	(Display *dpy,
	XpexEnumTypeIndex method));


EXTERN_FUNC(void, XpexSetTextAlignment,
	(Display *dpy,
	XpexTextValignment vertical,
	XpexTextHalignment horizontal));


EXTERN_FUNC(void, XpexSetTextBundleIndex,
	(Display *dpy,
	XpexTableIndex index));


EXTERN_FUNC(void , XpexSetTextColor,
	(Display *dpy,
	XpexColor *text_color));


EXTERN_FUNC(void, XpexSetTextColorIndex,
	(Display *dpy,
	XpexTableIndex text_color_index));


EXTERN_FUNC(void, XpexSetTextFontIndex,
	(Display *dpy,
	XpexTableIndex text_font_index));


EXTERN_FUNC(void, XpexSetTextPath,
	(Display *dpy,
	XpexTextPath path));


EXTERN_FUNC(void, XpexSetTextPrecision,
	(Display *dpy,
	XpexTextPrecision precision));


EXTERN_FUNC(void, XpexSetViewIndex,
	(Display *dpy,
	XpexTableIndex viewIndex));


EXTERN_FUNC(void, XpexText,
	(Display *dpy,
	XpexCoord3D *origin,
	XpexVector3D *vector1,
	XpexVector3D *vector2,
	char *string,
	int length));


EXTERN_FUNC(void, XpexText2D,
	(Display *dpy,
	XpexCoord2D *origin,
	char *string,
	int length));


EXTERN_FUNC(void, XpexTriangleStrip,
	(Display *dpy,
	XpexBitmaskShort facetAttr,
	XpexBitmaskShort vertAttr,
	XpexArrayOfFacetOptData	*facetData,
	XpexArrayOfVertex *vertices,
	int numVertices));


EXTERN_FUNC(void, XpexBeginRendering,
	(Display *dpy,
	XpexRenderer rdr_id,
	Drawable drawable_id));


EXTERN_FUNC(void, XpexBeginStructure,
	(Display *dpy,
	XpexRenderer rdr_id,
	XpexStructure s_id));


EXTERN_FUNC(void, XpexBuildTransformationMatrix,
	(XpexCoord3D *pt,            
    XpexVector3D *shift,         
    XpexFloat x_angle,        
    XpexFloat y_angle,        
    XpexFloat z_angle,        
    XpexVector3D *scale,         
    XpexMatrix matrix));         


EXTERN_FUNC(void, XpexBuildTransformationMatrix2D,
    (XpexCoord2D      *pt,            
    XpexVector2D     *shift,         
    XpexFloat        angle,          
    XpexVector2D     *scale,         
    XpexMatrix3X3    matrix));         


EXTERN_FUNC(void, XpexChangeNameSet,
	(Display  *dpy,
	XpexNameSet  ns_id,
	int  action, 
	XpexName  *names,
	int  count));


EXTERN_FUNC(void, XpexChangePC,
	(Display  *dpy,
	XpexPC  pc,
	XpexPCBitmask valueMask,
	XpexPCValues  *values)); 


EXTERN_FUNC(void, XpexChangePickDevice,
	(register Display  *dpy,
	XpexPhigsWks  wks,
	XpexEnumTypeIndex  devType,
	XpexBitmask valueMask,
	XpexPickDevice  *values));  


EXTERN_FUNC(void, XpexChangeRenderer,
	(Display *dpy,
	XpexRenderer rdr_id,
	XpexBitmask valueMask,
	XpexRendererAttributes *values));


EXTERN_FUNC(void, XpexChangeSC,
	(Display  *dpy,
	XpexSC  sc_id,
	XpexBitmask valuemask, 
	XpexSCValues  *values));


EXTERN_FUNC(void , XpexChangeStructureRefs,
	(Display *dpy,
	XpexStructure old_s_id,
	XpexStructure new_s_id));


EXTERN_FUNC(void, XpexCloseFont,
	(register Display *dpy,
	XpexFont font_id));


EXTERN_FUNC(void, XpexComposeMatrix,
    (XpexMatrix    a,              
    XpexMatrix    b,              
    XpexMatrix    m));              


EXTERN_FUNC(void, XpexComposeMatrix2D,
    (XpexMatrix3X3     a,              
    XpexMatrix3X3     b,              
    XpexMatrix3X3     m));              


EXTERN_FUNC(void, XpexComposeTransformationMatrix,
	(XpexMatrix      m,              
    XpexCoord3D     *pt,            
    XpexVector3D    *shift,         
    XpexFloat       x_ang,          
    XpexFloat       y_ang,          
    XpexFloat       z_ang,          
    XpexVector3D    *scale,         
    XpexMatrix      result));         


EXTERN_FUNC(void, XpexComposeTransformationMatrix2D,
	(XpexMatrix3X3    m,      
    XpexCoord2D      *pt,    
    XpexVector2D     *shift, 
    XpexFloat        angle,  
    XpexVector2D     *scale, 
    XpexMatrix3X3    result)); 


EXTERN_FUNC(void, XpexCopyElements,
	(Display *dpy,
	XpexStructure  src_s_id,
	XpexElementRange  *src_range,
	XpexStructure  dest_s_id,
	XpexElementPos  *dest_pos));


EXTERN_FUNC(void, XpexCopyLookupTable,
	(Display  *dpy,
	XpexLookupTable  src_lut_id,
	XpexLookupTable  dest_lut_id));


EXTERN_FUNC(void, XpexCopyNameSet,
	(Display *dpy,
	XpexNameSet src_ns_id,
	XpexNameSet dest_ns_id));


EXTERN_FUNC(void, XpexCopyPC,
	(Display  *dpy,
	XpexPC  src, dst,
	XpexPCBitmask valuemask));


EXTERN_FUNC(void, XpexCopySC,
	(Display *dpy,
	XpexSC src, dest,
	XpexBitmask valuemask));


EXTERN_FUNC(void, XpexCopyStructure,
	(Display *dpy,
	XpexStructure  src_s_id,
	XpexStructure  dest_s_id));


EXTERN_FUNC(void, XpexDeleteBetweenLabels,
	(Display *dpy,
	XpexStructure s_id,
	long  label1, label2)); 


EXTERN_FUNC(void, XpexDeleteElements,
	(Display *dpy,
	XpexStructure s_id,
	XpexElementRange *range));


EXTERN_FUNC(void, XpexDeleteElementsToLabel,
	(Display *dpy,
	XpexStructure s_id,
	XpexElementPos *position,
	long label));


EXTERN_FUNC(void, XpexDeleteTableEntries,
	(Display  *dpy,
	XpexLookupTable  lut_id,
	XpexTableIndex  start,
	int  count));


EXTERN_FUNC(void, XpexDestroyStructures,
    (Display  *dpy,
	XpexStructure  *s_ids,
	int  count));


EXTERN_FUNC(void, XpexEndRendering,
	(Display  *dpy,
	XpexRenderer  rdr_id,
	XpexSwitch  flush_flag));


EXTERN_FUNC(void, XpexEndStructure,
	(Display *dpy,
	XpexRenderer  rdr_id));


EXTERN_FUNC(void, XpexExecuteDeferredActions,
	(Display  *dpy,
	XpexPhigsWks  wks_id));


EXTERN_FUNC(void, XpexFreeEnumTypeInfo,
	(XpexBitmask format,
	XpexListOfEnumTypeDesc *info_list,
	int count));


EXTERN_FUNC(void, XpexFreeFontInfo,
    (XpexFontInfo *font_info));


EXTERN_FUNC(void, XpexFreeFontNames,
    (char *list[]));


EXTERN_FUNC(void, XpexFreeLookupTable,
	(Display  *dpy,
	XpexLookupTable  lut_id));


EXTERN_FUNC(void, XpexFreeNameSet,
	(Display *dpy,
	XpexNameSet ns_id));


EXTERN_FUNC(void, XpexFreePC,
	(Display	*dpy,
	XpexPC	pc_id));


EXTERN_FUNC(void, XpexFreePhigsWks,
	(Display *dpy,
	XpexPhigsWks wks_id));


EXTERN_FUNC(void, XpexFreePickMeasure,
	(register Display *dpy,
	XpexPickMeasure pm_id));


EXTERN_FUNC(void, XpexFreeRenderer,
	(Display *dpy,
	XpexRenderer rdr_id));


EXTERN_FUNC(void, XpexFreeSC,
	(Display  *dpy,
	XpexSC  sc));


EXTERN_FUNC(void, XpexPostStructure,
	(Display  *dpy,
	XpexPhigsWks  wks_id,
	XpexStructure  s_id,
	XpexFloat  priority));


EXTERN_FUNC(void, XpexRedrawAllStructures,
	(Display  *dpy,
	XpexPhigsWks  wks_id));


EXTERN_FUNC(void, XpexRedrawClipRegion,
	(Display  *dpy,
	XpexPhigsWks  wks_id,
	XpexDeviceRect *dev_rects,
	int count));


EXTERN_FUNC(void , XpexRenderNetwork,
	(register Display *dpy,
	XpexRenderer rdr_id,
	Drawable drawable_id,
	XpexStructure s_id));


EXTERN_FUNC(void, XpexRenderOutputCmds,
	(register Display *dpy,
	XpexRenderer rdr_id));


EXTERN_FUNC(void, XpexRotate,
	(XpexFloat  rotationAngle,  
	register XpexMatrix3X3  transformationMatrix));  


EXTERN_FUNC(void, XpexRotateX,
    (XpexFloat  rotationAngle,        
    register XpexMatrix  transformationMatrix)); 


EXTERN_FUNC(void, XpexRotateY,
    (XpexFloat  rotationAngle,
    register XpexMatrix  transformationMatrix));


EXTERN_FUNC(void, XpexRotateZ,
    (XpexFloat  rotationAngle,         
    register XpexMatrix  transformationMatrix));


EXTERN_FUNC(void, XpexScale,
    (XpexVector3D  *scaleVector, 
    register XpexMatrix  transformationMatrix));


EXTERN_FUNC(void, XpexScale2D,
    (XpexVector2D  *scaleVector, 
    register XpexMatrix3X3  transformationMatrix));


EXTERN_FUNC(void, XpexSetColorType,
	(Display *dpy,
	XpexColorType colorType));


EXTERN_FUNC(void, XpexSetDisplayUpdateMode,
	(Display  *dpy,
	XpexPhigsWks  wks_id,
	XpexEnumTypeIndex  mode));


EXTERN_FUNC(void, XpexSetEditingMode,
    (Display  *dpy,
    XpexStructure  s_id,
    int editing_mode));


EXTERN_FUNC(void, XpexSetElementPointer,
	(Display *dpy,
	XpexStructure s_id,
	XpexElementPos *position));


EXTERN_FUNC(void, XpexSetElementPointerAtLabel,
	(Display *dpy,
	XpexStructure s_id,
	long label, offset));


EXTERN_FUNC(void, XpexSetFloatingPointFormat,
	(Display *dpy,
	XpexEnumTypeIndex fpFormat));


EXTERN_FUNC(void, XpexSetHlhsrMode,
	(Display  *dpy,
	XpexPhigsWks  wks_id,
	XpexEnumTypeIndex  mode));


EXTERN_FUNC(void, XpexSetTableEntries,
	(Display  *dpy,
	XpexLookupTable  lut_id,
	XpexTableType table_type,
	XpexTableIndex  start,
	int  count,
	char *entries,
	int length));


EXTERN_FUNC(void, XpexSetViewPriority,
	(Display  *dpy,
	XpexPhigsWks  wks_id,
	XpexTableIndex  index1,
	XpexTableIndex  index2,
	short  priority));


EXTERN_FUNC(void, XpexSetViewRep,
	(Display *dpy,
	XpexPhigsWks wks_id,
	XpexViewRep *view_rep));


EXTERN_FUNC(void, XpexSetWksBufferMode,
	(Display  *dpy,
	XpexPhigsWks  wks_id,
	int  buffer_mode)); 


EXTERN_FUNC(void, XpexSetWksViewport,
	(Display  *dpy,
	XpexPhigsWks  wks_id,
	XpexViewport  *viewport)); 


EXTERN_FUNC(void, XpexSetWksWindow,
	(Display  *dpy,
	XpexPhigsWks  wks_id,
	XpexNpcSubvolume  *npc_subvolume)); 


EXTERN_FUNC(void, XpexStoreOutputCmds,
	(register Display *dpy,
	XpexStructure s_id));


EXTERN_FUNC(void, XpexTranslate,
    (XpexVector3D          *trans_vector, 
    register XpexMatrix   m));             


EXTERN_FUNC(void, XpexTranslate2D,
    (XpexVector2D  *trans_vector,  
    register XpexMatrix3X3 m));  


EXTERN_FUNC(void , XpexUnpostAllStructures,
	(Display  *dpy,
	XpexPhigsWks  wks_id));


EXTERN_FUNC(void, XpexUnpostStructure,
	(Display  *dpy,
	XpexPhigsWks  wks_id,
	XpexStructure  s_id));


EXTERN_FUNC(void, XpexUpdatePickMeasure,
	(register Display  *dpy,
	XpexPickMeasure  pm_id,
	char  *input_record,
	int length));


EXTERN_FUNC(void, XpexUpdateWorkstation,
	(Display  *dpy,
	XpexPhigsWks  wks_id));


#endif /* defined(_Xpexfuncs_h) */
