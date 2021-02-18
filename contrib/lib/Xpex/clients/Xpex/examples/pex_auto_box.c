/* $Header: pex_auto_box.c,v 2.11 91/09/11 16:05:23 sinyaw Exp $ */

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
#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "Xpexlib.h"
#include "Xpexutil.h"
#include "window.h"

#include "pex_auto_box.h"

static Display *dpy;
static char *dpyname = NULL;
static Window drawable;
static XpexWksResources wks_res, *pWksRes = &wks_res;

static int synchronize = 1;
static int immediateMode = 0;
static int phigsWorkstation = 0;
int useFillAreaSet = 0;

/* Globals */

XpexCoord3D   pt;
XpexVector3D  shift;
XpexFloat     x_ang, y_ang, z_ang;
XpexVector3D  scale;
XpexMatrix    bldmat, bldmat2, bldmat3, bldmat4;

XpexPhigsWks   wks_1;
XpexStructure  root;
XpexStructure  cube;
XpexStructure  sub;

/* Labels */
#define STYLE   1 
#define QUAD_1  101 
#define QUAD_2 	102 
#define QUAD_3 	103 
#define QUAD_4	104 
#define QUAD_1_1 105
#define QUAD_4_1 106

#define SET_ELEM_PTR_TO_BEGINNING(_DPY,_SID) \
{ \
	static XpexElementPos elem_pos = { \
		Xpex_Beginning, 0 \
	}; \
	XpexSetElementPointer(_DPY,_SID,&elem_pos); \
}

#define DELETE_ELEM(_DPY,_SID) \
{ \
	static XpexElementRange elem_range = { \
		{ Xpex_Current, 0 }, \
		{ Xpex_Current, 0 } \
	}; \
	XpexDeleteElements(_DPY, _SID, &elem_range); \
}

static void fill_area1();
static void fill_area2();

FuncPtr fill_area_func[2] = {
	fill_area1,
	fill_area2
};

static void
fill_area1(dpy,p,n)
	Display *dpy;
	XpexCoord3D *p;
	int n;
{
	XpexFillArea(dpy, 
	Xpex_UnknownShape, True, p, n);
}

static void
fill_area2(dpy,p,n)
	Display *dpy;
	XpexCoord3D *p;
	int n;
{
	XpexListOfCoord3D list; 
	list.num_points = n; 
	list.points = p; 
	XpexFillAreaSet(dpy, 
	Xpex_UnknownShape, True, Xpex_UnknownContour, 
	&list, 1); 
}

void
IdentityMatrix(mat)
	XpexFloat mat[4][4];
{
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            mat[i][j] = 0.0;
            if (i == j) mat[i][j] = 1.0;
        }
    }
} 

static void
rotate_views()
{
	float	theta;
	int		err;

/*
 * ... enable editing in the replace mode -- this will be used to update
 * rotation angles in 4 views
 */
    XpexSetDisplayUpdateMode(dpy, wks_1, Xpex_VisualizeEasy);
    XpexSetEditingMode(dpy, sub, XpexStructure_Replace);
/*
 * ...  set some constants for building the matrix
 */        
    pt.x = 10.; pt.y = 10.; pt.z = 10.;
    shift.x = 0.0; shift.y = 0.0; shift.z = 0.0;
    scale.x = 1.0; scale.y = 1.0; scale.z = 1.0;
/*
 * ...  make identity matrices
 */
    IdentityMatrix(bldmat);
    IdentityMatrix(bldmat2);
    IdentityMatrix(bldmat3);
    IdentityMatrix(bldmat4);
/*
 * ...  edit the structure
 */

/*
 * ... increment the angle in a postive angle
 */     

	XpexStoreOutputCmds(dpy, sub);

    for (theta = 0; theta < 4 * M_PI; theta += 0.4) {
/*
 * ...  set the pointer and the angles, 
 * ...	find the label, 
 * ...	build the matrix, 
 * ...	update the view
 */
		SET_ELEM_PTR_TO_BEGINNING(dpy, sub);
/*
 * ... viewport 1
 */
        x_ang = theta;
        y_ang = 0.0;
        z_ang = 0.0;

		XpexSetElementPointerAtLabel(dpy, sub, 1, 1);

        XpexBuildTransformationMatrix(&pt, &shift, 
		x_ang, y_ang, z_ang, &scale, bldmat);

	XpexSetLocalTransform(dpy, Xpex_Replace, bldmat);

/*
 * ... viewport 2
 */
	x_ang = 0;
	y_ang = theta;

	XpexSetElementPointerAtLabel(dpy, sub, 2, 1);

	XpexBuildTransformationMatrix(&pt, &shift, 
	x_ang, y_ang, z_ang, &scale, bldmat2);

	XpexSetLocalTransform(dpy, Xpex_Replace, bldmat2);

/*
 * ... viewport 3
 */

       	y_ang = 0;
       	z_ang = theta;

	XpexSetElementPointerAtLabel(dpy, sub, 3, 1);

	XpexBuildTransformationMatrix(&pt, &shift, 
	x_ang, y_ang, z_ang, &scale, bldmat3);

	XpexSetLocalTransform(dpy, Xpex_Replace, bldmat3);

/*
 * ... viewport 4
 */
	x_ang = theta;
	y_ang = theta;
	z_ang = theta;

	XpexSetElementPointerAtLabel(dpy, sub, 4, 1);

	XpexBuildTransformationMatrix(&pt, &shift, 
	x_ang, y_ang, z_ang, &scale, bldmat4);

	XpexSetLocalTransform(dpy, Xpex_Replace, bldmat4);

	    XpexExecuteDeferredActions(dpy, wks_1);

		XpexUpdateWorkstation(dpy, wks_1);
	}
}

static void
scale_views( scale_factor)
    float scale_factor;
{
    float           i;
    int             err;

/*
 * ... enable editing in the replace mode -- this will be used to update
 *                                       the scale matrix in 4 views
 */
    XpexSetDisplayUpdateMode(dpy, wks_1, Xpex_VisualizeEasy);

    XpexSetEditingMode(dpy, sub, XpexStructure_Replace);
/*
 * ...  open the structure for editing
 */
/*
 * ... increment the scale matrix 
 */     

    for (i = 1.0; i < 4.0; i += .2) {
/*
 * ...  set the pointer and the angles, find the label, build the matrix, update
 * ...  the view
 */
		SET_ELEM_PTR_TO_BEGINNING(dpy, sub);
/*
 * ... viewport 1
 */
        scale.x = scale.x * scale_factor; 
        scale.y = scale.y * scale_factor; 
        scale.z = scale.z * scale_factor;
	
        XpexSetElementPointerAtLabel(dpy, sub, 1, 1);

        XpexBuildTransformationMatrix(&pt, &shift, 
		x_ang, y_ang, z_ang, &scale, bldmat);

		
		XpexStoreOutputCmds(dpy, sub);
        XpexSetLocalTransform(dpy, Xpex_Replace, bldmat);

/*
 * ... viewport 2
 */
	    XpexSetElementPointerAtLabel(dpy, sub, 2, 1);

        XpexBuildTransformationMatrix(&pt, &shift, 
		x_ang, y_ang, z_ang, &scale, bldmat2);

        XpexSetLocalTransform(dpy, Xpex_Replace, bldmat2);
/*
 * ... viewport 3
 */
        XpexSetElementPointerAtLabel(dpy, sub, 3, 1);

        XpexBuildTransformationMatrix(&pt, &shift, 
		x_ang, y_ang, z_ang, &scale, bldmat3);

        XpexSetLocalTransform(dpy, Xpex_Replace, bldmat3);
/*
 * ... viewport 4
 */

        XpexSetElementPointerAtLabel(dpy, sub, 4, 1);

        XpexBuildTransformationMatrix(&pt, &shift, 
		x_ang, y_ang, z_ang, &scale, bldmat4);

        XpexSetLocalTransform(dpy, Xpex_Replace, bldmat4);

        XpexExecuteDeferredActions(dpy, wks_1);

        XpexUpdateWorkstation(dpy, wks_1);

    } /* end-for */
}

static void
load_box()
{
    XpexCoord3D p[5];
    XpexVector3D xlv;
    XpexMatrix mx;
    XpexVector3D scale;
    XpexVector3D shift;
    int err = 0;
    float xang, yang, zang;

    XpexListOfCoord3D side;

    side.num_points = 4; side.points = p;

    IdentityMatrix(mx);

	cube = XpexCreateStructure(dpy);
		
	XpexStoreOutputCmds(dpy, cube);
		XpexLabel(dpy, STYLE);
		XpexSetInteriorStyle(dpy, XpexInteriorStyle_Solid);
		XpexSetHlhsrId(dpy, Xpex_On); 
/*
 * ... make a square to make a front and back of the cube
 */
    p[0].x =  5.; p[0].y =  5.; p[0].z = 5.;
    p[1].x = 15.; p[1].y =  5.; p[1].z = 5.;
    p[2].x = 15.; p[2].y = 15.; p[2].z = 5.;
    p[3].x =  5.; p[3].y = 15.; p[3].z = 5.;
                   
/* front */
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

    xlv.x   = 5.0; xlv.y   =      5.0; xlv.z   = 5.0;
    scale.x = 1.0; scale.y =      1.0; scale.z = 1.0;         
    xang    = 0.0; yang    = 3.14*1.5; zang    = 0.0; 
    shift.x = 0.0; shift.y =      0.0; shift.z = 0.0;

	XpexBuildTransformationMatrix(&xlv, &shift, 
	xang, yang, zang, &scale, mx);

	XpexSetLocalTransform(dpy, Xpex_Replace, mx);
    XpexSetSurfaceColorIndex(dpy, GREEN);
	fill_area_func[useFillAreaSet](dpy, p, 4);

/* left */

    xlv.x   = 15.0; xlv.y   =       5.0; xlv.z   = 5.0;
    scale.x =  1.0; scale.y =       1.0; scale.z = 1.0;         
    xang    =  0.0; yang    = -3.14*1.5; zang    = 0.0; 
    shift.x =  0.0; shift.y =       0.0; shift.z = 0.0;

    XpexBuildTransformationMatrix(&xlv, &shift, 
	xang, yang, zang, &scale, mx);

	XpexSetLocalTransform(dpy, Xpex_Replace, mx);
	XpexSetSurfaceColorIndex(dpy, BLUE);
	fill_area_func[useFillAreaSet](dpy, p, 4);

/* bottom */

    xlv.x   =         5.0; xlv.y   = 5.0; xlv.z   = 5.0;
    scale.x =         1.0; scale.y = 1.0; scale.z = 1.0;         
    xang    = -1.5 * 3.14; yang    = 0.0; zang    = 0.0; 
    shift.x =         0.0; shift.y = 0.0; shift.z = 0.0;

	XpexBuildTransformationMatrix(&xlv, &shift, 
	xang, yang, zang, &scale, mx);

    XpexSetLocalTransform(dpy, Xpex_Replace, mx);
    XpexSetSurfaceColorIndex(dpy, YELLOW);
	fill_area_func[useFillAreaSet](dpy, p, 4);
/* top */

    xlv.x   = 5.0;        xlv.y   = 15.0; xlv.z   = 5.0;
    scale.x = 1.0;        scale.y =  1.0; scale.z = 1.0;         
    xang    = 1.5 * 3.14; yang    =  0.0; zang    = 0.0; 
    shift.x = 0.0;        shift.y =  0.0; shift.z = 0.0;

    XpexBuildTransformationMatrix(&xlv, &shift, 
	xang, yang, zang, &scale, mx);

    XpexSetLocalTransform(dpy, Xpex_Replace, mx);
    XpexSetSurfaceColorIndex(dpy, MAGENTA);
	fill_area_func[useFillAreaSet](dpy, p, 4);
}

void
InitViewEntries(viewEntries)
	XpexViewEntry viewEntries[4];
{
	static XpexViewMap3D view_maps[4] = {
		{
			{ -20.0, 20.0, -20.0, 20.0 },
			{ 0.0, .5, 0.5, 1.0, 0.0, 1.0 },
			Xpex_PERSPECTIVE,
			{ 0., 0., 1000.0 },
			0.0,
			-40.0,
			40.0
		},
		{
			{ -20.0, 20.0, -20.0, 20.0 },
			{ 0.5, 1.0, 0.5, 1.0, 0.0, 1.0 },
			Xpex_PERSPECTIVE,
			{ 0., 0., 1000.0 },
			0.0,
			-40.0,
			40.0
		},
		{
			{ -20.0, 20.0, -20.0, 20.0 },
			{ 0.0, .5, 0., .5, 0.0, 1.0 },
			Xpex_PERSPECTIVE,
			{ 0., 0., 1000.0 },
			0.0,
			-40.0,
			40.0
		},
		{
			{ -20.0, 20.0, -20.0, 20.0 },
			{ 0.5, 1.0, 0.,.5, 0.0, 1.0 },
			Xpex_PERSPECTIVE,
			{ 0., 0., 1000.0 },
			0.0,
			-40.0,
			40.0
		}
	};

    static XpexCoord3D  vrp = { 0.0, 0.0, 0.0 };    /*  origin */
    static XpexVector3D vpn = { 1.0, 1.0, 1.0 };/*  view plane normal */
    static XpexVector3D vup = { 0.0, 1.0, 0.0 };/*  view up vector */
		
    int err;

    err = XpexEvaluateViewOrientationMatrix(&vrp, &vpn, &vup, 
	viewEntries[0].orientation);

    if (err != 0)  {
        (void) printf("view orientation error ");
	}

    err = XpexEvaluateViewMappingMatrix(&view_maps[0], 
	viewEntries[0].mapping);

    if (err != 0) {
        (void) printf("view mapping error ");
	}

    viewEntries[0].clipFlags = 0;
    viewEntries[0].clipLimits.minval.x = view_maps[0].viewport.x_min;
    viewEntries[0].clipLimits.minval.y = view_maps[0].viewport.y_min;
    viewEntries[0].clipLimits.minval.z = view_maps[0].viewport.z_min;
    viewEntries[0].clipLimits.maxval.x = view_maps[0].viewport.x_max;
    viewEntries[0].clipLimits.maxval.y = view_maps[0].viewport.y_max;
    viewEntries[0].clipLimits.maxval.z = view_maps[0].viewport.z_max;
    
/*
 *   viewport # 2
 */
    err = XpexEvaluateViewOrientationMatrix(&vrp, &vpn, &vup, 
	viewEntries[1].orientation);

    if (err != 0) { 
        (void) printf("view orientation error ");
	}

    err = XpexEvaluateViewMappingMatrix(&view_maps[1], 
	viewEntries[1].mapping);

	if (err != 0) { 
		(void) printf("view mapping error ");
	}

    viewEntries[1].clipFlags = 0;
    viewEntries[1].clipLimits.minval.x = view_maps[1].viewport.x_min;
    viewEntries[1].clipLimits.minval.y = view_maps[1].viewport.y_min;
    viewEntries[1].clipLimits.minval.z = view_maps[1].viewport.z_min;
    viewEntries[1].clipLimits.maxval.x = view_maps[1].viewport.x_max;
    viewEntries[1].clipLimits.maxval.y = view_maps[1].viewport.y_max;
    viewEntries[1].clipLimits.maxval.z = view_maps[1].viewport.z_max;

/*
 *   viewport # 3
 */
	err = XpexEvaluateViewOrientationMatrix(&vrp, &vpn, &vup, 
	viewEntries[2].orientation);

	if (err != 0)  {
		(void) printf("view orientation error ");
	}

    err = XpexEvaluateViewMappingMatrix(&view_maps[2], 
	viewEntries[2].mapping);

	if (err != 0) {
		(void) printf("view mapping error ");
	}

    viewEntries[2].clipFlags = 0;
    viewEntries[2].clipLimits.minval.x = view_maps[2].viewport.x_min;
    viewEntries[2].clipLimits.minval.y = view_maps[2].viewport.y_min;
    viewEntries[2].clipLimits.minval.z = view_maps[2].viewport.z_min;
    viewEntries[2].clipLimits.maxval.x = view_maps[2].viewport.x_max;
    viewEntries[2].clipLimits.maxval.y = view_maps[2].viewport.y_max;
    viewEntries[2].clipLimits.maxval.z = view_maps[2].viewport.z_max;

/*
 *   viewport # 4
 */
    err = XpexEvaluateViewOrientationMatrix(&vrp, &vpn, &vup, 
	viewEntries[3].orientation);

    if (err != 0) {
		(void) printf ("view orientation error ");
	}

    err = XpexEvaluateViewMappingMatrix(&view_maps[3], 
	viewEntries[3].mapping);

    if (err != 0)  {
        (void) printf("view mapping error ");
	}

    viewEntries[3].clipFlags = 0;
    viewEntries[3].clipLimits.minval.x = view_maps[3].viewport.x_min;
    viewEntries[3].clipLimits.minval.y = view_maps[3].viewport.y_min;
    viewEntries[3].clipLimits.minval.z = view_maps[3].viewport.z_min;
    viewEntries[3].clipLimits.maxval.x = view_maps[3].viewport.x_max;
    viewEntries[3].clipLimits.maxval.y = view_maps[3].viewport.y_max;
    viewEntries[3].clipLimits.maxval.z = view_maps[3].viewport.z_max;
}
	
static void
load_views()
{
	XpexViewRep view_rep;
	XpexViewEntry view_entries[4];

	InitViewEntries(view_entries);

	view_rep.index = 1;
	view_rep.view = view_entries[0];
    
    XpexSetViewRep(dpy, wks_1, &view_rep);
/*
 *   viewport # 2
 */

    view_rep.index = 2;
	view_rep.view = view_entries[1];

    XpexSetViewRep(dpy, wks_1, &view_rep);
/*
 *   viewport # 3
 */

    view_rep.index = 3;
	view_rep.view = view_entries[2];

    XpexSetViewRep(dpy, wks_1, &view_rep);
/*
 *   viewport # 4
 */
    view_rep.index = 4;
	view_rep.view = view_entries[3];

    XpexSetViewRep(dpy, wks_1, &view_rep);
}

static void
build_css()
{
    XpexCoord2D  line[2];
    XpexMatrix   r1, r2, r3, r4;
    XpexCoord2D  text_pt;

    IdentityMatrix(r1);
    IdentityMatrix(r2);
    IdentityMatrix(r3);
    IdentityMatrix(r4);

/*
 * ... open the root structure
 */

/*
 *    popen_struct( ROOT); 
 *
 */
	
	root = XpexCreateStructure(dpy);

    sub = XpexCreateStructure(dpy);

    XpexSetEditingMode(dpy, root, XpexStructure_Insert);

	SET_ELEM_PTR_TO_BEGINNING(dpy, root);
/*
 * ...  set up lines that split the viewports
 *
 */
		
	XpexStoreOutputCmds(dpy, root);
		XpexSetLineWidth(dpy, 3.0);
		XpexSetLineColorIndex(dpy, WHITE);

    	line[0].x = .5; line[0].y = 0.0; 
    	line[1].x = .5; line[1].y = 1.0;
		XpexPolyline2D(dpy, line, 2);

    	line[0].x = 0.0; line[0].y = .5;
    	line[1].x = 1.0; line[1].y = .5;
		XpexPolyline2D(dpy, line, 2);
/*
 * ... text to explain each viewport
 */

		XpexSetTextColorIndex(dpy, YELLOW);
		XpexSetTextFontIndex(dpy, -2);
		XpexSetCharHeight(dpy, 0.025);
		XpexLabel(dpy, QUAD_2);

    	text_pt.x = 0; text_pt.y = .52;
		XpexText2D( dpy, &text_pt, " X Rotation", 
		strlen(" X Rotation"));

		XpexLabel(dpy, QUAD_1);

    	text_pt.x = .5; text_pt.y = .58;
		XpexText2D(dpy, &text_pt, " Y Rotation", 
		strlen(" Y Rotation"));

		XpexLabel(dpy, QUAD_1_1);

		text_pt.x = .5; text_pt.y = .52;
		XpexText2D(dpy, &text_pt, " Backfaces Culled", 
		strlen(" Backfaces Culled"));

    	XpexLabel(dpy, QUAD_3);

		text_pt.x = 0; text_pt.y = .02;
		XpexText2D(dpy, &text_pt, " Z Rotation",
		strlen(" Z Rotation"));

    	XpexLabel(dpy, QUAD_4);

    	text_pt.x = .5; text_pt.y = .08;
		XpexText2D(dpy, &text_pt, " XYZ Rotation",
		strlen(" XYZ Rotation"));

		XpexLabel(dpy, QUAD_4_1);
		text_pt.y = .02;
		XpexText2D(dpy, &text_pt, " FrontFaces Culled",
		strlen(" FrontFaces Culled"));

		XpexSetLineWidth(dpy, 1.0);

		XpexExecuteStructure(dpy, sub);

/* -------------------------------------------------------------- */

    XpexSetEditingMode(dpy, sub, XpexStructure_Insert);

	SET_ELEM_PTR_TO_BEGINNING(dpy, sub);

		
	XpexStoreOutputCmds(dpy, sub);
		XpexSetViewIndex(dpy, 1);
		XpexLabel(dpy, 1);
		XpexSetLocalTransform(dpy, Xpex_PreConcatenate, r1);
		/* Cull no faces in this view */
		XpexSetCullingMode(dpy, Xpex_BackFaces);
		XpexExecuteStructure(dpy, cube);

		XpexSetViewIndex(dpy, 2);
		XpexLabel(dpy, 2);
		XpexSetLocalTransform(dpy, Xpex_PreConcatenate, r2);
		/* Cull Backfaces in this view */
		XpexSetCullingMode(dpy, Xpex_BackFaces);
		XpexExecuteStructure( dpy, cube);

		XpexSetViewIndex(dpy, 3);
    	XpexLabel(dpy, 3);
		XpexSetLocalTransform(dpy, Xpex_PreConcatenate, r3);
		/* Cull no faces in this view */
		XpexSetCullingMode(dpy, Xpex_FrontFaces);
		XpexExecuteStructure(dpy, cube);

		XpexSetViewIndex(dpy, 4);
    	XpexLabel(dpy, 4);
		XpexSetLocalTransform(dpy, Xpex_PreConcatenate, r4);
		/* Cull frontfaces in this view */
		XpexSetCullingMode(dpy, Xpex_FrontFaces);
    	XpexExecuteStructure(dpy, cube);
}

static void
edit_css()
{
	XpexSetEditingMode(dpy, cube, XpexStructure_Replace);

	SET_ELEM_PTR_TO_BEGINNING(dpy, cube);
	XpexSetElementPointerAtLabel(dpy, cube, STYLE, 1);

		
	XpexStoreOutputCmds(dpy, cube);
		XpexSetInteriorStyle(dpy, XpexInteriorStyle_Hollow);
}

static void
relabel_quadrants()
{
    XpexCoord2D     text_pt;
/*
 * ... relabel the quadrants
 */

	SET_ELEM_PTR_TO_BEGINNING(dpy, root);
    XpexSetEditingMode(dpy, root, XpexStructure_Replace);
    XpexSetElementPointerAtLabel(dpy, root, QUAD_2, 1);

		
	XpexStoreOutputCmds(dpy, root);
    	text_pt.x = 0.0; text_pt.y = 0.52;
    	XpexText2D(dpy, &text_pt, " Zooming", strlen(" Zooming"));

    XpexSetElementPointerAtLabel(dpy, root, QUAD_1, 1);

		
	XpexStoreOutputCmds(dpy, root);
    	text_pt.x = .5; text_pt.y = .52;
    	XpexText2D(dpy, &text_pt, " Zooming", strlen(" Zooming"));

/* ----- */

    XpexSetElementPointerAtLabel(dpy, root, QUAD_3, 1);

		
	XpexStoreOutputCmds(dpy, root);
    	text_pt.x = 0.0; text_pt.y = .02;
    	XpexText2D(dpy, &text_pt, " Zooming", strlen(" Zooming"));

/* ----- */

    XpexSetElementPointerAtLabel(dpy, root, QUAD_4, 1);

		
	XpexStoreOutputCmds(dpy, root);
    	text_pt.x = .5; text_pt.y = .02;
    	XpexText2D(dpy, &text_pt, " Zooming", strlen(" Zooming"));

/* Remove the "culling labels from the quadrants */

	SET_ELEM_PTR_TO_BEGINNING(dpy, root);

	XpexSetElementPointerAtLabel(dpy, root, QUAD_1_1, 1);
	DELETE_ELEM(dpy, root);

	XpexSetElementPointerAtLabel(dpy, root, QUAD_4_1, 1);
	DELETE_ELEM(dpy, root);
}

static void
InitWks()
{
	static void create_phigs_wks();

/* Initialization of matrices */

    /* setup( 4, mat);
    setup( 4, s);

    setup( 4, r1);
    setup( 4, r2);
    setup( 4, r3);
    setup( 4, r4); */
/*
 * ...  set some constants for building the matrix
 */        
    pt.x    = 10.0; pt.y    = 10.0; pt.z    = 10.0;
    shift.x =  0.0; shift.y =  0.0; shift.z =  0.0;
    scale.x =  1.0; scale.y =  1.0; scale.z =  1.0;
/*
 * ...  make identity matrices
 */
    IdentityMatrix(bldmat);
    IdentityMatrix(bldmat2);
    IdentityMatrix(bldmat3);
    IdentityMatrix(bldmat4);


/*
 *   ... open a workstation 
 */
    create_phigs_wks();
/*
 * ... initialize the viewports
 */
    load_views();
}

InitPEX()
{

	if (!XpexInitialize(dpy)) {
		(void) fprintf(stderr, "PEX not supported\n");
		exit(-1);
	}
	
	XpexSetFloatingPointFormat(dpy, Xpex_Ieee_754_32); 
}

static void
Initialize()
{	
	InitPEXresources();
	InitXresources();
	InitWks();

	XpexSetupColormap(dpy, drawable, wks_res.color_approx_table); 
}

InitX()
{
	dpy = XOpenDisplay(dpyname);

	if (!dpy) {
		(void) fprintf(stderr,"Connection Failed\n");
		exit(-1);
	}
    XSynchronize(dpy, synchronize);
}

InitXresources()
{
	SetWindowLabel("pex_auto_box - PHIGS Workstation");
	SetIconLabel("pex_auto_box");
	drawable = CreateWindow(dpy);
}

InitPEXresources()
{
/*
 * ... load the structure containing the cube
 */
    load_box(); 

    build_css();
}

static void
draw_image()
{
	float scale_factor;

/*
 * ... post the structure  --- display it
 */

	XpexPostStructure(dpy, wks_1, root, 0.0);
/*
 * ... rotate the four views
 */

	rotate_views();

/*
 * ... turn off solids and make wireframe
 */
	edit_css();
/*
 * ... rotate the four views in wireframe
 */
	rotate_views();

	relabel_quadrants(); 
	
/*
 * ... scale down the four views
 */
	scale_factor = .5;
   	scale_views(scale_factor);
/*
 * ... scale up the four views
 */
	scale_factor = 2.0;
   	scale_views( scale_factor );

/*
 * test traversal
 */

	XpexExecuteDeferredActions(dpy, wks_1);
	XpexRedrawAllStructures(dpy, wks_1);

}

static void
create_phigs_wks()
{
    pWksRes->line_bundle = 
    XpexCreateLineBundleLUT(dpy, drawable);

    pWksRes->marker_bundle =
    XpexCreateMarkerBundleLUT(dpy,drawable);

    pWksRes->text_bundle =
    XpexCreateTextBundleLUT(dpy,drawable);

    pWksRes->interior_bundle = 
    XpexCreateInteriorBundleLUT(dpy,drawable);

    pWksRes->edge_bundle = 
    XpexCreateEdgeBundleLUT(dpy,drawable);

    pWksRes->pattern_table = 
    XpexCreatePatternLUT(dpy,drawable);

    pWksRes->text_font_table = 
    XpexCreateTextFontLUT(dpy,drawable);

    pWksRes->color_table = 
    XpexCreateColorLUT(dpy,drawable);

    pWksRes->light_table = 
    XpexCreateLightLUT(dpy,drawable);

    pWksRes->depth_cue_table = 
    XpexCreateDepthCueLUT(dpy,drawable);

    pWksRes->color_approx_table = 
    XpexCreateColorApproxLUT(dpy,drawable);

    pWksRes->highlight_incl = XpexCreateNameSet(dpy);
    pWksRes->highlight_excl = XpexCreateNameSet(dpy);
    pWksRes->invis_incl = XpexCreateNameSet(dpy);
    pWksRes->invis_excl = XpexCreateNameSet(dpy);

    wks_1 = XpexCreatePhigsWks(dpy, 
	drawable, pWksRes, Xpex_SingleBuffered);

    XpexSetHlhsrMode(dpy, wks_1, XpexHlhsr_Off); 

}

HandleArgs(argc,argv)
	int argc;
	char *argv[];
{
	register int i;

	if (argc < 2) {
		return;
	}
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-asynchronous")) {
			synchronize = 0;
		} else if (!strcmp("-display",argv[i])) {
			dpyname = argv[++i];
		} else if (!strcmp("-immediateMode",argv[i])) {
			immediateMode = 1;
		} else if (!strcmp("-phigsWorkstation",argv[i])) {
			phigsWorkstation = 1;
		} else if (!strcmp("-fillAreaSet",argv[i])) {
			useFillAreaSet = 1;
		} else {
			(void) printf(
			"%s: bad command option \"%s\"\n\n",
			argv[0], argv[i]);

			usage(argv[0]);
			exit(-1);
		}
	} /* end-for */
}

usage(name)
	char *name;
{
	static XpexString cmd_line_options[] = {
		"[-display display_name]",
		"\n\t[-phigsWorkstation] or [-immediateMode]",
		"\n\t[-asynchronous]",
		"\n\t[-fillAreaSet]",
		"\n",
		NULL
	};
	register int i = 0;

	(void) printf("usage: %s ", name);

	for (; cmd_line_options[i] != (XpexString) NULL; i++) {
		(void) printf("%s", cmd_line_options[i]);
	}
}

static void
clean_up()
{
    XpexFreeLookupTable(dpy,pWksRes->line_bundle);
    XpexFreeLookupTable(dpy,pWksRes->marker_bundle);
    XpexFreeLookupTable(dpy,pWksRes->text_bundle);
    XpexFreeLookupTable(dpy,pWksRes->interior_bundle);
    XpexFreeLookupTable(dpy,pWksRes->edge_bundle);
    XpexFreeLookupTable(dpy,pWksRes->pattern_table);
    XpexFreeLookupTable(dpy,pWksRes->text_font_table);
    XpexFreeLookupTable(dpy,pWksRes->color_table);
    XpexFreeLookupTable(dpy,pWksRes->light_table);
    XpexFreeLookupTable(dpy,pWksRes->depth_cue_table);
    XpexFreeLookupTable(dpy,pWksRes->color_approx_table);
    XpexFreeNameSet(dpy,pWksRes->highlight_incl);
    XpexFreeNameSet(dpy,pWksRes->highlight_excl); 
    XpexFreeNameSet(dpy,pWksRes->invis_incl); 
    XpexFreeNameSet(dpy,pWksRes->invis_excl); 

	XpexFreePhigsWks(dpy, wks_1);

	XDestroyWindow(dpy,drawable);

	XCloseDisplay(dpy);
}

wks_auto_box()
{
	Initialize();
	draw_image();
	clean_up();
}

int
main(argc,argv)
	int argc;
	char *argv[];
{
	HandleArgs(argc,argv);

	if ((!phigsWorkstation) && (!immediateMode)) {
		phigsWorkstation = immediateMode = 1;
	}

	if (phigsWorkstation) {
		InitX();
		InitPEX();
		wks_auto_box();
	}

	if (immediateMode) {
		InitX();
		InitPEX();
		imr_auto_box(dpy);
	}
	
	exit(0);
}
