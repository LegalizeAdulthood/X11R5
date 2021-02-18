/* $Header: dpy_wks_info.c,v 1.1 91/09/18 13:50:07 sinyaw Exp $ */

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
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "Xpexlib.h"
#include "Xpexutil.h"

#include "PEX_names.h"

static XpexLookupTable lineBundleLUT;
static XpexLookupTable markerBundleLUT;
static XpexLookupTable textBundleLUT;
static XpexLookupTable interiorBundleLUT;
static XpexLookupTable edgeBundleLUT;
static XpexLookupTable patternLUT;
static XpexLookupTable textFontLUT;
static XpexLookupTable colorLUT;
static XpexLookupTable lightLUT;
static XpexLookupTable depthCueLUT;
static XpexLookupTable colorApproxLUT;
static XpexRenderer rdr_id;
static XpexNameSet highlightIncl;
static XpexNameSet highlightExcl;
static XpexNameSet invisIncl;
static XpexNameSet invisExcl;
static XpexPhigsWks wks_id;

InitResources(dpy, drawable)
	Display *dpy;
	Drawable drawable;
{
	markerBundleLUT = XpexCreateMarkerBundleLUT(dpy, drawable);
	lineBundleLUT = XpexCreateLineBundleLUT(dpy, drawable);
	textBundleLUT = XpexCreateTextBundleLUT(dpy, drawable);
	interiorBundleLUT = XpexCreateInteriorBundleLUT(dpy, drawable);
	edgeBundleLUT = XpexCreateEdgeBundleLUT(dpy, drawable);
	patternLUT = XpexCreatePatternLUT(dpy, drawable);
	textFontLUT = XpexCreateTextFontLUT(dpy, drawable);
	colorLUT = XpexCreateColorLUT(dpy, drawable);
	lightLUT = XpexCreateLightLUT(dpy, drawable);
	depthCueLUT = XpexCreateDepthCueLUT(dpy, drawable);
	colorApproxLUT = XpexCreateColorApproxLUT(dpy, drawable);

	highlightIncl = XpexCreateNameSet(dpy);
	highlightExcl = XpexCreateNameSet(dpy);
	invisIncl = XpexCreateNameSet(dpy);
	invisExcl = XpexCreateNameSet(dpy);
}

dpy_wks_info(dpy, drawable)
	Display *dpy;
	Drawable drawable;
{
	static void display_wks_info();
	static void clean_up();

	InitResources(dpy, drawable);
	display_wks_info(dpy, drawable);
	dpy_pick_info(dpy);
	cleanup(dpy);
}

static void
display_wks_info(dpy, drawable)
	Display *dpy;
	Drawable drawable;
{
	XpexWksResources wks_res;
	XpexBitmask valuemask[2];
	XpexWksInfo info;
	float *data;

	wks_res.line_bundle = lineBundleLUT;
	wks_res.marker_bundle = markerBundleLUT;
	wks_res.text_bundle = textBundleLUT;
	wks_res.interior_bundle = interiorBundleLUT;
	wks_res.edge_bundle = edgeBundleLUT;
	wks_res.color_table = colorLUT;
	wks_res.depth_cue_table = depthCueLUT;
	wks_res.light_table = lightLUT;
	wks_res.color_approx_table = colorApproxLUT;
	wks_res.pattern_table = patternLUT;
	wks_res.text_font_table = textFontLUT;
	wks_res.highlight_incl = highlightIncl;
	wks_res.highlight_excl = highlightExcl;
	wks_res.invis_incl = invisIncl;
	wks_res.invis_excl = invisExcl;

	wks_id = XpexCreatePhigsWks(dpy, drawable, 
	&wks_res, Xpex_SingleBuffered);

	(void) fprintf(stderr, 
	"PHIGS Workstation Information (Wks Resource ID : %d)\n\n",
	wks_id);

	valuemask[0] = 
		XpexPW_DisplayUpdate | 
		XpexPW_VisualState |
		XpexPW_DisplaySurface |
		XpexPW_ViewUpdate |
		XpexPW_DefinedViews;

	valuemask[1] = 0x0;

	XpexGetWksInfo(dpy, wks_id, valuemask, &info);

	(void) fprintf(stderr, "\tDisplayUpdate: %s(%d)\n", 
		NameOfDisplayUpdateMethod(info.display_update),
		info.display_update);

	(void) fprintf(stderr, "\tVisualState: %s(%d)\n", 
		NameOfVisualState(info.visual_state),
		info.visual_state);

	(void) fprintf(stderr, "\tDisplaySurface: %s(%d)\n",
	((info.display_surface) ? "NotEmpty" : "Empty"),
	info.display_surface);

	(void) fprintf(stderr, "\tViewUpdate: %s(%d)\n",
	((info.view_update) ? "Pending" : "NotPending"),
	info.view_update);

	(void) fprintf(stderr, "\tDefinedViews: %d\n",
	info.defined_views.num_indices);

	valuemask[0] =
		XpexPW_WksUpdate |
		XpexPW_ReqNpcSubvolume |
		XpexPW_CurNpcSubvolume;

	valuemask[1] = 0x0;

	XpexGetWksInfo(dpy, wks_id, valuemask, &info);
	(void) fprintf(stderr, "\tWksUpdate: %s(%d)\n",
	((info.wks_update) ? "Pending" : "NotPending"),
	info.wks_update);

	(void) fprintf(stderr, 
		"\treq_NPC_subvolume: (%.1f, %.1f, %1.f),(%.1f, %.1f, %.1f)\n",
		info.req_NPC_subvolume.minval.x,
		info.req_NPC_subvolume.minval.y,
		info.req_NPC_subvolume.minval.z,
		info.req_NPC_subvolume.maxval.x,
		info.req_NPC_subvolume.maxval.y,
		info.req_NPC_subvolume.maxval.z);

	(void) fprintf(stderr, 
		"\tcur_NPC_subvolume: (%.1f, %.1f, %1.f),(%.1f, %.1f, %.1f)\n",
		info.cur_NPC_subvolume.minval.x,
		info.cur_NPC_subvolume.minval.y,
		info.cur_NPC_subvolume.minval.z,
		info.cur_NPC_subvolume.maxval.x,
		info.cur_NPC_subvolume.maxval.y,
		info.cur_NPC_subvolume.maxval.z);

	valuemask[0] = 
		XpexPW_ReqWksViewport |
		XpexPW_CurWksViewport |
		XpexPW_HlhsrUpdate |
		XpexPW_ReqHlhsrMode |
		XpexPW_CurHlhsrMode;

	valuemask[1] = 0x0;

	XpexGetWksInfo(dpy, wks_id, valuemask, &info); 

	(void) fprintf(stderr,
		"\treq_wks_viewpt: (%d, %d, %.1f),(%d, %d, %.1f), %s(%u)\n",
		info.req_wks_viewpt.minval.x,
		info.req_wks_viewpt.minval.y,
		info.req_wks_viewpt.minval.z,
		info.req_wks_viewpt.maxval.x,
		info.req_wks_viewpt.maxval.y,
		info.req_wks_viewpt.maxval.z,
		((info.req_wks_viewpt.useDrawable) ? "True" : "False"),
		info.req_wks_viewpt.useDrawable);

	(void) fprintf(stderr,
		"\tcur_wks_viewpt: (%d, %d, %.1f),(%d, %d, %.1f), %s(%u)\n",
		info.cur_wks_viewpt.minval.x,
		info.cur_wks_viewpt.minval.y,
		info.cur_wks_viewpt.minval.z,
		info.cur_wks_viewpt.maxval.x,
		info.cur_wks_viewpt.maxval.y,
		info.cur_wks_viewpt.maxval.z,
		((info.cur_wks_viewpt.useDrawable) ? "True" : "False"),
		info.req_wks_viewpt.useDrawable);

	(void) fprintf(stderr, "\tHLHSR_update: %s(%d)\n",
	info.hlhsr_update ? "Pending" : "NotPending",
	info.hlhsr_update);

	(void) fprintf(stderr, "\treq_HLHSR_mode: %d\n",
	info.req_hlhsr_mode);

	(void) fprintf(stderr, "\tcur_HLHSR_mode: %d\n",
	info.cur_hlhsr_mode);

	valuemask[0] = 
		XpexPW_Drawable |
		XpexPW_MarkerBundle | 
		XpexPW_TextBundle | 
		XpexPW_LineBundle |
		XpexPW_InteriorBundle |
		XpexPW_EdgeBundle |
		XpexPW_ColorTable |
		XpexPW_DepthCueTable |
		XpexPW_LightTable |
		XpexPW_ColorApproxTable |
		XpexPW_PatternTable |
		XpexPW_TextFontTable |
		XpexPW_HighlightIncl |
		XpexPW_HighlightExcl |
		XpexPW_InvisibilityIncl |
		XpexPW_InvisibilityExcl |
		XpexPW_PostedStructures;

	valuemask[1] = 0x0;

	XpexGetWksInfo(dpy, wks_id, valuemask, &info);

	(void) fprintf(stderr, "\tdrawable_id: %u\n", info.drawable_id);

	(void) fprintf(stderr, "\tmarker_bundle: %u\n", info.marker_bundle);
	(void) fprintf(stderr, "\ttext_bundle: %u\n", info.text_bundle);
	(void) fprintf(stderr, "\tline_bundle: %u\n", info.line_bundle);
	(void) fprintf(stderr, "\tinterior_bundle: %u\n", info.interior_bundle);

	(void) fprintf(stderr, "\tedge_bundle: %u\n", info.edge_bundle);

	(void) fprintf(stderr, "\tcolor_table: %u\n", info.color_table);

	(void) fprintf(stderr, 
	"\tdepth_cue_table: %u\n", info.depth_cue_table);

	(void) fprintf(stderr, "\tlight_table: %u\n", info.light_table);

	(void) fprintf(stderr, 
	"\tcolor_approx_table: %u\n", info.color_approx_table);

	(void) fprintf(stderr, "\tpattern_table: %u\n", info.pattern_table);

	(void) fprintf(stderr, "\ttext_font_table: %u\n", info.text_font_table);
	(void) fprintf(stderr, "\thighlight_incl: %u\n", info.highlight_incl);
	(void) fprintf(stderr, "\thighlight_excl: %u\n", info.highlight_excl);
	(void) fprintf(stderr, "\tinvis_incl: %u\n", info.invis_incl);
	(void) fprintf(stderr, "\tinvis_excl: %u\n", info.invis_excl);
	(void) fprintf(stderr, "\tposted_structs: %d\n", 
	info.posted_structs.num_info);

	valuemask[0] =
		XpexPW_NumPriorities |
		XpexPW_BufferUpdate;

	valuemask[1] = 
		XpexPW_ReqBufferMode | 
		XpexPW_CurBufferMode;

	XpexGetWksInfo(dpy, wks_id, valuemask, &info);

	(void) fprintf(stderr, "\tnum_priorities: %d\n", 
	info.num_priorities);


	(void) fprintf(stderr, "\tbuffer_update: %s(%d)\n", 
	(info.buffer_update) ? "Pending" : "NotPending", 
	info.buffer_update);

	(void) fprintf(stderr, "\treq_buffer_mode: %s(%d)\n", 
	(info.req_buffer_mode) ? "Double" : "Single", 
	info.req_buffer_mode);

	(void) fprintf(stderr, "\tcur_buffer_mode: %s(%d)\n", 
	(info.cur_buffer_mode) ? "Double" : "Single",
	info.cur_buffer_mode);

	dpy_dyn(dpy,drawable);
}

dpy_dyn(dpy,drawable)
	Display *dpy;
	Drawable drawable;
{
	static char *dyn_str[18] = {
		"\tview_rep            : %s\n",
		"\tmarker_bundle       : %s\n",
		"\ttext_bundle         : %s\n",
		"\tline_bundle         : %s\n",
		"\tinterior_bundle     : %s\n",
		"\tedge_bundle         : %s\n",
		"\tcolor_table         : %s\n",
		"\tpattern_table       : %s\n",
		"\twks_transform       : %s\n",
		"\thighlight_filter    : %s\n",
		"\tinvisibility_filter : %s\n",
		"\thlhsr_mode          : %s\n",
		"\tstructure_modify    : %s\n",
		"\tpost_structure      : %s\n",
		"\tunpost_structure    : %s\n",
		"\tdelete_structure    : %s\n",
		"\treference_modify    : %s\n",
		"\tbuffer_modify       : %s\n"
	};
	static char *wks_dyn_str[3] = {
		"IMM",
		"IRG",
		"CBS"
	};
	XpexDynamics dynamics;

	(void) fprintf(stderr, "\nPHIGS Workstation Dynamics\n\n");

	XpexGetDynamics(dpy, drawable, &dynamics);

	(void) fprintf(stderr, dyn_str[0], wks_dyn_str[dynamics.view_rep]);
	(void) fprintf(stderr, dyn_str[1], wks_dyn_str[dynamics.marker_bundle]);
	(void) fprintf(stderr, dyn_str[2], wks_dyn_str[dynamics.text_bundle]);
	(void) fprintf(stderr, dyn_str[3], wks_dyn_str[dynamics.line_bundle]);
	(void) fprintf(stderr, dyn_str[4], wks_dyn_str[dynamics.interior_bundle]);
	(void) fprintf(stderr, dyn_str[5], wks_dyn_str[dynamics.edge_bundle]);
	(void) fprintf(stderr, dyn_str[6], wks_dyn_str[dynamics.color_table]);
	(void) fprintf(stderr, dyn_str[7], wks_dyn_str[dynamics.pattern_table]);
	(void) fprintf(stderr, dyn_str[8], wks_dyn_str[dynamics.wks_transform]);
	(void) fprintf(stderr, dyn_str[9], wks_dyn_str[dynamics.highlight_filter]);
	(void) fprintf(stderr, dyn_str[10], wks_dyn_str[dynamics.invisibility_filter]);
	(void) fprintf(stderr, dyn_str[11], wks_dyn_str[dynamics.hlhsr_mode]);
	(void) fprintf(stderr, dyn_str[12], wks_dyn_str[dynamics.structure_modify]);
	(void) fprintf(stderr, dyn_str[13], wks_dyn_str[dynamics.post_structure]);
	(void) fprintf(stderr, dyn_str[14], wks_dyn_str[dynamics.unpost_structure]);
	(void) fprintf(stderr, dyn_str[15], wks_dyn_str[dynamics.delete_structure]);
	(void) fprintf(stderr, dyn_str[16], wks_dyn_str[dynamics.reference_modify]);
	(void) fprintf(stderr, dyn_str[17], wks_dyn_str[dynamics.buffer_modify]);

}

dpy_pick_info(dpy)
	Display *dpy;
{
	XpexPickMeasure pm_id;
	XpexPickDevice values;
	XpexPickDevice values2;

	XpexGetPickDevice(dpy, wks_id, 
		Xpex_PickDeviceDC_HitBox, 
		XpexPD_PickStatus | 
			XpexPD_PickPath | 
			XpexPD_PickPathOrder |
			XpexPD_PickIncl | 
			XpexPD_PickExcl, 
		&values);

	XpexGetPickDevice(dpy, 
		wks_id, 
		Xpex_PickDeviceDC_HitBox, 
		XpexPD_PickDataRec,
		&values);

	XpexGetPickDevice(dpy, 
		wks_id, 
		Xpex_PickDeviceDC_HitBox,
		XpexPD_PickPromptEchoType, 
		&values);

	XpexGetPickDevice(dpy, 
		wks_id, 
		Xpex_PickDeviceDC_HitBox, 
		XpexPD_PickEchoVolume,
		&values);

	XpexGetPickDevice(dpy, 
		wks_id, 
		Xpex_PickDeviceDC_HitBox, 
		XpexPD_PickEchoSwitch,
		&values);

	(void) fprintf(stderr, 
	"\nPick Device Descriptors(DC_HitBox)\n\n");

	pr_pick_desc(Xpex_PickDeviceDC_HitBox, &values);

/* ----------------------------------------------------- */

	(void) fprintf(stderr, 
	"\nPick Device Descriptors(NPC_HitVolume)\n\n");

	XpexGetPickDevice(dpy, 
		wks_id, 
		Xpex_PickDeviceNPC_HitVolume, 
		XpexPD_PickStatus |
			XpexPD_PickPath |
			XpexPD_PickPathOrder |
			XpexPD_PickIncl |
			XpexPD_PickExcl,
		&values2);

	XpexGetPickDevice(dpy, 
		wks_id, 
		Xpex_PickDeviceNPC_HitVolume, 
		XpexPD_PickDataRec,
		&values2);

	XpexGetPickDevice(dpy,
		wks_id,
		Xpex_PickDeviceNPC_HitVolume,
		XpexPD_PickPromptEchoType,
		&values2);

	XpexGetPickDevice(dpy, 
		wks_id, 
		Xpex_PickDeviceNPC_HitVolume, 
		XpexPD_PickEchoVolume,
		&values2);

	XpexGetPickDevice(dpy, 
		wks_id, 
		Xpex_PickDeviceNPC_HitVolume, 
		XpexPD_PickEchoSwitch,
		&values2);

	pr_pick_desc(Xpex_PickDeviceNPC_HitVolume, &values2);

/* ----------------------------------------------------- */

	pm_id = XpexCreatePickMeasure(dpy, 
	wks_id, Xpex_PickDeviceDC_HitBox);

	(void) fprintf(stderr, 
	"\nPick Measure Attributes(DC_HitBox)\n\n");
	{
		XpexPickMeasureAttributes pm_values;

		XpexGetPickMeasure(dpy, pm_id, XpexPM_Status, &pm_values);
		XpexGetPickMeasure(dpy, pm_id, XpexPM_Path, &pm_values);

		(void) fprintf(stderr, "\tpick_status: %u\n", 
		pm_values.pick_status);

		(void) fprintf(stderr, "\tpicked_prim: %u\n", 
		pm_values.picked_prim.num_refs);
	}
	XpexFreePickMeasure(dpy, pm_id);

	pm_id = XpexCreatePickMeasure(dpy, 
	wks_id, Xpex_PickDeviceNPC_HitVolume);

	(void) fprintf(stderr, 
	"\nPick Measure Attributes(NPC_HitVolume)\n\n");
	{
		XpexPickMeasureAttributes pm_values;

		XpexGetPickMeasure(dpy, pm_id, XpexPM_Status, &pm_values);
		XpexGetPickMeasure(dpy, pm_id, XpexPM_Path, &pm_values);

		(void) fprintf(stderr, "\tpick_status: %u\n", 
		pm_values.pick_status);

		(void) fprintf(stderr, "\tpicked_prim: %u\n", 
		pm_values.picked_prim.num_refs);
	}
	XpexFreePickMeasure(dpy, pm_id);
}

pr_pick_desc(dev_type, values)
	XpexPickDeviceType dev_type;
	XpexPickDevice *values;
{
	(void) fprintf(stderr, "\tPickStatus: %s(%d)\n",
	((values->pick_status) ? "Ok" : "NoPick"), values->pick_status);

	(void) fprintf(stderr, "\tPickPath: %d\n", 
	values->pick_path.num_refs);

	(void) fprintf(stderr, "\tPickPathOrder: %s(%d)\n", 
	((values->pick_path_order) ? "BottomFirst" : "TopFirst"),
	values->pick_path_order);

	(void) fprintf(stderr, "\tPickIncl: %u\n", values->pick_incl);
	(void) fprintf(stderr, "\tPickExcl: %u\n", values->pick_excl);

	if (dev_type == Xpex_PickDeviceDC_HitBox) {
		(void) fprintf(stderr, 
			"\tpick_data_rec: (%d, %d, %f)\n",
			values->pick_data_rec.data_rec1.position.x,
			values->pick_data_rec.data_rec1.position.y,
			values->pick_data_rec.data_rec1.distance);
	} else {
		(void) fprintf(stderr,
			"\tpick_data_rec: (%g, %g, %g), (%g, %g, %g)\n",
			values->pick_data_rec.data_rec2.minval.x,
			values->pick_data_rec.data_rec2.minval.y,
			values->pick_data_rec.data_rec2.minval.z,
			values->pick_data_rec.data_rec2.maxval.x,
			values->pick_data_rec.data_rec2.maxval.y,
			values->pick_data_rec.data_rec2.maxval.z);
	}

	(void) fprintf(stderr, "\tPickPromptEchoType: %d\n", 
	values->prompt_echo_type);

	(void) fprintf(stderr, 
		"\techo_volume: { (%d, %d, %g), (%d, %d, %g), %s(%d) }\n",
		values->echo_volume.minval.x, 
		values->echo_volume.minval.y,
		values->echo_volume.minval.z,
		values->echo_volume.maxval.x, 
		values->echo_volume.maxval.y,
		values->echo_volume.maxval.z,
		(values->echo_volume.useDrawable) ? "True" : "False",
		values->echo_volume.useDrawable);

	(void) fprintf(stderr, "\tEchoSwitch: %s(%d)\n", 
		((values->prompt_echo_type) ? "Echo" : "NoEcho"),
		values->prompt_echo_type);
}
	



static void
clean_up(dpy)
{
	XpexFreePhigsWks(dpy, wks_id); 
	XpexFreeLookupTable(dpy, lineBundleLUT);
	XpexFreeLookupTable(dpy, markerBundleLUT);
	XpexFreeLookupTable(dpy, textBundleLUT);
	XpexFreeLookupTable(dpy, interiorBundleLUT);
	XpexFreeLookupTable(dpy, edgeBundleLUT);
	XpexFreeLookupTable(dpy, patternLUT);
	XpexFreeLookupTable(dpy, colorLUT);
	XpexFreeLookupTable(dpy, textFontLUT);
	XpexFreeLookupTable(dpy, lightLUT);
	XpexFreeLookupTable(dpy, depthCueLUT);
	XpexFreeLookupTable(dpy, colorApproxLUT);
	XpexFreeNameSet(dpy, highlightIncl);
	XpexFreeNameSet(dpy, highlightExcl);
	XpexFreeNameSet(dpy, invisIncl);
	XpexFreeNameSet(dpy, invisExcl);
}
