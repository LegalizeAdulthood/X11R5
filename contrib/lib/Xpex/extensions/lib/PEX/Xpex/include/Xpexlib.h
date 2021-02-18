/* $Header: Xpexlib.h,v 2.4 91/09/11 16:05:43 sinyaw Exp $ */

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

#ifndef XPEXLIB_H_INCLUDED
#define XPEXLIB_H_INCLUDED

#include <X11/X.h>
#include <X11/Xmd.h>
#include <X11/Xlib.h>

#include "PEX.h"
#include "PEXprotost.h"
#include "PEXproto.h"

#include "Xpex.h"

typedef PEXFLOAT XpexFloat;

#include "PEX_data.h"

typedef pexAsfAttribute XpexAsfAttribute;
typedef pexAsfValue XpexAsfValue;
typedef pexBitmask XpexBitmask;
typedef pexBitmaskShort XpexBitmaskShort;
typedef pexCoordType XpexCoordType;
typedef pexComposition XpexComposition;
typedef pexCullMode XpexCullMode;
typedef pexEnumTypeIndex XpexEnumTypeIndex;
typedef pexLookupTable XpexLookupTable;
typedef pexName XpexName;
typedef pexNameSet XpexNameSet;
typedef pexPC XpexPC;
typedef pexFont XpexFont;
typedef XpexFloat XpexMatrix[4][4];
typedef XpexFloat XpexMatrix3X3[3][3];
typedef pexPhigsWks XpexPhigsWks;
typedef pexPickMeasure XpexPickMeasure;
typedef pexRenderer XpexRenderer;
typedef pexSC XpexSC;
typedef pexStructure XpexStructure;
typedef pexSwitch XpexSwitch;
typedef pexTableIndex XpexTableIndex;
typedef pexTableType XpexTableType;
typedef pexTextHAlignment XpexTextHalignment;
typedef pexTextVAlignment XpexTextValignment;
typedef pexTypeOrTableIndex XpexTypeOrTableIndex;
typedef pexColourType XpexColorType;
typedef CARD16 XpexShape;
typedef CARD8 XpexContour;
typedef CARD16 XpexTextPrecision;
typedef CARD16 XpexTextPath;
typedef CARD16 XpexBufferMode;
typedef pexEnumTypeIndex XpexHlhsrMode;
typedef INT16 XpexPickDeviceType;
typedef CARD16 XpexRendererState;
typedef INT16	XpexDisplayUpdate;
typedef CARD8	XpexVisualState;
typedef CARD8	XpexDisplayState;
typedef CARD8	XpexUpdateState;
typedef pexDynamicType XpexDynamicType;

typedef pexImpDepConstantName XpexImpDepConstantName;
typedef XpexBitmask XpexPCBitmask[XpexPC_BitmaskSize];
typedef XpexBitmask XpexPWBitmask[XpexPW_BitmaskSize];

typedef pexStructureInfo XpexPostedStructureInfo;
typedef pexVector2D XpexVector2D;
typedef pexVector3D XpexVector3D;
typedef pexCoord2D XpexCoord2D;
typedef pexCoord3D XpexCoord3D;
typedef pexCoord4D XpexCoord4D;

typedef pexRgbFloatColour XpexRgbFloatColor;
typedef pexHsvColour XpexHsvColor;
typedef pexHlsColour XpexHlsColor;
typedef pexCieColour XpexCieColor;
typedef pexRgb8Colour XpexRgb8Color;
typedef pexRgb16Colour XpexRgb16Color;
typedef pexIndexedColour XpexIndexedColor;
typedef pexFloatColour XpexFloatColor;
typedef pexColourEntryRgb XpexColorEntryRgb;
typedef pexColourEntryHsv XpexColorEntryHsv;
typedef pexColourEntryHls XpexColorEntryHls;
typedef pexColourEntryCie XpexColorEntryCie;
typedef pexColourEntryRgb8 XpexColorEntryRgb8;
typedef pexColourEntryRgb16 XpexColorEntryRgb16;
typedef pexColourEntryFloat XpexColorEntryFloat;

typedef pexCurveApprox XpexCurveApprox;
typedef pexDeviceCoord XpexDeviceCoord;
typedef pexDeviceRect XpexDeviceRect;
typedef pexElementInfo XpexElementInfo;
typedef pexElementPos XpexElementPos;
typedef pexElementRange XpexElementRange;
typedef pexElementRef XpexElementRef;
typedef pexExtentInfo  XpexExtentInfo;
typedef pexHalfSpace XpexHalfSpace;
typedef pexNameSetPair XpexNameSetPair;
typedef pexHalfSpace2D XpexHalfSpace2D;
typedef pexNpcSubvolume XpexNpcSubvolume;
typedef pexPickPath XpexPickElementRef;
typedef pexTextAlignmentData XpexTextAlignment;

typedef pexReflAttrDataIndexed XpexReflAttrDataIndexed;
typedef pexReflAttrDataRgb8 XpexReflAttrDataRgb8;
typedef pexReflAttrDataRgb16 XpexReflAttrDataRgb16;
typedef pexReflAttrDataFloat XpexReflAttrDataFloat;

typedef pexSurfaceApprox XpexSurfaceApprox;
typedef pexViewport XpexViewport;
typedef pexViewEntry XpexViewEntry;
typedef pexViewRep XpexViewRep;

typedef pexTableInfo XpexTableInfo;

typedef pexLineBundleDataIndexed XpexLineBundleDataIndexed;
typedef pexLineBundleDataRgb8 XpexLineBundleDataRgb8;
typedef pexLineBundleDataRgb16 XpexLineBundleDataRgb16;
typedef pexLineBundleDataFloat XpexLineBundleDataFloat;

typedef pexMarkerBundleDataIndexed XpexMarkerBundleDataIndexed;
typedef pexMarkerBundleDataRgb8 XpexMarkerBundleDataRgb8;
typedef pexMarkerBundleDataRgb16 XpexMarkerBundleDataRgb16;
typedef pexMarkerBundleDataFloat XpexMarkerBundleDataFloat;

typedef pexTextBundleDataIndexed XpexTextBundleDataIndexed;
typedef pexTextBundleDataRgb8 XpexTextBundleDataRgb8;
typedef pexTextBundleDataRgb16 XpexTextBundleDataRgb16;
typedef pexTextBundleDataFloat XpexTextBundleDataFloat;

typedef pexInteriorBundleDataIndexed XpexInteriorBundleDataIndexed;
typedef pexInteriorBundleDataRgb8 XpexInteriorBundleDataRgb8;
typedef pexInteriorBundleDataRgb16 XpexInteriorBundleDataRgb16;
typedef pexInteriorBundleDataFloat XpexInteriorBundleDataFloat;

typedef pexEdgeBundleDataIndexed XpexEdgeBundleDataIndexed;
typedef pexEdgeBundleDataRgb8 XpexEdgeBundleDataRgb8;
typedef pexEdgeBundleDataRgb16 XpexEdgeBundleDataRgb16;
typedef pexEdgeBundleDataFloat XpexEdgeBundleDataFloat;

typedef pexLightDataIndexed XpexLightDataIndexed;
typedef pexLightDataRgb8 XpexLightDataRgb8;
typedef pexLightDataRgb16 XpexLightDataRgb16;
typedef pexLightDataFloat XpexLightDataFloat;

typedef pexDepthCueDataIndexed XpexDepthCueDataIndexed;
typedef pexDepthCueDataRgb8 XpexDepthCueDataRgb8;
typedef pexDepthCueDataRgb16 XpexDepthCueDataRgb16;
typedef pexDepthCueDataFloat XpexDepthCueDataFloat;

typedef pexPSC_IsoparametricCurves XpexPscIsoCurves;
typedef pexPD_DC_HitBox XpexPD_DC_HitBox;
typedef pexPD_NPC_HitVolume XpexPD_NPC_HitVolume;

typedef	pexOptIndexed XpexOptIndexed;
typedef	pexOptRgb XpexOptRgb;
typedef	pexOptCie XpexOptCie;
typedef	pexOptHsv XpexOptHsv;
typedef	pexOptHls XpexOptHls;
typedef	pexOptRgb8 XpexOptRgb8;
typedef	pexOptRgb16 XpexOptRgb16;
typedef	pexOptNormal XpexOptNormal;
typedef	pexOptIndexedNormal XpexOptIndexedNormal;
typedef	pexOptRgbNormal XpexOptRgbNormal;
typedef	pexOptCieNormal XpexOptCieNormal;
typedef	pexOptHsvNormal XpexOptHsvNormal;
typedef	pexOptHlsNormal XpexOptHlsNormal;
typedef	pexOptRgb8Normal XpexOptRgb8Normal;
typedef	pexOptRgb16Normal XpexOptRgb16Normal;

typedef	pexVertexNoOptData XpexVertexNoOptData;
typedef	pexVertexIndexed XpexVertexIndexed;
typedef	pexVertexRgb XpexVertexRgb;
typedef	pexVertexCie XpexVertexCie;
typedef	pexVertexHsv XpexVertexHsv;
typedef	pexVertexHls XpexVertexHls;
typedef	pexVertexRgb8 XpexVertexRgb8;
typedef	pexVertexRgb16 XpexVertexRgb16;
typedef	pexVertexNormal XpexVertexNormal;
typedef	pexVertexEdges XpexVertexEdges;
typedef	pexVertexIndexedNormal XpexVertexIndexedNormal; 
typedef	pexVertexRgbNormal XpexVertexRgbNormal;
typedef	pexVertexCieNormal XpexVertexCieNormal;
typedef	pexVertexHsvNormal XpexVertexHsvNormal;
typedef	pexVertexHlsNormal XpexVertexHlsNormal;
typedef	pexVertexRgb8Normal XpexVertexRgb8Normal; 
typedef	pexVertexRgb16Normal XpexVertexRgb16Normal;
typedef	pexVertexIndexedEdges XpexVertexIndexedEdges;
typedef	pexVertexRgbEdges XpexVertexRgbEdges;
typedef	pexVertexCieEdges XpexVertexCieEdges;
typedef	pexVertexHsvEdges XpexVertexHsvEdges;
typedef	pexVertexHlsEdges XpexVertexHlsEdges;
typedef	pexVertexRgb8Edges XpexVertexRgb8Edges;
typedef	pexVertexRgb16Edges XpexVertexRgb16Edges;
typedef	pexVertexNormalEdges XpexVertexNormalEdges;
typedef	pexVertexIndexedNormalEdges XpexVertexIndexedNormalEdges;
typedef	pexVertexRgbNormalEdges XpexVertexRgbNormalEdges;
typedef	pexVertexCieNormalEdges XpexVertexCieNormalEdges;
typedef	pexVertexHsvNormalEdges XpexVertexHsvNormalEdges;
typedef	pexVertexHlsNormalEdges XpexVertexHlsNormalEdges;
typedef	pexVertexRgb8NormalEdges XpexVertexRgb8NormalEdges;
typedef	pexVertexRgb16NormalEdges XpexVertexRgb16NormalEdges;

typedef char * XpexString;

typedef struct {
	XpexColorType color_type;
	XpexEnumTypeIndex fp_format;
	XID oc_target_res_id; /* Structure or Renderer */
	int oc_target_type;
} XpexDisplayInfo;

typedef struct {
	int  num_indices;
	XpexTableIndex  *indices;
} XpexListOfTableIndex;

typedef struct {
	int	num_lists;
	XpexListOfTableIndex	*list;
} XpexListOfListOfTableIndex;

typedef struct {
	int		num_floats;
	XpexFloat	*f_value;
} XpexListOfFloat;

typedef struct {
    int  num_refs;
    XpexElementRef  *elem_refs;
} XpexListOfElementRef;

typedef struct {
    int num_paths;
    XpexListOfElementRef  *list;
} XpexListOfListOfElementRef;

typedef struct {
    int  num_info;
    XpexPostedStructureInfo  *info;
} XpexListOfPostedStructureInfo;

typedef union {
    XpexCoord2D point2;
    XpexCoord3D point3;
    XpexCoord4D point4;
} XpexCoord;

typedef struct {
    XpexCoord2D  lower_left;    
    XpexCoord2D  upper_right;    
} XpexRectangle;

typedef struct {
    XpexFloat x_min; /* x min */
    XpexFloat x_max; /* x max */
    XpexFloat y_min; /* y min */
    XpexFloat y_max; /* y max */
    XpexFloat z_min; /* z min */
    XpexFloat z_max; /* z max */
} XpexLimit3D;

typedef struct {
    XpexFloat x_min; /* x min */
    XpexFloat x_max; /* x max */
    XpexFloat y_min; /* y min */
    XpexFloat y_max; /* y max */
} XpexLimit2D;

typedef struct {
    int num_points;
    XpexCoord2D  *points;
} XpexListOfCoord2D;

typedef struct {
    int num_points;
    XpexCoord3D  *points;
} XpexListOfCoord3D;

typedef struct {
    int num_points;
    XpexCoord4D  *points;
} XpexListOfCoord4D;

typedef union {
	XpexCoord2D  *points_2d; /* where is this used ? */
	XpexCoord3D  *points_3d;
	XpexCoord4D  *points_4d;
} XpexArrayOfCoord;

typedef struct {
    int num_points;
    union {
        XpexCoord2D  *points_2d;
        XpexCoord3D  *points_3d;
        XpexCoord4D  *points_4d;
    } points;
} XpexListOfCoord;

typedef struct {
    int num_coords;
    XpexDeviceCoord  *device_coord;
} XpexListOfDeviceCoord;

typedef struct {
    int num_lists;
    XpexListOfCoord2D  *lists;
} XpexListOfListOfCoord2D;

typedef struct {
    int num_lists;
    XpexListOfCoord3D  *lists;
} XpexListOfListOfCoord3D;

typedef struct {
    int num_half_spaces;
    XpexHalfSpace *half_spaces;
} XpexListOfHalfSpace;

typedef struct {
    int num_half_spaces;
    XpexHalfSpace2D  *half_spaces;
} XpexListOfHalfSpace2D;

typedef struct {
	XpexSwitch  visibility;  /* curve visibility flag */
	int  curve_order;  /* curve order */
	XpexCoordType  type;  /* rationality */
	XpexEnumTypeIndex  approx_method;  /* TC Approx Method */
	XpexFloat  tolerance;  /* TC Approx value */
	XpexListOfFloat  knots;  /* curve knot vector */
	XpexFloat  t_min; /* curve parameter range */ 
	XpexFloat  t_max; 
	XpexListOfCoord ctrl_points; /* control points */
} XpexTrimCurve;

typedef struct {
	int  count;  /* number of trim curves in list */
	XpexTrimCurve  *curves; /* list of curves */
} XpexListOfTrimCurve;

typedef union {
	XpexTableIndex	index;
	XpexRgb8Color	rgb8;
	XpexRgb16Color	rgb16;
	XpexFloatColor	color_float;
} XpexColor;

typedef union {
	XpexIndexedColor *index;
	XpexRgb8Color *rgb8;
	XpexRgb16Color *rgb16;
	XpexFloatColor *color_float;
} XpexArrayOfColor;

typedef union {
    XpexRgb8Color  rgb8;
    XpexRgb16Color rgb16;
    XpexRgbFloatColor rgb;
    XpexHsvColor  hsv;
    XpexHlsColor  hls;
    XpexCieColor  cie;
    XpexFloatColor color_float;
} XpexDirectColor;

typedef struct {
    XpexColorType color_type;
	XpexColor value;
} XpexColorSpecifier;

typedef struct {
	XpexFloat  ambient;
	XpexFloat  diffuse;
	XpexFloat  specular;
	XpexFloat  specular_conc;
	XpexFloat  transmission;
	XpexColorSpecifier  specular_color;
} XpexReflectionAttr;

typedef struct {
	XpexColorType color_type;
	int numx;
	int numy;
	XpexArrayOfColor colours;
} XpexPatternEntry;

typedef struct {
	int num_fonts;
	XpexFont *fonts;
} XpexTextFontEntry;

typedef struct {
	XpexLimit2D  window;  /* window limits */
	XpexLimit3D  viewport;  /* viewport limits */
	int  proj_type;  /* projection type */
	XpexCoord3D  proj_ref_point;  /* projection reference point */
	XpexFloat  view_plane;  /* view plane distance */
	XpexFloat  back_plane;  /* back plane distance */
	XpexFloat  front_plane;  /* front plane distance */
} XpexViewMap3D;

typedef struct {
    XpexLimit2D  window;      /* window limits */
    XpexLimit2D  viewport;    /* viewport limits */
} XpexViewMap2D;

/* BORDER */
/* ------------------------------------------------- */

typedef struct {
	int  num_pairs;
	XpexNameSetPair	*pairs;
} XpexListOfNameSetPair;

typedef struct {
	XpexCoord3D search_pos;
	XpexFloat search_dist;
	short search_ceiling;
	Bool model_clip_flag;
	XpexListOfElementRef start_path;
	XpexListOfNameSetPair normal_list;
	XpexListOfNameSetPair inverted_list;
} XpexSCValues;

typedef struct {
    int  num_names;
    XpexName  *names;
} XpexListOfName;

typedef struct {
	int	num_info;
	XpexElementInfo	*elem_info;
} XpexListOfElementInfo;

typedef struct {
    int	num_types;
    short *elem_type;
} XpexListOfElementType;

typedef struct {
	XpexCoord3D  origin;
	XpexVector3D  direction;
	unsigned short  numberIntersections B16;
	unsigned short  pad B16;
	XpexCoord3D	 *intersections;
} XpexPscLevelCurves;

typedef union {
	XpexPscIsoCurves  iso_curves;
	XpexPscLevelCurves  mc_curves;
	XpexPscLevelCurves  wc_curves;
} XpexPscDataRecord;

typedef struct {
	XpexEnumTypeIndex characteristics;
	XpexPscDataRecord record;
} XpexParaSurfChar;

typedef struct {
	int	num_rects; 
	XpexDeviceRect *device_rect;
} XpexListOfDeviceRect; 

typedef struct {
	int	num_info;
	XpexExtentInfo	*extent_info;
} XpexListOfExtentInfo;

typedef struct {
    Atom name;
    unsigned long value;    
} XpexFontProp;

typedef struct {
    unsigned long	first_glyph;
    unsigned long	last_glyph;
    unsigned long	default_glyph;
    Bool			all_exist;
	Bool			stroke_font;
    int				n_properties;
	XpexFontProp	*properties;
} XpexFontInfo;

typedef struct {
	int	num_info;
	XpexFontInfo *font_info;
} XpexListOfFontInfo;

/* ------------------------------------------------
   The following data types are used 
   exclusively by Xpexlib functions that support
   the retrieval of Extension Info from the 
   server:

      XpexExtensionInfo,
      XpexEnumTypeDesc,
      XpexListOfEnumTypeDesc, 
      XpexEnumTypeInfo,
      XpexConstantName,
      XpexListOfConstantName, and
      XpexImpDepConstants.
------------------------------------------------ */

typedef struct {
    int proto_major;
    int proto_minor;
	char *vendor;
	int release_number;
	int subset_info;
} XpexExtensionInfo;

typedef struct {
	XpexEnumTypeIndex index;
	char *name;
} XpexEnumTypeDesc;

typedef struct {
	int	count;
	XpexEnumTypeDesc *desc;
} XpexListOfEnumTypeDesc;

/*
 * The following structure types are used only
 * by the XpexLib function XpexGetImpDepConstants()
 *
 */

typedef union {
	int intValue;
	XpexFloat floatValue;
} XpexImpDepConstant;

typedef pexAllImpDepConstants XpexAllImpDepConstants;

/* ------------------------------------------------
    Structure type is used by the Pipeline Context
    Support Functions Only.
------------------------------------------------ */

typedef struct {
/* Marker attributes */
	XpexEnumTypeIndex marker_type;
	XpexFloat marker_scale;
	XpexColorSpecifier marker_color;
	XpexTableIndex marker_bundle_index;
/* Text attributes */
	XpexTableIndex text_font_index;
    XpexTextPrecision  text_precision;
    XpexFloat char_expansion;
    XpexFloat char_spacing;
    XpexColorSpecifier text_color;
    XpexFloat char_height;
    XpexVector2D char_up_vector;
    XpexTextPath  text_path;
    XpexTextAlignment text_alignment;
    XpexFloat atext_height;
    XpexVector2D atext_up_vector;
    XpexTextPath  atext_path;
    XpexTextAlignment atext_alignment;
    XpexEnumTypeIndex atext_style;
    XpexTableIndex text_bundle_index;
/* Line and curve attributes */
    XpexEnumTypeIndex  line_type;
    XpexFloat  line_width;
    XpexColorSpecifier  line_color;
    XpexCurveApprox  curve_approx;
    XpexEnumTypeIndex  polyline_interp;
    XpexTableIndex  line_bundle_index;
/* Surface attributes */
    XpexEnumTypeIndex  interior_style;
    XpexTableIndex  interior_style_index;
    XpexColorSpecifier  surface_color;
    XpexReflectionAttr  reflection_attr;
    XpexEnumTypeIndex  reflection_model;
    XpexEnumTypeIndex  surface_interp;
    XpexEnumTypeIndex  bf_interior_style;
    XpexTableIndex  bf_interior_style_index;
    XpexColorSpecifier  bf_surface_color;
    XpexReflectionAttr  bf_reflection_attr;
    XpexEnumTypeIndex  bf_reflection_model;
    XpexEnumTypeIndex  bf_surface_interp;
    XpexSurfaceApprox  surface_approx;
    XpexCullMode  culling_mode;
    Bool  distinguish_flag;
    XpexCoord2D  pattern_size;
    XpexCoord3D  pattern_ref_pt;
    XpexVector3D  pattern_ref_vec1;
    XpexVector3D  pattern_ref_vec2;
    XpexTableIndex interior_bundle_index;
/* Surface edge attributes */
	XpexSwitch  surface_edge_flag;
	XpexEnumTypeIndex  surface_edge_type;
    XpexFloat  surface_edge_width;
    XpexColorSpecifier surface_edge_color;
    XpexTableIndex edge_bundle_index;
/* Geometry transformation attributes */
    XpexMatrix  local_transform;
    XpexMatrix  global_transform;
    XpexSwitch  model_clip;
    XpexListOfHalfSpace  model_clip_volume;
    XpexTableIndex  view_index;
/* Color transformation attributes */
    XpexListOfTableIndex  light_state;
    XpexTableIndex depth_cue_index;
    XpexTableIndex	color_approx_index;
    XpexEnumTypeIndex  rdr_color_model;
    XpexParaSurfChar  psurf_char;
/* ASF attributes */
	XpexAsfAttribute asf_enables;
	XpexAsfValue asfs;
/* Miscellaneous attributes */
	unsigned long  pick_id;
    unsigned long  hlhsr_identifier;
    XpexNameSet  name_set;
} XpexPCValues;

typedef struct {
	XpexPC pc_id;
	XpexListOfElementRef current_path;
	XpexRendererState renderer_state;
	XpexLookupTable	marker_bundle;
	XpexLookupTable	text_bundle;
	XpexLookupTable	line_bundle;
	XpexLookupTable	interior_bundle;
	XpexLookupTable	edge_bundle;
	XpexLookupTable	view_table;
	XpexLookupTable	color_table;
	XpexLookupTable	depth_cue_table;
	XpexLookupTable	light_table;
	XpexLookupTable	color_approx_table;
	XpexLookupTable	pattern_table;
	XpexLookupTable	text_font_table;
	XpexNameSet highlight_incl;
	XpexNameSet	highlight_excl;
	XpexNameSet	invis_incl;
	XpexNameSet	invis_excl;
	XpexHlhsrMode  hlhsr_mode;
	XpexNpcSubvolume  npc_subvolume;
	XpexViewport  viewport;
	XpexListOfDeviceRect  clip_list;
} XpexRendererAttributes;

typedef struct {
	XpexBitmask tables;
	XpexBitmask namesets;
	XpexBitmask attributes;
} XpexRendererDynamics;

typedef struct {
	int  editing_mode;
	int  element_ptr;
	int  num_elements;
	int  total_length;
	Bool  has_refs;
} XpexStructureInfo;

typedef struct {
	XpexLookupTable	marker_bundle;
	XpexLookupTable	text_bundle;
	XpexLookupTable	line_bundle;
	XpexLookupTable	interior_bundle;
	XpexLookupTable	edge_bundle;
	XpexLookupTable	color_table;
	XpexLookupTable	depth_cue_table;
	XpexLookupTable	light_table;
	XpexLookupTable	color_approx_table;
	XpexLookupTable	pattern_table;
	XpexLookupTable	text_font_table;
	XpexNameSet	highlight_incl;
	XpexNameSet	highlight_excl;
	XpexNameSet	invis_incl;
	XpexNameSet	invis_excl;
} XpexWksResources;

typedef struct {
	XpexDisplayUpdate	display_update;
	XpexVisualState		visual_state;
	XpexDisplayState	display_surface;
	XpexUpdateState		view_update;
	XpexListOfTableIndex	defined_views;
	XpexUpdateState		wks_update;
	XpexNpcSubvolume	req_NPC_subvolume;
	XpexNpcSubvolume	cur_NPC_subvolume;
	XpexViewport	req_wks_viewpt;
	XpexViewport	cur_wks_viewpt;
	XpexUpdateState	hlhsr_update;
	XpexHlhsrMode	req_hlhsr_mode;
	XpexHlhsrMode	cur_hlhsr_mode;
	Drawable		drawable_id;
	XpexLookupTable	marker_bundle;
	XpexLookupTable	text_bundle;
	XpexLookupTable	line_bundle;
	XpexLookupTable	interior_bundle;
	XpexLookupTable	edge_bundle;
	XpexLookupTable	color_table;
	XpexLookupTable	depth_cue_table;
	XpexLookupTable	light_table;
	XpexLookupTable	color_approx_table;
	XpexLookupTable	pattern_table;
	XpexLookupTable	text_font_table;
	XpexNameSet	highlight_incl;
	XpexNameSet	highlight_excl;
	XpexNameSet	invis_incl;
	XpexNameSet	invis_excl;
	XpexListOfPostedStructureInfo  posted_structs;
	unsigned long	num_priorities;
	XpexUpdateState	buffer_update;
	XpexBufferMode	req_buffer_mode;
	XpexBufferMode	cur_buffer_mode;
} XpexWksInfo;

typedef struct {
    XpexDynamicType  view_rep;
    XpexDynamicType  marker_bundle;
    XpexDynamicType  text_bundle;
    XpexDynamicType  line_bundle;
    XpexDynamicType  interior_bundle;
    XpexDynamicType  edge_bundle;
    XpexDynamicType  color_table;
    XpexDynamicType  pattern_table;
    XpexDynamicType  wks_transform;
    XpexDynamicType  highlight_filter;
    XpexDynamicType  invisibility_filter;
    XpexDynamicType  hlhsr_mode;
    XpexDynamicType  structure_modify;
    XpexDynamicType  post_structure;
    XpexDynamicType  unpost_structure;
    XpexDynamicType  delete_structure;
    XpexDynamicType  reference_modify;
    XpexDynamicType  buffer_modify;
} XpexDynamics;

/*
 * The following structure types
 * are required by the Pick Resource Functions.
 *
 */

typedef XpexPD_DC_HitBox XpexPickDataRec1;
typedef XpexPD_NPC_HitVolume XpexPickDataRec2;

typedef union {
	XpexPickDataRec1	data_rec1;
	XpexPickDataRec2 	data_rec2;
} XpexPickDataRecord;


typedef struct {
    int  num_refs;
    XpexPickElementRef *refs;
} XpexListOfPickElementRef;

typedef CARD16 XpexPickStatus;
typedef CARD16 XpexPickPathOrder;
typedef pexEnumTypeIndex XpexPromptEchoType;

typedef struct {
	XpexPickStatus		pick_status;
	XpexListOfPickElementRef	pick_path;
	XpexPickPathOrder	pick_path_order;
	XpexNameSet			pick_incl;
	XpexNameSet			pick_excl;
	XpexPickDataRecord	pick_data_rec;
	XpexPromptEchoType	prompt_echo_type;
	XpexViewport		echo_volume; 
	XpexSwitch			echo_switch;
} XpexPickDevice;

typedef struct {
    XpexPickStatus pick_status;
    XpexListOfPickElementRef  picked_prim;
} XpexPickMeasureAttributes;

typedef struct {
	unsigned short length;
	char  *data;
} XpexData;

typedef XpexData XpexGseData;

typedef union {
	struct {
		int unused;
	} gdp_rec1;
	XpexData unsupported;
} XpexGdpData;

typedef union {
	struct {
		int unused;
	} gdp2_rec1;
	XpexData unsupported;
} XpexGdp2dData;

typedef union {
	XpexOptIndexed indexed;
	XpexOptRgb rgb;
	XpexOptCie cie;
	XpexOptHsv hsv;
	XpexOptHls hls;
	XpexOptRgb8 rgb8;
	XpexOptRgb16 rgb16;
	XpexOptNormal normal;
	XpexOptIndexedNormal indexed_normal;
	XpexOptRgbNormal rgb_normal;
	XpexOptCieNormal cie_normal;
	XpexOptHsvNormal hsv_normal;
	XpexOptHlsNormal hls_normal;
	XpexOptRgb8Normal rgb8_normal;
	XpexOptRgb16Normal rgb16_normal;
} XpexFacetOptData;

typedef union {
	XpexOptIndexed *indexed;
	XpexOptRgb *rgb;
	XpexOptCie *cie;
	XpexOptHsv *hsv;
	XpexOptHls *hls;
	XpexOptRgb8 *rgb8;
	XpexOptRgb16 *rgb16;
	XpexOptNormal *normal;
	XpexOptIndexedNormal *indexed_normal;
	XpexOptRgbNormal *rgb_normal;
	XpexOptCieNormal *cie_normal;
	XpexOptHsvNormal *hsv_normal;
	XpexOptHlsNormal *hls_normal;
	XpexOptRgb8Normal *rgb8_normal;
	XpexOptRgb16Normal *rgb16_normal;
} XpexArrayOfOptData;

typedef XpexArrayOfOptData XpexArrayOfFacetOptData;

typedef union {
	XpexVertexNoOptData *pts;
	XpexVertexIndexed *pts_indexed;
	XpexVertexRgb *pts_rgb;
	XpexVertexCie *pts_cie;
	XpexVertexHsv *pts_hsv;
	XpexVertexHls *pts_hls;
	XpexVertexRgb8 *pts_rgb8;
	XpexVertexRgb16 *pts_rgb16;
	XpexVertexNormal *pts_normal;
	XpexVertexEdges *pts_edges;
	XpexVertexIndexedNormal *pts_indexed_normal;
	XpexVertexRgbNormal *pts_rgb_normal;
	XpexVertexCieNormal *pts_cie_normal;
	XpexVertexHsvNormal *pts_hsv_normal;
	XpexVertexHlsNormal *pts_hls_normal;
	XpexVertexRgb8Normal *pts_rgb8_normal;
	XpexVertexRgb16Normal *pts_rgb16_normal;
	XpexVertexIndexedEdges *pts_indexed_edges;
	XpexVertexRgbEdges *pts_rgb_edges;
	XpexVertexCieEdges *pts_cie_edges;
	XpexVertexHsvEdges *pts_hsv_edges;
	XpexVertexHlsEdges *pts_hls_edges;
	XpexVertexRgb8Edges *pts_rgb8_edges;
	XpexVertexRgb16Edges *pts_rgb16_edges;
	XpexVertexNormalEdges *pts_normal_edges;
	XpexVertexIndexedNormalEdges *pts_indexed_normal_edges;
	XpexVertexRgbNormalEdges *pts_rgb_normal_edges;
	XpexVertexCieNormalEdges *pts_cie_normal_edges;
	XpexVertexHsvNormalEdges *pts_hsv_normal_edges;
	XpexVertexHlsNormalEdges *pts_hls_normal_edges;
	XpexVertexRgb8NormalEdges *pts_rgb8_normal_edges;
	XpexVertexRgb16NormalEdges *pts_rgb16_normal_edges;
} XpexArrayOfVertex;

typedef struct {
	XpexFacetOptData facetData;
	unsigned long numVertices;
	XpexArrayOfVertex vertices;
} XpexFacet;

typedef struct {
	unsigned long numVertices;
	XpexArrayOfVertex vertices;
} XpexListOfVertex;

extern XExtCodes *XpexInitialize(
#if NeedFunctionPrototypes
	Display*			/* display */
#endif
);

#include "Xpexfuncs.h"

#endif /* XPEXLIB_H_INCLUDED */
