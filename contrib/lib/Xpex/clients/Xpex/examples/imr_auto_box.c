/* $Header: imr_auto_box.c,v 1.8 91/09/11 17:39:31 sinyaw Exp $ */

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
#include <math.h>
#include "Xpexlib.h"
#include "Xpexutil.h"
#include "window.h"

#include "pex_auto_box.h"

extern FuncPtr fill_area_func[2];
extern int useFillAreaSet;

static XpexString quad_msg[4][2] = {
	 { " X Rotation", " Zooming" },
	 { " Y Rotation", " Zooming" },
	 { " Z Rotation", " Zooming" },
	 { " XYZ Rotation", " Zooming" }
};

static Display *dpy;
static Drawable drawable;
static XpexPC pc_id;
static XpexPCBitmask pc_attrs;
static XpexPCValues pc_values;
static XpexRenderer rdr;
static XpexRendererAttributes rdr_attrs;

static XpexCoord3D	pt;
static XpexVector3D	shift;
static XpexFloat	x_ang, y_ang, z_ang;
static XpexVector3D  scale;
static XpexMatrix	bldmat, bldmat2, bldmat3, bldmat4;

static void
SetUpViews()
{
	extern void InitViewEntries();

	XpexViewEntry views[4];

	InitViewEntries(views);

	XpexSetTableEntries(dpy, rdr_attrs.view_table,
	Xpex_ViewLUT, 1, 4, (char *)views, 4 * sizeof(XpexViewEntry));
}

static void
InitXresources()
{
	SetWindowLabel("pex_auto_box - immediate mode");
	drawable = CreateWindow(dpy);
}

static void
InitPEXresources()
{
	pc_attrs[0] = 
		XpexPC_CharHeight | 
		XpexPC_LineWidth | 
		XpexPC_TextColor | 
		XpexPC_InteriorStyle;

	pc_attrs[1] = pc_attrs[2] = 0x0;

	pc_values.char_height = 0.025;
	pc_values.line_width = 3.0;
	pc_values.text_color.color_type = Xpex_IndexedColor;
	pc_values.text_color.value.index = YELLOW;
	pc_values.text_font_index = -2;
	pc_values.interior_style = XpexInteriorStyle_Solid;

	pc_id = XpexCreatePC(dpy, pc_attrs, &pc_values);

	rdr_attrs.pc_id = pc_id;

	rdr_attrs.color_approx_table =
	XpexCreateLookupTable(dpy, drawable, Xpex_ColorApproxLUT);

	rdr_attrs.view_table = 
	XpexCreateLookupTable(dpy, drawable, Xpex_ViewLUT);

	rdr_attrs.color_table =
	XpexCreateLookupTable(dpy, drawable, Xpex_ColorLUT);

	rdr = XpexCreateRenderer(dpy, 
		drawable, 
		XpexRD_ColorApproxTable | 
			XpexRD_ViewTable |
			XpexRD_ColorTable |
			XpexRD_PipelineContext,
		&rdr_attrs);

	XpexSetupColormap(dpy, 
		drawable, 
		rdr_attrs.color_approx_table);

	SetUpViews();
}

static void
Initialize(dpy)	
	Display *dpy;
{
	InitXresources(dpy);
	InitPEXresources(dpy);
}

static void
DrawCube()
{
	XpexCoord3D p[5];
	XpexVector3D xlv;
	XpexMatrix mx;
	XpexVector3D scale;
	XpexVector3D shift;
	XpexFloat xang, yang, zang;

 	p[0].x =  5.; p[0].y =  5.; p[0].z = 5.;
	p[1].x = 15.; p[1].y =  5.; p[1].z = 5.;
	p[2].x = 15.; p[2].y = 15.; p[2].z = 5.;
	p[3].x =  5.; p[3].y = 15.; p[3].z = 5.;

	XpexBeginStructure(dpy, rdr, 1);
	/* XpexSetHlhsrId(dpy, Xpex_Off); */ 
	xlv.x = 0.0; xlv.y = 0.0; xlv.z = 10.0;
	XpexTranslate(&xlv, mx);
	XpexSetLocalTransform(dpy, Xpex_Replace, mx);
	XpexSetSurfaceColorIndex(dpy, RED);
	fill_area_func[useFillAreaSet](dpy, p, 4);
	/* back */
	xlv.x = 5.0; xlv.y = 10.0; xlv.z = 5.0;
	scale.x = 1.0; scale.y = 1.0; scale.z = 1.0;
	xang = 3.14; yang = 0.0; zang = 0.0;
	shift.x = 0.0; shift.y = 0.0; shift.z = 0.0;
	XpexBuildTransformationMatrix( &xlv, &shift,
	xang, yang, zang, &scale, mx);
	XpexSetLocalTransform(dpy, Xpex_Replace, mx);
	XpexSetSurfaceColorIndex(dpy, CYAN);
	fill_area_func[useFillAreaSet](dpy, p, 4);
	/* right */
	xlv.x = 5.0; xlv.y = 5.0; xlv.z = 5.0;
	scale.x = 1.0; scale.y = 1.0; scale.z = 1.0;
	xang = 0.0; yang = 3.14 * 1.5; zang = 0.0;
	shift.x = 0.0; shift.y = 0.0; shift.z = 0.0;
	XpexBuildTransformationMatrix(&xlv, &shift,
	xang, yang, zang, &scale, mx);
	XpexSetLocalTransform(dpy, Xpex_Replace, mx);
	XpexSetSurfaceColorIndex(dpy, GREEN);
	fill_area_func[useFillAreaSet](dpy, p, 4);
	/* left */
	xlv.x = 15.0; xlv.y = 5.0; xlv.z = 5.0;
	scale.x = 1.0; scale.y = 1.0; scale.z = 1.0;
	xang = 0.0; yang = -3.14 * 1.5; zang = 0.0;
	shift.x = 0.0; shift.y = 0.0; shift.z = 0.0;
	XpexBuildTransformationMatrix(&xlv, &shift,
	xang, yang, zang, &scale, mx);
	XpexSetLocalTransform(dpy, Xpex_Replace, mx);
	XpexSetSurfaceColorIndex(dpy, BLUE);
	fill_area_func[useFillAreaSet](dpy, p, 4);
	/* bottom */
	xlv.x = 5.0; xlv.y = 5.0; xlv.z = 5.0;
	scale.x = 1.0; scale.y = 1.0; scale.z = 1.0;
	xang = -1.5 * 3.14; yang = 0.0; zang = 0.0;
	shift.x = 0.0; shift.y = 0.0; shift.z = 0.0;
	XpexBuildTransformationMatrix(&xlv, &shift,
	xang, yang, zang, &scale, mx);
	XpexSetLocalTransform(dpy, Xpex_Replace, mx);
	XpexSetSurfaceColorIndex(dpy, YELLOW);
	fill_area_func[useFillAreaSet](dpy, p, 4);
	/* top */
	xlv.x = 5.0; xlv.y = 15.0; xlv.z = 5.0;
	scale.x = 1.0; scale.y = 1.0; scale.z = 1.0;
	xang = 1.5 * 3.14; yang = 0.0; zang = 0.0;
	shift.x = 0.0; shift.y = 0.0; shift.z = 0.0;
	XpexBuildTransformationMatrix(&xlv, &shift,
	xang, yang, zang, &scale, mx);
	XpexSetLocalTransform(dpy, Xpex_Replace, mx);
	XpexSetSurfaceColorIndex(dpy, MAGENTA);
	fill_area_func[useFillAreaSet](dpy, p, 4);

	XpexEndStructure(dpy, rdr);
}

static void
draw_root(msg_index)
	int msg_index;
{
	XpexCoord2D line[2];
	XpexMatrix r1, r2, r3, r4;
	XpexCoord2D text_pt;

	IdentityMatrix(r1);
	IdentityMatrix(r2);
	IdentityMatrix(r3);
	IdentityMatrix(r4);

	XpexSetLineColorIndex(dpy, 1);
	line[0].x = .5; line[0].y = 0.0; 
	line[1].x = .5; line[0].y = 1.0; 
	XpexPolyline2D(dpy, line, 2);
	line[0].x = 0.0; line[0].y = 0.5; 
	line[1].x = 1.0; line[1].y = 0.5; 
	XpexPolyline2D(dpy, line, 2);

	text_pt.x = 0; text_pt.y = .52; 
	XpexText2D(dpy, &text_pt, quad_msg[0][msg_index], 
	strlen(quad_msg[0][msg_index]));

	text_pt.x = .5; text_pt.y = .52; 
	XpexText2D(dpy, &text_pt, quad_msg[1][msg_index], 
	strlen(quad_msg[1][msg_index]));

	text_pt.x = .0; text_pt.y = .02; 
	XpexText2D(dpy, &text_pt, quad_msg[2][msg_index],
	strlen(quad_msg[2][msg_index]));

	text_pt.x = .5; text_pt.y = .02; 
	XpexText2D(dpy, &text_pt,  quad_msg[3][msg_index],
	strlen(quad_msg[3][msg_index]));
}


static void
RotateViews()
{
	XpexFloat theta;

	pt.x = 10.; pt.y = 10.; pt.z = 10.;
	shift.x = 0.0; shift.y = 0.0; shift.z = 0.0;
	scale.x = 1.0; scale.y = 1.0; scale.z = 1.0;

	IdentityMatrix(bldmat);
	IdentityMatrix(bldmat2);
	IdentityMatrix(bldmat3);
	IdentityMatrix(bldmat4);


	for (theta = 0.; theta < 4 * M_PI; theta += 0.4) {
		XClearWindow(dpy, drawable);
		XpexBeginRendering(dpy, rdr, drawable);
		draw_root(0);

		/* viewport 1 */
		XpexSetViewIndex(dpy, 1);
		x_ang = theta; y_ang = 0.0; z_ang = 0.0;
		XpexBuildTransformationMatrix(&pt, &shift,
		x_ang, y_ang, z_ang, &scale, bldmat);
		XpexSetLocalTransform(dpy, Xpex_Replace, bldmat);
		XpexSetCullingMode(dpy, Xpex_BackFaces); 
		DrawCube();

		/* viewport 2 */
		XpexSetViewIndex(dpy, 2);
		x_ang = 0.;
		y_ang = theta;
		XpexBuildTransformationMatrix(&pt, &shift,
		x_ang, y_ang, z_ang, &scale, bldmat2);
		XpexSetLocalTransform(dpy, Xpex_Replace, bldmat2);
		XpexSetCullingMode(dpy, Xpex_BackFaces);
		DrawCube();

		/* viewport 3 */
		XpexSetViewIndex(dpy, 3);
		y_ang = 0.;
		z_ang = theta;
		XpexBuildTransformationMatrix(&pt, &shift,
		x_ang, y_ang, z_ang, &scale, bldmat3);
		XpexSetLocalTransform(dpy, Xpex_Replace, bldmat3);
		XpexSetCullingMode(dpy, Xpex_FrontFaces);
		DrawCube();

		/* viewport 4 */
		XpexSetViewIndex(dpy, 4);
		x_ang = theta;
		y_ang = theta;
		z_ang = theta;
		XpexBuildTransformationMatrix(&pt, &shift,
		x_ang, y_ang, z_ang, &scale, bldmat4);
		XpexSetLocalTransform(dpy, Xpex_Replace, bldmat4);
		XpexSetCullingMode(dpy, Xpex_FrontFaces);
		DrawCube();

		XpexEndRendering(dpy, rdr, False);
	}
}

static void 
ScaleViews(scale_factor)
	XpexFloat scale_factor;
{
	XpexFloat i;

	for (i = 1.0; i < 4.0; i += .2) {
		XClearWindow(dpy, drawable);
		XpexBeginRendering(dpy, rdr, drawable);
		draw_root(1);
		/* viewport 1 */
		scale.x = scale.x * scale_factor;
		scale.y = scale.y * scale_factor;
		scale.z = scale.z * scale_factor;
		XpexSetViewIndex(dpy, 1);
		XpexBuildTransformationMatrix(&pt, &shift,
		x_ang, y_ang, z_ang, &scale, bldmat);
		XpexSetLocalTransform(dpy, Xpex_Replace, bldmat);
		DrawCube();
		/*viewport 2 */
		XpexSetViewIndex(dpy, 2);
		XpexBuildTransformationMatrix(&pt, &shift,
		x_ang, y_ang, z_ang, &scale, bldmat2);
		XpexSetLocalTransform(dpy, Xpex_Replace, bldmat2);
		DrawCube();
		/*viewport 3 */
		XpexSetViewIndex(dpy, 3);
		XpexBuildTransformationMatrix(&pt, &shift,
		x_ang, y_ang, z_ang, &scale, bldmat3);
		XpexSetLocalTransform(dpy, Xpex_Replace, bldmat3);
		DrawCube();
		/*viewport 4 */
		XpexSetViewIndex(dpy, 4);
		XpexBuildTransformationMatrix(&pt, &shift,
		x_ang, y_ang, z_ang, &scale, bldmat3);
		XpexSetLocalTransform(dpy, Xpex_Replace, bldmat3);
		DrawCube();
		XpexEndRendering(dpy, rdr, False);
	}
}

static void
Quit()
{
	XpexFreePC(dpy, pc_id);
	XpexFreeRenderer(dpy, rdr);
	XDestroyWindow(dpy, drawable);
}

static void
DoAutoBox()
{
	XpexRenderOutputCmds(dpy, rdr);

	RotateViews();

	pc_attrs[0] = XpexPC_InteriorStyle;
	pc_attrs[1] = pc_attrs[2] = 0x0;
	pc_values.interior_style = XpexInteriorStyle_Hollow;
	XpexChangePC(dpy, pc_id, pc_attrs, &pc_values); 

	RotateViews();
	ScaleViews(0.5);
	ScaleViews(2.0);
}

imr_auto_box(theDisplay)
	Display *theDisplay;
{
	dpy = theDisplay;
	Initialize();
	DoAutoBox();
	Quit();
}
