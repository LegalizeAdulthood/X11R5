/* $Header: dpy_rdr_info.c,v 1.1 91/09/18 13:50:06 sinyaw Exp $ */

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
#include "Xpexlib.h"

#define GET_ATTR(_ATTR,_STRUCT_PTR) \
XpexGetRendererAttributes(dpy,rdr_id,_ATTR,_STRUCT_PTR)

#define PRINT(format,string) \
(void) fprintf(stderr,format,string)

static XpexRenderer rdr_id;

dpy_rdr_info(dpy,drawable)
	Display *dpy;
	Drawable drawable;
{
	static char *rdr_msg_str[24] = {
		"Renderer Attributes (Default Values)\n\n",
		"\tPipelineContext   : %u\n",
		"\tCurrentPath       : %d\n",
		"\tMarkerBundle      : %u\n",
		"\tTextBundle        : %u\n",
		"\tLineBundle        : %u\n",
		"\tInteriorBundle    : %u\n",
		"\tEdgeBundle        : %u\n",
		"\tViewTable         : %u\n",
		"\tColourTable       : %u\n",
		"\tDepthCueTable     : %u\n",
		"\tLightTable        : %u\n",
		"\tColourApproxTable : %u\n",
		"\tPatternTable      : %u\n",
		"\tTextFontTable     : %u\n",
		"\tHighLightIncl     : %u\n",
		"\tHighlightExcl     : %u\n",
		"\tInvisibilityIncl  : %u\n",
		"\tInvisibilityExcl  : %u\n",
		"\tRendererState     : %s(%d)\n",
		"\tHlhsrMode         : %d\n",
		"\tNpcSubvolume      : {(%.1f,%.1f,%.1f),(%.1f,%.1f,%.1f)}\n",
		"\tViewport          : {(%d,%d,%.1f),(%d,%d,%.1f),%u}\n",
		"\tClipList          : %d\n"
	};
	XpexRendererAttributes values;

	rdr_id = XpexCreateRenderer(dpy, drawable, 
	(XpexBitmask) 0, (XpexRendererAttributes *) NULL);

	(void) fprintf(stderr, rdr_msg_str[0]);

	GET_ATTR(XpexRD_PipelineContext, &values);

	PRINT(rdr_msg_str[1],values.pc_id);

	GET_ATTR(XpexRD_CurrentPath, &values);

	PRINT(rdr_msg_str[2],values.current_path.num_refs);

	GET_ATTR(XpexRD_RendererState, &values);

	(void) fprintf(stderr, rdr_msg_str[19], 
		((values.renderer_state) ? "Rendering" : "Idle"), 
		values.renderer_state);

	GET_ATTR(XpexRD_MarkerBundle, &values);

	PRINT(rdr_msg_str[3], values.marker_bundle);

	GET_ATTR(XpexRD_TextBundle, &values);

	PRINT(rdr_msg_str[4], values.text_bundle);

	GET_ATTR(XpexRD_LineBundle, &values);

	PRINT(rdr_msg_str[5], values.line_bundle);

	GET_ATTR(XpexRD_InteriorBundle, &values);

	PRINT(rdr_msg_str[6], values.interior_bundle);

	GET_ATTR(XpexRD_EdgeBundle, &values);

	PRINT(rdr_msg_str[7], values.edge_bundle);

	GET_ATTR(XpexRD_ViewTable, &values);

	PRINT(rdr_msg_str[8], values.view_table);

	GET_ATTR(XpexRD_ColorTable, &values);

	PRINT(rdr_msg_str[9], values.color_table);

	GET_ATTR(XpexRD_DepthCueTable, &values);

	PRINT(rdr_msg_str[10], values.depth_cue_table);

	GET_ATTR(XpexRD_LightTable, &values);

	PRINT(rdr_msg_str[11], values.light_table);

	GET_ATTR(XpexRD_ColorApproxTable, &values);

	PRINT(rdr_msg_str[12], values.color_approx_table);

	GET_ATTR(XpexRD_PatternTable, &values);

	PRINT(rdr_msg_str[13], values.pattern_table);

	GET_ATTR(XpexRD_TextFontTable, &values);

	PRINT(rdr_msg_str[14], values.text_font_table);

	GET_ATTR(XpexRD_HighlightIncl, &values);

	PRINT(rdr_msg_str[15], values.highlight_incl);

	GET_ATTR(XpexRD_HighlightExcl, &values);

	PRINT(rdr_msg_str[16], values.highlight_excl);

	GET_ATTR(XpexRD_InvisibilityIncl, &values);

	PRINT(rdr_msg_str[17], values.invis_incl);

	GET_ATTR(XpexRD_InvisibilityExcl, &values);

	PRINT(rdr_msg_str[18], values.invis_excl);

	GET_ATTR(XpexRD_HlhsrMode, &values);

	PRINT(rdr_msg_str[20], values.hlhsr_mode);

	GET_ATTR(XpexRD_NpcSubvolume, &values);

	(void) fprintf(stderr, rdr_msg_str[21], 
		values.npc_subvolume.minval.x, values.npc_subvolume.minval.y,
		values.npc_subvolume.minval.z,
		values.npc_subvolume.maxval.x, values.npc_subvolume.maxval.y,
		values.npc_subvolume.maxval.z);

	GET_ATTR(XpexRD_Viewport, &values);

	(void) fprintf(stderr, rdr_msg_str[22], 
		values.viewport.minval.x,
		values.viewport.minval.y,
		values.viewport.minval.z,
		values.viewport.maxval.x,
		values.viewport.maxval.y,
		values.viewport.maxval.z,
		values.viewport.useDrawable); 

	GET_ATTR(XpexRD_ClipList, &values);

	(void) fprintf(stderr, rdr_msg_str[23], values.clip_list.num_rects);

	(void) fprintf(stderr,"\n");

	dpy_rdr_dyn(dpy);

	XpexFreeRenderer(dpy, rdr_id);
}

dpy_rdr_dyn(dpy)
	Display *dpy;
{
	XpexRendererDynamics rdr_dynamics;

	(void) fprintf(stderr, 
	"Renderer Dynamics (Default Values)\n\n");

	XpexGetRendererDynamics(dpy, rdr_id, &rdr_dynamics);

	(void) fprintf(stderr, 
	"\tTables     : 0x%x\n", rdr_dynamics.tables);

	(void) fprintf(stderr, 
	"\tNamesets   : 0x%x\n", rdr_dynamics.namesets);

	(void) fprintf(stderr, 
	"\tAttributes : 0x%x\n", rdr_dynamics.attributes);
}
