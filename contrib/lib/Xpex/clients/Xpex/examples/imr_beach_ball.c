/* $Header: imr_beach_ball.c,v 1.4 91/09/11 16:05:29 sinyaw Exp $ */

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
#include <errno.h>
#include <ctype.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "Xpexlib.h"
#include "Xpexutil.h"
#include "window.h"


#define FILLAREA3D   
#define MAX_SPHERES         100

#define BLACK          0
#define WHITE          1
#define RED            2
#define GREEN          3
#define BLUE           4
#define YELLOW         5

#define WC_MIN        -5.0
#define WC_MAX         5.0

#define TIME_INC       1.0

#ifndef MIN
#define MIN( a, b)    (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX( a, b)    (((a) > (b)) ? (a) : (b))
#endif

/* Sphere velocity and location data. */
typedef struct {
    XpexVector3D  velocity;
    XpexMatrix    position;
} Sphere_data;

static XpexMatrix identity = { 
	1.0, 0.0, 0.0, 0.0,
   	0.0, 1.0, 0.0, 0.0,
   	0.0, 0.0, 1.0, 0.0,
   	0.0, 0.0, 0.0, 1.0 
};

static Sphere_data    sphere_data[MAX_SPHERES];
static int            sphere_count = 0;

extern int	num_spheres;
extern int	duration;
extern int	synchronize;

static Display *dpy;
static Drawable drawable;
static XpexPC pc_id;
static XpexPCBitmask pc_attrs;
static XpexPCValues pc_values;
static XpexRenderer rdr;
static XpexRendererAttributes rdr_attrs;
static int counter;

static void
add_sphere()
{
    register Sphere_data *data;

    if (sphere_count < MAX_SPHERES) {
    	data = &sphere_data[sphere_count++];
    	memcpy(data->position, identity, sizeof(XpexMatrix));
    	random_velocity(&data->velocity);
    }
}

static void
DrawSphere()
{
	extern int facet_sphere();

	/* Radius of spheres. */
	static double         radius = 1.0;

	XpexListOfCoord3D *facets;
	int num_facets, num_lat = 6, num_long = 8;
	register int i;

	num_facets = facet_sphere(radius, num_lat, num_long, &facets);
	/* (void) fprintf(stderr, "numfacets = %d\n", num_facets); */

	for (i = 0; i < num_facets; i++) {
		XpexSetSurfaceColorIndex(dpy, (i % 6) + 2);
#ifdef FILLAREA3D
		XpexFillArea(dpy, Xpex_UnknownShape, True,
		facets[i].points, facets[i].num_points);
#else /* !FILLAREA3D */
		XpexFillAreaSet(dpy, Xpex_UnknownShape,
		True, Xpex_UnknownContour, &facets[i], 1);
#endif /* FILLAREA3D */
	}
	free((char *)facets);
}

#define NEW_RI( _v, _dt, _r ) \
    { \
    (_r) += (_v) * (_dt); \
    if ( (_r) >= WC_MAX ) { \
         (_r) = 2.0 * WC_MAX - (_r); \
         (_v) = -(_v); \
    } else if ( (_r) <= WC_MIN ) { \
         (_r) = 2.0 * WC_MIN - (_r); \
         (_v) = -(_v); \
    } \
    }

static void
MoveSpheres()
{
	register Sphere_data *data;
	register int i;

	for (i = 0; i < sphere_count; i++) {
		data = &sphere_data[i];
		NEW_RI(data->velocity.x, TIME_INC, data->position[0][3])
		NEW_RI(data->velocity.y, TIME_INC, data->position[1][3])
		NEW_RI(data->velocity.z, TIME_INC, data->position[2][3])
		XpexSetLocalTransform(dpy, Xpex_Replace, data->position);
		DrawSphere();
	}
}



static void
InitXresources()
{
	SetWindowLabel("pex_beach_ball - immediate mode");
	SetIconLabel("pex_beach_ball");
	drawable = CreateWindow(dpy);
	XSynchronize(dpy,synchronize);
}

static void
InitPEXresources()
{
	static void InitView();

	pc_attrs[0] = XpexPC_InteriorStyle;
	pc_attrs[1] = XpexPC_ViewIndex;
	pc_attrs[2] = 0x0;
	pc_values.interior_style = XpexInteriorStyle_Solid;
	pc_values.view_index = 1;

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

	InitView();
}

InitViewEntry(viewEntry)
	XpexViewEntry *viewEntry;
{
	static XpexCoord3D   vrp = { 0.0, 0.0, 0.0 };
	static XpexVector3D  vup = { 0.0, 0.0, 1.0 };
	static XpexVector3D  vpn = { .680, -1.0, .260 };

	XpexViewMap3D  viewmap;

    viewmap.proj_type = Xpex_PARALLEL;
    viewmap.proj_ref_point.z = 6.0 * WC_MAX;
    viewmap.window.x_min = WC_MIN; 
	viewmap.window.x_max = WC_MAX;
    viewmap.window.y_min = WC_MIN; 
	viewmap.window.y_max = WC_MAX;
    viewmap.front_plane = 2.0 * WC_MAX;
    viewmap.back_plane =  2.0 * WC_MIN;
    viewmap.view_plane =  0.4 * viewmap.proj_ref_point.z;
    viewmap.proj_ref_point.x = 
	(viewmap.window.x_min + viewmap.window.x_max) / 2.0;
    viewmap.proj_ref_point.y = 
	(viewmap.window.y_min + viewmap.window.y_max) / 2.0;
    viewmap.viewport.x_min = 0.0; 
	viewmap.viewport.x_max = 1.0;
    viewmap.viewport.y_min = 0.0; 
	viewmap.viewport.y_max = 1.0;
    viewmap.viewport.z_min = 0.0; 
	viewmap.viewport.z_max = 1.0;

    if (XpexEvaluateViewMappingMatrix(&viewmap, viewEntry->mapping)) {
		(void) fprintf(stderr, "Error from eval mapping\n");
	}

    if (XpexEvaluateViewOrientationMatrix(&vrp, &vpn, &vup, 
	viewEntry->orientation)) {
        (void) fprintf(stderr, "Error from eval orientation\n");
	}

    viewEntry->clipFlags = Xpex_ClipXY | Xpex_ClipBack | Xpex_ClipFront;
    viewEntry->clipLimits.minval.x = viewmap.viewport.x_min;
    viewEntry->clipLimits.minval.y = viewmap.viewport.y_min;
    viewEntry->clipLimits.minval.z = viewmap.viewport.z_min;
    viewEntry->clipLimits.maxval.x = viewmap.viewport.x_max;
    viewEntry->clipLimits.maxval.y = viewmap.viewport.y_max;
    viewEntry->clipLimits.maxval.z = viewmap.viewport.z_max;
}


static void
InitView()
{
	XpexViewEntry view;

	InitViewEntry(&view);

	XpexSetTableEntries(dpy, rdr_attrs.view_table,
	Xpex_ViewLUT, 1, 1, (char *) &view, sizeof(XpexViewEntry));
}

static void
DrawImage(counter)
	int *counter;
{
	register int i;

    clock_reset();

	XpexRenderOutputCmds(dpy, rdr);

    for (i = 0;; i++, (*counter)++) {
		XClearWindow(dpy, drawable);
		XpexBeginRendering(dpy, rdr, drawable);
        MoveSpheres();
		XpexEndRendering(dpy, rdr, False);
    }
}

static void
Initialize()
{
	InitXresources();
	InitPEXresources();

	while (num_spheres--) {
		add_sphere();
	}
}

static void
stop_proc(counter)
	caddr_t counter;
{	
	printf("%d frame(s) posted\n", *(int *) counter);
	XCloseDisplay(dpy);
	exit(0);
}

imr_beach_ball(theDisplay)
	Display *theDisplay;
{
	dpy = theDisplay;

	Initialize();

	if (duration > 0) {
		start_clock(duration, stop_proc, &counter);
	}

	for (counter = 0;;) {
		DrawImage(&counter);
	}
}
