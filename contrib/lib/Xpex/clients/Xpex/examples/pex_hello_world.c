/* $Header: pex_hello_world.c,v 2.6 91/09/11 16:05:06 sinyaw Exp $ */

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
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "Xpexlib.h"
#include "Xpexutil.h"
#include "window.h"

#include "pex_hello_world.h"

#define WC_MIN        -10.0
#define WC_MAX         10.0

#define SetElementPointerToBeginning(_dpy,_s) \
{ \
	static XpexElementPos elem_pos = { \
		Xpex_Beginning, 0 \
	}; \
	XpexSetElementPointer((_dpy), (_s), &elem_pos); \
}

static Display *dpy;
static char *displayName = NULL;
static Drawable drawable;
static XpexStructure structure1;
static XpexWksResources wks_res;
static XpexPhigsWks wks1;

static int synchronize = 1;
static int immediateMode = 0;
static int phigsWorkstation = 0;

int duration;
int counter;

void
InitViewEntry(viewEntry)
	XpexViewEntry *viewEntry;
{
	static XpexViewMap3D map = {
		{ WC_MIN, WC_MAX, WC_MIN, WC_MAX }, /* window limits */
		{ 0.0, 1.0, 0.0, 1.0, 0.0, 1.0 }, /* viewport limits */
		Xpex_PERSPECTIVE, /* projection type */
		{ 0.0, 0.0, 10.0 }, /* projection reference point */
		0.0,  /* view_plane */
		-20.0, /* back_plane */
		10.0 /* front_plane */
	};
	static XpexCoord3D   vrp = { 0.0, 0.0, 0.0 }; 
	static XpexCoord3D   vup = { 0.0, 1.0, 0.0 }; 
	static XpexCoord3D   vpn = { 0.0, 0.0, 1.0 }; 

	if (XpexEvaluateViewMappingMatrix(&map, 
		viewEntry->mapping)) {
		(void) fprintf(stderr, "Error from eval mapping\n");
	}

	if (XpexEvaluateViewOrientationMatrix(&vrp, &vpn, &vup,
		viewEntry->orientation)) {
		(void) fprintf(stderr, "Error from eval orientation\n");
	}
	viewEntry->clipFlags = Xpex_ClipXY | Xpex_ClipBack | Xpex_ClipFront;
	viewEntry->clipLimits.minval.x = map.viewport.x_min;
	viewEntry->clipLimits.minval.y = map.viewport.y_min;
	viewEntry->clipLimits.minval.z = map.viewport.z_min;
	viewEntry->clipLimits.maxval.x = map.viewport.x_max;
	viewEntry->clipLimits.maxval.y = map.viewport.y_max;
	viewEntry->clipLimits.maxval.z = map.viewport.z_max;
}

static void
InitView()
{
	static XpexViewRep rep;
	static XpexViewEntry view_entry;

	InitViewEntry(&view_entry);

	rep.index = 1;
	rep.view = view_entry;

	XpexSetViewRep(dpy, wks1, &rep);
}

static void
InitWks()
{
	wks_res.line_bundle =
	XpexCreateLineBundleLUT(dpy, drawable);

	wks_res.marker_bundle =
	XpexCreateMarkerBundleLUT(dpy,drawable);

	wks_res.text_bundle =
	XpexCreateTextBundleLUT(dpy,drawable);

	wks_res.interior_bundle =
	XpexCreateInteriorBundleLUT(dpy,drawable);

	wks_res.edge_bundle =
	XpexCreateEdgeBundleLUT(dpy,drawable);

	wks_res.pattern_table =
	XpexCreatePatternLUT(dpy,drawable);

	wks_res.text_font_table =
	XpexCreateTextFontLUT(dpy,drawable);

	wks_res.color_table =
	XpexCreateColorLUT(dpy,drawable);

	wks_res.light_table =
	XpexCreateLightLUT(dpy,drawable);

	wks_res.depth_cue_table =
	XpexCreateDepthCueLUT(dpy,drawable);

	wks_res.color_approx_table =
	XpexCreateColorApproxLUT(dpy,drawable);

	wks_res.highlight_incl = XpexCreateNameSet(dpy);
	wks_res.highlight_excl = XpexCreateNameSet(dpy);
	wks_res.invis_incl = XpexCreateNameSet(dpy);
	wks_res.invis_excl = XpexCreateNameSet(dpy);

	XpexSetupColormap(dpy, drawable, wks_res.color_approx_table);

	wks1 = XpexCreatePhigsWks(dpy,
	drawable, &wks_res, Xpex_SingleBuffered);

	InitView();
}

static void
InitStructures()
{
	static	XpexCoord3D  origin 	= { 0.0, 0.0, 0.0 };
	static	XpexVector3D vector1	= { 1.0, 0.0, 0.0 };
	static	XpexVector3D vector2	= { 0.0, 1.0, 0.0 };
	static	char 		*message	= "Sony PEX Library";

	static XpexMatrix matrix = {
		{ 1.0, 0.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 0.0, 1.0 }
	};

	static XpexCoord3D view_window[5] = {
		{ WC_MIN, WC_MIN, 0.0 },
		{ WC_MAX, WC_MIN, 0.0 },
		{ WC_MAX, WC_MAX, 0.0 },
		{ WC_MIN, WC_MAX, 0.0 },
		{ WC_MIN, WC_MIN, 0.0 }
	};

	structure1 = XpexCreateStructure(dpy);

	XpexStoreOutputCmds(dpy, structure1);
	XpexSetEditingMode(dpy, structure1, XpexStructure_Insert);

		XpexSetViewIndex(dpy, 1); 
		XpexLabel(dpy, 1); 
		XpexSetLocalTransform(dpy, Xpex_Replace, matrix); 
		XpexSetTextColorIndex(dpy, MAGENTA);
		XpexSetTextAlignment(dpy, XpexValign_Half, XpexHalign_Center);
		XpexSetCharHeight(dpy, 1.0);
		XpexText(dpy, &origin, &vector1, &vector2, 
		message, strlen(message));
}

static void
InitX()
{
	dpy = XOpenDisplay(displayName);

	if (!dpy) {
		(void) fprintf(stderr, 
		"ERROR: Could not open a connection to X on display %s\n", 
		XDisplayName(NULL));
		exit(1);
	}
	XSynchronize(dpy, synchronize); 
}

static void
InitPEX()
{
	if (!XpexInitialize(dpy)) {
		(void) fprintf(stderr,
		"ERROR: PEX not supported on display %s\n",
		XDisplayName(NULL));
		exit(1);
	}
	XpexSetFloatingPointFormat(dpy, Xpex_Ieee_754_32);
}

static void
Initialize()
{
	InitX();
	InitPEX();
	InitStructures();

	SetWindowLabel("pex_hello_world - PHIGS Workstation");
	SetIconLabel("helloWorld");
	drawable = CreateWindow(dpy);

	InitWks();
}

static void
DrawImage(counter)
    int *counter;
{
	register float i;

	static XpexMatrix matrix_a = {
		{ 1.0, 0.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 0.0, 1.0 }
	};
	static XpexMatrix matrix_b = {
		{ 1.0, 0.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 0.0, 1.0 }
	};
	XpexMatrix composed_matrix;

	XpexPostStructure(dpy, wks1, structure1, 1.0);
	XpexSetDisplayUpdateMode(dpy, wks1, Xpex_VisualizeEach);
	XpexExecuteDeferredActions(dpy, wks1);
	XpexRedrawAllStructures(dpy, wks1); 
	XpexUpdateWorkstation(dpy, wks1);

	XpexSetEditingMode(dpy, structure1, XpexStructure_Replace);

	XpexStoreOutputCmds(dpy, structure1);
	for (i = 0.; i < 360.; i += 1., (*counter)++) {
		XpexSetDisplayUpdateMode(dpy, wks1, Xpex_VisualizeNone);
		SetElementPointerToBeginning(dpy,structure1);
		XpexSetElementPointerAtLabel(dpy, structure1, 1, 1);
		XpexRotateX(DEG_TO_RAD(i*7), matrix_a); 
		XpexRotateZ(DEG_TO_RAD(i*5), matrix_b); 
		XpexComposeMatrix(matrix_a, matrix_b, composed_matrix);
			XpexSetLocalTransform(dpy, Xpex_Replace, composed_matrix);

		XpexSetDisplayUpdateMode(dpy, wks1, Xpex_VisualizeEach);
	}

	XpexExecuteDeferredActions(dpy, wks1);
	XpexRedrawAllStructures(dpy, wks1);


}

static void
usage(name)
	char *name;
{
	(void) printf("%s: %s", name, PROGRAM_OPTIONS);
}

static void
handle_args(argc,argv)
	int argc;
	char *argv[];
{
	register int i;

	if (argc < 2) {
		return;
	}

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i],"-asynchronous")) {
			synchronize = 0;
		} else if (!strcmp(argv[i],"-immediateMode")) {
			immediateMode = 1;
		} else if (!strcmp(argv[i],"-phigsWorkstation")) {
			phigsWorkstation = 1;
		} else if (!strcmp(argv[i],"-display")) {
			displayName = argv[++i];
		} else if (!strcmp(argv[i],"-time")) {
			duration = atoi(argv[++i]);
		} else {
			usage(argv[0]);
			exit(-1);
		}
	}
}

static void
wks_hello_world()
{
    static void stop_proc();

    Initialize();

	if (duration > 0) {
	    start_clock(duration, stop_proc, &counter);
    }
    for (counter = 0;;)
	DrawImage(&counter);
}

static void
pex_hello_world()
{
	if (phigsWorkstation) {
		wks_hello_world();
	}
	if (immediateMode) {
		InitX();
		InitPEX();
		imr_hello_world(dpy);
	}
}

void static 
stop_proc(counter)
    caddr_t counter;
{
    printf("%d frame(s) posted\n", *(int *) counter);
    XCloseDisplay(dpy);
    exit(0);
}

int
main(argc, argv)
	int argc;
	char *argv[];
{
	handle_args(argc,argv);

	pex_hello_world();
}

