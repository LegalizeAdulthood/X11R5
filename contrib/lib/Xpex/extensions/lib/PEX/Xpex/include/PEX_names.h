/* -*-C-*- $Header: PEX_names.h,v 1.6 91/09/11 16:05:52 sinyaw Exp $ */
#ifndef PEX_NAMES_H
#define PEX_NAMES_H

#if defined(__STDC__)
#define EXTERN_FUNC(type, name, args) extern type name args
#else
#if defined(__cplusplus)
#define EXTERN_FUNC(type, name, args) extern "C" type name args
#else
#define EXTERN_FUNC(type, name, args) extern type name ()
#endif /* __cplusplus */
#endif /* __STDC__ */

EXTERN_FUNC(char *,XpexNameOfSubset,(int));
EXTERN_FUNC(char *,XpexNameOfAsfValue,(int));
EXTERN_FUNC(char *,XpexNameOfComposition,(int));
EXTERN_FUNC(char *,XpexNameOfCullMode,(int));
EXTERN_FUNC(char *,XpexNameOfCurveType,(int));
EXTERN_FUNC(char *,XpexNameOfSurfaceType,(int));
EXTERN_FUNC(char *,XpexNameOfEditMode,(int));
EXTERN_FUNC(char *,XpexNameOfRendererState,(int));
EXTERN_FUNC(char *,XpexNameOfShape,(int));
EXTERN_FUNC(char *,XpexNameOfTableType,(int));
EXTERN_FUNC(char *,XpexNameOfTextPath,(int));
EXTERN_FUNC(char *,XpexNameOfValign,(int));
EXTERN_FUNC(char *,XpexNameOfHalign,(int));
EXTERN_FUNC(char *,XpexNameOfTextPrecision,(int));
EXTERN_FUNC(char *,XpexNameOfUpdateState,(int));
EXTERN_FUNC(char *,XpexNameOfVisualState,(int));
EXTERN_FUNC(char *,XpexNameOfMarkerType,(int));
EXTERN_FUNC(char *,XpexNameOfAtextStyle,(int));
EXTERN_FUNC(char *,XpexNameOfInteriorStyle,(int));
EXTERN_FUNC(char *,XpexNameOfLineType,(int));
EXTERN_FUNC(char *,XpexNameOfSurfaceEdgeType,(int));
EXTERN_FUNC(char *,XpexNameOfPolylineInterp,(int));
EXTERN_FUNC(char *,XpexNameOfCurveApproxMethod,(int));
EXTERN_FUNC(char *,XpexNameOfSurfaceApproxMethod,(int));
EXTERN_FUNC(char *,XpexNameOfReflectionModel,(int));
EXTERN_FUNC(char *,XpexNameOfSurfaceInterpMethod,(int));
EXTERN_FUNC(char *,XpexNameOfModelClipOperator,(int));
EXTERN_FUNC(char *,XpexNameOfColorType,(int));
EXTERN_FUNC(char *,XpexNameOfFloatingPointFormat,(int));
EXTERN_FUNC(char *,XpexNameOfDisplayUpdateMethod,(int));
EXTERN_FUNC(char *,XpexNameOfRenderingColourModel,(int));
EXTERN_FUNC(char *,XpexNameOfSCAttribute,(int));
EXTERN_FUNC(char *,XpexNameOfRequest,(int));
EXTERN_FUNC(char *,XpexNameOfOutputCommand,(int));

/* convenient macros */
#define NameOfSubset XpexNameOfSubset
#define NameOfAsfValue XpexNameOfAsfValue
#define NameOfComposition XpexNameOfComposition
#define NameOfCullMode XpexNameOfCullMode
#define NameOfCurveType XpexNameOfCurveType
#define NameOfSurfaceType XpexNameOfSurfaceType
#define NameOfEditMode XpexNameOfEditMode
#define NameOfRendererState XpexNameOfRendererState
#define NameOfShape XpexNameOfShape
#define NameOfTableType XpexNameOfTableType
#define NameOfTextPath XpexNameOfTextPath
#define NameOfValign XpexNameOfValign
#define NameOfHalign XpexNameOfHalign
#define NameOfTextPrecision XpexNameOfTextPrecision
#define NameOfUpdateState XpexNameOfUpdateState
#define NameOfVisualState XpexNameOfVisualState
#define NameOfMarkerType XpexNameOfMarkerType
#define NameOfAtextStyle XpexNameOfAtextStyle
#define NameOfInteriorStyle XpexNameOfInteriorStyle
#define NameOfLineType XpexNameOfLineType
#define NameOfSurfaceEdgeType XpexNameOfSurfaceEdgeType
#define NameOfPolylineInterp XpexNameOfPolylineInterp
#define NameOfCurveApproxMethod XpexNameOfCurveApproxMethod
#define NameOfSurfaceApproxMethod XpexNameOfSurfaceApproxMethod
#define NameOfReflectionModel XpexNameOfReflectionModel
#define NameOfSurfaceInterpMethod XpexNameOfSurfaceInterpMethod
#define NameOfModelClipOperator XpexNameOfModelClipOperator
#define NameOfColorType XpexNameOfColorType
#define NameOfFloatingPointFormat XpexNameOfFloatingPointFormat
#define NameOfDisplayUpdateMethod XpexNameOfDisplayUpdateMethod
#define NameOfRenderingColourModel XpexNameOfRenderingColourModel
#define NameOfSCAttribute XpexNameOfSCAttribute
#define NameOfRequest XpexNameOfRequest
#define NameOfOutputCommand XpexNameOfOutputCommand

#endif /* PEX_NAMES_H */

