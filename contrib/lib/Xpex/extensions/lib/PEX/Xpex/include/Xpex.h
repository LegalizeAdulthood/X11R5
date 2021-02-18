/*
 *	$Header: Xpex.h,v 2.2 91/09/11 16:05:47 sinyaw Exp $
 */

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

#ifndef XPEX_H_INCLUDED
#define XPEX_H_INCLUDED

/* Subsets */
#define XpexCompleteImplementation	PEXCompleteImplementation 
#define XpexImmediateModeOnly PEXImmediateModeOnly
#define XpexPhigsWksOnly PEXPhigsWksOnly

/* Resources */
#define XpexAlreadyFreed PEXAlreadyFreed

/* Asf Attributes */

/* Masks for setting Asf's */
#define Xpex_MarkerTypeAsf		PEXMarkerTypeAsf
#define Xpex_MarkerScaleAsf		PEXMarkerScaleAsf
#define Xpex_MarkerColorAsf		PEXMarkerColourAsf
#define Xpex_TextFontIndexAsf	PEXTextFontIndexAsf
#define Xpex_TextPrecAsf	PEXTextPrecAsf
#define Xpex_CharExpansionAsf	PEXCharExpansionAsf
#define Xpex_CharSpacingAsf		PEXCharSpacingAsf
#define Xpex_TextColorAsf		PEXTextColourAsf
#define Xpex_LineTypeAsf		PEXLineTypeAsf
#define Xpex_LineWidthAsf		PEXLineWidthAsf
#define Xpex_LineColorAsf		PEXLineColourAsf
#define Xpex_CurveApproxAsf		PEXCurveApproxAsf
#define Xpex_PolylineInterpAsf	PEXPolylineInterpAsf
#define Xpex_InteriorStyleAsf		PEXInteriorStyleAsf
#define Xpex_InteriorStyleIndexAsf	PEXInteriorStyleIndexAsf
#define Xpex_SurfaceColorAsf		PEXSurfaceColourAsf
#define Xpex_SurfaceInterpAsf		PEXSurfaceInterpAsf
#define Xpex_ReflectionModelAsf		PEXReflectionModelAsf
#define Xpex_ReflectionAttrAsf		PEXReflectionAttrAsf
#define Xpex_BfInteriorStyleAsf		PEXBfInteriorStyleAsf
#define Xpex_BfInteriorStyleIndexAsf	PEXBfInteriorStyleIndexAsf
#define Xpex_BfSurfaceColorAsf		PEXBfSurfaceColourAsf
#define Xpex_BfSurfaceInterpAsf		PEXBfSurfaceInterpAsf
#define Xpex_BfReflectionModelAsf	PEXBfReflectionModelAsf
#define Xpex_BfReflectionAttrAsf	PEXBfReflectionAttrAsf
#define Xpex_SurfaceApproxAsf		PEXSurfaceApproxAsf
#define Xpex_SurfaceEdgesAsf		PEXSurfaceEdgesAsf
#define Xpex_SurfaceEdgeTypeAsf		PEXSurfaceEdgeTypeAsf
#define Xpex_SurfaceEdgeWidthAsf	PEXSurfaceEdgeWidthAsf
#define Xpex_SurfaceEdgeColorAsf	PEXSurfaceEdgeColourAsf
#define Xpex_MaxAsfShift	PEXMaxAsfShift

/* Asf Values */
#define Xpex_Bundled			PEXBundled
#define Xpex_Individual			PEXIndividual

/* Composition */
#define Xpex_PreConcatenate		PEXPreConcatenate
#define Xpex_PostConcatenate	PEXPostConcatenate
#define Xpex_Replace			PEXReplace

/* Cull mode */
/* 0 None */
#define Xpex_BackFaces	PEXBackFaces
#define Xpex_FrontFaces	PEXFrontFaces

/* Curve Type and Surface Type */
#define Xpex_Rational	PEXRational	
#define Xpex_NonRational	PEXNonRational

/* Edit Mode */
#define XpexStructure_Insert	PEXStructureInsert
#define XpexStructure_Replace	PEXStructureReplace 

/* Whence values */
#define Xpex_Beginning	PEXBeginning
#define Xpex_Current	PEXCurrent
#define Xpex_End		PEXEnd

/* Element Search */
#define Xpex_NotFound	PEXNotFound
#define Xpex_Found	PEXFound

/* GetEnumeratedType return format */
#define XpexET_Index	PEXETIndex
#define XpexET_Mnemonic	PEXETMnemonic
#define XpexET_Both		PEXETBoth

/* Enum Types */
#define XpexET_MarkerType	PEXETMarkerType
#define XpexET_ATextStyle	PEXETATextStyle
#define XpexET_InteriorStyle	PEXETInteriorStyle
#define XpexET_HatchStyle	PEXETHatchStyle
#define XpexET_LineType		PEXETLineType
#define XpexET_SurfaceEdgeType	PEXETSurfaceEdgeType
#define XpexET_PickDeviceType	PEXETPickDeviceType
#define XpexET_PolylineInterpMethod	PEXETPolylineInterpMethod
#define XpexET_CurveApproxMethod	PEXETCurveApproxMethod
#define XpexET_ReflectionModel	PEXETReflectionModel
#define XpexET_SurfaceInterpMethod	PEXETSurfaceInterpMethod
#define XpexET_SurfaceApproxMethod	PEXETSurfaceApproxMethod
#define XpexET_ModelClipOperator	PEXETModelClipOperator
#define XpexET_LightType	PEXETLightType
#define XpexET_ColorType	PEXETColourType
#define XpexET_FloatFormat	PEXETFloatFormat
#define XpexET_HlhsrMode	PEXETHlhsrMode
#define XpexET_PromptEchoType	PEXETPromptEchoType
#define XpexET_DisplayUpdateMode	PEXETDisplayUpdateMode
#define XpexET_ColorApproxType	PEXETColourApproxType
#define XpexET_ColorApproxModel	PEXETColourApproxModel
#define XpexET_GDP	PEXETGDP
#define XpexET_GDP3	PEXETGDP3
#define XpexET_GSE	PEXETGSE
#define XpexET_TrimCurveApproxMethod	PEXETTrimCurveApproxMethod
#define XpexET_RenderingColourModel	PEXETRenderingColourModel
#define XpexET_ParaSurfCharacteristics	PEXETParaSurfCharacteristics

/* Renderer state */
#define Xpex_Idle		PEXIdle
#define Xpex_Rendering	PEXRendering

/* Flags (e.g., Switches, Visibility, and Edges) */
#define Xpex_Off	PEXOff
#define Xpex_On		PEXOn

/* Shape hints */
/* Complex, Nonconvex, Convex, are defined  as 0, 1, 2 in X.h */
#define Xpex_Complex		PEXComplex
#define Xpex_Nonconvex		PEXNonconvex
#define Xpex_Convex			PEXConvex
#define Xpex_UnknownShape	PEXUnknownShape

/* Contour hints */
#define Xpex_Disjoint		PEXDisjoint
#define Xpex_Nested			PEXNested
#define Xpex_Intersecting	PEXIntersecting
#define Xpex_UnknownContour	PEXUnknownContour

/* Table Type */
#define Xpex_LineBundleLUT		PEXLineBundleLUT
#define Xpex_MarkerBundleLUT	PEXMarkerBundleLUT
#define Xpex_TextBundleLUT		PEXTextBundleLUT
#define Xpex_InteriorBundleLUT	PEXInteriorBundleLUT
#define Xpex_EdgeBundleLUT		PEXEdgeBundleLUT
#define Xpex_PatternLUT			PEXPatternLUT
#define Xpex_TextFontLUT		PEXTextFontLUT
#define Xpex_ColorLUT			PEXColourLUT
#define Xpex_ViewLUT			PEXViewLUT
#define Xpex_LightLUT			PEXLightLUT
#define Xpex_DepthCueLUT		PEXDepthCueLUT
#define Xpex_ColorApproxLUT		PEXColourApproxLUT
#define Xpex_MaxTableType		PEXMaxTableType

/* Status in GetTableEntry */
#define Xpex_DefaultEntry	PEXDefaultEntry
#define Xpex_DefinedEntry	PEXDefinedEntry

/* ValueType in GetTableEntr{y|ies} */
#define Xpex_SetValue		PEXSetValue
#define Xpex_RealizedValue	PEXRealizedValue

/* Constants for Path and Vertical and Horizontal alignment */
#define XpexPath_Right	PEXPathRight
#define XpexPath_Left	PEXPathLeft
#define XpexPath_Up		PEXPathUp
#define XpexPath_Down	PEXPathDown

#define XpexValign_Normal	PEXValignNormal
#define XpexValign_Top		PEXValignTop
#define XpexValign_Cap		PEXValignCap
#define XpexValign_Half		PEXValignHalf
#define XpexValign_Base		PEXValignBase
#define XpexValign_Bottom	PEXValignBottom

#define XpexHalign_Normal	PEXHalignNormal
#define XpexHalign_Left		PEXHalignLeft
#define XpexHalign_Center	PEXHalignCenter
#define XpexHalign_Right	PEXHalignRight

/* Text precision */
#define Xpex_StringPrecision	PEXStringPrecision
#define Xpex_CharPrecision		PEXCharPrecision
#define Xpex_StrokePrecision	PEXStrokePrecision

/* Character Set Widths */
#define XpexCS_Byte		PEXCSByte
#define XpexCS_Short	PEXCSShort
#define XpexCS_Long		PEXCSLong

/* Update State */
#define Xpex_NotPending	PEXNotPending
#define Xpex_Pending	PEXPending

/* Visual State */
#define Xpex_Correct	PEXCorrect
#define Xpex_Deferred	PEXDeferred
#define Xpex_Simulated	PEXSimulated

/* Display State */
#define Xpex_Empty	PEXEmpty
#define Xpex_NotEmpty	PEXNotEmpty

/* Buffer Mode */
#define Xpex_SingleBuffered	PEXSingleBuffered
#define Xpex_DoubleBuffered	PEXDoubleBuffered

/* Dynamic types */
#define Xpex_IMM	PEXIMM
#define Xpex_IRG	PEXIRG
#define Xpex_CBS	PEXCBS

/* Geometric attributes (Vertex, Facet) */
#define XpexGA_None		0X0000 
#define XpexGA_Color	PEXGAColour
#define XpexGA_Normal	PEXGANormal
#define XpexGA_Edges	PEXGAEdges


/* Pick Status */
#define Xpex_NoPick	PEXNoPick
#define Xpex_Ok		PEXOk

/* Pick Echo Switch */
#define Xpex_NoEcho	PEXNoEcho
#define Xpex_Echo		PEXEcho

/* Pick Path Order */
#define Xpex_TopFirst     PEXTopFirst
#define Xpex_BottomFirst  PEXBottomFirst

/* Flags for GetStructuresInNetwork */
#define Xpex_All             PEXAll
#define Xpex_Orphans         PEXOrphans

/* Path part for GetAncestors */
#define Xpex_TopPart	PEXTopPart
#define Xpex_BottomPart	PEXBottomPart

/* Direction for ElementSearch */
#define Xpex_Forward         PEXForward
#define Xpex_Backward        PEXBackward

/* Nameset changes */
#define XpexNs_Add             PEXNSAdd
#define XpexNs_Remove          PEXNSRemove
#define XpexNs_Replace         PEXNSReplace

/* Priorities */
#define Xpex_Higher          PEXHigher
#define Xpex_Lower           PEXLower

/* Enumerated Type Descriptors */

/* Marker Type */
#define XpexMarker_Dot		PEXMarkerDot
#define XpexMarker_Cross	PEXMarkerCross
#define XpexMarker_Asterisk	PEXMarkerAsterisk
#define XpexMarker_Circle	PEXMarkerCircle
#define XpexMarker_X		PEXMarkerX

/* ATextStyle */
#define XpexAtext_NotConnected	PEXATextNotConnected
#define XpexAtext_Connected		PEXATextConnected
/* InteriorStyle */
#define XpexInteriorStyle_Hollow	PEXInteriorStyleHollow
#define XpexInteriorStyle_Solid		PEXInteriorStyleSolid
#define XpexInteriorStyle_Pattern	PEXInteriorStylePattern
#define XpexInteriorStyle_Hatch		PEXInteriorStyleHatch
#define XpexInteriorStyle_Empty		PEXInteriorStyleEmpty
/* HatchStyle */

/* LineType */
#define XpexLineType_Solid		PEXLineTypeSolid
#define XpexLineType_Dashed		PEXLineTypeDashed
#define XpexLineType_Dotted		PEXLineTypeDotted
#define XpexLineType_DashDot	PEXLineTypeDashDot

/* SurfaceEdgeType */
#define XpexSurfaceEdge_Solid	PEXSurfaceEdgeSolid
#define XpexSurfaceEdge_Dashed	PEXSurfaceEdgeDashed
#define XpexSurfaceEdge_Dotted	PEXSurfaceEdgeDotted
#define XpexSurfaceEdge_DashDot	PEXSurfaceEdgeDashDot

/* PickDeviceType */
#define	Xpex_PickDeviceDC_HitBox		PEXPickDeviceDC_HitBox
#define	Xpex_PickDeviceNPC_HitVolume	PEXPickDeviceNPC_HitVolume
/* PolylineInterpMethod */
#define Xpex_PolylineInterpNone	PEXPolylineInterpNone
#define Xpex_PolylineInterpColor	PEXPolylineInterpColour
/* Curve(and Surface)(and Trim Curve)ApproxMethods */
#define XpexApprox_ImpDep				PEXApproxImpDep
#define	XpexApprox_ConstantBetweenKnots 		PEXApproxConstantBetweenKnots
#define XpexApprox_WcsChordalSize			PEXApproxWcsChordalSize
#define XpexApprox_NpcChordalSize			PEXApproxNpcChordalSize
#define XpexApprox_DcChordalSize			PEXApproxDcChordalSize
#define XpexCurveApprox_WcsChordalDev		PEXCurveApproxWcsChordalDev
#define XpexCurveApprox_NpcChordalDev		PEXCurveApproxNpcChordalDev
#define XpexCurveApprox_DcChordalDev		PEXCurveApproxDcChordalDev
#define XpexSurfaceApprox_WcsPlanarDev		PEXSurfaceApproxWcsPlanarDev
#define XpexSurfaceApprox_NpcPlanarDev		PEXSurfaceApproxNpcPlanarDev
#define XpexSurfaceApprox_DcPlanarDev		PEXSurfaceApproxDcPlanarDev
#define XpexApprox_WcsRelative			PEXApproxWcsRelative
#define XpexApprox_NpcRelative			PEXApproxNpcRelative
#define XpexApprox_DcRelative			PEXApproxDcRelative

/* ReflectionModel */
#define XpexReflection_NoShading	PEXReflectionNoShading
#define XpexReflection_Ambient		PEXReflectionAmbient
#define XpexReflection_Diffuse		PEXReflectionDiffuse
#define XpexReflection_Specular		PEXReflectionSpecular

/* SurfaceInterpMethod */
#define XpexSurfaceInterp_None			PEXSurfaceInterpNone
#define XpexSurfaceInterp_Color			PEXSurfaceInterpColour
#define XpexSurfaceInterp_DotProduct	PEXSurfaceInterpDotProduct
#define XpexSurfaceInterp_Normal		PEXSurfaceInterpNormal

/* ModelClipOperator */
#define XpexModelClip_Replace		PEXModelClipReplace
#define XpexModelClip_Intersection	PEXModelClipIntersection

/* LightType */
#define XpexLight_Ambient		PEXLightAmbient
#define XpexLight_WcsVector		PEXLightWcsVector
#define XpexLight_WcsPoint		PEXLightWcsPoint
#define XpexLight_WcsSpot		PEXLightWcsSpot

/* ColorType */
#define Xpex_IndexedColor	PEXIndexedColour
#define Xpex_RgbFloatColor	PEXRgbFloatColour
#define Xpex_CieFloatColor	PEXCieFloatColour
#define Xpex_HsvFloatColor	PEXHsvFloatColour
#define Xpex_HlsFloatColor	PEXHlsFloatColour
#define Xpex_Rgb8Color		PEXRgb8Colour
#define Xpex_Rgb16Color		PEXRgb16Colour
#define Xpex_MaxColor		PEXMaxColour

/* FloatFormat */
#define Xpex_Ieee_754_32	PEXIeee_754_32
#define Xpex_DEC_F_Floating	PEXDEC_F_Floating
#define Xpex_Ieee_754_64	PEXIeee_754_64
#define Xpex_DEC_D_Floating	PEXDEC_D_Floating

/* HlhsrMode */
#define XpexHlhsr_Off				PEXHlhsrOff
#define XpexHlhsr_ZBuffer			PEXHlhsrZBuffer
#define XpexHlhsr_Painters			PEXHlhsrPainters
#define XpexHlhsr_Scanline			PEXHlhsrScanline
#define XpexHlhsr_HiddenLineOnly	PEXHlhsrHiddenLineOnly
/* PromptEchoType */
#define Xpex_EchoPrimitive	PEXEchoPrimitive
#define Xpex_EchoStructure	PEXEchoStructure
#define Xpex_EchoNetwork	PEXEchoNetwork

/* DisplayUpdateMethod */
#define Xpex_VisualizeEach		PEXVisualizeEach
#define Xpex_VisualizeEasy		PEXVisualizeEasy
#define Xpex_VisualizeNone		PEXVisualizeNone
#define Xpex_SimulateSome		PEXSimulateSome
#define Xpex_VisualizeWhenever	PEXVisualizeWhenever

/* ColorApproxType */
#define Xpex_ColorSpace			PEXColourSpace
#define Xpex_ColorRange			PEXColourRange

/* ColorApproxMethod */
#define XpexColorApprox_RGB		PEXColourApproxRGB
#define XpexColorApprox_CIE		PEXColourApproxCIE
#define XpexColorApprox_HSV		PEXColourApproxHSV
#define XpexColorApprox_HLS		PEXColourApproxHLS
#define XpexColorApprox_YIQ		PEXColourApproxYIQ

/* RenderingColorModel	*/
#define XpexRdrColorModel_ImpDep	PEXRdrColourModelImpDep
#define XpexRdrColorModel_RGB		PEXRdrColourModelRGB
#define XpexRdrColorModel_CIE		PEXRdrColourModelCIE
#define XpexRdrColorModel_HSV		PEXRdrColourModelHSV
#define XpexRdrColorModel_HLS		PEXRdrColourModelHLS

/* ParametricSurfaceCharacteristics */
#define	XpexPsc_None			PEXPSCNone
#define	XpexPsc_ImpDep			PEXPSCImpDep
#define	XpexPsc_IsoCurves		PEXPSCIsoCurves
#define	XpexPsc_McLevelCurves	PEXPSCMcLevelCurves
#define	XpexPsc_WcLevelCurves	PEXPSCWcLevelCurves

/* Clipping */
#define Xpex_ClipXY		PEXClipXY
#define Xpex_ClipBack	PEXClipBack
#define Xpex_ClipFront	PEXClipFront

#define Xpex_Clip		PEXClip
#define Xpex_NoClip		PEXNoClip
/* Implementation Dependent Constant Names */
#define XpexID_DitheringSupported      PEXIDDitheringSupported
#define XpexID_MaxEdgeWidth        PEXIDMaxEdgeWidth
#define XpexID_MaxLineWidth        PEXIDMaxLineWidth
#define XpexID_MaxMarkerSize       PEXIDMaxMarkerSize
#define XpexID_MaxModelClipPlanes      PEXIDMaxModelClipPlanes
#define XpexID_MaxNameSetNames         PEXIDMaxNameSetNames
#define XpexID_MaxNonAmbientLights     PEXIDMaxNonAmbientLights
#define XpexID_MaxNURBOrder        PEXIDMaxNURBOrder
#define XpexID_MaxTrimCurveOrder       PEXIDMaxTrimCurveOrder
#define XpexID_MinEdgeWidth       PEXIDMinEdgeWidth
#define XpexID_MinLineWidth       PEXIDMinLineWidth
#define XpexID_MinMarkerSize      PEXIDMinMarkerSize
#define XpexID_NominalEdgeWidth       PEXIDNominalEdgeWidth
#define XpexID_NominalLineWidth       PEXIDNominalLineWidth
#define XpexID_NominalMarkerSize      PEXIDNominalMarkerSize
#define XpexID_NumSupportedEdgeWidths PEXIDNumSupportedEdgeWidths
#define XpexID_NumSupportedLineWidths PEXIDNumSupportedLineWidths
#define XpexID_NumSupportedMarkerSizes    PEXIDNumSupportedMarkerSizes
#define XpexID_BestColourApproximation    PEXIDBestColourApproximation
#define XpexID_TransparencySupported  PEXIDTransparencySupported
#define XpexID_DoubleBufferingSupported   PEXIDDoubleBufferingSupported
#define XpexID_ChromaticityRedU       PEXIDChromaticityRedU
#define XpexID_ChromaticityRedV       PEXIDChromaticityRedV
#define XpexID_LuminanceRed       PEXIDLuminanceRed
#define XpexID_ChromaticityGreenU     PEXIDChromaticityGreenU
#define XpexID_ChromaticityGreenV     PEXIDChromaticityGreenV
#define XpexID_LuminanceGreen     PEXIDLuminanceGreen
#define XpexID_ChromaticityBlueU      PEXIDChromaticityBlueU
#define XpexID_ChromaticityBlueV      PEXIDChromaticityBlueV
#define XpexID_LuminanceBlue      PEXIDLuminanceBlue
#define XpexID_ChromaticityWhiteU     PEXIDChromaticityWhiteU
#define XpexID_ChromaticityWhiteV     PEXIDChromaticityWhiteV
#define XpexID_LuminanceWhite     PEXIDLuminanceWhite
/* Constants for IDRgbBestApproximation */
#define Xpex_ColorApproxAnyValues	PEXColourApproxAnyValues
#define Xpex_ColorApproxPowersOf2	PEXColourApproxPowersOf2

#define XpexPW_BitmaskSize	PEXMSGetWksInfo
#define XpexPC_BitmaskSize		PEXMSPipeline

/* Pipeline Context */
#define XpexPC_MarkerType				(1L<<0)
#define XpexPC_MarkerScale				(1L<<1)
#define XpexPC_MarkerColor				(1L<<2)
#define XpexPC_MarkerBundleIndex		(1L<<3)
#define XpexPC_TextFont					(1L<<4)
#define XpexPC_TextPrecision			(1L<<5)
#define XpexPC_CharExpansion			(1L<<6)
#define XpexPC_CharSpacing				(1L<<7)
#define XpexPC_TextColor				(1L<<8)
#define XpexPC_CharHeight				(1L<<9)
#define XpexPC_CharUpVector				(1L<<10)
#define XpexPC_TextPath					(1L<<11)
#define XpexPC_TextAlignment			(1L<<12)
#define XpexPC_AtextHeight				(1L<<13)
#define XpexPC_AtextUpVector			(1L<<14)
#define XpexPC_AtextPath				(1L<<15)
#define XpexPC_AtextAlignment			(1L<<16)
#define XpexPC_AtextStyle				(1L<<17)
#define XpexPC_TextBundleIndex			(1L<<18)
#define XpexPC_LineType					(1L<<19)
#define XpexPC_LineWidth				(1L<<20)
#define XpexPC_LineColor				(1L<<21)
#define XpexPC_CurveApproximation		(1L<<22)
#define XpexPC_PolylineInterp			(1L<<23)
#define XpexPC_LineBundleIndex			(1L<<24)
#define XpexPC_InteriorStyle			(1L<<25)
#define XpexPC_InteriorStyleIndex		(1L<<26)
#define XpexPC_SurfaceColor				(1L<<27)
#define XpexPC_SurfaceReflAttr			(1L<<28)
#define XpexPC_SurfaceReflModel			(1L<<29)
#define XpexPC_SurfaceInterp			(1L<<30)
#define XpexPC_BfInteriorStyle			(1L<<31)

#define XpexPC_BfInteriorStyleIndex		(1L<<0)
#define XpexPC_BfSurfaceColor			(1L<<1)
#define XpexPC_BfSurfaceReflAttr		(1L<<2)
#define XpexPC_BfSurfaceReflModel	(1L<<3)
#define XpexPC_BfSurfaceInterp	(1L<<4)
#define XpexPC_SurfaceApproximation	(1L<<5)
#define XpexPC_CullingMode			(1L<<6)
#define XpexPC_DistinguishFlag	(1L<<7)
#define XpexPC_PatternSize	(1L<<8)
#define XpexPC_PatternRefPt (1L<<9)
#define XpexPC_PatternRefVec1 (1L<<10)
#define XpexPC_PatternRefVec2 (1L<<11)
#define XpexPC_InteriorBundleIndex (1L<<12)
#define XpexPC_SurfaceEdgeFlag	(1L<<13)
#define XpexPC_SurfaceEdgeType (1L<<14)
#define XpexPC_SurfaceEdgeWidth (1L<<15)
#define XpexPC_SurfaceEdgeColor (1L<<16)
#define XpexPC_EdgeBundleIndex (1L<<17)
#define XpexPC_LocalTransform (1L<<18)
#define XpexPC_GlobalTransform (1L<<19)
#define XpexPC_ModelClip (1L<<20)
#define XpexPC_ModelClipVolume (1L<<21)
#define XpexPC_ViewIndex	(1L<<22)
#define XpexPC_LightState (1L<<23)
#define XpexPC_DepthCueIndex (1L<<24)
#define XpexPC_SetAsfValues (1L<<25)
#define XpexPC_PickId			(1L<<26)
#define XpexPC_HlhsrIdentifier	(1L<<27)
#define XpexPC_NameSet			(1L<<28)
#define XpexPC_ColorApproxIndex	(1L<<29)
#define XpexPC_RenderingColorModel	(1L<<30)
#define XpexPC_ParaSurfCharacteristics	(1L<<31)

/* Renderer Bitmasks */
#define XpexRD_PipelineContext		 PEXRDPipelineContext
#define XpexRD_CurrentPath		 PEXRDCurrentPath
#define XpexRD_MarkerBundle		 PEXRDMarkerBundle
#define XpexRD_TextBundle			 PEXRDTextBundle
#define XpexRD_LineBundle			 PEXRDLineBundle
#define XpexRD_InteriorBundle		 PEXRDInteriorBundle
#define XpexRD_EdgeBundle			 PEXRDEdgeBundle
#define XpexRD_ViewTable			 PEXRDViewTable
#define XpexRD_ColorTable		 PEXRDColourTable
#define XpexRD_DepthCueTable		 PEXRDDepthCueTable
#define XpexRD_LightTable			PEXRDLightTable
#define XpexRD_ColorApproxTable		PEXRDColourApproxTable
#define XpexRD_PatternTable		PEXRDPatternTable
#define XpexRD_TextFontTable		PEXRDTextFontTable
#define XpexRD_HighlightIncl		PEXRDHighlightIncl
#define XpexRD_HighlightExcl		PEXRDHighlightExcl
#define XpexRD_InvisibilityIncl		PEXRDInvisibilityIncl
#define XpexRD_InvisibilityExcl		PEXRDInvisibilityExcl
#define XpexRD_RendererState		PEXRDRendererState
#define XpexRD_HlhsrMode		PEXRDHlhsrMode
#define XpexRD_NpcSubvolume		PEXRDNpcSubvolume
#define XpexRD_Viewport			PEXRDViewport
#define XpexRD_ClipList			PEXRDClipList
#define XpexRD_MaxShift	PEXMaxRDShift

/* Renderer Dynamics Bitmasks */
/*	tables		      */
#define XpexDyn_MarkerBundle  PEXDynMarkerBundle
#define XpexDyn_TextBundle  PEXDynTextBundle
#define XpexDyn_LineBundle  PEXDynLineBundle
#define XpexDyn_InteriorBundle  PEXDynInteriorBundle
#define XpexDyn_EdgeBundle  PEXDynEdgeBundle
#define XpexDyn_ViewTable  PEXDynViewTable
#define XpexDyn_ColorTable  PEXDynColourTable
#define XpexDyn_DepthCueTable  PEXDynDepthCueTable
#define XpexDyn_LightTable  PEXDynLightTable
#define XpexDyn_ColorApproxTable  PEXDynColourApproxTable
#define XpexDyn_PatternTable  PEXDynPatternTable
#define XpexDyn_TextFontTable  PEXDynTextFontTable
#define XpexDyn_MarkerBundleContents  PEXDynMarkerBundleContents
#define XpexDyn_TextBundleContents  PEXDynTextBundleContents
#define XpexDyn_LineBundleContents  PEXDynLineBundleContents
#define XpexDyn_InteriorBundleContents  PEXDynInteriorBundleContents
#define XpexDyn_EdgeBundleContents  PEXDynEdgeBundleContents
#define XpexDyn_ViewTableContents  PEXDynViewTableContents
#define XpexDyn_ColorTableContents  PEXDynColourTableContents
#define XpexDyn_DepthCueTableContents  PEXDynDepthCueTableContents
#define XpexDyn_LightTableContents  PEXDynLightTableContents
#define XpexDyn_ColorApproxContents  PEXDynColourApproxContents
#define XpexDyn_PatternTableContents  PEXDynPatternTableContents
#define XpexDyn_TextFontTableContents  PEXDynTextFontTableContents
/*	namesets	      */
#define XpexDyn_HighlightNameset  PEXDynHighlightNameset
#define XpexDyn_InvisibilityNameset  PEXDynInvisibilityNameset
#define XpexDyn_HighlightNamesetContents  PEXDynHighlightNamesetContents
#define XpexDyn_InvisibilityNamesetContents  PEXDynInvisibilityNamesetContents
/*	attributes	      */
#define XpexDyn_HlhsrMode  PEXDynHlhsrMode
#define XpexDyn_NpcSubvolume  PEXDynNpcSubvolume
#define XpexDyn_Viewport  PEXDynViewport
#define XpexDyn_ClipList  PEXDynClipList

/* Search Context Bitmasks */
#define XpexSC_Position            PEXSCPosition
#define XpexSC_Distance            PEXSCDistance
#define XpexSC_Ceiling             PEXSCCeiling
#define XpexSC_ModelClipFlag       PEXSCModelClipFlag
#define XpexSC_StartPath           PEXSCStartPath
#define XpexSC_NormalList          PEXSCNormalList
#define XpexSC_InvertedList        PEXSCInvertedList
#define XpexSC_All 0x0000007f

/* Phigs Workstation Attribute Bitmasks */
#define XpexPW_DisplayUpdate	(1L<<0)
#define XpexPW_VisualState		(1L<<1)
#define XpexPW_DisplaySurface	(1L<<2)
#define XpexPW_ViewUpdate		(1L<<3)
#define XpexPW_DefinedViews		(1L<<4)
#define XpexPW_WksUpdate		(1L<<5)
#define XpexPW_ReqNpcSubvolume	(1L<<6)
#define XpexPW_CurNpcSubvolume	(1L<<7)
#define XpexPW_ReqWksViewport	(1L<<8)
#define XpexPW_CurWksViewport	(1L<<9)
#define XpexPW_HlhsrUpdate		(1L<<10)
#define XpexPW_ReqHlhsrMode		(1L<<11)
#define XpexPW_CurHlhsrMode		(1L<<12)
#define XpexPW_Drawable			(1L<<13)
#define XpexPW_MarkerBundle		(1L<<14)
#define XpexPW_TextBundle		(1L<<15)
#define XpexPW_LineBundle		(1L<<16)
#define XpexPW_InteriorBundle	(1L<<17)
#define XpexPW_EdgeBundle		(1L<<18)
#define XpexPW_ColorTable		(1L<<19)
#define XpexPW_DepthCueTable	(1L<<20)
#define XpexPW_LightTable		(1L<<21)
#define XpexPW_ColorApproxTable	(1L<<22)
#define XpexPW_PatternTable		(1L<<23)
#define XpexPW_TextFontTable	(1L<<24)
#define XpexPW_HighlightIncl	(1L<<25)
#define XpexPW_HighlightExcl	(1L<<26)
#define XpexPW_InvisibilityIncl	(1L<<27)
#define XpexPW_InvisibilityExcl	(1L<<28)
#define XpexPW_PostedStructures	(1L<<29)
#define XpexPW_NumPriorities	(1L<<30)
#define XpexPW_BufferUpdate		(1L<<31)

#define XpexPW_ReqBufferMode	(1L<<0)
#define XpexPW_CurBufferMode	(1L<<1)

/* Pick Device Bitmasks */
#define XpexPD_PickStatus			PEXPDPickStatus
#define XpexPD_PickPath				PEXPDPickPath
#define XpexPD_PickPathOrder		PEXPDPickPathOrder
#define XpexPD_PickIncl				PEXPDPickIncl
#define XpexPD_PickExcl            	PEXPDPickExcl
#define XpexPD_PickDataRec			PEXPDPickDataRec
#define XpexPD_PickPromptEchoType	PEXPDPickPromptEchoType
#define XpexPD_PickEchoVolume		PEXPDPickEchoVolume
#define XpexPD_PickEchoSwitch		PEXPDPickEchoSwitch

/* Pick Measure Bitmasks */
#define XpexPM_Status	PEXPMStatus
#define XpexPM_Path		PEXPMPath

/*************************************************
 * ERROR CODES
 *************************************************/

#define Xpex_BadColorType			PEXColourTypeError
#define Xpex_BadRendererState 		PEXRendererStateError
#define Xpex_BadFloatingPointFormat PEXFloatingPointFormatError
#define Xpex_BadLabel				PEXLabelError
#define Xpex_BadLookupTable 		PEXLookupTableError
#define Xpex_BadNameSet 			PEXNameSetError
#define Xpex_BadPath 				PEXPathError
#define Xpex_BadFont 				PEXFontError
#define Xpex_BadPhigsWks 			PEXPhigsWksError
#define Xpex_BadPickMeasure 		PEXPickMeasureError
#define Xpex_BadPipelineContext 	PEXPipelineContextError
#define Xpex_BadRenderer 			PEXRendererError
#define Xpex_BadSearchContext 		PEXSearchContextError
#define Xpex_BadStructure 			PEXStructureError
#define Xpex_BadOutputCommand 		PEXOutputCommandError
#define Xpex_MaxError				PEXMaxError
#endif /* XPEX_H_INCLUDED */
