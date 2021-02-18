/* -*-C-*- */
#ifndef lint
static char rcsid[] = "$Header: XpexNames.c,v 1.2 91/09/11 16:06:23 sinyaw Exp $";
#endif

#include <stdlib.h>
#include "PEX.h"

typedef struct {
    int code;
    char *name;
} Table_data;


typedef struct {
#define TABLE_SORTED 1
    int flags;
    Table_data *data;
} Table;

#define IS_TABLE_SORTED(f) ((f) & TABLE_SORTED)
#define SET_TABLE_SORTED(f) ((f) |= TABLE_SORTED)

static int table_cmp(t1, t2)
    Table_data *t1;
    Table_data *t2;
{
    return t1->code - t2->code;
}

static int table_sort(tbl, size)
    Table *tbl;
    int size;
{
    qsort(tbl->data, size, sizeof(Table_data), table_cmp);
}


char *table_search(code, tbl, size)
    int code;
    Table *tbl;
    int size;
{
    int i;
    if (!IS_TABLE_SORTED(tbl->flags)) {
	table_sort(tbl, size);
	SET_TABLE_SORTED(tbl->flags);
    }

    if (code < size && tbl->data[code].code == code)
      return tbl->data[code].name;

    tbl->data[size-1].code = code;
    /* should engage at least binary search */
    for (i = 0; i < size - 1 && code != tbl->data[i].code; i++)
      /* empty */;

    return tbl->data[i].name;
}



char *XpexNameOfSubset(s) int s;
{
#define Subset_SIZE (sizeof(Subset_Table_data) / sizeof(Table_data))
    static Table_data Subset_Table_data[] = {
	{PEXCompleteImplementation, "CompleteImplementation"},
	{PEXImmediateModeOnly, "ImmediateModeOnly"},
	{PEXPhigsWksOnly, "PhigsWksOnly"},
	{0, "Error"}};
    static Table Subset_Table = {0, Subset_Table_data};

    return table_search(s, &Subset_Table, Subset_SIZE);
}

char *XpexNameOfAsfValue(val)
int val;
{
#define AsfValue_SIZE (sizeof(AsfValue_Table_data) / sizeof(Table_data))
    static Table_data AsfValue_Table_data[] = {
	{PEXBundled, "Bundled"},
	{PEXIndividual, "Individual"},
	{0, "Error"}};
    static Table AsfValue_Table = {0, AsfValue_Table_data};
    return table_search(val, &AsfValue_Table, AsfValue_SIZE);
}

char *XpexNameOfComposition(val)
int val;
{
#define Composition_SIZE (sizeof(Composition_Table_data) / sizeof(Table_data))
    static Table_data     Composition_Table_data[] = {
	{PEXPreConcatenate, "PreConcatenate"},
	{PEXPostConcatenate, "PostConcatenate"},
	{PEXReplace, "Replace"},
	{0, "Error"}};
    static Table Composition_Table = {0, Composition_Table_data};
    return table_search(val, &Composition_Table, Composition_SIZE);
}


char *XpexNameOfCullMode(val)
int val;
{
#define CullMode_SIZE (sizeof(CullMode_Table_data) / sizeof(Table_data))
    static Table_data CullMode_Table_data[] = {
	{0, "None"},
	{PEXBackFaces, "BackFaces"},
	{PEXFrontFaces, "FrontFaces"},
	{0, "Error"}};
    static Table CullMode_Table = {0, CullMode_Table_data};
    return table_search(val, &CullMode_Table, CullMode_SIZE);
}

char *XpexNameOfCurveType(val)
 int val;
{
#define CurveType_SIZE (sizeof(CurveType_Table_data) / sizeof(Table_data))
    static Table_data CurveType_Table_data[] = {
	{PEXRational, "Rational"},
	{PEXNonRational, "NonRational"},
	{0, "Error"}};
    static Table CurveType_Table = {0, CurveType_Table_data};
    return table_search(val, &CurveType_Table, CurveType_SIZE);
}

char *XpexNameOfSurfaceType(val)
 int val;
{
#define SurfaceType_SIZE (sizeof(SurfaceType_Table_data) / sizeof(Table_data))
    static Table_data SurfaceType_Table_data[] = {
	{PEXRational, "Rational"},
	{PEXNonRational, "NonRational"},
	{0, "Error"}};
    static Table SurfaceType_Table = {0, SurfaceType_Table_data};
    return table_search(val, &SurfaceType_Table, SurfaceType_SIZE);
}

char *XpexNameOfEditMode(val)
 int val;
{
#define EditMode_SIZE (sizeof(EditMode_Table_data) / sizeof(Table_data))
    static Table_data EditMode_Table_data[] = {
	{PEXStructureInsert, "StructureInsert"},
	{PEXStructureReplace, "StructureReplace"},
	{0, "Error"}};
    static Table EditMode_Table = {0, EditMode_Table_data};
    return table_search(val, &EditMode_Table, EditMode_SIZE);
}

char *XpexNameOfRendererState(val)
 int val;
{
#define RendererState_SIZE (sizeof(RendererState_Table_data) / sizeof(Table_data))
    static Table_data RendererState_Table_data[] = {
	{PEXIdle, "Idle"},
	{PEXRendering, "Rendering"},
	{0, "Error"}};
    static Table RendererState_Table = {0, RendererState_Table_data};
    return table_search(val, &RendererState_Table, RendererState_SIZE);
}

char *XpexNameOfShape(val)
 int val;
{
#define Shape_SIZE (sizeof(Shape_Table_data) / sizeof(Table_data))
    static Table_data Shape_Table_data[] = {
	{PEXComplex, "Complex"},
	{PEXNonconvex, "Nonconvex"},
	{PEXConvex, "Convex"},
	{PEXUnknownShape, "UnknownShape"},
	{0, "Error"}};
    static Table Shape_Table = {0, Shape_Table_data};
    return table_search(val, &Shape_Table, Shape_SIZE);
}

char *XpexNameOfTableType(type)
 int type;
{
#define TableType_SIZE (sizeof(TableType_Table_data) / sizeof(Table_data))
    static Table_data TableType_Table_data[] = {
	{PEXLineBundleLUT, "LineBundle"},
	{PEXMarkerBundleLUT, "MarkerBundle"},
	{PEXTextBundleLUT, "TextBundle"},
	{PEXInteriorBundleLUT, "InteriorBundle"},
	{PEXEdgeBundleLUT, "EdgeBundle"},
	{PEXPatternLUT, "Pattern"},
	{PEXTextFontLUT, "TextFont"},
	{PEXColourLUT, "Color"},
	{PEXViewLUT, "View"},
	{PEXLightLUT, "Light"},
	{PEXDepthCueLUT, "DepthCue"},
	{PEXColourApproxLUT, "ColorApprox"},
	{0, "Error"}};
    static Table TableType_Table = {0, TableType_Table_data};
    return table_search(type, &TableType_Table, TableType_SIZE);
}

char *XpexNameOfTextPath(tp)
 int tp;
{
#define TextPath_SIZE (sizeof(TextPath_Table_data) / sizeof(Table_data))
    static Table_data TextPath_Table_data[] = {
	{PEXPathRight, "PathRight"},
	{PEXPathLeft, "PathLeft"},
	{PEXPathUp, "PathUp"},
	{PEXPathDown, "PathDown"},
	{0, "Error"}};
    static Table TextPath_Table = {0, TextPath_Table_data};
    return table_search(tp, &TextPath_Table, TextPath_SIZE);
}

char *XpexNameOfValign(v)
 int v;
{
#define Valign_SIZE (sizeof(Valign_Table_data) / sizeof(Table_data))
    static Table_data Valign_Table_data[] = {
	{PEXValignNormal, "ValignNormal"},
	{PEXValignTop, "ValignTop"},
	{PEXValignCap, " ValignCap"},
	{PEXValignHalf, "ValignHalf"},
	{PEXValignBase, " ValignBase"},
	{PEXValignBottom, "ValignBottom"},
	{0, "Error"}};
    static Table Valign_Table = {0, Valign_Table_data};
    return table_search(v, &Valign_Table, Valign_SIZE);
}

char *XpexNameOfHalign(h)
 int h;
{
#define Halign_SIZE (sizeof(Halign_Table_data) / sizeof(Table_data))
    static Table_data Halign_Table_data[] = {
	{PEXHalignNormal, "HalignNormal"},
	{PEXHalignLeft, "HalignLeft"},
	{PEXHalignCenter, "HalignCenter"},
	{PEXHalignRight, "HalignRight"},
	{0, "Error"}};
    static Table Halign_Table = {0, Halign_Table_data};
    return table_search(h, &Halign_Table, Halign_SIZE);
}

char *XpexNameOfTextPrecision(tp)
 int tp;
{
#define TextPrecision_SIZE (sizeof(TextPrecision_Table_data) / sizeof(Table_data))
    static Table_data TextPrecision_Table_data[] = {
	{PEXStringPrecision, "String"},
	{PEXCharPrecision, "Char"},
	{PEXStrokePrecision, "Stroke"},
	{0, "Error"}};
    static Table TextPrecision_Table = {0, TextPrecision_Table_data};
    return table_search(tp, &TextPrecision_Table, TextPrecision_SIZE);
}

char *XpexNameOfUpdateState(us)
 int us;
{
#define UpdateState_SIZE (sizeof(UpdateState_Table_data) / sizeof(Table_data))
    static Table_data UpdateState_Table_data[] = {
	{PEXNotPending, "NotPending"},
	{PEXPending, "Pending"},
	{0, "Error"}};
    static Table UpdateState_Table = {0, UpdateState_Table_data};
    return table_search(us, &UpdateState_Table, UpdateState_SIZE);
}

char *XpexNameOfVisualState(vs)
 int vs;
{
#define VisualState_SIZE (sizeof(VisualState_Table_data) / sizeof(Table_data))
    static Table_data VisualState_Table_data[] = {
	{PEXCorrect, "Correct"},
	{PEXDeferred, "Deferred"},
	{PEXSimulated, "Simulated"},
	{0, "Error"}};
    static Table VisualState_Table = {0, VisualState_Table_data};
    return table_search(vs, &VisualState_Table, VisualState_SIZE);
}

char *XpexNameOfMarkerType(mt)
 int mt;
{
#define MarkerType_SIZE (sizeof(MarkerType_Table_data) / sizeof(Table_data))
    static Table_data MarkerType_Table_data[] = {
	{PEXMarkerDot, "MarkerDot"},
	{PEXMarkerCross, "MarkerCross"},
	{PEXMarkerAsterisk, "MarkerAsterisk"},
	{PEXMarkerCircle, "MarkerCircle"},
	{PEXMarkerX, "MarkerX"},
	{0, "Error"}};
    static Table MarkerType_Table = {0, MarkerType_Table_data};
    return table_search(mt, &MarkerType_Table, MarkerType_SIZE);
}

char *XpexNameOfAtextStyle(ats)
 int ats;
{
#define AtextStyle_SIZE (sizeof(AtextStyle_Table_data) / sizeof(Table_data))
    static Table_data AtextStyle_Table_data[] = {
	{PEXATextNotConnected, "ATextNotConnected"},
	{PEXATextConnected, "ATextConnected"},
	{0, "Error"}};
    static Table AtextStyle_Table = {0, AtextStyle_Table_data};
    return table_search(ats, &AtextStyle_Table, AtextStyle_SIZE);
}

char *XpexNameOfInteriorStyle(is)
 int is;
{
#define InteriorStyle_SIZE (sizeof(InteriorStyle_Table_data) / sizeof(Table_data))
    static Table_data InteriorStyle_Table_data[] = {
	{PEXInteriorStyleHollow, "InteriorStyleHollow"},
	{PEXInteriorStyleSolid, "InteriorStyleSolid"},
	{PEXInteriorStylePattern, "InteriorStylePattern"},
	{PEXInteriorStyleHatch, "InteriorStyleHatch"},
	{PEXInteriorStyleEmpty, "InteriorStyleEmpty"},
	{0, "Error"}};
    static Table InteriorStyle_Table = {0, InteriorStyle_Table_data};
    return table_search(is, &InteriorStyle_Table, InteriorStyle_SIZE);
}

char *XpexNameOfLineType(lt)
 int lt;
{
#define LineType_SIZE (sizeof(LineType_Table_data) / sizeof(Table_data))
    static Table_data LineType_Table_data[] = {
	{PEXLineTypeSolid, "LineTypeSolid"},
	{PEXLineTypeDashed, "LineTypeDashed"},
	{PEXLineTypeDotted, "LineTypeDotted"},
	{PEXLineTypeDashDot, "LineTypeDashDot"},
	{0, "Error"}};
    static Table LineType_Table = {0, LineType_Table_data};
    return table_search(lt, &LineType_Table, LineType_SIZE);
}

char *XpexNameOfSurfaceEdgeType(et)
 int et;
{
#define SurfaceEdgeType_SIZE (sizeof(SurfaceEdgeType_Table_data) / sizeof(Table_data))
    static Table_data SurfaceEdgeType_Table_data[] = {
	{PEXSurfaceEdgeSolid, "Solid"},
	{PEXSurfaceEdgeDashed, "Dashed"},
	{PEXSurfaceEdgeDotted, "Dotted"},
	{PEXSurfaceEdgeDashDot, "DashDot"},
	{0, "Error"}};
    static Table SurfaceEdgeType_Table = {0, SurfaceEdgeType_Table_data};
    return table_search(et, &SurfaceEdgeType_Table, SurfaceEdgeType_SIZE);
}

char *XpexNameOfPolylineInterp(pi)
 int pi;
{
#define PolylineInterp_SIZE (sizeof(PolylineInterp_Table_data) / sizeof(Table_data))
    static Table_data PolylineInterp_Table_data[] = {
	{PEXPolylineInterpNone, "PolylineInterpNone"},
	{PEXPolylineInterpColour, "PolylineInterpColour"},
	{0, "Error"}};
    static Table PolylineInterp_Table = {0, PolylineInterp_Table_data};
    return table_search(pi, &PolylineInterp_Table, PolylineInterp_SIZE);
}

char *XpexNameOfCurveApproxMethod(cam)
 int cam;
{
#define CurveApproxMethod_SIZE (sizeof(CurveApproxMethod_Table_data) / sizeof(Table_data))
    static Table_data CurveApproxMethod_Table_data[] = {
	{PEXApproxImpDep, "ImpDep"},
	{PEXApproxConstantBetweenKnots, "ConstantBetweenKnots"},
	{PEXApproxWcsChordalSize, "WcsChordalSize"},
	{PEXApproxNpcChordalSize, "NpcChordalSize"},
	{PEXApproxDcChordalSize, "DcChordalSize"},
	{PEXCurveApproxWcsChordalDev, "WcsChordalDev"},
	{PEXCurveApproxNpcChordalDev, "NpcChordalDev"},
	{PEXCurveApproxDcChordalDev, "DcChordalDev"},
	{PEXApproxWcsRelative, "WcsRelative"},
	{PEXApproxNpcRelative, "NpcRelative"},
	{PEXApproxDcRelative, "DcRelative"},
	{0, "Error"}};
    static Table CurveApproxMethod_Table = {0, CurveApproxMethod_Table_data};
    return table_search(cam, &CurveApproxMethod_Table, CurveApproxMethod_SIZE);
}

char *XpexNameOfSurfaceApproxMethod(sam)
 int sam;
{
#define SurfaceApproxMethod_SIZE (sizeof(SurfaceApproxMethod_Table_data) / sizeof(Table_data))
    static Table_data SurfaceApproxMethod_Table_data[] = {
	{PEXApproxImpDep, "ImpDep"},
	{PEXApproxConstantBetweenKnots, "ConstantBetweenKnots"},
	{PEXApproxWcsChordalSize, "WcsChordalSize"},
	{PEXApproxNpcChordalSize, "NpcChordalSize"},
	{PEXApproxDcChordalSize, "DcChordalSize"},
	{PEXSurfaceApproxWcsPlanarDev, "WcsPlanarDev"},
	{PEXSurfaceApproxNpcPlanarDev, "NpcPlanarDev"},
	{PEXSurfaceApproxDcPlanarDev, "DcPlanarDev"},
	{PEXApproxWcsRelative, "WcsRelative"},
	{PEXApproxNpcRelative, "NpcRelative"},
	{PEXApproxDcRelative, "DcRelative"},
	{0, "Error"}};
    static Table SurfaceApproxMethod_Table = {0, SurfaceApproxMethod_Table_data};
    return table_search(sam, &SurfaceApproxMethod_Table, SurfaceApproxMethod_SIZE);
}

char *XpexNameOfReflectionModel(m)
 int m;
{
#define ReflectionModel_SIZE (sizeof(ReflectionModel_Table_data) / sizeof(Table_data))
    static Table_data ReflectionModel_Table_data[] = {
	{PEXReflectionNoShading, "NoShading"},
	{PEXReflectionAmbient, "Ambient"},
	{PEXReflectionDiffuse, "Diffuse"},
	{PEXReflectionSpecular, "Specular"},
	{0, "Error"}};
    static Table ReflectionModel_Table = {0, ReflectionModel_Table_data};
    return table_search(m, &ReflectionModel_Table, ReflectionModel_SIZE);
}

char *XpexNameOfSurfaceInterpMethod(m)
 int m;
{
#define SurfaceInterpMethod_SIZE (sizeof(SurfaceInterpMethod_Table_data) / sizeof(Table_data))
    static Table_data SurfaceInterpMethod_Table_data[] = {
	{PEXSurfaceInterpNone, "None"},
	{PEXSurfaceInterpColour, "Colour"},
	{PEXSurfaceInterpDotProduct, "DotProduct"},
	{PEXSurfaceInterpNormal, "Normal"},
	{0, "Error"}};
    static Table SurfaceInterpMethod_Table = {0, SurfaceInterpMethod_Table_data};
    return table_search(m, &SurfaceInterpMethod_Table, SurfaceInterpMethod_SIZE);
}

char *XpexNameOfModelClipOperator(mco)
 int mco;
{
#define ModelClipOperator_SIZE (sizeof(ModelClipOperator_Table_data) / sizeof(Table_data))
    static Table_data ModelClipOperator_Table_data[] = {
	{PEXModelClipReplace, "Replace"},
	{PEXModelClipIntersection, "Intersection"},
	{0, "Error"}};
    static Table ModelClipOperator_Table = {0, ModelClipOperator_Table_data};
    return table_search(mco, &ModelClipOperator_Table, ModelClipOperator_SIZE);
}

char *XpexNameOfColorType(t)
 int t;
{
#define ColorType_SIZE (sizeof(ColorType_Table_data) / sizeof(Table_data))
    static Table_data ColorType_Table_data[] = {
	{PEXIndexedColour, "Indexed"},
	{PEXRgbFloatColour, "RgbFloat"},
	{PEXCieFloatColour, "CieFloat"},
	{PEXHsvFloatColour, "HsvFloat"},
	{PEXHlsFloatColour, "HlsFloat"},
	{PEXRgb8Colour, "Rgb8"},
	{PEXRgb16Colour, "Rgb16"},
	{0, "Error"}};
    static Table ColorType_Table = {0, ColorType_Table_data};
    return table_search(t, &ColorType_Table, ColorType_SIZE);
}

char *XpexNameOfFloatingPointFormat(t)
 int t;
{
#define FloatingPointFormat_SIZE (sizeof(FloatingPointFormat_Table_data) / sizeof(Table_data))
    static Table_data FloatingPointFormat_Table_data[] = {
	{PEXIeee_754_32, "Ieee_754_32"},
	{PEXDEC_F_Floating, "DEC_F_Floating"},
	{PEXIeee_754_64, "Ieee_754_64"},
	{PEXDEC_D_Floating, "DEC_D_Floating"},
	{0, "Error"}};
    static Table FloatingPointFormat_Table = {0, FloatingPointFormat_Table_data};
    return table_search(t, &FloatingPointFormat_Table, FloatingPointFormat_SIZE);
}

char *XpexNameOfDisplayUpdateMethod(val)
 int val;
{
#define DisplayUpdateMethod_SIZE (sizeof(DisplayUpdateMethod_Table_data) / sizeof(Table_data))
    static Table_data DisplayUpdateMethod_Table_data[] = {
	{PEXVisualizeEach, "VisualizeEach"},
	{PEXVisualizeEasy, "VisualizeEasy"},
	{PEXVisualizeNone, "VisualizeNone"},
	{PEXSimulateSome, "SimulateSome"},
	{PEXVisualizeWhenever, "VisualizeWhenever"},
	{0, "Error"}};
    static Table DisplayUpdateMethod_Table = {0, DisplayUpdateMethod_Table_data};
    return table_search(val, &DisplayUpdateMethod_Table, DisplayUpdateMethod_SIZE);
}

char *XpexNameOfRenderingColourModel(model)
 int model;
{
#define RenderingColourModel_SIZE (sizeof(RenderingColourModel_Table_data) / sizeof(Table_data))
  static Table_data RenderingColourModel_Table_data[] = {
      {PEXRdrColourModelImpDep, "ImpDep"},
      {PEXRdrColourModelRGB, "RGB"},
      {PEXRdrColourModelCIE, "CIE"},
      {PEXRdrColourModelHSV, "HSV"},
      {PEXRdrColourModelHLS, "HLS"},
      {0, "Error"}};
    static Table RenderingColourModel_Table = {0, RenderingColourModel_Table_data};
    return table_search(model, &RenderingColourModel_Table, RenderingColourModel_SIZE);
}

char *XpexNameOfSCAttribute(attr)
 int attr;
{
#define SCAttribute_SIZE (sizeof(SCAttribute_Table_data) / sizeof(Table_data))
    static Table_data SCAttribute_Table_data[] = {
	{PEXSCPosition, "SearchPosition"},
	{PEXSCDistance, "SearchDistance"},
	{PEXSCCeiling, "SearchCeiling"},
	{PEXSCModelClipFlag, "ModelClipFlag"},
	{PEXSCStartPath, "StartPath"},
	{PEXSCNormalList, "NormalList"},
	{PEXSCInvertedList, "InvertedList"},
	{0, "Error"}};
    static Table SCAttribute_Table = {0, SCAttribute_Table_data};
    return table_search(attr, &SCAttribute_Table, SCAttribute_SIZE);
}

char *XpexNameOfRequest(req)
 int req;
{
#define Request_SIZE (sizeof(Request_Table_data) / sizeof(Table_data))
    static Table_data Request_Table_data[] = {
	{PEX_GetExtensionInfo, "GetExtensionInfo"},
	{PEX_GetEnumeratedTypeInfo, "GetEnumeratedTypeInfo"},
	{PEX_GetImpDepConstants, "GetImpDepConstants"},
	{PEX_CreateLookupTable, "CreateLookupTable"},
	{PEX_CopyLookupTable, "CopyLookupTable"},
	{PEX_FreeLookupTable, "FreeLookupTable"},
	{PEX_GetTableInfo, "GetTableInfo"},
	{PEX_GetPredefinedEntries, "GetPredefinedEntries"},
	{PEX_GetDefinedIndices, "GetDefinedIndices"},
	{PEX_GetTableEntry, "GetTableEntry"},
	{PEX_GetTableEntries, "GetTableEntries"},
	{PEX_SetTableEntries, "SetTableEntries"},
	{PEX_DeleteTableEntries, "DeleteTableEntries"},
	{PEX_CreatePipelineContext, "CreatePipelineContext"},
	{PEX_CopyPipelineContext, "CopyPipelineContext"},
	{PEX_FreePipelineContext, "FreePipelineContext"},
	{PEX_GetPipelineContext, "GetPipelineContext"},
	{PEX_ChangePipelineContext, "ChangePipelineContext"},
	{PEX_CreateRenderer, "CreateRenderer"},
	{PEX_FreeRenderer, "FreeRenderer"},
	{PEX_ChangeRenderer, "ChangeRenderer"},
	{PEX_GetRendererAttributes, "GetRendererAttributes"},
	{PEX_GetRendererDynamics, "GetRendererDynamics"},
	{PEX_BeginRendering, "BeginRendering"},
	{PEX_EndRendering, "EndRendering"},
	{PEX_BeginStructure, "BeginStructure"},
	{PEX_EndStructure, "EndStructure"},
	{PEX_RenderOutputCommands, "RenderOutputCommands"},
	{PEX_RenderNetwork, "RenderNetwork"},
	{PEX_CreateStructure, "CreateStructure"},
	{PEX_CopyStructure, "CopyStructure"},
	{PEX_DestroyStructures, "DestroyStructures"},
	{PEX_GetStructureInfo, "GetStructureInfo"},
	{PEX_GetElementInfo, "GetElementInfo"},
	{PEX_GetStructuresInNetwork, "GetStructuresInNetwork"},
	{PEX_GetAncestors, "GetAncestors"},
	{PEX_GetDescendants, "GetDescendants"},
	{PEX_FetchElements, "FetchElements"},
	{PEX_SetEditingMode, "SetEditingMode"},
	{PEX_SetElementPointer, "SetElementPointer"},
	{PEX_SetElementPointerAtLabel, "SetElementPointerAtLabel"},
	{PEX_ElementSearch, "ElementSearch"},
	{PEX_StoreElements, "StoreElements"},
	{PEX_DeleteElements, "DeleteElements"},
	{PEX_DeleteElementsToLabel, "DeleteElementsToLabel"},
	{PEX_DeleteBetweenLabels, "DeleteBetweenLabels"},
	{PEX_CopyElements, "CopyElements"},
	{PEX_ChangeStructureRefs, "ChangeStructureRefs"},
	{PEX_CreateNameSet, "CreateNameSet"},
	{PEX_CopyNameSet, "CopyNameSet"},
	{PEX_FreeNameSet, "FreeNameSet"},
	{PEX_GetNameSet, "GetNameSet"},
	{PEX_ChangeNameSet, "ChangeNameSet"},
	{PEX_CreateSearchContext, "CreateSearchContext"},
	{PEX_CopySearchContext, "CopySearchContext"},
	{PEX_FreeSearchContext, "FreeSearchContext"},
	{PEX_GetSearchContext, "GetSearchContext"},
	{PEX_ChangeSearchContext, "ChangeSearchContext"},
	{PEX_SearchNetwork, "SearchNetwork"},
	{PEX_CreatePhigsWks, "CreatePhigsWks"},
	{PEX_FreePhigsWks, "FreePhigsWks"},
	{PEX_GetWksInfo, "GetWksInfo"},
	{PEX_GetDynamics, "GetDynamics"},
	{PEX_GetViewRep, "GetViewRep"},
	{PEX_RedrawAllStructures, "RedrawAllStructures"},
	{PEX_UpdateWorkstation, "UpdateWorkstation"},
	{PEX_RedrawClipRegion, "RedrawClipRegion"},
	{PEX_ExecuteDeferredActions, "ExecuteDeferredActions"},
	{PEX_SetViewPriority, "SetViewPriority"},
	{PEX_SetDisplayUpdateMode, "SetDisplayUpdateMode"},
	{PEX_MapDCtoWC, "MapDCtoWC"},
	{PEX_MapWCtoDC, "MapWCtoDC"},
	{PEX_SetViewRep, "SetViewRep"},
	{PEX_SetWksWindow, "SetWksWindow"},
	{PEX_SetWksViewport, "SetWksViewport"},
	{PEX_SetHlhsrMode, "SetHlhsrMode"},
	{PEX_SetWksBufferMode, "SetWksBufferMode"},
	{PEX_PostStructure, "PostStructure"},
	{PEX_UnpostStructure, "UnpostStructure"},
	{PEX_UnpostAllStructures, "UnpostAllStructures"},
	{PEX_GetWksPostings, "GetWksPostings"},
	{PEX_GetPickDevice, "GetPickDevice"},
	{PEX_ChangePickDevice, "ChangePickDevice"},
	{PEX_CreatePickMeasure, "CreatePickMeasure"},
	{PEX_FreePickMeasure, "FreePickMeasure"},
	{PEX_GetPickMeasure, "GetPickMeasure"},
	{PEX_UpdatePickMeasure, "UpdatePickMeasure"},
	{PEX_OpenFont, "OpenFont"},
	{PEX_CloseFont, "CloseFont"},
	{PEX_QueryFont, "QueryFont"},
	{PEX_ListFonts, "ListFonts"},
	{PEX_ListFontsWithInfo, "ListFontsWithInfo"},
	{PEX_QueryTextExtents, "QueryTextExtents"},
	{0, "Error"}};
    static Table Request_Table = {0, Request_Table_data};
    return table_search(req, &Request_Table, Request_SIZE);
}

char *XpexNameOfOutputCommand(oc)
 int oc;
{
#define OutputCommand_SIZE (sizeof(OutputCommand_Table_data) / sizeof(Table_data))
    static Table_data OutputCommand_Table_data[] = {
	{PEXOCMarkerType, "MarkerType"},
	{PEXOCMarkerScale, "MarkerScale"},
	{PEXOCMarkerColourIndex, "MarkerColourIndex"},
	{PEXOCMarkerColour, "MarkerColour"},
	{PEXOCMarkerBundleIndex, "MarkerBundleIndex"},
	{PEXOCTextFontIndex, "TextFontIndex"},
	{PEXOCTextPrecision, "TextPrecision"},
	{PEXOCCharExpansion, "CharExpansion"},
	{PEXOCCharSpacing, "CharSpacing"},
	{PEXOCTextColourIndex, "TextColourIndex"},
	{PEXOCTextColour, "TextColour"},
	{PEXOCCharHeight, "CharHeight"},
	{PEXOCCharUpVector, "CharUpVector"},
	{PEXOCTextPath, "TextPath"},
	{PEXOCTextAlignment, "TextAlignment"},
	{PEXOCAtextHeight, "AtextHeight"},
	{PEXOCAtextUpVector, "AtextUpVector"},
	{PEXOCAtextPath, "AtextPath"},
	{PEXOCAtextAlignment, "AtextAlignment"},
	{PEXOCAtextStyle, "AtextStyle"},
	{PEXOCTextBundleIndex, "TextBundleIndex"},
	{PEXOCLineType, "LineType"},
	{PEXOCLineWidth, "LineWidth"},
	{PEXOCLineColourIndex, "LineColourIndex"},
	{PEXOCLineColour, "LineColour"},
	{PEXOCCurveApproximation, "CurveApproximation"},
	{PEXOCPolylineInterp, "PolylineInterp"},
	{PEXOCLineBundleIndex, "LineBundleIndex"},
	{PEXOCInteriorStyle, "InteriorStyle"},
	{PEXOCInteriorStyleIndex, "InteriorStyleIndex"},
	{PEXOCSurfaceColourIndex, "SurfaceColourIndex"},
	{PEXOCSurfaceColour, "SurfaceColour"},
	{PEXOCSurfaceReflAttr, "SurfaceReflAttr"},
	{PEXOCSurfaceReflModel, "SurfaceReflModel"},
	{PEXOCSurfaceInterp, "SurfaceInterp"},
	{PEXOCBfInteriorStyle, "BfInteriorStyle"},
	{PEXOCBfInteriorStyleIndex, "BfInteriorStyleIndex"},
	{PEXOCBfSurfaceColourIndex, "BfSurfaceColourIndex"},
	{PEXOCBfSurfaceColour, "BfSurfaceColour"},
	{PEXOCBfSurfaceReflAttr, "BfSurfaceReflAttr"},
	{PEXOCBfSurfaceReflModel, "BfSurfaceReflModel"},
	{PEXOCBfSurfaceInterp, "BfSurfaceInterp"},
	{PEXOCSurfaceApproximation, "SurfaceApproximation"},
	{PEXOCCullingMode, "CullingMode"},
	{PEXOCDistinguishFlag, "DistinguishFlag"},
	{PEXOCPatternSize, "PatternSize"},
	{PEXOCPatternRefPt, "PatternRefPt"},
	{PEXOCPatternAttr, "PatternAttr"},
	{PEXOCInteriorBundleIndex, "InteriorBundleIndex"},
	{PEXOCSurfaceEdgeFlag, "SurfaceEdgeFlag"},
	{PEXOCSurfaceEdgeType, "SurfaceEdgeType"},
	{PEXOCSurfaceEdgeWidth, "SurfaceEdgeWidth"},
	{PEXOCSurfaceEdgeColourIndex, "SurfaceEdgeColourIndex"},
	{PEXOCSurfaceEdgeColour, "SurfaceEdgeColour"},
	{PEXOCEdgeBundleIndex, "EdgeBundleIndex"},
	{PEXOCSetAsfValues, "SetAsfValues"},
	{PEXOCLocalTransform, "LocalTransform"},
	{PEXOCLocalTransform2D, "LocalTransform2D"},
	{PEXOCGlobalTransform, "GlobalTransform"},
	{PEXOCGlobalTransform2D, "GlobalTransform2D"},
	{PEXOCModelClip, "ModelClip"},
	{PEXOCModelClipVolume, "ModelClipVolume"},
	{PEXOCModelClipVolume2D, "ModelClipVolume2D"},
	{PEXOCRestoreModelClip, "RestoreModelClip"},
	{PEXOCViewIndex, "ViewIndex"},
	{PEXOCLightState, "LightState"},
	{PEXOCDepthCueIndex, "DepthCueIndex"},
	{PEXOCPickId, "PickId"},
	{PEXOCHlhsrIdentifier, "HlhsrIdentifier"},
	{PEXOCColourApproxIndex, "ColourApproxIndex"},
	{PEXOCRenderingColourModel, "RenderingColourModel"},
	{PEXOCParaSurfCharacteristics, "ParaSurfCharacteristics"},
	{PEXOCAddToNameSet, "AddToNameSet"},
	{PEXOCRemoveFromNameSet, "RemoveFromNameSet"},
	{PEXOCExecuteStructure, "ExecuteStructure"},
	{PEXOCLabel, "Label"},
	{PEXOCApplicationData, "ApplicationData"},
	{PEXOCGse, "Gse"},
	{PEXOCMarker, "Marker"},
	{PEXOCMarker2D, "Marker2D"},
	{PEXOCText, "Text"},
	{PEXOCText2D, "Text2D"},
	{PEXOCAnnotationText, "AnnotationText"},
	{PEXOCAnnotationText2D, "AnnotationText2D"},
	{PEXOCPolyline, "Polyline"},
	{PEXOCPolyline2D, "Polyline2D"},
	{PEXOCPolylineSet, "PolylineSet"},
	{PEXOCNurbCurve, "NurbCurve"},
	{PEXOCFillArea, "FillArea"},
	{PEXOCFillArea2D, "FillArea2D"},
	{PEXOCExtFillArea, "ExtFillArea"},
	{PEXOCFillAreaSet, "FillAreaSet"},
	{PEXOCFillAreaSet2D, "FillAreaSet2D"},
	{PEXOCExtFillAreaSet, "ExtFillAreaSet"},
	{PEXOCTriangleStrip, "TriangleStrip"},
	{PEXOCQuadrilateralMesh, "QuadrilateralMesh"},
	{PEXOCSOFAS, "SOFAS"},
	{PEXOCNurbSurface, "NurbSurface"},
	{PEXOCCellArray, "CellArray"},
	{PEXOCCellArray2D, "CellArray2D"},
	{PEXOCExtCellArray, "ExtCellArray"},
	{PEXOCGdp, "Gdp"},
	{PEXOCGdp2D, "Gdp2D"},
	{0, "Error"}};
    static Table OutputCommand_Table = {0, OutputCommand_Table_data};
    return table_search(oc, &OutputCommand_Table, OutputCommand_SIZE);
}

